#ifndef COREWINDOW_H
#define COREWINDOW_H

#include <vector>
#include <iostream>

#include "eventmanager.h"

namespace gjh {

class CoreWindow : public EventManager
{

    std::string title;

    bool        configured{false};  // configures on first call to update
    bool        closed{false};

    bool        requestToggleFullScreen{false};
    int         currentWidth{100};
    int         currentHeight{100};

    bool        gotResizeEvent{false};
public:
    CoreWindow(std::string title, int width, int height);
    virtual ~CoreWindow();
protected:
    virtual void detachRenderer(bool releaseSurface) { }
private:

public:
    int    width()   { return currentWidth; }
    int    height()  { return currentHeight; }

    std::string getTitle() { return title; }

    bool update(bool autoEndDrawing);
    void close() { closed = true; }

    double mousePosNormalizedX() const { return 2.0 * mousePosX() / currentWidth - 1.0; }
    double mousePosNormalizedY() const { return 1.0 - 2.0 * mousePosY() / currentHeight; }

    virtual void setCursor(CoreWindowCursor cursor) = 0;

protected:
    virtual void configure();
    virtual void lateConfigure();
    virtual bool shouldClose() const = 0;
    virtual void handleToggleFullScreen() = 0;
    virtual void pollEvents() = 0;
    virtual void beginDrawing(bool wasResized) = 0;
    virtual void endDrawing() = 0;
protected:
    bool isClosed() const { return closed; }
    void toggleFullScreen() override;
    void setGotResize(int width, int height) override;


};
}

#endif // COREWINDOW_H
