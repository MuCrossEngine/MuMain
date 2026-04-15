// GlobalBitmap.cpp: implementation of the CGlobalBitmap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <turbojpeg/turbojpeg.h>
#define STB_IMAGE_IMPLEMENTATION
#include <turbojpeg/stb_image.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <gli/gli.hpp>

#include "GlobalBitmap.h"
#include "Platform/GameAssetPath.h"
#include "./Utilities/Log/ErrorReport.h"
#include "./Utilities/Log/muConsoleDebug.h"

CBitmapCache::CBitmapCache()
{
	memset(m_QuickCache, 0, sizeof(QUICK_CACHE) * NUMBER_OF_QUICK_CACHE);
}
CBitmapCache::~CBitmapCache() { Release(); }

bool CBitmapCache::Create()
{
	Release();

	DWORD dwRange = 0;

	dwRange = BITMAP_MAPTILE_END - BITMAP_MAPTILE_BEGIN;
	m_QuickCache[QUICK_CACHE_MAPTILE].dwBitmapIndexMin = BITMAP_MAPTILE_BEGIN;
	m_QuickCache[QUICK_CACHE_MAPTILE].dwBitmapIndexMax = BITMAP_MAPTILE_END;
	m_QuickCache[QUICK_CACHE_MAPTILE].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_MAPTILE].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_MAPTILE].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_MAPGRASS_END - BITMAP_MAPGRASS_BEGIN;
	m_QuickCache[QUICK_CACHE_MAPGRASS].dwBitmapIndexMin = BITMAP_MAPGRASS_BEGIN;
	m_QuickCache[QUICK_CACHE_MAPGRASS].dwBitmapIndexMax = BITMAP_MAPGRASS_END;
	m_QuickCache[QUICK_CACHE_MAPGRASS].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_MAPGRASS].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_MAPGRASS].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_WATER_END - BITMAP_WATER_BEGIN;
	m_QuickCache[QUICK_CACHE_WATER].dwBitmapIndexMin = BITMAP_WATER_BEGIN;
	m_QuickCache[QUICK_CACHE_WATER].dwBitmapIndexMax = BITMAP_WATER_END;
	m_QuickCache[QUICK_CACHE_WATER].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_WATER].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_WATER].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_CURSOR_END - BITMAP_CURSOR_BEGIN;
	m_QuickCache[QUICK_CACHE_CURSOR].dwBitmapIndexMin = BITMAP_CURSOR_BEGIN;
	m_QuickCache[QUICK_CACHE_CURSOR].dwBitmapIndexMax = BITMAP_CURSOR_END;
	m_QuickCache[QUICK_CACHE_CURSOR].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_CURSOR].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_CURSOR].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_FONT_END - BITMAP_FONT_BEGIN;
	m_QuickCache[QUICK_CACHE_FONT].dwBitmapIndexMin = BITMAP_FONT_BEGIN;
	m_QuickCache[QUICK_CACHE_FONT].dwBitmapIndexMax = BITMAP_FONT_END;
	m_QuickCache[QUICK_CACHE_FONT].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_FONT].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_FONT].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_INTERFACE_NEW_MAINFRAME_END - BITMAP_INTERFACE_NEW_MAINFRAME_BEGIN;
	m_QuickCache[QUICK_CACHE_MAINFRAME].dwBitmapIndexMin = BITMAP_INTERFACE_NEW_MAINFRAME_BEGIN;
	m_QuickCache[QUICK_CACHE_MAINFRAME].dwBitmapIndexMax = BITMAP_INTERFACE_NEW_MAINFRAME_END;
	m_QuickCache[QUICK_CACHE_MAINFRAME].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_MAINFRAME].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_MAINFRAME].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_INTERFACE_NEW_SKILLICON_END - BITMAP_INTERFACE_NEW_SKILLICON_BEGIN;
	m_QuickCache[QUICK_CACHE_SKILLICON].dwBitmapIndexMin = BITMAP_INTERFACE_NEW_SKILLICON_BEGIN;
	m_QuickCache[QUICK_CACHE_SKILLICON].dwBitmapIndexMax = BITMAP_INTERFACE_NEW_SKILLICON_END;
	m_QuickCache[QUICK_CACHE_SKILLICON].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_SKILLICON].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_SKILLICON].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	dwRange = BITMAP_PLAYER_TEXTURE_END - BITMAP_PLAYER_TEXTURE_BEGIN;
	m_QuickCache[QUICK_CACHE_PLAYER].dwBitmapIndexMin = BITMAP_PLAYER_TEXTURE_BEGIN;
	m_QuickCache[QUICK_CACHE_PLAYER].dwBitmapIndexMax = BITMAP_PLAYER_TEXTURE_END;
	m_QuickCache[QUICK_CACHE_PLAYER].dwRange = dwRange;
	m_QuickCache[QUICK_CACHE_PLAYER].ppBitmap = new BITMAP_t * [dwRange];
	memset(m_QuickCache[QUICK_CACHE_PLAYER].ppBitmap, 0, dwRange * sizeof(BITMAP_t*));

	m_pNullBitmap = new BITMAP_t;
	memset(m_pNullBitmap, 0, sizeof(BITMAP_t));
	m_ManageTimer.SetTimer(1500);

	return true;
}
void CBitmapCache::Release()
{
	SAFE_DELETE(m_pNullBitmap);

	RemoveAll();

	for (int i = 0; i < NUMBER_OF_QUICK_CACHE; i++)
	{
		if (m_QuickCache[i].ppBitmap)
			delete[] m_QuickCache[i].ppBitmap;
	}
	memset(m_QuickCache, 0, sizeof(QUICK_CACHE) * NUMBER_OF_QUICK_CACHE);
}

