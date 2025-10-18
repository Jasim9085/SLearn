#include "t760_engine/model/ModelLoader.h"
#include "t760_engine/model/T760FormatParser.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>

namespace t760 {

ModelLoader::ModelLoader(TensorManager& tensor_manager)
    : tensor_manager_(tensor_manager) {}

ModelLoader::~ModelLoader() {
    unload_model();
}

bool ModelLoader::is_model_loaded() const {
    return loaded_model_ != nullptr;
}

Model* ModelLoader::get_model() const {
    return loaded_model_.get();
}

void ModelLoader::unload_model() {
    if (loaded_model_) {
        loaded_model_.reset();
        std::cout << "Model unloaded successfully." << std::endl;
    }
}

bool ModelLoader::load_model(const std::string& model_path) {
    if (is_model_loaded()) {
        std::cerr << "Error: A model is already loaded. Please unload it first." << std::endl;
        return false;
    }

    try {
        std::unique_ptr<ModelConfig> config = T760FormatParser::parse_metadata(model_path);
        loaded_model_ = std::make_unique<Model>(std::move(config));
        
        std::ifstream model_file(model_path, std::ios::binary);
        if (!model_file.is_open()) {
            throw std::runtime_error("Failed to re-open model file for data loading.");
        }

        std::vector<std::unique_ptr<Tensor>> tensors;
        const auto& metadata_table = loaded_model_->get_config().tensor_metadata_table;
        tensors.reserve(metadata_table.size());

        for (const auto& meta : metadata_table) {
            TensorShape shape;
            for (const auto& dim : meta.dims) {
                if (dim > 0) shape.dims.push_back(dim);
            }

            DeviceType target_device = static_cast<DeviceType>(meta.processor_id);
            DataType data_type = static_cast<DataType>(meta.data_type);
            MemoryUsage mem_usage = MemoryUsage::HOST_VISIBLE_COHERENT;

            std::unique_ptr<Tensor> tensor = tensor_manager_.create_tensor(
                std::string(meta.name), shape, data_type, target_device, TensorLayout::DENSE, mem_usage
            );

            void* buffer_ptr = tensor->get_data();
            if (!buffer_ptr) {
                throw std::runtime_error("Failed to get mapped pointer for tensor: " + std::string(meta.name));
            }

            model_file.seekg(meta.offset);
            model_file.read(static_cast<char*>(buffer_ptr), meta.stored_size);
            if (!model_file) {
                throw std::runtime_error("Failed to read tensor data for: " + std::string(meta.name));
            }

            tensors.push_back(std::move(tensor));
        }

        loaded_model_->assign_tensors(std::move(tensors));
        std::cout << "Model loaded successfully into memory." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Failed to load model: " << e.what() << std::endl;
        loaded_model_.reset();
        return false;
    }

    return true;
}

}