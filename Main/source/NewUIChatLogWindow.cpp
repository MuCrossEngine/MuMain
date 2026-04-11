
#include "stdafx.h"

#include "NewUIChatLogWindow.h"
#include "NewUIManager.h"
#include "NewUISystem.h"
#include "DSPlaySound.h"
#include "UIControls.h"
#include "ZzzInterface.h"
#include "CGMProtect.h"
#include "ScaleForm.h"

using namespace SEASON3B;
extern float MouseX;
extern float MouseY;

SEASON3B::CNewUIChatLogWindow::CNewUIChatLogWindow()
{
	Init();
}

SEASON3B::CNewUIChatLogWindow::~CNewUIChatLogWindow()
{
	Release();
}

void SEASON3B::CNewUIChatLogWindow::Init()
{
	m_pNewUIMng = NULL;
	m_WndPos.x = 0;
	m_WndPos.y = 0;
	m_ScrollBtnPos.x = 0;
	m_ScrollBtnPos.y = 0;
#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
		m_WndSize.cx = 268;
	else
		m_WndSize.cx = WND_WIDTH;
#else
	m_WndSize.cx = WND_WIDTH;
#endif
	m_WndSize.cy = 0;
	m_nShowingLines = 6;
	m_iCurrentRenderEndLine = -1;
	m_fBackAlpha = 0.6f;
	m_EventState = EVENT_NONE;
	m_bShowFrame = false;
	m_CurrentRenderMsgType = TYPE_ALL_MESSAGE;
	m_bShowChatLog = true;
	m_bPointedMessage = false;
	m_iPointedMessageIndex = 0;

	SetAllMsgEnable();
#if MAIN_UPDATE > 303
	m_msgEnable[TYPE_SYSTEM_MESSAGE] = false;
#endif
}

void SEASON3B::CNewUIChatLogWindow::LoadImages()
{
	LoadBitmap("Interface\\newui_scrollbar_up.tga", IMAGE_SCROLL_TOP, GL_LINEAR);
	LoadBitmap("Interface\\newui_scrollbar_m.tga", IMAGE_SCROLL_MIDDLE, GL_LINEAR);
	LoadBitmap("Interface\\newui_scrollbar_down.tga", IMAGE_SCROLL_BOTTOM, GL_LINEAR);
	LoadBitmap("Interface\\newui_scroll_on.tga", IMAGE_SCROLLBAR_ON, GL_LINEAR);
	LoadBitmap("Interface\\newui_scroll_off.tga", IMAGE_SCROLLBAR_OFF, GL_LINEAR);
	LoadBitmap("Interface\\newui_scrollbar_stretch.jpg", IMAGE_DRAG_BTN, GL_LINEAR);
}

void SEASON3B::CNewUIChatLogWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_SCROLL_TOP);
	DeleteBitmap(IMAGE_SCROLL_MIDDLE);
	DeleteBitmap(IMAGE_SCROLL_BOTTOM);
	DeleteBitmap(IMAGE_SCROLLBAR_ON);
	DeleteBitmap(IMAGE_SCROLLBAR_OFF);
	DeleteBitmap(IMAGE_DRAG_BTN);
}