void CBitmapCache::Add(GLuint uiBitmapIndex, BITMAP_t* pBitmap)
{
	for (int i = 0; i < NUMBER_OF_QUICK_CACHE; i++)
	{
		if (uiBitmapIndex > m_QuickCache[i].dwBitmapIndexMin && uiBitmapIndex < m_QuickCache[i].dwBitmapIndexMax)
		{
			DWORD dwVI = uiBitmapIndex - m_QuickCache[i].dwBitmapIndexMin;
			if (pBitmap)
				m_QuickCache[i].ppBitmap[dwVI] = pBitmap;
			else
				m_QuickCache[i].ppBitmap[dwVI] = m_pNullBitmap;
			return;
		}
	}
	if (pBitmap)
	{
		if (BITMAP_PLAYER_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_PLAYER_TEXTURE_END >= uiBitmapIndex)
			m_mapCachePlayer.insert(type_cache_map::value_type(uiBitmapIndex, pBitmap));
		else if (BITMAP_INTERFACE_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_INTERFACE_TEXTURE_END >= uiBitmapIndex)
			m_mapCacheInterface.insert(type_cache_map::value_type(uiBitmapIndex, pBitmap));
		else if (BITMAP_EFFECT_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_EFFECT_TEXTURE_END >= uiBitmapIndex)
			m_mapCacheEffect.insert(type_cache_map::value_type(uiBitmapIndex, pBitmap));
		else
			m_mapCacheMain.insert(type_cache_map::value_type(uiBitmapIndex, pBitmap));
	}
}
void CBitmapCache::Remove(GLuint uiBitmapIndex)
{
	for (int i = 0; i < NUMBER_OF_QUICK_CACHE; i++)
	{
		if (uiBitmapIndex > m_QuickCache[i].dwBitmapIndexMin && uiBitmapIndex < m_QuickCache[i].dwBitmapIndexMax)
		{
			DWORD dwVI = uiBitmapIndex - m_QuickCache[i].dwBitmapIndexMin;
			m_QuickCache[i].ppBitmap[dwVI] = NULL;
			return;
		}
	}

	if (BITMAP_PLAYER_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_PLAYER_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCachePlayer.find(uiBitmapIndex);
		if (mi != m_mapCachePlayer.end())
			m_mapCachePlayer.erase(mi);
	}
	else if (BITMAP_INTERFACE_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_INTERFACE_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCacheInterface.find(uiBitmapIndex);
		if (mi != m_mapCacheInterface.end())
			m_mapCacheInterface.erase(mi);
	}
	else if (BITMAP_EFFECT_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_EFFECT_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCacheEffect.find(uiBitmapIndex);
		if (mi != m_mapCacheEffect.end())
			m_mapCacheEffect.erase(mi);
	}
	else
	{
		type_cache_map::iterator mi = m_mapCacheMain.find(uiBitmapIndex);
		if (mi != m_mapCacheMain.end())
			m_mapCacheMain.erase(mi);
	}
}
void CBitmapCache::RemoveAll()
{
	for (int i = 0; i < NUMBER_OF_QUICK_CACHE; i++)
	{
		memset(m_QuickCache[i].ppBitmap, 0, m_QuickCache[i].dwRange * sizeof(BITMAP_t*));
	}
	m_mapCacheMain.clear();
	m_mapCachePlayer.clear();
	m_mapCacheInterface.clear();
	m_mapCacheEffect.clear();
}

