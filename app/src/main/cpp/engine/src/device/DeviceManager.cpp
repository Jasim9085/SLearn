#include "t760_engine/device/DeviceManager.h"
#include "t760_engine/device/HardwareProber.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

namespace t760 {

Device::Device(DeviceType type, std::unique_ptr<DeviceCapabilities> caps)
    : type_(type), capabilities_(std::move(caps)) {}

Device::~Device() = default;

DeviceManager::DeviceManager() = default;

DeviceManager::~DeviceManager() {
    if (is_initialized_) {
        shutdown();
    }
}

void DeviceManager::initialize(const std::vector<DeviceConfig>& requested_devices) {
    if (is_initialized_) {
        throw std::runtime_error("DeviceManager is already initialized.");
    }

    system_caps_ = HardwareProber::probe();

    for (const auto& config : requested_devices) {
        if (!config.enabled) {
            continue;
        }
        
        std::unique_ptr<DeviceCapabilities> capabilities;
        switch (config.type) {
            case DeviceType::CPU:
                capabilities = std::make_unique<CpuCapabilities>(system_caps_.cpu);
                break;
            case DeviceType::GPU:
                if (system_caps_.gpu.supports_vulkan) {
                    capabilities = std::make_unique<GpuCapabilities>(system_caps_.gpu);
                }
                break;
            case DeviceType::NPU:
                if (system_caps_.npu.is_available) {
                    capabilities = std::make_unique<NpuCapabilities>(system_caps_.npu);
                }
                break;
            default:
                break;
        }

        if (capabilities) {
            active_devices_.push_back(std::make_unique<Device>(config.type, std::move(capabilities)));
            std::cout << "Activated device: " << static_cast<int>(config.type) << std::endl;
        } else {
            std::cerr << "Warning: Requested device " << static_cast<int>(config.type) 
                      << " is not available or could not be initialized." << std::endl;
        }
    }

    if (active_devices_.empty()) {
        throw std::runtime_error("No active devices were successfully configured.");
    }

    is_initialized_ = true;
}

void DeviceManager::shutdown() {
    if (!is_initialized_) {
        return;
    }
    
    active_devices_.clear();
    is_initialized_ = false;
    std::cout << "DeviceManager shut down." << std::endl;
}

const Device* DeviceManager::get_device(DeviceType type) const {
    auto it = std::find_if(active_devices_.begin(), active_devices_.end(),
        [type](const auto& device) { return device->get_type() == type; });
    
    if (it != active_devices_.end()) {
        return it->get();
    }
    return nullptr;
}

bool DeviceManager::has_device(DeviceType type) const {
    return get_device(type) != nullptr;
}

const std::vector<std::unique_ptr<Device>>& DeviceManager::get_active_devices() const {
    return active_devices_;
}

}