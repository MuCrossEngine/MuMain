#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// GameConfigBootstrap.h
// Loads CProtect (CGMProtect) and Configs.xtm on both platforms.
// On Windows this is done inside WinMain; on Android called from android_main.
// ─────────────────────────────────────────────────────────────────────────────

namespace GameConfigBootstrap
{
    // Load CProtect file (av-code45.pak / Configs.xtm).
    // Returns false if the file is missing or corrupt.
    bool Load();
}