#if MAIN_UPDATE > 303
bool SEASON3B::CNewUIChatLogWindow::RenderBackground()
{
	float fRenderPosX = (float)m_WndPos.x;
	float fRenderPosY = (float)m_WndPos.y - m_WndSize.cy;

	float fRenderSizeX = (float)m_WndSize.cx;
	float fRenderSizeY = (float)m_WndSize.cy;

	if (gmProtect->checkold_school())
	{
		fRenderSizeY += 22.f;
	}

	EnableAlphaTest();
	glColor4f(0.0f, 0.0f, 0.0f, GetBackAlpha());
	RenderColor(fRenderPosX, fRenderPosY, fRenderSizeX, fRenderSizeY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	DisableAlphaBlend();
	return true;
}

bool SEASON3B::CNewUIChatLogWindow::RenderFrame()
{
	if (gmProtect->checkold_school())
	{
		float fRenderPosX = m_WndPos.x;
		float fRenderPosY = m_WndPos.y - m_WndSize.cy;
		float RenderSizeX = m_WndSize.cx;
		float RenderSizeY = m_WndSize.cy + 22.f;

		EnableAlphaTest();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		float iRenderSizeY = 40.f;

		int NumLine = (RenderSizeY / iRenderSizeY);

		for (int i = 0; i < NumLine; ++i)
		{
			RenderImageF(Bitmap_vframe, fRenderPosX, fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

			RenderImageF(Bitmap_vframe, (fRenderPosX + RenderSizeX - 5.0), fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

			fRenderPosY += iRenderSizeY;
		}

		if ((float)((NumLine * iRenderSizeY) / RenderSizeY) > 0.f)
		{
			iRenderSizeY = RenderSizeY - (NumLine * iRenderSizeY);
			RenderImageF(Bitmap_vframe, fRenderPosX, fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

			RenderImageF(Bitmap_vframe, (fRenderPosX + RenderSizeX - 5.0), fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

			fRenderPosY += iRenderSizeY;
		}

		//RenderImageF(Bitmap_Message_box2, fRenderPosX, fRenderPosY, RenderSizeX, 5.0, 0, 0, 213.f, 5.f);

		fRenderPosY = (m_WndPos.y - m_WndSize.cy);

		RenderImageF(Bitmap_Message_box2, fRenderPosX, fRenderPosY, RenderSizeX, 5.0, 0, 0, 213.f, 5.f);


		float RenderFrameX = (fRenderPosX + RenderSizeX) - 21.0;
		float RenderFrameY = fRenderPosY + 21.0;
		RenderSizeY = 13.0;

		RenderFrameY = fRenderPosY + 8.0;
		sub_4193B1(Bitmap_Nis_btnarrow, 1, RenderFrameX, RenderFrameY, 13.0, RenderSizeY, 0.60000002, 0);

		RenderFrameY = (double)m_WndPos.y - 4.0;
		sub_4193B1(Bitmap_Nis_btnarrow, 1, RenderFrameX, RenderFrameY, 13.0, RenderSizeY, 0.60000002, 1);

		RenderFrameY = fRenderPosY + 21.0;
		RenderSizeY = ((double)m_WndPos.y - 17.0) - RenderFrameY;
		RenderBitmap(Bitmap_Nis_back, RenderFrameX, RenderFrameY, 13, RenderSizeY, 0, 0, 0.8125, 0.8125, 1, 1, 0.0);

		RenderBitmap(Bitmap_Nis_bar, RenderFrameX + 1, RenderFrameY, 11, RenderSizeY, 0, 0, 0.6875, 0.6875, 1, 1, 0.0);

		if (m_EventState == EVENT_RESIZING_BTN_DOWN)
			glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
		else
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		RenderFrameY = fRenderPosY - 8.0 + 4;
		RenderFrameX = (fRenderPosX + RenderSizeX) - 28.0;

		RenderBitmap(Bitmap_Nis_rsframe_t, RenderFrameX, RenderFrameY, 22.0, 12.0, 0, 0, 0.6875, 0.75, 1, 1, 0.0);

		DisableAlphaBlend();
	}
	else
	{
		float fRenderPosX = m_WndPos.x;
		float fRenderPosY = m_WndPos.y - m_WndSize.cy;

		EnableAlphaTest();
		if (m_EventState == EVENT_RESIZING_BTN_DOWN)
		{
			glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
		}
		else
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		RenderImage(IMAGE_DRAG_BTN, fRenderPosX, fRenderPosY - (float)RESIZING_BTN_HEIGHT, (float)m_WndSize.cx, 10.0f);
		DisableAlphaBlend();

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		RenderImage(IMAGE_SCROLL_TOP, fRenderPosX + m_WndSize.cx - SCROLL_BAR_WIDTH - WND_LEFT_RIGHT_EDGE, fRenderPosY + WND_TOP_BOTTOM_EDGE, 7, 3);

		for (int i = 0; i < (int)GetNumberOfShowingLines(); i++)
		{
			RenderImage(IMAGE_SCROLL_MIDDLE, fRenderPosX + m_WndSize.cx - SCROLL_BAR_WIDTH - WND_LEFT_RIGHT_EDGE, fRenderPosY + WND_TOP_BOTTOM_EDGE + (float)(i * SCROLL_MIDDLE_PART_HEIGHT + SCROLL_TOP_BOTTOM_PART_HEIGHT), 7, 15);
		}

		RenderImage(IMAGE_SCROLL_BOTTOM, fRenderPosX + m_WndSize.cx - SCROLL_BAR_WIDTH - WND_LEFT_RIGHT_EDGE, m_WndPos.y - WND_TOP_BOTTOM_EDGE - SCROLL_TOP_BOTTOM_PART_HEIGHT, 7, 3);

		EnableAlphaTest();
		if (m_EventState == EVENT_SCROLL_BTN_DOWN)
		{
			glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
		}
		else
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
		RenderImage(IMAGE_SCROLLBAR_ON, m_ScrollBtnPos.x, m_ScrollBtnPos.y, SCROLL_BTN_WIDTH, SCROLL_BTN_HEIGHT);
		DisableAlphaBlend();
	}

	return true;
}

bool SEASON3B::CNewUIChatLogWindow::RenderMessages()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y;

	if ((gmProtect->checkold_school()))
	{
		fRenderPosY = m_WndPos.y - m_WndSize.cy + 5;
	}
	else
	{
		fRenderPosY = m_WndPos.y - m_WndSize.cy + SCROLL_TOP_BOTTOM_PART_HEIGHT;
	}

	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	if (pvecMsgs == NULL)
	{
		assert(!"empty chat!");
		return false;
	}

	int iRenderStartLine = 0;
	if (GetCurrentRenderEndLine() < m_nShowingLines)
	{
		if ((gmProtect->checkold_school()))
			fRenderPosY = fRenderPosY + (13 * (m_nShowingLines - GetCurrentRenderEndLine() - 1));
		else
			fRenderPosY = fRenderPosY + (SCROLL_MIDDLE_PART_HEIGHT * (m_nShowingLines - GetCurrentRenderEndLine() - 1));
	}
	else
	{
		iRenderStartLine = GetCurrentRenderEndLine() - m_nShowingLines + 1;
	}

	BYTE byAlpha = 150;
	if (m_bShowFrame)
		byAlpha = 100;

	EnableAlphaTest();

	ReCalc_BasicPos(&iRenderStartLine, &fRenderPosY);

	int iNextLine = iRenderStartLine;

	m_bPointedMessage = false;

	for (int i = 0; ; i++)
	{
		if (iNextLine > GetCurrentRenderEndLine())
			break;

		if (iNextLine < 0)
		{
			if (iNextLine >= (int)pvecMsgs->size())
				break;
		}

		bool bRenderMessage = true;
		g_pRenderText->SetFont(g_hFont);

		CMessageText* pMsgText = (*pvecMsgs)[iNextLine];

		if (m_msgEnable[pMsgText->GetType()])
		{
			if (pMsgText->GetType() == TYPE_WHISPER_MESSAGE)
			{
				g_pRenderText->SetBgColor(255, 200, 50, 150);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_SYSTEM_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, 150);
				g_pRenderText->SetTextColor(100, 150, 255, 255);
			}
			else if (pMsgText->GetType() == TYPE_ERROR_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, 150);
				g_pRenderText->SetTextColor(255, 30, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_CHAT_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, byAlpha);
				g_pRenderText->SetTextColor(205, 220, 239, 255);
			}
			else if (pMsgText->GetType() == TYPE_PARTY_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 200, 255, 150);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GUILD_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 255, 150, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GUILD_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 255, 150, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_UNION_MESSAGE)
			{
				g_pRenderText->SetBgColor(200, 200, 0, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GM_MESSAGE)
			{
				g_pRenderText->SetBgColor(30, 30, 30, 200);
				g_pRenderText->SetTextColor(250, 200, 50, 255);
				g_pRenderText->SetFont(g_hFontBold);
			}
			else if (pMsgText->GetType() == TYPE_GENS_MESSAGE)
			{
				g_pRenderText->SetTextColor(0, 0, 0, 0xFFu);
				g_pRenderText->SetBgColor(0xE6u, 0xE6u, 0xE9u, 0xC8u);
				g_pRenderText->SetFont(g_hFontBold);
			}
			else
			{
				bRenderMessage = false;
			}

			if (bRenderMessage && !pMsgText->GetText().empty())
			{
				POINT ptRenderPos;
				if ((gmProtect->checkold_school()))
				{
					if (IsShowFrame())
						ptRenderPos.x = fRenderPosX + 10;
					else
						ptRenderPos.x = fRenderPosX + 5;

					ptRenderPos.y = (long)fRenderPosY + (13 * i) + ((GetNumberOfShowingLines() / 3) - 1) + 5;
				}
				else
				{
					ptRenderPos.x = (long)fRenderPosX + (long)WND_LEFT_RIGHT_EDGE;
					ptRenderPos.y = (long)fRenderPosY + (long)FONT_LEADING + ((long)SCROLL_MIDDLE_PART_HEIGHT * (long)i);
				}

				if (!pMsgText->GetID().empty())
				{
					std::string strIDUTF8 = "";
					g_pMultiLanguage->ConvertANSIToUTF8OrViceVersa(strIDUTF8, (pMsgText->GetID()).c_str());
					type_string strLine = strIDUTF8 + " : " + pMsgText->GetText();

					SIZE RealTextSize;
					g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strLine.c_str(), strLine.size(), &RealTextSize);

					RealTextSize.cy = RealTextSize.cy / g_fScreenRate_y;
					RealTextSize.cx = RealTextSize.cx / g_fScreenRate_x + 10;

					if (RealTextSize.cx > m_WndSize.cx)
					{
						RealTextSize.cx = m_WndSize.cx;
					}

					if (SEASON3B::CheckMouseIn(ptRenderPos.x, ptRenderPos.y, RealTextSize.cx, RealTextSize.cy))
					{
						m_bPointedMessage = true;
						m_iPointedMessageIndex = iNextLine;

						std::string strID = pMsgText->GetID();
						if (MouseRButton && strID.empty() == false)
						{
							g_pChatInputBox->SetWhsprID(strID.c_str());
							MouseRButton = false;
						}
					}

					if (m_bPointedMessage == true && m_iPointedMessageIndex == iNextLine)
					{
						g_pRenderText->SetBgColor(30, 30, 30, 180);
						g_pRenderText->SetTextColor(255, 128, 255, 255);
						//g_pMainFrame->SetItemTick(pMsgText->GetItemMemory());
					}


					g_pRenderText->RenderText(ptRenderPos.x, ptRenderPos.y, strLine.c_str());
				}
				else
				{
					g_pRenderText->RenderText(ptRenderPos.x, ptRenderPos.y, pMsgText->GetText().c_str());
				}
			}
		}
		else
		{
			i--;
		}
		++iNextLine;
	}
	DisableAlphaBlend();

	return true;
}
#else
bool SEASON3B::CNewUIChatLogWindow::RenderBackground()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y - m_WndSize.cy;

	float RenderSizeX = m_WndSize.cx;
	float RenderSizeY = m_WndSize.cy + 12.f;

	EnableAlphaTest();
	glColor4f(0.0f, 0.0f, 0.0f, GetBackAlpha());
	RenderColor(fRenderPosX, fRenderPosY, RenderSizeX, RenderSizeY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIChatLogWindow::RenderFrame()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y - m_WndSize.cy;
	float RenderSizeX = m_WndSize.cx;
	float RenderSizeY = m_WndSize.cy + 12.f;

	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float iRenderSizeY = 40.f;

	int NumLine = (RenderSizeY / iRenderSizeY);

	for (int i = 0; i < NumLine; ++i)
	{
		RenderImageF(Bitmap_vframe, fRenderPosX, fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

		RenderImageF(Bitmap_vframe, (fRenderPosX + RenderSizeX - 5.0), fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

		fRenderPosY += iRenderSizeY;
	}

	if ((float)((NumLine * iRenderSizeY) / RenderSizeY) > 0.f)
	{
		iRenderSizeY = RenderSizeY - (NumLine * iRenderSizeY);
		RenderImageF(Bitmap_vframe, fRenderPosX, fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

		RenderImageF(Bitmap_vframe, (fRenderPosX + RenderSizeX - 5.0), fRenderPosY, 5.0, iRenderSizeY, 0, 0, 5.0, iRenderSizeY);

		fRenderPosY += iRenderSizeY;
	}

	RenderImageF(Bitmap_Message_box2, fRenderPosX, fRenderPosY, RenderSizeX, 5.0, 0, 0, 213.f, 5.f);

	fRenderPosY = (m_WndPos.y - m_WndSize.cy);

	RenderImageF(Bitmap_Message_box2, fRenderPosX, fRenderPosY, RenderSizeX, 5.0, 0, 0, 213.f, 5.f);


	float RenderFrameX = (fRenderPosX + RenderSizeX) - 21.0;
	float RenderFrameY = fRenderPosY + 21.0;
	RenderSizeY = 13.0;

	RenderFrameY = fRenderPosY + 8.0;
	sub_4193B1(Bitmap_Nis_btnarrow, 1, RenderFrameX, RenderFrameY, 13.0, RenderSizeY, 0.60000002, 0);

	RenderFrameY = (double)m_WndPos.y - 4.0;
	sub_4193B1(Bitmap_Nis_btnarrow, 1, RenderFrameX, RenderFrameY, 13.0, RenderSizeY, 0.60000002, 1);

	RenderFrameY = fRenderPosY + 21.0;
	RenderSizeY = ((double)m_WndPos.y - 17.0) - RenderFrameY;
	RenderBitmap(Bitmap_Nis_back, RenderFrameX, RenderFrameY, 13, RenderSizeY, 0, 0, 0.8125, 0.8125, 1, 1, 0.0);

	RenderBitmap(Bitmap_Nis_bar, RenderFrameX + 1, RenderFrameY, 11, RenderSizeY, 0, 0, 0.6875, 0.6875, 1, 1, 0.0);


	if (m_EventState == EVENT_RESIZING_BTN_DOWN)
	{
		glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
	}
	else
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	RenderFrameY = fRenderPosY - 8.0 + 4;
	RenderFrameX = (fRenderPosX + RenderSizeX) - 28.0;

	RenderBitmap(Bitmap_Nis_rsframe_t, RenderFrameX, RenderFrameY, 22.0, 12.0, 0, 0, 0.6875, 0.75, 1, 1, 0.0);
	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIChatLogWindow::RenderMessages()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y - m_WndSize.cy + 5.0;


	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	if (pvecMsgs == NULL)
	{
		assert(!"empty chat!");
		return false;
	}

	int iRenderStartLine = 0;
	if (GetCurrentRenderEndLine() < m_nShowingLines)
	{
		fRenderPosY = fRenderPosY + (13 * (m_nShowingLines - GetCurrentRenderEndLine() - 1)) + 5;
	}
	else
	{
		iRenderStartLine = GetCurrentRenderEndLine() - m_nShowingLines + 1;
	}

	BYTE byAlpha = 150;
	if (m_bShowFrame)
		byAlpha = 100;

	EnableAlphaTest();

	ReCalc_BasicPos(&iRenderStartLine, &fRenderPosY);

	int iNextLine = iRenderStartLine;

	m_bPointedMessage = false;

	for (int i = 0; ; i++)
	{
		if (iNextLine > GetCurrentRenderEndLine())
			break;
		if (iNextLine < 0)
		{
			if (iNextLine >= (int)pvecMsgs->size())
				break;
		}

		bool bRenderMessage = true;
		g_pRenderText->SetFont(g_hFont);

		CMessageText* pMsgText = (*pvecMsgs)[iNextLine];

		if (m_msgEnable[pMsgText->GetType()])
		{
			if (pMsgText->GetType() == TYPE_WHISPER_MESSAGE)
			{
				g_pRenderText->SetBgColor(255, 200, 50, 150);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_SYSTEM_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, 150);
				g_pRenderText->SetTextColor(100, 150, 255, 255);
			}
			else if (pMsgText->GetType() == TYPE_ERROR_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, 150);
				g_pRenderText->SetTextColor(255, 30, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_CHAT_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 0, 0, byAlpha);
				g_pRenderText->SetTextColor(205, 220, 239, 255);
			}
			else if (pMsgText->GetType() == TYPE_PARTY_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 200, 255, 150);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GUILD_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 255, 150, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GUILD_MESSAGE)
			{
				g_pRenderText->SetBgColor(0, 255, 150, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_UNION_MESSAGE)
			{
				g_pRenderText->SetBgColor(200, 200, 0, 200);
				g_pRenderText->SetTextColor(0, 0, 0, 255);
			}
			else if (pMsgText->GetType() == TYPE_GM_MESSAGE)
			{
				g_pRenderText->SetBgColor(30, 30, 30, 200);
				g_pRenderText->SetTextColor(250, 200, 50, 255);
				g_pRenderText->SetFont(g_hFontBold);
			}
			else if (pMsgText->GetType() == TYPE_GENS_MESSAGE)
			{
				g_pRenderText->SetTextColor(0, 0, 0, 0xFFu);
				g_pRenderText->SetBgColor(0xE6u, 0xE6u, 0xE9u, 0xC8u);
				g_pRenderText->SetFont(g_hFontBold);
			}
			else
			{
				bRenderMessage = false;
			}

			if (bRenderMessage && !pMsgText->GetText().empty())
			{
				POINT ptRenderPos;
				if (IsShowFrame())
					ptRenderPos.x = fRenderPosX + 10;
				else
					ptRenderPos.x = fRenderPosX + 5;

				ptRenderPos.y = fRenderPosY + 5 + (13 * i);

				if (!pMsgText->GetID().empty())
				{
					std::string strIDUTF8 = "";
					g_pMultiLanguage->ConvertANSIToUTF8OrViceVersa(strIDUTF8, (pMsgText->GetID()).c_str());
					type_string strLine = strIDUTF8 + " : " + pMsgText->GetText();

					SIZE RealTextSize;
					g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), strLine.c_str(), strLine.size(), &RealTextSize);

					RealTextSize.cy = RealTextSize.cy / g_fScreenRate_y;
					RealTextSize.cx = RealTextSize.cx / g_fScreenRate_x + 10;

					if (RealTextSize.cx > m_WndSize.cx)
					{
						RealTextSize.cx = m_WndSize.cx;
					}

					if (SEASON3B::CheckMouseIn(ptRenderPos.x, ptRenderPos.y, RealTextSize.cx, RealTextSize.cy))
					{
						m_bPointedMessage = true;
						m_iPointedMessageIndex = iNextLine;

						std::string strID = pMsgText->GetID();
						if (MouseRButton && strID.empty() == false)
						{
							g_pChatInputBox->SetWhsprID(strID.c_str());
							MouseRButton = false;
						}
					}

					if (m_bPointedMessage == true && m_iPointedMessageIndex == iNextLine)
					{
						g_pRenderText->SetBgColor(30, 30, 30, 180);
						g_pRenderText->SetTextColor(255, 128, 255, 255);
					}

					if (GMProtect->IsRenderWave(strIDUTF8.c_str()))
					{
						SIZE TextSize;
						g_pRenderText->RenderWave(ptRenderPos.x, ptRenderPos.y, strLine.c_str(), 0, 0, RT3_SORT_LEFT, &TextSize, strIDUTF8.length());
					}
					else
					{
						g_pRenderText->RenderText(ptRenderPos.x, ptRenderPos.y, strLine.c_str());
					}
				}
				else
				{
					g_pRenderText->RenderText(ptRenderPos.x, ptRenderPos.y, pMsgText->GetText().c_str());
				}
			}
		}
		else
		{
			i--;
		}
		++iNextLine;
	}
	DisableAlphaBlend();

	return true;
}