size_t CBitmapCache::GetCacheSize()
{
	return m_mapCacheMain.size() + m_mapCachePlayer.size() +
		m_mapCacheInterface.size() + m_mapCacheEffect.size();
}

void CBitmapCache::Update()
{
	m_ManageTimer.UpdateTime();

	if (m_ManageTimer.IsTime())
	{
		type_cache_map::iterator mi = m_mapCacheMain.begin();
		for (; mi != m_mapCacheMain.end(); )
		{
			BITMAP_t* pBitmap = (*mi).second;
			if (pBitmap->dwCallCount > 0)
			{
				pBitmap->dwCallCount = 0;
				mi++;
			}
			else
			{
				mi = m_mapCacheMain.erase(mi);
			}
		}

		mi = m_mapCachePlayer.begin();
		for (; mi != m_mapCachePlayer.end(); )
		{
			BITMAP_t* pBitmap = (*mi).second;

			if (pBitmap->dwCallCount > 0)
			{
				pBitmap->dwCallCount = 0;
				mi++;
			}
			else
			{
				mi = m_mapCachePlayer.erase(mi);
			}
		}

		mi = m_mapCacheInterface.begin();
		for (; mi != m_mapCacheInterface.end(); )
		{
			BITMAP_t* pBitmap = (*mi).second;
			if (pBitmap->dwCallCount > 0)
			{
				pBitmap->dwCallCount = 0;
				mi++;
			}
			else
			{
				mi = m_mapCacheInterface.erase(mi);
			}
		}

		mi = m_mapCacheEffect.begin();
		for (; mi != m_mapCacheEffect.end(); )
		{
			BITMAP_t* pBitmap = (*mi).second;
			if (pBitmap->dwCallCount > 0)
			{
				pBitmap->dwCallCount = 0;
				mi++;
			}
			else
			{
				mi = m_mapCacheEffect.erase(mi);
			}
		}

#ifdef DEBUG_BITMAP_CACHE
		g_ConsoleDebug->Write(MCD_NORMAL, "M,P,I,E : (%d, %d, %d, %d)", m_mapCacheMain.size(),
			m_mapCachePlayer.size(), m_mapCacheInterface.size(), m_mapCacheEffect.size());
#endif // DEBUG_BITMAP_CACHE
	}
}

bool CBitmapCache::Find(GLuint uiBitmapIndex, BITMAP_t** ppBitmap)
{
	for (int i = 0; i < NUMBER_OF_QUICK_CACHE; i++)
	{
		if (uiBitmapIndex > m_QuickCache[i].dwBitmapIndexMin &&
			uiBitmapIndex < m_QuickCache[i].dwBitmapIndexMax)
		{
			DWORD dwVI = uiBitmapIndex - m_QuickCache[i].dwBitmapIndexMin;
			if (m_QuickCache[i].ppBitmap[dwVI])
			{
				if (m_QuickCache[i].ppBitmap[dwVI] == m_pNullBitmap)
					*ppBitmap = NULL;
				else
				{
					*ppBitmap = m_QuickCache[i].ppBitmap[dwVI];
				}
				return true;
			}
			return false;
		}
	}

	if (BITMAP_PLAYER_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_PLAYER_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCachePlayer.find(uiBitmapIndex);
		if (mi != m_mapCachePlayer.end())
		{
			*ppBitmap = (*mi).second;
			(*ppBitmap)->dwCallCount++;
			return true;
		}
	}
	else if (BITMAP_INTERFACE_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_INTERFACE_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCacheInterface.find(uiBitmapIndex);
		if (mi != m_mapCacheInterface.end())
		{
			*ppBitmap = (*mi).second;
			(*ppBitmap)->dwCallCount++;
			return true;
		}
	}
	else if (BITMAP_EFFECT_TEXTURE_BEGIN <= uiBitmapIndex && BITMAP_EFFECT_TEXTURE_END >= uiBitmapIndex)
	{
		type_cache_map::iterator mi = m_mapCacheEffect.find(uiBitmapIndex);
		if (mi != m_mapCacheEffect.end())
		{
			*ppBitmap = (*mi).second;
			(*ppBitmap)->dwCallCount++;
			return true;
		}
	}
	else
	{
		type_cache_map::iterator mi = m_mapCacheMain.find(uiBitmapIndex);
		if (mi != m_mapCacheMain.end())
		{
			*ppBitmap = (*mi).second;
			(*ppBitmap)->dwCallCount++;
			return true;
		}
	}
	return false;
}

