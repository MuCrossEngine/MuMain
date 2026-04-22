// NewUICommon.cpp: implementation of the CNewUICommon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUICommon.h"
#include "NewUIRenderNumber.h"
#include "NewUISystem.h"
#include "NewUICommonMessageBox.h"
#include "ZzzTexture.h"
#include "ZzzOpenglUtil.h"
#include "UIManager.h"
#include "NewUIButton.h"

extern float MouseX, MouseY;

bool SEASON3B::CreateOkMessageBox(const unicode::t_string& strMsg, DWORD dwColor, float fPriority)
{
	CNewUICommonMessageBox* pMsgBox = g_MessageBox->NewMessageBox(MSGBOX_CLASS(CNewUICommonMessageBox));

	if(pMsgBox)
	{
		return pMsgBox->Create(MSGBOX_COMMON_TYPE_OK, strMsg, dwColor);
	}

	return false;
}

bool SEASON3B::CreateOkCancelMessageBox(const unicode::t_string& strMsg, DWORD dwColor, float fPriority, EVENT_CALLBACK pFunc)
{
	bool success = false;
	CNewUICommonMessageBox* pMsgBox = g_MessageBox->NewMessageBox(MSGBOX_CLASS(CNewUICommonMessageBox));

	if (pMsgBox)
	{
		success = pMsgBox->Create(MSGBOX_COMMON_TYPE_OKCANCEL, strMsg, dwColor);

		if (pFunc)
		{
			//pMsgBox->AddCallbackFunc(pFunc, MSGBOX_EVENT_MOUSE_LBUTTON_UP);
			pMsgBox->AddCallbackFunc(pFunc, MSGBOX_EVENT_USER_COMMON_OK);
		}
	}

	return success;
}

int SEASON3B::IsPurchaseShop() 
{ 
	if( g_pMyShopInventory->IsVisible() ) 
	{
		return 1;
	}
	else if( g_pPurchaseShopInventory->IsVisible() ) 
	{
		return 2;
	}

	return -1;
}

//===BCUstom
int SEASON3B::TextDraw(HFONT font, int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	va_list args;
	va_start(args, Text);
	TCHAR Buff[256];
	_vsntprintf(Buff, sizeof(Buff) / sizeof(TCHAR), Text, args);
	va_end(args);

	int LineCount = 0;
	EnableAlphaTest();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	char* Line = strtok(Buff, "\n##");
	DWORD BKTextColor = g_pRenderText->GetTextColor();
	DWORD BKBGColor = g_pRenderText->GetBgColor();
	g_pRenderText->SetFont(font);
	g_pRenderText->SetTextColor((color & 0xff000000) >> 24, (color & 0x00ff0000) >> 16, (color & 0x0000ff00) >> 8, color & 0x000000ff);
	g_pRenderText->SetBgColor((bkcolor & 0xff000000) >> 24, (bkcolor & 0x00ff0000) >> 16, (bkcolor & 0x0000ff00) >> 8, bkcolor & 0x000000ff);
	while (Line != NULL)
	{
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align, 0);
		PosY += 10;
		Line = strtok(NULL, "\n##");

		LineCount++;
	}
	EnableAlphaTest(false);
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(BKTextColor);
	g_pRenderText->SetBgColor(BKBGColor);
	return LineCount;
}

bool SEASON3B::CheckMouseIn(float x, float y, float width, float height, bool bFixed)
{
	if (bFixed)
	{
		if (MouseX >= x && MouseX < x + width && MouseY >= y && MouseY < y + height)
			return true;
	}
	else
	{
		if (MouseRenderX >= x && MouseRenderX < x + width && MouseRenderY >= y && MouseRenderY < y + height)
			return true;
	}
	return false;
}

void SEASON3B::RenderImageF(GLuint uiImageType, float x, float y, float width, float height)
{
	BITMAP_t* pText = &Bitmaps[uiImageType];

	RenderBitmap(uiImageType, x, y, width, height, 0.0, 0.0, pText->output_width / pText->Width, pText->output_height / pText->Height, true, true, 0.0);
}

void SEASON3B::RenderImageF(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, float uw, float vh)
{
	BITMAP_t* pText = &Bitmaps[uiImageType];
	RenderBitmap(uiImageType, x, y, width, height, su / pText->Width, sv / pText->Height, uw / pText->Width, vh / pText->Height, true, true, 0.0);
}

void SEASON3B::RenderImageF(GLuint uiImageType, float x, float y, float width, float height, DWORD color, float su, float sv, float uw, float vh)
{
	BITMAP_t* pText = &Bitmaps[uiImageType];
	RenderColorBitmap(uiImageType, x, y, width, height, su / pText->Width, sv / pText->Height, uw / pText->Width, vh / pText->Height, color);
}

void SEASON3B::RenderImage2F(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, float uw, float vh)
{
	BITMAP_t* pText = &Bitmaps[uiImageType];
	RenderBitmap(uiImageType, x, y, width, height, su / pText->Width, sv / pText->Height, uw / pText->Width, vh / pText->Height, false, false, 0.0);

}

void SEASON3B::RenderImage(GLuint uiImageType, float x, float y)
{
	BITMAP_t* pImage = &Bitmaps[uiImageType];

	float su = 0.0f;
	float sv = 0.0f;

	float uw = pImage->output_width / pImage->Width;
	float vh = pImage->output_height / pImage->Height;

	RenderBitmap(uiImageType, x, y, (double)pImage->output_width, (double)pImage->output_height, su, sv, uw, vh);
}

void SEASON3B::RenderImages(GLuint uiImageType, float x, float y, float width, float height, float x1, float y1, float x2, float y2)
{
	BITMAP_t* pText = &Bitmaps[uiImageType];

	float uw = x2 - x1;
	float vh = y2 - y1;
	RenderBitmap(uiImageType, x, y, width, height, x1 / pText->Width, y1 / pText->Height, uw / pText->Width, vh / pText->Height, true, true, 0.0);
}

void SEASON3B::RenderFrameAnimation(GLuint Image, float x, float y, float width, float height, float u, float v, float uw, float vh, float Time, int Numcol, int nFrame, float uFrame, float vFrame)
{
	float factor = 0.0;
	float FrameX, FrameY;

	factor = nFrame / (Time * CLOCKS_PER_SEC);

	int frame = (int)(timeGetTime() * factor) % nFrame;

	if (uFrame == 0.0)
		FrameX = (double)(frame % Numcol) * uw;
	else
		FrameX = (double)(frame % Numcol) * uFrame;

	if (vFrame == 0.0)
		FrameY = (double)(frame / Numcol) * vh;
	else
		FrameY = (double)(frame / Numcol) * vFrame;

	RenderImageF(Image, x, y, width, height, u + FrameX, v + FrameY, uw, vh);
}

