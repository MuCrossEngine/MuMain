// NewUICommonMessageBox.cpp: implementation of the NewUICommonMessageBox class.
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "NewUIGuildMakeWindow.h"
#include "NewUIGuildInfoWindow.h"
#include "NewUIMyInventory.h"
#include "UIControls.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "wsclientinline.h"
#include "UIGuildInfo.h"
#include "UIManager.h"
#include "PersonalShopTitleImp.h"
#include "CComGem.h"
#include "MixMgr.h"
#include "CSQuest.h"
#include "NewUICryWolf.h"
#include "PortalMgr.h"
#include "GambleSystem.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "CGMRenderGroupMesh.h"

using namespace SEASON3B;

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern int DeleteGuildIndex;
extern char DeleteID[];
extern int DeleteIndex, AppointStatus;
extern int s_nTargetFireMemberIndex;
extern int Button_Down;
extern int BackUp_Key;
extern BYTE m_AltarState[];

extern BYTE Rank;
extern int Exp;
extern BYTE Ranking[5];
extern BYTE HeroClass[5];
extern int HeroScore[5];
extern char HeroName[5][MAX_ID_SIZE + 1];
extern char	View_Suc_Or_Fail;

SEASON3B::CNewUIMessageBoxButton::CNewUIMessageBoxButton()
{
	m_bEnable = true;

	m_dwTexType = 0;
	m_x = m_y = m_width = m_height = 0.f;

	m_EventState = EVENT_NONE;
	m_dwSizeType = MSGBOX_BTN_SIZE_OK;

#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
	m_iMoveTextPosX = 0;
	m_iMoveTextPosY = 0;
	m_bClickEffect = false;
	m_fButtonWidth = MSGBOX_BTN_WIDTH;
	m_fButtonHeight = MSGBOX_BTN_HEIGHT;
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
}

SEASON3B::CNewUIMessageBoxButton::~CNewUIMessageBoxButton()
{
}

bool SEASON3B::CNewUIMessageBoxButton::IsMouseIn()
{
	if (m_bEnable == false)
		return false;

	return SEASON3B::CheckMouseIn(m_x, m_y, m_width, m_height);
}

void SEASON3B::CNewUIMessageBoxButton::SetInfo(DWORD dwTexType, float x, float y, float width, float height, DWORD dwSizeType, bool bClickEffect)
{
	m_dwTexType = dwTexType;
	m_dwSizeType = dwSizeType;
	m_x = x; m_y = y;
	m_width = width; m_height = height;
	m_bClickEffect = bClickEffect;

#if MAIN_UPDATE > 603
	if (m_dwSizeType == MSGBOX_BTN_SIZE_OK)
	{
		m_fButtonWidth = MSGBOX_BTN_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_SMALL)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_SMALL_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_SMALL_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_BIG)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_BIG_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_HEIGHT;
	}
	else
	{
		m_fButtonWidth = width;
		m_fButtonHeight = height;
	}

	if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK
		|| m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL
		|| m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CLOSE)
	{
		m_width = MSGBOX_BTN_WIDTH;
		m_height = 24;
	}

#else
	if (m_dwSizeType == MSGBOX_BTN_SIZE_OK)
	{
		m_fButtonWidth = MSGBOX_BTN_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_SMALL)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_SMALL_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_SMALL_HEIGHT;
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_BIG)
	{
		m_fButtonWidth = MSGBOX_BTN_EMPTY_BIG_WIDTH;
		m_fButtonHeight = MSGBOX_BTN_EMPTY_HEIGHT;
	}
	else
	{
		m_fButtonWidth = width;
		m_fButtonHeight = height;
	}
#endif
}

void SEASON3B::CNewUIMessageBoxButton::MoveTextPos(int iX, int iY)
{
	m_iMoveTextPosX = iX;
	m_iMoveTextPosY = iY;
}

void SEASON3B::CNewUIMessageBoxButton::SetText(const unicode::t_char* strText)
{
	if (unicode::_strlen(strText) > 0)
	{
		m_strText = strText;
	}
}

void SEASON3B::CNewUIMessageBoxButton::AddBlank(int iAddBlank)
{
	m_y += iAddBlank;
}

void SEASON3B::CNewUIMessageBoxButton::Update()
{
	if (m_bEnable == false)
		return;

	if (m_EventState == EVENT_NONE && MouseLButtonPush == false && IsMouseIn() == true)
	{
		m_EventState = EVENT_BTN_HOVER;
		return;
	}
	if (m_EventState == EVENT_BTN_HOVER && MouseLButtonPush == false && IsMouseIn() == false)
	{
		m_EventState = EVENT_NONE;
		return;
	}
	if (m_EventState == EVENT_BTN_HOVER)
	{
		if (MouseLButtonPush == true)
		{
			if (IsMouseIn() == true)
			{
				m_EventState = EVENT_BTN_DOWN;
				return;
			}
		}

	}
	if (m_EventState == EVENT_BTN_DOWN && MouseLButtonPush == false)
	{
		m_EventState = EVENT_NONE;
		return;
	}
}

void SEASON3B::CNewUIMessageBoxButton::Render()
{
	bool bRender = true;

	if (m_bEnable == false)
		glColor4f(0.8f, 0.8f, 0.8f, 0.9f);
	else
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

#if MAIN_UPDATE > 603
	if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_ok, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 77.f), 208.f, 77.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_cancel, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 77.f), 208.f, 77.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CLOSE)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_close, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 77.f), 208.f, 77.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_empty, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 96.f), 411.f, 96.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_BIG)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_empty_big, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 96.f), 512.f, 96.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_SMALL)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_empty_small, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 96.f), 304.f, 96.f);
	}
	else if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_VERY_SMALL
		|| m_dwTexType == BITMAP_btn_empty_very_small)
	{
		bRender = false;
		SEASON3B::RenderImageF(BITMAP_msgbox_btn_empty_very_small, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 96.f), 304.f, 96.f);
	}
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY
			|| m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_SMALL
			|| m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_BIG
			)
		{
			bRender = false;
			SEASON3B::RenderImageF(Bitmap_Message_box3, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 64.f), 213.f, 64.f);
		}
		else if (m_dwSizeType == MSGBOX_BTN_SIZE_OK)
		{
			bRender = false;
			if (m_EventState != EVENT_BTN_DOWN)
			{
				SEASON3B::RenderImageF(Bitmap_win_button2, m_x, m_y, m_width, m_height, 0.f, 0.f, 49.f, 16.f);
			}
			else
			{
				SEASON3B::RenderImageF(Bitmap_win_button2, m_x, m_y, m_width, m_height, 0.f, 16.f, 49.f, 16.f);
			}

			if (m_strText.empty())
			{
				if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK)
					this->SetText(GlobalText[228]);
				else
					this->SetText(GlobalText[229]);
				m_bClickEffect = true;
			}
		}
	}
#else
	if (m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY
		|| m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_SMALL
		|| m_dwSizeType == MSGBOX_BTN_SIZE_EMPTY_BIG
		)
	{
		bRender = false;
		SEASON3B::RenderImageF(Bitmap_Message_box3, m_x, m_y, m_width, m_height, 0.f, ((float)m_EventState * 64.f), 213.f, 64.f);
	}
	else if (m_dwSizeType == MSGBOX_BTN_SIZE_OK)
	{
		bRender = false;
		if (m_EventState != EVENT_BTN_DOWN)
		{
			SEASON3B::RenderImageF(Bitmap_win_button2, m_x, m_y, m_width, m_height, 0.f, 0.f, 49.f, 16.f);
		}
		else
		{
			SEASON3B::RenderImageF(Bitmap_win_button2, m_x, m_y, m_width, m_height, 0.f, 16.f, 49.f, 16.f);
		}

		if (m_strText.empty())
		{
			if (m_dwTexType == CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK)
				this->SetText(GlobalText[228]);
			else
				this->SetText(GlobalText[229]);
			m_bClickEffect = true;
		}
		}
#endif

	if (bRender)
	{
		BITMAP_t* pImage = &Bitmaps[m_dwTexType];
		RenderBitmap(m_dwTexType, m_x, m_y, m_width, m_height,
			(0.5f / (float)pImage->Width), ((m_EventState * m_fButtonHeight + 0.5f) / (float)pImage->Height),
			(m_fButtonWidth - 0.5f) / (float)pImage->Width - (0.5f / (float)pImage->Width),
			(m_fButtonHeight - 0.5f) / (float)pImage->Height - (0.5f / (float)pImage->Height));
	}

	if (m_strText.size() > 0)
	{
		SIZE Fontsize;

		g_pRenderText->SetFont(g_hFont);

		g_pRenderText->SetBgColor(0);

		if (m_bEnable == false)
			g_pRenderText->SetTextColor(128, 128, 128, 255);
		else
			g_pRenderText->SetTextColor(255, 255, 255, 255);

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), m_strText.c_str(), m_strText.size(), &Fontsize);

		Fontsize.cx = Fontsize.cx / (g_fScreenRate_x);
		Fontsize.cy = Fontsize.cy / (g_fScreenRate_y);

		int x = m_x + ((m_width / 2) - (Fontsize.cx / 2));
		int y = m_y + ((m_height / 2) - (Fontsize.cy / 2));

		if ((m_bClickEffect == true) && (m_EventState == EVENT_BTN_DOWN))
		{
			g_pRenderText->RenderText(x + m_iMoveTextPosX + 1, y + m_iMoveTextPosY + 1, m_strText.c_str());
		}
		else
		{
			g_pRenderText->RenderText(x + m_iMoveTextPosX, y + m_iMoveTextPosY, m_strText.c_str());
		}
	}
	}

SEASON3B::CNewUICommonMessageBox::CNewUICommonMessageBox()
{
	m_dwType = 0;
}

SEASON3B::CNewUICommonMessageBox::~CNewUICommonMessageBox()
{
	type_vector_msgdata::iterator vi = m_MsgDataList.begin();
	for (; vi != m_MsgDataList.end(); vi++)
	{
		SAFE_DELETE(*vi);
	}
	m_MsgDataList.clear();
}

DWORD SEASON3B::CNewUICommonMessageBox::GetType()
{
	return m_dwType;
}

bool SEASON3B::CNewUICommonMessageBox::Create(DWORD dwType, float fPriority)
{
	int x, y, width, height;

	m_dwType = dwType;

	SetAddCallbackFunc();

	x = (SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2);
	y = 100;
	width = MSGBOX_WIDTH;
	height = MSGBOX_TOP_HEIGHT + MSGBOX_BOTTOM_HEIGHT;

	CNewUIMessageBoxBase::Create(x, y, width, height, fPriority);

	SetButtonInfo();

	return true;
}

