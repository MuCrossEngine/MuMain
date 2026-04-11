#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "supportingfeature.h"

namespace SEASON3B
{
	class CNewUICommandFilter : public CNewUIObj
	{
		enum IMAGE_LIST
		{
			IMAGE_TOP_BACK3 = BITMAP_IMAGE_FRAME_EMU + 3,
		};
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		int max_view;
		float filter_percent;
		std::string text_input;
		CNewUIScrollBarHTML m_pScrollBar;
		std::vector<CUSTOM_COMMAND_INFO> command_auto;
		std::vector<CUSTOM_COMMAND_INFO> command_server;
		std::vector<CUSTOM_COMMAND_INFO> command_filter;
	public:
		CNewUICommandFilter();
		virtual~CNewUICommandFilter();

		bool Create(CNewUIManager* pNewUIMng, float x, float y);
		void Release();
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

		void ReOrganizer();
		void filterClear();
		void RenderAutoComplete(float RenderFrameX, float RenderFrameY, std::string input);

		void SendCommand(int index, const char* lpszFormat, ...);
	private:
		void RenderFrame();
		void RenderTexte();
		int countMatches(const std::string& input, const std::string& command);
		std::vector<CUSTOM_COMMAND_INFO> filter_commands(int accountLevel, bool isAdmin);
		std::vector<CUSTOM_COMMAND_INFO> autocomplete(const std::string& input);
	};
}