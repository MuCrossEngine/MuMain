//*****************************************************************************
// Desc: implementation of the CNewUITrade class.
//*****************************************************************************

#include "stdafx.h"

#include "NewUITrade.h"
#include "NewUISystem.h"
#include "NewUICustomMessageBox.h"
#include "wsclientinline.h"
#include "CComGem.h"
#include "CGMJewelOfAction.h"

using namespace SEASON3B;

CNewUITrade::CNewUITrade()
{
	m_pNewUIMng = NULL;
	m_pYourInvenCtrl = m_pMyInvenCtrl = NULL;
	m_Pos.x = m_Pos.y = 0;
	last_time = std::chrono::steady_clock::now();
}

CNewUITrade::~CNewUITrade()
{
	Release();
}

bool CNewUITrade::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng || NULL == g_pNewUI3DRenderMng
		|| NULL == g_pNewItemMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_TRADE, this);

	m_pYourInvenCtrl = new CNewUIInventoryCtrl;
	if (false == m_pYourInvenCtrl->Create(g_pNewUI3DRenderMng, g_pNewItemMng, this, x + (TRADE_WIDTH - 160) / 2, y + 68, COLUMN_TRADE_INVEN, ROW_TRADE_INVEN))
	{
		SAFE_DELETE(m_pYourInvenCtrl);
		return false;
	}
	m_pYourInvenCtrl->LockInventory();

	m_pMyInvenCtrl = new CNewUIInventoryCtrl;

	if (false == m_pMyInvenCtrl->Create(g_pNewUI3DRenderMng, g_pNewItemMng, this, x + (TRADE_WIDTH - 160) / 2, y + 274, COLUMN_TRADE_INVEN, ROW_TRADE_INVEN))
	{
		SAFE_DELETE(m_pMyInvenCtrl);
		return false;
	}

#if MAIN_UPDATE > 603
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		m_pYourInvenCtrl->SetPos(x + 15.0, y + 70);
		m_pMyInvenCtrl->SetPos(x + 15.0, y + 270);
	}
#else
	m_pYourInvenCtrl->SetPos(x + 15.0, y + 70);
	m_pMyInvenCtrl->SetPos(x + 15.0, y + 270);

#endif

	SetPos(x, y);

	LoadImages();

	m_abtn[BTN_CLOSE].ChangeToolTipText(GlobalText[1002], true);
	m_abtn[BTN_ZEN_INPUT].ChangeToolTipText(GlobalText[227], true);
#if MAIN_UPDATE > 603
	Loadbutton(&m_abtn[BTN_CLOSE], 0, m_Pos.x, m_Pos.y, 20, 20, BITMAP_btn_exit_00, false, false, false, 0.f, 0.f, 69.f, 72.f);
	Loadbutton(&m_abtn[BTN_ZEN_INPUT], 5, m_Pos.x, m_Pos.y, 20, 20, BITMAP_btn_money03, false, false, false, 0.f, 0.f, 69.f, 72.f);

	m_posMyConfirm.x = m_abtn[BTN_ZEN_INPUT].GetPos().x + 22;
	m_posMyConfirm.y = m_abtn[BTN_ZEN_INPUT].GetPos().y;
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		SetButtonInterface(&m_abtn[BTN_CLOSE], m_Pos.x, m_Pos.y, 1, false);
		SetButtonInterface(&m_abtn[BTN_ZEN_INPUT], m_Pos.x, m_Pos.y, 7, false);

		m_abtn[BTN_CLOSE].SetPos(x + 137.0, y + 390);
		m_abtn[BTN_ZEN_INPUT].SetPos(x + 26.f, y + 390);
}
	else
	{
		m_abtn[BTN_CLOSE].ChangeButtonImgState(true, IMAGE_TRADE_BTN_CLOSE);
		m_abtn[BTN_CLOSE].ChangeButtonInfo(x + 13, y + 390, 36, 29);
		m_abtn[BTN_ZEN_INPUT].ChangeButtonImgState(true, IMAGE_TRADE_BTN_ZEN_INPUT);
		m_abtn[BTN_ZEN_INPUT].ChangeButtonInfo(x + 104, y + 390, 36, 29);
	}
#else
	SetButtonInterface(&m_abtn[BTN_CLOSE], m_Pos.x, m_Pos.y, 1, false);
	SetButtonInterface(&m_abtn[BTN_ZEN_INPUT], m_Pos.x, m_Pos.y, 7, false);

	m_abtn[BTN_CLOSE].SetPos(x + 137.0, y + 390);
	m_abtn[BTN_ZEN_INPUT].SetPos(x + 26.f, y + 390);
#endif

	::memset(m_szYourID, 0, MAX_ID_SIZE + 1);
	m_bTradeAlert = false;

	this->SetItemAutoMove(false);
	this->InitTradeInfo();
	this->InitYourInvenBackUp();

	Show(false);

	return true;
}

void CNewUITrade::InitTradeInfo()
{
	m_nYourLevel = 0;
	m_nYourGuildType = -1;
	m_nYourTradeGold = 0;
	m_bYourConfirm = false;
	m_nYourTradeCoin[0] = 0;
	m_nYourTradeCoin[1] = 0;
	m_nYourTradeCoin[2] = 0;
	//--
	m_nMyTradeGold = 0;
	m_nMyTradeWait = 0;
	m_bMyConfirm = false;
	m_nMyTradeCoin[0] = 0;
	m_nMyTradeCoin[1] = 0;
	m_nMyTradeCoin[2] = 0;
}

void CNewUITrade::InitYourInvenBackUp()
{
	for (int i = 0; i < MAX_TRADE_INVEN; ++i)
		m_aYourInvenBackUp[i].Type = -1;
}

void CNewUITrade::Release()
{
	UnloadImages();

	SAFE_DELETE(m_pMyInvenCtrl);
	SAFE_DELETE(m_pYourInvenCtrl);

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}

	if (g_pNewUI3DRenderMng)
		g_pNewUI3DRenderMng->DeleteUI2DEffectObject(UI2DEffectCallback);
}

void CNewUITrade::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;

#if MAIN_UPDATE > 603
	SEASON3B::LoadPos(&m_abtn[BTN_CLOSE], 0, x, y);
	SEASON3B::LoadPos(&m_abtn[BTN_ZEN_INPUT], 5, x, y);


	m_posMyConfirm.x = m_abtn[BTN_ZEN_INPUT].GetPos().x + 22;
	m_posMyConfirm.y = m_abtn[BTN_ZEN_INPUT].GetPos().y;

#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		m_posMyConfirm.x = m_Pos.x + 97.0;
		m_posMyConfirm.y = m_Pos.y + 390.0;
		m_abtn[BTN_CLOSE].SetPos(x + 137.0, y + 390.0);
		m_abtn[BTN_ZEN_INPUT].SetPos(x + 26.f, y + 390.0);
	}
	else
	{
		m_posMyConfirm.x = m_Pos.x + 144;
		m_posMyConfirm.y = m_Pos.y + 390;
		m_abtn[BTN_CLOSE].SetPos(x + 13, y + 390);
		m_abtn[BTN_ZEN_INPUT].SetPos(x + 104, y + 390);
	}
