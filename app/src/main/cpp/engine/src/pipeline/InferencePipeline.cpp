#include "t760_engine/pipeline/InferencePipeline.h"
#include "t760_engine/device/DeviceManager.h"
#include "t760_engine/tensor/TensorManager.h"
#include "t760_engine/tensor/Tensor.h"
#include <stdexcept>
#include <iostream>

namespace t760 {

InferencePipeline::InferencePipeline(DeviceManager& device_manager, TensorManager& tensor_manager)
    : device_manager_(device_manager), tensor_manager_(tensor_manager) {}

InferencePipeline::~InferencePipeline() { release(); }

void InferencePipeline::prepare(Model& model) {
    if (is_prepared_) { throw std::runtime_error("InferencePipeline is already prepared."); }
    active_model_ = &model;
    is_prepared_ = true;
}

void InferencePipeline::release() {
    std::lock_guard<std::mutex> lock(context_mtx_);
    conversation_contexts_.clear();
    active_model_ = nullptr;
    is_prepared_ = false;
}

ConversationHandle InferencePipeline::create_new_context() {
    if (!is_prepared_) { throw std::runtime_error("Pipeline must be prepared."); }
    std::lock_guard<std::mutex> lock(context_mtx_);
    auto handle = ConversationHandle{next_context_id_++};
    auto state = std::make_unique<ConversationState>();
    state->handle = handle;
    const auto& config = active_model_->get_config();
    const size_t layer_count = config.model_header.layer_count;
    state->kv_cache.reserve(layer_count);
    for (size_t i = 0; i < layer_count; ++i) {
        // FIX: Correct C++ syntax for initializing a struct's vector member.
        TensorShape kv_shape{{1, (int64_t)config.model_header.seq_len, (int64_t)config.model_header.kv_heads, (int64_t)config.model_header.head_size}};
        
        DeviceType target_device = device_manager_.has_device(DeviceType::GPU) ? DeviceType::GPU : DeviceType::CPU;
        auto k_cache = tensor_manager_.create_tensor("k_cache_" + std::to_string(i), kv_shape, DataType::FP16, target_device);
        auto v_cache = tensor_manager_.create_tensor("v_cache_" + std::to_string(i), kv_shape, DataType::FP16, target_device);
        state->kv_cache.emplace_back(std::move(k_cache), std::move(v_cache));
    }
    conversation_contexts_[handle.id] = std::move(state);
    return handle;
}

void InferencePipeline::destroy_context(ConversationHandle handle) {
    std::lock_guard<std::mutex> lock(context_mtx_);
    conversation_contexts_.erase(handle.id);
}

// FIX: Return type is now std::unique_ptr<Tensor>
std::unique_ptr<Tensor> InferencePipeline::execute(ConversationHandle handle, const std::vector<int>& input_token_ids) {
    if (!is_prepared_) { throw std::runtime_error("Cannot execute: pipeline is not prepared."); }
    
    std::unique_lock<std::mutex> lock(context_mtx_);
    auto it = conversation_contexts_.find(handle.id);
    if (it == conversation_contexts_.end()) { throw std::runtime_error("Invalid conversation handle."); }
    ConversationState* current_state = it->second.get();
    lock.unlock();

    // Placeholder logic...
    // FIX: Correct C++ syntax for initializing a struct's vector member.
    TensorShape output_shape{{1, (int64_t)input_token_ids.size()}};

    // FIX: Correctly return the unique_ptr by moving it.
    return tensor_manager_.create_tensor("output_logits", output_shape, DataType::FP32, DeviceType::CPU);
}

}