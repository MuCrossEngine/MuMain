#pragma once
#include "NewUIScrollBar.h"

namespace SEASON3B
{
	typedef struct
	{
		int x;
		int y;
		int index;
		unicode::t_string Name;
	}stItemList;

	class CUIListBox
	{
		enum IMAGE_LIST
		{
			IMAGE_TABLE_TOP_LEFT = BITMAP_INTERFACE_NEW_INVENTORY_BASE_BEGIN + 1,
			IMAGE_TABLE_TOP_RIGHT,
			IMAGE_TABLE_BOTTOM_LEFT,
			IMAGE_TABLE_BOTTOM_RIGHT,
			IMAGE_TABLE_TOP_PIXEL,
			IMAGE_TABLE_BOTTOM_PIXEL,
			IMAGE_TABLE_LEFT_PIXEL,
			IMAGE_TABLE_RIGHT_PIXEL,
		};
	public:
		CUIListBox();
		virtual~CUIListBox();
	private:
		/*+000*/ CNewUIScrollBar m_ScrollBar;
		/*+088*/ int m_iViewCount;
		/*+092*/ int m_iMaxLength;
		/*+096*/ std::vector<unicode::t_string> good_List;
		/*+124*/ int m_iPos_x;
		/*+128*/ int m_iPos_y;
		/*+132*/ int m_Width;
		/*+136*/ int m_Height;
		/*+140*/ int positionTo;
	public:
		bool Create(int x, int y, int width, int height, int viewcount, int iMaxLength);
		void ClearText();
		bool AddText(const unicode::t_char* pzText);
		bool CheckText(const unicode::t_char* pzText);
		const unicode::t_char* GetText(int index);
		const std::vector<unicode::t_string>& GetData();
		void DelSelectedListBoxText();
		void RenderTable(int x, int y, int iWidth, int iHeight);
		bool UpdateMouseEvent();
		bool Update();
		bool Render(bool bRenderOnly=true);
		void SetPos(int x, int y);
		void SetSize(int iWidth, int iHeight);
	};
}
