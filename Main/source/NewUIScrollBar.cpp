//=============================================================================
//	NewUIScrollBar.cpp
//=============================================================================
#include "StdAfx.h"
#include "NewUIScrollBar.h"

using namespace SEASON3B;

CNewUIScrollBar::CNewUIScrollBar()
{
	memset(&m_ptPos, 0, sizeof(POINT));
	memset(&m_ptScrollBtnStartPos, 0, sizeof(POINT));
	memset(&m_ptScrollBtnPos, 0, sizeof(POINT));

	m_iScrollBarPickGap = 0;

	m_iScrollBarMovePixel = 1;
	m_iScrollBarHeightPixel = 0;
	m_iScrollBarMiddleNum = 0;
	m_iScrollBarMiddleRemainderPixel = 0;

	m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_NORMAL;
	m_bScrollBtnActive = false;

	m_fPercentOfSize = 1.0f;

	m_iBeginPos = 0;
	m_iCurPos = 0;
	m_iMaxPos = 1;
}

CNewUIScrollBar::~CNewUIScrollBar()
{
	Release();
}

bool CNewUIScrollBar::Create(int iX, int iY, int iHeight)
{
	m_iHeight = iHeight;
	SetPos(iX, iY);

	LoadImages();
	Show(true);
	m_bScrollBtnActive = true;

	return true;
}

void CNewUIScrollBar::Release()
{
	UnloadImages();
}

float CNewUIScrollBar::GetLayerDepth()
{
	return 4.4f;
}

void CNewUIScrollBar::LoadImages()
{
	LoadBitmap("Interface\\newui_scrollbar_up.tga", IMAGE_SCROLL_TOP);
	LoadBitmap("Interface\\newui_scrollbar_m.tga", IMAGE_SCROLL_MIDDLE, GL_LINEAR);
	LoadBitmap("Interface\\newui_scrollbar_down.tga", IMAGE_SCROLL_BOTTOM);
	LoadBitmap("Interface\\newui_scroll_on.tga", IMAGE_SCROLLBAR_ON, GL_LINEAR);
	LoadBitmap("Interface\\newui_scroll_off.tga", IMAGE_SCROLLBAR_OFF, GL_LINEAR);
}

void CNewUIScrollBar::UnloadImages()
{
	DeleteBitmap(IMAGE_SCROLL_TOP);
	DeleteBitmap(IMAGE_SCROLL_MIDDLE);
	DeleteBitmap(IMAGE_SCROLL_BOTTOM);
	DeleteBitmap(IMAGE_SCROLLBAR_ON);
	DeleteBitmap(IMAGE_SCROLLBAR_OFF);
}

void CNewUIScrollBar::SetPos(int x, int y)
{
	m_ptPos.x = x;
	m_ptPos.y = y;

	m_ptScrollBtnStartPos.x = m_ptPos.x - (SCROLLBTN_WIDTH / 2 - SCROLLBAR_TOP_WIDTH / 2);
	m_ptScrollBtnStartPos.y = m_ptPos.y;

	m_ptScrollBtnPos.x = m_ptScrollBtnStartPos.x;
	m_ptScrollBtnPos.y = m_ptScrollBtnStartPos.y;

	m_iScrollBarMovePixel = m_iHeight - SCROLLBTN_HEIGHT;
	if (m_iScrollBarMovePixel < 0)
		m_iScrollBarMovePixel = 1;

	m_iScrollBarHeightPixel = m_iHeight;

	m_iScrollBarMiddleNum = (m_iScrollBarHeightPixel - (SCROLLBAR_TOP_HEIGHT * 2)) / SCROLLBAR_MIDDLE_HEIGHT;
	m_iScrollBarMiddleRemainderPixel = (m_iScrollBarHeightPixel - (SCROLLBAR_TOP_HEIGHT * 2)) % SCROLLBAR_MIDDLE_HEIGHT;
}

bool CNewUIScrollBar::UpdateBtnEvent()
{
	if (IsRelease(VK_LBUTTON))
	{
		m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_NORMAL;
		m_iScrollBarPickGap = 0;
		return true;
	}

	if (CheckMouseIn(m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT))
	{
		if (IsPress(VK_LBUTTON) && m_bScrollBtnActive == true)
		{
			m_iScrollBarPickGap = MouseY - m_ptScrollBtnPos.y;
			m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_CLICKED;
			return false;
		}
	}

	if (CheckMouseIn(m_ptPos.x, m_ptPos.y, SCROLLBAR_TOP_WIDTH, m_iScrollBarHeightPixel))
	{
		if (IsPress(VK_LBUTTON) && m_bScrollBtnActive == true)
		{
			float fPercent = (float)(MouseY - m_ptPos.y) / (float)m_iScrollBarMovePixel;
			SetPercent(fPercent);
			return true;
		}
	}

	return true;
}

bool CNewUIScrollBar::UpdateMouseEvent()
{
	if (UpdateBtnEvent() == true)
		return false;

	return true;
}

bool CNewUIScrollBar::UpdateKeyEvent()
{
	if (!IsVisible())
	{

	}

	return true;
}

