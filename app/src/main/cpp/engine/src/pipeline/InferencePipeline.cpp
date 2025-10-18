#include "t760_engine/pipeline/InferencePipeline.h"
#include "t760_engine/device/DeviceManager.h"
#include "t760_engine/tensor/TensorManager.h"
#include "t760_engine/tensor/Tensor.h"
#include <stdexcept>
#include <iostream>

namespace t760 {

InferencePipeline::InferencePipeline(DeviceManager& device_manager, TensorManager& tensor_manager)
    : device_manager_(device_manager), tensor_manager_(tensor_manager) {}

InferencePipeline::~InferencePipeline() {
    release();
}

void InferencePipeline::prepare(Model& model) {
    if (is_prepared_) {
        throw std::runtime_error("InferencePipeline is already prepared.");
    }
    active_model_ = &model;

    // Here, we would perform one-time preparations based on the model:
    // 1. Validate that the necessary device backends (CPU, GPU, NPU) are active.
    // 2. Pre-compile kernels or shaders for the specific model architecture.
    // 3. Set up the execution schedule based on the model's ExecutionPlanHeader.

    is_prepared_ = true;
    std::cout << "InferencePipeline prepared for model." << std::endl;
}

void InferencePipeline::release() {
    std::lock_guard<std::mutex> lock(context_mtx_);
    conversation_contexts_.clear();
    active_model_ = nullptr;
    is_prepared_ = false;
    std::cout << "InferencePipeline released." << std::endl;
}

ConversationHandle InferencePipeline::create_new_context() {
    if (!is_prepared_) {
        throw std::runtime_error("Pipeline must be prepared before creating a context.");
    }

    std::lock_guard<std::mutex> lock(context_mtx_);
    
    auto handle = ConversationHandle{next_context_id_++};
    auto state = std::make_unique<ConversationState>();
    state->handle = handle;

    // This is the core of the CSC: Pre-allocate the KV cache tensors for the new conversation.
    const auto& config = active_model_->get_config();
    const size_t layer_count = config.model_header.layer_count;
    state->kv_cache.reserve(layer_count);

    for (size_t i = 0; i < layer_count; ++i) {
        // The shape of the KV cache depends on the model architecture.
        // [batch_size, seq_len, num_kv_heads, head_size]
        // For now, we use placeholder shapes.
        TensorShape kv_shape = {1, (int64_t)config.model_header.seq_len, (int64_t)config.model_header.kv_heads, (int64_t)config.model_header.head_size};
        
        // The KV cache should be allocated on the device where attention is computed.
        // This is a simplification; a real implementation would check the execution plan.
        DeviceType target_device = DeviceType::GPU; 
        if (!device_manager_.has_device(DeviceType::GPU)) {
            target_device = DeviceType::CPU; // Fallback
        }

        auto k_cache = tensor_manager_.create_tensor("k_cache_" + std::to_string(i), kv_shape, DataType::FP16, target_device);
        auto v_cache = tensor_manager_.create_tensor("v_cache_" + std::to_string(i), kv_shape, DataType::FP16, target_device);
        state->kv_cache.emplace_back(std::move(k_cache), std::move(v_cache));
    }
    
    conversation_contexts_[handle.id] = std::move(state);
    
    std::cout << "Created new conversation context with handle: " << handle.id << std::endl;
    return handle;
}

void InferencePipeline::destroy_context(ConversationHandle handle) {
    std::lock_guard<std::mutex> lock(context_mtx_);
    if (conversation_contexts_.erase(handle.id) > 0) {
        std::cout << "Destroyed conversation context with handle: " << handle.id << std::endl;
    }
}

Tensor InferencePipeline::execute(ConversationHandle handle, const std::vector<int>& input_token_ids) {
    if (!is_prepared_) {
        throw std::runtime_error("Cannot execute: pipeline is not prepared.");
    }

    // 1. Retrieve the conversation state (including its KV cache).
    std::unique_lock<std::mutex> lock(context_mtx_);
    auto it = conversation_contexts_.find(handle.id);
    if (it == conversation_contexts_.end()) {
        throw std::runtime_error("Invalid conversation handle.");
    }
    ConversationState* current_state = it->second.get();
    lock.unlock();

    // 2. Perform the Prefill phase on the new input tokens.
    //    This involves feeding the tokens through the model and updating the KV cache.
    //    This is a highly complex step involving multiple layer executions.
    //    (Placeholder for Prefill logic)

    // 3. Perform the Generation (Decoding) phase.
    //    This involves a loop that generates one token at a time until an end condition is met.
    //    Each step uses the updated KV cache from the previous step.
    //    (Placeholder for Generation loop)
    
    std::cout << "Executing inference for " << input_token_ids.size() << " tokens on context " << handle.id << "." << std::endl;

    // Placeholder: Create and return a dummy output tensor.
    // A real implementation would return the generated sequence of token IDs.
    TensorShape output_shape = {1, (int64_t)input_token_ids.size()};
    auto output_tensor = tensor_manager_.create_tensor("output_logits", output_shape, DataType::FP32, DeviceType::CPU);

    return *output_tensor;
}

}