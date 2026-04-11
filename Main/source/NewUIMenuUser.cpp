#include "stdafx.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "DSPlaySound.h"
#include "CBInterface.h"
#include "CustomEventTime.h"
#include "NewUIMenuUser.h"

SEASON3B::CNewUIMenuUser::CNewUIMenuUser()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
}

SEASON3B::CNewUIMenuUser::~CNewUIMenuUser()
{
	Release();
}

bool SEASON3B::CNewUIMenuUser::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_CUSTOM_MENU, this);

		this->LoadImages();

		this->SetPos(x, y);

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIMenuUser::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();
	}
}

void SEASON3B::CNewUIMenuUser::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CNewUIMenuUser::LoadImages()
{
	LoadBitmap("Interface\\HUD\\top_back_3.tga", IMAGE_TOP_BACK3, GL_LINEAR);
}

void SEASON3B::CNewUIMenuUser::UnloadImages()
{
	DeleteBitmap(IMAGE_TOP_BACK3);
}

bool SEASON3B::CNewUIMenuUser::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUIMenuUser::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 214.f, 230.f))
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 192.f, RenderFrameY + 3.5f, 19.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
			return false;
		}
		RenderFrameY += 55;

		if (gmProtect->MenuButtonEventTime)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY, 108.0, 25.0) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				//if (gmProtect->WindowsEventTime)
				//{
				//	g_pNewUISystem->Show(INTERFACE_EVENT_TIME);
				//}
				g_CustomEventTime->OnOffWindow();
				g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
				PlayBuffer(SOUND_CLICK01);
				return false;
			}
			RenderFrameY += 30;
		}

		if (gmProtect->MenuButtonVipShop)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY, 108.0, 25.0) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				if (gmProtect->WindowsVipShop)
				{
					g_pNewUISystem->Show(INTERFACE_SHOW_VIP);
				}
				PlayBuffer(SOUND_CLICK01);
				return false;
			}
			RenderFrameY += 30;
		}

		if (gmProtect->MenuButtonRankTop)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY, 108.0, 25.0) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				if (gmProtect->WindowsRankTop)
				{
					g_pNewUISystem->Show(INTERFACE_RANKING_TOP);
				}
				PlayBuffer(SOUND_CLICK01);
				return false;
			}
			RenderFrameY += 30;
		}

		if (gmProtect->MenuButtonCommand)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY, 108.0, 25.0) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				if (gmProtect->WindowsCommand)
				{
					g_pNewUISystem->Show(INTERFACE_COMMAND_LIST);
				}
				PlayBuffer(SOUND_CLICK01);
				return false;
			}
			RenderFrameY += 30;
		}

		if (gmProtect->MenuButtonOptions)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY, 108.0, 25.0) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				g_pNewUISystem->Hide(INTERFACE_CUSTOM_MENU);
				g_pNewUISystem->Show(INTERFACE_OPTION);
				PlayBuffer(SOUND_CLICK01);
				return false;
			}
			RenderFrameY += 30;
		}
	}
	return !SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, 214.f, 230.f);
}

bool SEASON3B::CNewUIMenuUser::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();


	RenderButtons();

	RenderTexte();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIMenuUser::Update()
{
	return true;
}

float SEASON3B::CNewUIMenuUser::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUIMenuUser::OpenningProcess()
{
}

void SEASON3B::CNewUIMenuUser::ClosingProcess()
{
}

void SEASON3B::CNewUIMenuUser::RenderFrame()
{
	float x = m_Pos.x;
	float y = m_Pos.y;

	RenderImageF(IMAGE_TOP_BACK3, x, y, 214.f, 230.f, 0.0, 0.0, 854.0, 917.0); //-- background
}

void SEASON3B::CNewUIMenuUser::RenderTexte()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(CLRDW_WHITE);

	g_pRenderText->RenderFont(RenderFrameX + 12, RenderFrameY + 10, GlobalText[3450], 0, 0, RT3_SORT_LEFT);
	

	RenderFrameY += 55;

	if (gmProtect->MenuButtonEventTime)
	{
		g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, TextGlobal[25], 108, 25, RT3_SORT_CENTER);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonVipShop)
	{
		g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, TextGlobal[26], 108, 25, RT3_SORT_CENTER);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonRankTop)
	{
		g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, TextGlobal[27], 108, 25, RT3_SORT_CENTER);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonCommand)
	{
		g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, TextGlobal[28], 108, 25, RT3_SORT_CENTER);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonOptions)
	{
		g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, TextGlobal[44], 108, 25, RT3_SORT_CENTER);
		RenderFrameY += 30;
	}
}

void SEASON3B::CNewUIMenuUser::RenderButtons()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	RenderFrameY += 55;

	if (gmProtect->MenuButtonEventTime)
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY, 108, 25);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonVipShop)
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY, 108, 25);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonRankTop)
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY, 108, 25);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonCommand)
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY, 108, 25);
		RenderFrameY += 30;
	}

	if (gmProtect->MenuButtonOptions)
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY, 108, 25);
		RenderFrameY += 30;
	}
	EndRenderColor();
}

void SEASON3B::CNewUIMenuUser::RenderButton(float x, float y, float width, float height)
{
	if (SEASON3B::CheckMouseIn(x, y, width, height))
	{
		glColor4ub(0x3b, 0x42, 0x7c, 0xFF);

		if (MouseLButton)
		{
			glColor4ub(0x22, 0x27, 0x49, 0xFF);
		}
	}
	else
	{
		glColor4ub(0x2b, 0x30, 0x5f, 0xFF);
	}
	RenderColor(x, y, width, height, 0.0, 0);


	glColor4ub(0x40, 0x48, 0x8e, 0xFF);

	RenderColor(x, y, width, 1, 0.0, 0);
	RenderColor(x, y, 1, height, 0.0, 0);
	RenderColor(x, y + height - 1, width, 1, 0.0, 0);
	RenderColor(x + width - 1, y, 1, height, 0.0, 0);
}
