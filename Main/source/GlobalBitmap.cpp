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
#include <cstdint>

#include "GlobalBitmap.h"
#include "Platform/GameAssetPath.h"
#include "./Utilities/Log/ErrorReport.h"
#include "./Utilities/Log/muConsoleDebug.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

namespace
{
#ifdef __ANDROID__
constexpr const char* kTextureLogTag = "MUTexture";
#endif

std::string BuildPathWithExtension(const std::string& path, const char* extensionWithoutDot)
{
	char drive[_MAX_DRIVE] = { 0 };
	char dir[_MAX_DIR] = { 0 };
	char fname[_MAX_FNAME] = { 0 };

	_splitpath(path.c_str(), drive, dir, fname, NULL);

	std::string outPath = drive;
	outPath += dir;
	outPath += fname;
	outPath += '.';
	outPath += extensionWithoutDot;
	return outPath;
}

bool ReadBinaryFilePayload(const std::string& path, int headerSkip, std::vector<BYTE>& outData)
{
	if (headerSkip < 0)
	{
		return false;
	}

	FILE* file = MU_FOPEN(path.c_str(), "rb");
	if (!file)
	{
		return false;
	}

	fseek(file, 0, SEEK_END);
	const long fileSize = ftell(file);
	if (fileSize <= headerSkip)
	{
		fclose(file);
		return false;
	}

	const size_t payloadSize = static_cast<size_t>(fileSize - headerSkip);
	outData.resize(payloadSize);

	fseek(file, headerSkip, SEEK_SET);
	const size_t readBytes = fread(outData.data(), 1, payloadSize, file);
	fclose(file);

	if (readBytes != payloadSize)
	{
		outData.clear();
		return false;
	}

	return true;
}

bool LoadPackedOrRawEncoded(const std::string& filename,
	const char* packedExtensionWithoutDot,
	int packedHeaderSkip,
	std::vector<BYTE>& outEncoded,
	std::string& outResolvedPath)
{
	if (packedExtensionWithoutDot && packedExtensionWithoutDot[0] != '\0')
	{
		const std::string packedPath = BuildPathWithExtension(filename, packedExtensionWithoutDot);
		if (ReadBinaryFilePayload(packedPath, packedHeaderSkip, outEncoded))
		{
			outResolvedPath = packedPath;
			return true;
		}
	}

	if (ReadBinaryFilePayload(filename, 0, outEncoded))
	{
		outResolvedPath = filename;
		return true;
	}

	return false;
}

bool LoadPackedOrRawEncodedMulti(const std::string& filename,
	const char* packedExtensionWithoutDot1,
	int packedHeaderSkip1,
	const char* packedExtensionWithoutDot2,
	int packedHeaderSkip2,
	std::vector<BYTE>& outEncoded,
	std::string& outResolvedPath)
{
	if (packedExtensionWithoutDot1 && packedExtensionWithoutDot1[0] != '\0')
	{
		const std::string packedPath1 = BuildPathWithExtension(filename, packedExtensionWithoutDot1);
		if (ReadBinaryFilePayload(packedPath1, packedHeaderSkip1, outEncoded))
		{
			outResolvedPath = packedPath1;
			return true;
		}
	}

	if (packedExtensionWithoutDot2 && packedExtensionWithoutDot2[0] != '\0')
	{
		const std::string packedPath2 = BuildPathWithExtension(filename, packedExtensionWithoutDot2);
		if (ReadBinaryFilePayload(packedPath2, packedHeaderSkip2, outEncoded))
		{
			outResolvedPath = packedPath2;
			return true;
		}
	}

	if (ReadBinaryFilePayload(filename, 0, outEncoded))
	{
		outResolvedPath = filename;
		return true;
	}

	return false;
}

constexpr uint32_t MakeFourCC(char a, char b, char c, char d)
{
	return static_cast<uint32_t>(a)
		| (static_cast<uint32_t>(b) << 8u)
		| (static_cast<uint32_t>(c) << 16u)
		| (static_cast<uint32_t>(d) << 24u);
}

#pragma pack(push, 1)
struct DDSPixelFormat
{
	uint32_t size;
	uint32_t flags;
	uint32_t fourCC;
	uint32_t rgbBitCount;
	uint32_t rBitMask;
	uint32_t gBitMask;
	uint32_t bBitMask;
	uint32_t aBitMask;
};

struct DDSHeader
{
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t pitchOrLinearSize;
	uint32_t depth;
	uint32_t mipMapCount;
	uint32_t reserved1[11];
	DDSPixelFormat pixelFormat;
	uint32_t caps;
	uint32_t caps2;
	uint32_t caps3;
	uint32_t caps4;
	uint32_t reserved2;
};
#pragma pack(pop)

struct DXTColorBlock
{
	uint16_t color0;
	uint16_t color1;
	uint8_t rows[4];
};

struct DXT3AlphaBlock
{
	uint16_t rows[4];
};

struct DXT5AlphaBlock
{
	uint8_t alpha0;
	uint8_t alpha1;
	uint8_t bitmap[6];
};

inline BYTE Expand5To8(uint8_t value5)
{
	return static_cast<BYTE>((value5 << 3u) | (value5 >> 2u));
}

inline BYTE Expand6To8(uint8_t value6)
{
	return static_cast<BYTE>((value6 << 2u) | (value6 >> 4u));
}

void DecodeColor565(uint16_t packed565, BYTE& outR, BYTE& outG, BYTE& outB)
{
	outR = Expand5To8(static_cast<uint8_t>((packed565 >> 11u) & 0x1Fu));
	outG = Expand6To8(static_cast<uint8_t>((packed565 >> 5u) & 0x3Fu));
	outB = Expand5To8(static_cast<uint8_t>(packed565 & 0x1Fu));
}

void WriteRGBA(std::vector<BYTE>& rgba, int width, int x, int y, BYTE r, BYTE g, BYTE b, BYTE a)
{
	const size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)) * 4u;
	rgba[index + 0] = r;
	rgba[index + 1] = g;
	rgba[index + 2] = b;
	rgba[index + 3] = a;
}

