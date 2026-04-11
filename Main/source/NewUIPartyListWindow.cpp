// NewUIPartyListWindow.cpp: implementation of the CNewUIPartyInfo class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIPartyListWindow.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "ZzzInventory.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "CGMHeadChat.h"

using namespace SEASON3B;

CNewUIPartyListWindow::CNewUIPartyListWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = m_Pos.y = 0;
	m_bActive = false;
#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
		m_iVal = 24;
	else
		m_iVal = 19;
#else
	m_iVal = 19;
#endif
	m_iLimitUserIDHeight[0] = 48;
	m_iLimitUserIDHeight[1] = 58;
	m_iSelectedCharacter = -1;
	m_ibEnable = true;
	m_bEnableMove = true;
	m_bMoveWindow = false;
	m_iMouseClickPos_x = 0;
	m_iMouseClickPos_y = 0;
}

CNewUIPartyListWindow::~CNewUIPartyListWindow()
{
	Release();
}

bool CNewUIPartyListWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_PARTY_INFO_WINDOW, this);

	SetPos(x, y);

	LoadImages();

#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		for (int i = 0; i < MAX_PARTYS; i++)
		{
			int iVal = i * m_iVal;
			m_BtnPartyExit[i].ChangeButtonImgState(true, IMAGE_PARTY_LIST_EXIT);
			m_BtnPartyExit[i].ChangeButtonInfo(m_Pos.x + 63, m_Pos.y + 3 + iVal, 11, 11);
		}
	}
#endif
	Show(true);

	return true;
}

void CNewUIPartyListWindow::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void CNewUIPartyListWindow::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;

#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		for (int i = 0; i < MAX_PARTYS; i++)
		{
			int iVal = i * m_iVal;
			m_BtnPartyExit[i].SetPos(m_Pos.x + 63, m_Pos.y + 3 + iVal);
		}
	}
#endif
}

void CNewUIPartyListWindow::SetPos(int x)
{
	if (!gmProtect->checkold_school())
		SetPos(x - (PARTY_LIST_WINDOW_WIDTH + 2), m_Pos.y);
	else
		SetPos(x - (56), m_Pos.y);
}

int SEASON3B::CNewUIPartyListWindow::GetPosX()
{
	return m_Pos.x;
}

int CNewUIPartyListWindow::GetSelectedCharacter()
{
	if (m_iSelectedCharacter == -1)
		return -1;

	return Party[m_iSelectedCharacter].index;
}

int SEASON3B::CNewUIPartyListWindow::GetRightBottomPos(POINT* pos)
{
#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		(*pos).x = m_Pos.x + 0 + (PARTY_LIST_WINDOW_WIDTH + 2);
		(*pos).y = m_Pos.y + 0 + PartyNumber * m_iVal + 2;
	}
	else
	{
		(*pos).x = m_Pos.x + 0 + (52);
		(*pos).y = m_Pos.y + 0 + PartyNumber * m_iVal;
	}
#else
	(*pos).x = m_Pos.x + 0 + (PARTY_LIST_WINDOW_WIDTH + 2);
	(*pos).y = m_Pos.y + 0 + PartyNumber * m_iVal;
#endif
	return 0;
}

