// NewUIMyShopInventory.cpp: implementation of the CNewUIMyShopInventory class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIMyShopInventory.h"
#include "NewUISystem.h"
#include "NewUICustomMessageBox.h"
#include "wsclientinline.h"
#include "PersonalShopTitleImp.h"
#include "CGMProtect.h"

const int iMAX_SHOPTITLE_MULTI = 26;

namespace
{
	void RenderText(const char* text, int x, int y, int sx, int sy, DWORD color, DWORD backcolor, int sort, HFONT hFont = g_hFont)
	{
		g_pRenderText->SetFont(hFont);

		DWORD backuptextcolor = g_pRenderText->GetTextColor();
		DWORD backuptextbackcolor = g_pRenderText->GetBgColor();

		g_pRenderText->SetTextColor(color);
		g_pRenderText->SetBgColor(backcolor);
		g_pRenderText->RenderText(x, y, text, sx, sy, sort);

		g_pRenderText->SetTextColor(backuptextcolor);
		g_pRenderText->SetBgColor(backuptextbackcolor);
	}
};


SEASON3B::CNewUIMyShopInventory::CNewUIMyShopInventory() : m_SourceIndex(-1), m_TargetIndex(-1), m_EnablePersonalShop(false), MAX_SHOPTITLE_UTF16(min(26, (int)(MAX_SHOPTITLE / g_pMultiLanguage->GetNumByteForOneCharUTF8())))
{
	m_pNewUIMng = NULL;
	m_pNewInventoryCtrl = NULL;
	m_Pos.x = m_Pos.y = 0;
	m_EditBox = NULL;
	m_Button = NULL;
	m_bIsEnableInputValueTextBox = false;

	type_store = 0;
}

SEASON3B::CNewUIMyShopInventory::~CNewUIMyShopInventory()
{
	Release();
}

bool SEASON3B::CNewUIMyShopInventory::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng || NULL == g_pNewUI3DRenderMng || NULL == g_pNewItemMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MYSHOP_INVENTORY, this);

	m_pNewInventoryCtrl = new CNewUIInventoryCtrl;

	if (false == m_pNewInventoryCtrl->Create(g_pNewUI3DRenderMng, g_pNewItemMng, this, x + 15, y + 90, 8, 4))
	{
		SAFE_DELETE(m_pNewInventoryCtrl);
		return false;
	}

	m_pNewInventoryCtrl->SetToolTipType(TOOLTIP_TYPE_MY_SHOP);

	this->SetPos(x, y);

	m_EditBox = new CUITextInputBox;
	m_Button = new CNewUIButton[MYSHOPINVENTORY_MAXBUTTONCOUNT];

	LoadImages();

	m_Button[MYSHOPINVENTORY_EXIT].ChangeToolTipText(GlobalText[1002], true);
	m_Button[MYSHOPINVENTORY_OPEN].ChangeToolTipText(GlobalText[1107], true);
	m_Button[MYSHOPINVENTORY_CLOSE].ChangeToolTipText(GlobalText[1108], true);

#if MAIN_UPDATE > 603
	m_pNewInventoryCtrl->SetPos(m_Pos.x + 22, m_Pos.y + 110);

	m_Button[MYSHOPINVENTORY_EXIT].ChangeButtonImgState(true, BITMAP_btn_exit_00, false);
	m_Button[MYSHOPINVENTORY_EXIT].ChangeButtonInfo(m_Pos.x + 22, m_Pos.y + 384, 20, 20);

	m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonImgState(true, BITMAP_btn_openshop, false);
	m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonInfo(m_Pos.x + 44, m_Pos.y + 384, 20, 20);

	m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonImgState(true, BITMAP_btn_closeshop, false);
	m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonInfo(m_Pos.x + 66, m_Pos.y + 384, 20, 20);

	m_EditBox->Init(gwinhandle->GethWnd(), 200, 14, iMAX_SHOPTITLE_MULTI - 1);
	m_EditBox->SetPosition(m_Pos.x + 50, m_Pos.y + 50);

	m_Button[MYSHOPINVENTORY_EXIT].CurSize(69.f, 72.f);
	m_Button[MYSHOPINVENTORY_OPEN].CurSize(69.f, 72.f);
	m_Button[MYSHOPINVENTORY_CLOSE].CurSize(69.f, 72.f);
