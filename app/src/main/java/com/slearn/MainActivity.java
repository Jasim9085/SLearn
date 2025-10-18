package com.slearn;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";
    private NativeEngine nativeEngine;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = findViewById(R.id.sample_text); // Assuming you have a TextView with this ID
        tv.setText("Initializing T760 Engine...");

        nativeEngine = new NativeEngine();

        // Perform engine initialization and model loading on a background thread
        // to avoid blocking the UI thread.
        new Thread(() -> {
            boolean isInit = nativeEngine.nativeInit();
            Log.d(TAG, "Engine initialization success: " + isInit);

            if (isInit) {
                runOnUiThread(() -> tv.setText("Engine Initialized. Loading model..."));

                // IMPORTANT: You must copy your model.t760 file to a location accessible
                // by the app, such as the internal storage.
                String modelPath = getFilesDir().getAbsolutePath() + "/model.t760";
                
                boolean isLoaded = nativeEngine.nativeLoadModel(modelPath);
                Log.d(TAG, "Model loading success: " + isLoaded);

                if (isLoaded) {
                    runOnUiThread(() -> tv.setText("Model Loaded. Ready for inference!"));
                } else {
                    runOnUiThread(() -> tv.setText("Model loading FAILED."));
                }
            } else {
                runOnUiThread(() -> tv.setText("Engine initialization FAILED."));
            }
        }).start();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (nativeEngine != null) {
            nativeEngine.nativeShutdown();
        }
    }
}