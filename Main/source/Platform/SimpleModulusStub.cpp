#ifdef __ANDROID__

#include "../stdafx.h"
#include "../SimpleModulus.h"
#include "GameAssetPath.h"

#include <cstdio>
#include <cstring>
#include <android/log.h>

#define MU_SM_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MUAndroidCrypto", __VA_ARGS__)

namespace
{
	static const unsigned short KEYFILE_ALLKEY = 0x1111;
	static const unsigned short KEYFILE_ONEKEY = 0x1112;

	static unsigned int CalculateKeyfileSize(BOOL bMod, BOOL bEnc, BOOL bDec, BOOL bXor)
	{
		const unsigned int blockCount = (unsigned int)(bMod + bEnc + bDec + bXor);
		return (unsigned int)(sizeof(ChunkHeader) + blockCount * sizeof(DWORD) * SIZE_ENCRYPTION_KEY);
	}

	static bool ReadFully(FILE* fp, void* dst, size_t size)
	{
		return (fp != NULL && dst != NULL && std::fread(dst, 1, size, fp) == size);
	}

	static bool WriteFully(FILE* fp, const void* src, size_t size)
	{
		return (fp != NULL && src != NULL && std::fwrite(src, 1, size, fp) == size);
	}
}

DWORD CSimpleModulus::s_dwSaveLoadXOR[SIZE_ENCRYPTION_KEY] = { 0x3F08A79B, 0xE25CC287, 0x93D27AB9, 0x20DEA7BF };

CSimpleModulus::CSimpleModulus()
{
	Init();
}

CSimpleModulus::~CSimpleModulus()
{
}

void CSimpleModulus::Init(void)
{
	std::memset(m_dwModulus, 0, sizeof(m_dwModulus));
	std::memset(m_dwEncryptionKey, 0, sizeof(m_dwEncryptionKey));
	std::memset(m_dwDecryptionKey, 0, sizeof(m_dwDecryptionKey));
	std::memset(m_dwXORKey, 0, sizeof(m_dwXORKey));
}

int CSimpleModulus::Encrypt(void* lpTarget, void* lpSource, int iSize)
{
	const int iTotalSize = ((iSize + SIZE_ENCRYPTION_BLOCK - 1) / SIZE_ENCRYPTION_BLOCK) * SIZE_ENCRYPTED_BLOCK;

	if (lpTarget == NULL)
	{
		return iTotalSize;
	}

	LPBYTE pbySource = (LPBYTE)lpSource;
	LPBYTE pbyTarget = (LPBYTE)lpTarget;

	for (int i = 0; i < iSize; i += SIZE_ENCRYPTION_BLOCK, pbySource += SIZE_ENCRYPTION_BLOCK, pbyTarget += SIZE_ENCRYPTED_BLOCK)
	{
		int blockSize = SIZE_ENCRYPTION_BLOCK;
		if ((iSize - i) < SIZE_ENCRYPTION_BLOCK)
		{
			blockSize = (iSize - i);
		}

		EncryptBlock(pbyTarget, pbySource, blockSize);
	}

	return iTotalSize;
}

int CSimpleModulus::Decrypt(void* lpTarget, void* lpSource, int iSize)
{
	if (lpTarget == NULL)
	{
		return ((iSize + SIZE_ENCRYPTED_BLOCK - 1) / SIZE_ENCRYPTED_BLOCK) * SIZE_ENCRYPTION_BLOCK;
	}

	int iTotalSize = 0;
	LPBYTE pbyTarget = (LPBYTE)lpTarget;
	LPBYTE pbySource = (LPBYTE)lpSource;

	for (int i = 0; i < iSize; i += SIZE_ENCRYPTED_BLOCK, pbySource += SIZE_ENCRYPTED_BLOCK, pbyTarget += SIZE_ENCRYPTION_BLOCK)
	{
		const int iBlockSize = DecryptBlock(pbyTarget, pbySource);
		if (iBlockSize < 0)
		{
			return iBlockSize;
		}

		iTotalSize += iBlockSize;
	}

	return iTotalSize;
}

