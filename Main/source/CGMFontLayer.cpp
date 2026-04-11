#include "stdafx.h"
#include "UIControls.h"
#include "CGMFontLayer.h"
#include "MultiLanguage.h"
#include "./Utilities/Log/muConsoleDebug.h"

extern void BindTexture(int tex);

CGMFontLayer::CGMFontLayer()
{
	//BitmapIndex = -1;
	//output_width = 0;
	//output_hight = 0;
	//metrics_height = 0;

	BitmapFontIndex = -1;
}

CGMFontLayer::~CGMFontLayer()
{
	Characters.clear();
	hFontBuffer.clear();

	if (BitmapFontIndex != -1)
	{
		glDeleteTextures(1, &BitmapFontIndex);
	}

	for (int i = 0; i < MAX_LINE_FONT; i++)
	{
		if (NormalFont[i].BitmapIndex != -1)
		{
			glDeleteTextures(1, &NormalFont[i].BitmapIndex);
		}
		NormalFont[i].PakBuffer.clear();
	}
}

_FT_Bitmap* CGMFontLayer::GetULongChar(FT_ULong charcode)
{
	type_map_bitmap::iterator it = Characters.find(charcode);

	if (it != Characters.end())
	{
		return &it->second;
	}

	return NULL;
}

void CGMFontLayer::runtime_load_bitmap(GLuint* textures, GLsizei _width, GLsizei _height, BYTE* data)
{
	if ((*textures) != -1)
	{
		glDeleteTextures(1, textures);
	}
	glGenTextures(1, textures);
	glBindTexture(GL_TEXTURE_2D, *textures);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, _width, _height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

bool isChineseChar(FT_ULong charcode)
{
	return (charcode >= 0x4E00 && charcode <= 0x9FFF) ||
		(charcode >= 0x3400 && charcode <= 0x4DBF) ||
		(charcode >= 0x20000 && charcode <= 0x2A6DF) ||
		(charcode >= 0x2A700 && charcode <= 0x2B73F) ||
		(charcode >= 0x2B740 && charcode <= 0x2B81F) ||
		(charcode >= 0x2B820 && charcode <= 0x2CEAF) ||
		(charcode >= 0x2CEB0 && charcode <= 0x2EBEF) ||
		(charcode >= 0xF900 && charcode <= 0xFAFF);
}

bool isKoreanChar(FT_ULong charcode)
{
	return (charcode >= 0xAC00 && charcode <= 0xD7AF) ||
		(charcode >= 0x1100 && charcode <= 0x11FF) ||
		(charcode >= 0xA960 && charcode <= 0xA97F) ||
		(charcode >= 0xD7B0 && charcode <= 0xD7FF);
}

void CGMFontLayer::runtime_font_property(HDC hdc, HFONT hFont, DWORD dwTable, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize)
{
	TEXTMETRICW tm;

	SelectObject(hdc, hFont);

	if (GetTextMetricsW(hdc, &tm))
	{
		PixelSize = tm.tmHeight;

		if (tm.tmCharSet == GB2312_CHARSET)
		{
			dwTable = 0x66637474;
		}
	}

	DWORD fontSize = GetFontData(hdc, 0, 0, NULL, 0);

	if (fontSize == GDI_ERROR)
	{
		return;
	}

	std::vector<BYTE> fontData(fontSize, 0);

	if (GetFontData(hdc, dwTable, 0, fontData.data(), fontSize) == GDI_ERROR)
	{
		return;
	}

	FT_Face face;

	if (FT_New_Memory_Face(library, fontData.data(), fontSize, 0, &face))
	{
		return;
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
	{
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Set_Pixel_Sizes(face, 0, PixelSize);

	FontType->metrics_height = (face->size->metrics.height >> 6);

	int pen_x = 0, pen_y = 0;
	FT_UInt max_dim = (FontType->metrics_height)*ceilf(sqrtf(face->num_glyphs));

	FontType->output_width = 1;

	while (FontType->output_width < max_dim)
	{
		FontType->output_width <<= 1;
	}
	FontType->output_hight = FontType->output_width;

	int max_size = (FontType->output_width * FontType->output_hight);

	FontType->PakBuffer.resize(max_size, 0);

	int num_glyphs = 0;

	FT_UInt glyph_index = 1;
	FT_ULong Charcode = FT_Get_First_Char(face, &glyph_index);

	while (glyph_index != 0)
	{
		if (GetULongChar(Charcode) == NULL)
		{
			FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING);

			FT_Render_Glyph(face->glyph, ft_render_mode_normal);

			FT_Bitmap* pBitmap = &face->glyph->bitmap;

			if (pen_x + pBitmap->width >= FontType->output_width)
			{
				pen_x = 0;
				pen_y += FontType->metrics_height;
			}

			for (FT_UInt row = 0; row < pBitmap->rows; ++row)
			{
				for (FT_UInt col = 0; col < pBitmap->width; ++col)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					FontType->PakBuffer[y * FontType->output_width + x] = pBitmap->buffer[row * pBitmap->pitch + col];
				}
			}

			_FT_Bitmap pNewBitmap;
			pNewBitmap.FontType = FontIndex;
			pNewBitmap.RenderPosX = pen_x;
			pNewBitmap.RenderPosY = pen_y;
			pNewBitmap.RenderSizeX = pBitmap->width;
			pNewBitmap.RenderSizeY = pBitmap->rows;
			pNewBitmap.bitmap_left = face->glyph->bitmap_left;
			pNewBitmap.bitmap_top = face->glyph->bitmap_top;
			pNewBitmap.advance = face->glyph->advance.x >> 6;

			Characters.insert(type_map_bitmap::value_type(Charcode, pNewBitmap));

			pen_x += pBitmap->width + 1;
		}
		Charcode = FT_Get_Next_Char(face, Charcode, &glyph_index);
		num_glyphs++;
	}

	runtime_load_bitmap(&FontType->BitmapIndex, FontType->output_width, FontType->output_hight, FontType->PakBuffer.data());

	FT_Done_Face(face);
}

void CGMFontLayer::runtime_font_property(const char* file_base, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize, FT_Encoding encoding)
{
	FT_Face face;

	if (FT_New_Face(library, file_base, 0, &face))
	{
		return;
	}

	if (FT_Select_Charmap(face, encoding))
	{
		return;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Set_Pixel_Sizes(face, 0, PixelSize);

	FontType->metrics_height = (face->size->metrics.height >> 6);

	int pen_x = 0, pen_y = 0;
	FT_UInt max_dim = (FontType->metrics_height) * ceilf(sqrtf(face->num_glyphs));

	FontType->output_width = 1;

	while (FontType->output_width < max_dim)
	{
		FontType->output_width <<= 1;
	}
	FontType->output_hight = FontType->output_width;

	int max_size = (FontType->output_width * FontType->output_hight);

	FontType->PakBuffer.resize(max_size, 0);

	int num_glyphs = 0;

	FT_UInt glyph_index = 1;
	FT_ULong Charcode = FT_Get_First_Char(face, &glyph_index);

	while (glyph_index != 0)
	{
		if (GetULongChar(Charcode) == NULL)
		{
			FT_Load_Glyph(face, glyph_index, FT_LOAD_NO_HINTING);

			FT_Render_Glyph(face->glyph, ft_render_mode_normal);

			FT_Bitmap* pBitmap = &face->glyph->bitmap;

			if (pen_x + pBitmap->width >= FontType->output_width)
			{
				pen_x = 0;
				pen_y += FontType->metrics_height;
			}

			for (FT_UInt row = 0; row < pBitmap->rows; ++row)
			{
				for (FT_UInt col = 0; col < pBitmap->width; ++col)
				{
					int x = pen_x + col;
					int y = pen_y + row;
					FontType->PakBuffer[y * FontType->output_width + x] = pBitmap->buffer[row * pBitmap->pitch + col];
				}
			}

			_FT_Bitmap pNewBitmap;
			pNewBitmap.FontType = FontIndex;
			pNewBitmap.RenderPosX = pen_x;
			pNewBitmap.RenderPosY = pen_y;
			pNewBitmap.RenderSizeX = pBitmap->width;
			pNewBitmap.RenderSizeY = pBitmap->rows;
			pNewBitmap.bitmap_left = face->glyph->bitmap_left;
			pNewBitmap.bitmap_top = face->glyph->bitmap_top;
			pNewBitmap.advance = face->glyph->advance.x >> 6;

			Characters[Charcode] = pNewBitmap;

			pen_x += pBitmap->width + 1;
		}
		Charcode = FT_Get_Next_Char(face, Charcode, &glyph_index);
		num_glyphs++;
	}

	runtime_load_bitmap(&FontType->BitmapIndex, FontType->output_width, FontType->output_hight, FontType->PakBuffer.data());

	FT_Done_Face(face);
}

void CGMFontLayer::runtime_font_property(HFONT hFont, int PixelSize)
{
	FT_Library library;

	if (FT_Init_FreeType(&library))
	{
		return;
	}
	//HFONT _hFont;
	bool font_korean = GetPrivateProfileInt("SettingsFont", "font-korean", 0, ".\\config.ini");

	bool font_chinase = GetPrivateProfileInt("SettingsFont", "font-chinase", 0, ".\\config.ini");

	HDC hdc = GetDC(NULL);

	Characters.clear();

	if (font_chinase)
		runtime_font_property("Data\\Interface\\HUD\\fonts\\fonts_all.ttf", library, &NormalFont[0], 0, PixelSize);
	else
		runtime_font_property(hdc, hFont, 0, library, &NormalFont[0], 0, PixelSize);


	if (font_chinase)
	{
		runtime_font_property("Data\\Interface\\HUD\\fonts\\fonts_ch.ttf", library, &NormalFont[1], 1, PixelSize);
		//_hFont = CreateFont(PixelSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft YaHei UI");
		//runtime_font_property(hdc, _hFont, 0x66637474, library, &NormalFont[1], 1, PixelSize);
	}

	if (font_korean)
	{
		runtime_font_property("Data\\Interface\\HUD\\fonts\\fonts_kr.ttf", library, &NormalFont[2], 2, PixelSize);
		//_hFont = CreateFont(PixelSize, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Malgun Gothic");
		//runtime_font_property(hdc, _hFont, 0, library, &NormalFont[2], 2, PixelSize);
	}

	BYTE* buffer = new BYTE[512*32];

	runtime_load_bitmap(&BitmapFontIndex, 512, 32, buffer);

	SAFE_DELETE_ARRAY(buffer);

	FT_Done_FreeType(library);

	DeleteDC(hdc);
}

BOOL CGMFontLayer::_GetTextExtentPoint32(std::wstring wstrText, LPSIZE lpSize)
{
	if (lpSize)
	{
		lpSize->cx = 0;
		lpSize->cy = 0;

		for (auto wstr = wstrText.begin(); wstr != wstrText.end(); wstr++)
		{
			_FT_Bitmap* pBitmap;

			if (*wstr == 32)
				pBitmap = GetULongChar('|');
			else
				pBitmap = GetULongChar(*wstr);

			if (!pBitmap)
				pBitmap = GetULongChar(1);

			if (!pBitmap)
				continue;

			int metrics = ((NormalFont[pBitmap->FontType].metrics_height - pBitmap->bitmap_top) + pBitmap->RenderSizeY);

			if (lpSize->cy < metrics)
			{
				lpSize->cy = metrics;
			}

			lpSize->cx += pBitmap->advance;
		}
	}

	return 0;
}

BOOL CGMFontLayer::_GetTextExtentPoint32(LPCSTR lpString, int cbString, LPSIZE lpSize)
{
	std::wstring wstrText = L"";
	g_pMultiLanguage->ConvertCharToWideStr(wstrText, lpString);
	_GetTextExtentPoint32(wstrText, lpSize);
	return 0;
}

void CGMFontLayer::_TextOut(std::wstring wstrText, int& pen_x, int& pen_y)
{
	int Next_y = 0;
	int Next_x = 0;
	int max_size = 0;
	const int LIMIT_WIDTH = 512, LIMIT_HEIGHT = 32;

	max_size = (LIMIT_WIDTH * LIMIT_HEIGHT);

	hFontBuffer.clear();
	hFontBuffer.resize(max_size, 0);

	for (auto wstr = wstrText.begin(); wstr != wstrText.end(); wstr++)
	{
		_FT_Bitmap* pBitmap;

		if (*wstr == 32)
			pBitmap = GetULongChar('|');
		else
			pBitmap = GetULongChar(*wstr);

		if (!pBitmap)
			pBitmap = GetULongChar(1);

		if (!pBitmap)
			continue;

		if (*wstr != 32)
		{
			runtime_writebuffer(Next_x, Next_y, pBitmap);
		}

		int metrics = ((NormalFont[pBitmap->FontType].metrics_height - pBitmap->bitmap_top) + pBitmap->RenderSizeY);

		if (pen_y < metrics)
		{
			pen_y = metrics;
		}

		Next_x += pBitmap->advance;
	}

	pen_x = Next_x;

	glBindTexture(GL_TEXTURE_2D, BitmapFontIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, LIMIT_WIDTH, LIMIT_HEIGHT, 0, GL_ALPHA, GL_UNSIGNED_BYTE, hFontBuffer.data());
}

void CGMFontLayer::runtime_writebuffer(int off_x, int off_y, _FT_Bitmap* pBitmap)
{
	const int LIMIT_WIDTH = 512, LIMIT_HEIGHT = 32;

	off_x += pBitmap->bitmap_left;
	off_y += (NormalFont[pBitmap->FontType].metrics_height - pBitmap->bitmap_top);

	for (FT_UInt row = 0; row < (FT_UInt)pBitmap->RenderSizeY; ++row)
	{
		for (FT_UInt col = 0; col < (FT_UInt)pBitmap->RenderSizeX; ++col)
		{
			int x = off_x + col;
			int y = off_y + row;

			if (x < 0 || x >= LIMIT_WIDTH || y < 0 || y >= LIMIT_HEIGHT)
			{
				continue;
			}

			hFontBuffer[y * LIMIT_WIDTH + x] = NormalFont[pBitmap->FontType].PakBuffer[(pBitmap->RenderPosY + row) * NormalFont[pBitmap->FontType].output_width + (pBitmap->RenderPosX + col)];
		}
	}
}

void CGMFontLayer::runtime_render_map(int pen_x, int pen_y, int RealTextX, int RealTextY, int Width, int Height, bool background)
{
	const float LIMIT_WIDTH = 512, LIMIT_HEIGHT = 32;

	DWORD dwTextColor = g_pRenderText->GetTextColor();

	if (dwTextColor != 0)
	{
		EnableAlphaTest();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

		if (background)
		{
			glColor4f(0.0, 0.0, 0.0, 1.0);
			RenderBitmap(-((int)BitmapFontIndex), pen_x + 1, pen_y + 1, RealTextX, RealTextY, 0.0, 0.0, Width / LIMIT_WIDTH, Height / LIMIT_HEIGHT, false, false);
			glColor4ub(GetRed(dwTextColor), GetGreen(dwTextColor), GetBlue(dwTextColor), GetAlpha(dwTextColor));
			RenderBitmap(-((int)BitmapFontIndex), pen_x, pen_y, RealTextX, RealTextY, 0.0, 0.0, Width / LIMIT_WIDTH, Height / LIMIT_HEIGHT, false, false);
		}
		else
		{
			glColor4ub(GetRed(dwTextColor), GetGreen(dwTextColor), GetBlue(dwTextColor), GetAlpha(dwTextColor));
		}
		RenderBitmap(-((int)BitmapFontIndex), pen_x, pen_y, RealTextX, RealTextY, 0.0, 0.0, Width / LIMIT_WIDTH, Height / LIMIT_HEIGHT, false, false);
	}
}

void CGMFontLayer::RenderText(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth, int iHeight, int iSort, OUT SIZE* lpTextSize, bool background)
{
	if (pszText == NULL || (pszText[0] == '\0' && iHeight == 0))
		return;

	if (strlen(pszText) <= 0 && iHeight == 0)
		return;

	SIZE RealTextSize;

	std::wstring wstrText = L"";

	g_pMultiLanguage->ConvertCharToWideStr(wstrText, pszText);

	for (size_t i = 0; i < wstrText.size(); i++)
	{
		wstrText[i] = g_pMultiLanguage->ConvertFulltoHalfWidthChar(wstrText[i]);
	}

	_GetTextExtentPoint32(wstrText, &RealTextSize);

	POINT RealBoxPos = {iPos_x, iPos_y };
	SIZE RealBoxSize = {iWidth, iHeight };
	SIZE RealRenderingSize = { RealTextSize.cx, RealTextSize.cy };

	if (RealBoxSize.cx == 0)
		RealBoxSize.cx = RealTextSize.cx;

	if (RealBoxSize.cy == 0)
		RealBoxSize.cy = RealTextSize.cy;
	else
		RealBoxPos.y += ((RealBoxSize.cy - RealTextSize.cy) * 0.5);

	int iTab = 0;
	int iClipMove = 0;

	if (iSort == RT3_SORT_LEFT_CLIP)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
		{
			iClipMove = RealRenderingSize.cx - RealBoxSize.cx;
			RealRenderingSize.cx = RealBoxSize.cx;
		}
	}
	else if (iSort == RT3_SORT_LEFT)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
			RealRenderingSize.cx = RealBoxSize.cx;
	}
	else if (iSort == RT3_SORT_CENTER)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
		{
			iClipMove = (RealRenderingSize.cx - RealBoxSize.cx) / 2;
			RealRenderingSize.cx = RealBoxSize.cx;
		}
		else
		{
			iTab = (RealBoxSize.cx - RealRenderingSize.cx) / 2;
		}
	}
	else if (iSort == RT3_SORT_RIGHT)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
		{
			iClipMove = RealRenderingSize.cx - RealBoxSize.cx;
			RealRenderingSize.cx = RealBoxSize.cx;
		}
		else
		{
			iTab = RealBoxSize.cx - RealRenderingSize.cx;
		}
	}
	else if (iSort == RT3_WRITE_RIGHT_TO_LEFT)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
		{
			iClipMove = RealRenderingSize.cx - RealBoxSize.cx;
			RealRenderingSize.cx = RealBoxSize.cx;
		}
		else
		{
			iTab = RealBoxSize.cx - RealRenderingSize.cx;
		}
		RealBoxPos.x -= RealBoxSize.cx;
	}
	else if (iSort == RT3_WRITE_CENTER)
	{
		if (RealRenderingSize.cx > RealBoxSize.cx)
		{
			iClipMove = (RealRenderingSize.cx - RealBoxSize.cx) / 2;
			RealRenderingSize.cx = RealBoxSize.cx;
		}
		else
		{
			iTab = (RealBoxSize.cx - RealRenderingSize.cx) / 2;
		}
		RealBoxPos.x -= (RealBoxSize.cx / 2);
	}

	int Next_y = 0;
	int Next_x = 0;

	_TextOut(wstrText, Next_x, Next_y);

	GLfloat fCurColor[4];
	glGetFloatv(GL_CURRENT_COLOR, fCurColor);

	runtime_render_map(RealBoxPos.x, RealBoxPos.y, Next_x, Next_y, Next_x, Next_y, background);

	glColor4fv(fCurColor);

	if (lpTextSize)
	{
		lpTextSize->cx = RealBoxPos.x;
		lpTextSize->cy = RealBoxPos.y;
	}
}

