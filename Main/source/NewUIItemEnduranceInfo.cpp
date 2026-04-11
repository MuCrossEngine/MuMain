// NewUIItemEnduranceInfo.cpp: implementation of the CNewUIItemEnduranceInfo class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIItemEnduranceInfo.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "CharacterManager.h"
#include "MapManager.h"

#ifdef PJH_FIX_SPRIT
#include "GIPetManager.h"
#endif //PJH_FIX_SPRIT
#include "CGMProtect.h"

extern float g_fScreenRate_x;

using namespace SEASON3B;

CNewUIItemEnduranceInfo::CNewUIItemEnduranceInfo()
{
	memset(&m_UIStartPos, 0, sizeof(POINT));
	memset(&m_ItemDurUIStartPos, 0, sizeof(POINT));
	m_iTextEndPosX = 0;
	m_iCurArrowType = ARROWTYPE_NONE;
	m_iTooltipIndex = -1;
	m_iHideMap = 0;
}

CNewUIItemEnduranceInfo::~CNewUIItemEnduranceInfo()
{
	Release();
}

bool SEASON3B::CNewUIItemEnduranceInfo::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_ITEM_ENDURANCE_INFO, this);

	SetPos(x, y);
	LoadImages();
	InitImageIndex();
	Show(true);
	return true;
}

void SEASON3B::CNewUIItemEnduranceInfo::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIItemEnduranceInfo::SetPos(int x, int y)
{
	m_UIStartPos.x = x;
	m_UIStartPos.y = y;

	m_ItemDurUIStartPos.x = GetScreenWidth() - ITEM_DUR_WIDTH - 2;
	m_ItemDurUIStartPos.y = 140;

	m_iTextEndPosX = m_UIStartPos.x + PETHP_FRAME_WIDTH;
}

void SEASON3B::CNewUIItemEnduranceInfo::SetPos(int x)
{
	m_ItemDurUIStartPos.x = x - ITEM_DUR_WIDTH - 2;
	m_ItemDurUIStartPos.y = 140;
}

bool SEASON3B::CNewUIItemEnduranceInfo::UpdateMouseEvent()
{
	if (true == BtnProcess())
		return false;


#if MAIN_UPDATE > 303
	int iNextPosY = m_UIStartPos.y;

	if (Hero->Helper.Type >= MODEL_HELPER && Hero->Helper.Type <= MODEL_HELPER + 4
		|| Hero->Helper.Type == MODEL_HELPER + 64
		|| Hero->Helper.Type == MODEL_HELPER + 65
		|| Hero->Helper.Type == MODEL_HELPER + 67
		|| Hero->Helper.Type == MODEL_HELPER + 80
		|| Hero->Helper.Type == MODEL_HELPER + 106
		|| Hero->Helper.Type == MODEL_HELPER + 123
		|| Hero->Helper.Type == MODEL_HELPER + 37)
	{
		if (CheckMouseIn(m_UIStartPos.x, iNextPosY, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT))
			return false;

		iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
	}

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD)
	{
		if (Hero->m_pPet != NULL)
		{
			if (CheckMouseIn(m_UIStartPos.x, iNextPosY, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT))
				return false;

			iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
		}
	}

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		if (SummonLife > 0)
		{
			if (CheckMouseIn(m_UIStartPos.x, iNextPosY, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT))
				return false;

			iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
		}
	}
#else
	if (runtime_action(1))
	{
		return false;
	}
