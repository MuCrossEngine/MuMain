#include "stdafx.h"
#include "NewUISystem.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "CGMResetManager.h"
#include "wsclientinline.h"

SEASON3B::CGMResetManager::CGMResetManager()
{
	_TypeUI = 0;
	_Visible = false;
	memset(&reset, 0, sizeof(RESET_MANAGER_INFO));
	memset(&master, 0, sizeof(MASTER_RESET_MANAGER_INFO));

	RenderSizeX = 250;
	RenderSizeY = 250;

	RenderFrameX = 0;
	RenderFrameY = 0;
}

SEASON3B::CGMResetManager::~CGMResetManager()
{
}

void SEASON3B::CGMResetManager::LoadImages()
{
	LoadBitmap("Interface\\HUD\\newui_btn_empty.tga", BITMAP_btn_empty, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\newui_btn_empty_big.tga", BITMAP_btn_empty_big, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\newui_btn_empty_small.tga", BITMAP_btn_empty_small, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\newui_btn_empty_very_small.tga", BITMAP_btn_empty_very_small, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\newui_item_jewel.jpg", IMAGE_BOX_SERVER_INTERFACE, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\timerserver-bar.tga", IMAGE_TITTLE_SERVER_INTERFACE, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\timerserver-bar4.tga", IMAGE_BTN_SERVER_INTERFACE, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\bg250.tga", IMAGE_BG_SERVER_INTERFACE_250, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\button_close_navi.tga", IMAGE_BUTTON_CLOSE_NAVI, GL_LINEAR);




	LoadBitmap("Interface\\HUD\\EDIT\\effect_back_mng.tga", IMAGE_EFFECT_BACK_MNG, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_edit.tga", IMAGE_EFFECT_BTN_EDIT, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_save.tga", IMAGE_EFFECT_BTN_SAVE, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_clear.tga", IMAGE_EFFECT_BTN_CLEAR, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_settings.tga", IMAGE_EFFECT_BTN_SETTING, GL_LINEAR);
}

void SEASON3B::CGMResetManager::UnloadImages()
{
	DeleteBitmap(BITMAP_btn_empty);
	DeleteBitmap(BITMAP_btn_empty_big);
	DeleteBitmap(BITMAP_btn_empty_small);
	DeleteBitmap(BITMAP_btn_empty_very_small);
	DeleteBitmap(IMAGE_BTN_SERVER_INTERFACE);
	DeleteBitmap(IMAGE_BG_SERVER_INTERFACE_250);
	DeleteBitmap(IMAGE_BOX_SERVER_INTERFACE);
	DeleteBitmap(IMAGE_TITTLE_SERVER_INTERFACE);
	DeleteBitmap(IMAGE_BUTTON_CLOSE_NAVI);

	DeleteBitmap(IMAGE_EFFECT_BACK_MNG);
	DeleteBitmap(IMAGE_EFFECT_BTN_EDIT);
	DeleteBitmap(IMAGE_EFFECT_BTN_SAVE);
	DeleteBitmap(IMAGE_EFFECT_BTN_CLEAR);
	DeleteBitmap(IMAGE_EFFECT_BTN_SETTING);
	
}

void SEASON3B::CGMResetManager::SetResetInfo(int _CurReset, int _MaxReset, int _MinLevel, int _RewardPoint, DWORD _ReqMoney, int* _ItemIndex, int* _ItemCount)
{
	reset.m_Enable = true;
	reset.m_Count = _CurReset;
	reset.m_MaxCount = _MaxReset;
	reset.m_MinLevel = _MinLevel;
	reset.m_ReqMoney = _ReqMoney;
	reset.m_RewardPoint = _RewardPoint;

	reset.box_count = 0;
	memset(reset.m_ItemIndex, 0, sizeof(reset.m_ItemIndex));
	memset(reset.m_ItemCount, 0, sizeof(reset.m_ItemCount));

	for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
	{
		if (_ItemIndex[i] > 0 && _ItemCount[i] > 0)
		{
			reset.m_ItemIndex[i] = _ItemIndex[i];
			reset.m_ItemCount[i] = _ItemCount[i];
			reset.box_count++;
		}
	}
}

