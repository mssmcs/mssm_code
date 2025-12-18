#include "windowbase.h"
#include <limits>

using namespace std;

using namespace mssm;

CoreWindow::CoreWindow(string title, int width, int height)
    : title{title}
{
    currentWidth = width;
    currentHeight = height;
}

CoreWindow::~CoreWindow()
{
    std::cerr << "Destroying core window" << std::endl;
}


void CoreWindow::setGotResize(int width, int height)
{
    gotResizeEvent = true;
    currentWidth = width;
    currentHeight = height;
}

bool CoreWindow::update(bool autoEndDrawing)
{
    if (!configured) {
        configure();
        lateConfigure();
        start_time =
            std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();
        lastDrawTime = std::chrono::steady_clock::now();
    }
    else if (isDrawing && !closed && autoEndDrawing) {
        // submit previous drawing commands
        endDrawing(shouldClose());
        isDrawing = false;
    }

    if (closed || shouldClose()) {
        close();
        return false;
    }

    if (requestToggleFullScreen) {
        requestToggleFullScreen = false;
        handleToggleFullScreen();
    }

    updateInputs();

    pollEvents();

    bool wasResized = gotResizeEvent;

    if (gotResizeEvent) {
        gotResizeEvent = false;
        postEvent(currentWidth, currentHeight, EvtType::WindowResize, ModKey::None, 0);
    }

    isDrawing = beginDrawing(wasResized);

    auto currDrawTime = std::chrono::steady_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::microseconds>(currDrawTime - lastDrawTime).count();
    lastDrawTime = currDrawTime;

    return true;
}

void CoreWindow::toggleFullScreen()
{
    requestToggleFullScreen = true;
}

//#include <utility>

// TODO: think about this.  if valuable, move to utility
template <typename T>
typename std::vector<T>::iterator append(std::vector<T>&& src, std::vector<T>& dest)
{
    typename std::vector<T>::iterator result;

    if (dest.empty()) {

        dest = std::move(src);
        result = std::begin(dest);
    } else {
        result = dest.insert(std::end(dest),
                             std::make_move_iterator(std::begin(src)),
                             std::make_move_iterator(std::end(src)));
    }

    src.clear();
    src.shrink_to_fit();

    return result;
}

const std::vector<Event>& EventManager::events() const
{
    return _events;
}

void CoreWindow::configure()
{
    configured = true;


}

void CoreWindow::lateConfigure()
{

}