bool CNewUIScrollBar::Update()
{
	if (m_iScrollBtnMouseEvent == SCROLLBAR_MOUSEBTN_CLICKED)
	{
		float fPercent = (float)(MouseY - m_iScrollBarPickGap - m_ptPos.y) / (float)m_iScrollBarMovePixel;
		SetPercent(fPercent);
	}

	return true;
}

bool CNewUIScrollBar::Render()
{
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	RenderImage(IMAGE_SCROLL_TOP, m_ptPos.x, m_ptPos.y, SCROLLBAR_TOP_WIDTH, SCROLLBAR_TOP_HEIGHT);

	for (int i = 0; i < m_iScrollBarMiddleNum; i++)
	{
		RenderImage(IMAGE_SCROLL_MIDDLE, m_ptPos.x,
			m_ptPos.y + SCROLLBAR_TOP_HEIGHT + (i * SCROLLBAR_MIDDLE_HEIGHT), SCROLLBAR_TOP_WIDTH, SCROLLBAR_MIDDLE_HEIGHT);
	}

	if (m_iScrollBarMiddleRemainderPixel > 0)
	{
		RenderImage(IMAGE_SCROLL_MIDDLE, m_ptPos.x, m_ptPos.y + SCROLLBAR_TOP_HEIGHT + (m_iScrollBarMiddleNum * SCROLLBAR_MIDDLE_HEIGHT), SCROLLBAR_TOP_WIDTH, m_iScrollBarMiddleRemainderPixel);
	}

	RenderImage(IMAGE_SCROLL_BOTTOM, m_ptPos.x, m_ptPos.y + m_iHeight - SCROLLBAR_TOP_HEIGHT, SCROLLBAR_TOP_WIDTH, SCROLLBAR_TOP_HEIGHT);

	if (m_bScrollBtnActive == true)
	{
		if (m_iScrollBtnMouseEvent == SCROLLBAR_MOUSEBTN_CLICKED)
		{
			glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
		}
		RenderImage(IMAGE_SCROLLBAR_ON, m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT);
	}
	else
	{
		RenderImage(IMAGE_SCROLLBAR_OFF, m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT);
	}

	return true;
}


void CNewUIScrollBar::UpdateScrolling()
{
	m_ptScrollBtnPos.y = m_ptScrollBtnStartPos.y + m_fPercentOfSize * m_iScrollBarMovePixel;
}

void CNewUIScrollBar::ScrollUp(int iMoveValue)
{
	SetCurPos(m_iCurPos + iMoveValue);
}

void CNewUIScrollBar::ScrollDown(int iMoveValue)
{
	SetCurPos(m_iCurPos - iMoveValue);
}

void CNewUIScrollBar::SetPercent(float fPercent)
{
	if (fPercent <= 0.0f)
		m_fPercentOfSize = 0.0f;
	else if (fPercent >= 1.0f)
		m_fPercentOfSize = 1.0f;
	else
		m_fPercentOfSize = fPercent;

	m_iCurPos = m_iMaxPos * m_fPercentOfSize;
	UpdateScrolling();
}

void CNewUIScrollBar::SetMaxPos(int iMaxPos)
{
	if (iMaxPos < 1)
		iMaxPos = 1;

	m_iMaxPos = iMaxPos;
}

void CNewUIScrollBar::SetCurPos(int iPosValue)
{
	if (m_iBeginPos >= iPosValue)
		m_iCurPos = m_iBeginPos;
	else if (m_iMaxPos <= iPosValue)
		m_iCurPos = m_iMaxPos;
	else
		m_iCurPos = iPosValue;

	m_fPercentOfSize = (float)m_iCurPos / (float)m_iMaxPos;
	UpdateScrolling();
}

SEASON3B::CNewUIScrollBarHTML::CNewUIScrollBarHTML()
{
	memset(&m_ptPos, 0, sizeof(POINT));
	memset(&m_ptScrollBtnStartPos, 0, sizeof(POINT));
	memset(&m_ptScrollBtnPos, 0, sizeof(POINT));

	m_iScrollBarPickGap = 0;

	m_iScrollBarMovePixel = 1;
	m_iScrollBarHeightPixel = 0;
	m_iScrollBarMiddleNum = 0;
	m_iScrollBarMiddleRemainderPixel = 0;

	m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_NORMAL;
	m_bScrollBtnActive = false;

	m_fPercentOfSize = 1.0f;

	m_iBeginPos = 0;
	m_iCurPos = 0;
	m_iMaxPos = 1;
}

SEASON3B::CNewUIScrollBarHTML::~CNewUIScrollBarHTML()
{
}

bool SEASON3B::CNewUIScrollBarHTML::Create(int iX, int iY, int iHeight)
{
	m_iHeight = iHeight;
	SetPos(iX, iY);
	m_bScrollBtnActive = true;
	return true;
}

