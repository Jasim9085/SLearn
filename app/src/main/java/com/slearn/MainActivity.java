package com.slearn;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.ScrollView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    private NativeEngine nativeEngine;
    private long conversationHandle = 0;

    // UI Elements
    private TextView logTextView;
    private EditText promptEditText;
    private Button sendButton;
    private ScrollView scrollView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // --- Initialize UI Elements ---
        logTextView = findViewById(R.id.logTextView);
        promptEditText = findViewById(R.id.promptEditText);
        sendButton = findViewById(R.id.sendButton);
        scrollView = findViewById(R.id.scrollView);

        // Initially, disable the input until the model is loaded.
        promptEditText.setEnabled(false);
        sendButton.setEnabled(false);

        // --- Setup Engine and Button Listener ---
        nativeEngine = new NativeEngine();
        setupSendButtonListener();

        // Start the engine initialization on a background thread.
        initializeEngine();
    }

    private void initializeEngine() {
        logToScreen("Initializing T760 Engine...");
        new Thread(() -> {
            boolean isInit = nativeEngine.nativeInit();
            Log.d(TAG, "Engine initialization success: " + isInit);

            if (isInit) {
                logToScreen("Engine Initialized. Loading model...");

                // IMPORTANT: You must copy your model.t760 file to a location accessible
                // by the app, such as the internal storage, for this to work.
                String modelPath = getFilesDir().getAbsolutePath() + "/model.t760";
                
                boolean isLoaded = nativeEngine.nativeLoadModel(modelPath);
                Log.d(TAG, "Model loading success: " + isLoaded);

                if (isLoaded) {
                    conversationHandle = nativeEngine.nativeStartConversation();
                    logToScreen("Model Loaded. Ready for inference!");
                    runOnUiThread(() -> {
                        promptEditText.setEnabled(true);
                        sendButton.setEnabled(true);
                    });
                } else {
                    logToScreen("ERROR: Model loading FAILED.");
                }
            } else {
                logToScreen("FATAL: Engine initialization FAILED.");
            }
        }).start();
    }

    private void setupSendButtonListener() {
        sendButton.setOnClickListener(v -> {
            String prompt = promptEditText.getText().toString();
            if (prompt.isEmpty() || conversationHandle == 0) {
                return;
            }

            logToScreen("\n\n> " + prompt);
            promptEditText.setText(""); // Clear the input field

            // This is a placeholder for your actual tokenizer.
            // You would replace this with a call to your tokenization logic.
            int[] tokenIds = {1, 2, 3}; // Dummy token IDs from prompt

            // Run inference on a background thread.
            new Thread(() -> {
                int[] resultTokenIds = nativeEngine.nativeGenerate(conversationHandle, tokenIds);
                
                // This is a placeholder for de-tokenization.
                // You would convert the resultTokenIds back into a string.
                String resultText = "This is a generated response."; // Dummy response
                
                logToScreen("\n" + resultText);
            }).start();
        });
    }

    private void logToScreen(final String message) {
        runOnUiThread(() -> {
            logTextView.append(message + "\n");
            // Auto-scroll to the bottom
            scrollView.post(() -> scrollView.fullScroll(View.FOCUS_DOWN));
        });
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (nativeEngine != null) {
            if (conversationHandle != 0) {
                nativeEngine.nativeEndConversation(conversationHandle);
            }
            nativeEngine.nativeShutdown();
        }
    }
}
