#ifndef T760_MODEL_H
#define T760_MODEL_H

#include "t760_engine/model/ModelConfig.h"
#include "t760_engine/tensor/Tensor.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace t760 {

// Represents the fully loaded model in memory, with all its tensors.
class Model {
public:
    explicit Model(std::unique_ptr<ModelConfig> config);
    ~Model();

    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    const ModelConfig& get_config() const;
    const std::vector<Tensor*>& get_tensors_by_exec_order() const;
    Tensor* get_tensor(const std::string& name);

    // This method will be called by the ModelLoader to populate the model with tensors.
    void assign_tensors(std::vector<std::unique_ptr<Tensor>> tensors);

private:
    std::unique_ptr<ModelConfig> config_;
    std::vector<std::unique_ptr<Tensor>> owned_tensors_;
    std::vector<Tensor*> execution_ordered_tensors_;
    std::unordered_map<std::string, Tensor*> tensor_map_;
};

}

#endif // T760_MODEL_H