void SEASON3B::RenderFrameAnimation2(GLuint Image, float x, float y, float width, float height, float Rotate, float uw, float vh, float Time, int Numcol, int nFrame, bool fScale)
{
	float factor = 0.0;
	float FrameX, FrameY;

	DWORD currentTime = timeGetTime();
	factor = nFrame / (Time * CLOCKS_PER_SEC);

	int frame = (int)(currentTime * factor) % nFrame;

	FrameX = (double)(frame % Numcol) * uw;

	FrameY = (double)(frame / Numcol) * vh;

	RenderBitmapLocalRotate2(Image, x, y, width, height, Rotate, FrameX, FrameY, uw, vh, fScale);
}

void SEASON3B::RenderImage(GLuint uiImageType, float x, float y, float width, float height)
{
	BITMAP_t *pImage = &Bitmaps[uiImageType];

	float u, v, uw, vh;

	u = 0.5f / (float)pImage->Width;
	v = 0.5f / (float)pImage->Height;
	uw = (width - 0.5f) / (float)pImage->Width;
	vh = (height - 0.5f) / (float)pImage->Height;

	RenderBitmap(uiImageType, x, y, width, height, u, v, uw - u, vh - v);
}

void SEASON3B::RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv,float uw, float vh, DWORD color)
{
	RenderColorBitmap(uiImageType, x, y, width, height, su, sv, uw, vh, color);
}

void SEASON3B::RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv)
{
	BITMAP_t *pImage = &Bitmaps[uiImageType];

	float u, v, uw, vh;
	u = ((su + 0.5f) / (float)pImage->Width);
	v = ((sv + 0.5f) / (float)pImage->Height);
	uw = (width - 0.5f) / (float)pImage->Width - (0.5f / (float)pImage->Width);
	vh = (height - 0.5f) / (float)pImage->Height - (0.5f / (float)pImage->Height);

	RenderBitmap(uiImageType, x, y, width, height, u, v, uw, vh);
}

void SEASON3B::RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, DWORD color)
{
	BITMAP_t *pImage = &Bitmaps[uiImageType];

	float u, v, uw, vh;
	u = ((su + 0.5f) / (float)pImage->Width);
	v = ((sv + 0.5f) / (float)pImage->Height);
	uw = (width - 0.5f) / (float)pImage->Width - (0.5f / (float)pImage->Width);
	vh = (height - 0.5f) / (float)pImage->Height - (0.5f / (float)pImage->Height);

	RenderColorBitmap(uiImageType, x, y, width, height, u, v, uw, vh, color);	
}

float SEASON3B::RenderNumber(float x, float y, int iNum, float fScale)
{
	return g_RenderNumber->RenderNumber(x, y, iNum, fScale);
}

void SEASON3B::begin3D()
{
	EndBitmap();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport2(0, 0, WindowWidth, WindowHeight);
	gluPerspective2(1.f, (float)(WindowWidth) / (float)(WindowHeight), RENDER_ITEMVIEW_NEAR, RENDER_ITEMVIEW_FAR);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	GetOpenGLMatrix(CameraMatrix);
	EnableDepthTest();
	EnableDepthMask();
	glClear(GL_DEPTH_BUFFER_BIT);
}

void SEASON3B::endrender3D()
{
	UpdateMousePositionn();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	BeginBitmap();
}

void SEASON3B::RenderLocalItem3D(float sx, float sy, float Width, float Height, int Type, int Level, int Option1, int ExtOption, bool PickUp)
{
	SEASON3B::begin3D();

	Render2Item3D(sx, sy, Width, Height, Type, Level, Option1, ExtOption, PickUp);

	SEASON3B::endrender3D();
}

void SEASON3B::Render2Item3D(float sx, float sy, float Width, float Height, int Type, int Level, int Option1, int ExtOption, bool PickUp)
{
	Script_Item* item_info = GMItemMng->find(Type);

	if (item_info && item_info->SubIndex != -1)
	{
		sx = sx + ((Width - item_info->Getwidth()) / 2.f);
		sy = sy + ((Height - item_info->Getheight()) / 2.f);

		RenderItem3D(sx, sy, item_info->Getwidth(), item_info->Getheight(), Type, Level, Option1, ExtOption, PickUp);
	}
}

const char* SEASON3B::FontHTML(const char* text, BYTE Style, bool bold, bool addLineBreak)
{
	char strText[MAX_PATH];
	static char strHtml[MAX_PATH];

	memset(strHtml, 0, sizeof(strHtml));
	memset(strText, 0, sizeof(strText));

	const char* HexColor;

	int pos = 0;

	switch (Style)
	{
	case 0:
		HexColor = "#FFFFFF";
		break;
	case 1:
		HexColor = "#7FB2FF";
		break;
	case 2:
		HexColor = "#FF3319";
		break;
	case 3:
		HexColor = "#FFCC19";
		break;
	case 4:
		HexColor = "#19FF7F";
		break;
	case 5:
		HexColor = "#A00000";
		break;
	case 6:
		HexColor = "#FF19FF";
		break;
	case 7:
		HexColor = "#0000A0";
		break;
	case 8:
		HexColor = "#A06600";
		break;
	case 9:
		HexColor = "#00FF00";
		break;
	case 10:
		HexColor = "#B2B2B2";
		break;
	case 11:
		HexColor = "#CC7FCC";
		break;
	case 12:
		HexColor = "#B266FF";
		break;
	case 13:
		HexColor = "#E56B0A";
		break;
	case 14:
		HexColor = "#800080";
		break;
	case 15:
		HexColor = "#FFFF00";
		break;
	case 16:
		HexColor = "#00B0F0";
		break;
	case 17:
		HexColor = "#00B050";
		break;
	case 18:
		HexColor = "#CCFF00";
		break;
	case 19:
		HexColor = "#00FFFF";
		break;
	case 20:
		HexColor = "#BCBCBC";
		break;
	case 21:
		HexColor = "#C1C469";
		break;
	default:
		HexColor = "#FFFFFF";
		break;
	}


	for (const char* p = text; *p; ++p)
	{
		if (*p == '<')
		{
			strText[pos++] = '&';
			strText[pos++] = 'l';
			strText[pos++] = 't';
			strText[pos++] = ';';
		}
		else {
			strText[pos++] = *p;
		}
	}

	if (bold == true)
		sprintf_s(strHtml, "<font face='$NormalFont' color='%s'><b>%s</b></font>", HexColor, strText);
	else
		sprintf_s(strHtml, "<font face='$NormalFont' color='%s'>%s</font>", HexColor, strText);

	if (addLineBreak)
		strcat(strHtml, "<BR>");

	return strHtml;
}

