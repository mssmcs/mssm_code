#ifndef SVGSTRUCTELEMENT_H
#define SVGSTRUCTELEMENT_H

#include "svgelement.h"

class SvgStructElement;
class SvgPath;

using SvgStructElementPtr = std::shared_ptr<SvgStructElement>;
using SvgPathPtr = std::shared_ptr<SvgPath>;

class SvgStructElement : public SvgElement
{
protected:
    std::vector<SvgElementPtr> elements;
public:
    SvgStructElement();
    std::vector<SvgElementPtr> children();
    SvgStructElementPtr createGroup();
    SvgPathPtr createPath();

    virtual void beforeSave() override;
    virtual void afterSave() override;
    virtual void afterLoad() override;

    void load(tinyxml2::XMLElement* element) override;
    void saveAsChildOf(tinyxml2::XMLElement* xmlElement) override;
};

#endif // SVGSTRUCTELEMENT_H