bool SEASON3B::CNewUICommonMessageBox::Create(DWORD dwType, const type_string& strMsg, DWORD dwColor, BYTE byFontType, float fPriority)
{
	int x, y, width, height;

	m_dwType = dwType;

	SetAddCallbackFunc();

	x = (SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2);
	y = 100;
	width = MSGBOX_WIDTH;
	height = MSGBOX_TOP_HEIGHT + MSGBOX_BOTTOM_HEIGHT;

	CNewUIMessageBoxBase::Create(x, y, width, height, fPriority);

	SetButtonInfo();

	AddMsg(strMsg, dwColor, byFontType);

	return true;
}

void SEASON3B::CNewUICommonMessageBox::SetPos(int x, int y)
{
	CNewUIMessageBoxBase::SetPos(x, y);

	float fx, fy;
	fx = GetPos().x + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
	fy = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
	m_BtnOk.SetPos(fx, fy);
	fx = GetPos().x + (GetSize().cx / 2) + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
	fy = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
	m_BtnCancel.SetPos(fx, fy);
}

void SEASON3B::CNewUICommonMessageBox::SetAddCallbackFunc()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_ESC);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_RETURN);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_USER_COMMON_CANCEL);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_ESC);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_RETURN);
		break;
	}
}

void SEASON3B::CNewUICommonMessageBox::SetButtonInfo()
{
	float x, y, width, height;

	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		x = GetPos().x + (GetSize().cx / 2) - (MSGBOX_BTN_WIDTH / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		x = GetPos().x + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
		x = GetPos().x + (GetSize().cx / 2) + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
		m_BtnCancel.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
		break;
	}
}

void SEASON3B::CNewUICommonMessageBox::AddButtonBlank(int iAddLine)
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		m_BtnCancel.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		break;
	}
}

void SEASON3B::CNewUICommonMessageBox::AddButtonBlank(int iAddLine, int _iImgSize)
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.AddBlank(iAddLine * _iImgSize);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.AddBlank(iAddLine * _iImgSize);
		m_BtnCancel.AddBlank(iAddLine * _iImgSize);
		break;
	}
}

void SEASON3B::CNewUICommonMessageBox::AddMsg(const type_string& strMsg, DWORD dwColor, BYTE byFontType)
{
	int iOrigSize = m_MsgDataList.size();
	int iLine = SeparateText(strMsg, dwColor, byFontType);
	int iSize = m_MsgDataList.size();

	if (iSize > 2)
	{
		float height = GetSize().cy;

		if (iOrigSize < 2)
		{
			iLine = iLine + iOrigSize - 2;
		}

		height += (MSGBOX_MIDDLE_HEIGHT * iLine);
		SetSize(GetSize().cx, height);
		AddButtonBlank(iLine);
	}
}

int SEASON3B::CNewUICommonMessageBox::SeparateText(const type_string& strMsg, DWORD dwColor, BYTE byFontType, int _TextSize)
{
	SIZE TextSize;
	size_t TextExtentWidth;
	int iLine = 0;

	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strMsg.c_str(), strMsg.size(), &TextSize);
	TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

	if (TextExtentWidth <= (DWORD)_TextSize)
	{
		MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
		pMsg->strMsg = strMsg;
		pMsg->dwColor = dwColor;
		pMsg->byFontType = byFontType;
		m_MsgDataList.push_back(pMsg);

		iLine = 1;
		return iLine;
	}

	type_string strCutText, strRemainText;
	strRemainText = strMsg;

	bool bLoop = true;
	while (bLoop)
	{
		int prev_offset = 0;
		for (int cur_offset = 0; cur_offset < (int)strRemainText.size(); )
		{
			prev_offset = cur_offset;
			size_t offset = _mbclen((const unsigned char*)(strRemainText.c_str() + cur_offset));
			cur_offset += offset;

			type_string strTemp(strRemainText, 0, cur_offset/* size */);
			g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strTemp.c_str(), strTemp.size(), &TextSize);
			TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

			if (TextExtentWidth > (DWORD)_TextSize && cur_offset != 0)
			{
				strCutText = type_string(strRemainText, 0, prev_offset/* size */);
				strRemainText = type_string(strRemainText, prev_offset, strRemainText.size() - prev_offset/* size */);

				MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
				pMsg->strMsg = strCutText;
				pMsg->dwColor = dwColor;
				pMsg->byFontType = byFontType;
				m_MsgDataList.push_back(pMsg);
				iLine++;

				g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strRemainText.c_str(), strRemainText.size(), &TextSize);
				TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

				if (TextExtentWidth <= (DWORD)_TextSize)
				{
					MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
					pMsg->strMsg = strRemainText;
					pMsg->dwColor = dwColor;
					pMsg->byFontType = byFontType;
					m_MsgDataList.push_back(pMsg);
					iLine++;

					bLoop = false;
					break;
				}

				break;
			}
		}
	}

	return iLine;
}

bool SEASON3B::CNewUICommonMessageBox::Update()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.Update();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.Update();
		m_BtnCancel.Update();
		break;
	}

	return true;
}

bool SEASON3B::CNewUICommonMessageBox::Render()
{
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	RenderFrame();
	RenderTexts();
	RenderButtons();
	DisableAlphaBlend();
	return true;
}

void SEASON3B::CNewUICommonMessageBox::RenderFrame()
{
	int iMiddleFrameCount = m_MsgDataList.size() - 2;

	window_emergent(this, ((iMiddleFrameCount > 0) ? iMiddleFrameCount : 0));
}

void SEASON3B::CNewUICommonMessageBox::RenderTexts()
{
	float x, y;

	x = GetPos().x; y = GetPos().y + MSGBOX_TEXT_TOP_BLANK;
	type_vector_msgdata::iterator vi = m_MsgDataList.begin();
	for (; vi != m_MsgDataList.end(); vi++)
	{
		g_pRenderText->SetTextColor((*vi)->dwColor);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		switch ((*vi)->byFontType)
		{
		case MSGBOX_FONT_NORMAL:
			g_pRenderText->SetFont(g_hFont);
			break;
		case MSGBOX_FONT_BOLD:
			g_pRenderText->SetFont(g_hFontBold);
			break;
		}

		SIZE TextSize;
		size_t TextExtentWidth, TextExtentHeight;

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), (*vi)->strMsg.c_str(), (*vi)->strMsg.size(), &TextSize);
		TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);
		TextExtentHeight = (size_t)(TextSize.cy / g_fScreenRate_y);

		x = GetPos().x + (MSGBOX_WIDTH / 2) - (TextExtentWidth / 2);
		g_pRenderText->RenderText((int)x, (int)y, (*vi)->strMsg.c_str());
		y += (TextExtentHeight + 4);
	}
}

void SEASON3B::CNewUICommonMessageBox::RenderButtons()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.Render();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.Render();
		m_BtnCancel.Render();
		break;
	}
}

void SEASON3B::CNewUICommonMessageBox::LockOkButton()
{
	m_BtnOk.SetEnable(false);
}

CALLBACK_RESULT SEASON3B::CNewUICommonMessageBox::LButtonUp(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CNewUICommonMessageBox* pMsgBox = dynamic_cast<CNewUICommonMessageBox*>(pOwner);
	if (pMsgBox)
	{
		switch (pMsgBox->GetType())
		{
		case MSGBOX_COMMON_TYPE_OK:
			if (pMsgBox->m_BtnOk.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			break;
		case MSGBOX_COMMON_TYPE_OKCANCEL:
			if (pMsgBox->m_BtnOk.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			if (pMsgBox->m_BtnCancel.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_CANCEL);
				return CALLBACK_BREAK;
			}
			break;
		}

	}

	return CALLBACK_CONTINUE;
}

CALLBACK_RESULT SEASON3B::CNewUICommonMessageBox::Close(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

SEASON3B::CNewUI3DItemCommonMsgBox::CNewUI3DItemCommonMsgBox()
{
	ZeroMemory(&m_Item, sizeof(m_Item));
	m_iItemValue = 0;
}

SEASON3B::CNewUI3DItemCommonMsgBox::~CNewUI3DItemCommonMsgBox()
{
	Release();
}

DWORD SEASON3B::CNewUI3DItemCommonMsgBox::GetType()
{
	return m_dwType;
}

bool SEASON3B::CNewUI3DItemCommonMsgBox::Create(DWORD dwType, float fPriority)
{
	int x, y, width, height;

	m_dwType = dwType;

	SetAddCallbackFunc();

	x = (SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2);
	y = 100;
	width = MSGBOX_WIDTH;
	height = MSGBOX_TOP_HEIGHT + MSGBOX_MIDDLE_HEIGHT + MSGBOX_BOTTOM_HEIGHT;

	CNewUIMessageBoxBase::Create(x, y, width, height, fPriority);

	SetButtonInfo();

	if (g_pNewUI3DRenderMng)
	{
		g_pNewUI3DRenderMng->Add3DRenderObj(this);
	}

	return true;
}

bool SEASON3B::CNewUI3DItemCommonMsgBox::Create(DWORD dwType, const type_string& strMsg, DWORD dwColor, BYTE byFontType, float fPriority)
{
	int x, y, width, height;

	m_dwType = dwType;
	SetAddCallbackFunc();

	x = (SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2);
	y = 100;
	width = MSGBOX_WIDTH;
	height = MSGBOX_TOP_HEIGHT + MSGBOX_MIDDLE_HEIGHT + MSGBOX_BOTTOM_HEIGHT;

	CNewUIMessageBoxBase::Create(x, y, width, height, fPriority);

	SetButtonInfo();

	AddMsg(strMsg, dwColor, byFontType);

	if (g_pNewUI3DRenderMng)
		g_pNewUI3DRenderMng->Add3DRenderObj(this);

	return true;
}
void SEASON3B::CNewUI3DItemCommonMsgBox::Release()
{
	CNewUIMessageBoxBase::Release();

	if (g_pNewUI3DRenderMng)
		g_pNewUI3DRenderMng->Remove3DRenderObj(this);

	type_vector_msgdata::iterator vi = m_MsgDataList.begin();
	for (; vi != m_MsgDataList.end(); vi++)
	{
		SAFE_DELETE(*vi);
	}
	m_MsgDataList.clear();
}

void SEASON3B::CNewUI3DItemCommonMsgBox::Set3DItem(ITEM* pItem)
{
	if (pItem)
	{
		memcpy(&m_Item, pItem, sizeof(ITEM));
	}
}

void SEASON3B::CNewUI3DItemCommonMsgBox::SetItemValue(int iValue)
{
	m_iItemValue = iValue;
}

int SEASON3B::CNewUI3DItemCommonMsgBox::GetItemValue()
{
	return m_iItemValue;
}

void SEASON3B::CNewUI3DItemCommonMsgBox::AddMsg(const type_string& strMsg, DWORD dwColor, BYTE byFontType)
{
	int iOrigSize = m_MsgDataList.size();
	int iLine = SeparateText(strMsg, dwColor, byFontType);
	int iSize = m_MsgDataList.size();

	if (iSize > 3)
	{
		float height = GetSize().cy;

		if (iOrigSize < 3)
		{
			iLine = iLine + iOrigSize - 3;
		}

		height += (MSGBOX_MIDDLE_HEIGHT * iLine);
		SetSize(GetSize().cx, height);
		AddButtonBlank(iLine);
	}
}

CALLBACK_RESULT SEASON3B::CNewUI3DItemCommonMsgBox::LButtonUp(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CNewUI3DItemCommonMsgBox* pMsgBox = dynamic_cast<CNewUI3DItemCommonMsgBox*>(pOwner);
	if (pMsgBox)
	{
		switch (pMsgBox->GetType())
		{
		case MSGBOX_COMMON_TYPE_OK:
			if (pMsgBox->m_BtnOk.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			break;
		case MSGBOX_COMMON_TYPE_OKCANCEL:
			if (pMsgBox->m_BtnOk.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			if (pMsgBox->m_BtnCancel.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_CANCEL);
				return CALLBACK_BREAK;
			}
			break;
		}

	}

	return CALLBACK_CONTINUE;
}

CALLBACK_RESULT SEASON3B::CNewUI3DItemCommonMsgBox::Close(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CNewUI3DItemCommonMsgBox::Update()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.Update();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.Update();
		m_BtnCancel.Update();
		break;
	}

	return true;
}

void SEASON3B::CNewUI3DItemCommonMsgBox::SetAddCallbackFunc()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::Close, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::Close, MSGBOX_EVENT_PRESSKEY_ESC);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_RETURN);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::Close, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(SEASON3B::CNewUI3DItemCommonMsgBox::Close, MSGBOX_EVENT_USER_COMMON_CANCEL);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_ESC);
		//AddCallbackFunc(SEASON3B::CNewUICommonMessageBox::Close, MSGBOX_EVENT_PRESSKEY_RETURN);
		break;
	}
}

