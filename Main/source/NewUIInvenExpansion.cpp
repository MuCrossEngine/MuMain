#include "stdafx.h"
#include "DSPlaySound.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "NewUIInvenExpansion.h"

SEASON3B::CInvenOrgInterface::CInvenOrgInterface()
{
	this->InterfaceList = 0;
	this->posX = 0.0f;
	this->posY = 0.0f;
	this->myPosX = 0.0f;
	this->myPosY = 0.0f;
}

SEASON3B::CInvenOrgInterface::~CInvenOrgInterface()
{
}

void SEASON3B::CInvenOrgInterface::SetOrgInvenPos(int InterfaceList, float posX, float posY, float myPosX, float myPosY)
{
	this->InterfaceList = InterfaceList;
	this->posX = posX;
	this->posY = posY;
	this->myPosX = myPosX;
	this->myPosY = myPosY;
}

float SEASON3B::CInvenOrgInterface::GetOrgInvenPosX()
{
	return this->posX;
}

float SEASON3B::CInvenOrgInterface::GetOrgInvenPosY()
{
	return this->posY;
}

float SEASON3B::CInvenOrgInterface::GetOrgYourInvenPosX()
{
	return this->myPosX;
}

float SEASON3B::CInvenOrgInterface::GetOrgYourInvenPosY()
{
	return this->myPosY;
}

int SEASON3B::CInvenOrgInterface::GetInterfaceList()
{
	return this->InterfaceList;
}

SEASON3B::CNewUIInvenExpansion::CNewUIInvenExpansion()
{
	m_pNewUIMng = NULL;

	for (int i = 0; i < MAX_INVENT_EXPANSION; ++i)
	{
		m_InvBoxPos[i].x = 0;
		m_InvBoxPos[i].y = 0;
		isExtEnabled[i] = false;
	}

	m_Pos.x = 0;
	m_Pos.y = 0;

	ThirdInterface = false;

	InvenOrgInterface.clear();

	m_List.clear();

	RegLinkageUI();
}

SEASON3B::CNewUIInvenExpansion::~CNewUIInvenExpansion()
{
	Release();
}

bool SEASON3B::CNewUIInvenExpansion::Create(CNewUIManager* pUIManager, int posX, int posY)
{
	if (pUIManager && g_pNewItemMng)
	{
		m_pNewUIMng = pUIManager;

		m_pNewUIMng->AddUIObj(INTERFACE_INVENTORY_EXTENSION, this);

#ifdef test_main_dev
		for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
		{
			m_InvBoxPos[i].x = posX + 22;
			m_InvBoxPos[i].y = posY + 87 * i + 46;
		}
#else
		for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
		{
			m_InvBoxPos[i].x = posX + 14;
			m_InvBoxPos[i].y = posY + 87 * i + 46;
		}
#endif // test_main_dev

		SetButtonInfo();

		SetPos(posX, posY);

		LoadImages();

		Show(false);

		return true;
	}

	return false;
}

void SEASON3B::CNewUIInvenExpansion::Release()
{
	InvenOrgInterface.clear();

	m_List.clear();

	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
	}
}

void SEASON3B::CNewUIInvenExpansion::SetPos(int posX, int posY)
{
	m_Pos.x = posX;
	m_Pos.y = posY;

#if MAIN_UPDATE > 603
	m_BtnExit.SetPos(m_Pos.x + 22, m_Pos.y + 384);

	for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
	{
		m_InvBoxPos[i].x = m_Pos.x + 22;
		m_InvBoxPos[i].y = m_Pos.y + 87 * i + 36;
	}
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		SetButtonPosition(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
	}
	else
	{
		m_BtnExit.SetPos(m_Pos.x + 13, m_Pos.y + 391);
	}

	for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
	{
		m_InvBoxPos[i].x = posX + 14;
		m_InvBoxPos[i].y = posY + 87 * i + 46;
	}
#else
	SetButtonPosition(&m_BtnExit, m_Pos.x, m_Pos.y, 1);

	for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
	{
		m_InvBoxPos[i].x = posX + 14;
		m_InvBoxPos[i].y = posY + 87 * i + 46;
	}
#endif


	g_pMyInventory->SetInvenPos();

	if (IsVisible() == false)
		return;

	for (int i = 1; i < g_pMyInventory->GetInvenEnableCnt(); i++)
	{
		g_pMyInventory->UnlockInventory(i);

		g_pMyInventory->GetInventoryCtrl(i)->ShowInventory();
	}
}