const char* SEASON3B::FontHTML(const char* text, DWORD color, BYTE Size, bool Bold, bool addLineBreak, int aling_on)
{
	char strText[MAX_PATH];
	static char strHtml[MAX_PATH];

	memset(strHtml, 0, sizeof(strHtml));
	memset(strText, 0, sizeof(strText));

	int pos = 0;

	for (const char* p = text; *p; ++p)
	{
		if (*p == '<')
		{
			strText[pos++] = '&';
			strText[pos++] = 'l';
			strText[pos++] = 't';
			strText[pos++] = ';';
		}
		else {
			strText[pos++] = *p;
		}
	}

	unsigned char r = GetRed(color);
	unsigned char g = GetGreen(color);
	unsigned char b = GetBlue(color);

	char strColor[12];
	sprintf_s(strColor, "%02X%02X%02X", r, g, b);

	if (Size == 0)
	{
		if (aling_on == 0)
		{
			if (Bold == true)
				sprintf_s(strHtml, "<font face='$NormalFont' color=\"#%s\"><b>%s</b></font>", strColor, strText);
			else
				sprintf_s(strHtml, "<font face='$NormalFont' color=\"#%s\">%s</font>", strColor, strText);
		}
		else
		{
			char _Aling[100];

			switch (aling_on)
			{
			case 1:
				sprintf_s(_Aling, "left");
				break;
			case 2:
				sprintf_s(_Aling, "right");
				break;
			case 3:
				sprintf_s(_Aling, "center");
				break;
			default:
				sprintf_s(_Aling, "justify");
				break;
			}

			if (Bold == true)
				sprintf_s(strHtml, "<p align=\"%s\"><font face='$NormalFont' color=\"#%s\"><b>%s</b></font></p>", _Aling, strColor, strText);
			else
				sprintf_s(strHtml, "<p align=\"%s\"><font face='$NormalFont' color=\"#%s\">%s</font></p>", _Aling, strColor, strText);
		}
	}
	else
	{
		if (aling_on == 0)
		{
			if (Bold == true)
				sprintf_s(strHtml, "<font face='$NormalFont' color=\"#%s\" Size=\"%d\"><b>%s</b></font>", strColor, Size, strText);
			else
				sprintf_s(strHtml, "<font face='$NormalFont' color=\"#%s\" Size=\"%d\">%s</font>", strColor, Size, strText);
		}
		else
		{
			char _Aling[100];

			switch (aling_on)
			{
			case 1:
				sprintf_s(_Aling, "left");
				break;
			case 2:
				sprintf_s(_Aling, "right");
				break;
			case 3:
				sprintf_s(_Aling, "center");
				break;
			default:
				sprintf_s(_Aling, "justify");
				break;
			}

			if (Bold == true)
				sprintf_s(strHtml, "<p align=\"%s\"><font face='$NormalFont' Size=\"%d\" color=\"#%s\" letterSpacing=\"0.000000\" kerning=\"0\"><b>%s</b></font></p>", _Aling, Size, strColor, strText);
			else
				sprintf_s(strHtml, "<p align=\"%s\"><font face='$NormalFont' Size=\"%d\" color=\"#%s\" letterSpacing=\"0.000000\" kerning=\"0\">%s</font></p>", _Aling, Size, strColor, strText);
		}
	}
	//<p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>1</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>2</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>3</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>4</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>5</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>6</b></font></p><p align="center"><font face="$NormalFont" size="12" color="#ffc000" letterSpacing="0.000000" kerning="0"><b>7</b></font></p>
	std::string;

	if (addLineBreak)
		strcat(strHtml, "<BR>");

	return strHtml;
}

void SEASON3B::RenderlookFetch(int Image, float x, float y, bool Enable, bool bRight)
{
	if (bRight)
	{
		x -= 17;
	}

	if (CheckMouseIn(x, y, 17, 18) && MouseLButton)
	{
		RenderImage(Image, x, y, 17.f, 18.f, 0.0, 18.f);
	}
	else
	{
		RenderImage(Image, x, y, 17.f, 18.f, 0.0, (Enable == true) ? 0.0 : 18.f);
	}
}

bool SEASON3B::CheckMouseFetch(float x, float y, bool Enable, bool bRight)
{
	if (bRight)
	{
		x -= 17;
	}

	return (CheckMouseIn(x, y, 17, 18) && Enable);
}

void SEASON3B::RenderNavigationControls(float RenderFrameX, float RenderFrameY, float RenderSizeX, int aling_on)
{
	float sv = 0.0;
	if (CheckMouseIn(RenderFrameX, RenderFrameY, 17, 18) && MouseLButton)
		sv = 18.f;

	SEASON3B::RenderImage(CNewUIQuestProgress::IMAGE_QP_BTN_L, RenderFrameX, RenderFrameY, 17.f, 18.f, 0.0, sv);

	sv = 0.0;
	RenderFrameX += RenderSizeX - 17.f;
	if (CheckMouseIn(RenderFrameX, RenderFrameY, 17.f, 18.f) && MouseLButton)
		sv = 18.f;

	SEASON3B::RenderImage(CNewUIQuestProgress::IMAGE_QP_BTN_R, RenderFrameX, RenderFrameY, 17.f, 18.f, 0.0, sv);
}

bool SEASON3B::IsNone(int iVirtKey)
{ 
	return g_pNewKeyInput->IsNone(iVirtKey); 
}

bool SEASON3B::IsRelease(int iVirtKey)
{ 
	return g_pNewKeyInput->IsRelease(iVirtKey); 
}

bool SEASON3B::IsPress(int iVirtKey)
{ 
	return g_pNewKeyInput->IsPress(iVirtKey); 
}

bool SEASON3B::IsRepeat(int iVirtKey)
{ 
	return g_pNewKeyInput->IsRepeat(iVirtKey); 
}

SEASON3B::CNewKeyInput::CNewKeyInput()
{
	Init();
}

