//*****************************************************************************
// File: CharSelMainWin.cpp
//*****************************************************************************

#include "stdafx.h"
#include "CharSelMainWin.h"
#include "Input.h"
#include "UIMng.h"
#include "ZzzBMD.h"
#include "CGMFontLayer.h"

#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "UIGuildInfo.h"
#include "ZzzOpenData.h"
#include "ZzzOpenglUtil.h"
#include "CharInfoBalloon.h"
#include "ServerListManager.h"

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;

extern void CharacterTeleport(int key);


CCharSelMainWin::CCharSelMainWin()
{
	m_bAccountBlockItem = false;
}

CCharSelMainWin::~CCharSelMainWin()
{
}

void CCharSelMainWin::Create()
{
	m_asprBack[CSMW_SPR_DECO].Create(189, 103, BITMAP_LOG_IN + 2);
	m_asprBack[CSMW_SPR_INFO].Create(CInput::Instance().GetScreenWidth() - 266, 21);
	m_asprBack[CSMW_SPR_INFO].SetColor(0, 0, 0);
	m_asprBack[CSMW_SPR_INFO].SetAlpha(143);

	m_aBtn[CSMW_BTN_CREATE].Create(54, 30, BITMAP_LOG_IN + 3, 4, 2, 1, 3);
	m_aBtn[CSMW_BTN_MENU].Create(54, 30, BITMAP_LOG_IN + 4, 3, 2, 1);
	m_aBtn[CSMW_BTN_CONNECT].Create(54, 30, BITMAP_LOG_IN + 5, 4, 2, 1, 3);
	m_aBtn[CSMW_BTN_DELETE].Create(54, 30, BITMAP_LOG_IN + 6, 4, 2, 1, 3);

	CWin::Create(m_aBtn[0].GetWidth() * CSMW_BTN_MAX + m_asprBack[CSMW_SPR_INFO].GetWidth() + 6, m_aBtn[0].GetHeight(), -2);

	for (int i = 0; i < CSMW_BTN_MAX; ++i)
		CWin::RegisterButton(&m_aBtn[i]);

	m_bAccountBlockItem = false;

	for (int i = 0; i < 5; ++i)
	{
		CHARACTER* pCha = gmCharacters->GetCharacter(i);

		if (pCha->Object.Live && (pCha->CtlCode & CTLCODE_10ACCOUNT_BLOCKITEM))
		{
			m_bAccountBlockItem = true;
			break;
		}
	}
}

void CCharSelMainWin::PreRelease()
{
	for (int i = 0; i < CSMW_SPR_MAX; ++i)
		m_asprBack[i].Release();
}

void CCharSelMainWin::SetPosition(int nXCoord, int nYCoord)
{
	CWin::SetPosition(nXCoord, nYCoord);

	int nBtnWidth = m_aBtn[0].GetWidth();

	m_aBtn[CSMW_BTN_CREATE].SetPosition(nXCoord, nYCoord);
	m_aBtn[CSMW_BTN_MENU].SetPosition(nXCoord + nBtnWidth + 1, nYCoord);
	m_asprBack[CSMW_SPR_INFO].SetPosition(m_aBtn[CSMW_BTN_MENU].GetXPos() + nBtnWidth + 2, nYCoord + 5);

	int nWinRPosX = nXCoord + CWin::GetWidth();
	m_asprBack[CSMW_SPR_DECO].SetPosition(nWinRPosX - (m_asprBack[CSMW_SPR_DECO].GetWidth() - 22), nYCoord - 59);
	m_aBtn[CSMW_BTN_DELETE].SetPosition(nWinRPosX - nBtnWidth, nYCoord);
	m_aBtn[CSMW_BTN_CONNECT].SetPosition(nWinRPosX - (nBtnWidth * 2 + 1), nYCoord);
}

void CCharSelMainWin::Show(bool bShow)
{
	CWin::Show(bShow);

	int i;
	for (i = 0; i < CSMW_SPR_MAX; ++i)
		m_asprBack[i].Show(bShow);
	for (i = 0; i < CSMW_BTN_MAX; ++i)
		m_aBtn[i].Show(bShow);
}

bool CCharSelMainWin::CursorInWin(int nArea)
{
	if (!CWin::m_bShow)
		return false;

	switch (nArea)
	{
	case WA_MOVE:
		return false;
	}

	if (gmProtect->SceneCharacter == 4 && runtime_check_button() == true)
	{
		return true;
	}

	return CWin::CursorInWin(nArea);
}

