// NewUICharacterInfoWindow.cpp: implementation of the CNewUICharacterInfoWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUICharacterInfoWindow.h"
#include "NewUISystem.h"
#include "CharacterManager.h"
#include "CSItemOption.h"
#include "ZzzBMD.h"
#include "ZzzCharacter.h"
#include "UIControls.h"
#include "ZzzInterface.h"
#include "ZzzScene.h"
#include "ZzzInventory.h"
#include "SkillManager.h"
#include "UIJewelHarmony.h"
#include "UIManager.h"
#include "wsclientinline.h"
#include "ServerListManager.h"
#include "NewUICustomMessageBox.h"
#include "CGMHeadChat.h"
#include "CGMProtect.h"
#include "TextClien.h"

using namespace SEASON3B;

SEASON3B::CNewUICharacterInfoWindow::CNewUICharacterInfoWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = m_Pos.y = 0;
	m_inputIndex = 0;
	m_iCurTab = 0;
	last_time = std::chrono::steady_clock::now();
}

SEASON3B::CNewUICharacterInfoWindow::~CNewUICharacterInfoWindow()
{
	Release();
}

bool SEASON3B::CNewUICharacterInfoWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_CHARACTER, this);

	SetPos(x, y);

	LoadImages();

	SetButtonInfo();

	Show(false);

	return true;
}

void SEASON3B::CNewUICharacterInfoWindow::SetButtonInfo()
{
	unicode::t_char strText[256];

	m_pScrollBar.Create(m_Pos.x + 160, m_Pos.y + 155, 216);
	m_pScrollBar.SetPercent(0.0);

#if MAIN_UPDATE > 603
	float RenderPosX = m_Pos.x;
	float RenderPosY = m_Pos.y - 10.f;

	for (int i = 0; i < 5; i++)
	{
		m_BtnStat[i].ChangeButtonImgState(true, BITMAP_chainfo_btn_level, false, false, false, 0.f, 0.f, 65.f, 66.f);
	}

	m_BtnStat[STAT_STRENGTH].ChangeButtonInfo(RenderPosX + 140, RenderPosY + HEIGHT_STRENGTH + 2, 16, 16);
	m_BtnStat[STAT_DEXTERITY].ChangeButtonInfo(RenderPosX + 140, RenderPosY + HEIGHT_DEXTERITY + 2, 16, 16);
	m_BtnStat[STAT_VITALITY].ChangeButtonInfo(RenderPosX + 140, RenderPosY + HEIGHT_VITALITY + 2, 16, 16);
	m_BtnStat[STAT_ENERGY].ChangeButtonInfo(RenderPosX + 140, RenderPosY + HEIGHT_ENERGY + 2, 16, 16);
	m_BtnStat[STAT_CHARISMA].ChangeButtonInfo(RenderPosX + 140, RenderPosY + HEIGHT_CHARISMA + 2, 16, 16);

	SEASON3B::Loadbutton(&m_BtnExit, 0, RenderPosX, m_Pos.y, 20, 20, BITMAP_btn_exit_00, false, false, false, 0.f, 0.f, 69.f, 72.f);
	SEASON3B::Loadbutton(&m_BtnQuest, 1, RenderPosX, m_Pos.y, 20, 20, BITMAP_chainfo_btn_quest, false, false, false, 0.f, 0.f, 69.f, 72.f);
	SEASON3B::Loadbutton(&m_BtnPet, 2, RenderPosX, m_Pos.y, 20, 20, BITMAP_chainfo_btn_pet, false, false, false, 0.f, 0.f, 69.f, 72.f);


	if (GMProtect->IsMasterSkill() == true)
	{
		SEASON3B::Loadbutton(&m_BtnMasterLevel, 3, RenderPosX, m_Pos.y, 20, 20, BITMAP_chainfo_btn_master, false, false, false, 0.f, 0.f, 69.f, 72.f);

		m_BtnMasterLevel.ChangeToolTipText(GlobalText[1749], true);
	}

	unicode::_sprintf(strText, GlobalText[927], "C");
	m_BtnExit.ChangeToolTipText(strText, true);
	unicode::_sprintf(strText, "%s(%s)", GlobalText[1140], "T");
	m_BtnQuest.ChangeToolTipText(strText, true);
	m_BtnPet.ChangeToolTipText(GlobalText[1217], true);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		for (int i = STAT_STRENGTH; i <= STAT_CHARISMA; i++)
		{
			SetButtonImagen(&m_BtnStat[i], BITMAP_INTERFACE + 2, BITMAP_INTERFACE + 3);
		}

		SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);

		SetButtonInterface(&m_BtnQuest, m_Pos.x, m_Pos.y, 5);

		SetButtonInterface(&m_BtnPet, m_Pos.x, m_Pos.y, 6);

		m_BtnStat[STAT_STRENGTH].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_STRENGTH - 2, 24, 24);

		m_BtnStat[STAT_DEXTERITY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_DEXTERITY - 2, 24, 24);

		m_BtnStat[STAT_VITALITY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_VITALITY - 2, 24, 24);

		m_BtnStat[STAT_ENERGY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_ENERGY - 2, 24, 24);

		m_BtnStat[STAT_CHARISMA].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_CHARISMA - 2, 24, 24);
	}
	else
	{
		m_BtnStat[STAT_STRENGTH].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
		m_BtnStat[STAT_STRENGTH].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_STRENGTH + 2, 16, 15);

		m_BtnStat[STAT_DEXTERITY].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
		m_BtnStat[STAT_DEXTERITY].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_DEXTERITY + 2, 16, 15);

		m_BtnStat[STAT_VITALITY].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
		m_BtnStat[STAT_VITALITY].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_VITALITY + 2, 16, 15);

		m_BtnStat[STAT_ENERGY].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
		m_BtnStat[STAT_ENERGY].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_ENERGY + 2, 16, 15);

		m_BtnStat[STAT_CHARISMA].ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_STAT, false);
		m_BtnStat[STAT_CHARISMA].ChangeButtonInfo(m_Pos.x + 160, m_Pos.y + HEIGHT_CHARISMA + 2, 16, 15);

		m_BtnExit.ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_EXIT, false);
		m_BtnExit.ChangeButtonInfo(m_Pos.x + 13, m_Pos.y + 392, 36, 29);

		m_BtnQuest.ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_QUEST, false);
		m_BtnQuest.ChangeButtonInfo(m_Pos.x + 50, m_Pos.y + 392, 36, 29);

		m_BtnPet.ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_PET, false);
		m_BtnPet.ChangeButtonInfo(m_Pos.x + 87, m_Pos.y + 392, 36, 29);

		if (GMProtect->IsMasterSkill() == true)
		{
			m_BtnMasterLevel.ChangeButtonImgState(true, IMAGE_CHAINFO_BTN_MASTERLEVEL, false);
			m_BtnMasterLevel.ChangeButtonInfo(m_Pos.x + 124, m_Pos.y + 392, 36, 29);
			m_BtnMasterLevel.ChangeToolTipText(GlobalText[1749], true);
		}

		unicode::_sprintf(strText, GlobalText[927], "C");
		m_BtnExit.ChangeToolTipText(strText, true);

		unicode::_sprintf(strText, "%s(%s)", GlobalText[1140], "T");
		m_BtnQuest.ChangeToolTipText(strText, true);

		m_BtnPet.ChangeToolTipText(GlobalText[1217], true);
	}
#else
	for (int i = STAT_STRENGTH; i <= STAT_CHARISMA; i++)
	{
		SetButtonImagen(&m_BtnStat[i], BITMAP_INTERFACE + 2, BITMAP_INTERFACE + 3);
	}

	SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);

	SetButtonInterface(&m_BtnQuest, m_Pos.x, m_Pos.y, 5);

	SetButtonInterface(&m_BtnPet, m_Pos.x, m_Pos.y, 6);

	m_BtnStat[STAT_STRENGTH].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_STRENGTH - 2, 24, 24);

	m_BtnStat[STAT_DEXTERITY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_DEXTERITY - 2, 24, 24);

	m_BtnStat[STAT_VITALITY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_VITALITY - 2, 24, 24);

	m_BtnStat[STAT_ENERGY].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_ENERGY - 2, 24, 24);

	m_BtnStat[STAT_CHARISMA].ChangeButtonInfo(m_Pos.x + 125.0, m_Pos.y + HEIGHT_CHARISMA - 2, 24, 24);

	unicode::_sprintf(strText, GlobalText[927], "C");
	m_BtnExit.ChangeToolTipText(strText, true);

	unicode::_sprintf(strText, "%s(%s)", GlobalText[1140], "T");
	m_BtnQuest.ChangeToolTipText(strText, true);

	m_BtnPet.ChangeToolTipText(GlobalText[1217], true);
#endif
}

void SEASON3B::CNewUICharacterInfoWindow::Release()
{
	UnloadImages();

	m_advanceChar.clear();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUICharacterInfoWindow::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;


#if MAIN_UPDATE > 603
	float RenderPosY = m_Pos.y - 10.f;
	float RenderPosX = m_Pos.x;

	m_BtnStat[STAT_STRENGTH].SetPos(RenderPosX + 140, RenderPosY + HEIGHT_STRENGTH + 2);
	m_BtnStat[STAT_DEXTERITY].SetPos(RenderPosX + 140, RenderPosY + HEIGHT_DEXTERITY + 2);
	m_BtnStat[STAT_VITALITY].SetPos(RenderPosX + 140, RenderPosY + HEIGHT_VITALITY + 2);
	m_BtnStat[STAT_ENERGY].SetPos(RenderPosX + 140, RenderPosY + HEIGHT_ENERGY + 2);
	m_BtnStat[STAT_CHARISMA].SetPos(RenderPosX + 140, RenderPosY + HEIGHT_CHARISMA + 2);

	RenderPosY = m_Pos.y;
	SEASON3B::LoadPos(&m_BtnExit, 0, RenderPosX, RenderPosY);
	SEASON3B::LoadPos(&m_BtnQuest, 1, RenderPosX, RenderPosY);
	SEASON3B::LoadPos(&m_BtnPet, 2, RenderPosX, RenderPosY);

	if (GMProtect->IsMasterSkill() == true)
	{
		SEASON3B::LoadPos(&m_BtnMasterLevel, 3, RenderPosX, RenderPosY);
	}
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		m_BtnExit.SetPos(m_Pos.x + 20, m_Pos.y + 395);

		m_BtnQuest.SetPos(m_Pos.x + 48, m_Pos.y + 395);

		m_BtnPet.SetPos(m_Pos.x + 76, m_Pos.y + 395);

		m_BtnStat[STAT_STRENGTH].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_STRENGTH - 2);

		m_BtnStat[STAT_DEXTERITY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_DEXTERITY - 2);

		m_BtnStat[STAT_VITALITY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_VITALITY - 2);

		m_BtnStat[STAT_ENERGY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_ENERGY - 2);

		m_BtnStat[STAT_CHARISMA].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_CHARISMA - 2);
	}
	else
	{
		m_BtnStat[STAT_STRENGTH].SetPos(m_Pos.x + 160, m_Pos.y + HEIGHT_STRENGTH + 2);

		m_BtnStat[STAT_DEXTERITY].SetPos(m_Pos.x + 160, m_Pos.y + HEIGHT_DEXTERITY + 2);

		m_BtnStat[STAT_VITALITY].SetPos(m_Pos.x + 160, m_Pos.y + HEIGHT_VITALITY + 2);

		m_BtnStat[STAT_ENERGY].SetPos(m_Pos.x + 160, m_Pos.y + HEIGHT_ENERGY + 2);

		m_BtnStat[STAT_CHARISMA].SetPos(m_Pos.x + 160, m_Pos.y + HEIGHT_CHARISMA + 2);

		m_BtnExit.SetPos(m_Pos.x + 13, m_Pos.y + 392);

		m_BtnQuest.SetPos(m_Pos.x + 50, m_Pos.y + 392);

		m_BtnPet.SetPos(m_Pos.x + 87, m_Pos.y + 392);

		if (GMProtect->IsMasterSkill() == true)
		{
			m_BtnMasterLevel.SetPos(m_Pos.x + 124, m_Pos.y + 392);
		}
	}
#else
	m_BtnExit.SetPos(m_Pos.x + 20, m_Pos.y + 395);

	m_BtnQuest.SetPos(m_Pos.x + 48, m_Pos.y + 395);

	m_BtnPet.SetPos(m_Pos.x + 76, m_Pos.y + 395);

	m_BtnStat[STAT_STRENGTH].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_STRENGTH - 2);

	m_BtnStat[STAT_DEXTERITY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_DEXTERITY - 2);

	m_BtnStat[STAT_VITALITY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_VITALITY - 2);

	m_BtnStat[STAT_ENERGY].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_ENERGY - 2);

	m_BtnStat[STAT_CHARISMA].SetPos(m_Pos.x + 125.0, m_Pos.y + HEIGHT_CHARISMA - 2);