bool CNewUIPartyListWindow::BtnProcess()
{
#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		m_iSelectedCharacter = -1;

		if (PartyNumber > 1)
		{
			if (SEASON3B::CheckMouseIn(m_Pos.x - 14, m_Pos.y, 12, 12))
			{
				if (SEASON3B::IsPress(VK_LBUTTON))
					m_ibEnable = !m_ibEnable;
				return true;
			}
		}
	}

	for (int i = 0; i < PartyNumber; i++)
	{
		int iVal = i * m_iVal;

		if (m_ibEnable == false && i > 0)
			continue;

		if (!strcmp(Party[0].Name, Hero->ID) || !strcmp(Party[i].Name, Hero->ID))
		{
			if (m_BtnPartyExit[i].UpdateMouseEvent())
			{
				g_pPartyInfoWindow->LeaveParty(i);
				return true;
			}
		}

		if (!gmProtect->checkold_school())
		{
			if (CheckMouseIn(m_Pos.x, m_Pos.y + iVal, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT))
			{
				m_iSelectedCharacter = i;

				if (SelectedCharacter == -1)
				{
					CHARACTER* c = gmCharacters->GetCharacter(Party[i].index);
					if (c && c != Hero)
					{
						gmHeadChat->CreateChat(c->ID, "", c);
					}
				}

				if (SelectCharacterInPartyList(&Party[i]))
				{
					return true;
				}
			}
		}
		else
		{
			if (CheckMouseIn(m_Pos.x, m_Pos.y + iVal, 52, 19))
			{
				m_iSelectedCharacter = i;

				if (SelectedCharacter == -1)
				{
					CHARACTER* c = gmCharacters->GetCharacter(Party[i].index);
					if (c && c != Hero)
					{
						gmHeadChat->CreateChat(c->ID, "", c);
					}
				}

				if (SelectCharacterInPartyList(&Party[i]))
				{
					return true;
				}
			}
		}
	}
#else
	m_iSelectedCharacter = -1;

	if (PartyNumber > 1)
	{
		if (runtime_mouse_target(m_Pos.x, m_Pos.y))
		{
			return true;
		}
	}
#endif

	return false;
}

bool CNewUIPartyListWindow::UpdateMouseEvent()
{
#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		if (!m_bActive)
			return true;

		if (true == BtnProcess())
			return false;

		int partycount = PartyNumber;

		if (partycount > 0)
		{
			if (partycount > 1 && m_ibEnable == false)
			{
				partycount = 1;
			}

			if (MoveWindows())
			{
				return false;
			}

			int iHeight = (PARTY_LIST_WINDOW_HEIGHT * partycount) + (4 * (partycount - 1));
			if (CheckMouseIn(m_Pos.x, m_Pos.y, PARTY_LIST_WINDOW_WIDTH, iHeight))
			{
				return false;
			}
		}
	}
	else
	{
		if (true == BtnProcess())
			return false;
	}
#else
	if (true == BtnProcess())
		return false;
#endif

	return true;
}

bool CNewUIPartyListWindow::UpdateKeyEvent()
{
	return true;
}

bool CNewUIPartyListWindow::Update()
{
	if (PartyNumber <= 0)
	{
		m_bActive = false;
		return true;
	}

	m_bActive = true;

	for (int i = 0; i < PartyNumber; i++)
	{
		Party[i].index = -2;
	}

	return true;
}