#endif

	bool bRenderRingWarning = false;
	int	icntItemDurIcon = 0;
	POINT ItemDurPos = POINT(m_ItemDurUIStartPos);

	m_iTooltipIndex = -1;

	for (int i = EQUIPMENT_WEAPON_RIGHT; i < MAX_EQUIPMENT; ++i)
	{
		ITEM* pItem = &CharacterMachine->Equipment[i];

		if ((pItem->bPeriodItem == true) && (pItem->bExpiredPeriod == false))
		{
			continue;
		}

		if (i == EQUIPMENT_HELPER)
		{
			continue;
		}

		if (pItem->Type == -1)
		{
			continue;
		}

		if (i == EQUIPMENT_WEAPON_RIGHT)
		{
			if (pItem->Type == ITEM_BOW + 15)
			{
				continue;
			}
		}
		else if (i == EQUIPMENT_WEAPON_LEFT)
		{
			if (pItem->Type == ITEM_BOW + 7)
			{
				continue;
			}
		}

		int iLevel = (pItem->Level >> 3) & 15;

		if (i == EQUIPMENT_RING_LEFT || i == EQUIPMENT_RING_RIGHT)
		{
			if (pItem->Type == ITEM_HELPER + 20 && iLevel == 1
				|| iLevel == 2)
			{
				continue;
			}
		}


		Script_Item* pItemAtt = GMItemMng->find(pItem->Type);
		int iMaxDurability = calcMaxDurability(pItem, pItemAtt, iLevel);

		if (pItem->Durability <= iMaxDurability * 0.5f)
		{
			if (i == EQUIPMENT_RING_RIGHT)
			{
				if (CheckMouseIn(ItemDurPos.x, ItemDurPos.y, ITEM_DUR_WIDTH / 2, ITEM_DUR_HEIGHT))
				{
					m_iTooltipIndex = i;
					return false;
				}
				bRenderRingWarning = true;
			}
			else if (i == EQUIPMENT_RING_LEFT)
			{
				if (CheckMouseIn(ItemDurPos.x + (ITEM_DUR_WIDTH / 2), ItemDurPos.y, ITEM_DUR_WIDTH / 2, ITEM_DUR_HEIGHT))
				{
					m_iTooltipIndex = i;
					return false;
				}
				bRenderRingWarning = false;
			}
			else
			{
				if (CheckMouseIn(ItemDurPos.x, ItemDurPos.y, ITEM_DUR_WIDTH, ITEM_DUR_HEIGHT))
				{
					m_iTooltipIndex = i;
					return false;
				}
			}

			if (bRenderRingWarning == false)
			{
				icntItemDurIcon++;
				ItemDurPos.y += (ITEM_DUR_HEIGHT + UI_INTERVAL_WIDTH);

				if (icntItemDurIcon % 2 == 0)
				{
					ItemDurPos.y = m_ItemDurUIStartPos.y;
					ItemDurPos.x -= (ITEM_DUR_WIDTH + UI_INTERVAL_WIDTH);
				}
			}

		}


	}
	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::Update()
{
	if (!IsVisible())
		return true;

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		if (gCharacterManager.GetEquipedBowType() == BOWTYPE_BOW)
		{
			m_iCurArrowType = ARROWTYPE_BOW;
		}
		else if (gCharacterManager.GetEquipedBowType() == BOWTYPE_CROSSBOW)
		{
			m_iCurArrowType = ARROWTYPE_CROSSBOW;
		}
		else
		{
			m_iCurArrowType = ARROWTYPE_NONE;
		}
	}

	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::Render()
{
	EnableAlphaTest();
	glColor3f(1.f, 1.f, 1.f);
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	RenderLeft();
	RenderRight();
	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIItemEnduranceInfo::RenderLeft()
{
#if MAIN_UPDATE > 303
	// Todo
	if (gmProtect->checkold_school())
	{
		if (GetScreenWidth() == ((int)GetWindowsX))
		{
			if (g_pNewUIMiniMap->IsSuccess() && !m_iHideMap)
			{
				m_UIStartPos.x = GetScreenWidth() - 184;
			}
			else
			{
				m_UIStartPos.x = GetScreenWidth() - 56;
			}
		}
		else
		{
			m_UIStartPos.x = GetScreenWidth() - 56;
		}
		if (PartyNumber >= 1)
		{
			m_UIStartPos.x = (g_pPartyListWindow->GetPosX() - 56);
		}
	}
	this->RenderLeft(m_UIStartPos.x, m_UIStartPos.y);

#else
	runtime_action(0);
#endif

}

void SEASON3B::CNewUIItemEnduranceInfo::RenderRight()
{
	RenderItemEndurance(m_ItemDurUIStartPos.x, m_ItemDurUIStartPos.y);

	//#if MAIN_UPDATE > 303
	runtime_render_map();
	//#endif
}

float SEASON3B::CNewUIItemEnduranceInfo::RenderLeft(float RenderFrameX, float RenderFrameY)
{
	float iNextPosY = 0.0;

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		if (RenderNumArrow(RenderFrameX, RenderFrameY + iNextPosY))
		{
			iNextPosY += (UI_INTERVAL_HEIGHT + 10);
		}
	}

	if (RenderEquipedHelperLife(RenderFrameX, RenderFrameY + iNextPosY))
	{
		iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
	}

	if (RenderEquipedHelperLife(&Hero->MuunHelper[0], 237, RenderFrameX, RenderFrameY + iNextPosY))
	{
		iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
	}

	if (RenderEquipedHelperLife(&Hero->MuunHelper[1], 238, RenderFrameX, RenderFrameY + iNextPosY))
	{
		iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
	}


	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD)
	{
		if (RenderEquipedPetLife(RenderFrameX, RenderFrameY + iNextPosY))
		{
			iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
		}
	}

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		if (RenderSummonMonsterLife(RenderFrameX, RenderFrameY + iNextPosY))
		{
			iNextPosY += (UI_INTERVAL_HEIGHT + PETHP_FRAME_HEIGHT);
		}
	}

	if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton != 2)
	{
		EnableAlphaTest();
		glColor3f(1.f, 1.f, 1.f);
		RenderImageF(BITMAP_HERO_POSITION_INFO_BEGIN + 6, m_UIStartPos.x, iNextPosY + 4, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT);
	}

	return iNextPosY;
}

