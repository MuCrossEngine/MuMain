//*****************************************************************************
// File: LoginWin.cpp
//*****************************************************************************

#include "stdafx.h"
#include "LoginWin.h"
#include "Input.h"
#include "UIMng.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "UIControls.h"
#include "ZzzScene.h"
#include "wsclientinline.h"
#include "DSPlaySound.h"
#include "./Utilities/Log/muConsoleDebug.h"
#include "ProtocolSend.h"
#include "ServerListManager.h"
#include "./ExternalObject/leaf/regkey.h"
#include "CB_DangKyInGame.h"
#ifdef __ANDROID__
#include <android/log.h>
#define MU_FLOW_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MUAndroidFlow", __VA_ARGS__)
#endif

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern int g_iChatInputType;

CLoginWin::CLoginWin()
{
	m_pIDInputBox = NULL;
	m_pPassInputBox = NULL;

	FirstLoad = TRUE;

#ifdef SAVE_ACCOUNT_SYSTEM
#if MAIN_UPDATE > 303
	IsLoginTick = FALSE;
#endif
	_RenderFrameX = 0;
	_RenderFrameY = 0;
#endif // SAVE_ACCOUNT_SYSTEM

}

CLoginWin::~CLoginWin()
{
	SAFE_DELETE(m_pIDInputBox);
	SAFE_DELETE(m_pPassInputBox);
#ifdef SAVE_ACCOUNT_SYSTEM
	stAccountMacro.clear();
#endif // SAVE_ACCOUNT_SYSTEM
}

void CLoginWin::Create()
{
	CWin::Create(329, 245, BITMAP_LOG_IN + 7);

	m_asprInputBox[LIW_ACCOUNT].Create(156, 23, BITMAP_LOG_IN + 8);
	m_asprInputBox[LIW_PASSWORD].Create(156, 23, BITMAP_LOG_IN + 8);

	for (int i = 0; i < 2; ++i)
	{
		m_aBtn[i].Create(54, 30, BITMAP_BUTTON + i, 3, 2, 1);
		CWin::RegisterButton(&m_aBtn[i]);
	}
#if(CB_DANGKYINGAME)
	//=====
	m_DangKy.Create(54, 30, Bitmap_button_register, 3, 2, 1);
	CWin::RegisterButton(&m_DangKy);
#endif
#if MAIN_UPDATE > 303
#ifdef SAVE_ACCOUNT_SYSTEM
	if (gmProtect->remember_account)
	{
		m_aBtn[LIW_BTN].Create(15, 13, newui_ctskilldown, 3, 2, 1);
		m_aBtn[LIW_SAVE].Create(16, 16, BITMAP_CHECK_BTN, 2, 0, 0, -1, 1, 1, 1);

		CWin::RegisterButton(&m_aBtn[LIW_BTN]);
		CWin::RegisterButton(&m_aBtn[LIW_SAVE]);

		for (int i = 0; i < MAX_ACCOUNT_REG; i++)
		{
			m_BtnRemove[i].Create(11, 11, newui_party_x, 3, 2, 1);

			m_BtnaLogin[i].Create(116, 20, BITMAP_LOG_IN + 15, 3, 2, 1);

			CWin::RegisterButton(&m_BtnaLogin[i]);
			CWin::RegisterButton(&m_BtnRemove[i]);
		}

		ReloadAccount(true, true, false);
	}
	IsLoginTick = FALSE;
#endif // SAVE_ACCOUNT_SYSTEM
#else
	if (gmProtect->remember_account)
	{
		/*m_aBtn[LIW_BTN].Create(15, 13, newui_ctskilldown, 3, 2, 1);
		m_aBtn[LIW_SAVE].Create(16, 16, BITMAP_CHECK_BTN, 2, 0, 0, -1, 1, 1, 1);
		CWin::RegisterButton(&m_aBtn[LIW_BTN]);
		CWin::RegisterButton(&m_aBtn[LIW_SAVE]);

		for (int i = 0; i < MAX_ACCOUNT_REG; i++)
		{
			m_BtnRemove[i].Create(11, 11, newui_party_x, 3, 2, 1);

			m_BtnaLogin[i].Create(88, 32, newui_battleskill, 1, 0, 0);

			CWin::RegisterButton(&m_BtnaLogin[i]);
			CWin::RegisterButton(&m_BtnRemove[i]);
		}*/

		m_aBtn[LIW_SAVE].Create(16, 16, BITMAP_CHECK_BTN, 2, 0, 0, -1, 1, 1, 1);
		CWin::RegisterButton(&m_aBtn[LIW_SAVE]);

		ReloadAccount(true, true, false);
	}
#endif


	SAFE_DELETE(m_pIDInputBox);
	m_pIDInputBox = new CUITextInputBox;
	m_pIDInputBox->Init(gwinhandle->GethWnd(), 140, 14, MAX_ID_SIZE);
	m_pIDInputBox->SetBackColor(0, 0, 0, 255);
	m_pIDInputBox->SetTextColor(255, 255, 230, 210);
	m_pIDInputBox->SetFont(g_hFixFont);
	m_pIDInputBox->SetState(UISTATE_NORMAL);
	m_pIDInputBox->SetText(m_ID[MAX_ACCOUNT_REG]);

	SAFE_DELETE(m_pPassInputBox);
	m_pPassInputBox = new CUITextInputBox;
	m_pPassInputBox->Init(gwinhandle->GethWnd(), 140, 14, MAX_PASSWORD_SIZE, TRUE);
	m_pPassInputBox->SetBackColor(0, 0, 0, 25);
	m_pPassInputBox->SetTextColor(255, 255, 230, 210);
	m_pPassInputBox->SetFont(g_hFixFont);
	m_pPassInputBox->SetState(UISTATE_NORMAL);

	m_pIDInputBox->SetTabTarget(m_pPassInputBox);
	m_pPassInputBox->SetTabTarget(m_pIDInputBox);

	FirstLoad = TRUE;
}