SEASON3B::CNewKeyInput::~CNewKeyInput()
{

}

void SEASON3B::CNewKeyInput::Init()
{
	memset(&m_pInputInfo, 0, sizeof(INPUTSTATEINFO)*256);
}

SEASON3B::CNewKeyInput* SEASON3B::CNewKeyInput::GetInstance()
{
	static SEASON3B::CNewKeyInput s_Instance;
	return &s_Instance;
}

void SEASON3B::CNewKeyInput::ScanAsyncKeyState()
{
	if (gwinhandle->CheckWndActive())
	{
#ifdef __ANDROID__
#else
		for (int key = 0; key < 256; key++)
		{
			if (HIBYTE(GetAsyncKeyState(key)) & 0x80)
			{
				if (m_pInputInfo[key].byKeyState == KEY_NONE || m_pInputInfo[key].byKeyState == KEY_RELEASE)
				{
					// press event (key was up before but down now)
					m_pInputInfo[key].byKeyState = KEY_PRESS;
				}
				else if (m_pInputInfo[key].byKeyState == KEY_PRESS)
				{
					// drag event (key is still down)
					m_pInputInfo[key].byKeyState = KEY_REPEAT;
				}
			}
			else // Key is not currently pressed
			{
				if (m_pInputInfo[key].byKeyState == KEY_REPEAT || m_pInputInfo[key].byKeyState == KEY_PRESS)
				{
					// release event (key was down before but up now)
					m_pInputInfo[key].byKeyState = KEY_RELEASE;
				}
				else if (m_pInputInfo[key].byKeyState == KEY_RELEASE)
				{
					m_pInputInfo[key].byKeyState = KEY_NONE;
				}
			}
		}
#endif

		if (IsPress(VK_RETURN) && IsEnterPressed() == false)
		{
			m_pInputInfo[VK_RETURN].byKeyState = KEY_NONE;
		}
		SetEnterPressed(false);
	}
}

bool SEASON3B::CNewKeyInput::IsNone(int iVirtKey)
{
	if (gwinhandle->CheckWndActive())
		return (m_pInputInfo[iVirtKey].byKeyState == KEY_NONE) ? true : false;
	else
		return false;
}

bool SEASON3B::CNewKeyInput::IsRelease(int iVirtKey)
{
	if (gwinhandle->CheckWndActive())
		return (m_pInputInfo[iVirtKey].byKeyState == KEY_RELEASE) ? true : false;
	else
		return false;
}

bool SEASON3B::CNewKeyInput::IsPress(int iVirtKey)
{
	if (gwinhandle->CheckWndActive())
		return (m_pInputInfo[iVirtKey].byKeyState == KEY_PRESS) ? true : false;
	else
		return false;
}

bool SEASON3B::CNewKeyInput::IsRepeat(int iVirtKey)
{
	if (gwinhandle->CheckWndActive())
		return (m_pInputInfo[iVirtKey].byKeyState == KEY_REPEAT) ? true : false;
	else
		return false;
}

void SEASON3B::CNewKeyInput::SetKeyState(int iVirtKey, KEY_STATE KeyState)
{
	m_pInputInfo[iVirtKey].byKeyState = KeyState;
}


void SEASON3B::CCheckBox::Initialize()
{
	m_hTextFont = g_hFont;
	m_ImgIndex = 0;
	su = 0.f;
	sv = 0.f;
	m_bEnable = 0;
}

void SEASON3B::CCheckBox::Destroy()
{
}

void SEASON3B::CCheckBox::RegisterImgIndex(int imgindex)
{
	m_ImgIndex = imgindex;
}

void SEASON3B::CCheckBox::ChangeCheckBoxInfo(int x, int y, int sx, int sy, float su, float sv)
{
	SetPos(x, y);
	SetSize(sx, sy);
	this->su = su;
	this->sv = sv;
}

void SEASON3B::CCheckBox::ChangeCheckBoxText(unicode::t_string btname)
{
	m_Name = btname;
}

bool SEASON3B::CCheckBox::Create(int imgindex, int x, int y, int sx, int sy, unicode::t_string btname)
{
	RegisterImgIndex(imgindex);
	ChangeCheckBoxInfo(x, y, sx, sy);
	ChangeCheckBoxText(btname);
	return true;
}

bool SEASON3B::CCheckBox::Render()
{
	int posx = m_Pos.x + m_Size.cx;
	float movey = (double)(m_Size.cy * (m_bEnable == false));

#if MAIN_UPDATE > 603
	if (m_ImgIndex == BITMAP_check_box)
	{
		if (m_bEnable)
			movey = 46.f;
		else
			movey = 0.f;
		SEASON3B::RenderImageF(m_ImgIndex, (float)m_Pos.x, (float)m_Pos.y, (float)m_Size.cx, (float)m_Size.cy, su, sv + movey, 45.f, 46.f);
		posx += 2;
	}
	else
	{
		RenderImage(m_ImgIndex, (float)m_Pos.x, (float)m_Pos.y, (float)m_Size.cx, (float)m_Size.cy, su, sv + movey);
	}
#else
	RenderImage(m_ImgIndex, (float)m_Pos.x, (float)m_Pos.y, (float)m_Size.cx, (float)m_Size.cy, su, sv + movey);
#endif

	g_pRenderText->RenderText(posx, m_Pos.y + 4, m_Name.c_str(), 0, 0, RT3_SORT_LEFT, 0);

	return true;
}

bool SEASON3B::CCheckBox::UpdateMouseEvent()
{
	bool success = false;

	if (IsRelease(VK_LBUTTON) && CheckMouseIn(m_Pos.x, m_Pos.y, m_Size.cx, m_Size.cy))
	{
		success = true;
		m_bEnable = m_bEnable == false;
	}

	return success;
}

const POINT& SEASON3B::CCheckBox::GetPos()
{
	return m_Pos;
	// TODO: Insertar una instrucci�n "return" aqu�
}

void SEASON3B::CCheckBox::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CCheckBox::SetSize(int width, int height)
{
	m_Size.cx = width;
	m_Size.cy = height;
}

SEASON3B::stMacroUIImage::stMacroUIImage()
{
	m_ImgIndex = -1;
	m_Pos.x = m_Pos.y = 0;
	output_width = 0.0;
	output_height = 0.0;
}

SEASON3B::stMacroUIImage::~stMacroUIImage()
{
}

const POINT& SEASON3B::stMacroUIImage::GetPos()
{
	// TODO: Insertar una instrucci�n "return" aqu�
	return m_Pos;
}