#if MAIN_UPDATE <= 303
bool SEASON3B::CNewUIItemEnduranceInfo::runtime_action(int Action)
{
	float RenderFrameX = 0.f;
	float RenderFrameY = 12.f;
	static bool draw_info[3] = { false, false, false };

	for (int i = 0; i < 3; i++)
	{
		if (runtime_action_draw(i, RenderFrameX, RenderFrameY, 100.f, &draw_info[i], Action) && Action == 1)
		{
			return true;
		}
	}

	return false;
}

bool SEASON3B::CNewUIItemEnduranceInfo::runtime_action_draw(int index, float& RenderFrameX, float& RenderFrameY, float RenderSizeX, bool* bEnable, int Action)
{
	static float RenderSizeY[] = {0.f, 0.f, 0.f, 0.f, };

	float AddExpansion = 18;

	if (!(*bEnable) && (index == 0 || index == 1 || index == 2))
	{
		RenderSizeY[index] = 0;
	}

	if (Action == 0)
	{
		//TextGlobal[];
		char* text_name[] = { "Pet's" , "Buff's" , "Party's" , };

		g_pRenderText->SetFont(g_hFontBold);
		RenderImageF(IMAGE_BACK_FADED_R1, RenderFrameX, RenderFrameY, RenderSizeX, 15.0, 0.0, 0.0, 130.f, 126.0);
		RenderBtns(RenderFrameX + 2, RenderFrameY + 2, 11.0, 11.0, (*bEnable) ? 4 : 3);

		g_pRenderText->RenderText(RenderFrameX + 18, RenderFrameY, text_name[index], 0, 15);

		if ((*bEnable) && index == 0)
		{
			RenderSizeY[index] = this->RenderLeft(RenderFrameX + 4.f, RenderFrameY + AddExpansion);
		}
		else if (index == 1)
		{
			if (*bEnable)
			{
				g_pBuffWindow->SetPos(RenderFrameX + 4.f, RenderFrameY + AddExpansion);
				RenderSizeY[index] = 48;
			}
			else
			{
				g_pBuffWindow->SetPos(RenderFrameX + 4.f, -100);
			}
		}
		else if (index == 2)
		{
			if (*bEnable)
			{
				g_pPartyListWindow->SetPos(RenderFrameX + 4.f, RenderFrameY + AddExpansion);
				RenderSizeY[index] = (19 * PartyNumber);
			}
			else
			{
				g_pPartyListWindow->SetPos(-200, RenderFrameY + AddExpansion);
			}
		}
	}
	if (Action == 1)
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 2, RenderFrameY + 2, 11.0, 11.0))
		{
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				*bEnable = !*bEnable;
			}
			return true;
		}

		if ((*bEnable) && index == 0)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 4.f, RenderFrameY + AddExpansion, PETHP_FRAME_WIDTH, RenderSizeY[index]))
			{
				return true;
			}
		}
	}

	RenderFrameY += AddExpansion + RenderSizeY[index];

	return false;
}
#endif

bool SEASON3B::CNewUIItemEnduranceInfo::BtnProcess()
{
	return this->UpdateMouseWorld();
}

float SEASON3B::CNewUIItemEnduranceInfo::GetLayerDepth()
{
	return 2.5f;
}

void SEASON3B::CNewUIItemEnduranceInfo::OpenningProcess()
{

}

void SEASON3B::CNewUIItemEnduranceInfo::ClosingProcess()
{

}

bool SEASON3B::CNewUIItemEnduranceInfo::IsHideInterfaceWorld()
{
	return m_iHideMap;
}

void SEASON3B::CNewUIItemEnduranceInfo::HideInterfaceWorld(bool success)
{
	m_iHideMap = !success;
}

void SEASON3B::CNewUIItemEnduranceInfo::InitImageIndex()
{
	m_iItemDurImageIndex[EQUIPMENT_WEAPON_RIGHT] = IMAGE_ITEM_DUR_WEAPON;
	m_iItemDurImageIndex[EQUIPMENT_WEAPON_LEFT] = IMAGE_ITEM_DUR_SHIELD;
	m_iItemDurImageIndex[EQUIPMENT_HELM] = IMAGE_ITEM_DUR_CAP;
	m_iItemDurImageIndex[EQUIPMENT_ARMOR] = IMAGE_ITEM_DUR_UPPER;
	m_iItemDurImageIndex[EQUIPMENT_PANTS] = IMAGE_ITEM_DUR_LOWER;
	m_iItemDurImageIndex[EQUIPMENT_GLOVES] = IMAGE_ITEM_DUR_GLOVES;
	m_iItemDurImageIndex[EQUIPMENT_BOOTS] = IMAGE_ITEM_DUR_BOOTS;
	m_iItemDurImageIndex[EQUIPMENT_WING] = IMAGE_ITEM_DUR_WING;
	m_iItemDurImageIndex[EQUIPMENT_HELPER] = -1;
	m_iItemDurImageIndex[EQUIPMENT_AMULET] = IMAGE_ITEM_DUR_NECKLACE;
	m_iItemDurImageIndex[EQUIPMENT_RING_RIGHT] = IMAGE_ITEM_DUR_RING;
	m_iItemDurImageIndex[EQUIPMENT_RING_LEFT] = IMAGE_ITEM_DUR_RING;
}

