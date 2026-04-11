// NewUINameWindow.h: interface for the CNewUINameWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWUINAMEWINDOW_H__76B140FF_46CB_4DB6_9DA2_5F84F294D212__INCLUDED_)
#define AFX_NEWUINAMEWINDOW_H__76B140FF_46CB_4DB6_9DA2_5F84F294D212__INCLUDED_

#pragma once

#include "NewUIManager.h"
#include "ZzzTexture.h"

namespace SEASON3B
{
	class CNewUINameWindow  : public CNewUIObj
	{
	public:
		CNewUINameWindow();
		virtual ~CNewUINameWindow();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void LoadImages();
		void SetPos(int x, int y);
		
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		void RenderTooltip(float x, float y, float width, float height, bool fixed = false);
		void RenderTooltip_old(float x, float y, float width, float height, bool fixed = false);
		float GetLayerDepth();
		bool GetItemNameVisible() {
			return m_bShowItemName;
		};
	private:
		void RenderName();

		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		bool m_bShowItemName;
	};
	
}

#endif // !defined(AFX_NEWUINAMEWINDOW_H__76B140FF_46CB_4DB6_9DA2_5F84F294D212__INCLUDED_)