void CLoginWin::PreRelease()
{
	for (int i = 0; i < 2; ++i)
		m_asprInputBox[i].Release();
}

void CLoginWin::SetPosition(int nXCoord, int nYCoord)
{
	CWin::SetPosition(nXCoord, nYCoord);

	m_asprInputBox[LIW_ACCOUNT].SetPosition(nXCoord + 109, nYCoord + 106);
	m_asprInputBox[LIW_PASSWORD].SetPosition(nXCoord + 109, nYCoord + 131);

	m_pIDInputBox->SetPosition(int((m_asprInputBox[LIW_ACCOUNT].GetXPos() + 6) / g_fScreenRate_x), int((m_asprInputBox[LIW_ACCOUNT].GetYPos() + 6) / g_fScreenRate_y));
	m_pPassInputBox->SetPosition(int((m_asprInputBox[LIW_PASSWORD].GetXPos() + 6) / g_fScreenRate_x), int((m_asprInputBox[LIW_PASSWORD].GetYPos() + 6) / g_fScreenRate_y));

	m_aBtn[LIW_OK].SetPosition(nXCoord + 150, nYCoord + 178);
	m_aBtn[LIW_CANCEL].SetPosition(nXCoord + 211, nYCoord + 178);

#if MAIN_UPDATE > 303
#ifdef SAVE_ACCOUNT_SYSTEM
	if (gmProtect->remember_account)
	{
		_RenderFrameX = nXCoord;
		_RenderFrameY = nYCoord;

		m_aBtn[LIW_BTN].SetPosition(nXCoord + 246, nYCoord + 112);
		m_aBtn[LIW_SAVE].SetPosition(nXCoord + 246, nYCoord + 156);


		size_t i = 0;
		int iY = nYCoord + 128;

		for (i = 0; i < stAccountMacro.size(); i++)
		{
			m_BtnRemove[i].SetPosition(nXCoord + 230, iY + 5);
			m_BtnaLogin[i].SetPosition(nXCoord + 130, iY);
			iY += 19;
		}
	}
#endif // SAVE_ACCOUNT_SYSTEM
#else
	if (gmProtect->remember_account)
	{
		_RenderFrameX = nXCoord;
		_RenderFrameY = nYCoord + 238;

		m_aBtn[LIW_SAVE].SetPosition(nXCoord + 246, nYCoord + 156);

	}
#endif
#if(CB_DANGKYINGAME)
	m_DangKy.SetPosition(nXCoord + 60, nYCoord + 178);
#endif
}

