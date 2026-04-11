#include "stdafx.h"
#include "UIMng.h"
#include "UIControls.h"
#include "ConnectVersionHex.h"

ConnectVersionHex::ConnectVersionHex()
{
	ClientVersion = 0;
	OpenLauncher = false;
	CheckUpdateVersion = true;

	ChunkSize = 0;
	TotalSize = 0;
	Percentage = 0;
	FileVersion = 0;
}

ConnectVersionHex::~ConnectVersionHex()
{
}

ConnectVersionHex* ConnectVersionHex::Instance()
{
	static ConnectVersionHex sInstance;
	return &sInstance;
}

void ConnectVersionHex::Initialize()
{
	ClientVersion = GetPrivateProfileInt("LAUNCHER_INFO", "version", 0, GetExecutablePath("Data\\Local\\NetModule.MSIL").c_str());
}

void ConnectVersionHex::OpenUpdater()
{
	if (OpenLauncher)
	{
		// Ejecutar la nueva aplicación sin esperar a que termine
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		if (CreateProcess(
			_T(".\\Updater.exe"), // Nombre del archivo ejecutable
			NULL,              // Argumentos de línea de comandos
			NULL,              // Seguridad del proceso
			NULL,              // Seguridad del hilo
			FALSE,             // Heredar manejadores
			0,                 // Crear flags
			NULL,              // Variables de entorno
			NULL,              // Directorio de trabajo
			&si,               // Información de inicio
			&pi                // Información del proceso
		))
		{
			// Cerrar los manejadores del proceso y del hilo para evitar fugas de recursos
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		else
		{
			// Manejar el error en caso de fallo en CreateProcess
			_tprintf(_T("Error al crear el proceso: %d\n"), GetLastError());
		}
	}
}

void ConnectVersionHex::execute_launcher()
{
	std::string version_str = std::to_string(ClientVersion);

	WritePrivateProfileString("LAUNCHER_INFO", "version", version_str.c_str(), GetExecutablePath("Data\\Local\\NetModule.MSIL").c_str());

	OpenLauncher = true;

	gwinhandle->SendPostMessage(WM_DESTROY, 0, 0);
}

void ConnectVersionHex::SetClientVersion(int version)
{
	ClientVersion = version;
}

int ConnectVersionHex::GetClientVersion()
{
	return ClientVersion;
}

bool ConnectVersionHex::GetUpdateVersion()
{
	return CheckUpdateVersion;
}

void ConnectVersionHex::SetUpdateVersion(bool bEnable)
{
	CheckUpdateVersion = bEnable;
}

void ConnectVersionHex::write_received_file()
{
	RemoveDirectory(".\\.temp");

	if (CreateDirectory(".temp", NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		SetFileAttributes(".temp", FILE_ATTRIBUTE_HIDDEN);
		//SetFileAttributes(".temp", FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	}
}

void ConnectVersionHex::write_data_current(WORD fileVersion, DWORD offset, DWORD chunkSize, DWORD totalSize, BYTE* data)
{
	std::string fileName = ".temp\\update" + std::to_string(fileVersion) + ".pack";

	HANDLE file = CreateFile(fileName.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
	{
		return;
	}

	DWORD written;
	BOOL result = WriteFile(file, data, chunkSize, &written, NULL);

	CloseHandle(file);
}

void ConnectVersionHex::data_end()
{
	if (CheckUpdateVersion)
	{
		char pszText[MAX_PATH];

		float total = (Percentage * 100.0f);

		sprintf_s(pszText, "Downloading %.2f%% - Version %d ", total, FileVersion);

		g_pRenderText->SetFont(g_hFontBold);

		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(CLRDW_YELLOW);

		g_pRenderText->RenderText(0, GetWindowsY - 20, pszText, GetWindowsX, 0, RT3_SORT_CENTER);
	}
}

void ConnectVersionHex::RenderInterface(HDC hDC)
{
	CUIMng& rUIMng = CUIMng::Instance();

	EnableAlphaTest();

	DWORD dwNow = (Percentage * 10.0f);

	rUIMng.RenderTitleSceneUI(hDC, dwNow, 10);


}

void ConnectVersionHex::download_information(WORD fileVersion, DWORD chunkSize, DWORD totalSize, float bProgress)
{
	ChunkSize = chunkSize;
	TotalSize = totalSize;
	Percentage = bProgress;
	FileVersion = fileVersion;
}
