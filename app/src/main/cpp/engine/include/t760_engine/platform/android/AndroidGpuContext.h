#ifndef T760_ANDROID_GPU_CONTEXT_H
#define T760_ANDROID_GPU_CONTEXT_H

#include "t760_engine/platform/IGpuContext.h"
#include "t760_engine/platform/android/VulkanPlatformMemory.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace t760 {

class AndroidGpuContext : public IGpuContext {
public:
    AndroidGpuContext();
    ~AndroidGpuContext() override;

    AndroidGpuContext(const AndroidGpuContext&) = delete;
    AndroidGpuContext& operator=(const AndroidGpuContext&) = delete;

    bool initialize(const GpuCapabilities& caps) override;
    void shutdown() override;

    IPlatformMemory* get_allocator() override;
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

}

#endif // T760_ANDROID_GPU_CONTEXT_H