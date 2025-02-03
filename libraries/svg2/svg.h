#ifndef SVG_H
#define SVG_H

#include <string>

#include "svgstructelement.h"

class SvgElement;

class SvgAttr
{
public:
    std::string xmlns_dc;
    std::string xmlns_cc;
    std::string xmlns_rdf;
    std::string xmlns_svg;
    std::string xmlns;
    std::string width;
    std::string height;
    std::string viewBox;
    std::string id;
    std::string version;
public:
    SvgAttr();
};

class Svg : SvgStructElement
{
public:
    SvgAttr headerInfo;
public:
    Svg(const std::string& filename);
    void init();
    void save(const std::string& filename);
    void load(const std::string& filename);
};

#endif // SVG_H
