#include "t760_engine/platform/android/AndroidGpuContext.h"
#include <stdexcept>
#include <vector>
#include <iostream>

namespace t760 {

AndroidGpuContext::AndroidGpuContext() = default;

AndroidGpuContext::~AndroidGpuContext() {
    shutdown();
}

bool AndroidGpuContext::initialize(const GpuCapabilities& caps) {
    if (instance_ != VK_NULL_HANDLE) {
        std::cerr << "AndroidGpuContext is already initialized." << std::endl;
        return true;
    }

    if (!create_instance()) {
        std::cerr << "Failed to create Vulkan instance." << std::endl;
        return false;
    }

    if (!select_physical_device()) {
        std::cerr << "Failed to select a suitable Vulkan physical device." << std::endl;
        shutdown();
        return false;
    }

    if (!create_logical_device()) {
        std::cerr << "Failed to create Vulkan logical device." << std::endl;
        shutdown();
        return false;
    }
    
    memory_allocator_ = std::make_unique<VulkanPlatformMemory>(instance_, physical_device_, device_);
    memory_allocator_->initialize();

    std::cout << "Android Vulkan GPU Context Initialized Successfully." << std::endl;
    return true;
}

void AndroidGpuContext::shutdown() {
    if (memory_allocator_) {
        memory_allocator_->shutdown();
        memory_allocator_.reset();
    }
    if (device_ != VK_NULL_HANDLE) {
        vkDestroyDevice(device_, nullptr);
        device_ = VK_NULL_HANDLE;
    }
    if (instance_ != VK_NULL_HANDLE) {
        vkDestroyInstance(instance_, nullptr);
        instance_ = VK_NULL_HANDLE;
    }
    std::cout << "Android Vulkan GPU Context Shut Down." << std::endl;
}

IPlatformMemory* AndroidGpuContext::get_allocator() {
    return memory_allocator_.get();
}

void* AndroidGpuContext::get_command_queue() {
    return compute_queue_;
}

void* AndroidGpuContext::get_native_device() {
    return device_;
}

bool AndroidGpuContext::create_instance() {
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "T760_Inference_Engine";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "T760_Engine_Core";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

    return vkCreateInstance(&create_info, nullptr, &instance_) == VK_SUCCESS;
}

bool AndroidGpuContext::select_physical_device() {
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);
    if (device_count == 0) {
        return false;
    }

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance_, &device_count, devices.data());

    for (const auto& device : devices) {
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

        for (uint32_t i = 0; i < queue_families.size(); ++i) {
            if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                physical_device_ = device;
                compute_queue_family_index_ = i;
                return true;
            }
        }
    }

    return false;
}

bool AndroidGpuContext::create_logical_device() {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = compute_queue_family_index_;
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.queueCreateInfoCount = 1;
    create_info.pEnabledFeatures = &device_features;

    if (vkCreateDevice(physical_device_, &create_info, nullptr, &device_) != VK_SUCCESS) {
        return false;
    }

    vkGetDeviceQueue(device_, compute_queue_family_index_, 0, &compute_queue_);
    return true;
}

}