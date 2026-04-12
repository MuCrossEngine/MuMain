#pragma once
namespace GamePacketCryptoBootstrap
{
    // Load Enc1.dat / Dec2.dat encryption keys.
    // Must be called after GameConfigBootstrap::Load().
    bool Load();
}
