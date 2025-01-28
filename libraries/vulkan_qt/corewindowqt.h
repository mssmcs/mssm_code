#ifndef COREWINDOWQT_H
#define COREWINDOWQT_H

#include <QWindow>
#include <QVulkanWindow>
#include <QWidget>

#include "corewindow.h"

class CoreWindowQt : public QVulkanWindow, public gjh::CoreWindow
{
protected:
    bool closeRequested = false;
    bool detached = false;
    std::function<void(CoreWindowQt*)> drawCallback;
public:
    CoreWindowQt(std::function<void(CoreWindowQt*)> drawCallback);
    void setCallback(std::function<void(CoreWindowQt*)> drawCallback) { this->drawCallback = drawCallback; }
    // CoreWindow interface
protected:
    bool shouldClose() const override;
    void handleToggleFullScreen() override;
    void pollEvents() override;
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;

    bool event(QEvent *event) override;
    void setCursor(gjh::CoreWindowCursor cursor) override;
};

class CoreWindowQtWidget : public QWidget, public gjh::CoreWindow
{
    Q_OBJECT

protected:
    bool closeRequested = false;
    bool detached = false;
    std::function<void(CoreWindowQtWidget*)> drawCallback;
public:
    CoreWindowQtWidget(std::function<void(CoreWindowQtWidget*)> drawCallback);
    void setCallback(std::function<void(CoreWindowQtWidget*)> drawCallback) { this->drawCallback = drawCallback; }
    // CoreWindow interface
protected:
    bool shouldClose() const override;
    void handleToggleFullScreen() override;
    void pollEvents() override;
    void beginDrawing(bool wasResized) override;
    void endDrawing() override;

    bool event(QEvent *event) override;

    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
};


#endif // COREWINDOWQT_H