#if MAIN_UPDATE > 303
bool CNewUIPartyListWindow::Render()
{
	if (!m_bActive)
		return true;

	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	if (!gmProtect->checkold_school())
	{
		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->SetBgColor(0, 0, 0, 0);

		if (PartyNumber > 1)
		{
			float x = m_Pos.x - 8.f;
			if (m_ibEnable == false)
				RenderBitmapLocalRotate2(IMAGE_ND_BTN_L, x, m_Pos.y + 6, 12.f, 12.f, 90.f, 0.0, 0.0, 17.f / 32.f, 18.f / 64.f);
			else
				RenderBitmapLocalRotate2(IMAGE_ND_BTN_R, x, m_Pos.y + 6, 12.f, 12.f, 90.f, 0.0, 0.0, 17.f / 32.f, 18.f / 64.f);
		}

		for (int i = 0; i < PartyNumber; i++)
		{
			int iVal = i * m_iVal;

			if (m_ibEnable == false && i > 0)
				continue;

			glColor4f(0.f, 0.f, 0.f, 0.9f);
			RenderColor(float(m_Pos.x + 2), float(m_Pos.y + 2 + iVal), PARTY_LIST_WINDOW_WIDTH - 3, PARTY_LIST_WINDOW_HEIGHT - 6);
			EnableAlphaTest();

			if (Party[i].index == -1)
			{
				glColor4f(0.3f, 0.f, 0.f, 0.5f);
				RenderColor(m_Pos.x + 2, m_Pos.y + 2 + iVal, PARTY_LIST_WINDOW_WIDTH - 3, PARTY_LIST_WINDOW_HEIGHT - 6);
				EnableAlphaTest();
			}
			else
			{
				CHARACTER* pChar = gmCharacters->GetCharacter(Party[i].index);

				if (pChar)
				{
					OBJECT* pObj = &pChar->Object;

					if (g_isCharacterBuff(pObj, eBuff_Defense) == true)
					{
						glColor4f(0.2f, 1.f, 0.2f, 0.2f);
						RenderColor(m_Pos.x + 2, m_Pos.y + 2 + iVal, PARTY_LIST_WINDOW_WIDTH - 3, PARTY_LIST_WINDOW_HEIGHT - 6);
						EnableAlphaTest();
					}
				}
				if (m_iSelectedCharacter != -1 && m_iSelectedCharacter == i)
				{
					glColor4f(0.4f, 0.4f, 0.4f, 0.7f);
					RenderColor(m_Pos.x + 2, m_Pos.y + 2 + iVal, PARTY_LIST_WINDOW_WIDTH - 3, PARTY_LIST_WINDOW_HEIGHT - 6);
					EnableAlphaTest();
				}
			}

			EndRenderColor();
			RenderImage(IMAGE_PARTY_LIST_BACK, m_Pos.x, m_Pos.y + iVal, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT);

			if (i == 0)
			{
				if (Party[i].index == -1)
				{
					g_pRenderText->SetTextColor(RGBA(128, 75, 11, 255));
				}
				else
				{
					g_pRenderText->SetTextColor(RGBA(255, 148, 22, 255));
				}

				RenderImage(IMAGE_PARTY_LIST_FLAG, m_Pos.x + 53, m_Pos.y + 3, 9, 10);
				g_pRenderText->RenderText(m_Pos.x + 4, m_Pos.y + 4 + iVal, Party[i].Name, m_iLimitUserIDHeight[0], 0, RT3_SORT_LEFT);
			}
			else
			{
				if (Party[i].index == -1)
				{
					g_pRenderText->SetTextColor(RGBA(128, 128, 128, 255));
				}
				else
				{
					g_pRenderText->SetTextColor(RGBA(255, 255, 255, 255));
				}
				g_pRenderText->RenderText(m_Pos.x + 4, m_Pos.y + 4 + iVal, Party[i].Name, m_iLimitUserIDHeight[1], 0, RT3_SORT_LEFT);
			}

			int iStepHP = min(10, Party[i].stepHP);
			float fLife = ((float)iStepHP / (float)10) * (float)PARTY_LIST_HP_BAR_WIDTH;
			RenderImage(IMAGE_PARTY_LIST_HPBAR, m_Pos.x + 4, m_Pos.y + 16 + iVal, fLife, 3);

			if (!strcmp(Party[0].Name, Hero->ID) || !strcmp(Party[i].Name, Hero->ID))
			{
				m_BtnPartyExit[i].Render();
			}
		}
	}
	else
	{
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);

		if (PartyNumber >= 1)
		{
			float iRenderFrameY = m_Pos.y;
			float RenderFrameY = m_Pos.y;

			for (int i = 0; i < PartyNumber; i++)
			{
				if (Party[i].index != -1)
				{
					if (SEASON3B::CheckMouseIn(m_Pos.x, RenderFrameY, 52, 19) == false)
					{
						glColor4f(0.1, 0.1, 0.1, 0.69999999);
						RenderColor(m_Pos.x, RenderFrameY, 52, 19);
					}
					else
					{
						glColor4f(0.5, 0.5, 0.5, 0.80000001);
						RenderColor(m_Pos.x, RenderFrameY, 52, 19);
					}

					CHARACTER* pChar = gmCharacters->GetCharacter(Party[i].index);

					if (pChar)
					{
						OBJECT* pObj = &pChar->Object;

						if (g_isCharacterBuff(pObj, eBuff_Defense) == true)
						{
							glColor4f(0.2f, 1.f, 0.2f, 0.2f);
							RenderColor(m_Pos.x, RenderFrameY, 52, 19);
						}
					}
					g_pRenderText->SetTextColor(RGBA(255, 255, 255, 255));
				}
				else
				{
					glColor4f(0.3f, 0.f, 0.f, 0.5f);
					RenderColor(m_Pos.x, RenderFrameY, 52, 19);

					g_pRenderText->SetTextColor(RGBA(128, 128, 128, 255));
				}

				glColor4f(0.19607843, 0.039215688, 0.0, 1.0);
				RenderColor(m_Pos.x + 2.f, RenderFrameY + 12.f, 43.0, 4.0);

				int iStepHP = min(10, Party[i].stepHP);

				glColor4f(0.98039216, 0.039215688, 0.0, 1.0);

				if (iStepHP > 0)
				{
					float v130 = m_Pos.x + 4.f;

					do
					{
						RenderColor(v130, RenderFrameY + 13.f, 3.0, 2.0);
						--iStepHP;
						v130 += 4;
					} while (iStepHP);
				}

				EndRenderColor();
				EnableAlphaTest();

				g_pRenderText->SetFont(g_hFont);
				g_pRenderText->RenderText(m_Pos.x + 3.f, RenderFrameY + 2.f, Party[i].Name, 0, 0, RT3_SORT_LEFT);

				RenderFrameY += m_iVal;
			}

			EnableAlphaTest();

			glColor4f(1.f, 1.f, 1.f, 1.f);

			RenderBitmap(Bitmap_Message_box2, m_Pos.x - 2.0, iRenderFrameY - 2.f, 52 + 4.f, 3.0, 0, 0, 0.83203125, 0.625, 1, 1, 0.0);

			RenderBitmap(Bitmap_Message_box2, m_Pos.x - 2.0, RenderFrameY, 52 + 4.f, 3.0, 0, 0, 0.83203125, 0.625, 1, 1, 0.0);

			glColor4f(0.72156864, 0.59215689, 0.38039216, 0.5);

			RenderColor(m_Pos.x - 1.0, iRenderFrameY, 1.0, (float)(m_iVal * PartyNumber));

			RenderColor(m_Pos.x + 52.0, iRenderFrameY, 1.0, (float)(m_iVal * PartyNumber));

			EndRenderColor();
		}
	}

	DisableAlphaBlend();

	return true;
}
#else
bool CNewUIPartyListWindow::Render()
{
	if (m_bActive)
	{
		this->RenderFrame((float)m_Pos.x, (float)m_Pos.y);
		DisableAlphaBlend();
	}

	return true;
}
#endif