#elif MAIN_UPDATE > 303
	m_pNewInventoryCtrl->SetPos(m_Pos.x + 15, m_Pos.y + 110);

	if (gmProtect->checkold_school())
	{
		SetButtonInterface(&m_Button[MYSHOPINVENTORY_EXIT], m_Pos.x, m_Pos.y, 1);

		SetButtonImagen(&m_Button[MYSHOPINVENTORY_OPEN], Bitmap_text_button1, Bitmap_text_button2);
		SetButtonImagen(&m_Button[MYSHOPINVENTORY_CLOSE], Bitmap_text_button1, Bitmap_text_button2);

		m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonInfo(m_Pos.x + 48.0, m_Pos.y + 395.0, 56.0, 24.0);
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonInfo(m_Pos.x + 114.0, m_Pos.y + 395.0, 56.0, 24.0);

		m_Button[MYSHOPINVENTORY_OPEN].ChangeText(GlobalText[1107]);
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeText(GlobalText[1108]);

		if (m_EditBox)
		{
			m_EditBox->Init(gwinhandle->GethWnd(), 130, 14, iMAX_SHOPTITLE_MULTI - 1);
			m_EditBox->SetPlaceHolder(GlobalText[1105]);
		}

		this->SetTextPosition(m_Pos.x + 30, m_Pos.y + 70);
	}
	else
	{
		m_Button[MYSHOPINVENTORY_EXIT].ChangeButtonImgState(true, IMAGE_MYSHOPINVENTORY_EXIT_BTN, false);
		m_Button[MYSHOPINVENTORY_EXIT].ChangeButtonInfo(m_Pos.x + 13, m_Pos.y + 391, 36, 29);

		m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonImgState(true, IMAGE_MYSHOPINVENTORY_OPEN, false);
		m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonInfo(m_Pos.x + 53, m_Pos.y + 391, 36, 29);

		m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonImgState(true, IMAGE_MYSHOPINVENTORY_CLOSE, false);
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonInfo(m_Pos.x + 93, m_Pos.y + 391, 36, 29);

		if (m_EditBox)
		{
			m_EditBox->Init(gwinhandle->GethWnd(), 200, 14, iMAX_SHOPTITLE_MULTI - 1);
		}

		this->SetTextPosition(m_Pos.x + 50, m_Pos.y + 55);
	}
#else
	m_pNewInventoryCtrl->SetPos(m_Pos.x + 15, m_Pos.y + 110);

	SetButtonInterface(&m_Button[MYSHOPINVENTORY_EXIT], m_Pos.x, m_Pos.y, 1);

	SetButtonImagen(&m_Button[MYSHOPINVENTORY_OPEN], Bitmap_text_button1, Bitmap_text_button2);
	SetButtonImagen(&m_Button[MYSHOPINVENTORY_CLOSE], Bitmap_text_button1, Bitmap_text_button2);

	m_Button[MYSHOPINVENTORY_OPEN].ChangeButtonInfo(m_Pos.x + 48.0, m_Pos.y + 395.0, 56.0, 24.0);
	m_Button[MYSHOPINVENTORY_CLOSE].ChangeButtonInfo(m_Pos.x + 114.0, m_Pos.y + 395.0, 56.0, 24.0);

	m_Button[MYSHOPINVENTORY_OPEN].ChangeText(GlobalText[1107]);
	m_Button[MYSHOPINVENTORY_CLOSE].ChangeText(GlobalText[1108]);

	m_EditBox->Init(gwinhandle->GethWnd(), 130, 14, iMAX_SHOPTITLE_MULTI - 1);
	m_EditBox->SetPosition(m_Pos.x + 30, m_Pos.y + 70);
	m_EditBox->SetPlaceHolder(GlobalText[1105]);
#endif // MAIN_UPDATE == 303

	if (m_EditBox)
	{
		m_EditBox->SetTextColor(255, 255, 230, 210);
		m_EditBox->SetBackColor(0, 0, 0, 0);
		m_EditBox->SetFont(g_hFont);
	}

	ChangeEditBox(UISTATE_NORMAL);
	ChangePersonal(m_EnablePersonalShop);

	Show(false);

	return true;
}

void SEASON3B::CNewUIMyShopInventory::Release()
{
	SAFE_DELETE(m_pNewInventoryCtrl);
	SAFE_DELETE_ARRAY(m_Button);
	SAFE_DELETE(m_EditBox);

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}

	UnloadImages();
}

