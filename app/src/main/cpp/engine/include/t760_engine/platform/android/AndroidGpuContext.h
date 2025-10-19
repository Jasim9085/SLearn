#ifndef T760_ANDROID_GPU_CONTEXT_H
#define T760_ANDROID_GPU_CONTEXT_H

#include "t760_engine/platform/IGpuContext.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace t760 {

// FORWARD DECLARATION: This class needs to know about VulkanPlatformMemory
// but we don't include the full header here to keep this file clean.
class VulkanPlatformMemory;

class AndroidGpuContext : public IGpuContext {
public:
    AndroidGpuContext();
    ~AndroidGpuContext() override;

    AndroidGpuContext(const AndroidGpuContext&) = delete;
    AndroidGpuContext& operator=(const AndroidGpuContext&) = delete;

    bool initialize(const GpuCapabilities& caps) override;
    void shutdown() override;

    // CORRECTED: The return type must be IMemoryAllocator as defined in the interface.
    IMemoryAllocator* get_allocator() override;

    void* get_command_queue() override;
    void* get_native_device() override;

private:
    bool create_instance();
    bool select_physical_device();
    bool create_logical_device();

    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice device_ = VK_NULL_HANDLE;
    VkQueue compute_queue_ = VK_NULL_HANDLE;
    uint32_t compute_queue_family_index_ = 0;

    std::unique_ptr<VulkanPlatformMemory> memory_allocator_;
};

} // namespace t760

#endif // T760_ANDROID_GPU_CONTEXT_H