//*****************************************************************************
// File: NewUIStorageInventory.cpp
//*****************************************************************************

#include "stdafx.h"
#include "NewUIStorageInventory.h"
#include "NewUISystem.h"
#include "NewUICustomMessageBox.h"
#include "ZzzInventory.h"
#include "wsclientinline.h"
#ifdef KJH_PBG_ADD_INGAMESHOP_SYSTEM
#include "GameShop\MsgBoxIGSCommon.h"
#endif // KJH_PBG_ADD_INGAMESHOP_SYSTEM
#include "CGMProtect.h"

using namespace SEASON3B;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNewUIStorageInventory::CNewUIStorageInventory()
{
	m_pNewUIMng = NULL;
	for(int i = 0; i < MAX_WHAREHOUSE_EXPANSION; i++)
		m_pNewInventoryCtrl[i] = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;

	m_bLock = false;
	m_bCorrectPassword = false;
	m_bItemAutoMove = false;
	m_nBackupMouseX = 0;
	m_nBackupMouseY = 0;
	m_bTakeZen = false;
	m_nBackupTakeZen = 0;
	m_nBackupInvenIndex = 0;
	m_ExpansionState = false;
	m_InvenIndex = 0;
}

CNewUIStorageInventory::~CNewUIStorageInventory()
{
	Release();
}

bool CNewUIStorageInventory::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng || NULL == g_pNewUI3DRenderMng
		|| NULL == g_pNewItemMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_STORAGE, this);

	for (int i = 0; i < MAX_WHAREHOUSE_EXPANSION; i++)
	{
		m_pNewInventoryCtrl[i] = new CNewUIInventoryCtrl;
#ifdef test_main_dev
		if (false == m_pNewInventoryCtrl[i]->Create(g_pNewUI3DRenderMng, g_pNewItemMng, this, x + 22 - (i * WIN_WINDOW_SIZEX), y + 36, 8, 15))
		{
			SAFE_DELETE(m_pNewInventoryCtrl[i]);
			return false;
		}
#else
		if (false == m_pNewInventoryCtrl[i]->Create(g_pNewUI3DRenderMng, g_pNewItemMng, this, x + 15 - (i * WIN_WINDOW_SIZEX), y + 36, 8, 15))
		{
			SAFE_DELETE(m_pNewInventoryCtrl[i]);
			return false;
		}
#endif // test_main_dev
	}

	SetExpansionCloseState();

	SetPos(x, y);

	LoadImages();

#if MAIN_UPDATE > 603
	int anBtnPosX[MAX_BTN] = { 22, 44, 66 };
	int anToolTipText[MAX_BTN] = { 235, 236, 242 };

	for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
	{
		m_abtn[i].ChangeButtonImgState(true, BITMAP_btn_money01 + i, false, false, false, 0.0, 0.0, 69.f, 72.f);
		m_abtn[i].ChangeButtonInfo(x + anBtnPosX[i], y + 384, 20, 20);
		m_abtn[i].ChangeToolTipText(GlobalText[anToolTipText[i]], true);
		//m_abtn[i].CurSize(69.f, 72.f);
	}
	if (GMProtect->IsBaulExt() == true)
	{
		m_BtnExpansion.ChangeButtonImgState(true, BITMAP_btn_expansion, false, false, false, 0.0, 0.0, 69.f, 72.f);
		m_BtnExpansion.ChangeButtonInfo(x + 88, y + 384, 20, 20);
		m_BtnExpansion.ChangeToolTipText(GlobalText[3338], true);
		//m_BtnExpansion.CurSize(69.f, 72.f);
	}
	m_ExpansionState = false;
#else
	if (gmProtect->checkold_school())
	{
		m_pNewInventoryCtrl[0]->SetPos(m_Pos.x + 15, m_Pos.y + 50);
		m_pNewInventoryCtrl[1]->SetPos(m_Pos.x + 15 - 190, m_Pos.y + 50);

		SetButtonInterface(&m_abtn[BTN_INSERT_ZEN], m_Pos.x, m_Pos.y, 7, false);
		SetButtonInterface(&m_abtn[BTN_TAKE_ZEN], m_Pos.x, m_Pos.y, 8, false);
		SetButtonInterface(&m_abtn[BTN_LOCK], m_Pos.x, m_Pos.y, 9, false);

		m_abtn[BTN_INSERT_ZEN].ChangeToolTipText(GlobalText[235], true);
		m_abtn[BTN_TAKE_ZEN].ChangeToolTipText(GlobalText[236], true);
		m_abtn[BTN_LOCK].ChangeToolTipText(GlobalText[242], true);

		if (GMProtect->IsBaulExt() == true)
		{
			SetButtonImagen(&m_BtnExpansion, BITMAP_INTERFACE, BITMAP_INTERFACE + 1);
			m_BtnExpansion.ChangeButtonInfo(x + 140, y + 395, 24, 24);
			m_BtnExpansion.ChangeToolTipText(GlobalText[3338], true);
		}
		m_ExpansionState = false;
	}
	else
	{
		int anBtnPosX[MAX_BTN] = { 13, 50, 87 };
		int anToolTipText[MAX_BTN] = { 235, 236, 242 };

		for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
		{
			m_abtn[i].ChangeButtonImgState(true, IMAGE_STORAGE_BTN_INSERT_ZEN + i);
			m_abtn[i].ChangeButtonInfo(x + anBtnPosX[i], y + 390, 36, 29);
			m_abtn[i].ChangeToolTipText(GlobalText[anToolTipText[i]], true);
		}
		if (GMProtect->IsBaulExt() == true)
		{
			m_BtnExpansion.ChangeButtonImgState(true, IMAGE_STORAGE_EXPANSION);
			m_BtnExpansion.ChangeButtonInfo(x + 124, y + 390, 36, 29);
			m_BtnExpansion.ChangeToolTipText(GlobalText[3338], true);
		}
		m_ExpansionState = false;
	}
