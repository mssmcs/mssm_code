#define _USE_MATH_DEFINES

#include "graphics.h"
#include "svg.h"

#include <sstream>
#include <algorithm>

using namespace std;
using namespace mssm;



void graphicsMain(Graphics& g)
{
    Svg svg("testing.svg");

    svg.save("output.svg");

//    doc.root().addChild()

    while (g.draw())
    {
        g.clear();

        for (const Event& e : g.events())
        {
            switch (e.evtType)
            {
            case EvtType::MousePress:
                break;
            case EvtType::MouseRelease:
                break;
            case EvtType::MouseMove:
                break;
            case EvtType::KeyPress:
                break;
            case EvtType::KeyRelease:
                break;
            }
        }

        g.line(10,10, g.mousePos().x, g.mousePos().y);
    }
}

int main()
{
    Graphics g("Graphics App", graphicsMain);
}
