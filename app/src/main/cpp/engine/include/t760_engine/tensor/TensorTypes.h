#ifndef T760_TENSOR_TYPES_H
#define T760_TENSOR_TYPES_H

#include <vector>
#include <cstdint>

namespace t760 {

// Defines the physical layout of the tensor data in memory
enum class TensorLayout {
    // Standard row-major layout, e.g., NCHW
    DENSE,
    // NPU-specific packed/tiled format
    PACKED_NPU,
    // GPU-specific format, e.g., texture or block-linear
    OPTIMIZED_GPU
};

struct TensorShape {
    std::vector<int64_t> dims;

    size_t num_elements() const;
    bool is_scalar() const { return dims.empty(); }
    size_t rank() const { return dims.size(); }
};

}

#endif // T760_TENSOR_TYPES_H