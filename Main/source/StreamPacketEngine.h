// StreamPacketEngine.h: interface for the CStreamPacketEngine class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define MAX_SPE_BUFFERSIZE	(2048)
#define SIZE_SPE_XORFILTER	(32)

class CStreamPacketEngine
{
public:
	CStreamPacketEngine()
	{
		Clear();
	}
	virtual ~CStreamPacketEngine() {}
	void Clear(void)
	{
		m_wSize = 0;
	}

protected:
	WORD m_wSize;
	BYTE m_byBuffer[MAX_SPE_BUFFERSIZE];
public:
	__forceinline void Init(BYTE byType, BYTE byHeadCode)
	{
		Clear();
		AddData(&byType, sizeof(BYTE), FALSE);
		switch (byType)
		{
		case 0xC1:
			AddData(&m_wSize, 1, FALSE);
			break;
		case 0xC2:
			AddData(&m_wSize, 2, FALSE);
			break;
		default:
			assert("StreamPacketEngine type error(Start)");
			return;
		}
		AddData(&byHeadCode, sizeof(BYTE), FALSE);
	}
	__forceinline void* End(void)
	{
		switch (m_byBuffer[0])
		{
		case 0xC1:
			memcpy(&m_byBuffer[1], &m_wSize, 1);
			break;
		case 0xC2:
			memcpy(&m_byBuffer[1], ((BYTE*)&m_wSize) + 1, 1);
			memcpy(&m_byBuffer[2], &m_wSize, 1);
			break;
		default:
			assert("StreamPacketEngine type error(End)");
			return (NULL);
		}

		return (GetBuffer());
	}
protected:
	__forceinline void XorData(int iStart, int iEnd, int iDir = 1)
	{
		BYTE byXorFilter[SIZE_SPE_XORFILTER] =
		{
			0xAB, 0x11, 0xCD, 0xFE, 0x18, 0x23, 0xC5, 0xA3,
			0xCA, 0x33, 0xC1, 0xCC, 0x66, 0x67, 0x21, 0xF3,
			0x32, 0x12, 0x15, 0x35, 0x29, 0xFF, 0xFE, 0x1D,
			0x44, 0xEF, 0xCD, 0x41, 0x26, 0x3C, 0x4E, 0x4D
		};

		for (int i = iStart; i != iEnd; i += iDir)
		{
			m_byBuffer[i] ^= (m_byBuffer[i - 1] ^ byXorFilter[i % SIZE_SPE_XORFILTER]);
		}
	}
public:
	__forceinline void AddData(void* pSrc, WORD wSize, BOOL bXor = TRUE)
	{
		if (m_wSize + wSize > MAX_SPE_BUFFERSIZE)
		{
			assert("StreamPacketEngine buffer overflow");
			return;
		}
		memcpy(&(m_byBuffer[m_wSize]), pSrc, wSize);
		if (bXor)
		{
			XorData((int)m_wSize, (int)m_wSize + wSize);
		}
		m_wSize += wSize;
	}
	__forceinline void AddNullData(WORD wSize)
	{
		BYTE byBuffer[MAX_SPE_BUFFERSIZE];
		ZeroMemory(byBuffer, wSize);
		AddData(byBuffer, wSize);
	}
	template <class T>
	__forceinline CStreamPacketEngine& operator << (T Data)
	{
		AddData(&Data, sizeof(T));
		return (*this);
	}

	// Getting information
	int GetSize(void)
	{
		return (m_wSize);
	}

	void* GetBuffer(void)
	{
		return (m_byBuffer);
	}

	// Sending packet
	__forceinline void Send(BOOL bEncrypt = FALSE, BOOL bForceC4 = FALSE)
	{
		End();
		SendPacket((char*)m_byBuffer, m_wSize, bEncrypt, bForceC4);
	}
};
