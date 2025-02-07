#include "corewindowqt.h"
#include "QKeyEvent"
#include "QMouseEvent"
#include "QWheelEvent"
#include "QExposeEvent"
#include <QPainter>

CoreWindowQt::CoreWindowQt(std::function<void (CoreWindowQt *)> drawCallback)
    : mssm::CoreWindow("CoreWindowQt", 800, 600), drawCallback{drawCallback}
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

mssm::MouseButton toMssmMouseButton(Qt::MouseButton qtButton)
{
    switch (qtButton) {
    case Qt::LeftButton: return mssm::MouseButton::Left;
    case Qt::RightButton: return mssm::MouseButton::Right;
    case Qt::MiddleButton: return mssm::MouseButton::Middle;
    default: return mssm::MouseButton::None;
    }
}

mssm::ModKey toMssmModKey(Qt::KeyboardModifiers qtMods)
{
    int mods = 0;
    if (qtMods & Qt::ShiftModifier) {
        mods |= static_cast<int>(mssm::ModKey::Shift);
    }
    if (qtMods & Qt::ControlModifier) {
        mods |= static_cast<int>(mssm::ModKey::Ctrl);
    }
    if (qtMods & Qt::AltModifier) {
        mods |= static_cast<int>(mssm::ModKey::Alt);
    }
    return static_cast<mssm::ModKey>(mods);
}

mssm::Key toMssmKey(Qt::Key qtKey)
{
    switch (qtKey) {
    case Qt::Key_Escape: return mssm::Key::ESC;
    case Qt::Key_Tab: return mssm::Key::Tab;
    case Qt::Key_Backspace: return mssm::Key::Backspace;
    case Qt::Key_Return: return mssm::Key::ENTER;
    case Qt::Key_Enter: return mssm::Key::ENTER;
    case Qt::Key_Insert: return mssm::Key::Insert;
    case Qt::Key_Delete: return mssm::Key::Delete;
    case Qt::Key_Left: return mssm::Key::Left;
    case Qt::Key_Up: return mssm::Key::Up;
    case Qt::Key_Right: return mssm::Key::Right;
    case Qt::Key_Down: return mssm::Key::Down;
    case Qt::Key_PageUp: return mssm::Key::PageUp;
    case Qt::Key_PageDown: return mssm::Key::PageDown;
    case Qt::Key_Shift: return mssm::Key::LeftShift;
    case Qt::Key_Control: return mssm::Key::LeftCtrl;
    case Qt::Key_Alt: return mssm::Key::LeftAlt;
    case Qt::Key_A: return mssm::Key('A');
    case Qt::Key_B: return mssm::Key('B');
    case Qt::Key_C: return mssm::Key('C');
    case Qt::Key_D: return mssm::Key('D');
    case Qt::Key_E: return mssm::Key('E');
    case Qt::Key_F: return mssm::Key('F');
    case Qt::Key_G: return mssm::Key('G');
    case Qt::Key_H: return mssm::Key('H');
    case Qt::Key_I: return mssm::Key('I');
    case Qt::Key_J: return mssm::Key('J');
    case Qt::Key_K: return mssm::Key('K');
    case Qt::Key_L: return mssm::Key('L');
    case Qt::Key_M: return mssm::Key('M');
    case Qt::Key_N: return mssm::Key('N');
    case Qt::Key_O: return mssm::Key('O');
    case Qt::Key_P: return mssm::Key('P');
    case Qt::Key_Q: return mssm::Key('Q');
    case Qt::Key_R: return mssm::Key('R');
    case Qt::Key_S: return mssm::Key('S');
    case Qt::Key_T: return mssm::Key('T');
    case Qt::Key_U: return mssm::Key('U');
    case Qt::Key_V: return mssm::Key('V');
    case Qt::Key_W: return mssm::Key('W');
    case Qt::Key_X: return mssm::Key('X');
    case Qt::Key_Y: return mssm::Key('Y');
    case Qt::Key_Z: return mssm::Key('Z');
    case Qt::Key_0: return mssm::Key('0');
    case Qt::Key_1: return mssm::Key('1');
    case Qt::Key_2: return mssm::Key('2');
    case Qt::Key_3: return mssm::Key('3');
    case Qt::Key_4: return mssm::Key('4');
    case Qt::Key_5: return mssm::Key('5');
    case Qt::Key_6: return mssm::Key('6');
    case Qt::Key_7: return mssm::Key('7');
    case Qt::Key_8: return mssm::Key('8');
    case Qt::Key_9: return mssm::Key('9');
    case Qt::Key_Greater: return mssm::Key('>');
    case Qt::Key_Less: return mssm::Key('<');
    case Qt::Key_Comma: return mssm::Key(',');
    case Qt::Key_Period: return mssm::Key('.');
    case Qt::Key_Slash: return mssm::Key('/');
    case Qt::Key_Backslash: return mssm::Key('\\');
    case Qt::Key_Minus: return mssm::Key('-');
    case Qt::Key_Plus: return mssm::Key('+');
    case Qt::Key_Asterisk: return mssm::Key('*');
    case Qt::Key_AsciiCircum: return mssm::Key('^');
    case Qt::Key_Percent: return mssm::Key('%');
    case Qt::Key_Ampersand: return mssm::Key('&');
    case Qt::Key_Bar: return mssm::Key('|');
    case Qt::Key_Exclam: return mssm::Key('!');
    case Qt::Key_At: return mssm::Key('@');
    case Qt::Key_NumberSign: return mssm::Key('#');
    case Qt::Key_Dollar: return mssm::Key('$');
    case Qt::Key_ParenLeft: return mssm::Key('(');
    case Qt::Key_ParenRight: return mssm::Key(')');
    case Qt::Key_Equal: return mssm::Key('=');
    case Qt::Key_Colon: return mssm::Key(':');
    case Qt::Key_Semicolon: return mssm::Key(';');
    case Qt::Key_QuoteDbl: return mssm::Key('"');
    case Qt::Key_QuoteLeft: return mssm::Key('`');
    case Qt::Key_BraceLeft: return mssm::Key('{');
    case Qt::Key_BraceRight: return mssm::Key('}');
    case Qt::Key_BracketLeft: return mssm::Key('[');
    case Qt::Key_BracketRight: return mssm::Key(']');
    case Qt::Key_Apostrophe: return mssm::Key('\'');
    case Qt::Key_Underscore: return mssm::Key('_');
    case Qt::Key_hyphen: return mssm::Key('-');
    case Qt::Key_Space: return mssm::Key::Space;
    default: return mssm::Key::None;
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
            if (toMssmKey(combo.key()) == mssm::Key::None) {
                return false;
            }
            auto pos = QCursor::pos();
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::KeyPress, toMssmModKey(keyEvent->modifiers()), toMssmKey(combo.key()));
        }
        return true;
    case QEvent::KeyRelease:
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            auto combo = keyEvent->keyCombination();
            if (toMssmKey(combo.key()) == mssm::Key::None) {
                return false;
            }
            auto pos = QCursor::pos();
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::KeyRelease, toMssmModKey(keyEvent->modifiers()), toMssmKey(combo.key()));
        }
        return true;
    case QEvent::MouseButtonPress:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto pos = mouseEvent->pos();
            auto button = toMssmMouseButton(mouseEvent->button());
            if (button == mssm::MouseButton::None) {
                return false;
            }
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::MousePress, toMssmModKey(mouseEvent->modifiers()), button);
        }
        return true;
    case QEvent::MouseButtonRelease:
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            auto pos = mouseEvent->pos();
            auto button = toMssmMouseButton(mouseEvent->button());
            if (button == mssm::MouseButton::None) {
                return false;
            }
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseRelease, toMssmModKey(mouseEvent->modifiers()), button);
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
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseMove, toMssmModKey(mouseEvent->modifiers()), 0);
        }
        return true;
    case QEvent::Wheel:
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            auto pos = wheelEvent->angleDelta();
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseWheel, toMssmModKey(wheelEvent->modifiers()), pos.y());
        }
        return true;
    }
    return QWindow::event(event);
}