void SEASON3B::stMacroUIImage::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::stMacroUIImage::Register(int imgindex, int x, int y, int width, int height)
{
	m_ImgIndex = imgindex;
	m_Pos.x = x;
	m_Pos.y = y;
	output_width = width;
	output_height = height;
}

SEASON3B::stMacroUIText::stMacroUIText()
{
	m_Pos.x = m_Pos.y = 0;
	m_Name = "";
}

SEASON3B::stMacroUIText::~stMacroUIText()
{
}

const POINT& SEASON3B::stMacroUIText::GetPos()
{
	// TODO: Insertar una instrucci�n "return" aqu�
	return m_Pos;
}

void SEASON3B::stMacroUIText::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::stMacroUIText::Register(int x, int y, const char* pzText)
{
	m_Pos.x = x;
	m_Pos.y = y;
	m_Name = pzText;
}

void SEASON3B::stMacroUIText::Render()
{
	g_pRenderText->RenderText(m_Pos.x, m_Pos.y, m_Name.c_str(), 0, 0, RT3_SORT_LEFT, 0);
}

SEASON3B::CSlideBar::CSlideBar()
{
	m_Imgindex = 0;
	m_ImgBack = 0;
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_Width = 0;
	m_Height = 0;
	m_MaxLength = 0;
	m_iValue = 0;
	m_MinLength = 0;
}

SEASON3B::CSlideBar::~CSlideBar()
{
}

const POINT& SEASON3B::CSlideBar::GetPos()
{
	// TODO: Insertar una instrucci�n "return" aqu�
	return m_Pos;
}

void SEASON3B::CSlideBar::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CSlideBar::Create(int imgback, int imgindex, int x, int y, __int16 sx, __int16 sy, __int16 iMaxLength, __int16 start)
{
	m_Imgindex = imgindex;
	m_ImgBack = imgback;
	m_Pos.x = x;
	m_Pos.y = y;
	m_Width = sx;
	m_Height = sy;
	m_MaxLength = iMaxLength;
	m_MinLength = start;
}

bool SEASON3B::CSlideBar::MouseUpdate()
{
	int min = m_Width / m_MaxLength;
	if (CheckMouseIn(m_Pos.x - min, m_Pos.y, min + m_Width, m_Height) && IsRepeat(VK_LBUTTON))
	{
		int current = MouseX - m_Pos.x;
		if (current >= m_MinLength)
		{
			m_iValue = (current * m_MaxLength / m_Width) + 1;
		}
		else
		{
			m_iValue = m_MinLength;
		}
	}
	return true;
}

void SEASON3B::CSlideBar::Render()
{
	RenderImage(m_ImgBack, m_Pos.x, m_Pos.y, m_Width, m_Height);
	if (m_iValue > 0)
	{
		float width = (double)m_Width * 0.1 * m_iValue;

		RenderImage(m_Imgindex, m_Pos.x, m_Pos.y, width, m_Height);
	}
}

int SEASON3B::CSlideBar::GetSlideLevel()
{
	return m_iValue;
}

void SEASON3B::CSlideBar::SetSlideLevel(__int16 Value)
{
	m_iValue = Value;
}

SEASON3B::COptionButtonGroup::COptionButtonGroup()
{
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_w = 0;
	m_h = 0;
	m_index = 0;
	m_imgindex = 0;
}

SEASON3B::COptionButtonGroup::~COptionButtonGroup()
{
	std::vector<CCheckBox*>::iterator iter = this->m_box.begin();
	while (iter != this->m_box.end())
	{
		delete* iter;
		iter++;
	}
	this->m_box.shrink_to_fit();
	this->m_box.clear();
}

const POINT& SEASON3B::COptionButtonGroup::GetPos()
{
	return m_Pos;
}

void SEASON3B::COptionButtonGroup::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::COptionButtonGroup::Create(int imgindex, int x, int y, int w, int h, unsigned char count, float su, float sv)
{
	m_Pos.x = x;
	m_Pos.y = y;
	this->m_w = w;
	this->m_h = h;
	for (int i = 0; i < count; i++)
	{
		CCheckBox* check_box = new CCheckBox;
		check_box->RegisterImgIndex(imgindex);
		check_box->ChangeCheckBoxInfo(0, 0, w, h, su, sv);
		this->m_box.push_back(check_box);
	}
}

void SEASON3B::COptionButtonGroup::SetOptionText(unsigned char index, int offset_x, int offset_y, const char* text)
{
	unicode::t_string strText = text;
	this->m_box[index]->SetPos(m_Pos.x + offset_x, m_Pos.y + offset_y);
	this->m_box[index]->ChangeCheckBoxText(strText);
}

bool SEASON3B::COptionButtonGroup::UpdateMouseEvent()
{
	for (size_t i = 0; i < this->m_box.size(); i++)
	{
		if (true == this->m_box[i]->UpdateMouseEvent())
		{
			if (true == this->m_box[i]->IsSelected())
			{
				SetIndex(i);
				return true;
			}
		}
	}
	return false;
}

bool SEASON3B::COptionButtonGroup::Render()
{
	for (size_t i = 0; i < this->m_box.size(); i++)
	{
		this->m_box[i]->Render();
	}
	return true;
}

void SEASON3B::COptionButtonGroup::SetIndex(int index)
{
	if ((unsigned int)index < this->m_box.size())
		m_index = index;
	else
		m_index = 0;

	for (size_t i = 0; i < this->m_box.size(); i++)
	{
		if (i == this->m_index)
			this->m_box[i]->SetSelected(true);
		else
			this->m_box[i]->SetSelected(false);
	}
}

int SEASON3B::COptionButtonGroup::GetIndex()
{
	return m_index;
}

#ifdef EFFECT_MNG_HANDLE
SEASON3B::CInputComboBox::CInputComboBox()
{
	isVisible = false;
	m_iViewCount = 6;
	selectedIndex = -1;
	posX = 0;
	posY = 0;
	width = 50;
	height = 13;

	m_ScrollBar.Create(0, 0, height* m_iViewCount);
	m_ScrollBar.SetPercent(0);
}

SEASON3B::CInputComboBox::~CInputComboBox()
{
	RemoveAll();
}

void SEASON3B::CInputComboBox::setLocation(int x, int y)
{
	posX = x;
	posY = y;
	m_ScrollBar.SetPos(posX + (width - 14), posY + (height + 2));
}