int SEASON3B::CNewUI3DItemCommonMsgBox::SeparateText(const type_string& strMsg, DWORD dwColor, BYTE byFontType)
{
	SIZE TextSize;
	size_t TextExtentWidth;
	int iLine = 0;

	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strMsg.c_str(), strMsg.size(), &TextSize);
	TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

	if (TextExtentWidth <= MSGBOX_TEXT_MAXWIDTH_3DITEM)
	{
		MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
		pMsg->strMsg = strMsg;
		pMsg->dwColor = dwColor;
		pMsg->byFontType = byFontType;
		m_MsgDataList.push_back(pMsg);

		iLine = 1;
		return iLine;
	}

	type_string strCutText, strRemainText;
	strRemainText = strMsg;

	bool bLoop = true;
	while (bLoop)
	{
		int prev_offset = 0;

		for (int cur_offset = 0; cur_offset < (int)strRemainText.size(); )
		{
			prev_offset = cur_offset;
			size_t offset = _mbclen((const unsigned char*)(strRemainText.c_str() + cur_offset));
			cur_offset += offset;

			type_string strTemp(strRemainText, 0, cur_offset/* size */);

			g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strTemp.c_str(), strTemp.size(), &TextSize);
			TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

			if (TextExtentWidth > MSGBOX_TEXT_MAXWIDTH_3DITEM && cur_offset != 0)
			{
				strCutText = type_string(strRemainText, 0, prev_offset/* size */);
				strRemainText = type_string(strRemainText, prev_offset, strRemainText.size() - prev_offset/* size */);

				MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
				pMsg->strMsg = strCutText;
				pMsg->dwColor = dwColor;
				pMsg->byFontType = byFontType;
				m_MsgDataList.push_back(pMsg);
				iLine++;

				g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strRemainText.c_str(), strRemainText.size(), &TextSize);
				TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);

				if (TextExtentWidth <= MSGBOX_TEXT_MAXWIDTH_3DITEM)
				{
					MSGBOX_TEXTDATA* pMsg = new MSGBOX_TEXTDATA;
					pMsg->strMsg = strRemainText;
					pMsg->dwColor = dwColor;
					pMsg->byFontType = byFontType;
					m_MsgDataList.push_back(pMsg);
					iLine++;

					bLoop = false;
					break;
				}

				break;
			}
		}
	}

	return iLine;
}

void SEASON3B::CNewUI3DItemCommonMsgBox::SetButtonInfo()
{
	float x, y, width, height;

	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		x = GetPos().x + (GetSize().cx / 2) - (MSGBOX_BTN_WIDTH / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		x = GetPos().x + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnOk.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, width, height);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM

		x = GetPos().x + (GetSize().cx / 2) + (((GetSize().cx / 2) - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - (MSGBOX_BTN_HEIGHT + MSGBOX_BTN_BOTTOM_BLANK);
		width = MSGBOX_BTN_WIDTH;
		height = MSGBOX_BTN_HEIGHT;
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnCancel.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL, x, y, width, height, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
		m_BtnCancel.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL, x, y, width, height);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
		break;
}
}

void SEASON3B::CNewUI3DItemCommonMsgBox::AddButtonBlank(int iAddLine)
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		m_BtnCancel.AddBlank(iAddLine * MSGBOX_MIDDLE_HEIGHT);
		break;
	}
}

bool SEASON3B::CNewUI3DItemCommonMsgBox::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderFrame();

	RenderTexts();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUI3DItemCommonMsgBox::Render3D()
{
	float x, y, width, height;

	x = GetPos().x + 30;
	y = GetPos().y + 30;
	width = MSGBOX_3DITEM_WIDTH;
	height = MSGBOX_3DITEM_HEIGHT;

	RenderItem3D(x, y, width, height, m_Item.Type, m_Item.Level, m_Item.Option1, m_Item.ExtOption, true);		// PickUp
}

bool SEASON3B::CNewUI3DItemCommonMsgBox::IsVisible() const
{
	return true;
}

void SEASON3B::CNewUI3DItemCommonMsgBox::RenderFrame()
{
	int iMiddleFrameCount = m_MsgDataList.size() - 2;

	window_emergent(this, ((iMiddleFrameCount > 0) ? iMiddleFrameCount : 0));
}

void SEASON3B::CNewUI3DItemCommonMsgBox::RenderTexts()
{
	float x, y;

	x = GetPos().x + MSGBOX_TEXT_LEFT_BLANK_3DITEM; y = GetPos().y + MSGBOX_TEXT_TOP_BLANK;
	type_vector_msgdata::iterator vi = m_MsgDataList.begin();
	for (; vi != m_MsgDataList.end(); vi++)
	{
		g_pRenderText->SetTextColor((*vi)->dwColor);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		switch ((*vi)->byFontType)
		{
		case MSGBOX_FONT_NORMAL:
			g_pRenderText->SetFont(g_hFont);
			break;
		case MSGBOX_FONT_BOLD:
			g_pRenderText->SetFont(g_hFontBold);
			break;
		}

		SIZE TextSize;
		size_t TextExtentWidth, TextExtentHeight;

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), (*vi)->strMsg.c_str(), (*vi)->strMsg.size(), &TextSize);
		TextExtentWidth = (size_t)(TextSize.cx / g_fScreenRate_x);
		TextExtentHeight = (size_t)(TextSize.cy / g_fScreenRate_y);

		x = GetPos().x + 60 + ((GetSize().cx - 60) / 2) - (TextExtentWidth / 2);
		g_pRenderText->RenderText((int)x, (int)y, (*vi)->strMsg.c_str());
		y += (TextExtentHeight + 4);
	}
}

void SEASON3B::CNewUI3DItemCommonMsgBox::RenderButtons()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		m_BtnOk.Render();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		m_BtnOk.Render();
		m_BtnCancel.Render();
		break;
	}
}

bool SEASON3B::CServerLostMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK, 10.f))
		return false;

	pMsgBox->AddMsg(GlobalText[402]);
	pMsgBox->AddCallbackFunc(CServerLostMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->RemoveCallbackFunc(MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CServerLostMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_POP_ALL_EVENTS;
}

bool SEASON3B::CGuildRequestMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGuildRequestMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildRequestMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CGuildRequestMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CGuildRequestMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildAnswer(true);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildRequestMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildAnswer(false);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildFireMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGuildFireMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CNewUICommonMessageBox::Close, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CGuildFireMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	DeleteGuildIndex = s_nTargetFireMemberIndex;
	PlayBuffer(SOUND_CLICK01);
	return CALLBACK_BREAK;
}

