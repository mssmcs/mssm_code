#ifndef GRAPHICSBACKEND_H
#define GRAPHICSBACKEND_H

#include "canvas2d.h"
#include "corewindow.h"
#include "image.h"

class GraphicsBackendBase : public mssm::ImageLoader {
public:
    virtual ~GraphicsBackendBase() {}
    virtual gjh::EventSource* eventSource() =0;
    virtual mssm::Canvas2d* getCanvas() =0;
    virtual gjh::CoreWindow *getWindow() =0;
    virtual void setCursor(gjh::CoreWindowCursor cursor) = 0;
};

template <typename WINDOW, typename CANVAS>
class GraphicsBackend : public GraphicsBackendBase
{
protected:
    WINDOW *window;
    CANVAS *canvas{};
public:
    gjh::EventSource* eventSource() { return window; }
    mssm::Canvas2d* getCanvas() { return canvas; }
    gjh::CoreWindow *getWindow() { return window; }
    virtual ~GraphicsBackend() {}
};

#endif // GRAPHICSBACKEND_H