void CGMFontLayer::RenderWave(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth, int iHeight, int iSort, OUT SIZE* lpTextSize)
{
	char filter[10];
	char bufer[255];
	GLfloat fCurColor[4];
	DWORD dwTextColor = g_pRenderText->GetTextColor();
	glGetFloatv(GL_CURRENT_COLOR, fCurColor);

	SIZE TextSize;
	g_pRenderText->SetTextColor(0);

	this->RenderText(iPos_x, iPos_y, pszText, iWidth, iHeight, iSort, &TextSize, true);

	if (lpTextSize)
	{
		lpTextSize->cx = TextSize.cx;
		lpTextSize->cy = TextSize.cy;
	}

	int Size = strlen(pszText);

	iPos_x = TextSize.cx;
	for (BYTE i = 0; i < Size; i++)
	{
		DWORD Color = g_pRenderText->GenerateSinWaveColor(0.5f, 0.5f, 0.5f, 0, 2, 4, 128, 127, (i + GetTickCount() / 100 % 255));

		g_pRenderText->SetTextColor(Color);
		sprintf(filter, "%%.%ds", Size - i);
		sprintf(bufer, filter, pszText);

		this->RenderText(iPos_x, iPos_y, bufer, iWidth, iHeight, RT3_SORT_LEFT, NULL, i == 0);
	}

	glColor4fv(fCurColor);
	g_pRenderText->SetTextColor(dwTextColor);
}

void CGMFontLayer::runtime_render_map()
{
	//RenderColor(0, 100, 2048, 512.f, 0.9, 1, false);
	//EndRenderColor();
	//RenderBitmap(-((int)BitmapIndex), 0, 100, 2048, 512.f, 0.0, 0.0, 1.f, 512.f /2048.f, false, false);
}

int CGMFontLayer::getmetrics()
{
	return NormalFont[0].metrics_height;
}

CGMFontLayer* CGMFontLayer::Instance()
{
	static CGMFontLayer s_Instance;
	return &s_Instance;
}
