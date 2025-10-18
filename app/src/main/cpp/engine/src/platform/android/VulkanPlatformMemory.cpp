#include "t760_engine/platform/android/VulkanPlatformMemory.h"
#include "t760_engine/core/Constants.h"
#include <stdexcept>
#include <iostream>

// VMA Implementation - In a real project, you would add the VMA header to your include path
// and include "vk_mem_alloc.h" here. For this file, we assume it's available.
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace t760 {

// Helper struct to bundle Vulkan resources together for the deallocator lambda.
struct VulkanBufferHandle {
    VkBuffer buffer;
    VmaAllocation allocation;
};

VulkanPlatformMemory::VulkanPlatformMemory(VkInstance instance, VkPhysicalDevice physical_device, VkDevice logical_device)
    : instance_(instance), physical_device_(physical_device), device_(logical_device) {}

VulkanPlatformMemory::~VulkanPlatformMemory() {
    shutdown();
}

void VulkanPlatformMemory::initialize() {
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_1;
    allocatorInfo.physicalDevice = physical_device_;
    allocatorInfo.device = device_;
    allocatorInfo.instance = instance_;

    if (vmaCreateAllocator(&allocatorInfo, &vma_allocator_) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan Memory Allocator.");
    }
    std::cout << "Vulkan Memory Allocator (VMA) initialized." << std::endl;
}

void VulkanPlatformMemory::shutdown() {
    if (vma_allocator_ != nullptr) {
        vmaDestroyAllocator(vma_allocator_);
        vma_allocator_ = nullptr;
        std::cout << "Vulkan Memory Allocator (VMA) shut down." << std::endl;
    }
}

void VulkanPlatformMemory::deallocate(void* native_handle, void* mapped_ptr, size_t size) {
    if (!native_handle) return;
    VulkanBufferHandle* handle = static_cast<VulkanBufferHandle*>(native_handle);
    vmaDestroyBuffer(vma_allocator_, handle->buffer, handle->allocation);
    delete handle;
}

std::unique_ptr<Buffer> VulkanPlatformMemory::allocate(size_t size, MemoryUsage usage) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    VmaAllocationCreateInfo allocInfo = {};
    switch (usage) {
        case MemoryUsage::DEVICE_LOCAL:
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
            allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
            break;
        case MemoryUsage::HOST_VISIBLE_COHERENT:
            allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
            allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            break;
        case MemoryUsage::HOST_VISIBLE_CACHED:
             allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
             allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
            break;
    }

    VulkanBufferHandle* handle = new VulkanBufferHandle();
    VmaAllocationInfo allocationInfo;

    VkResult result = vmaCreateBuffer(vma_allocator_, &bufferInfo, &allocInfo, &handle->buffer, &handle->allocation, &allocationInfo);
    if (result != VK_SUCCESS) {
        delete handle;
        throw std::runtime_error("Failed to allocate Vulkan buffer using VMA.");
    }

    auto deallocator = [this](void* h, void* m, size_t s){ this->deallocate(h, m, s); };
    return std::make_unique<Buffer>(DeviceType::GPU, handle, allocationInfo.pMappedData, size, deallocator);
}

}