#include "canvasExtent.h"
#include "image.h"

#include <algorithm>
#include <cmath>
#include <limits>

void ExtentWrapper::resetExtents() {
    minExtent.x = std::numeric_limits<double>::max();
    minExtent.y = std::numeric_limits<double>::max();
    maxExtent.x = std::numeric_limits<double>::lowest();
    maxExtent.y = std::numeric_limits<double>::lowest();
}

void ExtentWrapper::extend(Vec2d p) {
    minExtent.x = std::min(minExtent.x, p.x);
    minExtent.y = std::min(minExtent.y, p.y);
    maxExtent.x = std::max(maxExtent.x, p.x);
    maxExtent.y = std::max(maxExtent.y, p.y);
}

// Canvas2d interface implementation - delegates to underlying canvas while tracking extents
bool ExtentWrapper::isDrawable() {
    return g.isDrawable();
}

int ExtentWrapper::width() {
    return g.width();
}

int ExtentWrapper::height() {
    return g.height();
}

void ExtentWrapper::setBackground(mssm::Color c) {
    g.setBackground(c);
}

void ExtentWrapper::line(Vec2d p1, Vec2d p2, mssm::Color c) {
    extend(p1);
    extend(p2);
    g.line(p1, p2, c);
}

void ExtentWrapper::ellipse(Vec2d center, double w, double h, mssm::Color c, mssm::Color f) {
    extend(Vec2d(center.x - w/2, center.y - h/2));
    extend(Vec2d(center.x + w/2, center.y + h/2));
    g.ellipse(center, w, h, c, f);
}

void ExtentWrapper::arc(Vec2d center, double w, double h, double a, double alen, mssm::Color c) {
    // For simplicity, we use the entire ellipse's bounding box
    extend(Vec2d(center.x - w/2, center.y - h/2));
    extend(Vec2d(center.x + w/2, center.y + h/2));
    g.arc(center, w, h, a, alen, c);
}

void ExtentWrapper::chord(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // For simplicity, we use the entire ellipse's bounding box
    extend(Vec2d(center.x - w/2, center.y - h/2));
    extend(Vec2d(center.x + w/2, center.y + h/2));
    g.chord(center, w, h, a, alen, c, f);
}

void ExtentWrapper::pie(Vec2d center, double w, double h, double a, double alen, mssm::Color c, mssm::Color f) {
    // For simplicity, we use the entire ellipse's bounding box
    extend(Vec2d(center.x - w/2, center.y - h/2));
    extend(Vec2d(center.x + w/2, center.y + h/2));
    g.pie(center, w, h, a, alen, c, f);
}

void ExtentWrapper::rect(Vec2d corner, double w, double h, mssm::Color c, mssm::Color f) {
    extend(corner);
    extend(Vec2d(corner.x + w, corner.y + h));
    g.rect(corner, w, h, c, f);
}

void ExtentWrapper::polygon(const std::vector<Vec2d> &points, mssm::Color border, mssm::Color fill) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polygon(points, border, fill);
}

void ExtentWrapper::polyline(const std::vector<Vec2d> &points, mssm::Color color) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polyline(points, color);
}

void ExtentWrapper::points(const std::vector<Vec2d> &points, mssm::Color c) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.points(points, c);
}

void ExtentWrapper::polygon(std::initializer_list<Vec2d> points, mssm::Color border, mssm::Color fill) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polygon(points, border, fill);
}

void ExtentWrapper::polyline(std::initializer_list<Vec2d> points, mssm::Color color) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polyline(points, color);
}

void ExtentWrapper::points(std::initializer_list<Vec2d> points, mssm::Color c) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.points(points, c);
}

void ExtentWrapper::text(Vec2d pos, const FontInfo &sizeAndFace, const std::string &str,
                         mssm::Color textcolor, HAlign hAlign, VAlign vAlign) {
    extend(pos);

    // Get the text extent to properly track the size
    TextExtents extents;
    textExtents(sizeAndFace, str, extents);

    // Adjust the position based on alignment
    double textX = pos.x;
    double textY = pos.y;

    // Adjust for horizontal alignment
    if (hAlign == HAlign::center) {
        textX -= extents.textWidth / 2;
    } else if (hAlign == HAlign::right) {
        textX -= extents.textWidth;
    }

    // Adjust for vertical alignment
    if (vAlign == VAlign::top) {
        textY += extents.fontAscent;
    } else if (vAlign == VAlign::center) {
        textY += (extents.fontAscent - extents.fontDescent) / 2;
    } else if (vAlign == VAlign::bottom) {
        textY -= extents.fontDescent;
    }

    // Extend the bounding box to include the entire text
    extend(Vec2d(textX, textY - extents.fontAscent));
    extend(Vec2d(textX + extents.textWidth, textY + extents.fontDescent));

    g.text(pos, sizeAndFace, str, textcolor, hAlign, vAlign);
}

