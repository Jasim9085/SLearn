#ifndef T760_IMEMORY_ALLOCATOR_H
#define T760_IMEMORY_ALLOCATOR_H

#include "t760_engine/memory/MemoryTypes.h"
#include <memory>

namespace t760 {

// This is the single, abstract base class for all device-specific memory allocators.
// It defines the contract for how memory is allocated for a specific hardware device.
class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;

    // Initializes the allocator, preparing any necessary device contexts.
    virtual void initialize() = 0;

    // Shuts down the allocator, releasing all associated resources.
    virtual void shutdown() = 0;

    // The core allocation method.
    // It must return a unique_ptr to a Buffer, which contains a deallocator
    // lambda, ensuring that memory is automatically and correctly freed when the Buffer
    // goes out of scope (RAII).
    virtual std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) = 0;
};

}

#endif // T760_IMEMORY_ALLOCATOR_H