void SEASON3B::CNewUIItemEnduranceInfo::RenderHPUI(int iX, int iY, unicode::t_char* pszName, int iLife, int iMaxLife/*=255*/, bool bWarning/*=false*/)
{
	EnableAlphaTest();
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	// HPUI_FRAME	
	if (bWarning == false)
		glColor4f(0.f, 0.f, 0.f, 0.7f);
	else
		glColor4f(0.2f, 0.f, 0.f, 0.7f);

#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		if (bWarning == false)
			glColor4f(0.1, 0.1, 0.1, 0.69999999);

		RenderColor(iX, iY, 50, 19);


		glColor4f(0.19607843, 0.039215688, 0.0, 1.0);
		RenderColor(iX + 2.f, iY + 12.f, 43.0, 4.0);

		int iStepHP = ((float)iLife / (float)iMaxLife) * 10;

		glColor4f(0.98039216, 0.039215688, 0.0, 1.0);

		if (iStepHP > 0)
		{
			float v130 = iX + 4.f;
			do
			{
				RenderColor(v130, iY + 13.f, 3.0, 2.0);
				--iStepHP;
				v130 += 4;
			} while (iStepHP);
		}

		EndRenderColor();
		EnableAlphaTest();

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->RenderText(iX + 3.f, iY + 2.f, pszName, 0, 0, RT3_SORT_LEFT);
	}
	else
	{
		RenderColor(iX + 2, iY + 2, PETHP_FRAME_WIDTH - 4, PETHP_FRAME_HEIGHT - 10);
		EndRenderColor();

		glColor4f(1.f, 1.f, 1.f, 1.f);
		RenderImage(IMAGE_PETHP_FRAME, iX, iY, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT);

		float fLife = ((float)iLife / (float)iMaxLife) * (float)PETHP_BAR_WIDTH;
		RenderImage(IMAGE_PETHP_BAR, iX + 4, iY + PETHP_FRAME_HEIGHT - PETHP_BAR_HEIGHT - 4, fLife, PETHP_BAR_HEIGHT);
		g_pRenderText->RenderText(iX + (PETHP_FRAME_WIDTH / 2), iY + 5, pszName, 0, 0, RT3_WRITE_CENTER);
	}
#else
	if (bWarning == false)
		glColor4f(0.1, 0.1, 0.1, 0.69999999);

	RenderColor(iX, iY, PETHP_FRAME_WIDTH, PETHP_FRAME_HEIGHT);


	glColor4f(0.19607843, 0.039215688, 0.0, 1.0);
	RenderColor(iX + 2.f, iY + 12.f, 43.0, 4.0);

	int iStepHP = ((float)iLife / (float)iMaxLife) * 10;

	glColor4f(0.98039216, 0.039215688, 0.0, 1.0);

	if (iStepHP > 0)
	{
		float v130 = iX + 4.f;

		do
		{
			RenderColor(v130, iY + 13.f, 3.0, 2.0);
			--iStepHP;
			v130 += 4;
		} while (iStepHP);
	}

	EndRenderColor();
	EnableAlphaTest();

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->RenderText(iX + 3.f, iY + 2.f, pszName, 0, 0, RT3_SORT_LEFT);
#endif
}

