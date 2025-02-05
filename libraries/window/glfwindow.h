#ifndef GLFWINDOW_H
#define GLFWINDOW_H

#include "windowbase.h"
#include <map>

struct GLFWwindow;
struct GLFWcursor;

namespace gjh {

GLFWwindow* buildWindow(WindowEventSink* owner, int width, int height, std::string title, bool isOpenGL);

class CoreWindowGLFW : public CoreWindow {
private:
    bool        isOpenGL;
    GLFWwindow *window{};
    int         windowedX{0};
    int         windowedY{0};
    int         windowedWidth;
    int         windowedHeight;
    CoreWindowCursor activeCursor{CoreWindowCursor::arrow};
    std::map<CoreWindowCursor, GLFWcursor*> cursors;
public:
    CoreWindowGLFW(std::string title, int width, int height, bool isOpenGL);
   ~CoreWindowGLFW();

   // CoreWindow interface
protected:
   virtual void configure() override;

private:
   virtual bool shouldClose() const override;
   virtual void handleToggleFullScreen() override;
   virtual void pollEvents() override;

protected:
   GLFWwindow* getWindow() { return window; }

    // CoreWindow interface
public:
    void setCursor(CoreWindowCursor cursor) override;
};
}

#endif // GLFWINDOW_H
