/*******************************************************************************
*	작 성 자 : 진혜진
*	작 성 일 : 2009.06.10
*	내    용 : Download에 필요한 정보 설정
*******************************************************************************/

#pragma once

#ifdef __ANDROID__
#include "Platform/AndroidWin32Compat.h"
#else
#include <Wininet.h>
#endif
#define DL_DEFAULT_BUFFER_SIZE			4096

typedef enum _DownloaderType
{
	FTP,
	HTTP,
} DownloaderType;

class DownloadFileInfo
{
public:
// Constructor, Destructor

	DownloadFileInfo();
	~DownloadFileInfo();

// Get Function
	//			파일 경로 가져오기
	TCHAR *		GetFileName();
	TCHAR *		GetLocalFilePath();
	TCHAR *		GetRemoteFilePath();
	TCHAR *		GetTargetDirPath();
	ULONGLONG	GetFileLength();
// Set Function
	//		파일 경로 설정
	void	SetFilePath(TCHAR * szFileName, 
						TCHAR * szLocalFilePath, 
						TCHAR * szRemoteFilePath,
						TCHAR * szTargerDirPath);
	void	SetFileLength(ULONGLONG uFileLength);

private:
// Member Object

	//			파일 이름
	TCHAR		m_szFileName[MAX_PATH];
	//			로컬 전체 경로
	TCHAR		m_szLocalFilePath[MAX_PATH];
	//			리모트 전체 경로
	TCHAR		m_szRemoteFilePath[INTERNET_MAX_URL_LENGTH];
	//			패치 풀어낼 폴더 경로
	TCHAR		m_szTargerDirPath[MAX_PATH];
	//			파일 사이즈
	ULONGLONG	m_uFileLength;
};

class DownloadServerInfo
{
public:
// Constructor, Destructor

	DownloadServerInfo();
	~DownloadServerInfo();

	
// Get Function

	//				서버 주소 가져오기
	TCHAR *			GetServerURL();
	//				서버 접속 계정 가져오기
	TCHAR *			GetUserID();
	//				서버 접속 계정 비번 가져오기
	TCHAR *			GetPassword();
	//				포트 번호 가져오기
	INTERNET_PORT	GetPort();
	//				타운로드 타입 가져오기
	DownloaderType	GetDownloaderType();
	//				버퍼 사이즈 가져오기
	DWORD			GetReadBufferSize();
	//				커낵트 타임아웃 가져오기
	DWORD			GetConnectTimeout();
	//				덮어쓰기 유무
	BOOL			IsOverWrite();
	//				패시브 모드 유무
	BOOL			IsPassive();


// Set Function

	//				서버 정보 설정
	void			SetServerInfo(TCHAR *			szServerURL, 
								  INTERNET_PORT		nPort,
								  TCHAR *			szUserID, 
								  TCHAR *			szPassword);
	//				다운로드 타입 설정
	void			SetDownloaderType(DownloaderType dwDownloaderType);
	//				버퍼 사이즈 설정
	void			SetReadBufferSize(DWORD dwReadBufferSize);
	//				로컬 파일 존재 시 덮어쓰기 설정
	void			SetOverWrite(BOOL bOverWrite);
	//				패시브 모드 설정
	void			SetPassiveMode(BOOL bPassive);
	//				커낵트 타임아웃 설정
	void			SetConnectTimeout(DWORD dwConnectTimeout);


private:
// Member Object

	// 							Server 주소
	TCHAR						m_szServerURL[INTERNET_MAX_URL_LENGTH];
	// 							접속 계정 명
	TCHAR						m_szUserID[INTERNET_MAX_USER_NAME_LENGTH];	
	// 							접속 계정 Password
	TCHAR						m_szPassword[INTERNET_MAX_PASSWORD_LENGTH];
	// 							접속 포트 default = INTERNET_DEFAULT_FTP_PORT (21)
	INTERNET_PORT				m_nPort;
	//							다운로더 타입 - 프로토콜
	DownloaderType				m_DownloaderType;
	// 							다운로드 패킷 사이즈 제한 default = 4096
	DWORD						m_dwReadBufferSize;
	//							Local File 존재할 경우 덮어쓰기 설정 default = TRUE
	BOOL						m_bOverWrite;
	//							Passive 설정 default = FALSE
	BOOL						m_bPassive;
	//							커낵트 타임아웃
	DWORD						m_dwConnectTimeout;
};