void DecodeDXTColorPalette(const DXTColorBlock& colorBlock, bool dxt1PunchThrough, BYTE outPalette[4][4])
{
	DecodeColor565(colorBlock.color0, outPalette[0][0], outPalette[0][1], outPalette[0][2]);
	DecodeColor565(colorBlock.color1, outPalette[1][0], outPalette[1][1], outPalette[1][2]);
	outPalette[0][3] = 255;
	outPalette[1][3] = 255;

	if (dxt1PunchThrough && colorBlock.color0 <= colorBlock.color1)
	{
		outPalette[2][0] = static_cast<BYTE>((outPalette[0][0] + outPalette[1][0]) / 2u);
		outPalette[2][1] = static_cast<BYTE>((outPalette[0][1] + outPalette[1][1]) / 2u);
		outPalette[2][2] = static_cast<BYTE>((outPalette[0][2] + outPalette[1][2]) / 2u);
		outPalette[2][3] = 255;

		outPalette[3][0] = 0;
		outPalette[3][1] = 0;
		outPalette[3][2] = 0;
		outPalette[3][3] = 0;
	}
	else
	{
		outPalette[2][0] = static_cast<BYTE>((2u * outPalette[0][0] + outPalette[1][0]) / 3u);
		outPalette[2][1] = static_cast<BYTE>((2u * outPalette[0][1] + outPalette[1][1]) / 3u);
		outPalette[2][2] = static_cast<BYTE>((2u * outPalette[0][2] + outPalette[1][2]) / 3u);
		outPalette[2][3] = 255;

		outPalette[3][0] = static_cast<BYTE>((outPalette[0][0] + 2u * outPalette[1][0]) / 3u);
		outPalette[3][1] = static_cast<BYTE>((outPalette[0][1] + 2u * outPalette[1][1]) / 3u);
		outPalette[3][2] = static_cast<BYTE>((outPalette[0][2] + 2u * outPalette[1][2]) / 3u);
		outPalette[3][3] = 255;
	}
}

void DecodeDXT1Block(const DXTColorBlock& colorBlock, int blockX, int blockY, int width, int height, std::vector<BYTE>& rgba)
{
	BYTE palette[4][4] = {};
	DecodeDXTColorPalette(colorBlock, true, palette);

	for (int row = 0; row < 4; ++row)
	{
		const int dstY = blockY * 4 + row;
		if (dstY >= height)
		{
			continue;
		}

		for (int col = 0; col < 4; ++col)
		{
			const int dstX = blockX * 4 + col;
			if (dstX >= width)
			{
				continue;
			}

			const uint8_t paletteIndex = static_cast<uint8_t>((colorBlock.rows[row] >> (col * 2)) & 0x3u);
			WriteRGBA(rgba, width, dstX, dstY,
				palette[paletteIndex][0],
				palette[paletteIndex][1],
				palette[paletteIndex][2],
				palette[paletteIndex][3]);
		}
	}
}

void DecodeDXT3Block(const DXT3AlphaBlock& alphaBlock, const DXTColorBlock& colorBlock, int blockX, int blockY, int width, int height, std::vector<BYTE>& rgba)
{
	BYTE palette[4][4] = {};
	DecodeDXTColorPalette(colorBlock, false, palette);

	for (int row = 0; row < 4; ++row)
	{
		const int dstY = blockY * 4 + row;
		if (dstY >= height)
		{
			continue;
		}

		for (int col = 0; col < 4; ++col)
		{
			const int dstX = blockX * 4 + col;
			if (dstX >= width)
			{
				continue;
			}

			const uint8_t paletteIndex = static_cast<uint8_t>((colorBlock.rows[row] >> (col * 2)) & 0x3u);
			const uint8_t alpha4 = static_cast<uint8_t>((alphaBlock.rows[row] >> (col * 4)) & 0xFu);
			const uint8_t alpha8 = static_cast<uint8_t>(alpha4 * 17u);

			WriteRGBA(rgba, width, dstX, dstY,
				palette[paletteIndex][0],
				palette[paletteIndex][1],
				palette[paletteIndex][2],
				alpha8);
		}
	}
}

