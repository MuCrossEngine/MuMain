///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <locale.h>
#include <zmouse.h>
#include "UIWindows.h"
#include "UIManager.h"
#include "ZzzOpenglUtil.h"
#include "ZzzTexture.h"
#include "ZzzOpenData.h"
#include "ZzzScene.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "zzzLodTerrain.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "Resource.h"
#include <imm.h>
#include "zzzpath.h"
#include "Nprotect.h"
#include "Local.h"
#include "PersonalShopTitleImp.h"
#include "./Utilities/Log/ErrorReport.h"
#include "UIMapName.h"		// rozy
#include "./ExternalObject/leaf/ExceptionHandler.h"
#include "./Utilities/Dump/CrashReporter.h"
#include "./Utilities/Log/muConsoleDebug.h"
#include "ProtocolSend.h"
#include "ProtectSysKey.h"

#include "CBTMessageBox.h"
#include "./ExternalObject/leaf/regkey.h"

#include "CSChaosCastle.h"
#include "GMHellas.h"
#include <io.h>
#include "Input.h"
#include "./Time/Timer.h"
#include "UIMng.h"

#ifdef MOVIE_DIRECTSHOW
#include <dshow.h>
#include "MovieScene.h"
#endif // MOVIE_DIRECTSHOW
#include "GameCensorship.h"
#include "w_MapHeaders.h"
#include "w_PetProcess.h"
#include <ThemidaInclude.h>
#include "MultiLanguage.h"
#include "CGMProtect.h"
#include "CAIController.h"

#include "ScaleForm.h"
#include "CGMFontLayer.h"

#include <tlhelp32.h>

#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

#include <wzAudio.h>
#pragma comment(lib, "wzAudio.lib")
#ifdef IMPLEMENT_IMGUI_WIN32
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#endif // IMPLEMENT_IMGUI_WIN32
#include "ConnectVersionHex.h"
#include "CShaderGL.h"

#include "TextClien.h"


extern BOOL g_bIMEBlock;
extern DWORD g_dwTopWindow;

CUITextInputBox* g_pSingleTextInputBox = NULL;
CUITextInputBox* g_pSinglePasswdInputBox = NULL;
int g_iChatInputType = 1;

CChatRoomSocketList* g_pChatRoomSocketList = NULL;

CMultiLanguage* pMultiLanguage = NULL;


#ifdef MOVIE_DIRECTSHOW
CMovieScene* g_pMovieScene = NULL;
#endif // MOVIE_DIRECTSHOW

CUIManager* g_pUIManager = NULL;
CUIMapName* g_pUIMapName = NULL;		// rozy

int Time_Effect = 0;
bool ashies = false;
int weather = rand() % 3;
HDC       g_hDC = NULL;
HGLRC     g_hRC = NULL;
HFONT     g_hFont = NULL;
HFONT     g_hFontBold = NULL;
HFONT     g_hFontBig = NULL;
HFONT     g_hFixFont = NULL;

CTimer* g_pTimer = NULL;	// performance counter.
bool      Destroy = false;
bool      ActiveIME = false;

BYTE* RendomMemoryDump;
//CHARACTER* CharacterMemoryDump;

int       RandomTable[100];

char TextMu[] = "mu.exe";

CErrorReport g_ErrorReport;

BOOL g_bMinimizedEnabled = FALSE;
int g_iScreenSaverOldValue = 60 * 15;

extern float g_fScreenRate_x;	// ※
extern float g_fScreenRate_y;

int g_iInactiveWarning = 0;
bool HangulDelete = false;
int Hangul = 0;
bool g_bEnterPressed = false;

float g_iMousePopPosition_x = 0;
float g_iMousePopPosition_y = 0;

char m_ID[6][11];
char m_Psz[6][11];
char m_Version[11];
char m_ExeVersion[11];
int  m_SaveAccount;
int  m_SoundOnOff;
int  m_MusicOnOff;
int  m_Resolution;
int targetIndexFPS;
int	m_nColorDepth;
int	g_iRenderTextType = 0;


extern int  LogIn;
extern char LogInID[];
extern int TimeRemain;
extern bool EnableFastInput;
void MainScene(HDC hDC);
char Mp3FileName[256];
std::string g_strSelectedML = "";
char g_aszMLSelection[MAX_LANGUAGE_NAME_LENGTH] = { '\0' };


void StopMp3(char* Name, BOOL bEnforce)
{
	if (!m_MusicOnOff && !bEnforce) return;

	if (Mp3FileName[0] != NULL)
	{
		if (strcmp(Name, Mp3FileName) == 0) {
			wzAudioStop();
			Mp3FileName[0] = NULL;
		}
	}
}

