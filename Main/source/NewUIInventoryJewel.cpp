#include "stdafx.h"
#include "WSclient.h"
#include "UIControls.h"
#include "ZzzInventory.h"
#include "wsclientinline.h"
#include "NewUIInventoryJewel.h"
#include "TextClien.h"

#define INVENTORY_JEWEL_Y			160
#define MAX_JEWEL_INVENTORY			6

SEASON3B::CNewUIInventoryJewel::CNewUIInventoryJewel()
{
	Initialize();
}

SEASON3B::CNewUIInventoryJewel::~CNewUIInventoryJewel()
{
	Release();
}

bool SEASON3B::CNewUIInventoryJewel::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_INVENTORY_JEWEL, this);

		SetPos(x, y);

		LoadBitmap("Interface\\InGameShop\\IGS_Storage_Page.tga", CNewUIInGameShop::IMAGE_IGS_STORAGE_PAGE, GL_LINEAR);

		InitButtons();

		return true;
	}
	return false;
}

void SEASON3B::CNewUIInventoryJewel::Initialize()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;

	m_dwCurIndex = -1;
	m_dwSelIndex = -1;
	m_nSelPage = 0;
	m_nMaxPage = 0;
	m_nOptionCurrent = 0;

	m_bItems.push_back({0, 6159, -1, 250000});
	m_bItems.push_back({1, 7181, -1, 250000});
	m_bItems.push_back({2, 7182, -1, 250000});
	m_bItems.push_back({3, 7184, -1, 250000});
	m_bItems.push_back({4, 7190, -1, 250000});
	m_bItems.push_back({5, 7199, -1, 250000});
	m_bItems.push_back({6, 7210, -1, 250000});
}

void SEASON3B::CNewUIInventoryJewel::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
	m_bItems.clear();
}

void SEASON3B::CNewUIInventoryJewel::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
	if (gmProtect->checkold_school())
	{
		m_Button1.SetPos(m_Pos.x + 25.00, m_Pos.y + 395.0);
		m_Button2.SetPos(m_Pos.x + 109.0, m_Pos.y + 395.0);
	}
	else
	{
		m_Button1.SetPos(m_Pos.x + 25.00, m_Pos.y + 390.0);
		m_Button2.SetPos(m_Pos.x + 112.0, m_Pos.y + 390.0);
	}

	m_ButtonBack.SetPos(m_Pos.x + 40.00, m_Pos.y + 360.0);
	m_ButtonNext.SetPos(m_Pos.x + 133.0, m_Pos.y + 360.0);

	m_nRectItem.top = m_Pos.y + 60;
	m_nRectItem.left = m_Pos.x + 70;
}

void SEASON3B::CNewUIInventoryJewel::InitButtons()
{
	m_Button1.ChangeText(GlobalText[235]);
	m_Button2.ChangeText(GlobalText[236]);

	if (gmProtect->checkold_school())
	{
		SetButtonImagen(&m_Button1, Bitmap_text_button1, Bitmap_text_button2);
		SetButtonImagen(&m_Button2, Bitmap_text_button1, Bitmap_text_button2);
		m_Button1.ChangeButtonInfo(m_Pos.x + 25.00, m_Pos.y + 395.0, 56.0, 24.0);
		m_Button2.ChangeButtonInfo(m_Pos.x + 109.0, m_Pos.y + 395.0, 56.0, 24.0);
	}
	else
	{
		m_Button1.ChangeButtonImgState(true, CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_VERY_SMALL, true);
		m_Button2.ChangeButtonImgState(true, CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_VERY_SMALL, true);

		m_Button1.ChangeButtonInfo(m_Pos.x + 25.00, m_Pos.y + 390.0, 53.0, 23.0);
		m_Button2.ChangeButtonInfo(m_Pos.x + 112.0, m_Pos.y + 390.0, 53.0, 23.0);
	}

	m_ButtonBack.ChangeButtonImgState(true, CNewUIQuestProgress::IMAGE_QP_BTN_L);
	m_ButtonNext.ChangeButtonImgState(true, CNewUIQuestProgress::IMAGE_QP_BTN_R);

	m_ButtonBack.ChangeButtonInfo(m_Pos.x + 40.00, m_Pos.y + 360.0, 17, 18);
	m_ButtonNext.ChangeButtonInfo(m_Pos.x + 133.0, m_Pos.y + 360.0, 17, 18);
}

