#ifndef T760_DEVICE_MANAGER_H
#define T760_DEVICE_MANAGER_H

#include "t760_engine/core/Types.h"
#include "t760_engine/device/DeviceCapabilities.h"
#include <vector>
#include <memory>

namespace t760 {

class Device {
public:
    Device(DeviceType type, std::unique_ptr<DeviceCapabilities> caps);
    ~Device();
    
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    DeviceType get_type() const { return type_; }

    template<typename T>
    const T* get_capabilities() const {
        return dynamic_cast<const T*>(capabilities_.get());
    }

private:
    DeviceType type_;
    std::unique_ptr<DeviceCapabilities> capabilities_;
};

class DeviceManager {
public:
    DeviceManager();
    ~DeviceManager();

    DeviceManager(const DeviceManager&) = delete;
    DeviceManager& operator=(const DeviceManager&) = delete;

    void initialize(const std::vector<DeviceConfig>& requested_devices);
    void shutdown();

    const Device* get_device(DeviceType type) const;
    bool has_device(DeviceType type) const;
    const std::vector<std::unique_ptr<Device>>& get_active_devices() const;

private:
    bool is_initialized_ = false;
    SystemHardwareCapabilities system_caps_;
    std::vector<std::unique_ptr<Device>> active_devices_;
};

}

#endif // T760_DEVICE_MANAGER_H