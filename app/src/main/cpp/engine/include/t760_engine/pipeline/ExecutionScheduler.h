#ifndef T760_EXECUTION_SCHEDULER_H
#define T760_EXECUTION_SCHEDULER_H

#include "t760_engine/model/Model.h"
#include "t760_engine/pipeline/LayerExecutor.h"
#include "t760_engine/device/DeviceManager.h"
#include <memory>
#include <unordered_map>

namespace t760 {

// Responsible for interpreting the model's execution plan and dispatching
// layers to the correct hardware-specific LayerExecutor.
class ExecutionScheduler {
public:
    ExecutionScheduler(const Model& model, const DeviceManager& device_manager);
    ~ExecutionScheduler();

    void execute_layer(size_t layer_index, const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs);

private:
    const Model& model_;
    const DeviceManager& device_manager_;
    std::unordered_map<DeviceType, std::unique_ptr<ILayerExecutor>> executors_;
};

}

#endif // T760_EXECUTION_SCHEDULER_H