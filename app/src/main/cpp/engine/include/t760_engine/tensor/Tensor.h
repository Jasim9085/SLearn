#ifndef T760_TENSOR_H
#define T760_TENSOR_H

#include "t760_engine/core/Types.h"
#include "t760_engine/tensor/TensorTypes.h"
#include "t760_engine/memory/MemoryTypes.h"
#include <memory>
#include <string>

namespace t760 {

class Tensor {
public:
    Tensor(const std::string& name, TensorShape shape, DataType dtype,
           TensorLayout layout, std::unique_ptr<Buffer> buffer);
    ~Tensor();

    Tensor(const Tensor&) = delete;
    Tensor& operator=(const Tensor&) = delete;
    Tensor(Tensor&&) = default;
    Tensor& operator=(Tensor&&) = default;

    const std::string& get_name() const { return name_; }
    const TensorShape& get_shape() const { return shape_; }
    DataType get_data_type() const { return data_type_; }
    TensorLayout get_layout() const { return layout_; }
    DeviceType get_device_type() const;
    Buffer* get_buffer() const { return buffer_.get(); }

    void* get_data() const; // Convenience method to get mapped pointer
    size_t get_size_in_bytes() const;

private:
    std::string name_;
    TensorShape shape_;
    DataType data_type_;
    TensorLayout layout_;
    std::unique_ptr<Buffer> buffer_;
};

}

#endif // T760_TENSOR_H