void SEASON3B::CNewUIMyShopInventory::LoadImages()
{
#ifdef test_main_dev
	LoadBitmap("Interface\\HUD_TEST\\box_openTitle.dds", BITMAP_box_opentitle, GL_LINEAR);
#else
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_MYSHOPINVENTORY_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back01.tga", IMAGE_MYSHOPINVENTORY_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_MYSHOPINVENTORY_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_MYSHOPINVENTORY_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_MYSHOPINVENTORY_BOTTOM, GL_LINEAR);
	LoadBitmap("Interface\\newui_exit_00.tga", IMAGE_MYSHOPINVENTORY_EXIT_BTN, GL_LINEAR);
	LoadBitmap("Interface\\newui_Box_openTitle.tga", IMAGE_MYSHOPINVENTORY_EDIT, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_openshop.tga", IMAGE_MYSHOPINVENTORY_OPEN, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_closeshop.tga", IMAGE_MYSHOPINVENTORY_CLOSE, GL_LINEAR);
#endif // test_main_dev
	LoadBitmap("Interface\\HUD\\Button.tga", IMAGE_MYSHOPINVENTORY_BUTTON, GL_LINEAR);
}

void SEASON3B::CNewUIMyShopInventory::UnloadImages()
{
#ifdef test_main_dev
	DeleteBitmap(BITMAP_box_opentitle);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_BUTTON);
#else
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_CLOSE);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_OPEN);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_EDIT);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_EXIT_BTN);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_BOTTOM);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_RIGHT);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_LEFT);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_TOP);
	DeleteBitmap(IMAGE_MYSHOPINVENTORY_BACK);
#endif // BITMAP_box_opentitle
}

void SEASON3B::CNewUIMyShopInventory::SetPos(int x, int y)
{
	m_Pos.x = x; m_Pos.y = y;

#if MAIN_UPDATE > 303
	if (m_Button != NULL)
	{
		m_Button[MYSHOPINVENTORY_EXIT].SetPos(m_Pos.x + 22.0, m_Pos.y + 384.0);
		m_Button[MYSHOPINVENTORY_OPEN].SetPos(m_Pos.x + 44.0, m_Pos.y + 384.0);
		m_Button[MYSHOPINVENTORY_CLOSE].SetPos(m_Pos.x + 66.0, m_Pos.y + 384.0);
	}

	this->SetTextPosition(m_Pos.x + 50, m_Pos.y + 50);
#elif MAIN_UPDATE > 303
	if (m_Button != NULL)
	{
		if (gmProtect->checkold_school())
		{
			m_Button[MYSHOPINVENTORY_EXIT].SetPos(m_Pos.x + 20.0, m_Pos.y + 395.0);
			m_Button[MYSHOPINVENTORY_OPEN].SetPos(m_Pos.x + 48.0, m_Pos.y + 395.0);
			m_Button[MYSHOPINVENTORY_CLOSE].SetPos(m_Pos.x + 114.0, m_Pos.y + 395.0);
		}
		else
		{
			m_Button[MYSHOPINVENTORY_EXIT].SetPos(m_Pos.x + 13, m_Pos.y + 391);
			m_Button[MYSHOPINVENTORY_OPEN].SetPos(m_Pos.x + 53, m_Pos.y + 391);
			m_Button[MYSHOPINVENTORY_CLOSE].SetPos(m_Pos.x + 93, m_Pos.y + 391);
		}
	}

	if (gmProtect->checkold_school())
		this->SetTextPosition(m_Pos.x + 30, m_Pos.y + 70);
	else
		this->SetTextPosition(m_Pos.x + 50, m_Pos.y + 55);
#else
	if (m_Button != NULL)
	{
		m_Button[MYSHOPINVENTORY_EXIT].SetPos(m_Pos.x + 20.0, m_Pos.y + 395.0);
		m_Button[MYSHOPINVENTORY_OPEN].SetPos(m_Pos.x + 48.0, m_Pos.y + 395.0);
		m_Button[MYSHOPINVENTORY_CLOSE].SetPos(m_Pos.x + 114.0, m_Pos.y + 395.0);
	}

	this->SetTextPosition(m_Pos.x + 30, m_Pos.y + 70);
#endif // MAIN_UPDATE == 303
}