#endif
}

bool SEASON3B::CNewUICharacterInfoWindow::UpdateMouseEvent()
{
	if (BtnProcess() == true)
	{
		return false;
	}

	if (CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY))
	{
		return false;
	}

	return true;
}

bool SEASON3B::CNewUICharacterInfoWindow::BtnProcess()
{
	float RenderPosY = m_Pos.y;

	POINT ptExitBtn1 = { m_Pos.x + 169, RenderPosY + 7 };

	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(ptExitBtn1.x, ptExitBtn1.y, 13, 12))
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_CHARACTER);
		return true;
	}


#if MAIN_UPDATE > 603
	RenderPosY += 32;
#elif MAIN_UPDATE > 303
	RenderPosY += 42;
#else
	RenderPosY += 32;
#endif

	for (int i = 0; i < 2; i++)
	{
		if (SEASON3B::CheckMouseIn(m_Pos.x + (57 * i) + 12, RenderPosY, 56, 22) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (i != m_iCurTab)
			{
				m_iCurTab = i;
				m_pScrollBar.SetPercent(0.0);
				m_pScrollBar.SetPos(m_Pos.x + WIN_WINDOW_SIZEX - 30, m_Pos.y + 155);
				return true;
			}
		}
	}

	RenderPosY = m_Pos.y;

	if (m_iCurTab == 1)
	{
		m_pScrollBar.UpdateMouseEvent();

		if (SEASON3B::CheckMouseIn(m_Pos.x + 12, RenderPosY + 150, 166, 230) == 1)
		{
			double prev = m_pScrollBar.GetPercent();
			if (MouseWheel <= 0)
			{
				if (MouseWheel < 0)
				{
					MouseWheel = 0;
					prev += 0.1;
					if (prev > 1.0)
						prev = 1.0;
					m_pScrollBar.SetPercent(prev);
				}
			}
			else
			{
				MouseWheel = 0;
				prev -= 0.1;
				if (prev < 0.0)
					prev = 0.0;
				m_pScrollBar.SetPercent(prev);
			}
		}
	}
	else
	{
		if (CharacterAttribute->LevelUpPoint > 0)
		{
			int iCount = 0;

			if (Hero->GetBaseClass() == CLASS_DARK_LORD)
				iCount = 5;
			else
				iCount = 4;

			for (int i = 0; i < iCount; ++i)
			{
				if (m_BtnStat[i].UpdateMouseEvent() == true)
				{
					OpenInputStatAdd(i);
					return true;
				}
			}
		}
	}

	if (m_BtnExit.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_CHARACTER);
		return true;
	}

	if (m_BtnQuest.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MYQUEST);
		return true;
	}

	if (m_BtnPet.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Toggle(SEASON3B::INTERFACE_PET);
		return true;
	}

#if MAIN_UPDATE > 303
	if (GMProtect->IsMasterSkill() == true)
	{
		if (m_BtnMasterLevel.UpdateMouseEvent() == true)
		{
			if (Hero->IsMasterLevel() == true && gCharacterManager.GetCharacterClass(Hero->Class) != CLASS_TEMPLENIGHT)
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MASTER_LEVEL);
			}
			return true;
		}
	}
#endif // MAIN_UPDATE != 303

	return false;
}

bool SEASON3B::CNewUICharacterInfoWindow::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_CHARACTER);
			PlayBuffer(SOUND_CLICK01);

			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUICharacterInfoWindow::Update()
{
	auto current_time = std::chrono::steady_clock::now();
	double difTime = std::chrono::duration<double>(current_time - last_time).count();

	if (difTime >= 1.0)
	{
		last_time = current_time;
		if (CharacterAttribute->PenaltyTimePK > 0)
		{
			CharacterAttribute->PenaltyTimePK -= 1;
		}
	}

	if (IsVisible())
	{
		if (m_iCurTab == 1)
		{
			m_pScrollBar.Update();
		}
	}
	return true;
}

void SEASON3B::CNewUICharacterInfoWindow::RenderFrame()
{
	float RenderPosX = m_Pos.x;
	float RenderPosY = m_Pos.y;

	SEASON3B::window_nomal(RenderPosX, RenderPosY, 3);

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	if (m_iCurTab == 1)
	{
		//RenderTable(m_Pos.x + 12, m_Pos.y + 64, 166, 66);
		//RenderTable(m_Pos.x + 12, m_Pos.y + 150, 166, 230);
	}
	else
	{
		RenderPosY -= 10.f;
		//RenderTable(m_Pos.x + 12, m_Pos.y + 64, 166, 56);
		RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + 64.f, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);
		RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + HEIGHT_STRENGTH, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);
		RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + HEIGHT_DEXTERITY, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);
		RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + HEIGHT_VITALITY, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);
		RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + HEIGHT_ENERGY, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);

		if (Hero->GetBaseClass() == CLASS_DARK_LORD)
		{
			RenderImageF(BITMAP_chainfo_textbox, RenderPosX + 12, RenderPosY + HEIGHT_CHARISMA, 110.f, 19.f, 0.0, 0.0, 406.f, 72.f);
		}
	}
#else

	if (gmProtect->checkold_school())
	{
		if (m_iCurTab == 1)
		{
			RenderGoldRect(m_Pos.x + 16, m_Pos.y + 64, 158, 66, 2);
			RenderGoldRect(m_Pos.x + 16, m_Pos.y + 150, 158, 230, 2);
		}
		else
		{
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + 66.0, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_STRENGTH, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_DEXTERITY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_VITALITY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_ENERGY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);

			if (Hero->GetBaseClass() == CLASS_DARK_LORD)
			{
				RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_CHARISMA, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
			}
		}
	}
	else
	{
		if (m_iCurTab == 1)
		{
			RenderTable(m_Pos.x + 12, m_Pos.y + 64, 166, 66);
			RenderTable(m_Pos.x + 12, m_Pos.y + 150, 166, 230);
		}
		else
		{
			RenderTable(m_Pos.x + 12, m_Pos.y + 64, 166, 56);

			RenderImage(IMAGE_CHAINFO_TEXTBOX, m_Pos.x + 11, m_Pos.y + HEIGHT_STRENGTH, 170.f, 21.f);
			RenderImage(IMAGE_CHAINFO_TEXTBOX, m_Pos.x + 11, m_Pos.y + HEIGHT_DEXTERITY, 170.f, 21.f);
			RenderImage(IMAGE_CHAINFO_TEXTBOX, m_Pos.x + 11, m_Pos.y + HEIGHT_VITALITY, 170.f, 21.f);
			RenderImage(IMAGE_CHAINFO_TEXTBOX, m_Pos.x + 11, m_Pos.y + HEIGHT_ENERGY, 170.f, 21.f);

			if (Hero->GetBaseClass() == CLASS_DARK_LORD)
			{
				RenderImage(IMAGE_CHAINFO_TEXTBOX, m_Pos.x + 11, m_Pos.y + HEIGHT_CHARISMA, 170.f, 21.f);
			}
		}
	}
#endif // test_main_dev
#else

	if (m_iCurTab == 1)
	{
		RenderGoldRect(m_Pos.x + 16, m_Pos.y + 64, 158, 66, 2);
		RenderGoldRect(m_Pos.x + 16, m_Pos.y + 150, 158, 230, 2);
	}
	else
	{
		RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + 56.0, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);

		RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_STRENGTH, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
		RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_DEXTERITY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
		RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_VITALITY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
		RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_ENERGY, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);

		if (Hero->GetBaseClass() == CLASS_DARK_LORD)
		{
			RenderBitmap(BITMAP_INTERFACE + 4, m_Pos.x + 13.0, m_Pos.y + HEIGHT_CHARISMA, 75.0, 21.0, 0, 0, 0.5859375, 0.65625, 1, 1, 0.0);
		}
	}
#endif
}

bool SEASON3B::CNewUICharacterInfoWindow::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderFrame();

	RenderTexts();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUICharacterInfoWindow::RenderTexts()
{
	RenderSubjectTexts();

	if (m_iCurTab == 0)
	{
		RenderTableTexts();
		RenderAttribute();
	}
	else
	{
		RenderAdavanceInfo();
		RenderEquipementAttribute();
	}
}

void SEASON3B::CNewUICharacterInfoWindow::RenderSubjectTexts()
{
	char strID[256];
	sprintf(strID, "%s", CharacterAttribute->Name);
	unicode::t_char strClassName[256];
	unicode::_sprintf(strClassName, "(%s)", gCharacterManager.GetCharacterClassText(CharacterAttribute->Class));

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(20, 20, 20, 20);
	gmHeadChat->SetPlayerColor(Hero->PK);

#ifdef test_main_dev
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 9, strID, WIN_WINDOW_SIZEX, 0, RT3_SORT_CENTER);
#else
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 12, strID, WIN_WINDOW_SIZEX, 0, RT3_SORT_CENTER);
#endif // test_main_dev


	unicode::t_char strServerName[MAX_TEXT_LENGTH];

	if (gmProtect->ServerNameDefault)
	{
		const char* apszGlobalText[4] = { GlobalText[461], GlobalText[460], GlobalText[3130], GlobalText[3131] };
		sprintf(strServerName, apszGlobalText[g_ServerListManager->GetNonPVPInfo()], g_ServerListManager->GetSelectServerName(), g_ServerListManager->GetSelectServerIndex());
	}
	else
	{
		sprintf(strServerName, "%s", g_ServerListManager->GetSelectServerName());
	}
	float fAlpha = sinf(WorldTime * 0.001f) + 1.f;

#ifdef test_main_dev
	g_pRenderText->SetTextColor(255, 255, 255, 127 * (2.f - fAlpha));
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 19, strClassName, WIN_WINDOW_SIZEX, 0, RT3_SORT_CENTER);
	g_pRenderText->SetTextColor(255, 255, 255, 127 * fAlpha);
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 19, strServerName, WIN_WINDOW_SIZEX, 0, RT3_SORT_CENTER);
#else
#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		g_pRenderText->SetTextColor(255, 255, 255, 127 * (2.f - fAlpha));
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 22, strClassName, 190, 0, RT3_SORT_CENTER);
		g_pRenderText->SetTextColor(255, 255, 255, 127 * fAlpha);
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 22, strServerName, 190, 0, RT3_SORT_CENTER);
	}
	else
	{
		g_pRenderText->SetTextColor(255, 255, 255, 127 * (2.f - fAlpha));
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 27, strClassName, 190, 0, RT3_SORT_CENTER);
		g_pRenderText->SetTextColor(255, 255, 255, 127 * fAlpha);
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 27, strServerName, 190, 0, RT3_SORT_CENTER);
	}
#else
	g_pRenderText->SetTextColor(255, 255, 255, 127 * (2.f - fAlpha));
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 22, strClassName, 190, 0, RT3_SORT_CENTER);
	g_pRenderText->SetTextColor(255, 255, 255, 127 * fAlpha);
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 22, strServerName, 190, 0, RT3_SORT_CENTER);
#endif
#endif // test_main_dev
}

