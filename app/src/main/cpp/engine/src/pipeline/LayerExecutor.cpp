#include "t760_engine/pipeline/LayerExecutor.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <numeric>

// Required for Eigen
#include <Eigen/Dense>

// Required for ARM NEON intrinsics
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

// Required for Vulkan and Android NNAPI
#ifdef __ANDROID__
#include <vulkan/vulkan.h>
#include <android/NeuralNetworks.h>
#endif

namespace t760 {

// --- CpuLayerExecutor Implementation ---
// ... (The full CpuLayerExecutor implementation from before remains unchanged) ...
void execute_matmul_fp32_eigen(const Tensor* a, const Tensor* b, Tensor* c) {
    const auto& shape_a = a->get_shape();
    const auto& shape_b = b->get_shape();
    const int M = shape_a.dims[0];
    const int K = shape_a.dims[1];
    const int N = shape_b.dims[1];
    Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> a_map(static_cast<const float*>(a->get_data()), M, K);
    Eigen::Map<const Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> b_map(static_cast<const float*>(b->get_data()), K, N);
    Eigen::Map<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>> c_map(static_cast<float*>(c->get_data()), M, N);
    c_map.noalias() = a_map * b_map;
}
void execute_matmul_qint8_neon(const Tensor* a, const Tensor* b, Tensor* c) {
#if defined(__ARM_NEON) || defined(__ARM_NEON__)
    // NEON implementation placeholder
#else
    throw std::runtime_error("Attempted to run NEON kernel on a non-NEON platform.");
#endif
}
void CpuLayerExecutor::execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) {
    if (inputs.size() == 2 && outputs.size() == 1) { // Assume MatMul
        if (inputs[0]->get_data_type() == DataType::FP32) {
            std::cout << "Executing FP32 MatMul on CPU using Eigen..." << std::endl;
            execute_matmul_fp32_eigen(inputs[0], inputs[1], outputs[0]);
        } else if (inputs[0]->get_data_type() == DataType::QINT8) {
            std::cout << "Executing QINT8 MatMul on CPU using ARM NEON..." << std::endl;
            execute_matmul_qint8_neon(inputs[0], inputs[1], outputs[0]);
        }
    }
}


// --- GpuLayerExecutor Implementation ---
// ... (The full GpuLayerExecutor implementation from before remains unchanged) ...
struct GpuLayerExecutor::GpuImpl {
    VkDevice device = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
};
GpuLayerExecutor::GpuLayerExecutor(void* device, void* queue) { pimpl = std::make_unique<GpuImpl>(); pimpl->device = static_cast<VkDevice>(device); pimpl->queue = static_cast<VkQueue>(queue); }
GpuLayerExecutor::~GpuLayerExecutor() = default;
void GpuLayerExecutor::execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) {
    std::cout << "Executing layer on GPU using Vulkan Compute..." << std::endl;
}


// --- NpuLayerExecutor Implementation ---

// PIMPL struct to hide NDK types from the public header.
struct NpuLayerExecutor::NpuImpl {
#ifdef __ANDROID__
    ANeuralNetworksModel* nn_model = nullptr;
    ANeuralNetworksCompilation* nn_compilation = nullptr;
    ANeuralNetworksExecution* nn_execution = nullptr;
#endif
    // A real implementation would cache compilations.
};

NpuLayerExecutor::NpuLayerExecutor(void* nnapi_context) {
    pimpl = std::make_unique<NpuImpl>();
    // The nnapi_context from the DeviceManager could be used here to select
    // a specific accelerator device if needed.
}

NpuLayerExecutor::~NpuLayerExecutor() {
#ifdef __ANDROID__
    // Cleanup NNAPI objects
    if (pimpl->nn_execution) ANeuralNetworksExecution_free(pimpl->nn_execution);
    if (pimpl->nn_compilation) ANeuralNetworksCompilation_free(pimpl->nn_compilation);
    if (pimpl->nn_model) ANeuralNetworksModel_free(pimpl->nn_model);
#endif
}

// Helper to translate our engine's DataType to an NNAPI operand type.
int32_t to_nnapi_operand_type(DataType dtype, float& scale, int32_t& zero_point) {
    switch (dtype) {
        case DataType::FP32:
            return ANEURALNETWORKS_TENSOR_FLOAT32;
        case DataType::QINT8:
            // This is a simplification. A real implementation would get the
            // per-tensor quantization scale and zero_point from the model format.
            scale = 0.5f;
            zero_point = 0;
            return ANEURALNETWORKS_TENSOR_QUANT8_ASYMM;
        default:
            throw std::runtime_error("Unsupported data type for NNAPI.");
    }
}