void ExtentWrapper::textExtents(const FontInfo &sizeAndFace, const std::string &str, TextExtents &extents) {
    g.textExtents(sizeAndFace, str, extents);
}

double ExtentWrapper::textWidth(const FontInfo &sizeAndFace, const std::string &str) {
    return g.textWidth(sizeAndFace, str);
}

std::vector<double> ExtentWrapper::getCharacterXOffsets(const FontInfo &sizeAndFace, double startX, const std::string &text) {
    return g.getCharacterXOffsets(sizeAndFace, startX, text);
}

void ExtentWrapper::point(Vec2d pos, mssm::Color c) {
    extend(pos);
    g.point(pos, c);
}

void ExtentWrapper::image(Vec2d pos, const mssm::Image &img, double alpha) {
    extend(pos);
    extend(Vec2d(pos.x + img.width(), pos.y + img.height()));
    g.image(pos, img, alpha);
}

void ExtentWrapper::image(Vec2d pos, const mssm::Image &img, Vec2d src, int srcw, int srch, double alpha) {
    extend(pos);
    extend(Vec2d(pos.x + srcw, pos.y + srch));
    g.image(pos, img, src, srcw, srch, alpha);
}

void ExtentWrapper::image(Vec2d pos, double w, double h, const mssm::Image &img, double alpha) {
    extend(pos);
    extend(Vec2d(pos.x + w, pos.y + h));
    g.image(pos, w, h, img, alpha);
}

void ExtentWrapper::image(Vec2d pos, double w, double h, const mssm::Image &img, Vec2d src, int srcw, int srch, double alpha) {
    extend(pos);
    extend(Vec2d(pos.x + w, pos.y + h));
    g.image(pos, w, h, img, src, srcw, srch, alpha);
}

void ExtentWrapper::imageC(Vec2d center, double angle, const mssm::Image &img, double alpha) {
    // For rotated images, we use the enclosing circle as an approximation
    double radius = std::sqrt(img.width() * img.width() + img.height() * img.height()) / 2;
    extend(Vec2d(center.x - radius, center.y - radius));
    extend(Vec2d(center.x + radius, center.y + radius));
    g.imageC(center, angle, img, alpha);
}

void ExtentWrapper::imageC(Vec2d center, double angle, const mssm::Image &img, Vec2d src, int srcw, int srch, double alpha) {
    // For rotated images, we use the enclosing circle as an approximation
    double radius = std::sqrt(srcw * srcw + srch * srch) / 2;
    extend(Vec2d(center.x - radius, center.y - radius));
    extend(Vec2d(center.x + radius, center.y + radius));
    g.imageC(center, angle, img, src, srcw, srch, alpha);
}

void ExtentWrapper::imageC(Vec2d center, double angle, double w, double h, const mssm::Image &img, double alpha) {
    // For rotated images, we use the enclosing circle as an approximation
    double radius = std::sqrt(w * w + h * h) / 2;
    extend(Vec2d(center.x - radius, center.y - radius));
    extend(Vec2d(center.x + radius, center.y + radius));
    g.imageC(center, angle, w, h, img, alpha);
}

void ExtentWrapper::imageC(Vec2d center, double angle, double w, double h, const mssm::Image &img, Vec2d src, int srcw, int srch, double alpha) {
    // For rotated images, we use the enclosing circle as an approximation
    double radius = std::sqrt(w * w + h * h) / 2;
    extend(Vec2d(center.x - radius, center.y - radius));
    extend(Vec2d(center.x + radius, center.y + radius));
    g.imageC(center, angle, w, h, img, src, srcw, srch, alpha);
}

bool ExtentWrapper::isClipped(Vec2d pos) const {
    return g.isClipped(pos);
}

void ExtentWrapper::pushClip(int x, int y, int w, int h, bool replace) {
    g.pushClip(x, y, w, h, replace);
}

void ExtentWrapper::popClip() {
    g.popClip();
}

void ExtentWrapper::setClip(int x, int y, int w, int h) {
    g.setClip(x, y, w, h);
}

void ExtentWrapper::resetClip() {
    g.resetClip();
}

void ExtentWrapper::setViewport(int x, int y, int w, int h) {
    g.setViewport(x, y, w, h);
}

void ExtentWrapper::resetViewport() {
    g.resetViewport();
}

void ExtentWrapper::pushGroup(std::string groupName) {
    g.pushGroup(groupName);
}

void ExtentWrapper::popGroup() {
    g.popGroup();
}

void ExtentWrapper::polygonPattern(const std::vector<Vec2d> &points, mssm::Color c, mssm::Color f) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polygonPattern(points, c, f);
}

void ExtentWrapper::polygonPattern(std::initializer_list<Vec2d> points, mssm::Color c, mssm::Color f) {
    for (const auto& pt : points) {
        extend(pt);
    }
    g.polygonPattern(points, c, f);
}