void SEASON3B::CNewUICharacterInfoWindow::RenderTableTexts()
{
	unicode::t_char strLevel[32];
	unicode::t_char strExp[128];
	unicode::t_char strPoint[128];

	unicode::_sprintf(strLevel, GlobalText[200], CharacterAttribute->Level/*+ Master_Level_Data.nMLevel*/);

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		unicode::_sprintf(strExp, "----------");
	}
	else
	{
		unicode::_sprintf(strExp, GlobalText[201], CharacterAttribute->Experience, CharacterAttribute->NextExperince);
	}

	if (CharacterAttribute->Level > 9)
	{
		int iMinus = 0;
		int iMaxMinus = 0;
		if (CharacterAttribute->wMinusPoint == 0)
		{
			iMinus = 0;
		}
		else
		{
			iMinus -= CharacterAttribute->wMinusPoint;
		}

		iMaxMinus -= CharacterAttribute->wMaxMinusPoint;

		unicode::_sprintf(strPoint, "%s %d/%d | %s %d/%d",
			GlobalText[1412], CharacterAttribute->AddPoint, CharacterAttribute->MaxAddPoint,
			GlobalText[1903], iMinus, iMaxMinus);
	}
	else
	{
		wsprintf(strPoint, "%s %d/%d | %s %d/%d", GlobalText[1412], 0, 0, GlobalText[1903], 0, 0);
	}

	int iAddPoint, iMinusPoint;

	if (CharacterAttribute->AddPoint <= 10)
	{
		iAddPoint = 100;
	}
	else
	{
		int iTemp = 0;
		if (CharacterAttribute->MaxAddPoint != 0)
		{
			iTemp = (CharacterAttribute->AddPoint * 100) / CharacterAttribute->MaxAddPoint;
		}
		if (iTemp <= 10)
		{
			iAddPoint = 70;
		}
		else if (iTemp > 10 && iTemp <= 30)
		{
			iAddPoint = 60;
		}
		else if (iTemp > 30 && iTemp <= 50)
		{
			iAddPoint = 50;
		}
		else if (iTemp > 50)
		{
			iAddPoint = 40;
		}
	}

	if (CharacterAttribute->wMinusPoint <= 10)
	{
		iMinusPoint = 100;
	}
	else
	{
		int iTemp = 0;
		if (CharacterAttribute->wMaxMinusPoint != 0)
		{
			iTemp = (CharacterAttribute->wMinusPoint * 100) / CharacterAttribute->wMaxMinusPoint;
		}

		if (iTemp <= 10)
		{
			iMinusPoint = 70;
		}
		else if (iTemp > 10 && iTemp <= 30)
		{
			iMinusPoint = 60;
		}
		else if (iTemp > 30 && iTemp <= 50)
		{
			iMinusPoint = 50;
		}
		else if (iTemp > 50)
		{
			iMinusPoint = 40;
		}
	}

	unicode::t_char strPointProbability[128];
	unicode::_sprintf(strPointProbability, GlobalText[1907], iAddPoint, iMinusPoint);

	g_pRenderText->SetFont(g_hFontBold);

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	float RenderPosX = m_Pos.x;
	float RenderPosY = m_Pos.y - 8.f;

	g_pRenderText->SetTextColor(230, 230, 0, 255);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->RenderText(RenderPosX + 12, RenderPosY + 68, strLevel, 74, 0, RT3_SORT_CENTER);
	//m_Pos.x + 12, RenderPosY + HEIGHT_STRENGTH + 6, GlobalText[166], 74, 0, RT3_SORT_CENTER
	if (CharacterAttribute->LevelUpPoint > 0)
	{
		unicode::t_char strLevelUpPoint[128];

		if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == false || CharacterAttribute->LevelUpPoint > 0)
		{
			unicode::_sprintf(strLevelUpPoint, GlobalText[217], CharacterAttribute->LevelUpPoint);
		}
		else
		{
			unicode::_sprintf(strLevelUpPoint, "");
		}

		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(30, 110, 200, 255);
		g_pRenderText->SetTextColor(0, 0, 0, 255);
		//g_pRenderText->RenderText(m_Pos.x + 94, m_Pos.y + 66, strLevelUpPoint, 80, 0, 3, 0);
		g_pRenderText->RenderText(RenderPosX + 125, RenderPosY + 68, strLevelUpPoint, 62, 0, 3);
		g_pRenderText->SetBgColor(0);
	}

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->RenderText(RenderPosX + 18, RenderPosY + 80, strExp);


	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(76, 197, 254, 255);
	g_pRenderText->RenderText(RenderPosX + 18, RenderPosY + 92, strPointProbability);

	g_pRenderText->SetTextColor(76, 197, 254, 255);
	g_pRenderText->RenderText(RenderPosX + 18, RenderPosY + 104, strPoint);
#else
	if (gmProtect->checkold_school())
	{
		g_pRenderText->SetTextColor(230, 230, 0, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(m_Pos.x + 24, m_Pos.y + 72, strLevel);

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 128);
		g_pRenderText->RenderText(m_Pos.x + 24, m_Pos.y + 92, strExp, 142);

		g_pRenderText->SetTextColor(100, 150, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 24, m_Pos.y + 104, strPoint, 142);

		//g_pRenderText->SetTextColor(76, 197, 254, 255);
		//g_pRenderText->RenderText(m_Pos.x + 94, m_Pos.y + 78, strPointProbability);

		if (CharacterAttribute->LevelUpPoint > 0)
		{
			unicode::t_char strLevelUpPoint[128];

			if (CharacterAttribute->LevelUpPoint > 0)
			{
				unicode::_sprintf(strLevelUpPoint, GlobalText[217], CharacterAttribute->LevelUpPoint);
			}

			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetBgColor(30, 110, 200, 255);
			g_pRenderText->SetTextColor(0, 0, 0, 255);
			g_pRenderText->RenderText(m_Pos.x + 94, m_Pos.y + 66, strLevelUpPoint, 80, 0, 3, 0);
		}
	}
	else
	{
		g_pRenderText->SetTextColor(230, 230, 0, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + 68, strLevel);

		if (CharacterAttribute->LevelUpPoint > 0)
		{
			unicode::t_char strLevelUpPoint[128];

			if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == false || CharacterAttribute->LevelUpPoint > 0)
			{
				unicode::_sprintf(strLevelUpPoint, GlobalText[217], CharacterAttribute->LevelUpPoint);
			}
			else
			{
				unicode::_sprintf(strLevelUpPoint, "");
			}

			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetTextColor(255, 138, 0, 255);
			g_pRenderText->SetBgColor(0, 0, 0, 0);
			g_pRenderText->RenderText(m_Pos.x + 110, m_Pos.y + 68, strLevelUpPoint);
		}

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + 80, strExp);


		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(76, 197, 254, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + 92, strPointProbability);

		g_pRenderText->SetTextColor(76, 197, 254, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + 104, strPoint);
	}
#endif
#else
	g_pRenderText->SetTextColor(230, 230, 0, 255);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->RenderText(m_Pos.x + 24, m_Pos.y + 62, strLevel);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 80, strExp, 150);

	g_pRenderText->SetTextColor(100, 150, 255, 255);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 90, strPoint, 150);


	switch (Hero->PK)
	{
	case PVP_HERO2:
	case PVP_HERO1:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[487]);
		break;
	case PVP_NEUTRAL:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[488]);
		break;
	case PVP_CAUTION:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[489]);
		break;
	case PVP_MURDERER1:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[490]);
		break;
	case PVP_MURDERER2:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[491]);
		break;
	default:
		sprintf_s(strPoint, "PK %s: %s", GlobalText[1448], GlobalText[488]);
		break;
	}

	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 100, strPoint, 150);

	if (CharacterAttribute->PenaltyTimePK > 0 && CharacterAttribute->PenaltyTimePK != 0xFFFFFFFF)
	{
		//g_pRenderText->SetBgColor(0);

		DWORD hours = CharacterAttribute->PenaltyTimePK / 3600;
		DWORD minutes = (CharacterAttribute->PenaltyTimePK % 3600) / 60;

		sprintf_s(strPoint, GlobalText[2372], hours, minutes);

		g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 110, strPoint, 150); //-- Remaining
	}


	if (CharacterAttribute->LevelUpPoint > 0)
	{
		unicode::t_char strLevelUpPoint[128];

		if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == false || CharacterAttribute->LevelUpPoint > 0)
		{
			unicode::_sprintf(strLevelUpPoint, GlobalText[217], CharacterAttribute->LevelUpPoint);
		}
		else
		{
			unicode::_sprintf(strLevelUpPoint, "");
		}

		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(30, 110, 200, 255);
		g_pRenderText->SetTextColor(0, 0, 0, 255);
		g_pRenderText->RenderText(m_Pos.x + 93, m_Pos.y + 60, strLevelUpPoint, 80, 0, 3, 0);
	}
#endif
}

void SEASON3B::CNewUICharacterInfoWindow::RenderAttribute()
{
	unicode::t_char strStrength[32];
	unicode::t_char strDexterity[32];
	unicode::t_char strVitality[256];
	unicode::t_char strBlocking[256];
	unicode::t_char strAttakMamage[256];

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);

	DWORD wStrength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;

	DWORD text_size = 0;
	float RenderPosY = m_Pos.y;

#if MAIN_UPDATE > 303

#ifdef test_main_dev
	RenderPosY -= 10;
#else
	if (gmProtect->checkold_school())
	{
		text_size = 130;
	}
#endif // test_main_dev

	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion1))
	{
		g_pRenderText->SetTextColor(255, 120, 0, 255);
	}
	else
	{
		if (CharacterAttribute->AddStrength)
		{
			g_pRenderText->SetTextColor(100, 150, 255, 255);
		}
		else
		{
			g_pRenderText->SetTextColor(230, 230, 0, 255);
		}
	}

#ifdef test_main_dev
	unicode::_sprintf(strStrength, "%s: %d", GlobalText[166], wStrength);
	g_pRenderText->RenderText(m_Pos.x + 34, RenderPosY + HEIGHT_STRENGTH + 6, strStrength);
#else
	if (gmProtect->checkold_school())
	{
		unicode::_sprintf(strStrength, "%s: %d", GlobalText[166], wStrength); //Sức mạnh
		g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_STRENGTH + 6, strStrength);
		g_pRenderText->SetBgColor(0, 0, 0, 128);
	}
	else
	{
		unicode::_sprintf(strStrength, "%d", wStrength);

		g_pRenderText->RenderText(m_Pos.x + 12, RenderPosY + HEIGHT_STRENGTH + 6, GlobalText[166], 74, 0, RT3_SORT_CENTER); //Sức mạnh
		g_pRenderText->RenderText(m_Pos.x + 86, RenderPosY + HEIGHT_STRENGTH + 6, strStrength, 86, 0, RT3_SORT_CENTER);
	}
#endif
#else
	text_size = 130;

	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion1))
	{
		g_pRenderText->SetTextColor(255, 120, 0, 255);
	}
	else
	{
		if (CharacterAttribute->AddStrength)
		{
			g_pRenderText->SetTextColor(100, 150, 255, 255);
		}
		else
		{
			g_pRenderText->SetTextColor(230, 230, 0, 255);
		}
	}

	unicode::_sprintf(strStrength, "%s: %d", GlobalText[166], wStrength);

	g_pRenderText->RenderText(m_Pos.x + 24, m_Pos.y + HEIGHT_STRENGTH + 6, strStrength);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