void SEASON3B::CNewUIScrollBarHTML::SetPos(int x, int y)
{
	m_ptPos.x = x;
	m_ptPos.y = y;

	m_ptScrollBtnStartPos.x = m_ptPos.x - (SCROLLBTN_WIDTH / 2 - SCROLLBAR_TOP_WIDTH / 2);
	m_ptScrollBtnStartPos.y = m_ptPos.y;

	m_ptScrollBtnPos.x = m_ptScrollBtnStartPos.x;
	m_ptScrollBtnPos.y = m_ptScrollBtnStartPos.y;

	m_iScrollBarMovePixel = m_iHeight - SCROLLBTN_HEIGHT;
	if (m_iScrollBarMovePixel < 0)
		m_iScrollBarMovePixel = 1;

	m_iScrollBarHeightPixel = m_iHeight;

	m_iScrollBarMiddleNum = (m_iScrollBarHeightPixel - (SCROLLBAR_TOP_HEIGHT * 2)) / SCROLLBAR_MIDDLE_HEIGHT;
	m_iScrollBarMiddleRemainderPixel = (m_iScrollBarHeightPixel - (SCROLLBAR_TOP_HEIGHT * 2)) % SCROLLBAR_MIDDLE_HEIGHT;
}

bool SEASON3B::CNewUIScrollBarHTML::UpdateBtnEvent()
{
	if (IsRelease(VK_LBUTTON))
	{
		m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_NORMAL;
		m_iScrollBarPickGap = 0;
		return true;
	}

	if (CheckMouseIn(m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT))
	{
		if (IsPress(VK_LBUTTON) && m_bScrollBtnActive == true)
		{
			m_iScrollBarPickGap = MouseY - m_ptScrollBtnPos.y;
			m_iScrollBtnMouseEvent = SCROLLBAR_MOUSEBTN_CLICKED;
			return false;
		}
	}

	if (CheckMouseIn(m_ptPos.x, m_ptPos.y, SCROLLBAR_TOP_WIDTH, m_iScrollBarHeightPixel))
	{
		if (IsPress(VK_LBUTTON) && m_bScrollBtnActive == true)
		{
			float fPercent = (float)(MouseY - m_ptPos.y) / (float)m_iScrollBarMovePixel;
			SetPercent(fPercent);
			return true;
		}
	}
	return false;
}

bool SEASON3B::CNewUIScrollBarHTML::UpdateMouseEvent()
{
	if (UpdateBtnEvent() == true)
		return false;

	return true;
}

bool SEASON3B::CNewUIScrollBarHTML::UpdateKeyEvent()
{
	return false;
}

bool SEASON3B::CNewUIScrollBarHTML::Update()
{
	if (m_iScrollBtnMouseEvent == SCROLLBAR_MOUSEBTN_CLICKED)
	{
		float fPercent = (float)(MouseY - m_iScrollBarPickGap - m_ptPos.y) / (float)m_iScrollBarMovePixel;
		SetPercent(fPercent);
	}

	return false;
}

bool SEASON3B::CNewUIScrollBarHTML::Render()
{
	EnableAlphaTest();
	if (m_bScrollBtnActive == true)
	{
		glColor4ub(150, 150, 150, 0xFF);
		if (m_iScrollBtnMouseEvent == SCROLLBAR_MOUSEBTN_CLICKED)
		{
			glColor4ub(80, 80, 80, 0xFF);
		}
		RenderColor(m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT, 0.0, 0);
	}
	else
	{
		glColor4ub(100, 100, 100, 0xFF);
		RenderColor(m_ptScrollBtnPos.x, m_ptScrollBtnPos.y, SCROLLBTN_WIDTH, SCROLLBTN_HEIGHT, 0.0, 0);
	}
	EndRenderColor();
	return false;
}

void SEASON3B::CNewUIScrollBarHTML::UpdateScrolling()
{
	m_ptScrollBtnPos.y = m_ptScrollBtnStartPos.y + m_fPercentOfSize * m_iScrollBarMovePixel;
}

void SEASON3B::CNewUIScrollBarHTML::ScrollUp(int iMoveValue)
{
	SetCurPos(m_iCurPos + iMoveValue);
}

void SEASON3B::CNewUIScrollBarHTML::ScrollDown(int iMoveValue)
{
	SetCurPos(m_iCurPos - iMoveValue);
}

void SEASON3B::CNewUIScrollBarHTML::SetPercent(float fPercent)
{
	if (fPercent <= 0.0f)
		m_fPercentOfSize = 0.0f;
	else if (fPercent >= 1.0f)
		m_fPercentOfSize = 1.0f;
	else
		m_fPercentOfSize = fPercent;

	m_iCurPos = m_iMaxPos * m_fPercentOfSize;
	UpdateScrolling();
}

void SEASON3B::CNewUIScrollBarHTML::SetMaxPos(int iMaxPos)
{
	if (iMaxPos < 1)
		iMaxPos = 1;

	m_iMaxPos = iMaxPos;
}

void SEASON3B::CNewUIScrollBarHTML::SetCurPos(int iMoveValue)
{
	if (m_iBeginPos >= iMoveValue)
		m_iCurPos = m_iBeginPos;
	else if (m_iMaxPos <= iMoveValue)
		m_iCurPos = m_iMaxPos;
	else
		m_iCurPos = iMoveValue;
	m_fPercentOfSize = (float)m_iCurPos / (float)m_iMaxPos;
	UpdateScrolling();
}
