// NewUICharacterInfoWindow.h: interface for the CNewUICharacterInfoWindow class.
//////////////////////////////////////////////////////////////////////
#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIMessageBox.h"
#include "NewUIMyInventory.h"
#include "NewUIButton.h"
#include "NewUIScrollBar.h"

namespace SEASON3B
{
	class CHARACTER_ADVANCE_DESC
	{
	public:
		BOOL Normal;
		BOOL IsTitle;
		DWORD Value;
		std::string name;

		CHARACTER_ADVANCE_DESC() :name(""), Normal(TRUE), Value(0), IsTitle(FALSE){
		}
		CHARACTER_ADVANCE_DESC(const std::string& strText, BOOL normal, BOOL title, DWORD value)
			:name(strText), Normal(normal), IsTitle(title), Value(value)
		{
		}
		CHARACTER_ADVANCE_DESC(const std::string& strText, BOOL normal, DWORD value)
			:name(strText), Normal(normal), Value(value), IsTitle(FALSE)
		{
		}
		~CHARACTER_ADVANCE_DESC() {
		}
		BOOL CheckTittle() const {
			return IsTitle;
		}
		void Render(int x, int y, int width, bool back);
	};

	class CNewUICharacterInfoWindow : public CNewUIObj  
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_CHAINFO_BACK = CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK,			// newui_msgbox_back.jpg
			IMAGE_CHAINFO_TOP = CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP2,		// newui_item_back04.tga	(190,64)
			IMAGE_CHAINFO_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,		// newui_item_back02-l.tga	(21,320)
			IMAGE_CHAINFO_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,	// newui_item_back02-r.tga	(21,320)
			IMAGE_CHAINFO_BOTTOM = CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM,	// newui_item_back03.tga	(190,45)

			IMAGE_CHAINFO_TABLE_TOP_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_LEFT,	//. newui_item_table01(L).tga (14,14)
			IMAGE_CHAINFO_TABLE_TOP_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_RIGHT,	//. newui_item_table01(R).tga (14,14)
			IMAGE_CHAINFO_TABLE_BOTTOM_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_LEFT,	//. newui_item_table02(L).tga (14,14)
			IMAGE_CHAINFO_TABLE_BOTTOM_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_RIGHT,	//. newui_item_table02(R).tga (14,14)
			IMAGE_CHAINFO_TABLE_TOP_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_PIXEL,			//. newui_item_table03(up).tga (1, 14)
			IMAGE_CHAINFO_TABLE_BOTTOM_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_PIXEL,	//. newui_item_table03(dw).tga (1,14)
			IMAGE_CHAINFO_TABLE_LEFT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_LEFT_PIXEL,		//. newui_item_table03(L).tga (14,1)
			IMAGE_CHAINFO_TABLE_RIGHT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_RIGHT_PIXEL,		//. newui_item_table03(R).tga (14,1)

			IMAGE_CHAINFO_BTN_EXIT = CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN,
			IMAGE_MAIN_TABLE_TOP_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_LEFT,	//. newui_item_table01(L).tga (14,14)
			IMAGE_MAIN_TABLE_TOP_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_RIGHT,	//. newui_item_table01(R).tga (14,14)
			IMAGE_MAIN_TABLE_BOTTOM_LEFT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_LEFT,	//. newui_item_table02(L).tga (14,14)
			IMAGE_MAIN_TABLE_BOTTOM_RIGHT = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_RIGHT,	//. newui_item_table02(R).tga (14,14)
			IMAGE_MAIN_TABLE_TOP_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_TOP_PIXEL,			//. newui_item_table03(up).tga (1, 14)
			IMAGE_MAIN_TABLE_BOTTOM_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_BOTTOM_PIXEL,	//. newui_item_table03(dw).tga (1,14)
			IMAGE_MAIN_TABLE_LEFT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_LEFT_PIXEL,		//. newui_item_table03(L).tga (14,1)
			IMAGE_MAIN_TABLE_RIGHT_PIXEL = CNewUIInventoryCtrl::IMAGE_ITEM_TABLE_RIGHT_PIXEL,


			IMAGE_CHAINFO_TEXTBOX = BITMAP_INTERFACE_NEW_CHAINFO_WINDOW_BEGIN,
			IMAGE_CHAINFO_BTN_STAT,
			IMAGE_CHAINFO_BTN_QUEST,
			IMAGE_CHAINFO_BTN_PET,
			IMAGE_CHAINFO_BTN_MASTERLEVEL,
		};

		enum
		{
			CHAINFO_WINDOW_WIDTH = WIN_WINDOW_SIZEX,
			CHAINFO_WINDOW_HEIGHT = WIN_WINDOW_SIZEY,
			HEIGHT_STRENGTH = 120,
			HEIGHT_DEXTERITY = 175,
			HEIGHT_VITALITY = 240,
			HEIGHT_ENERGY = 295,
			HEIGHT_CHARISMA = 350,
			BTN_STAT_COUNT = 5,
			STAT_STRENGTH = 0,
			STAT_DEXTERITY,
			STAT_VITALITY,
			STAT_ENERGY,
			STAT_CHARISMA,
		};
		
	private:
		CNewUIManager*				m_pNewUIMng;
		POINT						m_Pos;

		CNewUIButton m_BtnStat[BTN_STAT_COUNT];
		CNewUIButton m_BtnExit;
		CNewUIButton m_BtnQuest;
		CNewUIButton m_BtnPet;
#if MAIN_UPDATE > 303
		CNewUIButton m_BtnMasterLevel;
#endif // MAIN_UPDATE > 303
		int m_inputIndex;
		/*+1572*/	int			m_iCurTab;
		/*+1576*/	CNewUIScrollBar m_pScrollBar;
		/*+1580*/	std::vector<CHARACTER_ADVANCE_DESC> m_advanceChar;
		std::chrono::steady_clock::time_point last_time;
	public:
		CNewUICharacterInfoWindow();
		virtual ~CNewUICharacterInfoWindow();
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void SetPos(int x, int y);
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		float GetLayerDepth();	//. 5.1f
		void EventOrderWindows(double WindowsX, double WindowsY);
		void OpenningProcess();
		void OpenInputStatAdd(int index);
		int GetInputIndex();
		void ReceiveAdvanceChar(BYTE* ReceiveBuffer);
	private:
		void LoadImages();
		void UnloadImages();
		void ResetEquipmentLevel();
		void SetButtonInfo();
		bool BtnProcess();
		void RenderFrame();
		void RenderTexts();
		void RenderSubjectTexts();
		void RenderTableTexts();
		void RenderAttribute();
		void RenderButtons();
		void RenderAdavanceInfo();
		void RenderEquipementAttribute();
		void RenderTable(float x, float y, float width, float height);
	};
}

