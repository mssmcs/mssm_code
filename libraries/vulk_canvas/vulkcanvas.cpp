#include "vulkcanvas.h"
//#include "geometry.h"
#include "image.h"
#include "paths.h"
#include "polypartition.h"
#include "vfontrenderer.h"
#include <vector>
#include "vertexattrvulk.h"

using namespace mssm;

void addAttribute(std::vector<VkVertexInputAttributeDescription> &attributeDescriptions, VkFormat format, size_t offset)
{
    VkVertexInputAttributeDescription attributeDescription{};
    attributeDescription.binding = 0;
    attributeDescription.location = attributeDescriptions.size();  // note: overwritten later
    attributeDescription.format = format;
    attributeDescription.offset = offset;
    attributeDescriptions.push_back(attributeDescription);
}

inline void ViewProj::setupOrtho(OrthoLayout layout,
                                 int windowWidth,
                                 int windowHeight,
                                 double near,
                                 double far)
{
    switch (layout) {
    case OrthoLayout::originLowerLeft:
        mat4x4_ortho(proj, -1, windowWidth-1, -1, windowHeight-1, near, far);
        break;
    case OrthoLayout::originUpperLeft:
        mat4x4_ortho(proj, 0, windowWidth, windowHeight, 0, near, far);
        break;
    }
}

void perspective(mat4x4 m, float width, float height, float near, float far)
{
    const auto twoNear = 2.0f * near;
    const auto farMinusNear = far - near;

    m[0][0] = twoNear / width;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = -twoNear / height;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -far / farMinusNear;
    m[2][3] = -1.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = -near * far / farMinusNear;
    m[3][3] = 0.0;
}

// int windowWidth, int windowHeight just to calculate aspect ratio
inline void ViewProj::setupPerspective(double fovRadians, int windowWidth, int windowHeight, double near, double far)
{
    double tanHalfFov = tan(fovRadians / 2);
    double aspect = (double)windowWidth / (double)windowHeight;
    double height = 2 * tanHalfFov * near;
    double width = height * aspect;

    perspective(proj, width, height, near, far);
}

inline void ViewProj::setCamera(Vec3d eye, Vec3d lookAt, Vec3d up)
{
    vec3 eyef{static_cast<float>(eye.x),static_cast<float>(eye.y),static_cast<float>(eye.z)};
    vec3 center{static_cast<float>(lookAt.x),static_cast<float>(lookAt.y),static_cast<float>(lookAt.z)};
    vec3 upf{static_cast<float>(up.x),static_cast<float>(up.y),static_cast<float>(up.z)};

    mat4x4_look_at(view, eyef, center, upf);
}

inline void ViewProj::setCamera2d(Vec2d pos, double scale)
{

}

VulkCanvas::VulkCanvas(VulkSurfaceRenderManager &renderManager)
    : VulkCanvasBase{renderManager}
{

}


