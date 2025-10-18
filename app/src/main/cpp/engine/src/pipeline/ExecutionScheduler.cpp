#include "t760_engine/pipeline/ExecutionScheduler.h"
#include <stdexcept>

namespace t760 {

ExecutionScheduler::ExecutionScheduler(const Model& model, const DeviceManager& device_manager)
    : model_(model), device_manager_(device_manager) {

    if (device_manager_.has_device(DeviceType::CPU)) {
        executors_[DeviceType::CPU] = std::make_unique<CpuLayerExecutor>();
    }
    if (device_manager_.has_device(DeviceType::GPU)) {
        const auto* gpu_device = device_manager_.get_device(DeviceType::GPU);
        // This is a placeholder for getting the real backend context
        void* vk_device = nullptr; // Get from a future AndroidPlatformBackend
        void* vk_queue = nullptr; // Get from a future AndroidPlatformBackend
        executors_[DeviceType::GPU] = std::make_unique<GpuLayerExecutor>(vk_device, vk_queue);
    }
    if (device_manager_.has_device(DeviceType::NPU)) {
        // This is a placeholder for getting the real backend context
        void* nnapi_context = nullptr; // Get from a future AndroidPlatformBackend
        executors_[DeviceType::NPU] = std::make_unique<NpuLayerExecutor>(nnapi_context);
    }
}

ExecutionScheduler::~ExecutionScheduler() = default;

void ExecutionScheduler::execute_layer(size_t layer_index, const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) {
    const auto& metadata_table = model_.get_config().tensor_metadata_table;
    if (layer_index >= metadata_table.size()) {
        throw std::out_of_range("Layer index is out of range of the model's tensor metadata table.");
    }

    const auto& tensor_meta = metadata_table[layer_index];
    DeviceType target_device = static_cast<DeviceType>(tensor_meta.processor_id);

    auto it = executors_.find(target_device);
    if (it == executors_.end()) {
        throw std::runtime_error("No executor available for the target device specified in the model plan.");
    }

    it->second->execute(inputs, outputs);
}

}