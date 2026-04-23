///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wsctlc.h"
#include "wsctlc_addon.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#ifdef __ANDROID__
#include <android/log.h>
#define MU_SOCKET_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MUAndroidSocket", __VA_ARGS__)
#endif

CWsctlc::CWsctlc()
{
	m_hWnd = NULL;
	m_bGame = FALSE;
	m_iMaxSockets = 0;
	m_SendBuf[0] = '\0';
	m_nSendBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_nRecvBufLen = 0;
	m_RemoteIp[0] = '\0';
	m_RemotePort = 0;

	m_pPacketQueue = new CPacketQueue;
	m_LogPrint = 0;
	m_logfp = NULL;
	m_socket = INVALID_SOCKET;
#ifdef PBG_LOG_PACKET_WINSOCKERROR
	remove(PACKET_LOG_FILE);
#endif //PBG_LOG_PACKET_WINSOCKERROR
	return;
}

CWsctlc::~CWsctlc()
{
	delete m_pPacketQueue;
	LogPrintOff();
}

void DecryptData(BYTE* lpMsg, int size) // OK
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] ^ GMProtect->EncDecKey[0]) - GMProtect->EncDecKey[1];
	}
}

void EncryptData(BYTE* lpMsg, int size) // OK
{
	for (int n = 0; n < size; n++)
	{
		lpMsg[n] = (lpMsg[n] + GMProtect->EncDecKey[1]) ^ GMProtect->EncDecKey[0];
	}
}

bool CheckSocketPort(SOCKET s) // OK
{
	SOCKADDR_IN addr;

	int addr_len = sizeof(addr);

	if (getpeername(s, (SOCKADDR*)&addr, &addr_len) == SOCKET_ERROR)
	{
		return false;
	}

	if (PORT_RANGE(ntohs(addr.sin_port)) == 0)
	{
		return false;
	}
	return true;
}

int WINAPI MyRecvEnc(SOCKET s, char* buf, int len, int flags) // OK
{
	int result = recv(s, (char*)buf, len, flags);

	if (result == SOCKET_ERROR || result == 0)
	{
		return result;
	}
	if (CheckSocketPort(s) != 0)
	{
		DecryptData((BYTE*)buf, result);
	}
	return result;
}

int WINAPI MySend(SOCKET s, char* buf, int len, int flags) // OK
{
	if (CheckSocketPort(s) != 0)
	{
		EncryptData((BYTE*)buf, len);
	}
	return send(s, buf, len, flags);
}

BOOL CWsctlc::Startup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0)
	{
		g_ErrorReport.Write("Winsock DLL Initialize error.\r\n");
		MessageBox(NULL, "WINSOCK DLL", "Error", MB_OK);
		return FALSE;
	}

	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		WSACleanup();
		g_ErrorReport.Write("Winsock version low.\r\n");
		MessageBox(NULL, "WINSOCK", "Error", MB_OK);
		return FALSE;
	}
	m_socket = INVALID_SOCKET;
	m_iMaxSockets = wsaData.iMaxSockets;
	LogPrintOn();
	return TRUE;
}