#endif

bool SEASON3B::CNewUIChatLogWindow::Create(CNewUIManager* pNewUIMng, int x, int y, int nShowingLines /* = 6 */)
{
	Release();

	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_CHATLOGWINDOW, this);
	this->SetPos(x, y);

	SetNumberOfShowingLines(nShowingLines);
	LoadImages();

	return true;
}

void SEASON3B::CNewUIChatLogWindow::Release()
{
	UnloadImages();
	ResetFilter();
	ClearAll(0);

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
	Init();
}

void SEASON3B::CNewUIChatLogWindow::SetPos(int x, int y)
{
	m_WndPos.x = x;
	m_WndPos.y = y;
}

#ifdef ITEM_CHAT_SELL_SYSTEM
void SEASON3B::CNewUIChatLogWindow::AddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType /*= TYPE_ALL_MESSAGE*/, void* MemoryData)
{
	if (strID.empty() && strText.empty())
	{
		return;
	}

	if (GetNumberOfLines(MsgType) >= MAX_NUMBER_OF_LINES)
	{
		RemoveFrontLine(MsgType);
	}

	if (GetNumberOfLines(TYPE_ALL_MESSAGE) >= MAX_NUMBER_OF_LINES)
	{
		RemoveFrontLine(TYPE_ALL_MESSAGE);
	}

	if (m_vecFilters.empty() == true)
	{
		ProcessAddText(strID, strText, MsgType, ErrMsgType, MemoryData);
	}
	else
	{
		if (MsgType != TYPE_CHAT_MESSAGE)
		{
			ProcessAddText(strID, strText, MsgType, ErrMsgType, MemoryData);
		}
		else if (CheckFilterText(strID) || CheckFilterText(strText))
		{
			ProcessAddText(strID, strText, MsgType, ErrMsgType, MemoryData);

			if (g_pOption->IsWhisperSound())
			{
				PlayBuffer(SOUND_WHISPER);
			}
		}
	}
}

