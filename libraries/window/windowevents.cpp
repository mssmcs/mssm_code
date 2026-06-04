#include "windowevents.h"

#include <iostream>

namespace mssm {

std::ostream& operator<<(std::ostream& os, const MouseButton& k)
{
    switch (k) {
    case MouseButton::None:   os << "MouseButton::None"; break;
    case MouseButton::Left:   os << "MouseButton::Left"; break;
    case MouseButton::Right:  os << "MouseButton::Right"; break;
    case MouseButton::Middle: os << "MouseButton::Middle"; break;
    default:                  os << "MouseButton::" << (static_cast<int>(k) + 1); break;
    }
    return os;
}

} // namespace mssm
