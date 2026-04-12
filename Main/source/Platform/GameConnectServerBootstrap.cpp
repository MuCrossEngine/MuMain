#include "GameConnectServerBootstrap.h"
// Declared in ServerListManager.cpp / ServerGroup.cpp
extern bool LoadServerList();
namespace GameConnectServerBootstrap {
    bool Load() { return LoadServerList(); }
}