void SEASON3B::CNewUIChatLogWindow::ProcessAddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType, void* MemoryData)
{
	type_vector_msgs* pvecMsgs = GetMsgs(MsgType);

	if (pvecMsgs == NULL)
	{
		assert(!"Empty Message");
		return;
	}

	//gfxChat::AddChatInfo(MsgType, strID.c_str(), strText.c_str());

	int nScrollLines = 0;
	if (strText.size() >= 20)
	{
		type_string strText1, strText2;
		SeparateText(strID, strText, strText1, strText2);

		if (!strText1.empty())
		{
			CMessageText* pMsgText = new CMessageText;
			if (!pMsgText->Create(strID, strText1, MsgType, MemoryData))
				delete pMsgText;
			else
			{
				pvecMsgs->push_back(pMsgText);
			}

			CMessageText* pAllMsgText = new CMessageText;
			if (!pAllMsgText->Create(strID, strText1, MsgType, MemoryData))
				delete pAllMsgText;
			else
			{
				m_vecAllMsgs.push_back(pAllMsgText);
			}

			if ((MsgType == TYPE_ERROR_MESSAGE) && (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
			{
				type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
				if (pErrvecMsgs == NULL)
				{
					assert(!"Error Chat");
					return;
				}

				CMessageText* pErrMsgText = new CMessageText;
				if (!pErrMsgText->Create(strID, strText1, MsgType, MemoryData))
					delete pErrMsgText;
				else
				{
					pErrvecMsgs->push_back(pErrMsgText);
				}
			}

			if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
			{
				nScrollLines++;
			}
		}
		if (!strText2.empty())
		{
			CMessageText* pMsgText = new CMessageText;
			if (!pMsgText->Create("", strText2, MsgType, MemoryData))
				delete pMsgText;
			else
			{
				pvecMsgs->push_back(pMsgText);
			}

			CMessageText* pAllMsgText = new CMessageText;
			if (!pAllMsgText->Create("", strText2, MsgType, MemoryData))
				delete pAllMsgText;
			else
			{
				m_vecAllMsgs.push_back(pAllMsgText);
			}

			if ((MsgType == TYPE_ERROR_MESSAGE) && (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
			{
				type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
				if (pErrvecMsgs == NULL)
				{
					assert(!"Error chat 2");
					return;
				}

				CMessageText* pErrMsgText = new CMessageText;
				if (!pErrMsgText->Create("", strText2, MsgType, MemoryData))
					delete pErrMsgText;
				else
				{
					pErrvecMsgs->push_back(pErrMsgText);
				}
			}

			if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
			{
				nScrollLines++;
			}
		}
	}
	else
	{
		CMessageText* pMsgText = new CMessageText;
		if (!pMsgText->Create(strID, strText, MsgType, MemoryData))
			delete pMsgText;
		else
		{
			pvecMsgs->push_back(pMsgText);
		}

		CMessageText* pAllMsgText = new CMessageText;
		if (!pAllMsgText->Create(strID, strText, MsgType, MemoryData))
			delete pAllMsgText;
		else
		{
			m_vecAllMsgs.push_back(pAllMsgText);
		}

		if ((MsgType == TYPE_ERROR_MESSAGE)
			&& (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
		{
			type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
			if (pErrvecMsgs == NULL)
			{
				assert(!"Error chat 3");
				return;
			}

			CMessageText* pErrMsgText = new CMessageText;
			if (!pErrMsgText->Create(strID, strText, MsgType, MemoryData))
				delete pErrMsgText;
			else
			{
				pErrvecMsgs->push_back(pErrMsgText);
			}
		}

		if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
		{
			nScrollLines++;
		}
	}

	pvecMsgs = GetMsgs(GetCurrentMsgType());
	if (pvecMsgs == NULL)
	{
		assert(!"Error chat 4");
		return;
	}

	//. Auto Scrolling
	if (nScrollLines > 0 && ((pvecMsgs->size() - (m_iCurrentRenderEndLine + 1) - nScrollLines) < 3))
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
	else if (!m_bShowFrame)
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
}

#else
void SEASON3B::CNewUIChatLogWindow::AddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType /*= TYPE_ALL_MESSAGE*/)
{
	if (strID.empty() && strText.empty())
	{
		return;
	}

	if (GetNumberOfLines(MsgType) >= MAX_NUMBER_OF_LINES)
	{
		RemoveFrontLine(MsgType);
	}

	if (GetNumberOfLines(TYPE_ALL_MESSAGE) >= MAX_NUMBER_OF_LINES)
	{
		RemoveFrontLine(TYPE_ALL_MESSAGE);
	}

	if (m_vecFilters.empty() == true)
	{
		ProcessAddText(strID, strText, MsgType, ErrMsgType);
	}
	else
	{
		if (MsgType != TYPE_CHAT_MESSAGE)
		{
			ProcessAddText(strID, strText, MsgType, ErrMsgType);
		}
		else if (CheckFilterText(strID) || CheckFilterText(strText))
		{
			ProcessAddText(strID, strText, MsgType, ErrMsgType);

			if (g_pOption->IsWhisperSound())
			{
				PlayBuffer(SOUND_WHISPER);
			}
		}
	}
}

void SEASON3B::CNewUIChatLogWindow::ProcessAddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType)
{
	type_vector_msgs* pvecMsgs = GetMsgs(MsgType);

	if (pvecMsgs == NULL)
	{
		assert(!"Empty Message");
		return;
	}

	int nScrollLines = 0;
	if (strText.size() >= 20)
	{
		type_string strText1, strText2;
		SeparateText(strID, strText, strText1, strText2);

		if (!strText1.empty())
		{
			CMessageText* pMsgText = new CMessageText;
			if (!pMsgText->Create(strID, strText1, MsgType))
				delete pMsgText;
			else
			{
				pvecMsgs->push_back(pMsgText);
			}

			CMessageText* pAllMsgText = new CMessageText;
			if (!pAllMsgText->Create(strID, strText1, MsgType))
				delete pAllMsgText;
			else
			{
				m_vecAllMsgs.push_back(pAllMsgText);
			}

			if ((MsgType == TYPE_ERROR_MESSAGE) && (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
			{
				type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
				if (pErrvecMsgs == NULL)
				{
					assert(!"Error Chat");
					return;
				}

				CMessageText* pErrMsgText = new CMessageText;
				if (!pErrMsgText->Create(strID, strText1, MsgType))
					delete pErrMsgText;
				else
				{
					pErrvecMsgs->push_back(pErrMsgText);
				}
			}

			if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
			{
				nScrollLines++;
			}
		}
		if (!strText2.empty())
		{
			CMessageText* pMsgText = new CMessageText;
			if (!pMsgText->Create("", strText2, MsgType))
				delete pMsgText;
			else
			{
				pvecMsgs->push_back(pMsgText);
			}

			CMessageText* pAllMsgText = new CMessageText;
			if (!pAllMsgText->Create("", strText2, MsgType))
				delete pAllMsgText;
			else
			{
				m_vecAllMsgs.push_back(pAllMsgText);
			}

			if ((MsgType == TYPE_ERROR_MESSAGE) && (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
			{
				type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
				if (pErrvecMsgs == NULL)
				{
					assert(!"Error chat 2");
					return;
				}

				CMessageText* pErrMsgText = new CMessageText;
				if (!pErrMsgText->Create("", strText2, MsgType))
					delete pErrMsgText;
				else
				{
					pErrvecMsgs->push_back(pErrMsgText);
				}
			}

			if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
			{
				nScrollLines++;
			}
		}
	}
	else
	{
		CMessageText* pMsgText = new CMessageText;
		if (!pMsgText->Create(strID, strText, MsgType))
			delete pMsgText;
		else
		{
			pvecMsgs->push_back(pMsgText);
		}

		CMessageText* pAllMsgText = new CMessageText;
		if (!pAllMsgText->Create(strID, strText, MsgType))
			delete pAllMsgText;
		else
		{
			m_vecAllMsgs.push_back(pAllMsgText);
		}

		if ((MsgType == TYPE_ERROR_MESSAGE)
			&& (ErrMsgType != TYPE_ERROR_MESSAGE && ErrMsgType != TYPE_ALL_MESSAGE))
		{
			type_vector_msgs* pErrvecMsgs = GetMsgs(ErrMsgType);
			if (pErrvecMsgs == NULL)
			{
				assert(!"Error chat 3");
				return;
			}

			CMessageText* pErrMsgText = new CMessageText;
			if (!pErrMsgText->Create(strID, strText, MsgType))
				delete pErrMsgText;
			else
			{
				pErrvecMsgs->push_back(pErrMsgText);
			}
		}

		if (GetCurrentMsgType() == TYPE_ALL_MESSAGE || GetCurrentMsgType() == MsgType)
		{
			nScrollLines++;
		}
	}

	pvecMsgs = GetMsgs(GetCurrentMsgType());
	if (pvecMsgs == NULL)
	{
		assert(!"Error chat 4");
		return;
	}

	//. Auto Scrolling
	if (nScrollLines > 0 && ((pvecMsgs->size() - (m_iCurrentRenderEndLine + 1) - nScrollLines) < 3))
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
	else if (!m_bShowFrame)
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
}
#endif // ITEM_CHAT_SELL_SYSTEM

void SEASON3B::CNewUIChatLogWindow::RemoveFrontLine(MESSAGE_TYPE MsgType)
{
	type_vector_msgs* pvecMsgs = GetMsgs(MsgType);

	if (pvecMsgs == NULL)
	{
		assert(!"Empty Message RemoveFrontLine");
		return;
	}

	type_vector_msgs::iterator vi = pvecMsgs->begin();
	if (vi != pvecMsgs->end())
	{
		delete (*vi);
		vi = pvecMsgs->erase(vi);
	}

	if (MsgType == GetCurrentMsgType())
	{
		Scrolling(GetCurrentRenderEndLine());
	}
}

void SEASON3B::CNewUIChatLogWindow::Clear(MESSAGE_TYPE MsgType)
{
	type_vector_msgs* pvecMsgs = GetMsgs(MsgType);
	if (pvecMsgs == NULL)
	{
		assert(!"Empty Message CNewUIChatLogWindow");
		return;
	}

	type_vector_msgs::iterator vi_msg = pvecMsgs->begin();
	for (; vi_msg != pvecMsgs->end(); vi_msg++)
		delete (*vi_msg);
	pvecMsgs->clear();

	if (MsgType == GetCurrentMsgType())
	{
		m_iCurrentRenderEndLine = -1;
	}
}

size_t SEASON3B::CNewUIChatLogWindow::GetNumberOfLines(MESSAGE_TYPE MsgType)
{
	type_vector_msgs* pvecMsgs = GetMsgs(MsgType);
	if (pvecMsgs == NULL)
	{
		return 0;
	}

	return pvecMsgs->size();
}

int SEASON3B::CNewUIChatLogWindow::GetCurrentRenderEndLine() const
{
	return m_iCurrentRenderEndLine;
}

void SEASON3B::CNewUIChatLogWindow::Scrolling(int nRenderEndLine)
{
	type_vector_msgs* pvecMsgs = GetMsgs(m_CurrentRenderMsgType);
	if (pvecMsgs == NULL)
	{
		assert(!"Empty message Scrolling");
		return;
	}

	if ((int)pvecMsgs->size() <= m_nShowingLines)
	{
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
	}
	else
	{
		if (nRenderEndLine < m_nShowingLines)
			m_iCurrentRenderEndLine = m_nShowingLines - 1;

		else if (nRenderEndLine >= (int)pvecMsgs->size())
			m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
		else
			m_iCurrentRenderEndLine = nRenderEndLine;
	}
}

void SEASON3B::CNewUIChatLogWindow::SetFilterText(const type_string& strFilterText)
{
	bool bPrevFilter = false;

	if (!m_vecFilters.empty())
	{
		bPrevFilter = true;
		ResetFilter();
	}

	unicode::t_char szTemp[MAX_CHAT_BUFFER_SIZE + 1] = { 0, };
	unicode::_strncpy(szTemp, strFilterText.c_str(), MAX_CHAT_BUFFER_SIZE);
	szTemp[MAX_CHAT_BUFFER_SIZE] = '\0';
	unicode::t_char* token = unicode::_strtok(szTemp, " ");
	token = unicode::_strtok(NULL, " ");

	if (token == NULL)
	{
		ResetFilter();
		AddText("", GlobalText[756], TYPE_SYSTEM_MESSAGE);
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			if (NULL == token)
			{
				break;
			}
			AddFilterWord(token);
			token = unicode::_strtok(NULL, " ");
		}

		AddText("", GlobalText[755], TYPE_SYSTEM_MESSAGE);
	}
}

void SEASON3B::CNewUIChatLogWindow::ResetFilter()
{
	m_vecFilters.clear();
}

void SEASON3B::CNewUIChatLogWindow::SetSizeAuto()
{
	SetNumberOfShowingLines(GetNumberOfShowingLines() + 3);
}

void SEASON3B::CNewUIChatLogWindow::SetNumberOfShowingLines(int nShowingLines, OUT LPSIZE lpBoxSize)
{
	m_nShowingLines = (int)(nShowingLines / 3) * 3;

	if (m_nShowingLines < 3)
		m_nShowingLines = 3;

	if (m_nShowingLines > 15)
		m_nShowingLines = 3;

	if (m_nShowingLines > GetCurrentRenderEndLine())
		Scrolling(m_nShowingLines - 1);

	UpdateWndSize();
	UpdateScrollPos();

#if MAIN_UPDATE > 303
	if (lpBoxSize)
	{
		if (gmProtect->checkold_school())
		{
			lpBoxSize->cx = m_WndSize.cx;
			lpBoxSize->cy = 40 * (GetNumberOfShowingLines() / 3) + 10;
		}
		else
		{
			lpBoxSize->cx = m_WndSize.cx;
			lpBoxSize->cy = (SCROLL_MIDDLE_PART_HEIGHT * GetNumberOfShowingLines()) + (SCROLL_TOP_BOTTOM_PART_HEIGHT * 2) + (WND_TOP_BOTTOM_EDGE * 2);
		}
	}
#else
	if (lpBoxSize)
	{
		lpBoxSize->cx = m_WndSize.cx;
		lpBoxSize->cy = 40 * (GetNumberOfShowingLines() / 3) + 10;
	}
#endif
}
size_t SEASON3B::CNewUIChatLogWindow::GetNumberOfShowingLines() const
{
	return m_nShowingLines;
}

void SEASON3B::CNewUIChatLogWindow::SetBackAlphaAuto()
{
	m_fBackAlpha += 0.2f;

	if (m_fBackAlpha > 0.9f)
	{
		m_fBackAlpha = 0.2f;
	}
}

void SEASON3B::CNewUIChatLogWindow::SetBackAlpha(float fAlpha)
{
	if (fAlpha < 0.f)
		m_fBackAlpha = 0.f;
	else if (fAlpha > 1.f)
		m_fBackAlpha = 0.8;
	else
		m_fBackAlpha = fAlpha;
}

float SEASON3B::CNewUIChatLogWindow::GetBackAlpha() const
{
	return m_fBackAlpha;
}

void SEASON3B::CNewUIChatLogWindow::ShowFrame()
{
	m_bShowFrame = true;

	if (gmProtect->checkold_school())
	{
		m_WndPos.x = pos_center() + 186.0;
		//m_msgEnable[TYPE_SYSTEM_MESSAGE] = false;
	}
}

void SEASON3B::CNewUIChatLogWindow::HideFrame()
{
	m_bShowFrame = false;
	if (gmProtect->checkold_school())
	{
		m_WndPos.x = 0;
		//m_WndPos.y = 95;
		//m_msgEnable[TYPE_SYSTEM_MESSAGE] = true;
	}
}

bool SEASON3B::CNewUIChatLogWindow::IsShowFrame()
{
	return m_bShowFrame;
}

bool SEASON3B::CNewUIChatLogWindow::UpdateMouseEvent()
{
#if MAIN_UPDATE < 603
	if (this->UpdateButton())
	{
		return false;
	}
#endif

	if (m_EventState == EVENT_NONE && !MouseLButtonPush && SEASON3B::CheckMouseIn(m_WndPos.x, m_WndPos.y - m_WndSize.cy, m_WndSize.cx, m_WndSize.cy))
	{
		m_EventState = EVENT_CLIENT_WND_HOVER;
		return false;
	}
	if (!MouseLButtonPush && m_EventState == EVENT_CLIENT_WND_HOVER && !SEASON3B::CheckMouseIn(m_WndPos.x, m_WndPos.y - m_WndSize.cy, m_WndSize.cx, m_WndSize.cy))
	{
		m_EventState = EVENT_NONE;
		return true;
	}

	if (m_EventState == EVENT_CLIENT_WND_HOVER)
	{
		if (MouseWheel > 0)
		{
			Scrolling(GetCurrentRenderEndLine() - 1);
		}
		if (MouseWheel < 0)
		{
			Scrolling(GetCurrentRenderEndLine() + 1);
		}
		if (MouseWheel != 0)
		{
			MouseWheel = 0;
		}
	}

	if (m_bShowFrame)
	{
		if (m_EventState == EVENT_CLIENT_WND_HOVER && MouseLButtonPush && SEASON3B::CheckMouseIn(m_ScrollBtnPos.x, m_ScrollBtnPos.y, SCROLL_BTN_WIDTH, SCROLL_BTN_HEIGHT))
		{

			m_EventState = EVENT_SCROLL_BTN_DOWN;
			m_iGrapRelativePosY = MouseY - m_ScrollBtnPos.y;
			return false;
		}
		if (m_EventState == EVENT_SCROLL_BTN_DOWN)
		{
			if (SEASON3B::IsRepeat(VK_LBUTTON))
			{
				if (GetNumberOfLines(GetCurrentMsgType()) > GetNumberOfShowingLines())
				{
					if (MouseY - m_iGrapRelativePosY < m_WndPos.y - m_WndSize.cy + WND_TOP_BOTTOM_EDGE)
					{
						Scrolling(GetNumberOfShowingLines() - 1);
						m_ScrollBtnPos.y = m_WndPos.y - m_WndSize.cy + WND_TOP_BOTTOM_EDGE;
					}
					else if (MouseY - m_iGrapRelativePosY > m_WndPos.y - SCROLL_BTN_HEIGHT - WND_TOP_BOTTOM_EDGE)
					{
						Scrolling(GetNumberOfLines(GetCurrentMsgType()) - 1);
						m_ScrollBtnPos.y = m_WndPos.y - SCROLL_BTN_HEIGHT - WND_TOP_BOTTOM_EDGE;
					}
					else
					{
						float fScrollRate = (float)((MouseY - m_iGrapRelativePosY) - (m_WndPos.y - m_WndSize.cy + WND_TOP_BOTTOM_EDGE)) / (float)(m_WndSize.cy - WND_TOP_BOTTOM_EDGE * 2 - SCROLL_BTN_HEIGHT);
						Scrolling(GetNumberOfShowingLines() + (float)(GetNumberOfLines(GetCurrentMsgType()) - GetNumberOfShowingLines()) * fScrollRate);

						m_ScrollBtnPos.y = MouseY - m_iGrapRelativePosY;
					}
				}
				return false;
			}
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				m_EventState = EVENT_NONE;
				return true;
			}
		}

		POINT ptResizingBtn = { m_WndPos.x, m_WndPos.y - m_WndSize.cy - RESIZING_BTN_HEIGHT };

		if (m_EventState == EVENT_NONE && false == MouseLButtonPush && SEASON3B::CheckMouseIn(ptResizingBtn.x, ptResizingBtn.y, RESIZING_BTN_WIDTH, RESIZING_BTN_HEIGHT))
		{
			m_EventState = EVENT_RESIZING_BTN_HOVER;
			return false;
		}
		if (false == MouseLButtonPush && m_EventState == EVENT_RESIZING_BTN_HOVER && false == SEASON3B::CheckMouseIn(ptResizingBtn.x, ptResizingBtn.y, RESIZING_BTN_WIDTH, RESIZING_BTN_HEIGHT))
		{
			m_EventState = EVENT_NONE;
			return true;
		}
		if (m_EventState == EVENT_RESIZING_BTN_HOVER && MouseLButtonPush && SEASON3B::CheckMouseIn(ptResizingBtn.x, ptResizingBtn.y, RESIZING_BTN_WIDTH, RESIZING_BTN_HEIGHT))
		{
			m_EventState = EVENT_RESIZING_BTN_DOWN;
			return false;
		}
		if (m_EventState == EVENT_RESIZING_BTN_DOWN)
		{
			if (MouseLButtonPush)
			{
				int nTopSections = (15 - GetNumberOfShowingLines()) / 3;
				int nBottomSections = (GetNumberOfShowingLines() - 3) / 3;
				for (int i = 0; i < nTopSections; i++)
				{
					if (SEASON3B::CheckMouseIn(0, ptResizingBtn.y - RESIZING_BTN_HEIGHT - ((i + 1) * SCROLL_MIDDLE_PART_HEIGHT * 3 * 2), GetWindowsX, SCROLL_MIDDLE_PART_HEIGHT * 3 + RESIZING_BTN_HEIGHT))
					{
						SetNumberOfShowingLines(GetNumberOfShowingLines() + (i + 1) * 3);
						return false;
					}
				}
				for (int i = 0; i < nBottomSections; i++)
				{
					if (SEASON3B::CheckMouseIn(0, ptResizingBtn.y + RESIZING_BTN_HEIGHT + ((i + 1) * SCROLL_MIDDLE_PART_HEIGHT * 3), GetWindowsX, RESIZING_BTN_HEIGHT + SCROLL_MIDDLE_PART_HEIGHT * 3))
					{
						SetNumberOfShowingLines(GetNumberOfShowingLines() - (i + 1) * 3);
						return false;
					}
				}
				if (SEASON3B::CheckMouseIn(0, 0, GetWindowsX, m_WndPos.y - (SCROLL_MIDDLE_PART_HEIGHT * 15 + RESIZING_BTN_HEIGHT + SCROLL_TOP_BOTTOM_PART_HEIGHT * 2)))
				{
					SetNumberOfShowingLines(15);
				}
				if (SEASON3B::CheckMouseIn(0, m_WndPos.y - (SCROLL_MIDDLE_PART_HEIGHT * 3 + SCROLL_TOP_BOTTOM_PART_HEIGHT * 2), GetWindowsX, SCROLL_MIDDLE_PART_HEIGHT * 3 + SCROLL_TOP_BOTTOM_PART_HEIGHT * 2))
				{
					SetNumberOfShowingLines(3);
				}
				return false;
			}
			if (false == MouseLButtonPush || true == MouseLButtonPop)
			{
				m_EventState = EVENT_NONE;
				return true;
			}
		}
	}
	return true;
}

bool SEASON3B::CNewUIChatLogWindow::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIChatLogWindow::Update()
{
	UpdateScrollPos();

	return true;
}

#if MAIN_UPDATE > 303
bool SEASON3B::CNewUIChatLogWindow::Render()
{
	if (m_bShowFrame && RenderBackground() == false)
	{
		return false;
	}

	if (m_bShowChatLog)
	{
		if (m_msgEnable[TYPE_SYSTEM_MESSAGE] == false)
		{
			SupportPosition();
			SetAllMsgDisable();

			if (gmProtect->checkold_school())
			{
				SetRelativeData(m_bShowFrame ? -5 : 0, 95, 6);
			}
			else
			{
				if (gmProtect->MenuSwitch != 0 && gmProtect->MenuTypeButton != 2)
					SetRelativeData(0, 168, 4);
				else
					SetRelativeData(0, 145, 4);
			}

			SetMsgVisible(TYPE_SYSTEM_MESSAGE, true);
			RenderMessages();
			SetMsgVisible(TYPE_SYSTEM_MESSAGE, false);
			RecoverBackPosition();
		}

		if (RenderMessages() == false)
		{
			return false;
		}
	}


	if (m_bShowFrame && RenderFrame() == false)
	{
		return false;
	}

	return true;
}
#else
bool SEASON3B::CNewUIChatLogWindow::Render()
{
	if (m_bShowChatLog)
	{
		if (m_bShowFrame && RenderBackground() == false)
		{
			return false;
		}

		if (m_bShowChatLog)
		{
			if (RenderMessages() == false)
			{
				return false;
			}
		}

		if (m_bShowFrame && RenderFrame() == false)
		{
			return false;
		}
	}
	RenderButton();

	return true;
}
#endif

float SEASON3B::CNewUIChatLogWindow::GetLayerDepth()
{
	return 5.1f;
}

float SEASON3B::CNewUIChatLogWindow::GetKeyEventOrder()
{
	return 8.0f;
}

void SEASON3B::CNewUIChatLogWindow::EventOrderWindows(double WindowsX, double WindowsY)
{
	if (gmProtect->checkold_school())
	{
		this->SetPos(pos_left(), (int)pos_botton() + 410);
	}
	else
	{
		this->SetPos(pos_left(), (int)pos_botton() + 383);
	}
}

bool SEASON3B::CNewUIChatLogWindow::CheckChatRedundancy(const type_string& strText, int iSearchLine/* = 1*/)
{
	type_vector_msgs* pvecMsgs = GetMsgs(TYPE_ALL_MESSAGE);
	if (pvecMsgs == NULL)
	{
		return 0;
	}

	if (pvecMsgs->empty()) return false;
	type_vector_msgs::reverse_iterator vri_msgs = pvecMsgs->rbegin();
	for (int i = 0; (i < iSearchLine) || (vri_msgs != pvecMsgs->rend()); vri_msgs++, i++)
		if (0 == (*vri_msgs)->GetText().compare(strText))
			return true;
	return false;
}

void SEASON3B::CNewUIChatLogWindow::SeparateText(IN const type_string& strID, IN const type_string& strText, OUT type_string& strText1, OUT type_string& strText2)
{
	extern float g_fScreenRate_x;

	SIZE TextSize;
	type_string strIDPart = strID + " : ";
	std::wstring wstrUTF16 = L"";

	g_pMultiLanguage->ConvertCharToWideStr(wstrUTF16, strIDPart.c_str());
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), wstrUTF16.c_str(), wstrUTF16.length(), &TextSize);
	size_t MaxFirstLineWidth = CLIENT_WIDTH - (size_t)(TextSize.cx / g_fScreenRate_x);

	g_pMultiLanguage->ConvertCharToWideStr(wstrUTF16, strText.c_str());
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), wstrUTF16.c_str(), wstrUTF16.length(), &TextSize);

	BOOL bSpaceExist = (wstrUTF16.find_last_of(L" ") != std::wstring::npos) ? TRUE : FALSE;
	int iLocToken = wstrUTF16.length();

	while (((size_t)(TextSize.cx / g_fScreenRate_x) > MaxFirstLineWidth) && (iLocToken > -1))
	{
		iLocToken = (bSpaceExist) ? wstrUTF16.find_last_of(L" ", iLocToken - 1) : iLocToken - 1;

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), (wstrUTF16.substr(0, iLocToken)).c_str(), iLocToken, &TextSize);
	}

#ifdef CHINESE_LANGUAGE
	if (iLocToken > 0)
	{
		g_pMultiLanguage->ConvertWideCharToStr(strText1, (wstrUTF16.substr(0, iLocToken)).c_str(), CHINESE_CP);
		g_pMultiLanguage->ConvertWideCharToStr(strText2, (wstrUTF16.substr(iLocToken, wstrUTF16.length() - iLocToken)).c_str(), CHINESE_CP);
	}
	else
	{
		g_pMultiLanguage->ConvertWideCharToStr(strText1, wstrUTF16.c_str(), CHINESE_CP);
	}
#else
	g_pMultiLanguage->ConvertWideCharToStr(strText1, (wstrUTF16.substr(0, iLocToken)).c_str(), CP_UTF8);
	g_pMultiLanguage->ConvertWideCharToStr(strText2, (wstrUTF16.substr(iLocToken, wstrUTF16.length() - iLocToken)).c_str(), CP_UTF8);
#endif
}