#endif // test_main_dev




	m_bLock = false;
	SetItemAutoMove(false);
	InitBackupItemInfo();

	Show(false);

	return true;
}

void CNewUIStorageInventory::Release()
{
	UnloadImages();

	for (int i = 0; i < MAX_WHAREHOUSE_EXPANSION; i++)
		SAFE_DELETE(m_pNewInventoryCtrl[i]);

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void CNewUIStorageInventory::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;

#if MAIN_UPDATE > 603
	int anBtnPosX[MAX_BTN] = { 22, 44, 66 };

	for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
	{
		m_abtn[i].SetPos(x + anBtnPosX[i], y + 384);
	}
	if (GMProtect->IsBaulExt() == true)
	{
		m_BtnExpansion.SetPos(x + 88, y + 384);
	}
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		int anBtnPosX[MAX_BTN] = { 26, 76, 126 };

		if (GMProtect->IsBaulExt() == true)
		{
			anBtnPosX[1] = 64;
			anBtnPosX[2] = 102;
			m_BtnExpansion.SetPos(x + 140, y + 395);
		}

		for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
		{
			m_abtn[i].SetPos(m_Pos.x + anBtnPosX[i], y + 395);
		}
}
	else
	{
		int anBtnPosX[MAX_BTN] = { 13, 50, 87 };

		for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
		{
			m_abtn[i].SetPos(x + anBtnPosX[i], y + 390);
		}
		if (GMProtect->IsBaulExt() == true)
		{
			m_BtnExpansion.SetPos(x + 124, y + 390);
		}
	}
#else
	int anBtnPosX[MAX_BTN] = { 26, 76, 126 };

	if (GMProtect->IsBaulExt() == true)
	{
		anBtnPosX[1] = 64;
		anBtnPosX[2] = 102;
		m_BtnExpansion.SetPos(x + 140, y + 395);
	}

	for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
	{
		m_abtn[i].SetPos(m_Pos.x + anBtnPosX[i], y + 395);
	}
#endif
}

void SEASON3B::CNewUIStorageInventory::SetExpansionOpenState()
{
	m_ExpansionState = true;
	m_pNewInventoryCtrl[1]->UnlockInventory();
	m_pNewInventoryCtrl[1]->ShowInventory();
}

void SEASON3B::CNewUIStorageInventory::SetExpansionCloseState()
{
	m_ExpansionState = false;
	m_pNewInventoryCtrl[1]->LockInventory();
	m_pNewInventoryCtrl[1]->HideInventory();
}

void SEASON3B::CNewUIStorageInventory::LockBtnExpansion()
{
	if (GMProtect->IsBaulExt() == true)
	{
		m_BtnExpansion.Lock();
		m_BtnExpansion.ChangeImgColor(BUTTON_STATE_UP, RGBA(100, 100, 100, 255));
		m_BtnExpansion.ChangeTextColor(RGBA(100, 100, 100, 255));
	}
}

void SEASON3B::CNewUIStorageInventory::UnLockBtnExpansion()
{
	if (GMProtect->IsBaulExt() == true)
	{
		m_BtnExpansion.UnLock();
		m_BtnExpansion.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnExpansion.ChangeTextColor(RGBA(255, 255, 255, 255));
	}
}

int SEASON3B::CNewUIStorageInventory::FindInvenIndex(int index)
{
	int InvenIndex;

	if (index < 0 || index >= 120)
	{
		if (index < 120 || index >= 240)
			InvenIndex = 2;
		else
			InvenIndex = 1;
	}
	else
	{
		InvenIndex = 0;
	}
	return InvenIndex;
}

int SEASON3B::CNewUIStorageInventory::SlotCntSendDstInvenIndex(int index, int InvenIndex)
{
	if (InvenIndex > 0)
		index += 120;
	return index;
}

int SEASON3B::CNewUIStorageInventory::SlotCntSendSrcInvenIndex(int index, int InvenIndex)
{
	if (InvenIndex > 0)
		index += 120;
	return index;
}

