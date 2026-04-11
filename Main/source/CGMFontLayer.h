#pragma once
#define MAX_LINE_FONT			3
#define CHARSET_DEFAULT			1
#define CHARSET_GB2312			2
#define CHARSET_HANGUL			3

class _FT_Bitmap
{
public:
	_FT_Bitmap() : load_glyph(false)
	{
		FontType = 0;
		RenderPosX = 0;
		RenderPosY = 0;
		RenderSizeX = 0;
		RenderSizeY = 0;
		bitmap_left = 0;
		bitmap_top = 0;
		advance = 0;
	}
	int FontType;
	int RenderPosX;
	int RenderPosY;
	int RenderSizeX;
	int RenderSizeY;
	int bitmap_left;
	int bitmap_top;
	int advance;
	bool load_glyph;
};

class BitmapFont
{
public:
	BitmapFont() {
		BitmapIndex = -1;
		output_width = 1;
		output_hight = 1;
		metrics_height = 1;
	};

	FT_UInt BitmapIndex;
	FT_UInt output_width;
	FT_UInt output_hight;
	FT_UInt metrics_height;
	std::vector<unsigned char> PakBuffer;
};

typedef std::map<FT_ULong, _FT_Bitmap> type_map_bitmap;

class CGMFontLayer
{
public:
	CGMFontLayer();
	virtual~CGMFontLayer();
	_FT_Bitmap* GetULongChar(FT_ULong charcode);
	void runtime_load_bitmap(GLuint* textures, GLsizei _width, GLsizei _height, BYTE* data);
	void runtime_font_property(HFONT hFont, int PixelSize);
	void runtime_font_property(HDC hdc, HFONT hFont, DWORD dwTable, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize);
	void runtime_font_property(const char* file_base, FT_Library library, BitmapFont* FontType, int FontIndex, int PixelSize, FT_Encoding encoding = FT_ENCODING_UNICODE);
	BOOL _GetTextExtentPoint32(std::wstring wstrText, LPSIZE lpSize);
	BOOL _GetTextExtentPoint32(LPCSTR lpString, int cbString, LPSIZE lpSize);
	void _TextOut(std::wstring wstrText, int& pen_x, int& pen_y);

	void runtime_writebuffer(int off_x, int off_y, _FT_Bitmap* pBitmap);
	void runtime_render_map(int pen_x, int pen_y, int RealTextX, int RealTextY, int Width, int Height, bool background = true);
	void RenderText(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth = 0, int iHeight = 0, int iSort = RT3_SORT_LEFT, OUT SIZE* lpTextSize = NULL, bool background = true);
	void RenderWave(int iPos_x, int iPos_y, const unicode::t_char* pszText, int iWidth = 0, int iHeight = 0, int iSort = RT3_SORT_LEFT, OUT SIZE* lpTextSize = NULL);

	void runtime_render_map();

	int getmetrics();
	static CGMFontLayer* Instance();
private:
	BitmapFont NormalFont[MAX_LINE_FONT];
	FT_UInt BitmapFontIndex;

	std::vector<unsigned char> hFontBuffer;


	type_map_bitmap Characters;
};

#define GMFontLayer			(CGMFontLayer::Instance())