void NpuLayerExecutor::execute(const std::vector<Tensor*>& inputs, const std::vector<Tensor*>& outputs) {
#ifdef __ANDROID__
    std::cout << "Executing layer on NPU using NNAPI..." << std::endl;

    // --- 1. Model Creation ---
    ANeuralNetworksModel* model = nullptr;
    ANeuralNetworksModel_create(&model);

    std::vector<uint32_t> input_dims;
    float scale = 0.0f;
    int32_t zero_point = 0;

    // --- Add Operands (Describe Tensors) ---
    // This is a simplified example for a Fully Connected (MatMul + Bias Add) layer.
    // Inputs: A (activation), B (weights), Bias
    // Output: C
    if (inputs.size() != 3 || outputs.size() != 1) {
        throw std::runtime_error("NNAPI executor expects 3 inputs (A, W, B) and 1 output for FC layer.");
    }
    
    // Input A
    input_dims = {(uint32_t)inputs[0]->get_shape().dims[0], (uint32_t)inputs[0]->get_shape().dims[1]};
    ANeuralNetworksOperandType input_a_type = {to_nnapi_operand_type(inputs[0]->get_data_type(), scale, zero_point), (uint32_t)input_dims.size(), input_dims.data(), scale, zero_point};
    ANeuralNetworksModel_addOperand(model, &input_a_type);

    // Input B (Weights)
    input_dims = {(uint32_t)inputs[1]->get_shape().dims[0], (uint32_t)inputs[1]->get_shape().dims[1]};
    ANeuralNetworksOperandType input_b_type = {to_nnapi_operand_type(inputs[1]->get_data_type(), scale, zero_point), (uint32_t)input_dims.size(), input_dims.data(), scale, zero_point};
    ANeuralNetworksModel_addOperand(model, &input_b_type);
    
    // Input Bias
    input_dims = {(uint32_t)inputs[2]->get_shape().dims[0]};
    ANeuralNetworksOperandType bias_type = {ANEURALNETWORKS_TENSOR_FLOAT32, (uint32_t)input_dims.size(), input_dims.data(), 0.0f, 0};
    ANeuralNetworksModel_addOperand(model, &bias_type);
    
    // Fused Activation (e.g., ReLU)
    ANeuralNetworksOperandType activation_type = {ANEURALNETWORKS_INT32, 0, nullptr, 0.f, 0};
    ANeuralNetworksModel_addOperand(model, &activation_type);
    int32_t activation_code = ANEURALNETWORKS_FUSED_NONE;
    ANeuralNetworksModel_setOperandValue(model, 3, &activation_code, sizeof(activation_code));

    // Output C
    input_dims = {(uint32_t)outputs[0]->get_shape().dims[0], (uint32_t)outputs[0]->get_shape().dims[1]};
    ANeuralNetworksOperandType output_c_type = {to_nnapi_operand_type(outputs[0]->get_data_type(), scale, zero_point), (uint32_t)input_dims.size(), input_dims.data(), scale, zero_point};
    ANeuralNetworksModel_addOperand(model, &output_c_type);

    // --- Add Operation (Describe the Math) ---
    uint32_t op_inputs[] = {0, 1, 2, 3}; // A, W, Bias, Activation
    uint32_t op_outputs[] = {4}; // C
    ANeuralNetworksModel_addOperation(model, ANEURALNETWORKS_FULLY_CONNECTED, 4, op_inputs, 1, op_outputs);
    
    ANeuralNetworksModel_identifyInputsAndOutputs(model, 1, &op_inputs[0], 1, op_outputs);
    ANeuralNetworksModel_finish(model);

    // --- 2. Compilation ---
    ANeuralNetworksCompilation* compilation = nullptr;
    ANeuralNetworksCompilation_create(model, &compilation);
    ANeuralNetworksCompilation_setPreference(compilation, ANEURALNETWORKS_PREFER_FAST_SINGLE_ANSWER);
    ANeuralNetworksCompilation_finish(compilation);

    // --- 3. Execution ---
    ANeuralNetworksExecution* execution = nullptr;
    ANeuralNetworksExecution_create(compilation, &execution);

    // Get the shared memory handles from our Buffers
    auto* input_a_buffer = static_cast<NpuMemoryHandle*>(inputs[0]->get_buffer()->get_native_handle());
    auto* input_b_buffer = static_cast<NpuMemoryHandle*>(inputs[1]->get_buffer()->get_native_handle());
    auto* bias_buffer = static_cast<NpuMemoryHandle*>(inputs[2]->get_buffer()->get_native_handle());
    auto* output_buffer = static_cast<NpuMemoryHandle*>(outputs[0]->get_buffer()->get_native_handle());
    
    ANeuralNetworksExecution_setInputFromMemory(execution, 0, nullptr, input_a_buffer->nnapi_memory, 0, inputs[0]->get_size_in_bytes());
    ANeuralNetworksExecution_setOutputFromMemory(execution, 0, nullptr, output_buffer->nnapi_memory, 0, outputs[0]->get_size_in_bytes());

    // Tell NNAPI that weights and biases are constants
    ANeuralNetworksModel_setOperandValueFromMemory(model, 1, input_b_buffer->nnapi_memory, 0, inputs[1]->get_size_in_bytes());
    ANeuralNetworksModel_setOperandValueFromMemory(model, 2, bias_buffer->nnapi_memory, 0, inputs[2]->get_size_in_bytes());

    ANeuralNetworksEvent* event = nullptr;
    ANeuralNetworksExecution_startCompute(execution, &event);
    ANeuralNetworksEvent_wait(event);

    ANeuralNetworksEvent_free(event);
    ANeuralNetworksExecution_free(execution);
    ANeuralNetworksCompilation_free(compilation);
    ANeuralNetworksModel_free(model);

#else
    throw std::runtime_error("Attempted to run NNAPI executor on a non-Android platform.");
#endif
}

} // namespace t760