void SEASON3B::CNewUIInvenExpansion::SetButtonInfo()
{
	unicode::t_char pszText[100];

	unicode::_sprintf(pszText, GlobalText[927], "K");

	m_BtnExit.ChangeToolTipText(pszText, true);


#if MAIN_UPDATE > 603
	m_BtnExit.ChangeButtonImgState(true, BITMAP_btn_exit_00, false, false, false, 0.f, 0.f, 69.f, 72.f);
	m_BtnExit.ChangeButtonInfo(m_Pos.x + 22, m_Pos.y + 384, 20, 20);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
	}
	else
	{
		m_BtnExit.ChangeButtonImgState(true, CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN, false);
		m_BtnExit.ChangeButtonInfo(m_Pos.x + 13, m_Pos.y + 391, 36, 29);
	}

	m_BtnPurchase.ChangeButtonImgState(true, IMAGE_STORAGE_BTN_LOCK);
	m_BtnPurchase.ChangeButtonInfo(m_Pos.x, m_Pos.y, 36, 29);
#else
	SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
	m_BtnPurchase.ChangeButtonImgState(true, IMAGE_STORAGE_BTN_LOCK);
	m_BtnPurchase.ChangeButtonInfo(m_Pos.x, m_Pos.y, 36, 29);
#endif
}

bool SEASON3B::CNewUIInvenExpansion::Render()
{
	EnableAlphaTest();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderFrame();

	RenderTexts();

	RenderInvenExpansion();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIInvenExpansion::Update()
{
	return true;
}

bool SEASON3B::CNewUIInvenExpansion::UpdateMouseEvent()
{
	bool success = false;

	if (BtnProcess() == true)
		return false;

	if (CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, 429))
	{

		/*if (IsPress(VK_RBUTTON))
		{
			ResetRButton();
		}
		else
		{
			if (!g_pMyInventory->GetRepairMode())
				ResetLButton();
		}*/
	}

	return true;
}

bool SEASON3B::CNewUIInvenExpansion::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(INTERFACE_INVENTORY_EXTENSION) && IsPress(VK_ESCAPE))
	{
		if (!g_pNPCShop->IsSellingItem())
		{
			g_pNewUISystem->Hide(INTERFACE_INVENTORY);
			g_pNewUISystem->Hide(INTERFACE_INVENTORY_EXTENSION);
			PlayBuffer(SOUND_CLICK01);
		}
	}
	return true;
}

float SEASON3B::CNewUIInvenExpansion::GetLayerDepth()
{
	return 4.25;
}

bool SEASON3B::CNewUIInvenExpansion::BtnProcess()
{
	int PosX = GetPos()->x + 169;

	int PosY = GetPos()->y + 7;

	if (IsRelease(VK_LBUTTON) == true && CheckMouseIn(PosX, PosY, 13, 12) == true)
	{
		g_pNewUISystem->Hide(INTERFACE_INVENTORY_EXTENSION);
		return true;
	}

	if (m_BuyExpansionActive && m_BtnPurchase.UpdateMouseEvent())
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBuyInvExtRequestMsgBoxLayout), &pMsgBox);

		unicode::t_char buytext[255];

		unicode::_sprintf(buytext, GlobalText[3185], m_PurchaseCoin);

		pMsgBox->AddMsg(buytext);

		PlayBuffer(SOUND_CLICK01);

		return true;
	}

	if (m_BtnExit.UpdateMouseEvent())
	{
		g_pNewUISystem->Hide(INTERFACE_INVENTORY_EXTENSION);
		return true;
	}

	return false;
}

void SEASON3B::CNewUIInvenExpansion::ResetLButton()
{
	MouseLButton = false;

	MouseLButtonPop = false;

	MouseLButtonPush = false;
}

void SEASON3B::CNewUIInvenExpansion::ResetRButton()
{
	MouseRButton = false;

	MouseRButtonPop = false;

	MouseRButtonPush = false;
}