void SEASON3B::CNewUIItemEnduranceInfo::RenderTooltip(int iX, int iY, const ITEM* pItem, const DWORD& dwTextColor)
{
	Script_Item* pItemAtt = GMItemMng->find(pItem->Type);
	SIZE TextSize = { 0, 0 };

	int iLevel = (pItem->Level >> 3) & 15;
	int iMaxDurability = calcMaxDurability(pItem, pItemAtt, iLevel);

	unicode::t_char szText[256] = { NULL, };
	unicode::_sprintf(szText, "%s (%d/%d)", pItemAtt->Name, pItem->Durability, iMaxDurability);
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), szText, 1, &TextSize);

	g_pRenderText->SetBgColor(0, 0, 0, 128);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(dwTextColor);
	int iTextlen = unicode::_strlen(szText);

	int iTooltipWidth = (/*TextSize.cx*/7 * iTextlen) / g_fScreenRate_x;

	if (iX + (iTooltipWidth / 2) > GetScreenWidth())
	{
		iX = GetScreenWidth() - (iTooltipWidth / 2);
	}

	g_pRenderText->RenderText(iX, iY, szText, 0, 0, RT3_WRITE_CENTER);

}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderEquipedHelperLife(int iX, int iY)
{
	if (Hero->Helper.Type >= MODEL_HELPER && Hero->Helper.Type <= MODEL_HELPER + 4
		|| Hero->Helper.Type == MODEL_HELPER + 64
		|| Hero->Helper.Type == MODEL_HELPER + 65
		|| Hero->Helper.Type == MODEL_HELPER + 67
		|| Hero->Helper.Type == MODEL_HELPER + 80
		|| Hero->Helper.Type == MODEL_HELPER + 106
		|| Hero->Helper.Type == MODEL_HELPER + 123
		|| Hero->Helper.Type == MODEL_HELPER + 37)
	{
		unicode::t_char szText[256] = { NULL, };

		switch (Hero->Helper.Type)
		{
		case MODEL_HELPER:
		{
			unicode::_sprintf(szText, GlobalText[353]);
		}
		break;
		case MODEL_HELPER + 1:
		{
			Script_Item* p = GMItemMng->find(Hero->Helper.Type - MODEL_SWORD);
			unicode::_sprintf(szText, p->Name);
		}
		break;
		case MODEL_HELPER + 2:
		{
			unicode::_sprintf(szText, GlobalText[355]);
		}
		break;
		case MODEL_HELPER + 3:
		{
			unicode::_sprintf(szText, GlobalText[354]);
		}
		break;
		case MODEL_HELPER + 4:
		{
			unicode::_sprintf(szText, GlobalText[1187]);
		}
		break;
		case MODEL_HELPER + 37:
		{
			unicode::_sprintf(szText, GlobalText[1916]);
		}
		break;
		case MODEL_HELPER + 64:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 64)->Name);
		}
		break;
		case MODEL_HELPER + 65:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 65)->Name);
		}
		break;
		case MODEL_HELPER + 67:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 67)->Name);
		}
		break;
		case MODEL_HELPER + 80:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 80)->Name);
		}
		break;
		case MODEL_HELPER + 106:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 106)->Name);
		}
		break;
		case MODEL_HELPER + 123:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 123)->Name);
		}
		break;
		}

		int iLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;

		RenderHPUI(iX, iY, szText, iLife);

		return true;
	}
	else if (Hero->Helper.Type != -1)
	{
		int iLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;

		unicode::t_char szText[256] = { NULL, };

		unicode::_sprintf(szText, GMItemMng->find(Hero->Helper.Type - MODEL_ITEM)->Name);

		RenderHPUI(iX, iY, szText, iLife);

		return true;
	}

	return false;
}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderEquipedHelperLife(PART_t* f, int index, int iX, int iY)
{
	if (CharacterMachine->CheckEquipment(index) == false)
	{
		return false;
	}

	if (f->Type >= MODEL_HELPER && f->Type <= MODEL_HELPER + 4
		|| f->Type == MODEL_HELPER + 64
		|| f->Type == MODEL_HELPER + 65
		|| f->Type == MODEL_HELPER + 67
		|| f->Type == MODEL_HELPER + 80
		|| f->Type == MODEL_HELPER + 106
		|| f->Type == MODEL_HELPER + 123
		|| f->Type == MODEL_HELPER + 37)
	{
		unicode::t_char szText[256] = { NULL, };

		switch (f->Type)
		{
		case MODEL_HELPER:
		{
			unicode::_sprintf(szText, GlobalText[353]);
		}
		break;
		case MODEL_HELPER + 1:
		{
			Script_Item* p = GMItemMng->find(f->Type - MODEL_SWORD);
			unicode::_sprintf(szText, p->Name);
		}
		break;
		case MODEL_HELPER + 2:
		{
			unicode::_sprintf(szText, GlobalText[355]);
		}
		break;
		case MODEL_HELPER + 3:
		{
			unicode::_sprintf(szText, GlobalText[354]);
		}
		break;
		case MODEL_HELPER + 4:
		{
			unicode::_sprintf(szText, GlobalText[1187]);
		}
		break;
		case MODEL_HELPER + 37:
		{
			unicode::_sprintf(szText, GlobalText[1916]);
		}
		break;
		case MODEL_HELPER + 64:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 64)->Name);
		}
		break;
		case MODEL_HELPER + 65:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 65)->Name);
		}
		break;
		case MODEL_HELPER + 67:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 67)->Name);
		}
		break;
		case MODEL_HELPER + 80:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 80)->Name);
		}
		break;
		case MODEL_HELPER + 106:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 106)->Name);
		}
		break;
		case MODEL_HELPER + 123:
		{
			unicode::_sprintf(szText, GMItemMng->find(ITEM_HELPER + 123)->Name);
		}
		break;
		}

		int iLife = CharacterMachine->GetOnlyEquipment(index)->Durability;

		RenderHPUI(iX, iY, szText, iLife);

		return true;
	}
	else if (f->Type != -1)
	{
		int iLife = CharacterMachine->GetOnlyEquipment(index)->Durability;

		unicode::t_char szText[256] = { NULL, };

		unicode::_sprintf(szText, GMItemMng->find(f->Type - MODEL_ITEM)->Name);

		RenderHPUI(iX, iY, szText, iLife);

		return true;
	}
	return false;
}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderEquipedPetLife(int iX, int iY)
{
	if (Hero->m_pPet == NULL)
		return false;

	unicode::t_char szText[256] = { NULL, };
	unicode::_sprintf(szText, GlobalText[1214]);

	int iLife = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Durability;

	RenderHPUI(iX, iY, szText, iLife);
	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderSummonMonsterLife(int iX, int iY)
{
	if (SummonLife <= 0)
		return false;

	unicode::t_char szText[256] = { NULL, };
	unicode::_sprintf(szText, GlobalText[356]);

	RenderHPUI(iX, iY, szText, SummonLife, 100);

	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderNumArrow(int iX, int iY)
{
	if (m_iCurArrowType == ARROWTYPE_NONE)
		return false;

	unicode::t_char szText[256] = { NULL, };
	int iNumEquipedArrowDurability = 0;
	int iNumArrowSetInInven = 0;

	if (m_iCurArrowType == ARROWTYPE_BOW)
	{
		iNumArrowSetInInven = g_pMyInventory->GetNumItemByType(ARROWTYPE_BOW);

		if (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type == ITEM_BOW + 15)
		{
			iNumEquipedArrowDurability = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Durability;
		}

		if ((iNumArrowSetInInven == 0) && (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type != ITEM_BOW + 15))
			return false;

		unicode::_sprintf(szText, GlobalText[351], iNumEquipedArrowDurability, iNumArrowSetInInven);
	}
	else if (m_iCurArrowType == ARROWTYPE_CROSSBOW)
	{
		iNumArrowSetInInven = g_pMyInventory->GetNumItemByType(ARROWTYPE_CROSSBOW);

		if (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type == ITEM_BOW + 7)
		{
			iNumEquipedArrowDurability = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Durability;
		}

		if ((iNumArrowSetInInven == 0) && (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type != ITEM_BOW + 7))
			return false;

		unicode::_sprintf(szText, GlobalText[352], iNumEquipedArrowDurability, iNumArrowSetInInven);
	}

	g_pRenderText->SetBgColor(0, 0, 0, 180);
	g_pRenderText->SetTextColor(255, 160, 0, 255);
	g_pRenderText->RenderText(iX, iY, szText, 0, 0, RT3_SORT_LEFT);

	return true;
}

bool SEASON3B::CNewUIItemEnduranceInfo::RenderItemEndurance(int ix, int iY)
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE))
		return false;

	glColor4f(1.f, 1.f, 1.f, 1.f);


	POINT ItemDurPos = POINT(m_ItemDurUIStartPos);
	int icntItemDurIcon = 0;
	int iTempImageIndex;
	bool bRenderRingWarning = false;

	for (int i = EQUIPMENT_WEAPON_RIGHT; i < MAX_EQUIPMENT; ++i)
	{
		ITEM* pItem = &CharacterMachine->Equipment[i];

		iTempImageIndex = m_iItemDurImageIndex[i];

		if ((pItem->bPeriodItem == true) && (pItem->bExpiredPeriod == false))
		{
			continue;
		}

		if (i == EQUIPMENT_HELPER)
		{
			continue;
		}

		if (pItem->Type == -1)
		{
			continue;
		}

		if (i == EQUIPMENT_WEAPON_RIGHT)
		{
			if (pItem->Type == ITEM_BOW + 15)
			{
				continue;
			}
		}
		else if (i == EQUIPMENT_WEAPON_LEFT)
		{
			if (gCharacterManager.GetEquipedBowType(pItem) == BOWTYPE_BOW)
			{
				iTempImageIndex = m_iItemDurImageIndex[EQUIPMENT_WEAPON_RIGHT];
			}

			if (pItem->Type == ITEM_BOW + 7)
			{
				continue;
			}
		}

		int iLevel = (pItem->Level >> 3) & 15;

		if (i == EQUIPMENT_RING_LEFT || i == EQUIPMENT_RING_RIGHT)
		{
			if (pItem->Type == ITEM_HELPER + 20 && iLevel == 1
				|| iLevel == 2)
			{
				continue;
			}
		}

		Script_Item* pItemAtt = GMItemMng->find(pItem->Type);
		int iMaxDurability = calcMaxDurability(pItem, pItemAtt, iLevel);

		if (pItem->Durability > iMaxDurability * 0.5f)
		{
			continue;
		}

		EnableAlphaTest();
		glColor4f(1.f, 1.f, 1.f, 1.f);

		if (i != EQUIPMENT_RING_LEFT || bRenderRingWarning != true)
		{
			RenderImage(m_iItemDurImageIndex[i], ItemDurPos.x, ItemDurPos.y, ITEM_DUR_WIDTH, ITEM_DUR_HEIGHT);
		}

		if (i == EQUIPMENT_RING_RIGHT)
		{
			bRenderRingWarning = true;
		}

		if (pItem->Durability == 0)
		{
			glColor4f(1.f, 0.0f, 0.f, 0.5f);
		}
		else if (pItem->Durability <= iMaxDurability * 0.2f)
		{
			glColor4f(1.f, 0.2f, 0.0f, 0.5f);
		}
		else if (pItem->Durability <= iMaxDurability * 0.3f)
		{
			glColor4f(1.0f, 0.5f, 0.f, 0.5f);
		}
		else if (pItem->Durability <= iMaxDurability * 0.5f)
		{
			glColor4f(1.f, 1.f, 0.f, 0.5f);
		}

		if (i == EQUIPMENT_RING_RIGHT)
		{
			RenderColor(ItemDurPos.x, ItemDurPos.y, ITEM_DUR_WIDTH / 2, ITEM_DUR_HEIGHT);
		}
		else if (i == EQUIPMENT_RING_LEFT)
		{
			RenderColor(ItemDurPos.x + (ITEM_DUR_WIDTH / 2), ItemDurPos.y, ITEM_DUR_WIDTH / 2, ITEM_DUR_HEIGHT);
			bRenderRingWarning = false;
		}
		else
		{
			RenderColor(ItemDurPos.x, ItemDurPos.y, ITEM_DUR_WIDTH, ITEM_DUR_HEIGHT);
		}

		EndRenderColor();
		DisableAlphaBlend();

		if (bRenderRingWarning == false)
		{
			icntItemDurIcon++;
			ItemDurPos.y += (ITEM_DUR_HEIGHT + UI_INTERVAL_WIDTH);

			if (icntItemDurIcon % 2 == 0)
			{
				ItemDurPos.y = m_ItemDurUIStartPos.y;
				ItemDurPos.x -= (ITEM_DUR_WIDTH + UI_INTERVAL_WIDTH);
			}
		}
	}

	if (m_iTooltipIndex != -1)
	{
		ITEM* pItem = &CharacterMachine->Equipment[m_iTooltipIndex];
		DWORD dwColor = 0xFFFFFFFF;

		Script_Item* pItemAttr = GMItemMng->find(pItem->Type);
		int iLevel = (pItem->Level >> 3) & 15;
		int iMaxDurability = calcMaxDurability(pItem, pItemAttr, iLevel);

		if (pItem->Durability <= (iMaxDurability * 0.5f))
		{
			if (pItem->Durability <= 0)
			{
				dwColor = 0xFF0000FF;
			}
			else if (pItem->Durability <= (iMaxDurability * 0.2f))
			{
				dwColor = 0xFF0053FF;
			}
			else if (pItem->Durability <= (iMaxDurability * 0.3f))
			{
				dwColor = 0xFF00A8FF;
			}
			else if (pItem->Durability <= (iMaxDurability * 0.5f))
			{
				dwColor = 0xFF00FFFF;
			}

			RenderTooltip(MouseX, MouseY - 10, pItem, dwColor);
		}
	}
	return true;
}

