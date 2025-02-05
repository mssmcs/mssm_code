#include "icondrawer.h"
#include "paths.h"

using namespace std;
using namespace mssm;

IconDB staticIcondDb;

IconDrawer::IconDrawer(mssm::Canvas2d &canvas)
    : canvas{canvas}
{
    iconDb = &staticIcondDb;
}

IconDB::IconDB()
{
    loadIcon(IconId::checkboxEmpty, "Material\\svg\\check_box_outline_blank_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
    loadIcon(IconId::checkboxFilled, "Material\\svg\\check_box_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
    loadIcon(IconId::radioEmpty, "Material\\svg\\radio_button_unchecked_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
    loadIcon(IconId::radioFilled, "Material\\svg\\radio_button_checked_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
    loadIcon(IconId::toggleClosed, "Material\\svg\\expand_circle_right_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
    loadIcon(IconId::toggleOpen, "Material\\svg\\expand_circle_down_24dp_5F6368_FILL0_wght400_GRAD0_opsz24.svg");
}

void IconDB::loadIcons(std::initializer_list<IconInfo> icons)
{
    for (auto& icon : icons)
    {
        loadIcon(icon.id, icon.filename);
    }
}

void IconDB::loadIcon(IconId id, std::string filename)
{
    int idx = static_cast<int>(id);
    if (idx >= iconSvgs.size())
    {
        iconSvgs.resize(idx + 1);
    }
    iconSvgs[idx] = make_unique<Svg>(Paths::findAsset(filename));
}

Svg &IconDB::getIcon(IconId id)
{
    int idx = static_cast<int>(id);
    if (idx < iconSvgs.size())
    {
        return *iconSvgs[idx];
    }
    else
    {
        throw runtime_error("Icon not loaded");
    }
}

void IconDrawer::polygon(const vector<Vec2d> &points, Color &stroke, Color &fill)
{
    canvas.polygon(points, stroke, fill);
}

void IconDrawer::polyline(const vector<Vec2d> &points, Color &stroke)
{
    canvas.polyline(points, stroke);
}

void IconDrawer::drawIcon(IconId id, Vec2d position, mssm::Color stroke, mssm::Color fill)
{
    Svg& svg = iconDb->getIcon(id);
    // svg.draw(canvas, position, fill, stroke);
   // svg.draw(*this, position, fill, stroke);
}
