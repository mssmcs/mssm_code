#include "windowevents.h"

#include <gtest/gtest.h>

using namespace mssm;

TEST(EventMouseMask, LeftButtonDown)
{
    Event e;
    e.evtType = EvtType::MouseMove;
    e.arg = 1;
    EXPECT_TRUE(e.hasMouseButtonDown(MouseButton::Left));
    EXPECT_FALSE(e.hasMouseButtonDown(MouseButton::Right));
    EXPECT_EQ(e.primaryMouseButtonDown(), MouseButton::Left);
}

TEST(EventMouseMask, RightButtonDown)
{
    Event e;
    e.evtType = EvtType::MouseMove;
    e.arg = 2;
    EXPECT_TRUE(e.hasMouseButtonDown(MouseButton::Right));
    EXPECT_EQ(e.primaryMouseButtonDown(), MouseButton::Right);
}

TEST(EventMouseMask, NotMouseMove)
{
    Event e;
    e.evtType = EvtType::MousePress;
    e.arg = 1;
    EXPECT_FALSE(e.hasMouseButtonDown(MouseButton::Left));
    EXPECT_FALSE(e.anyMouseButtonDown());
}
