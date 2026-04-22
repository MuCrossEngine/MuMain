#ifdef __ANDROID__

#include "../stdafx.h"
#include "../ServerListManager.h"
#include "../WSclient.h"
#include "../ZzzScene.h"
#include "GameAssetPath.h"

extern BYTE Version[SIZE_PROTOCOLVERSION];
extern BYTE Serial[SIZE_PROTOCOLSERIAL + 1];

namespace
{
	char g_AndroidServerIp[32] = "74.63.218.132";
	constexpr WORD kAndroidServerPort = 44404;

	std::string ResolveAndroidDataPath(const char* windowsStylePath)
	{
		if (windowsStylePath == nullptr)
		{
			return std::string();
		}

		std::string normalized(windowsStylePath);
		for (char& ch : normalized)
		{
			if (ch == '\\')
			{
				ch = '/';
			}
		}

		return GameAssetPath::Resolve(normalized.c_str());
	}
}

bool runtime_load_protect()
{
	MAIN_INFO_ENV kernelInfo;
	memset(&kernelInfo, 0, sizeof(kernelInfo));

	const std::string connectPath = ResolveAndroidDataPath("Data\\Local\\Connect.msil");
	std::string mainInfoPath = ResolveAndroidDataPath("Data\\av-code45.pak");

	if (GMProtect->ReadMainConnect(connectPath, &kernelInfo, sizeof(MAIN_INFO_ENV)) == false)
	{
		return false;
	}

	if (GMProtect->LoadMainFileInfo(GMProtect->m_MainInfo, mainInfoPath) == false)
	{
		return false;
	}

	g_ServerPort = kAndroidServerPort;

	szServerIpAddress = g_AndroidServerIp;

	Version[0] = (kernelInfo.ClientVersion[0] + 1);
	Version[1] = (kernelInfo.ClientVersion[2] + 2);
	Version[2] = (kernelInfo.ClientVersion[3] + 3);
	Version[3] = (kernelInfo.ClientVersion[5] + 4);
	Version[4] = (kernelInfo.ClientVersion[6] + 5);

	memcpy(Serial, kernelInfo.ClientSerial, sizeof(Serial));

	if (GMProtect->m_MainInfo.SceneLogin == 0)
	{
		GMProtect->m_MainInfo.SceneLogin = 1;
	}

	return true;
}

bool LoadServerList()
{
	g_ServerListManager->LoadServerListScript();
	return true;
}

bool LoadEncryptionKeys()
{
	const bool encOk = (g_SimpleModulusCS.LoadEncryptionKey(const_cast<char*>("Data\\Enc1.dat")) == TRUE);
	const bool decOk = (g_SimpleModulusSC.LoadDecryptionKey(const_cast<char*>("Data\\Dec2.dat")) == TRUE);
	return encOk && decOk;
}

#endif // __ANDROID__
