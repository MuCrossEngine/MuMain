package com.mucrossengine.client;

import android.app.NativeActivity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.WindowManager;

public class MainActivity extends NativeActivity {

    static {
        System.loadLibrary("mucrossengine");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Keep screen on while game is running
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Request storage permission on Android 11+
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            if (!Environment.isExternalStorageManager()) {
                Intent intent = new Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.setData(Uri.parse("package:" + getPackageName()));
                startActivity(intent);
            }
        }

        super.onCreate(savedInstanceState);
    }

    // Called from C++ to get external files dir path
    public String getExternalFilesPath() {
        java.io.File dir = getExternalFilesDir(null);
        if (dir != null) return dir.getAbsolutePath();
        return getFilesDir().getAbsolutePath();
    }

    // Called from C++ to show soft keyboard
    public void showSoftKeyboard() {
        android.view.inputmethod.InputMethodManager imm =
            (android.view.inputmethod.InputMethodManager)
            getSystemService(android.content.Context.INPUT_METHOD_SERVICE);
        if (imm != null) {
            imm.showSoftInput(getWindow().getDecorView(),
                android.view.inputmethod.InputMethodManager.SHOW_FORCED);
        }
    }

    // Called from C++ to hide soft keyboard
    public void hideSoftKeyboard() {
        android.view.inputmethod.InputMethodManager imm =
            (android.view.inputmethod.InputMethodManager)
            getSystemService(android.content.Context.INPUT_METHOD_SERVICE);
        if (imm != null) {
            imm.hideSoftInputFromWindow(getWindow().getDecorView().getWindowToken(), 0);
        }
    }
}
