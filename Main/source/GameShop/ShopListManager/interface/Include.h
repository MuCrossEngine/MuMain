#pragma once

#pragma warning(disable : 4995)

#include <iostream>
#ifdef __ANDROID__
#include "Platform/AndroidWin32Compat.h"
#include "Platform/WinsockAndroidCompat.h"
#else
#include <Windows.h>
#include <Wininet.h>
#include <crtdbg.h>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib, "Wininet.lib")
#endif

#include "GameShop/ShopListManager/interface/WZResult/WZResult.h"
#include "GameShop/ShopListManager/interface/DownloadInfo.h"
#include "GameShop/ShopListManager/interface/IDownloaderStateEvent.h"