#else
	m_posMyConfirm.x = m_Pos.x + 97.0;
	m_posMyConfirm.y = m_Pos.y + 390.0;

	m_abtn[BTN_CLOSE].SetPos(x + 137.0, y + 390.0);

	m_abtn[BTN_ZEN_INPUT].SetPos(x + 26.f, y + 390.0);
#endif // MAIN_UPDATE == 303
}

bool CNewUITrade::UpdateMouseEvent()
{
	if ((m_pYourInvenCtrl && false == m_pYourInvenCtrl->UpdateMouseEvent())
		|| (m_pMyInvenCtrl && false == m_pMyInvenCtrl->UpdateMouseEvent()))
	{
		if (SEASON3B::IsPress(VK_LBUTTON)
			&& CNewUIInventoryCtrl::GetPickedItem()->GetOwnerInventory() == m_pMyInvenCtrl
			&& m_bMyConfirm)
		{
			m_bMyConfirm = false;
			SendRequestTradeResult(m_bMyConfirm);
		}

		return false;
	}

	ProcessMyInvenCtrl();

	if (ProcessBtns())
		return false;

	if (CheckMouseIn(m_Pos.x, m_Pos.y, TRADE_WIDTH, TRADE_HEIGHT))
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

bool CNewUITrade::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			SendRequestTradeExit();
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_TRADE);
			PlayBuffer(SOUND_CLICK01);

			return false;
		}
	}
	return true;
}

bool CNewUITrade::Update()
{
	if ((m_pYourInvenCtrl && false == m_pYourInvenCtrl->Update())
		|| (m_pMyInvenCtrl && false == m_pMyInvenCtrl->Update()))
		return false;

	auto current_time = std::chrono::steady_clock::now();
	double difTime = std::chrono::duration<double>(current_time - last_time).count();

	if (difTime >= 1.0)
	{
		last_time = current_time;

		if (IsVisible())
		{
			if (m_nMyTradeWait > 0)
			{
				--m_nMyTradeWait;
			}
		}
	}

	return true;
}

bool CNewUITrade::Render()
{
	::EnableAlphaTest();

	::glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderBackImage();

	RenderText();

	if (m_pYourInvenCtrl)
		m_pYourInvenCtrl->Render();
	if (m_pMyInvenCtrl)
		m_pMyInvenCtrl->Render();

	for (int i = BTN_CLOSE; i < MAX_BTN; ++i)
		m_abtn[i].Render();

	if (g_pNewUI3DRenderMng)
		g_pNewUI3DRenderMng->RenderUI2DEffect(INVENTORY_CAMERA_Z_ORDER,
			UI2DEffectCallback, this, 0, 0);

	::DisableAlphaBlend();

	return true;
}

void CNewUITrade::UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB)
{
	if (pClass)
	{
		CNewUITrade* pNewUITrade = (CNewUITrade*)pClass;
		pNewUITrade->RenderWarningArrow();
	}
}

CNewUIInventoryCtrl* SEASON3B::CNewUITrade::GetYourInvenCtrl() const
{
	return m_pYourInvenCtrl;
}

CNewUIInventoryCtrl* SEASON3B::CNewUITrade::GetMyInvenCtrl() const
{
	return m_pMyInvenCtrl;
}

void CNewUITrade::RenderBackImage()
{
	SEASON3B::window_nomal((float)m_Pos.x, (float)m_Pos.y, 0, 220.f);

#if MAIN_UPDATE > 603
	RenderImageF(BITMAP_box_opentitle, m_Pos.x + ((WIN_WINDOW_SIZEX - 172.f) / 2.f), m_Pos.y + 37, 172.f, 24.f, 0.0, 0.0, 718.0, 146.0);

	RenderImageF(BITMAP_box_opentitle, m_Pos.x + ((WIN_WINDOW_SIZEX - 172.f) / 2.f), m_Pos.y + 243, 172.f, 24.f, 0.0, 0.0, 718.0, 146.0);

	RenderImageF(BITMAP_trade_money, m_Pos.x + ((WIN_WINDOW_SIZEX - 170.f) / 2.f), m_Pos.y + 150, 170.f, 22.f, 0.0, 0.0, 626.0, 74.0);

	RenderImageF(BITMAP_trade_money, m_Pos.x + ((WIN_WINDOW_SIZEX - 170.f) / 2.f), m_Pos.y + 356, 170.f, 22.f, 0.0, 0.0, 626.0, 74.0);

	float fSrcY = m_bYourConfirm ? 72.f : 0.f;
	RenderImageF(BITMAP_btn_checked, m_posMyConfirm.x, m_Pos.y + 180, 20.f, 20.f, 0.f, fSrcY, 69.f, 72.f);

	DWORD dwColor = m_nMyTradeWait > 0 ? RGBA(255, 0, 0, 255) : RGBA(255, 255, 255, 255);

	fSrcY = m_bMyConfirm ? 72.f : 0.f;
	RenderImageF(BITMAP_btn_checked, m_posMyConfirm.x, m_posMyConfirm.y, 20.f, 20.f, dwColor, 0.f, fSrcY, 69.f, 72.f);

#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		if (gmProtect->trade_system_pro)
		{
			glColor4f(0.0, 0.0, 0.0, 0.7);

			RenderColor(m_Pos.x + 15, m_Pos.y + 153, 160, 15, 0.0, 0);

			RenderColor(m_Pos.x + 15, m_Pos.y + 170, 160, 15, 0.0, 0);

			RenderColor(m_Pos.x + 15, m_Pos.y + 354, 160, 15, 0.0, 0);

			RenderColor(m_Pos.x + 15, m_Pos.y + 371, 160, 15, 0.0, 0);

			EndRenderColor();
		}
		else
		{
			RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 160, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);

			RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 360.0, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);
		}

		if (m_bYourConfirm)
			RenderImage(BITMAP_INVENTORY_BUTTON + 11, m_Pos.x + 140.0, m_Pos.y + 185.0, 24.0, 24.0, 0, 0, 0.75, 0.75);
		else
			RenderImage(BITMAP_INVENTORY_BUTTON + 10, m_Pos.x + 140.0, m_Pos.y + 185.0, 24.0, 24.0, 0, 0, 0.75, 0.75);

		DWORD dwColor = m_nMyTradeWait > 0 ? RGBA(255, 0, 0, 255) : RGBA(255, 255, 255, 255);

		if (m_bMyConfirm)
			RenderImage(BITMAP_INVENTORY_BUTTON + 11, m_Pos.x + 97.0, m_Pos.y + 390.0, 24.0, 24.0, 0, 0, 0.75, 0.75, dwColor);
		else
			RenderImage(BITMAP_INVENTORY_BUTTON + 10, m_Pos.x + 97.0, m_Pos.y + 390.0, 24.0, 24.0, 0, 0, 0.75, 0.75, dwColor);
}
	else
	{
		//RenderImage(IMAGE_TRADE_LINE, m_Pos.x + 1, m_Pos.y + 220, 188.f, 21.f);
		RenderImage(IMAGE_TRADE_NICK_BACK, m_Pos.x + 11, m_Pos.y + 37, 171.f, 26.f);
		RenderImage(IMAGE_TRADE_MONEY, m_Pos.x + 11, m_Pos.y + 150, 170.f, 26.f);
		RenderImage(IMAGE_TRADE_NICK_BACK, m_Pos.x + 11, m_Pos.y + 243, 171.f, 26.f);
		RenderImage(IMAGE_TRADE_MONEY, m_Pos.x + 11, m_Pos.y + 356, 170.f, 26.f);

		float fSrcY = m_bYourConfirm ? 29.f : 0.f;
		RenderImage(IMAGE_TRADE_CONFIRM, m_Pos.x + 146, m_Pos.y + 186, CONFIRM_WIDTH, CONFIRM_HEIGHT, 0.f, fSrcY);

		DWORD dwColor = m_nMyTradeWait > 0 ? RGBA(255, 0, 0, 255) : RGBA(255, 255, 255, 255);

		fSrcY = m_bMyConfirm ? 29.f : 0.f;
		RenderImage(IMAGE_TRADE_CONFIRM, m_posMyConfirm.x, m_posMyConfirm.y, CONFIRM_WIDTH, CONFIRM_HEIGHT, 0.f, fSrcY, dwColor);
	}
