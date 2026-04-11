#pragma once

#include "NewUIBase.h"
#include "NewUIButton.h"
#include "NewUIMyInventory.h"

namespace SEASON3B
{

	class CNewUIInventoryCore : public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_ITEM_CORE = CNewUIMyInventory::IMAGE_ITEM_CORE,
			IMAGE_ITEM_DECOR_CORE = CNewUIMyInventory::IMAGE_ITEM_DECOR_CORE,
		};
	private:
		/*+000*/ CNewUIManager* m_pNewUIMng;
		/*+036*/ POINT m_Pos;
		/*+460*/ CNewUIButton m_BtnExit;
	public:
		CNewUIInventoryCore();
		virtual~CNewUIInventoryCore();
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void SetPos(int x, int y);
		const POINT& GetPos() const;

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		float GetLayerDepth();

		void OpeningProcess();
		void ClosingProcess();
		bool CheckExpansionInventory();
	protected:
		void SetButtonInfo();
		void RenderFrame();
		void RenderTexts();
		void RenderButtons();
		void RenderInven();
	};
}

