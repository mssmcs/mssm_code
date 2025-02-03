#include "camera.h"
#include <cstdio>
#include <string>
#include <chrono>
#include <thread> // for sleep
#include <vector>

using namespace mssm;

void myCustomLogFunction(uint32_t level, const char *string)
{
    printf("== %s", string);
}

std::string FourCCToString(uint32_t fourcc)
{
    std::string v;
    for(uint32_t i=0; i<4; i++)
    {
        v += static_cast<char>(fourcc & 0xFF);
        fourcc >>= 8;
    }
    return v;
}

bool writeBufferAsPPM(uint32_t frameNum, uint32_t width, uint32_t height, const uint8_t *bufferPtr, size_t bytes)
{
    char fname[100];
    sprintf(fname, "frame_%d.ppm",frameNum);

    FILE *fout = fopen(fname, "wb");
    if (fout == 0)
    {
        fprintf(stderr, "Cannot open %s for writing\n", fname);
        return false;
    }

    fprintf(fout, "P6 %d %d 255\n", width, height); // PGM header
    fwrite(bufferPtr, 1, bytes, fout);
    fclose(fout);

    return true;
}

void showAutoProperty(CapContext ctx, int32_t streamID, uint32_t propertyID)
{
    uint32_t bValue;
    if (Cap_getAutoProperty(ctx, streamID, propertyID, &bValue)==CAPRESULT_OK)
    {
        if (bValue)
        {
            printf("Auto\n");
        }
        else
        {
            printf("Manual\n");
        }
    }
    else
    {
        printf("Unsupported\n");
    }
}

void showAutoProperties(CapContext ctx, int32_t streamID)
{
    printf("White balance: ");
    showAutoProperty(ctx, streamID, CAPPROPID_WHITEBALANCE);

    printf("Exposure     : ");
    showAutoProperty(ctx, streamID, CAPPROPID_EXPOSURE);

    printf("Focus        : ");
    showAutoProperty(ctx, streamID, CAPPROPID_FOCUS);

    printf("Zoom         : ");
    showAutoProperty(ctx, streamID, CAPPROPID_ZOOM);

    printf("Gain         : ");
    showAutoProperty(ctx, streamID, CAPPROPID_GAIN);
}

void showProperty(CapContext ctx, int32_t streamID, uint32_t propertyID)
{
    int32_t value;
    if (Cap_getProperty(ctx, streamID, propertyID, &value)==CAPRESULT_OK)
    {
        printf("%d\n", value);
    }
    else
    {
        printf("Unsupported\n");
    }
}

void showProperties(CapContext ctx, int32_t streamID)
{
    printf("White balance: ");
    showProperty(ctx, streamID, CAPPROPID_WHITEBALANCE);

    printf("Exposure     : ");
    showProperty(ctx, streamID, CAPPROPID_EXPOSURE);

    printf("Focus        : ");
    showProperty(ctx, streamID, CAPPROPID_FOCUS);

    printf("Zoom         : ");
    showProperty(ctx, streamID, CAPPROPID_ZOOM);

    printf("Gain         : ");
    showProperty(ctx, streamID, CAPPROPID_GAIN);

    printf("Brightness   : ");
    showProperty(ctx, streamID, CAPPROPID_BRIGHTNESS);

    printf("Contrast     : ");
    showProperty(ctx, streamID, CAPPROPID_CONTRAST);

    printf("Saturation   : ");
    showProperty(ctx, streamID, CAPPROPID_SATURATION);

    printf("Gamma        : ");
    showProperty(ctx, streamID, CAPPROPID_GAMMA);
}

