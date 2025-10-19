#ifndef T760_INPU_CONTEXT_H
#define T760_INPU_CONTEXT_H

#include "t760_engine/device/DeviceCapabilities.h"

namespace t760 {

// FORWARD DECLARATION
class IMemoryAllocator;

// Interface for managing the NPU device via its driver (e.g., NNAPI)
class INpuContext {
public:
    virtual ~INpuContext() = default;

    virtual bool initialize(const NpuCapabilities& caps) = 0;
    virtual void shutdown() = 0;

    virtual IMemoryAllocator* get_allocator() = 0;
    virtual void* get_native_context() = 0;
};

} // namespace t760

#endif // T760_INPU_CONTEXT_H