void CLoginWin::Show(bool bShow)
{
	CWin::Show(bShow);

	for (int i = 0; i < 2; ++i)
	{
		m_asprInputBox[i].Show(bShow);
		m_aBtn[i].Show(bShow);
	}

#ifdef SAVE_ACCOUNT_SYSTEM
	if (gmProtect->remember_account)
	{
		m_aBtn[LIW_BTN].Show(bShow);
		m_aBtn[LIW_SAVE].Show(bShow);

#if MAIN_UPDATE > 303
		OpenAccountTable(false);
#else
		OpenAccountTable(bShow);
#endif
	}
#endif // SAVE_ACCOUNT_SYSTEM
#if(CB_DANGKYINGAME)
	m_DangKy.Show(bShow);
#endif
}

bool CLoginWin::CursorInWin(int nArea)
{
	if (!CWin::m_bShow)
		return false;

	switch (nArea)
	{
	case WA_MOVE:
		return false;
	}

	return CWin::CursorInWin(nArea);
}

#ifdef SAVE_ACCOUNT_SYSTEM
void CLoginWin::UpdateWhileActive(double dDeltaTick)
{
#if(CB_DANGKYINGAME)
	if (gInterface->Data[eWindow_DangKyInGame].OnShow)
	{
		return;
	}
#endif
#if MAIN_UPDATE > 303
	if (IsLoginTick && gmProtect->remember_account)
	{
		if (m_aBtn[LIW_BTN].IsClick())
		{
			if (m_aBtn[LIW_BTN].GetTexID() == newui_ctskilldown)
			{
				OpenAccountTable(true);
			}
			else
			{
				OpenAccountTable(false);
			}
		}
		else if (CInput::Instance().IsKeyDown(VK_ESCAPE))
		{
			OpenAccountTable(false);
		}
		else
		{
			for (size_t i = 0; i < stAccountMacro.size(); i++)
			{
				if (m_BtnRemove[i].IsClick())
				{
					stAccountMacro.erase(stAccountMacro.begin() + i);
					ReloadAccount(false, true, true);
					OpenAccountTable(false);
					return;
				}
				else if (m_BtnaLogin[i].IsClick())
				{
					m_pIDInputBox->SetText(stAccountMacro[i].NameId.c_str());
					m_pPassInputBox->SetText(stAccountMacro[i].PassID.c_str());
					RequestLogin(false);
					return;
				}
			}
		}
	}
	else
	{
		if (m_aBtn[LIW_OK].IsClick())
		{
			RequestLogin(m_aBtn[LIW_SAVE].IsCheck());
		}
		else if (m_aBtn[LIW_CANCEL].IsClick())
		{
			CancelLogin();
		}
#if(CB_DANGKYINGAME)
		else if (m_DangKy.IsClick())
		{
			if (gCB_DangKyInGame) gCB_DangKyInGame->OpenOnOff();
		}
#endif
		else if (gmProtect->remember_account && m_aBtn[LIW_SAVE].IsClick())
		{
			m_SaveAccount = m_aBtn[LIW_SAVE].IsCheck();

			leaf::CRegKey regkey;
			regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");
			regkey.WriteDword("SaveAccount", m_SaveAccount);
		}
		else if (m_aBtn[LIW_BTN].IsClick())
		{
			if (m_aBtn[LIW_BTN].GetTexID() == newui_ctskilldown)
			{
				OpenAccountTable(true);
			}
			else
			{
				OpenAccountTable(false);
			}
		}
		else if (CInput::Instance().IsKeyDown(VK_RETURN))
		{
			::PlayBuffer(SOUND_CLICK01);
			RequestLogin(m_aBtn[LIW_SAVE].IsCheck());
		}
		else if (CInput::Instance().IsKeyDown(VK_ESCAPE))
		{
			::PlayBuffer(SOUND_CLICK01);
			CancelLogin();
			CUIMng::Instance().SetSysMenuWinShow(false);
		}
	}
#else
	CInput& rInput = CInput::Instance();

	if (m_aBtn[LIW_OK].IsClick())
	{
		RequestLogin();
	}
	else if (m_aBtn[LIW_CANCEL].IsClick())
	{
		CancelLogin();
	}
	else if (rInput.IsKeyDown(VK_RETURN))
	{
		::PlayBuffer(SOUND_CLICK01);
		RequestLogin();
	}
	else if (rInput.IsKeyDown(VK_ESCAPE))
	{
		::PlayBuffer(SOUND_CLICK01);
		CancelLogin();
		CUIMng::Instance().SetSysMenuWinShow(false);
	}
	else
	{

		if (gmProtect->remember_account && m_aBtn[LIW_SAVE].IsClick())
		{
			m_SaveAccount = m_aBtn[LIW_SAVE].IsCheck();

			leaf::CRegKey regkey;
			regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");
			regkey.WriteDword("SaveAccount", m_SaveAccount);
		}
	}
#endif
}
#else
void CLoginWin::UpdateWhileActive(double dDeltaTick)
{
	CInput& rInput = CInput::Instance();

	if (m_aBtn[LIW_OK].IsClick())
	{
		RequestLogin();
	}
	else if (m_aBtn[LIW_CANCEL].IsClick())
	{
		CancelLogin();
	}
	else if (CInput::Instance().IsKeyDown(VK_RETURN))
	{
		::PlayBuffer(SOUND_CLICK01);
		RequestLogin();
	}
	else if (CInput::Instance().IsKeyDown(VK_ESCAPE))
	{
		::PlayBuffer(SOUND_CLICK01);
		CancelLogin();
		CUIMng::Instance().SetSysMenuWinShow(false);
	}
}
#endif // SAVE_ACCOUNT_SYSTEM

