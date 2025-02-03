#include "svgcanvas.h"

using namespace mssm;

SVGCanvas::SVGCanvas(string filename, int width, int height)
    : w{width}, h{height}
{
    svgFile.open(filename);
    svgFile << "<svg version=\"1.1\"" << endl;
    svgFile << "width=\"" << width << "\" height=\"" << height << "\"" << endl;
    svgFile << "xmlns=\"http://www.w3.org/2000/svg\">" << endl;
    //    svgFile << "<svg viewBox=\"0 0 100 100\">" << endl;
}

mssm::SVGCanvas::~SVGCanvas()
{
    svgFile << "</svg>" << endl;
    svgFile.close();
}

int mssm::SVGCanvas::width()
{
    return w;
}

int mssm::SVGCanvas::height()
{
    return h;
}

void mssm::SVGCanvas::setBackground(Color c)
{
}


//  void WriteCircle(int cx, int cy, int r, string fill) {
//  svgFile << "<circle cx=\"" << cx << "\" cy=\"" << cy << "\" r=\"" << r << "\" fill=\"" << fill << "\" />" << endl;
//  }
//  void WriteRectangle(int x, int y, int width, int height, string fill) {
//  svgFile << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\"" << height << "\" fill=\"" << fill << "\" />" << endl;
//  }
//  void WriteLine(int x1, int y1, int x2, int y2, string stroke) {
//  svgFile << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 << "\" y2=\"" << y2 << "\" stroke=\"" << stroke << "\" />" << endl;
//  }
//  void WritePolyline(vector<pair<int, int>> points, string stroke) {
//  svgFile << "<polyline points=\"" << points.first << "," << points.second;
//  for (int i = 1; i < points.size(); i++) {
//  svgFile << " " << points[i].first << "," << points[i].second;
//  }
//  svgFile << "\" stroke=\"" << stroke << "\" />" << endl;
//  }
//  void WritePolygon(vector<pair<int, int>> points, string fill, string stroke) {
//  svgFile << "<polygon points=\"" << points.first << "," << points.second;
//  for (int i = 1; i < points.size(); i++) {
//  svgFile << " " << points[i].first << "," << points[i].second;
//  }
//  svgFile << "\" fill=\"" << fill << "\" stroke=\"" << stroke << "\" />" << endl;
//  }
//  void WriteText(int x, int y, string text, string font, int size, string fill) {
//  svgFile << "<text x=\"" << x << "\" y=\"" << y << "\" font=\"" << font << "\" font-size=\"" << size << "\" fill=\"" << fill << "\">" << text << "</text>" << endl;
//  }
//  void WriteEllipse(int cx, int cy, int rx, int ry, string fill, string stroke) {
//  svgFile << "<ellipse cx=\"" << cx << "\" cy=\"" << cy << "\" rx=\"" << rx << "\" ry=\"" << ry << "\" fill=\"" << fill << "\" stroke=\"" << stroke << "\" />" << endl;
//  }
//  void WriteArc(int x1, int y1, int x2, int y2, int rx, int ry, string stroke) {
//  svgFile << "<path d=\"M " << x1 << " " << y1 << " A " << rx << " " << ry << " 0 0 1 " << x2 << " " << y2 << "\" stroke=\"" << stroke << "\" />" << endl;


void mssm::SVGCanvas::line(Vec2d p1, Vec2d p2, Color c)
{
    svgFile << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"" << toString(c) << "\" />" << endl;
}

void mssm::SVGCanvas::ellipse(Vec2d center, double w, double h, Color c, Color f)
{
    svgFile << "<ellipse cx=\"" << center.x << "\" cy=\"" << center.y << "\" rx=\"" << (w/2) << "\" ry=\"" << (h/2) << "\" stroke=\"" << toString(c) << "\" fill=\"" << toString(f) << "\" />" << endl;
}

void mssm::SVGCanvas::arc(Vec2d center, double w, double h, double a, double alen, Color c)
{
}