bool SEASON3B::CNewUIPartyListWindow::MoveWindows()
{
	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT))
	{
		if (MouseLButton == true && m_bMoveWindow == false)
		{
			m_bMoveWindow = true;
			m_iMouseClickPos_x = MouseX;
			m_iMouseClickPos_y = MouseY;
		}
	}

	if (m_bMoveWindow == true)
	{
		if (MouseLButton == true)
		{
			int iPosX = m_Pos.x;
			int iPosY = m_Pos.y;

			if (iPosX + MouseX - m_iMouseClickPos_x < 0)
				iPosX = 0;
			else if (iPosX + PARTY_LIST_WINDOW_WIDTH + MouseX - m_iMouseClickPos_x > gwinhandle->GetScreenX())
				iPosX = gwinhandle->GetScreenX() - PARTY_LIST_WINDOW_WIDTH;
			else
				iPosX += MouseX - m_iMouseClickPos_x;

			if (iPosY + MouseY - m_iMouseClickPos_y < 0)
			{
				iPosY = 0;
			}
			else if (iPosY + PARTY_LIST_WINDOW_HEIGHT + MouseY - m_iMouseClickPos_y > gwinhandle->GetScreenY())
			{
				iPosY = gwinhandle->GetScreenY() - PARTY_LIST_WINDOW_HEIGHT;
			}
			else
			{
				iPosY += MouseY - m_iMouseClickPos_y;
			}
			if (iPosX != m_Pos.x || iPosY != m_Pos.y)
			{
				m_bEnableMove = false;
				SetPos(iPosX, iPosY);
			}

			m_iMouseClickPos_x = MouseX;
			m_iMouseClickPos_y = MouseY;
			return true;
		}
		else
		{
			m_bMoveWindow = false;
		}
	}
	return false;

}

