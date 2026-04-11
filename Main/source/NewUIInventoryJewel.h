#if !defined(AFX_NEWUIINVENTORYJEWEL_H__1151C4F9_04A5_47B1_A717_E7905BEEAD08__INCLUDED_)
#define AFX_NEWUIINVENTORYJEWEL_H__1151C4F9_04A5_47B1_A717_E7905BEEAD08__INCLUDED_
#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "NewUIMyInventory.h"

namespace SEASON3B
{
	class WareHoly
	{
	public:
		WareHoly() :bKeyIndex(0), bItemindex(-1), bItemLevel(-1), bItemCount(0) {};
		WareHoly(BYTE KeyIndex, int itemindex, int itemLevel, __int64 ItemCount) :
			bKeyIndex(KeyIndex), bItemindex(itemindex), bItemLevel(itemLevel), bItemCount(ItemCount){
		};
		~WareHoly() {};

		void setIndex(int index) {
			bItemindex = index;
		};
		int GetIndex() {
			return bItemindex;
		};
		int GetLevel() {
			if (bItemLevel != -1)
				return bItemLevel;
			else
				return 0;
		};
		void setValue(__int64 value) {
			bItemCount = value;
		};
		__int64 GetValue() {
			return bItemCount;
		};

	private:
		BYTE bKeyIndex;
		int bItemindex;
		int bItemLevel;
		__int64 bItemCount;
	};

	class CNewUIInventoryJewel : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_INVENTORY_BACK = CNewUIMyInventory::IMAGE_INVENTORY_BACK,
			IMAGE_INVENTORY_BACK_TOP2 = CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP2,
			IMAGE_INVENTORY_BACK_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,
			IMAGE_INVENTORY_BACK_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,
			IMAGE_INVENTORY_BACK_BOTTOM = CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM,
			IMAGE_OPTION_BUTTON = BITMAP_MACRO_MAIN_BEGIN + 1,

		};
	public:
		CNewUIInventoryJewel();
		virtual~CNewUIInventoryJewel();
	private:
	/*+012*/ CNewUIManager* m_pNewUIMng;
	/*+020*/ POINT m_Pos;
	int		m_dwCurIndex;
	int		m_dwSelIndex;
	int		m_nSelPage;
	int		m_nMaxPage;
	int		m_nOptionCurrent;
	RECT	m_nRectItem;

	CNewUIButton m_Button1;
	CNewUIButton m_Button2;
	CNewUIButton m_ButtonNext;
	CNewUIButton m_ButtonBack;

	std::vector<WareHoly>m_bItems;
	public:
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Initialize();
		void Release();
		void SetPos(int x, int y);
		void InitButtons();
		bool Render();
		bool Update();
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		float GetLayerDepth();
		bool CheckExpansionInventory();
		void OpenningProcess();
		void ClosingProcess();
		void RemoveData();
		void InsertData(BYTE Index, short ItemIndex, short ItemLevel, __int64 count);
		bool check_budget(DWORD _iCount);
	private:
		void RenderFrame();
		void RenderTexts();
		void RenderInter();
		void RenderButtons();
		void RenderHoly();
		void FrameTarget(float iPos_x, float iPos_y, float width, float height, DWORD color);
		void render_option_group();
	};
}

#endif