BOOL CWsctlc::ShutdownConnection(SOCKET sd)
{
	// Disallow any further data sends.  This will tell the other side
	// that we want to go away now.  If we skip this step, we don't
	// shut the connection down nicely.
	if (shutdown(sd, SD_SEND) == SOCKET_ERROR)
	{
		return false;
	}

	// Receive any extra data still sitting on the socket.  After all
	// data is received, this call will block until the remote host
	// acknowledges the TCP control packet sent by the shutdown above.
	// Then we'll get a 0 back from recv, signalling that the remote
	// host has closed its side of the connection.
	char acReadBuffer[1024];

	while (true)
	{
		int nNewBytes = MyRecvEnc(sd, acReadBuffer, 1024, 0);
		if (nNewBytes == SOCKET_ERROR)
		{
			return false;
		}
		else if (nNewBytes != 0)
		{
			//  cerr << endl << "FYI, received " << nNewBytes <<
			  //        " unexpected bytes during shutdown." << endl;
		}
		else
		{
			// Okay, we're done!
			break;
		}
	}

	// Close the socket.
	if (closesocket(sd) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

void CWsctlc::Cleanup()
{
	WSACleanup();
}

int CWsctlc::Create(HWND hWnd, BOOL bGame)
{
	m_socket = socket(PF_INET, SOCK_STREAM, 0);
	m_bGame = bGame;
	if (m_bGame)
	{
		g_bGameServerConnected = FALSE;
	}

	if (m_socket == INVALID_SOCKET)
	{
		char lpszMessage[128];
		wsprintf(lpszMessage, "WSAGetLastError %d", WSAGetLastError());
		g_ErrorReport.Write(lpszMessage);
		g_ErrorReport.Write("\r\n");
		MessageBox(NULL, lpszMessage, "Error", MB_OK);
		return FALSE;
	}

#ifdef __ANDROID__
	{
		unsigned long nonBlocking = 1;
		if (ioctlsocket(m_socket, 0x8004667e, &nonBlocking) == SOCKET_ERROR)
		{
			g_ErrorReport.Write("[Socket] failed to set non-blocking mode on Android. errno=%d\r\n", WSAGetLastError());
		}
	}
#endif

	m_hWnd = hWnd;
	m_RemoteIp[0] = '\0';
	m_RemotePort = 0;
	return TRUE;
}

BOOL CWsctlc::Close()
{
	SOCKET closingSocket = m_socket;
	m_socket = INVALID_SOCKET;

	if (closingSocket == INVALID_SOCKET)
	{
		return TRUE;
	}

	if (m_bGame)
	{
		if (g_bGameServerConnected == TRUE)
		{
			if (CheckSocketPort(closingSocket) != 0)
			{
				g_pReconnectUI->ReconnectOnCloseSocket();
			}
		}
		g_bGameServerConnected = FALSE;
	}

	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 0;

	int iRetVal = setsockopt(closingSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	if (iRetVal == SOCKET_ERROR)
	{
		WSAGetLastError();
	}

	ZeroMemory(m_RecvBuf, sizeof(MAX_RECVBUF));
	ZeroMemory(m_SendBuf, sizeof(MAX_SENDBUF));

	m_nSendBufLen = 0;
	m_nRecvBufLen = 0;

#ifdef __ANDROID__
	MU_SOCKET_LOG("Close: game=%d remote=%s:%u", m_bGame, m_RemoteIp[0] ? m_RemoteIp : "?", m_RemotePort);
#endif
	m_RemoteIp[0] = '\0';
	m_RemotePort = 0;

	// Clear Packet Queue
	while (!m_pPacketQueue->IsEmpty())
	{
		m_pPacketQueue->PopPacket();
	}
	g_ErrorReport.Write("[Socket Closed][Clear PacketQueue]\r\n");

	closesocket(closingSocket);
	return TRUE;
}

BOOL CWsctlc::Close(SOCKET& socket)
{
	SOCKET closingSocket = socket;
	socket = INVALID_SOCKET;

	if (closingSocket == INVALID_SOCKET)
	{
		return TRUE;
	}

	if (m_bGame)
	{
		g_bGameServerConnected = FALSE;
	}

	closesocket(closingSocket);
	return TRUE;
}

SOCKET CWsctlc::GetSocket()
{
	return m_socket;
}

int CWsctlc::Connect(char* ip_addr, unsigned short port, DWORD WinMsgNum)
{
	sockaddr_in		addr;
	int nResult;
	struct hostent* host = NULL;

	if (m_hWnd == NULL)
	{
		MessageBox(NULL, "Connect Error", "Error", MB_OK);
		return FALSE;
	}
	addr.sin_family = PF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip_addr);

	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(ip_addr);
		if (host == NULL)
		{
			return 2;
		}
		CopyMemory(&addr.sin_addr, host->h_addr_list[0], host->h_length);
	}

	if (addr.sin_addr.s_addr == htonl(INADDR_LOOPBACK))
	{	// local host
		return (FALSE);
	}

	nResult = connect(m_socket, (LPSOCKADDR)&addr, sizeof(addr));
	if (nResult == SOCKET_ERROR)
	{
		int nError = WSAGetLastError();
#ifdef _DEBUG		
		LogPrint("Connect error (%d)", nError);
#endif // _DEBUG
		if (nError != WSAEWOULDBLOCK && nError != WSAEINPROGRESS)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			return FALSE;
		}

		fd_set writeSet;
		fd_set exceptSet;
		FD_ZERO(&writeSet);
		FD_ZERO(&exceptSet);
		FD_SET(m_socket, &writeSet);
		FD_SET(m_socket, &exceptSet);

		timeval connectTimeout;
		connectTimeout.tv_sec = 3;
		connectTimeout.tv_usec = 0;

		int nSelect = select((int)(m_socket + 1), NULL, &writeSet, &exceptSet, &connectTimeout);
		if (nSelect <= 0 || FD_ISSET(m_socket, &exceptSet))
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			return FALSE;
		}

		int nSocketError = 0;
	#ifdef __ANDROID__
		socklen_t nSocketErrorLen = sizeof(nSocketError);
	#else
		int nSocketErrorLen = sizeof(nSocketError);
	#endif
		if (getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char*)&nSocketError, &nSocketErrorLen) == SOCKET_ERROR || nSocketError != 0)
		{
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
			return FALSE;
		}
	}

	timeval timeout;
	timeout.tv_sec = 10; // Tiempo de espera en segundos
	timeout.tv_usec = 0;
	setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
	setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

	nResult = WSAAsyncSelect(m_socket, m_hWnd, WinMsgNum, FD_READ | FD_WRITE | FD_CLOSE);

	if (nResult == SOCKET_ERROR)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		//cLogProc.Add("Client WSAAsyncSelect error %d", WSAGetLastError());
		return FALSE;
	}

	strncpy(m_RemoteIp, ip_addr, sizeof(m_RemoteIp) - 1);
	m_RemoteIp[sizeof(m_RemoteIp) - 1] = '\0';
	m_RemotePort = port;
