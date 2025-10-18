#include "t760_engine/platform/android/AndroidNpuContext.h"
#include <iostream>

namespace t760 {

AndroidNpuContext::AndroidNpuContext() = default;

AndroidNpuContext::~AndroidNpuContext() {
    shutdown();
}

bool AndroidNpuContext::initialize(const NpuCapabilities& caps) {
    if (is_initialized_) {
        return true;
    }

    memory_allocator_ = std::make_unique<AndroidNpuMemory>();
    memory_allocator_->initialize();

    // The main NNAPI initialization happens when a model is loaded and compiled.
    // This context simply prepares the memory allocator.

    is_initialized_ = true;
    std::cout << "Android NNAPI NPU Context Initialized Successfully." << std::endl;
    return true;
}

void AndroidNpuContext::shutdown() {
    if (memory_allocator_) {
        memory_allocator_->shutdown();
        memory_allocator_.reset();
    }
    is_initialized_ = false;
    std::cout << "Android NNAPI NPU Context Shut Down." << std::endl;
}

IPlatformMemory* AndroidNpuContext::get_allocator() {
    return memory_allocator_.get();
}

void* AndroidNpuContext::get_native_context() {
    // In a real implementation, this would return a handle to a pre-compiled
    // NNAPI model/compilation. For now, it returns null.
    return nullptr;
}

}