void BuildDXT5AlphaPalette(const DXT5AlphaBlock& alphaBlock, BYTE outAlpha[8])
{
	outAlpha[0] = alphaBlock.alpha0;
	outAlpha[1] = alphaBlock.alpha1;

	if (outAlpha[0] > outAlpha[1])
	{
		outAlpha[2] = static_cast<BYTE>((6u * outAlpha[0] + 1u * outAlpha[1]) / 7u);
		outAlpha[3] = static_cast<BYTE>((5u * outAlpha[0] + 2u * outAlpha[1]) / 7u);
		outAlpha[4] = static_cast<BYTE>((4u * outAlpha[0] + 3u * outAlpha[1]) / 7u);
		outAlpha[5] = static_cast<BYTE>((3u * outAlpha[0] + 4u * outAlpha[1]) / 7u);
		outAlpha[6] = static_cast<BYTE>((2u * outAlpha[0] + 5u * outAlpha[1]) / 7u);
		outAlpha[7] = static_cast<BYTE>((1u * outAlpha[0] + 6u * outAlpha[1]) / 7u);
	}
	else
	{
		outAlpha[2] = static_cast<BYTE>((4u * outAlpha[0] + 1u * outAlpha[1]) / 5u);
		outAlpha[3] = static_cast<BYTE>((3u * outAlpha[0] + 2u * outAlpha[1]) / 5u);
		outAlpha[4] = static_cast<BYTE>((2u * outAlpha[0] + 3u * outAlpha[1]) / 5u);
		outAlpha[5] = static_cast<BYTE>((1u * outAlpha[0] + 4u * outAlpha[1]) / 5u);
		outAlpha[6] = 0;
		outAlpha[7] = 255;
	}
}

void DecodeDXT5Block(const DXT5AlphaBlock& alphaBlock, const DXTColorBlock& colorBlock, int blockX, int blockY, int width, int height, std::vector<BYTE>& rgba)
{
	BYTE palette[4][4] = {};
	DecodeDXTColorPalette(colorBlock, false, palette);

	BYTE alphaPalette[8] = {};
	BuildDXT5AlphaPalette(alphaBlock, alphaPalette);

	uint64_t alphaBitmap = 0;
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[0]);
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[1]) << 8u;
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[2]) << 16u;
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[3]) << 24u;
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[4]) << 32u;
	alphaBitmap |= static_cast<uint64_t>(alphaBlock.bitmap[5]) << 40u;

	for (int row = 0; row < 4; ++row)
	{
		const int dstY = blockY * 4 + row;
		if (dstY >= height)
		{
			continue;
		}

		for (int col = 0; col < 4; ++col)
		{
			const int dstX = blockX * 4 + col;
			if (dstX >= width)
			{
				continue;
			}

			const uint8_t paletteIndex = static_cast<uint8_t>((colorBlock.rows[row] >> (col * 2)) & 0x3u);
			const uint8_t alphaIndex = static_cast<uint8_t>((alphaBitmap >> ((row * 4 + col) * 3)) & 0x7u);

			WriteRGBA(rgba, width, dstX, dstY,
				palette[paletteIndex][0],
				palette[paletteIndex][1],
				palette[paletteIndex][2],
				alphaPalette[alphaIndex]);
		}
	}
}

