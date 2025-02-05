#ifndef WINDOWBASE_H
#define WINDOWBASE_H

#include <vector>
#include <iostream>
#include <chrono>

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

    std::chrono::microseconds::rep start_time;
    std::chrono::steady_clock::time_point lastDrawTime;
    std::chrono::microseconds::rep elapsed;

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
public:
    double timeMicros() const override
    {
        auto microseconds_since_epoch =
            std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();

        return microseconds_since_epoch - start_time;
    }

    double timeSeconds() const override{ return timeMicros() / 1000000.0; }
    double timeMs() const override { return timeMicros() / 1000.0; }
    double elapsedMs() const override{ return elapsed/1000.0; }
    double elapsedMicros() const override { return elapsed; }
    double elapsedSeconds() const override { return elapsed/1000000.0; }
};
}

#endif // WINDOWBASE_H
