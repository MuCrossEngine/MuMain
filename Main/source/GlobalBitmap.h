// GlobalBitmap.h: interface for the CGlobalBitmap class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#pragma warning(disable : 4786)

#include <map>
#include <deque>
#include <string>
#include <setjmp.h>
#include "./Time/Timer.h"

#define MAX_BITMAP_FILE_NAME 256

#pragma pack(push, 1)
typedef struct
{
	GLuint	BitmapIndex;
	char	FileName[MAX_BITMAP_FILE_NAME];
	float	Width;
	float	Height;
	unsigned int output_width;
	unsigned int output_height;
	BYTE	Components;
	GLuint	TextureNumber;
	BYTE	Ref;
	std::vector<BYTE> Buffer;
	//BYTE*   Buffer;
private:
	friend class CBitmapCache;
	DWORD	dwCallCount;
} BITMAP_t;
#pragma pack(pop)

class CBitmapCache
{
	enum
	{
		QUICK_CACHE_MAPTILE = 0,
		QUICK_CACHE_MAPGRASS,
		QUICK_CACHE_WATER,
		QUICK_CACHE_CURSOR,
		QUICK_CACHE_FONT,
		QUICK_CACHE_MAINFRAME,
		QUICK_CACHE_SKILLICON,
		QUICK_CACHE_PLAYER,

		NUMBER_OF_QUICK_CACHE,
	};

	typedef struct _QUICK_CACHE
	{
		DWORD		dwBitmapIndexMin;
		DWORD		dwBitmapIndexMax;
		DWORD		dwRange;
		BITMAP_t** ppBitmap;
	} QUICK_CACHE;
	typedef std::map<GLuint, BITMAP_t*, std::less<GLuint> > type_cache_map;

	type_cache_map		m_mapCacheMain;
	type_cache_map		m_mapCachePlayer;
	type_cache_map		m_mapCacheInterface;
	type_cache_map		m_mapCacheEffect;

	QUICK_CACHE			m_QuickCache[NUMBER_OF_QUICK_CACHE];
	BITMAP_t* m_pNullBitmap;

	CTimer2				m_ManageTimer;

public:
	CBitmapCache();
	~CBitmapCache();

	bool Create();
	void Release();

	void Add(GLuint uiBitmapIndex, BITMAP_t* pBitmap);
	void Remove(GLuint uiBitmapIndex);
	void RemoveAll();

	size_t GetCacheSize();

	void Update();

	bool Find(GLuint uiBitmapIndex, BITMAP_t** ppBitmap);
};

class CGlobalBitmap
{
	enum
	{
#if MAIN_UPDATE > 603
		MAX_WIDTH = 2048,
		MAX_HEIGHT = 2048,
#else
		MAX_WIDTH = 1024,
		MAX_HEIGHT = 1024,
#endif
	};
	typedef std::map<GLuint, BITMAP_t*, std::less<GLuint> >	type_bitmap_map;
	typedef std::list<GLuint> type_index_list;
	type_bitmap_map	m_mapBitmap;

	type_index_list m_listNonamedIndex;
	GLuint	m_uiAlternate, m_uiTextureIndexStream;
	long long	m_dwUsedTextureMemory;
	CBitmapCache	m_BitmapCache;
#ifdef DEBUG_BITMAP_CACHE
	CTimer2				m_DebugOutputTimer;
#endif // DEBUG_BITMAP_CACHE

	std::vector<std::pair<std::string, GLuint>> m_BitmapName;
	//std::map<std::string, GLuint> m_BitmapName;


	void Init();
public:
	CGlobalBitmap();
	virtual ~CGlobalBitmap();

	GLuint LoadImage(const std::string& filename, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE);
	bool LoadImage(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE);

	void UnloadImage(GLuint uiBitmapIndex, bool bForce = false);
	void UnloadAllImages();

	BITMAP_t* GetTexture(GLuint uiBitmapIndex);
	BITMAP_t* FindTexture(GLuint uiBitmapIndex);
	BITMAP_t* FindTexture(const std::string& filename);
	BITMAP_t* FindTextureByName(const std::string& name);

	DWORD GetUsedTextureMemory() const;
	size_t GetNumberOfTexture() const;

	bool ConvertFormat(const unicode::t_string& filename);

	void Manage();

	inline BITMAP_t& operator [] (GLuint uiBitmapIndex) { return *GetTexture(uiBitmapIndex); }


	int findByFullPath(const std::string& fullPath);
	int findByFileName(const std::string& name);
protected:
	GLuint GenerateTextureIndex();
	GLuint FindAvailableTextureIndex(GLuint uiSeed);

	bool OpenJpeg(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE);
	bool OpenTga(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter = GL_NEAREST, GLuint uiWrapMode = GL_CLAMP_TO_EDGE);
	bool OpenDDS_DXT5(GLuint uiBitmapIndex, const std::string& filename);

	void CreateMipmappedTexture(GLuint* TextureNumber, GLuint Components, GLuint Width, GLuint Height, BYTE* textureBuff, GLuint uiFilter, GLuint uiWrapMode);
	//void CreateMipmappedTexture(BITMAP_t* pNewBitmap, GLuint uiFilter, GLuint uiWrapMode);

	void SplitFileName(IN const std::string& filepath, OUT std::string& filename, bool bIncludeExt);
	void SplitExt(IN const std::string& filepath, OUT std::string& ext, bool bIncludeDot);
	void ExchangeExt(IN const std::string& in_filepath, IN const std::string& ext, OUT std::string& out_filepath);
	bool Save_Image(const unicode::t_string& src, const unicode::t_string& dest, int cDumpHeader);
	bool removeByFullPath(const std::string& fullPath);

	int leftoverSize(int Size);
};

extern CGlobalBitmap Bitmaps;