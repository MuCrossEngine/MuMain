// NewUIHeroPositionInfo.cpp: implementation of the CNewUIHeroPositionInfo class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapManager.h"
#include "NewUISystem.h"
#include "CAIController.h"
#include "wsclientinline.h"
#include "NewUIHeroPositionInfo.h"
#include "CGMProtect.h"

using namespace SEASON3B;

SEASON3B::CNewUIHeroPositionInfo::CNewUIHeroPositionInfo()
{
	m_pNewUIMng = NULL;
	WidenX = 0;
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_CurHeroPosition.x = 0;
	m_CurHeroPosition.y = 0;
}

SEASON3B::CNewUIHeroPositionInfo::~CNewUIHeroPositionInfo()
{
	Release();
}

bool SEASON3B::CNewUIHeroPositionInfo::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_HERO_POSITION_INFO, this);

#if MAIN_UPDATE > 303
	WidenX = HERO_POSITION_INFO_BASEB_WINDOW_WIDTH;
	if (WindowWidth == 800)
	{
		WidenX = (HERO_POSITION_INFO_BASEB_WINDOW_WIDTH + (HERO_POSITION_INFO_BASEB_WINDOW_WIDTH * 0.4f));
	}
	else// if (WindowWidth == 1024)
	{
		WidenX = (HERO_POSITION_INFO_BASEB_WINDOW_WIDTH + (HERO_POSITION_INFO_BASEB_WINDOW_WIDTH * 0.2f));
	}

	SetPos(x, y);

	LoadImages();

	SetButtonInfo();

	Show(true);
#else
	Show(false);
#endif // MAIN_UPDATE == 303

	return true;
}

void SEASON3B::CNewUIHeroPositionInfo::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIHeroPositionInfo::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CNewUIHeroPositionInfo::SetButtonInfo()
{
	unicode::t_string tooltiptext1 = GlobalText[3561];
	unicode::t_string btname1 = "";

	SetButtonInfo(
		&m_BtnSetup,
		IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 3,
		m_Pos.x + WidenX + 41,
		m_Pos.y,
		18,
		13,
		1,
		0,
		1,
		1u,
		btname1,
		tooltiptext1,
		0);

	unicode::t_string tooltiptext2 = GlobalText[3562];

	SetButtonInfo(
		&m_BtnStart,
		IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 4,
		m_Pos.x + WidenX + 59,
		m_Pos.y,
		18,
		13,
		1,
		0,
		1,
		1u,
		btname1,
		tooltiptext2,
		0);

	unicode::t_string tooltiptext3 = GlobalText[3563];

	SetButtonInfo(
		&m_BtnPause,
		IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 5,
		m_Pos.x + WidenX + 59,
		m_Pos.y,
		18,
		13,
		1,
		0,
		1,
		1u,
		btname1,
		tooltiptext3,
		0);

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
	{
		unicode::t_string tooltiptext4 = "Menu";

		SetButtonInfo(
			&m_BtniMenu,
			IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 6,
			m_Pos.x + WidenX + 77,
			m_Pos.y,
			18,
			13,
			1,
			0,
			1,
			1u,
			btname1,
			tooltiptext4,
			0);

		MoveTextTipPos(&m_BtniMenu, 0, 9);
	}

#if MAIN_UPDATE > 603
	m_BtnSetup.SetSize(15, 13);
	m_BtnStart.SetSize(15, 13);
	m_BtnPause.SetSize(15, 13);

	m_BtnSetup.SetPos(m_Pos.x + 127, m_Pos.y + 7);
	m_BtnStart.SetPos(m_Pos.x + 144, m_Pos.y + 7);
	m_BtnPause.SetPos(m_Pos.x + 144, m_Pos.y + 7);

	m_BtnSetup.CurSize(58, 50);
	m_BtnStart.CurSize(58, 50);
	m_BtnPause.CurSize(58, 50);

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
	{
		m_BtniMenu.SetPos(m_Pos.x + 110, m_Pos.y + 7);
		m_BtniMenu.SetSize(15, 13);
		m_BtniMenu.CurSize(58, 50);
	}
#endif // test_main_dev

	MoveTextTipPos(&m_BtnSetup, 0, 9);
	MoveTextTipPos(&m_BtnStart, 0, 9);
	MoveTextTipPos(&m_BtnPause, 0, 9);
}

