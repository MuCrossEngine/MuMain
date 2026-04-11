//*****************************************************************************
// File: NewUIStorageInventory.h
//*****************************************************************************

#if !defined(AFX_NEWUISTORAGEINVENTORY_H__BD790479_EDDE_4981_9B03_A12163A58D5D__INCLUDED_)
#define AFX_NEWUISTORAGEINVENTORY_H__BD790479_EDDE_4981_9B03_A12163A58D5D__INCLUDED_

#pragma once

#include "NewUIBase.h"
#include "NewUIInventoryCtrl.h"
#include "NewUIMessageBox.h"
#include "NewUIMyInventory.h"
#include "NewUIButton.h"

namespace SEASON3B
{
	class CNewUIStorageInventory : public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_STORAGE_BACK = CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK,	// Reference
			IMAGE_STORAGE_TOP = CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP,
			IMAGE_STORAGE_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,
			IMAGE_STORAGE_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,
			IMAGE_STORAGE_BOTTOM = CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM,
			IMAGE_STORAGE_EXPANSION = CNewUIMyInventory::IMAGE_INVENTORY_EXPANSION_BTN,

			IMAGE_STORAGE_BTN_INSERT_ZEN = BITMAP_INTERFACE_NEW_STORAGE_BEGIN,
			IMAGE_STORAGE_BTN_TAKE_ZEN = BITMAP_INTERFACE_NEW_STORAGE_BEGIN + 1,
			IMAGE_STORAGE_BTN_UNLOCK = BITMAP_INTERFACE_NEW_STORAGE_BEGIN + 2,
			IMAGE_STORAGE_BTN_LOCK = BITMAP_INTERFACE_NEW_STORAGE_BEGIN + 3,

			IMAGE_STORAGE_MONEY = BITMAP_INTERFACE_NEW_STORAGE_BEGIN + 4,
		};
		
	private:
		enum
		{
			STORAGE_WIDTH = WIN_WINDOW_SIZEX,
			STORAGE_HEIGHT = WIN_WINDOW_SIZEY,
		};

		enum STORAGE_BUTTON
		{
			BTN_INSERT_ZEN = 0,
			BTN_TAKE_ZEN,
			BTN_LOCK,
			MAX_BTN
		};
		
		CNewUIManager*			m_pNewUIMng;
		POINT					m_Pos;
		CNewUIButton			m_abtn[MAX_BTN];
		CNewUIInventoryCtrl*	m_pNewInventoryCtrl[MAX_WHAREHOUSE_EXPANSION];
		CNewUIButton			m_BtnExpansion;
		bool					m_bLock;
		bool					m_bCorrectPassword;
		bool					m_bItemAutoMove;
		int						m_nBackupMouseX;
		int						m_nBackupMouseY;
		bool					m_bTakeZen;
		int						m_nBackupTakeZen;
		int						m_nBackupInvenIndex;
		/*+0980*/	BOOL m_ExpansionState;
		/*+0984*/	int m_InvenIndex;
	public:
		CNewUIStorageInventory();
		virtual ~CNewUIStorageInventory();
		
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void SetPos(int x, int y);

		void SetExpansionOpenState();
		void SetExpansionCloseState();
		void LockBtnExpansion();
		void UnLockBtnExpansion();


		int FindInvenIndex(int index);
		int SlotCntSendDstInvenIndex(int index, int InvenIndex);
		int SlotCntSendSrcInvenIndex(int index, int InvenIndex);
		int SlotCntReceiveInvenIndex(int index);
		int IsInvenctrlSame(CNewUIInventoryCtrl* InventoryCtrl);
		ITEM* FindItemAtPt(int x, int y, int* InvenIndex);
		int GetInvenSlotIndex(unsigned __int8 cx, unsigned __int8 cy, int InvenIndex);
		void RenderExpansionInven();
		int GetSourceStorage(int index, CNewUIInventoryCtrl* InventoryCtrl);
		int GetPickItemTarget(CNewUIPickedItem* pPickedItem);

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		float GetLayerDepth();	//. 2.2f
		void EventOrderWindows(double WindowsX, double WindowsY);

		CNewUIInventoryCtrl* GetInventoryCtrl(int InvenIndex) const;

		bool ClosingProcess();
		bool InsertItem(int iIndex, BYTE* pbyItemPacket);
		int FindEmptySlot(ITEM* pItemObj);

		bool IsLockStorage(){ return m_bLock; }
		bool IsCorrectPassword(){ return m_bCorrectPassword; }
		bool IsItemAutoMove(){ return m_bItemAutoMove; }
		void SetBackupTakeZen(int nZen);
		void ProcessMyInvenItemAutoMove();
		void SendRequestItemToMyInven(ITEM* pItemObj, int nStorageIndex, int nInvenIndex);
		void ProcessToReceiveStorageStatus(BYTE byStatus);
		void ProcessToReceiveStorageItems(int nIndex, BYTE* pbyItemPacket);
		void ProcessStorageItemAutoMoveSuccess();
		void ProcessStorageItemAutoMoveFailure();
		int GetPointedItemIndex();
		void SetItemAutoMove(bool bItemAutoMove);
	private:
		void LoadImages();
		void UnloadImages();

		void RenderBackImage();
		void RenderText();

		void ChangeLockBtnImage();
		void DeleteAllItems();

		void LockStorage(bool bLock);
		void SetCorrectPassword(bool bCorrectPassword);
		void InitBackupItemInfo();
		int GetBackupTakeZen();
		void SetBackupInvenIndex(int nInvenIndex);
		int GetBackupInvenIndex();
		void ProcessInventoryCtrl();
		bool ProcessBtns();
		void ProcessStorageItemAutoMove();
		void SendRequestItemToStorage(ITEM* pItemObj, int nInvenIndex, int nStorageIndex);
	};
}

#endif // !defined(AFX_NEWUISTORAGEINVENTORY_H__BD790479_EDDE_4981_9B03_A12163A58D5D__INCLUDED_)
