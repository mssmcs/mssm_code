#include "corewindowqt.h"
#include "QKeyEvent"
#include "QMouseEvent"
#include "QWheelEvent"
#include "QExposeEvent"
#include <QPainter>

CoreWindowQt::CoreWindowQt(std::function<void (CoreWindowQt *)> drawCallback)
    : gjh::CoreWindow("CoreWindowQt", 800, 600), drawCallback{drawCallback}
{
    this->setSurfaceType(QSurface::VulkanSurface);
}

bool CoreWindowQt::shouldClose() const
{
    return closeRequested;
}

void CoreWindowQt::handleToggleFullScreen()
{

}

void CoreWindowQt::pollEvents()
{

}

void CoreWindowQt::beginDrawing(bool wasResized)
{

}

void CoreWindowQt::endDrawing()
{

}

gjh::MouseButton toGjhMouseButton(Qt::MouseButton qtButton)
{
    switch (qtButton) {
    case Qt::LeftButton: return gjh::MouseButton::Left;
    case Qt::RightButton: return gjh::MouseButton::Right;
    case Qt::MiddleButton: return gjh::MouseButton::Middle;
    default: return gjh::MouseButton::None;
    }
}

gjh::ModKey toGjhModKey(Qt::KeyboardModifiers qtMods)
{
    int mods = 0;
    if (qtMods & Qt::ShiftModifier) {
        mods |= static_cast<int>(gjh::ModKey::Shift);
    }
    if (qtMods & Qt::ControlModifier) {
        mods |= static_cast<int>(gjh::ModKey::Ctrl);
    }
    if (qtMods & Qt::AltModifier) {
        mods |= static_cast<int>(gjh::ModKey::Alt);
    }
    return static_cast<gjh::ModKey>(mods);
}

gjh::Key toGjhKey(Qt::Key qtKey)
{
    switch (qtKey) {
    case Qt::Key_Escape: return gjh::Key::ESC;
    case Qt::Key_Tab: return gjh::Key::Tab;
    case Qt::Key_Backspace: return gjh::Key::Backspace;
    case Qt::Key_Return: return gjh::Key::ENTER;
    case Qt::Key_Enter: return gjh::Key::ENTER;
    case Qt::Key_Insert: return gjh::Key::Insert;
    case Qt::Key_Delete: return gjh::Key::Delete;
    case Qt::Key_Left: return gjh::Key::Left;
    case Qt::Key_Up: return gjh::Key::Up;
    case Qt::Key_Right: return gjh::Key::Right;
    case Qt::Key_Down: return gjh::Key::Down;
    case Qt::Key_PageUp: return gjh::Key::PageUp;
    case Qt::Key_PageDown: return gjh::Key::PageDown;
    case Qt::Key_Shift: return gjh::Key::LeftShift;
    case Qt::Key_Control: return gjh::Key::LeftCtrl;
    case Qt::Key_Alt: return gjh::Key::LeftAlt;
    case Qt::Key_A: return gjh::Key('A');
    case Qt::Key_B: return gjh::Key('B');
    case Qt::Key_C: return gjh::Key('C');
    case Qt::Key_D: return gjh::Key('D');
    case Qt::Key_E: return gjh::Key('E');
    case Qt::Key_F: return gjh::Key('F');
    case Qt::Key_G: return gjh::Key('G');
    case Qt::Key_H: return gjh::Key('H');
    case Qt::Key_I: return gjh::Key('I');
    case Qt::Key_J: return gjh::Key('J');
    case Qt::Key_K: return gjh::Key('K');
    case Qt::Key_L: return gjh::Key('L');
    case Qt::Key_M: return gjh::Key('M');
    case Qt::Key_N: return gjh::Key('N');
    case Qt::Key_O: return gjh::Key('O');
    case Qt::Key_P: return gjh::Key('P');
    case Qt::Key_Q: return gjh::Key('Q');
    case Qt::Key_R: return gjh::Key('R');
    case Qt::Key_S: return gjh::Key('S');
    case Qt::Key_T: return gjh::Key('T');
    case Qt::Key_U: return gjh::Key('U');
    case Qt::Key_V: return gjh::Key('V');
    case Qt::Key_W: return gjh::Key('W');
    case Qt::Key_X: return gjh::Key('X');
    case Qt::Key_Y: return gjh::Key('Y');
    case Qt::Key_Z: return gjh::Key('Z');
    case Qt::Key_0: return gjh::Key('0');
    case Qt::Key_1: return gjh::Key('1');
    case Qt::Key_2: return gjh::Key('2');
    case Qt::Key_3: return gjh::Key('3');
    case Qt::Key_4: return gjh::Key('4');
    case Qt::Key_5: return gjh::Key('5');
    case Qt::Key_6: return gjh::Key('6');
    case Qt::Key_7: return gjh::Key('7');
    case Qt::Key_8: return gjh::Key('8');
    case Qt::Key_9: return gjh::Key('9');
    case Qt::Key_Greater: return gjh::Key('>');
    case Qt::Key_Less: return gjh::Key('<');
    case Qt::Key_Comma: return gjh::Key(',');
    case Qt::Key_Period: return gjh::Key('.');
    case Qt::Key_Slash: return gjh::Key('/');
    case Qt::Key_Backslash: return gjh::Key('\\');
    case Qt::Key_Minus: return gjh::Key('-');
    case Qt::Key_Plus: return gjh::Key('+');
    case Qt::Key_Asterisk: return gjh::Key('*');
    case Qt::Key_AsciiCircum: return gjh::Key('^');
    case Qt::Key_Percent: return gjh::Key('%');
    case Qt::Key_Ampersand: return gjh::Key('&');
    case Qt::Key_Bar: return gjh::Key('|');
    case Qt::Key_Exclam: return gjh::Key('!');
    case Qt::Key_At: return gjh::Key('@');
    case Qt::Key_NumberSign: return gjh::Key('#');
    case Qt::Key_Dollar: return gjh::Key('$');
    case Qt::Key_ParenLeft: return gjh::Key('(');
    case Qt::Key_ParenRight: return gjh::Key(')');
    case Qt::Key_Equal: return gjh::Key('=');
    case Qt::Key_Colon: return gjh::Key(':');
    case Qt::Key_Semicolon: return gjh::Key(';');
    case Qt::Key_QuoteDbl: return gjh::Key('"');
    case Qt::Key_QuoteLeft: return gjh::Key('`');
    case Qt::Key_BraceLeft: return gjh::Key('{');
    case Qt::Key_BraceRight: return gjh::Key('}');
    case Qt::Key_BracketLeft: return gjh::Key('[');
    case Qt::Key_BracketRight: return gjh::Key(']');
    case Qt::Key_Apostrophe: return gjh::Key('\'');
    case Qt::Key_Underscore: return gjh::Key('_');
    case Qt::Key_hyphen: return gjh::Key('-');
    case Qt::Key_Space: return gjh::Key::Space;
    default: return gjh::Key::None;
    }
}