int SEASON3B::CNewUIStorageInventory::SlotCntReceiveInvenIndex(int index)
{
	if (index <= 240)
	{
		if (index >= 120)
			index = index - 120;
	}
	return index;
}

int SEASON3B::CNewUIStorageInventory::IsInvenctrlSame(CNewUIInventoryCtrl* InventoryCtrl)
{
	for (int i = 0; i < 2; i++)
	{
		if (InventoryCtrl == m_pNewInventoryCtrl[i])
		{
			return i;
		}
	}
	return -1;
}

ITEM* SEASON3B::CNewUIStorageInventory::FindItemAtPt(int x, int y, int* InvenIndex)
{
	ITEM* pItem = NULL;
	(*InvenIndex) = 0;

	for (int i = 0; i < 2; i++)
	{
		(*InvenIndex) = i;
		pItem = m_pNewInventoryCtrl[i]->FindItemAtPt(x, y);
		if (pItem)
			break;
	}
	return pItem;
}

int SEASON3B::CNewUIStorageInventory::GetInvenSlotIndex(unsigned __int8 cx, unsigned __int8 cy, int InvenIndex)
{
	int Index = cx + 8 * cy;

	if (InvenIndex > 0)
		Index += 120;

	return Index;
}

void SEASON3B::CNewUIStorageInventory::RenderExpansionInven()
{
	if (m_pNewInventoryCtrl[1])
	{
		m_pNewInventoryCtrl[1]->Render(true);
	}
}

int SEASON3B::CNewUIStorageInventory::GetSourceStorage(int index, CNewUIInventoryCtrl* InventoryCtrl)
{
	int InvenIndex = 0;

	for (int i = 0; i < 2; i++)
	{
		if (InventoryCtrl == m_pNewInventoryCtrl[i])
			InvenIndex = i;
	}
	return SlotCntSendSrcInvenIndex(index, InvenIndex);
}

int SEASON3B::CNewUIStorageInventory::GetPickItemTarget(CNewUIPickedItem* pPickedItem)
{
	for (int i = 0; i < 2; i++)
	{
		int iTargetIndex = pPickedItem->GetTargetLinealPos(m_pNewInventoryCtrl[i]);
		if (iTargetIndex != -1)
		{
			m_InvenIndex = i;
			return iTargetIndex;
		}
	}
	return -1;
}

bool CNewUIStorageInventory::UpdateMouseEvent()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_pNewInventoryCtrl[i] && !m_pNewInventoryCtrl[i]->UpdateMouseEvent())
			return false;
	}

	ProcessInventoryCtrl();

	if (ProcessBtns())
		return false;

	if (CheckMouseIn(m_Pos.x, m_Pos.y, STORAGE_WIDTH, STORAGE_HEIGHT))
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
	else if (g_pStorageExpansion->CheckExpansionInventory())
	{
		return false;
	}

	return true;
}

bool CNewUIStorageInventory::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_STORAGE);
			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}
	return true;
}

bool CNewUIStorageInventory::Update()
{
	for (int i = 0; i < 2; i++)
	{
		if (m_pNewInventoryCtrl[i] && false == m_pNewInventoryCtrl[i]->Update())
			return false;
	}

	return true;
}

bool CNewUIStorageInventory::Render()
{
	::EnableAlphaTest();

	::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderBackImage();

	RenderText();

	if (m_pNewInventoryCtrl[0])
		m_pNewInventoryCtrl[0]->Render();

	if (GMProtect->IsBaulExt() == true)
	{
		m_BtnExpansion.Render();
	}
	for (int i = BTN_INSERT_ZEN; i < MAX_BTN; ++i)
		m_abtn[i].Render();

	::DisableAlphaBlend();

	return true;
}

void CNewUIStorageInventory::RenderBackImage()
{
	SEASON3B::window_nomal((float)m_Pos.x, (float)m_Pos.y);

#if MAIN_UPDATE > 603
	RenderImageF(BITMAP_storage_money, m_Pos.x + (WIN_WINDOW_SIZEX - 190) / 2 + 8, m_Pos.y + 338, 173.f, 48.f, 0.0, 0.0, 626, 176);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 353, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);
	}
	else
	{
		RenderImage(IMAGE_STORAGE_MONEY, m_Pos.x + 10, m_Pos.y + 342, 170.f, 46.f);
	}
#else
	RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 353, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);
#endif
}

void CNewUIStorageInventory::RenderText()
{
	unicode::t_char szTemp[128];
	int nTempZen;

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);

	unicode::_sprintf(szTemp, "%s (%s)", GlobalText[234], GlobalText[m_bLock ? 241 : 240]);
	if (m_bLock)
		g_pRenderText->SetTextColor(240, 32, 32, 255);
	else
		g_pRenderText->SetTextColor(216, 216, 216, 255);

	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 11, szTemp, WIN_WINDOW_SIZEX, 0, RT3_SORT_CENTER);