bool SEASON3B::CNewUIInvenExpansion::CheckExpansionInventory()
{
	if (IsVisible())
		return CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, 429) != false;
	else
		return false;
}

void SEASON3B::CNewUIInvenExpansion::InitExpansion()
{
	m_BuyExpansionActive = false;

	for (int i = 0; i < MAX_INVENT_EXPANSION; i++)
	{
		isExtEnabled[i] = false;
	}
}

void SEASON3B::CNewUIInvenExpansion::LoadImages()
{
	LoadBitmap("Interface\\newui_item_add_table.tga", item_add_table, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\newui_item_add_marking_non.JPG", item_add_marking_non, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\newui_item_add_marking_no01.tga", item_add_marking_no01, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\newui_item_add_marking_no02.tga", item_add_marking_no02, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\newui_item_add_marking_no03.tga", item_add_marking_no03, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Interface\\newui_item_add_marking_no04.tga", item_add_marking_no04, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
}

void SEASON3B::CNewUIInvenExpansion::UnloadImages()
{
	for (int i = item_add_table; i <= item_add_marking_no04; ++i)
	{
		DeleteBitmap(i);
	}
}

void SEASON3B::CNewUIInvenExpansion::RenderFrame()
{
	SEASON3B::window_nomal(m_Pos.x, m_Pos.y, 3);
}

void SEASON3B::CNewUIInvenExpansion::RenderTexts()
{
	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->SetBgColor(0, 0, 0, 0);

	g_pRenderText->SetTextColor(255, 255, 255, 0xFFu);

#ifdef test_main_dev
	g_pRenderText->RenderText(GetPos()->x + ((WIN_WINDOW_SIZEX - 84) / 2), GetPos()->y + 9, GlobalText[3323], 84, 0, 3, 0);

	if (m_BuyExpansionActive && m_CheckBuyExpansion)
	{
		unicode::t_char buytext[255];

		unicode::_sprintf(buytext, GlobalText[3184], m_PurchaseCoin);

		g_pRenderText->SetTextColor(255, 189, 25, 0xFFu);

		g_pRenderText->RenderText(GetPos()->x + ((WIN_WINDOW_SIZEX - 84) / 2), GetPos()->y + 19, buytext, 84, 0, 3, 0);
	}
#else
	g_pRenderText->RenderText(GetPos()->x, GetPos()->y + 15, GlobalText[3323], WIN_WINDOW_SIZEX, 0, 3, 0);

	if (m_BuyExpansionActive && m_CheckBuyExpansion)
	{
		unicode::t_char buytext[255];

		unicode::_sprintf(buytext, GlobalText[3184], m_PurchaseCoin);

		g_pRenderText->SetTextColor(255, 189, 25, 0xFFu);

		g_pRenderText->RenderText(GetPos()->x, GetPos()->y + 28, buytext, WIN_WINDOW_SIZEX, 0, 3, 0);
	}
#endif // test_main_dev
}

void SEASON3B::CNewUIInvenExpansion::RenderButtons()
{
	m_BtnExit.Render();
}

void SEASON3B::CNewUIInvenExpansion::RenderInvenExpansion()
{
	g_pMyInventory->RenderExpansionInven();

	bool onlyRender = false;

	m_CheckBuyExpansion = false;

	for (size_t i = 0; i < MAX_INVENT_EXPANSION && i < gmProtect->MaxInventoryExtension; i++)
	{
#ifdef test_main_dev
		if (isExtEnabled[i] != false)
		{
			continue;
		}

		RenderImage(item_add_table, (float)(m_InvBoxPos[i].x - 4), (float)(m_InvBoxPos[i].y - 3), 173.0, 87.0);
#else
		RenderImage(item_add_table, (float)(m_InvBoxPos[i].x - 4), (float)(m_InvBoxPos[i].y - 3), 173.0, 87.0);

		if (isExtEnabled[i] != false)
		{
			continue;
		}
#endif // test_main_dev

		RenderImage(item_add_marking_non, (float)m_InvBoxPos[i].x, (float)m_InvBoxPos[i].y, 161.0, 81.0);


		if (m_BuyExpansionActive && onlyRender == false)
		{
			if (CheckMouseIn(m_InvBoxPos[i].x, m_InvBoxPos[i].y, 161, 81))
			{
				m_CheckBuyExpansion = true;
			}

			onlyRender = true;

			m_BtnPurchase.SetPos(m_InvBoxPos[i].x + 62, m_InvBoxPos[i].y + 26);
			m_BtnPurchase.Render();
		}
		else
		{
			RenderImage(i + item_add_marking_no01, (float)(m_InvBoxPos[i].x + 68), (float)(m_InvBoxPos[i].y + 26), 25.0, 28.0);
		}
	}
}

void SEASON3B::CNewUIInvenExpansion::OpeningProcess()
{
	g_pMyInventory->SetExpansionOpenState();
}

void SEASON3B::CNewUIInvenExpansion::ClosingProcess()
{
	g_pMyInventory->SetExpansionCloseState();

	InitMultiPosition();

	g_pMyInventory->SetInvenPos();
}

void SEASON3B::CNewUIInvenExpansion::InitMultiPosition()
{
	float WinOpen0 = pos_right(640.f);
	float WinOpen1 = (WinOpen0 - WIN_WINDOW_SIZEX);
	float WinOpen2 = (WinOpen1 - WIN_WINDOW_SIZEX);
	float WinOpen3 = (WinOpen2 - WIN_WINDOW_SIZEX);

	g_pMyShopInventory->SetPos(WinOpen2, 0);

	g_pInventoryJewel->SetPos(WinOpen2, 0);

	int PosY = this->GetUIPosition(INTERFACE_MYSHOP_INVENTORY, 0)->y;

	int PosX = this->GetUIPosition(INTERFACE_MYSHOP_INVENTORY, 0)->x + WinOpen2;

	g_pMyShopInventory->GetInventoryCtrl()->SetPos(PosX, PosY);

	g_pGoblinMixExpansion->SetPos(WinOpen2, 0);

	g_pMixInventory->SetPos(WinOpen2, 0);

	PosY = this->GetUIPosition(INTERFACE_MIXINVENTORY, 0)->y;

	PosX = this->GetUIPosition(INTERFACE_MIXINVENTORY, 0)->x + WinOpen2;

	g_pMixInventory->GetInventoryCtrl()->SetPos(PosX, PosY);

	g_pNPCShop->SetPos(WinOpen2, 0);

	PosY = this->GetUIPosition(INTERFACE_NPCSHOP, 0)->y;

	PosX = this->GetUIPosition(INTERFACE_NPCSHOP, 0)->x + WinOpen2;

	g_pNPCShop->GetInventoryCtrl()->SetPos(PosX, PosY);

	g_pPurchaseShopInventory->SetPos(WinOpen2, 0);

	PosY = this->GetUIPosition(INTERFACE_PURCHASESHOP_INVENTORY, 0)->y;

	PosX = this->GetUIPosition(INTERFACE_PURCHASESHOP_INVENTORY, 0)->x + WinOpen2;

	g_pPurchaseShopInventory->GetInventoryCtrl()->SetPos(PosX, PosY);

	//g_pLuckyItemWnd->SetPos(WinOpen2, 0);

	//PosY = this->GetUIPosition(INTERFACE_LUCKYITEMWND, 0)->y;

	//PosX = this->GetUIPosition(INTERFACE_LUCKYITEMWND, 0)->x + WinOpen2;

	//g_pLuckyItemWnd->GetInventoryCtrl()->SetPos(PosX, PosY);

	g_pStorageInventory->SetPos(WinOpen2, 0);

	PosY = this->GetUIPosition(INTERFACE_STORAGE, 0)->y;

	PosX = this->GetUIPosition(INTERFACE_STORAGE, 0)->x + WinOpen2;

	g_pStorageInventory->GetInventoryCtrl(0)->SetPos(PosX, PosY);

	g_pTrade->SetPos(WinOpen2, 0);

	PosY = this->GetUIPosition(INTERFACE_TRADE, 1)->y;

	PosX = this->GetUIPosition(INTERFACE_TRADE, 1)->x + WinOpen2;

	g_pTrade->GetYourInvenCtrl()->SetPos(PosX, PosY);

	PosY = this->GetUIPosition(INTERFACE_TRADE, 0)->y;

	PosX = this->GetUIPosition(INTERFACE_TRADE, 0)->x + WinOpen2;

	g_pTrade->GetMyInvenCtrl()->SetPos(PosX, PosY);

	g_pInventoryCore->SetPos(WinOpen2, 0);

	this->ThirdInterface = false;

	this->SetPos(WinOpen2, 0);
}

bool SEASON3B::CNewUIInvenExpansion::SetThirdPosition(DWORD winIndex)
{
	float WinOpen0 = pos_right(640.f);
	float WinOpen1 = (WinOpen0 - WIN_WINDOW_SIZEX);
	float WinOpen2 = (WinOpen1 - WIN_WINDOW_SIZEX);
	float WinOpen3 = (WinOpen2 - WIN_WINDOW_SIZEX);

	SetPos(WinOpen2, 0);

	int ThirdIndex = IsThirdInterface(winIndex);

	switch (ThirdIndex)
	{
	case INTERFACE_TRADE:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pTrade->SetPos(WinOpen3, 0);
		g_pTrade->GetMyInvenCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		g_pTrade->GetYourInvenCtrl()->SetPos(GetUIPosition(ThirdIndex, 1)->x + WinOpen3, GetUIPosition(ThirdIndex, 1)->y);
		break;
	case INTERFACE_STORAGE:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pStorageInventory->SetPos(WinOpen3, 0);
		g_pStorageInventory->GetInventoryCtrl(0)->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_MIXINVENTORY:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pMixInventory->SetPos(WinOpen3, 0);
		g_pMixInventory->GetInventoryCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_MIX_EXPANSION:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pGoblinMixExpansion->SetPos(WinOpen3, 0);
		break;
	case INTERFACE_NPCSHOP:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pNPCShop->SetPos(WinOpen3, 0);
		g_pNPCShop->GetInventoryCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_MYSHOP_INVENTORY:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pMyShopInventory->SetPos(WinOpen3, 0);
		g_pMyShopInventory->GetInventoryCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_PURCHASESHOP_INVENTORY:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pPurchaseShopInventory->SetPos(WinOpen3, 0);
		g_pPurchaseShopInventory->GetInventoryCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_CHARACTER:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		SetPos(WinOpen3, 0);
		break;
	case INTERFACE_LUCKYITEMWND:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		//g_pLuckyItemWnd->SetPos(WinOpen3, 0);
		//g_pLuckyItemWnd->GetInventoryCtrl()->SetPos(GetUIPosition(ThirdIndex, 0)->x + WinOpen3, GetUIPosition(ThirdIndex, 0)->y);
		break;
	case INTERFACE_INVENTORY_JEWEL:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pInventoryJewel->SetPos(WinOpen3, 0);
		break;
	case INTERFACE_INVENTORY_CORE:
		g_pNewUIMng->ShowInterface(INTERFACE_INVENTORY);
		g_pNewUIMng->ShowInterface(ThirdIndex);
		g_pInventoryCore->SetPos(WinOpen3, 0);
		break;
	default:
		return false;
	}

	return true;
}

int SEASON3B::CNewUIInvenExpansion::IsThirdInterface(int winIndex)
{
	return FindLinkageUI(winIndex);
}

void SEASON3B::CNewUIInvenExpansion::RegLinkageUI()
{

#if MAIN_UPDATE > 603
	float RenderPosX = 22.f;
#else
	float RenderPosX = 15.f;
#endif // test_main_dev

	if (gmProtect->checkold_school())
	{
		//-- store
		myOrgInterface.SetOrgInvenPos(INTERFACE_MYSHOP_INVENTORY, RenderPosX, 110.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- warehouse
		myOrgInterface.SetOrgInvenPos(INTERFACE_STORAGE, RenderPosX, 50.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- trade
		myOrgInterface.SetOrgInvenPos(INTERFACE_TRADE, RenderPosX, 270.0, RenderPosX, 70.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- store others
		myOrgInterface.SetOrgInvenPos(INTERFACE_PURCHASESHOP_INVENTORY, RenderPosX, 110.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());
	}
	else
	{
		//-- store
		myOrgInterface.SetOrgInvenPos(INTERFACE_MYSHOP_INVENTORY, RenderPosX, 90.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- warehouse
		myOrgInterface.SetOrgInvenPos(INTERFACE_STORAGE, RenderPosX, 36.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- trade
		myOrgInterface.SetOrgInvenPos(INTERFACE_TRADE, RenderPosX, 274.0, RenderPosX, 68.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());

		//-- store others
		myOrgInterface.SetOrgInvenPos(INTERFACE_PURCHASESHOP_INVENTORY, RenderPosX, 90.0, 0.0, 0.0);
		InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
		m_List.push_back(myOrgInterface.GetInterfaceList());
	}

	//-- npc shop
	myOrgInterface.SetOrgInvenPos(INTERFACE_NPCSHOP, RenderPosX, 50.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());

	//-- mix inventory
	myOrgInterface.SetOrgInvenPos(INTERFACE_MIXINVENTORY, RenderPosX, 110.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());


	//-- mix inventory
	myOrgInterface.SetOrgInvenPos(INTERFACE_MIX_EXPANSION, 0.0, 0.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());

	//-- lucky
	myOrgInterface.SetOrgInvenPos(INTERFACE_LUCKYITEMWND, RenderPosX, 110.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());

	//-- character info
	myOrgInterface.SetOrgInvenPos(INTERFACE_CHARACTER, 0.0, 0.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());

	myOrgInterface.SetOrgInvenPos(INTERFACE_INVENTORY_JEWEL, 0.0, 0.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());

	myOrgInterface.SetOrgInvenPos(INTERFACE_INVENTORY_CORE, 0.0, 0.0, 0.0, 0.0);
	InvenOrgInterface.insert(type_map_inven::value_type(myOrgInterface.GetInterfaceList(), myOrgInterface));
	m_List.push_back(myOrgInterface.GetInterfaceList());
}

int SEASON3B::CNewUIInvenExpansion::FindLinkageUI(int winIndex)
{
	for (std::list<int>::iterator it = m_List.begin(); it != m_List.end(); it++)
	{
		if (g_pNewUISystem->IsVisible(*it) == false)
		{
			continue;
		}
		ThirdInterface = 1;
		return *it;
	}

	ThirdInterface = 0;

	return winIndex;
}

tagPOINT* SEASON3B::CNewUIInvenExpansion::GetUIPosition(int index, bool isMyPos)
{
	static tagPOINT lpPoint = { 0,0 };

	std::map<int, CInvenOrgInterface>::iterator it = this->InvenOrgInterface.find(index);

	if (it == this->InvenOrgInterface.end())
	{
		return &lpPoint;
	}

	if (isMyPos == true)
	{
		lpPoint.x = it->second.GetOrgYourInvenPosX();

		lpPoint.y = it->second.GetOrgYourInvenPosY();

		return &lpPoint;
	}

	lpPoint.x = it->second.GetOrgInvenPosX();

	lpPoint.y = it->second.GetOrgInvenPosY();

	return &lpPoint;
}

bool SEASON3B::CNewUIInvenExpansion::SetEnableExpansionInven(BYTE count, DWORD Price)
{
	bool result = false;

	int MaxCount = count;

	this->InitExpansion();

	if (count >= (MAX_INVENT_EXPANSION + 1) || count >= (gmProtect->MaxInventoryExtension + 1))
	{
		MaxCount = 0;
	}

	m_PurchaseCoin = Price;

	if ((count < MAX_INVENT_EXPANSION && count < gmProtect->MaxInventoryExtension) && m_PurchaseCoin > 0)
	{
		m_BuyExpansionActive = true;
	}

	for (int i = 0; i < MaxCount; i++)
	{
		this->isExtEnabled[i] = true;

		result = 1;
	}

	g_pMyInventory->SetInvenEnableCnt(MaxCount);

	return result;
}

tagPOINT* SEASON3B::CNewUIInvenExpansion::GetPos()
{
	return &m_Pos;
}

tagPOINT* SEASON3B::CNewUIInvenExpansion::GetIEStartPos(int index)
{
	return &m_InvBoxPos[index];
}