bool CoreWindowQt::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::PlatformSurface:
        if (static_cast<QPlatformSurfaceEvent *>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
            // cleanup!
            std::cout << "Need to cleanup!!!" << std::endl;
            detachRenderer(true);
            detached = true;
        }
        break;
    case QEvent::UpdateRequest:
        if (detached) {
            return true;
        }
        if (update(true)) {
            drawCallback(this);
            requestUpdate(); // render continuously
        }
        return true;
    case QEvent::Close:
        if (detached) {
            return true;
        }
        // The window system is closing the window; accept the close request.
        closeRequested = true;
        return QWindow::event(event);
    case QEvent::Resize:
        {
            QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
            this->setGotResize(resizeEvent->size().width(), resizeEvent->size().height());
        }
        return QWindow::event(event);
    case QEvent::Expose:
        // Handle expose event
        requestUpdate();
        return QWindow::event(event);
    case QEvent::KeyPress:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            auto combo = keyEvent->keyCombination();
            if (toGjhKey(combo.key()) == gjh::Key::None) {
                return false;
            }
            auto pos = QCursor::pos();
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::KeyPress, toGjhModKey(keyEvent->modifiers()), toGjhKey(combo.key()));
        }
        return true;
    case QEvent::KeyRelease:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            auto combo = keyEvent->keyCombination();
            if (toGjhKey(combo.key()) == gjh::Key::None) {
                return false;
            }
            auto pos = QCursor::pos();
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::KeyRelease, toGjhModKey(keyEvent->modifiers()), toGjhKey(combo.key()));
        }
        return true;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto pos = mouseEvent->pos();
            auto button = toGjhMouseButton(mouseEvent->button());
            if (button == gjh::MouseButton::None) {
                return false;
            }
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::MousePress, toGjhModKey(mouseEvent->modifiers()), button);
        }
        return true;
    case QEvent::MouseButtonRelease:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto pos = mouseEvent->pos();
            auto button = toGjhMouseButton(mouseEvent->button());
            if (button == gjh::MouseButton::None) {
                return false;
            }
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseRelease, toGjhModKey(mouseEvent->modifiers()), button);
        }
        return true;
    case QEvent::MouseMove:
        // Handle mouse move event
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto buttons = mouseEvent->buttons();
            if (buttons == Qt::NoButton) {
                return false;
            }
            auto pos = mouseEvent->pos();
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseMove, toGjhModKey(mouseEvent->modifiers()), 0);
        }
        return true;
    case QEvent::Wheel:
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            auto pos = wheelEvent->angleDelta();
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseWheel, toGjhModKey(wheelEvent->modifiers()), pos.y());
        }
        return true;
    }
    return QWindow::event(event);
}

