package com.mucrossengine.client;

import android.app.NativeActivity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.view.WindowManager;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

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

    // Called from C++ to fetch optional downloader base URL from launch intent.
    // Example: adb shell am start ... --es MU_ASSET_SERVER http://10.0.2.2:8080/mu_assets
    public String getAssetServerUrl() {
        Intent intent = getIntent();
        if (intent == null) return "";
        String value = intent.getStringExtra("MU_ASSET_SERVER");
        return value != null ? value : "";
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

    // Called from C++ to extract a downloaded zip package into targetDir.
    public boolean extractZipArchive(String zipPath, String targetDir) {
        if (zipPath == null || targetDir == null || zipPath.isEmpty() || targetDir.isEmpty()) {
            return false;
        }

        File zipFile = new File(zipPath);
        if (!zipFile.exists() || !zipFile.isFile()) {
            return false;
        }

        File outputRoot = new File(targetDir);
        if (!outputRoot.exists() && !outputRoot.mkdirs()) {
            return false;
        }

        final String canonicalRootPath;
        try {
            canonicalRootPath = outputRoot.getCanonicalPath();
        } catch (IOException e) {
            return false;
        }

        final String canonicalRootPrefix = canonicalRootPath.endsWith(File.separator)
            ? canonicalRootPath
            : canonicalRootPath + File.separator;

        boolean extractedAnyFile = false;
        byte[] buffer = new byte[8192];

        try (ZipInputStream zis = new ZipInputStream(new BufferedInputStream(new FileInputStream(zipFile)))) {
            ZipEntry entry;
            while ((entry = zis.getNextEntry()) != null) {
                String entryName = entry.getName();
                if (entryName == null || entryName.isEmpty()) {
                    zis.closeEntry();
                    continue;
                }

                entryName = entryName.replace('\\', '/');
                while (entryName.startsWith("/")) {
                    entryName = entryName.substring(1);
                }
                if (entryName.isEmpty()) {
                    zis.closeEntry();
                    continue;
                }

                File outFile = new File(outputRoot, entryName);
                String canonicalOutPath = outFile.getCanonicalPath();
                if (!canonicalOutPath.equals(canonicalRootPath)
                    && !canonicalOutPath.startsWith(canonicalRootPrefix)) {
                    zis.closeEntry();
                    return false;
                }

                if (entry.isDirectory()) {
                    if (!outFile.exists() && !outFile.mkdirs()) {
                        zis.closeEntry();
                        return false;
                    }
                    zis.closeEntry();
                    continue;
                }

                File parent = outFile.getParentFile();
                if (parent != null && !parent.exists() && !parent.mkdirs()) {
                    zis.closeEntry();
                    return false;
                }

                try (FileOutputStream fos = new FileOutputStream(outFile)) {
                    int count;
                    while ((count = zis.read(buffer)) > 0) {
                        fos.write(buffer, 0, count);
                    }
                    fos.flush();
                    extractedAnyFile = true;
                }
                zis.closeEntry();
            }
        } catch (IOException e) {
            return false;
        }

        return extractedAnyFile;
    }
}
