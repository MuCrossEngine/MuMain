#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// WinsockAndroidCompat.h
// Shims the WinSock2 API so that WSclient.cpp / SocketSystem.cpp / wsctlc.cpp
// compile and run on Android using POSIX sockets.
// ─────────────────────────────────────────────────────────────────────────────
#ifndef __ANDROID__
#  error "Android only"
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <string.h>

// ─────────────────────────────────────────────────────────────────────────────
// Types
// ─────────────────────────────────────────────────────────────────────────────
typedef int     SOCKET;
typedef struct sockaddr     SOCKADDR;
typedef struct sockaddr*    LPSOCKADDR;
typedef struct sockaddr     SOCKADDR_A;
typedef struct sockaddr_in  SOCKADDR_IN;
typedef struct linger       LINGER;

#define INVALID_SOCKET  ((SOCKET)(-1))
#define SOCKET_ERROR    (-1)

// ─────────────────────────────────────────────────────────────────────────────
// WSA stubs
// ─────────────────────────────────────────────────────────────────────────────
struct WSADATA {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[257];
    char szSystemStatus[129];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
};
inline int  WSAStartup(unsigned short version, WSADATA* data)
{
    if (data)
    {
        memset(data, 0, sizeof(*data));
        data->wVersion = version;
        data->wHighVersion = version;
        data->iMaxSockets = 1024;
    }
    return 0;
}
inline int  WSACleanup()                          { return 0; }
inline int  WSAGetLastError()                     { return errno; }
inline void WSASetLastError(int e)                { errno = e; }

// WSAAsyncSelect is replaced by PollSocketIO() — see AndroidNetworkPollCompat.h
inline int  WSAAsyncSelect(SOCKET, void*, unsigned int, long) { return 0; }

inline int getpeername(SOCKET s, SOCKADDR* name, int* namelen)
{
    if (!namelen) return SOCKET_ERROR;
    socklen_t len = (socklen_t)(*namelen);
    int rc = ::getpeername(s, (struct sockaddr*)name, &len);
    *namelen = (int)len;
    return rc;
}

// ─────────────────────────────────────────────────────────────────────────────
// Socket function aliases (POSIX → WinSock naming)
// ─────────────────────────────────────────────────────────────────────────────
inline int closesocket(SOCKET s) { return close(s); }

// ioctlsocket → fcntl
inline int ioctlsocket(SOCKET s, long cmd, unsigned long* argp)
{
    if (cmd == /*FIONBIO*/ 0x8004667e)
    {
        int flags = fcntl(s, F_GETFL, 0);
        if (*argp) flags |= O_NONBLOCK;
        else       flags &= ~O_NONBLOCK;
        return fcntl(s, F_SETFL, flags);
    }
    return -1;
}

// ─────────────────────────────────────────────────────────────────────────────
// WinSock error codes → POSIX equivalents
// ─────────────────────────────────────────────────────────────────────────────
#define WSAEWOULDBLOCK  EWOULDBLOCK
#define WSAEINPROGRESS  EINPROGRESS
#define WSAEALREADY     EALREADY
#define WSAENOTSOCK     ENOTSOCK
#define WSAECONNREFUSED ECONNREFUSED
#define WSAETIMEDOUT    ETIMEDOUT
#define WSAECONNRESET   ECONNRESET
#define WSAENOTCONN     ENOTCONN
#define WSAEHOSTUNREACH EHOSTUNREACH
#define WSAENETUNREACH  ENETUNREACH
#define WSAEADDRINUSE   EADDRINUSE

#ifndef FD_READ
#  define FD_READ  0x01
#endif
#ifndef FD_CONNECT
#  define FD_CONNECT 0x10
#endif
#ifndef FD_WRITE
#  define FD_WRITE 0x02
#endif
#ifndef FD_CLOSE
#  define FD_CLOSE 0x20
#endif

// ─────────────────────────────────────────────────────────────────────────────
// WinSock constants that differ from POSIX
// ─────────────────────────────────────────────────────────────────────────────
#ifndef SD_RECEIVE
#  define SD_RECEIVE SHUT_RD
#  define SD_SEND    SHUT_WR
#  define SD_BOTH    SHUT_RDWR
#endif