#endif

	int iAttackDamageMin = 0;
	int iAttackDamageMax = 0;

	bool bAttackDamage = GetAttackDamage(&iAttackDamageMin, &iAttackDamageMax);

	int Add_Dex = 0;
	int Add_Rat = 0;
	int Add_Dfe = 0;
	int Add_Att_Max = 0;
	int Add_Att_Min = 0;
	int Add_Dfe_Rat = 0;
	int Add_Ch_Dfe = 0;
	int Add_Mana_Max = 0;
	int Add_Mana_Min = 0;

	ITEM* pWeaponRight = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* pWeaponLeft = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];

	int iAttackRating = CharacterAttribute->AttackRating + Add_Rat;
	int iAttackRatingPK = CharacterAttribute->AttackRatingPK + Add_Dex;
	iAttackDamageMax += Add_Att_Max;
	iAttackDamageMin += Add_Att_Min;

	if (g_isCharacterBuff((&Hero->Object), eBuff_AddAG))
	{
		DWORD wDexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
		iAttackRating += wDexterity;
		iAttackRatingPK += wDexterity;
		if (PartyNumber >= 3)
		{
			int iPlusRating = (wDexterity * ((PartyNumber - 2) * 0.01f));
			iAttackRating += iPlusRating;
			iAttackRatingPK = iPlusRating;
		}
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_HelpNpc))
	{
		int iTemp = 0;
		if (CharacterAttribute->Level > 180)
		{
			iTemp = (180 / 3) + 45;
		}
		else
		{
			iTemp = (CharacterAttribute->Level / 3) + 45;
		}

		iAttackDamageMin += iTemp;
		iAttackDamageMax += iTemp;
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_Berserker))
	{
		int nTemp = CharacterAttribute->Strength + CharacterAttribute->AddStrength
			+ CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
		float fTemp = int(CharacterAttribute->Energy / 30) / 100.f;
		iAttackDamageMin += nTemp / 7 * fTemp;
		iAttackDamageMax += nTemp / 4 * fTemp;
	}

	int iMinIndex = 0, iMaxIndex = 0, iMagicalIndex = 0;

	StrengthenCapability SC_r, SC_l;

	int rlevel = (pWeaponRight->Level >> 3) & 15;

	if (rlevel >= pWeaponRight->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&SC_r, pWeaponRight, 1);

		if (SC_r.SI_isSP)
		{
			iMinIndex = SC_r.SI_SP.SI_minattackpower;
			iMaxIndex = SC_r.SI_SP.SI_maxattackpower;
			iMagicalIndex = SC_r.SI_SP.SI_magicalpower;
		}
	}

	int llevel = (pWeaponLeft->Level >> 3) & 15;

	if (llevel >= pWeaponLeft->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&SC_l, pWeaponLeft, 1);

		if (SC_l.SI_isSP)
		{
			iMinIndex += SC_l.SI_SP.SI_minattackpower;
			iMaxIndex += SC_l.SI_SP.SI_maxattackpower;
			iMagicalIndex += SC_l.SI_SP.SI_magicalpower;
		}
	}

	int iDefenseRate = 0, iAttackPowerRate = 0;

	StrengthenCapability rightinfo, leftinfo;

	int iRightLevel = (pWeaponRight->Level >> 3) & 15;

	if (iRightLevel >= pWeaponRight->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&rightinfo, pWeaponRight, 1);
	}

	int iLeftLevel = (pWeaponLeft->Level >> 3) & 15;

	if (iLeftLevel >= pWeaponLeft->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&leftinfo, pWeaponLeft, 1);
	}

	if (rightinfo.SI_isSP)
	{
		iAttackPowerRate += rightinfo.SI_SP.SI_attackpowerRate;
	}
	if (leftinfo.SI_isSP)
	{
		iAttackPowerRate += leftinfo.SI_SP.SI_attackpowerRate;
	}

	for (int k = EQUIPMENT_WEAPON_LEFT; k < MAX_EQUIPMENT; ++k)
	{
		StrengthenCapability defenseinfo;

		ITEM* pItem = &CharacterMachine->Equipment[k];

		int eqlevel = (pItem->Level >> 3) & 15;

		if (eqlevel >= pItem->Jewel_Of_Harmony_OptionLevel)
		{
			g_pUIJewelHarmonyinfo->GetStrengthenCapability(&defenseinfo, pItem, 2);
		}

		if (defenseinfo.SI_isSD)
		{
			iDefenseRate += defenseinfo.SI_SD.SI_defenseRate;
		}
	}

	int itemoption380Attack = 0;
	int itemoption380Defense = 0;

	for (int j = 0; j < MAX_EQUIPMENT; ++j)
	{
		bool is380item = CharacterMachine->Equipment[j].option_380;
		int i380type = CharacterMachine->Equipment[j].Type;

		if (is380item && i380type > -1 && i380type < MAX_ITEM)
		{
			ITEM_ADD_OPTION item380option;

			item380option = g_pItemAddOptioninfo->GetItemAddOtioninfo(CharacterMachine->Equipment[j].Type);

			if (item380option.m_byOption1 == 1)
			{
				itemoption380Attack += item380option.m_byValue1;
			}

			if (item380option.m_byOption2 == 1)
			{
				itemoption380Attack += item380option.m_byValue2;
			}

			if (item380option.m_byOption1 == 3)
			{
				itemoption380Defense += item380option.m_byValue1;
			}

			if (item380option.m_byOption2 == 3)
			{
				itemoption380Defense += item380option.m_byValue2;
			}
		}
	}

	ITEM* pItemRingLeft, * pItemRingRight;

	pItemRingLeft = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];
	pItemRingRight = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];
	if (pItemRingLeft && pItemRingRight)
	{
		int iNonExpiredLRingType = -1;
		int iNonExpiredRRingType = -1;
		if (!pItemRingLeft->bPeriodItem || !pItemRingLeft->bExpiredPeriod)
		{
			iNonExpiredLRingType = pItemRingLeft->Type;
		}
		if (!pItemRingRight->bPeriodItem || !pItemRingRight->bExpiredPeriod)
		{
			iNonExpiredRRingType = pItemRingRight->Type;
		}

		int maxIAttackDamageMin = 0;
		int maxIAttackDamageMax = 0;
		if (iNonExpiredLRingType == ITEM_HELPER + 41 || iNonExpiredRRingType == ITEM_HELPER + 41)
		{
			maxIAttackDamageMin = max(maxIAttackDamageMin, 20);
			maxIAttackDamageMax = max(maxIAttackDamageMax, 20);
		}
		if (iNonExpiredLRingType == ITEM_HELPER + 76 || iNonExpiredRRingType == ITEM_HELPER + 76)
		{
			maxIAttackDamageMin = max(maxIAttackDamageMin, 30);
			maxIAttackDamageMax = max(maxIAttackDamageMax, 30);
		}
		if (iNonExpiredLRingType == ITEM_HELPER + 122 || iNonExpiredRRingType == ITEM_HELPER + 122)
		{
			maxIAttackDamageMin = max(maxIAttackDamageMin, 40);
			maxIAttackDamageMax = max(maxIAttackDamageMax, 40);
		}

		iAttackDamageMin += maxIAttackDamageMin;
		iAttackDamageMax += maxIAttackDamageMax;
	}

	ITEM* pItemHelper = &CharacterMachine->Equipment[EQUIPMENT_HELPER];
	if (pItemHelper)
	{
		if (pItemHelper->Type == ITEM_HELPER + 37 && pItemHelper->Option1 == 0x04)
		{
			DWORD wLevel = CharacterAttribute->Level;
			iAttackDamageMin += (wLevel / 12);
			iAttackDamageMax += (wLevel / 12);
		}
		if (pItemHelper->Type == ITEM_HELPER + 64)
		{
			if (false == pItemHelper->bExpiredPeriod)
			{
				iAttackDamageMin += int(float(iAttackDamageMin) * 0.4f);
				iAttackDamageMax += int(float(iAttackDamageMax) * 0.4f);
			}
		}
		if (pItemHelper->Type == ITEM_HELPER + 123)
		{
			if (false == pItemHelper->bExpiredPeriod)
			{
				iAttackDamageMin += int(float(iAttackDamageMin) * 0.2f);
				iAttackDamageMax += int(float(iAttackDamageMax) * 0.2f);
			}
		}
		if (pItemHelper->Type == ITEM_HELPER + 1)
		{
			iAttackDamageMin += int(float(iAttackDamageMin) * 0.3f);
			iAttackDamageMax += int(float(iAttackDamageMax) * 0.3f);
		}
	}

#ifdef SHUTDOWN_CHARACTER_INFO
	DWORD _AttackDamageMin = iAttackDamageMin;
	DWORD _AttackDamageMax = iAttackDamageMax;

	iAttackRating = CharacterAttribute->Work_AttackSuccessRate;
	CharacterAttribute->GetPhisyDamage(&_AttackDamageMin, &_AttackDamageMax);

	if (iAttackRating > 0)
		unicode::_sprintf(strAttakMamage, GlobalText[203], _AttackDamageMin, _AttackDamageMax, iAttackRating);
	else
		unicode::_sprintf(strAttakMamage, GlobalText[204], _AttackDamageMin, _AttackDamageMax);
#else
	if (iAttackRating > 0)
	{
		if (iAttackDamageMin + iMinIndex >= iAttackDamageMax + iMaxIndex)
		{
			unicode::_sprintf(strAttakMamage, GlobalText[203], iAttackDamageMax + iMaxIndex, iAttackDamageMax + iMaxIndex, iAttackRating);
		}
		else
		{
			unicode::_sprintf(strAttakMamage, GlobalText[203], iAttackDamageMin + iMinIndex, iAttackDamageMax + iMaxIndex, iAttackRating);
		}
	}
	else
	{
		if (iAttackDamageMin + iMinIndex >= iAttackDamageMax + iMaxIndex)
		{
			unicode::_sprintf(strAttakMamage, GlobalText[204], iAttackDamageMax + iMaxIndex, iAttackDamageMax + iMaxIndex);
		}
		else
		{
			unicode::_sprintf(strAttakMamage, GlobalText[204], iAttackDamageMin + iMinIndex, iAttackDamageMax + iMaxIndex);
		}
	}
#endif // SHUTDOWN_CHARACTER_INFO

	g_pRenderText->SetFont(g_hFont);
	if (bAttackDamage)
	{
		g_pRenderText->SetTextColor(100, 150, 255, 255);
	}
	else
	{
		g_pRenderText->SetTextColor(255, 255, 255, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin2))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll3))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_Petal))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}


	int iY = HEIGHT_STRENGTH + 25;

	g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + HEIGHT_STRENGTH + 25, strAttakMamage, text_size);

#ifdef SHUTDOWN_CHARACTER_INFO
	if (CharacterAttribute->Work_AttackSuccessRatePvP > 0)
	{
		unicode::_sprintf(strAttakMamage, GlobalText[2044], CharacterAttribute->Work_AttackSuccessRatePvP);
		iY += 13;
		g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strAttakMamage, text_size);
	}
#else
	if (iAttackRatingPK > 0)
	{
		if (itemoption380Attack != 0 || iAttackPowerRate != 0)
		{
			unicode::_sprintf(strAttakMamage, GlobalText[2109], iAttackRatingPK, itemoption380Attack + iAttackPowerRate);
		}
		else
		{
			unicode::_sprintf(strAttakMamage, GlobalText[2044], iAttackRatingPK);
		}

		iY += 13;

		g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strAttakMamage, text_size);
	}
#endif // ViewAttackSuccessRatePvP



	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);

	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion2))
	{
		g_pRenderText->SetTextColor(255, 120, 0, 255);
	}
	else
	{
		if (CharacterAttribute->AddDexterity)
		{
			g_pRenderText->SetTextColor(100, 150, 255, 255);
		}
		else
		{
			g_pRenderText->SetTextColor(230, 230, 0, 255);
		}
	}

	DWORD wDexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	unicode::_sprintf(strDexterity, "%s: %d", GlobalText[1702], wDexterity);
	g_pRenderText->RenderText(m_Pos.x + 34, RenderPosY + HEIGHT_DEXTERITY + 6, strDexterity);
#else
	if (gmProtect->checkold_school())
	{
		unicode::_sprintf(strDexterity, "%s: %d", GlobalText[1702], wDexterity);
		g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_DEXTERITY + 6, strDexterity);
		g_pRenderText->SetBgColor(0, 0, 0, 128);
	}
	else
	{
		unicode::_sprintf(strDexterity, "%d", wDexterity);
		g_pRenderText->RenderText(m_Pos.x + 12, RenderPosY + HEIGHT_DEXTERITY + 6, GlobalText[1702], 74, 0, RT3_SORT_CENTER);
		g_pRenderText->RenderText(m_Pos.x + 86, RenderPosY + HEIGHT_DEXTERITY + 6, strDexterity, 86, 0, RT3_SORT_CENTER);
	}
#endif // test_main_dev
#else
	unicode::_sprintf(strDexterity, "%s: %d", GlobalText[1702], wDexterity);
	g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_DEXTERITY + 6, strDexterity);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
