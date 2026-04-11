#pragma once
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "NewUIListGroupButton.h"

#define MIX_INVENTORY_SIZE		120
#define MAX_MIX_NAME_SIZE		32
#define MAX_ITEM_VIEW			10

typedef struct
{
	BYTE         Header[5];
	BYTE         Value;
} PHEADER_MIX_DEFAULT, * LPPHEADER_MIX_DEFAULT;

typedef struct
{
	BYTE         Header[5];
	BYTE         Value;
	BYTE         iSuccessRate;
	BYTE         iRequiredType;
	WORD         iTalismanOfLuck;
	WORD         iTalismanOfChaos;
	WORD         iTalismanLuckRate;
	DWORD        iRequiredZen;
} PHEADER_MIX_INFORMATION, * LPPHEADER_MIX_INFORMATION;

namespace SEASON3B
{
	enum _MIX_SOURCE_STATUS
	{
		MIX_SOURCE_ERROR,
		MIX_SOURCE_NO,
		MIX_SOURCE_PARTIALLY,
		MIX_SOURCE_YES
	};

	enum _SPECIAL_ITEM_RECIPE_
	{
		RCP_SP_EXCELLENT = 1,
		RCP_SP_ADD380ITEM = 2,
		RCP_SP_SETITEM = 4,
		RCP_SP_HARMONY = 8,
		RCP_SP_SOCKETITEM = 16,
	};

	typedef struct
	{
		WORD iMixIndex;
		char iName[MAX_MIX_NAME_SIZE];
	}MIX_BTN_TEMPLATE;

	typedef struct
	{
		int mainIndex;
		int minIndex;
		int maxIndex;
		BYTE minLevel;
		BYTE maxLevel;
		BYTE minDurability;
		BYTE maxDurability;
		BYTE skill;
		BYTE luck;
		BYTE minOpc;
		BYTE maxOpc;
		BYTE exeItem;
		BYTE accItem;
		BYTE minCnt;
		BYTE maxCnt;
	}MIX_RECIPE_ITEM;


	class MIX_RECIPIENT_CHECK
	{
	public:
		MIX_RECIPIENT_CHECK();
		bool IsMixItem(ITEM* pItem);
		bool CheckRecipe(ITEM* pItem);
		void GetSourceName(MIX_RECIPE_ITEM* pRecipeItem);
	public:
		int SpecialItem;
		int iNumMixItem;
		std::string pszNameMix;
		MIX_RECIPE_ITEM pMixItems;
	};

	class MIX_ITEM_ELEMENT
	{
	public:
		MIX_ITEM_ELEMENT() : pItemObj(NULL) {};

		const char* GetName() {
			return m_ItemName.c_str();
		}
	public:
		ITEM* pItemObj;
		std::string m_ItemName;
	};

	class CNewUIMixExpansion : public CNewUIObj
	{
	public:
		CNewUIMixExpansion();
		virtual~CNewUIMixExpansion();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void SetPos(int x, int y);

		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth();
		void OpeningProcess();
		bool ClosingProcess();

		//--
		int FindEmptySlot();
		bool InsertItem(int iLinealPos, BYTE* pbyItemPacket);
		void DeleteItem(int iLinealPos, bool Success = true);
		void DeleteAllItems();
	private:
		bool IsReadyToMix();
		void SetButtonInfo();
		bool ButtonProcess();
		bool InventoryProcess();

		void RecipientCheck();
		void RenderFrame();
		void RenderText();
		void RenderButton();
		void RenderItemList();
		void RenderTargetBox(float iPos_x, float iPos_y, float width, float height, DWORD color = ARGB(0xff, 0xfc, 0x77, 0x02));
		void SendRequestInfo(int index, BYTE Type);

		int GetNumIndexMix() {
			return m_iNumMixIndex;
		}
	public:
		void ReceiveMixOptions(BYTE* ReceiveBuffer);
		void ReceiveMixRecipients(BYTE* ReceiveBuffer);
		void ReceiveInventory(BYTE* ReceiveBuffer);
		bool IsMixItem(ITEM* pItemObj);

		bool IsItemAutoMove();
		void ProcessItemAutoMoveSuccess();
		void ProcessItemAutoMoveFailure();
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int m_iMixState;
		int m_iNumMixIndex;
		int m_iNumItemCnt;
		int m_iRenderItem;
		int m_iTargetItem;
		int m_iRenderTooltipX;
		int m_iRenderTooltipY;
		//-------------------
		WORD  m_iSuccessAddRate;
		WORD  m_iTalismanOfLuck;
		WORD  m_iTalismanOfChaos;
		WORD  m_iTalismanLuckRate;
		BYTE  m_iSuccessRate;
		BYTE  m_iRequiredType;
		DWORD m_iRequiredZen;

		int m_bItemAutoMove;
		int m_BackupTargetItem;
		DWORD m_StateNormal;
		CNewUIButton m_BtnMix;
		CNewUIScrollBar m_pScroll;

		std::vector<DWORD> m_pdwItemCheckBox;
		std::vector<MIX_ITEM_ELEMENT> m_vecItem;

		std::vector<MIX_BTN_TEMPLATE> m_InfoBtnCnt;
		std::vector<MIX_RECIPIENT_CHECK> mixRecipient;
		CNewUIListGroupButton* m_mixOptionList;

	};

}