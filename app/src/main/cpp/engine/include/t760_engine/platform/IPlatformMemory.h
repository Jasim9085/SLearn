#ifndef T760_IPLATFORM_MEMORY_H
#define T760_IPLATFORM_MEMORY_H

#include "t760_engine/memory/MemoryTypes.h"
#include <memory>

namespace t760 {

// Interface for a device-specific memory allocator
class IPlatformMemory {
public:
    virtual ~IPlatformMemory() = default;
    virtual std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) = 0;
};

}

#endif // T760_IPLATFORM_MEMORY_H