void SEASON3B::CNewUIMyShopInventory::GetTitle(unicode::t_string& titletext)
{
	if (m_EditBox)
	{
		wchar_t* pwszTitle = new wchar_t[iMAX_SHOPTITLE_MULTI];
		m_EditBox->GetText(pwszTitle, iMAX_SHOPTITLE_MULTI);
#ifdef CHINESE_LANGUAGE
		g_pMultiLanguage->ConvertWideCharToStr(titletext, pwszTitle, CHINESE_CP);
#else
		g_pMultiLanguage->ConvertWideCharToStr(titletext, pwszTitle, CP_UTF8);
#endif
		delete[] pwszTitle;
	}
}

bool SEASON3B::CNewUIMyShopInventory::InsertItem(int iIndex, BYTE* pbyItemPacket)
{
	if (m_pNewInventoryCtrl)
	{
		return m_pNewInventoryCtrl->AddItem(iIndex, pbyItemPacket);
	}

	return false;
}

void SEASON3B::CNewUIMyShopInventory::DeleteItem(int iIndex)
{
	if (m_pNewInventoryCtrl)
	{
		ITEM* pItem = m_pNewInventoryCtrl->FindItem(iIndex);
		if (pItem != NULL)
			m_pNewInventoryCtrl->RemoveItem(pItem);
	}
}

void SEASON3B::CNewUIMyShopInventory::DeleteAllItems()
{
	if (m_pNewInventoryCtrl)
	{
		m_pNewInventoryCtrl->RemoveAllItems();
	}
}

ITEM* SEASON3B::CNewUIMyShopInventory::FindItem(int iLinealPos)
{
	if (m_pNewInventoryCtrl)
		return m_pNewInventoryCtrl->FindItem(iLinealPos);
	return NULL;
}

void SEASON3B::CNewUIMyShopInventory::ChangePersonal(bool state)
{
	m_EnablePersonalShop = state;

	if (m_EnablePersonalShop)
	{
		m_Button[MYSHOPINVENTORY_OPEN].ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_OPEN].ChangeTextColor(RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_OPEN].UnLock();
		m_Button[MYSHOPINVENTORY_OPEN].ChangeToolTipText(GlobalText[1106], true);
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeTextColor(RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_CLOSE].UnLock();
	}
	else
	{
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeImgColor(BUTTON_STATE_UP, RGBA(100, 100, 100, 255));
		m_Button[MYSHOPINVENTORY_CLOSE].ChangeTextColor(RGBA(100, 100, 100, 255));
		m_Button[MYSHOPINVENTORY_CLOSE].Lock();
		m_Button[MYSHOPINVENTORY_OPEN].ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_OPEN].ChangeTextColor(RGBA(255, 255, 255, 255));
		m_Button[MYSHOPINVENTORY_OPEN].UnLock();
		m_Button[MYSHOPINVENTORY_OPEN].ChangeToolTipText(GlobalText[1107], true);
	}
}

void SEASON3B::CNewUIMyShopInventory::OpenButtonLock()
{
	m_Button[MYSHOPINVENTORY_OPEN].ChangeImgColor(BUTTON_STATE_UP, RGBA(100, 100, 100, 255));
	m_Button[MYSHOPINVENTORY_OPEN].ChangeTextColor(RGBA(100, 100, 100, 255));
	m_Button[MYSHOPINVENTORY_OPEN].Lock();
	m_Button[MYSHOPINVENTORY_OPEN].ChangeToolTipText(GlobalText[1107], true);
}

void SEASON3B::CNewUIMyShopInventory::OpenButtonUnLock()
{
	m_Button[MYSHOPINVENTORY_OPEN].ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
	m_Button[MYSHOPINVENTORY_OPEN].ChangeTextColor(RGBA(255, 255, 255, 255));
	m_Button[MYSHOPINVENTORY_OPEN].UnLock();
	m_Button[MYSHOPINVENTORY_OPEN].ChangeToolTipText(GlobalText[1107], true);
}

const bool SEASON3B::CNewUIMyShopInventory::IsEnablePersonalShop() const
{
	return m_EnablePersonalShop;
}

void SEASON3B::CNewUIMyShopInventory::ChangeEditBox(const UISTATES type)
{
	if (m_EditBox)
	{
		m_EditBox->SetState(type);
		if (type == UISTATE_NORMAL)
			m_EditBox->GiveFocus();
		m_EditBox->SetText(NULL);
	}
}

