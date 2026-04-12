#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// GameDownloader.h
// First-run game data downloader.
// Checks if game data is present in getExternalFilesDir(); if not, downloads
// it from the asset server, verifies CRC32, and extracts it.
// Renders a progress screen via GLES3 during download.
// ─────────────────────────────────────────────────────────────────────────────

class AndroidEglWindow;

namespace GameDownloader
{
    // Returns true if all required game data files are present and valid.
    bool IsDataReady();

    // Downloads missing files and shows progress screen.
    // eglWindow is used to render the progress UI during download.
    // Returns false if download failed.
    bool DownloadAll(AndroidEglWindow* eglWindow);

    // Base URL for the asset server (set via environment or config)
    void SetServerURL(const char* url);
    const char* GetServerURL();
}
