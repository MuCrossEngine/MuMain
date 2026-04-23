package com.mucrossengine.client;

import android.app.NativeActivity;
import android.content.Intent;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.util.Log;
import android.text.Editable;
import android.text.InputType;
import android.text.TextWatcher;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.FrameLayout;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.util.Locale;
import java.io.ByteArrayOutputStream;
import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class MainActivity extends NativeActivity {

    private static final String TAG = "MUAssetsJava";

    private volatile int lastHttpStatusCode = 0;
    private EditText imeBridgeEditText;
    private boolean suppressImeCallback = false;

    private native void nativeOnCommitText(String text);
    private native void nativeOnBackspace(int count);

    static {
        System.setProperty("http.keepAlive", "false");
        System.setProperty("http.maxConnections", "1");
        System.loadLibrary("mucrossengine");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Keep screen on while game is running
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        super.onCreate(savedInstanceState);
        setupImeBridge();
    }

    private void setupImeBridge() {
        FrameLayout contentRoot = findViewById(android.R.id.content);
        if (contentRoot == null) {
            return;
        }

        imeBridgeEditText = new EditText(this);
        imeBridgeEditText.setSingleLine(true);
        imeBridgeEditText.setInputType(InputType.TYPE_CLASS_TEXT
            | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS
            | InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD);
        imeBridgeEditText.setImeOptions(imeBridgeEditText.getImeOptions() | 0x10000000); // IME_FLAG_NO_EXTRACT_UI
        imeBridgeEditText.setAlpha(0.0f);
        imeBridgeEditText.setCursorVisible(false);

        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(1, 1);
        imeBridgeEditText.setLayoutParams(params);

        imeBridgeEditText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
                if (suppressImeCallback) {
                    return;
                }

                if (before > 0 && count == 0) {
                    nativeOnBackspace(before);
                    return;
                }

                if (count > 0 && s != null) {
                    int end = Math.min(start + count, s.length());
                    if (start >= 0 && start < end) {
                        CharSequence committed = s.subSequence(start, end);
                        nativeOnCommitText(committed.toString());
                    }
                }
            }

            @Override
            public void afterTextChanged(Editable s) {
            }
        });

        imeBridgeEditText.setOnKeyListener((view, keyCode, event) -> {
            if (event.getAction() == KeyEvent.ACTION_DOWN && keyCode == KeyEvent.KEYCODE_DEL) {
                nativeOnBackspace(1);
                return true;
            }
            return false;
        });

        contentRoot.addView(imeBridgeEditText);
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

    // Called from C++ HTTP compat layer. Returns response bytes for 2xx.
    public byte[] httpGetBytes(String url) {
        lastHttpStatusCode = 0;
        if (url == null || url.isEmpty()) {
            return null;
        }

        HttpURLConnection connection = null;
        try {
            URL target = new URL(url);
            connection = (HttpURLConnection) target.openConnection();
            connection.setInstanceFollowRedirects(true);
            connection.setRequestMethod("GET");
            connection.setConnectTimeout(15000);
            connection.setReadTimeout(120000);
            connection.setUseCaches(false);
            connection.setRequestProperty("Connection", "close");
            connection.setRequestProperty("Accept-Encoding", "identity");
            connection.setRequestProperty("User-Agent", "MuAndroid/1.0");
            connection.setDoInput(true);

            int status = connection.getResponseCode();
            lastHttpStatusCode = status;

            InputStream input = (status >= 200 && status < 300)
                ? connection.getInputStream()
                : connection.getErrorStream();
            if (input == null) {
                return null;
            }

            int contentLength = connection.getContentLength();
            ByteArrayOutputStream output = new ByteArrayOutputStream(Math.max(contentLength, 8192));
            byte[] buffer = new byte[8192];
            int read;
            while ((read = input.read(buffer)) != -1) {
                output.write(buffer, 0, read);
            }
            input.close();

            if (status >= 200 && status < 300) {
                return output.toByteArray();
            }
            return null;
        } catch (IOException exception) {
            Log.e(TAG, "httpGetBytes failed for " + url + ": " + exception.getMessage());
            return null;
        } finally {
            if (connection != null) {
                connection.disconnect();
            }
        }
    }

    public int getLastHttpStatusCode() {
        return lastHttpStatusCode;
    }

    // Called from C++ to show soft keyboard
    public void showSoftKeyboard() {
        runOnUiThread(() -> {
            android.view.inputmethod.InputMethodManager imm =
                (android.view.inputmethod.InputMethodManager)
                getSystemService(android.content.Context.INPUT_METHOD_SERVICE);
            if (imm == null) {
                return;
            }

            if (imeBridgeEditText != null) {
                suppressImeCallback = true;
                imeBridgeEditText.setText("");
                imeBridgeEditText.setSelection(0);
                suppressImeCallback = false;
                imeBridgeEditText.requestFocus();
                imm.showSoftInput(imeBridgeEditText,
                    android.view.inputmethod.InputMethodManager.SHOW_IMPLICIT);
            } else {
                imm.showSoftInput(getWindow().getDecorView(),
                    android.view.inputmethod.InputMethodManager.SHOW_IMPLICIT);
            }
        });
    }

    // Called from C++ to get a deterministic device HWID in the same 8-8-8-8 format as PC.
    public String getAndroidHardwareId() {
        try {
            final String androidId = Settings.Secure.getString(getContentResolver(), Settings.Secure.ANDROID_ID);
            final String seed =
                (androidId != null ? androidId : "") + "|" +
                (Build.BOARD != null ? Build.BOARD : "") + "|" +
                (Build.BRAND != null ? Build.BRAND : "") + "|" +
                (Build.DEVICE != null ? Build.DEVICE : "") + "|" +
                (Build.MANUFACTURER != null ? Build.MANUFACTURER : "") + "|" +
                (Build.MODEL != null ? Build.MODEL : "") + "|" +
                (Build.FINGERPRINT != null ? Build.FINGERPRINT : "");

            final MessageDigest digest = MessageDigest.getInstance("SHA-256");
            final byte[] hash = digest.digest(seed.getBytes(StandardCharsets.UTF_8));
            if (hash == null || hash.length < 16) {
                return "ANDROID-00000000-00000000-00000000";
            }

            final int part1 = readIntBigEndian(hash, 0);
            final int part2 = readIntBigEndian(hash, 4);
            final int part3 = readIntBigEndian(hash, 8);
            final int part4 = readIntBigEndian(hash, 12);

            return String.format(Locale.US, "%08X-%08X-%08X-%08X", part1, part2, part3, part4);
        } catch (Exception exception) {
            Log.w(TAG, "getAndroidHardwareId failed: " + exception.getMessage());
            return "ANDROID-00000000-00000000-00000000";
        }
    }

    private static int readIntBigEndian(byte[] data, int offset) {
        return ((data[offset] & 0xFF) << 24)
            | ((data[offset + 1] & 0xFF) << 16)
            | ((data[offset + 2] & 0xFF) << 8)
            | (data[offset + 3] & 0xFF);
    }

    // Called from C++ to hide soft keyboard
    public void hideSoftKeyboard() {
        runOnUiThread(() -> {
            android.view.inputmethod.InputMethodManager imm =
                (android.view.inputmethod.InputMethodManager)
                getSystemService(android.content.Context.INPUT_METHOD_SERVICE);
            if (imm == null) {
                return;
            }

            if (imeBridgeEditText != null) {
                imm.hideSoftInputFromWindow(imeBridgeEditText.getWindowToken(), 0);
                imeBridgeEditText.clearFocus();
            } else {
                imm.hideSoftInputFromWindow(getWindow().getDecorView().getWindowToken(), 0);
            }
        });
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

    public boolean copyAssetDirectoryToPath(String assetDir, String targetDir) {
        if (assetDir == null || targetDir == null || assetDir.isEmpty() || targetDir.isEmpty()) {
            return false;
        }

        File destination = new File(targetDir);
        if (!destination.exists() && !destination.mkdirs()) {
            return false;
        }

        AssetManager assetManager = getAssets();
        return copyAssetEntryRecursive(assetManager, assetDir, destination);
    }

    private boolean copyAssetEntryRecursive(AssetManager assetManager, String assetPath, File destination) {
        try {
            String[] children = assetManager.list(assetPath);
            if (children != null && children.length > 0) {
                if (!destination.exists() && !destination.mkdirs()) {
                    return false;
                }

                for (String child : children) {
                    String childAssetPath = assetPath + "/" + child;
                    File childDestination = new File(destination, child);
                    if (!copyAssetEntryRecursive(assetManager, childAssetPath, childDestination)) {
                        return false;
                    }
                }
                return true;
            }

            File parent = destination.getParentFile();
            if (parent != null && !parent.exists() && !parent.mkdirs()) {
                return false;
            }

            try (InputStream input = assetManager.open(assetPath);
                 FileOutputStream output = new FileOutputStream(destination)) {
                byte[] buffer = new byte[8192];
                int read;
                while ((read = input.read(buffer)) != -1) {
                    output.write(buffer, 0, read);
                }
                output.flush();
            }
            return true;
        } catch (IOException exception) {
            Log.e(TAG, "copyAssetEntryRecursive failed for " + assetPath + ": " + exception.getMessage());
            return false;
        }
    }
}