bool SEASON3B::CNewUIMyShopInventory::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYSHOP_INVENTORY) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}

	return true;
}

bool SEASON3B::CNewUIMyShopInventory::MyShopInventoryProcess()
{
	if (CheckMouseIn(m_Pos.x, m_Pos.y, INVENTORY_WIDTH, INVENTORY_HEIGHT) == false)
	{
		return false;
	}

	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	if (m_pNewInventoryCtrl && pPickedItem && IsRelease(VK_LBUTTON))
	{
		ITEM* pItemObj = pPickedItem->GetItem();
		int iSourceIndex = pPickedItem->GetSourceLinealPos();
		int iTargetIndex = pPickedItem->GetTargetLinealPos(m_pNewInventoryCtrl);

		if (IsPersonalShopBan(pItemObj))
			m_pNewInventoryCtrl->SetSquareColorNormal(1.0f, 0.0f, 0.0f);
		else
			m_pNewInventoryCtrl->SetSquareColorNormal(0.1f, 0.4f, 0.8f);

		if (iTargetIndex == -1)
		{
			return true;
		}

		int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

		if (InvenIndex != -1)
		{
			if (IsPersonalShopBan(pItemObj) == true)
			{
				g_pChatListBox->AddText("", GlobalText[2226], SEASON3B::TYPE_ERROR_MESSAGE);
				return true;
			}

			iSourceIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(iSourceIndex, InvenIndex);

			if (m_pNewInventoryCtrl->CanMove(iTargetIndex, pItemObj))
			{
				ChangeSourceIndex(iSourceIndex + MAX_EQUIPMENT);
				ChangeTargetIndex(iTargetIndex);

				if (gmProtect->SwitchPersonalShopType == 0)
					CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopItemValueMsgBoxLayout));
				else
					CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopProItemValueMsgBoxLayout));

				SetInputValueTextBox(true);

				pPickedItem->HidePickedItem();
				return true;
			}
		}
		else if (pPickedItem->GetOwnerInventory() == NULL)
		{
			if (IsPersonalShopBan(pItemObj) == true)
			{
				g_pChatListBox->AddText("", GlobalText[2226], SEASON3B::TYPE_ERROR_MESSAGE);
				return true;
			}

			if (m_pNewInventoryCtrl->CanMove(iTargetIndex, pItemObj))
			{
				ChangeSourceIndex(iSourceIndex);
				ChangeTargetIndex(iTargetIndex);

				if (gmProtect->SwitchPersonalShopType == 0)
					CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopItemValueMsgBoxLayout));
				else
					CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopProItemValueMsgBoxLayout));

				SetInputValueTextBox(true);

				pPickedItem->HidePickedItem();
				return true;
			}
		}
		else if (pPickedItem->GetOwnerInventory() == m_pNewInventoryCtrl)
		{
			if (m_pNewInventoryCtrl->CanMove(iTargetIndex, pItemObj))
			{
				ChangeSourceIndex(iSourceIndex);
				ChangeTargetIndex(iTargetIndex);
				SendRequestEquipmentItem(REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iSourceIndex, pItemObj,
					REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iTargetIndex);
				return true;
			}
		}
	}
	else if (m_pNewInventoryCtrl && !pPickedItem && IsPress(VK_RBUTTON))
	{
		int iCurSquareIndex = m_pNewInventoryCtrl->FindItemptIndex(MouseX, MouseY);

		if (iCurSquareIndex != -1)
		{
			ChangeSourceIndex(iCurSquareIndex);
			ChangeTargetIndex(-1);

			if (gmProtect->SwitchPersonalShopType == 0)
				CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopItemValueMsgBoxLayout));
			else
				CreateMessageBox(MSGBOX_LAYOUT_CLASS(CPersonalShopProItemValueMsgBoxLayout));

			SetInputValueTextBox(true);
			return true;
		}
	}

	return false;
}

void SEASON3B::CNewUIMyShopInventory::SetTextPosition(int x, int y)
{
	if (m_EditBox)
	{
		m_EditBox->SetPosition(x, y);
	}
}