#ifdef test_main_dev
	nTempZen = CharacterMachine->StorageGold;
	ConvertGold(nTempZen, szTemp);

	g_pRenderText->SetTextColor(::getGoldColor(nTempZen));
	g_pRenderText->RenderText(m_Pos.x + 168, m_Pos.y + 342 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

	g_pRenderText->SetTextColor(240, 64, 64, 255);
	g_pRenderText->RenderText(m_Pos.x + 10 + 15, m_Pos.y + 342 + 29, GlobalText[266]);

	__int64 iTotalLevel = (__int64)CharacterAttribute->Level + Master_Level_Data.nMLevel;

	nTempZen = int(double(iTotalLevel) * double(iTotalLevel) * 0.04);
	nTempZen += m_bLock ? int(CharacterAttribute->Level) * 2 : 0;
	nTempZen = max(1, nTempZen);

	if (nTempZen >= 1000)
		nTempZen = nTempZen / 100 * 100;
	else if (nTempZen >= 100)
		nTempZen = nTempZen / 10 * 10;

	::ConvertGold(nTempZen, szTemp);
	g_pRenderText->SetTextColor(255, 220, 150, 255);
	g_pRenderText->RenderText(m_Pos.x + 168, m_Pos.y + 342 + 29, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);
#else
	if (gmProtect->checkold_school())
	{
		nTempZen = CharacterMachine->StorageGold;
		ConvertGold(nTempZen, szTemp);

		g_pRenderText->SetTextColor(::getGoldColor(nTempZen));
		g_pRenderText->RenderText(m_Pos.x + 60, m_Pos.y + 353, szTemp, 0, 18);

		g_pRenderText->SetBgColor(40, 40, 40, 255);
		g_pRenderText->SetTextColor(255, 220, 150, 255);
		g_pRenderText->RenderText(m_Pos.x + 30, m_Pos.y + 358, "Zen", 0, 0);


		g_pRenderText->SetTextColor(255, 0, 0, 255);
		g_pRenderText->RenderText(m_Pos.x + 50, m_Pos.y + 374, GlobalText[266], 0, 0);

		__int64 iTotalLevel = (__int64)CharacterAttribute->Level + Master_Level_Data.nMLevel;

		nTempZen = int(double(iTotalLevel) * double(iTotalLevel) * 0.04);
		nTempZen += m_bLock ? int(CharacterAttribute->Level) * 2 : 0;
		nTempZen = max(1, nTempZen);

		if (nTempZen >= 1000)
			nTempZen = nTempZen / 100 * 100;
		else if (nTempZen >= 100)
			nTempZen = nTempZen / 10 * 10;

		::ConvertGold(nTempZen, szTemp);
		g_pRenderText->SetTextColor(255, 220, 150, 255);
		g_pRenderText->RenderText(m_Pos.x + 120, m_Pos.y + 374, szTemp, 0, 0);
	}
	else
	{
		nTempZen = CharacterMachine->StorageGold;
		ConvertGold(nTempZen, szTemp);

		g_pRenderText->SetTextColor(::getGoldColor(nTempZen));
		g_pRenderText->RenderText(m_Pos.x + 168, m_Pos.y + 342 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

		g_pRenderText->SetTextColor(240, 64, 64, 255);
		g_pRenderText->RenderText(m_Pos.x + 10 + 15, m_Pos.y + 342 + 29, GlobalText[266]);

		__int64 iTotalLevel = (__int64)CharacterAttribute->Level + Master_Level_Data.nMLevel;

		nTempZen = int(double(iTotalLevel) * double(iTotalLevel) * 0.04);
		nTempZen += m_bLock ? int(CharacterAttribute->Level) * 2 : 0;
		nTempZen = max(1, nTempZen);

		if (nTempZen >= 1000)
			nTempZen = nTempZen / 100 * 100;
		else if (nTempZen >= 100)
			nTempZen = nTempZen / 10 * 10;

		::ConvertGold(nTempZen, szTemp);
		g_pRenderText->SetTextColor(255, 220, 150, 255);
		g_pRenderText->RenderText(m_Pos.x + 168, m_Pos.y + 342 + 29, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);
	}
#endif // test_main_dev
}

float CNewUIStorageInventory::GetLayerDepth()
{
	return 2.2f;
}

void SEASON3B::CNewUIStorageInventory::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);
	float RenderFrameY = (m_Pos.y);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, (int)RenderFrameY);

	if (m_pNewInventoryCtrl[0] != NULL && m_pNewInventoryCtrl[1] != NULL)
	{
#if MAIN_UPDATE > 603
		m_pNewInventoryCtrl[0]->SetPos(m_Pos.x + 22, m_Pos.y + 36);
		m_pNewInventoryCtrl[1]->SetPos(m_Pos.x + 22 - WIN_WINDOW_SIZEX, m_Pos.y + 36);
#else
		if (gmProtect->checkold_school())
		{
			m_pNewInventoryCtrl[0]->SetPos(m_Pos.x + 15, m_Pos.y + 50);
			m_pNewInventoryCtrl[1]->SetPos(m_Pos.x + 15 - WIN_WINDOW_SIZEX, m_Pos.y + 50);
		}
		else
		{
			m_pNewInventoryCtrl[0]->SetPos(m_Pos.x + 15, m_Pos.y + 36);
			m_pNewInventoryCtrl[1]->SetPos(m_Pos.x + 15 - WIN_WINDOW_SIZEX, m_Pos.y + 36);
		}
#endif // test_main_dev
	}
}

