#ifndef T760_ANDROID_PLATFORM_BACKEND_H
#define T760_ANDROID_PLATFORM_BACKEND_H

#include "t760_engine/platform/IPlatformBackend.h"
#include "t760_engine/platform/android/AndroidGpuContext.h"
#include "t760_engine/platform/android/AndroidNpuContext.h"
#include "t760_engine/memory/CpuAllocator.h"

namespace t760 {

class AndroidPlatformBackend : public IPlatformBackend {
public:
    AndroidPlatformBackend();
    ~AndroidPlatformBackend() override;

    AndroidPlatformBackend(const AndroidPlatformBackend&) = delete;
    AndroidPlatformBackend& operator=(const AndroidPlatformBackend&) = delete;

    void initialize(const DeviceManager& device_manager) override;
    void shutdown() override;

    IGpuContext* get_gpu_context() override;
    INpuContext* get_npu_context() override;
    IPlatformMemory* get_cpu_allocator() override;

private:
    bool is_initialized_ = false;
    std::unique_ptr<AndroidGpuContext> gpu_context_;
    std::unique_ptr<AndroidNpuContext> npu_context_;
    std::unique_ptr<CpuAllocator> cpu_allocator_;
};

}

#endif // T760_ANDROID_PLATFORM_BACKEND_H