void CLoginWin::UpdateWhileShow(double dDeltaTick)
{
#if MAIN_UPDATE > 303
	if (gmProtect->remember_account)
	{
		if (IsLoginTick == FALSE)
		{
			m_pIDInputBox->DoAction();
			m_pPassInputBox->DoAction();
		}
	}
	else
	{
		m_pIDInputBox->DoAction();
		m_pPassInputBox->DoAction();
	}
#else
	m_pIDInputBox->DoAction();
	m_pPassInputBox->DoAction();
#endif
}

void CLoginWin::RenderControls()
{
	if (this->FirstLoad == TRUE)
	{
		if (m_ID[MAX_ACCOUNT_REG][0] != '\0')
		{
			this->GetPassInputBox()->GiveFocus();
		}
		else
		{
			this->GetIDInputBox()->GiveFocus();
		}
		this->FirstLoad = 0;
	}

	for (int i = 0; i < 2; ++i)
	{
		m_asprInputBox[i].Render();
	}

	CWin::RenderButtons();

	m_pIDInputBox->Render();
	m_pPassInputBox->Render();

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFixFont);
	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->RenderText(int((CWin::GetXPos() + 30) / g_fScreenRate_x), int((CWin::GetYPos() + 113) / g_fScreenRate_y), GlobalText[450]);
	g_pRenderText->RenderText(int((CWin::GetXPos() + 30) / g_fScreenRate_x), int((CWin::GetYPos() + 139) / g_fScreenRate_y), GlobalText[451]);

	unicode::t_char szServerName[MAX_TEXT_LENGTH];

	if (gmProtect->ServerNameDefault)
	{
		const char* apszGlobalText[4] = { GlobalText[461], GlobalText[460], GlobalText[3130], GlobalText[3131] };

		sprintf(szServerName, apszGlobalText[g_ServerListManager->GetNonPVPInfo()], g_ServerListManager->GetSelectServerName(), g_ServerListManager->GetSelectServerIndex());
	}
	else
	{
		sprintf(szServerName, "%s", g_ServerListManager->GetSelectServerName());
	}

	g_pRenderText->RenderText(int((CWin::GetXPos() + 111) / g_fScreenRate_x), int((CWin::GetYPos() + 80) / g_fScreenRate_y), szServerName);

