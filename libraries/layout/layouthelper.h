#ifndef LAYOUTHELPER_H
#define LAYOUTHELPER_H

//#include "layoutcore.h"
#include "layoutcontainers.h"
#include "layoutwidgets.h"
#include "layouttabs.h"
#include "layoutmenu.h"
#include "layoutflyout.h"
#include "layoutexpander.h"
#include <functional>

namespace LayoutHelper {

using Builder = std::function<LayoutPtr(LayoutContext*)>;
using GridRowBuilder = std::function<LayoutGridRowPtr(LayoutContext*)>;

template<typename T>
using Configurator = std::function<void(T*)>;

template<typename T>
class LayoutConfig {
public:
    Configurator<T> configFunc{};
    std::string  name;
public:
    LayoutConfig() {}

    LayoutConfig(std::string name) : name{name} {}
    LayoutConfig(Configurator<T> configFunc) : configFunc{configFunc} {}

    LayoutConfig(std::string name, Configurator<T> configFunc) : configFunc{configFunc}, name{name} {}

    void applyTo(std::shared_ptr<T>& ptr) const
    {
        ptr->setName(name);
        if (configFunc) {
            configFunc(ptr.get());
        }
    }
};

class Wrapper {
private:
    Builder layout{};
public:
    Wrapper(Builder layout) : layout{layout}
    {
    }
    Wrapper(mssm::Color color)
    {
        layout = [color](LayoutContext* context) { return LayoutColor::make(context, color); } ;
    }
    Wrapper(std::string text)
    {
        layout = [text](LayoutContext* context) { return LayoutText::make(context, text); } ;
    }
    Wrapper(const char* text)
    {
        layout = [text](LayoutContext* context) { return LayoutLabel::make(context, text); } ;
    }
    Wrapper(LayoutPtr child)
    {
        layout = [child](LayoutContext* context) { return child; } ;
    }

    operator Builder() const { return layout; }
};

template <typename T>
class BuilderBase {
protected:
    LayoutConfig<T> config{};
    using ConfigFunc = Configurator<T>;
public:
    BuilderBase(LayoutConfig<T> config = {}) : config{config} {}
};

class HSplit : BuilderBase<LayoutSplitter> {
private:
    Builder left;
    Builder right;
public:
    HSplit(LayoutConfig<LayoutSplitter> config, Wrapper a, Wrapper b) : BuilderBase{config}, left{a}, right{b} {}
    HSplit(Wrapper a, Wrapper b) : left{a}, right{b} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class VSplit : BuilderBase<LayoutSplitter>  {
private:
    Builder top;
    Builder bottom;
public:
    VSplit(LayoutConfig<LayoutSplitter> config, Wrapper a, Wrapper b) : BuilderBase{config}, top{a}, bottom{b} {}
    VSplit(Wrapper a, Wrapper b) : top{a}, bottom{b} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class HStack : BuilderBase<LayoutStacked>  {
private:
    std::vector<Builder> children;
public:
    HStack(LayoutConfig<LayoutStacked> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    HStack(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class VStack : BuilderBase<LayoutStacked>  {
private:
    std::vector<Builder> children;
public:
    VStack(LayoutConfig<LayoutStacked> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    VStack(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class GridRowWrapper {
private:
    std::vector<Builder> children;
public:
    GridRowWrapper(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator GridRowBuilder() const;
};

class Grid : BuilderBase<LayoutGrid>  {
private:
    std::vector<GridRowBuilder> children;
public:
    Grid(LayoutConfig<LayoutGrid> config, std::initializer_list<GridRowWrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    Grid(std::initializer_list<GridRowWrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class Button : BuilderBase<LayoutButton>  {
private:
    std::string label;
public:
    Button(LayoutConfig<LayoutButton> config, std::string label) : BuilderBase{config}, label{label} {}
    Button(std::string label) : label{label} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};


class Panel : BuilderBase<LayoutColor>  {
private:
    mssm::Color color;
    SizeBound2d bound;
public:
    Panel(LayoutConfig<LayoutColor> config, mssm::Color color, SizeBound2d bound = {200,200,50,50}) : BuilderBase{config}, color{color}, bound{bound} {}
    Panel(mssm::Color color, SizeBound2d bound = {200,200,50,50}) : color{color}, bound{bound} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class ImagePanel : BuilderBase<LayoutImage> {
private:
    mssm::Image image;
public:
    ImagePanel(LayoutConfig<LayoutImage> config, mssm::Image image) : BuilderBase{config}, image{image} {}
    ImagePanel(mssm::Image image) : image{image} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class Slider : BuilderBase<LayoutSlider>  {
private:
    bool isHorizontal{true};
    double minValue{0};
    double maxValue{100};
    double value{20};
public:
    Slider(LayoutConfig<LayoutSlider> config, bool isHorizontal) : BuilderBase{config}, isHorizontal{isHorizontal}  {}
    Slider(bool isHorizontal) : isHorizontal{isHorizontal}  {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class Scroll : BuilderBase<LayoutScroll>  {
private:
    Builder child;
public:
    Scroll(LayoutConfig<LayoutScroll> config, Wrapper child) : BuilderBase{config}, child{child} {}
    Scroll(Wrapper child) : child{child} {}
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

Builder operator|(Wrapper aw, Wrapper bw);
Builder operator/(Wrapper aw, Wrapper bw);


class HTabs : BuilderBase<LayoutTabs>  {
private:
    std::vector<Builder> children;
public:
    HTabs(LayoutConfig<LayoutTabs> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    HTabs(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class VTabs : BuilderBase<LayoutTabs>  {
private:
    std::vector<Builder> children;
public:
    VTabs(LayoutConfig<LayoutTabs> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    VTabs(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class HMenu : BuilderBase<LayoutMenu>  {
private:
    std::vector<Builder> children;
public:
    HMenu(LayoutConfig<LayoutMenu> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    HMenu(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class VMenu : BuilderBase<LayoutMenu>  {
private:
    std::vector<Builder> children;
public:
    VMenu(LayoutConfig<LayoutMenu> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    VMenu(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class Flyout : BuilderBase<LayoutFlyout>  {
private:
    std::vector<Builder> children;
public:
    Flyout(LayoutConfig<LayoutFlyout> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    Flyout(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

class Expander : BuilderBase<LayoutExpander>  {
private:
    std::vector<Builder> children;
public:
    Expander(LayoutConfig<LayoutExpander> config, std::initializer_list<Wrapper> kids) : BuilderBase{config} {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    Expander(std::initializer_list<Wrapper> kids) {
        for(auto& c : kids) {
            children.push_back(c);
        }
    }
    operator Builder() const;
    operator Wrapper() const { return static_cast<Builder>(*this); }
};

} // namespace LayoutHelper

#endif // LAYOUTHELPER_H