void SEASON3B::CGMResetManager::SetMasterResetInfo(int _MasterReset, int _MaxMasterReset, int _MinLevel, int _MinReset, int _RewardPoint, DWORD _ReqMoney, int* _ItemIndex, int* _ItemCount)
{
	master.m_Enable = true;
	master.m_Count = _MasterReset;
	master.m_MaxCount = _MaxMasterReset;
	master.m_MinReset = _MinReset;
	master.m_MinLevel = _MinLevel;
	master.m_ReqMoney = _ReqMoney;
	master.m_RewardPoint = _RewardPoint;

	master.box_count = 0;
	memset(master.m_ItemIndex, 0, sizeof(master.m_ItemIndex));
	memset(master.m_ItemCount, 0, sizeof(master.m_ItemCount));

	for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
	{
		if (_ItemIndex[i] > 0 && _ItemCount[i] > 0)
		{
			master.m_ItemIndex[i] = _ItemIndex[i];
			master.m_ItemCount[i] = _ItemCount[i];
			master.box_count++;
		}
	}
}

void SEASON3B::CGMResetManager::Render()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (_Visible)
		{
			this->RenderFrame();
		}

		this->RenderButtons();
	}
}

bool SEASON3B::CGMResetManager::UpdateKeyEvent()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (_Visible && SEASON3B::IsPress(VK_ESCAPE))
		{
			this->ClosingProcess();
		}
	}
	return false;
}

bool SEASON3B::CGMResetManager::UpdateMouseEvent()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (ButtonProcess())
		{
			return true;
		}

		if (_Visible && !g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP) && !g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP) && !g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			)
		{
			if ((_TypeUI == 0 || _TypeUI == 1) && SEASON3B::CheckMouseIn(RenderFrameX + RenderSizeX - 20.0, RenderFrameY + 5.0, 16.0, 16.0))
			{
				if (SEASON3B::IsRelease(VK_LBUTTON))
				{
					this->ClosingProcess();
					return true;
				}
			}

			if (_TypeUI == 0)
			{
				if (SEASON3B::CheckMouseIn(RenderFrameX + ((RenderSizeX - 64.f) / 2.f), RenderFrameY + RenderSizeY - 50.f, 64.f, 29.f))
				{
					if (SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_pNewUICommandServer->SendCommand(11, "");
					}
				}
			}
			if (_TypeUI == 1)
			{
				if (SEASON3B::CheckMouseIn(RenderFrameX + ((RenderSizeX - 64.f) / 2.f), RenderFrameY + RenderSizeY - 50.f, 64.f, 29.f))
				{
					if (SEASON3B::IsRelease(VK_LBUTTON))
					{
						g_pNewUICommandServer->SendCommand(22, "");
					}
				}
			}

			return SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY);
		}
	}
	return false;
}

bool SEASON3B::CGMResetManager::ButtonProcess() //-- buttons open windows
{
	if (gmProtect->WindowsResetInfo)
	{
		float iRenderFrameX = GetWindowsX - 110;

		if (!(iRenderFrameX > GetScreenWidth()
			|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			|| g_pNewUISystem->IsVisible(INTERFACE_WINDOW_MENU)
			))
		{
			if (reset.m_Enable)
			{
				if (SEASON3B::CheckMouseIn(iRenderFrameX, GetWindowsY - 82, 105, 22))
				{
					if (SEASON3B::IsRelease(VK_LBUTTON))
					{
						_TypeUI = 0;
						_Visible = !_Visible;

						RenderSizeX = (reset.box_count * 22.f) + 30.f;

						if (RenderSizeX < 190)
						{
							RenderSizeX = 190;
						}

						RenderFrameX = PositionX_The_Mid(RenderSizeX);
						RenderFrameY = PositionY_The_Mid(RenderSizeY);
					}
					return true;
				}
				iRenderFrameX -= 100;
			}

			if (master.m_Enable)
			{
				if (SEASON3B::CheckMouseIn(iRenderFrameX, GetWindowsY - 82, 105, 22))
				{
					if (SEASON3B::IsRelease(VK_LBUTTON))
					{
						_TypeUI = 1;
						_Visible = !_Visible;


						RenderSizeX = (master.box_count * 22.f) + 30.f;

						if (RenderSizeX < 190)
						{
							RenderSizeX = 190;
						}

						RenderFrameX = PositionX_The_Mid(RenderSizeX);
						RenderFrameY = PositionY_The_Mid(RenderSizeY);
					}
					return true;
				}
			}
		}
	}

	return false;
}