bool SEASON3B::CNewUIInventoryJewel::Render()
{
	EnableAlphaTest();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderFrame();

	RenderInter();

	RenderTexts();

	RenderButtons();

	RenderHoly();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIInventoryJewel::Update()
{
	return true;
}

bool SEASON3B::CNewUIInventoryJewel::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x + 25.f;
	float RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y;

	m_dwCurIndex = -1;

	for (size_t i = 0; i < 4; i++)
	{
		if (SEASON3B::IsRelease(VK_LBUTTON) && SEASON3B::CheckMouseIn(RenderFrameX + (i * 40.0) + 5.0, RenderFrameY - 25.0, 12.f, 12.f))
		{
			m_nOptionCurrent = i;
			return false;
		}
	}

	size_t index = m_nSelPage * MAX_JEWEL_INVENTORY;

	for (size_t i = index; i < m_bItems.size() && i < index + MAX_JEWEL_INVENTORY; i++)
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 140.f, 30.f))
		{
			m_dwCurIndex = i;
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				m_dwSelIndex = i;
				return false;
			}
		}
		RenderFrameY += 32.f;
	}

	if (m_ButtonNext.UpdateMouseEvent())
	{
		if (m_nSelPage < m_nMaxPage)
		{
			m_nSelPage++;
		}

		if (m_nSelPage == m_nMaxPage)
			m_ButtonNext.Lock();
		if (m_nSelPage > 0)
			m_ButtonBack.UnLock();

		return false;
	}

	if (m_ButtonBack.UpdateMouseEvent())
	{
		if (m_nSelPage > 0)
		{
			m_nSelPage--;
		}

		if (m_nSelPage == 0)
			m_ButtonBack.Lock();
		if (m_nSelPage < m_nMaxPage)
			m_ButtonNext.UnLock();

		return false;
	}

	if (m_Button1.UpdateMouseEvent())
	{
		if (m_dwSelIndex != -1)
		{
			SendRequestAddJewelOfInventory(m_dwSelIndex, 0, 0);
		}
		return false;
	}
	if (m_Button2.UpdateMouseEvent())
	{
		check_budget(1);
		return false;
	}
	return true;
}

bool SEASON3B::CNewUIInventoryJewel::UpdateKeyEvent()
{
	return true;
}

float SEASON3B::CNewUIInventoryJewel::GetLayerDepth()
{
	return 4.25;
}

bool SEASON3B::CNewUIInventoryJewel::CheckExpansionInventory()
{
	if (IsVisible())
		return CheckMouseIn(m_Pos.x, m_Pos.y, 190, 429) != false;
	else
		return false;
}

void SEASON3B::CNewUIInventoryJewel::OpenningProcess()
{
	m_nSelPage = 0;
	m_dwSelIndex = -1;
	m_dwCurIndex = -1;
	m_ButtonBack.Lock();
	m_ButtonNext.UnLock();
	m_nOptionCurrent = 0;
}

void SEASON3B::CNewUIInventoryJewel::ClosingProcess()
{
	m_nSelPage = 0;
	m_dwSelIndex = -1;
	m_dwCurIndex = -1;
	m_ButtonBack.Lock();
	m_ButtonNext.UnLock();
}

void SEASON3B::CNewUIInventoryJewel::RenderFrame()
{
	if (gmProtect->checkold_school())
	{
		RenderInventoryInterface(m_Pos.x, m_Pos.y, 0);
	}
	else
	{
		RenderImage(IMAGE_INVENTORY_BACK, this->m_Pos.x, this->m_Pos.y, 190.0, 429.0);

		RenderImage(IMAGE_INVENTORY_BACK_TOP2, this->m_Pos.x, this->m_Pos.y, 190.0, 64.0);

		RenderImage(IMAGE_INVENTORY_BACK_LEFT, (float)this->m_Pos.x, (float)(this->m_Pos.y + 64), 21.0, 320.0);

		RenderImage(IMAGE_INVENTORY_BACK_RIGHT, (float)(this->m_Pos.x + 169), (float)(this->m_Pos.y + 64), 21.0, 320.0);

		RenderImage(IMAGE_INVENTORY_BACK_BOTTOM, (float)this->m_Pos.x, (float)(this->m_Pos.y + 384), 190.0, 45.0);
	}

	RenderImageF(CNewUIInGameShop::IMAGE_IGS_STORAGE_PAGE, m_Pos.x + 66, this->m_Pos.y + 358.0, 58.0, 22.0, 0.0, 0.0, 80.0, 30.0);

	RenderGoldRect(m_nRectItem.left, m_nRectItem.top, 50, 50, 3);
}