void SEASON3B::CNewUIItemEnduranceInfo::LoadImages()
{
	LoadBitmap("Interface\\newui_Pet_Back.tga", IMAGE_PETHP_FRAME, GL_LINEAR);
	LoadBitmap("Interface\\newui_Pet_HpBar.jpg", IMAGE_PETHP_BAR, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_boots.tga", IMAGE_ITEM_DUR_BOOTS, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_cap.tga", IMAGE_ITEM_DUR_CAP, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_gloves.tga", IMAGE_ITEM_DUR_GLOVES, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_lower.tga", IMAGE_ITEM_DUR_LOWER, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_necklace.tga", IMAGE_ITEM_DUR_NECKLACE, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_ring.tga", IMAGE_ITEM_DUR_RING, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_shield.tga", IMAGE_ITEM_DUR_SHIELD, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_upper.tga", IMAGE_ITEM_DUR_UPPER, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_weapon.tga", IMAGE_ITEM_DUR_WEAPON, GL_LINEAR);
	LoadBitmap("Interface\\newui_durable_wing.tga", IMAGE_ITEM_DUR_WING, GL_LINEAR);

	LoadBitmap("Interface\\HUD\\newui_map_back.tga", IMAGE_HUB_WORLD_TAR, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\newui_map_user.tga", IMAGE_HUB_WORLD_USER, GL_LINEAR);
}

void SEASON3B::CNewUIItemEnduranceInfo::UnloadImages()
{
	DeleteBitmap(IMAGE_PETHP_FRAME);
	DeleteBitmap(IMAGE_PETHP_BAR);
	DeleteBitmap(IMAGE_ITEM_DUR_BOOTS);
	DeleteBitmap(IMAGE_ITEM_DUR_CAP);
	DeleteBitmap(IMAGE_ITEM_DUR_GLOVES);
	DeleteBitmap(IMAGE_ITEM_DUR_LOWER);
	DeleteBitmap(IMAGE_ITEM_DUR_NECKLACE);
	DeleteBitmap(IMAGE_ITEM_DUR_RING);
	DeleteBitmap(IMAGE_ITEM_DUR_SHIELD);
	DeleteBitmap(IMAGE_ITEM_DUR_UPPER);
	DeleteBitmap(IMAGE_ITEM_DUR_WEAPON);
	DeleteBitmap(IMAGE_ITEM_DUR_WING);

	DeleteBitmap(IMAGE_HUB_WORLD_TAR);
	DeleteBitmap(IMAGE_HUB_WORLD_USER);
}


void SEASON3B::CNewUIItemEnduranceInfo::runtime_render_fps(float x, float y, float RenderSizeX)
{
	unicode::t_char szDebugText[128];

	RenderImageF(IMAGE_BACK_FADED_R2, x, y, RenderSizeX, 15.0, 0.0, 0.0, 130.f, 126.0);

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(CLRDW_GOLD);

	unicode::_sprintf(szDebugText, "FPS: %.0f", FPS);

	g_pRenderText->RenderFont(x + 70, y, szDebugText, 0, 15.0, RT3_SORT_LEFT);

	unicode::_sprintf(szDebugText, "RTT: %d ms", gsteady_clock->get_ping_time());

	g_pRenderText->RenderFont(x + 100, y, szDebugText, 0, 15.0, RT3_SORT_LEFT);
}

void SEASON3B::CNewUIItemEnduranceInfo::runtime_render_map()
{
	EnableAlphaTest();
	glColor3f(1.f, 1.f, 1.f);

	float RenderFrameX = GetWindowsX - 128;

	if (GetScreenWidth() == ((int)GetWindowsX))
	{
		float RenderFrameY = 0.0;
		if (g_pNewUIMiniMap->IsSuccess())
		{
			if (m_iHideMap)
			{
				runtime_render_fps((RenderFrameX - 23.f), RenderFrameY, 136.f);
				//RenderFrameY -= 16.f;
			}
			else
			{
				runtime_render_fps((RenderFrameX - 136.f), RenderFrameY, 136.f);
				//RenderFrameY -= 16.f;

				if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP) == false)
				{
					g_pNewUIMiniMap->runtime_render_map(false);
				}

				RenderBtns(RenderFrameX, RenderFrameY, 15.0, 15.0, 0);
				RenderBtns(RenderFrameX + 15.0, RenderFrameY, 15.0, 15.0, 1);
				RenderBtns(RenderFrameX + 30.0, RenderFrameY, 83.0, 15.0, 2);

				g_pRenderText->SetFont(g_hFont);
				g_pRenderText->SetBgColor(0);
				g_pRenderText->SetTextColor(-1);
				g_pRenderText->RenderText(RenderFrameX + 30.0, RenderFrameY, gMapManager->GetMapName(), 83, 15, RT3_SORT_CENTER);
			}

			RenderBtns(RenderFrameX + 113, RenderFrameY, 15.0, 15.0, 3);
		}
		else
		{
			runtime_render_fps(RenderFrameX - 136.f, RenderFrameY, 136.f);
		}
	}
}