bool SEASON3B::CMapEnterWerwolfMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->SetPos((SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2), 50);
	pMsgBox->AddMsg(GlobalText[1658], RGBA(254, 176, 72, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1659], RGBA(170, 218, 146, 255));
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1660]);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1676]);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1661]);

	BYTE byQuestState = g_csQuest.getQuestState2(QUEST_3RD_CHANGE_UP_2);
	if (QUEST_ING != byQuestState && QUEST_END != byQuestState)
	{
		pMsgBox->LockOkButton();
	}

	pMsgBox->AddCallbackFunc(CMapEnterWerwolfMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CMapEnterWerwolfMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	DWORD dwGold = CharacterMachine->Gold;

	if (dwGold >= 3000000)
	{
		SendRequestEnterOnWerwolf();
	}
	else
	{
		g_pChatListBox->AddText("", GlobalText[423], SEASON3B::TYPE_ERROR_MESSAGE);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool CMapEnterGateKeeperMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->SetPos((SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2), 50);
	pMsgBox->AddMsg(GlobalText[1662], 0xFF49B0FF, MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1663], 0xFF61F191);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1664]);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[1677]);

	BYTE byQuestState = g_csQuest.getQuestState2(QUEST_3RD_CHANGE_UP_3);
	if (QUEST_ING != byQuestState)
	{
		pMsgBox->LockOkButton();
	}

	pMsgBox->AddCallbackFunc(CMapEnterGateKeeperMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT CMapEnterGateKeeperMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestEnterOnGateKeeper();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CPartyMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(PartyKey));

	if (pCharacter)
	{
		pMsgBox->AddMsg(pCharacter->ID);
		pMsgBox->AddMsg(GlobalText[425]);
	}
	pMsgBox->AddCallbackFunc(CPartyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CPartyMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CPartyMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestPartyAnswer(true);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CPartyMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestPartyAnswer(false);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CTradeMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	char szYourID[MAX_ID_SIZE + 1];
	g_pTrade->GetYourID(szYourID);
	pMsgBox->AddMsg(szYourID);
	pMsgBox->AddMsg(GlobalText[419]);
	pMsgBox->AddCallbackFunc(CTradeMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CTradeMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CTradeMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestTradeAnswer(true);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CTradeMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestTradeAnswer(false);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CTradeAlertMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	DWORD adwColor[4] = { RGBA(255, 178, 0, 255), RGBA(255, 178, 0, 255), RGBA(255, 178, 0, 255), RGBA(255, 32, 32, 255) };

	for (int i = 0; i < 4; ++i)
		pMsgBox->AddMsg(GlobalText[371 + i], adwColor[i], MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CTradeAlertMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CTradeAlertMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CTradeAlertMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	g_pTrade->AlertTrade();
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CTradeAlertMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildWarMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL, 15.f))
		return false;

	unicode::t_char strText[128];
	unicode::_sprintf(strText, GlobalText[430], GuildWarName);
	pMsgBox->AddMsg(strText);
	pMsgBox->AddMsg(GlobalText[431]);

	pMsgBox->AddCallbackFunc(CGuildWarMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildWarMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CGuildWarMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildWarAnswer(true);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildWarMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildWarAnswer(false);
	InitGuildWar();
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CBattleSoccerMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL, 15.f))
		return false;

	unicode::t_char strText[128];
	unicode::_sprintf(strText, GlobalText[430], GuildWarName);
	pMsgBox->AddMsg(strText);
	pMsgBox->AddMsg(GlobalText[432]);

	pMsgBox->AddCallbackFunc(CBattleSoccerMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CBattleSoccerMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CBattleSoccerMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildWarAnswer(true);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CBattleSoccerMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildWarAnswer(false);
	InitGuildWar();
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CServerImmigrationErrorMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[401]);

	pMsgBox->AddCallbackFunc(CServerImmigrationErrorMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CServerImmigrationErrorMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CPersonalshopCreateMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[1131]);

	pMsgBox->AddCallbackFunc(CPersonalshopCreateMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CPersonalshopCreateMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CPersonalshopCreateMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	unicode::t_string Temp;
	char name[MAX_SHOPTITLE];
	g_pMyShopInventory->GetTitle(Temp);

	strcpy(name, Temp.c_str());
	SendRequestCreatePersonalShop(name);
	g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
	g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CPersonalshopCreateMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CFenrirRepairMsgBoxLayout::SetLayout()
{
	CFenrirRepairMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[1865], RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CFenrirRepairMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CFenrirRepairMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CFenrirRepairMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CFenrirRepairMsgBox* pMsgBox = dynamic_cast<CFenrirRepairMsgBox*>(pOwner);
	if (pMsgBox == false)
	{
		return CALLBACK_CONTINUE;
	}

	SendRequestUse(pMsgBox->GetSourceIndex(), pMsgBox->GetTargetIndex());
	CNewUIInventoryCtrl::BackupPickedItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CFenrirRepairMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CNewUIInventoryCtrl::BackupPickedItem();
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

void SEASON3B::CFenrirRepairMsgBox::SetSourceIndex(int iIndex)
{
	m_iSourceIndex = iIndex;
}

void SEASON3B::CFenrirRepairMsgBox::SetTargetIndex(int iIndex)
{
	m_iTargetIndex = iIndex;
}

int SEASON3B::CFenrirRepairMsgBox::GetSourceIndex()
{
	return m_iSourceIndex;
}

int SEASON3B::CFenrirRepairMsgBox::GetTargetIndex()
{
	return m_iTargetIndex;
}

bool SEASON3B::CInfinityArrowCancelMsgBoxLayout::SetLayout()
{
	extern int g_iCancelSkillTarget;
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	unicode::t_char strText[MAX_GLOBAL_TEXT_STRING];
	unicode::_sprintf(strText, "%s%s", SkillAttribute[AT_SKILL_INFINITY_ARROW].Name, GlobalText[2046]);
	g_iCancelSkillTarget = AT_SKILL_INFINITY_ARROW;

	pMsgBox->AddMsg(strText, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CInfinityArrowCancelMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CInfinityArrowCancelMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CInfinityArrowCancelMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	SendRequestCancelMagic(g_iCancelSkillTarget, HeroKey);
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CInfinityArrowCancelMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CBuffSwellOfMPCancelMsgBoxLayOut::SetLayout()
{
	extern int g_iCancelSkillTarget;
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	unicode::t_char strText[MAX_GLOBAL_TEXT_STRING];
	unicode::_sprintf(strText, "%s%s", SkillAttribute[AT_SKILL_SWELL_OF_MAGICPOWER].Name, GlobalText[2046]);
	g_iCancelSkillTarget = AT_SKILL_SWELL_OF_MAGICPOWER;

	pMsgBox->AddMsg(strText, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CBuffSwellOfMPCancelMsgBoxLayOut::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CBuffSwellOfMPCancelMsgBoxLayOut::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPCancelMsgBoxLayOut::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	SendRequestCancelMagic(g_iCancelSkillTarget, HeroKey);
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPCancelMsgBoxLayOut::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CBuffSwellOfMPUpCancelMsgBoxLayOut::SetLayout()
{
	extern int g_iCancelSkillTarget;
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	unicode::t_char strText[MAX_GLOBAL_TEXT_STRING];
	unicode::_sprintf(strText, "%s%s", SkillAttribute[Skill_Expansion_of_Wizardry_Power_Up].Name, GlobalText[2046]);
	g_iCancelSkillTarget = Skill_Expansion_of_Wizardry_Power_Up;

	pMsgBox->AddMsg(strText, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CBuffSwellOfMPUpCancelMsgBoxLayOut::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CBuffSwellOfMPUpCancelMsgBoxLayOut::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPUpCancelMsgBoxLayOut::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	SendRequestCancelMagic(g_iCancelSkillTarget, HeroKey);
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPUpCancelMsgBoxLayOut::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CBuffSwellOfMPMasteryCancelMsgBoxLayOut::SetLayout()
{
	extern int g_iCancelSkillTarget;
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	unicode::t_char strText[MAX_GLOBAL_TEXT_STRING];
	unicode::_sprintf(strText, "%s%s", SkillAttribute[Skill_Expansion_of_Wizardry_Mastery].Name, GlobalText[2046]);
	g_iCancelSkillTarget = Skill_Expansion_of_Wizardry_Mastery;

	pMsgBox->AddMsg(strText, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CBuffSwellOfMPMasteryCancelMsgBoxLayOut::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CBuffSwellOfMPMasteryCancelMsgBoxLayOut::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPMasteryCancelMsgBoxLayOut::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	SendRequestCancelMagic(g_iCancelSkillTarget, HeroKey);
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CBuffSwellOfMPMasteryCancelMsgBoxLayOut::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	extern int g_iCancelSkillTarget;
	g_iCancelSkillTarget = 0;

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGemIntegrationUnityCheckMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGemIntegrationUnityCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGemIntegrationUnityCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationUnityCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::ProcessCSAction();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	COMGEM::Exit();

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationUnityCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::GetBack();

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(CGemIntegrationUnityMsgBoxLayout));

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGemIntegrationUnityResultMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	unicode::t_char strText[256] = { 0, };
	unicode::_sprintf(strText, "%s%s %s", GlobalText[1801], GlobalText[1816], GlobalText[858]);
	pMsgBox->AddMsg(strText, RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CGemIntegrationUnityResultMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationUnityResultMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::Exit();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGemIntegrationDisjointCheckMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGemIntegrationDisjointCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGemIntegrationDisjointCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationDisjointCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::ProcessCSAction();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	COMGEM::Exit();

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationDisjointCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::GetBack();

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(CGemIntegrationDisjointMsgBoxLayout));

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGemIntegrationDisjointResultMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	unicode::t_char strText[256] = { 0, };
	unicode::_sprintf(strText, "%s%s %s", GlobalText[1800], GlobalText[1816], GlobalText[858]);
	pMsgBox->AddMsg(strText, RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CGemIntegrationDisjointResultMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CGemIntegrationDisjointResultMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	COMGEM::Exit();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CChaosCastleTimeCheckMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CChaosCastleTimeCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CChaosCastleTimeCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CChaosCastleTimeCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CChaosCastleTimeCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	ITEM* pItem = g_pMyInventory->GetStandbyItem();
	if (pItem)
	{
		int iSrcIndex = g_pMyInventory->GetStandbyItemIndex();
		SendRequestMoveToEventMatch2(((pItem->Level >> 3) & 15), MAX_EQUIPMENT + iSrcIndex);
	}
	else
	{
		__asm { int 3 };
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CChaosCastleTimeCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CHarvestEventLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2020], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddCallbackFunc(CHarvestEventLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CHarvestEventLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CHarvestEventLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequest3ColorHarvestItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CHarvestEventLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CWhiteAngelEventLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2020], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CWhiteAngelEventLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CWhiteAngelEventLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CWhiteAngelEventLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestWhiteAngelItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CWhiteAngelEventLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool  SEASON3B::CLuckyItemMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	// ¾ÆÀÌÅÛ Á¦¸ñ
	int				nTextIndex[10] = { 0, };
	eLUCKYITEMTYPE	eAct = g_pLuckyItemWnd->GetAct();

	switch (eAct)
	{
	case eLuckyItemType_Trade:
		nTextIndex[0] = 3288;
		nTextIndex[1] = 3297;
		nTextIndex[2] = 3298;
		nTextIndex[3] = 3299;
		break;
	case eLuckyItemType_Refinery:
		nTextIndex[0] = 3289;
		nTextIndex[1] = 539;
		break;
	default:
		return false;
		break;
	}

	pMsgBox->AddMsg(GlobalText[nTextIndex[0]], RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	for (int i = 1; i < 10; i++)
	{
		if (nTextIndex[i] <= 0)	break;
		pMsgBox->AddMsg(GlobalText[nTextIndex[i]], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	}

	pMsgBox->AddCallbackFunc(CLuckyItemMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CLuckyItemMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CLuckyItemMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestMix(g_pLuckyItemWnd->SetWndAction(eLuckyItem_Act), 0);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CLuckyItemMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool  SEASON3B::CMixCheckMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	unicode::t_char strText[256];
	if (g_MixRecipeMgr.GetCurRecipe()->m_iMixName[1] == 0)
	{
		unicode::_sprintf(strText, "%s", GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[0]]);
	}
	else if (g_MixRecipeMgr.GetCurRecipe()->m_iMixName[2] == 0)
	{
		unicode::_sprintf(strText, "%s %s", GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[0]],
			GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[1]]);
	}
	else
	{
		unicode::_sprintf(strText, "%s %s %s", GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[0]],
			GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[1]],
			GlobalText[g_MixRecipeMgr.GetCurRecipe()->m_iMixName[2]]);
	}

	pMsgBox->AddMsg(strText, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[539], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddCallbackFunc(CMixCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CMixCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CMixCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_REQUESTED);
	SendRequestMix(g_MixRecipeMgr.GetCurMixID(), g_MixRecipeMgr.GetMixSubType());

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CMixCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUseReviveCharmMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2607]);
	pMsgBox->AddCallbackFunc(CUseReviveCharmMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUseReviveCharmMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CUseReviveCharmMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	BYTE srcIndex = g_pMyInventory->GetStandbyItemIndex();
	SendRequestUse(srcIndex, 0);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUseReviveCharmMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUsePortalCharmMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2609]);
	pMsgBox->AddCallbackFunc(CUsePortalCharmMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUsePortalCharmMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CUsePortalCharmMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	g_PortalMgr.SavePortalPosition();
	BYTE srcIndex = g_pMyInventory->GetStandbyItemIndex();
	SendRequestUse(srcIndex, 0);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUsePortalCharmMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CReturnPortalCharmMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2607]);
	pMsgBox->AddCallbackFunc(CReturnPortalCharmMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CReturnPortalCharmMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CReturnPortalCharmMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	BYTE srcIndex = g_pMyInventory->GetStandbyItemIndex();
	SendRequestUse(srcIndex, 0);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CReturnPortalCharmMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CDuelCreateErrorMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2692], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddMsg(GlobalText[2693], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CDuelCreateErrorMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CDuelCreateErrorMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CDuelWatchErrorMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2706], RGBA(255, 255, 128, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[2707], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CDuelWatchErrorMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CDuelWatchErrorMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CDoppelGangerMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddCallbackFunc(CDoppelGangerMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CDoppelGangerMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildRelationShipMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGuildRelationShipMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildRelationShipMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CGuildRelationShipMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CGuildRelationShipMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	const SEASON3B::ServerMessageInfo info = g_pGuildInfoWindow->GetServerMessage();

	SendRequestGuildRelationShipResult(info.s_byRelationShipType, info.s_byRelationShipRequestType, 0x01, info.s_byTargetUserIndexH, info.s_byTargetUserIndexL);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildRelationShipMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	const SEASON3B::ServerMessageInfo info = g_pGuildInfoWindow->GetServerMessage();

	SendRequestGuildRelationShipResult(info.s_byRelationShipType, info.s_byRelationShipRequestType, 0x00, info.s_byTargetUserIndexH, info.s_byTargetUserIndexL);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCastleMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CCastleMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CCastleMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CCastleMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	switch (g_pCastleWindow->GetCurrMsgBoxRequest())
	{
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_BUY_GATE:
		g_SenatusInfo.DoGateRepairAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_REPAIR_GATE:
		g_SenatusInfo.DoGateRepairAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_UPGRADE_GATE_HP:
		g_SenatusInfo.DoGateUpgradeHPAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_UPGRADE_GATE_DEFENSE:
		g_SenatusInfo.DoGateUpgradeDefenseAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_BUY_STATUE:
		g_SenatusInfo.DoStatueRepairAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_REPAIR_STATUE:
		g_SenatusInfo.DoStatueRepairAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_UPGRADE_STATUE_HP:
		g_SenatusInfo.DoStatueUpgradeHPAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_UPGRADE_STATUE_DEFENSE:
		g_SenatusInfo.DoStatueUpgradeDefenseAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_UPGRADE_STATUE_RECOVER:
		g_SenatusInfo.DoStatueUpgradeRecoverAction();
		break;
	case SEASON3B::CNewUICastleWindow::CASTLE_MSGREQ_APPLY_TAX:
		g_SenatusInfo.DoApplyTaxAction();
		break;
	default:
		break;
	};

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CCastleMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CSiegeLevelMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1429], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddMsg(GlobalText[1430], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CSiegeLevelMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CSiegeLevelMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CSiegeGiveUpMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[1498], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CSiegeGiveUpMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CSiegeGiveUpMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CSiegeGiveUpMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestBCGiveUp(0x01);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CSiegeGiveUpMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGatemanMoneyMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1627], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddMsg(GlobalText[1628], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CGatemanMoneyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CGatemanMoneyMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}
bool SEASON3B::CGatemanFailMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();

	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[860], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	pMsgBox->AddCallbackFunc(CGatemanFailMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CGatemanFailMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CQuestGiveUpMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[2817]);

	pMsgBox->AddCallbackFunc(CQuestGiveUpMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CQuestGiveUpMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CQuestGiveUpMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestQuestGiveUp(g_pMyQuestInfoWindow->GetSelQuestIndex());

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CQuestGiveUpMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

#ifdef ASG_ADD_TIME_LIMIT_QUEST
bool SEASON3B::CQuestCountLimitMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[3279]);
	pMsgBox->AddMsg(GlobalText[3280]);
	pMsgBox->AddMsg(GlobalText[3281]);
	pMsgBox->AddMsg(GlobalText[3282]);
	pMsgBox->AddMsg(GlobalText[3283]);

	pMsgBox->AddCallbackFunc(CQuestCountLimitMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return true;
}

CALLBACK_RESULT SEASON3B::CQuestCountLimitMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}
#endif	// ASG_ADD_TIME_LIMIT_QUEST

bool SEASON3B::CCanNotUseWordMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[392], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);
	pMsgBox->AddMsg(GlobalText[393], RGBA(255, 255, 255, 255), MSGBOX_FONT_NORMAL);

	return true;
}

bool SEASON3B::CHighValueItemCheckMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();
	ITEM* pItem = NULL;
	if (pPickedItem)
	{
		pItem = pPickedItem->GetItem();
	}
	if (pItem)
	{
		pMsgBox->Set3DItem(pItem);
	}

	pMsgBox->AddMsg(GlobalText[536], RGBA(255, 0, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[537], RGBA(255, 178, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[538], RGBA(255, 178, 0, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CHighValueItemCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CHighValueItemCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CHighValueItemCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	int iSourceIndex = -1;
	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	if (pPickedItem)
	{
		int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

		if (InvenIndex == -1)
		{
			iSourceIndex = pPickedItem->GetSourceLinealPos();
		}
		else
		{
			iSourceIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(pPickedItem->GetSourceLinealPos(), InvenIndex) + MAX_EQUIPMENT_INDEX;
		}
	}

	if (iSourceIndex != -1)
	{
		SendRequestSell(iSourceIndex);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CHighValueItemCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CNewUIInventoryCtrl::BackupPickedItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUseFruitMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	ITEM* pItem = g_pMyInventory->GetStandbyItem();
	if (pItem == NULL)
	{
		return false;
	}

	pMsgBox->Set3DItem(pItem);

	unicode::t_char strName[50] = { 0, };
	if (pItem->Type == ITEM_HELPER + 15)
	{
		switch ((pItem->Level >> 3) & 15)
		{
		case 0:
			unicode::_sprintf(strName, "%s", GlobalText[168]); //Năng lượng
			break;
		case 1:
			unicode::_sprintf(strName, "%s", GlobalText[169]); //Thể lực
			break;
		case 2:
			unicode::_sprintf(strName, "%s", GlobalText[167]); //Nhanh nhẹn
			break;
		case 3:
			unicode::_sprintf(strName, "%s", GlobalText[166]); //Sức mạnh
			break;
		case 4:
			unicode::_sprintf(strName, "%s", GlobalText[1900]); //Mệnh Lệnh
			break;
		}
	}

	pMsgBox->AddMsg(strName, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[376], RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CUseFruitMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUseFruitMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CUseFruitMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CUseFruitCheckMsgBoxLayout));

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUseFruitMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUsePartChargeFruitMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	ITEM* pItem = g_pMyInventory->GetStandbyItem();
	if (pItem == NULL)
	{
		return false;
	}

	pMsgBox->Set3DItem(pItem);

	unicode::t_char strName[50] = { 0, };

	if (pItem->Type == ITEM_HELPER + 54)
	{
		unicode::_sprintf(strName, "%s", GlobalText[166]);
	}
	else if (pItem->Type == ITEM_HELPER + 55)
	{
		unicode::_sprintf(strName, "%s", GlobalText[167]);
	}
	else if (pItem->Type == ITEM_HELPER + 56)
	{
		unicode::_sprintf(strName, "%s", GlobalText[169]);
	}
	else if (pItem->Type == ITEM_HELPER + 57)
	{
		unicode::_sprintf(strName, "%s", GlobalText[168]);
	}
	else if (pItem->Type == ITEM_HELPER + 58)
	{
		unicode::_sprintf(strName, "%s", GlobalText[1900]);
	}

	pMsgBox->AddMsg(strName, RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[2524], RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[2525], RGBA(255, 255, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CUsePartChargeFruitMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUsePartChargeFruitMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CUsePartChargeFruitMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	BYTE srcIndex = g_pMyInventory->GetStandbyItemIndex();
	SendRequestUse(srcIndex, 0);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);
	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUsePartChargeFruitMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);
	return CALLBACK_BREAK;
}

bool SEASON3B::CPersonalShopItemValueCheckMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	ITEM* pItemObj = NULL;
	if (g_pMyShopInventory->GetTargetIndex() == -1)
	{
		pItemObj = g_pMyShopInventory->FindItem(g_pMyShopInventory->GetSourceIndex());
	}
	else
	{
		if (pPickedItem)
		{
			pItemObj = pPickedItem->GetItem();
		}
	}

	if (pItemObj)
	{
		pMsgBox->Set3DItem(pItemObj);
	}

	pMsgBox->AddCallbackFunc(CPersonalShopItemValueCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CPersonalShopItemValueCheckMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_RETURN);
	pMsgBox->AddCallbackFunc(CPersonalShopItemValueCheckMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);

	return true;
}

CALLBACK_RESULT SEASON3B::CPersonalShopItemValueCheckMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CNewUI3DItemCommonMsgBox* pMsgBox = dynamic_cast<CNewUI3DItemCommonMsgBox*>(pOwner);
	if (pMsgBox == false)
	{
		return CALLBACK_CONTINUE;
	}

	if (g_pMyShopInventory->IsEnablePersonalShop() == true)
	{
		SendRequestDestoryPersonalShop();
	}

	CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

	int iSourceIndex = -1, iTargetIndex = -1;

	if (pPickedItem)
	{
		ITEM* pItemObj = pPickedItem->GetItem();
		iSourceIndex = pPickedItem->GetSourceLinealPos();
		iTargetIndex = g_pMyShopInventory->GetTargetIndex();

		int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

		if (InvenIndex != -1)
		{
			iSourceIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(iSourceIndex, InvenIndex);

			int iItemPrice = pMsgBox->GetItemValue();
			BYTE byIndex = MAX_EQUIPMENT + iSourceIndex;

			SendRequestSetSalePrice(byIndex, iItemPrice, 0, 0, 0, 0);

			SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, MAX_EQUIPMENT + iSourceIndex, pItemObj, REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iTargetIndex);
		}

		else if (pPickedItem->GetOwnerInventory() == NULL)
		{
			int iItemPrice = pMsgBox->GetItemValue();
			BYTE byIndex = iSourceIndex;
			SendRequestSetSalePrice(byIndex, iItemPrice, 0, 0, 0, 0);

			SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, iSourceIndex, pItemObj, REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iTargetIndex);
		}
		else if (pPickedItem->GetOwnerInventory() == g_pMyShopInventory->GetInventoryCtrl())
		{
			int iItemPrice = pMsgBox->GetItemValue();
			BYTE byIndex = MAX_MY_INVENTORY_INDEX + iSourceIndex;

			SendRequestSetSalePrice(byIndex, iItemPrice, 0, 0, 0, 0);

			SendRequestEquipmentItem(REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iSourceIndex, pItemObj, REQUEST_EQUIPMENT_MYSHOP, MAX_MY_INVENTORY_INDEX + iTargetIndex);
		}

		AddPersonalItemPrice(MAX_MY_INVENTORY_INDEX + iTargetIndex, 0, pMsgBox->GetItemValue(), 0, 0, 0, g_IsPurchaseShop);
	}
	else
	{
		iSourceIndex = g_pMyShopInventory->GetSourceIndex();
		iSourceIndex += MAX_MY_INVENTORY_INDEX;

		int iItemPrice = pMsgBox->GetItemValue();
		BYTE byIndex = iSourceIndex;
		SendRequestSetSalePrice(byIndex, iItemPrice, 0, 0, 0, 0);
		AddPersonalItemPrice(byIndex, 0, iItemPrice, 0, 0, 0, g_IsPurchaseShop);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}


CALLBACK_RESULT SEASON3B::CPersonalShopItemValueCheckMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CPersonalShopItemBuyMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	ITEM* pItem = g_pPurchaseShopInventory->FindItem(g_pPurchaseShopInventory->GetSourceIndex());
	if (pItem)
	{
		pMsgBox->Set3DItem(pItem);
	}

	pMsgBox->AddMsg(GlobalText[1100]);
	pMsgBox->AddCallbackFunc(CPersonalShopItemBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CPersonalShopItemBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_RETURN);
	pMsgBox->AddCallbackFunc(CPersonalShopItemBuyMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CPersonalShopItemBuyMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	ITEM* pItem = g_pPurchaseShopInventory->FindItem(g_pPurchaseShopInventory->GetSourceIndex());
	CHARACTER* pCha = gmCharacters->GetCharacter(g_pPurchaseShopInventory->GetShopCharacterIndex());

	if (pItem && pCha)
	{
		int sourceIndex = MAX_MY_INVENTORY_INDEX + g_pPurchaseShopInventory->GetSourceIndex();
		SendRequestPurchase(pCha->Key, pCha->ID, sourceIndex);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CPersonalShopItemBuyMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::COsbourneMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2223], RGBA(255, 0, 0, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(" ");
	pMsgBox->AddMsg(GlobalText[2224], RGBA(223, 191, 103, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(COsbourneMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(COsbourneMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::COsbourneMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	g_MixRecipeMgr.SetMixType(SEASON3A::MIXTYPE_OSBOURNE);
	g_pNewUISystem->Show(SEASON3B::INTERFACE_MIXINVENTORY);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildOutPerson::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1270], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CGuildOutPerson::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildOutPerson::OkBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CGuildOutPerson::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildBreakMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddMsg(GlobalText[1363], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[1364], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[1365], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[1366], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);

	pMsgBox->AddCallbackFunc(CGuildBreakMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildBreakMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CGuildBreakMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

CALLBACK_RESULT SEASON3B::CGuildBreakMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGuildBreakPasswordMsgBoxLayout));

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildBreakMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildPerson_Get_Out::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	char Buff[300];
	wsprintf(Buff, GlobalText[1367], GuildList[DeleteIndex].Name);
	pMsgBox->AddMsg(Buff, RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddMsg(GlobalText[1369], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CGuildPerson_Get_Out::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildPerson_Get_Out::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CGuildPerson_Get_Out::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

CALLBACK_RESULT SEASON3B::CGuildPerson_Get_Out::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGuildBreakPasswordMsgBoxLayout));

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildPerson_Get_Out::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);
	return CALLBACK_BREAK;
}

bool SEASON3B::CGuildPerson_Cancel_Position_MsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CGuildPerson_Cancel_Position_MsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGuildPerson_Cancel_Position_MsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CGuildPerson_Cancel_Position_MsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

CALLBACK_RESULT SEASON3B::CGuildPerson_Cancel_Position_MsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestGuildAssign(0x03, G_PERSON, GuildList[DeleteIndex].Name);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGuildPerson_Cancel_Position_MsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Result_Set_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	char Text[300];

	wsprintf(Text, "%s    %s    %s    %s", GlobalText[680], GlobalText[681], GlobalText[1973], GlobalText[1977]);

	int TextColor = (255 << 24) + (21 << 16) + (148 << 8) + (255);
	pMsgBox->AddMsg(Text, TextColor);

	TextColor = (255 << 24) + (255 << 16) + (255 << 8) + (255);

	for (int i = 0; i < 5; i++)
	{
		if (HeroScore[i] == -1)
			continue;

		wsprintf(Text, "%d      %s      %s      %d", i + 1, HeroName[i], gCharacterManager.GetCharacterClassText(HeroClass[i]), HeroScore[i]);

		pMsgBox->AddMsg(Text, TextColor);
	}

	TextColor = (255 << 24) + (255 << 16) + (0 << 8) + (255);
	pMsgBox->AddMsg("    ", TextColor);
	pMsgBox->AddMsg("    ", TextColor);
	pMsgBox->AddMsg("    ", TextColor);
	pMsgBox->AddMsg("    ", TextColor);

	if (View_Suc_Or_Fail == 1)
	{
		wsprintf(Text, GlobalText[2000]);
		pMsgBox->AddMsg(Text, TextColor);
		wsprintf(Text, GlobalText[2001]);
		pMsgBox->AddMsg(Text, TextColor);
	}
	else
	{
		pMsgBox->AddMsg(GlobalText[2009], TextColor);
	}

	pMsgBox->AddCallbackFunc(CCry_Wolf_Result_Set_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Result_Set_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Ing_Set_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2002]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Ing_Set_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Ing_Set_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Destroy_Set_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2003]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Destroy_Set_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Destroy_Set_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Wat_Set_Temple1::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2008]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Wat_Set_Temple1::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Wat_Set_Temple1::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Dont_Set_Temple1::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[2004]);
	pMsgBox->AddMsg(GlobalText[2005]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Dont_Set_Temple1::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Dont_Set_Temple1::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Dont_Set_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1956]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Dont_Set_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Dont_Set_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Set_Temple1::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1952]);


	pMsgBox->AddCallbackFunc(CCry_Wolf_Set_Temple1::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Set_Temple1::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Set_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1953]);
	pMsgBox->AddMsg(GlobalText[1954]);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(PartyKey));

	if (pCharacter)
	{
		BackUp_Key = pCharacter->Key;
	}

	pMsgBox->AddCallbackFunc(CCry_Wolf_Get_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Set_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}