void SEASON3B::CInputComboBox::setBounds(int x, int y, int sx, int sy)
{
	width = sx;
	height = sy;
	setLocation(x, y);
}

void SEASON3B::CInputComboBox::addItem(const ItemComboBox& object)
{
	objects.push_back(object);
}

void SEASON3B::CInputComboBox::SelectedIndex(int index)
{
	if (index >= 0 && index < (int)objects.size())
	{
		selectedIndex = index;
		m_ScrollBar.SetPercent(0);
	}
}

void SEASON3B::CInputComboBox::RemoveAll()
{
	selectedIndex = -1;
	m_ScrollBar.SetPercent(0);
	objects.clear();
}

int SEASON3B::CInputComboBox::getSelectedIndex() const
{
	return selectedIndex;
}

const char* SEASON3B::CInputComboBox::getSelectedText()
{
	if (selectedIndex >= 0 && selectedIndex < (int)objects.size())
	{
		return objects[selectedIndex].text.c_str();
	}
	else {
		return "None";
	}
}

const char* SEASON3B::CInputComboBox::getIndexText(int index)
{
	if (index >= 0 && index < (int)objects.size())
	{
		return objects[index].text.c_str();
	}
	else {
		return "None";
	}
}

int SEASON3B::CInputComboBox::getSelectedId()
{
	if (selectedIndex >= 0 && selectedIndex < (int)objects.size())
	{
		return objects[selectedIndex].id;
	}
	else {
		return -1;
	}
}

void SEASON3B::CInputComboBox::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	//SEASON3B::RenderImage(BITMAP_GUILDMAKE_BEGIN, posX, posY, 108.f, 23.f);

	int btX = posX + width - 13;
	int btY = posY;

	if (isVisible == true)
	{
		if (SEASON3B::CheckMouseIn(btX, posY, 15.f, 13.f))
		{
			if (MouseLButton)
				SEASON3B::RenderImage(IMAGE_BTN_UP, btX, btY, 15.f, 13.f, 0.0, 26.f);
			else
				SEASON3B::RenderImage(IMAGE_BTN_UP, btX, btY, 15.f, 13.f, 0.0, 13.f);
		}
		else
		{
			SEASON3B::RenderImage(IMAGE_BTN_UP, btX, btY, 15.f, 13.f, 0.0, 0.0);
		}
	}
	else
	{
		if (SEASON3B::CheckMouseIn(btX, btY, 15.f, 13.f))
		{
			if (MouseLButton)
				SEASON3B::RenderImage(IMAGE_BTN_DOWN, btX, btY, 15.f, 13.f, 0.0, 26.f);
			else
				SEASON3B::RenderImage(IMAGE_BTN_DOWN, btX, btY, 15.f, 13.f, 0.0, 13.f);
		}
		else
		{
			SEASON3B::RenderImage(IMAGE_BTN_DOWN, btX, btY, 15.f, 13.f, 0.0, 0.0);
		}
	}

	this->RenderFrameWork();
}

void SEASON3B::CInputComboBox::RenderFrameWork()
{
	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(-1);

	g_pRenderText->SetFont(g_hFont);

	g_pRenderText->RenderFont(posX + 4, posY, getSelectedText(), (width - 16), height, 1);

	if (isVisible == true)
	{
		size_t secure = 0;
		size_t currentLoop = 0;
		size_t Loop = objects.size();

		if (Loop > m_iViewCount)
		{
			double prev = m_ScrollBar.GetPercent();
			currentLoop = (size_t)((double)(unsigned int)(Loop - m_iViewCount) * prev);
		}

		int Y = posY + (height + 2);

		glColor4f(0.f, 0.f, 0.f, 0.8f);
		RenderColor(posX, Y, width, (height * m_iViewCount));
		EndRenderColor();

		for (size_t i = currentLoop; i < Loop && secure < m_iViewCount; i++, secure++)
		{
			if (SEASON3B::CheckMouseIn(posX, Y, (width - 16), height))
			{
				if (SEASON3B::IsRelease(VK_LBUTTON))
				{
					SelectedIndex(i);
					isVisible = false;
				}
			}
			g_pRenderText->RenderFont(posX + 4, Y, getIndexText(i), (width - 16), height, 1);
			Y += height;
		}

		m_ScrollBar.Render();
	}
}

bool SEASON3B::CInputComboBox::UpdateMouse()
{
	int btX = posX + width - 13;

	if (SEASON3B::CheckMouseIn(btX, posY, 15.f, 13.f))
	{
		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			isVisible = !isVisible;
		}
		return true;
	}
	if (isVisible == true)
	{
		m_ScrollBar.Update();
		m_ScrollBar.UpdateMouseEvent();

		if (SEASON3B::CheckMouseIn(posX, posY + (height + 2), width, (height * m_iViewCount)))
		{
			return true;
		}
	}

	return false;
}

bool SEASON3B::CInputComboBox::Update()
{
	return false;
}
#endif // EFFECT_MNG_HANDLE

void SEASON3B::runtime_number_mask(int groupid, float RenderFrameX, float RenderFrameY, int numerador, int size)
{
	std::string numStr = std::to_string(numerador);
	float desface = size * 0.4375;
	float totalWidth = numStr.size() * desface;
	float startX = RenderFrameX - (totalWidth / 2.0f);

	float uw = 32.f;
	float vh = 32.f;

	if (groupid < 4)
	{
		EnableAlphaBlend();
		for (size_t i = 0; i < numStr.size(); ++i)
		{
			char digit = numStr[i];
			int digitValue = digit - '0';
			float posX = startX + i * desface; // Desplazar cada d�gito en 32 p�xeles

			SEASON3B::RenderImageF(BITMAP_group_effra_light, posX, RenderFrameY, size, size, (digitValue * uw), (groupid * vh), uw, vh);
			//glColor4f(0.6, 0.6, 0.6, 0.5);
			//SEASON3B::RenderImageF(BITMAP_group_effra_light, posX, RenderFrameY, 16.f, 16.f, su, sv, uw, vh);
		}
	}

	EnableAlphaTest(true);
	glColor4f(0.8, 0.8, 0.8, 1.0f);
	for (size_t i = 0; i < numStr.size(); ++i)
	{
		char digit = numStr[i];
		int digitValue = digit - '0';
		float posX = startX + i * desface; // Desplazar cada d�gito en 32 p�xeles
		SEASON3B::RenderImageF(BITMAP_normal_effra_light, posX, RenderFrameY, size, size, (digitValue * uw), 0.0, uw, vh);
	}
}