bool SEASON3B::CNewUIChatLogWindow::CheckFilterText(const type_string& strTestText)
{
	type_vector_filters::iterator vi_filters = m_vecFilters.begin();
	for (; vi_filters != m_vecFilters.end(); vi_filters++)
	{
		if (FindText(strTestText.c_str(), (*vi_filters).c_str()) == true)
		{
			return true;
		}
	}

	return false;
}

void SEASON3B::CNewUIChatLogWindow::UpdateWndSize()
{
#if MAIN_UPDATE > 303
	m_WndSize.cx = m_WndSize.cx;
	if (gmProtect->checkold_school())
	{
		m_WndSize.cy = 40 * (GetNumberOfShowingLines() / 3) + 10;
	}
	else
	{
		m_WndSize.cy = (SCROLL_MIDDLE_PART_HEIGHT * GetNumberOfShowingLines()) + ((SCROLL_TOP_BOTTOM_PART_HEIGHT + WND_TOP_BOTTOM_EDGE) * 2);
	}
#else
	m_WndSize.cx = m_WndSize.cx;
	m_WndSize.cy = 40 * (GetNumberOfShowingLines() / 3) + 10;
#endif // MAIN_UPDATE > 303
}


void SEASON3B::CNewUIChatLogWindow::UpdateScrollPos()
{
	float fPosRate = 1.f;

	if (GetNumberOfLines(GetCurrentMsgType()) > GetNumberOfShowingLines())
	{
		if ((int)GetNumberOfShowingLines() <= GetCurrentRenderEndLine())
		{
			fPosRate = (float)(GetCurrentRenderEndLine() + 1 - GetNumberOfShowingLines()) / (float)(GetNumberOfLines(GetCurrentMsgType()) - GetNumberOfShowingLines());
		}
		else
		{
			fPosRate = 0.f;
		}
	}
	if (m_EventState != EVENT_SCROLL_BTN_DOWN)
	{
		m_ScrollBtnPos.x = m_WndPos.x + m_WndSize.cx - SCROLL_BAR_WIDTH - WND_LEFT_RIGHT_EDGE - 4;
		m_ScrollBtnPos.y = m_WndPos.y - m_WndSize.cy + WND_TOP_BOTTOM_EDGE + ((float)(m_WndSize.cy - SCROLL_BTN_HEIGHT - WND_TOP_BOTTOM_EDGE * 2) * fPosRate);
	}
}