#else
	if (gmProtect->trade_system_pro)
	{
		glColor4f(0.0, 0.0, 0.0, 0.7);

		RenderColor(m_Pos.x + 15, m_Pos.y + 153, 160, 20, 0.0, 0);

		RenderColor(m_Pos.x + 15, m_Pos.y + 354, 160, 20, 0.0, 0);

		EndRenderColor();
	}
	else
	{
		RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 160, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);

		RenderImage(BITMAP_INVENTORY + 11, m_Pos.x + 50.0, m_Pos.y + 360.0, 113.0, 18.0, 0, 0, 0.8828125, 0.5625);
	}

	if (m_bYourConfirm)
		RenderImage(BITMAP_INVENTORY_BUTTON + 11, m_Pos.x + 140.0, m_Pos.y + 185.0, 24.0, 24.0, 0, 0, 0.75, 0.75);
	else
		RenderImage(BITMAP_INVENTORY_BUTTON + 10, m_Pos.x + 140.0, m_Pos.y + 185.0, 24.0, 24.0, 0, 0, 0.75, 0.75);

	DWORD dwColor = m_nMyTradeWait > 0 ? RGBA(255, 0, 0, 255) : RGBA(255, 255, 255, 255);

	if (m_bMyConfirm)
		RenderImage(BITMAP_INVENTORY_BUTTON + 11, m_Pos.x + 97.0, m_Pos.y + 390.0, 24.0, 24.0, 0, 0, 0.75, 0.75, dwColor);
	else
		RenderImage(BITMAP_INVENTORY_BUTTON + 10, m_Pos.x + 97.0, m_Pos.y + 390.0, 24.0, 24.0, 0, 0, 0.75, 0.75, dwColor);
#endif
}

void CNewUITrade::RenderText()
{
	int nLevel;
	DWORD dwColor;
	unicode::t_char szTemp[128];

#if MAIN_UPDATE > 603
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(216, 216, 216, 255);
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 15, GlobalText[226], TRADE_WIDTH, 0, RT3_SORT_CENTER);


	int Addx = (TRADE_WIDTH - 190) / 2;

	for (int i = 0; i < MAX_MARKS; ++i)
	{
		if (GuildMark[i].Key != -1 && GuildMark[i].Key == m_nYourGuildType)
		{
			::CreateGuildMark(i, false);
			::RenderBitmap(BITMAP_GUILD, (float)m_Pos.x + 15 + Addx, (float)m_Pos.y + 42, 16, 16);
			g_pRenderText->RenderText(m_Pos.x + 16 + Addx, m_Pos.y + 30, GuildMark[i].GuildName);
			break;
		}
	}

	g_pRenderText->SetFont(g_hFontBig);

	g_pRenderText->SetTextColor(210, 230, 255, 255);
	g_pRenderText->RenderText(m_Pos.x + 32 + Addx, m_Pos.y + 43, m_szYourID);

	g_pRenderText->SetFont(g_hFontBold);

	ConvertYourLevel(nLevel, dwColor);
	if (nLevel == 400)
		unicode::_sprintf(szTemp, "%d", nLevel);
	else
		unicode::_sprintf(szTemp, GlobalText[369], nLevel);

	g_pRenderText->SetTextColor(dwColor);
	g_pRenderText->RenderText(m_Pos.x + 134 + Addx, m_Pos.y + 48, "Lv.");
	g_pRenderText->RenderText(m_Pos.x + 148 + Addx, m_Pos.y + 48, szTemp);

	::ConvertGold(m_nYourTradeGold, szTemp);
	g_pRenderText->SetTextColor(::getGoldColor(m_nYourTradeGold));
	g_pRenderText->RenderText(m_Pos.x + 160, m_Pos.y + 150 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

	::ConvertGold(m_nMyTradeGold, szTemp);
	g_pRenderText->SetTextColor(::getGoldColor(m_nMyTradeGold));
	g_pRenderText->RenderText(m_Pos.x + 160, m_Pos.y + 356 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

	g_pRenderText->SetTextColor(210, 230, 255, 255);
	g_pRenderText->RenderText(m_Pos.x + 20 + Addx, m_Pos.y + 253, Hero->ID);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(0);

		g_pRenderText->SetTextColor(216, 216, 216, 255);
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 11, GlobalText[226], TRADE_WIDTH, 0, RT3_SORT_CENTER);

		g_pRenderText->SetBgColor(40, 40, 40, 128);
		for (int i = 0; i < MAX_MARKS; ++i)
		{
			if (GuildMark[i].Key != -1 && GuildMark[i].Key == m_nYourGuildType)
			{
				CreateGuildMark(i, false);
				RenderBitmap(BITMAP_GUILD, (float)m_Pos.x - 32.0, (float)m_Pos.y + 40.0, 32.0, 32.0);
				g_pRenderText->RenderText(m_Pos.x - 32, m_Pos.y + 30, GuildMark[i].GuildName);
				break;
			}
		}

		g_pRenderText->SetFont(g_hFontBig);
		g_pRenderText->SetBgColor(40, 40, 40, 128);
		g_pRenderText->SetTextColor(210, 230, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 45, m_szYourID);

		g_pRenderText->SetFont(g_hFontBold);

		ConvertYourLevel(nLevel, dwColor);
		if (nLevel == 400)
		{
			unicode::_sprintf(szTemp, "%d", nLevel);
		}
		else
		{
			unicode::_sprintf(szTemp, GlobalText[369], nLevel);
		}
		g_pRenderText->SetTextColor(dwColor);
		g_pRenderText->RenderText(m_Pos.x + 140, m_Pos.y + 45, "Level");
		g_pRenderText->RenderText(m_Pos.x + 140, m_Pos.y + 55, szTemp);

		g_pRenderText->SetBgColor(0);

		if (gmProtect->trade_system_pro)
		{
			this->RenderGold(m_Pos.x + 18, m_Pos.y + 153, m_nYourTradeGold, 0);
			this->RenderGold(m_Pos.x + 105, m_Pos.y + 153, m_nYourTradeCoin[0], 1);

			this->RenderGold(m_Pos.x + 18, m_Pos.y + 170, m_nYourTradeCoin[1], 2);
			this->RenderGold(m_Pos.x + 105, m_Pos.y + 170, m_nYourTradeCoin[2], 3);

			this->RenderGold(m_Pos.x + 18, m_Pos.y + 354, m_nMyTradeGold, 0);
			this->RenderGold(m_Pos.x + 105, m_Pos.y + 354, m_nMyTradeCoin[0], 1);

			this->RenderGold(m_Pos.x + 18, m_Pos.y + 371, m_nMyTradeCoin[1], 2);
			this->RenderGold(m_Pos.x + 105, m_Pos.y + 371, m_nMyTradeCoin[2], 3);
		}
		else
		{
			::ConvertGold(m_nYourTradeGold, szTemp);
			g_pRenderText->SetTextColor(::getGoldColor(m_nYourTradeGold));
			g_pRenderText->RenderText(m_Pos.x + 60, m_Pos.y + 160, szTemp, 0, 18);

			::ConvertGold(m_nMyTradeGold, szTemp);
			g_pRenderText->SetTextColor(::getGoldColor(m_nMyTradeGold));
			g_pRenderText->RenderText(m_Pos.x + 60, m_Pos.y + 360, szTemp, 0, 18);
		}

		g_pRenderText->SetBgColor(40, 40, 40, 128);

		g_pRenderText->SetTextColor(210, 230, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 253, Hero->ID);
	}
	else
	{
		g_pRenderText->SetFont(g_hFontBold);
		g_pRenderText->SetBgColor(0);

		g_pRenderText->SetTextColor(216, 216, 216, 255);
		g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 11, GlobalText[226], TRADE_WIDTH, 0, RT3_SORT_CENTER);

		for (int i = 0; i < MAX_MARKS; ++i)
		{
			if (GuildMark[i].Key != -1 && GuildMark[i].Key == m_nYourGuildType)
			{
				::CreateGuildMark(i, false);
				::RenderBitmap(BITMAP_GUILD, (float)m_Pos.x + 15, (float)m_Pos.y + 42, 16, 16);
				g_pRenderText->RenderText(m_Pos.x + 16, m_Pos.y + 30, GuildMark[i].GuildName);
				break;
			}
		}

		g_pRenderText->SetFont(g_hFontBig);

		g_pRenderText->SetTextColor(210, 230, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 32, m_Pos.y + 43, m_szYourID);

		g_pRenderText->SetFont(g_hFontBold);

		ConvertYourLevel(nLevel, dwColor);
		if (nLevel == 400)
		{
			unicode::_sprintf(szTemp, "%d", nLevel);
		}
		else
		{
			unicode::_sprintf(szTemp, GlobalText[369], nLevel);
		}
		g_pRenderText->SetTextColor(dwColor);
		g_pRenderText->RenderText(m_Pos.x + 134, m_Pos.y + 48, "Lv.");
		g_pRenderText->RenderText(m_Pos.x + 148, m_Pos.y + 48, szTemp);

		::ConvertGold(m_nYourTradeGold, szTemp);
		g_pRenderText->SetTextColor(::getGoldColor(m_nYourTradeGold));
		g_pRenderText->RenderText(m_Pos.x + 170, m_Pos.y + 150 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

		::ConvertGold(m_nMyTradeGold, szTemp);
		g_pRenderText->SetTextColor(::getGoldColor(m_nMyTradeGold));
		g_pRenderText->RenderText(m_Pos.x + 170, m_Pos.y + 356 + 8, szTemp, 0, 0, RT3_WRITE_RIGHT_TO_LEFT);

		g_pRenderText->SetTextColor(210, 230, 255, 255);
		g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 253, Hero->ID);
	}


