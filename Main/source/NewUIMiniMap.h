//////////////////////////////////////////////////////////////////////
// NewUIGuildInfoWindow.h: interface for the CNewUIGuildInfoWindow class.
//////////////////////////////////////////////////////////////////////

#pragma once

#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIMainFrameWindow.h"
#include "NewUIChatLogWindow.h"
#include "NewUIMyInventory.h"
#include "PathFinding.h"

#define MAX_MAP_SIZE				512

namespace SEASON3B
{
	class CNewUIMiniMap : public CNewUIObj, public CPathFinding
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_MINIMAP_INTERFACE		= BITMAP_MINI_MAP_BEGIN,
		};

		enum MASTER_DATA
		{
			SKILL_ICON_DATA_WDITH = 4,
			SKILL_ICON_DATA_HEIGHT = 8,
			SKILL_ICON_WIDTH = 20,
			SKILL_ICON_HEIGHT = 28,
			SKILL_ICON_STARTX1 = 75,
			SKILL_ICON_STARTY1 = 75,
		};

		enum EVENT_STATE
		{
			EVENT_NONE = 0,
			EVENT_SCROLL_BTN_DOWN,
		};
	private:
		unicode::t_string		m_TooltipText;
		DWORD					m_TooltipTextColor;
		CNewUIManager*			m_pNewUIMng;
		POINT					m_Lenth[6];
		int						m_MiniPos;
		CNewUIButton			m_BtnExit;
		//MINI_MAP				m_Mini_Map_Data[MAX_MINI_MAP_DATA];
		float					m_Btn_Loc[MAX_MINI_MAP_DATA][4];
		bool					m_bSuccess;
		std::vector<MINI_MAP>	m_MinimapData;
		bool m_State;
		int m_Map;
		int m_MoveX;
		int m_MoveY;
		BOOL m_Move;



		float m_RenderZoom;
		float m_RenderFrameX;
		float m_RenderFrameY;
		float m_RenderSizeX;
		float m_RenderSizeY;
	public:
		CNewUIMiniMap();
		virtual ~CNewUIMiniMap();
		
		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();
		void LoadImages();
		void UnloadImages();
		void SetPos(int x, int y);
		void SetBtnPos(int Num ,float x, float y, float nx,float ny);

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();

		float GetLayerDepth();	//. 8.1f
		void LoadImages(const char* Filename );

		bool IsSuccess();
		bool movement_automatic();
		void OpenningProcess();
		void ClosingProcess();


		bool IsMoving();
		void StopMove();
		float GetZoom();
		void Zoom(float value);

		bool runtime_update_target();
		void runtime_render_map(bool rendername);
	private:
		bool Check_Mouse(int mx,int my);
		bool Check_Btn(int mx,int my);


		void runtime_move();
		void runtime_render_objet(int i, int x, int y, int group, int type, float angle, const char* name, const vec3_t angles);
		void runtime_render_name(int i, const char* name);

		bool runtime_calc_target(float mx, float my, int* dx, int* dy);

	};
}