void CSimpleModulus::EncryptBlock(void* lpTarget, void* lpSource, int nSize)
{
	DWORD dwEncBuffer[SIZE_ENCRYPTION_KEY];
	std::memset(lpTarget, 0, SIZE_ENCRYPTED_BLOCK);

	LPBYTE lpSrcPtr = (LPBYTE)lpSource;
	DWORD dwPrev = 0;

	for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++, lpSrcPtr += 2)
	{
		DWORD dwNext = 0;
		std::memcpy(&dwNext, lpSrcPtr, 2);

		dwEncBuffer[i] = ((dwNext ^ m_dwXORKey[i] ^ dwPrev) * m_dwEncryptionKey[i]) % m_dwModulus[i];
		dwPrev = dwEncBuffer[i] & 0xFFFF;
	}

	dwPrev = dwEncBuffer[SIZE_ENCRYPTION_KEY - 1] & 0xFFFF;

	for (int i = SIZE_ENCRYPTION_KEY - 2; i >= 0; i--)
	{
		const DWORD dwSrc = dwEncBuffer[i] & 0xFFFF;
		dwEncBuffer[i] = dwEncBuffer[i] ^ m_dwXORKey[i] ^ dwPrev;
		dwPrev = dwSrc;
	}

	int nTotalBits = 0;
	for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
	{
		nTotalBits = AddBits(lpTarget, nTotalBits, &dwEncBuffer[i], 0, 16);
		nTotalBits = AddBits(lpTarget, nTotalBits, &dwEncBuffer[i], 22, 2);
	}

	BYTE cCheckSum[2];
	cCheckSum[0] = (nSize & 0xFF) ^ 0x3D;
	cCheckSum[1] = 0xF8;

	LPBYTE lpSourceSeek = (LPBYTE)lpSource;
	for (int i = 0; i < SIZE_ENCRYPTION_BLOCK; i++)
	{
		cCheckSum[1] ^= lpSourceSeek[i];
	}

	cCheckSum[0] ^= cCheckSum[1];
	nTotalBits = AddBits(lpTarget, nTotalBits, &cCheckSum, 0, 16);
}

int CSimpleModulus::DecryptBlock(void* lpTarget, void* lpSource)
{
	LPBYTE lpEncrypted = (LPBYTE)lpSource;
	DWORD dwDecBuffer[SIZE_ENCRYPTION_KEY];

	std::memset(lpTarget, 0, SIZE_ENCRYPTION_BLOCK);
	LPBYTE lpTgtPtr = (LPBYTE)lpTarget;
	std::memset(dwDecBuffer, 0, sizeof(dwDecBuffer));

	int nTotalBits = 0;
	for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
	{
		AddBits(&dwDecBuffer[i], 0, lpEncrypted, nTotalBits, 16);
		nTotalBits += 16;

		AddBits(&dwDecBuffer[i], 22, lpEncrypted, nTotalBits, 2);
		nTotalBits += 2;
	}

	DWORD dwPrev = dwDecBuffer[SIZE_ENCRYPTION_KEY - 1] & 0xFFFF;
	for (int i = SIZE_ENCRYPTION_KEY - 2; i >= 0; i--)
	{
		dwDecBuffer[i] ^= m_dwXORKey[i];
		dwDecBuffer[i] ^= dwPrev;
		dwPrev = dwDecBuffer[i] & 0xFFFF;
	}

	dwPrev = 0;
	for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++, lpTgtPtr += 2)
	{
		const DWORD dwResult = (dwDecBuffer[i] * m_dwDecryptionKey[i]) % m_dwModulus[i] ^ m_dwXORKey[i] ^ dwPrev;
		std::memcpy(lpTgtPtr, &dwResult, 2);
		dwPrev = dwDecBuffer[i] & 0xFFFF;
	}

	BYTE cCheckSum[2] = { 0, 0 };
	AddBits(&cCheckSum, 0, lpEncrypted, nTotalBits, 16);
	nTotalBits += 16;
	cCheckSum[0] = cCheckSum[1] ^ (cCheckSum[0] ^ 0x3D);

	LPBYTE lpTgtSeek = (LPBYTE)lpTarget;
	BYTE cTempCheckSum = 0xF8;
	for (int i = 0; i < SIZE_ENCRYPTION_BLOCK; i++)
	{
		cTempCheckSum ^= lpTgtSeek[i];
	}

	if (cCheckSum[1] != cTempCheckSum)
	{
		return -1;
	}

	return cCheckSum[0];
}