bool DecodeDDS(const std::vector<BYTE>& encoded,
	int& outWidth,
	int& outHeight,
	int& outSourceComponents,
	std::vector<BYTE>& outRgbaPixels)
{
	outWidth = 0;
	outHeight = 0;
	outSourceComponents = 0;
	outRgbaPixels.clear();

	if (encoded.size() < 4u + sizeof(DDSHeader))
	{
		return false;
	}

	if (!(encoded[0] == 'D' && encoded[1] == 'D' && encoded[2] == 'S' && encoded[3] == ' '))
	{
		return false;
	}

	const auto* header = reinterpret_cast<const DDSHeader*>(encoded.data() + 4u);
	if (header->size != 124u || header->pixelFormat.size != 32u)
	{
		return false;
	}

	const uint32_t width = header->width;
	const uint32_t height = header->height;
	if (width == 0u || height == 0u)
	{
		return false;
	}

	outWidth = static_cast<int>(width);
	outHeight = static_cast<int>(height);
	outSourceComponents = 4;
	outRgbaPixels.assign(static_cast<size_t>(width) * static_cast<size_t>(height) * 4u, 0);

	const size_t dataOffset = 4u + sizeof(DDSHeader);
	if (dataOffset >= encoded.size())
	{
		return false;
	}

	const BYTE* data = encoded.data() + dataOffset;
	const size_t dataSize = encoded.size() - dataOffset;

	const uint32_t fourCC = header->pixelFormat.fourCC;
	const size_t blocksWide = (static_cast<size_t>(width) + 3u) / 4u;
	const size_t blocksHigh = (static_cast<size_t>(height) + 3u) / 4u;

	if (fourCC == MakeFourCC('D', 'X', 'T', '1'))
	{
		const size_t blockSize = 8u;
		const size_t required = blocksWide * blocksHigh * blockSize;
		if (required > dataSize)
		{
			return false;
		}

		for (size_t by = 0; by < blocksHigh; ++by)
		{
			for (size_t bx = 0; bx < blocksWide; ++bx)
			{
				const size_t blockIndex = by * blocksWide + bx;
				const auto* colorBlock = reinterpret_cast<const DXTColorBlock*>(data + blockIndex * blockSize);
				DecodeDXT1Block(*colorBlock, static_cast<int>(bx), static_cast<int>(by), outWidth, outHeight, outRgbaPixels);
			}
		}
		return true;
	}

	if (fourCC == MakeFourCC('D', 'X', 'T', '3'))
	{
		const size_t blockSize = 16u;
		const size_t required = blocksWide * blocksHigh * blockSize;
		if (required > dataSize)
		{
			return false;
		}

		for (size_t by = 0; by < blocksHigh; ++by)
		{
			for (size_t bx = 0; bx < blocksWide; ++bx)
			{
				const size_t blockIndex = by * blocksWide + bx;
				const BYTE* blockPtr = data + blockIndex * blockSize;
				const auto* alphaBlock = reinterpret_cast<const DXT3AlphaBlock*>(blockPtr);
				const auto* colorBlock = reinterpret_cast<const DXTColorBlock*>(blockPtr + sizeof(DXT3AlphaBlock));
				DecodeDXT3Block(*alphaBlock, *colorBlock, static_cast<int>(bx), static_cast<int>(by), outWidth, outHeight, outRgbaPixels);
			}
		}
		return true;
	}

	if (fourCC == MakeFourCC('D', 'X', 'T', '5'))
	{
		const size_t blockSize = 16u;
		const size_t required = blocksWide * blocksHigh * blockSize;
		if (required > dataSize)
		{
			return false;
		}

		for (size_t by = 0; by < blocksHigh; ++by)
		{
			for (size_t bx = 0; bx < blocksWide; ++bx)
			{
				const size_t blockIndex = by * blocksWide + bx;
				const BYTE* blockPtr = data + blockIndex * blockSize;
				const auto* alphaBlock = reinterpret_cast<const DXT5AlphaBlock*>(blockPtr);
				const auto* colorBlock = reinterpret_cast<const DXTColorBlock*>(blockPtr + sizeof(DXT5AlphaBlock));
				DecodeDXT5Block(*alphaBlock, *colorBlock, static_cast<int>(bx), static_cast<int>(by), outWidth, outHeight, outRgbaPixels);
			}
		}
		return true;
	}

	// Uncompressed 32-bits RGBA/BGRA.
	if (header->pixelFormat.rgbBitCount == 32u && dataSize >= static_cast<size_t>(width) * static_cast<size_t>(height) * 4u)
	{
		const bool isBGRA =
			header->pixelFormat.rBitMask == 0x00ff0000u &&
			header->pixelFormat.gBitMask == 0x0000ff00u &&
			header->pixelFormat.bBitMask == 0x000000ffu &&
			header->pixelFormat.aBitMask == 0xff000000u;
		const bool isRGBA =
			header->pixelFormat.rBitMask == 0x000000ffu &&
			header->pixelFormat.gBitMask == 0x0000ff00u &&
			header->pixelFormat.bBitMask == 0x00ff0000u &&
			header->pixelFormat.aBitMask == 0xff000000u;

		const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
		if (isRGBA)
		{
			memcpy(outRgbaPixels.data(), data, pixelCount * 4u);
			return true;
		}
		if (isBGRA)
		{
			for (size_t i = 0; i < pixelCount; ++i)
			{
				outRgbaPixels[i * 4 + 0] = data[i * 4 + 2];
				outRgbaPixels[i * 4 + 1] = data[i * 4 + 1];
				outRgbaPixels[i * 4 + 2] = data[i * 4 + 0];
				outRgbaPixels[i * 4 + 3] = data[i * 4 + 3];
			}
			return true;
		}
	}

	return false;
}

bool HasTransparentAlpha(const BYTE* rgbaPixels, size_t pixelCount)
{
	for (size_t i = 0; i < pixelCount; ++i)
	{
		if (rgbaPixels[i * 4 + 3] < 255)
		{
			return true;
		}
	}
	return false;
}

