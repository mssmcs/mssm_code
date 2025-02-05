#include "rectdraw.h"


constexpr Vec2d ptOnCircle(Vec2d center, double radius, double angle)
{
    return center + Vec2d{radius*cos(angle), -radius*sin(angle)};
}

void appendArcPts(std::vector<Vec2d>& pts, Vec2d center, double radius, double startAngle, double endAngle, int numPts)
{
    pts.push_back(ptOnCircle(center, radius, startAngle));
    double increment = (endAngle - startAngle)/(numPts-1);
    for (int i = 1; i < numPts-1; i++) {
        startAngle += increment;
        pts.push_back(ptOnCircle(center, radius, startAngle));
    }
    pts.push_back(ptOnCircle(center, radius, endAngle));
}

std::vector<Vec2d> arcPts(Vec2d center, double radius, double startAngle, double endAngle, int numPts)
{
    std::vector<Vec2d> pts;
    for (int i = 0; i < numPts; i++) {
        double angle = startAngle + (endAngle - startAngle)*i/(numPts-1);
        pts.push_back(center + Vec2d{radius*cos(angle), radius*sin(angle)});
    }
    return pts;
}