void SEASON3B::runtime_render_mask(int img, float porcento, float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY, float su, float sv, float uw, float vh, int type)
{
	if (type == 0)
	{
		float SizeVHY = (vh * porcento);
		float renderSizeY = (porcento * RenderSizeY);
		float renderY = RenderFrameY + (RenderSizeY - renderSizeY);
		SEASON3B::RenderImageF(img, RenderFrameX, renderY, RenderSizeX, renderSizeY, su, sv + (vh - SizeVHY), uw, SizeVHY);
	}
	if (type == 2)
	{
		RenderSizeX *= (porcento);
		SEASON3B::RenderImageF(img, RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY, su, sv, (uw * porcento), vh);
	}
	if (type == 3)
	{
		float renderSizeX = (porcento * RenderSizeX);
		RenderFrameX += (RenderSizeX - renderSizeX);
		su += (uw - (uw * porcento));
		SEASON3B::RenderImageF(img, RenderFrameX, RenderFrameY, renderSizeX, RenderSizeY, su, sv, (uw * porcento), vh);
	}
}

void SEASON3B::LoadPos(CNewUIButton* eBtn, int index, int RenderPosX, int RenderPosY)
{
#if MAIN_UPDATE > 603
	if (eBtn && (index >= 0 && index <= 5))
	{
		RenderPosY += 384;
		RenderPosX += 22 + (eBtn->GetSize().x + 2) * index;
		eBtn->SetPos(RenderPosX, RenderPosY);
	}
#else
#endif
}

void SEASON3B::Loadbutton(CNewUIButton* eBtn, int index, int RenderPosX, int RenderPosY, int RenderSizeX, int RenderSizeY, int imgindex, bool overflg, bool isimgwidth, bool bClickEffect, float su, float sv, float uw, float vh)
{
#if MAIN_UPDATE > 603
	if (eBtn && (index >= 0 && index <= 5))
	{
		RenderPosX += 22;
		eBtn->ChangeButtonImgState(true, imgindex, overflg, isimgwidth, bClickEffect, su, sv, uw, vh);
		eBtn->ChangeButtonInfo(RenderPosX + (RenderSizeX + 2)* index, RenderPosY + 384, RenderSizeX, RenderSizeY);
	}
#else
#endif
}

void SEASON3B::window_nomal(float RenderOriginX, float RenderOriginY, int type, float division)
{
	float RenderFrameX = RenderOriginX;
	float RenderFrameY = RenderOriginY;

#if MAIN_UPDATE > 603
	if (type == 1)
		RenderImageF(BITMAP_interface_back3, RenderFrameX, RenderFrameY, (float)WIN_WINDOW_SIZEX, 276.f, 0.0, 0.0, 752.0, 1024.f);
	else
		RenderImageF(BITMAP_interface_back1, RenderFrameX, RenderFrameY, (float)WIN_WINDOW_SIZEX, 276.f, 0.0, 0.0, 752.0, 1024.f);

	if (type == 4)
	{
		SEASON3B::RenderImageF(CNewUIMyInventory::IMAGE_ITEM_DECOR_CORE, RenderFrameX + ((WIN_WINDOW_SIZEX - 180.f) * 0.5f), RenderFrameY + 25.f, 180.f, 177.f, 0.0, 6.0, 375.f, 369.f);
	}

	RenderFrameY += 276.f;
	RenderImageF(BITMAP_interface_back2, RenderFrameX, RenderFrameY, (float)WIN_WINDOW_SIZEX, 152.f, 0.0, 0.0, 752.0, 561.0);

	if (division != 0.f)
	{
		division -= 9.f;
		RenderImageF(BITMAP_msgbox_line, RenderFrameX + 6, RenderOriginY + division, (float)WIN_WINDOW_SIZEX - 12.f, 40.f, 0.0, 0.0, 611.f, 132.f);
	}

#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		RenderInventoryInterface(RenderFrameX, RenderFrameY, (int)division);
	}
	else
	{
		float RenderSizeY = 64.f;
		RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK, RenderFrameX, RenderFrameY, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY);
		if (type == 0)
			RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP, RenderFrameX, RenderFrameY, WIN_WINDOW_SIZEX, RenderSizeY);
		else
			RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK_TOP2, RenderFrameX, RenderFrameY, WIN_WINDOW_SIZEX, RenderSizeY);

		RenderFrameY += RenderSizeY;
		RenderSizeY = WIN_WINDOW_SIZEY - 109.f;

		RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK_LEFT, RenderFrameX, RenderFrameY, 21.f, RenderSizeY);
		RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK_RIGHT, RenderFrameX + WIN_WINDOW_SIZEX - 21.f, RenderFrameY, 21.f, RenderSizeY);

		RenderFrameY += RenderSizeY;
		RenderSizeY = 45.f;
		RenderImage(CNewUIMyInventory::IMAGE_INVENTORY_BACK_BOTTOM, RenderFrameX, RenderFrameY, WIN_WINDOW_SIZEY, RenderSizeY);

		if (division != 0.f)
		{
			RenderImage(CNewUIMyQuestInfoWindow::IMAGE_MYQUEST_LINE, RenderFrameX + 1, RenderOriginY + division, 188.f, 21.f);
		}
	}
#else
	if (division != 0.f)
	{
		division += 5.f;
	}
	RenderInventoryInterface(RenderFrameX, RenderFrameY, (int)division);
#endif

#if MAIN_UPDATE > 603

#elif MAIN_UPDATE > 303

#else

#endif
}