void SEASON3B::CGMResetManager::Render3d()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (_Visible == false
			|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			)
		{
			return;
		}

		if (_TypeUI == 0)
		{
			float x = RenderFrameX + ((RenderSizeX - (reset.box_count * 22.f)) / 2.f);

			for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
			{
				if (reset.m_ItemIndex[i] != -1 && reset.m_ItemCount[i] > 0)
				{
					SEASON3B::Render2Item3D(x, RenderFrameY + 93.f, 22, 22, reset.m_ItemIndex[i], 0, 0, 0);
					x += 22.f;
				}
			}
		}
		if (_TypeUI == 1)
		{
			float x = RenderFrameX + ((RenderSizeX - (master.box_count * 22.f)) / 2.f);

			for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
			{
				if (master.m_ItemIndex[i] != -1 && master.m_ItemCount[i] > 0)
				{
					SEASON3B::Render2Item3D(x, RenderFrameY + 93.f, 22, 22, master.m_ItemIndex[i], 0, 0, 0);
					x += 22.f;
				}
			}
		}
	}
}

void SEASON3B::CGMResetManager::RenderItemNumber()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (_Visible == false
			|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			)
		{
			return;
		}

		glColor4f(1.f, 1.f, 1.f, 1.f);
		if (_TypeUI == 0)
		{
			float x = RenderFrameX + ((RenderSizeX - (reset.box_count * 22.f)) / 2.f) + 11.f;

			for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
			{
				if (reset.m_ItemIndex[i] != -1 && reset.m_ItemCount[i] > 0)
				{
					SEASON3B::RenderNumber(x, RenderFrameY + 90.f, reset.m_ItemCount[i], 0.9);
					x += 22.f;
				}
			}
		}

		if (_TypeUI == 1)
		{
			float x = RenderFrameX + ((RenderSizeX - (master.box_count * 22.f)) / 2.f) + 11.f;

			for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
			{
				if (master.m_ItemIndex[i] != -1 && master.m_ItemCount[i] > 0)
				{
					SEASON3B::RenderNumber(x, RenderFrameY + 90.f, master.m_ItemCount[i], 0.9);
					x += 22.f;
				}
			}
		}
	}
}

void SEASON3B::CGMResetManager::RenderFrame()
{
	if (gmProtect->WindowsResetInfo)
	{
		if (g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			)
		{
			_Visible = false;
			return;
		}

		if (_Visible == true)
		{
			SEASON3B::RenderImageF(IMAGE_BG_SERVER_INTERFACE_250, RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY, 0.0, 0.0, 250.0, 250.0);

			SEASON3B::RenderImageF(BITMAP_btn_empty_small, RenderFrameX + ((RenderSizeX - 64.f) / 2.f), RenderFrameY + RenderSizeY - 50.f, 64.f, 29.f, 0.0, 0.0, 64.0, 29.0);

			SEASON3B::RenderImageF(IMAGE_TITTLE_SERVER_INTERFACE, RenderFrameX + ((RenderSizeX - 105.f) / 2.f), RenderFrameY + 15.f, 105, 22, 0.0, 0.0, 223.0, 32.0);

			SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, RenderFrameX + RenderSizeX - 20.0, RenderFrameY + 5.0, 16.0, 16.0, 0.0, 0.0, 15.0, 15.0);

			if (_TypeUI == 0)
			{
				this->RenderResetInfo();
			}

			if (_TypeUI == 1)
			{
				this->RenderMasterResetInfo();
			}

			if (_TypeUI == 0 || _TypeUI == 1)
			{
				if (SEASON3B::CheckMouseIn(RenderFrameX + RenderSizeX - 20.0, RenderFrameY + 5.0, 16.0, 16.0))
				{
					glColor4f(0.5, 0.5, 0.5, 1.0);
					SEASON3B::RenderImageF(IMAGE_BUTTON_CLOSE_NAVI, RenderFrameX + RenderSizeX - 20.0, RenderFrameY + 5.0, 16.0, 16.0, 0.0, 0.0, 15.0, 15.0);
					glColor4f(1.0, 1.0, 1.0, 1.0);
				}
			}
		}
	}
}