bool HasNonBinaryAlpha(const BYTE* rgbaPixels, size_t pixelCount)
{
	for (size_t i = 0; i < pixelCount; ++i)
	{
		const BYTE alpha = rgbaPixels[i * 4 + 3];
		if (alpha != 0 && alpha != 255)
		{
			return true;
		}
	}
	return false;
}

void LogTextureAlphaDebug(const char* stage,
	const std::string& filename,
	int sourceComponents,
	int finalComponents,
	int width,
	int height,
	bool hasTransparentAlpha)
{
#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, kTextureLogTag,
		"%s file=%s srcComp=%d finalComp=%d size=%dx%d hasTransparentAlpha=%d",
		stage,
		filename.c_str(),
		sourceComponents,
		finalComponents,
		width,
		height,
		hasTransparentAlpha ? 1 : 0);
#else
	(void)stage;
	(void)filename;
	(void)sourceComponents;
	(void)finalComponents;
	(void)width;
	(void)height;
	(void)hasTransparentAlpha;
#endif
}

bool DecodeRGBAWithStbi(const std::vector<BYTE>& encoded,
	int& outWidth,
	int& outHeight,
	int& outSourceComponents,
	std::vector<BYTE>& outRgbaPixels)
{
	outWidth = 0;
	outHeight = 0;
	outSourceComponents = 0;
	outRgbaPixels.clear();

	if (encoded.empty())
	{
		return false;
	}

	(void)stbi_info_from_memory(
		encoded.data(),
		static_cast<int>(encoded.size()),
		&outWidth,
		&outHeight,
		&outSourceComponents);

	unsigned char* decoded = stbi_load_from_memory(
		encoded.data(),
		static_cast<int>(encoded.size()),
		&outWidth,
		&outHeight,
		&outSourceComponents,
		4);
	if (!decoded)
	{
		return false;
	}

	const size_t rgbaBytes = static_cast<size_t>(outWidth) * static_cast<size_t>(outHeight) * 4u;
	outRgbaPixels.assign(decoded, decoded + rgbaBytes);
	stbi_image_free(decoded);
	return true;
}
}

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

#ifdef __ANDROID__
	if (uiBitmapIndex == 31713)
	{
		__android_log_print(ANDROID_LOG_WARN,
			kTextureLogTag,
			"LoadImage request idx=31713 file=%s ext=%s filter=0x%x wrap=0x%x",
			filename.c_str(),
			ext.c_str(),
			uiFilter,
			uiWrapMode);
	}
#endif

	bool loaded = false;

	if (0 == _stricmp(ext.c_str(), "jpg") || 0 == _stricmp(ext.c_str(), "jpeg"))
		loaded = OpenJpeg(uiBitmapIndex, filename, uiFilter, uiWrapMode);
	else if (0 == _stricmp(ext.c_str(), "tga"))
		loaded = OpenTga(uiBitmapIndex, filename, uiFilter, uiWrapMode);
	else if (0 == _stricmp(ext.c_str(), "png"))
		loaded = OpenPng(uiBitmapIndex, filename, uiFilter, uiWrapMode);
	else if (0 == _stricmp(ext.c_str(), "bmp"))
		loaded = OpenBmp(uiBitmapIndex, filename, uiFilter, uiWrapMode);
	else if (0 == _stricmp(ext.c_str(), "dds"))
		loaded = OpenDDS_DXT5(uiBitmapIndex, filename, uiFilter, uiWrapMode);

#ifdef __ANDROID__
	if (uiBitmapIndex == 31713)
	{
		__android_log_print(ANDROID_LOG_WARN,
			kTextureLogTag,
			"LoadImage result idx=31713 file=%s loaded=%d",
			filename.c_str(),
			loaded ? 1 : 0);
	}
#endif

	return loaded;
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
	#ifdef __ANDROID__
		static int s_missingTextureWarnCount = 0;
		if (s_missingTextureWarnCount < 120)
		{
			++s_missingTextureWarnCount;
			__android_log_print(ANDROID_LOG_WARN, kTextureLogTag,
				"GetTexture missing index=%u (fallback error bitmap)",
				static_cast<unsigned>(uiBitmapIndex));
		}
	#endif
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

	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = jpegwidth;
	pNewBitmap->output_height = jpegheight;
	pNewBitmap->Components = channels_in_file;
	pNewBitmap->HasAlpha = false;
	pNewBitmap->HasNonBinaryAlpha = false;
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
#ifdef __ANDROID__
	GLESFF::RegisterTextureFormat(pNewBitmap->TextureNumber, channels_in_file == 4);