float CNewUIPartyListWindow::GetLayerDepth()
{
	return 3.4f;
}

void SEASON3B::CNewUIPartyListWindow::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);
	float RenderFrameY = (m_Pos.y);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, (int)RenderFrameY);
}

void CNewUIPartyListWindow::OpenningProcess()
{
}

void CNewUIPartyListWindow::ClosingProcess()
{
}

bool CNewUIPartyListWindow::SelectCharacterInPartyList(PARTY_t* pMember)
{
	int HeroClass = Hero->GetBaseClass();

	bool result;
	int Skill = CharacterAttribute->Skill[Hero->CurrentSkill];

	if (HeroClass != Fairy_Elf && HeroClass && HeroClass != Summoner
		|| (Skill != Skill_Heal
			&& Skill != Skill_Greater_Defense
			&& Skill != Skill_Defense_Increase_Strengthener
			&& Skill != Skill_Attack_Increase_Strengthener
			&& Skill != Skill_Greater_Damage
			&& Skill != Skill_Teleport_Ally
			&& Skill != Skill_Soul_Barrier
			&& Skill != Skill_Heal_Strengthener
			&& Skill != Skill_Soul_Barrier_Strengthener
			&& Skill != Skill_Soul_Barrier_Proficiency
			&& Skill != Skill_Soul_Barrier_Mastery
			&& Skill != Skill_Attack_Increase_Mastery
			&& Skill != Skill_Defense_Increase_Mastery
			&& Skill != Skill_Damage_Reflection
			&& Skill != Skill_Recovery))
	{
		result = false;
	}
	else
	{
		SelectedCharacter = pMember->index;

		if (SelectedCharacter == -2)
		{
			SelectedCharacter = -1;
		}
		result = true;
	}
	return result;
}

