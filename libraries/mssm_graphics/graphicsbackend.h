#ifndef GRAPHICSBACKEND_H
#define GRAPHICSBACKEND_H

#include "canvas2d.h"
#include "windowbase.h"
#include "image.h"

template <typename WINDOW, typename CANVAS>
class GraphicsBackendBase : public mssm::ImageLoader {
public:
    virtual ~GraphicsBackendBase() {}
    virtual gjh::EventSource* eventSource() =0;
    virtual CANVAS* getCanvas() =0;
    virtual WINDOW* getWindow() =0;
    virtual void setCursor(gjh::CoreWindowCursor cursor) = 0;
};

template <typename WINDOW, typename CANVAS>
class GraphicsBackend : public GraphicsBackendBase<WINDOW, CANVAS>
{
protected:
    WINDOW *window;
    CANVAS *canvas{};
public:
    gjh::EventSource* eventSource() { return window; }
    CANVAS* getCanvas() { return canvas; }
    WINDOW* getWindow() { return window; }
    virtual ~GraphicsBackend() {}
};

#endif // GRAPHICSBACKEND_H
