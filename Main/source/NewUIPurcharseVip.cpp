#include "stdafx.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "NewUIPurcharseVip.h"

SEASON3B::CNewUIPurcharseVip::CNewUIPurcharseVip()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
	targetIndex = 0;
}

SEASON3B::CNewUIPurcharseVip::~CNewUIPurcharseVip()
{
	Release();
}

bool SEASON3B::CNewUIPurcharseVip::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_SHOW_VIP, this);

		this->LoadImages();

		this->SetPos(x, y);

		this->SetInfo();

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIPurcharseVip::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();

		shopList.clear();
	}
}

void SEASON3B::CNewUIPurcharseVip::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CNewUIPurcharseVip::SetInfo()
{
	targetIndex = 0;
	shopList.resize(GMProtect->CustomBuyVip.size());  // Ajusta el tamaño del destino
	std::copy(GMProtect->CustomBuyVip.begin(), GMProtect->CustomBuyVip.end(), shopList.begin());
}

void SEASON3B::CNewUIPurcharseVip::LoadImages()
{
	LoadBitmap("Interface\\HUD\\top_back_3.tga", IMAGE_TOP_BACK3, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\membership_data.tga", IMAGE_MEMBERSHIP_DATA, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\button_arrow_L.tga", IMAGE_button_arrow_l, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\button_arrow_R.tga", IMAGE_button_arrow_r, GL_LINEAR);
}

void SEASON3B::CNewUIPurcharseVip::UnloadImages()
{
	DeleteBitmap(IMAGE_TOP_BACK3);
	DeleteBitmap(IMAGE_MEMBERSHIP_DATA);
	DeleteBitmap(IMAGE_button_arrow_l);
	DeleteBitmap(IMAGE_button_arrow_r);
}

bool SEASON3B::CNewUIPurcharseVip::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_SHOW_VIP);
			return false;
		}

		if (this->Checktarget())
		{
			if (SEASON3B::IsRelease(VK_LEFT))
			{
				if (targetIndex > 0)
				{
					targetIndex--;
				}
				return false;
			}
			if (SEASON3B::IsRelease(VK_RIGHT))
			{
				if (targetIndex < shopList.size() - 1)
				{
					targetIndex++;
				}
				return false;
			}
		}
	}
	return true;
}

bool SEASON3B::CNewUIPurcharseVip::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 214.f, 230.f))
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 192.f, RenderFrameY + 3.5f, 19.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_SHOW_VIP);
			return false;
		}

		if (targetIndex > 0)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 15, RenderFrameY + 50, 25, 40) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				targetIndex--;
				return false;
			}
		}
		if (targetIndex < shopList.size() - 1)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 174, RenderFrameY + 50, 25, 40) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				targetIndex++;
				return false;
			}
		}

		if (this->Checktarget())
		{
			TEMPLATE_MEMBERSHIP* Data = &shopList[targetIndex];

			if ((CharacterMachine->AccountLevel < Data->AccountLevel) && (Data->Coin[0] != 0 || Data->Coin[1] != 0 || Data->Coin[2] != 0))
			{
				if (SEASON3B::CheckMouseIn(RenderFrameX + 53.f, RenderFrameY + 185, 108, 25) && SEASON3B::IsRelease(VK_LBUTTON))
				{
					CreateOkCancelMessageBox(GlobalText[2897], -1, 3.f, ButtonOK);
					return false;
				}
			}
		}
	}
	return !SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, 214.f, 230.f);
}

bool SEASON3B::CNewUIPurcharseVip::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();

	if (this->Checktarget())
	{
		RenderButtons();

		RenderTexte();
	}

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIPurcharseVip::Update()
{
	return true;
}

float SEASON3B::CNewUIPurcharseVip::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUIPurcharseVip::OpenningProcess()
{
	targetIndex = 0;
}

void SEASON3B::CNewUIPurcharseVip::ClosingProcess()
{
	targetIndex = 0;
}

int SEASON3B::CNewUIPurcharseVip::GetLevelPurcharse()
{
	return shopList[targetIndex].Index;
}

bool SEASON3B::CNewUIPurcharseVip::Checktarget()
{
	return (targetIndex >= 0 && targetIndex < shopList.size());
}

void SEASON3B::CNewUIPurcharseVip::RenderFrame()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	SEASON3B::window_backmsg(RenderFrameX, RenderFrameY, 214.f, 230.f);

	if (this->Checktarget())
	{
		if (shopList[targetIndex].AccountLevel == 1)
		{
			SEASON3B::RenderImageF(IMAGE_MEMBERSHIP_DATA, RenderFrameX + 77.0, RenderFrameY + 45, 60.f, 50.f, 2.f, 2.f, 170.f, 149.f); //-- background
		}
		if (shopList[targetIndex].AccountLevel == 2)
		{
			SEASON3B::RenderImageF(IMAGE_MEMBERSHIP_DATA, RenderFrameX + 77.0, RenderFrameY + 45, 60.f, 50.f, 177.f, 2.f, 170.f, 149.f); //-- background
		}
		if (shopList[targetIndex].AccountLevel == 3)
		{
			SEASON3B::RenderImageF(IMAGE_MEMBERSHIP_DATA, RenderFrameX + 77.0, RenderFrameY + 45, 60.f, 50.f, 353.f, 2.f, 170.f, 149.f); //-- background
		}
	}

	//SEASON3B::RenderImageF(IMAGE_TOP_BACK3, RenderFrameX, RenderFrameY, 214.f, 230.f, 0.0, 0.0, 854.0, 917.0); //-- background
}