void SEASON3B::CGMResetManager::RenderButtons()
{
	if (gmProtect->WindowsResetInfo)
	{
		float iRenderFrameX = GetWindowsX - 110;
		float iRenderFrameY = GetWindowsY - 82;

		if ((int)iRenderFrameX > GetScreenWidth()
			|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
			|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
			|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
			|| g_pNewUISystem->IsVisible(INTERFACE_WINDOW_MENU)
			)
		{
			return;
		}

		if (reset.m_Enable)
		{
			SEASON3B::RenderImageF(IMAGE_BTN_SERVER_INTERFACE, iRenderFrameX, iRenderFrameY, 105, 22, 0.0, 0.0, 223.0, 32.0);

			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetBgColor(0);
			g_pRenderText->SetTextColor(CLRDW_YELLOW);
			g_pRenderText->RenderText(iRenderFrameX, iRenderFrameY, "Reset", 105, 22, RT3_SORT_CENTER);

			iRenderFrameX -= 100;
		}

		if (master.m_Enable)
		{
			SEASON3B::RenderImageF(IMAGE_BTN_SERVER_INTERFACE, iRenderFrameX, iRenderFrameY, 105, 22, 0.0, 0.0, 223.0, 32.0);

			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetBgColor(0);
			g_pRenderText->SetTextColor(CLRDW_YELLOW);
			g_pRenderText->RenderText(iRenderFrameX, iRenderFrameY, "MasterReset", 105, 22, RT3_SORT_CENTER);
		}
	}
}

void SEASON3B::CGMResetManager::ClosingProcess()
{
	if (gmProtect->WindowsResetInfo)
	{
		_Visible = false;
	}
}

void SEASON3B::CGMResetManager::RenderResetInfo()
{
	float x = RenderFrameX + ((RenderSizeX - (reset.box_count * 22.f)) / 2.f);

	for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
	{
		if (reset.m_ItemIndex[i] != -1 && reset.m_ItemCount[i] > 0)
		{
			SEASON3B::RenderImageF(IMAGE_BOX_SERVER_INTERFACE, x, RenderFrameY + 95.f, 22, 22, 0.0, 0.0, 38.f, 38.f);
			x += 22.f;
		}
	}

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(CLRDW_GOLD);

	char pzTextTittle[100];
	sprintf_s(pzTextTittle, "%s: %d of %d", "Reset", reset.m_Count, reset.m_MaxCount);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 15, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	sprintf_s(pzTextTittle, GlobalText[2809]); //-- Requirements
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 40, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	if (CharacterAttribute->Level >= reset.m_MinLevel)
		g_pRenderText->SetTextColor(CLRDW_GREEN);
	else
		g_pRenderText->SetTextColor(CLRDW_GRAY);

	sprintf_s(pzTextTittle, "Level: %d of %d", CharacterAttribute->Level, master.m_MinLevel); //-- Min. Level
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 55, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	if (CharacterMachine->Gold >= reset.m_ReqMoney)
		g_pRenderText->SetTextColor(CLRDW_GREEN);
	else
		g_pRenderText->SetTextColor(CLRDW_GRAY);
	sprintf_s(pzTextTittle, GlobalText[2094], reset.m_ReqMoney); //-- Required zen : %d zen
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 70, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	int addY = 130;
	g_pRenderText->SetTextColor(CLRDW_GOLD);

	sprintf_s(pzTextTittle, GlobalText[2810]); //-- Reward
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);
	addY += 15;

	g_pRenderText->SetTextColor(CLRDW_GREEN);
	sprintf_s(pzTextTittle, "%s Points: %d", GlobalText[918], reset.m_RewardPoint);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);
	addY += 25;

	g_pRenderText->SetTextColor(CLRDW_CYAN);
	sprintf_s(pzTextTittle, GlobalText[2525]);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);


	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->RenderText(RenderFrameX + ((RenderSizeX - 64.f) / 2.f), RenderFrameY + RenderSizeY - 50.f, GlobalText[337], 64.f, 29.f, RT3_SORT_CENTER);
}

