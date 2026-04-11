#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"

namespace SEASON3B
{
	struct TEMPLATE_EVENT_TIME
	{
		int m_Index;
		std::string m_Name;
		int m_Time;
		TEMPLATE_EVENT_TIME()
		{
			m_Index = -1;
			m_Time = -1;
		}
		TEMPLATE_EVENT_TIME(int index, const std::string& n, int s)
			: m_Index(index), m_Name(n), m_Time(s)
		{
		}
		const char* GetName() {
			return m_Name.c_str();
		}
	};


	class CNewUIEventTimeView : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_TOP_BACK1 = BITMAP_IMAGE_FRAME_EMU + 1,
			IMAGE_TOP_BACK2 = BITMAP_IMAGE_FRAME_EMU + 2,
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
		};
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int m_EventIndex;
		int m_EventTotal[3];
		DWORD m_iViewCount;
		DWORD m_EventTimeTickCount;
		CNewUIScrollBarHTML m_pScrollBar;
		std::vector<TEMPLATE_EVENT_TIME> m_EventTime[3];
	public:
		CNewUIEventTimeView();
		virtual~CNewUIEventTimeView();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();
		void Init();
		void SetInfo();
		void SetPos(float x, float y);
		void LoadImages();
		void UnloadImages();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth(); //. 10.5f

		void OpenningProcess();
		void ClosingProcess();


		void RenderFrame();
		void RenderTexte();
	};

}