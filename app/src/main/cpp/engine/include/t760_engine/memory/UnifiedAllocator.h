#ifndef T760_UNIFIED_ALLOCATOR_H
#define T760_UNIFIED_ALLOCATOR_H

#include "t760_engine/memory/MemoryTypes.h"
#include "t760_engine/memory/IMemoryAllocator.h"
#include <memory>
#include <mutex>
#include <unordered_map>

namespace t760 {

class DeviceManager;

class UnifiedAllocator {
public:
    explicit UnifiedAllocator(const DeviceManager& device_manager);
    ~UnifiedAllocator();

    UnifiedAllocator(const UnifiedAllocator&) = delete;
    UnifiedAllocator& operator=(const UnifiedAllocator&) = delete;

    void initialize();
    void shutdown();

    std::unique_ptr<Buffer> allocate(size_t size, DeviceType device, MemoryUsage usage = MemoryUsage::DEVICE_LOCAL);

private:
    const DeviceManager& device_manager_;
    std::unordered_map<DeviceType, std::unique_ptr<IMemoryAllocator>> backends_;
    bool is_initialized_ = false;
    std::mutex mtx_;
};

}

#endif // T760_UNIFIED_ALLOCATOR_H