CGlobalBitmap::CGlobalBitmap()
{
	Init();
	m_BitmapCache.Create();

#ifdef DEBUG_BITMAP_CACHE
	m_DebugOutputTimer.SetTimer(5000);
#endif // DEBUG_BITMAP_CACHE
}
CGlobalBitmap::~CGlobalBitmap()
{
	UnloadAllImages();
}
void CGlobalBitmap::Init()
{
	m_uiAlternate = 0;
	m_uiTextureIndexStream = BITMAP_NONAMED_TEXTURES_BEGIN;
	m_dwUsedTextureMemory = 0;
}

GLuint CGlobalBitmap::LoadImage(const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	BITMAP_t* pBitmap = FindTexture(filename);

	if (pBitmap)
	{
		if (pBitmap->Ref > 0)
		{
			if (0 == _stricmp(pBitmap->FileName, filename.c_str()))
			{
				pBitmap->Ref++;

				return pBitmap->BitmapIndex;
			}
		}
	}
	else
	{
		GLuint uiNewTextureIndex = GenerateTextureIndex();
		if (true == LoadImage(uiNewTextureIndex, filename, uiFilter, uiWrapMode))
		{
			m_listNonamedIndex.push_back(uiNewTextureIndex);

			return uiNewTextureIndex;
		}
	}
	return BITMAP_UNKNOWN;
}

bool CGlobalBitmap::LoadImage(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	unsigned int UICLAMP = GL_CLAMP_TO_EDGE;
	unsigned int UIREPEAT = GL_REPEAT;

	if (uiWrapMode != UICLAMP && uiWrapMode != UIREPEAT)
	{
#ifdef _DEBUG
		static unsigned int	uiCnt2 = 0;
		int			iBuff;	iBuff = 0;

		char		szDebugOutput[256];

		iBuff = iBuff + sprintf(iBuff + szDebugOutput, "%d. Call No CLAMP & No REPEAT. \n", uiCnt2++);
		OutputDebugString(szDebugOutput);
#endif
	}

	type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);

	if (mi != m_mapBitmap.end())
	{
		BITMAP_t* pBitmap = (*mi).second;
		if (pBitmap->Ref > 0)
		{
			if (0 == _stricmp(pBitmap->FileName, filename.c_str()))
			{
				pBitmap->Ref++;
				return true;
			}
			else
			{
				g_ErrorReport.Write("File not found %s (%d)->%s\r\n", pBitmap->FileName, uiBitmapIndex, filename.c_str());
				UnloadImage(uiBitmapIndex, true);
			}
		}
	}

	std::string ext;
	SplitExt(filename, ext, false);

	if (0 == _stricmp(ext.c_str(), "jpg"))
		return OpenJpeg(uiBitmapIndex, filename, uiFilter, uiWrapMode);
	else if (0 == _stricmp(ext.c_str(), "tga"))
		return OpenTga(uiBitmapIndex, filename, uiFilter, uiWrapMode);

	return false;
}

void CGlobalBitmap::UnloadImage(GLuint uiBitmapIndex, bool bForce)
{
	type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);

	if (mi != m_mapBitmap.end())
	{
		BITMAP_t* pBitmap = (*mi).second;

		if (--pBitmap->Ref == 0 || bForce)
		{
			glDeleteTextures(1, &(pBitmap->TextureNumber));

			m_dwUsedTextureMemory -= (DWORD)(pBitmap->Width * pBitmap->Height * pBitmap->Components);

			pBitmap->Buffer.clear();
			removeByFullPath(pBitmap->FileName);

			delete pBitmap;
			m_mapBitmap.erase(mi);

			if (uiBitmapIndex >= BITMAP_NONAMED_TEXTURES_BEGIN && uiBitmapIndex <= BITMAP_NONAMED_TEXTURES_END)
			{
				m_listNonamedIndex.remove(uiBitmapIndex);
			}
			m_BitmapCache.Remove(uiBitmapIndex);
		}
	}
}
void CGlobalBitmap::UnloadAllImages()
{
	type_bitmap_map::iterator mi = m_mapBitmap.begin();
	for (; mi != m_mapBitmap.end(); mi++)
	{
		BITMAP_t* pBitmap = (*mi).second;
		pBitmap->Buffer.clear();
		delete pBitmap;
	}

	m_mapBitmap.clear();
	m_BitmapName.clear();
	m_listNonamedIndex.clear();
	m_BitmapCache.RemoveAll();
	Init();
}