#endif


	bool bDexSuccess = true;
	int iBaseClass = Hero->GetBaseClass();

	for (int i = EQUIPMENT_HELM; i <= EQUIPMENT_BOOTS; ++i)
	{
		if (iBaseClass == CLASS_DARK)
		{
			if ((CharacterMachine->Equipment[i].Type == -1 && (i != EQUIPMENT_HELM && iBaseClass == CLASS_DARK))
				|| (CharacterMachine->Equipment[i].Type != -1 && CharacterMachine->Equipment[i].Durability <= 0))
			{
				bDexSuccess = false;
				break;
			}
		}
		else if (iBaseClass == CLASS_RAGEFIGHTER)
		{
			if ((CharacterMachine->Equipment[i].Type == -1 && (i != EQUIPMENT_GLOVES && iBaseClass == CLASS_RAGEFIGHTER))
				|| (CharacterMachine->Equipment[i].Type != -1 && CharacterMachine->Equipment[i].Durability <= 0))
			{
				bDexSuccess = false;
				break;
			}
		}
		else
		{
			if ((CharacterMachine->Equipment[i].Type == -1) ||
				(CharacterMachine->Equipment[i].Type != -1 && CharacterMachine->Equipment[i].Durability <= 0))
			{
				bDexSuccess = false;
				break;
			}
		}
	}

	if (bDexSuccess)
	{
		int iType;
		if (iBaseClass == CLASS_DARK)
		{

			iType = CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type;

			if (
				(iType != ITEM_ARMOR + 15)
				&& (iType != ITEM_ARMOR + 20)
				&& (iType != ITEM_ARMOR + 23)
				&& (iType != ITEM_ARMOR + 32)
				&& (iType != ITEM_ARMOR + 37)
				&& (iType != ITEM_ARMOR + 47)
				&& (iType != ITEM_ARMOR + 48)
				)
			{
				bDexSuccess = false;
			}

			iType = iType % MAX_ITEM_INDEX;
		}
		else
		{
			iType = CharacterMachine->Equipment[EQUIPMENT_HELM].Type % MAX_ITEM_INDEX;
		}

		if (bDexSuccess)
		{
			for (int i = EQUIPMENT_ARMOR; i <= EQUIPMENT_BOOTS; ++i)
			{
				if (iBaseClass == CLASS_RAGEFIGHTER && i == EQUIPMENT_GLOVES)
					continue;

				if (iType != CharacterMachine->Equipment[i].Type % MAX_ITEM_INDEX)
				{
					bDexSuccess = false;
					break;
				}
			}
		}
	}

	int t_adjdef = CharacterAttribute->Defense + Add_Ch_Dfe;

	if (g_isCharacterBuff((&Hero->Object), eBuff_HelpNpc))
	{
		if (CharacterAttribute->Level > 180)
		{
			t_adjdef += 180 / 5 + 50;
		}
		else
		{
			t_adjdef += (CharacterAttribute->Level / 5 + 50);
		}
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_Berserker))
	{
		int nTemp = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
		float fTemp = (40 - int(CharacterAttribute->Energy / 60)) / 100.f;
		fTemp = MAX(fTemp, 0.1f);
		t_adjdef -= nTemp / 3 * fTemp;
	}

	int maxdefense = 0;

	for (int j = 0; j < MAX_EQUIPMENT; ++j)
	{
		int TempLevel = (CharacterMachine->Equipment[j].Level >> 3) & 15;
		if (TempLevel >= CharacterMachine->Equipment[j].Jewel_Of_Harmony_OptionLevel)
		{
			StrengthenCapability SC;

			g_pUIJewelHarmonyinfo->GetStrengthenCapability(&SC, &CharacterMachine->Equipment[j], 2);

			if (SC.SI_isSD)
			{
				maxdefense += SC.SI_SD.SI_defense;
			}
		}
	}

	int iChangeRingAddDefense = 0;

	pItemRingLeft = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];
	pItemRingRight = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];
	if (pItemRingLeft->Type == ITEM_HELPER + 39 || pItemRingRight->Type == ITEM_HELPER + 39)
	{
		iChangeRingAddDefense = (t_adjdef + maxdefense) / 10;
	}

	if (Hero->Helper.Type == MODEL_HELPER + 80)
		iChangeRingAddDefense += 50;

	if (Hero->Helper.Type == MODEL_HELPER + 106)
		iChangeRingAddDefense += 50;

	int nAdd_FulBlocking = 0;
	if (g_isCharacterBuff((&Hero->Object), eBuff_Def_up_Ourforces))
	{
#ifdef PBG_MOD_RAGEFIGHTERSOUND
		int _AddStat = (10 + (float)(CharacterAttribute->Energy - 80) / 10);
#else //PBG_MOD_RAGEFIGHTERSOUND
		int _AddStat = (10 + (float)(CharacterAttribute->Energy - 160) / 50);
#endif //PBG_MOD_RAGEFIGHTERSOUND

		if (_AddStat > 100)
			_AddStat = 100;

		_AddStat = CharacterAttribute->SuccessfulBlocking * _AddStat / 100;
		nAdd_FulBlocking += _AddStat;
	}

#ifdef SHUTDOWN_CHARACTER_INFO
	if (CharacterAttribute->SuccessfulBlocking > 0)
		unicode::_sprintf(strBlocking, GlobalText[208], CharacterAttribute->Work_Defense, CharacterAttribute->Work_DefenseSuccessRate);
	else
		unicode::_sprintf(strBlocking, GlobalText[209], CharacterAttribute->Work_Defense);
#else
	if (bDexSuccess)
	{
		// memorylock
		if (CharacterAttribute->SuccessfulBlocking > 0)
		{
			if (nAdd_FulBlocking)
				unicode::_sprintf(strBlocking, GlobalText[206], t_adjdef + maxdefense + iChangeRingAddDefense, CharacterAttribute->SuccessfulBlocking, (CharacterAttribute->SuccessfulBlocking) / 10 + nAdd_FulBlocking);
			else
				unicode::_sprintf(strBlocking, GlobalText[206], t_adjdef + maxdefense + iChangeRingAddDefense, CharacterAttribute->SuccessfulBlocking, (CharacterAttribute->SuccessfulBlocking) / 10);
		}
		else
		{
			unicode::_sprintf(strBlocking, GlobalText[207], t_adjdef + maxdefense + iChangeRingAddDefense, (t_adjdef + iChangeRingAddDefense) / 10);
		}
	}
	else
	{
		if (CharacterAttribute->SuccessfulBlocking > 0)
		{
			if (nAdd_FulBlocking)
				unicode::_sprintf(strBlocking, GlobalText[206], t_adjdef + maxdefense + iChangeRingAddDefense, CharacterAttribute->SuccessfulBlocking, nAdd_FulBlocking);
			else
				unicode::_sprintf(strBlocking, GlobalText[208], t_adjdef + maxdefense + iChangeRingAddDefense, CharacterAttribute->SuccessfulBlocking);
		}
		else
		{
			unicode::_sprintf(strBlocking, GlobalText[209], t_adjdef + maxdefense + iChangeRingAddDefense);
		}
	}
#endif // SHUTDOWN_CHARACTER_INFO

	iY = HEIGHT_DEXTERITY + 24;

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin3))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll2))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_Def_up_Ourforces))
	{
		g_pRenderText->SetTextColor(100, 150, 255, 255);
	}

	g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strBlocking, text_size);


#ifdef SHUTDOWN_CHARACTER_INFO
	unicode::_sprintf(strBlocking, GlobalText[64], CharacterAttribute->Work_PhysiSpeed);
#else
	DWORD wAttackSpeed = CLASS_WIZARD == iBaseClass || CLASS_SUMMONER == iBaseClass
		? CharacterAttribute->MagicSpeed : CharacterAttribute->AttackSpeed;

	unicode::_sprintf(strBlocking, GlobalText[64], wAttackSpeed);
#endif // SHUTDOWN_CHARACTER_INFO

	iY += 13;

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin1))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll1))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	ITEM* phelper = &CharacterMachine->Equipment[EQUIPMENT_HELPER];

	if (phelper->Durability != 0 && (phelper->Type == ITEM_HELPER + 64 || phelper->Type == ITEM_HELPER + 123))
	{
		if (IsRequireEquipItem(phelper))
		{
			if (false == pItemHelper->bExpiredPeriod)
			{
				g_pRenderText->SetTextColor(255, 0, 240, 255);
			}
		}
	}

	g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strBlocking, text_size);

#ifdef SHUTDOWN_CHARACTER_INFO
	unicode::_sprintf(strBlocking, GlobalText[2045], CharacterAttribute->Work_DefenseSuccessRatePvP);
#else
	if (itemoption380Defense != 0 || iDefenseRate != 0)
	{
		unicode::_sprintf(strBlocking, GlobalText[2110], CharacterAttribute->SuccessfulBlockingPK + Add_Dfe, itemoption380Defense + iDefenseRate);
	}
	else
	{
		unicode::_sprintf(strBlocking, GlobalText[2045], CharacterAttribute->SuccessfulBlockingPK + Add_Dfe);
	}
#endif // SHUTDOWN_CHARACTER_INFO



	iY += 13;
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strBlocking, text_size);


	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);

	DWORD wVitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;

	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion3))
	{
		g_pRenderText->SetTextColor(255, 120, 0, 255);
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_Hp_up_Ourforces))
	{
#ifdef PBG_MOD_RAGEFIGHTERSOUND
		CharacterMachine->CalculateAll();
		wVitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;
#else //PBG_MOD_RAGEFIGHTERSOUND
		DWORD _AddStat = (DWORD)(30 + (DWORD)((CharacterAttribute->Energy - 380) / 10));
		if (_AddStat > 200)
		{
			_AddStat = 200;
		}
		wVitality = CharacterAttribute->Vitality + _AddStat;
#endif //PBG_MOD_RAGEFIGHTERSOUND
		g_pRenderText->SetTextColor(100, 150, 255, 255);
	}
	else if (CharacterAttribute->AddVitality)
	{
		g_pRenderText->SetTextColor(100, 150, 255, 255);
	}
	else
	{
		g_pRenderText->SetTextColor(230, 230, 0, 255);
	}

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	unicode::_sprintf(strVitality, "%s: %d", GlobalText[169], wVitality);
	g_pRenderText->RenderText(m_Pos.x + 34, RenderPosY + HEIGHT_VITALITY + 6, strVitality);
#else
	if (gmProtect->checkold_school())
	{
		unicode::_sprintf(strVitality, "%s: %d", GlobalText[169], wVitality);
		g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_VITALITY + 6, strVitality);
		g_pRenderText->SetBgColor(0, 0, 0, 128);
	}
	else
	{
		unicode::_sprintf(strVitality, "%d", wVitality);
		g_pRenderText->RenderText(m_Pos.x + 12, RenderPosY + HEIGHT_VITALITY + 6, GlobalText[169], 74, 0, RT3_SORT_CENTER);
		g_pRenderText->RenderText(m_Pos.x + 86, RenderPosY + HEIGHT_VITALITY + 6, strVitality, 86, 0, RT3_SORT_CENTER);
	}
#endif // test_main_dev

#else
	unicode::_sprintf(strVitality, "%s: %d", GlobalText[169], wVitality);
	g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_VITALITY + 6, strVitality);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
#endif

	g_pRenderText->SetFont(g_hFont);

	unicode::_sprintf(strVitality, GlobalText[211], CharacterAttribute->Life, CharacterAttribute->LifeMax);

	g_pRenderText->SetTextColor(255, 255, 255, 255);

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin4))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll5))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_RiceCake))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (phelper->Durability != 0 && phelper->Type == ITEM_HELPER + 65)
	{
		if (IsRequireEquipItem(phelper))
		{
			if (false == pItemHelper->bExpiredPeriod)
			{
				g_pRenderText->SetTextColor(255, 0, 240, 255);
			}
		}
	}

	iY = HEIGHT_VITALITY + 24;
	g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strVitality, text_size);

	if (iBaseClass == CLASS_RAGEFIGHTER)
	{
		iY += 13;
#ifdef SHUTDOWN_CHARACTER_INFO
		unicode::_sprintf(strVitality, GlobalText[3155], CharacterAttribute->Work_RFDamageMultiplierA);
#else
		unicode::_sprintf(strVitality, GlobalText[3155], 50 + (wVitality / 10));
#endif // SHUTDOWN_CHARACTER_INFO


		g_pRenderText->RenderText(m_Pos.x + 20, RenderPosY + iY, strVitality, text_size);
	}

	g_pRenderText->SetFont(g_hFontBold);

	DWORD wEnergy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;

	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion4))
	{
		g_pRenderText->SetTextColor(255, 120, 0, 255);
	}
	else if (CharacterAttribute->AddEnergy)
	{
		g_pRenderText->SetTextColor(100, 150, 255, 255);
	}
	else
	{
		g_pRenderText->SetTextColor(230, 230, 0, 255);
	}

	unicode::t_char strEnergy[256];

	g_pRenderText->SetBgColor(0);

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	unicode::_sprintf(strEnergy, "%s: %d", GlobalText[168], wEnergy);
	g_pRenderText->RenderText(m_Pos.x + 34, RenderPosY + HEIGHT_ENERGY + 6, strEnergy);
#else
	if (gmProtect->checkold_school())
	{
		unicode::_sprintf(strEnergy, "%s: %d", GlobalText[168], wEnergy);
		g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_ENERGY + 6, strEnergy);
		g_pRenderText->SetBgColor(0, 0, 0, 128);
	}
	else
	{
		unicode::_sprintf(strEnergy, "%d", wEnergy);
		g_pRenderText->RenderText(m_Pos.x + 12, RenderPosY + HEIGHT_ENERGY + 6, GlobalText[168], 74, 0, RT3_SORT_CENTER);
		g_pRenderText->RenderText(m_Pos.x + 86, RenderPosY + HEIGHT_ENERGY + 6, strEnergy, 86, 0, RT3_SORT_CENTER);
	}
#endif // test_main_dev

#else
	unicode::_sprintf(strEnergy, "%s: %d", GlobalText[168], wEnergy);
	g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_ENERGY + 6, strEnergy);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