#ifdef ITEM_CHAT_SELL_SYSTEM
ITEM* SEASON3B::CNewUIChatLogWindow::GetMsgsIndent()
{
	if (m_bPointedMessage)
	{
		type_vector_msgs* pvecMsgs = this->GetMsgs(this->GetCurrentMsgType());

		if (pvecMsgs != NULL)
		{
			if (m_iPointedMessageIndex >= 0 && m_iPointedMessageIndex < (int)pvecMsgs->size())
			{
				CMessageText* pMsgText = (*pvecMsgs)[m_iPointedMessageIndex];

				return pMsgText->GetItemIndent();
			}
		}
	}

	return NULL;
}
#endif // ITEM_CHAT_SELL_SYSTEM

void SEASON3B::CNewUIChatLogWindow::AddFilterWord(const type_string& strWord)
{
	if (m_vecFilters.size() > 5)
		return;

	type_vector_filters::iterator vi_filters = m_vecFilters.begin();
	for (; vi_filters != m_vecFilters.end(); vi_filters++)
	{
		if (0 == (*vi_filters).compare(strWord))
		{
			return;
		}
	}

	m_vecFilters.push_back(strWord);
}

void SEASON3B::CNewUIChatLogWindow::ClearAll(int type)
{
	for (int i = TYPE_ALL_MESSAGE; i < NUMBER_OF_TYPES; i++)
	{
		Clear((MESSAGE_TYPE)i);
	}

	m_iCurrentRenderEndLine = -1;

	if (type == -1)
	{
		//gfxChat::ClearAllChatHistory();
	}
}

