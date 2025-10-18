#ifndef T760_CONSTANTS_H
#define T760_CONSTANTS_H

#include <cstdint>

namespace t760::constants {

// T760 SoC Hardware Specifications
constexpr uint32_t T760_CPU_A76_CORES = 4;
constexpr uint32_t T760_CPU_A55_CORES = 4;
constexpr uint32_t T760_CPU_TOTAL_CORES = 8;
constexpr uint32_t T760_GPU_COMPUTE_UNITS = 4; // Mali-G57 MC4
constexpr float T760_NPU_TOPS = 3.2f;

// Memory and Cache Hierarchy
constexpr uint32_t T760_L1_CACHE_SIZE_KB = 64; // Per-core L1 cache
constexpr uint32_t T760_L2_CACHE_SIZE_KB = 512; // Shared L2 cache in cluster
constexpr uint32_t T760_CACHE_LINE_SIZE_BYTES = 64;
constexpr uint32_t T760_DEFAULT_MEMORY_ALIGNMENT_BYTES = 128;

// Performance and Scheduling Hints
constexpr uint32_t T760_A76_AFFINITY_MASK = 0xF0; // Cores 4, 5, 6, 7
constexpr uint32_t T760_A55_AFFINITY_MASK = 0x0F; // Cores 0, 1, 2, 3
constexpr uint32_t T760_DEFAULT_THREAD_COUNT = 4; // Target A76 cores by default
constexpr uint32_t T760_GPU_OPTIMAL_WORKGROUP_SIZE = 128;
constexpr uint32_t T760_NPU_OPTIMAL_BATCH_SIZE = 1;

// Engine Behavior Configuration
constexpr uint32_t MAX_SUPPORTED_SEQ_LEN = 4096;
constexpr uint32_t MAX_CONCURRENT_CONVERSATIONS = 8;

} // namespace t760::constants

#endif // T760_CONSTANTS_H