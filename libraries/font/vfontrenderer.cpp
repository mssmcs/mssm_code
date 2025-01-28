#include "vfontrenderer.h"
#include "color.h"
//#include "vulkcanvas.h"

#include <stdio.h>
#include <string.h>

#define FONTSTASH_IMPLEMENTATION

//#define FONS_USE_FREETYPE

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used stuff from Windows headers
#include <windows.h>
//#include <minwindef.h>
#include <stringapiset.h>
#endif



#include "fontstash.h"


// Function to compute x offsets for each character in a string
// NOTE: font, size must be set appropriately before calling this function
std::vector<double> getCharacterXOffsets(FONScontext* fs, const FontInfo &sizeAndFace, double startX, std::string text) {
    std::vector<double> xOffsets;
    FONStextIter iter;
    FONSquad quad;

    text.append(1, '_'); // Add a char at the end to get the last character's x position

    fonsClearState(fs);
    fonsSetSize(fs, sizeAndFace.getSize());
    fonsSetFont(fs, sizeAndFace.getFaceIdx());

    // Initialize the iterator
    fonsTextIterInit(fs, &iter, static_cast<float>(startX), 0.0f, text.c_str(), nullptr); // Cast startX to float

    // Iterate over the text and collect x positions
    while (fonsTextIterNext(fs, &iter, &quad)) {
        xOffsets.push_back(static_cast<double>(iter.x)); // Convert iter.x from float to double
    }

    return xOffsets;
}


using namespace std;

int vfontRenderCreate(void *uptr, int width, int height)
{
    return reinterpret_cast<VulkFontRenderer *>(uptr)->fontRenderCreate(width, height);
}

int vfontRenderResize(void *uptr, int width, int height)
{
    return reinterpret_cast<VulkFontRenderer *>(uptr)->fontRenderResize(width, height);
}

void vfontRenderUpdate(void *uptr, int *rect, const unsigned char *data)
{
    reinterpret_cast<VulkFontRenderer *>(uptr)->fontRenderUpdate(rect, data);
}

void vfontRenderDraw(
    void *uptr, const float *verts, const float *tcoords, const unsigned int *colors, int nverts)
{
    reinterpret_cast<VulkFontRenderer *>(uptr)->fontRenderDraw(verts, tcoords, colors, nverts);
}

void vfontRenderDelete(void *uptr)
{
    reinterpret_cast<VulkFontRenderer *>(uptr)->fontRenderDelete();
}

static void expandAtlas(FONScontext *stash)
{
    int w = 0, h = 0;
    fonsGetAtlasSize(stash, &w, &h);

    if (w < h)
        w *= 2;
    else
        h *= 2;

    if (h > 1024) {
        fonsResetAtlas(stash, 1024, 1024);
        printf("Reset atlas\n");
    } else {
        fonsExpandAtlas(stash, w, h);
        printf("expanded atlas to %d x %d\n", w, h);
    }
}

static void resetAtlas(FONScontext *stash)
{
    fonsResetAtlas(stash, 256, 256);
    printf("reset atlas to 256 x 256\n");
}

void vstashError(void *uptr, int error, int val)
{
    (void) uptr;
    FONScontext *stash = (FONScontext *) uptr;
    switch (error) {
    case FONS_ATLAS_FULL:
        printf("atlas full\n");
        expandAtlas(stash);
        break;
    case FONS_SCRATCH_FULL:
        printf("scratch full, tried to allocate %d\n", val); //  has %d\n", val, FONS_SCRATCH_BUF_SIZE);
        break;
    case FONS_STATES_OVERFLOW:
        printf("states overflow\n");
        break;
    case FONS_STATES_UNDERFLOW:
        printf("statels underflow\n");
        break;
    }
}

FONS_DEF FONScontext *glfonsCreate(VulkFontRenderer *fr, int width, int height, int flags)
{
    FONSparams params;

    memset(&params, 0, sizeof(params));
    params.width = width;
    params.height = height;
    params.flags = (unsigned char) flags;
    params.renderCreate = vfontRenderCreate;
    params.renderResize = vfontRenderResize;
    params.renderUpdate = vfontRenderUpdate;
    params.renderDraw = vfontRenderDraw;
    params.renderDelete = vfontRenderDelete;
    params.userPtr = fr;

    return fonsCreateInternal(&params);
}