void SEASON3B::CNewUIInventoryJewel::RenderTexts()
{
	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->SetBgColor(0, 0, 0, 0);

	g_pRenderText->SetTextColor(255, 255, 255, 0xFFu);

	char strText[100];
	sprintf_s(strText, gTextClien.TextClien_Khac[3]);

	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 15, strText, 190, 0, 3, 0);

	sprintf_s(strText, "%d / %d", m_nSelPage+1, m_nMaxPage+1);

	g_pRenderText->RenderText(m_Pos.x + 66, m_Pos.y + 358, strText, 58, 22, 3, 0);


	float RenderFrameX = m_Pos.x + 25.f;
	float RenderFrameY = m_Pos.y + 60.f;

	RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y;

	size_t index = m_nSelPage * MAX_JEWEL_INVENTORY;

	for (size_t i = index; i < m_bItems.size() && i < index + MAX_JEWEL_INVENTORY; i++)
	{
		WareHoly* item = &m_bItems[i];

		Script_Item* item_info = GMItemMng->find(item->GetIndex());

		sprintf_s(strText, "x %lld", item->GetValue());

		if (item->GetValue() <= 0)
			g_pRenderText->SetTextColor(180, 180, 180, 0xFFu);
		else
			g_pRenderText->SetTextColor(CLRDW_GOLD);

		if (item_info->Name[0] != '\0')
		{
			g_pRenderText->RenderText(RenderFrameX + 30.f, RenderFrameY + 5.f, item_info->Name);
			g_pRenderText->RenderText(RenderFrameX + 40.f, RenderFrameY + 17.f, strText);
		}
		else
		{
			g_pRenderText->RenderText(RenderFrameX + 30.f, RenderFrameY + 5.f, "Jewel Unknown");

			g_pRenderText->RenderText(RenderFrameX + 40.f, RenderFrameY + 17.f, strText);
		}
		RenderFrameY += 32.f;
	}
}

void SEASON3B::CNewUIInventoryJewel::RenderInter()
{
	EnableAlphaTest(true);
	glColor4f(0.0, 0.0, 0.0, 0.7);

	float RenderFrameX = m_Pos.x + 25.f;
	float RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y;

	int index = (m_nSelPage * MAX_JEWEL_INVENTORY);

	for (size_t i = index; i < m_bItems.size() && i < (size_t)(index + MAX_JEWEL_INVENTORY); i++)
	{
		RenderColor(RenderFrameX, RenderFrameY, 140.0, 30.0);
		RenderFrameY += 32.f;
	}
	EndRenderColor();

	RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y;

	if (m_dwCurIndex != -1 && m_dwCurIndex >= index && m_dwCurIndex < index + MAX_JEWEL_INVENTORY)
	{
		FrameTarget(RenderFrameX, RenderFrameY + ((m_dwCurIndex - index) * 32.0), 140.0, 30.0, RGBA(98, 223, 31, 0xff));
	}
	if ((m_dwSelIndex != -1 && m_dwSelIndex != m_dwCurIndex) && m_dwSelIndex >= index && m_dwSelIndex < index + MAX_JEWEL_INVENTORY)
	{
		FrameTarget(RenderFrameX, RenderFrameY + ((m_dwSelIndex - index) * 32.0), 140.0, 30.0, RGBA(98, 223, 31, 0xff));
	}
}

void SEASON3B::CNewUIInventoryJewel::RenderButtons()
{
	m_Button1.Render();

	m_Button2.Render();

	m_ButtonNext.Render();

	m_ButtonBack.Render();

	this->render_option_group();
}

