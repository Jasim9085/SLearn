#ifndef T760_VULKAN_PLATFORM_MEMORY_H
#define T760_VULKAN_PLATFORM_MEMORY_H

#include "t760_engine/memory/IMemoryAllocator.h" // CORRECTED: Include the base class definition
#include <vulkan/vulkan.h>

// Forward-declare the VMA types to avoid including the VMA header in our public interface.
struct VmaAllocator_T;
using VmaAllocator = VmaAllocator_T*;
struct VmaAllocation_T;
using VmaAllocation = VmaAllocation_T*;

namespace t760 {

// CORRECTED: It inherits from IMemoryAllocator
class VulkanPlatformMemory : public IMemoryAllocator {
public:
    VulkanPlatformMemory(VkInstance instance, VkPhysicalDevice physical_device, VkDevice logical_device);
    ~VulkanPlatformMemory() override;

    void initialize() override;
    void shutdown() override;

    std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) override;

private:
    void deallocate(void* native_handle, void* mapped_ptr, size_t size);

    VkInstance instance_;
    VkPhysicalDevice physical_device_;
    VkDevice device_;
    VmaAllocator vma_allocator_ = nullptr;
};

} // namespace t760

#endif // T760_VULKAN_PLATFORM_MEMORY_H