void VulkCanvas::initializePipelines()
{

    uint32_t bindingOffset = 0;

    descSetLayout1 = descSetManager.addLayout()
        .addUniformBufferBinding()
        .addTextureBinding(maxNumTextures)  // size of texture array
        .build(device, descSetManager, bindingOffset, DescriptorBindingFrequency::PerFrame);

    bindingOffset += descSetLayout1->numBindings();

    descSetLayout2 = descSetManager.addLayout()
        .addTextureBinding(1)
        .build(device, descSetManager, bindingOffset, DescriptorBindingFrequency::PerFrame);

    descSet1 = descSetLayout1->createDescSet();
    descSet2 = descSetLayout2->createDescSet();

    pipelineLayout.initialize<PushConstant>({descSetLayout1, descSetLayout2});

    pipelineDS = VulkPipelineDescriptorSets(pipelineLayout);
    pipelineDS.addDescSet(descSet1);
    pipelineDS.addDescSet(descSet2);

    uniformBuffer = std::make_unique<VulkUniformBuffer<UniformBufferObject>>(device, renderManager.getNumFramesInFlight());

    textureSampler = VulkSampler(device);

    auto fontRegular = Paths::findAsset("DroidSerif-Regular.ttf");
    auto fontItalic = Paths::findAsset("DroidSerif-Italic.ttf");
    auto fontBold = Paths::findAsset("DroidSerif-Bold.ttf");

    fontAtlas = addTexture(512, 512, VK_FORMAT_R8_UNORM);

    fontRenderer = std::unique_ptr<VulkFontRenderer>(new VulkFontRenderer(renderManager.getGraphicsCommandPool(),
                                        fontAtlas,
                                        [this](const float *verts,
                                               const float *tcoords,
                                               const unsigned int *colors,
                                               int nverts) {
                                            renderFont(verts, tcoords, colors, nverts);
                                        },
                                                                          fontRegular, fontItalic, fontBold));



    // BUFFERS

    vRect = renderManager.createBuffer<RectVert>(100);
    vTexturedRect = renderManager.createBuffer<TexturedRectVert>(100);
    vTexturedRectUV = renderManager.createBuffer<RectVertUV>(100);
    vBuff2d = renderManager.createBuffer<Vertex2d>(1000);
    vBuff2dUV = renderManager.createBuffer<Vertex2dUV>(500);
    vBuff3dUV = renderManager.createBuffer<Vertex3dUV>(500);
    iBuff = renderManager.createIndexBuffer<uint32_t>(1000);

    // PIPELINES

    plGradientTri = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        "default.vert.glsl.spv",
        "default.frag.glsl.spv",
        pipelineLayout,
        vBuff2d, false);

    pl3dLine = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        "default3d.vert.glsl.spv",
        "default3d.frag.glsl.spv",
        pipelineLayout,
        vBuff3dUV, true);

    pl3dTri = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        "default3d.vert.glsl.spv",
        "default3d.frag.glsl.spv",
        pipelineLayout,
        vBuff3dUV, true);

    plTexturedRect = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        "texturedRect.vert.glsl.spv",
        "texturedRect.frag.glsl.spv",
        pipelineLayout,
        vTexturedRect, false);

    plTexturedRectUV = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        "texturedRectUV.vert.glsl.spv",
        "texturedRectUV.frag.glsl.spv",
        pipelineLayout,
        vTexturedRectUV, false);

    plGradientLine = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
        "default.vert.glsl.spv",
        "default.frag.glsl.spv",
        pipelineLayout,
        vBuff2d, false);

    plPoints = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        "default.vert.glsl.spv",
        "default.frag.glsl.spv",
        pipelineLayout,
        vBuff2d, false);

    plEllipse = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        "ellipse.vert.glsl.spv",
        "ellipse.frag.glsl.spv",
        pipelineLayout,
        vRect, false);

    plRect = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
        "rect.vert.glsl.spv",
        "rect.frag.glsl.spv",
        pipelineLayout,
        vRect, false);

    plFontTri = createPipeline(
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        "font.vert.glsl.spv",
        "font.frag.glsl.spv",
        pipelineLayout,
        vBuff2dUV, false);

    for (int i = 0; i < renderManager.getNumFramesInFlight(); i++) {
        VulkDescSetUpdates updates(*descSetLayout1, descSet1.handle(i));
        updates.addBufferUpdate(0, uniformBuffer->buffer(i));
        updates.addImageUpdate(1, renderManager.imageViews(), textureSampler, maxNumTextures);
        updates.apply();
    }

    std::cerr << "Done Creating pipelines" << std::endl;

}

void VulkCanvas::setModelMatrix(mat4x4& model)
{
    dc->sendPushConstants(pipelineLayout, model);
}

void VulkCanvas::resetModelMatrix()
{
    mat4x4 model;
    mat4x4_identity(model);
    dc->sendPushConstants(pipelineLayout, model);
}

void VulkCanvas::setCameraParams(const CameraParams &params)
{
    cameraParams = params;
}

void VulkCanvas::setCameraParams(Vec3d eye, Vec3d target, Vec3d up, double near, double far)
{
    this->cameraParams.camera = eye;
    this->cameraParams.target = target;
    this->cameraParams.up = up;
    this->cameraParams.near = near;
    this->cameraParams.far = far;
}

