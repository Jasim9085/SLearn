#include "t760_engine/core/Engine.h"
#include <iostream>
#include <vector>
#include <stdexcept>

// This is the main entry point for our desktop test runner.
// It demonstrates the full lifecycle of the T760 Engine.
int main(int argc, char* argv[]) {
    std::cout << "--- T760 Engine Desktop Runner ---" << std::endl;

    // 1. Instantiate the Engine
    t760::Engine engine;

    try {
        // 2. Configure the Engine for the T760 SoC
        // This configuration enables all hardware backends as per our design.
        t760::EngineConfig config;
        config.devices = {
            {t760::DeviceType::CPU, 0, true},
            {t760::DeviceType::GPU, 0, true},
            {t760::DeviceType::NPU, 0, true}
        };
        config.enable_profiling = false;

        std::cout << "\n[1] Initializing Engine..." << std::endl;
        engine.initialize(config);
        std::cout << "Engine Initialized. State: " << static_cast<int>(engine.get_state()) << std::endl;

        // 3. Load our custom T760-UOMF model file
        std::string model_path = "path/to/your/model.t760"; // Placeholder path
        std::cout << "\n[2] Loading model: " << model_path << std::endl;
        if (!engine.load_model(model_path)) {
            std::cerr << "Model loading failed. Please check the path and file integrity." << std::endl;
            engine.shutdown();
            return 1;
        }
        std::cout << "Model Loaded. State: " << static_cast<int>(engine.get_state()) << std::endl;

        // 4. Run a sample inference using the Conversational State Cache (CSC)
        std::cout << "\n[3] Running sample inference..." << std::endl;
        t760::ConversationHandle handle = engine.start_new_conversation();
        if (handle.is_valid()) {
            std::cout << "  - Started new conversation with handle: " << handle.id << std::endl;
            
            std::vector<int> input_tokens = { 1, 50256 }; // Dummy tokens
            std::cout << "  - Generating response for " << input_tokens.size() << " tokens..." << std::endl;
            
            // Execute the pipeline. The returned tensor is a placeholder for now.
            t760::Tensor result = engine.generate(handle, input_tokens);
            
            std::cout << "  - Inference complete." << std::endl;

            engine.end_conversation(handle);
            std::cout << "  - Ended conversation." << std::endl;
        }

        // 5. Unload the model to free up resources
        std::cout << "\n[4] Unloading model..." << std::endl;
        engine.unload_model();
        std::cout << "Model Unloaded. State: " << static_cast<int>(engine.get_state()) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\nFATAL ERROR: An exception occurred during engine execution: "
                  << e.what() << std::endl;
        
        // Attempt a graceful shutdown
        engine.shutdown();
        return 1;
    }

    // 6. Gracefully shut down the engine
    std::cout << "\n[5] Shutting down engine..." << std::endl;
    engine.shutdown();
    std::cout << "Engine Shut Down. State: " << static_cast<int>(engine.get_state()) << std::endl;

    std::cout << "\n--- Engine run complete. ---" << std::endl;

    return 0;
}