bool SEASON3B::CNewUIHeroPositionInfo::BtnProcess()
{
#if MAIN_UPDATE > 303
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE)
		|| g_pNewUISystem->IsImpossibleTradeInterface())
	{
		return false;
	}

	if (GMProtect->shutdown_oficial_helper() == true)
	{
		if (m_BtnSetup.UpdateMouseEvent())
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MACRO_OFICIAL);
			PlayBuffer(SOUND_CLICK01);

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL))
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MACRO_OFICIAL_SUB);
			}
			return true;
		}

		if (gmAIController->IsRunning() != true)
		{
			if (m_BtnStart.UpdateMouseEvent())
			{
				if (gmAIController->CanUseAIController())
				{
					SendRequestStartHelper(false);
				}
				return true;
			}
		}
		else
		{
			if (m_BtnPause.UpdateMouseEvent())
			{
				SendRequestStartHelper(true);
				return true;
			}
		}
	}

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
	{
		if (m_BtniMenu.UpdateMouseEvent())
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_CUSTOM_MENU);
			//for (int i = BITMAP_PLAYER_TEXTURE_BEGIN; i < BITMAP_SKIN_PANTS_SUCCUBUS; i++)
			//{
			//	gmClientModels->SaveImg(i);
			//}
			return true;
		}
	}
#endif
	return false;
}


bool SEASON3B::CNewUIHeroPositionInfo::UpdateMouseEvent()
{
#if MAIN_UPDATE > 603
	if (true == BtnProcess())
		return false;

	int Width = HERO_POSITION_INFO_BASEA_WINDOW_WIDTH + WidenX;

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
	{
		Width += 91;
	}
	else
	{
		Width += 73;
	}

	if (CheckMouseIn(m_Pos.x, m_Pos.y, Width, HERO_POSITION_INFO_BASE_WINDOW_HEIGHT))
		return false;
#elif MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		if (true == BtnProcess())
			return false;

		int Width = HERO_POSITION_INFO_BASEA_WINDOW_WIDTH + WidenX;

		if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
		{
			Width += 91;
		}
		else
		{
			Width += 73;
		}

		if (CheckMouseIn(m_Pos.x, m_Pos.y, Width, HERO_POSITION_INFO_BASE_WINDOW_HEIGHT))
			return false;
	}
#endif
	return true;
}

bool SEASON3B::CNewUIHeroPositionInfo::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIHeroPositionInfo::Update()
{
#if MAIN_UPDATE > 603
	if (GMProtect->shutdown_oficial_helper() == true)
	{
		if ((IsVisible() == true) && (Hero != NULL))
		{
			m_CurHeroPosition.x = (Hero->PositionX);
			m_CurHeroPosition.y = (Hero->PositionY);
		}
	}
#elif MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		if (GMProtect->shutdown_oficial_helper() == true)
		{
			if ((IsVisible() == true) && (Hero != NULL))
			{
				m_CurHeroPosition.x = (Hero->PositionX);
				m_CurHeroPosition.y = (Hero->PositionY);
			}
		}
	}
#endif
	return true;
}

void SEASON3B::CNewUIHeroPositionInfo::RenderFrame()
{
#if MAIN_UPDATE > 603
	unicode::t_char szText[256] = { NULL, };

	RenderImageF(IMAGE_HERO_POSITION_INFO_BASE_WINDOW, m_Pos.x, m_Pos.y, 190, 26, 0.0, 0.0, 717.0, 99.0);

	unicode::_sprintf(szText, "%s (%d , %d)", gMapManager->GetMapName(gMapManager->currentMap), m_CurHeroPosition.x, m_CurHeroPosition.y);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->RenderText(m_Pos.x + 30, m_Pos.y + 8, szText, 0, 0, RT3_SORT_LEFT);
#elif MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		unicode::t_char szText[256] = { NULL, };

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);

		RenderImage(IMAGE_HERO_POSITION_INFO_BASE_WINDOW, m_Pos.x, m_Pos.y, float(HERO_POSITION_INFO_BASEA_WINDOW_WIDTH), float(HERO_POSITION_INFO_BASE_WINDOW_HEIGHT));

		RenderImage(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 1, m_Pos.x + HERO_POSITION_INFO_BASEA_WINDOW_WIDTH, m_Pos.y, float(WidenX), float(HERO_POSITION_INFO_BASE_WINDOW_HEIGHT), 0.1f, 0.f, 22.4f / 32.f, 25.f / 32.f);

		if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
		{
			RenderImage(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2, m_Pos.x + HERO_POSITION_INFO_BASEA_WINDOW_WIDTH + WidenX, m_Pos.y, 91.f, 20.f);
		}
		else
		{
			RenderImage(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2, m_Pos.x + HERO_POSITION_INFO_BASEA_WINDOW_WIDTH + WidenX, m_Pos.y, 73.f, 20.f);
		}

		unicode::_sprintf(szText, "%s (%d , %d)", gMapManager->GetMapName(gMapManager->currentMap), m_CurHeroPosition.x, m_CurHeroPosition.y);

		g_pRenderText->RenderText(m_Pos.x + 10, m_Pos.y + 5, szText, WidenX + 20, 13 - 4, RT3_SORT_CENTER);
	}
