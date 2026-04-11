#include "stdafx.h"
#include "UIControls.h"

#include "NewUIListGroupButton.h"


SEASON3B::CNewUIListGroupButton::CNewUIListGroupButton()
{
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_trgCurrent = -1;
	m_iViewCount = 0;

	m_distance = 2;
	m_ButtonSize.x = 54;
	m_ButtonSize.y = 23;
	m_imageIndex = BITMAP_btn_empty_very_small;
}

SEASON3B::CNewUIListGroupButton::~CNewUIListGroupButton()
{
	textList.clear();
}

void SEASON3B::CNewUIListGroupButton::clear()
{
	m_trgCurrent = -1;
	textList.clear();
}

void SEASON3B::CNewUIListGroupButton::push_back(std::string psText)
{
	textList.push_back(psText);
}

void SEASON3B::CNewUIListGroupButton::Create(int x, int y, int max, int imgindex, int width, int height, int distance)
{
	m_Pos.x = x;
	m_Pos.y = y;
	m_iViewCount = max;

	m_distance = distance;
	m_ButtonSize.x = width;
	m_ButtonSize.y = height;
	m_imageIndex = imgindex;

	m_pScrollBar.Create(x + (width + distance), y, (max * (height + 1)));
	m_pScrollBar.SetPercent(0);
}

void SEASON3B::CNewUIListGroupButton::SetPosition(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
	m_pScrollBar.SetPos(m_Pos.x + (m_ButtonSize.x + m_distance), m_Pos.y);
}

void SEASON3B::CNewUIListGroupButton::Render()
{
	int secure = 0;
	int current = 0;
	int good_count = textList.size();

	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	if (good_count > m_iViewCount)
	{
		double prev = m_pScrollBar.GetPercent();
		current = (int)((double)(unsigned int)(good_count - m_iViewCount) * prev);
	}

	int RenderFrameY = m_Pos.y;

	for (int i = current; i < good_count && secure < m_iViewCount; i++, secure++)
	{
		this->RenderButton(m_imageIndex, m_Pos.x, RenderFrameY, textList[i], m_trgCurrent == i);
		RenderFrameY += (m_ButtonSize.y + 1);
	}

	if (good_count > m_iViewCount)
	{
		m_pScrollBar.Render();
	}
}

bool SEASON3B::CNewUIListGroupButton::UpdateMouseEvent()
{
	m_pScrollBar.Update();

	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, (int)(m_ButtonSize.x + m_distance + 24), m_iViewCount * (m_ButtonSize.y + 1)) == 1)
	{
		double prev = m_pScrollBar.GetPercent();
		if (MouseWheel <= 0)
		{
			if (MouseWheel < 0)
			{
				MouseWheel = 0;
				prev += 0.1;
				if (prev > 1.0)
					prev = 1.0;
				m_pScrollBar.SetPercent(prev);
			}
		}
		else
		{
			MouseWheel = 0;
			prev -= 0.1;
			if (prev < 0.0)
				prev = 0.0;
			m_pScrollBar.SetPercent(prev);
		}

		int secure = 0;
		int current = 0;
		int good_count = textList.size();

		if (good_count > m_iViewCount)
		{
			current = (int)((double)(unsigned int)(good_count - m_iViewCount) * prev);
		}

		int PosY = m_Pos.y;
		for (int i = current; i < good_count && secure < m_iViewCount; i++, secure++)
		{
			if (SEASON3B::CheckMouseIn(m_Pos.x, PosY, m_ButtonSize.x, m_ButtonSize.y) && SEASON3B::IsRelease(VK_LBUTTON))
			{
				m_trgCurrent = i;
				return true;
			}
			PosY += (m_ButtonSize.y + 1);
		}
	}
	m_pScrollBar.UpdateMouseEvent();

	return false;
}

void SEASON3B::CNewUIListGroupButton::SetCurrent(int index)
{
	m_trgCurrent = index;
}

int SEASON3B::CNewUIListGroupButton::GetCurrent()
{
	return m_trgCurrent;
}

int SEASON3B::CNewUIListGroupButton::as_interget()
{
	size_t i = m_trgCurrent;

	if (i >= 0 && i < textList.size())
	{
		return std::stoi(textList[i]);
	}

	return 0;
}

std::string SEASON3B::CNewUIListGroupButton::as_string()
{
	size_t i = m_trgCurrent;

	if (i >= 0 && i < textList.size())
	{
		return textList[i].c_str();
	}
	return "";
}

void SEASON3B::CNewUIListGroupButton::RenderButton(int image, float x, float y, std::string text, bool select)
{
	float uv = 0.0;

	if (select == false)
	{
		if (SEASON3B::CheckMouseIn((int)x, (int)y, m_ButtonSize.x, m_ButtonSize.y))
		{
			if (MouseLButton)
				uv = (float)(m_ButtonSize.y * 2);
			else
				uv = (float)m_ButtonSize.y;
		}
	}
	else
	{
		uv = (float)(m_ButtonSize.y * 2);
	}

	SEASON3B::RenderImage(image, x, y, (float)m_ButtonSize.x, (float)m_ButtonSize.y, 0.0, uv);

	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->SetTextColor(CLRDW_WHITE);

	g_pRenderText->SetBgColor(0);

	g_pRenderText->RenderFont((int)x, (int)y, text.c_str(), m_ButtonSize.x, m_ButtonSize.y, RT3_SORT_CENTER);
}
