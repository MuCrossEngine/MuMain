#pragma once
#include "NewUIBase.h"
#include "NewUIManager.h"

enum eReconnectStatus
{
	RECONNECT_STATUS_NONE = 0,
	RECONNECT_STATUS_RECONNECT = 1,
	RECONNECT_STATUS_DISCONNECT = 2,
};

enum eReconnectProgress
{
	RECONNECT_PROGRESS_NONE = 0,
	RECONNECT_PROGRESS_CONNECTED = 1,
	RECONNECT_PROGRESS_JOINED = 2,
	RECONNECT_PROGRESS_CHAR_LIST = 3,
	RECONNECT_PROGRESS_CHAR_INFO = 4,
};

namespace SEASON3B
{
	class CNewUIReconnect : public CNewUIObj
	{
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		char GameServerAddress[16];
		WORD GameServerPort;
		char ReconnectAccount[11];
		char ReconnectPassword[11];
		char ReconnectName[11];
		char ReconnectMapServerAddress[16];
		WORD ReconnectMapServerPort;
		DWORD ReconnectStatus;
		DWORD ReconnectProgress;
		DWORD ReconnectCurTime;
		DWORD ReconnectMaxTime;
		DWORD ReconnectCurWait;
		DWORD ReconnectMaxWait;
		DWORD ReconnectAuthSend;
		DWORD ReconnectHelperOn;
	public:
		CNewUIReconnect();
		virtual~CNewUIReconnect();
		bool Create(CNewUIManager* pNewUIMng, int ix, int iy);
		void Release();
		void SetPos(int x, int y);
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool Render();
		bool Update();
		float GetLayerDepth(); //. 10.5f
	private:
		void RenderFrame();
		void RenderText();
	public:
		DWORD GetReconnectStatus();
		DWORD GetReconnectMaxTime();
		DWORD GetReconnectMaxWait();
		DWORD GetReconnectCurTime();
		DWORD GetReconnectCurWait();
		DWORD GetReconnectProgress();
		void ReconnecGameServerLoad();
		void ReconnecGameServerAuth();
		void SetReconnectCurTime(DWORD value);
		void ReconnectSetInfo(DWORD status, DWORD progress, DWORD CurWait, DWORD MaxWait);
		void ReconnectOnCloseSocket();
		void ReconnectOnMapServerMove(char* address, WORD port);
		void ReconnectOnMapServerMoveAuth(BYTE Value);
		void ReconnectOnConnectAccount(BYTE Value);
		void ReconnectOnCloseClient(BYTE Value);
		void ReconnectOnCharacterList();
		void ReconnectOnCharacterInfo();
		void ReconnectViewportDestroy();
		BOOL ReconnectCreateConnection(char* address, WORD port);

		void ReconnectGetAccountInfo(char* szID, char* szPass);

	};
}