#ifdef __ANDROID__
	MU_SOCKET_LOG("Connect: game=%d remote=%s:%u", m_bGame, m_RemoteIp, m_RemotePort);
#endif
	return 1;
}

int CWsctlc::sSend(SOCKET socket, char* buf, int len)
{
	int nResult;


	int nLeft = len;
	int nDx = 0;
#ifdef __ANDROID__
	if (len >= 4)
	{
		const BYTE b0 = (BYTE)buf[0];
		const BYTE b1 = (len > 1) ? (BYTE)buf[1] : 0;
		const BYTE b2 = (len > 2) ? (BYTE)buf[2] : 0;
		const BYTE b3 = (len > 3) ? (BYTE)buf[3] : 0;
		const BYTE b4 = (len > 4) ? (BYTE)buf[4] : 0;

		if ((b0 == 0xC1 && b2 == 0xF1) || b0 == 0xC3 || b0 == 0xC4)
		{
			MU_SOCKET_LOG("sSend: game=%d remote=%s:%u len=%d bytes=%02X %02X %02X %02X %02X",
				m_bGame, m_RemoteIp[0] ? m_RemoteIp : "?", m_RemotePort, len, b0, b1, b2, b3, b4);
		}
	}
#endif

	while (1)
	{
		nResult = MySend(socket, (char*)buf + nDx, len - nDx, 0);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				g_ConsoleDebug->Write(MCD_ERROR, "[Send Packet Error] WSAGetLastError() != WSAEWOULDBLOCK");
				g_ErrorReport.Write("[Send Packet Error] WSAGetLastError() != WSAEWOULDBLOCK\r\n");
				Close();
				return FALSE;
			}
			else
			{
				if ((m_nSendBufLen + len) > MAX_SENDBUF)
				{

					g_ConsoleDebug->Write(MCD_ERROR, "Send Packet Error] SendBuffer Overflow");

					g_ErrorReport.Write("[Send Packet Error] SendBuffer Overflow\r\n");
					Close();
					return FALSE;
				}
				memcpy(m_SendBuf + m_nSendBufLen, buf, nLeft);
				m_nSendBufLen += nLeft;
				//LogPrint("send() WSAEWOULDBLOCK : %d", WSAGetLastError());
				return FALSE;
			}
		}
		else
		{
			if (nResult == 0)
			{
				//LogPrint("send()  result is zero", WSAGetLastError());
				break;
			}
			if (m_LogPrint)
			{
				LogHexPrintS((BYTE*)buf, nResult);
			}
		}
		nDx += nResult;
		nLeft -= nResult;
		if (nLeft <= 0)
			break;
	}
	return TRUE;
}

int CWsctlc::FDWriteSend()
{
	int nResult;
	int nDx = 0;

	while (m_nSendBufLen > 0)
	{
		nResult = MySend(m_socket, (char*)m_SendBuf + nDx, m_nSendBufLen - nDx, 0);

		if (nResult == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				g_ErrorReport.Write("FDWriteSend Error 1.\r\n");
				Close();
				return FALSE;
			}
			else
			{
				break;
			}
		}
		else {
			if (nResult <= 0)
			{
				g_ErrorReport.Write("FDWriteSend Error 2.\r\n");
				Close();
				return FALSE;
			}
			if (m_LogPrint)
			{
				LogHexPrintS((BYTE*)m_SendBuf, nResult);
			}
		}
		nDx += nResult;
		m_nSendBufLen -= nResult;
	}
	return TRUE;
}


