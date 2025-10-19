#ifndef T760_INFERENCE_PIPELINE_H
#define T760_INFERENCE_PIPELINE_H

#include "t760_engine/core/Types.h"
#include "t760_engine/model/Model.h"
#include "t760_engine/pipeline/PipelineTypes.h"
#include <vector>
#include <memory>
#include <mutex>

namespace t760 {

class DeviceManager;
class TensorManager;
class Tensor;

class InferencePipeline {
public:
    InferencePipeline(DeviceManager& device_manager, TensorManager& tensor_manager);
    ~InferencePipeline();

    InferencePipeline(const InferencePipeline&) = delete;
    InferencePipeline& operator=(const InferencePipeline&) = delete;

    void prepare(Model& model);
    void release();
    ConversationHandle create_new_context();
    void destroy_context(ConversationHandle handle);
    std::unique_ptr<Tensor> execute(ConversationHandle handle, const std::vector<int>& input_token_ids);

private:
    DeviceManager& device_manager_;
    TensorManager& tensor_manager_;
    Model* active_model_ = nullptr;
    bool is_prepared_ = false;
    std::mutex context_mtx_;
    uint64_t next_context_id_ = 1;
    std::unordered_map<uint64_t, std::unique_ptr<ConversationState>> conversation_contexts_;
};

}

#endif // T760_INFERENCE_PIPELINE_H