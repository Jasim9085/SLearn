#include "t760_engine/tensor/TensorManager.h"
#include <stdexcept>

namespace t760 {

static size_t get_size_for_data_type(DataType dtype) {
    switch (dtype) {
        case DataType::FP32: return 4;
        case DataType::FP16: return 2;
        case DataType::BF16: return 2;
        case DataType::INT8: return 1;
        case DataType::UINT8: return 1;
        case DataType::QINT16: return 2;
        case DataType::QINT8: return 1;
        case DataType::QINT4: return 1;
        default: throw std::runtime_error("Unsupported data type.");
    }
}

TensorManager::TensorManager(IPlatformBackend& backend) : platform_backend_(backend) {}

TensorManager::~TensorManager() = default;

std::unique_ptr<Tensor> TensorManager::create_tensor(const std::string& name, const TensorShape& shape,
                                                     DataType dtype, DeviceType device,
                                                     TensorLayout layout, MemoryUsage usage) {
    std::lock_guard<std::mutex> lock(mtx_);

    size_t num_elements = shape.num_elements();
    size_t size_in_bytes;

    if (dtype == DataType::QINT4) {
        size_in_bytes = (num_elements + 1) / 2;
    } else {
        size_in_bytes = num_elements * get_size_for_data_type(dtype);
    }
    
    if (size_in_bytes == 0 && num_elements > 0) {
        throw std::runtime_error("Calculated size in bytes is zero for non-empty tensor.");
    }

    IPlatformMemory* allocator = nullptr;
    switch (device) {
        case DeviceType::CPU:
        case DeviceType::SHARED:
            allocator = platform_backend_.get_cpu_allocator();
            break;
        case DeviceType::GPU:
            if (auto gpu = platform_backend_.get_gpu_context()) {
                allocator = gpu->get_allocator();
            }
            break;
        case DeviceType::NPU:
            if (auto npu = platform_backend_.get_npu_context()) {
                allocator = npu->get_allocator();
            }
            break;
    }

    if (!allocator) {
        throw std::runtime_error("Could not find a valid memory allocator for the target device.");
    }

    auto buffer = allocator->allocate(size_in_bytes, usage);
    if (!buffer) {
        throw std::bad_alloc();
    }

    return std::make_unique<Tensor>(name, shape, dtype, layout, std::move(buffer));
}

}