#else
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(216, 216, 216, 255);
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y + 11, GlobalText[226], TRADE_WIDTH, 0, RT3_SORT_CENTER);

	g_pRenderText->SetBgColor(40, 40, 40, 128);
	for (int i = 0; i < MAX_MARKS; ++i)
	{
		if (GuildMark[i].Key != -1 && GuildMark[i].Key == m_nYourGuildType)
		{
			CreateGuildMark(i, false);
			RenderBitmap(BITMAP_GUILD, (float)m_Pos.x - 32.0, (float)m_Pos.y + 40.0, 32.0, 32.0);
			g_pRenderText->RenderText(m_Pos.x - 32, m_Pos.y + 30, GuildMark[i].GuildName);
			break;
		}
	}

	g_pRenderText->SetFont(g_hFontBig);
	g_pRenderText->SetBgColor(40, 40, 40, 128);
	g_pRenderText->SetTextColor(210, 230, 255, 255);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 45, m_szYourID);

	g_pRenderText->SetFont(g_hFontBold);

	ConvertYourLevel(nLevel, dwColor);
	if (nLevel == 400)
	{
		unicode::_sprintf(szTemp, "%d", nLevel);
	}
	else
	{
		unicode::_sprintf(szTemp, GlobalText[369], nLevel);
	}
	g_pRenderText->SetTextColor(dwColor);
	g_pRenderText->RenderText(m_Pos.x + 140, m_Pos.y + 45, "Level");
	g_pRenderText->RenderText(m_Pos.x + 140, m_Pos.y + 55, szTemp);

	g_pRenderText->SetBgColor(0);

	if (gmProtect->trade_system_pro)
	{
		this->RenderGold(m_Pos.x + 18, m_Pos.y + 153, m_nYourTradeGold, 0);
		this->RenderGold(m_Pos.x + 105, m_Pos.y + 153, m_nYourTradeCoin[0], 1);

		//this->RenderGold(m_Pos.x + 18, m_Pos.y + 170, m_nYourTradeCoin[1], 2);
		//this->RenderGold(m_Pos.x + 105, m_Pos.y + 170, m_nYourTradeCoin[2], 3);

		this->RenderGold(m_Pos.x + 18, m_Pos.y + 354, m_nMyTradeGold, 0);
		this->RenderGold(m_Pos.x + 105, m_Pos.y + 354, m_nMyTradeCoin[0], 1);

		//this->RenderGold(m_Pos.x + 18, m_Pos.y + 371, m_nMyTradeCoin[1], 2);
		//this->RenderGold(m_Pos.x + 105, m_Pos.y + 371, m_nMyTradeCoin[2], 3);
	}
	else
	{
		::ConvertGold(m_nYourTradeGold, szTemp);
		g_pRenderText->SetTextColor(::getGoldColor(m_nYourTradeGold));
		g_pRenderText->RenderText(m_Pos.x + 60, m_Pos.y + 160, szTemp, 0, 18);

		::ConvertGold(m_nMyTradeGold, szTemp);
		g_pRenderText->SetTextColor(::getGoldColor(m_nMyTradeGold));
		g_pRenderText->RenderText(m_Pos.x + 60, m_Pos.y + 360, szTemp, 0, 18);
	}

	g_pRenderText->SetBgColor(40, 40, 40, 128);
	g_pRenderText->SetTextColor(210, 230, 255, 255);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 253, Hero->ID);
#endif // MAIN_UPDATE == 303


	int nAlpha = int(min(255, sin(WorldTime / 200) * 200 + 275));
	g_pRenderText->SetTextColor(210, 0, 0, nAlpha);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 185, GlobalText[370]);
	g_pRenderText->SetTextColor(255, 220, 150, 255);
	g_pRenderText->RenderText(m_Pos.x + 45, m_Pos.y + 185, GlobalText[365]);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 200, GlobalText[366]);
	g_pRenderText->RenderText(m_Pos.x + 20, m_Pos.y + 215, GlobalText[367]);
}

