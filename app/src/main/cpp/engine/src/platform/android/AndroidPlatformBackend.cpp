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
        std::cout << "CPU backend initialized." << std::endl;
    }

    if (device_manager.has_device(DeviceType::GPU)) {
        const auto* gpu_device = device_manager.get_device(DeviceType::GPU);
        const auto* caps = gpu_device->get_capabilities<GpuCapabilities>();
        if (caps) {
            gpu_context_ = std::make_unique<AndroidGpuContext>();
            if (!gpu_context_->initialize(*caps)) {
                std::cerr << "Warning: Failed to initialize GPU context. GPU will be unavailable." << std::endl;
                gpu_context_.reset();
            } else {
                 std::cout << "GPU backend initialized." << std::endl;
            }
        }
    }

    if (device_manager.has_device(DeviceType::NPU)) {
        const auto* npu_device = device_manager.get_device(DeviceType::NPU);
        const auto* caps = npu_device->get_capabilities<NpuCapabilities>();
        if (caps) {
            npu_context_ = std::make_unique<AndroidNpuContext>();
            if (!npu_context_->initialize(*caps)) {
                std::cerr << "Warning: Failed to initialize NPU context. NPU will be unavailable." << std::endl;
                npu_context_.reset();
            } else {
                std::cout << "NPU backend initialized." << std::endl;
            }
        }
    }
    
    is_initialized_ = true;
}

void AndroidPlatformBackend::shutdown() {
    if (!is_initialized_) {
        return;
    }
    if (gpu_context_) {
        gpu_context_->shutdown();
        gpu_context_.reset();
    }
    if (npu_context_) {
        npu_context_->shutdown();
        npu_context_.reset();
    }
    if (cpu_allocator_) {
        cpu_allocator_->shutdown();
        cpu_allocator_.reset();
    }
    is_initialized_ = false;
    std::cout << "AndroidPlatformBackend shut down." << std::endl;
}

IGpuContext* AndroidPlatformBackend::get_gpu_context() const {
    return gpu_context_.get();
}

INpuContext* AndroidPlatformBackend::get_npu_context() const {
    return npu_context_.get();
}

IPlatformMemory* AndroidPlatformBackend::get_cpu_allocator() const {
    return cpu_allocator_.get();
}

}