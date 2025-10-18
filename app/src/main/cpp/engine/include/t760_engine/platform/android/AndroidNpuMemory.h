#ifndef T760_ANDROID_NPU_MEMORY_H
#define T760_ANDROID_NPU_MEMORY_H

#include "t760_engine/platform/IPlatformMemory.h"

// Forward-declare NDK types to avoid including NDK headers in our public interface.
struct ANeuralNetworksMemory;

namespace t760 {

class AndroidNpuMemory : public IPlatformMemory {
public:
    AndroidNpuMemory();
    ~AndroidNpuMemory() override;

    void initialize();
    void shutdown();

    std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) override;

private:
    void deallocate(void* native_handle, void* mapped_ptr, size_t size);
};

}

#endif // T760_ANDROID_NPU_MEMORY_H