void CNewUITrade::RenderWarningArrow()
{
	::EnableAlphaTest();

	int nYourItems = m_pYourInvenCtrl->GetNumberOfItems();
	ITEM* pYourItemObj;
	int nWidth;
	float fX, fY;
	POINT ptYourInvenCtrl = m_pYourInvenCtrl->GetPos();

	for (int i = 0; i < nYourItems; ++i)
	{
		pYourItemObj = m_pYourInvenCtrl->GetItem(i);
		if (ITEM_COLOR_TRADE_WARNING == pYourItemObj->byColorState)
		{
			fX = (float)ptYourInvenCtrl.x + (pYourItemObj->x * INVENTORY_SQUARE_WIDTH);
			fY = (float)ptYourInvenCtrl.y + (pYourItemObj->y * INVENTORY_SQUARE_WIDTH) + sinf(WorldTime * 0.015f);

			::glColor3f(0.f, 1.f, 1.f);
			::RenderBitmap(IMAGE_TRADE_WARNING_ARROW, fX, fY + 5, 24.f, 24.f,
				0.f, 0.4f);
			::glColor3f(1.f, 1.f, 1.f);

			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetTextColor(255, 255, 255, 255);
			g_pRenderText->SetBgColor(210, 0, 0, 255);
			nWidth = (int)GMItemMng->find(pYourItemObj->Type)->Width
				* INVENTORY_SQUARE_WIDTH;
			g_pRenderText->RenderText((int)fX, (int)fY, GlobalText[370],
				nWidth, 0, RT3_SORT_CENTER);
		}
	}

	::DisableAlphaBlend();
}

void CNewUITrade::ConvertYourLevel(int& rnLevel, DWORD& rdwColor)
{
	if (m_nYourLevel >= 400)
	{
		rnLevel = 400;
		rdwColor = (255 << 24) + (153 << 16) + (153 << 8) + (255);
	}
	else if (m_nYourLevel >= 300)
	{
		rnLevel = 300;
		rdwColor = (255 << 24) + (255 << 16) + (153 << 8) + (255);
	}
	else if (m_nYourLevel >= 200)
	{
		rnLevel = 200;
		rdwColor = (255 << 24) + (255 << 16) + (230 << 8) + (210);
	}
	else if (m_nYourLevel >= 100)
	{
		rnLevel = 100;
		rdwColor = (255 << 24) + (24 << 16) + (201 << 8) + (0);
	}
	else if (m_nYourLevel >= 50)
	{
		rnLevel = 50;
		rdwColor = (255 << 24) + (0 << 16) + (150 << 8) + (255);
	}
	else
	{
		rnLevel = 10;
		rdwColor = (255 << 24) + (0 << 16) + (0 << 8) + (255);
	}
}

float CNewUITrade::GetLayerDepth()
{
	return 2.1f;
}

void SEASON3B::CNewUITrade::EventOrderWindows(double WindowsX, double WindowsY)
{
	float RenderFrameX = (GetWindowsX);
	float RenderFrameY = (m_Pos.y);

	RenderFrameX -= (WindowsX - m_Pos.x);

	this->SetPos((int)RenderFrameX, (int)RenderFrameY);

	if (m_pYourInvenCtrl != NULL && m_pMyInvenCtrl != NULL)
	{
		if (gmProtect->checkold_school())
		{
			m_pYourInvenCtrl->SetPos(RenderFrameX + 15.0, RenderFrameY + 70);
			m_pMyInvenCtrl->SetPos(RenderFrameX + 15.0, RenderFrameY + 270);
		}
		else
		{
			m_pYourInvenCtrl->SetPos(RenderFrameX + 15.0, RenderFrameY + 68.0);
			m_pMyInvenCtrl->SetPos(RenderFrameX + 15.0, RenderFrameY + 274.0);
		}
	}
}

void CNewUITrade::LoadImages()
{

#if MAIN_UPDATE > 603
	LoadBitmap("Interface\\HUD_TEST\\newui_trade_money.dds", BITMAP_trade_money, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_bt_money01.dds", BITMAP_btn_money03, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_bt_accept.dds", BITMAP_btn_checked, GL_LINEAR);
#endif
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_TRADE_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back01.tga", IMAGE_TRADE_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-L.tga", IMAGE_TRADE_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back02-R.tga", IMAGE_TRADE_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_TRADE_BOTTOM, GL_LINEAR);
	LoadBitmap("Interface\\newui_myquest_Line.tga", IMAGE_TRADE_LINE, GL_LINEAR);
	LoadBitmap("Interface\\newui_Account_title.tga", IMAGE_TRADE_NICK_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_money.tga", IMAGE_TRADE_MONEY, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_accept.tga", IMAGE_TRADE_CONFIRM, GL_LINEAR);
	LoadBitmap("Interface\\CursorSitDown.tga", IMAGE_TRADE_WARNING_ARROW, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\newui_exit_00.tga", IMAGE_TRADE_BTN_CLOSE, GL_LINEAR);
	LoadBitmap("Interface\\newui_Bt_money01.tga", IMAGE_TRADE_BTN_ZEN_INPUT, GL_LINEAR);
}

void CNewUITrade::UnloadImages()
{
#if MAIN_UPDATE > 603
	DeleteBitmap(BITMAP_trade_money);
	DeleteBitmap(BITMAP_btn_money03);
	DeleteBitmap(BITMAP_btn_checked);
#endif
	DeleteBitmap(IMAGE_TRADE_BTN_ZEN_INPUT);
	DeleteBitmap(IMAGE_TRADE_BTN_CLOSE);
	DeleteBitmap(IMAGE_TRADE_WARNING_ARROW);
	DeleteBitmap(IMAGE_TRADE_CONFIRM);
	DeleteBitmap(IMAGE_TRADE_MONEY);
	DeleteBitmap(IMAGE_TRADE_NICK_BACK);
	DeleteBitmap(IMAGE_TRADE_LINE);
	DeleteBitmap(IMAGE_TRADE_BOTTOM);
	DeleteBitmap(IMAGE_TRADE_RIGHT);
	DeleteBitmap(IMAGE_TRADE_LEFT);
	DeleteBitmap(IMAGE_TRADE_TOP);
	DeleteBitmap(IMAGE_TRADE_BACK);
}

void CNewUITrade::ClosingProcess()
{
	m_pYourInvenCtrl->RemoveAllItems();
	m_pMyInvenCtrl->RemoveAllItems();

	if (m_bTradeAlert)
		InitYourInvenBackUp();
}

void CNewUITrade::ProcessMyInvenCtrl()
{
	if (NULL == m_pMyInvenCtrl)
		return;

	if (SEASON3B::IsPress(VK_LBUTTON))
	{
		CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();
		if (NULL == pPickedItem)
			return;

		ITEM* pItemObj = pPickedItem->GetItem();

		int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

		if (InvenIndex != -1)
		{
			int nSrcIndex = pPickedItem->GetSourceLinealPos();
			int nDstIndex = pPickedItem->GetTargetLinealPos(m_pMyInvenCtrl);

			nSrcIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(nSrcIndex, InvenIndex);

			if (nDstIndex != -1 && m_pMyInvenCtrl->CanMove(nDstIndex, pItemObj))
				SendRequestItemToTrade(pItemObj, MAX_EQUIPMENT_INDEX + nSrcIndex, nDstIndex);
		}

		else if (pPickedItem->GetOwnerInventory() == m_pMyInvenCtrl)
		{
			int nSrcIndex = pPickedItem->GetSourceLinealPos();
			int nDstIndex = pPickedItem->GetTargetLinealPos(m_pMyInvenCtrl);
			if (nDstIndex != -1 && m_pMyInvenCtrl->CanMove(nDstIndex, pItemObj))
			{
				SendRequestEquipmentItem(REQUEST_EQUIPMENT_TRADE, nSrcIndex, pItemObj, REQUEST_EQUIPMENT_TRADE, nDstIndex);
			}
		}
		else if (pItemObj->ex_src_type == ITEM_EX_SRC_EQUIPMENT)
		{
			int nSrcIndex = pPickedItem->GetSourceLinealPos();
			int nDstIndex = pPickedItem->GetTargetLinealPos(m_pMyInvenCtrl);
			if (nDstIndex != -1 && m_pMyInvenCtrl->CanMove(nDstIndex, pItemObj))
				SendRequestItemToTrade(pItemObj, nSrcIndex, nDstIndex);
		}
	}
	else if (SEASON3B::IsPress(VK_RBUTTON))
	{
		this->ProcessTradeItemAutoMove();
	}
}

void CNewUITrade::SendRequestItemToTrade(ITEM* pItemObj, int nInvenIndex, int nTradeIndex)
{
	if (::IsTradeBan(pItemObj))
	{
		g_pChatListBox->AddText("", GlobalText[494], SEASON3B::TYPE_ERROR_MESSAGE);
	}
	else
	{
		m_bMyConfirm = false;
		SendRequestTradeResult(m_bMyConfirm);
		SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, nInvenIndex, pItemObj, REQUEST_EQUIPMENT_TRADE, nTradeIndex);
	}
}

