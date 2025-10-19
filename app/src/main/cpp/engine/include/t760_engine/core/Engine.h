#ifndef T760_ENGINE_H
#define T760_ENGINE_H

#include "t760_engine/core/Types.h"
#include <string>
#include <memory>
#include <vector>

namespace t760 {

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

    void initialize(const EngineConfig& config);
    void shutdown();
    bool load_model(const std::string& model_path);
    void unload_model();
    ConversationHandle start_new_conversation();
    void end_conversation(ConversationHandle handle);
    std::unique_ptr<Tensor> generate(ConversationHandle handle, const std::vector<int>& input_token_ids);
    EngineState get_state() const;
    bool is_model_loaded() const;

private:
    EngineState state_ = EngineState::UNINITIALIZED;
    std::unique_ptr<DeviceManager> device_manager_;
    std::unique_ptr<IPlatformBackend> platform_backend_;
    std::unique_ptr<TensorManager> tensor_manager_;
    std::unique_ptr<ModelLoader> model_loader_;
    std::unique_ptr<InferencePipeline> inference_pipeline_;
};
}

#endif // T760_ENGINE_H