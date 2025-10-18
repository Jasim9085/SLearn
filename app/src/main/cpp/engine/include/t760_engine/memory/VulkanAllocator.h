#ifndef T760_VULKAN_ALLOCATOR_H
#define T760_VULKAN_ALLOCATOR_H

#include "t760_engine/memory/IMemoryAllocator.h"
#include "t760_engine/device/DeviceCapabilities.h"

// Forward declare VMA types to avoid including heavy Vulkan headers here
struct VmaAllocator_T;
using VmaAllocator = VmaAllocator_T*;

namespace t760 {

class VulkanAllocator : public IMemoryAllocator {
public:
    VulkanAllocator(const GpuCapabilities& caps);
    ~VulkanAllocator();
    
    void initialize() override;
    void shutdown() override;
    std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) override;
    
private:
    const GpuCapabilities& capabilities_;
    VmaAllocator vma_allocator_ = nullptr;
};

}

#endif // T760_VULKAN_ALLOCATOR_H