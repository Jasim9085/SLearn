#include "t760_engine/memory/UnifiedAllocator.h"
#include "t760_engine/device/DeviceManager.h"
#include "t760_engine/memory/CpuAllocator.h"
#include "t760_engine/memory/VulkanAllocator.h" // Ready for future implementation
#include <stdexcept>
#include <iostream>

namespace t760 {

UnifiedAllocator::UnifiedAllocator(const DeviceManager& device_manager)
    : device_manager_(device_manager) {}

UnifiedAllocator::~UnifiedAllocator() {
    if (is_initialized_) {
        shutdown();
    }
}

void UnifiedAllocator::initialize() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (is_initialized_) {
        throw std::runtime_error("UnifiedAllocator already initialized.");
    }

    if (device_manager_.has_device(DeviceType::CPU)) {
        backends_[DeviceType::CPU] = std::make_unique<CpuAllocator>();
        backends_[DeviceType::CPU]->initialize();
    }

    if (device_manager_.has_device(DeviceType::GPU)) {
        const auto* caps = device_manager_.get_device(DeviceType::GPU)->get_capabilities<GpuCapabilities>();
        if (caps) {
            // backends_[DeviceType::GPU] = std::make_unique<VulkanAllocator>(*caps);
            // backends_[DeviceType::GPU]->initialize();
            std::cout << "VulkanAllocator backend registered (implementation pending)." << std::endl;
        }
    }
    
    // Add NPU backend registration here when NpuAllocator is created

    is_initialized_ = true;
    std::cout << "UnifiedAllocator initialized with concrete backends." << std::endl;
}

void UnifiedAllocator::shutdown() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!is_initialized_) {
        return;
    }
    for (auto& pair : backends_) {
        pair.second->shutdown();
    }
    backends_.clear();
    is_initialized_ = false;
    std::cout << "UnifiedAllocator shut down." << std::endl;
}

std::unique_ptr<Buffer> UnifiedAllocator::allocate(size_t size, DeviceType device, MemoryUsage usage) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!is_initialized_) {
        throw std::runtime_error("UnifiedAllocator not initialized.");
    }
    
    auto it = backends_.find(device);
    if (it == backends_.end()) {
        if (device == DeviceType::SHARED && backends_.count(DeviceType::CPU)) {
            it = backends_.find(DeviceType::CPU);
        } else {
            throw std::runtime_error("No memory backend registered for the requested device type.");
        }
    }

    return it->second->allocate(size, usage);
}

}