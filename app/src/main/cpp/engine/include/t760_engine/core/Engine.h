#ifndef T760_ENGINE_H
#define T760_ENGINE_H

#include "t760_engine/core/Types.h"
#include <string>
#include <memory>
#include <vector>

namespace t760 {

// Forward declarations for all major engine components
class IPlatformBackend;
class DeviceManager;
class ModelLoader;
class TensorManager;
class InferencePipeline;
class Tensor;

class Engine {
public:
    Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    // Initialize the engine with a specific configuration. This starts all hardware backends.
    void initialize(const EngineConfig& config);
    // Shut down all hardware backends and release resources.
    void shutdown();

    bool load_model(const std::string& model_path);
    void unload_model();
    
    // CSC Management
    ConversationHandle start_new_conversation();
    void end_conversation(ConversationHandle handle);

    // Main inference entry point
    Tensor generate(ConversationHandle handle, const std::vector<int>& input_token_ids);

    EngineState get_state() const;
    bool is_model_loaded() const;

private:
    EngineState state_ = EngineState::UNINITIALIZED;

    // The order of these members is important for correct destruction!
    std::unique_ptr<DeviceManager> device_manager_;
    std::unique_ptr<IPlatformBackend> platform_backend_;
    std::unique_ptr<TensorManager> tensor_manager_;
    std::unique_ptr<ModelLoader> model_loader_;
    std::unique_ptr<InferencePipeline> inference_pipeline_;
};

}

#endif // T760_ENGINE_H