VulkFontRenderer::VulkFontRenderer(VulkCommandPool &commandPool,
                                   VulkImage* fontAtlas,
                                   std::function<void(const float *verts,
                                                      const float *tcoords,
                                                      const unsigned int *colors,
                                                      int nverts)> drawCallback,
                                   string regularPath,
                                   string italicPath,
                                   string boldPath)
    : commandPool{commandPool}, fontAtlas{fontAtlas}, drawCallback{drawCallback}
{
    fs = glfonsCreate(this, 512, 512, FONS_ZERO_TOPLEFT);
    if (fs == NULL) {
        printf("Could not create stash.\n");
    }
    fonsSetErrorCallback(fs, vstashError, fs);

    fontNormal = fonsAddFont(fs, "sans", regularPath.c_str());
    if (fontNormal == FONS_INVALID) {
        printf("Could not add font normal.\n");
        //return;
    }
    fontItalic = fonsAddFont(fs, "sans-italic", italicPath.c_str());
    if (fontItalic == FONS_INVALID) {
        printf("Could not add font italic.\n");
        //return;
    }
    fontBold = fonsAddFont(fs, "sans-bold", boldPath.c_str());
    if (fontBold == FONS_INVALID) {
        printf("Could not add font bold.\n");
       // return;
    }
}

VulkFontRenderer::~VulkFontRenderer()
{
    fonsDeleteInternal(fs);
}


FONS_DEF unsigned int vulkFonsRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return (r) | (g << 8) | (b << 16) | (a << 24);
}

int VulkFontRenderer::fontRenderCreate(int width, int height)
{

    this->width = width;
    this->height = height;

    return 1;
}

int VulkFontRenderer::fontRenderResize(int width, int height)
{
    //cout << std::this_thread::get_id() << "FRR ";

    // Reuse create to resize too.
    return fontRenderCreate(width, height);
}

void VulkFontRenderer::fontRenderUpdate(int *rect, const unsigned char *data)
{
    //cout << "UPD " << endl;

    VkRect2D rect2d{rect[0], rect[1], static_cast<uint32_t>(rect[2]), static_cast<uint32_t>(rect[3])};

    auto numPixels = fontAtlas->width()*fontAtlas->height();

    fontAtlas->updatePixels<unsigned char>(commandPool, rect2d, { data, numPixels });

}

void VulkFontRenderer::fontRenderDraw(const float *verts,
                                  const float *tcoords,
                                  const unsigned int *colors,
                                  int nverts)
{
    drawCallback(verts, tcoords, colors, nverts);
}

void VulkFontRenderer::flush()
{
    vertCache.clear();
    tcoordCache.clear();
    colorCache.clear();
}

void VulkFontRenderer::fontRenderDelete()
{
}

void VulkFontRenderer::draw(double x, double y, int size, const std::string &str, mssm::Color textColor, HAlign hAlign, VAlign vAlign)
{
    fonsClearState(fs);
    fonsSetSize(fs, size);
    fonsSetFont(fs, fontNormal);
    fonsSetColor(fs, textColor.toUIntARGB());
    fonsSetAlign(fs, static_cast<int>(hAlign) | static_cast<int>(vAlign));
    fonsDrawText(fs, x, y, str.c_str(), NULL);


}

void VulkFontRenderer::textExtents(const FontInfo &sizeAndFace, const std::string &str, TextExtents &extents)
{
    float metrics[4]{};

    fonsSetSize(fs, sizeAndFace.getSize());
    fonsSetFont(fs, sizeAndFace.getFaceIdx());

    double advance = fonsTextBounds(fs, 0, 0, str.c_str(), str.c_str() + str.length(), metrics);

    extents.textHeight = metrics[3]-metrics[1];  // ymax-ymin
    extents.textWidth  = metrics[2]-metrics[0];  // xmax-xmin

    extents.textAdvance = advance;

    fonsVertMetrics(fs, &extents.fontAscent, &extents.fontDescent, &extents.fontHeight);
}

double VulkFontRenderer::textWidth(const FontInfo &sizeAndFace, const std::string &str)
{
    if (str.contains(' ')) {  // TODO: hack, fonsTextBounds doesn't handle spaces correctly
        auto s = str;
        std::replace(s.begin(), s.end(), ' ', '_');
        return textWidth(sizeAndFace, s);
    }

    float metrics[4]{};

    fonsSetSize(fs, sizeAndFace.getSize());
    fonsSetFont(fs, sizeAndFace.getFaceIdx());

    fonsTextBounds(fs, 0, 0, str.c_str(), str.c_str() + str.length(), metrics);

    return metrics[2]-metrics[0];  // xmax-xmin
}

std::vector<double> VulkFontRenderer::getCharacterXOffsets(const FontInfo &sizeAndFace,
                                                           double startX,
                                                           const std::string &text)
{
    return ::getCharacterXOffsets(fs, sizeAndFace, startX, text);
}