void VulkCanvas::setLightParams(Vec3d pos, mssm::Color color)
{
    this->lightPosition = cast<Vec3f>(pos);
    //this->lightColor = cast<Vec3f>(color);

}

void VulkCanvas::setModelMatrixRotate(Vec2d center, double angle)
{
    mat4x4 model;
    mat4x4 t1;
    mat4x4 r1;
    mat4x4 t2;
    mat4x4_identity(r1);
    mat4x4_translate(t1, -center.x, -center.y, 0);
    mat4x4_rotate_Z(r1, r1, angle);
    mat4x4_translate(t2, center.x, center.y, 0);

    // model = t2 * r1 * t1 * model;

    mat4x4 tmp;
    mat4x4_mul(tmp, t2, r1);
    mat4x4_mul(model, tmp, t1);
  //  mat4x4_mul(model, tmp, model);

    setModelMatrix(model);
}

void VulkCanvas::renderFont(const float *verts,
                            const float *tcoords,
                            const unsigned int *colors,
                            int nverts)
{
    vBuff2dUV->ensureSpace(nverts);
    auto idx = vBuff2dUV->nextVertIdx();
    for (int i = 0; i < nverts; i++) {
        vBuff2dUV->push(Vec2d{verts[i * 2], verts[i * 2 + 1]}, mssm::Color::fromIntARGB(colors[i]), Vec2f{tcoords[i * 2], tcoords[i * 2 + 1]});
    }

    dc->cmdBindPipeline(plFontTri, pipelineDS);
    dc->commandBuffer->draw(nverts, 1, idx, 0);
}

void VulkCanvas::drawTimeStats()
{
    const auto& stats = renderManager.timeStats;
    mssm::Color colors[] = { RED, GREEN, BLUE, YELLOW, CYAN, ORANGE, PURPLE, WHITE };

    // RED wait for frame in flight
    // GREEN vkAcquireNextImageKHR
    // BLUE make all the canvas calls
    // YELLOW vkQueueSubmit
    // CYAN vkQueuePresentKHR

    double scale = 50;

    Vec2d pos{0, 5};
    for (int i = 0; i < stats.size()-1; i++) {
        double w = stats[i] / scale;
        rect(pos, w, 15, mssm::Color{255, 255, 255, 0}, colors[i]);
        pos.x += w;
    }

    rect({0,0}, 1000000.0/60/scale, 20, YELLOW, TRANSPARENT);
}

void VulkCanvas::setEllipseMode(bool daniel)
{
    this->daniel = daniel;
}


void createTransform(mat4x4& proj, mat4x4& view, Vec3d camera, Vec3d target, double width, double height)
{
    mat4x4 i;
    mat4x4 p;

    mat4x4_identity(i);

    //The view matrix controls camera position and angle.
    vec3 eye={static_cast<float>(camera.x),static_cast<float>(camera.y),static_cast<float>(camera.z)};
    vec3 center={static_cast<float>(target.x),static_cast<float>(target.y),static_cast<float>(target.z)};

    vec3 up={0,1,0};
    mat4x4_look_at(view,eye,center,up);

    mat4x4_perspective(proj, 0.75, width/(float)height, 100, 6000);
    //mat4x4_ortho(proj, -1000, 1000, -1000.f, 1000.f, -1000.f, 1000.f);

    // mat4x4 s;
    // mat4x4_scale(s, i, width);  // scale to screen size
    // s[3][3] = 1;

    // mat4x4 t;
    // mat4x4_translate(t, width/2, height/2, 0);  // shift to center of screen

    // mat4x4_mul(proj, t, s);
    // mat4x4_mul(proj, proj, p);
}



