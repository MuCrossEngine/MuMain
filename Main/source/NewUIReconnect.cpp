#include "stdafx.h"
#include "CGMProtect.h"
#include "UIControls.h"
#include "CGMCharacter.h"
#include "wsclientinline.h"
#include "NewUIMessageBox.h"
#include "NewUIReconnect.h"
#include "PersonalShopTitleImp.h"
#ifdef __ANDROID__
#include <android/log.h>
#define MU_FLOW_LOG(...) __android_log_print(ANDROID_LOG_INFO, "MUAndroidFlow", __VA_ARGS__)
#endif

extern void UnRegisterBuff(eBuffState buff, OBJECT* o);
extern void RegisterBuff(eBuffState buff, OBJECT* o, const int bufftime = 0);

SEASON3B::CNewUIReconnect::CNewUIReconnect()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
	memset(GameServerAddress, 0, sizeof(GameServerAddress));
	GameServerPort = 0;
	memset(ReconnectAccount, 0, sizeof(ReconnectAccount));
	memset(ReconnectPassword, 0, sizeof(ReconnectPassword));
	memset(ReconnectName, 0, sizeof(ReconnectName));
	memset(ReconnectMapServerAddress, 0, sizeof(ReconnectMapServerAddress));
	ReconnectMapServerPort = 0;
	ReconnectStatus = RECONNECT_STATUS_NONE;
	ReconnectProgress = RECONNECT_PROGRESS_NONE;
	ReconnectCurTime = 0;
	ReconnectMaxTime = 0;
	ReconnectCurWait = 0;
	ReconnectMaxWait = 0;
	ReconnectAuthSend = 0;
	ReconnectHelperOn = 0;
}

SEASON3B::CNewUIReconnect::~CNewUIReconnect()
{
	Release();
}

bool SEASON3B::CNewUIReconnect::Create(CNewUIManager* pNewUIMng, int ix, int iy)
{
	bool Success = false;
	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_RECONNECT, this);

		SetPos(ix, iy);

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUIReconnect::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
	}
}

void SEASON3B::CNewUIReconnect::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

bool SEASON3B::CNewUIReconnect::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIReconnect::UpdateMouseEvent()
{
	return true;
}

bool SEASON3B::CNewUIReconnect::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	this->RenderFrame();

	this->RenderText();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIReconnect::Update()
{
	if (SceneFlag == MAIN_SCENE)
	{
		if (gmProtect->ReconnectTime > 0)
		{
			if (GetReconnectStatus() != RECONNECT_STATUS_RECONNECT && IsVisible())
			{
				this->Show(false);
			}
			if (GetReconnectStatus() == RECONNECT_STATUS_RECONNECT && !IsVisible())
			{
				this->Show(true);
			}

			if (IsVisible() && GetReconnectStatus() == RECONNECT_STATUS_RECONNECT)
			{
				if ((GetTickCount() - GetReconnectMaxTime()) > GetReconnectMaxWait())
				{
					ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT, RECONNECT_PROGRESS_NONE, 0, 0);
					SocketClient.Close();
					return false;
				}

				if ((GetTickCount() - GetReconnectCurTime()) < GetReconnectCurWait())
				{
					return false;
				}

				switch (GetReconnectProgress())
				{
				case RECONNECT_PROGRESS_NONE:
					ReconnecGameServerLoad();
					break;
				case RECONNECT_PROGRESS_CONNECTED:
					ReconnecGameServerAuth();
					break;
				}
				SetReconnectCurTime(GetTickCount());
			}
		}
	}
	return true;
}

float SEASON3B::CNewUIReconnect::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUIReconnect::RenderFrame()
{
	float x = m_Pos.x;
	float y = m_Pos.y;

	float progress = ((GetReconnectMaxWait() == 0) ? 0 : (((GetTickCount() - GetReconnectMaxTime()) * 150.f) / (float)GetReconnectMaxWait()));

	SEASON3B::RenderImage(BITMAP_MESSAGE_WIN, x, y, 352.f, 113.f); //-- background

	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_PROGRESS_BG, x + 96, y + 47, 160.f, 18.f); //-- background

	progress = ((progress > 150.f) ? 150.f : progress);

	SEASON3B::RenderImage(CNewUIMessageBoxMng::IMAGE_MSGBOX_PROGRESS_BAR, x + 101, y + 52, progress, 8.f); //-- background
}

