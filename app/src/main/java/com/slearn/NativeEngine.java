package com.slearn;

public class NativeEngine {

    // Load our compiled C++ library (`libt760_engine_native.so`)
    static {
        System.loadLibrary("t760_engine_native");
    }

    // --- JNI Method Declarations ---
    // These methods must match the function names in JniBridge.cpp

    /**
     * Initializes the C++ engine and all hardware backends.
     * @return true on success, false on failure.
     */
    public native boolean nativeInit();

    /**
     * Shuts down the C++ engine and releases all resources.
     */
    public native void nativeShutdown();

    /**
     * Loads a model from the given file path.
     * @param modelPath The absolute path to the .t760 model file.
     * @return true on success, false on failure.
     */
    public native boolean nativeLoadModel(String modelPath);

    /**
     * Unloads the currently active model.
     */
    public native void nativeUnloadModel();

    /**
     * Creates a new conversation context in the C++ engine.
     * @return A handle (long) to the new conversation, or 0 if failed.
     */
    public native long nativeStartConversation();

    /**
     * Releases the resources for a given conversation context.
     * @param handle The handle of the conversation to end.
     */
    public native void nativeEndConversation(long handle);

    /**
     * Runs inference for a given conversation with new input tokens.
     * @param handle The handle of the conversation.
     * @param tokenIds An array of new input token IDs.
     * @return An array of generated output token IDs.
     */
    public native int[] nativeGenerate(long handle, int[] tokenIds);
}