SEASON3B::CNewUIChatLogWindow::type_vector_msgs* SEASON3B::CNewUIChatLogWindow::GetMsgs(MESSAGE_TYPE MsgType)
{
	switch (MsgType)
	{
	case TYPE_ALL_MESSAGE:
		return &m_vecAllMsgs;
	case TYPE_CHAT_MESSAGE:
		return &m_VecChatMsgs;
	case TYPE_WHISPER_MESSAGE:
		return &m_vecWhisperMsgs;
	case TYPE_SYSTEM_MESSAGE:
		return &m_VecSystemMsgs;
	case TYPE_ERROR_MESSAGE:
		return &m_vecErrorMsgs;
	case TYPE_PARTY_MESSAGE:
		return &m_vecPartyMsgs;
	case TYPE_GUILD_MESSAGE:
		return &m_vecGuildMsgs;
	case TYPE_UNION_MESSAGE:
		return &m_vecUnionMsgs;
	case TYPE_GM_MESSAGE:
		return &m_vecGMMsgs;
	case TYPE_GENS_MESSAGE:
		return &m_vecGensMsgs;
	}

	return NULL;
}

void SEASON3B::CNewUIChatLogWindow::ChangeMessage(MESSAGE_TYPE MsgType)
{
	m_CurrentRenderMsgType = MsgType;

	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	if (pvecMsgs != NULL)
	{
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
	}
}