int CWsctlc::nRecv()
{
	int nResult;

	if (m_nRecvBufLen >= MAX_RECVBUF)
	{
		g_ErrorReport.Write("Receive Packet Buffer Overflow.\r\n");
		return 1;
	}

	nResult = MyRecvEnc(m_socket, (char*)m_RecvBuf + m_nRecvBufLen, MAX_RECVBUF - m_nRecvBufLen, 0);

	if (nResult == 0)
	{
	#ifdef __ANDROID__
		MU_SOCKET_LOG("FD_CLOSE: remote closed connection game=%d remote=%s:%u", m_bGame, m_RemoteIp[0] ? m_RemoteIp : "?", m_RemotePort);
		Close();
	#endif
		return 1;
	}
	if (nResult == SOCKET_ERROR)
	{
		const int nError = WSAGetLastError();
		if (nError == WSAEWOULDBLOCK)
		{
			return 1;
		}
		else {
	#ifdef __ANDROID__
			MU_SOCKET_LOG("FD_CLOSE: socket error=%d game=%d remote=%s:%u", nError, m_bGame, m_RemoteIp[0] ? m_RemoteIp : "?", m_RemotePort);
			Close();
	#endif
	#ifdef _DEBUG
			LogPrint("recv() %d", nError);
	#endif
		}
		return 1;
	}
	m_nRecvBufLen += nResult;

	if (m_nRecvBufLen < 3)
		return 3;

	int lOfs = 0;
	int size = 0;

	while (1)
	{
		if (m_RecvBuf[lOfs] == 0xC1 || m_RecvBuf[lOfs] == 0xC3)
		{
			WSCTLC_LPPBMSG_HEAD lphead = (WSCTLC_LPPBMSG_HEAD)(m_RecvBuf + lOfs);
			size = (int)lphead->size;
		}
		else if (m_RecvBuf[lOfs] == 0xC2 || m_RecvBuf[lOfs] == 0xC4)
		{
			WSCTLC_LPPWMSG_HEAD lphead = (WSCTLC_LPPWMSG_HEAD)(m_RecvBuf + lOfs);
			size = (((int)(lphead->sizeH)) << 8) + lphead->sizeL;
		}
		else {
#ifdef _DEBUG
			LogPrint("Packet Error.(%s %d)", __FILE__, __LINE__);
#endif
			m_nRecvBufLen = 0;
			return FALSE;
		}


		if (size <= 0 || size > MAX_RECVBUF) // Verificaci�n extra de tama�o inv�lido
		{
#ifdef _DEBUG
			LogPrint("size %d", size);
#endif
			m_nRecvBufLen = 0;
			return FALSE;
		}
		else if (size <= m_nRecvBufLen)
		{
			m_pPacketQueue->PushPacket(m_RecvBuf + lOfs, size);

			if (m_LogPrint)
			{
				LogHexPrint((BYTE*)(m_RecvBuf + lOfs), size);
			}
			lOfs += size;
			m_nRecvBufLen -= size;
		}
		else
		{
			if (lOfs > 0)
			{
				memmove(m_RecvBuf, m_RecvBuf + lOfs, m_nRecvBufLen);
			}
			break;
		}
	}

	m_pPacketQueue->ClearGarbage();

	return 0;
}

BYTE* CWsctlc::GetReadMsg()
{
	if (!m_pPacketQueue->IsEmpty())
	{
		CPacket* pPacket = m_pPacketQueue->FrontPacket();
		m_pPacketQueue->PopPacket();
		return pPacket->GetBuffer();
	}
	return NULL;
}

void CWsctlc::LogPrintOn()
{
#ifdef _DEBUG
	m_LogPrint = 1;
	m_logfp = fopen("wsctlc.log", "wt");
#endif
}

void CWsctlc::LogPrintOff()
{
#ifndef _DEBUG
	return;
#endif
	if (m_LogPrint)
	{
		m_LogPrint = 0;
		if (m_logfp != NULL)
			fclose(m_logfp);
	}
}

void CWsctlc::LogHexPrint(BYTE* buf, int size)
{
#ifndef _DEBUG
	return;
#endif
	if (m_LogPrint == 0)
		return;

	if (buf[0] == 0xC1)
	{
		if (buf[2] == 0x26 || buf[2] == 0x27 || buf[2] == 0x28 || buf[2] == 0x2a)
		{
			fprintf(m_logfp, "R 0x%02x %d\n", buf[2], buf[3]);
			return;
		}
		fprintf(m_logfp, "R 0x%02x %d\n", buf[2], buf[3]);
	}
	else
	{
		fprintf(m_logfp, "R 0x%02x %d\n", buf[3], buf[4]);
	}
}

void CWsctlc::LogHexPrintS(BYTE* buf, int size)
{
#ifndef _DEBUG
	return;
#endif
	if (m_LogPrint == 0)
		return;

	if (buf[0] == 0xC1)
	{
		if (buf[2] == 0x26 || buf[2] == 0x27 || buf[2] == 0x28 || buf[2] == 0x2a)
		{
			fprintf(m_logfp, "S 0x%02x %d\n", buf[2], buf[3]);
			return;
		}
		fprintf(m_logfp, "S 0x%02x %d\n", buf[2], buf[3]);
	}
}


void CWsctlc::LogPrint(char* szlog, ...)
{
#ifndef _DEBUG
	return;
#endif
	if (m_LogPrint == 0) return;

	char szBuffer[256] = "";
	va_list		pArguments;

	va_start(pArguments, szlog);
	vsprintf(szBuffer, szlog, pArguments);
	va_end(pArguments);
	fprintf(m_logfp, "%s\n", szBuffer);
}
