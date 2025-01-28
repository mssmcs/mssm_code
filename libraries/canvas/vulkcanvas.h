#ifndef VULKCANVAS_H
#define VULKCANVAS_H

#include "triwriter.h"
#include "vertextypes.h"
#include "vulkcanvasbase.h"

#include "canvas.h"
#include "vec2d.h"
#include "vec3d.h"
#include "linmath.h"
#include "vfontrenderer.h"

#include <numbers>

struct UniformBufferObject {
    mat4x4 view;
    mat4x4 proj;
    mat4x4 view3d;
    mat4x4 proj3d;
    alignas(16) Vec3f lightPosition;
    alignas(16) Vec3f lightColor;
    alignas(16) Vec3f viewPos; // Camera position
};

#undef near
#undef far

class CameraParams {
public:
    Vec3d camera;
    Vec3d target;
    Vec3d up{0, 1, 0};
    double fov{1.57079632679}; // 90 degrees
    double near{10};
    double far{200};
};

class ViewProj {
public:
    enum class OrthoLayout {
        originLowerLeft,
        originUpperLeft,
    };
public:
    mat4x4 view; // world to view
    mat4x4 proj; // view to clip
public:
    ViewProj() {
        mat4x4_identity(view);
        mat4x4_identity(proj);
    };

    void setupPerspective(CameraParams &cameraParams, int windowWidth, int windowHeight) {
        setupPerspective(cameraParams.fov, windowWidth, windowHeight, cameraParams.near, cameraParams.far);
        setCamera(cameraParams.camera, cameraParams.target, cameraParams.up);
    }

    // these two methods set up projection matrix
    void setupOrtho(OrthoLayout layout, int windowWidth, int windowHeight, double near, double far);
    void setupPerspective(double fov, int windowWidth, int windowHeight, double near, double far);

    // these two methods set up the view matrix
    void setCamera(Vec3d eye, Vec3d lookAt, Vec3d up);
    void setCamera2d(Vec2d pos, double scale);

    void assignTo(mat4x4 v, mat4x4 p) {
        mat4x4_dup(v, view);
        mat4x4_dup(p, proj);
    }
};

struct PushConstant {
    mat4x4 model;
};

class VulkCanvas : public VulkCanvasBase, public mssm::Canvas
{
    uint32_t maxNumTextures = 20;

    bool daniel{false};

    enum class EllipseForm { full, chord, arc, pie };

    std::unique_ptr<VulkFontRenderer> fontRenderer;

    VulkDescSetLayout* descSetLayout1;
    VulkDescSetLayout* descSetLayout2;

    VulkPipelineLayout pipelineLayout;
    VulkDescSet        descSet1;
    VulkDescSet        descSet2;

    VulkPipelineDescriptorSets pipelineDS;

    VulkBoundPipeline plRect;
    VulkBoundPipeline plEllipse;
    VulkBoundPipeline plPoints;
    VulkBoundPipeline plGradientTri;
    VulkBoundPipeline plGradientLine;
    VulkBoundPipeline plTexturedRect;
    VulkBoundPipeline plTexturedRectUV;
    VulkBoundPipeline plTexturedTri;
    VulkBoundPipeline plFontTri;
    VulkBoundPipeline pl3dLine;
    VulkBoundPipeline pl3dTri;

    std::unique_ptr<VulkUniformBuffer<UniformBufferObject>> uniformBuffer;

    std::vector<VkRect2D> clipRects;

    VulkSampler textureSampler;

    VulkImage *fontAtlas;

    VulkSmartBuffer<RectVert> *vRect;
    VulkSmartBuffer<TexturedRectVert> *vTexturedRect;
    VulkSmartBuffer<RectVertUV> *vTexturedRectUV;
    VulkSmartBuffer<Vertex2d> *vBuff2d;
    VulkSmartBuffer<Vertex2dUV> *vBuff2dUV;
    VulkSmartBuffer<Vertex3dUV> *vBuff3dUV;
    VulkSmartBuffer<uint32_t> *iBuff;


public:
    VulkCanvas(VulkSurfaceRenderManager &renderManager);