void PlayMp3(char* Name, BOOL bEnforce)
{
	if (Destroy) return;
	if (!m_MusicOnOff && !bEnforce) return;

	if (strcmp(Name, Mp3FileName) == 0)
	{
		return;
	}
	else
	{
		wzAudioPlay(Name, 1);
		strcpy(Mp3FileName, Name);
	}
}

bool IsEndMp3()
{
	if (100 == wzAudioGetStreamOffsetRange())
		return true;
	return false;
}

int GetMp3PlayPosition()
{
	return wzAudioGetStreamOffsetRange();
}

void CheckHack(void)
{
	SendCheck();
}

GLvoid KillGLWindow(GLvoid)
{
	if (g_hRC)
	{
		if (!wglMakeCurrent(NULL, NULL))
		{
			g_ErrorReport.Write("GL - Release Of DC And RC Failed\r\n");
			MessageBox(NULL, "Release Of DC And RC Failed.", "Error", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(g_hRC))
		{
			g_ErrorReport.Write("GL - Release Rendering Context Failed\r\n");
			MessageBox(NULL, "Release Rendering Context Failed.", "Error", MB_OK | MB_ICONINFORMATION);
		}

		g_hRC = NULL;
	}

	if (g_hDC && !ReleaseDC(gwinhandle->GethWnd(), g_hDC))
	{
		g_ErrorReport.Write("GL - OpenGL Release Error\r\n");
		MessageBox(NULL, "OpenGL Release Error.", "Error", MB_OK | MB_ICONINFORMATION);
		g_hDC = NULL;
	}

	if (!gwinhandle->CheckWndMode())
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}
}

BOOL GetFileNameOfFilePath(char* lpszFile, char* lpszPath)
{
	if (lpszPath != NULL)
	{
		int iFind = (int)'\\';
		char* lpFound = lpszPath;
		char* lpOld = lpFound;
		while (lpFound)
		{
			lpOld = lpFound;
			lpFound = strchr(lpFound + 1, iFind);
		}

		if (strchr(lpszPath, iFind))
		{
			strcpy(lpszFile, lpOld + 1);
		}
		else
		{
			strcpy(lpszFile, lpOld);
		}

		BOOL bCheck = TRUE;
		for (char* lpTemp = lpszFile; bCheck; ++lpTemp)
		{
			switch (*lpTemp)
			{
			case '\"':
			case '\\':
			case '/':
			case ' ':
				*lpTemp = '\0';
			case '\0':
				bCheck = FALSE;
				break;
			}
		}
	}
	return (TRUE);
}

HANDLE g_hMainExe = INVALID_HANDLE_VALUE;