#endif

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
	std::vector<BYTE> encoded;
	std::string resolvedPath;
	if (!LoadPackedOrRawEncoded(filename, "OZT", 4, encoded, resolvedPath))
	{
		return false;
	}

	int sourceWidth = 0;
	int sourceHeight = 0;
	int sourceComponents = 0;
	std::vector<BYTE> rgbaPixels;
	if (!DecodeRGBAWithStbi(encoded, sourceWidth, sourceHeight, sourceComponents, rgbaPixels))
	{
		return false;
	}

	if (sourceWidth > MAX_WIDTH || sourceHeight > MAX_HEIGHT)
	{
		return false;
	}

	const bool hasTransparentAlpha = HasTransparentAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	const bool hasNonBinaryAlpha = hasTransparentAlpha &&
		HasNonBinaryAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	int finalComponents = hasTransparentAlpha ? 4 : 3;
#ifdef __ANDROID__
	// Legacy render paths branch heavily on Components==4.
	// Preserve RGBA for 4-channel sources even when alpha is fully opaque.
	if (sourceComponents == 4)
	{
		finalComponents = 4;
	}
#endif
	LogTextureAlphaDebug("LoadTga", resolvedPath, sourceComponents, finalComponents, sourceWidth, sourceHeight, hasTransparentAlpha);

#ifdef __ANDROID__
	if (!hasTransparentAlpha)
	{
		__android_log_print(ANDROID_LOG_WARN, kTextureLogTag,
			"Texture loaded without alpha: %s (srcComp=%d)", resolvedPath.c_str(), sourceComponents);
	}
#endif

	const int textureWidth = leftoverSize(sourceWidth);
	const int textureHeight = leftoverSize(sourceHeight);

	BITMAP_t* pNewBitmap = new BITMAP_t;
	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = sourceWidth;
	pNewBitmap->output_height = sourceHeight;
	pNewBitmap->Components = static_cast<BYTE>(finalComponents);
	pNewBitmap->HasAlpha = (finalComponents == 4);
	pNewBitmap->HasNonBinaryAlpha = (finalComponents == 4) ? hasNonBinaryAlpha : false;
	pNewBitmap->Ref = 1;
	std::snprintf(pNewBitmap->FileName, MAX_BITMAP_FILE_NAME, "%s", filename.c_str());

	const size_t textureBufferSize = static_cast<size_t>(textureWidth) * static_cast<size_t>(textureHeight) * static_cast<size_t>(finalComponents);
	pNewBitmap->Buffer.resize(textureBufferSize, 0);
	m_dwUsedTextureMemory += textureBufferSize;

	BYTE* textureBuffer = pNewBitmap->Buffer.data();
	for (int row = 0; row < sourceHeight; ++row)
	{
		const BYTE* srcRow = rgbaPixels.data() + static_cast<size_t>(row) * static_cast<size_t>(sourceWidth) * 4u;
		BYTE* dstRow = textureBuffer + static_cast<size_t>(row) * static_cast<size_t>(textureWidth) * static_cast<size_t>(finalComponents);

		if (finalComponents == 4)
		{
			memcpy(dstRow, srcRow, static_cast<size_t>(sourceWidth) * 4u);
		}
		else
		{
			for (int col = 0; col < sourceWidth; ++col)
			{
				dstRow[col * 3 + 0] = srcRow[col * 4 + 0];
				dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
				dstRow[col * 3 + 2] = srcRow[col * 4 + 2];
			}
		}
	}

	this->CreateMipmappedTexture(&pNewBitmap->TextureNumber, finalComponents, textureWidth, textureHeight, textureBuffer, uiFilter, uiWrapMode);
#ifdef __ANDROID__
	// Track usable transparency, not storage format.
	// Some assets are uploaded as RGBA for compatibility but have fully opaque alpha.
	GLESFF::RegisterTextureFormat(pNewBitmap->TextureNumber, hasTransparentAlpha);
#endif

	m_mapBitmap.insert(type_bitmap_map::value_type(uiBitmapIndex, pNewBitmap));
	m_BitmapName.push_back(std::pair<std::string, GLuint>(pNewBitmap->FileName, uiBitmapIndex));

	if (uiBitmapIndex != BITMAP_INTERFACE_MAP && uiBitmapIndex != BITMAP_GUILD && uiBitmapIndex != BITMAP_FONT)
	{
		m_dwUsedTextureMemory -= textureBufferSize;
		pNewBitmap->Buffer.clear();
	}

	return true;
}

bool CGlobalBitmap::OpenPng(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	// PNG assets are stored as regular files in Android resources and may also be packed as OZT.
	return OpenTga(uiBitmapIndex, filename, uiFilter, uiWrapMode);
}