//CMaster_Level_Interface
bool SEASON3B::CMaster_Level_Interface::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

#if MAIN_UPDATE > 303
	int Need_Point = g_pMasterSkillTree->GetConsumePoint();

	char szText[256];
	pMsgBox->AddMsg(GlobalText[1771]);
	unicode::_sprintf(szText, GlobalText[1772], Need_Point);
	pMsgBox->AddMsg(szText);

	pMsgBox->AddCallbackFunc(CMaster_Level_Interface::OkBtnDown, MSGBOX_EVENT_PRESSKEY_RETURN);
	pMsgBox->AddCallbackFunc(CMaster_Level_Interface::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CMaster_Level_Interface::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	//	pMsgBox->AddCallbackFunc(CCry_Wolf_Get_Temple::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
#endif // MAIN_UPDATE != 303
	return true;
}

CALLBACK_RESULT SEASON3B::CMaster_Level_Interface::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	MouseLButton = false;
	MouseLButtonPop = false;
	MouseLButtonPush = false;
	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CMaster_Level_Interface::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
#if MAIN_UPDATE > 303
	int In_Skill = g_pMasterSkillTree->GetCurSkillID();

	SendRequestMasterLevelSkill(In_Skill);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	MouseLButton = false;
	MouseLButtonPop = false;
	MouseLButtonPush = false;
