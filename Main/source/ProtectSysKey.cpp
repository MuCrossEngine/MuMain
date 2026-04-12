#ifdef __ANDROID__
#include "stdafx.h"
#include "ProtectSysKey.h"

bool ProtectSysKey::AttachProtectSysKey(HINSTANCE, HWND)
{
	return true;
}

void ProtectSysKey::DetachProtectSysKey()
{
}

#else
#include "stdafx.h"
#include "ProtectSysKey.h"

bool ProtectSysKey::AttachProtectSysKey(HINSTANCE hInst, HWND hWnd)
{
	return ProtectSysKey::CProtectSysKey::GetObjPtr()->AttachProtectSysKey(hInst, hWnd);
}

void ProtectSysKey::DetachProtectSysKey()
{
	ProtectSysKey::CProtectSysKey::GetObjPtr()->DetachProtectSysKey();
}

ProtectSysKey::CProtectSysKey::CProtectSysKey()
	: m_hKeyboardHook(NULL), m_hWnd(NULL)
{
}

ProtectSysKey::CProtectSysKey::~CProtectSysKey()
{
	DetachProtectSysKey();
}

bool ProtectSysKey::CProtectSysKey::AttachProtectSysKey(HINSTANCE hInst, HWND hWnd)
{
	if(!m_hKeyboardHook)
	{
		m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, CProtectSysKey::LowLevelKeyHookProc, hInst, 0);

		if(RegisterHotKey( hWnd, 0, MOD_ALT, VK_TAB))
			m_hWnd = hWnd;

		return true;
	}
	return false;
}

void ProtectSysKey::CProtectSysKey::DetachProtectSysKey()
{
	if(m_hKeyboardHook)
	{
		UnhookWindowsHookEx(m_hKeyboardHook);
		m_hKeyboardHook = NULL;
	}
	if(m_hWnd)
	{
		UnregisterHotKey(m_hWnd, 0);
		m_hWnd = NULL;
	}
}

HHOOK ProtectSysKey::CProtectSysKey::GetHookHandle() const
{
	return m_hKeyboardHook;
}

ProtectSysKey::CProtectSysKey* ProtectSysKey::CProtectSysKey::GetObjPtr()
{
	static CProtectSysKey s_Instance;
	return &s_Instance;
}

LRESULT CALLBACK ProtectSysKey::CProtectSysKey::LowLevelKeyHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (!nCode && wParam >= WM_KEYDOWN && wParam <= WM_KEYUP)
	{
		PKBDLLHOOKSTRUCT pKBHookStruct = (PKBDLLHOOKSTRUCT)lParam;

		if (pKBHookStruct->vkCode == VK_ESCAPE)
		{
			if (SEASON3B::IsRepeat(VK_CONTROL) == 1)
				return 1;
		}
		else if (pKBHookStruct->vkCode >= VK_LWIN && pKBHookStruct->vkCode <= VK_RWIN 
			&& !gwinhandle->CheckWndMode()
			)
		{
			return 1;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

#endif // __ANDROID__
