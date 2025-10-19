#ifndef T760_ANDROID_NPU_MEMORY_H
#define T760_ANDROID_NPU_MEMORY_H

#include "t760_engine/memory/IMemoryAllocator.h" // CORRECTED: Include the base class definition

// Forward-declare NDK types to avoid including NDK headers in our public interface.
struct ANeuralNetworksMemory;

namespace t760 {

// CORRECTED: It inherits from IMemoryAllocator
class AndroidNpuMemory : public IMemoryAllocator {
public:
    AndroidNpuMemory();
    ~AndroidNpuMemory() override;

    void initialize() override;
    void shutdown() override;

    std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) override;

private:
    void deallocate(void* native_handle, void* mapped_ptr, size_t size);
};

} // namespace t760

#endif // T760_ANDROID_NPU_MEMORY_H