void VulkCanvas::beginPaint()
{
    VulkCanvasBase::beginPaint();  // must be first thing

    ViewProj vp2d;
    ViewProj vp3d;

    vp3d.setupPerspective(cameraParams, extent.width, extent.height);

    vp2d.setupOrtho(ViewProj::OrthoLayout::originLowerLeft, extent.width, extent.height, 100, -100);

    vp2d.assignTo(ubo.view, ubo.proj);
    vp3d.assignTo(ubo.view3d, ubo.proj3d);

    ubo.lightColor = Vec3f{1,1,1};
    ubo.viewPos = cast<Vec3f>(cameraParams.camera);
    ubo.lightPosition = lightPosition;

    uniformBuffer->update(renderManager.flightNumber(), ubo);

    if (renderManager.getImagesDirty()) {
        auto views = renderManager.imageViews();
        for (int i = 0; i < renderManager.getNumFramesInFlight(); i++) {
            VulkDescSetUpdates updates(*descSetLayout1, descSet1.handle(i));
            updates.addBufferUpdate(0, uniformBuffer->buffer(i));
            updates.addImageUpdate(1, views, textureSampler, maxNumTextures);
            updates.apply();
        }
        renderManager.clearImagesDirty();
    }

    VulkDescSetUpdates updates2(*descSetLayout2, descSet2.handle(renderManager.flightNumber()));
    updates2.addImageUpdate(2, { fontAtlas->imageView() }, textureSampler, 1);
    updates2.apply();

    resetModelMatrix();
    clipRects.clear();
    resetClip();
    resetViewport();

    dc->commandBuffer->bindIndexBuffer(iBuff->buffer(), 0);
}

void VulkCanvas::endPaint()
{
    this->resetModelMatrix();
    //drawTimeStats();

    VulkCanvasBase::endPaint();  // must be last thing
}

int VulkCanvas::width()
{
    return extent.width;
}

int VulkCanvas::height()
{
    return extent.height;
}

void VulkCanvas::setBackground(mssm::Color c) {}

void VulkCanvas::line(Vec2d p1, Vec2d p2, mssm::Color c)
{
    vBuff2d->ensureSpace(2);

    auto idx = vBuff2d->push(p1, c);
    vBuff2d->push(p2, c);

    dc->cmdBindPipeline(plGradientLine, pipelineDS);
    dc->commandBuffer->draw(2, 1, idx, 0);
}

void VulkCanvas::ellipse(Vec2d center, double width, double height, mssm::Color border, mssm::Color fill)
{
    if (daniel) {
        vRect->ensureSpace(1);
        auto idx = vRect->push(center-Vec2d{width/2, height/2}, width, height, border, fill);
        dc->cmdBindPipeline(plEllipse, pipelineDS);
        dc->commandBuffer->draw(4, 1, 0, idx);
    }
    else {
        generalizedEllipse<EllipseForm::full>(center, width, height, border, fill);
    }
}

void VulkCanvas::arc(Vec2d center, double width, double height, double startAngle, double arcLength, mssm::Color border)
{
    generalizedEllipse<EllipseForm::arc>(center, width, height, border, mssm::TRANSPARENT, startAngle, arcLength);
}

void VulkCanvas::chord(Vec2d center, double width, double height, double startAngle, double arcLength, mssm::Color border, mssm::Color fill)
{
    generalizedEllipse<EllipseForm::chord>(center, width, height, border, fill, startAngle, arcLength);
}

void VulkCanvas::pie(Vec2d center, double width, double height, double startAngle, double arcLength, mssm::Color border, mssm::Color fill)
{
    generalizedEllipse<EllipseForm::pie>(center, width, height, border, fill, startAngle, arcLength);
}

