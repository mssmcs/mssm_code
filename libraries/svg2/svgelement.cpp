#include "svgelement.h"
#include "svgpath.h"
#include "svgstructelement.h"
#include "tinyxml2.h"


using namespace std;


SvgElement::SvgElement() {}

void SvgElement::loadAttributes(tinyxml2::XMLElement* xmlElement) {
    for (const tinyxml2::XMLAttribute* attr = xmlElement->FirstAttribute(); attr; attr = attr->Next()) {
        attributes[attr->Name()] = attr->Value();
    }
}

void SvgElement::saveAttributes(tinyxml2::XMLElement* xmlElement) {
    for (const auto& attr : attributes) {
        xmlElement->SetAttribute(attr.first.c_str(), attr.second.c_str());
    }
}

SvgElementPtr SvgElement::createFromXml(tinyxml2::XMLElement* xmlElement) {
    SvgElementPtr ptr;

    std::string name = xmlElement->Name();

    if (name == "path") {
        ptr.reset(new SvgPath());
    } else if (name == "g") {
        ptr.reset(new SvgStructElement());
    } else if (name == "rect") {
        // Handle 'rect' element
    } else if (name == "ellipse") {
        // Handle 'ellipse' element
    }

    if (ptr) {
        ptr->load(xmlElement);
    }

    return ptr;
}
