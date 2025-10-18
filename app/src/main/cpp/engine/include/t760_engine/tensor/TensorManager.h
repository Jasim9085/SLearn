#ifndef T760_TENSOR_MANAGER_H
#define T760_TENSOR_MANAGER_H

#include "t760_engine/tensor/Tensor.h"
#include "t760_engine/platform/IPlatformBackend.h"
#include <memory>
#include <string>
#include <mutex>

namespace t760 {

// Manages the creation of all tensors, delegating memory allocation
// to the appropriate device-specific allocator from the platform backend.
class TensorManager {
public:
    explicit TensorManager(IPlatformBackend& backend);
    ~TensorManager();

    TensorManager(const TensorManager&) = delete;
    TensorManager& operator=(const TensorManager&) = delete;

    std::unique_ptr<Tensor> create_tensor(const std::string& name, const TensorShape& shape,
                                          DataType dtype, DeviceType device,
                                          TensorLayout layout = TensorLayout::DENSE,
                                          MemoryUsage usage = MemoryUsage::DEVICE_LOCAL);

private:
    IPlatformBackend& platform_backend_;
    std::mutex mtx_;
};

}

#endif // T760_TENSOR_MANAGER_H