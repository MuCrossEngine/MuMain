// NewUINameWindow.cpp: implementation of the CNewUINameWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUINameWindow.h"
#include "ZzzBmd.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "UIControls.h"
#include "CSChaosCastle.h"
#include "PersonalShopTitleImp.h"
#include "MatchEvent.h"
#include "MapManager.h"
#include "ScaleForm.h"
#include "CGMHeadChat.h"
#include "CGMItemDropName.h"
#include "jpexs.h"


using namespace SEASON3B;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SEASON3B::CNewUINameWindow::CNewUINameWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = m_Pos.y = 0;

	#ifdef __ANDROID__
	m_bShowItemName = true;
	#else
	m_bShowItemName = false;
	#endif
}

SEASON3B::CNewUINameWindow::~CNewUINameWindow()
{
	Release();
}

bool SEASON3B::CNewUINameWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_NAME_WINDOW, this);

	LoadImages();

	SetPos(x, y);

	Show(true);

	return true;
}

void SEASON3B::CNewUINameWindow::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUINameWindow::LoadImages()
{
	LoadBitmap("Interface\\HUD\\TooltipHUD.tga", IMAGE_TOOLTIP_ID, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Interface\\HUD\\hudPlayerName_I1.tga", IMAGE_HUD_PLAYER, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Interface\\HUD\\bg01.tga", IMAGE_HUD_INVASION, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Interface\\HUD\\PersonalShopPurchase_I1.tga", IMAGE_PURCHARSE_CAPTION_1, GL_LINEAR);
	
	LoadBitmap("Interface\\cr_gr.TGA", IMAGE_BACK_FADED_R1);
	LoadBitmap("Interface\\cr_gr2.TGA", IMAGE_BACK_FADED_R2);
}

void SEASON3B::CNewUINameWindow::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

bool SEASON3B::CNewUINameWindow::UpdateMouseEvent()
{
	return true;
}

bool SEASON3B::CNewUINameWindow::UpdateKeyEvent()
{
#ifndef __ANDROID__
	if (SEASON3B::IsPress(VK_MENU) == true)
	{
		m_bShowItemName = !m_bShowItemName;
		//GMItemDropName->SetVisibleInfo(-1, m_bShowItemName);
	}
#endif

	return true;
}

bool SEASON3B::CNewUINameWindow::Update()
{
	return true;
}

bool SEASON3B::CNewUINameWindow::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	this->RenderName();

	RenderTimes();

	matchEvent::RenderMatchTimes();

	gmHeadChat->RenderBooleans();

	DrawPersonalShopTitleImp();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUINameWindow::RenderTooltip(float posX, float posY, float RenderSizeX, float RenderSizeY, bool fixed)
{
	if (fixed)
	{
		posX *= g_fScreenRate_x;
		posY *= g_fScreenRate_y;
		RenderSizeX *= g_fScreenRate_x;
		RenderSizeY *= g_fScreenRate_y;
	}

	jpexs::RenderBoxjpexs_dds(IMAGE_HUD_PLAYER, posX, posY, RenderSizeX, RenderSizeY);
}

void SEASON3B::CNewUINameWindow::RenderTooltip_old(float x, float y, float width, float height, bool fixed)
{
	glColor4f(0.0, 0.0, 0.0, 1.f);
	RenderColor(x - 1.f, y - 1.f, width + 2.f, 1.0, 0.0, 0, fixed);
	RenderColor(x - 1.f, y + height, width + 2.f, 1.0, 0.0, 0, fixed);
	RenderColor(x - 1.f, y - 1.f, 1.0, height + 1.f, 0.0, 0, fixed);
	RenderColor(x + width, y - 1.f, 1.0, height + 1.f, 0.0, 0, fixed);

	glColor4f(0.0, 0.0, 0.0, 0.8f);
	RenderColor(x, y, width, height, 0.0, 0, fixed);
	EndRenderColor();
}

void SEASON3B::CNewUINameWindow::RenderName()
{
	//if (g_bGMObservation == true)
	//{
		for (size_t i = 0; i < MAX_CHARACTERS_CLIENT; i++)
		{
			CHARACTER* pCharacter = gmCharacters->GetCharacter(i);
			OBJECT* o = &pCharacter->Object;

			if (o->Live && (o->Kind == KIND_PLAYER || o->Kind == KIND_MONSTER) && pCharacter->Dead == 0)
			{
#ifdef PLAYER_INFO_SYSTEM
				if (gmProtect->m_RenderCharacterName)
				{
					gmHeadChat->CreateChat(pCharacter->ID, "", pCharacter);
				}
				else
				{
					if (IsShopTitleVisible(pCharacter) == false)
					{
						gmHeadChat->CreateChat(pCharacter->ID, "", pCharacter);
					}
				}
#else
				if (IsShopTitleVisible(pCharacter) == false)
				{
					gmHeadChat->CreateChat(pCharacter->ID, "", pCharacter);
				}
#endif // PLAYER_INFO_SYSTEM
			}
		}
	//}

#ifndef GUILD_WAR_EVENT
	if (gMapManager->InChaosCastle() == true)
	{
		if (FindText(Hero->ID, "webzen") == false)
		{
			if (SelectedNpc != -1 || SelectedCharacter != -1)
			{
				return;
			}
		}
	}
#endif//GUILD_WAR_EVENT

	if (SelectedItem != -1 || SelectedNpc != -1 || SelectedCharacter != -1)
	{
		if (SelectedNpc != -1)
		{
			CHARACTER* c = gmCharacters->GetCharacter(SelectedNpc);
			OBJECT* o = &c->Object;
			gmHeadChat->CreateChat(c->ID, "", c);
		}
		else if (SelectedCharacter != -1)
		{
			CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);

			OBJECT* o = &c->Object;
			if (o->Kind == KIND_MONSTER)
			{
				g_pRenderText->SetTextColor(255, 230, 200, 255);
				g_pRenderText->SetBgColor(100, 0, 0, 255);
				g_pRenderText->RenderText((GetWindowsX / 2), 2, c->ID, 0, 0, RT3_WRITE_CENTER);
			}
			else if (!::IsStrifeMap(World) || Hero->m_byGensInfluence == c->m_byGensInfluence)
			{
				if (IsShopTitleVisible(c) == false)
				{
					gmHeadChat->CreateChat(c->ID, "", c);
				}
			}
		}
		else if (SelectedItem != -1)
		{
			RenderItemName(&Items[SelectedItem], SelectedItem, &Items[SelectedItem].Object, Items[SelectedItem].Item.Level, Items[SelectedItem].Item.Option1, Items[SelectedItem].Item.ExtOption, false);
		}
	}

	bool bShowAllItemNames = (m_bShowItemName || SEASON3B::IsRepeat(VK_MENU));
	#ifdef __ANDROID__
	bShowAllItemNames = true;
	#endif

	if (bShowAllItemNames)
	{
		for (int i = 0; i < MAX_ITEMS; i++)
		{
			OBJECT* o = &Items[i].Object;
			if (o->Live)
			{
				if (o->Visible && i != SelectedItem)
				{
					RenderItemName(&Items[i], i, o, Items[i].Item.Level, Items[i].Item.Option1, Items[i].Item.ExtOption, true);
				}
			}
		}
	}

	if (CharacterMachine->PowerLevelUp)
	{
		auto current_time = std::chrono::steady_clock::now();
		double difTime = std::chrono::duration<double>(current_time - CharacterMachine->last_time).count();

		double time_checked = 1.5;

		if (difTime >= time_checked)
		{
			CharacterMachine->PowerLevelUp = false;
			CharacterMachine->last_time = current_time;
		}

		float Alpha = (float)(1.f - (difTime / time_checked));
		float MoveY = (float)(60.f * (1.f - (difTime / time_checked)));

		glColor4f(1.f, 1.f, 1.f, Alpha);

		vec3_t Position;
		int ScreenX = 0, ScreenY = 0;

		OBJECT* pObj = &Hero->Object;
		Vector(pObj->Position[0], pObj->Position[1], pObj->Position[2] + pObj->BoundingBoxMax[2] + 100.f, Position);

		Projection(Position, &ScreenX, &ScreenY);

		ScreenY = ScreenY + (Alpha * 50);

		RenderNumberHQ(ScreenX + 6.f, ScreenY - 8.f, CharacterMachine->ATTKPowerValue, 7.f, 10.f);
		RenderBitmap(BITMAP_TEXT_POWER, ScreenX-5, ScreenY, 60.f, 28.f, 0.0, 0.0, 1.f, 118.f / 128.f, true, true, 0.0);
	}
}

float SEASON3B::CNewUINameWindow::GetLayerDepth()
{
	return 1.0f;
}