CNewUIInventoryCtrl* CNewUIStorageInventory::GetInventoryCtrl(int InvenIndex) const
{
	return m_pNewInventoryCtrl[InvenIndex];
}

void CNewUIStorageInventory::LoadImages()
{
#if MAIN_UPDATE > 603
	LoadBitmap("Interface\\HUD_TEST\\newui_storage_money.dds", BITMAP_storage_money, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\btn_money01.dds", BITMAP_btn_money01, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\btn_money02.dds", BITMAP_btn_money02, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\btn_openshop.dds", BITMAP_btn_lock01, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\btn_closeshop.dds", BITMAP_btn_lock02, GL_LINEAR);
	//LoadBitmap("Interface\\HUD_TEST\\btn_lock01.tga", BITMAP_btn_lock01, GL_LINEAR);
	//LoadBitmap("Interface\\HUD_TEST\\btn_lock02.tga", BITMAP_btn_lock02, GL_LINEAR);
#else
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_STORAGE_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back01.tga", IMAGE_STORAGE_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_STORAGE_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_STORAGE_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_STORAGE_BOTTOM, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_money01.tga", IMAGE_STORAGE_BTN_INSERT_ZEN, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_money02.tga", IMAGE_STORAGE_BTN_TAKE_ZEN, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_lock02.tga", IMAGE_STORAGE_BTN_UNLOCK, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_lock.tga", IMAGE_STORAGE_BTN_LOCK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_money3.tga", IMAGE_STORAGE_MONEY, GL_LINEAR);
#endif // test_main_dev
}

void CNewUIStorageInventory::UnloadImages()
{
#if MAIN_UPDATE > 603
	DeleteBitmap(BITMAP_storage_money);
	DeleteBitmap(BITMAP_btn_money01);
	DeleteBitmap(BITMAP_btn_money02);
	DeleteBitmap(BITMAP_btn_lock01);
	DeleteBitmap(BITMAP_btn_lock02);
#else
	DeleteBitmap(IMAGE_STORAGE_MONEY);
	DeleteBitmap(IMAGE_STORAGE_BTN_LOCK);
	DeleteBitmap(IMAGE_STORAGE_BTN_UNLOCK);
	DeleteBitmap(IMAGE_STORAGE_BTN_TAKE_ZEN);
	DeleteBitmap(IMAGE_STORAGE_BTN_INSERT_ZEN);
	DeleteBitmap(IMAGE_STORAGE_BOTTOM);
	DeleteBitmap(IMAGE_STORAGE_RIGHT);
	DeleteBitmap(IMAGE_STORAGE_LEFT);
	DeleteBitmap(IMAGE_STORAGE_TOP);
	DeleteBitmap(IMAGE_STORAGE_BACK);
#endif // test_main_dev
}

void CNewUIStorageInventory::LockStorage(bool bLock)
{
	m_bLock = bLock;
	ChangeLockBtnImage();
}

void SEASON3B::CNewUIStorageInventory::SetCorrectPassword(bool bCorrectPassword)
{
	m_bCorrectPassword = bCorrectPassword;
}

void CNewUIStorageInventory::ChangeLockBtnImage()
{
#if MAIN_UPDATE > 603
	m_abtn[BTN_LOCK].UnRegisterButtonState();
	if (m_bLock)
		m_abtn[BTN_LOCK].ChangeButtonImgState(true, BITMAP_btn_lock01, false, false, false, 0.0, 0.0, 69.f, 72.f);
	else
		m_abtn[BTN_LOCK].ChangeButtonImgState(true, BITMAP_btn_lock02, false, false, false, 0.0, 0.0, 69.f, 72.f);
#else
	if (gmProtect->checkold_school())
	{
		if (m_bLock)
		{
			SetButtonInterface(&m_abtn[BTN_LOCK], m_Pos.x, m_Pos.y, 10, false);
		}
		else
		{
			SetButtonInterface(&m_abtn[BTN_LOCK], m_Pos.x, m_Pos.y, 9, false);
		}
	}
	else
	{
		m_abtn[BTN_LOCK].UnRegisterButtonState();
		if (m_bLock)
			m_abtn[BTN_LOCK].ChangeButtonImgState(true, IMAGE_STORAGE_BTN_LOCK);
		else
			m_abtn[BTN_LOCK].ChangeButtonImgState(true, IMAGE_STORAGE_BTN_UNLOCK);
	}
#endif // test_main_dev
}

