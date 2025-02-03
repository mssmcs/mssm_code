#include "svgstructelement.h"
#include "svgpath.h"

SvgStructElement::SvgStructElement()
{

}

void SvgStructElement::beforeSave()
{

}

void SvgStructElement::afterSave()
{

}

void SvgStructElement::afterLoad()
{

}

std::vector<SvgElementPtr> SvgStructElement::children()
{
    return elements;
}

SvgStructElementPtr SvgStructElement::createGroup()
{
    SvgStructElementPtr group;

    group.reset(new SvgStructElement());

    elements.push_back(group);

    return group;
}

SvgPathPtr SvgStructElement::createPath()
{
    SvgPathPtr path;

    path.reset(new SvgPath());

    elements.push_back(path);

    return path;
}

void SvgStructElement::load(tinyxml2::XMLElement* element) {
    loadAttributes(element);

    for (tinyxml2::XMLElement* child = element->FirstChildElement(); child; child = child->NextSiblingElement()) {
        auto svgElement = SvgElement::createFromXml(child);
        if (svgElement) {
            elements.push_back(svgElement);
        }
    }

    afterLoad();
}

void SvgStructElement::saveAsChildOf(tinyxml2::XMLElement* parent) {
    tinyxml2::XMLElement* group = parent->GetDocument()->NewElement("g");
    parent->InsertEndChild(group);

    beforeSave();

    saveAttributes(group);

    for (auto& child : elements) {
        child->saveAsChildOf(group);
    }

    afterSave();
}
