// NewUIPartyListWindow.h: interface for the CNewUIPartyInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NEWUIPARTYMINIWINDOW_H_)
#define _NEWUIPARTYMINIWINDOW_H_

#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "NewUIQuestProgress.h"
#include "NewUIPartyInfoWindow.h"

namespace SEASON3B
{
	class CNewUIPartyListWindow : public CNewUIObj  
	{
	public:
		enum IMAGE_LIST
		{	
			// 기타 이미지
			IMAGE_PARTY_LIST_FLAG = CNewUIPartyInfoWindow::IMAGE_PARTY_FLAG,		// newui_party_flag.tga			(9, 10)	
			IMAGE_PARTY_LIST_EXIT = CNewUIPartyInfoWindow::IMAGE_PARTY_EXIT,		// newui_party_x.tga			(11, 11)
			IMAGE_ND_BTN_L = CNewUIQuestProgress::IMAGE_QP_BTN_L,				// Quest_bt_L.tga	(17,36)
			IMAGE_ND_BTN_R = CNewUIQuestProgress::IMAGE_QP_BTN_R,
			// 파티미니창
			IMAGE_PARTY_LIST_BACK = BITMAP_PARTY_MINILIST_BEGIN,					// newui_party_back.tga			(77, 23)
			IMAGE_PARTY_LIST_HPBAR,													// newui_party_hpbar.jpg		(69, 3)										
		};
	private:
#if MAIN_UPDATE > 303
		enum PARTY_WINDOW_SIZE
		{
			PARTY_LIST_WINDOW_WIDTH = 77,
			PARTY_LIST_WINDOW_HEIGHT = 23,
			PARTY_LIST_HP_BAR_WIDTH = 69,
			PARTY_LIST_HP_BAR_HEIGHT = 3,
		};
#else
		enum PARTY_WINDOW_SIZE
		{
			PARTY_LIST_WINDOW_WIDTH = 50,
			PARTY_LIST_WINDOW_HEIGHT = 19,
			PARTY_LIST_HP_BAR_WIDTH = 69,
			PARTY_LIST_HP_BAR_HEIGHT = 3,
		};
#endif

		enum PARTY_BG_COLOR
		{
			PARTY_LIST_BGCOLOR_DEFAULT = 0,
			PARTY_LIST_BGCOLOR_RED,
			PARTY_LIST_BGCOLOR_GREEN
		};
		
	private:
		CNewUIManager*				m_pNewUIMng;
		POINT						m_Pos;
#if MAIN_UPDATE > 303
		CNewUIButton				m_BtnPartyExit[MAX_PARTYS];
#endif
		bool m_bActive;
		bool m_ibEnable;
		bool m_bEnableMove;
		bool m_bMoveWindow;
		int  m_iVal;
		int  m_iSelectedCharacter;
		int  m_iLimitUserIDHeight[2];
		int  m_iMouseClickPos_x;
		int  m_iMouseClickPos_y;
	public:
		CNewUIPartyListWindow();
		virtual ~CNewUIPartyListWindow();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		
		void SetPos(int x, int y);
		void SetPos(int x);
		int GetPosX();
		
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		bool BtnProcess();
		
		float GetLayerDepth();	//. 5.4f
		void EventOrderWindows(double WindowsX, double WindowsY);

		void OpenningProcess();
		void ClosingProcess();	
		
		int GetSelectedCharacter();
		int GetRightBottomPos(POINT* pos);

		bool IsEnableMove()
		{
			return m_bEnableMove;
		};

#if MAIN_UPDATE <= 303
		void RenderFrame(float RenderFrameX, float RenderFrameY);
		bool runtime_mouse_target(float RenderFrameX, float RenderFrameY);
#endif
	private:
		void LoadImages();
		void UnloadImages();
		bool SelectCharacterInPartyList(PARTY_t* pMember);
		bool MoveWindows();
	};
}

#endif // !defined(_NEWUIPARTYMINIWINDOW_H_)