void SEASON3B::CNewUITrade::RenderGold(int x, int y, DWORD nGold, int index)
{
	char szTemp[128];
	char szGold[128];

	ConvertGold(nGold, szTemp, 0);

	if (index == 0)
	{
		sprintf_s(szGold, "%s: %s", GlobalText[224], szTemp);
	}
	else if (index == 1)
	{
		sprintf_s(szGold, "%s %s", TextGlobal[51], szTemp);
	}
	else if (index == 2)
	{
		sprintf_s(szGold, "%s %s", TextGlobal[52], szTemp);
	}
	else
	{
		sprintf_s(szGold, "%s %s", TextGlobal[53], szTemp);
	}

	g_pRenderText->SetTextColor(getGoldColor(nGold));

	g_pRenderText->RenderFont(x, y, szGold, 80, 20, RT3_SORT_LEFT);
}

void CNewUITrade::SendRequestItemToMyInven(ITEM* pItemObj, int nTradeIndex, int nInvenIndex)
{
	SendRequestEquipmentItem(REQUEST_EQUIPMENT_TRADE, nTradeIndex, pItemObj, REQUEST_EQUIPMENT_INVENTORY, nInvenIndex);

	if (m_bMyConfirm)
	{
		AlertTrade();
	}
	m_nMyTradeWait = 6;
}

void CNewUITrade::SendRequestMyGoldInput(int nInputGold)
{
	if (nInputGold > (int)CharacterMachine->Gold + m_nMyTradeGold)
	{
		SEASON3B::CreateOkMessageBox(GlobalText[423]);
	}
	else
	{
		if (m_bMyConfirm)
		{
			m_bMyConfirm = false;
			SendRequestTradeResult(m_bMyConfirm);
		}

		if (m_nMyTradeGold > 0
			|| m_nMyTradeCoin[0] > 0
			|| m_nMyTradeCoin[1] > 0
			|| m_nMyTradeCoin[2] > 0)
		{
			m_nMyTradeWait = 6;
		}

		m_nTempMyTradeGold = nInputGold;
		m_nTempMyTradeCoin[0] = 0;
		m_nTempMyTradeCoin[1] = 0;
		m_nTempMyTradeCoin[2] = 0;
		SendRequestTradeGold(nInputGold, 0, 0, 0);
	}
}

void SEASON3B::CNewUITrade::SendRequestMyGoldInput(int nGold, int nCoin1, int nCoin2, int nCoin3)
{
	if (nGold > (int)CharacterMachine->Gold + m_nMyTradeGold
		|| nCoin1 > (int)CharacterMachine->Coin1 + m_nMyTradeCoin[0]
		|| nCoin2 > (int)CharacterMachine->Coin2 + m_nMyTradeCoin[1]
		|| nCoin3 > (int)CharacterMachine->Coin3 + m_nMyTradeCoin[2]
		)
	{
		SEASON3B::CreateOkMessageBox(GlobalText[423]);
	}
	else
	{
		if (m_bMyConfirm)
		{
			m_bMyConfirm = false;
			SendRequestTradeResult(m_bMyConfirm);
		}

		if (m_nMyTradeGold > 0
			|| m_nMyTradeCoin[0] > 0
			|| m_nMyTradeCoin[1] > 0
			|| m_nMyTradeCoin[2] > 0)
		{
			m_nMyTradeWait = 6;
		}

		m_nTempMyTradeGold = nGold;
		m_nTempMyTradeCoin[0] = nCoin1;
		m_nTempMyTradeCoin[1] = nCoin2;
		m_nTempMyTradeCoin[2] = nCoin3;
		SendRequestTradeGold(nGold, nCoin1, nCoin2, nCoin3);
	}
}

void CNewUITrade::ProcessCloseBtn()
{
	if (CNewUIInventoryCtrl::GetPickedItem() == NULL)
	{
		m_bTradeAlert = false;
		SendRequestTradeExit();
	}
}

bool CNewUITrade::ProcessBtns()
{
	//if (m_nMyTradeWait > 0)
	//	--m_nMyTradeWait;

	if (m_abtn[BTN_CLOSE].UpdateMouseEvent())
	{
		::PlayBuffer(SOUND_CLICK01);
		ProcessCloseBtn();
		return true;
	}
	else if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(m_Pos.x + 169, m_Pos.y + 7, 13, 12))
	{
		::PlayBuffer(SOUND_CLICK01);
		ProcessCloseBtn();
		return true;
	}
	else if (m_abtn[BTN_ZEN_INPUT].UpdateMouseEvent())
	{
		if (gmProtect->trade_system_pro)
		{
			CTradeCoinMsgBoxLayout* pMsgBox = g_MessageBox->NewMessageBox(MSGBOX_CLASS(CTradeCoinMsgBoxLayout));

			if (pMsgBox)
			{
				pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL, "Prueba");
			}
		}
		else
		{
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CTradeZenMsgBoxLayout));
		}
		::PlayBuffer(SOUND_CLICK01);
		return true;
	}
	else if (SEASON3B::IsRelease(VK_LBUTTON) && CheckMouseIn(m_posMyConfirm.x, m_posMyConfirm.y, CONFIRM_WIDTH, CONFIRM_HEIGHT))
	{
		if (0 == m_nMyTradeWait && CNewUIInventoryCtrl::GetPickedItem() == NULL)
		{
			::PlayBuffer(SOUND_CLICK01);

			if (m_bTradeAlert && !m_bMyConfirm)
			{
				SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CTradeAlertMsgBoxLayout));
			}
			else
			{
				AlertTrade();
			}
		}
		return true;
	}

	return false;
}

