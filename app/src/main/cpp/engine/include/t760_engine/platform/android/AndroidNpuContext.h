#ifndef T760_ANDROID_NPU_CONTEXT_H
#define T760_ANDROID_NPU_CONTEXT_H

#include "t760_engine/platform/INpuContext.h"
#include "t760_engine/platform/IPlatformMemory.h"
#include "t760_engine/platform/android/AndroidNpuMemory.h"
#include <memory>

namespace t760 {

class AndroidNpuContext : public INpuContext {
public:
    AndroidNpuContext();
    ~AndroidNpuContext() override;

    AndroidNpuContext(const AndroidNpuContext&) = delete;
    AndroidNpuContext& operator=(const AndroidNpuContext&) = delete;

    bool initialize(const NpuCapabilities& caps) override;
    void shutdown() override;

    IPlatformMemory* get_allocator() override;
    void* get_native_context() override;

private:
    std::unique_ptr<AndroidNpuMemory> memory_allocator_;
    bool is_initialized_ = false;

    // In a real implementation, this would hold pointers to NNAPI
    // ANeuralNetworksModel and ANeuralNetworksCompilation objects
    // that are built when the engine's model is loaded.
};

}

#endif // T760_ANDROID_NPU_CONTEXT_H