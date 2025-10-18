package com.slearn;

import android.content.Context;
import android.os.Build;
import android.provider.Settings;
import org.json.JSONObject;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.HttpURLConnection;
import java.net.URL;
import java.nio.charset.StandardCharsets;

public class CrashHandler implements Thread.UncaughtExceptionHandler {
    private final Context context;
    private final Thread.UncaughtExceptionHandler defaultHandler;

    public CrashHandler(Context ctx) {
        this.context = ctx.getApplicationContext();
        this.defaultHandler = Thread.getDefaultUncaughtExceptionHandler();
    }

    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        try {
            log.d(throwable);
        } catch (Exception e) {
            // Failsafe if remote logging fails
        } finally {
            if (defaultHandler != null) {
                defaultHandler.uncaughtException(thread, throwable);
            }
            System.exit(2);
        }
    }
    }
}