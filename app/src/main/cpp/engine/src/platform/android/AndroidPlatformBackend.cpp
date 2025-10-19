#include "t760_engine/platform/android/AndroidPlatformBackend.h"
#include <stdexcept>
#include <iostream>

namespace t760 {

AndroidPlatformBackend::AndroidPlatformBackend() = default;

AndroidPlatformBackend::~AndroidPlatformBackend() {
    shutdown();
}

void AndroidPlatformBackend::initialize(const DeviceManager& device_manager) {
    if (is_initialized_) {
        throw std::runtime_error("AndroidPlatformBackend is already initialized.");
    }

    if (device_manager.has_device(DeviceType::CPU)) {
        cpu_allocator_ = std::make_unique<CpuAllocator>();
        cpu_allocator_->initialize();
    }

    if (device_manager.has_device(DeviceType::GPU)) {
        const auto* caps = device_manager.get_device(DeviceType::GPU)->get_capabilities<GpuCapabilities>();
        if (caps) {
            gpu_context_ = std::make_unique<AndroidGpuContext>();
            if (!gpu_context_->initialize(*caps)) {
                gpu_context_.reset();
            }
        }
    }

    if (device_manager.has_device(DeviceType::NPU)) {
        const auto* caps = device_manager.get_device(DeviceType::NPU)->get_capabilities<NpuCapabilities>();
        if (caps) {
            npu_context_ = std::make_unique<AndroidNpuContext>();
            if (!npu_context_->initialize(*caps)) {
                npu_context_.reset();
            }
        }
    }
    is_initialized_ = true;
}

void AndroidPlatformBackend::shutdown() {
    if (!is_initialized_) {
        return;
    }
    if (gpu_context_) gpu_context_->shutdown();
    if (npu_context_) npu_context_->shutdown();
    if (cpu_allocator_) cpu_allocator_->shutdown();
    gpu_context_.reset();
    npu_context_.reset();
    cpu_allocator_.reset();
    is_initialized_ = false;
}

// CORRECTED: Added 'const' to match the declaration in the header file.
IGpuContext* AndroidPlatformBackend::get_gpu_context() const {
    return gpu_context_.get();
}

// CORRECTED: Added 'const'
INpuContext* AndroidPlatformBackend::get_npu_context() const {
    return npu_context_.get();
}

// CORRECTED: Return type is IMemoryAllocator* and the function is const.
IMemoryAllocator* AndroidPlatformBackend::get_cpu_allocator() const {
    return cpu_allocator_.get();
}

}