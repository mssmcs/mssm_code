#include "canvas3d.h"


void mssm::CameraParams::Frame(Vec3d position, double radius)
{
    // compute camera position such that an object with given radius, centered at position, will be nicely "framed"
    // look dir retained
    Vec3d lookDir = ForwardDir();
    up = UpDir();
    target = position;
    double dist = radius / tan(fov/2);
    camera = target - lookDir * dist;
}

void mssm::CameraParams::Translate(Vec3d offset)
{
    Vec3d u = RightDir();
    Vec3d v = UpDir();
    Vec3d w = BackDir();

    Vec3d trans = u*offset.x + v*offset.y + w*offset.z;

    camera += trans;
    target += trans;
}

void mssm::CameraParams::Pan(Vec2d screenDisplacement, double screenWidth)
{
    double tDist = (target-camera).magnitude();
    double worldWidthAtTarget = 2.0*tDist*tan(fov/2);
    Vec2d wd = screenDisplacement * worldWidthAtTarget/screenWidth;
    Translate({wd.x, wd.y, 0});
}

void mssm::CameraParams::Zoom(double factor)
{
    Vec3d lookDir = ForwardDir();
    double dist = (target-camera).magnitude();
    dist *= factor;
    camera = target - lookDir * dist;
}

void mssm::CameraParams::Roll(double angle)
{
    Vec3d lookDir = ForwardDir();
    Vec3d u = UpDir();
    up = rotateAroundAxis(u, lookDir, angle);
}

void mssm::CameraParams::Pitch(double angle)
{
    double tDist = (target-camera).magnitude();
    Vec3d lookDir = ForwardDir();
    Vec3d axis = RightDir();
    Vec3d u = UpDir();
    lookDir = rotateAroundAxis(lookDir, axis, angle);
    up = rotateAroundAxis(u, axis, angle);
    target = camera + lookDir * tDist;
}

void mssm::CameraParams::Yaw(double angle)
{
    double tDist = (target-camera).magnitude();
    Vec3d lookDir = ForwardDir();
    Vec3d axis = UpDir();
    up = axis;
    lookDir = rotateAroundAxis(lookDir, axis, angle);
    target = camera + lookDir * tDist;
}

void mssm::CameraParams::OrbitHoriz(double angle)
{
    Vec3d v = camera - target;
    v = rotateAroundAxis(v, up, angle);
    camera = target + v;
}

void mssm::CameraParams::OrbitVert(double angle)
{
    Vec3d r = RightDir();
    Vec3d v = camera - target;
    v = rotateAroundAxis(v, r, angle);
    up = rotateAroundAxis(up, r, angle);
    camera = target + v;
}