void VulkCanvas::rect(Vec2d corner, double w, double h, mssm::Color border, mssm::Color fill)
{
    // vRect->ensureSpace(1);
    // auto idx = vRect->push(corner, w, h, border, fill);
    // dc->cmdBindPipeline(plRect, pipelineDS);
    // dc->commandBuffer->draw(4, 1, 0, idx);

    bool hasBorder = border.a > 0;
    bool hasFill = fill.a > 0;

    int numVerts = (hasBorder ? 5 : 0) + (hasFill ? 4 : 0);

    vBuff2d->ensureSpace(numVerts);

    Vec2d& ul = corner;
    Vec2d ur = corner + Vec2d{w, 0};
    Vec2d lr = corner + Vec2d{w, h};
    Vec2d ll = corner + Vec2d{0, h};

    uint32_t borderStartIdx{};

    if (hasBorder) {
        borderStartIdx = vBuff2d->push(ul, border);
        vBuff2d->push(ll-Vec2d{0,1}, border);
        vBuff2d->push(lr-Vec2d{1,1}, border);
        vBuff2d->push(ur-Vec2d{1,0}, border);
        vBuff2d->push(ul, border);
    }

    if (hasFill) {
        uint32_t triStartIdx = vBuff2d->push(ul-Vec2d{1,1}, fill);
        vBuff2d->push(ur-Vec2d{1,1}, fill);
        vBuff2d->push(lr-Vec2d{1,1}, fill);
        vBuff2d->push(ll-Vec2d{1,1}, fill);

        iBuff->ensureSpace(6);
        auto startIdx = iBuff->nextVertIdx();
        iBuff->push(static_cast<uint32_t>(triStartIdx));
        iBuff->push(static_cast<uint32_t>(triStartIdx+2));
        iBuff->push(static_cast<uint32_t>(triStartIdx+1));
        iBuff->push(static_cast<uint32_t>(triStartIdx));
        iBuff->push(static_cast<uint32_t>(triStartIdx+3));
        iBuff->push(static_cast<uint32_t>(triStartIdx+2));

        dc->cmdBindPipeline(plGradientTri, pipelineDS);
        dc->commandBuffer->bindIndexBuffer(iBuff->buffer(), 0);
        dc->commandBuffer->drawIndexed(6, 1, startIdx, 0, 0);
    }

    if (hasBorder) {
        dc->cmdBindPipeline(plGradientLine, pipelineDS);
        dc->commandBuffer->draw(5, 1, borderStartIdx, 0);
    }
}


void VulkCanvas::line3d(Vec3d p1, Vec3d p2, mssm::Color c)
{
    vBuff3dUV->ensureSpace(2);
    auto idx = vBuff3dUV->push(p1, Vec3d{ 0,0,0 }, c, Vec2f{ 0,0 });
    vBuff3dUV->push(p2, Vec3d{ 0,0,0 },  c, Vec2f{ 0,0 });
    dc->cmdBindPipeline(pl3dLine, pipelineDS);
    dc->commandBuffer->draw(2, 1, idx, 0);
}

void VulkCanvas::polygon3d(const std::vector<Vec3d> &points, mssm::Color border, mssm::Color fill)
{
    if (points.size() != 3) {
        throw std::logic_error("Only Tris!");
    }

    vBuff3dUV->ensureSpace(3);

    auto idx = vBuff3dUV->nextVertIdx();

    Vec3d normal = crossProduct(points[1] - points[0], points[2] - points[0]).unit();

    for (auto &p : points) {
        vBuff3dUV->push(p, normal, fill, Vec2f{0,0});
    }

    dc->cmdBindPipeline(pl3dTri, pipelineDS);
    dc->commandBuffer->draw(3, 1, idx, 0);
}

void VulkCanvas::text(Vec2d pos,
                      const FontInfo &sizeAndFace,
                      const std::string &str,
                      mssm::Color textcolor,
                      HAlign hAlign = HAlign::left,
                      VAlign vAlign = VAlign::baseline)
{
    //  throw std::runtime_error("VulkCanvas::text() not implemented");
   fontRenderer->draw(pos, sizeAndFace.getSize(), str, textcolor, hAlign, vAlign);
}

void VulkCanvas::textExtents(const FontInfo &sizeAndFace,
                             const std::string &str,
                             TextExtents &extents)
{
    fontRenderer->textExtents(sizeAndFace.getSize(), str, extents);
}

double VulkCanvas::textWidth(const FontInfo &sizeAndFace, const std::string &str)
{
    return fontRenderer->textWidth(sizeAndFace.getSize(), str);
}

std::vector<double> VulkCanvas::getCharacterXOffsets(const FontInfo& sizeAndFace, double startX, const std::string& text)
{
    return fontRenderer->getCharacterXOffsets(sizeAndFace.getSize(), startX, text);
}