void SEASON3B::CNewUIReconnect::RenderText()
{
	int x = m_Pos.x;
	int y = m_Pos.y;

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(-1);
	g_pRenderText->SetFont(g_hFontBold);

	//char buff[256];
	switch (GetReconnectProgress())
	{
	case RECONNECT_PROGRESS_NONE:
		//wsprintf(buff, gCustomMessage.GetMessage(1));
		g_pRenderText->RenderFont(x, y, "Neu tinh trang nay lien tuc xay ra, vui long lien he Admin qua Zalo de khac phuc...", 352, 55, RT3_SORT_CENTER);
		break;
	case RECONNECT_PROGRESS_CONNECTED:
		//wsprintf(buff, gCustomMessage.GetMessage(2));
		g_pRenderText->RenderFont(x, y, "Logging in...", 352, 55, RT3_SORT_CENTER);
		break;
	case RECONNECT_PROGRESS_JOINED:
		//wsprintf(buff, gCustomMessage.GetMessage(3));
		g_pRenderText->RenderFont(x, y, "Waiting character list...", 352, 55, RT3_SORT_CENTER);
		break;
	case RECONNECT_PROGRESS_CHAR_LIST:
		//wsprintf(buff, gCustomMessage.GetMessage(4));
		g_pRenderText->RenderFont(x, y, "Waiting character info...", 352, 55, RT3_SORT_CENTER);
		break;
	}
}

#include "CAIController.h"

void SEASON3B::CNewUIReconnect::ReconnectOnCloseSocket()
{
	if (gmProtect->ReconnectTime > 0)
	{
		if (SceneFlag == MAIN_SCENE && ReconnectStatus != RECONNECT_STATUS_DISCONNECT)
		{
			ReconnectSetInfo(RECONNECT_STATUS_RECONNECT, RECONNECT_PROGRESS_NONE, 30000, gmProtect->ReconnectTime);//gProtect.m_MainInfo.ReconnectTime);

			ReconnectAuthSend = 0;

			ReconnectHelperOn = gmAIController->IsRunning();

			ReconnectViewportDestroy();

			PartyNumber = 0;

			memcpy(ReconnectName, CharacterAttribute->Name, sizeof(ReconnectName));
		}
	}
}

void SEASON3B::CNewUIReconnect::ReconnectOnMapServerMove(char* address, WORD port)
{
	if (ReconnectStatus != RECONNECT_STATUS_RECONNECT || ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST)
	{
		strcpy_s(ReconnectMapServerAddress, address);

		ReconnectMapServerPort = port;

		ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT, ((ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST) ? ReconnectProgress : RECONNECT_PROGRESS_NONE), 0, 0);
	}
}

void SEASON3B::CNewUIReconnect::ReconnectGetAccountInfo(char* szID, char* szPass)
{
	memcpy(ReconnectAccount, szID, sizeof(ReconnectAccount));

	memcpy(ReconnectPassword, szPass, sizeof(ReconnectPassword));
}

void SEASON3B::CNewUIReconnect::ReconnectOnMapServerMoveAuth(BYTE Value)
{
	if (ReconnectStatus != RECONNECT_STATUS_RECONNECT)
	{
		if (Value == 1)
		{
			ReconnectSetInfo(((ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST) ? ReconnectStatus : RECONNECT_STATUS_NONE), ((ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST) ? ReconnectProgress : RECONNECT_PROGRESS_NONE), 0, 0);
		}
		else
		{
			ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT, RECONNECT_PROGRESS_NONE, 0, 0);
		}
	}
}

void SEASON3B::CNewUIReconnect::ReconnectOnConnectAccount(BYTE Value)
{
	if (ReconnectProgress == RECONNECT_PROGRESS_CONNECTED)
	{
		if (ReconnectAuthSend != 0)
		{
			if (Value == 1)
			{
				int byLanguage = g_pMultiLanguage->GetLanguage();

				SendRequestCharactersList(byLanguage);
				ReconnectSetInfo(RECONNECT_STATUS_RECONNECT, RECONNECT_PROGRESS_JOINED, 30000, 30000);
			}
			else
			{
				if (Value == 3)
				{
					ReconnectSetInfo(RECONNECT_STATUS_RECONNECT, RECONNECT_PROGRESS_CONNECTED, 10000, 30000);
					ReconnectAuthSend = 0;
				}
				else
				{
					ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT, RECONNECT_PROGRESS_NONE, 0, 0);
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CServerLostMsgBoxLayout));
				}
			}
		}
	}
}

void SEASON3B::CNewUIReconnect::ReconnectOnCloseClient(BYTE Value)
{
	if (ReconnectStatus != RECONNECT_STATUS_RECONNECT)
	{
		if (Value == 0 || Value == 2)
		{
			ReconnectSetInfo(RECONNECT_STATUS_DISCONNECT, RECONNECT_PROGRESS_NONE, 0, 0);
		}
	}
}

