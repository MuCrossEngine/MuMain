// CryptTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#ifndef __ANDROID__
#include <windows.h>
#include <conio.h>
#endif
#include "GCCertification.h"

#include <string>

#ifdef LEM_ADD_GAMECHU

wstring g_ServerKey = L"#GameOn#%@MU@$@2010@";

GCCertificaltionHelper* GCCertificaltionHelper::Instance()
{
	static GCCertificaltionHelper hInstance;     

	//g_ErrorReport.Write(" instance = %d", &hInstance);
	
	return &hInstance;
}

GCCertificaltionHelper::GCCertificaltionHelper() 
{ 
	Init();
}

GCCertificaltionHelper::~GCCertificaltionHelper()
{ 
	Release();
}

void GCCertificaltionHelper::Init( void )
{
	m_wParam				= L"";	
	m_wStatIndex			= L"";
	m_szError				= "";
	m_bGameChu				= false;
	m_hInstGCLauncherDLL	= NULL;
	m_pfCheckCertification	= NULL;

	memset( &m_szParam, 0, MAX_GAMECHU_AUTHINFO );
	memset( &m_szStat, 0, MAX_GAMECHU_USERINFO+1 );
	memset( &m_UserData, 0, sizeof(gcBaseUserInfo) );

#ifdef FOR_WORK
	g_ErrorReport.Write(" [GAMECHU_ INIT]---------------- \0\n");
#endif // FOR_WORK
}
void GCCertificaltionHelper::Release( void )
{
	if(m_hInstGCLauncherDLL)
	{
		::FreeLibrary(m_hInstGCLauncherDLL); 
		
		m_hInstGCLauncherDLL   = NULL;
		m_pfCheckCertification = NULL;
	}
}

#ifdef _DEBUG
void GCCertificaltionHelper::Debug_GameChuMyDataSetBuffer( void )
{
	m_wParam     = L"680476|1232532778|_|24|69863|35cddb8c2a3a97e69da2afa0c67a89b9";
	m_wStatIndex = L"01301985";
}
#endif // _DEBUG


//----------------------------------------------------------------------------------------
// Function: ���� �Ķ���� �����Ѵ�.
// Input   : �Ķ���� ���ڿ�
// Output  : ����� ( 0: ����,			  1: �߸��Ⱦ�ȣȭ��,	10: �ð��ʰ�, 100:�Ľ� ����, 
//					101: �������������, 102: �Ķ���Ͱ�����,  1000: �˼����¿���
//------------------------------------------------------------------------[lem_2010.10.11]-
void GCCertificaltionHelper::Set_GameChuMyData( PSTR _szCmdLine )
{
	wstring	wCmdLine	= L"";
	wstring	wSpace		= L" ";
	wstring::size_type size;
	int		nResult	= 1000;
	int		nLength = 0;
	WCHAR	szTemp[MAX_GAMECHU_AUTHINFO+MAX_GAMECHU_USERINFO] = L"";

#ifdef FOR_WORK
	g_ErrorReport.Write( " >>>>>>>>> PARAM: %s   <<<<<<<<\n", _szCmdLine );
#endif // FOR_WORK

	// ���ڿ� �����ڵ�� ��ȯ
	MultiByteToWideChar(CP_ACP, 0, _szCmdLine, -1, szTemp, strlen(_szCmdLine) );
	wCmdLine = szTemp;
	
	// �Ķ���� �� �и�
 	size			= wCmdLine.find_first_of(wSpace);
	m_wParam		= wCmdLine.substr(0, size);
	m_wStatIndex	= wCmdLine.substr(size+1);

#ifdef _DEBUG
	//Debug_GameChuMyDataSetBuffer();
#endif
	
	// char�� ����
	nLength =	m_wParam.length();
    WideCharToMultiByte(CP_ACP, 0, m_wParam.c_str(), -1, m_szParam, nLength, 0, 0);

	nLength = MAX_GAMECHU_USERINFO;
 	WideCharToMultiByte(CP_ACP, 0, m_wStatIndex.c_str(), -1, m_szStat, nLength, 0, 0);

	nResult			= CheckCertification(m_wParam.c_str(), m_wStatIndex.c_str(), g_ServerKey.c_str(), &m_UserData, 600);

	// nResult ���� 0�̸� ���� �α���, ������ ����Ÿ�ͷ� ������ �����̴�.
	if( nResult == 0 )	m_bGameChu = true;
	else
	{
		Init();
		Set_Error(nResult);
	}

#ifdef FOR_WORK
	g_ErrorReport.Write( " -AuthInfo: %s,  -StatInfo: %s, -bGameChu: %d ----- \n", m_szParam, m_szStat, m_bGameChu );
	g_ErrorReport.Write( " -RETURN :%d, -bGameChu: %d ----- \n", nResult, m_bGameChu );
#endif // FOR_WORK
	//MessageBox( NULL, m_szError.c_str(), "ERROR", MB_OK);
}

void GCCertificaltionHelper::Set_Error( int _nVal )
{
	m_szError	= "";
	switch( _nVal )
	{
		case eDATA_SUCCESS:
			m_szError = " [Gamechu] Login SUCCESS ---- ! ";
		break;
		case eDATA_HASHVALUE:
			m_szError = " [Gamechu] Login Error: eDATA_HASHVALUE 1";
		break; 
		case eDATA_TIMEOUT:
			m_szError = " [Gamechu] Login Error: eDATA_TIMEOUT 10";
		break; 
		case eDATA_PASSINGFAIL:
			m_szError = " [Gamechu] Login Error: eDATA_PASSINGFAIL 100";
		break; 
		case eDATA_USERINFOERROR:
			m_szError = " [Gamechu] Login Error: eDATA_USERINFOERROR 101";
		break; 
		case eDATA_PARAMERROR:
			m_szError = " [Gamechu] Login Error: eDATA_PARAMERROR 102";
		break; 
		case eDATA_UNKNOWNERROR:
			m_szError = " [Gamechu] Login Error: eDATA_UNKNOWNERROR 1000";
		break; 

		default:
			m_szError = " [Gamechu] Login Error: DataLoading Failed";
		break;
	}
}
#endif // LEM_ADD_GAMECHU