#else
#endif
}

void SEASON3B::CNewUIHeroPositionInfo::RenderButton()
{
#if MAIN_UPDATE > 603
	m_BtnSetup.Render();

	if (gmAIController->IsRunning() != true)
		m_BtnStart.Render();
	else
		m_BtnPause.Render();

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
	{
		m_BtniMenu.Render();
	}
#elif MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		if (GMProtect->shutdown_oficial_helper() == true)
		{
			m_BtnSetup.Render();

			if (gmAIController->IsRunning() != true)
			{
				m_BtnStart.Render();
			}
			else
			{
				m_BtnPause.Render();
			}
		}

		if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton == 2)
		{
			m_BtniMenu.Render();
		}
	}
#else
#endif
}

bool SEASON3B::CNewUIHeroPositionInfo::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();

	RenderButton();

	DisableAlphaBlend();

	return true;
}

float SEASON3B::CNewUIHeroPositionInfo::GetLayerDepth()
{
	return 4.3;
}

void SEASON3B::CNewUIHeroPositionInfo::OpenningProcess()
{

}

void SEASON3B::CNewUIHeroPositionInfo::ClosingProcess()
{

}

void SEASON3B::CNewUIHeroPositionInfo::SetCurHeroPosition(int x, int y)
{
	m_CurHeroPosition.x = x;
	m_CurHeroPosition.y = y;
}

void SEASON3B::CNewUIHeroPositionInfo::MoveTextTipPos(CNewUIButton* Button, int iX, int iY)
{
	Button->MoveTextTipPos(iX, iY);
}

void SEASON3B::CNewUIHeroPositionInfo::SetButtonInfo(CNewUIButton* Button, int imgindex, int x, int y, int sx, int sy, bool overflg, bool isimgwidth, bool bClickEffect, bool MoveTxt, unicode::t_string btname, unicode::t_string tooltiptext, bool istoppos)
{
	Button->ChangeButtonImgState(1, imgindex, overflg, isimgwidth, bClickEffect);
	Button->ChangeButtonInfo(x, y, sx, sy);

	if(btname.size() > 0)
		Button->ChangeText(btname);

	if (tooltiptext.size() > 0)
		Button->ChangeToolTipText(tooltiptext, istoppos);

	if (MoveTxt)
	{
		Button->MoveTextPos(0, -1);
	}
}

void SEASON3B::CNewUIHeroPositionInfo::LoadImages()
{
#if MAIN_UPDATE > 603
	LoadBitmap("Interface\\HUD_TEST\\Macro_Helper.dds", IMAGE_HERO_POSITION_INFO_BASE_WINDOW, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\Macro_Helper_btn1.dds", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 3, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\Macro_Helper_btn2.dds", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 4, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\Macro_Helper_btn2.dds", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 5, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\Macro_Helper_btn3.dds", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 6, GL_LINEAR);
#elif MAIN_UPDATE > 303
	LoadBitmap("Interface\\Minimap_positionA.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW, GL_LINEAR);
	LoadBitmap("Interface\\Minimap_positionB.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 1, GL_LINEAR);

	LoadBitmap("Interface\\MacroUI\\MacroUI_Setup.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 3, GL_LINEAR);
	LoadBitmap("Interface\\MacroUI\\MacroUI_Start.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 4, GL_LINEAR);
	LoadBitmap("Interface\\MacroUI\\MacroUI_Stop.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 5, GL_LINEAR);

	if (gmProtect->MenuSwitch != 0)
	{
		if (gmProtect->MenuTypeButton == 2)
		{
			LoadBitmap("Interface\\MacroUI\\Minimap_positionExt.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2, GL_LINEAR);
			LoadBitmap("Interface\\MacroUI\\MacroUI_Settings.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 6, GL_LINEAR);
		}
		else
		{
			LoadBitmap("Interface\\HUD\\Menu1.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 6, GL_LINEAR);
			LoadBitmap("Interface\\MacroUI\\Minimap_positionC.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2, GL_LINEAR);
		}
	}
	else
	{
		LoadBitmap("Interface\\MacroUI\\Minimap_positionC.tga", IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2, GL_LINEAR);
	}
#endif
}

void SEASON3B::CNewUIHeroPositionInfo::UnloadImages()
{
#if MAIN_UPDATE > 303
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW);
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 1);
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 2);
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 3);
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 4);
	DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 5);
	if (gmProtect->MenuSwitch != 0)
	{
		DeleteBitmap(IMAGE_HERO_POSITION_INFO_BASE_WINDOW + 6);
	}
#endif // MAIN_UPDATE == 303
}


