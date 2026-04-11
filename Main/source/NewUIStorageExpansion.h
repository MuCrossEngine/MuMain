#if !defined(AFX_NEWUISTORAGEEXPANSION_H__BD790479_EDDE_4981_9B03_A12163A58D5D__INCLUDED_)
#define AFX_NEWUISTORAGEEXPANSION_H__BD790479_EDDE_4981_9B03_A12163A58D5D__INCLUDED_

#pragma once

#include "NewUIBase.h"
#include "NewUIButton.h"
#include "NewUIInventoryCtrl.h"


namespace SEASON3B
{
	class CNewUIStorageExpansion : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_STORAGE_BACK = CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK,	// Reference
			IMAGE_STORAGE_TOP = CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP,
			IMAGE_STORAGE_LEFT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT,
			IMAGE_STORAGE_RIGHT = CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT,
			IMAGE_STORAGE_BOTTOM = CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM,
			IMAGE_STORAGE_EXIT_BTN = CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN,
		};
	private:
		/*+012*/ CNewUIManager* m_pNewUIMng;
		/*+016*/ POINT m_Pos;
		/*+024*/ CNewUIButton m_BtnExit;
		/*+196*/ bool m_bLock;
	public:
		CNewUIStorageExpansion();
		virtual~CNewUIStorageExpansion();
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void SetPos(int x, int y);

		tagPOINT* GetPos();

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		float GetLayerDepth();	//. 2.2f
		void OpenningProcess();
		void ClosingProcess();
		bool CheckExpansionInventory();
		bool IsExpansionStorage();
		void SetExpansionStorage(bool Success);
	private:
		void LoadImages();
		void UnloadImages();
		void RenderFrame();
		void RenderTexts();
		void RenderButtons();
		bool BtnProcess();
		void SetBtnInfo();
	};
}
#endif