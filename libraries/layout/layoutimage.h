#ifndef LAYOUTIMAGE_H
#define LAYOUTIMAGE_H

#include "layoutcore.h"

#include "image.h"

class LayoutImage : public LayoutBase {
    mssm::Image image;
    SizeBound2d bound;
public:
    LayoutImage(Private privateTag, LayoutContext* context, mssm::Image img);
    virtual ~LayoutImage();
    void updateImage(mssm::Image img);
    static std::shared_ptr<LayoutImage> make(LayoutContext* context, mssm::Image img) { return std::make_shared<LayoutImage>(Private{}, context, img); }
    std::string getTypeStr() const override { return "Image"; }
    void draw(const PropertyBag& parentProps, mssm::Canvas2d& g) override;
    SizeBound2d getBound(const PropertyBag& parentProps) override;
    void resize(const PropertyBag& parentProps, const RectI& rect) override;
    void foreachChildImpl(std::function<void(LayoutBase*)> f, ForeachContext context, bool includeOverlay, bool includeCollapsed) override;
    EvtRes onMouse(const PropertyBag& parentProps, MouseEventReason reason, const MouseEvt &evt) override;
};

#endif // LAYOUTIMAGE_H