void VulkCanvas::point(Vec2d pos, mssm::Color color)
{
    auto startIdx = vBuff2d->nextVertIdx();
    vBuff2d->ensureSpace(1);
    vBuff2d->push(pos, color);
    dc->cmdBindPipeline(plPoints, pipelineDS);
    dc->commandBuffer->draw(1, 1, startIdx, 0);
}

void VulkCanvas::image(Vec2d pos, const mssm::Image &img)
{
    vTexturedRect->ensureSpace(1);
    auto idx = vTexturedRect->push(pos, static_cast<double>(img.width()), static_cast<double>(img.height()), WHITE, img.textureIndex()); // img.width(), img.height(), WHITE, WHITE);
    dc->cmdBindPipeline(this->plTexturedRect, pipelineDS);
    dc->commandBuffer->draw(4, 1, 0, idx);
}

void VulkCanvas::image(Vec2d pos, const mssm::Image &img, Vec2d src, int srcw, int srch)
{
    vTexturedRectUV->ensureSpace(1);
    Vec2f fPos(pos);
    Vec2f fSize(srcw, srch);
    Vec2f uvPos(src.x/img.width(), src.y/img.height());
    Vec2f uvSize(static_cast<float>(srcw)/img.width(), static_cast<float>(srch)/img.height());
    Vec4f fillColor = WHITE.toRealVec4<Vec4f>();
    auto idx = vTexturedRectUV->push(fPos, fSize, uvPos, uvSize, fillColor, img.textureIndex()); // img.width(), img.height(), WHITE, WHITE);
    dc->cmdBindPipeline(this->plTexturedRectUV, pipelineDS);
    dc->commandBuffer->draw(4, 1, 0, idx);

}

void VulkCanvas::image(Vec2d pos, double w, double h, const mssm::Image &img)
{
    vTexturedRect->ensureSpace(1);
    auto idx = vTexturedRect->push(pos, w, h, WHITE, img.textureIndex()); // img.width(), img.height(), WHITE, WHITE);
    dc->cmdBindPipeline(this->plTexturedRect, pipelineDS);
    dc->commandBuffer->draw(4, 1, 0, idx);
}

void VulkCanvas::image(Vec2d pos, double w, double h, const mssm::Image &img, Vec2d src, int srcw, int srch)
{
    vTexturedRectUV->ensureSpace(1);
    Vec2f fPos(pos);
    Vec2f fSize(w, h);
    Vec2f uvPos(src.x/img.width(), src.y/img.height());
    Vec2f uvSize(static_cast<float>(srcw)/img.width(), static_cast<float>(srch)/img.height());
    Vec4f fillColor = WHITE.toRealVec4<Vec4f>();
    auto idx = vTexturedRectUV->push(fPos, fSize, uvPos, uvSize, fillColor, img.textureIndex()); // img.width(), img.height(), WHITE, WHITE);
    dc->cmdBindPipeline(this->plTexturedRectUV, pipelineDS);
    dc->commandBuffer->draw(4, 1, 0, idx);
}

void VulkCanvas::imageC(Vec2d center, double angle, const mssm::Image &img)
{
    setModelMatrixRotate(center, angle);
    image(center - Vec2d{img.width()/2, img.height()/2}, img);
    resetModelMatrix();
//    ellipse(center, 10,10,WHITE, TRANS);
}

void VulkCanvas::imageC(Vec2d center, double angle, const mssm::Image &img, Vec2d src, int srcw, int srch)
{
    setModelMatrixRotate(center, angle);
    image(center - Vec2d{img.width()/2, img.height()/2}, img, src, srcw, srch);
    resetModelMatrix();
//    ellipse(center, 10,10,WHITE, TRANS);
}

void VulkCanvas::imageC(Vec2d center, double angle, double w, double h, const mssm::Image &img)
{
    setModelMatrixRotate(center, angle);
    image(center - Vec2d{w/2, h/2}, w, h, img);
    resetModelMatrix();
//    ellipse(center, 10,10,WHITE, TRANS);
}