bool SEASON3B::CNewUIMyShopInventory::UpdateMouseEvent()
{
	if (m_pNewInventoryCtrl && false == m_pNewInventoryCtrl->UpdateMouseEvent())
	{
		return false;
	}

	if (CheckMouseIn(m_Pos.x, m_Pos.y, INVENTORY_WIDTH, INVENTORY_HEIGHT))
	{
		if (MyShopInventoryProcess() == true)
		{
			return false;
		}

		POINT ptExitBtn1 = { m_Pos.x + 169, m_Pos.y + 7 };

		if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(ptExitBtn1.x, ptExitBtn1.y, 13, 12))
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
			return false;
		}

		if (m_EditBox)
		{
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				if(CheckMouseIn(m_EditBox->GetPosition_x(), m_EditBox->GetPosition_y(), m_EditBox->GetWidth(), m_EditBox->GetHeight()))
					ChangeEditBox(UISTATE_NORMAL);
				else
					SetFocus(gwinhandle->GethWnd());
			}
		}

		if (m_EnablePersonalShop)
		{
			if (gmProtect->CustomOffStoreEnable)
			{
				float RenderFrameX = m_Pos.x + 40;
				float RenderFrameY = m_Pos.y + 350;

				if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 108, 30) && SEASON3B::IsRelease(VK_LBUTTON))
				{
					CGSendOffTrade(6);
					g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
					return false;
				}
			}
		}
		else
		{
			if (gmProtect->SwitchPersonalShopType == 0)
			{
				float RenderFrameX = m_Pos.x + 20;
				float RenderFrameY = m_Pos.y + 341;

				if (gmProtect->CustomStoreEnableJoB)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(0);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
				if (gmProtect->CustomStoreEnableJoS)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX + 56.f, RenderFrameY, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(1);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
				if (gmProtect->CustomStoreEnableJoC)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX + 112.f, RenderFrameY, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(2);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
				if (gmProtect->CustomStoreEnableCoin1)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY + 21.f, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(3);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
				if (gmProtect->CustomStoreEnableCoin2)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX + 56.f, RenderFrameY + 21.f, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(4);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
				if (gmProtect->CustomStoreEnableCoin3)
				{
					if (SEASON3B::CheckMouseIn(RenderFrameX + 112.f, RenderFrameY + 21.f, 40.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
					{
						CGSendOffTrade(5);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						return false;
					}
				}
			}
		}
	}

	if (m_EditBox)
	{
		m_EditBox->DoAction();
	}

	for (int i = 0; i < MYSHOPINVENTORY_MAXBUTTONCOUNT; ++i)
	{
		if (m_Button[i].UpdateMouseEvent())
		{
			switch (i)
			{
			case 0:
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
			}
			return false;
			case 1:
			{
				unicode::t_string strTitle;
				GetTitle(strTitle);
				if (IsExistUndecidedPrice() == false && strTitle.size() > 0)
				{
					if (m_EnablePersonalShop == false)
					{
						SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPersonalshopCreateMsgBoxLayout));
					}
					else
					{
						SendRequestCreatePersonalShop(const_cast<char*>(strTitle.c_str()));
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
						g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);
					}
				}
				else
				{
					g_pChatListBox->AddText("", GlobalText[1119], SEASON3B::TYPE_ERROR_MESSAGE);
				}
			}
			return false;
			case 2:
			{
				SendRequestDestoryPersonalShop();

				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);
			}
			return false;
			}
		}
	}

	if (CheckMouseIn(m_Pos.x, m_Pos.y, INVENTORY_WIDTH, INVENTORY_HEIGHT))
	{
		if (SEASON3B::IsPress(VK_RBUTTON))
		{
			MouseRButton = false;
			MouseRButtonPop = false;
			MouseRButtonPush = false;
			return false;
		}

		if (SEASON3B::IsNone(VK_LBUTTON) == false)
		{
			return false;
		}
	}

	return true;
}

bool SEASON3B::CNewUIMyShopInventory::Update()
{
	if (m_pNewInventoryCtrl && false == m_pNewInventoryCtrl->Update())
	{
		return false;
	}

	return true;
}

