//*****************************************************************************
// File: NewUITrade.h
//
// Desc: interface for the CNewUITrade class.
//		 °Å·¡Ã¢ Å¬·¡½º.
//
// producer: Ahn Sang-Kyu
//*****************************************************************************

#if !defined(AFX_NEWUITRADE_H__25FC9B24_8F86_4791_B246_689326623DFB__INCLUDED_)
#define AFX_NEWUITRADE_H__25FC9B24_8F86_4791_B246_689326623DFB__INCLUDED_

#pragma once

#include "NewUIBase.h"
#include "NewUIMessageBox.h"
#include "NewUIMyInventory.h"
#include "NewUIMyQuestInfoWindow.h"
#include "NewUIStorageInventory.h"

namespace SEASON3B
{
	class CNewUITrade : public CNewUIObj  
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_TRADE_BACK = CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK,	// Reference
			IMAGE_TRADE_TOP = CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP,
			IMAGE_TRADE_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,
			IMAGE_TRADE_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,
			IMAGE_TRADE_BOTTOM = CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM,

			IMAGE_TRADE_LINE = CNewUIMyQuestInfoWindow::IMAGE_MYQUEST_LINE,
			IMAGE_TRADE_NICK_BACK = BITMAP_INTERFACE_NEW_TRADE_BEGIN,
			IMAGE_TRADE_MONEY = CNewUIMyInventory::IMAGE_INVENTORY_MONEY,
			IMAGE_TRADE_CONFIRM = BITMAP_INTERFACE_NEW_TRADE_BEGIN + 1,
			IMAGE_TRADE_WARNING_ARROW = BITMAP_CURSOR+7,

			IMAGE_TRADE_BTN_CLOSE = CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN,
			IMAGE_TRADE_BTN_ZEN_INPUT = CNewUIStorageInventory::IMAGE_STORAGE_BTN_INSERT_ZEN,
		};
		
	private:
		enum
		{
			TRADE_WIDTH = WIN_WINDOW_SIZEX,
			TRADE_HEIGHT = WIN_WINDOW_SIZEY,
			CONFIRM_WIDTH = 36,
			CONFIRM_HEIGHT = 29,
			COLUMN_TRADE_INVEN = 8,
			ROW_TRADE_INVEN = 4,
			MAX_TRADE_INVEN = COLUMN_TRADE_INVEN * ROW_TRADE_INVEN,
		};

		enum TRADE_BUTTON
		{
			BTN_CLOSE = 0,			// Ã¢ ´Ý±â.
			BTN_ZEN_INPUT,			// Á¨ ÀÔ·Â.
			MAX_BTN
		};

		CNewUIManager*			m_pNewUIMng;
		POINT					m_Pos;
		CNewUIButton			m_abtn[MAX_BTN];
		POINT					m_posMyConfirm;
		CNewUIInventoryCtrl*	m_pYourInvenCtrl;
		CNewUIInventoryCtrl*	m_pMyInvenCtrl;
		ITEM					m_aYourInvenBackUp[MAX_TRADE_INVEN];
		/*+3808*/ char m_szYourID[MAX_ID_SIZE + 1];
		/*+3820*/ int  m_nYourLevel;
		/*+3824*/ int  m_nYourGuildType;
		/*+3828*/ int  m_nYourTradeGold;
		/*+3832*/ int  m_nMyTradeGold;
		/*+3836*/ int  m_nTempMyTradeGold;
		/*+3844*/ int  m_nMyTradeWait;
		/*+3840*/ bool m_bYourConfirm;
		/*+3841*/ bool m_bMyConfirm;
		/*+3848*/ bool m_bTradeAlert;
		/*+3849*/ bool m_bItemAutoMove;
		/*+3852*/ int  m_nBackupItemIndex;
		/*+3856*/ int  m_nBackupMouseX;
		/*+3860*/ int  m_nBackupMouseY;
		//==================================
		/*+3832*/ int  m_nMyTradeCoin[3];
		/*+3828*/ int  m_nYourTradeCoin[3];
		/*+3836*/ int  m_nTempMyTradeCoin[3];

		std::chrono::steady_clock::time_point last_time;
	public:
		CNewUITrade();
		virtual ~CNewUITrade();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		
		void SetPos(int x, int y);
		
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		
		float GetLayerDepth();	//. 2.1f
		void EventOrderWindows(double WindowsX, double WindowsY);

		static void UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB);
		
		CNewUIInventoryCtrl* GetYourInvenCtrl() const;
		CNewUIInventoryCtrl* GetMyInvenCtrl() const;

		void ProcessCloseBtn();
		void ClosingProcess();

		void GetYourID(char* pszYourID);
		void SetYourTradeGold(int nGold, int nCoin1, int nCoin2, int nCoin3);

		void SendRequestMyGoldInput(int nInputGold);
		void SendRequestMyGoldInput(int nGold, int nCoin1, int nCoin2, int nCoin3);
		void SendRequestItemToMyInven(ITEM* pItemObj, int nTradeIndex, int nInvenIndex);

		void ProcessToReceiveTradeRequest(BYTE* pbyYourID);
		void ProcessToReceiveTradeResult(LPPTRADE pTradeData);
		void ProcessToReceiveYourItemDelete(BYTE byYourInvenIndex);
		void ProcessToReceiveYourItemAdd(BYTE byYourInvenIndex, BYTE* pbyItemPacket);
		void ProcessToReceiveMyTradeGold(BYTE bySuccess);
		void ProcessToReceiveYourConfirm(BYTE byState);
		void ProcessToReceiveTradeExit(BYTE byState);
		void ProcessToReceiveTradeItems(int nIndex, BYTE* pbyItemPacket);

		void AlertTrade();

		int GetPointedItemIndexMyInven();
		int GetPointedItemIndexYourInven();
		int FindEmptySlot(ITEM* pItemObj);
		void SetItemAutoMove(bool ItemAutoMove);
		void InitBackupItemInfo();
		bool ProcessTradeItemAutoMove();
		bool ProcessInvenItemAutoMove();

		bool ProcessTradeItemAutoMoveSuccess();
		bool ProcessTradeItemAutoMoveFailure();
		bool IsItemAutoMove();
	private:
		void LoadImages();
		void UnloadImages();

		void RenderBackImage();
		void RenderText();
		void RenderWarningArrow();

		void ProcessMyInvenCtrl();
		bool ProcessBtns();

		void ConvertYourLevel(int& rnLevel, DWORD& rdwColor);
		void InitTradeInfo();
		void InitYourInvenBackUp();
		void BackUpYourInven(int nYourInvenIndex);
		void BackUpYourInven(ITEM* pYourItemObj);
		void AlertYourTradeInven();
		void SendRequestItemToTrade(ITEM* pItemObj, int nInvenIndex, int nTradeIndex);
		void RenderGold(int x, int y, DWORD nGold, int index);
	};
}

#endif // !defined(AFX_NEWUITRADE_H__25FC9B24_8F86_4791_B246_689326623DFB__INCLUDED_)