bool CGlobalBitmap::OpenBmp(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	std::vector<BYTE> encoded;
	std::string resolvedPath;
	if (!LoadPackedOrRawEncoded(filename, "OZB", 4, encoded, resolvedPath))
	{
		return false;
	}

	int sourceWidth = 0;
	int sourceHeight = 0;
	int sourceComponents = 0;
	std::vector<BYTE> rgbaPixels;
	if (!DecodeRGBAWithStbi(encoded, sourceWidth, sourceHeight, sourceComponents, rgbaPixels))
	{
		return false;
	}

	if (sourceWidth > MAX_WIDTH || sourceHeight > MAX_HEIGHT)
	{
		return false;
	}

	const bool hasTransparentAlpha = HasTransparentAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	const bool hasNonBinaryAlpha = hasTransparentAlpha &&
		HasNonBinaryAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	int finalComponents = hasTransparentAlpha ? 4 : 3;
#ifdef __ANDROID__
	if (sourceComponents == 4)
	{
		finalComponents = 4;
	}
#endif
	LogTextureAlphaDebug("LoadBmp", resolvedPath, sourceComponents, finalComponents, sourceWidth, sourceHeight, hasTransparentAlpha);

#ifdef __ANDROID__
	if (!hasTransparentAlpha)
	{
		__android_log_print(ANDROID_LOG_WARN, kTextureLogTag,
			"BMP loaded without alpha: %s (srcComp=%d)", resolvedPath.c_str(), sourceComponents);
	}
#endif

	const int textureWidth = leftoverSize(sourceWidth);
	const int textureHeight = leftoverSize(sourceHeight);
	BITMAP_t* pNewBitmap = new BITMAP_t;
	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = sourceWidth;
	pNewBitmap->output_height = sourceHeight;
	pNewBitmap->Components = static_cast<BYTE>(finalComponents);
	pNewBitmap->HasAlpha = (finalComponents == 4);
	pNewBitmap->HasNonBinaryAlpha = (finalComponents == 4) ? hasNonBinaryAlpha : false;
	pNewBitmap->Ref = 1;
	std::snprintf(pNewBitmap->FileName, MAX_BITMAP_FILE_NAME, "%s", filename.c_str());

	const size_t textureBufferSize = static_cast<size_t>(textureWidth) * static_cast<size_t>(textureHeight) * static_cast<size_t>(finalComponents);
	pNewBitmap->Buffer.resize(textureBufferSize, 0);
	m_dwUsedTextureMemory += textureBufferSize;

	BYTE* textureBuffer = pNewBitmap->Buffer.data();
	for (int row = 0; row < sourceHeight; ++row)
	{
		const BYTE* srcRow = rgbaPixels.data() + static_cast<size_t>(row) * static_cast<size_t>(sourceWidth) * 4u;
		BYTE* dstRow = textureBuffer + static_cast<size_t>(row) * static_cast<size_t>(textureWidth) * static_cast<size_t>(finalComponents);

		if (finalComponents == 4)
		{
			memcpy(dstRow, srcRow, static_cast<size_t>(sourceWidth) * 4u);
		}
		else
		{
			for (int col = 0; col < sourceWidth; ++col)
			{
				dstRow[col * 3 + 0] = srcRow[col * 4 + 0];
				dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
				dstRow[col * 3 + 2] = srcRow[col * 4 + 2];
			}
		}
	}

	this->CreateMipmappedTexture(&pNewBitmap->TextureNumber, finalComponents, textureWidth, textureHeight, textureBuffer, uiFilter, uiWrapMode);
#ifdef __ANDROID__
	// Track usable transparency, not storage format.
	// Some assets are uploaded as RGBA for compatibility but have fully opaque alpha.
	GLESFF::RegisterTextureFormat(pNewBitmap->TextureNumber, hasTransparentAlpha);
#endif
	m_mapBitmap.insert(type_bitmap_map::value_type(uiBitmapIndex, pNewBitmap));
	m_BitmapName.push_back(std::pair<std::string, GLuint>(pNewBitmap->FileName, uiBitmapIndex));

	if (uiBitmapIndex != BITMAP_INTERFACE_MAP && uiBitmapIndex != BITMAP_GUILD && uiBitmapIndex != BITMAP_FONT)
	{
		m_dwUsedTextureMemory -= textureBufferSize;
		pNewBitmap->Buffer.clear();
	}

	return true;
}

