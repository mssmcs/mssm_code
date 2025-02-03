#ifndef TRIWRITER_H
#define TRIWRITER_H

#include "vulksmartbuffer.h"
#include "vulksurfacerendermanager.h"

template<typename VertexType>
class TriWriter
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

    template <typename... Args>
    inline void push(Args&&... args)
    {
        vBuff.push(std::forward<Args>(args)...);
    }

    void finish()
    {
        auto vCount = vBuff.nextVertIdx() - startIdx;
        // assertm(vCount == triCount * 3, "triCount triangles were not written");
        dc->commandBuffer->draw(vCount, 1, startIdx, 0);
    }
};

#endif // TRIWRITER_H