void SEASON3B::window_emergent(CNewUIMessageBoxBase* pMsgBox, int iMiddleFrameCount, int type)
{
	float iRenderFrameX = pMsgBox->GetPos().x;
	float iRenderFrameY = pMsgBox->GetPos().y;
	float iRenderSizeX = pMsgBox->GetSize().cx;
	float iRenderSizeY = pMsgBox->GetSize().cy;

#if MAIN_UPDATE > 603
	if (type == 0)
	{
		iRenderSizeY = MSGBOX_TOP_HEIGHT;
		iRenderSizeY += (MSGBOX_MIDDLE_HEIGHT * iMiddleFrameCount);
		iRenderSizeY += MSGBOX_BOTTOM_HEIGHT;

		iRenderFrameY += (iRenderSizeY - 250.f) / 2.f;
		SEASON3B::RenderImageF(BITMAP_msgbox_back_frame, iRenderFrameX, iRenderFrameY + 10, iRenderSizeX, 250.f, 0.0, 0.0, 665.f, 913.f);
	}
	else
	{
		float calculo_vertical = ((iRenderSizeY - 28) / 428.f) * 1460.f;
		SEASON3B::RenderImageF(BITMAP_window_box_2048, iRenderFrameX + 8, iRenderFrameY + 14, iRenderSizeX - 16.f, iRenderSizeY - 28.f, 770.f, 0.0, 698.f, calculo_vertical);

		SEASON3B::RenderImageF(BITMAP_window_box_2048, iRenderFrameX, iRenderFrameY, iRenderSizeX, MSGBOX_TOP_HEIGHT, 0.0, 0.0, 756.f, 245.f);

		iRenderFrameY += MSGBOX_TOP_HEIGHT;

		if (iMiddleFrameCount != 0)
		{
			SEASON3B::RenderImageF(BITMAP_window_box_2048, iRenderFrameX, iRenderFrameY, iRenderSizeX, (MSGBOX_MIDDLE_HEIGHT * iMiddleFrameCount), 0.0, 245.f, 756.f, (55.f * iMiddleFrameCount));
			iRenderFrameY += MSGBOX_MIDDLE_HEIGHT * iMiddleFrameCount;
		}

		SEASON3B::RenderImageF(BITMAP_window_box_2048, iRenderFrameX, iRenderFrameY, iRenderSizeX, MSGBOX_BOTTOM_HEIGHT, 0.0, 1386.f, 756.f, 184.f);
	}
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		g_pUIManager->RenderWindowBase(iRenderSizeY, -1, iRenderFrameY);
	}
	else
	{
		RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_BACK, iRenderFrameX, iRenderFrameY + 2.f, (iRenderSizeX - MSGBOX_BACK_BLANK_WIDTH), (iRenderSizeY - MSGBOX_BACK_BLANK_HEIGHT));

		RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_TOP, iRenderFrameX, iRenderFrameY, iRenderSizeX, MSGBOX_TOP_HEIGHT);

		iRenderFrameY += MSGBOX_TOP_HEIGHT;

		for (int i = 0; i < iMiddleFrameCount; ++i)
		{
			RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_MIDDLE, iRenderFrameX, iRenderFrameY, iRenderSizeX, MSGBOX_MIDDLE_HEIGHT);
			iRenderFrameY += MSGBOX_MIDDLE_HEIGHT;
		}

		RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_BOTTOM, iRenderFrameX, iRenderFrameY, iRenderSizeX, MSGBOX_BOTTOM_HEIGHT);
	}
#else
	g_pUIManager->RenderWindowBase(iRenderSizeY, -1, iRenderFrameY);
#endif
}

//-- size max: 228, 252
void SEASON3B::window_backmsg(float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY)
{
	int width = (int)(RenderSizeX / 3.f);
	int hight = (int)(RenderSizeY / 2.f);

	int inventory_depth = CNewUIMyInventory::IMAGE_INVENTORY_BACK2;
	int inventory_border = CNewUIMyInventory::IMAGE_INVENTORY_BORDER;

	if (width > 76)
		width = 76;

	if (hight > 126)
		hight = 126;

	int leftover = RenderSizeX - (2 * width);
	int bothover = RenderSizeY - (2 * hight);

	int width_depth = RenderSizeX - 12.f;
	int hight_depth = RenderSizeY - 10.f;

	float iPos_x = RenderFrameX + (RenderSizeX - (float)width);
	float iPos_y = RenderFrameY + (RenderSizeY - (float)hight);

	//-- Background (parte central de la textura de fondo)
	SEASON3B::RenderImageF(inventory_depth, RenderFrameX + 6.f, RenderFrameY + 2.f, (float)width_depth, hight_depth, 0.0, 0.0, (float)width_depth >= 225.f ? 224.f : (float)width_depth, (float)hight_depth >= 512.f ? 512.f : (float)hight_depth);

	//-- border left
	SEASON3B::RenderImageF(inventory_border, RenderFrameX, RenderFrameY, (float)width, (float)hight, 0.0, 0.0, width, (float)hight);

	//-- border right
	SEASON3B::RenderImageF(inventory_border, iPos_x, RenderFrameY, (float)width, hight, (float)(228 - width), 0.0, (float)width, (float)hight);

	//-- border left down
	SEASON3B::RenderImageF(inventory_border, RenderFrameX, iPos_y, (float)width, (float)hight, 0.0, (float)(252 - hight), (float)width, (float)hight);

	//-- border right down
	SEASON3B::RenderImageF(inventory_border, iPos_x, iPos_y, (float)width, (float)hight, (float)(228 - width), (float)(252 - hight), (float)width, (float)hight);


	//-- leftover
	if (leftover > 0)
	{
		int max_width = 164;
		int sections = leftover / max_width;
		int remaining = leftover % max_width;
		float posx = RenderFrameX + (float)width;

		for (int i = 0; i < sections; i++)
		{
			SEASON3B::RenderImageF(inventory_border, posx, RenderFrameY, (float)max_width, hight, 32.f, 0.0, (float)max_width, hight);
			SEASON3B::RenderImageF(inventory_border, posx, iPos_y, (float)max_width, hight, 32.f, (float)(252 - hight), (float)max_width, hight);
			posx += (float)max_width;
		}

		if (remaining > 0)
		{
			SEASON3B::RenderImageF(inventory_border, posx, RenderFrameY, (float)remaining, hight, 32.f, 0.0, (float)remaining, hight);
			SEASON3B::RenderImageF(inventory_border, posx, iPos_y, (float)remaining, hight, 32.f, (float)(252 - hight), (float)remaining, hight);
		}
	}

	if (bothover > 0)
	{
		int max_width = 120;
		int sections = bothover / max_width;
		int remaining = bothover % max_width;
		float posy = RenderFrameY + (float)hight;
		float posx;
		for (int i = 0; i < sections; i++)
		{
			posx = RenderFrameX;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)max_width, 0.f, 72.f, (float)width, max_width);
			posx = iPos_x;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)max_width, (float)(228 - width), 72.f, (float)width, max_width);
			posy += (float)max_width;
		}

		if (remaining > 0)
		{
			posx = RenderFrameX;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)remaining, 0.f, 72.f, (float)width, remaining);
			posx = iPos_x;
			SEASON3B::RenderImageF(inventory_border, posx, posy, (float)width, (float)remaining, (float)(228 - width), 72.f, (float)width, remaining);
		}
	}
}