BITMAP_t* CGlobalBitmap::GetTexture(GLuint uiBitmapIndex)
{
	BITMAP_t* pBitmap = NULL;
	if (false == m_BitmapCache.Find(uiBitmapIndex, &pBitmap))
	{
		type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);
		if (mi != m_mapBitmap.end())
			pBitmap = (*mi).second;
		m_BitmapCache.Add(uiBitmapIndex, pBitmap);
	}
	if (NULL == pBitmap)
	{
		static BITMAP_t s_Error;
		memset(&s_Error, 0, sizeof(BITMAP_t));
		strcpy(s_Error.FileName, "CGlobalBitmap::GetTexture Error!!!");
		pBitmap = &s_Error;
	}
	return pBitmap;
}

BITMAP_t* CGlobalBitmap::FindTexture(GLuint uiBitmapIndex)
{
	BITMAP_t* pBitmap = NULL;
	if (false == m_BitmapCache.Find(uiBitmapIndex, &pBitmap))
	{
		type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);
		if (mi != m_mapBitmap.end())
			pBitmap = (*mi).second;
		if (pBitmap != NULL)
			m_BitmapCache.Add(uiBitmapIndex, pBitmap);
	}
	return pBitmap;
}

BITMAP_t* CGlobalBitmap::FindTexture(const std::string& filename)
{
	BITMAP_t* pBitmap = NULL;
	GLuint uiBitmapIndex = findByFullPath(filename);

	if (uiBitmapIndex != 0)
	{
		type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);
		if (mi != m_mapBitmap.end())
			pBitmap = (*mi).second;
	}

	return pBitmap;
}

BITMAP_t* CGlobalBitmap::FindTextureByName(const std::string& name)
{
	BITMAP_t* pBitmap = NULL;
	GLuint uiBitmapIndex = findByFileName(name);

	if (uiBitmapIndex != 0)
	{
		type_bitmap_map::iterator mi = m_mapBitmap.find(uiBitmapIndex);
		if (mi != m_mapBitmap.end())
			pBitmap = (*mi).second;
	}

	return pBitmap;
}

DWORD CGlobalBitmap::GetUsedTextureMemory() const
{
	return m_dwUsedTextureMemory;
}

size_t CGlobalBitmap::GetNumberOfTexture() const
{
	return m_mapBitmap.size();
}

void CGlobalBitmap::Manage()
{
#ifdef DEBUG_BITMAP_CACHE
	m_DebugOutputTimer.UpdateTime();
	if (m_DebugOutputTimer.IsTime())
	{
		g_ConsoleDebug->Write(MCD_NORMAL, "CacheSize=%d(NumberOfTexture=%d)", m_BitmapCache.GetCacheSize(), GetNumberOfTexture());
	}
#endif // DEBUG_BITMAP_CACHE
	m_BitmapCache.Update();
}

GLuint CGlobalBitmap::GenerateTextureIndex()
{
	GLuint uiAvailableTextureIndex = FindAvailableTextureIndex(m_uiTextureIndexStream);
	if (uiAvailableTextureIndex >= BITMAP_NONAMED_TEXTURES_END)
	{
		m_uiAlternate++;
		m_uiTextureIndexStream = BITMAP_NONAMED_TEXTURES_BEGIN;
		uiAvailableTextureIndex = FindAvailableTextureIndex(m_uiTextureIndexStream);
	}
	return m_uiTextureIndexStream = uiAvailableTextureIndex;
}

GLuint CGlobalBitmap::FindAvailableTextureIndex(GLuint uiSeed)
{
	if (m_uiAlternate > 0)
	{
		type_index_list::iterator li = std::find(m_listNonamedIndex.begin(), m_listNonamedIndex.end(), uiSeed + 1);
		if (li != m_listNonamedIndex.end())
			return FindAvailableTextureIndex(uiSeed + 1);
	}
	return uiSeed + 1;
}

