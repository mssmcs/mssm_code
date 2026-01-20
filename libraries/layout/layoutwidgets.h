#ifndef LAYOUTWIDGETS_H
#define LAYOUTWIDGETS_H

#include "layoutcore.h"
#include "texteditbox.h"
#include "layoutadapter.h"
#include "layoutscroll.h"
#include "layoutslider.h"
#include "layoutdraghandle.h"
#include "layoutbutton.h"
#include "layouttext.h"
#include "layoutmodalframe.h"
#include "layouttooltipanchor.h"
#include "layoutlabel.h"
#include "layoutimage.h"
#include "layouttoggle.h"



class LayoutColor : public LayoutBase {
    mssm::Color color;
    SizeBound2d bound;
public:
    LayoutColor(Private privateTag, LayoutContext* context, mssm::Color color, SizeBound2d bound = {200,200,50,50});
    virtual ~LayoutColor();
    static std::shared_ptr<LayoutColor> make(LayoutContext* context, mssm::Color color, SizeBound2d bound) { return std::make_shared<LayoutColor>(Private{}, context, color, bound); }
    static std::shared_ptr<LayoutColor> make(LayoutContext* context, mssm::Color color) { return std::make_shared<LayoutColor>(Private{}, context, color); }
    std::string getTypeStr() const override { return "Color"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};



















#endif // LAYOUTWIDGETS_H
