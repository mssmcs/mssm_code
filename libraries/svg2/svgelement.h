#ifndef SVGELEMENT_H
#define SVGELEMENT_H

#include <vector>
#include <memory>
#include <map>
#include <string>
#include "tinyxml2.h"

namespace zen
{
    class XmlElement;
}

// template <typename I>
// struct iter_pair : std::pair<I, I>
// {
//     iter_pair(const std::pair<I, I>& p) : std::pair<I,I>{p} {}
// //    using std::pair<I, I>::pair; // inherit the constructors from std::pair

//     I begin() { return this->first; }
//     I end() { return this->second; }
// };

// template<typename T>
// iter_pair<T> iterable(const std::pair<T, T>& p)
// {
//     return iter_pair<T>{p};
// }

class SvgElement;
using SvgElementPtr = std::shared_ptr<SvgElement>;


class SvgElement
{
protected:
    std::map<std::string, std::string> attributes;
public:
    SvgElement();
    virtual ~SvgElement() {}

    std::string& attribute(const std::string& key) { return attributes[key]; }

    virtual void beforeSave() = 0;  // build svg attributes from other data if necessary for the save
    virtual void afterSave()  = 0;  // clean up svg attributes (get rid of any that were needed only for the save)
    virtual void afterLoad()  = 0;  // clean up svg attributes (get rid of any attributes that have been converted to another form)

    void loadAttributes(tinyxml2::XMLElement* xmlElement);
    void saveAttributes(tinyxml2::XMLElement* xmlElement);

    virtual void load(tinyxml2::XMLElement* xmlElement) = 0;
    virtual void saveAsChildOf(tinyxml2::XMLElement* xmlElement) = 0;

    static SvgElementPtr createFromXml(tinyxml2::XMLElement* xmlElement);
};


#endif // SVGELEMENT_H
