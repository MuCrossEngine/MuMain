// NewUIDuelWindow.cpp: implementation of the CNewUIDuelWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIDuelWindow.h"
#include "ZzzTexture.h"
#include "ZzzInventory.h"
#include "ZzzBMD.h"
#include "ZzzCharacter.h"
#include "UIControls.h"
#include "DuelMgr.h"

using namespace SEASON3B;

extern int GetScreenWidth();

SEASON3B::CNewUIDuelWindow::CNewUIDuelWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = m_Pos.y = 0;
}

SEASON3B::CNewUIDuelWindow::~CNewUIDuelWindow()
{
	Release();
}

bool SEASON3B::CNewUIDuelWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_DUEL_WINDOW, this);

	SetPos(x, y);

	LoadImages();

	Show(false);

	return true;
}

void SEASON3B::CNewUIDuelWindow::Release()
{
	if(m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj( this );
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIDuelWindow::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

bool SEASON3B::CNewUIDuelWindow::UpdateMouseEvent()
{
	return true;
}

bool SEASON3B::CNewUIDuelWindow::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIDuelWindow::Update()
{
	return true;
}

bool SEASON3B::CNewUIDuelWindow::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderFrame();

	RenderContents();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIDuelWindow::RenderFrame()
{
#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		float framerenderX = (GetScreenWidth() * 0.5f);

		float v51 = framerenderX - 150.0;

		EnableAlphaTest(1);
		glColor4f(0.0, 0.0, 0.0, 0.5);
		RenderColor(v51, 20.0, 300.0, 40.0);
		glColor3f(1.0, 1.0, 1.0);
		glColor4f(0.64999998, 0.52999997, 0.34, 1.0);
		RenderColor(v51, 20.0, 300.0, 1.0);
		RenderColor(v51, 60.0, 300.0, 1.0);
		RenderColor(v51, 20.0, 1.0, 40.0);
		v51 = framerenderX + 150.0;
		RenderColor(v51, 20.0, 1.0, 40.0);
		EndRenderColor();
	}
	else
	{
		RenderImage(IMAGE_DUEL_BACK, m_Pos.x, m_Pos.y, 131, 70);
	}
#else
	float framerenderX = (GetScreenWidth() * 0.5f);

	float v51 = framerenderX - 150.0;

	EnableAlphaTest(1);
	glColor4f(0.0, 0.0, 0.0, 0.5);
	RenderColor(v51, 20.0, 300.0, 40.0);
	glColor3f(1.0, 1.0, 1.0);
	glColor4f(0.64999998, 0.52999997, 0.34, 1.0);
	RenderColor(v51, 20.0, 300.0, 1.0);
	RenderColor(v51, 60.0, 300.0, 1.0);
	RenderColor(v51, 20.0, 1.0, 40.0);
	v51 = framerenderX + 150.0;
	RenderColor(v51, 20.0, 1.0, 40.0);
	EndRenderColor();
#endif // MAIN_UPDATE == 303
}

void SEASON3B::CNewUIDuelWindow::RenderContents()
{
	unicode::t_char strMyScore[12];
	unicode::t_char strDuelScore[12];
	unicode::_sprintf(strMyScore, "%d", g_DuelMgr.GetScore(DUEL_HERO));
	unicode::_sprintf(strDuelScore, "%d", g_DuelMgr.GetScore(DUEL_ENEMY));

#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		float framerenderX = (GetScreenWidth() * 0.5f);

		g_pRenderText->SetFont(g_hFontBig);
		g_pRenderText->SetBgColor(0);

		g_pRenderText->SetTextColor(0, 0, 0, 255);
		g_pRenderText->RenderText((framerenderX - 79.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_HERO), 0, 0, RT3_WRITE_CENTER);
		g_pRenderText->SetTextColor(0, 150, 255, 255);
		g_pRenderText->RenderText((framerenderX - 80.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_HERO), 0, 0, RT3_WRITE_CENTER);


		g_pRenderText->SetTextColor(255, 150, 0, 255);
		g_pRenderText->RenderText((framerenderX), 30, GlobalText[1394], 0, 0, RT3_WRITE_CENTER);


		g_pRenderText->SetTextColor(0, 0, 0, 255);
		g_pRenderText->RenderText((framerenderX + 81.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY), 0, 0, RT3_WRITE_CENTER);
		g_pRenderText->SetTextColor(255, 25, 25, 255);
		g_pRenderText->RenderText((framerenderX + 80.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY), 0, 0, RT3_WRITE_CENTER);

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(0, 150, 255, 255);
		g_pRenderText->RenderText((framerenderX - 80.f), 48, strMyScore, 0, 0, RT3_WRITE_CENTER);

		g_pRenderText->SetTextColor(255, 25, 25, 255);
		g_pRenderText->RenderText((framerenderX + 80.f), 48, strDuelScore, 0, 0, RT3_WRITE_CENTER);
	}
	else
	{
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetTextColor(0, 0, 0, 255);
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetTextColor(0, 150, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 55, m_Pos.y + 33, g_DuelMgr.GetDuelPlayerID(DUEL_HERO));
		g_pRenderText->RenderText(m_Pos.x + 31, m_Pos.y + 33, strMyScore);
		g_pRenderText->SetTextColor(255, 25, 25, 255);
		g_pRenderText->RenderText(m_Pos.x + 55, m_Pos.y + 56, g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY));
		g_pRenderText->RenderText(m_Pos.x + 31, m_Pos.y + 56, strDuelScore);
	}
#else
	float framerenderX = (GetScreenWidth() * 0.5f);

	g_pRenderText->SetFont(g_hFontBig);
	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(0, 0, 0, 255);
	g_pRenderText->RenderText((framerenderX - 79.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_HERO), 0, 0, RT3_WRITE_CENTER);
	g_pRenderText->SetTextColor(0, 150, 255, 255);
	g_pRenderText->RenderText((framerenderX - 80.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_HERO), 0, 0, RT3_WRITE_CENTER);


	g_pRenderText->SetTextColor(255, 150, 0, 255);
	g_pRenderText->RenderText((framerenderX), 30, GlobalText[1394], 0, 0, RT3_WRITE_CENTER);


	g_pRenderText->SetTextColor(0, 0, 0, 255);
	g_pRenderText->RenderText((framerenderX + 81.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY), 0, 0, RT3_WRITE_CENTER);
	g_pRenderText->SetTextColor(255, 25, 25, 255);
	g_pRenderText->RenderText((framerenderX + 80.f), 31, g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY), 0, 0, RT3_WRITE_CENTER);

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(0, 150, 255, 255);
	g_pRenderText->RenderText((framerenderX - 80.f), 48, strMyScore, 0, 0, RT3_WRITE_CENTER);

	g_pRenderText->SetTextColor(255, 25, 25, 255);
	g_pRenderText->RenderText((framerenderX + 80.f), 48, strDuelScore, 0, 0, RT3_WRITE_CENTER);
#endif // MAIN_UPDATE == 303

}

float SEASON3B::CNewUIDuelWindow::GetLayerDepth()
{
	return 1.1f;
}

void SEASON3B::CNewUIDuelWindow::LoadImages()
{
	LoadBitmap("Interface\\newui_Figure_ground.tga", IMAGE_DUEL_BACK, GL_LINEAR);
}

void SEASON3B::CNewUIDuelWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_DUEL_BACK);
}