#if MAIN_UPDATE <= 303
void SEASON3B::CNewUIPartyListWindow::RenderFrame(float RenderFrameX, float RenderFrameY)
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	if (PartyNumber >= 1)
	{
		float iRenderFrameY = RenderFrameY;

		for (int i = 0; i < PartyNumber; i++)
		{
			if (Party[i].index != -1)
			{
				if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT) == false)
				{
					glColor4f(0.1, 0.1, 0.1, 0.69999999);
					RenderColor(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT);
				}
				else
				{
					glColor4f(0.5, 0.5, 0.5, 0.80000001);
					RenderColor(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT);
				}

				CHARACTER* pChar = gmCharacters->GetCharacter(Party[i].index);

				if (pChar)
				{
					OBJECT* pObj = &pChar->Object;

					if (g_isCharacterBuff(pObj, eBuff_Defense) == true)
					{
						glColor4f(0.2f, 1.f, 0.2f, 0.2f);
						RenderColor(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT);
					}
				}
				g_pRenderText->SetTextColor(RGBA(255, 255, 255, 255));
			}
			else
			{
				glColor4f(0.3f, 0.f, 0.f, 0.5f);
				RenderColor(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT);

				g_pRenderText->SetTextColor(RGBA(128, 128, 128, 255));
			}

			glColor4f(0.19607843, 0.039215688, 0.0, 1.0);
			RenderColor(RenderFrameX + 2.f, RenderFrameY + 12.f, 43.0, 4.0);

			int iStepHP = min(10, Party[i].stepHP);

			glColor4f(0.98039216, 0.039215688, 0.0, 1.0);

			if (iStepHP > 0)
			{
				float v130 = RenderFrameX + 4.f;

				do
				{
					RenderColor(v130, RenderFrameY + 13.f, 3.0, 2.0);
					--iStepHP;
					v130 += 4;
				} while (iStepHP);
			}

			EndRenderColor();
			EnableAlphaTest();

			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->RenderText(RenderFrameX + 3.f, RenderFrameY + 2.f, Party[i].Name, 0, 0, RT3_SORT_LEFT);

			RenderFrameY += m_iVal;
		}

		//EnableAlphaTest();
		//
		//glColor4f(1.f, 1.f, 1.f, 1.f);
		//
		//RenderBitmap(Bitmap_Message_box2, RenderFrameX - 2.0, iRenderFrameY - 2.f, PARTY_LIST_WINDOW_WIDTH + 4.f, 3.0, 0, 0, 0.83203125, 0.625, 1, 1, 0.0);
		//
		//RenderBitmap(Bitmap_Message_box2, RenderFrameX - 2.0, RenderFrameY, PARTY_LIST_WINDOW_WIDTH + 4.f, 3.0, 0, 0, 0.83203125, 0.625, 1, 1, 0.0);
		//
		//glColor4f(0.72156864, 0.59215689, 0.38039216, 0.5);
		//
		//RenderColor(RenderFrameX - 1.0, iRenderFrameY, 1.0, (float)(m_iVal * PartyNumber));
		//
		//RenderColor(RenderFrameX + 50.0, iRenderFrameY, 1.0, (float)(m_iVal * PartyNumber));
		//
		//EndRenderColor();
	}
}

bool SEASON3B::CNewUIPartyListWindow::runtime_mouse_target(float RenderFrameX, float RenderFrameY)
{
	m_iSelectedCharacter = -1;

	if (CheckMouseIn(RenderFrameX, RenderFrameY, PARTY_LIST_WINDOW_WIDTH, (PARTY_LIST_WINDOW_HEIGHT * PartyNumber)))
	{
		for (int i = 0; i < PartyNumber; i++)
		{
			if (CheckMouseIn(RenderFrameX, RenderFrameY + (i * m_iVal), PARTY_LIST_WINDOW_WIDTH, PARTY_LIST_WINDOW_HEIGHT))
			{
				m_iSelectedCharacter = i;

				if (SelectedCharacter == -1)
				{
					CHARACTER* c = gmCharacters->GetCharacter(Party[i].index);

					if (c && c != Hero)
					{
						gmHeadChat->CreateChat(c->ID, "", c);
					}
				}

				if (SelectCharacterInPartyList(&Party[i]))
				{
					return true;
				}
			}
		}
		return true;
	}

	return false;
}
#endif

void CNewUIPartyListWindow::LoadImages()
{
	LoadBitmap("Interface\\newui_party_x.tga", IMAGE_PARTY_LIST_EXIT, GL_LINEAR);
#if MAIN_UPDATE > 303
	LoadBitmap("Interface\\newui_party_flag.tga", IMAGE_PARTY_LIST_FLAG, GL_LINEAR);
	LoadBitmap("Interface\\newui_party_back.tga", IMAGE_PARTY_LIST_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_party_hpbar.jpg", IMAGE_PARTY_LIST_HPBAR, GL_LINEAR);
#endif
}

void CNewUIPartyListWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_PARTY_LIST_EXIT);
#if MAIN_UPDATE > 303
	DeleteBitmap(IMAGE_PARTY_LIST_FLAG);
	DeleteBitmap(IMAGE_PARTY_LIST_BACK);
	DeleteBitmap(IMAGE_PARTY_LIST_HPBAR);
#endif
}