void CNewUITrade::AlertTrade()
{
	m_bMyConfirm = !m_bMyConfirm;
	m_bTradeAlert = true;
	SendRequestTradeResult(m_bMyConfirm);
}

void CNewUITrade::GetYourID(char* pszYourID)
{
	::strcpy(pszYourID, m_szYourID);
}

void SEASON3B::CNewUITrade::SetYourTradeGold(int nGold, int nCoin1, int nCoin2, int nCoin3)
{
	m_nYourTradeGold = nGold;
	m_nYourTradeCoin[0] = nCoin1;
	m_nYourTradeCoin[1] = nCoin2;
	m_nYourTradeCoin[2] = nCoin3;
}

void CNewUITrade::ProcessToReceiveTradeRequest(BYTE* pbyYourID)
{
	if (g_pNewUISystem->IsImpossibleTradeInterface())
	{
		SendRequestTradeAnswer(false);
		return;
	}

	::memcpy(m_szYourID, pbyYourID, MAX_ID_SIZE);
	m_szYourID[MAX_ID_SIZE] = NULL;

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CTradeMsgBoxLayout));

	SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
}

void CNewUITrade::ProcessToReceiveTradeResult(LPPTRADE pTradeData)
{
	switch (pTradeData->SubCode)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[492], SEASON3B::TYPE_ERROR_MESSAGE);
		break;

	case 2:
		g_pChatListBox->AddText("", GlobalText[493], SEASON3B::TYPE_ERROR_MESSAGE);
		break;

	case 1:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_TRADE);

		InitTradeInfo();

		int x = 260 * MouseX / gwinhandle->GetScreenX();
		::SetCursorPos(x * WindowWidth / gwinhandle->GetScreenX(), MouseY * WindowHeight / gwinhandle->GetScreenY());

		char szTempID[MAX_ID_SIZE + 1];
		::memcpy(szTempID, pTradeData->ID, MAX_ID_SIZE);
		szTempID[MAX_ID_SIZE] = NULL;

		if (!m_bTradeAlert && ::strcmp(m_szYourID, szTempID))
			InitYourInvenBackUp();

		m_bTradeAlert = false;
		m_nYourGuildType = pTradeData->GuildKey;
		::memcpy(m_szYourID, pTradeData->ID, MAX_ID_SIZE);
		m_szYourID[MAX_ID_SIZE] = NULL;
		m_nYourLevel = pTradeData->Level;   //  상대방 레벨.
		break;
	}
}

void CNewUITrade::ProcessToReceiveYourItemDelete(BYTE byYourInvenIndex)
{
	BackUpYourInven(int(byYourInvenIndex));
	ITEM* pYourItemObj = m_pYourInvenCtrl->FindItem(int(byYourInvenIndex));
	m_pYourInvenCtrl->RemoveItem(pYourItemObj);
	AlertYourTradeInven();
	::PlayBuffer(SOUND_GET_ITEM01);
}

void CNewUITrade::BackUpYourInven(int nYourInvenIndex)
{
	ITEM* pYourItemObj = m_pYourInvenCtrl->FindItem(nYourInvenIndex);
	BackUpYourInven(pYourItemObj);
}

void CNewUITrade::BackUpYourInven(ITEM* pYourItemObj)
{
	if ((pYourItemObj->Type >= ITEM_HELPER && pYourItemObj->Type <= ITEM_HELPER + 4)
		|| (pYourItemObj->Type == ITEM_POTION + 13 || pYourItemObj->Type == ITEM_POTION + 14 || pYourItemObj->Type == ITEM_POTION + 16)
		|| (pYourItemObj->Type >= ITEM_POTION + 31)
		|| (COMGEM::isCompiledGem(pYourItemObj))
		|| (pYourItemObj->Type >= ITEM_WING && pYourItemObj->Type <= ITEM_WING + 6)
		|| (pYourItemObj->Type >= ITEM_HELPER + 30)
		|| (pYourItemObj->Type >= ITEM_WING + 36 && pYourItemObj->Type <= ITEM_WING + 43)
		|| (pYourItemObj->Type == ITEM_WING + 15)
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		|| (pYourItemObj->Type >= ITEM_WING + 49 && pYourItemObj->Type <= ITEM_WING + 50)
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		|| ((((pYourItemObj->Level >> 3) & 15) > 4 && pYourItemObj->Type < ITEM_WING) || (pYourItemObj->Option1 & 63) > 0)
		|| GMJewelOfAction->CheckOfJewelByItem(pYourItemObj->Type))
	{
		int nCompareValue;
		bool bSameItem = false;

		for (int i = 0; i < MAX_TRADE_INVEN; ++i)
		{
			if (-1 == m_aYourInvenBackUp[i].Type)
				continue;

			nCompareValue = ::CompareItem(m_aYourInvenBackUp[i], *pYourItemObj);
			if (0 == nCompareValue)
			{
				bSameItem = true;
				break;
			}
			else if (-1 == nCompareValue)
			{
				bSameItem = true;
				m_aYourInvenBackUp[i] = *pYourItemObj;
				break;
			}
			else if (2 != nCompareValue)
			{
				bSameItem = true;
			}
		}

		if (!bSameItem)
		{
			for (int i = 0; i < MAX_TRADE_INVEN; ++i)
			{
				if (-1 == m_aYourInvenBackUp[i].Type)
				{
					m_aYourInvenBackUp[i] = *pYourItemObj;
					break;
				}
			}
		}
	}
}

void CNewUITrade::ProcessToReceiveYourItemAdd(BYTE byYourInvenIndex, BYTE* pbyItemPacket)
{
	m_pYourInvenCtrl->AddItem(byYourInvenIndex, pbyItemPacket);
	AlertYourTradeInven();
	::PlayBuffer(SOUND_GET_ITEM01);
}

void CNewUITrade::AlertYourTradeInven()
{
	int nCount = 0;
	int nCompareItemType[10];

	m_bTradeAlert = false;

	int nYourItems = m_pYourInvenCtrl->GetNumberOfItems();
	ITEM* pYourItemObj;
	int nCompareValue;

	for (int i = 0; i < nYourItems; ++i)
	{
		pYourItemObj = m_pYourInvenCtrl->GetItem(i);
		for (int j = 0; j < MAX_TRADE_INVEN; ++j)
		{
			if (m_aYourInvenBackUp[j].Type == pYourItemObj->Type)
			{
				nCompareValue = ::CompareItem(m_aYourInvenBackUp[j], *pYourItemObj);
				if (1 == nCompareValue)
				{
					m_bTradeAlert = true;
					pYourItemObj->byColorState = ITEM_COLOR_TRADE_WARNING;
				}
				else
				{
					if (0 == nCompareValue)
						nCompareItemType[nCount++] = m_aYourInvenBackUp[j].Type;

					pYourItemObj->byColorState = ITEM_COLOR_NORMAL;
					break;
				}
			}
		}
	}

	if (nCount > 0)
	{
		m_bTradeAlert = false;
		for (int i = 0; i < nCount; ++i)
		{
			for (int j = 0; j < nYourItems; ++j)
			{
				pYourItemObj = m_pYourInvenCtrl->GetItem(j);
				if (nCompareItemType[i] == pYourItemObj->Type)
					pYourItemObj->byColorState = ITEM_COLOR_NORMAL;
			}
		}
	}
}

