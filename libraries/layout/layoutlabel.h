#ifndef LAYOUTLABEL_H
#define LAYOUTLABEL_H

#include "layoutcore.h"

class LayoutLabel : public LayoutBase {
    std::string text;
    FontInfo sizeAndFace;
    HAlign hAlign;
    VAlign vAlign;
    Padding padding{4};
public:
    mssm::Color borderColor{mssm::TRANS};
    mssm::Color backgroundColor{mssm::TRANS};
    mssm::Color textColor{mssm::WHITE};
public:
    LayoutLabel(Private privateTag, LayoutContext* context, std::string text, const FontInfo& sizeAndFace);
    virtual ~LayoutLabel();
    static std::shared_ptr<LayoutLabel> make(LayoutContext* context, std::string text, int fontSize = 20) { return std::make_shared<LayoutLabel>(Private{}, context, text, fontSize); }
    std::string getTypeStr() const override { return "Label"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
};

#endif // LAYOUTLABEL_H