void SEASON3B::CNewUIReconnect::ReconnectOnCharacterList()
{
	if (ReconnectProgress == RECONNECT_PROGRESS_JOINED)
	{
		SendRequestJoinMapServer(ReconnectName);

		ReconnectSetInfo(RECONNECT_STATUS_RECONNECT, RECONNECT_PROGRESS_CHAR_LIST, 30000, 30000);
	}
}

void SEASON3B::CNewUIReconnect::ReconnectOnCharacterInfo()
{
	if (ReconnectProgress == RECONNECT_PROGRESS_CHAR_LIST)
	{
		if (ReconnectHelperOn != 0)
		{
			SendRequestStartHelper(0);
		}
	}

	ReconnectSetInfo(RECONNECT_STATUS_NONE, RECONNECT_PROGRESS_NONE, 0, 0);
}

void SEASON3B::CNewUIReconnect::ReconnectViewportDestroy()
{
	CHARACTER* c = NULL;

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		c = gmCharacters->GetCharacter(i);

		if (!c->Object.Live) continue;

		int iIndex = g_pPurchaseShopInventory->GetShopCharacterIndex();

		if (iIndex >= 0 && iIndex < MAX_CHARACTERS_CLIENT)
		{
			CHARACTER* pCha = gmCharacters->GetCharacter(iIndex);
			if (pCha && pCha->Key == c->Key)
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);
			}
		}

		int buffSize = g_CharacterBuffSize((&c->Object));

		for (int k = 0; k < buffSize; k++)
		{
			UnRegisterBuff(g_CharacterBuff((&c->Object), k), &c->Object);
		}

		DeleteCharacter(c->Key);

		CHARACTER* pPlayer = FindCharacterTagShopTitle(c->Key);

		if (pPlayer)
		{
			RemoveShopTitle(pPlayer);
		}
	}
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectStatus()
{
	return ReconnectStatus;
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectMaxTime()
{
	return ReconnectMaxTime;
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectMaxWait()
{
	return ReconnectMaxWait;
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectCurTime()
{
	return ReconnectCurTime;
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectCurWait()
{
	return ReconnectCurWait;
}

DWORD SEASON3B::CNewUIReconnect::GetReconnectProgress()
{
	return ReconnectProgress;
}

void SEASON3B::CNewUIReconnect::SetReconnectCurTime(DWORD value)
{
	ReconnectCurTime = value;
}

void SEASON3B::CNewUIReconnect::ReconnecGameServerLoad()
{
	if (ReconnectCreateConnection(GameServerAddress, GameServerPort) != 0)
	{
		g_bGameServerConnected = 1;

		ReconnectSetInfo(RECONNECT_STATUS_RECONNECT, RECONNECT_PROGRESS_CONNECTED, 10000, 30000);
	}
}

void SEASON3B::CNewUIReconnect::ReconnectSetInfo(DWORD status, DWORD progress, DWORD CurWait, DWORD MaxWait)
{
	if (gmProtect->ReconnectTime > 0)
	{
		ReconnectStatus = status;

		ReconnectProgress = progress;

		ReconnectCurTime = GetTickCount();

		ReconnectMaxTime = GetTickCount();

		ReconnectCurWait = CurWait;

		ReconnectMaxWait = MaxWait;

		ReconnectAuthSend = ((status == RECONNECT_STATUS_NONE) ? 0 : ReconnectAuthSend);

		ReconnectHelperOn = ((status == RECONNECT_STATUS_NONE) ? 0 : ReconnectHelperOn);
	}
}

void SEASON3B::CNewUIReconnect::ReconnecGameServerAuth()
{
	if (((ReconnectAuthSend == 0) ? (ReconnectAuthSend++) : ReconnectAuthSend) != 0)
	{
		return;
	}

	SendRequestLogIn(ReconnectAccount, ReconnectPassword);
}

BOOL SEASON3B::CNewUIReconnect::ReconnectCreateConnection(char* address, WORD port)
{
	#ifdef __ANDROID__
	MU_FLOW_LOG("ReconnectCreateConnection: address=%s port=%u", address ? address : "", (unsigned int)port);
	#endif
	if (gmProtect->ReconnectTime > 0)
	{
		if (PORT_RANGE(port) != 0 && GameServerAddress != address)
		{
			if (strcmp(ReconnectMapServerAddress, address) != 0 || ReconnectMapServerPort != port)
			{
				snprintf(GameServerAddress, sizeof(GameServerAddress), "%s", address);

				GameServerPort = port;

				memset(ReconnectMapServerAddress, 0, sizeof(ReconnectMapServerAddress));

				ReconnectMapServerPort = 0;
			}
		}
	}

	return CreateSocket(address, port);
}