
#if !defined(AFX_NEWUIINVENEXPANSION_H__1151C4F9_04A5_47B1_A717_E7905BEEAD08__INCLUDED_)
#define AFX_NEWUIINVENEXPANSION_H__1151C4F9_04A5_47B1_A717_E7905BEEAD08__INCLUDED_
#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"

namespace SEASON3B
{
	class CInvenOrgInterface
	{
	public:
		CInvenOrgInterface();
		~CInvenOrgInterface();
		void SetOrgInvenPos(int InterfaceList, float posX, float posY, float myPosX, float myPosY);
		float GetOrgInvenPosX();
		float GetOrgInvenPosY();
		float GetOrgYourInvenPosX();
		float GetOrgYourInvenPosY();
		int GetInterfaceList();
	private:
		float posX;
		float posY;
		float myPosX;
		float myPosY;
		DWORD InterfaceList;
	};

	class CNewUIInvenExpansion : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			item_add_table = BITMAP_INVENTORY_EXT_BEGIN,
			item_add_marking_non = BITMAP_INVENTORY_EXT_BEGIN + 1,
			item_add_marking_no01 = BITMAP_INVENTORY_EXT_BEGIN + 2,
			item_add_marking_no02 = BITMAP_INVENTORY_EXT_BEGIN + 3,
			item_add_marking_no03 = BITMAP_INVENTORY_EXT_BEGIN + 4,
			item_add_marking_no04 = BITMAP_INVENTORY_EXT_BEGIN + 5,
			IMAGE_STORAGE_BTN_LOCK = BITMAP_INTERFACE_NEW_STORAGE_BEGIN + 3,
		};
		typedef std::map<int, CInvenOrgInterface> type_map_inven;
	public:
		CNewUIInvenExpansion();
		virtual~CNewUIInvenExpansion();
		bool Create(CNewUIManager* pUIManager, int posX, int posY);
		void Release();
		void SetPos(int posX, int posY);
		void SetButtonInfo();
		bool Render();
		bool Update();
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		float GetLayerDepth();

		bool BtnProcess();
		void ResetLButton();
		void ResetRButton();
		bool CheckExpansionInventory();
		void InitExpansion();
		void LoadImages();
		void UnloadImages();
		void RenderFrame();
		void RenderTexts();
		void RenderButtons();
		void RenderInvenExpansion();
		void OpeningProcess();
		void ClosingProcess();
		void InitMultiPosition();
		bool SetThirdPosition(DWORD winIndex);
		int IsThirdInterface(int winIndex);
		bool GetThirdInterface() {
			return this->ThirdInterface;
		};

		void RegLinkageUI();
		int FindLinkageUI(int winIndex);
		tagPOINT* GetUIPosition(int index, bool isMyPos);
		bool SetEnableExpansionInven(BYTE count, DWORD Price);
		tagPOINT* GetPos();
		tagPOINT* GetIEStartPos(int index);
	private:
		/*+012*/	CNewUIManager* m_pNewUIMng;
		/*+016*/	bool  isExtEnabled[MAX_INVENT_EXPANSION];
		/*+020*/	POINT m_Pos;
		/*+028*/	POINT m_InvBoxPos[MAX_INVENT_EXPANSION];
		/*+052*/	CNewUIButton m_BtnExit;
		/*+224*/	bool ThirdInterface;
		/*+228*/	CInvenOrgInterface myOrgInterface;
		/*+248*/	type_map_inven InvenOrgInterface;
		/*+280*/	std::list<int> m_List;



		/*+052*/	CNewUIButton m_BtnPurchase;
		/*+224*/	bool m_BuyExpansionActive;
		/*+224*/	bool m_CheckBuyExpansion;
		/*+224*/	DWORD m_PurchaseCoin;
	};
}

#endif // !defined(AFX_NEWUIINVENEXPANSION_H__1151C4F9_04A5_47B1_A717_E7905BEEAD08__INCLUDED_)
