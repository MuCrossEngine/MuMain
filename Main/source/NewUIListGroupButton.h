#pragma once

namespace SEASON3B
{
	class CNewUIListGroupButton
	{
	public:
		CNewUIListGroupButton();
		virtual~CNewUIListGroupButton();
		void clear();
		void push_back(std::string psText);
		void Create(int x, int y, int max, int imgindex = BITMAP_btn_empty_very_small, int width = 54, int height = 23, int distance = 2);
		void SetPosition(int x, int y);
		void Render();
		bool UpdateMouseEvent();
		void SetCurrent(int index);
		int GetCurrent();
		int as_interget();
		std::string as_string();
	private:
		POINT m_Pos;
		POINT m_ButtonSize;
		int m_distance;
		int m_imageIndex;

		int m_iViewCount;
		int m_trgCurrent;
		CNewUIScrollBarHTML m_pScrollBar;
		std::vector<std::string> textList;
		void RenderButton(int image, float x, float y, std::string text, bool select);
	};
}