bool CGlobalBitmap::OpenJpeg(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	std::string filename_ozj;
	ExchangeExt(filename, "OZJ", filename_ozj);

	FILE* compressedFile = MU_FOPEN(filename_ozj.c_str(), "rb");

	if (compressedFile == NULL)
	{
		return false;
	}

	fseek(compressedFile, 0, SEEK_END);

	long fileSize = ftell(compressedFile);

	if (fileSize <= 24)
	{
		//std::cerr << "Error: Archivo JPEG inv�lido o muy peque�o." << std::endl;
		fclose(compressedFile);
		return false;
	}

	fileSize -= 24;  // Ajuste de tama�o
	fseek(compressedFile, 24, SEEK_SET);  // Saltar los primeros 24 bytes

	std::vector<BYTE> PakBuffer(fileSize, 0);
	fread(PakBuffer.data(), 1, fileSize, compressedFile);
	fclose(compressedFile);

	tjhandle jpegDecompressor = tjInitDecompress();

	int jpegwidth, jpegheight, subsamp, colorspace;
	if (tjDecompressHeader3(jpegDecompressor, PakBuffer.data(), PakBuffer.size(), &jpegwidth, &jpegheight, &subsamp, &colorspace) != 0)
	{
		//std::cerr << "Error al leer encabezado JPEG: " << tjGetErrorStr() << std::endl;
		tjDestroy(jpegDecompressor);
		return false;
	}

	if (jpegwidth > MAX_WIDTH || jpegheight > MAX_HEIGHT)
	{
		//std::cerr << "Error: La imagen supera el tama�o m�ximo permitido." << std::endl;
		tjDestroy(jpegDecompressor);
		return false;
	}

	std::vector<BYTE> imageData(jpegwidth * jpegheight * 3);

	if (tjDecompress2(jpegDecompressor, PakBuffer.data(), PakBuffer.size(), imageData.data(), jpegwidth, 0, jpegheight, TJPF_RGB, TJFLAG_FASTDCT) != 0)
	{
		//std::cerr << "Error al descomprimir JPEG: " << tjGetErrorStr() << std::endl;
		tjDestroy(jpegDecompressor);
		return false;
	}

	tjDestroy(jpegDecompressor);

	int channels_in_file = 3;

	int textureWidth = leftoverSize(jpegwidth);

	int textureHeight = leftoverSize(jpegheight);

	BITMAP_t* pNewBitmap = new BITMAP_t;
	memset(pNewBitmap, 0, sizeof(BITMAP_t));

	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = jpegwidth;
	pNewBitmap->output_height = jpegheight;
	pNewBitmap->Components = channels_in_file;
	pNewBitmap->Ref = 1;
	std::snprintf(pNewBitmap->FileName, MAX_BITMAP_FILE_NAME, "%s", filename.c_str());

	size_t textureBufferSize = textureWidth * textureHeight * pNewBitmap->Components;

	pNewBitmap->Buffer.resize(textureBufferSize, 0);

	m_dwUsedTextureMemory += textureBufferSize;

	BYTE* textbuff = pNewBitmap->Buffer.data();

	if (jpegwidth != textureWidth || jpegheight != textureHeight)
	{
		// Si la GPU NO soporta NPOT, copiar l�nea por l�nea
		int row_size = jpegwidth * channels_in_file;

		for (int row = 0; row < jpegheight; row++)
		{
			std::copy(imageData.begin() + (row * row_size),
				imageData.begin() + (row * row_size) + row_size,
				textbuff + (row * textureWidth * channels_in_file));
		}
	}
	else
	{
		// Si la GPU soporta NPOT, copiamos directamente
		std::copy(imageData.begin(), imageData.end(), textbuff);
	}

	this->CreateMipmappedTexture(&pNewBitmap->TextureNumber, channels_in_file, textureWidth, textureHeight, textbuff, uiFilter, uiWrapMode);

	m_mapBitmap.insert(type_bitmap_map::value_type(uiBitmapIndex, pNewBitmap));

	m_BitmapName.push_back(std::pair<std::string, GLuint>(pNewBitmap->FileName, uiBitmapIndex));

	if (uiBitmapIndex != BITMAP_INTERFACE_MAP && uiBitmapIndex != BITMAP_GUILD && uiBitmapIndex != BITMAP_FONT)
	{
		m_dwUsedTextureMemory -= textureBufferSize;
		pNewBitmap->Buffer.clear();
	}

	return true;
}