void VulkCanvas::imageC(Vec2d center,
                        double angle,
                        double w,
                        double h,
                        const mssm::Image &img,
                        Vec2d src,
                        int srcw,
                        int srch)
{
    setModelMatrixRotate(center, angle);
    image(center - Vec2d{w/2, h/2}, w, h, img, src, srcw, srch);
    resetModelMatrix();
//    ellipse(center, 10,10,WHITE, TRANS);
}

bool VulkCanvas::isClipped(Vec2d pos) const
{
    if (clipRects.empty()) {
        return false;
    }
    auto& clip = clipRects.back();
    return pos.x < clip.offset.x ||
           pos.y < clip.offset.y ||
           pos.x >= clip.offset.x + clip.extent.width ||
           pos.y >= clip.offset.y + clip.extent.height;
}

void VulkCanvas::pushClip(int x, int y, int w, int h, bool replace)
{
    w = std::max(0,w);
    h = std::max(0,h);
    if (replace || clipRects.empty()) {
        VkRect2D rect = {{x, y}, {static_cast<uint32_t>(w), static_cast<uint32_t>(h)}};
        clipRects.push_back(rect);
        dc->commandBuffer->setScissor(rect);
    }
    else {
        // intersect with current clip
        VkRect2D rect;
        VkRect2D& prev = clipRects.back();
        rect.offset.x = std::max(prev.offset.x, x);
        rect.offset.y = std::max(prev.offset.y, y);
        auto oldEndX = static_cast<int32_t>(prev.offset.x + prev.extent.width);
        auto oldEndY = static_cast<int32_t>(prev.offset.y + prev.extent.height);
        auto endX = static_cast<int32_t>(x + w);
        auto endY = static_cast<int32_t>(y + h);
        rect.extent.width = std::max(0, std::min(oldEndX, endX) - rect.offset.x);
        rect.extent.height = std::max(0, std::min(oldEndY, endY) - rect.offset.y);
        clipRects.push_back(rect);
        dc->commandBuffer->setScissor(rect);
    }
}

void VulkCanvas::popClip()
{
    clipRects.pop_back();
    if (clipRects.empty()) {
        resetClip();
    }
    else {
        dc->commandBuffer->setScissor(clipRects.back());
    }
}

void VulkCanvas::setClip(int x, int y, int w, int h)
{
    w = std::max(0,w);
    h = std::max(0,h);
    dc->commandBuffer->setScissor({{x,y},{static_cast<uint32_t>(w),static_cast<uint32_t>(h)}});
}

void VulkCanvas::resetClip()
{
    dc->commandBuffer->setScissor({{0,0},extent});
}

void VulkCanvas::setViewport(int x, int y, int w, int h)
{
    dc->commandBuffer->setViewport({{x,y},{static_cast<uint32_t>(w),static_cast<uint32_t>(h)}});
}

void VulkCanvas::resetViewport()
{
    dc->commandBuffer->setViewport({{0,0},extent});
}

bool VulkCanvas::isDrawable()
{
    return renderManager.isDrawable();
}

template<typename T>
void VulkCanvas::t_polyline(T points, mssm::Color color, bool closed)
{
    int numV = end(points) - begin(points);

    if (numV == 0) {
        return;
    }

    bool addPoint = false;

    if ((closed && numV > 2) || numV == 1) {
        numV++;
        addPoint = true;
    }

    // here we know the number of verts we'll need

    vBuff2d->ensureSpace(numV);

    auto startIdx = vBuff2d->nextVertIdx();

    for (auto &p : points) {
        vBuff2d->push(p, color);
    }

    if (addPoint) {
        vBuff2d->push(*begin(points), color);
    }

    // now we're committed

    dc->cmdBindPipeline(plGradientLine, pipelineDS);
    dc->commandBuffer->draw(numV, 1, startIdx, 0);
}

