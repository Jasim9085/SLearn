#ifndef T760_MODEL_CONFIG_H
#define T760_MODEL_CONFIG_H

#include <cstdint>
#include <vector>

namespace t760 {

// Directly maps to the T760Header in the file format
#pragma pack(push, 1)
struct ModelHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t arch_type;
    uint32_t layer_count;
    uint32_t vocab_size;
    uint32_t hidden_size;
    uint32_t intermediate_size;
    uint32_t heads;
    uint32_t head_size;
    uint32_t kv_heads;
    uint32_t seq_len;
    uint8_t  quant_method;
    uint8_t  alignment;
    uint16_t reserved;
    float    rope_freq_base;
    uint8_t  checksum[16];
};

// Directly maps to the T760HardwareHeader
struct HardwareConfigHeader {
    uint8_t  npu_precision;
    uint8_t  npu_tensor_cores;
    uint16_t npu_batch_size;
    uint8_t  gpu_compute_units;
    uint8_t  gpu_warp_size;
    uint16_t gpu_workgroup_size;
    uint8_t  a75_cores_mask;
    uint8_t  a55_cores_mask;
    uint8_t  migration_hint;
    uint8_t  cache_level;
    uint8_t  dma_burst_size;
    uint16_t memory_channels;
    uint8_t  reserved[48];
};

// Directly maps to the ExecutionPlanHeader
struct ExecutionPlanHeader {
    uint32_t npu_tensors_start_idx;
    uint32_t npu_tensors_end_idx;
    uint32_t gpu_tensors_start_idx;
    uint32_t gpu_tensors_end_idx;
    uint32_t cpu_tensors_start_idx;
    uint32_t cpu_tensors_end_idx;
};

// Directly maps to the TensorBlock in the file's index table
struct TensorMetadata {
    char       name[128];
    uint8_t    processor_id;
    uint8_t    reserved[3];
    uint32_t   data_type;
    uint64_t   offset;
    uint64_t   stored_size;
    uint64_t   original_size;
    uint32_t   dims[4];
};
#pragma pack(pop)

// A high-level config structure holding all parsed metadata from the file
struct ModelConfig {
    ModelHeader model_header;
    HardwareConfigHeader hardware_header;
    ExecutionPlanHeader exec_plan_header;
    std::vector<TensorMetadata> tensor_metadata_table;
};

}

#endif // T760_MODEL_CONFIG_H