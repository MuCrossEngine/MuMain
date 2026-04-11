#pragma once
typedef bool					mu_boolean;
typedef signed char				mu_int8;
typedef signed short			mu_int16;
typedef signed int				mu_int32;
typedef signed long long		mu_int64;
typedef unsigned char			mu_uint8;
typedef unsigned short			mu_uint16;
typedef unsigned int			mu_uint32;
typedef unsigned long long		mu_uint64;
typedef float					mu_float;
typedef double					mu_double;

#define TRAY_ID_ICON			(WM_USER + 100)
#define TRAY_ID_MESSAGE			(WM_USER + 101)

//#define 
#include "Resolutions.h"

class CChatRoomSocketList;

class CWINHANDLE
{
public:
	CWINHANDLE();
	virtual~CWINHANDLE();
	void Release();
	HWND Create(HINSTANCE hCurrentInst, mu_uint32 RenderSizeX, mu_uint32 RenderSizeY);
	void Destroyer();
	void InitSize(mu_uint32 RenderSizeX, mu_uint32 RenderSizeY);
	void SetFontSize(mu_uint32 FontSize);
	void Resize(mu_uint32 RenderSizeX, mu_uint32 RenderSizeY);
	MSG winLoop();

	HWND GethWnd();
	void SetInstance(HINSTANCE hInst);
	HINSTANCE GetInstance();
	void SetWndActive(mu_boolean bActive);
	mu_boolean CheckWndActive();
	void UpdateWndActive();
	void SetWndMode(mu_boolean bActive);
	mu_boolean CheckWndMode();
	void SetDisplayIndex(mu_uint8 index, mu_boolean Fontload = true);
	mu_uint8 GetDisplayIndex();
	mu_uint8 GetDisplayIndex(const std::string text_name);
	mu_float GetScreenX();
	mu_float GetScreenY();
	ResolutionConfig* LoadCurrentConfig();
	void Check_State();
	void Change_State(mu_boolean bActive);
	mu_boolean CheckWndIconic();
	mu_boolean CheckPerformance();
	void SendWindowMessage(const char* format, bool destroyAfter, ...);
	void SendNowMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	void SendPostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	static CWINHANDLE* Instance();
	static LONG FAR PASCAL WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	HWND hWnd;
	HINSTANCE hInstance;
	NOTIFYICONDATA nid;
	mu_uint8 wndIndex;
	mu_boolean WndMode;
	mu_boolean WndActive;
	mu_boolean WndIconic;
	mu_float iWinWidth;
	mu_float iWinHight;
	CSettings res_setting;
};

extern int g_iInactiveWarning;
extern BOOL g_bMinimizedEnabled;
extern float g_iMousePopPosition_x;
extern float g_iMousePopPosition_y;
extern CChatRoomSocketList* g_pChatRoomSocketList;

#ifdef IMPLEMENT_IMGUI_WIN32
extern void DestroyImGuiWindow();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // IMPLEMENT_IMGUI_WIN32

#define gwinhandle			(CWINHANDLE::Instance())
