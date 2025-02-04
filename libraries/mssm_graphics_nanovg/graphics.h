#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "graphicsbase.h"

#include "nanovgwindow.h"

namespace mssm {

class Graphics : public mssm::GraphicsBase<NanovgWindow, mssm::Canvas2d>, public Canvas2dWrapper
{
public:
    Graphics(std::string title, int width, int height);
    virtual ~Graphics() {}
};

}

#endif // GRAPHICS_H