bool CNewUIStorageInventory::ClosingProcess()
{
	if (EquipmentItem)
		return false;

	SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
	DeleteAllItems();
	SendRequestStorageExit();
	return true;
}

bool CNewUIStorageInventory::InsertItem(int nIndex, BYTE* pbyItemPacket)
{
	int invIndex = FindInvenIndex(nIndex);
	nIndex = SlotCntReceiveInvenIndex(nIndex);

	if (m_pNewInventoryCtrl[invIndex])
		return m_pNewInventoryCtrl[invIndex]->AddItem(nIndex, pbyItemPacket);

	return false;
}

void CNewUIStorageInventory::DeleteAllItems()
{
	for (int i = 0; i < 2; ++i)
	{
		if (m_pNewInventoryCtrl[i])
			m_pNewInventoryCtrl[i]->RemoveAllItems();
	}
}

void CNewUIStorageInventory::ProcessInventoryCtrl()
{
	if (CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, STORAGE_HEIGHT) == false)
	{
		if (!g_pStorageExpansion->CheckExpansionInventory())
			return;
	}

	for (int i = 0; i < 2; ++i)
	{
		if (m_pNewInventoryCtrl[i] == NULL)
			return;
	}

	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	if (pPickedItem)
	{
		ITEM* pItemObj = pPickedItem->GetItem();
		if (NULL == pItemObj)
			return;

		if (SEASON3B::IsPress(VK_LBUTTON))
		{
			int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

			if (InvenIndex == -1)
			{
				if (IsInvenctrlSame(pPickedItem->GetOwnerInventory()) == -1)
				{
					if (pItemObj->ex_src_type == ITEM_EX_SRC_EQUIPMENT)
					{
						int nSrcIndex = pPickedItem->GetSourceLinealPos();
						int nDstIndex = GetPickItemTarget(pPickedItem);

						if (nDstIndex != -1 && m_pNewInventoryCtrl[m_InvenIndex]->CanMove(nDstIndex, pItemObj))
						{
							nSrcIndex = GetSourceStorage(nSrcIndex, pPickedItem->GetOwnerInventory());

							nDstIndex = SlotCntSendSrcInvenIndex(nDstIndex, m_InvenIndex);
							SendRequestItemToStorage(pItemObj, nSrcIndex, nDstIndex);
						}
					}
				}
				else
				{
					int nSrcIndex = pPickedItem->GetSourceLinealPos();
					int nDstIndex = GetPickItemTarget(pPickedItem);
					if (nDstIndex != -1 && m_pNewInventoryCtrl[m_InvenIndex]->CanMove(nDstIndex, pItemObj))
					{
						nSrcIndex = GetSourceStorage(nSrcIndex, pPickedItem->GetOwnerInventory());
						nDstIndex = SlotCntSendSrcInvenIndex(nDstIndex, m_InvenIndex);
						SendRequestEquipmentItem(REQUEST_EQUIPMENT_STORAGE, nSrcIndex, pItemObj, REQUEST_EQUIPMENT_STORAGE, nDstIndex);
					}
				}
			}
			else
			{
				int nSrcIndex = pPickedItem->GetSourceLinealPos();
				nSrcIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(nSrcIndex, InvenIndex);

				int nDstIndex = GetPickItemTarget(pPickedItem);
				if (nDstIndex != -1 && m_pNewInventoryCtrl[m_InvenIndex]->CanMove(nDstIndex, pItemObj))
				{
					nDstIndex = SlotCntSendSrcInvenIndex(nDstIndex, m_InvenIndex);
					SendRequestItemToStorage(pItemObj, MAX_EQUIPMENT_INDEX + nSrcIndex, nDstIndex);
				}
			}
		}
		else
		{
#ifdef LEM_ADD_LUCKYITEM
			if (::IsStoreBan(pItemObj))
				m_pNewInventoryCtrl->SetSquareColorNormal(1.0f, 0.0f, 0.0f);
			else
				m_pNewInventoryCtrl->SetSquareColorNormal(0.1f, 0.4f, 0.8f);
#endif // LEM_ADD_LUCKYITEM
		}
	}
	else if (SEASON3B::IsPress(VK_RBUTTON))
	{
		ProcessStorageItemAutoMove();
	}
}

void CNewUIStorageInventory::ProcessStorageItemAutoMove()
{
	if (g_pPickedItem)
		if (g_pPickedItem->GetItem())
			return;

	if (IsItemAutoMove())
		return;

	int InvenIndex;
	ITEM* pItemObj = FindItemAtPt(MouseX, MouseY, &InvenIndex);

	if (pItemObj)
	{
		int nDstIndex = g_pMyInventory->FindEmptySlot(pItemObj);
		if (-1 != nDstIndex)
		{
			SetItemAutoMove(true);

			int nSrcIndex = GetInvenSlotIndex(pItemObj->x, pItemObj->y, InvenIndex);
			SendRequestItemToMyInven(pItemObj, nSrcIndex, MAX_EQUIPMENT_INDEX + nDstIndex);
			::PlayBuffer(SOUND_GET_ITEM01);
		}
	}
}