void mssm::SVGCanvas::chord(Vec2d center, double w, double h, double a, double alen, Color c, Color f)
{
}

void mssm::SVGCanvas::pie(Vec2d center, double w, double h, double a, double alen, Color c, Color f)
{
}

void mssm::SVGCanvas::rect(Vec2d corner, double w, double h, Color c, Color f)
{
    svgFile << "<rect x=\"" << corner.x << "\" y=\"" << corner.y << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"" << toString(c) << "\" fill=\"" << toString(f) << "\" />" << endl;
}

void mssm::SVGCanvas::polygon(const std::vector<Vec2d> &points, Color border, Color fill)
{
    svgFile << "<polygon points=\"" << points[0].x << "," << points[0].y;
    for (int i = 1; i < points.size(); i++) {
        svgFile << " " << points[i].x << "," << points[i].y;
    }
    svgFile << "\" fill=\"" << toString(fill) << "\" stroke=\"" << toString(border) << "\" />" << endl;
}

void mssm::SVGCanvas::polyline(const std::vector<Vec2d> &points, Color color)
{
    svgFile << "<polyline points=\"" << points[0].x << "," << points[0].y;
    for (int i = 1; i < points.size(); i++) {
        svgFile << " " << points[i].x << "," << points[i].y;
    }
    svgFile << "\" stroke=\"" << toString(color) << "\" />" << endl;
}

void mssm::SVGCanvas::points(const std::vector<Vec2d> &points, Color c)
{
}

void mssm::SVGCanvas::polygon(const Array<Vec2d> &pts, Color border, Color fill)
{
    polygon(pts.asVector(), border, fill);
}

void mssm::SVGCanvas::polyline(const Array<Vec2d> &pts, Color color)
{
    polyline(pts.asVector(), color);
}

void mssm::SVGCanvas::points(const Array<Vec2d> &pts, Color c)
{
    points(pts.asVector(), c);
}

void mssm::SVGCanvas::polygon(std::initializer_list<Vec2d> pts, Color border, Color fill)
{
    polygon(vector<Vec2d>(pts), border, fill);
}

void mssm::SVGCanvas::polyline(std::initializer_list<Vec2d> pts, Color color)
{
    polyline(vector<Vec2d>(pts), color);
}

void mssm::SVGCanvas::points(std::initializer_list<Vec2d> pts, Color c)
{
    points(vector<Vec2d>(pts), c);
}

void mssm::SVGCanvas::text(Vec2d pos, const FontInfo &sizeAndFace, const string &str, Color textColor, HAlign hAlign, VAlign vAlign)
{
}

void mssm::SVGCanvas::textExtents(double size, const string &str, TextExtents &extents)
{
}

double mssm::SVGCanvas::textWidth(double size, const string &str)
{
    return 0;
}

void mssm::SVGCanvas::point(Vec2d pos, Color c)
{
}

void mssm::SVGCanvas::image(Vec2d pos, const Image &img)
{
}

void mssm::SVGCanvas::image(Vec2d pos, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::SVGCanvas::image(Vec2d pos, double w, double h, const Image &img)
{
}

void mssm::SVGCanvas::image(Vec2d pos, double w, double h, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::SVGCanvas::imageC(Vec2d center, double angle, const Image &img)
{
}

void mssm::SVGCanvas::imageC(Vec2d center, double angle, const Image &img, Vec2d src, int srcw, int srch)
{
}

void mssm::SVGCanvas::imageC(Vec2d center, double angle, double w, double h, const Image &img)
{
}

void mssm::SVGCanvas::imageC(Vec2d center, double angle, double w, double h, const Image &img, Vec2d src, int srcw, int srch)
{
}

string SVGCanvas::toString(Color color) const
{
    stringstream ss;
    ss << "rgba(" << int(color.r) << "," << int(color.g) << "," << int(color.b) << "," << (int(color.a) / 255.0) << ")";
    return ss.str();
}