void SEASON3B::CNewUIPurcharseVip::RenderTexte()
{
	char psText[MAX_PATH];
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	TEMPLATE_MEMBERSHIP* Data = &shopList[targetIndex];

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0x0, 0x0, 0x0, 0x0);

	g_pRenderText->SetTextColor(CLRDW_GOLD);
	g_pRenderText->RenderFont(RenderFrameX + 107, RenderFrameY + 10, TextGlobal[26], 0, 0, RT3_WRITE_CENTER);

	g_pRenderText->SetTextColor(CLRDW_CYAN);
	g_pRenderText->RenderText(RenderFrameX + 107.f, RenderFrameY + 115, Data->text_name, 0, 0, RT3_WRITE_CENTER);

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(CLRDW_WHITE);

	sprintf_s(psText, TextGlobal[40], Data->CntExp);
	g_pRenderText->RenderText(RenderFrameX + 40.f, RenderFrameY + 130, psText, 0, 0, RT3_SORT_LEFT);

	sprintf_s(psText, TextGlobal[41], Data->CntDrop);
	g_pRenderText->RenderText(RenderFrameX + 40.f, RenderFrameY + 140, psText, 0, 0, RT3_SORT_LEFT);

	sprintf_s(psText, TextGlobal[42], Data->CntDays);
	g_pRenderText->RenderText(RenderFrameX + 40.f, RenderFrameY + 150, psText, 0, 0, RT3_SORT_LEFT);

	if ((CharacterMachine->AccountLevel < Data->AccountLevel) && (Data->Coin[0] != 0 || Data->Coin[1] != 0 || Data->Coin[2] != 0))
	{
		RenderFrameY += 185;
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetTextColor(0x0, 0x0, 0x0, 0xFF);

		if (Data->Coin[0] != 0 && Data->Coin[1] == 0 && Data->Coin[2] == 0)
		{
			sprintf_s(psText, "%d %s", Data->Coin[0], TextGlobal[58]);
			g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, psText, 108, 25, RT3_SORT_CENTER);
		}
		else if (Data->Coin[0] == 0 && Data->Coin[1] != 0 && Data->Coin[2] == 0)
		{
			sprintf_s(psText, "%d %s", Data->Coin[1], TextGlobal[59]);
			g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, psText, 108, 25, RT3_SORT_CENTER);
		}
		else if (Data->Coin[0] == 0 && Data->Coin[1] == 0 && Data->Coin[2] != 0)
		{
			sprintf_s(psText, "%d %s", Data->Coin[2], TextGlobal[60]);
			g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, psText, 108, 25, RT3_SORT_CENTER);
		}
		else
		{
			sprintf_s(psText, "[%d %s][%d %s][%d %s]", Data->Coin[0], TextGlobal[58], Data->Coin[1], TextGlobal[59], Data->Coin[2], TextGlobal[60]);

			g_pRenderText->SetTextColor(CLRDW_GOLD);
			g_pRenderText->RenderText(RenderFrameX + 107.f, RenderFrameY - 12, psText, 0, 0, RT3_WRITE_CENTER);

			g_pRenderText->SetTextColor(0x0, 0x0, 0x0, 0xFF);
			g_pRenderText->RenderText(RenderFrameX + 53.f, RenderFrameY, GlobalText[2293], 108, 25, RT3_SORT_CENTER);
		}
	}
}

void SEASON3B::CNewUIPurcharseVip::RenderButtons()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	TEMPLATE_MEMBERSHIP* Data = &shopList[targetIndex];

	if (targetIndex > 0)
	{
		RenderImage(IMAGE_button_arrow_l, RenderFrameX + 15, RenderFrameY + 50, 25, 40);
	}
	if (targetIndex < shopList.size() - 1)
	{
		RenderImage(IMAGE_button_arrow_r, RenderFrameX + 174, RenderFrameY + 50, 25, 40);
	}

	if ((CharacterMachine->AccountLevel < Data->AccountLevel) && (Data->Coin[0] != 0 || Data->Coin[1] != 0 || Data->Coin[2] != 0))
	{
		RenderButton(RenderFrameX + 53.f, RenderFrameY + 185, 108, 25);
		EndRenderColor();
	}
}

void SEASON3B::CNewUIPurcharseVip::RenderButton(float x, float y, float width, float height)
{
	if (SEASON3B::CheckMouseIn(x, y, width, height))
	{
		glColor4ub(0xfb, 0xdd, 0x1f, 0xFF);

		if (MouseLButton)
		{
			glColor4ub(0xcc, 0xb2, 0x0f, 0xFF);
		}
	}
	else
	{
		glColor4ub(0xe9, 0xcd, 0x1a, 0xFF);
	}

	RenderColor(x, y, width, height, 0.0, 0);

	glColor4ub(0xa8, 0x92, 0x08, 0xFF);
	RenderColor(x, y, width, 1, 0.0, 0);
	RenderColor(x, y, 1, height, 0.0, 0);
	RenderColor(x, y + height - 1, width, 1, 0.0, 0);
	RenderColor(x + width - 1, y, 1, height, 0.0, 0);
}

SEASON3B::CALLBACK_RESULT SEASON3B::CNewUIPurcharseVip::ButtonOK(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	int LevelPurcharse = g_pNewUILevelPurcharse->GetLevelPurcharse();

	SendRequestPurcharseMembership(LevelPurcharse);

	PlayBuffer(SOUND_CLICK01);

	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}