void CNewUIStorageInventory::ProcessMyInvenItemAutoMove()
{
	if (g_pPickedItem)
		if (g_pPickedItem->GetItem())
			return;

	if (IsItemAutoMove())
		return;

	int InvenIndex = -1;

	ITEM* pItemObj = g_pMyInventory->FindItemAtPt(MouseX, MouseY, &InvenIndex);

	if (pItemObj)
	{
		if (pItemObj->Type == ITEM_HELPER + 20)
			return;

		int nDstIndex = FindEmptySlot(pItemObj);

		if (-1 != nDstIndex)
		{
			SetItemAutoMove(true);

			int nSrcIndex = g_pMyInventory->GetInvenSlotIndex(pItemObj->x, pItemObj->y, InvenIndex);
			SendRequestItemToStorage(pItemObj, nSrcIndex + MAX_EQUIPMENT_INDEX, nDstIndex);
			::PlayBuffer(SOUND_GET_ITEM01);
		}
	}

}

void CNewUIStorageInventory::SendRequestItemToMyInven(ITEM* pItemObj, int nStorageIndex, int nInvenIndex)
{
	if (!IsLockStorage() || IsCorrectPassword())
	{
		SendRequestEquipmentItem(REQUEST_EQUIPMENT_STORAGE, nStorageIndex, pItemObj, REQUEST_EQUIPMENT_INVENTORY, nInvenIndex);

	}
	else
	{
		SetBackupInvenIndex(nInvenIndex);
		if (!IsItemAutoMove())
			g_pPickedItem->HidePickedItem();

		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPasswordKeyPadMsgBoxLayout));
	}
}

void CNewUIStorageInventory::SendRequestItemToStorage(ITEM* pItemObj, int nInvenIndex, int nStorageIndex)
{
	if (::IsStoreBan(pItemObj))
	{
#ifdef KJH_PBG_ADD_INGAMESHOP_SYSTEM
		// MessageBox
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[3028], GlobalText[667]);
#endif // KJH_PBG_ADD_INGAMESHOP_SYSTEM

		g_pChatListBox->AddText("", GlobalText[667], SEASON3B::TYPE_ERROR_MESSAGE);
		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

		if (IsItemAutoMove())
			SetItemAutoMove(false);
	}
	else
	{
		SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, nInvenIndex, pItemObj, REQUEST_EQUIPMENT_STORAGE, nStorageIndex);
	}
}

bool CNewUIStorageInventory::ProcessBtns()
{
	if (m_abtn[BTN_INSERT_ZEN].UpdateMouseEvent())
	{
		SEASON3B::CreateMessageBox(
			MSGBOX_LAYOUT_CLASS(SEASON3B::CZenReceiptMsgBoxLayout));
		return true;
	}
	else if (m_abtn[BTN_TAKE_ZEN].UpdateMouseEvent())
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CZenPaymentMsgBoxLayout));
		return true;
	}
	else if (m_abtn[BTN_LOCK].UpdateMouseEvent())
	{
		if (m_bLock)
		{
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CStorageUnlockMsgBoxLayout));
		}
		else
		{
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CStorageLockKeyPadMsgBoxLayout));
		}
		return true;
	}
	else if (m_BtnExpansion.UpdateMouseEvent() && GMProtect->IsBaulExt())
	{
		if (m_ExpansionState)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_STORAGE_EXTENSION);
			SetExpansionCloseState();
		}
		else
		{
			g_pNewUISystem->Show(SEASON3B::INTERFACE_STORAGE_EXTENSION);
			SetExpansionOpenState();
		}
		return true;
	}
	else if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 169, m_Pos.y + 7, 13, 12))
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_STORAGE);
		return true;
	}
	return false;
}

void CNewUIStorageInventory::SetItemAutoMove(bool bItemAutoMove)
{
	m_bItemAutoMove = bItemAutoMove;

	if (bItemAutoMove)
	{
		m_nBackupMouseX = MouseX;
		m_nBackupMouseY = MouseY;
	}
	else
	{
		m_nBackupMouseX = 0;
		m_nBackupMouseY = 0;
	}
}

void CNewUIStorageInventory::InitBackupItemInfo()
{
	m_bTakeZen = false;
	m_nBackupTakeZen = 0;
	m_nBackupInvenIndex = -1;
}

int SEASON3B::CNewUIStorageInventory::GetBackupTakeZen()
{
	return m_nBackupTakeZen;
}

void CNewUIStorageInventory::SetBackupTakeZen(int nZen)
{
	m_bTakeZen = true;
	m_nBackupTakeZen = nZen;
}