void CoreWindowQt::setCursor(mssm::CoreWindowCursor cursor)
{
    throw std::logic_error("not implemented");
}


CoreWindowQtWidget::CoreWindowQtWidget(std::function<void (CoreWindowQtWidget *)> drawCallback)
    : mssm::CoreWindow("CoreWindowQtWidget", 800, 600), drawCallback{drawCallback}
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
        // if (mssm::CoreWindow::update(false)) {
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
        if (toMssmKey(combo.key()) == mssm::Key::None) {
            return false;
        }
        auto pos = QCursor::pos();
        this->postEvent(pos.x(), pos.y(), mssm::EvtType::KeyPress, toMssmModKey(keyEvent->modifiers()), toMssmKey(combo.key()));
    }
    break;
    case QEvent::KeyRelease:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        auto combo = keyEvent->keyCombination();
        if (toMssmKey(combo.key()) == mssm::Key::None) {
            return false;
        }
        auto pos = QCursor::pos();
        this->postEvent(pos.x(), pos.y(), mssm::EvtType::KeyRelease, toMssmModKey(keyEvent->modifiers()), toMssmKey(combo.key()));
    }
    break;
    case QEvent::MouseButtonPress:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        auto pos = mouseEvent->pos();
        auto button = toMssmMouseButton(mouseEvent->button());
        if (button == mssm::MouseButton::None) {
            return false;
        }
        this->postEvent(pos.x(), pos.y(), mssm::EvtType::MousePress, toMssmModKey(mouseEvent->modifiers()), button);
    }
    break;
    case QEvent::MouseButtonRelease:
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        auto pos = mouseEvent->pos();
        auto button = toMssmMouseButton(mouseEvent->button());
        if (button == mssm::MouseButton::None) {
            return false;
        }
        this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseRelease, toMssmModKey(mouseEvent->modifiers()), button);
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
            this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseMove, toMssmModKey(mouseEvent->modifiers()), 0);
        }
    break;
    case QEvent::Wheel:
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        auto pos = wheelEvent->angleDelta();
        this->postEvent(pos.x(), pos.y(), mssm::EvtType::MouseWheel, toMssmModKey(wheelEvent->modifiers()), pos.y());
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

    if (mssm::CoreWindow::update(true)) {
        //drawCallback(this);
        std::cout << "Callback: ";
       // QWidget::update(); // render continuously
    }

    std::cout << " Bye" << std::endl;

   // endDrawing();
}