void estimateFrameRate(CapContext ctx, int32_t streamID)
{
    std::chrono::time_point<std::chrono::system_clock> tstart, tend;
    tstart = std::chrono::system_clock::now();
    uint32_t fstart = Cap_getStreamFrameCount(ctx, streamID);
    std::this_thread::sleep_for (std::chrono::seconds(2));
    uint32_t fend = Cap_getStreamFrameCount(ctx, streamID);
    tend = std::chrono::system_clock::now();
    std::chrono::duration<double> fsec = tend-tstart;
    uint32_t frames = fend - fstart;
    printf("Frames = %d\n", frames);
    std::chrono::milliseconds d = std::chrono::duration_cast<std::chrono::milliseconds>(fsec);
    printf("Measured fps=%5.2f\n", 1000.0f*frames/static_cast<float>(d.count()));
}

Camera::Camera(mssm::Graphics &g) : g{g}
{
    uint32_t deviceFormatID = 0;
    uint32_t deviceID       = 0;

    ctx = Cap_createContext();

    deviceCount = Cap_getDeviceCount(ctx);

}

Camera::~Camera()
{
    if (streamId >= 0) {
      Cap_closeStream(ctx, streamId);
    }
    Cap_releaseContext(ctx);
}

bool Camera::open(const FormatInfo &format)
{
    if (streamId >= 0) {
      Cap_closeStream(ctx, streamId);
    }
    openFormat = format;
    streamId = Cap_openStream(ctx, openFormat.deviceId, openFormat.formatId);
    return isOpen();
}

bool Camera::isOpen()
{
    return streamId >= 0;
}

mssm::Image Camera::capture()
{
    if (!isOpen()) {
        Image img(g, 100, 100, PURPLE);
        return img;
    }

    CapFormatInfo finfo;
    Cap_getFormatInfo(ctx, openFormat.deviceId, openFormat.formatId, &finfo);

    g.cout << "BytesPerPixel: " << finfo.bpp << endl;
    g.cout << "Format: " << FourCCToString(finfo.fourcc) << endl;

    std::vector<uint8_t> buffer;
    buffer.resize(openFormat.width*openFormat.height*3);

    Image img(g, openFormat.width, openFormat.height, RED, true);

    if (Cap_captureFrame(ctx, streamId, &buffer[0], openFormat.width*openFormat.height*3) == CAPRESULT_OK)
    {
        Color *pixels = img.pixels();

        for (int i = 0; i < buffer.size(); i+= 3) {
            Color& c{pixels[i/3]};
            c.r = buffer[i];
            c.g = buffer[i+1];
            c.b = buffer[i+2];
        }

        img.updatePixels();
    }

    return img;
}

CameraInfo::CameraInfo(Graphics &g) : g{g}
{
    refresh();
}

CameraInfo::~CameraInfo()
{
    Cap_releaseContext(ctx);
}

int CameraInfo::numDevices()
{
    return deviceCount;
}

DeviceInfo CameraInfo::deviceInfo(int deviceIndex)
{
    DeviceInfo di;

    di.index = deviceIndex;
    di.name = Cap_getDeviceName(ctx, deviceIndex);
    di.uniqueId = Cap_getDeviceUniqueID(ctx, deviceIndex);

    return di;
}

std::vector<FormatInfo> CameraInfo::formats(int deviceIndex)
{
    std::vector<FormatInfo> fmts;

    // show all supported frame buffer formats
    int32_t nFormats = Cap_getNumFormats(ctx, deviceIndex);

    std::string fourccString;

    for(int32_t j=0; j<nFormats; j++)
    {
        FormatInfo fi;

        CapFormatInfo finfo;
        Cap_getFormatInfo(ctx, deviceIndex, j, &finfo);
        fourccString = FourCCToString(finfo.fourcc);

        stringstream ss;

        ss << finfo.width << "x" << finfo.height << ":" << fourccString;

        fi.width  = finfo.width;
        fi.height = finfo.height;
        fi.formatId = j;
        fi.deviceId = deviceIndex;
        fi.formatName = ss.str();
        fmts.push_back(fi);

    }


    return fmts;
}

void CameraInfo::refresh()
{
    if (ctx) {
        Cap_releaseContext(ctx);
    }

    ctx = Cap_createContext();

    deviceCount = Cap_getDeviceCount(ctx);
}
