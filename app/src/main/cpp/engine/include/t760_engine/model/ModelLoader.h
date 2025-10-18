#ifndef T760_MODEL_LOADER_H
#define T760_MODEL_LOADER_H

#include "t760_engine/model/Model.h"
#include "t760_engine/tensor/TensorManager.h"
#include <string>
#include <memory>

namespace t760 {

class ModelLoader {
public:
    explicit ModelLoader(TensorManager& tensor_manager);
    ~ModelLoader();

    ModelLoader(const ModelLoader&) = delete;
    ModelLoader& operator=(const ModelLoader&) = delete;

    bool load_model(const std::string& model_path);
    void unload_model();

    bool is_model_loaded() const;
    Model* get_model() const;

private:
    TensorManager& tensor_manager_;
    std::unique_ptr<Model> loaded_model_;
};

}

#endif // T760_MODEL_LOADER_H