void SEASON3B::CGMResetManager::RenderMasterResetInfo()
{
	float x = RenderFrameX + ((RenderSizeX - (master.box_count * 22.f)) / 2.f);

	for (int i = 0; i < MAX_ITEM_JEWEL_KOF; i++)
	{
		if (master.m_ItemIndex[i] != -1 && master.m_ItemCount[i] > 0)
		{
			SEASON3B::RenderImageF(IMAGE_BOX_SERVER_INTERFACE, x, RenderFrameY + 95.f, 22, 22, 0.0, 0.0, 38.f, 38.f);
			x += 22.f;
		}
	}

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(CLRDW_GOLD);

	char pzTextTittle[100];
	sprintf_s(pzTextTittle, "%s: %d of %d", "Master Reset", master.m_Count, master.m_MaxCount);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 15, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	sprintf_s(pzTextTittle, GlobalText[2809]); //-- Requirements
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 40, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	if (reset.m_Count >= master.m_MinReset)
		g_pRenderText->SetTextColor(CLRDW_GREEN);
	else
		g_pRenderText->SetTextColor(CLRDW_GRAY);

	sprintf_s(pzTextTittle, "Reset: %d of %d", reset.m_Count, master.m_MinReset); //-- Min. Level
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 51, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	if (CharacterAttribute->Level >= master.m_MinLevel)
		g_pRenderText->SetTextColor(CLRDW_GREEN);
	else
		g_pRenderText->SetTextColor(CLRDW_GRAY);

	sprintf_s(pzTextTittle, "Level: %d of %d", CharacterAttribute->Level, master.m_MinLevel); //-- Min. Level
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 62, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	if (CharacterMachine->Gold >= master.m_ReqMoney)
		g_pRenderText->SetTextColor(CLRDW_GREEN);
	else
		g_pRenderText->SetTextColor(CLRDW_GRAY);
	sprintf_s(pzTextTittle, GlobalText[2094], master.m_ReqMoney); //-- Required zen : %d zen
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + 73, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	int addY = 130;
	g_pRenderText->SetTextColor(CLRDW_GOLD);

	sprintf_s(pzTextTittle, GlobalText[2810]); //-- Reward
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);
	addY += 15;

	g_pRenderText->SetTextColor(CLRDW_GREEN);
	sprintf_s(pzTextTittle, "%s Points: %d", GlobalText[918], master.m_RewardPoint);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);
	addY += 25;

	g_pRenderText->SetTextColor(CLRDW_CYAN);
	sprintf_s(pzTextTittle, GlobalText[2525]);
	g_pRenderText->RenderText(RenderFrameX + (RenderSizeX / 2.f), RenderFrameY + addY, pzTextTittle, 0, 22, RT3_WRITE_CENTER);

	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->RenderText(RenderFrameX + ((RenderSizeX - 64.f) / 2.f), RenderFrameY + RenderSizeY - 50.f, GlobalText[337], 64.f, 29.f, RT3_SORT_CENTER);
}