#if MAIN_UPDATE <= 303
	float RenderFrameX = _RenderFrameX - 5;

	CInput& rInput = CInput::Instance();
	RECT rc = { 0, 0, 0, 0 };

	EnableAlphaTest();
	glColor4f(1.0, 1.0, 1.0, 1.0);

	for (size_t i = 0; i < stAccountMacro.size(); i++)
	{
		RenderBitmap(newui_battleskill, RenderFrameX, _RenderFrameY, 82, 28, 0.0, 0.0, 128.f / 128.f, 32.f / 32.f, false, false, 0.0);
		g_pRenderText->RenderTextClipped(RenderFrameX, _RenderFrameY, stAccountMacro[i].NameId.c_str(), 88, 28, 3);

		::SetRect(&rc, RenderFrameX, _RenderFrameY, RenderFrameX + 82, _RenderFrameY + 28);

		if (::PtInRect(&rc, rInput.GetCursorPos()))
		{
			if (rInput.IsLBtnUp())
			{
				m_pIDInputBox->SetText(stAccountMacro[i].NameId.c_str());
				m_pPassInputBox->SetText(stAccountMacro[i].PassID.c_str());
				RequestLogin(false);
				return;
			}
			else if(rInput.IsRBtnUp())
			{
				stAccountMacro.erase(stAccountMacro.begin() + i);
				ReloadAccount(false, true, true);
				return;
			}
		}
		RenderFrameX += 82;
	}
#endif
}

void CLoginWin::RequestLogin(bool Save)
{
	#ifdef __ANDROID__
	MU_FLOW_LOG("RequestLogin: state=%d gameConnected=%d", CurrentProtocolState, g_bGameServerConnected ? 1 : 0);
	#endif
	if (CurrentProtocolState != REQUEST_JOIN_SERVER)
	{
		CUIMng::Instance().HideWin(this);

		char szID[MAX_ID_SIZE + 1] = { 0, };
		char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
		m_pIDInputBox->GetText(szID, MAX_ID_SIZE + 1);
		m_pPassInputBox->GetText(szPass, MAX_PASSWORD_SIZE + 1);

		if (unicode::_strlen(szID))
		{
			if (unicode::_strlen(szPass))
			{
				if (CurrentProtocolState == RECEIVE_JOIN_SERVER_SUCCESS)
				{
				#ifdef __ANDROID__
					MU_FLOW_LOG("RequestLogin: sending login id=%s", szID);
				#endif
					g_ConsoleDebug->Write(MCD_NORMAL, "Login with the following account: %s", szID);

					g_ErrorReport.Write("> Login Request.\r\n");
					g_ErrorReport.Write("> Try to Login \"%s\"\r\n", szID);

#ifdef SAVE_ACCOUNT_SYSTEM
					if (Save && gmProtect->remember_account)
					{
						SaveAccount(szID, szPass);
					}
#endif // SAVE_ACCOUNT_SYSTEM

					SendRequestLogIn(szID, szPass);

					g_pReconnectUI->ReconnectGetAccountInfo(szID, szPass);
				}
				#ifdef __ANDROID__
				else
				{
					MU_FLOW_LOG("RequestLogin: blocked currentState=%d", CurrentProtocolState);
				}
				#endif
			}
			else
			{
				CUIMng::Instance().PopUpMsgWin(MESSAGE_INPUT_PASSWORD);
			}
		}
		else
		{
			CUIMng::Instance().PopUpMsgWin(MESSAGE_INPUT_ID);
		}
	}
}

#ifdef SAVE_ACCOUNT_SYSTEM
void CLoginWin::CancelLogin()
{
	ConnectConnectionServer();
	CUIMng::Instance().HideWin(this);

	if (gmProtect->remember_account)
	{
		OpenAccountTable(false);
	}
}