SEASON3B::MESSAGE_TYPE SEASON3B::CNewUIChatLogWindow::GetCurrentMsgType() const
{
	return m_CurrentRenderMsgType;
}

void SEASON3B::CNewUIChatLogWindow::SetMsgVisible(int MsgType, bool bShow)
{
	m_msgEnable[MsgType] = bShow;

	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	if (pvecMsgs != NULL)
	{
		m_iCurrentRenderEndLine = pvecMsgs->size() - 1;
	}
}

void SEASON3B::CNewUIChatLogWindow::SetAllMsgEnable()
{
	for (int i = TYPE_ALL_MESSAGE; i < NUMBER_OF_TYPES; i++)
	{
		m_msgEnable[i] = true;
	}
}

void SEASON3B::CNewUIChatLogWindow::SetAllMsgDisable()
{
	for (int i = TYPE_ALL_MESSAGE; i < NUMBER_OF_TYPES; i++)
	{
		m_msgEnable[i] = false;
	}
}

void SEASON3B::CNewUIChatLogWindow::ReCalc_BasicPos(int* RenderStart, float* RenderPosY)
{
	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	for (int i = (*RenderStart); ; ++i)
	{
		if (i > GetCurrentRenderEndLine())
			break;

		if (!m_msgEnable[(*pvecMsgs)[i]->GetType()])
		{
			if ((--(*RenderStart) & 0x80000000) != 0)
			{
				(*RenderStart) = 0;
#if MAIN_UPDATE > 303
				if (gmProtect->checkold_school())
					(*RenderPosY) = (*RenderPosY) + 13.0;
				else
					(*RenderPosY) = (*RenderPosY) + 15.0;
#else
				(*RenderPosY) = (*RenderPosY) + 13.0;
#endif
			}
			else
			{
				ReCalc_RenderMessages(RenderStart, RenderPosY);
			}
		}
	}
}

void SEASON3B::CNewUIChatLogWindow::ReCalc_RenderMessages(int* RenderStart, float* RenderPosY)
{
	type_vector_msgs* pvecMsgs = GetMsgs(GetCurrentMsgType());

	if (((*RenderStart) & 0x80000000) != 0)
	{
		(*RenderStart) = 0;
#if MAIN_UPDATE > 303
		if (gmProtect->checkold_school())
			(*RenderPosY) = (*RenderPosY) + 13.0;
		else
			(*RenderPosY) = (*RenderPosY) + 15.0;
#else
		(*RenderPosY) = (*RenderPosY) + 13.0;
#endif
	}
	else
	{
		if (!m_msgEnable[(*pvecMsgs)[(*RenderStart)]->GetType()])
		{
			--(*RenderStart);
			ReCalc_RenderMessages(RenderStart, RenderPosY);
		}
	}
}

void SEASON3B::CNewUIChatLogWindow::SupportPosition()
{
	m_Pos = m_WndPos;
	m_Size = m_WndSize;
	m_MaxLines = m_nShowingLines;
	memcpy(m_msgBackEnable, m_msgEnable, sizeof(m_msgBackEnable));
}

void SEASON3B::CNewUIChatLogWindow::RecoverBackPosition()
{
	m_WndPos = m_Pos;
	m_WndSize = m_Size;
	m_nShowingLines = m_MaxLines;
	memcpy(m_msgEnable, m_msgBackEnable, sizeof(m_msgEnable));
}

void SEASON3B::CNewUIChatLogWindow::SetRelativeData(int x, int y, int numberline)
{
	m_WndPos.x = x;
	m_WndPos.y = y;
	m_nShowingLines = numberline;
	UpdateWndSize();
}

void SEASON3B::RemoveItem(ITEM* pItem)
{
	if (g_pNewItemMng)
	{
		g_pNewItemMng->DeleteItem(pItem);
	}
}


void SEASON3B::CNewUIChatLogWindow::RenderButton()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y - 20.f;
	float RenderSizeX = m_WndSize.cx;

	if (m_bShowChatLog)
		fRenderPosX += RenderSizeX + 10.f;
	else
		fRenderPosX += 10.f;

	if (SEASON3B::CheckMouseIn(0, (m_WndPos.y - m_WndSize.cy), fRenderPosX + 13.0, m_WndSize.cy + 30) || !m_bShowChatLog)
	{
		RenderBitmapLocalRotate2(Bitmap_Nis_btnarrow, fRenderPosX, fRenderPosY, 13.0, 13.0, (m_bShowChatLog) ? 90.f : -90.f, 0.0, 0.0, 13.f / 16.f, 13.f / 16.f);
	}
}

bool SEASON3B::CNewUIChatLogWindow::UpdateButton()
{
	float fRenderPosX = m_WndPos.x;
	float fRenderPosY = m_WndPos.y - 20.f;
	float RenderSizeX = m_WndSize.cx;

	if (m_bShowChatLog)
		fRenderPosX += RenderSizeX + 10.f;
	else
		fRenderPosX += 10.f;

	if (SEASON3B::CheckMouseIn(fRenderPosX - 6.f, fRenderPosY - 6.f, 13.0, 13.0))
	{
		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			m_bShowChatLog = !m_bShowChatLog;
		}

		return true;
	}


	return false;
}

void SEASON3B::CNewUIChatLogWindow::sub_4193B1(int a1, int a2, float a3, float a4, float a5, float a6, GLfloat alpha, int a8)
{
	int v8; // eax
	float v10; // [esp+0h] [ebp-3Ch]
	float v11; // [esp+0h] [ebp-3Ch]
	float v12; // [esp+4h] [ebp-38h]
	float v13; // [esp+4h] [ebp-38h]
	float v14; // [esp+Ch] [ebp-30h]
	float v17; // [esp+18h] [ebp-24h]
	float v18; // [esp+18h] [ebp-24h]
	float v20; // [esp+1Ch] [ebp-20h]
	float v21; // [esp+1Ch] [ebp-20h]

	if (a2 == 1)
	{
		if (m_EventState == EVENT_SCROLL_BTN_DOWN)
		{
			glColor4f(0.80000001, 0.80000001, 0.80000001, alpha);
			v21 = (a6 - 1.0) / 16.0;
			v18 = (a5 - 1.0) / 16.0;
			if (a8)
			{
				v13 = a4 + 1.0 - a6;
				v11 = a3 + 1.0;
				RenderBitmap(a1, v11, v13, a5, a6, 0, 13.f / 16.f, v18, -v21, 1, 1, 0.0);
			}
			else
			{
				v12 = a4 + 1.0;
				v10 = a3 + 1.0;
				RenderBitmap(a1, v10, v12, a5, a6, 0, 0, v18, v21, 1, 1, 0.0);
			}
			glColor4f(1.0, 1.0, 1.0, alpha);
		}
		else
		{
			v21 = a6 / 16.0;
			v18 = a5 / 16.0;
			if (a8)
			{
				v13 = a4 - a6;
				v11 = a3;
				RenderBitmap(a1, v11, v13, a5, a6, 0, 13.f / 16.f, v18, -v21, 1, 1, 0.0);
			}
			else
			{
				v12 = a4;
				v10 = a3;
				RenderBitmap(a1, v10, v12, a5, a6, 0, 0, v18, v21, 1, 1, 0.0);
			}
		}
	}
	else
	{
		v20 = a6 / 16.0;
		v17 = a5 / 16.0;
		v8 = -(a8 != 0);
		v8 = v8 & 0xFE;
		v14 = (double)(v8 + 1) * a6;
		RenderBitmap(a1, a3, a4, a5, v14, 0, 0, v17, v20, 1, 1, 0.0);
	}
}