void CNewUITrade::ProcessToReceiveMyTradeGold(BYTE bySuccess)
{
	m_nMyTradeGold = bySuccess ? m_nTempMyTradeGold : 0;
	m_nMyTradeCoin[0] = bySuccess ? m_nTempMyTradeCoin[0] : 0;
	m_nMyTradeCoin[1] = bySuccess ? m_nTempMyTradeCoin[1] : 0;
	m_nMyTradeCoin[2] = bySuccess ? m_nTempMyTradeCoin[2] : 0;
}

void CNewUITrade::ProcessToReceiveYourConfirm(BYTE byState)
{
	switch (byState)
	{
	case 0:
		m_bYourConfirm = false;
		break;
	case 1:
		m_bYourConfirm = true;
		break;
	case 2:
		m_bMyConfirm = false;
		m_bYourConfirm = false;
		m_nMyTradeWait = 6;
		break;
	case 3:
		break;

	}

	PlayBuffer(SOUND_CLICK01);
}

void CNewUITrade::ProcessToReceiveTradeExit(BYTE byState)
{
	switch (byState)
	{
	case 0:
	{
		g_pChatListBox->AddText("", GlobalText[492], SEASON3B::TYPE_ERROR_MESSAGE);

		m_bTradeAlert = false;

		int nYourItems = m_pYourInvenCtrl->GetNumberOfItems();
		for (int i = 0; i < nYourItems; ++i)
			BackUpYourInven(m_pYourInvenCtrl->GetItem(i));
	}
	break;

	case 2:
		g_pChatListBox->AddText("", GlobalText[495], SEASON3B::TYPE_ERROR_MESSAGE);
		break;

	case 3:
		g_pChatListBox->AddText("", GlobalText[496], SEASON3B::TYPE_ERROR_MESSAGE);
		break;

	case 4:
		g_pChatListBox->AddText("", GlobalText[2108], SEASON3B::TYPE_ERROR_MESSAGE);
		break;
	}

	SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

	g_MessageBox->PopMessageBox();

	g_pNewUISystem->Hide(SEASON3B::INTERFACE_TRADE);
}

void CNewUITrade::ProcessToReceiveTradeItems(int nIndex, BYTE* pbyItemPacket)
{
	SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

	if (nIndex >= 0)
	{
		int MaxIndex = (m_pMyInvenCtrl->GetNumberOfColumn() * m_pMyInvenCtrl->GetNumberOfRow());

		if (nIndex < MaxIndex)
		{
			if (this->IsItemAutoMove())
			{
				int InvenIndex = -1;
				ITEM* pItemObj = g_pMyInventory->FindItemMousePt(m_nBackupMouseX, m_nBackupMouseY, &InvenIndex);

				if (pItemObj)
				{
					g_pMyInventory->GetInventoryCtrl(InvenIndex)->RemoveItem(pItemObj);
				}
				this->SetItemAutoMove(false);
			}
		}

		m_pMyInvenCtrl->AddItem(nIndex, pbyItemPacket);
	}
}

int SEASON3B::CNewUITrade::GetPointedItemIndexMyInven()
{
	return m_pMyInvenCtrl->GetPointedSquareIndex();
}

int SEASON3B::CNewUITrade::GetPointedItemIndexYourInven()
{
	return m_pYourInvenCtrl->GetPointedSquareIndex();
}

int SEASON3B::CNewUITrade::FindEmptySlot(ITEM* pItemObj)
{
	int iSrcIndex = -1;

	if (pItemObj)
	{
		if (m_pMyInvenCtrl)
		{
			iSrcIndex = m_pMyInvenCtrl->FindEmptySlot(pItemObj);
		}
	}

	return iSrcIndex;
}

void SEASON3B::CNewUITrade::SetItemAutoMove(bool ItemAutoMove)
{
	m_bItemAutoMove = ItemAutoMove;

	if (ItemAutoMove)
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

void SEASON3B::CNewUITrade::InitBackupItemInfo()
{
	m_nBackupItemIndex = -1;
}

bool SEASON3B::CNewUITrade::ProcessTradeItemAutoMove()
{
	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	bool r_succes = false;

	if (pPickedItem && pPickedItem->GetItem())
	{
		r_succes = false;
	}
	else if (this->IsItemAutoMove())
	{
		r_succes = false;
	}
	else
	{
		int nSrcIndex = -1;
		int nDstIndex = -1;
		ITEM* pItemObj = m_pMyInvenCtrl->FindItemAtPt(MouseX, MouseY);

		if (pItemObj && (nDstIndex = g_pMyInventory->FindEmptySlot(pItemObj), nDstIndex != -1))
		{
			this->SetItemAutoMove(true);
			nSrcIndex = pItemObj->x + m_pMyInvenCtrl->GetNumberOfColumn() * pItemObj->y;
			this->SendRequestItemToMyInven(pItemObj, nSrcIndex, nDstIndex + MAX_EQUIPMENT_INDEX);
			PlayBuffer(SOUND_GET_ITEM01);
			r_succes = true;
		}
		else
		{
			r_succes = false;
		}
	}


	return r_succes;
}

bool SEASON3B::CNewUITrade::ProcessInvenItemAutoMove()
{
	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	if (pPickedItem && pPickedItem->GetItem())
	{
		return false;
	}

	if (this->IsItemAutoMove())
	{
		return false;
	}

	int nSrcIndex = -1;
	int nDstIndex = -1;
	int InvenIndex = -1;

	ITEM* pItemObj = g_pMyInventory->FindItemAtPt(MouseX, MouseY, &InvenIndex);

	if (pItemObj)
	{
		nDstIndex = m_pMyInvenCtrl->FindEmptySlot(pItemObj);

		if (IsTradeBan(pItemObj))
		{
			g_pChatListBox->AddText("", GlobalText[494], TYPE_ERROR_MESSAGE, TYPE_ALL_MESSAGE);
		}
		else
		{
			if (nDstIndex != -1)
			{
				this->SetItemAutoMove(true);
				nSrcIndex = g_pMyInventory->GetInvenSlotIndex(pItemObj->x, pItemObj->y, InvenIndex);
				this->SendRequestItemToTrade(pItemObj, nSrcIndex + MAX_EQUIPMENT_INDEX, nDstIndex);
				return true;
			}
		}
	}
	return false;
}

bool SEASON3B::CNewUITrade::ProcessTradeItemAutoMoveSuccess()
{
	if (IsVisible())
	{
		if (this->IsItemAutoMove())
		{
			ITEM* pItemObj = m_pMyInvenCtrl->FindItemAtPt(MouseX, MouseY);

			if (pItemObj)
			{
				m_pMyInvenCtrl->RemoveItem(pItemObj);
			}
			this->SetItemAutoMove(false);
			return true;
		}
	}
	return false;
}

bool SEASON3B::CNewUITrade::ProcessTradeItemAutoMoveFailure()
{
	if (IsVisible())
	{
		this->InitBackupItemInfo();
		this->SetItemAutoMove(false);
		return true;
	}

	return false;

}

bool SEASON3B::CNewUITrade::IsItemAutoMove()
{
	return m_bItemAutoMove;
}