void SEASON3B::CNewUIInventoryJewel::RenderHoly()
{
	SEASON3B::begin3D();

	float RenderFrameX = m_Pos.x + 25.f;
	float RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y;

	size_t index = (m_nSelPage * MAX_JEWEL_INVENTORY);

	for (size_t i = index; i < m_bItems.size() && i < index + MAX_JEWEL_INVENTORY; i++)
	{
		WareHoly* item = &m_bItems[i];

		Render2Item3D(RenderFrameX + 5.f, RenderFrameY + 5.f, 20.0, 20.0, item->GetIndex(), item->GetLevel(), 0, 0, false);

		RenderFrameY += 32.f;
	}

	if (m_dwSelIndex != -1 && m_dwSelIndex < (int)m_bItems.size())
	{
		RenderFrameX = m_nRectItem.left;
		RenderFrameY = m_nRectItem.top;

		WareHoly* item = &m_bItems[m_dwSelIndex];

		Render2Item3D(RenderFrameX, RenderFrameY, 50, 50, item->GetIndex(), item->GetLevel(), 0, 0, false);
	}

	SEASON3B::endrender3D();
}

void SEASON3B::CNewUIInventoryJewel::FrameTarget(float iPos_x, float iPos_y, float width, float height, DWORD color)
{
	EnableAlphaTest(true);

	BYTE red = GetRed(color);
	BYTE green = GetGreen(color);
	BYTE blue = GetBlue(color);

	glColor4ub(red, green, blue, 30);
	RenderColor(iPos_x, iPos_y, width, height, 0.0, 0);

	glColor4ub(red, green, blue, 255);
	RenderColor(iPos_x, iPos_y, width, 1.f, 0.0, 0);
	RenderColor(iPos_x, iPos_y + height, width, 1.f, 0.0, 0);

	RenderColor(iPos_x, iPos_y + 1.f, 1.f, height - 1.f, 0.0, 0);
	RenderColor(iPos_x + width - 1.f, iPos_y + 1.f, 1.f, height - 1.f, 0.0, 0);

	EndRenderColor();
}

void SEASON3B::CNewUIInventoryJewel::render_option_group()
{
	float RenderFrameX = m_Pos.x + 30.f;
	float RenderFrameY = m_Pos.y + INVENTORY_JEWEL_Y - 25;

	const char* _num[] = { "1", "10", "20", "30" };

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(-1);
	g_pRenderText->SetFont(g_hFontBold);

	RenderGoldRect(RenderFrameX - 5.0, RenderFrameY - 15.0, 140.0, 30, 2);

	for (size_t i = 0; i < 4; i++)
	{
		g_pRenderText->RenderText(RenderFrameX, RenderFrameY - 10, _num[i], 15, 0, 3);

		if (m_nOptionCurrent == i)
		{
			SEASON3B::RenderImageF(IMAGE_OPTION_BUTTON, RenderFrameX, RenderFrameY, 12, 12, 0.0, 0.0, 15.0, 15.0);
		}
		else
		{
			SEASON3B::RenderImageF(IMAGE_OPTION_BUTTON, RenderFrameX, RenderFrameY, 12, 12, 0.0, 15.0, 15.0, 15.0);
		}
		RenderFrameX += 40.0;
	}
}

void SEASON3B::CNewUIInventoryJewel::RemoveData()
{
	m_bItems.clear();
}

void SEASON3B::CNewUIInventoryJewel::InsertData(BYTE Index, short ItemIndex, short ItemLevel, __int64 count)
{
	if (ItemIndex >= ITEM_WING && ItemIndex < MAX_ITEM)
	{
		m_bItems.push_back({ Index, ItemIndex, ItemLevel, count });
	}

	m_nMaxPage = (m_bItems.size() / MAX_JEWEL_INVENTORY);
}

bool SEASON3B::CNewUIInventoryJewel::check_budget(DWORD _iCount)
{
	if (m_dwSelIndex >= 0 && m_dwSelIndex < (int)m_bItems.size())
	{
		/*DWORD _iValue = _iCount;

		__int64 _iStock = m_bItems[m_dwSelIndex].GetValue();

		if (m_nOptionCurrent == 0)
			_iValue = _iCount;
		else
			_iValue = _iCount * (10 * m_nOptionCurrent);

		if (_iStock >= _iValue)
		{
			return true;
		}*/
		SendRequestRemoveJewelOfInventory(m_dwSelIndex, m_nOptionCurrent, _iCount);
	}

	return false;
}