void CLoginWin::OpenAccountTable(bool bVisible)
{
#if MAIN_UPDATE > 303
	if (gmProtect->remember_account)
	{
		this->IsLoginTick = bVisible;

		if (bVisible == false)
		{
			m_aBtn[LIW_BTN].SetTextID(newui_ctskilldown);

			for (int i = 0; i < MAX_ACCOUNT_REG; i++)
			{
				m_BtnRemove[i].Show(bVisible);
				m_BtnaLogin[i].Show(bVisible);
			}
		}
		else
		{
			m_aBtn[LIW_BTN].SetTextID(newui_ctskillup);

			for (size_t i = 0; i < stAccountMacro.size(); i++)
			{
				m_BtnRemove[i].Show(true);
				m_BtnaLogin[i].Show(true);
			}
		}
	}
#endif
}

void CLoginWin::ReloadAccount(bool reload, bool update, bool save)
{
	if (gmProtect->remember_account)
	{
		size_t i = 0;

		if (reload)
		{
			stAccountMacro.clear();

			for (i = 0; i < MAX_ACCOUNT_REG; i++)
			{
				if (m_ID[i][0] != '\0')
				{
					stAccountMacro.push_back({ m_ID[i], m_Psz[i] });
				}
			}
		}

#if MAIN_UPDATE > 303
		if (update)
		{
			DWORD adwJobBtnClr[] =
			{
				CLRDW_WHITE, CLRDW_WHITE, CLRDW_WHITE, CLRDW_WHITE,
				CLRDW_WHITE, CLRDW_WHITE, CLRDW_WHITE, CLRDW_WHITE,
			};

			i = 0;

			for (i = 0; i < stAccountMacro.size(); i++)
			{
				LOGIN_ACCOUNT_REG* Info = &stAccountMacro[i];
				m_BtnaLogin[i].SetText(Info->NameId.c_str(), adwJobBtnClr);
			}

			m_aBtn[LIW_SAVE].SetCheck(m_SaveAccount);
		}
#else
		if (update)
		{
			m_aBtn[LIW_SAVE].SetCheck(m_SaveAccount);
		}
#endif

		if (save)
		{
			DWORD dwSize = sizeof(m_ID);

			memset(m_ID, 0, sizeof(m_ID));
			memset(m_Psz, 0, sizeof(m_Psz));

			i = 0;
			for (i = 0; i < stAccountMacro.size(); i++)
			{
				LOGIN_ACCOUNT_REG* Info = &stAccountMacro[i];

				strncpy_s(m_ID[i], Info->NameId.c_str(), dwSize);
				strncpy_s(m_Psz[i], Info->PassID.c_str(), dwSize);
			}

			leaf::CRegKey regkey;
			regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");

			if (regkey.WriteBinary("_LiwID", (LPBYTE)m_ID, dwSize))
			{
				dwSize = sizeof(m_Psz);
				regkey.WriteBinary("_LiwKey", (LPBYTE)m_Psz, dwSize);
			}
		}
	}
}

void CLoginWin::SaveAccount(std::string NameId, std::string PassID)
{
	if (gmProtect->remember_account)
	{
		bool accountFound = false;

		for (auto& account : stAccountMacro)
		{
			if (account.NameId == NameId)
			{
				account.PassID = PassID;
				accountFound = true;
				break;
			}
		}

		// Si no se encontr�, agregamos la nueva cuenta
		if (!accountFound)
		{
			if (stAccountMacro.size() < MAX_ACCOUNT_REG)
			{
				stAccountMacro.push_back({ NameId, PassID });
			}
		}

		ReloadAccount(false, true, true);
	}
}
#else
void CLoginWin::CancelLogin()
{
	ConnectConnectionServer();
	CUIMng::Instance().HideWin(this);
}
#endif // SAVE_ACCOUNT_SYSTEM

void CLoginWin::ConnectConnectionServer()
{
	LogIn = 0;
	CurrentProtocolState = REQUEST_JOIN_SERVER;
	if (CreateSocket(szServerIpAddress, g_ServerPort))
	{
		SendRequestServerHWID();
	}
}

CUITextInputBox* CLoginWin::GetIDInputBox() const
{
	return m_pIDInputBox;
}

CUITextInputBox* CLoginWin::GetPassInputBox() const
{
	return m_pPassInputBox;
}
