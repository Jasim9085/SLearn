#ifndef T760_IGPU_CONTEXT_H
#define T760_IGPU_CONTEXT_H

#include "t760_engine/device/DeviceCapabilities.h"
#include "t760_engine/platform/IPlatformMemory.h"

namespace t760 {

// Interface for managing the GPU device and its resources
class IGpuContext {
public:
    virtual ~IGpuContext() = default;
    virtual bool initialize(const GpuCapabilities& caps) = 0;
    virtual void shutdown() = 0;
    virtual IPlatformMemory* get_allocator() = 0;
    virtual void* get_command_queue() = 0; // e.g., VkQueue
    virtual void* get_native_device() = 0;   // e.g., VkDevice
};

}

#endif // T760_IGPU_CONTEXT_H