int CSimpleModulus::AddBits(void* lpBuffer, int nNumBufferBits, void* lpBits, int nInitialBit, int nNumBits)
{
	int nBufferSize = GetByteOfBit(nNumBits + nInitialBit - 1);
	nBufferSize = nBufferSize - GetByteOfBit(nInitialBit) + 1;

	LPBYTE lpTemp = new BYTE[nBufferSize + 1];
	std::memset(lpTemp, 0, nBufferSize + 1);
	std::memcpy(lpTemp, (LPBYTE)lpBits + GetByteOfBit(nInitialBit), nBufferSize);

	const int nLastBitMod8 = (nNumBits + nInitialBit) % SIZE_ENCRYPTION_BLOCK;
	if (nLastBitMod8 != 0)
	{
		lpTemp[nBufferSize - 1] &= (nLastBitMod8 | 0xFF) << (SIZE_ENCRYPTION_BLOCK - nLastBitMod8);
	}

	const int nShiftLeft = (nInitialBit % SIZE_ENCRYPTION_BLOCK);
	const int nShiftRight = (nNumBufferBits % SIZE_ENCRYPTION_BLOCK);

	Shift(lpTemp, nBufferSize, -nShiftLeft);
	Shift(lpTemp, nBufferSize + 1, nShiftRight);

	const int nMax = ((nShiftRight <= nShiftLeft) ? 0 : 1) + nBufferSize;
	LPBYTE lpTarget = (LPBYTE)lpBuffer + GetByteOfBit(nNumBufferBits);
	LPBYTE lpSeek = lpTemp;
	for (int i = 0; i < nMax; i++, lpTarget++, lpSeek++)
	{
		*lpTarget |= *lpSeek;
	}

	delete[] lpTemp;
	return nNumBufferBits + nNumBits;
}

void CSimpleModulus::Shift(void* lpBuffer, int nByte, int nShift)
{
	if (nShift == 0)
	{
		return;
	}

	if (nShift > 0)
	{
		LPBYTE lpTemp = (LPBYTE)lpBuffer + (nByte - 1);
		for (int i = nByte - 1; i > 0; i--, lpTemp--)
		{
			*lpTemp = (*(lpTemp - 1) << (SIZE_ENCRYPTION_BLOCK - nShift)) | (*lpTemp >> nShift);
		}
		*lpTemp = *lpTemp >> nShift;
	}
	else
	{
		const int nRealShift = -nShift;
		LPBYTE lpTemp = (LPBYTE)lpBuffer;
		for (int i = 0; i < nByte - 1; i++, lpTemp++)
		{
			*lpTemp = (*(lpTemp + 1) >> (SIZE_ENCRYPTION_BLOCK - nRealShift)) | (*lpTemp << nRealShift);
		}
		*lpTemp = *lpTemp << nRealShift;
	}
}

int CSimpleModulus::GetByteOfBit(int nBit)
{
	return nBit >> 3;
}

BOOL CSimpleModulus::SaveAllKey(char* lpszFileName)
{
	return SaveKey(lpszFileName, KEYFILE_ALLKEY, TRUE, TRUE, TRUE, TRUE);
}

BOOL CSimpleModulus::LoadAllKey(char* lpszFileName)
{
	return LoadKey(lpszFileName, KEYFILE_ALLKEY, TRUE, TRUE, TRUE, TRUE);
}

BOOL CSimpleModulus::SaveEncryptionKey(char* lpszFileName)
{
	return SaveKey(lpszFileName, KEYFILE_ONEKEY, TRUE, TRUE, FALSE, TRUE);
}

BOOL CSimpleModulus::LoadEncryptionKey(char* lpszFileName)
{
	return LoadKey(lpszFileName, KEYFILE_ONEKEY, TRUE, TRUE, FALSE, TRUE);
}

BOOL CSimpleModulus::SaveDecryptionKey(char* lpszFileName)
{
	return SaveKey(lpszFileName, KEYFILE_ONEKEY, TRUE, FALSE, TRUE, TRUE);
}

BOOL CSimpleModulus::LoadDecryptionKey(char* lpszFileName)
{
	return LoadKey(lpszFileName, KEYFILE_ONEKEY, TRUE, FALSE, TRUE, TRUE);
}