void SEASON3B::CNewUIMyShopInventory::RenderFrame()
{
	SEASON3B::window_nomal((float)m_Pos.x, (float)m_Pos.y);

#if MAIN_UPDATE > 603
	RenderImageF(BITMAP_box_opentitle, m_Pos.x + ((WIN_WINDOW_SIZEX - 127.f) / 2.f), m_Pos.y + 45, 127.f, 26.f, 0.0, 0.0, 718.0, 146.0);

	RenderText(GlobalText[1102], m_Pos.x + ((WIN_WINDOW_SIZEX - 84) / 2), m_Pos.y + 12, 84, 0, 0xFF49B0FF, 0x00000000, RT3_SORT_CENTER);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 20.0, m_Pos.y + 65.0, 150.0, 24.0, 0, 0, 0.8828125, 0.5625);
	}
	else
	{
		RenderImage(IMAGE_MYSHOPINVENTORY_EDIT, m_Pos.x + 12, m_Pos.y + 49, 169.f, 26.f);
	}

	if (gmProtect->checkold_school())
		RenderText(GlobalText[1102], m_Pos.x + 35, m_Pos.y + 15, 120, 0, 0xFF49B0FF, RGBA(20, 20, 20, 255), RT3_SORT_CENTER);
	else
		RenderText(GlobalText[1102], m_Pos.x, m_Pos.y + 15, 190, 0, 0xFF49B0FF, 0x00000000, RT3_SORT_CENTER);
#else
	RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 20.0, m_Pos.y + 65.0, 150.0, 24.0, 0, 0, 0.8828125, 0.5625);
	RenderText(GlobalText[1102], m_Pos.x + 35, m_Pos.y + 15, 120, 0, 0xFF49B0FF, RGBA(20, 20, 20, 255), RT3_SORT_CENTER);
#endif

	if (m_EnablePersonalShop)
	{
		if (gmProtect->CustomOffStoreEnable)
		{

			float RenderFrameX = m_Pos.x + 40;
			float RenderFrameY = m_Pos.y + 350;

			float uv = 0.0;

			if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 108, 30))
			{
				uv = 90.f / 128.f;
				if (MouseLButton)
				{
					uv = 60.f / 128.f;
				}
			}
			SEASON3B::RenderImage(BITMAP_TEXT_BTN, RenderFrameX, RenderFrameY, 108, 30, 0.0, uv, 108.f / 128.f, 30.f / 128.f);

			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetBgColor(0, 0, 0, 0);
			g_pRenderText->SetTextColor(CLRDW_YELLOW);
			g_pRenderText->RenderText(RenderFrameX, RenderFrameY, "OFF STORE", 108, 30, RT3_SORT_CENTER);
		}
	}
	else
	{
		if (gmProtect->SwitchPersonalShopType == 0)
		{
			float RenderFrameX = m_Pos.x + 20;
			float RenderFrameY = m_Pos.y + 341;

			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetBgColor(0, 0, 0, 0);
			g_pRenderText->SetTextColor(CLRDW_WHITE);

			if (gmProtect->CustomStoreEnableJoB)
			{
				RenderButton(RenderFrameX, RenderFrameY, 40.f, 19.f, GlobalText[1806]);
			}
			if (gmProtect->CustomStoreEnableJoS)
			{
				RenderButton(RenderFrameX + 56.f, RenderFrameY, 40.f, 19.f, GlobalText[1807]);
			}
			if (gmProtect->CustomStoreEnableJoC)
			{
				RenderButton(RenderFrameX + 112.f, RenderFrameY, 40.f, 19.f, GlobalText[3316]);
			}
			if (gmProtect->CustomStoreEnableCoin1)
			{
				RenderButton(RenderFrameX, RenderFrameY + 21.f, 40.f, 19.f, TextGlobal[58]);
			}
			if (gmProtect->CustomStoreEnableCoin2)
			{
				RenderButton(RenderFrameX + 56.f, RenderFrameY + 21.f, 40.f, 19.f, TextGlobal[59]);
			}
			if (gmProtect->CustomStoreEnableCoin3)
			{
				RenderButton(RenderFrameX + 112.f, RenderFrameY + 21.f, 40.f, 19.f, TextGlobal[60]);
			}
		}
	}
}

