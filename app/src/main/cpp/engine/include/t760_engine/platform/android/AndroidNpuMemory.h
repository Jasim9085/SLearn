#ifndef T760_ANDROID_NPU_CONTEXT_H
#define T760_ANDROID_NPU_CONTEXT_H

#include "t760_engine/platform/INpuContext.h"
#include <memory>

namespace t760 {

// FORWARD DECLARATION
class AndroidNpuMemory;

class AndroidNpuContext : public INpuContext {
public:
    AndroidNpuContext();
    ~AndroidNpuContext() override;

    AndroidNpuContext(const AndroidNpuContext&) = delete;
    AndroidNpuContext& operator=(const AndroidNpuContext&) = delete;

    bool initialize(const NpuCapabilities& caps) override;
    void shutdown() override;

    // CORRECTED: The return type must be IMemoryAllocator as defined in the interface.
    IMemoryAllocator* get_allocator() override;
    
    void* get_native_context() override;

private:
    std::unique_ptr<AndroidNpuMemory> memory_allocator_;
    bool is_initialized_ = false;
};

} // namespace t760

#endif // T760_ANDROID_NPU_CONTEXT_H