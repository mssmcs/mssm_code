#ifndef DAWNGRAPHICSBACKEND_H
#define DAWNGRAPHICSBACKEND_H

#include "graphicsbackend.h"
#include "webgpu_context.h"

class DawnGraphicsBackend : public GraphicsBackendBase<mssm::CoreWindow, mssm::Canvas2d> {
private:
    mssm::WebGpuContext context;
public:
    DawnGraphicsBackend() = default;
    ~DawnGraphicsBackend() override = default;

    mssm::EventSource* eventSource() override;
    mssm::Canvas2d* getCanvas1() override;
    mssm::CoreWindow* getWindow1() override;
    void setCursor(mssm::CoreWindowCursor cursor) override;
    mssm::ImageLoader* getImageLoader() override;
    MeshLoader* getMeshLoader() override;

    bool initialize(std::string& errorMessage);
};

#endif // DAWNGRAPHICSBACKEND_H

