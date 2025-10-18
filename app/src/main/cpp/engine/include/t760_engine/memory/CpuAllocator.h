#ifndef T760_CPU_ALLOCATOR_H
#define T760_CPU_ALLOCATOR_H

#include "t760_engine/memory/IMemoryAllocator.h"

namespace t760 {

class CpuAllocator : public IMemoryAllocator {
public:
    void initialize() override;
    void shutdown() override;
    std::unique_ptr<Buffer> allocate(size_t size, MemoryUsage usage) override;

private:
    void deallocate(void* handle, void* mapped_ptr, size_t size);
};

}

#endif // T760_CPU_ALLOCATOR_H