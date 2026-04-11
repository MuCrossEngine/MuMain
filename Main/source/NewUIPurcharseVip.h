#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "supportingfeature.h"

namespace SEASON3B
{
	class CNewUIPurcharseVip : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
			IMAGE_MEMBERSHIP_DATA = BITMAP_IMAGE_FRAME_EMU + 7,
			IMAGE_button_arrow_l,
			IMAGE_button_arrow_r,
		};
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		size_t targetIndex;
		std::vector<TEMPLATE_MEMBERSHIP> shopList;
	public:
		CNewUIPurcharseVip();
		virtual~CNewUIPurcharseVip();
		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();
		void SetPos(float x, float y);
		void SetInfo();
		void LoadImages();
		void UnloadImages();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth(); //. 10.5f
		void OpenningProcess();
		void ClosingProcess();
		int GetLevelPurcharse();
		void RenderButton(float x, float y, float width, float height);
		static CALLBACK_RESULT ButtonOK(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam);
	private:
		bool Checktarget();
		void RenderFrame();
		void RenderTexte();
		void RenderButtons();
	};
}