bool CGlobalBitmap::OpenTga(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	std::string filename_ozt;
	ExchangeExt(filename, "OZT", filename_ozt);

	FILE* compressedFile = MU_FOPEN(filename_ozt.c_str(), "rb");

	if (compressedFile == NULL)
	{
		return false;
	}

	fseek(compressedFile, 0, SEEK_END);

	long  fileSize = ftell(compressedFile);

	if (fileSize <= 4)
	{
		fclose(compressedFile);
		return false;
	}

	fileSize -= 4;  // Ajuste de tama�o
	std::vector<BYTE> filebuffer(fileSize, 0);

	fseek(compressedFile, 4, SEEK_SET);  // Saltar los primeros 24 bytes
	fread(filebuffer.data(), 1, fileSize, compressedFile);
	fclose(compressedFile);

	int tgawidth, tgaheight, channels_in_file;

	BYTE* imageData = stbi_load_from_memory(filebuffer.data(), fileSize, &tgawidth, &tgaheight, &channels_in_file, 0);

	if (!imageData)
	{
		return false;
	}

	if ((channels_in_file != 3 && channels_in_file != 4) || tgawidth > MAX_WIDTH || tgaheight > MAX_HEIGHT)
	{
		stbi_image_free(imageData);
		return false;
	}

	int textureWidth = leftoverSize(tgawidth);

	int textureHeight = leftoverSize(tgaheight);

	BITMAP_t* pNewBitmap = new BITMAP_t;
	memset(pNewBitmap, 0, sizeof(BITMAP_t));

	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = tgawidth;
	pNewBitmap->output_height = tgaheight;
	pNewBitmap->Components = channels_in_file;
	pNewBitmap->Ref = 1;
	std::snprintf(pNewBitmap->FileName, MAX_BITMAP_FILE_NAME, "%s", filename.c_str());


	size_t textureBufferSize = textureWidth * textureHeight * pNewBitmap->Components;

	pNewBitmap->Buffer.resize(textureBufferSize, 0);

	m_dwUsedTextureMemory += textureBufferSize;

	BYTE* textbuff = pNewBitmap->Buffer.data();

	if (tgawidth != textureWidth || tgaheight != textureHeight)
	{
		// Si la GPU NO soporta NPOT, copiar l�nea por l�nea
		int row_size = tgawidth * channels_in_file;

		for (int row = 0; row < tgaheight; row++)
		{
			std::copy(imageData + (row * row_size),
				imageData + (row * row_size) + row_size,
				textbuff + (row * textureWidth * channels_in_file));
		}
	}
	else
	{
		std::copy(imageData, imageData + textureBufferSize, textbuff);
	}

	this->CreateMipmappedTexture(&pNewBitmap->TextureNumber, channels_in_file, textureWidth, textureHeight, textbuff, uiFilter, uiWrapMode);

	m_mapBitmap.insert(type_bitmap_map::value_type(uiBitmapIndex, pNewBitmap));

	m_BitmapName.push_back(std::pair<std::string, GLuint>(pNewBitmap->FileName, uiBitmapIndex));

	if (uiBitmapIndex != BITMAP_INTERFACE_MAP && uiBitmapIndex != BITMAP_GUILD && uiBitmapIndex != BITMAP_FONT)
	{
		m_dwUsedTextureMemory -= textureBufferSize;
		pNewBitmap->Buffer.clear();
	}

	stbi_image_free(imageData);

	return true;
}

void CGlobalBitmap::CreateMipmappedTexture(GLuint* TextureNumber, GLuint Components, GLuint Width, GLuint Height, BYTE* textureBuff, GLuint uiFilter, GLuint uiWrapMode)
{
	GLenum format = (Components == 4) ? GL_RGBA : GL_RGB;

	glGenTextures(1, TextureNumber);

	glBindTexture(GL_TEXTURE_2D,*TextureNumber);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, textureBuff);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	GLint maxMipLevels = 0;

	glGetIntegerv(GL_TEXTURE_MAX_LEVEL, &maxMipLevels);

	if (maxMipLevels != 0 && !GMProtect->IsWindows11())
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uiFilter);

		if (uiFilter == GL_LINEAR_MIPMAP_LINEAR || uiFilter == GL_LINEAR_MIPMAP_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glGenerateMipmap(GL_TEXTURE_2D); //-- Solo para mapas y terrenos
		}
		else if (uiFilter == GL_NEAREST_MIPMAP_LINEAR || uiFilter == GL_NEAREST_MIPMAP_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			glGenerateMipmap(GL_TEXTURE_2D); //-- Solo para mapas y terrenos
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, uiFilter);
		}
	}
	else
	{
		if (uiFilter == GL_LINEAR_MIPMAP_LINEAR || uiFilter == GL_LINEAR_MIPMAP_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (uiFilter == GL_NEAREST_MIPMAP_LINEAR || uiFilter == GL_NEAREST_MIPMAP_NEAREST)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, uiFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, uiFilter);
		}
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, uiWrapMode);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, uiWrapMode);

#if defined(ANISOTROPY_FUNCTIONAL) && !defined(__ANDROID__)
	if (GLEW_EXT_texture_filter_anisotropic && !GMProtect->IsWindows11())
	{
		GLfloat maxAnisotropy = 0.0;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

		if (maxAnisotropy >= 1.f)
		{
			maxAnisotropy = 4.f;
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		}
	}
#endif // ANISOTROPY_FUNCTIONAL && !__ANDROID__

	glBindTexture(GL_TEXTURE_2D, 0);
}