#endif // MAIN_UPDATE != 303

	return CALLBACK_BREAK;
}

bool SEASON3B::CCry_Wolf_Get_Temple::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	char szText[256];
	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(TargetNpc));

	if (pCharacter)
	{
		BackUp_Key = pCharacter->Key;
		int Num = pCharacter->Object.Type - MODEL_CRYWOLF_ALTAR1;
		BYTE State = (m_AltarState[Num] & 0x0f);
		unicode::_sprintf(szText, GlobalText[2006], State);
		pMsgBox->AddMsg(szText);
		pMsgBox->AddMsg(GlobalText[2007]);
		BackUp_Key = pCharacter->Key;
		pMsgBox->AddCallbackFunc(CCry_Wolf_Get_Temple::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
		pMsgBox->AddCallbackFunc(CCry_Wolf_Get_Temple::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	}
	return true;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Get_Temple::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	Button_Down = 1;
	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Set_Temple1));
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CCry_Wolf_Get_Temple::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	if (Hero->Helper.Type == MODEL_HELPER + 2 || Hero->Helper.Type == MODEL_HELPER + 3 || Hero->Helper.Type == MODEL_HELPER + 37)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Dont_Set_Temple));
	}
	else
	{
		Button_Down = 2;
		SendRequestCrywolfAltarContract(BackUp_Key);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUnionGuild_Break_MsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	char szText[256];
	unicode::_sprintf(szText, GlobalText[1423], DeleteID);
	pMsgBox->AddMsg(szText);
	pMsgBox->AddMsg(GlobalText[1369]);
	pMsgBox->AddCallbackFunc(CUnionGuild_Break_MsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUnionGuild_Break_MsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CUnionGuild_Break_MsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

CALLBACK_RESULT SEASON3B::CUnionGuild_Break_MsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUnionGuild_Break_MsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestBanUnionGuild(DeleteID);

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUnionGuild_Out_MsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return false;

	pMsgBox->AddMsg(GlobalText[1271], RGBA(255, 255, 255, 255), MSGBOX_FONT_BOLD);
	pMsgBox->AddCallbackFunc(CUnionGuild_Out_MsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUnionGuild_Out_MsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

CALLBACK_RESULT SEASON3B::CUnionGuild_Out_MsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUseSantaInvitationMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return FALSE;

	pMsgBox->AddMsg(GlobalText[2590]);
	pMsgBox->AddCallbackFunc(CUseSantaInvitationMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CUseSantaInvitationMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return TRUE;
}

CALLBACK_RESULT SEASON3B::CUseSantaInvitationMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	ITEM* pItem = g_pMyInventory->GetStandbyItem();

	if (pItem)
	{
		int iSrcIndex = g_pMyInventory->GetStandbyItemIndex();
		SendRequestUse(iSrcIndex, 0);
	}
	else
	{
		//N/A
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CUseSantaInvitationMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool CSantaTownLeaveMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return FALSE;

	pMsgBox->AddMsg(GlobalText[2586]);
	pMsgBox->AddCallbackFunc(CSantaTownLeaveMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CSantaTownLeaveMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return TRUE;
}

CALLBACK_RESULT CSantaTownLeaveMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestMoveDevias();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT CSantaTownLeaveMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool CSantaTownSantaMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return FALSE;

	pMsgBox->AddCallbackFunc(CSantaTownSantaMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CSantaTownSantaMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return TRUE;
}

CALLBACK_RESULT CSantaTownSantaMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestSantaItem();

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT CSantaTownSantaMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUseRegistLuckyCoinMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	unicode::t_char szText[100] = { 0, };
	unicode::_sprintf(szText, GlobalText[580], GlobalText[1894]);
	pMsgBox->AddMsg(szText);
	pMsgBox->AddCallbackFunc(CUseRegistLuckyCoinMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	return TRUE;
}

CALLBACK_RESULT SEASON3B::CUseRegistLuckyCoinMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CRegistOverLuckyCoinMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	pMsgBox->AddMsg(GlobalText[2859]);

	pMsgBox->AddCallbackFunc(CRegistOverLuckyCoinMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return TRUE;
}
CALLBACK_RESULT SEASON3B::CRegistOverLuckyCoinMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}


bool SEASON3B::CExchangeLuckyCoinMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	unicode::t_char szText[100] = { 0, };
	unicode::_sprintf(szText, GlobalText[580], GlobalText[1940]);
	pMsgBox->AddMsg(szText);

	pMsgBox->AddCallbackFunc(CExchangeLuckyCoinMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return TRUE;
}

CALLBACK_RESULT SEASON3B::CExchangeLuckyCoinMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CExchangeLuckyCoinInvenErrMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	pMsgBox->AddMsg(GlobalText[2306]);

	pMsgBox->AddCallbackFunc(CExchangeLuckyCoinInvenErrMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return TRUE;
}

CALLBACK_RESULT SEASON3B::CExchangeLuckyCoinInvenErrMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CGambleBuyMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	ITEM* pItem = g_pNPCShop->GetStandbyItem();
	if (pItem == NULL)
	{
		return false;
	}
	pMsgBox->Set3DItem(pItem);
	pMsgBox->AddMsg(GlobalText[1612]);
	pMsgBox->AddCallbackFunc(CGambleBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CGambleBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_RETURN);
	pMsgBox->AddCallbackFunc(CGambleBuyMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CGambleBuyMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	GambleSystem& gambleSys = GambleSystem::Instance();
	LPBUYITEMINFO pItemInfo = NULL;

	if (gambleSys.IsGambleShop())
	{
		pItemInfo = gambleSys.GetBuyItemInfo();
		SendRequestBuy(pItemInfo->ItemIndex, pItemInfo->ItemCost);
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CGambleBuyMsgBoxLayout::CancelBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CEmpireGuardianMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	pMsgBox->AddCallbackFunc(CEmpireGuardianMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return TRUE;
}

CALLBACK_RESULT SEASON3B::CEmpireGuardianMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CUnitedMarketPlaceMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return FALSE;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OK))
		return FALSE;

	pMsgBox->AddCallbackFunc(CUnitedMarketPlaceMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);

	return TRUE;
}
CALLBACK_RESULT SEASON3B::CUnitedMarketPlaceMsgBoxLayout::OkBtnDown(class CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CConfirmBuyMsgBoxLayout::SetLayout()
{
	CNewUI3DItemCommonMsgBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;
	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	ITEM* pItem = g_pNPCShop->GetStandbyBuyItem();

	if (pItem == NULL)
	{
		return false;
	}
	pMsgBox->Set3DItem(pItem);
	pMsgBox->AddMsg(GlobalText[536], ARGB(255, 252, 0, 0), 1);
	pMsgBox->AddMsg(GlobalText[537], RGBA(0xFFu, 0xB2u, 0, 0xFFu), 1);
	pMsgBox->AddMsg(GlobalText[1612], RGBA(0xFFu, 0xB2u, 0, 0xFFu), 1);
	pMsgBox->AddCallbackFunc(CConfirmBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CConfirmBuyMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_PRESSKEY_RETURN);
	pMsgBox->AddCallbackFunc(CConfirmBuyMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	return true;
}

CALLBACK_RESULT SEASON3B::CConfirmBuyMsgBoxLayout::OkBtnDown(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	SendRequestBuy(g_pNPCShop->GetStandbyBuyItemKey());

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CConfirmBuyMsgBoxLayout::CancelBtnDown(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

bool SEASON3B::CBuyInvExtRequestMsgBoxLayout::SetLayout()
{
	CNewUICommonMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CBuyInvExtRequestMsgBoxLayout::OkBtnDown, MSGBOX_EVENT_USER_COMMON_OK);
	pMsgBox->AddCallbackFunc(CBuyInvExtRequestMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_USER_COMMON_CANCEL);
	pMsgBox->AddCallbackFunc(CBuyInvExtRequestMsgBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);

	return true;
}

CALLBACK_RESULT SEASON3B::CBuyInvExtRequestMsgBoxLayout::OkBtnDown(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	SendRequestBuyInventory(g_pMyInventory->GetInvenEnableCnt() - 1);
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CBuyInvExtRequestMsgBoxLayout::CancelBtnDown(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

SEASON3B::CTradeCoinMsgBoxLayout::CTradeCoinMsgBoxLayout()
{
	m_pInputBox[0] = NULL;
	m_pInputBox[1] = NULL;
	m_pInputBox[2] = NULL;
	m_pInputBox[3] = NULL;
	//m_pInputBox = NULL;
	m_dwType = MSGBOX_COMMON_TYPE_OK;
}

SEASON3B::CTradeCoinMsgBoxLayout::~CTradeCoinMsgBoxLayout()
{
	SAFE_DELETE(m_pInputBox[0]);
	SAFE_DELETE(m_pInputBox[1]);
	SAFE_DELETE(m_pInputBox[2]);
	SAFE_DELETE(m_pInputBox[3]);
}

DWORD SEASON3B::CTradeCoinMsgBoxLayout::GetType()
{
	return m_dwType;
}

bool SEASON3B::CTradeCoinMsgBoxLayout::Create(DWORD dwType, const type_string& strMsg, DWORD dwColor, BYTE byFontType, float fPriority)
{
	int x, y, width, height;

	m_dwType = dwType;

	SetAddCallbackFunc();

	x = (SCREEN_WIDTH / 2) - (MSGBOX_WIDTH / 2);
	y = 100;
	width = MSGBOX_WIDTH;
#if MAIN_UPDATE > 303
	height = MSGBOX_LINE_WIDTH;
#else
	height = MSGBOX_LINE_WIDTH - 50;
#endif
	CNewUIMessageBoxBase::Create(x, y, width, height, fPriority);

	SetButtonInfo();

	CreateInput();

	return false;
}

void SEASON3B::CTradeCoinMsgBoxLayout::SetAddCallbackFunc()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		AddCallbackFunc(CTradeCoinMsgBoxLayout::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(CTradeCoinMsgBoxLayout::Success, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(CTradeCoinMsgBoxLayout::Close, MSGBOX_EVENT_USER_COMMON_CANCEL);
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		AddCallbackFunc(CTradeCoinMsgBoxLayout::LButtonUp, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
		AddCallbackFunc(CTradeCoinMsgBoxLayout::Success, MSGBOX_EVENT_USER_COMMON_OK);
		AddCallbackFunc(CTradeCoinMsgBoxLayout::Close, MSGBOX_EVENT_USER_COMMON_CANCEL);
		break;
	}
}

void SEASON3B::CTradeCoinMsgBoxLayout::SetButtonInfo()
{
	float x, y;

	if (m_dwType == MSGBOX_COMMON_TYPE_OKCANCEL)
	{
		x = GetPos().x + ((GetSize().cx / 2 - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - 50;
		BtnOK.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, MSGBOX_BTN_WIDTH, MSGBOX_BTN_HEIGHT, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK, 0);

		x = GetPos().x + ((GetSize().cx / 2 - MSGBOX_BTN_WIDTH) / 2) + (GetSize().cx / 2);
		BtnExit.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_CANCEL, x, y, MSGBOX_BTN_WIDTH, MSGBOX_BTN_HEIGHT, CNewUIMessageBoxButton::MSGBOX_BTN_SIZE_OK, 0);
	}
	else
	{
		x = GetPos().x + ((GetSize().cx - MSGBOX_BTN_WIDTH) / 2);
		y = GetPos().y + GetSize().cy - 50;
		BtnOK.SetInfo(CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_OK, x, y, MSGBOX_BTN_WIDTH, MSGBOX_BTN_HEIGHT, 1, 0);
	}
}

void SEASON3B::CTradeCoinMsgBoxLayout::CreateInput()
{
	int x = GetPos().x + 120;
	int y = GetPos().y + 65;

	for (int i = 0; i < 4; i++)
	{
		SAFE_DELETE(m_pInputBox[i]);
		m_pInputBox[i] = new CUITextInputBox;

		if (m_pInputBox[i])
		{
			m_pInputBox[i]->Init(gwinhandle->GethWnd(), 50, 12, 10, false);

			m_pInputBox[i]->SetPosition(x, y);
			m_pInputBox[i]->SetTextColor(255, 255, 230, 210);
			m_pInputBox[i]->SetBackColor(255, 0, 0, 0);
			m_pInputBox[i]->SetFont(g_hFont);
			m_pInputBox[i]->SetState(0);
			m_pInputBox[i]->SetOption(9);
			m_pInputBox[i]->SetText("0");
			y += 25;
		}
	}

	if (m_pInputBox[0] != NULL && m_pInputBox[1] != NULL)
	{
		m_pInputBox[0]->GiveFocus();
		m_pInputBox[0]->SetTabTarget(m_pInputBox[1]);
	}
	if (m_pInputBox[1] != NULL && m_pInputBox[2] != NULL)
	{
		m_pInputBox[1]->SetTabTarget(m_pInputBox[2]);
	}
	if (m_pInputBox[2] != NULL && m_pInputBox[3] != NULL)
	{
		m_pInputBox[2]->SetTabTarget(m_pInputBox[3]);
	}
	if (m_pInputBox[3] != NULL && m_pInputBox[0] != NULL)
	{
		m_pInputBox[3]->SetTabTarget(m_pInputBox[0]);
	}
}

bool SEASON3B::CTradeCoinMsgBoxLayout::Update()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		BtnOK.Update();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		BtnOK.Update();
		BtnExit.Update();
		break;
	}

	bool HaveFocus = false;

#if MAIN_UPDATE > 303
	for (int i = 0; i < 4; i++)
	{
		if (m_pInputBox[i])
		{
			m_pInputBox[i]->DoAction();

			if (m_pInputBox[i]->HaveFocus() && g_MessageBox->GetRelatedWnd() != m_pInputBox[i]->GetHandle())
			{
				g_MessageBox->SetRelatedWnd(m_pInputBox[i]->GetHandle());
			}


			if (m_pInputBox[i]->HaveFocus())
			{
				HaveFocus = true;
			}
		}
	}
#else
	for (int i = 0; i < 2; i++)
	{
		if (m_pInputBox[i])
		{
			m_pInputBox[i]->DoAction();

			if (m_pInputBox[i]->HaveFocus() && g_MessageBox->GetRelatedWnd() != m_pInputBox[i]->GetHandle())
			{
				g_MessageBox->SetRelatedWnd(m_pInputBox[i]->GetHandle());
			}


			if (m_pInputBox[i]->HaveFocus())
			{
				HaveFocus = true;
			}
		}
	}
#endif

	if (HaveFocus == false && g_MessageBox->GetRelatedWnd() != gwinhandle->GethWnd())
	{
		g_MessageBox->SetRelatedWnd(gwinhandle->GethWnd());
	}

	return true;
}

bool SEASON3B::CTradeCoinMsgBoxLayout::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderFrame();

	RenderTexts();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CTradeCoinMsgBoxLayout::RenderFrame()
{
	float x, y, height;

	x = GetPos().x;
	y = GetPos().y;

#if MAIN_UPDATE > 303
	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK, x, y + 2.0, 222.0, (GetSize().cy - 8.0));
	y += 4;
	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_TOP, x, y, 230.0, 67.0);

	y += 67.0;
	height = 15.0;

	for (int i = 0; i < 7; ++i)
	{
		SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_MIDDLE, x, y, 230.0, height);
		y += height;
	}
#else
	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK, x, y + 2.0, 222.0, (GetSize().cy - 8.0));
	y += 4;
	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_TOP, x, y, 230.0, 67.0);

	y += 67.0;
	height = 15.0;

	for (int i = 0; i < 4; ++i)
	{
		SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_MIDDLE, x, y, 230.0, height);
		y += height;
	}
#endif
	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_BOTTOM, x, y, 230.0, 50.0);

	y = GetPos().y + 60.0;
	height = 25.0;