template<typename T>
void VulkCanvas::t_polygon(T points, mssm::Color border, mssm::Color fill)
{
    int numV = end(points) - begin(points);

    int idx = 0;
    int vIndex = vBuff2d->nextVertIdx();
    TPPLPoly poly;
    poly.Init(numV);
    for (auto &p : points) {
        auto &v = poly[idx++];
        v.x = p.x;
        v.y = p.y;
        v.id = vIndex++;
    }
    poly.SetOrientation(TPPL_ORIENTATION_CCW);

    TPPLPartition pp;
    TPPLPolyList list;

    pp.Triangulate_EC(&poly, &list);

    auto numTriIndices = list.size()*3;

    iBuff->ensureSpace(numTriIndices);
    vBuff2d->ensureSpace(numV);

    for (auto &p : points) {
        vBuff2d->push(Vec2d{p.x, p.y}, fill);
    }

    auto startIdx = iBuff->nextVertIdx();

    for (auto &tri : list) {
        for (auto i = 0; i < tri.GetNumPoints(); ++i) {
            iBuff->push(static_cast<uint32_t>(tri.GetPoint(i).id));
        }
    }

    // vertices pushed

    dc->cmdBindPipeline(plGradientTri, pipelineDS);
    dc->commandBuffer->bindIndexBuffer(iBuff->buffer(), 0);
    dc->commandBuffer->drawIndexed(numTriIndices, 1, startIdx, 0, 0);

    if (border.a > 0 && border != fill) {
        t_polyline(points, border, true);
    }
}

template<typename T>

void VulkCanvas::t_points(T points, mssm::Color color)
{
    auto numV = end(points) - begin(points);
    auto startIdx = vBuff2d->nextVertIdx();

    vBuff2d->ensureSpace(numV);

    for (auto &p : points) {
        vBuff2d->push(p, color);
    }

    dc->cmdBindPipeline(plPoints, pipelineDS);
    dc->commandBuffer->draw(numV, 1, startIdx, 0);
}

void VulkCanvas::polygon(const std::vector<Vec2d> &points, Color border, Color fill)
{
    t_polygon(points, border, fill);
}

void VulkCanvas::polyline(const std::vector<Vec2d> &points, Color color)
{
    t_polyline(points, color, false);
}

void VulkCanvas::points(const std::vector<Vec2d> &points, Color c)
{
    t_points(points, c);
}

#ifdef MSSM_ARRAY_H
void VulkCanvas::polygon(const Array<Vec2d> &points, Color border, Color fill)
{
    t_polygon(points, border, fill);
}

void VulkCanvas::polyline(const Array<Vec2d> &points, Color color)
{
    t_polyline(points, color);
}

void VulkCanvas::points(const Array<Vec2d> &points, Color c)
{
    t_points(points, c);
}
#endif // MSSM_ARRAY_H

void VulkCanvas::polygon(std::initializer_list<Vec2d> points, Color border, Color fill)
{
    t_polygon(points, border, fill);
}

void VulkCanvas::polyline(std::initializer_list<Vec2d> points, Color color)
{
    t_polyline(points, color, false);
}

void VulkCanvas::points(std::initializer_list<Vec2d> points, Color c)
{
    t_points(points, c);
}

// void VulkBoundTexture::load(VulkCommandPool& commandPool, VulkBindingSet& bindingSet, VkSampler textureSampler, std::string filename, bool retainBuffer)
// {
//     image.load(commandPool, Paths::findAsset(filename), retainBuffer);

//     textureBinding = std::make_unique<VulkTexture>(bindingSet, image.imageView(), textureSampler);
// }

// void VulkBoundTexture::create(VulkCommandPool &commandPool,VulkBindingSet &bindingSet,
//                               VkSampler textureSampler,int width,int height, VkFormat format)
// {
//     image.create(commandPool, width, height, format, true);

//     textureBinding = std::make_unique<VulkTexture>(bindingSet, image.imageView(), textureSampler);
// }




void VulkCanvas::pushGroup(std::string groupName)
{
}

void VulkCanvas::popGroup()
{
}

void VulkCanvas::polygonPattern(const std::vector<Vec2d> &points, mssm::Color c, mssm::Color f)
{
    throw std::runtime_error("VulkCanvas::polygonPattern() not implemented");
}

void VulkCanvas::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f)
{
    throw std::runtime_error("VulkCanvas::polygonPattern() not implemented");
}