void CCharSelMainWin::UpdateDisplay()
{
	m_aBtn[CSMW_BTN_CREATE].SetEnable(false);
	int i = 0;

	for (i = 0; i < 5; ++i)
	{
		CHARACTER* pCha = gmCharacters->GetCharacter(i);

		if (pCha && !pCha->Object.Live)
		{
			m_aBtn[CSMW_BTN_CREATE].SetEnable(true);
			break;
		}
	}

	if (SelectedHero > -1)
	{
		m_aBtn[CSMW_BTN_CONNECT].SetEnable(true);
		m_aBtn[CSMW_BTN_DELETE].SetEnable(true);
	}
	else
	{
		m_aBtn[CSMW_BTN_CONNECT].SetEnable(false);
		m_aBtn[CSMW_BTN_DELETE].SetEnable(false);
	}


	bool bNobodyCharacter = true;

	for (int i = 0; i < 5; ++i)
	{
		CHARACTER* pCha = gmCharacters->GetCharacter(i);

		if (pCha && pCha->Object.Live == true)
		{
			bNobodyCharacter = false;
			break;
		}
	}

	if (bNobodyCharacter == true)
	{
		CUIMng& rUIMng = CUIMng::Instance();

		rUIMng.m_CharMakeWin.UpdateDisplay();
		rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
		rUIMng.m_CharMakeWin.UpdateDisplay(0);
	}
}

void CCharSelMainWin::UpdateWhileActive(double dDeltaTick)
{
	if (dDeltaTick >= 0)
	{
		if (m_aBtn[CSMW_BTN_CONNECT].IsClick())
		{
			::StartGame();
		}
		else if (m_aBtn[CSMW_BTN_MENU].IsClick())
		{
			CUIMng& rUIMng = CUIMng::Instance();
			rUIMng.ShowWin(&rUIMng.m_SysMenuWin);
			rUIMng.SetSysMenuWinShow(true);
		}
		else if (m_aBtn[CSMW_BTN_CREATE].IsClick())
		{
			CUIMng& rUIMng = CUIMng::Instance();

			rUIMng.m_CharMakeWin.UpdateDisplay();
			rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
			rUIMng.m_CharMakeWin.UpdateDisplay(0);
		}
		else if (m_aBtn[CSMW_BTN_DELETE].IsClick())
		{
			DeleteCharacter();
		}
		else
		{
			if (gmProtect->SceneCharacter == 4)
			{
				for (int i = 0; i < 5; i++)
				{
					update_button_info(i, GetWindowsX - 156, 30 + i * 40);
				}
			}
		}
	}
}


void CCharSelMainWin::RenderControls()
{
	for (int i = 0; i < CSMW_SPR_MAX; ++i)
		m_asprBack[i].Render();

	::EnableAlphaTest();
	::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	g_pRenderText->SetFont(g_hFixFont);
	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->SetBgColor(0);

	if (m_bAccountBlockItem)
	{
		g_pRenderText->SetTextColor(0, 0, 0, 255);
		g_pRenderText->SetBgColor(255, 255, 0, 128);
		g_pRenderText->RenderText(320, 330, GlobalText[436], 0, 0, RT3_WRITE_CENTER);
		g_pRenderText->RenderText(320, 348, GlobalText[437], 0, 0, RT3_WRITE_CENTER);
	}

	if (gmProtect->SceneCharacter == 4)
	{
		for (int i = 0; i < 5; i++)
		{
			render_button_info(i, GetWindowsX - 156, 30 + i * 40);
		}
	}

	CWin::RenderButtons();
}

void CCharSelMainWin::DeleteCharacter()
{
	CHARACTER* pCha = gmCharacters->GetCharacter(SelectedHero);

	if (pCha)
	{
		if (pCha->GuildStatus != G_NONE)
			CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_GUILDWARNING);
		else if (pCha->CtlCode & (CTLCODE_02BLOCKITEM | CTLCODE_10ACCOUNT_BLOCKITEM))
			CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_ID_BLOCK);
		else
			CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_CONFIRM);
	}
}

bool CCharSelMainWin::runtime_check_button()
{
	int _RenderFrameX = GetWindowsX - 156;
	int _RenderFrameY = 30;

	return SEASON3B::CheckMouseIn(_RenderFrameX, _RenderFrameY, 140, 200);
}