bool CGlobalBitmap::OpenDDS_DXT5(GLuint uiBitmapIndex, const std::string& filename, GLuint uiFilter, GLuint uiWrapMode)
{
	std::vector<BYTE> encoded;
	std::string resolvedPath;
	if (!LoadPackedOrRawEncodedMulti(filename, "OZD", 4, "OZT", 4, encoded, resolvedPath))
	{
		return false;
	}

	int sourceWidth = 0;
	int sourceHeight = 0;
	int sourceComponents = 0;
	std::vector<BYTE> rgbaPixels;

	if (!DecodeRGBAWithStbi(encoded, sourceWidth, sourceHeight, sourceComponents, rgbaPixels))
	{
		// Raw DDS (DXT1/3/5 and uncompressed RGBA/BGRA) fallback.
		if (!DecodeDDS(encoded, sourceWidth, sourceHeight, sourceComponents, rgbaPixels))
		{
			return false;
		}
	}

	if (sourceWidth <= 0 || sourceHeight <= 0 || sourceWidth > MAX_WIDTH || sourceHeight > MAX_HEIGHT)
	{
		return false;
	}

	const bool hasTransparentAlpha = HasTransparentAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	const bool hasNonBinaryAlpha = hasTransparentAlpha &&
		HasNonBinaryAlpha(rgbaPixels.data(), static_cast<size_t>(sourceWidth) * static_cast<size_t>(sourceHeight));
	int finalComponents = hasTransparentAlpha ? 4 : 3;
#ifdef __ANDROID__
	if (sourceComponents == 4)
	{
		finalComponents = 4;
	}
#endif
	LogTextureAlphaDebug("LoadDds", resolvedPath, sourceComponents, finalComponents, sourceWidth, sourceHeight, hasTransparentAlpha);

#ifdef __ANDROID__
	if (!hasTransparentAlpha)
	{
		__android_log_print(ANDROID_LOG_WARN, kTextureLogTag,
			"DDS loaded without alpha: %s (srcComp=%d)", resolvedPath.c_str(), sourceComponents);
	}
#endif

	const int textureWidth = leftoverSize(sourceWidth);
	const int textureHeight = leftoverSize(sourceHeight);
	BITMAP_t* pNewBitmap = new BITMAP_t;
	pNewBitmap->BitmapIndex = uiBitmapIndex;
	pNewBitmap->Width = (float)textureWidth;
	pNewBitmap->Height = (float)textureHeight;
	pNewBitmap->output_width = sourceWidth;
	pNewBitmap->output_height = sourceHeight;
	pNewBitmap->Components = static_cast<BYTE>(finalComponents);
	pNewBitmap->HasAlpha = (finalComponents == 4);
	pNewBitmap->HasNonBinaryAlpha = (finalComponents == 4) ? hasNonBinaryAlpha : false;
	pNewBitmap->Ref = 1;
	std::snprintf(pNewBitmap->FileName, MAX_BITMAP_FILE_NAME, "%s", filename.c_str());

	const size_t textureBufferSize = static_cast<size_t>(textureWidth) * static_cast<size_t>(textureHeight) * static_cast<size_t>(finalComponents);
	pNewBitmap->Buffer.resize(textureBufferSize, 0);
	m_dwUsedTextureMemory += textureBufferSize;

	BYTE* textureBuffer = pNewBitmap->Buffer.data();
	for (int row = 0; row < sourceHeight; ++row)
	{
		const BYTE* srcRow = rgbaPixels.data() + static_cast<size_t>(row) * static_cast<size_t>(sourceWidth) * 4u;
		BYTE* dstRow = textureBuffer + static_cast<size_t>(row) * static_cast<size_t>(textureWidth) * static_cast<size_t>(finalComponents);

		if (finalComponents == 4)
		{
			memcpy(dstRow, srcRow, static_cast<size_t>(sourceWidth) * 4u);
		}
		else
		{
			for (int col = 0; col < sourceWidth; ++col)
			{
				dstRow[col * 3 + 0] = srcRow[col * 4 + 0];
				dstRow[col * 3 + 1] = srcRow[col * 4 + 1];
				dstRow[col * 3 + 2] = srcRow[col * 4 + 2];
			}
		}
	}

	this->CreateMipmappedTexture(&pNewBitmap->TextureNumber, finalComponents, textureWidth, textureHeight, textureBuffer, uiFilter, uiWrapMode);
#ifdef __ANDROID__
	// Track usable transparency, not storage format.
	// Some assets are uploaded as RGBA for compatibility but have fully opaque alpha.
	GLESFF::RegisterTextureFormat(pNewBitmap->TextureNumber, hasTransparentAlpha);
#endif
	m_mapBitmap.insert(type_bitmap_map::value_type(uiBitmapIndex, pNewBitmap));
	m_BitmapName.push_back(std::pair<std::string, GLuint>(pNewBitmap->FileName, uiBitmapIndex));

	if (uiBitmapIndex != BITMAP_INTERFACE_MAP && uiBitmapIndex != BITMAP_GUILD && uiBitmapIndex != BITMAP_FONT)
	{
		m_dwUsedTextureMemory -= textureBufferSize;
		pNewBitmap->Buffer.clear();
	}

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

#ifdef __ANDROID__
	// On GLES3, GL_TEXTURE_MAX_LEVEL is a per-texture param, not a global state.
	// glGetIntegerv(GL_TEXTURE_MAX_LEVEL) generates GL_INVALID_ENUM on GLES3.
	// Skip mipmaps on Android; use simple filter fallback below.
	GLint maxMipLevels = 0;
#else
	GLint maxMipLevels = 0;

	glGetIntegerv(GL_TEXTURE_MAX_LEVEL, &maxMipLevels);
#endif

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
