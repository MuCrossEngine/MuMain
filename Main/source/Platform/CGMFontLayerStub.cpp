#ifdef __ANDROID__

#include "../stdafx.h"
#include "../CGMFontLayer.h"
#include "../UIControls.h"
#include "../MultiLanguage.h"

#include <cstring>

CGMFontLayer::CGMFontLayer()
{
	BitmapFontIndex = static_cast<FT_UInt>(-1);
}

CGMFontLayer::~CGMFontLayer() = default;

_FT_Bitmap* CGMFontLayer::GetULongChar(FT_ULong charcode)
{
	(void)charcode;
	return nullptr;
}

void CGMFontLayer::runtime_load_bitmap(GLuint* textures, GLsizei _width, GLsizei _height, BYTE* data)
{
	(void)textures;
	(void)_width;
	(void)_height;
	(void)data;
}

void CGMFontLayer::runtime_font_property(HFONT hFont, int PixelSize)
{
	(void)hFont;
	(void)PixelSize;
}

void CGMFontLayer::runtime_font_property(HDC hdc, HFONT hFont, DWORD dwTable, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize)
{
	(void)hdc;
	(void)hFont;
	(void)dwTable;
	(void)library;
	(void)FontType;
	(void)FontIndex;
	(void)PixelSize;
}

void CGMFontLayer::runtime_font_property(const char* file_base, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize, FT_Encoding encoding)
{
	(void)file_base;
	(void)library;
	(void)FontType;
	(void)FontIndex;
	(void)PixelSize;
	(void)encoding;
}

BOOL CGMFontLayer::_GetTextExtentPoint32(std::wstring wstrText, LPSIZE lpSize)
{
	if (lpSize == NULL)
	{
		return FALSE;
	}

	HDC hdc = g_pRenderText ? g_pRenderText->GetFontDC() : NULL;
	if (hdc == NULL)
	{
		lpSize->cx = static_cast<LONG>(wstrText.size() * 8);
		lpSize->cy = static_cast<LONG>(getmetrics());
		return TRUE;
	}

	return GetTextExtentPoint32W(hdc, wstrText.c_str(), (int)wstrText.length(), lpSize);
}

BOOL CGMFontLayer::_GetTextExtentPoint32(LPCSTR lpString, int cbString, LPSIZE lpSize)
{
	if (lpSize == NULL)
	{
		return FALSE;
	}

	HDC hdc = g_pRenderText ? g_pRenderText->GetFontDC() : NULL;
	if (hdc == NULL || lpString == NULL)
	{
		lpSize->cx = 0;
		lpSize->cy = static_cast<LONG>(getmetrics());
		return TRUE;
	}

	int textLength = cbString;
	if (textLength <= 0)
	{
		textLength = (int)std::strlen(lpString);
	}

	return g_pMultiLanguage->_GetTextExtentPoint32(hdc, lpString, textLength, lpSize);
}

void CGMFontLayer::_TextOut(std::wstring wstrText, int& pen_x, int& pen_y)
{
	SIZE size = { 0, 0 };
	if (_GetTextExtentPoint32(wstrText, &size))
	{
		pen_x += size.cx;
		if (pen_y < size.cy)
		{
			pen_y = size.cy;
		}
	}
}

void CGMFontLayer::runtime_writebuffer(int off_x, int off_y, _FT_Bitmap* pBitmap)
{
	(void)off_x;
	(void)off_y;
	(void)pBitmap;
}

void CGMFontLayer::runtime_render_map(int pen_x, int pen_y, int RealTextX, int RealTextY, int Width, int Height, bool background)
{
	(void)pen_x;
	(void)pen_y;
	(void)RealTextX;
	(void)RealTextY;
	(void)Width;
	(void)Height;
	(void)background;
}

void CGMFontLayer::RenderText(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth, int iHeight, int iSort, OUT SIZE* lpTextSize, bool background)
{
	if (pszText == NULL || g_pRenderText == NULL)
	{
		if (lpTextSize)
		{
			lpTextSize->cx = 0;
			lpTextSize->cy = 0;
		}
		return;
	}

	g_pRenderText->RenderText(iPos_x, iPos_y, pszText, iWidth, iHeight, iSort, lpTextSize, background);
}

void CGMFontLayer::RenderWave(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth, int iHeight, int iSort, OUT SIZE* lpTextSize)
{
	RenderText(iPos_x, iPos_y, pszText, iWidth, iHeight, iSort, lpTextSize, false);
}

void CGMFontLayer::runtime_render_map()
{
}

int CGMFontLayer::getmetrics()
{
	if (g_pRenderText)
	{
		HDC hdc = g_pRenderText->GetFontDC();
		if (hdc)
		{
			TEXTMETRICW tm = {};
			if (GetTextMetricsW(hdc, &tm))
			{
				return tm.tmHeight > 0 ? tm.tmHeight : 16;
			}
		}
	}

	return 16;
}

CGMFontLayer* CGMFontLayer::Instance()
{
	static CGMFontLayer s_instance;
	return &s_instance;
}

#endif // __ANDROID__
