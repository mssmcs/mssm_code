#ifndef CAMERA_H
#define CAMERA_H

#include "graphics.h"
#include "openpnp-capture.h"

class FormatInfo {
public:
    int deviceId;
    int formatId;
    int width;
    int height;
    string deviceName;
    string formatName;
};

class DeviceInfo {
public:
    int    index;
    string name;
    string uniqueId;
};

class CameraInfo
{
    mssm::Graphics& g;
    bool contextCreated{false};
    CapContext ctx{nullptr};
    int deviceCount;
public:
    CameraInfo(mssm::Graphics& g);
    virtual ~CameraInfo();
    int numDevices();
    DeviceInfo deviceInfo(int deviceIndex);
    std::vector<FormatInfo> formats(int deviceIndex);
    void refresh();
};

class Camera
{
    mssm::Graphics& g;
    bool contextCreated{false};
    CapContext ctx;
    int32_t streamId{-1};
    uint32_t deviceCount;
    FormatInfo openFormat;
public:
    Camera( mssm::Graphics& g);
    virtual ~Camera();
    bool open(const FormatInfo& format);
    bool isOpen();
    mssm::Image capture();
};

#endif // CAMERA_H
