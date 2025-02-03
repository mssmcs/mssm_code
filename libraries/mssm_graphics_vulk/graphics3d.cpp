#include "graphics3d.h"


GraphicsBackend<VulkBackendWindow, mssm::Canvas3d> *loadGraphicsBackend(std::string title, int width, int height);


mssm::Graphics3d::Graphics3d(std::string title, int width, int height)
    : GraphicsBase(title, width, height,loadGraphicsBackend),
    Canvas3dWrapper(backend->getCanvas())
{

}