#if MAIN_UPDATE > 303
	for (int i = 0; i < 4; ++i)
	{
		SEASON3B::RenderImage(CNewUICharacterInfoWindow::IMAGE_CHAINFO_TEXTBOX, x + 30, y, 170, 21.0);
		y += height;
}
#else
	for (int i = 0; i < 2; ++i)
	{
		SEASON3B::RenderImage(CNewUICharacterInfoWindow::IMAGE_CHAINFO_TEXTBOX, x + 30, y, 170, 21.0);
		y += height;
	}
#endif
}

void SEASON3B::CTradeCoinMsgBoxLayout::RenderTexts()
{
	int x = GetPos().x;
	int y = GetPos().y + 40;

	g_pRenderText->RenderFont(x, y, GlobalText[422], 230, 0, 3);

	y = GetPos().y + 60;

	const char* pzText[] = { GlobalText[224], TextGlobal[51], TextGlobal[52], TextGlobal[53] };

#if MAIN_UPDATE > 303
	for (int i = 0; i < 4; ++i)
	{
		g_pRenderText->RenderFont(x + 30, y, pzText[i], 85, 21, 3);
		y += 25;
}
#else
	for (int i = 0; i < 2; ++i)
	{
		g_pRenderText->RenderFont(x + 30, y, pzText[i], 85, 21, 3);
		y += 25;
	}
#endif
}

