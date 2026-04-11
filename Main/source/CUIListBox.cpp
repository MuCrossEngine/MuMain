#include "stdafx.h"
#include "UIControls.h"
#include "CUIListBox.h"

SEASON3B::CUIListBox::CUIListBox()
{
	m_iViewCount = 0;
	m_iMaxLength = 0;
	m_iPos_x = 0;
	m_iPos_y = 0;
	m_Width = 0;
	m_Height = 0;
	positionTo = 0;
	good_List.clear();
}

SEASON3B::CUIListBox::~CUIListBox()
{
	good_List.clear();
}

bool SEASON3B::CUIListBox::Create(int x, int y, int iWidth, int iHeight, int viewcount, int iMaxLength)
{
	SetPos(x, y);
	SetSize(iWidth, iHeight);
	m_iViewCount = viewcount;
	m_iMaxLength = iMaxLength;
	positionTo = 0;
	m_ScrollBar.Create(x + iWidth - 10, m_iPos_y, iHeight - 5);
	m_ScrollBar.SetPercent(0.0);
	return false;
}

void SEASON3B::CUIListBox::ClearText()
{
	good_List.clear();
}

bool SEASON3B::CUIListBox::AddText(const unicode::t_char* pzText)
{
	if (pzText == NULL || pzText[0] == '\0' || CheckText(pzText))
	{
		return false;
	}

	size_t good_count = good_List.size();

	if (good_count >= (unsigned int)m_iMaxLength)
	{
		unicode::t_char psText[255];
		memset(psText, 0, sizeof(psText));
		unicode::_sprintf(psText, GlobalText[3570], m_iMaxLength);
		SEASON3B::CreateOkMessageBox(psText, -1);
		return false;
	}
	else
	{
		good_List.push_back(pzText);
	}
	return true;
}

bool SEASON3B::CUIListBox::CheckText(const unicode::t_char* pzText)
{
	return std::find(good_List.begin(), good_List.end(), pzText) != good_List.end();
}

const unicode::t_char* SEASON3B::CUIListBox::GetText(int index)
{
	size_t good_count = good_List.size();

	for (size_t i = 0; i < good_count; i++)
	{
		if (i == index)
			return good_List[i].c_str();
	}

	return NULL;
}

const std::vector<unicode::t_string>& SEASON3B::CUIListBox::GetData()
{
	return good_List;
	// TODO: Insertar una instrucción "return" aquí
}

void SEASON3B::CUIListBox::DelSelectedListBoxText()
{
	size_t good_count = good_List.size();

	if (good_count && (unsigned int)positionTo < good_count)
	{
		if (positionTo >= 0)
		{
			good_List.erase(good_List.begin() + positionTo);
			positionTo = -1;
		}
		else
		{
			CreateOkMessageBox(GlobalText[3572], -1);
		}
	}
	else
	{
		CreateOkMessageBox(GlobalText[3571], -1);
	}
}

void SEASON3B::CUIListBox::RenderTable(int x, int y, int width, int height)
{
	EnableAlphaTest();
	glColor4f(0.0, 0.0, 0.0, 0.40000001);
	RenderColor((x + 3), (y + 2), (width - 7), (height - 7), 0.0, 0);
	EndRenderColor();

	RenderImage(IMAGE_TABLE_TOP_LEFT, x, y, 14.0, 14.0);

	RenderImage(IMAGE_TABLE_TOP_RIGHT, (x + width - 14), y, 14.0, 14.0);

	RenderImage(IMAGE_TABLE_BOTTOM_LEFT, x, (y + height - 14), 14.0, 14.0);

	RenderImage(IMAGE_TABLE_BOTTOM_RIGHT, (x + width - 14), (y + height - 14), 14.0, 14.0);

	RenderImage(IMAGE_TABLE_TOP_PIXEL, (x + 6), y, (width - 12), 14.0);

	RenderImage(IMAGE_TABLE_RIGHT_PIXEL, (x + width - 14), (y + 6), 14.0, (height - 14));

	RenderImage(IMAGE_TABLE_BOTTOM_PIXEL, (x + 6), (y + height - 14), (width - 12), 14.0);

	RenderImage(IMAGE_TABLE_LEFT_PIXEL, x, (y + 6), 14.0, (height - 14));
}

bool SEASON3B::CUIListBox::UpdateMouseEvent()
{
	int secure = 0;
	int current = 0;
	int good_count = good_List.size();

	if (good_count > m_iViewCount)
	{
		double prev = m_ScrollBar.GetPercent();

		current = (int)((double)(unsigned int)(good_count - m_iViewCount) * prev);
	}

	//positionTo = 0;

	for (int i = current; i < good_count && secure < m_iViewCount; i++, secure++)
	{
		if (IsPress(VK_LBUTTON) && CheckMouseIn(m_iPos_x + 10, m_iPos_y + 12 * secure + 9, m_Width, 12))
		{
			positionTo = i;
		}
	}

	if (CheckMouseIn(m_iPos_x, m_iPos_y, m_Width, m_Height) == 1)
	{
		double prev = m_ScrollBar.GetPercent();
		if (MouseWheel <= 0)
		{
			if (MouseWheel < 0)
			{
				MouseWheel = 0;
				prev += 0.1;
				if (prev > 1.0)
					prev = 1.0;
				m_ScrollBar.SetPercent(prev);
			}
		}
		else
		{
			MouseWheel = 0;
			prev -= 0.1;
			if (prev < 0.0)
				prev = 0.0;
			m_ScrollBar.SetPercent(prev);
		}
	}
	if (good_count > m_iViewCount)
	{
		m_ScrollBar.UpdateMouseEvent();
	}
	return false;
}

bool SEASON3B::CUIListBox::Update()
{
	return false;
}

bool SEASON3B::CUIListBox::Render(bool bRenderOnly)
{
	int secure = 0;
	int current = 0;
	int good_count = good_List.size();

	if(bRenderOnly)
		RenderTable(m_iPos_x, m_iPos_y, m_Width, m_Height);

	if (good_count > m_iViewCount)
	{
		double prev = m_ScrollBar.GetPercent();

		current = (int)((double)(unsigned int)(good_count - m_iViewCount) * prev);
	}

	g_pRenderText->SetTextColor(-1);
	g_pRenderText->SetBgColor(0, 0, 0, 0);

	for (int i = current; i < good_count && secure < m_iViewCount; i++, secure++)
	{
		if (positionTo == i)
		{
			EnableAlphaTest();
			glColor4f(1.0, 1.0, 0, 0.5);
			RenderColor(m_iPos_x + 8, m_iPos_y + 12 * secure + 7, m_Width - 16, 14, 0.0, 0);
			EndRenderColor();
		}
		g_pRenderText->RenderText(m_iPos_x + 10, m_iPos_y + 12 * secure + 9, good_List[i].c_str());
	}


	if (good_count > m_iViewCount)
	{
		m_ScrollBar.Render();
	}

	return false;
}

void SEASON3B::CUIListBox::SetPos(int x, int y)
{
	m_iPos_y = y;
	m_iPos_x = x;
}

void SEASON3B::CUIListBox::SetSize(int iWidth, int iHeight)
{
	m_Width = iWidth;
	m_Height = iHeight;
}
