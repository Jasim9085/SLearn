#ifndef T760_DEVICE_CAPABILITIES_H
#define T760_DEVICE_CAPABILITIES_H

#include "t760_engine/core/Types.h"
#include <string>
#include <vector>

namespace t760 {

class DeviceCapabilities {
public:
    virtual ~DeviceCapabilities() = default;
    virtual DeviceType get_type() const = 0;
};

struct CpuCapabilities : public DeviceCapabilities {
    uint32_t total_cores = 0;
    uint32_t performance_cores = 0;
    uint32_t efficiency_cores = 0;
    bool has_neon_support = false;
    std::string architecture_name;

    DeviceType get_type() const override { return DeviceType::CPU; }
};

struct GpuCapabilities : public DeviceCapabilities {
    std::string device_name;
    uint32_t compute_units = 0;
    uint64_t dedicated_memory_mb = 0;
    bool has_fp16_support = false;
    bool supports_vulkan = false;
    void* vulkan_physical_device = nullptr;
    void* vulkan_logical_device = nullptr;

    DeviceType get_type() const override { return DeviceType::GPU; }
};

struct NpuCapabilities : public DeviceCapabilities {
    bool is_available = false;
    std::string driver_version;
    float theoretical_tops = 0.0f;
    bool supports_int8 = false;
    bool supports_int16 = false;
    void* nnapi_context = nullptr;

    DeviceType get_type() const override { return DeviceType::NPU; }
};

struct SystemHardwareCapabilities {
    CpuCapabilities cpu;
    GpuCapabilities gpu;
    NpuCapabilities npu;
};

}

#endif // T760_DEVICE_CAPABILITIES_H