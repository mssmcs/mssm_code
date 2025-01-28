#ifndef VULKSTBI_H
#define VULKSTBI_H

#include "VulkImage.h"

VulkImageBuffer loadImageIntoBuffer(VulkDevice& device, std::string filename);

#endif // VULKSTBI_H