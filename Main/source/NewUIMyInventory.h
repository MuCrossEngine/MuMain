// NewUIPersonalInventory.h: interface for the CNewUIMyInventory class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWUIMYINVENTORY_H__74DA6D7A_CF5A_46E9_8C72_9D38F0DC95EC__INCLUDED_)
#define AFX_NEWUIMYINVENTORY_H__74DA6D7A_CF5A_46E9_8C72_9D38F0DC95EC__INCLUDED_

#pragma once

#include "NewUIBase.h"
#include "NewUIInventoryCtrl.h"
#include "NewUIMessageBox.h"
#include "NewUI3DRenderMng.h"
#include "NewUIButton.h"


namespace SEASON3B
{
	class CNewUIMyInventory : public CNewUIObj, public INewUI3DRenderObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_INVENTORY_MYSHOP_OPEN_BTN = BITMAP_MYSHOPINTERFACE_NEW_PERSONALINVENTORY_BEGIN + 1,
			IMAGE_INVENTORY_MYSHOP_CLOSE_BTN = BITMAP_MYSHOPINTERFACE_NEW_PERSONALINVENTORY_BEGIN + 2,
			IMAGE_INVENTORY_BACK = CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK,	// Reference
			IMAGE_INVENTORY_BACK_TOP = BITMAP_INTERFACE_NEW_PERSONALINVENTORY_BEGIN,	//"newui_item_back01.tga"
			IMAGE_INVENTORY_BACK_TOP2,	//"newui_item_back04.tga"
			IMAGE_INVENTORY_BACK_LEFT,	//"newui_item_back02-L.tga"
			IMAGE_INVENTORY_BACK_RIGHT,	//"newui_item_back02-R.tga"
			IMAGE_INVENTORY_BACK_BOTTOM,	//"newui_item_back03.tga"
			IMAGE_INVENTORY_ITEM_BOOT,	//"newui_item_boots.tga"
			IMAGE_INVENTORY_ITEM_HELM,	//"newui_item_cap.tga"
			IMAGE_INVENTORY_ITEM_FAIRY,	//"newui_item_fairy.tga"
			IMAGE_INVENTORY_ITEM_WING,	//"newui_item_wing.tga"
			IMAGE_INVENTORY_ITEM_RIGHT,	//"newui_item_weapon(L).tga"
			IMAGE_INVENTORY_ITEM_LEFT,	//"newui_item_weapon(R).tga"
			IMAGE_INVENTORY_ITEM_ARMOR,	//"newui_item_upper.tga"
			IMAGE_INVENTORY_ITEM_GLOVES,	//"newui_item_gloves.tga"
			IMAGE_INVENTORY_ITEM_PANTS,	//"newui_item_lower.tga"
			IMAGE_INVENTORY_ITEM_RING,	//"newui_item_ring.tga"
			IMAGE_INVENTORY_ITEM_NECKLACE,	//"newui_item_necklace.tga"
			IMAGE_INVENTORY_MONEY,	//"newui_item_money.tga"
			IMAGE_INVENTORY_EXIT_BTN, //"newui_exit_00.tga"
			IMAGE_INVENTORY_REPAIR_BTN, //"newui_repair_00.tga"
			IMAGE_INVENTORY_JEWEL_BTN,
			IMAGE_INVENTORY_EXPANSION_BTN,
			IMAGE_ITEM_PENTAGRAM,
			IMAGE_ITEM_EARRING,
			IMAGE_ITEM_MUUN,
			IMAGE_ITEM_FLAG,
			IMAGE_ITEM_CORE,
			IMAGE_ITEM_DECOR_CORE,
			IMAGE_INVENTORY_BACK2,
			IMAGE_INVENTORY_BORDER,
		};

		enum REPAIR_MODE
		{
			REPAIR_MODE_OFF = 0,
			REPAIR_MODE_ON,
		};

		enum MYSHOP_MODE
		{
			MYSHOP_MODE_OPEN = 0,
			MYSHOP_MODE_CLOSE,
		};

	private:
		enum ITEM_OPTION
		{
			ITEM_SET_OPTION = 1,
			ITEM_SOCKET_SET_OPTION = 2,
		};

		enum
		{
			INVENTORY_WIDTH = WIN_WINDOW_SIZEX,
			INVENTORY_HEIGHT = WIN_WINDOW_SIZEY,
		};

		CNewUIManager* m_pNewUIMng;
		CNewUI3DRenderMng* m_pNewUI3DRenderMng;
		/*+0024*/ CNewUIInventoryCtrl* m_pNewInventoryCtrl[MAX_INVENT_EXPANSION + 1];
		/*+0036*/ POINT m_Pos;
		/*+0044*/ EQUIPMENT_ITEM m_Equipment[MAX_NEW_EQUIPMENT];
		/*+0284*/ int	m_iPointedSlot;
		/*+0288*/ CNewUIButton m_BtnRepair;
		/*+0460*/ CNewUIButton m_BtnExit;
		/*+0632*/ CNewUIButton m_BtnMyShop;
		/*+0804*/ CNewUIButton m_BtnJewel;
		/*+0804*/ CNewUIButton m_BtnExpansion;
		/*+0976*/ BOOL m_StraightMode;
		/*+0980*/ BOOL m_ExpansionState;
		/*+0984*/ int m_InvenIndex;
		/*+0988*/ DWORD unknown_988;
		/*+0992*/ std::list<int> m_ExpansionBack;
		/*+1016*/ MYSHOP_MODE m_MyShopMode;
		/*+1020*/ REPAIR_MODE m_RepairMode;
		/*+1024*/ DWORD m_dwStandbyItemKey;
		/*+1028*/ bool m_bRepairEnableLevel;
		/*+1029*/ bool m_bMyShopOpen;
		/*+1032*/ DWORD m_iBackPointed;
		int m_nBackupMouseX;
		int m_nBackupMouseY;
		bool m_bItemAutoMove;
		int m_CurButtonIndex;
		CNewUIInventoryCtrl* m_bInventoryBase;
		std::vector<int> EquipmentInven;
	public:
		CNewUIMyInventory();
		virtual ~CNewUIMyInventory();

		bool Create(CNewUIManager* pNewUIMng, CNewUI3DRenderMng* pNewUI3DRenderMng, int x, int y);
		void Release();

		bool EquipItem(int iIndex, BYTE* pbyItemPacket, bool equipement = false);
		void UnequipItem(int iIndex);
		void UnequipAllItems();

		bool IsEquipable(int iIndex, ITEM* pItem);

		bool InsertItem(int iIndex, BYTE* pbyItemPacket);
		void DeleteItem(int iIndex);
		void DeleteAllItems();

		void SetPos(int x, int y);
		const POINT& GetPos() const;

		REPAIR_MODE GetRepairMode() const;
		void SetRepairMode(bool bRepair);

		BOOL IsInvenItem(const short sType);
		BOOL IsUsedItem(const short sType);

		void ExpansionInit();
		void SetExpansionOpenState();
		void SetExpansionCloseState();
		void LockBtnExpansion();
		void UnLockBtnExpansion();
		bool IsStraightMode();
		bool CanOpenExpansionInterface();
		int FindInvenIndex(int index);
		int SlotCntSendDstInvenIndex(int index, int InvenIndex);
		int SlotCntSendSrcInvenIndex(int index, int InvenIndex);
		int SlotCntReceiveInvenIndex(int index, int size);
		void RenderExpansionInven();
		void SetInvenPos();
		bool SendStraighItem();
		int GetPickItemTarget(CNewUIPickedItem* pPickedItem);
		int IsInvenctrlSame(CNewUIInventoryCtrl* InventoryCtrl);
		int GetInvenSlotIndex(int cx, int cy, int InvenIndex);
		ITEM* FindItemMousePt(int x, int y, int* InvenIndex);

		void LockInventory(int InvenIndex);
		void UnlockInventory(int InvenIndex);

		int GetItemCount(short int siType, int iLevel);
		int GetNumberOfItems();
		ITEM* GetItem(int lineal_pos);
		bool IsCheckInvenIndex(int InvenIndex);
		ITEM* FindItemAtPt(int x, int y, int* InvenIndex);
		ITEM* FindTypeItem(short int siType);
		void SetInvenEnableCnt(int size);
		int GetInvenEnableCnt();

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		void Render3D();
		bool IsVisible() const;
		void EventOrderWindows(double WindowsX, double WindowsY);
		void RenderItemEquipment(EQUIPMENT_ITEM* equipment, ITEM* pEquippedItem);

		void OpenningProcess();
		void ClosingProcess();

		float GetLayerDepth(); //. 4.2f
		CNewUIInventoryCtrl* GetInventoryCtrl(int index) const;
		ITEM* FindItem(int iLinealPos, int InvenIndex = 0);

		ITEM* FindItemByKey(DWORD dwKey);
		int FindItemIndex(short int siType, int iLevel = -1);
		int FindItemReverseIndex(short sType, int iLevel = -1);
		int FindItemReverseKind2(short sType, int iLevel = -1);
		int FindEmptySlot(IN int cx, IN int cy);
		int FindEmptySlot(ITEM* pItem, bool bcheckPick = false);

		bool IsItem(short int siType, bool bcheckPick = false);
		int	GetNumItemByKey(DWORD dwItemKey);
		int GetNumItemByType(short sItemType);
		int GetPointedItemIndex();
		int FindManaItemIndex();
		static void UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB);
		//. Exporting Functions
		void SetStandbyItemKey(DWORD dwItemKey);
		DWORD GetStandbyItemKey() const;
		int GetStandbyItemIndex();
		ITEM* GetStandbyItem();
		bool IsRepairEnableLevel();
		void SetRepairEnableLevel(bool bOver);
		void ChangeMyShopButtonStateOpen();
		void ChangeMyShopButtonStateClose();
		void LockMyShopButtonOpen();
		void UnlockMyShopButtonOpen();
		void CreateEquippingEffect(int index, ITEM* pItem);
		static bool CanRegisterItemHotKey(int iType);


		bool IsItemAutoMove();
		bool MoveItemProcess();
		void SetItemAutoMove(bool bItemAutoMove);
		bool ProcessItemAutoMove(CNewUIInventoryCtrl* pNewInventory, BYTE SourceFlag);
		bool ProcessMyInvenItemAutoMove(CNewUIInventoryCtrl* pNewInventory, BYTE TargetFlag);
		void ProcessItemAutoMoveSuccess();
		void ProcessItemAutoMoveFailure();

		void SetEquipementRender(int index, bool bRender = true);
		void SetEquipementPosition(int index, int x, int y);
		bool CheckEquipementIndex(int index);
	protected:
		void DeleteEquippingEffect();
		void DeleteEquippingEffectBug(int index, ITEM* pItem);
		void SetEquipmentInfo(EQUIPMENT_ITEM* Equipment, int index, int imageindex, int x, int y, int width, int height);
		void SetEquipmentSlotInfo();
		void SetButtonInfo();
	private:
		void LoadImages();
		void UnloadImages();
		void RenderFrame();
		void RenderSetOption();
		void RenderSocketOption();
		void RenderEquippedItem();
		void RenderButtons();
		void RenderInventoryDetails();
		bool EquipmentWindowProcess();
		bool InventoryProcess();
		bool BtnProcess();
		void RenderItemToolTip(int iSlotIndex);
		bool CanOpenMyShopInterface();
		void ToggleRepairMode();
		void ResetMouseLButton();
		void ResetMouseRButton();
	};
}

#endif // !defined(AFX_NEWUIMYINVENTORY_H__74DA6D7A_CF5A_46E9_8C72_9D38F0DC95EC__INCLUDED_)
