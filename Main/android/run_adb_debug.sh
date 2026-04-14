#!/bin/bash
# ─────────────────────────────────────────────────────────────────────────────
# run_adb_debug.sh — Build, install and launch MU Cross Engine on Android
# Usage:
#   ./run_adb_debug.sh              # normal build + install + launch
#   ./run_adb_debug.sh --clean      # clean build first
# ─────────────────────────────────────────────────────────────────────────────
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PACKAGE="com.mucrossengine.client"
ACTIVITY=".MainActivity"

cd "$SCRIPT_DIR"

if [ -x "./gradlew" ]; then
    GRADLE_CMD="./gradlew"
else
    GRADLE_CMD="gradle"
fi

if [ "$1" == "--clean" ]; then
    echo "[MU] Clean build..."
    $GRADLE_CMD clean
fi

echo "[MU] Building..."
$GRADLE_CMD assembleDebug

APK="$SCRIPT_DIR/app/build/outputs/apk/debug/app-debug.apk"

echo "[MU] Installing $APK..."
adb install -r "$APK"

echo "[MU] Launching..."
if [ -n "${MU_ASSET_SERVER:-}" ]; then
    echo "[MU] Using asset server override: $MU_ASSET_SERVER"
    adb shell am start -n "$PACKAGE/$PACKAGE$ACTIVITY" --es MU_ASSET_SERVER "$MU_ASSET_SERVER"
else
    adb shell am start -n "$PACKAGE/$PACKAGE$ACTIVITY"
fi

echo "[MU] Streaming logcat (Ctrl+C to stop)..."
adb logcat -c
adb logcat -s \
    MUAndroid:V \
    MURender:V \
    MUAudio:V \
    MUNetwork:V \
    MUAssets:V \
    AndroidRuntime:E
