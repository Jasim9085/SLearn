#ifndef T760_IGPU_CONTEXT_H
#define T760_IGPU_CONTEXT_H

#include "t760_engine/device/DeviceCapabilities.h"

namespace t760 {

// FORWARD DECLARATION: This tells the compiler that IMemoryAllocator is a class
// without needing the full definition. This breaks the circular dependency.
class IMemoryAllocator;

// Interface for managing the GPU device and its resources
class IGpuContext {
public:
    virtual ~IGpuContext() = default;

    virtual bool initialize(const GpuCapabilities& caps) = 0;
    virtual void shutdown() = 0;

    virtual IMemoryAllocator* get_allocator() = 0;
    virtual void* get_command_queue() = 0;
    virtual void* get_native_device() = 0;
};

} // namespace t760

#endif // T760_IGPU_CONTEXT_H