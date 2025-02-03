#include "objcanvas.h"

using namespace mssm;
using namespace std;

constexpr const char* notImplMsg = "Function not yet implemented";


ObjCanvas::ObjCanvas(string filename, int width, int height)
    : w{width}, h{height}
{

}

int mssm::ObjCanvas::width()
{
    return w;
}

int mssm::ObjCanvas::height()
{
    return h;
}

void mssm::ObjCanvas::setBackground(Color c)
{
}

void mssm::ObjCanvas::line(Vec2d p1, Vec2d p2, Color c)
{
}

void mssm::ObjCanvas::ellipse(Vec2d center, double w, double h, Color c, Color f)
{
}

void mssm::ObjCanvas::arc(Vec2d center, double w, double h, double a, double alen, Color c)
{
}

void mssm::ObjCanvas::chord(Vec2d center, double w, double h, double a, double alen, Color c, Color f)
{
}

void mssm::ObjCanvas::pie(Vec2d center, double w, double h, double a, double alen, Color c, Color f)
{
}

void mssm::ObjCanvas::rect(Vec2d corner, double w, double h, Color c, Color f)
{
}

void mssm::ObjCanvas::polygon(const std::vector<Vec2d> &points, Color border, Color fill)
{
}

void mssm::ObjCanvas::polyline(const std::vector<Vec2d> &points, Color color)
{
}

void mssm::ObjCanvas::points(const std::vector<Vec2d> &points, Color c)
{
}

#ifdef SUPPORT_MSSM_ARRAY
void mssm::ObjCanvas::polygon(const Array<Vec2d> &pts, Color border, Color fill)
{
    polygon(pts.asVector(), border, fill);
}

void mssm::ObjCanvas::polyline(const Array<Vec2d> &pts, Color color)
{
    polyline(pts.asVector(), color);
}

void mssm::ObjCanvas::points(const Array<Vec2d> &pts, Color c)
{
    points(pts.asVector(), c);
}
#endif

void mssm::ObjCanvas::polygon(std::initializer_list<Vec2d> pts, Color border, Color fill)
{
    polygon(vector<Vec2d>(pts), border, fill);
}

void mssm::ObjCanvas::polyline(std::initializer_list<Vec2d> pts, Color color)
{
    polyline(vector<Vec2d>(pts), color);
}

void mssm::ObjCanvas::points(std::initializer_list<Vec2d> pts, Color c)
{
    points(vector<Vec2d>(pts), c);
}

void mssm::ObjCanvas::text(Vec2d pos, const FontInfo &sizeAndFace, const string &str, Color textColor, HAlign hAlign, VAlign vAlign)
{
}

void mssm::ObjCanvas::textExtents(const FontInfo &sizeAndFace, const string &str, TextExtents &extents)
{
}

double mssm::ObjCanvas::textWidth(const FontInfo &sizeAndFace, const string &str)
{
    throw std::logic_error(notImplMsg);
}

void mssm::ObjCanvas::point(Vec2d pos, Color c)
{
}

void mssm::ObjCanvas::image(Vec2d pos, const Image &img)
{
}

void mssm::ObjCanvas::image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::ObjCanvas::image(Vec2d pos, double w, double h, const Image &img)
{
}

void mssm::ObjCanvas::image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::ObjCanvas::imageC(Vec2d center, double angle, const Image &img)
{
}

void mssm::ObjCanvas::imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::ObjCanvas::imageC(Vec2d center, double angle, double w, double h, const Image &img)
{
}

void mssm::ObjCanvas::imageC(Vec2d center, double angle, double w, double h, const Image &img, Vec2d src, int srcw, int srch)
{
}
