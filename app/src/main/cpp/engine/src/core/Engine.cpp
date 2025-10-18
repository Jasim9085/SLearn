#include "t760_engine/core/Engine.h"
#include "t760_engine/device/DeviceManager.h"
#include "t760_engine/tensor/TensorManager.h"
#include "t760_engine/model/ModelLoader.h"
#include "t760_engine/pipeline/InferencePipeline.h"
#include "t760_engine/tensor/Tensor.h"

// This is the critical include that selects our concrete Android implementation.
#include "t760_engine/platform/android/AndroidPlatformBackend.h"

#include <stdexcept>
#include <iostream>

namespace t760 {

Engine::Engine() = default;

Engine::~Engine() {
    if (state_ != EngineState::SHUTDOWN && state_ != EngineState::UNINITIALIZED) {
        shutdown();
    }
}

void Engine::initialize(const EngineConfig& config) {
    if (state_ != EngineState::UNINITIALIZED) {
        throw std::runtime_error("Engine is already initialized or in an invalid state.");
    }

    try {
        // 1. Discover the hardware capabilities first.
        device_manager_ = std::make_unique<DeviceManager>();
        device_manager_->initialize(config.devices);

        // 2. Initialize the platform backend, which starts the GPU/NPU contexts.
        platform_backend_ = std::make_unique<AndroidPlatformBackend>();
        platform_backend_->initialize(*device_manager_);
        
        // 3. Initialize the TensorManager, giving it access to the platform's allocators.
        tensor_manager_ = std::make_unique<TensorManager>(*platform_backend_);

        // 4. Initialize the ModelLoader, giving it the TensorManager to create tensors.
        model_loader_ = std::make_unique<ModelLoader>(*tensor_manager_);

        // 5. Initialize the InferencePipeline.
        inference_pipeline_ = std::make_unique<InferencePipeline>(*device_manager_, *tensor_manager_);

        state_ = EngineState::INITIALIZED;
        std::cout << "T760 Inference Engine Initialized with Android Platform Backend." << std::endl;
    } catch (const std::exception& e) {
        state_ = EngineState::ERROR_STATE;
        std::cerr << "Engine initialization failed: " << e.what() << std::endl;
        // Attempt to clean up partially initialized components
        shutdown();
        throw;
    }
}

void Engine::shutdown() {
    if (state_ == EngineState::UNINITIALIZED || state_ == EngineState::SHUTDOWN) {
        return;
    }
    
    // The order of shutdown is the reverse of initialization to respect dependencies.
    unload_model();

    inference_pipeline_.reset();
    model_loader_.reset();
    tensor_manager_.reset();
    
    if (platform_backend_) {
        platform_backend_->shutdown();
        platform_backend_.reset();
    }
    
    device_manager_.reset();

    state_ = EngineState::SHUTDOWN;
    std::cout << "T760 Inference Engine Shut Down." << std::endl;
}

bool Engine::load_model(const std::string& model_path) {
    if (state_ != EngineState::INITIALIZED) {
        throw std::runtime_error("Engine must be in INITIALIZED state to load a model.");
    }
    
    bool success = model_loader_->load_model(model_path);
    if (success) {
        inference_pipeline_->prepare(*model_loader_->get_model());
        state_ = EngineState::MODEL_LOADED;
    }
    return success;
}

void Engine::unload_model() {
    if (state_ == EngineState::MODEL_LOADED || state_ == EngineState::INFERENCE_ACTIVE) {
        inference_pipeline_->release();
        model_loader_->unload_model();
        state_ = EngineState::INITIALIZED;
    }
}

ConversationHandle Engine::start_new_conversation() {
    if (state_ != EngineState::MODEL_LOADED && state_ != EngineState::INFERENCE_ACTIVE) {
        throw std::runtime_error("A model must be loaded to start a conversation.");
    }
    return inference_pipeline_->create_new_context();
}

void Engine::end_conversation(ConversationHandle handle) {
    if (state_ == EngineState::MODEL_LOADED || state_ == EngineState::INFERENCE_ACTIVE) {
        inference_pipeline_->destroy_context(handle);
    }
}

Tensor Engine::generate(ConversationHandle handle, const std::vector<int>& input_token_ids) {
    if (state_ != EngineState::MODEL_LOADED && state_ != EngineState::INFERENCE_ACTIVE) {
        throw std::runtime_error("Engine must be in MODEL_LOADED state for inference.");
    }

    EngineState previous_state = state_;
    state_ = EngineState::INFERENCE_ACTIVE;
    Tensor result = inference_pipeline_->execute(handle, input_token_ids);
    state_ = previous_state;
    return result;
}

EngineState Engine::get_state() const {
    return state_;
}

bool Engine::is_model_loaded() const {
    return state_ == EngineState::MODEL_LOADED || state_ == EngineState::INFERENCE_ACTIVE;
}

}