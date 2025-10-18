#include "t760_engine/platform/android/AndroidNpuMemory.h"
#include <android/sharedmem.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <android/log.h>

// For Android NDK's NNAPI
#include <android/NeuralNetworks.h>

#define LOG_TAG "T760_NpuMemory"
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace t760 {

// Helper struct to manage NDK shared memory resources.
struct NpuMemoryHandle {
    int file_descriptor;
    ANeuralNetworksMemory* nnapi_memory;
};

AndroidNpuMemory::AndroidNpuMemory() = default;

AndroidNpuMemory::~AndroidNpuMemory() = default;

void AndroidNpuMemory::initialize() {
    // No specific initialization needed for the allocator itself.
}

void AndroidNpuMemory::shutdown() {
    // No specific shutdown needed for the allocator itself.
}

void AndroidNpuMemory::deallocate(void* native_handle, void* mapped_ptr, size_t size) {
    if (!native_handle) return;
    NpuMemoryHandle* handle = static_cast<NpuMemoryHandle*>(native_handle);

    if (handle->nnapi_memory) {
        ANeuralNetworksMemory_free(handle->nnapi_memory);
    }

    if (mapped_ptr) {
        munmap(mapped_ptr, size);
    }

    if (handle->file_descriptor >= 0) {
        close(handle->file_descriptor);
    }
    
    delete handle;
}

std::unique_ptr<Buffer> AndroidNpuMemory::allocate(size_t size, MemoryUsage usage) {
    // NNAPI works with shared memory file descriptors.
    int fd = ASharedMemory_create(nullptr, size);
    if (fd < 0) {
        ALOGE("ASharedMemory_create failed for size %zu", size);
        throw std::runtime_error("Failed to create Android shared memory for NPU.");
    }

    // Map the shared memory region into our process's address space to write to it.
    void* mapped_ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped_ptr == MAP_FAILED) {
        close(fd);
        ALOGE("mmap failed for NPU shared memory.");
        throw std::runtime_error("Failed to map NPU shared memory.");
    }

    // Create an NNAPI memory object from the file descriptor.
    ANeuralNetworksMemory* nn_memory = nullptr;
    int result = ANeuralNetworksMemory_createFromFd(size, PROT_READ | PROT_WRITE, fd, 0, &nn_memory);
    if (result != ANEURALNETWORKS_NO_ERROR) {
        munmap(mapped_ptr, size);
        close(fd);
        ALOGE("ANeuralNetworksMemory_createFromFd failed with error %d", result);
        throw std::runtime_error("Failed to create NNAPI memory object.");
    }
    
    NpuMemoryHandle* handle = new NpuMemoryHandle{fd, nn_memory};
    
    auto deallocator = [this](void* h, void* m, size_t s){ this->deallocate(h, m, s); };
    
    // The native_handle for the Buffer will be our NpuMemoryHandle struct,
    // which contains everything needed for both NNAPI and deallocation.
    return std::make_unique<Buffer>(DeviceType::NPU, handle, mapped_ptr, size, deallocator);
}

}