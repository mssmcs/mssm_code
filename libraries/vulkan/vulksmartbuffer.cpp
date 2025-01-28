#include "vulksmartbuffer.h"
#include "vulksynchronization.h"

VulkSmartBufferBase::VulkSmartBufferBase(VulkFramebufferSynchronization &sync)
    : sync{sync}
{

}

VulkSmartBufferBase::~VulkSmartBufferBase()
{

}

int VulkSmartBufferBase::frameInFlight()
{
    return sync.getCurrentFlight();
}

