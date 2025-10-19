#ifndef T760_IPLATFORM_BACKEND_H
#define T760_IPLATFORM_BACKEND_H

#include "t760_engine/platform/IGpuContext.h"
#include "t760_engine/platform/INpuContext.h"
#include "t760_engine/memory/IMemoryAllocator.h" // CORRECTED: Use the single, correct interface
#include "t760_engine/device/DeviceManager.h"
#include <memory>

namespace t760 {

class IPlatformBackend {
public:
    virtual ~IPlatformBackend() = default;

    virtual void initialize(const DeviceManager& device_manager) = 0;
    virtual void shutdown() = 0;

    // CORRECTED: Added 'const'
    virtual IGpuContext* get_gpu_context() const = 0;
    virtual INpuContext* get_npu_context() const = 0;
    virtual IMemoryAllocator* get_cpu_allocator() const = 0;
};

}

#endif // T760_IPLATFORM_BACKEND_H
