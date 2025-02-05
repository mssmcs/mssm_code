#ifndef ICONDRAWER_H
#define ICONDRAWER_H

#include "canvas2d.h"
#include "svg.h"

enum class IconId {
    checkboxEmpty,
    checkboxFilled,
    radioEmpty,
    radioFilled,
    toggleClosed,
    toggleOpen
};


class IconDB {
public:
    struct IconInfo {
        IconId id;
        std::string filename;
    };
private:
   std::vector<std::unique_ptr<Svg>> iconSvgs;
public:
   IconDB();
   Svg& getIcon(IconId id);
protected:
   void loadIcons(std::initializer_list<IconInfo> icons);
   void loadIcon(IconId id, std::string filename);
};

class IconDrawer //: public SvgRenderer
{
private:
    IconDB *iconDb;
    mssm::Canvas2d& canvas;
public:
    IconDrawer(mssm::Canvas2d& canvas);
    void drawIcon(IconId id, Vec2d position, mssm::Color stroke, mssm::Color fill);
    void polygon(const std::vector<Vec2d> &points, mssm::Color &stroke, mssm::Color &fill);
    void polyline(const std::vector<Vec2d> &points, mssm::Color &stroke);
};

#endif // ICONDRAWER_H
