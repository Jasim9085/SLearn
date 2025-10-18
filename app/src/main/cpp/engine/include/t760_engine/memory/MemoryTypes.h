#ifndef T760_MEMORY_TYPES_H
#define T760_MEMORY_TYPES_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include "t760_engine/core/Types.h"

namespace t760 {

enum class MemoryUsage {
    DEVICE_LOCAL,
    HOST_VISIBLE_COHERENT,
    HOST_VISIBLE_CACHED
};

class Buffer {
public:
    using Deallocator = std::function<void(void* native_handle, void* mapped_ptr, size_t size)>;

    Buffer(DeviceType device, void* handle, void* mapped_ptr, size_t size, Deallocator deallocator);
    ~Buffer();

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;

    DeviceType get_device_type() const { return device_type_; }
    void* get_native_handle() const { return native_handle_; }
    void* get_mapped_ptr() const { return mapped_ptr_; }
    size_t get_size() const { return size_; }
    bool is_mapped() const { return mapped_ptr_ != nullptr; }

private:
    DeviceType device_type_ = DeviceType::CPU;
    void* native_handle_ = nullptr;
    void* mapped_ptr_ = nullptr;
    size_t size_ = 0;
    Deallocator deallocator_;
};

}

#endif // T760_MEMORY_TYPES_H