BOOL OpenMainExe(void)
{
#ifdef _DEBUG
	return (TRUE);
#endif
	char lpszFile[MAX_PATH];
	char* lpszCommandLine = GetCommandLine();
	GetFileNameOfFilePath(lpszFile, lpszCommandLine);

	g_hMainExe = CreateFile((char*)lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	return (INVALID_HANDLE_VALUE != g_hMainExe);
}

void CloseMainExe(void)
{
	CloseHandle(g_hMainExe);

#ifdef MAX_INSTANCE_GAME
	GMProtect->runtime_delete_mutex();
#endif // MAX_INSTANCE_GAME
}

WORD DecryptCheckSumKey(WORD wSource)
{
	WORD wAcc = wSource ^ 0xB479;
	return ((wAcc >> 10) << 4) | (wAcc & 0xF);
}

DWORD GenerateCheckSum(BYTE* pbyBuffer, DWORD dwSize, WORD wKey)
{
	DWORD dwKey = (DWORD)wKey;
	DWORD dwResult = dwKey << 9;
	for (DWORD dwChecked = 0; dwChecked <= dwSize - 4; dwChecked += 4)
	{
		DWORD dwTemp;
		memcpy(&dwTemp, pbyBuffer + dwChecked, sizeof(DWORD));

		switch ((dwChecked / 4 + wKey) % 3)
		{
		case 0:
			dwResult ^= dwTemp;
			break;
		case 1:
			dwResult += dwTemp;
			break;
		case 2:
			dwResult <<= (dwTemp % 11);
			dwResult ^= dwTemp;
			break;
		}

		if (0 == (dwChecked % 4))
		{
			dwResult ^= ((dwKey + dwResult) >> ((dwChecked / 4) % 16 + 3));
		}
	}

	return (dwResult);
}

DWORD GetCheckSum(WORD wKey)
{
	wKey = DecryptCheckSumKey(wKey);

	char lpszFile[MAX_PATH];

	strcpy(lpszFile, "data\\local\\Gameguard.csr");

	HANDLE hFile = CreateFile((char*)lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return (0);
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	BYTE* pbyBuffer = new BYTE[dwSize];
	DWORD dwNumber;
	if (ReadFile(hFile, pbyBuffer, dwSize, &dwNumber, 0))
	{
		CloseHandle(hFile);
	}

	DWORD dwCheckSum = GenerateCheckSum(pbyBuffer, dwSize, wKey);
	delete[] pbyBuffer;

	return (dwCheckSum);
}

BOOL GetFileVersion(char* lpszFileName, WORD* pwVersion)
{
	DWORD dwHandle = 1;
	DWORD dwLen = GetFileVersionInfoSize(lpszFileName, &dwHandle);

	if (dwLen == 0 || dwHandle == 0)
	{
		return (FALSE);
	}

	BYTE* pbyData = new BYTE[dwLen];

	if (GetFileVersionInfo(lpszFileName, dwHandle, dwLen, pbyData) == false)
	{
		delete[] pbyData;
		return (FALSE);
	}

	VS_FIXEDFILEINFO* pffi;
	UINT uLen;
	if (!VerQueryValue(pbyData, "\\", (LPVOID*)&pffi, &uLen))
	{
		delete[] pbyData;
		return (FALSE);
	}

	pwVersion[0] = HIWORD(pffi->dwFileVersionMS);
	pwVersion[1] = LOWORD(pffi->dwFileVersionMS);
	pwVersion[2] = HIWORD(pffi->dwFileVersionLS);
	pwVersion[3] = LOWORD(pffi->dwFileVersionLS);

	delete[] pbyData;

	return (TRUE);
}

extern PATH* path;

void DestroyWindow()
{
	//. save volume level
	leaf::CRegKey regkey;
	regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");
	regkey.WriteDword("VolumeLevel", g_pOption->GetVolumeLevel());

	CUIMng::Instance().Release();

#ifdef MOVIE_DIRECTSHOW
	if (g_pMovieScene)
	{
		g_pMovieScene->Destroy();
	}
#endif // MOVIE_DIRECTSHOW

	//. release font handle
	if (g_hFont)
		DeleteObject((HGDIOBJ)g_hFont);

	if (g_hFontBold)
		DeleteObject((HGDIOBJ)g_hFontBold);

	if (g_hFontBig)
		DeleteObject((HGDIOBJ)g_hFontBig);

	ReleaseCharacters();

	if (g_hFixFont)
		DeleteObject((HGDIOBJ)g_hFixFont);

	SAFE_DELETE_ARRAY(GateAttribute);

	SAFE_DELETE_ARRAY(SkillAttribute);

	SAFE_DELETE(CharacterMachine);

	DeleteWaterTerrain();

#ifdef MOVIE_DIRECTSHOW
	if (SceneFlag != MOVIE_SCENE)
#endif // MOVIE_DIRECTSHOW
	{
		gMapManager->DeleteObjects();

		// Object.
		for (int i = MODEL_LOGO; i < MAX_MODELS; i++)
		{
			gmClientModels->GetModel(i)->Release();
		}
		// Bitmap
		Bitmaps.UnloadAllImages();
	}

	SAFE_DELETE_ARRAY(RendomMemoryDump);

#ifdef DYNAMIC_FRUSTRUM
	DeleteAllFrustrum();
#endif //DYNAMIC_FRUSTRUM

	SAFE_DELETE(g_pSingleTextInputBox);
	SAFE_DELETE(g_pSinglePasswdInputBox);

	SAFE_DELETE(g_pChatRoomSocketList);
	SAFE_DELETE(g_pUIMapName);	// rozy
	SAFE_DELETE(g_pTimer);
	SAFE_DELETE(g_pUIManager);

#ifdef MOVIE_DIRECTSHOW
	SAFE_DELETE(g_pMovieScene);
#endif // MOVIE_DIRECTSHOW

	SAFE_DELETE(pMultiLanguage);
	BoostRest(g_BuffSystem);
	BoostRest(g_MapProcess);
	BoostRest(g_petProcess);

	g_ErrorReport.Write("Destroy");

	HWND shWnd = FindWindow(NULL, "MuPlayer");
	if (shWnd)
		SendMessage(shWnd, WM_DESTROY, 0, 0);
}

void DestroySound()
{
	for (int i = 0; i < MAX_BUFFER; i++)
		ReleaseBuffer(i);

	FreeDirectSound();
	wzAudioDestroy();
}

bool CreateOpenglWindow()
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	if (!(g_hDC = GetDC(gwinhandle->GethWnd())))
	{
		g_ErrorReport.Write("OpenGL Get DC Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Get DC Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	GLuint PixelFormat;

	if (!(PixelFormat = ChoosePixelFormat(g_hDC, &pfd)))
	{
		g_ErrorReport.Write("OpenGL Choose Pixel Format Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Choose Pixel Format Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!DescribePixelFormat(g_hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd))
	{
		g_ErrorReport.Write("OpenGL Describe Pixel Format Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Describe Pixel Format Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(g_hDC, PixelFormat, &pfd))
	{
		g_ErrorReport.Write("OpenGL Set Pixel Format Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Set Pixel Format Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(g_hRC = wglCreateContext(g_hDC)))
	{
		g_ErrorReport.Write("OpenGL Create Context Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Create Context Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(g_hDC, g_hRC))
	{
		g_ErrorReport.Write("OpenGL Make Current Error - ErrorCode : %d\r\n", GetLastError());
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Make Current Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	glewInit();

	glEnable(GL_MULTISAMPLE);

	ShowWindow(gwinhandle->GethWnd(), SW_SHOW);

	SetForegroundWindow(gwinhandle->GethWnd());

	SetFocus(gwinhandle->GethWnd());

	return true;
}

void CreateImGuiWindow()
{
#ifdef IMPLEMENT_IMGUI_WIN32
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(gwinhandle->GethWnd());

	ImGui_ImplOpenGL2_Init();
#endif // IMPLEMENT_IMGUI_WIN32
}

void DestroyImGuiWindow()
{
#ifdef IMPLEMENT_IMGUI_WIN32
	ImGui_ImplOpenGL2_Shutdown();

	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
#endif // IMPLEMENT_IMGUI_WIN32
}

BOOL OpenInitFile()
{
	char szIniFilePath[256 + 20] = "";
	char szCurrentDir[256];

	GetCurrentDirectory(256, szCurrentDir);

	strcpy(szIniFilePath, szCurrentDir);
	if (szCurrentDir[strlen(szCurrentDir) - 1] == '\\')
		strcat(szIniFilePath, "config.ini");
	else
		strcat(szIniFilePath, "\\config.ini");

	GetPrivateProfileString("LOGIN", "Version", "", m_Version, 11, szIniFilePath);

	GMProtect->HookComplete();

	char* lpszCommandLine = GetCommandLine();
	char lpszFile[MAX_PATH];
	if (GetFileNameOfFilePath(lpszFile, lpszCommandLine))
	{
		WORD wVersion[4];
		if (GetFileVersion(lpszFile, wVersion))
		{
			sprintf(m_ExeVersion, "%d.%02d", wVersion[0], wVersion[1]);
			if (wVersion[2] > 0)
			{
				char lpszMinorVersion[3] = "a";
				if (wVersion[2] > 26)
				{
					lpszMinorVersion[0] = 'A';
					lpszMinorVersion[0] += (wVersion[2] - 27);
					lpszMinorVersion[1] = '+';
				}
				else
				{
					lpszMinorVersion[0] += (wVersion[2] - 1);
				}
				strcat(m_ExeVersion, lpszMinorVersion);
			}
		}
		else
		{
			strcpy(m_ExeVersion, m_Version);
		}
	}
	else
	{
		strcpy(m_ExeVersion, m_Version);
	}

	//#ifdef _DEBUG

	m_SoundOnOff = 1;
	m_MusicOnOff = 1;
	m_Resolution = 0;
	m_nColorDepth = 0;
	targetIndexFPS = 0;

	memset(m_ID, 0, sizeof(m_ID));
	memset(m_Psz, 0, sizeof(m_Psz));

	HKEY hKey;
	DWORD dwDisp;
	DWORD dwSize;
	int bUseWindowMode = TRUE;

	//leaf::CRegKey regkey;
	//
	//regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");
	//
	//if (!regkey.ReadDword("_LiwID", dwSize))
	//{
	//	regkey.WriteDword("VolumeLevel", value);
	//}

	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
	{
#ifdef SAVE_ACCOUNT_SYSTEM
		dwSize = sizeof(m_ID);

		if (RegQueryValueEx(hKey, "_LiwID", 0, REG_NONE, (LPBYTE)m_ID, &dwSize) == ERROR_SUCCESS)
		{
			dwSize = sizeof(m_Psz);
			if (RegQueryValueEx(hKey, "_LiwKey", 0, REG_NONE, (LPBYTE)m_Psz, &dwSize) != ERROR_SUCCESS)
			{
			}
		}

		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "SaveAccount", 0, NULL, (LPBYTE)&m_SaveAccount, &dwSize) != ERROR_SUCCESS)
		{
			m_SaveAccount = true;
		}
#endif

		dwSize = 11;
		if (RegQueryValueEx(hKey, "ID", 0, NULL, (LPBYTE)m_ID[MAX_ACCOUNT_REG], &dwSize) != ERROR_SUCCESS)
		{
		}

		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "SoundOnOff", 0, NULL, (LPBYTE)&m_SoundOnOff, &dwSize) != ERROR_SUCCESS)
		{
			m_SoundOnOff = true;
		}

		if (RegQueryValueEx(hKey, "MusicOnOff", 0, NULL, (LPBYTE)&m_MusicOnOff, &dwSize) != ERROR_SUCCESS)
		{
			m_MusicOnOff = false;
		}

		if (RegQueryValueEx(hKey, "Resolution", 0, NULL, (LPBYTE)&m_Resolution, &dwSize) != ERROR_SUCCESS)
		{
			m_Resolution = 1;
		}

		if (RegQueryValueEx(hKey, "ColorDepth", 0, NULL, (LPBYTE)&m_nColorDepth, &dwSize) != ERROR_SUCCESS)
		{
			m_nColorDepth = 0;
		}

		if (RegQueryValueEx(hKey, "TextOut", 0, NULL, (LPBYTE)&g_iRenderTextType, &dwSize) != ERROR_SUCCESS)
		{
			g_iRenderTextType = 0;
		}

		if (RegQueryValueEx(hKey, "targetIndexFPS", 0, NULL, (LPBYTE)&targetIndexFPS, &dwSize) != ERROR_SUCCESS)
		{
			targetIndexFPS = 0;
		}

		g_iChatInputType = 1;

		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "WindowMode", 0, NULL, (LPBYTE)&bUseWindowMode, &dwSize) != ERROR_SUCCESS)
		{
			bUseWindowMode = TRUE;
		}

		dwSize = MAX_LANGUAGE_NAME_LENGTH;
		if (RegQueryValueEx(hKey, "LangSelection", 0, NULL, (LPBYTE)g_aszMLSelection, &dwSize) != ERROR_SUCCESS)
		{
			strcpy(g_aszMLSelection, "Eng");
		}
		g_strSelectedML = g_aszMLSelection;
	}
	RegCloseKey(hKey);

	gwinhandle->SetWndMode(bUseWindowMode);

	gwinhandle->SetDisplayIndex(m_Resolution);

	return TRUE;
}

BOOL Util_CheckOption(char* lpszCommandLine, unsigned char cOption, char* lpszString)
{
	unsigned char cComp[2];
	cComp[0] = cOption; cComp[1] = cOption;
	if (islower((int)cOption))
	{
		cComp[1] = toupper((int)cOption);
	}
	else if (isupper((int)cOption))
	{
		cComp[1] = tolower((int)cOption);
	}

	int nFind = (int)'/';
	unsigned char* lpFound = (unsigned char*)lpszCommandLine;
	while (lpFound)
	{
		lpFound = (unsigned char*)strchr((char*)(lpFound + 1), nFind);
		if (lpFound && (*(lpFound + 1) == cComp[0] || *(lpFound + 1) == cComp[1]))
		{	// 발견
			if (lpszString)
			{
				int nCount = 0;
				for (unsigned char* lpSeek = lpFound + 2; *lpSeek != ' ' && *lpSeek != '\0'; lpSeek++)
				{
					nCount++;
				}

				memcpy(lpszString, lpFound + 2, nCount);
				lpszString[nCount] = '\0';
			}
			return (TRUE);
		}
	}

	return (FALSE);
}

BOOL UpdateFile(char* lpszOld, char* lpszNew)
{
	SetFileAttributes(lpszOld, FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(lpszNew, FILE_ATTRIBUTE_NORMAL);

	DWORD dwStartTickCount = ::GetTickCount();

	while (::GetTickCount() - dwStartTickCount < 5000)
	{
		if (CopyFile(lpszOld, lpszNew, FALSE))
		{	// 성공
			DeleteFile(lpszOld);
			return (TRUE);
		}
	}
	g_ErrorReport.Write("%s to %s CopyFile Error : %d\r\n", lpszNew, lpszOld, GetLastError());
	return (FALSE);
}

extern int TimeRemain;

char g_lpszCmdURL[50];
BOOL g_bInactiveTimeChecked = FALSE;

BOOL KillExeProcess(char* lpszExe)
{
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	//  Take a snapshot of all processes in the system. 

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (FALSE);

	//  Fill in the size of the structure before using it. 

	pe32.dwSize = sizeof(PROCESSENTRY32);

	//  Walk the snapshot of the processes, and for each process, 
	//  display information. 

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (stricmp(pe32.szExeFile, lpszExe) == 0)
			{
				HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

				if (process)
				{
					TerminateProcess(process, 0);
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;    // could not walk the list of processes 

	// Do not forget to clean up the snapshot object. 

	CloseHandle(hProcessSnap);

	return bRet;
}

BOOL GetConnectServerInfo(PSTR szCmdLine, char* lpszURL, WORD* pwPort)
{
	char lpszTemp[256] = { 0, };
	if (Util_CheckOption(szCmdLine, 'y', lpszTemp))
	{
		BYTE bySuffle[] = { 0x0C, 0x07, 0x03, 0x13 };

		for (int i = 0; i < (int)strlen(lpszTemp); i++)
			lpszTemp[i] -= bySuffle[i % 4];
		strcpy(lpszURL, lpszTemp);

		if (Util_CheckOption(szCmdLine, 'z', lpszTemp))
		{
			for (int j = 0; j < (int)strlen(lpszTemp); j++)
				lpszTemp[j] -= bySuffle[j % 4];
			*pwPort = atoi(lpszTemp);
		}
		g_ErrorReport.Write("[Virtual Connection] Connect IP : %s, Port : %d\r\n", lpszURL, *pwPort);
		return (TRUE);
	}
	if (!Util_CheckOption(szCmdLine, 'u', lpszTemp))
	{
		return (FALSE);
	}
	strcpy(lpszURL, lpszTemp);
	if (!Util_CheckOption(szCmdLine, 'p', lpszTemp))
	{
		return (FALSE);
	}
	*pwPort = atoi(lpszTemp);

	return (TRUE);
}

bool ExceptionCallback(_EXCEPTION_POINTERS* pExceptionInfo)
{
	char path[MAX_PATH];
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	wsprintf(path, "%d-%d-%d_%dh%dm%ds.dmp", SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

	HANDLE file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (file != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pExceptionInfo;
		mdei.ClientPointers = FALSE; // Set to TRUE if the callback routines require full-memory minidump.

		if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, MiniDumpWithIndirectlyReferencedMemory, &mdei, 0, 0))
		{
			CloseHandle(file);
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}

	// Ensure to close the file handle even if the minidump write fails
	if (file != INVALID_HANDLE_VALUE)
	{
		CloseHandle(file);
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

void ThreadFreeVRAM(LPVOID lpThreadParameter)
{
	const DWORD sec_avg_max = 5000;

	HANDLE hThread = GetCurrentThread(); // Obtener el handle del hilo actual

	SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
	while (TRUE)
	{
		Sleep(sec_avg_max);
		SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
	}
}

void memory_const_checked(LPVOID lpThreadParameter)
{
	const DWORD sec_avg_max = 60000;

	while (TRUE)
	{
		Sleep(sec_avg_max);

		GMProtect->runtime_checked_crc32();
	}
}

//Nvidia Update
extern "C"
{
	_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}

// Force NVidia Optimus to use NVidia GPU on drivers 302 and later.
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

extern "C" __declspec(dllexport) void GetSharedValue(char* appgui, int Size)
{
	if (appgui)
	{
		strncpy(appgui, "{72494e4d-b660-4ec3-b431-f5366b514018}", Size);
	}
}

void EnablePermanentDep()
{
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");

	if (hNtdll == nullptr)
	{
		return;
	}

	lpNtSetInformationProcess NtSetInformationProcess = (lpNtSetInformationProcess)GetProcAddress(hNtdll, "NtSetInformationProcess");
	if (NtSetInformationProcess == nullptr)
	{
		return;
	}

	ULONG ExecuteFlags = 0x02;
	NTSTATUS Status = NtSetInformationProcess(NtCurrentProcess, 0x22, &ExecuteFlags, sizeof(ExecuteFlags));

	if (Status != 0)
	{
		return;
	}
}

BOOL CALLBACK ConfigDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		{
			int radioButtons[] = { IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4 };
			CheckDlgButton(hDlg, radioButtons[targetIndexFPS], BST_CHECKED);
		}
		//
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			int radioButtons[] = { IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4 };

			// Buscar el radio button marcado
			for (int i = 0; i < 4; i++)
			{
				if (IsDlgButtonChecked(hDlg, radioButtons[i]) == BST_CHECKED)
				{
					targetIndexFPS = i;
					break;
				}
			}
			leaf::CRegKey regkey;

			regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");
			regkey.WriteDword("targetIndexFPS", targetIndexFPS);

			switch (targetIndexFPS)
			{
			case 1:
				gmProtect->ajust_fps_render = 30;
				break;
			case 2:
				gmProtect->ajust_fps_render = 40;
				break;
			case 3:
				gmProtect->ajust_fps_render = 60;
				break;
			default:
				gmProtect->ajust_fps_render = 25;
				break;
			}

			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		case IDCANCEL:
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	SetProcessDPIAware();

	if (GMProtect->ReadMainFile() == false)
	{
		ExitProcess(0);
		return 0;
	}

	EnablePermanentDep();

	HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadFreeVRAM, 0, 0, 0);

	//if (GMProtect->IsWindows7OrLower() == false)
	//{
	//	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)memory_const_checked, 0, 0, 0);
	//}

	timeBeginPeriod(1);

	leaf::AttachExceptionHandler(ExceptionCallback);

	char lpszExeVersion[256] = "unknown";

	char* lpszCommandLine = GetCommandLine();
	char lpszFile[MAX_PATH];
	WORD wVersion[4] = { 0, };
	if (GetFileNameOfFilePath(lpszFile, lpszCommandLine))
	{
		if (GetFileVersion(lpszFile, wVersion))
		{
			sprintf(lpszExeVersion, "%d.%02d", wVersion[0], wVersion[1]);
			if (wVersion[2] > 0)
			{
				char lpszMinorVersion[2] = "a";
				lpszMinorVersion[0] += (wVersion[2] - 1);
				strcat(lpszExeVersion, lpszMinorVersion);
			}
		}
	}

	g_ErrorReport.Write("\r\n");
	g_ErrorReport.WriteLogBegin();
	g_ErrorReport.AddSeparator();
	g_ErrorReport.Write("Mu online %s (%s) executed. (%d.%d.%d.%d)\r\n", lpszExeVersion, "Eng", wVersion[0], wVersion[1], wVersion[2], wVersion[3]);

	g_ConsoleDebug->Write(MCD_NORMAL, "Mu Online (Version: %d.%d.%d.%d)", wVersion[0], wVersion[1], wVersion[2], wVersion[3]);

	g_ErrorReport.WriteCurrentTime();

	ER_SystemInfo si;

	ZeroMemory(&si, sizeof(ER_SystemInfo));
	GetSystemInfo(&si);

	g_ErrorReport.AddSeparator();
	g_ErrorReport.WriteSystemInfo(&si);
	g_ErrorReport.AddSeparator();

	VM_START
	WORD wPortNumber;
	if (GetConnectServerInfo(szCmdLine, g_lpszCmdURL, &wPortNumber))
	{
		szServerIpAddress = g_lpszCmdURL;
		g_ServerPort = wPortNumber;
	}
	VM_END

	if (!OpenMainExe())
	{
		return false;
	}

	VM_START
	g_SimpleModulusCS.LoadEncryptionKey("Data\\Enc1.dat");
	g_SimpleModulusSC.LoadDecryptionKey("Data\\Dec2.dat");
	VM_END

	g_ErrorReport.Write("> To read config.ini.\r\n");

	if (OpenInitFile() == FALSE)
	{
		g_ErrorReport.Write("config.ini read error\r\n");
		return false;
	}

	GMProtect->CheckPluginFile();

	pMultiLanguage = new CMultiLanguage(g_strSelectedML);

	if (gmProtect->shutdown_popup)
	{
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG3), NULL, ConfigDialogProc);
	}

	if (g_iChatInputType == 1)
		ShowCursor(FALSE);

	gwinhandle->Create(hInstance, WindowWidth, WindowHeight);

	g_ErrorReport.Write("> Start window success.\r\n");

#ifdef SHUTDOWN_SCALEFORM_INFO
	gfxinit->runtime_connection();
#endif // SHUTDOWN_SCALEFORM_INFO

	if (!CreateOpenglWindow())
	{
		return FALSE;
	}

#ifdef IMPLEMENT_IMGUI_WIN32
	CreateImGuiWindow();
#endif // IMPLEMENT_IMGUI_WIN32

	g_ErrorReport.Write("> OpenGL init success.\r\n");

	g_ErrorReport.AddSeparator();

	g_ErrorReport.WriteOpenGLInfo();

	g_ErrorReport.AddSeparator();

	g_ErrorReport.WriteSoundCardInfo();

	ShowWindow(gwinhandle->GethWnd(), nCmdShow);

	UpdateWindow(gwinhandle->GethWnd());

	//g_ErrorReport.WriteImeInfo( gwinhandle->GethWnd());
	g_ErrorReport.AddSeparator();

#ifdef V_SYNCRONIZE
	InitVSync();

	if (IsVSyncAvailable())
	{
		EnableVSync();
		//SetTargetFps(-1); // unlimited
	}
#endif // V_SINCRONIZE

#ifdef SHADER_VERSION_TEST
	gShaderGL->Init();
#endif // SHADER_VERSION_TEST

	setlocale(LC_ALL, "en_US");

	CInput::Instance().Create(gwinhandle->GethWnd(), WindowWidth, WindowHeight);

	g_pNewUISystem->Create();

	if (m_MusicOnOff)
	{
		wzAudioCreate(gwinhandle->GethWnd());
		wzAudioOption(WZAOPT_STOPBEFOREPLAY, 1);
	}

	if (m_SoundOnOff)
	{
		DWORD value;

		InitDirectSound(gwinhandle->GethWnd());

		leaf::CRegKey regkey;
		regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");

		if (!regkey.ReadDword("VolumeLevel", value))
		{
			value = 5;	//. default setting
			regkey.WriteDword("VolumeLevel", value);
		}
		if (value < 0 || value >= 10)
			value = 5;

		g_pOption->SetVolumeLevel(int(value));
		SetEffectVolumeLevel(g_pOption->GetVolumeLevel());
	}

	SetTimer(gwinhandle->GethWnd(), HACK_TIMER, 20 * 1000, NULL);

	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++)
	{
		RandomTable[i] = rand() % 360;
	}

	RendomMemoryDump = new BYTE[rand() % 100 + 1];
	GateAttribute = new GATE_ATTRIBUTE[MAX_GATES];
	SkillAttribute = new SKILL_ATTRIBUTE[MAX_SKILLS];


	static CGMCharacter gcCharacter;
	static CGMModelManager gcModel;

	CharacterMachine = new CHARACTER_MACHINE;

	memset(GateAttribute, 0, sizeof(GATE_ATTRIBUTE) * (MAX_GATES));
	memset(SkillAttribute, 0, sizeof(SKILL_ATTRIBUTE) * (MAX_SKILLS));
	memset(CharacterMachine, 0, sizeof(CHARACTER_MACHINE));

	CharacterAttribute = &CharacterMachine->Character;
	CharacterMachine->Init();

	Hero = gmCharacters->GetCharacter(0);

	if (g_iChatInputType == 1)
	{
		g_pSingleTextInputBox = new CUITextInputBox;
		g_pSinglePasswdInputBox = new CUITextInputBox;
	}

	g_pChatRoomSocketList = new CChatRoomSocketList;
	g_pUIManager = new CUIManager;
	g_pUIMapName = new CUIMapName;	// rozy
	g_pTimer = new CTimer();

#ifdef MOVIE_DIRECTSHOW
	g_pMovieScene = new CMovieScene;
#endif // MOVIE_DIRECTSHOW


	static CAIController pAIController(Hero);

	g_BuffSystem = BuffStateSystem::Make();

	g_MapProcess = MapProcess::Make();

	g_petProcess = PetProcess::Make();

	CUIMng::Instance().Create();

	if (g_iChatInputType == 1)
	{
		g_pSingleTextInputBox->Init(gwinhandle->GethWnd(), 200, 20);
		g_pSinglePasswdInputBox->Init(gwinhandle->GethWnd(), 200, 20, 9, TRUE);
		g_pSingleTextInputBox->SetState(UISTATE_HIDE);
		g_pSinglePasswdInputBox->SetState(UISTATE_HIDE);

		g_pSingleTextInputBox->SetFont(g_hFont);
		g_pSinglePasswdInputBox->SetFont(g_hFont);

		g_bIMEBlock = FALSE;
		HIMC  hIMC = ImmGetContext(gwinhandle->GethWnd());
		ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
		ImmReleaseContext(gwinhandle->GethWnd(), hIMC);
		SaveIMEStatus();
		g_bIMEBlock = TRUE;
	}

	if (!gwinhandle->CheckWndMode())
	{
		int nOldVal;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, 1, &nOldVal, 0);
		SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &g_iScreenSaverOldValue, 0);
		SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 300 * 60, NULL, 0);
	}

#ifdef SAVE_PACKET
	DeleteFile(PACKET_SAVE_FILE);
#endif




	gTextClien.Load();








	MSG msg = gwinhandle->winLoop();

	gwinhandle->Destroyer();

	if (hThread) CloseHandle(hThread);

	timeEndPeriod(1);

	return msg.wParam;
}