void SEASON3B::CTradeCoinMsgBoxLayout::RenderButtons()
{
	switch (m_dwType)
	{
	case MSGBOX_COMMON_TYPE_OK:
		BtnOK.Render();
		break;
	case MSGBOX_COMMON_TYPE_OKCANCEL:
		BtnOK.Render();
		BtnExit.Render();
		break;
	}

#if MAIN_UPDATE > 303
	for (int i = 0; i < 4; i++)
	{
		if (m_pInputBox[i] != NULL)
			m_pInputBox[i]->Render();
	}
#else
	for (int i = 0; i < 2; i++)
	{
		if (m_pInputBox[i] != NULL)
			m_pInputBox[i]->Render();
	}
#endif
}

void SEASON3B::CTradeCoinMsgBoxLayout::ProcessOk()
{
	int nZen = 0;
	int nCoin1 = 0;
	int nCoin2 = 0;
	int nCoin3 = 0;

	if (m_pInputBox[0] != NULL)
		nZen = m_pInputBox[0]->GetAsInt();

	if (m_pInputBox[1] != NULL)
		nCoin1 = m_pInputBox[1]->GetAsInt();

	if (m_pInputBox[2] != NULL)
		nCoin2 = m_pInputBox[2]->GetAsInt();

	if (m_pInputBox[3] != NULL)
		nCoin3 = m_pInputBox[3]->GetAsInt();

	if (nZen != 0 || nCoin1 != 0 || nCoin2 != 0 || nCoin3 != 0)
	{
		g_pTrade->SendRequestMyGoldInput(nZen, nCoin1, nCoin2, nCoin3);
	}
}

CALLBACK_RESULT SEASON3B::CTradeCoinMsgBoxLayout::Close(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CTradeCoinMsgBoxLayout::Success(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CTradeCoinMsgBoxLayout* pMsgBox = dynamic_cast<CTradeCoinMsgBoxLayout*>(pOwner);

	if (pMsgBox)
	{
		pMsgBox->ProcessOk();
	}

	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}

CALLBACK_RESULT SEASON3B::CTradeCoinMsgBoxLayout::LButtonUp(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	CTradeCoinMsgBoxLayout* pMsgBox = dynamic_cast<CTradeCoinMsgBoxLayout*>(pOwner);
	if (pMsgBox)
	{
		switch (pMsgBox->GetType())
		{
		case MSGBOX_COMMON_TYPE_OK:
			if (pMsgBox->BtnOK.IsMouseIn() == true)
			{
				pMsgBox->ProcessOk();
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			break;
		case MSGBOX_COMMON_TYPE_OKCANCEL:
			if (pMsgBox->BtnOK.IsMouseIn() == true)
			{
				pMsgBox->ProcessOk();
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_OK);
				return CALLBACK_BREAK;
			}
			if (pMsgBox->BtnExit.IsMouseIn() == true)
			{
				g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_USER_COMMON_CANCEL);
				return CALLBACK_BREAK;
			}
			break;
		}
	}
	return CALLBACK_CONTINUE;
}

#ifdef EFFECT_KERNEL_VERTEX
bool SEASON3B::CNewUIMakeRenderMessageBox::Create(DWORD dwType, float fPriority)
{
	float RenderSizeX = 459.f;
	float RenderSizeY = 264.f;

	float RenderFrameX = (GetWindowsX - RenderSizeX) * 0.5f;
	float RenderFrameY = (((GetWindowsY - 51.f) - RenderSizeY) * 0.5f);

	CNewUIMessageBoxBase::Create(RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY, fPriority);

	return true;
}

bool SEASON3B::CNewUIMakeRenderMessageBox::Create(DWORD dwType, const type_string& strMsg, DWORD dwColor, BYTE byFontType, float fPriority)
{
	float RenderSizeX = 459.f;
	float RenderSizeY = 264.f;

	float RenderFrameX = (GetWindowsX - RenderSizeX) * 0.5f;
	float RenderFrameY = (((GetWindowsY - 51.f) - RenderSizeY) * 0.5f);

	CNewUIMessageBoxBase::Create(RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY, fPriority);


	//AddCallbackFunc(SEASON3B::CSystemMenuMsgBox::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	return true;
}

bool SEASON3B::CNewUIMakeRenderMessageBox::Update()
{
	GMRenderGroupMesh->runtime_update_interface();

	return true;
}

bool SEASON3B::CNewUIMakeRenderMessageBox::Render()
{
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderMsgBackColor(true);

	GMRenderGroupMesh->runtime_make_interface();

	DisableAlphaBlend();
	return true;
}

bool SEASON3B::CNewUIMakeRenderMessageBoxLayout::SetLayout()
{
	CNewUIMakeRenderMessageBox* pMsgBox = GetMsgBox();
	if (0 == pMsgBox)
		return false;

	if (false == pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL))
		return false;

	pMsgBox->AddCallbackFunc(CNewUIMakeRenderMessageBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_ESC);
	pMsgBox->AddCallbackFunc(CNewUIMakeRenderMessageBoxLayout::CancelBtnDown, MSGBOX_EVENT_PRESSKEY_Y);

	return true;
}

CALLBACK_RESULT SEASON3B::CNewUIMakeRenderMessageBoxLayout::CancelBtnDown(CNewUIMessageBoxBase* pOwner, const leaf::xstreambuf& xParam)
{
	PlayBuffer(SOUND_CLICK01);
	g_MessageBox->SendEvent(pOwner, MSGBOX_EVENT_DESTROY);

	return CALLBACK_BREAK;
}
#endif // EFFECT_KERNEL_VERTEX