    UniformBufferObject ubo;

    CameraParams cameraParams;
    Vec3f lightPosition;


    void initializePipelines() override;


    Vertex2d &getVertex(uint32_t idx) { return vBuff2d->elementAt(idx); }

  //  Vertex2dUV &getVertexUV(uint32_t idx) { return vBuff2dUV->elementAt(idx); }

    // Canvas interface
    void setModelMatrix(mat4x4 &model) override;
    void resetModelMatrix() override;
    void setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far) override;
    void setLightParams(Vec3d pos, mssm::Color color) override;

protected:

    void setModelMatrixRotate(Vec2d center, double angle);

    template<typename T>
    void t_polygon(T points, mssm::Color border, mssm::Color fill);

    template<typename T>
    void t_polyline(T points, mssm::Color color, bool closed);

    template<typename T>
    void t_points(T points, mssm::Color c);

    void renderFont(const float *verts,
                    const float *tcoords,
                    const unsigned int *colors,
                    int nverts);

public:
    void drawTimeStats();
    void setEllipseMode(bool daniel);
    void beginPaint() override;
    void endPaint() override;
    virtual int width() override;
    virtual int height() override;
    virtual void setBackground(mssm::Color c) override;
    virtual void line(Vec2d p1, Vec2d p2, mssm::Color c) override;
    virtual void ellipse(
        Vec2d center, double width, double height, mssm::Color c, mssm::Color f) override;
    virtual void arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) override;
    virtual void chord(Vec2d center,
                       double w,
                       double h,
                       double a,
                       double alen,
                       mssm::Color c,
                       mssm::Color f) override;
    virtual void pie(Vec2d center,
                     double w,
                     double h,
                     double a,
                     double alen,
                     mssm::Color c,
                     mssm::Color f) override;
    virtual void rect(
        Vec2d corner, double w, double h, mssm::Color border, mssm::Color fill) override;


    void line(Vec3d p1, Vec3d p2, mssm::Color c) override;
    void polygon(const std::vector<Vec3d> &points,
                         mssm::Color border,
                         mssm::Color fill) override;

    virtual void polygon(const std::vector<Vec2d> &points,
                         mssm::Color border,
                         mssm::Color fill) override;
    virtual void polyline(const std::vector<Vec2d> &points, mssm::Color color) override;
    virtual void points(const std::vector<Vec2d> &points, mssm::Color c) override;
    virtual void polygon(std::initializer_list<Vec2d> points,
                         mssm::Color border,
                         mssm::Color fill) override;
    virtual void polyline(std::initializer_list<Vec2d> points, mssm::Color color) override;
    virtual void points(std::initializer_list<Vec2d> points, mssm::Color c) override;
    virtual void text(Vec2d pos,
                      const FontInfo &sizeAndFace,
                      const std::string &str,
                      mssm::Color textcolor,
                      HAlign hAlign,
                      VAlign vAlign) override;
    virtual void textExtents(const FontInfo &sizeAndFace,
                             const std::string &str,
                             TextExtents &extents) override;
    virtual double textWidth(const FontInfo &sizeAndFace, const std::string &str) override;
    std::vector<double> getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text) override;
    virtual void point(Vec2d pos, mssm::Color c) override;
    virtual void image(Vec2d pos, const mssm::Image &img) override;
    virtual void image(Vec2d pos, const mssm::Image &img, Vec2d src, int srcw, int srch) override;
    virtual void image(Vec2d pos, double w, double h, const mssm::Image &img) override;
    virtual void image(Vec2d pos,
                       double w,
                       double h,
                       const mssm::Image &img,
                       Vec2d src,
                       int srcw,
                       int srch) override;
    virtual void imageC(Vec2d center, double angle, const mssm::Image &img) override;
    virtual void imageC(
        Vec2d center, double angle, const mssm::Image &img, Vec2d src, int srcw, int srch) override;
    virtual void imageC(
        Vec2d center, double angle, double w, double h, const mssm::Image &img) override;
    virtual void imageC(Vec2d center,
                        double angle,
                        double w,
                        double h,
                        const mssm::Image &img,
                        Vec2d src,
                        int srcw,
                        int srch) override;
    bool isClipped(Vec2d pos) const override;
    void setClip(int x, int y, int w, int h) override;
    void resetClip() override;
    void pushClip(int x, int y, int w, int h, bool replace) override;
    void popClip() override;
    void setViewport(int x, int y, int w, int h) override;
    void resetViewport() override;

    // Canvas interface
