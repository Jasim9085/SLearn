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
    private GenerationController generationController;
    private long conversationHandle = 0;

    private TextView logTextView;
    private EditText promptEditText;
    private Button sendButton;
    private ScrollView scrollView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        logTextView = findViewById(R.id.logTextView);
        promptEditText = findViewById(R.id.promptEditText);
        sendButton = findViewById(R.id.sendButton);
        scrollView = findViewById(R.id.scrollView);

        promptEditText.setEnabled(false);
        sendButton.setEnabled(false);

        nativeEngine = new NativeEngine();
        setupSendButtonListener();
        initializeSystem();
    }

    private void initializeSystem() {
        logToScreen("Initializing System...");
        new Thread(() -> {
            try {
                // 1. Initialize Tokenizer (must be done first)
                // IMPORTANT: You MUST add 'vocab.json' to your app's 'src/main/assets' folder.
                generationController = new GenerationController(getApplicationContext());
                logToScreen("Tokenizer loaded.");

                // 2. Initialize C++ Engine
                boolean isInit = nativeEngine.nativeInit();
                if (!isInit) {
                    logToScreen("FATAL: Engine initialization FAILED.");
                    return;
                }
                logToScreen("Engine Initialized. Loading model...");

                // 3. Load Model
                String modelPath = getFilesDir().getAbsolutePath() + "/model.t760";
                boolean isLoaded = nativeEngine.nativeLoadModel(modelPath);

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
            } catch (Exception e) {
                Log.e(TAG, "Initialization failed", e);
                logToScreen("FATAL: Initialization failed: " + e.getMessage());
            }
        }).start();
    }

    private void setupSendButtonListener() {
        sendButton.setOnClickListener(v -> {
            String prompt = promptEditText.getText().toString();
            if (prompt.isEmpty() || conversationHandle == 0 || generationController == null) {
                return;
            }

            logToScreen("\n\n> " + prompt);
            promptEditText.setText("");
            promptEditText.setEnabled(false); // Disable input during generation
            sendButton.setEnabled(false);

            new Thread(() -> {
                // 1. Tokenize the user's prompt using our Java controller.
                int[] tokenIds = generationController.tokenize(prompt);

                // 2. Start the generation loop, which is managed by the Java controller.
                // The controller will call the C++ engine repeatedly.
                generationController.generate(
                    nativeEngine,
                    conversationHandle,
                    tokenIds,
                    100, // Max new tokens to generate
                    this::streamToScreen // Callback function to handle each generated token
                );
                
                // Re-enable UI after generation is complete
                runOnUiThread(() -> {
                    promptEditText.setEnabled(true);
                    sendButton.setEnabled(true);
                });
            }).start();
        });
    }

    // This method is called by the controller for each new token.
    private void streamToScreen(final String token) {
        runOnUiThread(() -> {
            logTextView.append(token.replace(" ", " ")); // Post-process token string if needed
            scrollView.post(() -> scrollView.fullScroll(View.FOCUS_DOWN));
        });
    }

    private void logToScreen(final String message) {
        runOnUiThread(() -> {
            logTextView.append(message + "\n");
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
