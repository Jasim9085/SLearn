#ifndef T760_IMEMORY_ALLOCATOR_H
#define T760_IMEMORY_ALLOCATOR_H

#include "t760_engine/memory/MemoryTypes.h"
#include <memory>

namespace t760 {

class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) = 0;
};

}

#endif // T760_IMEMORY_ALLOCATOR_H