public:
    virtual bool isDrawable() override;

protected:
    template<EllipseForm form>
    void generalizedEllipse(Vec2d center,
                            double width,
                            double height,
                            mssm::Color border,
                            mssm::Color fill = {0, 0, 0, 0},
                            double aStart = 0,
                            double aLength = std::numbers::pi * 2)
    {
        uint32_t sideCount = std::max(6u,
                                      std::min(50u, static_cast<uint32_t>(std::max(width, height))));

        iBuff->ensureSpace(sideCount * 3);
        vBuff2d->ensureSpace(sideCount + 3);

        double deltaA = aLength / sideCount;

        if constexpr (form != EllipseForm::arc) {
            if (fill.a > 0) {
                auto idxStart = iBuff->nextVertIdx();
                auto vIdxC = vBuff2d->nextVertIdx();
                double angle = aStart;
                Vec2d p0 = {center.x + (width / 2) * cos(-angle),
                            center.y + (height / 2) * sin(-angle)};
                if (form == EllipseForm::chord) {
                    Vec2d pn = {center.x + (width / 2) * cos(-(angle + aLength)),
                                center.y + (height / 2) * sin(-(angle + aLength))};
                    vBuff2d->push((p0 + pn) / 2, fill);
                } else {
                    vBuff2d->push(center, fill);
                }
                auto prevIdx = vBuff2d->push(p0, fill);
                for (int i = 0; i < sideCount; i++) {
                    angle += deltaA;
                    Vec2d p = {center.x + (width / 2) * cos(-angle),
                               center.y + (height / 2) * sin(-angle)};
                    auto idx = vBuff2d->push(p, fill);
                    iBuff->push(vIdxC);
                    iBuff->push(prevIdx);
                    iBuff->push(idx);
                    prevIdx = idx;
                }

                dc->cmdBindPipeline(plGradientTri, pipelineDS);
                dc->commandBuffer->bindIndexBuffer(iBuff->buffer(), 0);
                dc->commandBuffer->drawIndexed(sideCount * 3, 1, idxStart, 0, 0);
            }
        }
        if (border.a > 0 && border != fill) {
            this->vBuff2d->ensureSpace(sideCount + 6);

            auto pointCount = sideCount + 1;
            double angle = aStart;
            auto startVertex = vBuff2d->nextVertIdx();
            for (int i = 0; i < pointCount; i++) {
                Vec2d p = {center.x + (width / 2) * cos(-angle),
                           center.y + (height / 2) * sin(-angle)};
                vBuff2d->push(p, border);
                angle += deltaA;
            }
            if constexpr (form == EllipseForm::chord) {
                vBuff2d->push(getVertex(startVertex));
                pointCount++;
            } else if constexpr (form == EllipseForm::pie) {
                vBuff2d->push(center, border);
                vBuff2d->push(getVertex(startVertex));
                pointCount += 2;
            }
            dc->cmdBindPipeline(plGradientLine, pipelineDS);
            dc->commandBuffer->draw(pointCount, 1, startVertex, 0);
        }
    }
  public:
    TriWriter<Vertex3dUV> getTriangleWriter(uint32_t triCount) override
	{
        dc->cmdBindPipeline(pl3dTri, pipelineDS);
		return TriWriter<Vertex3dUV>(dc, *vBuff3dUV, triCount);
	}
};

#endif // VULKCANVAS_H