#endif

	g_pRenderText->SetFont(g_hFont);

	unicode::_sprintf(strEnergy, GlobalText[213], CharacterAttribute->Mana, CharacterAttribute->ManaMax);

	g_pRenderText->SetTextColor(255, 255, 255, 255);

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin5))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll6))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_Liguor))
	{
		g_pRenderText->SetTextColor(255, 0, 240, 255);
	}

	iY = HEIGHT_ENERGY + 24;

	g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);

	if (iBaseClass == CLASS_WIZARD || iBaseClass == CLASS_DARK || iBaseClass == CLASS_SUMMONER)
	{
		int Level = (pWeaponRight->Level >> 3) & 15;
		int iMagicDamageMin;
		int iMagicDamageMax;

		gCharacterManager.GetMagicSkillDamage(CharacterAttribute->Skill[Hero->CurrentSkill], &iMagicDamageMin, &iMagicDamageMax);

		int iMagicDamageMinInitial = iMagicDamageMin;
		int iMagicDamageMaxInitial = iMagicDamageMax;

		iMagicDamageMin += Add_Mana_Min;
		iMagicDamageMax += Add_Mana_Max;

		if (CharacterAttribute->Ability & ABILITY_PLUS_DAMAGE)
		{
			iMagicDamageMin += 10;
			iMagicDamageMax += 10;
		}

		int maxMg = 0;

		for (int j = 0; j < MAX_EQUIPMENT; ++j)
		{
			int TempLevel = (CharacterMachine->Equipment[j].Level >> 3) & 15;

			if (TempLevel >= CharacterMachine->Equipment[j].Jewel_Of_Harmony_OptionLevel)
			{
				StrengthenCapability SC;
				g_pUIJewelHarmonyinfo->GetStrengthenCapability(&SC, &CharacterMachine->Equipment[j], 1);

				if (SC.SI_isSP)
				{
					maxMg += SC.SI_SP.SI_magicalpower;
				}
			}
		}

		pItemRingLeft = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];
		pItemRingRight = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];


		int iNonExpiredLRingType = -1;
		int iNonExpiredRRingType = -1;

		if (!pItemRingLeft->bPeriodItem || !pItemRingLeft->bExpiredPeriod)
		{
			iNonExpiredLRingType = pItemRingLeft->Type;
		}
		if (!pItemRingRight->bPeriodItem || !pItemRingRight->bExpiredPeriod)
		{
			iNonExpiredRRingType = pItemRingRight->Type;
		}

		int maxIMagicDamageMin = 0;
		int maxIMagicDamageMax = 0;

		if (iNonExpiredLRingType == ITEM_HELPER + 41 || iNonExpiredRRingType == ITEM_HELPER + 41)
		{
			maxIMagicDamageMin = max(maxIMagicDamageMin, 20);
			maxIMagicDamageMax = max(maxIMagicDamageMax, 20);
		}
		if (iNonExpiredLRingType == ITEM_HELPER + 76 || iNonExpiredRRingType == ITEM_HELPER + 76)
		{
			maxIMagicDamageMin = max(maxIMagicDamageMin, 30);
			maxIMagicDamageMax = max(maxIMagicDamageMax, 30);
		}
		if (iNonExpiredLRingType == ITEM_HELPER + 122 || iNonExpiredRRingType == ITEM_HELPER + 122)
		{
			maxIMagicDamageMin = max(maxIMagicDamageMin, 40);
			maxIMagicDamageMax = max(maxIMagicDamageMax, 40);
		}

		iMagicDamageMin += maxIMagicDamageMin;
		iMagicDamageMax += maxIMagicDamageMax;

		pItemHelper = &CharacterMachine->Equipment[EQUIPMENT_HELPER];
		if (pItemHelper)
		{
			if (pItemHelper->Type == ITEM_HELPER + 37 && pItemHelper->Option1 == 0x04)
			{
				DWORD wLevel = CharacterAttribute->Level;
				iMagicDamageMin += (wLevel / 25);
				iMagicDamageMax += (wLevel / 25);
			}

			if (pItemHelper->Type == ITEM_HELPER + 64)
			{
				if (false == pItemHelper->bExpiredPeriod)
				{
					iMagicDamageMin += int(float(iMagicDamageMin) * 0.4f);
					iMagicDamageMax += int(float(iMagicDamageMax) * 0.4f);
				}

			}
			if (pItemHelper->Type == ITEM_HELPER + 123)
			{
				if (false == pItemHelper->bExpiredPeriod)
				{
					iMagicDamageMin += int(float(iMagicDamageMin) * 0.2f);
					iMagicDamageMax += int(float(iMagicDamageMax) * 0.2f);
				}
			}
			if (pItemHelper->Type == ITEM_HELPER + 1)
			{
				iMagicDamageMin += int(float(iMagicDamageMin) * 0.3f);
				iMagicDamageMax += int(float(iMagicDamageMax) * 0.3f);
			}
		}

		if (g_isCharacterBuff((&Hero->Object), eBuff_Berserker))
		{
			int nTemp = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
			float fTemp = int(CharacterAttribute->Energy / 30) / 100.f;
			iMagicDamageMin += nTemp / 9 * fTemp;
			iMagicDamageMax += nTemp / 4 * fTemp;
		}

		Script_Item* p = GMItemMng->find(pWeaponRight->Type);

#ifdef SHUTDOWN_CHARACTER_INFO
		DWORD _MagicDamageMin = iMagicDamageMin;
		DWORD _MagicDamageMax = iMagicDamageMax;

		if (p && (p->Kind2 == ItemKind2::BOOK
			|| p->Kind2 == ItemKind2::MAGIC_SWORD
			|| p->Kind2 == ItemKind2::STAFF
			|| p->Kind2 == ItemKind2::STICK))
		{
			DWORD magicPercent = (float)(pWeaponRight->MagicPower) / 100;

			CharacterAttribute->GetMagicDamage(&_MagicDamageMin, &_MagicDamageMax, &magicPercent);
			unicode::_sprintf(strEnergy, GlobalText[215], _MagicDamageMin, _MagicDamageMax, magicPercent);
		}
		else
		{
			CharacterAttribute->GetMagicDamage(&_MagicDamageMin, &_MagicDamageMax);
			unicode::_sprintf(strEnergy, GlobalText[216], _MagicDamageMin, _MagicDamageMax);
		}
#else
		if (p && (p->Kind2 == ItemKind2::BOOK
			|| p->Kind2 == ItemKind2::MAGIC_SWORD
			|| p->Kind2 == ItemKind2::STAFF
			|| p->Kind2 == ItemKind2::STICK))
		{
			float magicPercent = (float)(pWeaponRight->MagicPower) / 100;

			float   percent = CalcDurabilityPercent(pWeaponRight->Durability, p->MagicDur, pWeaponRight->Level, pWeaponRight->Option1, pWeaponRight->ExtOption);

			magicPercent = magicPercent - magicPercent * percent;
			unicode::_sprintf(strEnergy, GlobalText[215], iMagicDamageMin + maxMg, iMagicDamageMax + maxMg, (int)((iMagicDamageMaxInitial + maxMg) * magicPercent));
		}
		else
		{
			unicode::_sprintf(strEnergy, GlobalText[216], iMagicDamageMin + maxMg, iMagicDamageMax + maxMg);
		}
#endif // SHUTDOWN_CHARACTER_INFO


		iY += 13;

		g_pRenderText->SetTextColor(255, 255, 255, 255);

		if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin2))
		{
			g_pRenderText->SetTextColor(255, 0, 240, 255);
		}

		if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll4))
		{
			g_pRenderText->SetTextColor(255, 0, 240, 255);
		}

		if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_Petal))
		{
			g_pRenderText->SetTextColor(255, 0, 240, 255);
		}
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}

	if (iBaseClass == CLASS_SUMMONER)
	{
		int iCurseDamageMin = 0;
		int iCurseDamageMax = 0;

		gCharacterManager.GetCurseSkillDamage(CharacterAttribute->Skill[Hero->CurrentSkill], &iCurseDamageMin, &iCurseDamageMax);

		if (g_isCharacterBuff((&Hero->Object), eBuff_Berserker))
		{
			int nTemp = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
			float fTemp = int(CharacterAttribute->Energy / 30) / 100.f;
			iCurseDamageMin += nTemp / 9 * fTemp;
			iCurseDamageMax += nTemp / 4 * fTemp;
		}

		int iNonExpiredLRingType = -1;
		int iNonExpiredRRingType = -1;

		if (!pItemRingLeft->bPeriodItem || !pItemRingLeft->bExpiredPeriod)
		{
			iNonExpiredLRingType = pItemRingLeft->Type;
		}
		if (!pItemRingRight->bPeriodItem || !pItemRingRight->bExpiredPeriod)
		{
			iNonExpiredRRingType = pItemRingRight->Type;
		}

		int maxICurseDamageMin = 0;
		int maxICurseDamageMax = 0;

		if (iNonExpiredLRingType == ITEM_HELPER + 76 || iNonExpiredRRingType == ITEM_HELPER + 76)
		{
			maxICurseDamageMin = max(maxICurseDamageMin, 30);
			maxICurseDamageMax = max(maxICurseDamageMax, 30);
		}
		if (iNonExpiredLRingType == ITEM_HELPER + 122 || iNonExpiredRRingType == ITEM_HELPER + 122)
		{
			maxICurseDamageMin = max(maxICurseDamageMin, 40);
			maxICurseDamageMax = max(maxICurseDamageMax, 40);
		}

		iCurseDamageMin += maxICurseDamageMin;
		iCurseDamageMax += maxICurseDamageMax;

		pItemHelper = &CharacterMachine->Equipment[EQUIPMENT_HELPER];
		if (pItemHelper)
		{
			if (pItemHelper->Type == ITEM_HELPER + 64)
			{
				if (false == pItemHelper->bExpiredPeriod)
				{
					iCurseDamageMin += int(float(iCurseDamageMin) * 0.4f);
					iCurseDamageMax += int(float(iCurseDamageMax) * 0.4f);
				}
			}
			if (pItemHelper->Type == ITEM_HELPER + 123)
			{
				if (false == pItemHelper->bExpiredPeriod)
				{
					iCurseDamageMin += int(float(iCurseDamageMin) * 0.2f);
					iCurseDamageMax += int(float(iCurseDamageMax) * 0.2f);
				}
			}
		}

		int Weapon02Kind2 = GMItemMng->GetKind2(pWeaponLeft->Type);

#ifdef SHUTDOWN_CHARACTER_INFO
		DWORD _CurseDamageMin = iCurseDamageMin;
		DWORD _iCurseDamageMax = iCurseDamageMax;

		if (Weapon02Kind2 == ItemKind2::BOOK)
		{
			DWORD fCursePercent =0;
			CharacterAttribute->GetCurseDamage(&_CurseDamageMin, &_iCurseDamageMax, &fCursePercent);
			unicode::_sprintf(strEnergy, GlobalText[1693], iCurseDamageMin, iCurseDamageMax, fCursePercent);
		}
		else
		{
			CharacterAttribute->GetCurseDamage(&_CurseDamageMin, &_iCurseDamageMax);
			unicode::_sprintf(strEnergy, GlobalText[1694], _CurseDamageMin, _iCurseDamageMax);
		}
#else
		if (Weapon02Kind2 == ItemKind2::BOOK)
		{
			float fCursePercent = (float)(pWeaponLeft->MagicPower) / 100;

			Script_Item* p = GMItemMng->find(pWeaponLeft->Type);
			float fPercent = ::CalcDurabilityPercent(pWeaponLeft->Durability, p->MagicDur, pWeaponLeft->Level, pWeaponLeft->Option1, pWeaponLeft->ExtOption);

			fCursePercent -= fCursePercent * fPercent;
			unicode::_sprintf(strEnergy, GlobalText[1693], iCurseDamageMin, iCurseDamageMax, (int)((iCurseDamageMax)*fCursePercent));
		}
		else
		{
			unicode::_sprintf(strEnergy, GlobalText[1694], iCurseDamageMin, iCurseDamageMax);
		}
#endif // SHUTDOWN_CHARACTER_INFO

		iY += 13;
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}

	iY += 13;

#ifdef SHUTDOWN_CHARACTER_INFO
	if (iBaseClass == CLASS_KNIGHT || iBaseClass == CLASS_DARK || iBaseClass == CLASS_DARK_LORD) // || iBaseClass == CLASS_RAGEFIGHTER)
	{
		unicode::_sprintf(strEnergy, GlobalText[582], CharacterAttribute->Work_DamageMultiplier);
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}

	if (iBaseClass == CLASS_RAGEFIGHTER)
	{
		unicode::_sprintf(strEnergy, GlobalText[3156], CharacterAttribute->Work_RFDamageMultiplierB);
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
		iY += 13;
		unicode::_sprintf(strEnergy, GlobalText[3157], CharacterAttribute->Work_RFDamageMultiplierC);
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}
#else
	if (iBaseClass == CLASS_KNIGHT)
	{
		unicode::_sprintf(strEnergy, GlobalText[582], 200 + (wEnergy / 10));
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}
	if (iBaseClass == CLASS_DARK)
	{
		unicode::_sprintf(strEnergy, GlobalText[582], 200);
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}
	if (iBaseClass == CLASS_DARK_LORD)
	{
		unicode::_sprintf(strEnergy, GlobalText[582], 200 + (wEnergy / 20));
		g_pRenderText->RenderText(m_Pos.x + 18, RenderPosY + iY, strEnergy, text_size);
	}
	if (iBaseClass == CLASS_RAGEFIGHTER)
	{
		unicode::_sprintf(strEnergy, GlobalText[3156], 50 + (wEnergy / 10));
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + iY, strEnergy, text_size);
		iY += 13;
		unicode::_sprintf(strEnergy, GlobalText[3157], 100 + (wDexterity / 8 + wEnergy / 10));
		g_pRenderText->RenderText(m_Pos.x + 18, m_Pos.y + iY, strEnergy, text_size);
	}
