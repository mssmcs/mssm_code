#ifndef TRIWRITER_H
#define TRIWRITER_H

#include "ITriWriter.h"
#include "vulksmartbuffer.h"
#include "vulksurfacerendermanager.h"

template<typename VertexType>
class TriWriter : public ITriWriter<VertexType>
{
    VulkSmartBuffer<VertexType>& vBuff;
    uint32_t triCount;
    uint32_t startIdx;
    VulkDrawContext* dc;
public:
    TriWriter(VulkDrawContext* dc, VulkSmartBuffer<VertexType>& vBuff, uint32_t triCount)
        : dc{ dc },
        vBuff { vBuff },
        triCount{ triCount }
    {
        startIdx = vBuff.nextVertIdx();
        vBuff.ensureSpace(triCount * 3);
    }

    virtual ~TriWriter() {}

    virtual void push(const VertexType& v) override
    {
        vBuff.push(v);
    }

    virtual void push(const VertexType& v1, const VertexType& v2) override
    {
        vBuff.push(v1);
        vBuff.push(v2);
    }

    virtual void push(const VertexType& v1, const VertexType& v2, const VertexType& v3) override
    {
        vBuff.push(v1);
        vBuff.push(v2);
        vBuff.push(v3);
    }

    // template <typename... Args>
    // inline void push(Args&&... args)
    // {
    //     vBuff.push(std::forward<Args>(args)...);
    // }

    virtual void finish() override
    {
        auto vCount = vBuff.nextVertIdx() - startIdx;
        // assertm(vCount == triCount * 3, "triCount triangles were not written");
        dc->commandBuffer->draw(vCount, 1, startIdx, 0);
    }
};

#endif // TRIWRITER_H