void CGlobalBitmap::SplitFileName(IN const std::string& filepath, OUT std::string& filename, bool bIncludeExt)
{
	char __fname[_MAX_FNAME] = { 0, };
	char __ext[_MAX_EXT] = { 0, };
	_splitpath(filepath.c_str(), NULL, NULL, __fname, __ext);
	filename = __fname;

	if (bIncludeExt)
	{
		filename += __ext;
	}
}
void CGlobalBitmap::SplitExt(IN const std::string& filepath, OUT std::string& ext, bool bIncludeDot)
{
	char __ext[_MAX_EXT] = { 0, };
	_splitpath(filepath.c_str(), NULL, NULL, NULL, __ext);

	if (bIncludeDot)
	{
		ext = __ext;
	}
	else
	{
		if ((__ext[0] == '.') && __ext[1])
			ext = __ext + 1;
	}
}
void CGlobalBitmap::ExchangeExt(IN const std::string& in_filepath, IN const std::string& ext, OUT std::string& out_filepath)
{
	char __drive[_MAX_DRIVE] = { 0, };
	char __dir[_MAX_DIR] = { 0, };
	char __fname[_MAX_FNAME] = { 0, };
	_splitpath(in_filepath.c_str(), __drive, __dir, __fname, NULL);

	out_filepath = __drive;
	out_filepath += __dir;
	out_filepath += __fname;
	out_filepath += '.';
	out_filepath += ext;
}

bool CGlobalBitmap::ConvertFormat(const unicode::t_string& filename)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	::_splitpath(filename.c_str(), drive, dir, fname, ext);

	std::string strPath = drive; strPath += dir;
	std::string strName = fname;

	if (_stricmp(ext, ".jpg") == 0)
	{
		unicode::t_string strSaveName = strPath + strName + ".OZJ";
		return Save_Image(filename, strSaveName.c_str(), 24);
	}
	else if (_stricmp(ext, ".tga") == 0)
	{
		unicode::t_string strSaveName = strPath + strName + ".OZT";
		return Save_Image(filename, strSaveName.c_str(), 4);
	}
	else if (_stricmp(ext, ".bmp") == 0)
	{
		unicode::t_string strSaveName = strPath + strName + ".OZB";
		return Save_Image(filename, strSaveName.c_str(), 4);
	}
	else
	{
	}

	return false;
}

bool CGlobalBitmap::Save_Image(const unicode::t_string& src, const unicode::t_string& dest, int cDumpHeader)
{
	FILE* fp = fopen(src.c_str(), "rb");
	if (fp == NULL)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pTempBuf = new char[size];
	fread(pTempBuf, 1, size, fp);
	fclose(fp);

	fp = fopen(dest.c_str(), "wb");
	if (fp == NULL)
		return false;

	fwrite(pTempBuf, 1, cDumpHeader, fp);
	fwrite(pTempBuf, 1, size, fp);
	fclose(fp);

	delete[] pTempBuf;

	return true;
}

int CGlobalBitmap::findByFullPath(const std::string& fullPath)
{
	auto it = std::find_if(m_BitmapName.begin(), m_BitmapName.end(), [&fullPath](const std::pair<std::string, int>& entry) {
			return entry.first == fullPath;
		});

	if (it != m_BitmapName.end())
	{
		return it->second;  // Devuelve el valor (ID) asociado
	}

	return 0;
}

int CGlobalBitmap::findByFileName(const std::string& name)
{
	auto it = std::find_if(m_BitmapName.begin(), m_BitmapName.end(), [&name](const std::pair<std::string, int>& entry) {
			std::string fileName;
			_SplitFileName(entry.first, fileName);  // Extrae el nombre del archivo de la ruta
			return fileName == name;  // Compara el nombre del archivo
		});

	if (it != m_BitmapName.end())
	{
		return it->second;  // Devuelve el valor (ID) asociado
	}
	return 0;
}

bool CGlobalBitmap::removeByFullPath(const std::string& fullPath)
{
	auto it = std::find_if(m_BitmapName.begin(), m_BitmapName.end(), [&fullPath](const std::pair<std::string, int>& entry) {
			return entry.first == fullPath;  // Compara la ruta completa
		});
	if (it != m_BitmapName.end())
	{
		m_BitmapName.erase(it);  // Elimina el elemento
		return true;  // Se elimin� el elemento
	}
	return false;  // No se encontr� el elemento
}

int CGlobalBitmap::leftoverSize(int Size)
{
	if (Size <= 1)
		return 1; // Maneja el caso de 0 o 1 (potencia de 2)
	if ((Size & (Size - 1)) == 0)
		return Size; // Si es ya potencia de 2, devuelve el mismo n�mero

	return (1 << (int)std::ceil(std::log2(Size))); // Si no es potencia de 2, redondea
}