#endif // SHUTDOWN_CHARACTER_INFO

	if (iBaseClass == CLASS_DARK_LORD)
	{
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetFont(g_hFontBold);

		DWORD wCharisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;

		if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion5))
		{
			g_pRenderText->SetTextColor(255, 120, 0, 255);
		}
		else if (CharacterAttribute->AddCharisma)
		{
			g_pRenderText->SetTextColor(100, 150, 255, 255);
		}
		else
		{
			g_pRenderText->SetTextColor(230, 230, 0, 255);
		}

#if MAIN_UPDATE > 303
		unicode::t_char strCharisma[256];
#ifdef test_main_dev
		unicode::_sprintf(strCharisma, "%s: %d", GlobalText[1900], wCharisma);
		g_pRenderText->RenderText(m_Pos.x + 34, RenderPosY + HEIGHT_CHARISMA + 6, strCharisma);
#else
		if (gmProtect->checkold_school())
		{
			unicode::_sprintf(strCharisma, "%s: %d", GlobalText[1900], wCharisma);
			g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_CHARISMA + 6, strCharisma);
		}
		else
		{
			unicode::_sprintf(strCharisma, "%d", wCharisma);
			g_pRenderText->RenderText(m_Pos.x + 12, RenderPosY + HEIGHT_CHARISMA + 6, GlobalText[1900], 74, 0, RT3_SORT_CENTER);
			g_pRenderText->RenderText(m_Pos.x + 86, RenderPosY + HEIGHT_CHARISMA + 6, strCharisma, 86, 0, RT3_SORT_CENTER);
		}
#endif // test_main_dev
#else
		unicode::t_char strCharisma[256];
		unicode::_sprintf(strCharisma, "%s: %d", GlobalText[1900], wCharisma);
		g_pRenderText->RenderText(m_Pos.x + 24, RenderPosY + HEIGHT_CHARISMA + 6, strCharisma);
#endif
	}
}

void SEASON3B::CNewUICharacterInfoWindow::RenderButtons()
{
	float PosX, PosY;

#if MAIN_UPDATE > 303
#ifdef test_main_dev
	PosX = m_Pos.x + 12;
	PosY = m_Pos.y + 32;
#else
	PosX = m_Pos.x + 12;
	PosY = m_Pos.y + 42;
#endif // test_main_dev
#else
	PosX = m_Pos.x + 12;
	PosY = m_Pos.y + 32;
#endif // MAIN_UPDATE != 303

	int Text[] = { 681 , 685 , 0 };

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(CLRDW_WHITE);

#ifdef test_main_dev
	for (int i = 0; i < 2; i++)
	{
		if (i == m_iCurTab)
		{
			SEASON3B::RenderImageF(BITMAP_guild_tab04, PosX, PosY, 56.f, 22.f, 0.0, 77.f, 173.f, 77.f);
			g_pRenderText->RenderFont(PosX, PosY, GlobalText[Text[i]], 56, 22, RT3_SORT_CENTER);
		}
		else
		{
			SEASON3B::RenderImageF(BITMAP_guild_tab04, PosX, PosY, 56.f, 22.f, 0.0, 0.0, 173.f, 77.f);
			g_pRenderText->RenderFont(PosX, PosY + 2, GlobalText[Text[i]], 56, 22, RT3_SORT_CENTER);
		}
		PosX += 57;
	}
#else
	for (int i = 0; i < 2; i++)
	{
		if (i == m_iCurTab)
		{

			SEASON3B::RenderImage(CNewUIPetInfoWindow::IMAGE_PETINFO_TAB_BUTTON, PosX, PosY, 56.f, 22.f, 0.0, 22.f);
			g_pRenderText->RenderFont(PosX, PosY, GlobalText[Text[i]], 56, 22, RT3_SORT_CENTER);
		}
		else
		{
			SEASON3B::RenderImage(CNewUIPetInfoWindow::IMAGE_PETINFO_TAB_BUTTON, PosX, PosY, 56.f, 22.f, 0.0, 0.0);
			g_pRenderText->RenderFont(PosX, PosY + 2, GlobalText[Text[i]], 56, 22, RT3_SORT_CENTER);
		}
		PosX += 57;
	}
#endif // test_main_dev


#ifdef test_main_dev
	if (m_iCurTab == 1)
	{
		m_BtnExit.Render();
		m_pScrollBar.Render();
	}
	else
	{
		int iCount = 0;

		if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD)
			iCount = 5;
		else
			iCount = 4;

		if (CharacterAttribute->LevelUpPoint > 0)
		{
			for (int i = 0; i < iCount; ++i)
			{
				m_BtnStat[i].Render();
			}
		}

		m_BtnExit.Render();
		m_BtnQuest.Render();
		m_BtnPet.Render();

		if (GMProtect->IsMasterSkill() == true)
		{
			m_BtnMasterLevel.Render();
		}
	}
#else
	if (m_iCurTab == 1)
	{
		m_BtnExit.Render();
		m_BtnQuest.Render();
		m_BtnPet.Render();
#if MAIN_UPDATE > 303
		if (GMProtect->IsMasterSkill() == true)
		{
			m_BtnMasterLevel.Render();
		}
#endif // MAIN_UPDATE != 303

		m_pScrollBar.Render();
	}
	else
	{
		int iBaseClass = gCharacterManager.GetBaseClass(Hero->Class);
		int iCount = 0;
		if (iBaseClass == CLASS_DARK_LORD)
		{
			iCount = 5;
		}
		else
		{
			iCount = 4;
		}

		if (CharacterAttribute->LevelUpPoint > 0)
		{
			for (int i = 0; i < iCount; ++i)
			{
				m_BtnStat[i].Render();
			}
		}

		m_BtnExit.Render();
		m_BtnQuest.Render();
		m_BtnPet.Render();

#if MAIN_UPDATE > 303
		if (GMProtect->IsMasterSkill() == true)
		{
			m_BtnMasterLevel.Render();
		}
#endif // MAIN_UPDATE != 303
			}
#endif // test_main_dev
}

void SEASON3B::CNewUICharacterInfoWindow::RenderAdavanceInfo()
{
	int PosX = m_Pos.x;
	int PosY = m_Pos.y;

	char strText[128];

//#ifdef test_main_dev
//	PosY -= 10;
//#endif // test_main_dev
	//g_pRenderText->SetFont(g_hFontBold);
	//RenderImageF(32241, PosX + 20, PosY + 70, 28.0, 28.0, 0.0, 84.5, 84.5, 84.5);

#if MAIN_UPDATE >= 303
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0, 0, 0, 0);
	g_pRenderText->SetTextColor(100, 150, 255, 255);

	if (CharacterMachine->Lock)
		sprintf_s(strText, gTextClien.TextClien_ThongTin[0]); //Lock: Active z
	
	else
		sprintf_s(strText, gTextClien.TextClien_ThongTin[1]); //Lock: Disabled
	g_pRenderText->RenderFont(PosX + 20, PosY + 70, strText, 150, 0, RT3_SORT_LEFT);

	sprintf_s(strText, gTextClien.TextClien_ThongTin[2], CharacterMachine->AccountLevel); //AccountLevel: %d
	g_pRenderText->RenderFont(PosX + 20, PosY + 86, strText, 150, 0, RT3_SORT_LEFT);

	if (CharacterMachine->AccountExpireDate[0] != '\0')
		sprintf_s(strText, gTextClien.TextClien_ThongTin[3], CharacterMachine->AccountExpireDate); //Accoun Expire: %s
	else
		sprintf_s(strText, gTextClien.TextClien_ThongTin[4]); //Accoun Expire: --------

	g_pRenderText->RenderFont(PosX + 20, PosY + 102, strText, 150, 0, RT3_SORT_LEFT);
#else
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0, 0, 0, 128);
	g_pRenderText->SetTextColor(100, 150, 255, 255);

	if (CharacterMachine->Lock)
		sprintf_s(strText, "Lock: Active");
	else
		sprintf_s(strText, "Lock: Disabled");
	g_pRenderText->RenderFont(PosX + 20, PosY + 70, strText, 150, 0, RT3_SORT_LEFT);

	sprintf_s(strText, "AccountLevel: %d", CharacterMachine->AccountLevel);
	g_pRenderText->RenderFont(PosX + 20, PosY + 86, strText, 150, 0, RT3_SORT_LEFT);

	if (CharacterMachine->AccountExpireDate[0] != '\0')
		sprintf_s(strText, "Accoun Expire: %s", CharacterMachine->AccountExpireDate);
	else
		sprintf_s(strText, "Accoun Expire: --------");

	g_pRenderText->RenderFont(PosX + 20, PosY + 102, strText, 150, 0, RT3_SORT_LEFT);
#endif // MAIN_UPDATE


	//g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 80, strExp, 150);

	//sprintf_s(strText, "Gear. Power");
	//g_pRenderText->RenderFont(PosX + 56, PosY + 70, strText, 0, 15, RT3_SORT_LEFT);
	//
	////--
	//g_pRenderText->SetTextColor(CLRDW_WHITE3);
	//g_pRenderText->SetFont(g_hFont);
	//sprintf_s(strText, "Kill Count: %d", 0);
	//g_pRenderText->RenderFont(PosX + 95, PosY + 102, strText, 0, 0, RT3_SORT_LEFT);
	//
	//sprintf_s(strText, "Dead Count: %d", 0);
	//g_pRenderText->RenderFont(PosX + 95, PosY + 114, strText, 0, 0, RT3_SORT_LEFT);

	//glColor4f(0.8, 0.8, 0.8, 1.f);
	//SEASON3B::RenderNumber(PosX + 80, PosY + 85, 0, 0.85);
	//glColor4f(1.f, 1.f, 1.f, 1.f);
}

void SEASON3B::CNewUICharacterInfoWindow::RenderEquipementAttribute()
{
	int PosX = m_Pos.x;
	int PosY = m_Pos.y;

	int Loop = m_advanceChar.size();
	int currentLoop = 0;

	if (Loop > 15)
	{
		double prev = m_pScrollBar.GetPercent();
		currentLoop = (int)((double)(unsigned int)(Loop - 15) * prev);
	}

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(CLRDW_GOLD);
	g_pRenderText->RenderFont(PosX + 25, PosY + 135, gTextClien.TextClien_ThongTin[5], 0, 0, RT3_SORT_LEFT); //"-Effects and Options"

	int secure = 0;
	g_pRenderText->SetFont(g_hFont);
	for (int i = currentLoop; i < Loop && secure < 15; i++, secure++)
	{
		if (m_advanceChar[i].CheckTittle())
		{
			EnableAlphaTest();
			glColor4f(0.0, 0.0, 0.0, 0.8);
			RenderColor((int)PosX + 19, (int)PosY + (secure * 14) + 158, WIN_WINDOW_SIZEX - 58, 14.f, 0.0, 0);
			EndRenderColor();
		}
		else
		{
			if (i % 2 == 0)
			{
				EnableAlphaTest();
				glColor4f(0.2, 0.2, 0.2, 0.8);
				RenderColor((int)PosX + 19, (int)PosY + (secure * 14) + 158, WIN_WINDOW_SIZEX - 58, 14.f, 0.0, 0);
				EndRenderColor();
			}
		}
		m_advanceChar[i].Render((int)PosX + 21, (int)PosY + (secure * 14) + 158, WIN_WINDOW_SIZEX - 62, false);
	}
}

void SEASON3B::CNewUICharacterInfoWindow::RenderTable(float x, float y, float width, float height)
{
	EnableAlphaTest();
	glColor4f(0.0, 0.0, 0.0, 0.40000001);
	RenderColor((x + 3), (y + 2), (width - 7), (height - 7), 0.0, 0);
	EndRenderColor();

	RenderImage(IMAGE_MAIN_TABLE_TOP_LEFT, x, y, 14.0, 14.0);
	RenderImage(IMAGE_MAIN_TABLE_TOP_RIGHT, (x + width - 14), y, 14.0, 14.0);
	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_LEFT, x, (y + height - 14), 14.0, 14.0);
	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_RIGHT, (x + width - 14), (y + height - 14), 14.0, 14.0);
	RenderImage(IMAGE_MAIN_TABLE_TOP_PIXEL, (x + 6), y, (width - 12), 14.0);
	RenderImage(IMAGE_MAIN_TABLE_RIGHT_PIXEL, (x + width - 14), (y + 6), 14.0, (height - 14));
	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_PIXEL, (x + 6), (y + height - 14), (width - 12), 14.0);
	RenderImage(IMAGE_MAIN_TABLE_LEFT_PIXEL, x, (y + 6), 14.0, (height - 14));
}

