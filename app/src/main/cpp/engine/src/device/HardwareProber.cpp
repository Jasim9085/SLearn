#include "t760_engine/device/HardwareProber.h"
#include "t760_engine/core/Constants.h"

namespace t760 {

SystemHardwareCapabilities HardwareProber::probe() {
    SystemHardwareCapabilities caps;

    caps.cpu.architecture_name = "ARMv8.2-A (Cortex-A76 + Cortex-A55)";
    caps.cpu.total_cores = constants::T760_CPU_TOTAL_CORES;
    caps.cpu.performance_cores = constants::T760_CPU_A76_CORES;
    caps.cpu.efficiency_cores = constants::T760_CPU_A55_CORES;
    caps.cpu.has_neon_support = true;

    caps.gpu.device_name = "Mali-G57 MC4";
    caps.gpu.compute_units = constants::T760_GPU_COMPUTE_UNITS;
    caps.gpu.dedicated_memory_mb = 0;
    caps.gpu.has_fp16_support = true;
    caps.gpu.supports_vulkan = true;

    caps.npu.is_available = true;
    caps.npu.driver_version = "NNAPI 1.3+";
    caps.npu.theoretical_tops = constants::T760_NPU_TOPS;
    caps.npu.supports_int8 = true;
    caps.npu.supports_int16 = true;

    return caps;
}

}