bool SEASON3B::CNewUIItemEnduranceInfo::UpdateMouseWorld()
{
	if (GetScreenWidth() == ((int)GetWindowsX) && g_pNewUIMiniMap->IsSuccess())
	{
		float RenderFrameY = 0.0f;
		float RenderFrameX = GetWindowsX - 128;

		if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 128, 15.f + 128))
		{
			if (!this->IsHideInterfaceWorld() && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP) == false)
			{

#if MAIN_UPDATE > 303
				if (g_pPartyListWindow->GetPosX() >= RenderFrameX)
				{
					g_pPartyListWindow->SetPos(RenderFrameX);
				}
#endif

				if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 15.f, 15.f) && SEASON3B::IsRelease(VK_LBUTTON))
				{
					g_pNewUIMiniMap->Zoom(-0.5f);
					return true;
				}
				if (SEASON3B::CheckMouseIn(RenderFrameX + 15.f, RenderFrameY, 15.f, 15.f) && SEASON3B::IsRelease(VK_LBUTTON))
				{
					g_pNewUIMiniMap->Zoom(0.5f);
					return true;
				}

				if (g_pNewUIMiniMap->runtime_update_target())
				{
					return true;
				}
			}
		}

		if (SEASON3B::CheckMouseIn(RenderFrameX + 113.f, RenderFrameY, 15.f, 15.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			m_iHideMap = !m_iHideMap;
#if MAIN_UPDATE > 303
			if (g_pPartyListWindow->IsEnableMove())
			{
				if (IsHideInterfaceWorld())
					g_pPartyListWindow->SetPos(GetWindowsX);
				else
					g_pPartyListWindow->SetPos(RenderFrameX);
			}
#endif
			return true;
		}
		return SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 128, 15);
	}

	return false;
}

void SEASON3B::CNewUIItemEnduranceInfo::RenderBtns(float x, float y, float width, float height, int index)
{
	float u, v, uw, vh;

	u = 0.0; v = 0.0; uw = 31.0; vh = 31.0;

	if (index == 0)
	{
		u = 0.0;
	}
	else if (index == 1)
	{
		u = 34.0;
	}
	else if (index == 2)
	{
		u = 68.0;
		uw = 176.0;
	}
	else if (index == 3)
	{
		u = 246.0;
	}

	if (SEASON3B::CheckMouseIn(x, y, width, height))
	{
		v = 33.0;
		if (MouseLButton)
			v = 66.0;
	}

	RenderImageF(IMAGE_HUB_WORLD_TAR, x, y, width, height, u, v, uw, vh);
}