void CNewUIStorageInventory::SetBackupInvenIndex(int nInvenIndex)
{
	m_bTakeZen = false;
	m_nBackupInvenIndex = nInvenIndex;
}

int SEASON3B::CNewUIStorageInventory::GetBackupInvenIndex()
{
	return m_nBackupInvenIndex;
}

int CNewUIStorageInventory::FindEmptySlot(ITEM* pItemObj)
{
	int Index = -1;

	if (pItemObj)
	{
		Script_Item* pItemAttr = GMItemMng->find(pItemObj->Type);

		for (int i = 0; i < 2; i++)
		{
			if (m_pNewInventoryCtrl[i])
			{
				int Index = m_pNewInventoryCtrl[i]->FindEmptySlot(pItemAttr->Width, pItemAttr->Height);

				if (Index != -1)
					return this->SlotCntSendSrcInvenIndex(Index, i);

				if (g_pStorageExpansion->IsExpansionStorage() || i)
				{
					if (i == 1)
					{
						g_pChatListBox->AddText("", GlobalText[3342], SEASON3B::TYPE_SYSTEM_MESSAGE);
					}
				}
				else
				{
					g_pChatListBox->AddText("", GlobalText[3343], SEASON3B::TYPE_SYSTEM_MESSAGE);
				}
			}
		}
	}

	return Index;
}

void CNewUIStorageInventory::ProcessToReceiveStorageStatus(BYTE byStatus)
{
	switch (byStatus)
	{
	case 0:
		LockStorage(false);
		SetCorrectPassword(false);
		break;
	case 1:
		LockStorage(true);
		SetCorrectPassword(false);
		break;
	case 10:
		SEASON3B::CreateOkMessageBox(GlobalText[440]);
		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
		ProcessStorageItemAutoMoveFailure();
		break;
	case 11:
		SEASON3B::CreateOkMessageBox(GlobalText[441]);
		break;
	case 12:
		if (IsLockStorage() && !IsCorrectPassword())
		{
			if (m_bTakeZen)
			{
				SendRequestStorageGold(1, GetBackupTakeZen());
				InitBackupItemInfo();
			}
			else
			{
				ITEM* pItemObj = NULL;
				int nStorageIndex = -1;

				if (IsItemAutoMove())
				{
					int InvenIndex = -1;

					pItemObj = FindItemAtPt(m_nBackupMouseX, m_nBackupMouseY, &InvenIndex);

					if (pItemObj)
					{
						nStorageIndex = GetInvenSlotIndex(pItemObj->x, pItemObj->y, InvenIndex);
					}
				}
				else
				{
					nStorageIndex = g_pPickedItem->GetSourceLinealPos();
					pItemObj = g_pPickedItem->GetItem();
				}
				if (nStorageIndex != -1 && pItemObj)
				{
					SendRequestEquipmentItem(REQUEST_EQUIPMENT_STORAGE, nStorageIndex, pItemObj, REQUEST_EQUIPMENT_INVENTORY, GetBackupInvenIndex());
				}
				InitBackupItemInfo();
			}
		}
		LockStorage(true);
		SetCorrectPassword(true);
		break;
	case 13:
		SEASON3B::CreateOkMessageBox(GlobalText[401]);
		break;
	}
}

void CNewUIStorageInventory::ProcessToReceiveStorageItems(int nIndex, BYTE* pbyItemPacket)
{
	SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

	if (nIndex >= 0 && nIndex < 240)
	{
		if (this->IsItemAutoMove())
		{
			int InvenIndex = -1;
			ITEM* pItemObj = g_pMyInventory->FindItemAtPt(m_nBackupMouseX, m_nBackupMouseY, &InvenIndex);

			if (pItemObj)
			{
				g_pMyInventory->GetInventoryCtrl(InvenIndex)->RemoveItem(pItemObj);
			}
			this->SetItemAutoMove(false);
		}
		this->InsertItem(nIndex, pbyItemPacket);
	}
}

void CNewUIStorageInventory::ProcessStorageItemAutoMoveSuccess()
{
	if (!IsVisible())
		return;

	if (this->IsItemAutoMove())
	{
		int InvenIndex = -1;

		ITEM* pItemObj = FindItemAtPt(m_nBackupMouseX, m_nBackupMouseY, &InvenIndex);
		if(pItemObj)
			m_pNewInventoryCtrl[InvenIndex]->RemoveItem(pItemObj);
		this->SetItemAutoMove(false);
	}
}

void CNewUIStorageInventory::ProcessStorageItemAutoMoveFailure()
{
	if (!IsVisible())
		return;

	this->InitBackupItemInfo();
	this->SetItemAutoMove(false);
}

int SEASON3B::CNewUIStorageInventory::GetPointedItemIndex()
{
	int Index = -1;
	for (int i = 0; i < 2; ++i)
	{
		Index = m_pNewInventoryCtrl[i]->GetPointedSquareIndex();
		if (Index != -1)
			return SlotCntSendSrcInvenIndex(Index, i);
	}
	return Index;
}