BOOL CSimpleModulus::SaveKey(char* lpszFileName, unsigned short sID, BOOL bMod, BOOL bEnc, BOOL bDec, BOOL bXOR)
{
	FILE* fp = MU_FOPEN(lpszFileName, "wb");
	if (fp == NULL)
	{
		return FALSE;
	}

	ChunkHeader chunk;
	chunk.m_sID = sID;
	chunk.m_iSize = CalculateKeyfileSize(bMod, bEnc, bDec, bXOR);

	if (!WriteFully(fp, &chunk, sizeof(chunk)))
	{
		std::fclose(fp);
		return FALSE;
	}

	DWORD buffer[SIZE_ENCRYPTION_KEY];
	if (bMod != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			buffer[i] = m_dwModulus[i] ^ s_dwSaveLoadXOR[i];
		}
		if (!WriteFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
	}

	if (bEnc != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			buffer[i] = m_dwEncryptionKey[i] ^ s_dwSaveLoadXOR[i];
		}
		if (!WriteFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
	}

	if (bDec != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			buffer[i] = m_dwDecryptionKey[i] ^ s_dwSaveLoadXOR[i];
		}
		if (!WriteFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
	}

	if (bXOR != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			buffer[i] = m_dwXORKey[i] ^ s_dwSaveLoadXOR[i];
		}
		if (!WriteFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
	}

	std::fclose(fp);
	return TRUE;
}

BOOL CSimpleModulus::LoadKey(char* lpszFileName, unsigned short sID, BOOL bMod, BOOL bEnc, BOOL bDec, BOOL bXOR)
{
	FILE* fp = MU_FOPEN(lpszFileName, "rb");
	if (fp == NULL)
	{
		MU_SM_LOG("LoadKey: failed to open '%s'", (lpszFileName != NULL) ? lpszFileName : "(null)");
		return FALSE;
	}

	ChunkHeader chunk;
	if (!ReadFully(fp, &chunk, sizeof(chunk)))
	{
		std::fclose(fp);
		return FALSE;
	}

	if (chunk.m_sID != sID || chunk.m_iSize != CalculateKeyfileSize(bMod, bEnc, bDec, bXOR))
	{
		std::fclose(fp);
		MU_SM_LOG("LoadKey: invalid header for '%s' id=0x%04X size=%u", (lpszFileName != NULL) ? lpszFileName : "(null)", chunk.m_sID, chunk.m_iSize);
		return FALSE;
	}

	DWORD buffer[SIZE_ENCRYPTION_KEY];
	if (bMod != FALSE)
	{
		if (!ReadFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwModulus[i] = s_dwSaveLoadXOR[i] ^ buffer[i];
		}
	}

	if (bEnc != FALSE)
	{
		if (!ReadFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwEncryptionKey[i] = s_dwSaveLoadXOR[i] ^ buffer[i];
		}
	}

	if (bDec != FALSE)
	{
		if (!ReadFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwDecryptionKey[i] = s_dwSaveLoadXOR[i] ^ buffer[i];
		}
	}

	if (bXOR != FALSE)
	{
		if (!ReadFully(fp, buffer, sizeof(buffer)))
		{
			std::fclose(fp);
			return FALSE;
		}
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwXORKey[i] = s_dwSaveLoadXOR[i] ^ buffer[i];
		}
	}

	std::fclose(fp);
	MU_SM_LOG("LoadKey: success '%s'", (lpszFileName != NULL) ? lpszFileName : "(null)");
	return TRUE;
}

BOOL CSimpleModulus::LoadKeyFromBuffer(BYTE* pbyBuffer, BOOL bMod, BOOL bEnc, BOOL bDec, BOOL bXOR)
{
	if (pbyBuffer == NULL)
	{
		return FALSE;
	}

	DWORD* pdwSeek = (DWORD*)pbyBuffer;
	if (bMod != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwModulus[i] = pdwSeek[i] ^ s_dwSaveLoadXOR[i];
		}
		pdwSeek += SIZE_ENCRYPTION_KEY;
	}

	if (bEnc != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwEncryptionKey[i] = pdwSeek[i] ^ s_dwSaveLoadXOR[i];
		}
		pdwSeek += SIZE_ENCRYPTION_KEY;
	}

	if (bDec != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwDecryptionKey[i] = pdwSeek[i] ^ s_dwSaveLoadXOR[i];
		}
		pdwSeek += SIZE_ENCRYPTION_KEY;
	}

	if (bXOR != FALSE)
	{
		for (int i = 0; i < SIZE_ENCRYPTION_KEY; i++)
		{
			m_dwXORKey[i] = pdwSeek[i] ^ s_dwSaveLoadXOR[i];
		}
	}

	return TRUE;
}

#endif // __ANDROID__
