#include "t760_engine/model/Model.h"
#include <stdexcept>
#include <algorithm>

namespace t760 {

Model::Model(std::unique_ptr<ModelConfig> config) : config_(std::move(config)) {
    if (!config_) { throw std::invalid_argument("Model must be constructed with a valid configuration."); }
}

Model::~Model() = default;

const ModelConfig& Model::get_config() const { return *config_; }
const std::vector<Tensor*>& Model::get_tensors_by_exec_order() const { return execution_ordered_tensors_; }

Tensor* Model::get_tensor(const std::string& name) {
    auto it = tensor_map_.find(name);
    return (it != tensor_map_.end()) ? it->second : nullptr;
}

void Model::assign_tensors(std::vector<std::unique_ptr<Tensor>> tensors) {
    if (tensors.size() != config_->tensor_metadata_table.size()) { throw std::runtime_error("Tensor count mismatch."); }
    owned_tensors_ = std::move(tensors);
    tensor_map_.clear();
    execution_ordered_tensors_.clear();
    tensor_map_.reserve(owned_tensors_.size());
    execution_ordered_tensors_.reserve(owned_tensors_.size());
    for (const auto& tensor_ptr : owned_tensors_) {
        tensor_map_[tensor_ptr->get_name()] = tensor_ptr.get();
    }
    for (const auto& meta : config_->tensor_metadata_table) {
        std::string tensor_name(meta.name);
        Tensor* tensor = get_tensor(tensor_name);
        if (!tensor) { throw std::runtime_error("Tensor not found during assignment: " + tensor_name); }
        execution_ordered_tensors_.push_back(tensor);
    }
}

}