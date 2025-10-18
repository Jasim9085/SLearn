#include "t760_engine/memory/CpuAllocator.h"
#include "t760_engine/core/Constants.h"
#include <stdexcept>
#include <iostream>

#ifdef _WIN32
#include <malloc.h>
#else
#include <stdlib.h>
#endif

namespace t760 {

void CpuAllocator::initialize() {
    // CPU requires no special context initialization
}

void CpuAllocator::shutdown() {
    // CPU requires no special context shutdown
}

void CpuAllocator::deallocate(void* handle, void* mapped_ptr, size_t size) {
    #ifdef _WIN32
    _aligned_free(handle);
    #else
    free(handle);
    #endif
}

std::unique_ptr<Buffer> CpuAllocator::allocate(size_t size, MemoryUsage usage) {
    void* ptr = nullptr;
    #ifdef _WIN32
    ptr = _aligned_malloc(size, constants::T760_DEFAULT_MEMORY_ALIGNMENT_BYTES);
    #else
    if (posix_memalign(&ptr, constants::T760_DEFAULT_MEMORY_ALIGNMENT_BYTES, size) != 0) {
        ptr = nullptr;
    }
    #endif

    if (!ptr) {
        throw std::bad_alloc();
    }
    
    auto deallocator = [this](void* h, void* m, size_t s){ this->deallocate(h, m, s); };
    return std::make_unique<Buffer>(DeviceType::CPU, ptr, ptr, size, deallocator);
}

}