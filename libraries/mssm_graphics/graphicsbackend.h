#ifndef GRAPHICSBACKEND_H
#define GRAPHICSBACKEND_H

// #include "canvas2d.h"
#include "corewindow.h"
#include "image.h"

template <typename WINDOW, typename CANVAS>
class GraphicsBackend : public mssm::ImageLoader
{
protected:
    WINDOW *window;
    CANVAS *canvas{};
public:
    gjh::EventSource* eventSource() { return window; }
    CANVAS* getCanvas() { return canvas; }
    WINDOW *getWindow() { return window; }
    virtual ~GraphicsBackend() {};
    virtual void setCursor(gjh::CoreWindowCursor cursor) = 0;
};

#endif // GRAPHICSBACKEND_H
