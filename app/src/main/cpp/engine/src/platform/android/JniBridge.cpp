#include <jni.h>
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "t760_engine/core/Engine.h"
#include "t760_engine/core/Types.h"
#include "t760_engine/tensor/Tensor.h"

static std::unique_ptr<t760::Engine> g_engine = nullptr;
static std::mutex g_engine_mutex;

extern "C" JNIEXPORT jboolean JNICALL
Java_com_slearn_NativeEngine_nativeInit(
    JNIEnv* env,
    jobject /* this */) {

    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (g_engine) {
        return JNI_TRUE;
    }
    try {
        g_engine = std::make_unique<t760::Engine>();
        t760::EngineConfig config;
        config.devices = {
            {t760::DeviceType::CPU, 0, true},
            {t760::DeviceType::GPU, 0, true},
            {t760::DeviceType::NPU, 0, true}
        };
        g_engine->initialize(config);
    } catch (const std::exception& e) {
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

extern "C" JNIEXPORT void JNICALL
Java_com_slearn_NativeEngine_nativeShutdown(
    JNIEnv* env,
    jobject /* this */) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (g_engine) {
        g_engine->shutdown();
        g_engine.reset();
    }
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_slearn_NativeEngine_nativeLoadModel(
    JNIEnv* env,
    jobject /* this */,
    jstring model_path) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (!g_engine) return JNI_FALSE;
    const char* c_model_path = env->GetStringUTFChars(model_path, nullptr);
    if (c_model_path == nullptr) return JNI_FALSE;
    std::string path_str(c_model_path);
    env->ReleaseStringUTFChars(model_path, c_model_path);
    return static_cast<jboolean>(g_engine->load_model(path_str));
}

extern "C" JNIEXPORT void JNICALL
Java_com_slearn_NativeEngine_nativeUnloadModel(
    JNIEnv* env,
    jobject /* this */) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (g_engine) g_engine->unload_model();
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_slearn_NativeEngine_nativeStartConversation(
    JNIEnv* env,
    jobject /* this */) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (!g_engine || !g_engine->is_model_loaded()) return 0;
    t760::ConversationHandle handle = g_engine->start_new_conversation();
    return static_cast<jlong>(handle.id);
}

extern "C" JNIEXPORT void JNICALL
Java_com_slearn_NativeEngine_nativeEndConversation(
    JNIEnv* env,
    jobject /* this */,
    jlong handle_id) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (g_engine) g_engine->end_conversation({static_cast<uint64_t>(handle_id)});
}

extern "C" JNIEXPORT jintArray JNICALL
Java_com_slearn_NativeEngine_nativeGenerate(
    JNIEnv* env,
    jobject /* this */,
    jlong handle_id,
    jintArray token_ids) {
    std::lock_guard<std::mutex> lock(g_engine_mutex);
    if (!g_engine || !g_engine->is_model_loaded()) return nullptr;

    jint* token_elements = env->GetIntArrayElements(token_ids, nullptr);
    if (token_elements == nullptr) return nullptr;
    jsize length = env->GetArrayLength(token_ids);
    std::vector<int> input_tokens(token_elements, token_elements + length);
    env->ReleaseIntArrayElements(token_ids, token_elements, JNI_ABORT);

    t760::ConversationHandle handle{static_cast<uint64_t>(handle_id)};
    t760::Tensor result_tensor = g_engine->generate(handle, input_tokens);

    std::vector<int> output_tokens = {1, 2, 3}; // Dummy output
    jintArray result_array = env->NewIntArray(output_tokens.size());
    if (result_array == nullptr) return nullptr;
    env->SetIntArrayRegion(result_array, 0, output_tokens.size(), output_tokens.data());
    return result_array;
}