void SEASON3B::CNewUIMyShopInventory::RenderTextInfo()
{
	unicode::t_char Text[100];

	if (m_EnablePersonalShop)
	{
		RenderText(GlobalText[1103], m_Pos.x, m_Pos.y + 200, 190, 0, RGBA(215, 138, 0, 255), 0x00000000, RT3_SORT_CENTER, g_hFontBold);
	}

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[370]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 230, 0, 0, RGBA(255, 45, 47, 255), 0x00000000, RT3_SORT_LEFT, g_hFontBold);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1109]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 250, 0, 0, RGBA(247, 206, 77, 255), 0x00000000, RT3_SORT_LEFT);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1111]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 262, 0, 0, RGBA(247, 206, 77, 255), 0x00000000, RT3_SORT_LEFT);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1112]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 274, 0, 0, RGBA(247, 206, 77, 255), 0x00000000, RT3_SORT_LEFT);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1113]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 286, 0, 0, RGBA(247, 206, 77, 255), 0x00000000, RT3_SORT_LEFT);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1115]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 298, 0, 0, RGBA(247, 206, 77, 255), 0x00000000, RT3_SORT_LEFT);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1134]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 320, 0, 0, RGBA(255, 45, 47, 255), 0x00000000, RT3_SORT_LEFT, g_hFontBold);

	memset(&Text, 0, sizeof(unicode::t_char) * 100);
	sprintf(Text, GlobalText[1135]);
	RenderText(Text, m_Pos.x + 30, m_Pos.y + 332, 0, 0, RGBA(255, 45, 47, 255), 0x00000000, RT3_SORT_LEFT, g_hFontBold);
}

void SEASON3B::CNewUIMyShopInventory::RenderButton(float posx, float posy, float width, float height, const char* psText)
{
	DWORD Color = CLRDW_WHITE;

	if (SEASON3B::CheckMouseIn(posx, posy, width, height))
	{
		Color = ARGB(255, 220, 220, 220);
	}
	SEASON3B::RenderImage(IMAGE_MYSHOPINVENTORY_BUTTON, posx, posy, width, height, 0.0, 0.0, 40.f / 64.f, 19.f / 32.f, Color);


	g_pRenderText->RenderText((int)posx, (int)posy, psText, (int)width, (int)height, RT3_SORT_CENTER);
}

bool SEASON3B::CNewUIMyShopInventory::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();

	RenderTextInfo();

	if (m_EditBox)
	{
		m_EditBox->Render();
	}

	if (m_pNewInventoryCtrl)
	{
		m_pNewInventoryCtrl->Render();
	}

	for (int i = 0; i < MYSHOPINVENTORY_MAXBUTTONCOUNT; ++i)
	{
		m_Button[i].Render();
	}

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIMyShopInventory::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);
	float RenderFrameY = (m_Pos.y);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, (int)RenderFrameY);

	if (m_pNewInventoryCtrl != NULL)
	{
#ifdef test_main_dev
		m_pNewInventoryCtrl->SetPos(m_Pos.x + 22, m_Pos.y + 90);
#else
		if (gmProtect->checkold_school())
			m_pNewInventoryCtrl->SetPos(m_Pos.x + 15, m_Pos.y + 110);
		else
			m_pNewInventoryCtrl->SetPos(m_Pos.x + 15, m_Pos.y + 90);
#endif // test_main_dev

	}
}

void SEASON3B::CNewUIMyShopInventory::ClosingProcess()
{
	CNewUIInventoryCtrl::BackupPickedItem();
	g_pMyInventory->ChangeMyShopButtonStateOpen();
	SetFocus(gwinhandle->GethWnd());
}

int SEASON3B::CNewUIMyShopInventory::GetPointedItemIndex()
{
	int iPointedItemIndex = m_pNewInventoryCtrl->GetPointedSquareIndex();

	if (iPointedItemIndex != -1)
	{
		iPointedItemIndex += MAX_MY_INVENTORY_INDEX;
	}

	return iPointedItemIndex;
}

void SEASON3B::CNewUIMyShopInventory::ResetSubject()
{
	if (m_EditBox)
	{
		m_EditBox->SetText(NULL);
	}
}

bool SEASON3B::CNewUIMyShopInventory::IsEnableInputValueTextBox()
{
	return m_bIsEnableInputValueTextBox;
}

void SEASON3B::CNewUIMyShopInventory::SetInputValueTextBox(bool bIsEnable)
{
	m_bIsEnableInputValueTextBox = bIsEnable;
}

void SEASON3B::CNewUIMyShopInventory::SetTypeValue(int Type)
{
	type_store = Type;
}

int SEASON3B::CNewUIMyShopInventory::GetTypeValue()
{
	return type_store;
}
