#include "t760_engine/model/Model.h"
#include <stdexcept>
#include <algorithm>

namespace t760 {

Model::Model(std::unique_ptr<ModelConfig> config)
    : config_(std::move(config)) {
    if (!config_) {
        throw std::invalid_argument("Model must be constructed with a valid configuration.");
    }
}

Model::~Model() = default;

const ModelConfig& Model::get_config() const {
    return *config_;
}

const std::vector<Tensor*>& Model::get_tensors_by_exec_order() const {
    return execution_ordered_tensors_;
}

Tensor* Model::get_tensor(const std::string& name) {
    auto it = tensor_map_.find(name);
    if (it != tensor_map_.end()) {
        return it->second;
    }
    return nullptr;
}

void Model::assign_tensors(std::vector<std::unique_ptr<Tensor>> tensors) {
    if (tensors.size() != config_->tensor_metadata_table.size()) {
        throw std::runtime_error("Mismatch between number of tensors and model metadata.");
    }

    owned_tensors_ = std::move(tensors);
    tensor_map_.clear();
    execution_ordered_tensors_.clear();
    tensor_map_.reserve(owned_tensors_.size());
    execution_ordered_tensors_.reserve(owned_tensors_.size());

    for (const auto& tensor_ptr : owned_tensors_) {
        tensor_map_[tensor_ptr->get_name()] = tensor_ptr.get();
    }

    // This is the critical step that respects our T760-UOMF execution plan.
    // We populate the execution_ordered_tensors vector based on the sorted
    // order defined in the file's metadata table.
    for (const auto& meta : config_->tensor_metadata_table) {
        std::string tensor_name(meta.name);
        Tensor* tensor = get_tensor(tensor_name);
        if (!tensor) {
            throw std::runtime_error("Tensor '" + tensor_name + "' not found during model assignment.");
        }
        execution_ordered_tensors_.push_back(tensor);
    }
}

}```

#### **src/model/T760FormatParser.cpp**
```cpp
#include "t760_engine/model/T760FormatParser.h"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <numeric>

namespace t760 {

std::unique_ptr<ModelConfig> T760FormatParser::parse_metadata(const std::string& file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open model file: " + file_path);
    }

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto config = std::make_unique<ModelConfig>();

    // 1. Read and validate the main model header
    if (file_size < sizeof(ModelHeader)) {
        throw std::runtime_error("Model file is too small to contain a valid header.");
    }
    file.read(reinterpret_cast<char*>(&config->model_header), sizeof(ModelHeader));

    if (config->model_header.magic != 0x54373630) { // "T760"
        throw std::runtime_error("Invalid model file magic number.");
    }
    if (config->model_header.version < 3) { // Assuming v3 is the one with full exec plan
        throw std::runtime_error("Unsupported model file version.");
    }

    // 2. Read hardware and execution plan headers
    size_t expected_header_section_size = sizeof(ModelHeader) + sizeof(HardwareConfigHeader) + sizeof(ExecutionPlanHeader);
    if (file_size < expected_header_section_size) {
        throw std::runtime_error("Model file is too small for hardware/execution plan headers.");
    }
    file.read(reinterpret_cast<char*>(&config->hardware_header), sizeof(HardwareConfigHeader));
    file.read(reinterpret_cast<char*>(&config->exec_plan_header), sizeof(ExecutionPlanHeader));

    // 3. Read the tensor index table
    size_t num_tensors = config->exec_plan_header.cpu_tensors_end_idx;
    size_t tensor_table_size = num_tensors * sizeof(TensorMetadata);
    size_t expected_total_metadata_size = expected_header_section_size + tensor_table_size;

    if (file_size < expected_total_metadata_size) {
        throw std::runtime_error("Model file is too small for the tensor index table.");
    }

    config->tensor_metadata_table.resize(num_tensors);
    file.read(reinterpret_cast<char*>(config->tensor_metadata_table.data()), tensor_table_size);

    // 4. Basic validation of the execution plan indices
    if (config->exec_plan_header.npu_tensors_end_idx > num_tensors ||
        config->exec_plan_header.gpu_tensors_end_idx > num_tensors ||
        config->exec_plan_header.npu_tensors_end_idx < config->exec_plan_header.npu_tensors_start_idx ||
        config->exec_plan_header.gpu_tensors_end_idx < config->exec_plan_header.gpu_tensors_start_idx ||
        config->exec_plan_header.cpu_tensors_end_idx < config->exec_plan_header.cpu_tensors_start_idx) {
        throw std::runtime_error("Invalid execution plan indices in model file header.");
    }

    return config;
}

}