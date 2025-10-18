#ifndef T760_LAYER_EXECUTOR_H
#define T760_LAYER_EXECUTOR_H

#include "t760_engine/tensor/Tensor.h"
#include "t760_engine/core/Types.h"
#include <vector>
#include <memory>

namespace t760 {

// An abstract base class for executing a layer on a specific hardware device.
class ILayerExecutor {
public:
    virtual ~ILayerExecutor() = default;
    virtual void execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) = 0;
};

// Concrete implementation for CPU execution using NEON/Eigen.
class CpuLayerExecutor : public ILayerExecutor {
public:
    void execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) override;
};

// Concrete implementation for GPU execution using Vulkan compute shaders.
class GpuLayerExecutor : public ILayerExecutor {
public:
    // The executor now requires the Vulkan device and queue handles to function.
    GpuLayerExecutor(void* device, void* queue);
    ~GpuLayerExecutor() override;

    GpuLayerExecutor(const GpuLayerExecutor&) = delete;
    GpuLayerExecutor& operator=(const GpuLayerExecutor&) = delete;

    void execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) override;

private:
    // Using the PIMPL (Pointer to Implementation) idiom to hide the complex
    // Vulkan implementation details from this public header file.
    struct GpuImpl;
    std::unique_ptr<GpuImpl> pimpl;
};

// Concrete implementation for NPU execution using NNAPI.
class NpuLayerExecutor : public ILayerExecutor {
public:
    // The executor will require the NNAPI context/compilation handles.
    explicit NpuLayerExecutor(void* nnapi_context);
    ~NpuLayerExecutor() override;
    
    NpuLayerExecutor(const NpuLayerExecutor&) = delete;
    NpuLayerExecutor& operator=(const NpuLayerExecutor&) = delete;

    void execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) override;

private:
    struct NpuImpl;
    std::unique_ptr<NpuImpl> pimpl;
};

}

#endif // T760_LAYER_EXECUTOR_H