bool CCharSelMainWin::update_button_info(int i, int _RenderFrameX, int _RenderFrameY)
{
	CInput& rInput = CInput::Instance();
	CUIMng& rUIMng = CUIMng::Instance();

	CHARACTER* pCharacter = gmCharacters->GetCharacter(i);

	OBJECT* pObject = &pCharacter->Object;

	if (SEASON3B::CheckMouseIn(_RenderFrameX, _RenderFrameY, 140, 34.f))
	{
		if (pCharacter && pObject->Live == true)
		{
			SelectedCharacter = i;

			if (pObject->Position[0] == 13629.f && pObject->Position[1] == 12339.f)
			{
				pObject->Angle[2] -= timefac(3);
			}

			if (MouseLButtonDBClick)
			{
				if (SelectedHero == SelectedCharacter)
				{
					::StartGame();
				}
				MouseLButtonDBClick = false;
				return true;
			}
			else if (rInput.IsLBtnUp())
			{
				SelectedHero = SelectedCharacter;

				CharacterTeleport(SelectedCharacter);

				rUIMng.m_CharSelMainWin.UpdateDisplay();
				return true;
			}
		}
		else
		{
			if (rInput.IsLBtnUp())
			{
				rUIMng.ShowWin(&rUIMng.m_CharMakeWin);
				rUIMng.m_CharMakeWin.UpdateDisplay(0);
				return true;
			}
		}
	}
	else
	{
		if (SelectedCharacter != i)
		{
			if (pCharacter && pObject->Live == true)
			{
				if (pObject->Position[0] == 13629.f && pObject->Position[1] == 12339.f && pObject->Angle[2] != 90.f)
				{
					pObject->Angle[2] = 90.f;
				}
			}
		}
	}
	return false;
}

void CCharSelMainWin::render_button_info(int i, int _RenderFrameX, int _RenderFrameY)
{
	CHARACTER* pCharacter = gmCharacters->GetCharacter(i);

	g_pRenderText->SetBgColor(0);

	if (pCharacter && pCharacter->Object.Live == true)
	{
		CUIMng& rUIMng = CUIMng::Instance();

		if (SEASON3B::CheckMouseIn(_RenderFrameX, _RenderFrameY, 140, 34.f) || SelectedHero == i)
			SEASON3B::RenderImageF(BITMAP_LOG_IN + 15, _RenderFrameX, _RenderFrameY, 140, 34.f, 583.f, 725.f, 227.f, 55.f); //-- selected__player
		else
			SEASON3B::RenderImageF(BITMAP_LOG_IN + 15, _RenderFrameX, _RenderFrameY, 140, 34.f, 583.f, 668.f, 227.f, 55.f); //-- player

		CCharInfoBalloon* balloon = rUIMng.m_CharInfoBalloonMng.GetCharacter(i);

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(0xFF00DCFF);
		g_pRenderText->RenderText((_RenderFrameX + 25), (_RenderFrameY + 20), balloon->GetName(), 0, 0, RT3_SORT_LEFT);

		g_pRenderText->RenderText((_RenderFrameX + 100), (_RenderFrameY + 20), balloon->GetOnLevel(), 0, 0, RT3_SORT_LEFT);

		g_pRenderText->SetTextColor(0xFFFFFFFF);
		g_pRenderText->RenderText((_RenderFrameX + 10), (_RenderFrameY + 6), balloon->GetClassname(), 0, 0, RT3_SORT_LEFT);

		g_pRenderText->RenderText((_RenderFrameX + 100), (_RenderFrameY + 6), balloon->GetGuild(), 0, 0, RT3_SORT_LEFT);

	}
	else
	{
		if (SEASON3B::CheckMouseIn(_RenderFrameX, _RenderFrameY, 140, 34.f))
			SEASON3B::RenderImageF(BITMAP_LOG_IN + 15, _RenderFrameX, _RenderFrameY, 140, 34.f, 583.f, 782.f, 227.f, 55.f); //-- selected__none
		else
			SEASON3B::RenderImageF(BITMAP_LOG_IN + 15, _RenderFrameX, _RenderFrameY, 140, 34.f, 583.f, 839.f, 227.f, 55.f); //-- none


		g_pRenderText->SetTextColor(0xFF00DCFF);
		GMFontLayer->RenderText((_RenderFrameX + 70) * g_fScreenRate_x, (_RenderFrameY * g_fScreenRate_y), "Empty Character Slot", 0, (34.f * g_fScreenRate_y), RT3_WRITE_CENTER);
	}
}