float SEASON3B::CNewUICharacterInfoWindow::GetLayerDepth()
{
	return 5.1f;
}

void SEASON3B::CNewUICharacterInfoWindow::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, m_Pos.y);
}

void SEASON3B::CNewUICharacterInfoWindow::LoadImages()
{
#ifdef test_main_dev
	LoadBitmap("Interface\\HUD_TEST\\interface_back1.dds", BITMAP_interface_back1, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\interface_back2.dds", BITMAP_interface_back2, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\cha_textbox.dds", BITMAP_chainfo_textbox, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\chainfo_btn_level.dds", BITMAP_chainfo_btn_level, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\chainfo_btn_quest.dds", BITMAP_chainfo_btn_quest, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\chainfo_btn_pet.dds", BITMAP_chainfo_btn_pet, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\btn_exit_00.dds", BITMAP_btn_exit_00, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\guild_tab04.dds", BITMAP_guild_tab04, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\chainfo_btn_master.dds", BITMAP_chainfo_btn_master, GL_LINEAR);	
#else
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_CHAINFO_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back04.tga", IMAGE_CHAINFO_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_CHAINFO_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_CHAINFO_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_CHAINFO_BOTTOM, GL_LINEAR);

	LoadBitmap("Interface\\newui_item_table01(L).tga", IMAGE_CHAINFO_TABLE_TOP_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table01(R).tga", IMAGE_CHAINFO_TABLE_TOP_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table02(L).tga", IMAGE_CHAINFO_TABLE_BOTTOM_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table02(R).tga", IMAGE_CHAINFO_TABLE_BOTTOM_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table03(Up).tga", IMAGE_CHAINFO_TABLE_TOP_PIXEL, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table03(Dw).tga", IMAGE_CHAINFO_TABLE_BOTTOM_PIXEL, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table03(L).tga", IMAGE_CHAINFO_TABLE_LEFT_PIXEL, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_table03(R).tga", IMAGE_CHAINFO_TABLE_RIGHT_PIXEL, GL_LINEAR);

	LoadBitmap("Interface\\newui_exit_00.tga", IMAGE_CHAINFO_BTN_EXIT, GL_LINEAR);

	LoadBitmap("Interface\\newui_cha_textbox02.tga", IMAGE_CHAINFO_TEXTBOX, GL_LINEAR);
	LoadBitmap("Interface\\newui_chainfo_btn_level.tga", IMAGE_CHAINFO_BTN_STAT, GL_LINEAR);
	LoadBitmap("Interface\\newui_chainfo_btn_quest.tga", IMAGE_CHAINFO_BTN_QUEST, GL_LINEAR);
	LoadBitmap("Interface\\newui_chainfo_btn_pet.tga", IMAGE_CHAINFO_BTN_PET, GL_LINEAR);

#if MAIN_UPDATE > 303
	if (GMProtect->IsMasterSkill() == true)
	{
		LoadBitmap("Interface\\newui_chainfo_btn_master.tga", IMAGE_CHAINFO_BTN_MASTERLEVEL, GL_LINEAR);
	}
#endif // MAIN_UPDATE != 303
#endif // test_main_dev
}

void SEASON3B::CNewUICharacterInfoWindow::UnloadImages()
{
#ifdef test_main_dev
	DeleteBitmap(BITMAP_interface_back1);
	DeleteBitmap(BITMAP_interface_back2);
	DeleteBitmap(BITMAP_chainfo_textbox);
	DeleteBitmap(BITMAP_chainfo_btn_level);
	DeleteBitmap(BITMAP_chainfo_btn_quest);
	DeleteBitmap(BITMAP_chainfo_btn_pet);
	DeleteBitmap(BITMAP_chainfo_btn_master);
	DeleteBitmap(BITMAP_btn_exit_00);
	
#else
#if MAIN_UPDATE > 303
	DeleteBitmap(IMAGE_CHAINFO_BTN_MASTERLEVEL);
#endif // MAIN_UPDATE != 303

	DeleteBitmap(IMAGE_CHAINFO_BTN_PET);
	DeleteBitmap(IMAGE_CHAINFO_BTN_QUEST);
	DeleteBitmap(IMAGE_CHAINFO_BTN_STAT);
	DeleteBitmap(IMAGE_CHAINFO_TEXTBOX);

	DeleteBitmap(IMAGE_CHAINFO_BTN_EXIT);

	DeleteBitmap(IMAGE_CHAINFO_BOTTOM);
	DeleteBitmap(IMAGE_CHAINFO_RIGHT);
	DeleteBitmap(IMAGE_CHAINFO_LEFT);
	DeleteBitmap(IMAGE_CHAINFO_TOP);
	DeleteBitmap(IMAGE_CHAINFO_BACK);

	DeleteBitmap(IMAGE_CHAINFO_TABLE_RIGHT_PIXEL);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_LEFT_PIXEL);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_BOTTOM_PIXEL);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_TOP_PIXEL);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_BOTTOM_RIGHT);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_BOTTOM_LEFT);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_TOP_RIGHT);
	DeleteBitmap(IMAGE_CHAINFO_TABLE_TOP_LEFT);
#endif // test_main_dev

}

void SEASON3B::CNewUICharacterInfoWindow::OpenningProcess()
{
	m_iCurTab = 0;
	ResetEquipmentLevel();

#if MAIN_UPDATE > 303
	if (GMProtect->IsMasterSkill() == true)
	{
		if (gCharacterManager.IsMasterLevel(Hero->Class) == true)
		{
			m_BtnMasterLevel.UnLock();
			m_BtnMasterLevel.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
			m_BtnMasterLevel.ChangeTextColor(RGBA(255, 255, 255, 255));
		}
		else
		{
			m_BtnMasterLevel.Lock();
			m_BtnMasterLevel.ChangeImgColor(BUTTON_STATE_UP, RGBA(100, 100, 100, 255));
			m_BtnMasterLevel.ChangeTextColor(RGBA(100, 100, 100, 255));
		}
	}
#endif // MAIN_UPDATE != 303

	g_csItemOption.init();

	if (CharacterMachine->IsZeroDurability())
	{
		CharacterMachine->CalculateAll();
	}

	if (g_QuestMng.IsIndexInCurQuestIndexList(0x10009))
	{
		if (g_QuestMng.IsEPRequestRewardState(0x10009))
		{
			g_pMyQuestInfoWindow->UnselectQuestList();
			SendSatisfyQuestRequestFromClient(0x10009);
			g_QuestMng.SetEPRequestRewardState(0x10009, false);
		}
	}
}

void SEASON3B::CNewUICharacterInfoWindow::ResetEquipmentLevel()
{
	ITEM* pItem = CharacterMachine->Equipment;
	Hero->Weapon[0].Level = (pItem[EQUIPMENT_WEAPON_RIGHT].Level >> 3) & 15;
	Hero->Weapon[1].Level = (pItem[EQUIPMENT_WEAPON_LEFT].Level >> 3) & 15;
	Hero->BodyPart[BODYPART_HELM].Level = (pItem[EQUIPMENT_HELM].Level >> 3) & 15;
	Hero->BodyPart[BODYPART_ARMOR].Level = (pItem[EQUIPMENT_ARMOR].Level >> 3) & 15;
	Hero->BodyPart[BODYPART_PANTS].Level = (pItem[EQUIPMENT_PANTS].Level >> 3) & 15;
	Hero->BodyPart[BODYPART_GLOVES].Level = (pItem[EQUIPMENT_GLOVES].Level >> 3) & 15;
	Hero->BodyPart[BODYPART_BOOTS].Level = (pItem[EQUIPMENT_BOOTS].Level >> 3) & 15;

	CheckFullSet(Hero);
}

void SEASON3B::CNewUICharacterInfoWindow::OpenInputStatAdd(int index)
{
	m_inputIndex = index;

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CAddPointsReceiptMsgBoxLayout));
	//SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CAddPointsReceiptMsgBoxLayout));
}

int SEASON3B::CNewUICharacterInfoWindow::GetInputIndex()
{
	return m_inputIndex;
}

void SEASON3B::CNewUICharacterInfoWindow::ReceiveAdvanceChar(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CHARACTER_ADVANCE Data = (LPPRECEIVE_CHARACTER_ADVANCE)ReceiveBuffer;

	m_advanceChar.clear();

	//CharacterMachine->AtkPower = Data->m_PowerATK;
	//
	//CharacterMachine->KillCount = Data->m_KillCount;
	//
	//CharacterMachine->DeadCount = Data->m_DeadCount;
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[6], TRUE, TRUE, 0)); //"ATK Info"

	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[7], TRUE, Data->AttackSuccessRate));		   //"ATK Success Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[8], !TRUE, Data->m_ReflectDmg));			   //"Reflect Dmg Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[9], !TRUE, Data->m_ReflectDmgRate));		   //"Reflect Dmg FULL"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[10], !TRUE, Data->m_CriticalDmgRate));		   //"Critical Dmg Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[11], !TRUE, Data->m_ExcellentDmgRate));	   //"Excellent Dmg Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[12], !TRUE, Data->m_DoubleDmgRate));		   //"Double Dmg Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[13], !TRUE, Data->m_IncIgnoreShield));		   //"Ign DEF SD "
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[14], !TRUE, Data->m_IncIgnoreDefense));	   //"Ign Enemy's DEF"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[15], !TRUE, Data->m_ResIgnoreDefense));	   //"Ign Enemy's ATK"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[16], !TRUE, Data->m_TripleDmgRate));		   //"Triple Dmg Rate"

	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[17], TRUE, TRUE, 0));							//"- DEF Info"

	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[18], TRUE, Data->DefenseSuccessRate));			 //"DEF Success Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[19], !TRUE, Data->m_ResAttackStunRate));		 //"Resistance Stuck"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[20], !TRUE, Data->m_ResCriticalDmgRate));		 //"Critical Def Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[21], !TRUE, Data->m_ResExcellentDmgRate));		 //"Excellent Def Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[22], !TRUE, Data->m_ResDoubleDmgRate));		 //"Double Def Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[23], !TRUE, Data->m_DmgReductionRate));		 //"Reducction Dmg Rate"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[24], !TRUE, Data->m_ResIgnoreShield));			 //"Ign ATK SD"

	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[25], FALSE, Data->OffFullHPRestoreRate)); //"Recovery Offen HP"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[26], FALSE, Data->DefFullHPRestoreRate)); // "Recovery Defen HP"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[27], FALSE, Data->OffFullMPRestoreRate)); // "Recovery Offen MP"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[28], FALSE, Data->DefFullMPRestoreRate)); // "Recovery Defen MP"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[29], FALSE, Data->OffFullSDRestoreRate)); // "Recovery Offen SD"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[30], FALSE, Data->DefFullSDRestoreRate)); // "Recovery Defen SD"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[31], FALSE, Data->OffFullBPRestoreRate)); // "Recovery Offen BP"
	m_advanceChar.push_back(CHARACTER_ADVANCE_DESC(gTextClien.TextClien_ThongTin[32], FALSE, Data->DefFullBPRestoreRate)); // "Recovery Defen BP"
}

void SEASON3B::CHARACTER_ADVANCE_DESC::Render(int x, int y, int width, bool back)
{

	if (IsTitle)
	{
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetTextColor(0xFF, 0x61, 0, 0xFF);
		g_pRenderText->RenderFont(x, y, name.c_str(), width, 14, RT3_SORT_LEFT);
	}
	else
	{
		char strText[50];
		if (Value > 0)
		{
			if (Normal)
				sprintf_s(strText, "%d", Value);
			else
				sprintf_s(strText, "%d.00%%", Value);
			g_pRenderText->SetTextColor(CLRDW_GREEN);
		}
		else
		{
			sprintf_s(strText, "---");
			g_pRenderText->SetTextColor(CLRDW_GRAY);
		}

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->RenderFont(x, y, name.c_str(), width, 14, RT3_SORT_LEFT);

		g_pRenderText->SetTextColor(CLRDW_GOLD);
		g_pRenderText->RenderFont(x, y, strText, width, 14, RT3_SORT_RIGHT);
	}
}
