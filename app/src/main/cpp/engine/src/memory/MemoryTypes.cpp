#include "t760_engine/memory/MemoryTypes.h"
#include <utility>

namespace t760 {

Buffer::Buffer(DeviceType device, void* handle, void* mapped_ptr, size_t size, Deallocator deallocator)
    : device_type_(device), native_handle_(handle), mapped_ptr_(mapped_ptr), 
      size_(size), deallocator_(std::move(deallocator)) {}

Buffer::~Buffer() {
    if (deallocator_) {
        deallocator_(native_handle_, mapped_ptr_, size_);
    }
}

Buffer::Buffer(Buffer&& other) noexcept
    : device_type_(other.device_type_),
      native_handle_(other.native_handle_),
      mapped_ptr_(other.mapped_ptr_),
      size_(other.size_),
      deallocator_(std::move(other.deallocator_)) {
    other.native_handle_ = nullptr;
    other.mapped_ptr_ = nullptr;
    other.size_ = 0;
    other.deallocator_ = nullptr;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
    if (this != &other) {
        if (deallocator_) {
            deallocator_(native_handle_, mapped_ptr_, size_);
        }
        
        device_type_ = other.device_type_;
        native_handle_ = other.native_handle_;
        mapped_ptr_ = other.mapped_ptr_;
        size_ = other.size_;
        deallocator_ = std::move(other.deallocator_);
        
        other.native_handle_ = nullptr;
        other.mapped_ptr_ = nullptr;
        other.size_ = 0;
        other.deallocator_ = nullptr;
    }
    return *this;
}

}