#ifndef FONTRENDERER_H
#define FONTRENDERER_H

#include <string>
#include <vector>

#include "color.h"
#include "vulkimage.h"
#include "textinfo.h"
#include <functional>

typedef struct FONScontext FONScontext;

class VulkFontRenderer
{
    std::function<void(const float *verts,
                       const float *tcoords,
                       const unsigned int *colors,
                       int nverts)> drawCallback;

    VulkCommandPool& commandPool;

    VulkImage* fontAtlas;

    FONScontext *fs = NULL;

    int fontNormal;
    int fontItalic;
    int fontBold;

    uint32_t tex{0};
    int width, height;

    std::vector<float> vertCache;
    std::vector<unsigned int> colorCache;
    std::vector<float> tcoordCache;

    int totalVerts{0};

public:
    VulkFontRenderer(VulkCommandPool& commandPool,
                     VulkImage* fontAtlas,
                     std::function<void(const float *verts,
                                        const float *tcoords,
                                        const unsigned int *colors,
                                        int nverts)> drawCallback,
                 std::string regularPath,
                 std::string italicPath,
                 std::string boldPath);
    ~VulkFontRenderer();

    //  void display(int width, int height);

    void draw(double x,
              double y,
              int size,
              const std::string &str,
              mssm::Color textColor = mssm::WHITE,
              HAlign hAlign = HAlign::left,
              VAlign vAlign = VAlign::baseline);

    template<typename V2D>
    void draw(V2D pos,
              int size,
              const std::string &str,
              mssm::Color textColor = mssm::WHITE,
              HAlign hAlign = HAlign::left,
              VAlign vAlign = VAlign::baseline)
    {
        draw(pos.x, pos.y, size, str, textColor, hAlign, vAlign);
    }

    void   textExtents(const FontInfo &sizeAndFace, const std::string& str, TextExtents& extents);
    double textWidth(const FontInfo &sizeAndFace, const std::string& str);
    std::vector<double> getCharacterXOffsets(const FontInfo &sizeAndFace, double startX, const std::string& text);

    void flush();

private:
    int fontRenderCreate(int width, int height);
    int fontRenderResize(int width, int height);
    void fontRenderUpdate(int *rect, const unsigned char *data);
    void fontRenderDraw(const float *verts,
                        const float *tcoords,
                        const unsigned int *colors,
                        int nverts);
    void fontRenderDelete();

    friend int vfontRenderCreate(void *uptr, int width, int height);
    friend int vfontRenderResize(void *uptr, int width, int height);
    friend void vfontRenderUpdate(void *uptr, int *rect, const unsigned char *data);
    friend void vfontRenderDraw(void *uptr,
                        const float *verts,
                        const float *tcoords,
                        const unsigned int *colors,
                        int nverts);
    friend void vfontRenderDelete(void *uptr);
};

#endif // FONTRENDERER_H
