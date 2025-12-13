#ifndef GRAPHICSBACKEND_H
#define GRAPHICSBACKEND_H

#include "canvas2d.h"
#include "windowbase.h"
#include "image.h"
#include "staticmesh.h"

template <typename WINDOW, typename CANVAS>
class GraphicsBackendBase {
public:
    virtual ~GraphicsBackendBase() {}
    virtual mssm::EventSource* eventSource() =0;
    virtual CANVAS* getCanvas1() =0;
    virtual WINDOW* getWindow1() =0;
    virtual void setCursor(mssm::CoreWindowCursor cursor) = 0;
    virtual mssm::ImageLoader* getImageLoader() = 0;
    virtual MeshLoader* getMeshLoader() = 0;
};

template <typename WINDOW, typename CANVAS>
class GraphicsBackend : public GraphicsBackendBase<WINDOW, CANVAS>
{
protected:
    WINDOW *window;
    CANVAS *canvas{};
public:
    mssm::EventSource* eventSource() { return window; }
    CANVAS* getCanvas1() { return canvas; }
    WINDOW* getWindow1() { return window; }
    virtual ~GraphicsBackend() {}
};

#endif // GRAPHICSBACKEND_H