void CoreWindowQt::setCursor(gjh::CoreWindowCursor cursor)
{
    throw std::logic_error("not implemented");
}


CoreWindowQtWidget::CoreWindowQtWidget(std::function<void (CoreWindowQtWidget *)> drawCallback)
    : gjh::CoreWindow("CoreWindowQtWidget", 800, 600), drawCallback{drawCallback}
{
    //this->setSurfaceType(QSurface::VulkanSurface);
}

bool CoreWindowQtWidget::shouldClose() const
{
    return closeRequested;
}

void CoreWindowQtWidget::handleToggleFullScreen()
{

}

void CoreWindowQtWidget::pollEvents()
{

}

void CoreWindowQtWidget::beginDrawing(bool wasResized)
{

}

void CoreWindowQtWidget::endDrawing()
{

}

bool CoreWindowQtWidget::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::PlatformSurface:
        if (static_cast<QPlatformSurfaceEvent *>(event)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed) {
            // cleanup!
            std::cout << "Need to cleanup!!!" << std::endl;
            detachRenderer(true);
            detached = true;
        }
        break;
    case QEvent::UpdateRequest:
        // if (detached) {
        //     return true;
        // }
        // if (gjh::CoreWindow::update(false)) {
        //     drawCallback(this);
        //     QWidget::update(); // render continuously
        // }
        break;
    case QEvent::Close:
        if (detached) {
            return true;
        }
        // The window system is closing the window; accept the close request.
        closeRequested = true;
        break;
    case QEvent::Resize:
    {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        this->setGotResize(resizeEvent->size().width(), resizeEvent->size().height());
    }
        return QWidget::event(event);
    case QEvent::Expose:
        // Handle expose event
        // QWidget::update();
        return QWidget::event(event);
    case QEvent::KeyPress:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        auto combo = keyEvent->keyCombination();
        if (toGjhKey(combo.key()) == gjh::Key::None) {
            return false;
        }
        auto pos = QCursor::pos();
        this->postEvent(pos.x(), pos.y(), gjh::EvtType::KeyPress, toGjhModKey(keyEvent->modifiers()), toGjhKey(combo.key()));
    }
    break;
    case QEvent::KeyRelease:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        auto combo = keyEvent->keyCombination();
        if (toGjhKey(combo.key()) == gjh::Key::None) {
            return false;
        }
        auto pos = QCursor::pos();
        this->postEvent(pos.x(), pos.y(), gjh::EvtType::KeyRelease, toGjhModKey(keyEvent->modifiers()), toGjhKey(combo.key()));
    }
    break;
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        auto pos = mouseEvent->pos();
        auto button = toGjhMouseButton(mouseEvent->button());
        if (button == gjh::MouseButton::None) {
            return false;
        }
        this->postEvent(pos.x(), pos.y(), gjh::EvtType::MousePress, toGjhModKey(mouseEvent->modifiers()), button);
    }
    break;
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        auto pos = mouseEvent->pos();
        auto button = toGjhMouseButton(mouseEvent->button());
        if (button == gjh::MouseButton::None) {
            return false;
        }
        this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseRelease, toGjhModKey(mouseEvent->modifiers()), button);
    }
    break;
    case QEvent::MouseMove:
        // Handle mouse move event
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto buttons = mouseEvent->buttons();
            if (buttons == Qt::NoButton) {
                return false;
            }
            auto pos = mouseEvent->pos();
            this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseMove, toGjhModKey(mouseEvent->modifiers()), 0);
        }
    break;
    case QEvent::Wheel:
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        auto pos = wheelEvent->angleDelta();
        this->postEvent(pos.x(), pos.y(), gjh::EvtType::MouseWheel, toGjhModKey(wheelEvent->modifiers()), pos.y());
    }
    break;
    }


    return QWidget::event(event);
}

void CoreWindowQtWidget::resizeEvent(QResizeEvent *event)
{

}

void CoreWindowQtWidget::paintEvent(QPaintEvent *event)
{
    static int drawCount = 0;

    std::cout << "PaintEvent: " << drawCount++ << " ";
    // {
    //     QPainter painter(this);
    //     painter.setRenderHint(QPainter::Antialiasing);
    //     painter.fillRect(rect(), QColor(255, 0, 0)); // Clear background to black (or any color)
    // }

    if (detached) {
        return;
    }

    if (gjh::CoreWindow::update(true)) {
        //drawCallback(this);
        std::cout << "Callback: ";
       // QWidget::update(); // render continuously
    }

    std::cout << " Bye" << std::endl;

   // endDrawing();
}
