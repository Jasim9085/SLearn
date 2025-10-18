#include "t760_engine/tensor/Tensor.h"
#include <stdexcept>

namespace t760 {

Tensor::Tensor(const std::string& name, TensorShape shape, DataType dtype,
               TensorLayout layout, std::unique_ptr<Buffer> buffer)
    : name_(name),
      shape_(std::move(shape)),
      data_type_(dtype),
      layout_(layout),
      buffer_(std::move(buffer)) {
    if (!buffer_) {
        throw std::invalid_argument("Tensor must be constructed with a valid buffer.");
    }
}

Tensor::~Tensor() = default;

DeviceType Tensor::get_device_type() const {
    return buffer_ ? buffer_->get_device_type() : DeviceType::CPU;
}

void* Tensor::get_data() const {
    if (!buffer_) {
        return nullptr;
    }
    if (!buffer_->is_mapped()) {
        // In a real engine, you might trigger a map operation here,
        // but for now, we enforce that it must already be mapped.
        throw std::runtime_error("Attempted to get data pointer from an unmapped buffer.");
    }
    return buffer_->get_mapped_ptr();
}

size_t Tensor::get_size_in_bytes() const {
    return buffer_ ? buffer_->get_size() : 0;
}

}