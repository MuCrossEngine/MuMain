#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"


namespace SEASON3B
{
	class CNewUIMenuUser : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
		};
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
	public:
		CNewUIMenuUser();
		virtual~CNewUIMenuUser();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();

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
	private:
		void RenderFrame();
		void RenderTexte();
		void RenderButtons();
		void RenderButton(float x, float y, float width, float height);
	};

}