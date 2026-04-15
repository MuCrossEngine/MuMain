#include "stdafx.h"
#include "UIManager.h"
#include "GuildCache.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "ZzzLodTerrain.h"
#include "ZzzAI.h"
#include "ZzzTexture.h"
#include "ZzzEffect.h"
#include "ZzzOpenglUtil.h"
#include "ZzzOpenData.h"
#include "ZzzScene.h"
#include "wsclientinline.h"
#include "DSPlaySound.h"
#include "./Utilities/Log/DebugAngel.h"
#include "./Utilities/Log/ErrorReport.h"
#include "./Utilities/Memory/MemoryLock.h"
#include "MatchEvent.h"
#include "GOBoid.h"
#ifdef SAVE_PACKET
#include "./ExternalObject/leaf/stdleaf.h"
#endif // SAVE_PACKET
#include "CSQuest.h"
#include "PersonalShopTitleImp.h"
#include "GMHellas.h"
#include "npcBreeder.h"
#include "GIPetManager.h"
#include "CSMapServer.h"
#include "GMBattleCastle.h"
#include "npcGateSwitch.h"
#include "CComGem.h"
#include "UIMapName.h" // rozy
#include "UIMng.h"
#include "GMCrywolf1st.h"
#include "CDirection.h"
#include "GM_Kanturu_3rd.h"
#include "CSParts.h"
#include "PhysicsManager.h"
#include "Event.h"
#include "MixMgr.h"
#include "MapManager.h"
#include "UIGuardsMan.h"
#include "NewUISystem.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "NewUIInventoryCtrl.h"
#include "w_CursedTemple.h"
#include "SummonSystem.h"
#include "ProtocolSend.h"
#include "CharacterManager.h"
#include "SkillManager.h"

#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM	
#include "GameShop/InGameShopSystem.h"
#include "GameShop/MsgBoxIGSCommon.h"
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
#include "DuelMgr.h"
#include "QuestMng.h"
#include "PortalMgr.h"
#include "w_MapHeaders.h"
#include "w_PetProcess.h"
#include "GambleSystem.h"
#ifdef PBG_ADD_SECRETBUFF
#include "FatigueTimeSystem.h"
#endif //PBG_ADD_SECRETBUFF
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
#include "MonkSystem.h"
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
#include "ServerListManager.h"
#include "CAIController.h"
#include "CGMProtect.h"
#include "CGMHeadChat.h"
#include "ConnectVersionHex.h"
#include "CGMItemDropName.h"
#include "ScaleForm.h"
#include "CGMInvasionManager.h"
#include "CGMResetManager.h"
#include "CGMJewelOfAction.h"
#include "CGMPhysicsManager.h"

extern BYTE m_AltarState[];
extern int g_iChatInputType;
extern BOOL g_bUseChatListBox;

#ifdef WINDOWMODE
extern BOOL g_bUseWindowMode;
#endif //WINDOWMODE
extern int TotalPacketSize;
extern int g_iKeyPadEnable;
extern char DebugTextCount;
extern short g_shCameraLevel;
extern BYTE DebugText[MAX_DEBUG_MAX][256];
extern int  DebugTextLength[MAX_DEBUG_MAX];
extern CUITextInputBox* g_pSingleTextInputBox;
extern CChatRoomSocketList* g_pChatRoomSocketList;

#ifdef _PVP_ADD_MOVE_SCROLL
extern CMurdererMove g_MurdererMove;
#endif	// _PVP_ADD_MOVE_SCROLL

#ifdef _PVP_DYNAMIC_SERVER_TYPE
extern BOOL g_bIsCurrentServerPvP;
#endif	// _PVP_DYNAMIC_SERVER_TYPE
extern int EnableEvent;

extern bool g_PetEnableDuel;
extern CUIMapName* g_pUIMapName; // rozy

MASTER_LEVEL_VALUE	Master_Level_Data;
BYTE Version[SIZE_PROTOCOLVERSION] = { '1' + 1, '0' + 2, '4' + 3, '0' + 4, '5' + 5 };
BYTE Serial[SIZE_PROTOCOLSERIAL + 1] = { "TbYehR2hFUPBKgZj" };
int DirTable[16] = { -1,-1,  0,-1,  1,-1,  1,0,  1,1,  0,1,  -1,1,  -1,0 };

CWsctlc SocketClient;
CWsctlc* g_pSocketClient = &SocketClient;
CSimpleModulus g_SimpleModulusCS;
CSimpleModulus g_SimpleModulusSC;
PMSG_MATCH_RESULT	g_wtMatchResult;
PMSG_MATCH_TIMEVIEW	g_wtMatchTimeLeft;
CROWN_SWITCH_INFO* Switch_Info = NULL;

BYTE g_byPacketSerialSend = 0;
BYTE g_byPacketSerialRecv = 0;
BOOL g_bGameServerConnected = FALSE;

DWORD g_dwLatestMagicTick = 0;

int HeroKey;
int g_iGoalEffect = 0;
int CurrentProtocolState;

char Question[31];
char Password[MAX_ID_SIZE + 1];
char QuestionID[MAX_ID_SIZE + 1];

int  LogIn = 0;
char LogInID[MAX_ID_SIZE + 1] = { 0, };

bool First = false;
int FirstTime = 0;

bool LogOut = false;

steady_clock_ ChatTime = 0;
char ChatText[256];

char ChatWhisperID[MAX_ID_SIZE + 1];


BOOL g_bPacketAfter_EquipmentItem = FALSE;
BYTE g_byPacketAfter_EquipmentItem[256];
void ReceiveTradeExit(BYTE* ReceiveBuffer);

int BuyCost = 0;
int MoveCount = 0;
int EnableUse = 0;
int CurrentSkill = 0;
bool Teleport = false;
int HeroIndex;
int SummonLife = 0;
int SendGetItem = -1;
int SendDropItem = -1;
bool EnableGuildWar = false;
int  GuildWarIndex = -1;
char GuildWarName[8 + 1];
int  GuildWarScore[2];
bool EnableSoccer = false;
BYTE HeroSoccerTeam = 0;
int  SoccerTime;
char SoccerTeamName[2][8 + 1];
bool SoccerObserver = false;

static  const   BYTE    NOT_GET_ITEM = 0xff;
static  const   BYTE    GET_ITEM_ZEN = 0xfe;
static  const   BYTE    GET_ITEM_MULTI = 0xfd;
extern int ItemKey;

extern BOOL Util_CheckOption(char* lpszCommandLine, unsigned char cOption, char* lpszString);

void AddDebugText(unsigned char* Buffer, int Size)
{
	if (DebugTextCount > MAX_DEBUG_MAX - 1)
	{
		DebugTextCount = MAX_DEBUG_MAX - 1;
		for (int i = 1; i < MAX_DEBUG_MAX; i++)
		{
			memcpy(DebugText[i - 1], DebugText[i], DebugTextLength[i]);
			DebugTextLength[i - 1] = DebugTextLength[i];
		}
	}
	if (Size <= 256)
	{
		memcpy(DebugText[DebugTextCount], Buffer, Size);
		DebugTextLength[DebugTextCount] = Size;
		DebugTextCount++;
	}
}

BOOL CreateSocket(char* IpAddr, unsigned short Port)
{
	BOOL bResult = TRUE;

	static bool First;
	if (!First)
	{
		First = true;
		SocketClient.Startup();
#ifdef _DEBUG
		SocketClient.LogPrintOn();
#endif
	}
	g_ErrorReport.Write("[Connect to Server] ip address = %s, port = %d\r\n", IpAddr, Port);

	g_ConsoleDebug->Write(MCD_NORMAL, "[Connect to Server] ip address = %s, port = %d", IpAddr, Port);

	SocketClient.Create(gwinhandle->GethWnd(), TRUE);
	if (SocketClient.Connect(IpAddr, Port, WM_ASYNCSELECTMSG) == FALSE)
	{
		g_ErrorReport.Write("Failed to connect. ");
		g_ErrorReport.WriteCurrentTime();
		CUIMng::Instance().PopUpMsgWin(MESSAGE_SERVER_LOST);
		bResult = FALSE;
	}
	g_byPacketSerialSend = 0;
	g_byPacketSerialRecv = 0;

	return (bResult);
}

void DeleteSocket()
{
	SocketClient.Close();
}

int FindGuildName(char* Name)
{
	for (int i = 0; i < MARK_EDIT; i++)
	{
		MARK_t* p = &GuildMark[i];
		if (strcmp(p->GuildName, Name) == NULL)
		{
			return i;
		}
	}
	return -1;
}

void GuildTeam(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	if (o->Live)
	{
		c->GuildTeam = 0;

		if (Hero->GuildMarkIndex != -1 && c->GuildMarkIndex == Hero->GuildMarkIndex)
			c->GuildTeam = 1;

		if (EnableGuildWar)
		{
			if (GuildWarIndex == -1 && GuildWarName[0])
			{
				GuildWarIndex = FindGuildName(GuildWarName);
			}
			if (GuildWarIndex >= 0 && c->GuildMarkIndex == GuildWarIndex)
			{
				c->GuildTeam = 2;
			}
		}
	}
}

void InitGuildWar()
{
	EnableGuildWar = false;
	GuildWarIndex = -1;
	GuildWarName[0] = NULL;
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c)
		{
			GuildTeam(c);
		}
	}
}

void ReceiveServerList(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
	int Offset = sizeof(PHEADER_DEFAULT_SUBCODE_WORD);

	BYTE Value2 = *(ReceiveBuffer + Offset++);

	int TotalServer = MAKEWORD(Value2, Data->Value);

	g_ServerListManager->Release();

	g_ServerListManager->SetTotalServer(TotalServer);

	for (int i = 0; i < TotalServer; i++)
	{
		LPPRECEIVE_SERVER_LIST Data2 = (LPPRECEIVE_SERVER_LIST)(ReceiveBuffer + Offset);

		g_ServerListManager->InsertServerGroup(Data2->Index, Data2->Percent, Data2->ServerName);

		g_ConsoleDebug->Write(MCD_RECEIVE, "0xF4 [ReceiveServerList(%d %d %d): %s]", i, Data2->Index, Data2->Percent, Data2->ServerName);

		Offset += sizeof(PRECEIVE_SERVER_LIST);
	}

	CUIMng& rUIMng = CUIMng::Instance();
	if (!rUIMng.m_CreditWin.IsShow())
	{
		rUIMng.ShowWin(&rUIMng.m_ServerSelWin);
		rUIMng.m_ServerSelWin.UpdateDisplay();
		rUIMng.ShowWin(&rUIMng.m_LoginMainWin);
	}

#ifdef __ANDROID__
	__android_log_print(ANDROID_LOG_INFO, "MUAndroid", "ReceiveServerList total=%d", TotalServer);
#endif

	g_ErrorReport.Write("Success Receive Server List.\r\n");
	g_ConsoleDebug->Write(MCD_RECEIVE, "0xF4 [ReceiveServerList]");
}

void ReceiveServerConnect(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SERVER_ADDRESS Data = (LPPRECEIVE_SERVER_ADDRESS)ReceiveBuffer;
	char MyAddressIp[16];
	memset(MyAddressIp, 0, 16);
	memcpy(MyAddressIp, (char*)Data->IP, 15);
	g_ErrorReport.Write("[ReceiveServerConnect]");
	SocketClient.Close();

	if (g_pReconnectUI->ReconnectCreateConnection(MyAddressIp, Data->Port))
	{
		g_bGameServerConnected = TRUE;
	}

	char Text[100];
	sprintf(Text, GlobalText[481], MyAddressIp, Data->Port);
	g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE); //Đã kết nối với máy chủ
}

void ReceiveServerConnectBusy(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SERVER_BUSY Data = (LPPRECEIVE_SERVER_BUSY)ReceiveBuffer;
	SendRequestServerList();
}

#ifdef SOCKET_LAUNCHER
void ReceiveServerVersion(BYTE* ReceiveBuffer)
{
	LPPHEADER_SERVER_UPDATE_VERSION Data = (LPPHEADER_SERVER_UPDATE_VERSION)ReceiveBuffer;

	if (Data->state == 0) //-- continen la misma version
	{
		DeleteSocket();

		GMConnectHex->SetUpdateVersion(false);
	}
	else if (Data->state == 1) //-- tienen diferente version y hay que descargar el archivo
	{
		GMConnectHex->write_received_file();

		GMConnectHex->SetUpdateVersion(true);
	}
	else if (Data->state == 3) //-- no existe actualizacion pero el cliente tiene un numero diferente
	{
		DeleteSocket();

		GMConnectHex->SetClientVersion(Data->Version);

		GMConnectHex->SetUpdateVersion(false);
	}
	else if (Data->state == 3) //-- no existe actualizacion pero el cliente tiene un numero diferente
	{
		DeleteSocket();
		gwinhandle->SendPostMessage(WM_DESTROY, 0, 0);
	}
	else
	{
		DeleteSocket();

		GMConnectHex->SetClientVersion(Data->Version);

		GMConnectHex->execute_launcher();
	}
}

void ReceiveServerUpdate(BYTE* ReceiveBuffer)
{
	LPPHEADER_SERVER_VERSION Data = (LPPHEADER_SERVER_VERSION)ReceiveBuffer;

	DWORD totalSize = Data->TotalSize;
	DWORD chunkSize = Data->Size;
	WORD fileVersion = Data->FileVersion;
	DWORD offset = Data->Offset;

	DWORD currentProgress = offset + chunkSize;

	// Calcular el porcentaje
	float percentage = (static_cast<float>(currentProgress) / static_cast<float>(totalSize));

	GMConnectHex->download_information(fileVersion, offset, totalSize, percentage);

	GMConnectHex->write_data_current(fileVersion, offset, chunkSize, totalSize, (ReceiveBuffer + sizeof(PHEADER_SERVER_VERSION)));
}
#endif // SOCKET_LAUNCHER

void ReceiveJoinServer(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_JOIN_SERVER Data2 = (LPPRECEIVE_JOIN_SERVER)ReceiveBuffer;

	if (LogIn != 0)
	{
		HeroKey = ((int)(Data2->NumberH) << 8) + Data2->NumberL;
		g_csMapServer.SendChangeMapServer();
	}
	else
	{
		CUIMng& rUIMng = CUIMng::Instance();

		switch (Data2->Result)
		{
		case 0x01:
			rUIMng.ShowWin(&rUIMng.m_LoginWin);
			HeroKey = ((int)(Data2->NumberH) << 8) + Data2->NumberL;
			CurrentProtocolState = RECEIVE_JOIN_SERVER_SUCCESS;
			break;

		default:
			g_ErrorReport.Write("Connectting error. ");
			g_ErrorReport.WriteCurrentTime();
			rUIMng.PopUpMsgWin(MESSAGE_SERVER_LOST);
			break;
		}
		for (int i = 0; i < SIZE_PROTOCOLVERSION; i++)
		{
			if (Version[i] - (i + 1) != Data2->Version[i])
			{
				rUIMng.HideWin(&rUIMng.m_LoginWin);
				rUIMng.PopUpMsgWin(MESSAGE_VERSION);
				g_ErrorReport.Write("Version dismatch - Join server.\r\n");
			}
		}
	}

	g_GuildCache.Reset();

#if defined _DEBUG || defined FOR_WORK
	if (Data2->Result == 0x01)
	{
		char lpszTemp[256];
		if (Util_CheckOption(GetCommandLine(), 'i', lpszTemp))
		{
			g_ErrorReport.Write("> Try to Login \"%s\"\r\n", m_ID);
			SendRequestLogIn(m_ID, lpszTemp);
		}
	}
#endif
}

void ReceiveConfirmPassword(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CONFIRM_PASSWORD Data = (LPPRECEIVE_CONFIRM_PASSWORD)ReceiveBuffer;

	switch (Data->Result)
	{
	case 0:
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD_FAIL_ID;
		break;
	case 1:
		BuxConvert(Data->Question, sizeof(Data->Question));
		strcpy(Question, (char*)Data->Question);
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD_SUCCESS;
		break;
	}
}

void ReceiveConfirmPassword2(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CONFIRM_PASSWORD2 Data = (LPPRECEIVE_CONFIRM_PASSWORD2)ReceiveBuffer;

	switch (Data->Result)
	{
	case 1:
		BuxConvert(Data->Password, sizeof(Data->Password));
		strcpy(Password, (char*)Data->Password);
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD2_SUCCESS;
		break;
	case 0:
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD2_FAIL_ID;
		break;
	case 2:
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD2_FAIL_ANSWER;
		break;
	case 3:
		CurrentProtocolState = RECEIVE_CONFIRM_PASSWORD2_FAIL_RESIDENT;
		break;
	}
}

void ReceiveChangePassword(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
	switch (Data->Value)
	{
	case 1:
		CurrentProtocolState = RECEIVE_CHANGE_PASSWORD_SUCCESS;
		break;
	case 0:
		CurrentProtocolState = RECEIVE_CHANGE_PASSWORD_FAIL_ID;
		break;
	case 2:
		CurrentProtocolState = RECEIVE_CHANGE_PASSWORD_FAIL_RESIDENT;
		break;
	case 3:
		CurrentProtocolState = RECEIVE_CHANGE_PASSWORD_FAIL_PASSWORD;
		break;
	}
}
CHARACTER_ENABLE g_CharCardEnable;

void ReceiveCharacterList(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_CHARACTER_LIST Data = (LPPHEADER_DEFAULT_CHARACTER_LIST)ReceiveBuffer;

	InitGuildWar();

	SetCreateMaxClass(Data->ClassCode);

	if (SceneFlag == LOG_IN_SCENE)
	{
		if (gmProtect->SceneCharacter == 1)
		{
			gMapManager->currentMap = -1;
		}
		else if (gmProtect->SceneCharacter == 2)
		{
			gMapManager->currentMap = WD_54CHARACTERSCENE;
		}
		else if (gmProtect->SceneCharacter == 3)
		{
			gMapManager->currentMap = WD_78NEW_CHARACTER_SCENE;
		}
		else if (gmProtect->SceneCharacter == 4)
		{
			gMapManager->currentMap = WD_93NEW_CHARACTER_SCENE;
		}
		else
		{
			gMapManager->currentMap = WD_74NEW_CHARACTER_SCENE;
		}
	}

	int Offset = sizeof(PHEADER_DEFAULT_CHARACTER_LIST);

	int character_first = -1;

	for (int i = 0; i < Data->MaxClass; i++)
	{
		LPPRECEIVE_CHARACTER_LIST Data2 = (LPPRECEIVE_CHARACTER_LIST)(ReceiveBuffer + Offset);

		int iClass = gCharacterManager.ChangeServerClassTypeToClientClassType(Data2->Class);

		float fPos[2], fAngle = 0.0f;

		fPos[0] = 0.0;
		fPos[1] = 0.0;

		if (gmProtect->SceneCharacter == 1)
		{
			fPos[0] = (Data2->Index * 100.0);
			fPos[1] = ((Data2->Index * 50.0) - 50.0);
			fAngle = (Data2->Index - 1) * 15.f;
		}
		else if (gmProtect->SceneCharacter == 2)
		{
			switch (Data2->Index)
			{
			case 0:
				fPos[0] = 12880.0f; fPos[1] = 15046.0f; fAngle = 100.0f; break;
			case 1:
				fPos[0] = 12863.0f; fPos[1] = 15251.0f; fAngle = 75.0f; break;
			case 2:
				fPos[0] = 12974.0f; fPos[1] = 15425.0f; fAngle = 50.0f; break;
			case 3:
				fPos[0] = 13151.0f; fPos[1] = 15533.0f; fAngle = 25.0f; break;
			case 4:
				fPos[0] = 13349.6f; fPos[1] = 15527.0f; fAngle = 0.0f; break;
			default:
				return;
			}
		}
		else if (gmProtect->SceneCharacter == 3)
		{
			switch (Data2->Index)
			{
			case 0:
				fPos[0] = 22628.0f;	fPos[1] = 15012.0f;	fAngle = 100.0f; break;
			case 1:
				fPos[0] = 22700.0f;	fPos[1] = 15201.0f;	fAngle = 75.0f; break;
			case 2:
				fPos[0] = 22840.0f;	fPos[1] = 15355.0f;	fAngle = 50.0f; break;
			case 3:
				fPos[0] = 23019.0f;	fPos[1] = 15443.0f;	fAngle = 25.0f; break;
			case 4:
				fPos[0] = 23211.6f;	fPos[1] = 15467.0f;	fAngle = 0.0f; break;
			default:
				return;
			}
		}
		else if (gmProtect->SceneCharacter == 4)
		{
			if (character_first == -1)
			{
				fPos[0] = 13629.f;
				fPos[1] = 12339.f;
				fAngle = 90.f;

				SelectedCharacter = Data2->Index;
				SelectedHero = Data2->Index;
				character_first = Data2->Index;
			}
			else
			{
				fPos[0] = 8046.f;
				fPos[1] = 19400.f;
				fAngle = 90.f;
			}
		}
		else
		{
			switch (Data2->Index)
			{
			case 0:
				fPos[0] = 8008.0f;	fPos[1] = 18885.0f;	fAngle = 115.0f; break;
			case 1:
				fPos[0] = 7986.0f;	fPos[1] = 19145.0f;	fAngle = 90.0f; break;
			case 2:
				fPos[0] = 8046.0f;	fPos[1] = 19400.0f;	fAngle = 75.0f; break;
			case 3:
				fPos[0] = 8133.0f;	fPos[1] = 19645.0f;	fAngle = 60.0f; break;
			case 4:
				fPos[0] = 8282.0f;	fPos[1] = 19845.0f;	fAngle = 35.0f; break;
			default:
				return;
			}
		}

		CHARACTER* c = CreateHero(Data2->Index, iClass, 0, fPos[0], fPos[1], fAngle);

		c->Level = Data2->Level;

		c->m_Reset = Data2->Reset;

		c->m_MasterReset = Data2->MasterReset;

		c->CtlCode = Data2->CtlCode;

		strncpy_s(c->ID, Data2->ID, MAX_ID_SIZE);
		c->ID[MAX_ID_SIZE] = NULL;

		ChangeCharacterExt(Data2->Index, Data2->Equipment);

		c->GuildStatus = Data2->byGuildStatus;

		Offset += sizeof(PRECEIVE_CHARACTER_LIST);
	}

	CurrentProtocolState = RECEIVE_CHARACTERS_LIST;

	g_pStorageExpansion->SetExpansionStorage(Data->ExtWarehouse);

	g_pReconnectUI->ReconnectOnCharacterList();
}

void ReceiveCharacterCard_New(BYTE* ReceiveBuffer)
{
	LPPHEADER_CHARACTERCARD Data = (LPPHEADER_CHARACTERCARD)ReceiveBuffer;

	g_CharCardEnable.bCharacterEnable[0] = false;
	g_CharCardEnable.bCharacterEnable[1] = false;
	g_CharCardEnable.bCharacterEnable[2] = false;
	g_CharCardEnable.bCharacterEnable[3] = false;

	if (Data->CharacterCard & CLASS_DARK_CARD)
		g_CharCardEnable.bCharacterEnable[0] = true;

	if (Data->CharacterCard & CLASS_DARK_LORD_CARD)
		g_CharCardEnable.bCharacterEnable[1] = true;

	if (Data->CharacterCard & CLASS_SUMMONER_CARD)
		g_CharCardEnable.bCharacterEnable[2] = true;

	if (Data->CharacterCard & CLASS_RAGE_CARD)
		g_CharCardEnable.bCharacterEnable[3] = true;

	CUIMng::Instance().m_CharMakeWin.UpdateDisplay();
}

extern void CharacterTeleport(int key);

void ReceiveCreateCharacter(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CREATE_CHARACTER Data = (LPPRECEIVE_CREATE_CHARACTER)ReceiveBuffer;

	if (Data->Result == 1)
	{
		float fPos[2], fAngle = 0.0f;

		fPos[0] = 0.0;
		fPos[1] = 0.0;

		if (gmProtect->SceneCharacter == 1)
		{
			fPos[0] = (Data->Index * 100.0);
			fPos[1] = ((Data->Index * 50.0) - 50.0);
			fAngle = (Data->Index - 1) * 15.f;
		}
		else if (gmProtect->SceneCharacter == 2)
		{
			switch (Data->Index)
			{
			case 0:
				fPos[0] = 12880.0f; fPos[1] = 15046.0f; fAngle = 100.0f; break;
			case 1:
				fPos[0] = 12863.0f; fPos[1] = 15251.0f; fAngle = 75.0f; break;
			case 2:
				fPos[0] = 12974.0f; fPos[1] = 15425.0f; fAngle = 50.0f; break;
			case 3:
				fPos[0] = 13151.0f; fPos[1] = 15533.0f; fAngle = 25.0f; break;
			case 4:
				fPos[0] = 13349.6f; fPos[1] = 15527.0f; fAngle = 0.0f; break;
			default:
				return;
			}
		}
		else if (gmProtect->SceneCharacter == 3)
		{
			switch (Data->Index)
			{
			case 0:
				fPos[0] = 22628.0f;	fPos[1] = 15012.0f;	fAngle = 100.0f; break;
			case 1:
				fPos[0] = 22700.0f;	fPos[1] = 15201.0f;	fAngle = 75.0f; break;
			case 2:
				fPos[0] = 22840.0f;	fPos[1] = 15355.0f;	fAngle = 50.0f; break;
			case 3:
				fPos[0] = 23019.0f;	fPos[1] = 15443.0f;	fAngle = 25.0f; break;
			case 4:
				fPos[0] = 23211.6f;	fPos[1] = 15467.0f;	fAngle = 0.0f; break;
			default:
				return;
			}
		}
		else if (gmProtect->SceneCharacter == 4)
		{
			CharacterTeleport(-1);

			fPos[0] = 13629.f;
			fPos[1] = 12339.f;
			fAngle = 90.f;
			SelectedCharacter = Data->Index;
			SelectedHero = Data->Index;
		}
		else
		{
			switch (Data->Index)
			{
			case 0:
				fPos[0] = 8008.0f;	fPos[1] = 18885.0f;	fAngle = 115.0f; break;
			case 1:
				fPos[0] = 7986.0f;	fPos[1] = 19145.0f;	fAngle = 90.0f; break;
			case 2:
				fPos[0] = 8046.0f;	fPos[1] = 19400.0f;	fAngle = 75.0f; break;
			case 3:
				fPos[0] = 8133.0f;	fPos[1] = 19645.0f;	fAngle = 60.0f; break;
			case 4:
				fPos[0] = 8282.0f;	fPos[1] = 19845.0f;	fAngle = 35.0f; break;
			default:
				return;
			}
		}

		DeleteCharacter(Data->Index);
		CHARACTER* pCharacter = CreateHero(Data->Index, CharacterView.Class, CharacterView.Skin, fPos[0], fPos[1], fAngle);

		if (pCharacter)
		{
			pCharacter->Level = Data->Level;
			pCharacter->m_Reset = 0;
			pCharacter->m_MasterReset = 0;

			int iClass = gCharacterManager.ChangeServerClassTypeToClientClassType(Data->Class);

			pCharacter->Class = iClass;

			strncpy_s(pCharacter->ID, Data->ID, MAX_ID_SIZE);

			pCharacter->ID[MAX_ID_SIZE] = NULL;
		}

		CurrentProtocolState = RECEIVE_CREATE_CHARACTER_SUCCESS;
		CUIMng& rUIMng = CUIMng::Instance();
		rUIMng.CloseMsgWin();
		rUIMng.m_CharSelMainWin.UpdateDisplay();
		rUIMng.m_CharInfoBalloonMng.UpdateDisplay();
	}
	else if (Data->Result == 0)
	{
		CUIMng::Instance().PopUpMsgWin(RECEIVE_CREATE_CHARACTER_FAIL);
	}
	else if (Data->Result == 2)
	{
		CUIMng::Instance().PopUpMsgWin(RECEIVE_CREATE_CHARACTER_FAIL2);
	}
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x01 [ReceiveCreateCharacter]");
}

void ReceiveDeleteCharacter(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;

	switch (Data->Value)
	{
	case 1:
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedHero);
		if (pCharacter)
		{
			int iKey = pCharacter->Key;
			CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_SUCCESS);
			DeleteCharacter(iKey);
		}
	}
	break;
	case 0:
		CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_GUILDWARNING);
		break;
	case 3:
		CUIMng::Instance().PopUpMsgWin(MESSAGE_DELETE_CHARACTER_ITEM_BLOCK);
		break;
	case 2:
	default:
		CUIMng::Instance().PopUpMsgWin(MESSAGE_STORAGE_RESIDENTWRONG);
		break;
	}
}


extern void StopMusic();

void InitGame()
{

	EnableUse = 0;
	SendGetItem = -1;
	SummonLife = 0;
	SoccerTime = 0;
	SoccerObserver = false;

	g_wtMatchResult.Clear();
	g_wtMatchTimeLeft.m_Time = 0;
	g_iGoalEffect = 0;

	g_shCameraLevel = 0;

	SelectedNpc = -1;
	SelectedOperate = -1;
	SelectedCharacter = -1;
	SelectedItem = -1;

	Attacking = -1;
	g_pOption->SetAutoAttack(true);
	g_pOption->SetWhisperSound(false);

	CheckInventory = NULL;

	SendExitInventory();

	g_iFollowCharacter = -1;

	LockInputStatus = false;

	RepairEnable = 0;
	CheckSkill = -1;

	ClearNotice();

	CharacterAttribute->Ability = 0;
	CharacterAttribute->AbilityTime[0] = 0;
	CharacterAttribute->AbilityTime[1] = 0;
	CharacterAttribute->AbilityTime[2] = 0;

	CharacterMachine->StorageGold = 0;
	CharacterMachine->Gold = 0;

	g_shEventChipCount = 0;
	g_shMutoNumber[0] = -1;
	g_shMutoNumber[1] = -1;
	g_shMutoNumber[2] = -1;

	ClearWhisperID();

	matchEvent::ClearMatchInfo();

	InitPartyList();

	g_csQuest.clearQuest();

	g_DuelMgr.Reset();
	g_pNewUISystem->Hide(SEASON3B::INTERFACE_DUEL_WINDOW);

	if (g_pUIManager)
		g_pUIManager->Init();

	if (g_pSiegeWarfare)
		g_pSiegeWarfare->InitMiniMapUI();

	g_Direction.Init();
	g_Direction.DeleteMonster();

	RemoveAllPerosnalItemPrice(PSHOPWNDTYPE_SALE);
	RemoveAllPerosnalItemPrice(PSHOPWNDTYPE_PURCHASE);

	g_pNewUIHotKey->SetStateGameOver(false);
	g_pMyShopInventory->ResetSubject();
	g_pChatListBox->ResetFilter();

	g_pGuildInfoWindow->NoticeClear();

	//CameraFactorPtr->DefaultValues();
}

BOOL ReceiveLogOut(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LogOut = false;
	LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;

	g_pReconnectUI->ReconnectOnCloseClient(Data->Value);

	switch (Data->Value)
	{
	case 0:
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		break;
	case 1:
		StopMusic();
		AllStopSound();

		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

		ReleaseMainData();
		CryWolfMVPInit();

		SceneFlag = CHARACTER_SCENE;

		SendRequestCharactersList(g_pMultiLanguage->GetLanguage());

		InitCharacterScene = false;
		InitMainScene = false;
		EnableMainRender = false;
		CurrentProtocolState = REQUEST_JOIN_SERVER;
		InitGame();
		break;
	case 2:
		if (SceneFlag == MAIN_SCENE)
		{
			CryWolfMVPInit();
			StopMusic();
			AllStopSound();
			SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
			ReleaseMainData();
		}
		g_ErrorReport.Write("[ReceiveLogOut]");
		SocketClient.Close();

		ReleaseCharacterSceneData();
		SceneFlag = LOG_IN_SCENE;

		InitLogIn = false;
		InitCharacterScene = false;
		InitMainScene = false;
		EnableMainRender = false;
		CurrentProtocolState = REQUEST_JOIN_SERVER;

		LogIn = 0;
		g_csMapServer.Init();
		InitGame();
		break;
	case 3:
		SendRequestServerOff(REQUEST_CHARACTER_OFF_ATTK);

		if (SceneFlag == MAIN_SCENE)
		{
			CryWolfMVPInit();
			StopMusic();
			AllStopSound();
			SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
			ReleaseMainData();
		}

		ReleaseCharacterSceneData();

		SceneFlag = LOG_IN_SCENE;

		InitLogIn = false;
		InitCharacterScene = false;
		InitMainScene = false;
		EnableMainRender = false;
		CurrentProtocolState = REQUEST_JOIN_SERVER;

		LogIn = 0;
		g_csMapServer.Init();
		InitGame();
		SocketClient.Close();
		break;
	case 4:
		SendRequestServerOff(REQUEST_CHARACTER_OFF_STOR);

		if (SceneFlag == MAIN_SCENE)
		{
			CryWolfMVPInit();
			StopMusic();
			AllStopSound();
			SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
			ReleaseMainData();
		}

		ReleaseCharacterSceneData();
		SceneFlag = LOG_IN_SCENE;

		InitLogIn = false;
		InitCharacterScene = false;
		InitMainScene = false;
		EnableMainRender = false;
		CurrentProtocolState = REQUEST_JOIN_SERVER;

		LogIn = 0;
		g_csMapServer.Init();
		InitGame();
		SocketClient.Close();
		break;
	}

	g_pWindowMgr->Reset();
	g_pFriendList->ClearFriendList();
	g_pLetterList->ClearLetterList();

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x02 [ReceiveServerList(%d)]", Data->Value);

	return (TRUE);
}

__int64 ConvertByteToLong(BYTE* btBuff)
{
	__int64 Data = 0x0000000000000000;

	Data |= btBuff[0];
	Data <<= 8;
	Data |= btBuff[1];
	Data <<= 8;
	Data |= btBuff[2];
	Data <<= 8;
	Data |= btBuff[3];
	Data <<= 8;
	Data |= btBuff[4];
	Data <<= 8;
	Data |= btBuff[5];
	Data <<= 8;
	Data |= btBuff[6];
	Data <<= 8;
	Data |= btBuff[7];

	return Data;
}

BOOL ReceiveJoinMapServer(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	MouseLButton = false;

	LPPRECEIVE_JOIN_MAP_SERVER Data = (LPPRECEIVE_JOIN_MAP_SERVER)ReceiveBuffer;

	__int64 Data_Exp = 0x0000000000000000;
	Master_Level_Data.lMasterLevel_Experince = Data_Exp;

	Data_Exp = ConvertByteToLong(Data->btMExp);

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
		Master_Level_Data.lMasterLevel_Experince = Data_Exp;
	else
		CharacterAttribute->Experience = (int)Data_Exp;

	Data_Exp = 0x0000000000000000;
	Master_Level_Data.lNext_MasterLevel_Experince = Data_Exp;

	Data_Exp = ConvertByteToLong(Data->btMNextExp);

	CharacterAttribute->MaxCharacterLevel = Data->MaxCharacterLevel;

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
		Master_Level_Data.lNext_MasterLevel_Experince = Data_Exp;
	else
		CharacterAttribute->NextExperince = (int)Data_Exp;

	CharacterAttribute->LevelUpPoint = Data->btLevelUpPoint;
	CharacterAttribute->Strength = Data->btStrength;
	CharacterAttribute->Dexterity = Data->btDexterity;
	CharacterAttribute->Vitality = Data->btVitality;
	CharacterAttribute->Energy = Data->btEnergy;
	CharacterAttribute->Charisma = Data->btCharisma;
	//--
	CharacterAttribute->Life = Data->btLife;
	CharacterAttribute->LifeMax = Data->btLifeMax;
	CharacterAttribute->Mana = Data->btMana;
	CharacterAttribute->ManaMax = Data->btManaMax;
	CharacterAttribute->SkillMana = Data->btSkillMana;
	CharacterAttribute->SkillManaMax = Data->btSkillManaMax;
	CharacterAttribute->Shield = Data->btShield;
	CharacterAttribute->ShieldMax = Data->btShieldMax;
	//--
	CharacterAttribute->Ability = 0;
	CharacterAttribute->AbilityTime[0] = 0;
	CharacterAttribute->AbilityTime[1] = 0;
	CharacterAttribute->AbilityTime[2] = 0;
	CharacterAttribute->AddPoint = Data->AddPoint;
	CharacterAttribute->MaxAddPoint = Data->MaxAddPoint;
	CharacterAttribute->wMinusPoint = Data->wMinusPoint;
	CharacterAttribute->wMaxMinusPoint = Data->wMaxMinusPoint;
	CharacterMachine->Gold = Data->Gold;
	CharacterMachine->AccountLevel = Data->AccountLevel;

	if (Data->PenaltyTime != 0xFFFFFFFF)
	{
		CharacterAttribute->PenaltyTimePK = Data->PenaltyTime;
	}
	else
	{
		CharacterAttribute->PenaltyTimePK = 0;
	}

	if (GMProtect->IsInvExt())
		g_pInvenExpansion->SetEnableExpansionInven(Data->InventoryExt, Data->PriceInventoryExt);
	else
		g_pInvenExpansion->SetEnableExpansionInven(0, 0);

	g_pEnterBloodCastle->CreateEntrace();

	g_pEnterDevilSquare->CreateEntrace();

	g_pCursedTempleEnterWindow->CreateEntrace();

	gMapManager->currentMap = Data->Map;

	gMapManager->LoadWorld(gMapManager->currentMap);

	if (gMapManager->currentMap == WD_34CRYWOLF_1ST)
	{
		SendRequestCrywolfInfo();
	}

	matchEvent::CreateEventMatch(gMapManager->currentMap);

	HeroIndex = rand() % MAX_CHARACTERS_CLIENT;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(HeroIndex);

	if (pCharacter)
	{
		CreateCharacterPointer(pCharacter, MODEL_PLAYER, Data->PositionX, Data->PositionY, ((float)Data->Angle - 1.f) * 45.f);
		pCharacter->Key = HeroKey;
		OBJECT* o = &pCharacter->Object;
		pCharacter->Class = CharacterAttribute->Class;
		pCharacter->Skin = 0;
		pCharacter->PK = Data->PK;
		pCharacter->CtlCode = Data->CtlCode;
		o->Kind = KIND_PLAYER;
		SetCharacterClass(pCharacter);
		//pCharacter->SetIDColor();

		Hero = pCharacter;

		memcpy(pCharacter->ID, (char*)CharacterAttribute->Name, MAX_ID_SIZE);

		for (int i = 0; i < MAX_NEW_EQUIPMENT; ++i)
		{
			CharacterMachine->Equipment[i].Type = -1;
			CharacterMachine->Equipment[i].Level = 0;
			CharacterMachine->Equipment[i].Option1 = 0;
		}
		pCharacter->ID[MAX_ID_SIZE] = NULL;
		CreateEffect(BITMAP_MAGIC + 2, o->Position, o->Angle, o->Light, 0, o);

		CurrentProtocolState = RECEIVE_JOIN_MAP_SERVER;

		LockInputStatus = false;
		CheckIME_Status(true, 0);

		LoadingWorld = 30;
		MouseUpdateTime = 0;
		MouseUpdateTimeMax = 6;

		CreatePetDarkSpirit_Now(Hero);

		CreateEffect(BITMAP_MAGIC + 2, o->Position, o->Angle, o->Light, 0, o);
		o->Alpha = 0.f;

		g_pNewUICommandServer->ReOrganizer();
	}

	g_pNewUISystem->HideAll();

	SelectedItem = -1;
	SelectedNpc = -1;
	SelectedCharacter = -1;
	SelectedOperate = -1;
	Attacking = -1;
	RepairEnable = 0;

	Hero->Movement = false;
	SetPlayerStop(Hero);

	if (gMapManager->InBloodCastle() == false)
	{
		StopBuffer(SOUND_BLOODCASTLE, true);
	}

	if (gMapManager->InChaosCastle() == false)
	{
		StopBuffer(SOUND_CHAOSCASTLE, true);
		StopBuffer(SOUND_CHAOS_ENVIR, true);
	}

	if (gMapManager->IsEmpireGuardian1() == false &&
		gMapManager->IsEmpireGuardian2() == false &&
		gMapManager->IsEmpireGuardian3() == false &&
		gMapManager->IsEmpireGuardian4() == false)
	{
		StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_RAIN, true);
		StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_FOG, true);
		StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_STORM, true);
		StopBuffer(SOUND_EMPIREGUARDIAN_INDOOR_SOUND, true);
	}

#if MAIN_UPDATE > 303
	if (gCharacterManager.IsMasterLevel(Hero->Class) == true)
	{
		//-- aqui un codigo de master skill
		g_pMasterSkillTree->SetMasterType(gCharacterManager.GetCharacterClass(Hero->Class));
	}
#endif // MAIN_UPDATE != 303

	g_pUIMapName->ShowMapName();

	CreateMyGensInfluenceGroundEffect();

	if (!(gMapManager->currentMap >= WD_65DOPPLEGANGER1 && gMapManager->currentMap <= WD_68DOPPLEGANGER4))
	{
		char Text[256];
		sprintf(Text, "%s%s", GlobalText[484], gMapManager->GetMapName(gMapManager->currentMap));

		g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE); //Chào mừng bạn đến với 
	}

	if (gMapManager->currentMap == WD_30BATTLECASTLE)
	{
		if (g_pSiegeWarfare)
		{
			g_pSiegeWarfare->CreateMiniMapUI();
		}
	}

	if (gMapManager->currentMap < WD_65DOPPLEGANGER1 || gMapManager->currentMap > WD_68DOPPLEGANGER4)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_DOPPELGANGER_FRAME);
	}

	if (gMapManager->currentMap < WD_69EMPIREGUARDIAN1 || WD_72EMPIREGUARDIAN4 < gMapManager->currentMap)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER);
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x03 [ReceiveJoinMapServer]");

	g_pReconnectUI->ReconnectOnCharacterInfo();

	return (TRUE);
}

void ReceiveRevival(BYTE* ReceiveBuffer)
{
	MouseLButton = false;
	Teleport = false;
	Hero->Object.Live = false;
	LPPRECEIVE_REVIVAL Data = (LPPRECEIVE_REVIVAL)ReceiveBuffer;

#ifdef PROTO_EXTRA
	CharacterAttribute->Life = Data->btLife;
	CharacterAttribute->Mana = Data->btMana;
	CharacterAttribute->Shield = Data->btShield;
	CharacterAttribute->SkillMana = Data->btSkillMana;
#else
	CharacterAttribute->Life = Data->Life;
	CharacterAttribute->Mana = Data->Mana;
	CharacterAttribute->Shield = Data->Shield;
	CharacterAttribute->SkillMana = Data->SkillMana;
#endif // PROTO_EXTRA

	__int64 Data_Exp = 0x0000000000000000;
	Master_Level_Data.lMasterLevel_Experince = Data_Exp;

	Data_Exp = ConvertByteToLong(Data->btMExp);

	if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		Master_Level_Data.lMasterLevel_Experince = Data_Exp;
	}
	else
	{
		CharacterAttribute->Experience = (int)Data_Exp;
	}

	CharacterMachine->Gold = Data->Gold;

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		gmCharacters->GetCharacter(i)->Object.Live = false;
	}

	BYTE Temp = Hero->PK;
	int TempGuild = Hero->GuildMarkIndex;

	OBJECT* o;
	CHARACTER* pCharacter = gmCharacters->GetCharacter(HeroIndex);

	if (pCharacter)
	{
		o = &pCharacter->Object;
		BYTE BackUpGuildStatus = pCharacter->GuildStatus;
		BYTE BackUpGuildType = pCharacter->GuildType;
		BYTE BackUpGuildRelationShip = pCharacter->GuildRelationShip;
		BYTE byBackupEtcPart = pCharacter->EtcPart;

		CreateCharacterPointer(pCharacter, MODEL_PLAYER, Data->PositionX, Data->PositionY, ((float)Data->Angle - 1.f) * 45.f);
		pCharacter->Key = HeroKey;
		pCharacter->GuildStatus = BackUpGuildStatus;
		pCharacter->GuildType = BackUpGuildType;
		pCharacter->GuildRelationShip = BackUpGuildRelationShip;
		pCharacter->EtcPart = byBackupEtcPart;

		pCharacter->Class = CharacterAttribute->Class;
		pCharacter->Skin = 0;
		pCharacter->PK = Temp;
		o->Kind = KIND_PLAYER;
		pCharacter->GuildMarkIndex = TempGuild;
		pCharacter->SafeZone = true;
		SetCharacterClass(pCharacter);

		//pCharacter->SetIDColor();

#ifdef PK_ATTACK_TESTSERVER_LOG
		PrintPKLog(c);
#endif // PK_ATTACK_TESTSERVER_LOG

		SetPlayerStop(pCharacter);
		CreateEffect(BITMAP_MAGIC + 2, o->Position, o->Angle, o->Light, 0, o);
	}
	ClearItems();
	ClearCharacters(HeroKey);
	RemoveAllShopTitleExceptHero();

	if (gMapManager->currentMap >= WD_45CURSEDTEMPLE_LV1 && gMapManager->currentMap <= WD_45CURSEDTEMPLE_LV6)
	{
		if (!(Data->Map >= WD_45CURSEDTEMPLE_LV1 && Data->Map <= WD_45CURSEDTEMPLE_LV6))
		{
			g_CursedTemple->ResetCursedTemple();
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM);
		}
	}

	if (gMapManager->currentMap != Data->Map)
	{
		int OldWorld = gMapManager->currentMap;

		gMapManager->currentMap = Data->Map;
		gMapManager->LoadWorld(gMapManager->currentMap);

		if ((gMapManager->InChaosCastle(OldWorld) == true && OldWorld != gMapManager->currentMap)
			|| gMapManager->InChaosCastle() == true)
		{
			SetCharacterClass(Hero);
		}

		if (gMapManager->InChaosCastle() == false)
		{
			StopBuffer(SOUND_CHAOSCASTLE, true);
			StopBuffer(SOUND_CHAOS_ENVIR, true);
		}

		if (gMapManager->IsEmpireGuardian1() == false &&
			gMapManager->IsEmpireGuardian2() == false &&
			gMapManager->IsEmpireGuardian3() == false &&
			gMapManager->IsEmpireGuardian4() == false)
		{
			StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_RAIN, true);
			StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_FOG, true);
			StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_STORM, true);
			StopBuffer(SOUND_EMPIREGUARDIAN_INDOOR_SOUND, true);
		}

		if (pCharacter)
		{
			if (gMapManager->currentMap == -1 || pCharacter->Helper.Type != MODEL_HELPER + 3 || pCharacter->SafeZone)
			{
				o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
			}
			else
			{
				if (gMapManager->currentMap == WD_8TARKAN || gMapManager->currentMap == WD_10HEAVEN)
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 90.f;
				else
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
			}
		}
	}

	if (pCharacter)
	{
		CreatePetDarkSpirit_Now(pCharacter);
		GuildTeam(pCharacter);
	}
	SummonLife = 0;

	g_pUIMapName->ShowMapName();	// rozy

	CreateMyGensInfluenceGroundEffect();

	if (gMapManager->currentMap < WD_65DOPPLEGANGER1 || gMapManager->currentMap > WD_68DOPPLEGANGER4)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_DOPPELGANGER_FRAME);
	}
	if (gMapManager->currentMap < WD_69EMPIREGUARDIAN1 || WD_72EMPIREGUARDIAN4 < gMapManager->currentMap)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER);
	}

	g_pNewUISystem->HideAll();

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x04 [ReceiveRevival]");
}

void ReceiveMagicList(BYTE* ReceiveBuffer)
{
	int Master_Skill_Bool = -1;
	int Skill_Bool = -1;

	LPPHEADER_MAGIC_LIST_COUNT Data = (LPPHEADER_MAGIC_LIST_COUNT)ReceiveBuffer;
	int Offset = sizeof(PHEADER_MAGIC_LIST_COUNT);
	if (Data->Value == 0xFF)
	{
		LPPRECEIVE_MAGIC_LIST Data2 = (LPPRECEIVE_MAGIC_LIST)(ReceiveBuffer + Offset);
		CharacterAttribute->Skill[Data2->Index] = 0;
	}
	else if (Data->Value == 0xFE)
	{
		LPPRECEIVE_MAGIC_LIST Data2 = (LPPRECEIVE_MAGIC_LIST)(ReceiveBuffer + Offset);
		CharacterAttribute->Skill[Data2->Index] = Data2->Type;
	}
	else if (Data->ListType == 0x02)
	{
		for (int i = 0; i < Data->Value; ++i)
		{
			LPPRECEIVE_MAGIC_LIST Data2 = (LPPRECEIVE_MAGIC_LIST)(ReceiveBuffer + Offset);
			CharacterAttribute->Skill[Data2->Index] = 0;
		}
	}
	else
	{
		if (Data->ListType == 0x00)
		{
			ZeroMemory(CharacterAttribute->Skill, MAX_SKILLS * sizeof(WORD));
		}
		for (int i = 0; i < Data->Value; i++)
		{
			LPPRECEIVE_MAGIC_LIST Data2 = (LPPRECEIVE_MAGIC_LIST)(ReceiveBuffer + Offset);
			CharacterAttribute->Skill[Data2->Index] = Data2->Type;
			Offset += sizeof(PRECEIVE_MAGIC_LIST);
		}
		if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD)
		{
			for (int i = 0; i < PET_CMD_END; ++i)
			{
				CharacterAttribute->Skill[AT_PET_COMMAND_DEFAULT + i] = AT_PET_COMMAND_DEFAULT + i;
			}
		}
	}

	CharacterAttribute->SkillNumber = 0;
	CharacterAttribute->SkillMasterNumber = 0;

	int SkillType = 0;
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		SkillType = CharacterAttribute->Skill[i];
		if (SkillType != 0)
		{
			CharacterAttribute->SkillNumber = i + 1;
			BYTE SkillUseType = SkillAttribute[SkillType].SkillUseType;
			if (SkillUseType == SKILL_USE_TYPE_MASTER)
			{
				CharacterAttribute->SkillMasterNumber++;
			}
		}
	}
	if (Hero->CurrentSkill >= CharacterAttribute->SkillNumber)
		Hero->CurrentSkill = 0;
	if (CharacterAttribute->SkillNumber == 1)
		Hero->CurrentSkill = 0;


	if (!CharacterAttribute->Skill[Hero->CurrentSkill])
	{
		Hero->CurrentSkill = 0;
	}

	int Skill = 0;
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		Skill = CharacterAttribute->Skill[Hero->CurrentSkill];
		if (Skill >= AT_SKILL_STUN && Skill <= AT_SKILL_REMOVAL_BUFF)
		{
			Hero->CurrentSkill++;
		}
		else
		{
			if (CharacterAttribute->Skill[Hero->CurrentSkill])
			{
				return;
			}
			Hero->CurrentSkill++;
		}
	}
}

BOOL ReceiveInventory(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
	{
		CharacterMachine->Equipment[i].Type = -1;
		CharacterMachine->Equipment[i].Option1 = 0;
	}

	g_pMyInventory->UnequipAllItems();
	g_pMyInventory->DeleteAllItems();
	g_pMyShopInventory->DeleteAllItems();

	if (bEncrypted)
	{
		LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer; //LPPHEADER_DEFAULT_SUBCODE_WORD 6byte
		int Offset = sizeof(PHEADER_DEFAULT_SUBCODE_WORD);
		gGoboidManager->DeleteBug(&Hero->Object);
		giPetManager::DeletePet(Hero);
		ThePetProcess().DeletePet(Hero);

		for (int i = 0; i < Data->Value; i++)
		{
			LPPRECEIVE_INVENTORY Data2 = (LPPRECEIVE_INVENTORY)(ReceiveBuffer + Offset); //LPPRECEIVE_INVENTORY 8byte

			SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

			int target_index = Data2->Index;

			if (CharacterMachine->CheckEquipment(target_index) == false)
			{
				if (target_index < MAX_EQUIPMENT_INDEX || target_index >= MAX_MY_INVENTORY_INDEX)
				{
					if (target_index >= MAX_MY_INVENTORY_INDEX && target_index < MAX_MY_SHOP_INVENTORY_INDEX)
					{
						g_pMyShopInventory->InsertItem(target_index - MAX_MY_INVENTORY_INDEX, Data2->Item);
					}
				}
				else
				{
					g_pMyInventory->InsertItem(target_index - MAX_EQUIPMENT_INDEX, Data2->Item);
				}
			}
			else
			{
				g_pMyInventory->EquipItem(target_index, Data2->Item);
			}
			Offset += sizeof(PRECEIVE_INVENTORY);
		}

		CharacterMachine->CalculateAll();
		SetAttackSpeed();
		//SetHotKeyReset();
	}
	else
	{
		SendHackingChecked(0, 243);
	}
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x10 [ReceiveInventory]");

	return bEncrypted;
}

void ReceiveDeleteInventory(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
	if (Data->SubCode != 0xff)
	{
		int itemindex = Data->SubCode;

		if (CharacterMachine->CheckEquipment(itemindex) == true)
		{
			g_pMyInventory->UnequipItem(itemindex);
		}
		else
		{
			if (itemindex < MAX_EQUIPMENT_INDEX || itemindex >= MAX_MY_INVENTORY_INDEX)
			{
				if (itemindex >= MAX_MY_INVENTORY_INDEX && itemindex < MAX_MY_SHOP_INVENTORY_INDEX)
				{
					g_pMyShopInventory->DeleteItem(itemindex - MAX_MY_INVENTORY_INDEX);
				}
			}
			else
			{
				g_pMyInventory->DeleteItem(itemindex - MAX_EQUIPMENT_INDEX);
			}
		}
	}

	if (Data->Value)
	{
		EnableUse = 0;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x28 [ReceiveDeleteInventory(%d %d)]", Data->SubCode, Data->Value);
}

void ReceiveTradeInventory(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
	int Offset = sizeof(PHEADER_DEFAULT_SUBCODE_WORD);

	if (Data->SubCode == 3)
	{
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		PlayBuffer(SOUND_MIX01);
		PlayBuffer(SOUND_BREAK01);
		g_pMixInventory->DeleteAllItems();
	}
	else if (Data->SubCode == 5)
	{
		g_pChatListBox->AddText("", GlobalText[1208], SEASON3B::TYPE_ERROR_MESSAGE); //Quá trình tái tạo đã thất bại
		PlayBuffer(SOUND_MIX01);
		PlayBuffer(SOUND_BREAK01);
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		g_pMixInventory->DeleteAllItems();
	}
	else
	{
		//for (int i = 0; i < MAX_SHOP_INVENTORY; i++)
		//{
		//	ShopInventory[i].Type = -1;
		//	ShopInventory[i].Number = 0;
		//}
	}

	for (int i = 0; i < Data->Value; i++)
	{
		LPPRECEIVE_INVENTORY Data2 = (LPPRECEIVE_INVENTORY)(ReceiveBuffer + Offset);

		if (Data->SubCode == 3)
		{
			g_pMixInventory->InsertItem(Data2->Index, Data2->Item);
		}
		else if (Data->SubCode == 5)
		{
			g_pMixInventory->InsertItem(Data2->Index, Data2->Item);
		}
		else
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP) == true)
			{
				g_pNPCShop->InsertItem(Data2->Index, Data2->Item);
			}
			else if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE))
			{
				g_pStorageInventory->InsertItem(Data2->Index, Data2->Item);
			}
		}

		Offset += sizeof(PRECEIVE_INVENTORY);
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x31 [ReceiveTradeInventory]");
}

void ReceiveChat(BYTE* ReceiveBuffer)
{
	if (SceneFlag == LOG_IN_SCENE)
	{
		g_ErrorReport.Write("Send Request Server List.\r\n");
		SendRequestServerList();
	}
	else
	{
		LPPCHATING Data = (LPPCHATING)ReceiveBuffer;

		char ID[MAX_ID_SIZE + 1];
		memcpy(ID, (char*)Data->ID, MAX_ID_SIZE);
		ID[MAX_ID_SIZE] = 0;

		char Text[MAX_CHAT_SIZE + 1];
		memset(Text, 0, MAX_CHAT_SIZE + 1);

		if (Data->ChatText[0] == '~')
		{
			for (int i = 0; i < MAX_CHAT_SIZE - 1; i++)
				Text[i] = Data->ChatText[i + 1];
			g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_PARTY_MESSAGE);
		}
		else if (Data->ChatText[0] == '$')
		{
			for (int i = 0; i < MAX_CHAT_SIZE - 1; i++)
				Text[i] = Data->ChatText[i + 1];
			g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_GENS_MESSAGE);
		}
		else if (Data->ChatText[0] == '@' && Data->ChatText[1] == '@')
		{
			for (int i = 0; i < MAX_CHAT_SIZE - 2; i++)
				Text[i] = Data->ChatText[i + 2];
			g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_UNION_MESSAGE);
		}
		else if (Data->ChatText[0] == '@')
		{
			for (int i = 0; i < MAX_CHAT_SIZE - 1; i++)
				Text[i] = Data->ChatText[i + 1];
			g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_GUILD_MESSAGE);
		}
		else if (Data->ChatText[0] == '#')
		{
			for (int i = 0; i < MAX_CHAT_SIZE - 1; i++)
				Text[i] = Data->ChatText[i + 1];

			CHARACTER* pFindGm = NULL;

			for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* c = gmCharacters->GetCharacter(i);
				OBJECT* o = &c->Object;
				if (o->Live && o->Kind == KIND_PLAYER && (g_isCharacterBuff((&c->Object), eBuff_GMEffect) || (c->CtlCode == CTLCODE_20OPERATOR) || (c->CtlCode == CTLCODE_08OPERATOR)))
				{
					if (strcmp(c->ID, ID) == NULL)
					{
						pFindGm = c; break;
					}
				}
			}
			if (pFindGm)
			{
				gmHeadChat->AssignChat(ID, Text);

				g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_GM_MESSAGE);
			}
			else
			{
				gmHeadChat->AssignChat(ID, Text, 1);
			}
		}
		else
		{
			memcpy(Text, (char*)Data->ChatText, MAX_CHAT_SIZE);
			CHARACTER* pFindGm = NULL;
			for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* c = gmCharacters->GetCharacter(i);
				OBJECT* o = &c->Object;
				if (o->Live && o->Kind == KIND_PLAYER && g_isCharacterBuff((&c->Object), eBuff_GMEffect) || (c->CtlCode == CTLCODE_20OPERATOR) || (c->CtlCode == CTLCODE_08OPERATOR))
				{
					if (strcmp(c->ID, ID) == NULL)
					{
						pFindGm = c;
						break;
					}
				}
			}
			if (pFindGm)
			{
				gmHeadChat->AssignChat(ID, Text);
				g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_GM_MESSAGE);
			}
			else
			{
				gmHeadChat->AssignChat(ID, Text);
				g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_CHAT_MESSAGE);
			}
		}
	}
}

void ReceiveChatWhisper(BYTE* ReceiveBuffer)
{
	if (g_pChatInputBox->IsBlockWhisper() == true)
	{
		return;
	}

	LPPCHATING Data = (LPPCHATING)ReceiveBuffer;

	char ID[MAX_ID_SIZE + 1];
	memset(ID, 0, MAX_ID_SIZE + 1);
	memcpy(ID, (char*)Data->ID, MAX_ID_SIZE);

	char Text[MAX_CHAT_SIZE + 1];
	memset(Text, 0, MAX_CHAT_SIZE + 1);
	memcpy(Text, (char*)Data->ChatText, MAX_CHAT_SIZE);
	Text[MAX_CHAT_SIZE] = 0;

	RegistWhisperID(10, ID);

	if (g_pOption->IsWhisperSound())
	{
		PlayBuffer(SOUND_WHISPER);
	}
	g_pChatListBox->AddText(ID, Text, SEASON3B::TYPE_WHISPER_MESSAGE);
}

void ReceiveChatWhisperResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
	{
		g_pChatListBox->AddText(ChatWhisperID, GlobalText[482], SEASON3B::TYPE_ERROR_MESSAGE, SEASON3B::TYPE_WHISPER_MESSAGE);
	}
	break;
	}
}

void ReceiveChatKey(BYTE* ReceiveBuffer)
{
	LPPCHATING_KEY Data = (LPPCHATING_KEY)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	int Index = FindCharacterIndex(Key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

	if (pCharacter)
	{
		if (Hero->GuildStatus == G_MASTER && !strcmp(pCharacter->ID, "±æµå ¸¶½ºÅÍ"))
		{
			g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCGUILDMASTER);

			GuildInputEnable = true;
			InputEnable = false;
			ClearInput(FALSE);
			InputTextMax[0] = 8;
			InputNumber = 0;

			if (Hero->GuildMarkIndex == -1)
				Hero->GuildMarkIndex = MARK_EDIT;
			return;
		}
		gmHeadChat->CreateChat(pCharacter->ID, (char*)Data->ChatText, pCharacter);
	}
}

void ReceiveNotice(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_NOTICE Data = (LPPRECEIVE_NOTICE)ReceiveBuffer;
	if (Data->Result == 0)
	{
		CreateNotice((char*)Data->Notice, 0);
	}
	else if (Data->Result == 1)
	{
		if (CHARACTER_SCENE != SceneFlag)
		{
			g_pChatListBox->AddText("", (char*)Data->Notice, SEASON3B::TYPE_SYSTEM_MESSAGE);
			EnableUse = 0;
		}
		else
		{
			CUIMng& rUIMng = CUIMng::Instance();
			rUIMng.AddServerMsg((char*)Data->Notice);
		}
	}
	else if (Data->Result == 2)
	{
		char Text[100];
		sprintf(Text, GlobalText[483], (char*)Data->Notice);
		CreateNotice(Text, 1);
		g_pGuildInfoWindow->AddGuildNotice((char*)Data->Notice);
	}
	else if (Data->Result >= 10 && Data->Result <= 15)
	{
		if (Data->Notice != NULL && Data->Notice[0] != '\0')
		{
			g_pSlideHelpMgr->AddSlide(Data->Count, Data->Delay, (char*)Data->Notice, Data->Result - 10, Data->Speed / 10.0f, Data->Color);
		}
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x0D [ReceiveNotice(%s)]", Data->Notice);
}

void ReceiveMoveCharacter(BYTE* ReceiveBuffer)
{
	LPPMOVE_CHARACTER Data = (LPPMOVE_CHARACTER)ReceiveBuffer;

	int  Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int index = FindCharacterIndex(Key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

	if (pCharacter && pCharacter->Dead == 0)
	{
		OBJECT* o = &pCharacter->Object;
		pCharacter->TargetAngle = Data->Path[0] >> 4;

		if (Key == HeroKey)
		{
			if (!pCharacter->Movement)
			{
				pCharacter->PositionX = Data->PositionX;
				pCharacter->PositionY = Data->PositionY;
			}
		}
		else
		{
			pCharacter->TargetX = Data->PositionX;
			pCharacter->TargetY = Data->PositionY;
			if (o->Type == MODEL_MONSTER01 + 52)
			{
				pCharacter->PositionX = Data->PositionX;
				pCharacter->PositionY = Data->PositionY;
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], pCharacter->PositionX * 100.f, pCharacter->PositionY * 100.f);
				SetPlayerWalk(pCharacter);
				pCharacter->JumpTime = 1;
			}
			else if (pCharacter->Appear == 0)
			{
				int iDefaultWall = TW_CHARACTER;

				if (World >= WD_65DOPPLEGANGER1 && World <= WD_68DOPPLEGANGER4 && Key != HeroKey)
				{
					iDefaultWall = TW_NOMOVE;
				}

				if (PathFinding2(pCharacter->PositionX, pCharacter->PositionY, pCharacter->TargetX, pCharacter->TargetY, &pCharacter->Path, 0.0f, iDefaultWall))
				{
					pCharacter->Movement = true;
				}
				else
				{
					pCharacter->Movement = false;
					SetPlayerStop(pCharacter);
				}
			}
		}
		g_ConsoleDebug->Write(MCD_RECEIVE, "ID : %s | sX : %d | sY : %d | tX : %d | tY : %d", pCharacter->ID, pCharacter->PositionX, pCharacter->PositionY, pCharacter->TargetX, pCharacter->TargetY);
	}
}

void ReceiveMovePosition(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MOVE_POSITION Data = (LPPRECEIVE_MOVE_POSITION)ReceiveBuffer;

	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int index = FindCharacterIndex(Key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;
		if (o->Type == MODEL_BALL)
		{
			o->Gravity = 20.f;
			o->Direction[1] = (Data->PositionX - (pCharacter->PositionX)) * 400.f;
			o->Direction[0] = -(Data->PositionY - (pCharacter->PositionY)) * 400.f;
			PlayBuffer(SOUND_MENU01);
			CreateBomb(o->Position, false);
		}

		pCharacter->PositionX = Data->PositionX;
		pCharacter->PositionY = Data->PositionY;
		pCharacter->TargetX = Data->PositionX;
		pCharacter->TargetY = Data->PositionY;
		pCharacter->JumpTime = 1;
	}
}

BOOL ReceiveTeleport(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

	LPPRECEIVE_TELEPORT_POSITION Data = (LPPRECEIVE_TELEPORT_POSITION)ReceiveBuffer;
	Hero->PositionX = Data->PositionX;
	Hero->PositionY = Data->PositionY;

	Hero->JumpTime = 0;

	OBJECT* o = &Hero->Object;
	o->Position[0] = ((float)(Hero->PositionX) + 0.5f) * TERRAIN_SCALE;
	o->Position[1] = ((float)(Hero->PositionY) + 0.5f) * TERRAIN_SCALE;

	if (gMapManager->currentMap == -1 || Hero->Helper.Type != MODEL_HELPER + 3 || Hero->SafeZone)
	{
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
	}
	else
	{
		if (gMapManager->currentMap == WD_8TARKAN || gMapManager->currentMap == WD_10HEAVEN)
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 90.f;
		else
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
	}

	if (gMapManager->currentMap >= WD_45CURSEDTEMPLE_LV1 && gMapManager->currentMap <= WD_45CURSEDTEMPLE_LV6)
	{
		if (!(Data->Map >= WD_45CURSEDTEMPLE_LV1 && Data->Map <= WD_45CURSEDTEMPLE_LV6))
		{
			g_CursedTemple->ResetCursedTemple();
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM);
		}
	}

	int iEtcPart = Hero->EtcPart;

	o->Angle[2] = ((float)(Data->Angle) - 1.f) * 45.f;
	if (Data->Flag == 0)
	{
		CreateTeleportEnd(o);
		Teleport = false;
	}
	else
	{
		ClearItems();
		ClearCharacters(HeroKey);
		RemoveAllShopTitleExceptHero();
		if (gMapManager->currentMap != Data->Map)
		{
			int OldWorld = gMapManager->currentMap;

			gMapManager->currentMap = Data->Map;
			gMapManager->LoadWorld(gMapManager->currentMap);

			if (gMapManager->currentMap == WD_34CRYWOLF_1ST)
				SendRequestCrywolfInfo();

			if ((gMapManager->InChaosCastle(OldWorld) == true && OldWorld != gMapManager->currentMap) || gMapManager->InChaosCastle() == true)
			{
				PlayBuffer(SOUND_CHAOS_ENVIR, NULL, true);

				g_pNewUISystem->Hide(SEASON3B::INTERFACE_FRIEND);

				SetCharacterClass(Hero);
				gGoboidManager->DeleteBug(&Hero->Object);
			}
			if (gMapManager->InChaosCastle() == false)
			{
				StopBuffer(SOUND_CHAOSCASTLE, true);
				StopBuffer(SOUND_CHAOS_ENVIR, true);
			}

			if (gMapManager->IsEmpireGuardian1() == false &&
				gMapManager->IsEmpireGuardian2() == false &&
				gMapManager->IsEmpireGuardian3() == false &&
				gMapManager->IsEmpireGuardian4() == false)
			{
				StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_RAIN, true);
				StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_FOG, true);
				StopBuffer(SOUND_EMPIREGUARDIAN_WEATHER_STORM, true);
				StopBuffer(SOUND_EMPIREGUARDIAN_INDOOR_SOUND, true);
			}

			matchEvent::CreateEventMatch(gMapManager->currentMap);

			if (gMapManager->currentMap == -1 || Hero->Helper.Type != MODEL_HELPER + 3 || Hero->SafeZone)
			{
				o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
			}
			else
			{
				if (gMapManager->currentMap == WD_8TARKAN || gMapManager->currentMap == WD_10HEAVEN)
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 90.f;
				else
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
			}

			if (gMapManager->currentMap >= WD_65DOPPLEGANGER1 && gMapManager->currentMap <= WD_68DOPPLEGANGER4);
			else
			{
				char Text[256];
				sprintf(Text, "%s%s", GlobalText[484], gMapManager->GetMapName(gMapManager->currentMap));

				g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
		}
		SendRequestFinishLoading();

		LoadingWorld = 30;

		MouseUpdateTime = 0;
		MouseUpdateTimeMax = 6;

		if (gMapManager->currentMap < WD_65DOPPLEGANGER1 || gMapManager->currentMap > WD_68DOPPLEGANGER4)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_DOPPELGANGER_FRAME);
		}
		if (gMapManager->currentMap < WD_69EMPIREGUARDIAN1 || WD_72EMPIREGUARDIAN4 < gMapManager->currentMap)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER);
		}

		g_pNewUISystem->HideAll();

		CreatePetDarkSpirit_Now(Hero);
		CreateEffect(BITMAP_MAGIC + 2, o->Position, o->Angle, o->Light, 0, o);

		o->Alpha = 0.f;
		EnableEvent = 0; //USE_EVENT_ELDORADO

		SelectedItem = -1;
		SelectedNpc = -1;
		SelectedCharacter = -1;
		SelectedOperate = -1;
		Attacking = -1;
		RepairEnable = 0;
	}

	Hero->Movement = false;
	SetPlayerStop(Hero);

	if (Data->Flag)
		g_pUIMapName->ShowMapName();	// rozy

	CreateMyGensInfluenceGroundEffect();

	if (gMapManager->InBattleCastle() && battleCastle::IsBattleCastleStart())
	{
		Hero->EtcPart = iEtcPart;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x1C [ReceiveTeleport(%d)]", Data->Flag);

	return (TRUE);
}

void ReceiveEquipment(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_EQUIPMENT Data = (LPPRECEIVE_EQUIPMENT)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int index = FindCharacterIndex(Key);

	if (index != MAX_CHARACTERS_CLIENT)
		ChangeCharacterExt(index, Data->Equipment, NULL, NULL);
}

void ReceiveChangePlayer(BYTE* ReceiveBuffer)
{
	LPPCHANGE_CHARACTER Data = (LPPCHANGE_CHARACTER)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int index = FindCharacterIndex(Key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;

		int Type = ConvertItemType(Data->Item);
		BYTE Level = Data->Item[1] & 0xF;
		BYTE Option = Data->Item[3] & 0x3F;
		BYTE ExtOption = Data->Item[4];

		switch (Data->iSrcIndex)
		{
		case 0:
			if (Type == 0x1FFF)
			{
				pCharacter->Weapon[0].Type = -1;
				pCharacter->Weapon[0].Option1 = 0;
			}
			else
			{
				pCharacter->Weapon[0].Type = MODEL_ITEM + Type;
				pCharacter->Weapon[0].Level = LevelConvert(Level);
				pCharacter->Weapon[0].Option1 = Option;
			}
			break;
		case 1:
			if (Type == 0x1FFF)
			{
				pCharacter->Weapon[1].Type = -1;
				pCharacter->Weapon[1].Option1 = 0;
				DeletePet(pCharacter);
			}
			else
			{
				pCharacter->Weapon[1].Type = MODEL_ITEM + Type;
				pCharacter->Weapon[1].Level = LevelConvert(Level);
				pCharacter->Weapon[1].Option1 = Option;
				CreatePetDarkSpirit_Now(pCharacter);
				g_SummonSystem.RemoveEquipEffects(pCharacter);
			}
			break;
		case 2:
			if (Type == 0x1FFF)
			{
				pCharacter->BodyPart[BODYPART_HELM].Type = MODEL_BODY_HELM + gCharacterManager.GetSkinModelIndex(pCharacter->Class);
				pCharacter->BodyPart[BODYPART_HELM].Level = 0;
				pCharacter->BodyPart[BODYPART_HELM].Option1 = 0;
				pCharacter->BodyPart[BODYPART_HELM].ExtOption = 0;
			}
			else
			{
				pCharacter->BodyPart[BODYPART_HELM].Type = MODEL_ITEM + Type;
				pCharacter->BodyPart[BODYPART_HELM].Level = LevelConvert(Level);
				pCharacter->BodyPart[BODYPART_HELM].Option1 = Option;
				pCharacter->BodyPart[BODYPART_HELM].ExtOption = ExtOption;
			}
			break;
		case 3:
			if (Type == 0x1FFF)
			{
				pCharacter->BodyPart[BODYPART_ARMOR].Type = MODEL_BODY_ARMOR + gCharacterManager.GetSkinModelIndex(pCharacter->Class);
				pCharacter->BodyPart[BODYPART_ARMOR].Level = 0;
				pCharacter->BodyPart[BODYPART_ARMOR].Option1 = 0;
				pCharacter->BodyPart[BODYPART_ARMOR].ExtOption = 0;
			}
			else
			{
				pCharacter->BodyPart[BODYPART_ARMOR].Type = MODEL_ITEM + Type;
				pCharacter->BodyPart[BODYPART_ARMOR].Level = LevelConvert(Level);
				pCharacter->BodyPart[BODYPART_ARMOR].Option1 = Option;
				pCharacter->BodyPart[BODYPART_ARMOR].ExtOption = ExtOption;
			}
			break;
		case 4:
			if (Type == 0x1FFF)
			{
				pCharacter->BodyPart[BODYPART_PANTS].Type = MODEL_BODY_PANTS + gCharacterManager.GetSkinModelIndex(pCharacter->Class);
				pCharacter->BodyPart[BODYPART_PANTS].Level = 0;
				pCharacter->BodyPart[BODYPART_PANTS].Option1 = 0;
				pCharacter->BodyPart[BODYPART_PANTS].ExtOption = 0;
			}
			else
			{
				pCharacter->BodyPart[BODYPART_PANTS].Type = MODEL_ITEM + Type;
				pCharacter->BodyPart[BODYPART_PANTS].Level = LevelConvert(Level);
				pCharacter->BodyPart[BODYPART_PANTS].Option1 = Option;
				pCharacter->BodyPart[BODYPART_PANTS].ExtOption = ExtOption;
			}
			break;
		case 5:
			if (Type == 0x1FFF)
			{
				pCharacter->BodyPart[BODYPART_GLOVES].Type = MODEL_BODY_GLOVES + gCharacterManager.GetSkinModelIndex(pCharacter->Class);
				pCharacter->BodyPart[BODYPART_GLOVES].Level = 0;
				pCharacter->BodyPart[BODYPART_GLOVES].Option1 = 0;
				pCharacter->BodyPart[BODYPART_GLOVES].ExtOption = 0;
			}
			else
			{
				pCharacter->BodyPart[BODYPART_GLOVES].Type = MODEL_ITEM + Type;
				pCharacter->BodyPart[BODYPART_GLOVES].Level = LevelConvert(Level);
				pCharacter->BodyPart[BODYPART_GLOVES].Option1 = Option;
				pCharacter->BodyPart[BODYPART_GLOVES].ExtOption = ExtOption;
			}
			break;
		case 6:
			if (Type == 0x1FFF)
			{
				pCharacter->BodyPart[BODYPART_BOOTS].Type = MODEL_BODY_BOOTS + gCharacterManager.GetSkinModelIndex(pCharacter->Class);
				pCharacter->BodyPart[BODYPART_BOOTS].Level = 0;
				pCharacter->BodyPart[BODYPART_BOOTS].Option1 = 0;
				pCharacter->BodyPart[BODYPART_BOOTS].ExtOption = 0;
			}
			else
			{
				pCharacter->BodyPart[BODYPART_BOOTS].Type = MODEL_ITEM + Type;
				pCharacter->BodyPart[BODYPART_BOOTS].Level = LevelConvert(Level);
				pCharacter->BodyPart[BODYPART_BOOTS].Option1 = Option;
				pCharacter->BodyPart[BODYPART_BOOTS].ExtOption = ExtOption;
			}
			break;
		case 7:
			if (Type == 0x1FFF)
			{
				if (pCharacter->Wing.Type == MODEL_WING + 39 ||
					pCharacter->Wing.Type == MODEL_HELPER + 30 ||
					pCharacter->Wing.Type == MODEL_WING + 130 ||
					pCharacter->Wing.Type == MODEL_WING + 49 ||
					pCharacter->Wing.Type == MODEL_WING + 50 ||
					pCharacter->Wing.Type == MODEL_WING + 135 ||
					pCharacter->Wing.Type == MODEL_WING + 40 ||
					GMPhysicsManager->physics_work_out(pCharacter->Wing.Type)
					)
				{
					DeleteCloth(pCharacter, o);
				}
				pCharacter->Wing.Type = -1;
				pCharacter->Wing.ClothType = 0;
			}
			else
			{
				pCharacter->Wing.Type = MODEL_ITEM + Type;
				pCharacter->Wing.Level = 0;
				pCharacter->Wing.ClothType = 0;

				if (pCharacter->Wing.Type == MODEL_WING + 39 ||
					pCharacter->Wing.Type == MODEL_HELPER + 30 ||
					pCharacter->Wing.Type == MODEL_WING + 130 ||
					pCharacter->Wing.Type == MODEL_WING + 49 ||
					pCharacter->Wing.Type == MODEL_WING + 50 ||
					pCharacter->Wing.Type == MODEL_WING + 135 ||
					pCharacter->Wing.Type == MODEL_WING + 40 ||
					GMPhysicsManager->physics_work_out(pCharacter->Wing.Type))
				{
					DeleteCloth(pCharacter, o);
				}
			}
			break;
		case 8:
			if (Type == 0x1FFF)
			{
				pCharacter->Helper.Type = -1;
				gGoboidManager->DeleteBug(o, 0);
				ThePetProcess().DeletePet(pCharacter, pCharacter->Helper.Type, true);
			}
			else
			{
				pCharacter->Helper.Type = MODEL_ITEM + Type;
				//c->Helper.Level = LevelConvert(Level);
				pCharacter->Helper.Level = 0;
				switch (Type)
				{
				case ITEM_HELPER:
					gGoboidManager->CreateBug(MODEL_HELPER, o->Position, o); break;
				case ITEM_HELPER + 2:
					gGoboidManager->CreateBug(MODEL_UNICON, o->Position, o); break;
				case ITEM_HELPER + 3:
					gGoboidManager->CreateBug(MODEL_PEGASUS, o->Position, o); break;
				case ITEM_HELPER + 4:
					gGoboidManager->CreateBug(MODEL_DARK_HORSE, o->Position, o); break;
				case ITEM_HELPER + 37:
					pCharacter->Helper.Option1 = Option;
					if (Option == 0x01)
					{
						gGoboidManager->CreateBug(MODEL_FENRIR_BLACK, o->Position, o);
					}
					else if (Option == 0x02)
					{
						gGoboidManager->CreateBug(MODEL_FENRIR_BLUE, o->Position, o);
					}
					else if (Option == 0x04)
					{
						gGoboidManager->CreateBug(MODEL_FENRIR_GOLD, o->Position, o);
					}
					else
					{
						gGoboidManager->CreateBug(MODEL_FENRIR_RED, o->Position, o);
					}
					break;
				case ITEM_HELPER + 64:
				case ITEM_HELPER + 65:
				{
					ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter);
				}
				break;
				case ITEM_HELPER + 67:
					ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter); break;
				case ITEM_HELPER + 80:
					ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter); break;
				case ITEM_HELPER + 106:
					ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter); break;
				case ITEM_HELPER + 123:
					ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter); break;
				default:
					if (ThePetProcess().IsPet(Type))
					{
						ThePetProcess().CreatePet(Type, pCharacter->Helper.Type, o->Position, pCharacter); break;
					}
					else
					{
						CreateGoboId(Type, o->Position, o);
					}
					break;
				}
			}
			break;
		case 237:
			if (Type == 0x1FFF)
			{
				pCharacter->MuunHelper[0].Type = -1;
				gGoboidManager->DeleteBug(o, 1);
			}
			else
			{
				pCharacter->MuunHelper[0].Type = MODEL_ITEM + Type;
				CreateGoboId(Type, o->Position, o, 0, 1);
			}
			break;
		case 238:
			if (Type == 0x1FFF)
			{
				pCharacter->MuunHelper[1].Type = -1;
				gGoboidManager->DeleteBug(o, 2);
			}
			else
			{
				pCharacter->MuunHelper[1].Type = MODEL_ITEM + Type;
				CreateGoboId(Type, o->Position, o, 0, 2);
			}
			break;
#ifdef SHUTDOWN_CORE_WING4
		case 241:
			if (Type != 0x1FFF)
			{
				pCharacter->Core.Type = Type + MODEL_ITEM;
				pCharacter->Core.Level = LevelConvert(Level);
				pCharacter->Core.Option1 = Option;
				pCharacter->Core.ExtOption = ExtOption;
			}
			else
			{
				pCharacter->Flag.Type = -1;
			}
			break;
#endif // SHUTDOWN_CORE_WING4
#ifdef SYSTEM_FLAG_NAT
		case 242:
			if (Type != 0x1FFF)
			{
				pCharacter->Flag.Type = Type + MODEL_ITEM;
				pCharacter->Flag.Level = LevelConvert(Level);
				pCharacter->Flag.Option1 = Option;
				pCharacter->Flag.ExtOption = ExtOption;
			}
			else
			{
				pCharacter->Flag.Type = -1;
			}
			break;
#endif // SYSTEM_FLAG_NAT
		}

		ChangeChaosCastleUnit(pCharacter);

		SetCharacterScale(pCharacter);
	}
}

void RegisterBuff(eBuffState buff, OBJECT* o, const int bufftime = 0);

void UnRegisterBuff(eBuffState buff, OBJECT* o);

void ReceiveCreatePlayerViewport(BYTE* ReceiveBuffer, int Size)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;

	int Offset = sizeof(PWHEADER_DEFAULT_WORD);

	for (int i = 0; i < Data->Value; i++)
	{
		LPPCREATE_CHARACTER Data2 = (LPPCREATE_CHARACTER)(ReceiveBuffer + Offset);
		WORD Key = ((WORD)(Data2->KeyH) << 8) + Data2->KeyL;
		int CreateFlag = (Key >> 15);
		Key &= 0x7FFF;

		char Temp[MAX_ID_SIZE + 1];

		memcpy(Temp, (char*)Data2->Name, MAX_ID_SIZE);

		Temp[MAX_ID_SIZE] = '\0';

		if (!FindText(Temp, "webzen", false))
		{
			int Index;

			short BackUpGuildMarkIndex = -1;
			BYTE BackUpGuildStatus = 0;
			BYTE BackUpGuildType = 0;
			BYTE BackUpGuildRelationShip = 0;
			BYTE BackUpGuildMasterKillCount = 0;
			BYTE EtcPart = 0;
			BYTE BackupCtlcode = 0;

			CHARACTER* pCharacter;

			if ((Data2->ActionPose & 0x07) == 1)
			{
				Index = FindCharacterIndex(Key);
				pCharacter = gmCharacters->GetCharacter(Index);

				if (pCharacter)
				{
					BackUpGuildMarkIndex = pCharacter->GuildMarkIndex;
					BackUpGuildStatus = pCharacter->GuildStatus;
					BackUpGuildType = pCharacter->GuildType;
					BackUpGuildRelationShip = pCharacter->GuildRelationShip;
					EtcPart = pCharacter->EtcPart;
					BackUpGuildMasterKillCount = pCharacter->GuildMasterKillCount;

					if (pCharacter == Hero)
						BackupCtlcode = pCharacter->CtlCode;
				}
			}
			else
			{
				Index = FindCharacterIndex(Key);
				pCharacter = gmCharacters->GetCharacter(Index);

				if (pCharacter)
				{
					BackUpGuildMarkIndex = pCharacter->GuildMarkIndex;
					BackUpGuildStatus = pCharacter->GuildStatus;
					BackUpGuildType = pCharacter->GuildType;
					BackUpGuildRelationShip = pCharacter->GuildRelationShip;
					BackUpGuildMasterKillCount = pCharacter->GuildMasterKillCount;
					EtcPart = pCharacter->EtcPart;

					if (pCharacter == Hero)
						BackupCtlcode = pCharacter->CtlCode;
				}
			}

			CHARACTER* c = CreateCharacter(Key, MODEL_PLAYER, Data2->PositionX, Data2->PositionY, 0);
			DeleteCloth(c, &c->Object);

			OBJECT* o = &c->Object;

			c->Class = gCharacterManager.ChangeServerClassTypeToClientClassType(Data2->ServerClass);
			c->Skin = 0;
			c->PK = Data2->dir_and_pk_level & 0xf;
			o->Kind = KIND_PLAYER;

			switch (Data2->ActionPose & 0x07)
			{
			case 1:
				CreateTeleportEnd(o);
				AddDebugText(ReceiveBuffer, Size);
				break;
			case 2:
				if (gCharacterManager.IsFemale(c->Class))
					SetAction(o, PLAYER_SIT_FEMALE1);
				else
					SetAction(o, PLAYER_SIT1);
				break;
			case 3:
				if (gCharacterManager.IsFemale(c->Class))
					SetAction(o, PLAYER_POSE_FEMALE1);
				else
					SetAction(o, PLAYER_POSE1);
				break;
			case 4:
				if (gCharacterManager.IsFemale(c->Class))
					SetAction(o, PLAYER_HEALING_FEMALE1);
				else
					SetAction(o, PLAYER_HEALING1);
				break;
			}

			c->PositionX = Data2->PositionX;
			c->PositionY = Data2->PositionY;
			c->TargetX = Data2->TX;
			c->TargetY = Data2->TY;

			c->Object.Angle[2] = ((float)(Data2->dir_and_pk_level >> 4) - 1.0) * 45.0;

			if (CreateFlag)
			{
				c->Object.Position[0] = ((c->PositionX) + 0.5f) * TERRAIN_SCALE;
				c->Object.Position[1] = ((c->PositionY) + 0.5f) * TERRAIN_SCALE;
				CreateEffect(BITMAP_MAGIC + 2, o->Position, o->Angle, o->Light, 0, o);
				c->Object.Alpha = 0.f;
			}
			else if (PathFinding2(c->PositionX, c->PositionY, c->TargetX, c->TargetY, &c->Path))
			{
				c->Movement = true;
			}

			if (gMapManager->InHellas())
			{
				CreateJoint(BITMAP_FLARE + 1, o->Position, o->Position, o->Angle, 8, o, 20.f);
			}

			ChangeCharacterExt(FindCharacterIndex(Key), Data2->Equipment, NULL, NULL);

			if ((Data2->ActionPose & 0x07) != 1 || Index == MAX_CHARACTERS_CLIENT)
			{
				c->GuildMarkIndex = BackUpGuildMarkIndex;
				c->GuildStatus = BackUpGuildStatus;
				c->GuildType = BackUpGuildType;
				c->GuildRelationShip = BackUpGuildRelationShip;
				c->GuildMasterKillCount = BackUpGuildMasterKillCount;
				c->EtcPart = EtcPart;

				pCharacter = gmCharacters->GetCharacter(Index);

				if (pCharacter && pCharacter == Hero)
					c->CtlCode = BackupCtlcode;

			}
			else
			{
				c->GuildMarkIndex = BackUpGuildMarkIndex;
				c->GuildStatus = BackUpGuildStatus;
				c->GuildType = BackUpGuildType;
				c->GuildRelationShip = BackUpGuildRelationShip;
				c->EtcPart = EtcPart;
				c->GuildMasterKillCount = BackUpGuildMasterKillCount;

				pCharacter = gmCharacters->GetCharacter(Index);

				if (pCharacter && pCharacter == Hero)
					c->CtlCode = BackupCtlcode;
			}

			memcpy(c->ID, (char*)Data2->Name, MAX_ID_SIZE);
			c->ID[MAX_ID_SIZE] = NULL;

			if ((Data2->ActionPose & 0x07) == 1 && Index != MAX_CHARACTERS_CLIENT)
			{
				c->EtcPart = EtcPart;
			}

			c->SetLife(Data2->CurHP, Data2->MaxHP);

			for (int j = 0; j < Data2->s_BuffCount; ++j)
			{
				RegisterBuff(static_cast<eBuffState>(Data2->s_BuffEffectState[j]), o);

				battleCastle::SettingBattleFormation(c, static_cast<eBuffState>(Data2->s_BuffEffectState[j]));
			}

			if (gMapManager->InBattleCastle() && battleCastle::IsBattleCastleStart())
			{
				g_pSiegeWarfare->InitSkillUI();
			}
		}

		Offset += (sizeof(PCREATE_CHARACTER) - (sizeof(BYTE) * (MAX_BUFF_SLOT_INDEX - Data2->s_BuffCount)));
	}

	gmAIController->SetActiveCharacter(Hero);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x12 [ReceiveCreatePlayerViewport(%d)]", Data->Value);
}

void ReceiveCreateTransformViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);

	for (int i = 0; i < Data->Value; i++)
	{
		LPPCREATE_TRANSFORM Data2 = (LPPCREATE_TRANSFORM)(ReceiveBuffer + Offset);
		WORD Key = ((WORD)(Data2->KeyH) << 8) + Data2->KeyL;
		int CreateFlag = (Key >> 15);
		Key &= 0x7FFF;

		char Temp[MAX_ID_SIZE + 1];
		memcpy(Temp, (char*)Data2->Name, MAX_ID_SIZE);
		Temp[MAX_ID_SIZE] = NULL;



		short sBackUpGuildMarkIndex = -1;
		BYTE byBackUpGuildStatus = 0;
		BYTE byBackUpGuildType = 0;
		BYTE byBackUpGuildRelationShip = 0;
		BYTE byBackUpGuildMasterKillCount = 0;
		BYTE byEtcPart = 0;
		BYTE byBackupCtlcode = 0;

		int iIndex = FindCharacterIndex(Key);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(iIndex);

		if (pCharacter)
		{
			sBackUpGuildMarkIndex = pCharacter->GuildMarkIndex;
			byBackUpGuildStatus = pCharacter->GuildStatus;
			byBackUpGuildType = pCharacter->GuildType;
			byBackUpGuildRelationShip = pCharacter->GuildRelationShip;
			byBackUpGuildMasterKillCount = pCharacter->GuildMasterKillCount;
			byEtcPart = pCharacter->EtcPart;
			byBackupCtlcode = pCharacter->CtlCode;
		}

		if (FindText(Temp, "webzen") == false)
		{
			int Class = gCharacterManager.ChangeServerClassTypeToClientClassType(Data2->ServerClass);

			WORD Type = ((WORD)(Data2->TypeH) << 8) + Data2->TypeL;

			CHARACTER* c = CreateMonster(Type, Data2->PositionX, Data2->PositionY, Key);

			OBJECT* o = &c->Object;

			if (c->MonsterIndex == 7)
			{
				o->Scale = 0.8f;
			}

			if (Type == 373)
			{
				DeleteCloth(c, o);
			}

			if (Type == 404 || Type == 405)
			{
				DeleteCloth(c, o);
			}

			DeleteCloth(c, o);

			c->GuildMarkIndex = sBackUpGuildMarkIndex;
			c->GuildStatus = byBackUpGuildStatus;
			c->GuildType = byBackUpGuildType;
			c->GuildRelationShip = byBackUpGuildRelationShip;
			c->GuildMasterKillCount = byBackUpGuildMasterKillCount;
			c->EtcPart = byEtcPart;
			c->CtlCode = byBackupCtlcode;
			c->Class = Class;
			c->PK = Data2->dir_and_pk_level & 0xf;
			o->Kind = KIND_PLAYER;
			c->Change = true;

			for (int j = 0; j < Data2->s_BuffCount; ++j)
			{
				RegisterBuff(static_cast<eBuffState>(Data2->s_BuffEffectState[j]), o);
				battleCastle::SettingBattleFormation(c, static_cast<eBuffState>(Data2->s_BuffEffectState[j]));

				g_ConsoleDebug->Write(MCD_RECEIVE, "ID : %s, Buff : %d", c->ID, static_cast<int>(Data2->s_BuffEffectState[j]));
			}

			c->PositionX = Data2->PositionX;
			c->PositionY = Data2->PositionY;
			c->TargetX = Data2->PositionX;
			c->TargetY = Data2->PositionY;
			c->Object.Angle[2] = ((float)(Data2->dir_and_pk_level >> 4) - 1.f) * 45.f;

			if (CreateFlag)
			{
				c->Object.Position[0] = ((c->PositionX) + 0.5f) * TERRAIN_SCALE;
				c->Object.Position[1] = ((c->PositionY) + 0.5f) * TERRAIN_SCALE;
				c->Object.Alpha = 0.f;
				CreateEffect(MODEL_MAGIC_CIRCLE1, o->Position, o->Angle, o->Light, 0, o);
				CreateParticle(BITMAP_LIGHTNING + 1, o->Position, o->Angle, o->Light, 2, 1.f, o);
			}
			else if (PathFinding2((c->PositionX), (c->PositionY), Data2->TX, Data2->TY, &c->Path))
			{
				c->Movement = true;
			}

			memcpy(c->ID, (char*)Data2->Name, MAX_ID_SIZE);
			c->ID[MAX_ID_SIZE] = NULL;

			c->SetLife(Data2->CurHP, Data2->MaxHP);

			ChangeCharacterExt(FindCharacterIndex(Key), Data2->Equipment);

			//c->AddUIInfo();
		}

		Offset += (sizeof(PCREATE_TRANSFORM) - (sizeof(BYTE) * (MAX_BUFF_SLOT_INDEX - Data2->s_BuffCount)));
	}

	gmAIController->SetActiveCharacter(Hero);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x45 [ReceiveCreateTransformViewport(%d)]", Data->Value);
}

void AppearMonster(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	switch (c->MonsterIndex)
	{
	case 44:
		SetAction(o, MONSTER01_STOP2);
		o->PriorAction = MONSTER01_STOP2;
		c->Object.Alpha = 1.f;
		PlayBuffer(SOUND_MONSTER + 124);
		break;
	case 21:
		SetAction(o, MONSTER01_STOP2);
		o->PriorAction = MONSTER01_STOP2;
		c->Object.Alpha = 1.f;
		PlayBuffer(SOUND_ASSASSIN);
		break;
	case 53:
	case 54:
		c->Appear = 60;
		SetAction(o, MONSTER01_STOP2);
		o->PriorAction = MONSTER01_STOP2;
		c->Object.Alpha = 1.f;
		//PlayBuffer(SOUND_ASSASSIN);
		break;
	case 85:
	case 91:
	case 97:
	case 114:
	case 120:
	case 126:
		PlayBuffer(SOUND_MONSTER + 161);
		break;
	case 440:
	case 340:
	case 341:
	case 344:
	case 345:
	{
		if (g_Direction.m_CMVP.m_iCryWolfState == CRYWOLF_STATE_READY)
			c->Object.Alpha = 1.0f;
		else
		{
			c->Object.Alpha = 0.f;

			vec3_t vPos;
			Vector(c->Object.Position[0] + 20.0f, c->Object.Position[1] + 20.0f, c->Object.Position[2], vPos);

			CreateJoint(BITMAP_JOINT_THUNDER + 1, vPos, vPos, c->Object.Angle, 7, NULL, 60.f + rand() % 10);
			CreateJoint(BITMAP_JOINT_THUNDER + 1, vPos, vPos, c->Object.Angle, 7, NULL, 50.f + rand() % 10);
			CreateJoint(BITMAP_JOINT_THUNDER + 1, vPos, vPos, c->Object.Angle, 7, NULL, 50.f + rand() % 10);
			CreateJoint(BITMAP_JOINT_THUNDER + 1, vPos, vPos, c->Object.Angle, 7, NULL, 60.f + rand() % 10);

			CreateParticle(BITMAP_SMOKE + 4, c->Object.Position, c->Object.Angle, c->Object.Light, 1, 5.0f);
			CreateParticle(BITMAP_SMOKE + 4, c->Object.Position, c->Object.Angle, c->Object.Light, 1, 5.0f);
			CreateParticle(BITMAP_SMOKE + 4, c->Object.Position, c->Object.Angle, c->Object.Light, 1, 5.0f);

			Vector(c->Object.Position[0], c->Object.Position[1], c->Object.Position[2] + 120.0f, vPos);
			CreateJoint(BITMAP_JOINT_THUNDER, c->Object.Position, vPos, c->Object.Angle, 17);
			CreateJoint(BITMAP_JOINT_THUNDER, c->Object.Position, vPos, c->Object.Angle, 17);
			CreateJoint(BITMAP_JOINT_THUNDER, c->Object.Position, vPos, c->Object.Angle, 17);
			CreateJoint(BITMAP_JOINT_THUNDER, c->Object.Position, vPos, c->Object.Angle, 17);
		}
	}
	break;
	case 362:
	case 363:
	{
		SetAction(o, MONSTER01_APEAR);
		c->Object.Alpha = 0.f;
	}
	break;
	default:
		if (o->Type == MODEL_PLAYER)
		{
			SetAction(o, PLAYER_COME_UP);
			o->PriorAction = PLAYER_COME_UP;
			c->Object.Alpha = 1.f;
		}
		else
		{
			c->Object.Alpha = 0.f;
		}
		break;
	}
}

void ReceiveCreateMonsterViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);
	for (int i = 0; i < Data->Value; i++)
	{
		LPPCREATE_MONSTER Data2 = (LPPCREATE_MONSTER)(ReceiveBuffer + Offset);
		WORD Key = ((WORD)(Data2->KeyH) << 8) + Data2->KeyL;

		BYTE bMyMob = (Data2->TypeH) & 0x80;
		BYTE byBuildTime = (Data2->TypeH & 0x70) >> 4;
		WORD Type = ((WORD)(Data2->TypeH & 0x03) << 8) + Data2->TypeL;

		if (Type != 106)
			int a = 0;

		int CreateFlag = (Key >> 15);
		int TeleportFlag = (Data2->KeyH & 0x40) >> 6;

		Key &= 0x7FFF;
		CHARACTER* pCharacter = CreateMonster(Type, Data2->PositionX, Data2->PositionY, Key);

		if (pCharacter != NULL)
		{
			//pCharacter->AddUIInfo();


			OBJECT* o = &pCharacter->Object;

			for (int j = 0; j < Data2->s_BuffCount; ++j)
			{
				RegisterBuff(static_cast<eBuffState>(Data2->s_BuffEffectState[j]), o);

				g_ConsoleDebug->Write(MCD_RECEIVE, "ID : %s, Buff : %d", pCharacter->ID, static_cast<int>(Data2->s_BuffEffectState[j]));
			}

			float fAngle = 45.0f;
			if (o->Type == MODEL_MONSTER01 + 99)
			{
				if (pCharacter->PositionY == 90)
					fAngle = 0.0f;
				if (pCharacter->PositionX == 62)
					fAngle = -135.0f;
				if (pCharacter->PositionX == 183)
					fAngle = 90.0f;
			}
			else if (o->Type == MODEL_KANTURU2ND_ENTER_NPC)
			{
				o->m_fEdgeScale = 1.0f;
				fAngle = 90.f;
			}

			pCharacter->Object.Angle[2] = ((float)(Data2->Path >> 4) - 1.f) * fAngle;

			if (o->Type >= MODEL_LITTLESANTA + 1 && o->Type <= MODEL_LITTLESANTA + 4)
			{
				pCharacter->Object.Angle[2] = 90.f;
			}
			else if (o->Type >= MODEL_LITTLESANTA + 6 && o->Type <= MODEL_LITTLESANTA + 7)
			{
				pCharacter->Object.Angle[2] = 90.f;
			}
			else if (o->Type == MODEL_XMAS2008_SANTA_NPC)
			{
				pCharacter->Object.Angle[2] = 20.f;
			}
			else if (o->Type == MODEL_UNITEDMARKETPLACE_JULIA)
			{
				pCharacter->Object.Angle[2] = 49.0f;
			}

			pCharacter->PositionX = Data2->PositionX;
			pCharacter->PositionY = Data2->PositionY;
			pCharacter->TargetX = Data2->PositionX;
			pCharacter->TargetY = Data2->PositionY;

			pCharacter->SetLife(Data2->CurHP, Data2->MaxHP);


			pCharacter->m_byFriend = bMyMob;
			o->m_byBuildTime = byBuildTime;

			if (gMapManager->InBattleCastle() && pCharacter->MonsterIndex == 277)
			{
				if (g_isCharacterBuff((&pCharacter->Object), eBuff_CastleGateIsOpen))
				{
					SetAction(&pCharacter->Object, 1);
					battleCastle::SetCastleGate_Attribute(Data2->PositionX, Data2->PositionY, 0);
				}
				else
				{
					SetAction(&pCharacter->Object, 0);
					battleCastle::SetCastleGate_Attribute(Data2->PositionX, Data2->PositionY, 1);
				}
			}
			if (CreateFlag)
			{
				AppearMonster(pCharacter);
			}
			else if (gMapManager->currentMap == WD_39KANTURU_3RD && o->Type == MODEL_MONSTER01 + 121 && TeleportFlag)
			{
				vec3_t Light;
				Vector(1.0f, 1.0f, 1.0f, Light);
				o->AlphaTarget = 1.f;
				CreateEffect(BITMAP_SPARK + 1, o->Position, o->Angle, Light);
				CreateEffect(BITMAP_SPARK + 1, o->Position, o->Angle, Light);
				PlayBuffer(SOUND_KANTURU_3RD_NIGHTMARE_TELE);
			}

			int iDefaultWall = TW_CHARACTER;

			if (gMapManager->currentMap >= WD_65DOPPLEGANGER1 && gMapManager->currentMap <= WD_68DOPPLEGANGER4 && Key != HeroKey)
			{
				iDefaultWall = TW_NOMOVE;
			}

			else if (PathFinding2(pCharacter->PositionX, pCharacter->PositionY, Data2->TargetX, Data2->TargetY, &pCharacter->Path, 0.0f, iDefaultWall))
			{
				pCharacter->Movement = true;
			}
			else
			{
				pCharacter->Movement = false;
			}
		}

		g_ConsoleDebug->Write(MCD_RECEIVE, "0x13 [ReceiveCreateMonsterViewport(Type : %d | Key : %d)]", Type, Key);

		Offset += (sizeof(PCREATE_MONSTER) - (sizeof(BYTE) * (MAX_BUFF_SLOT_INDEX - Data2->s_BuffCount)));
	}
}

void ReceiveCreateSummonViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);

	for (int i = 0; i < Data->Value; i++)
	{
		LPPCREATE_SUMMON Data2 = (LPPCREATE_SUMMON)(ReceiveBuffer + Offset);
		WORD Key = ((WORD)(Data2->KeyH) << 8) + Data2->KeyL;
		WORD Type = ((WORD)(Data2->TypeH) << 8) + Data2->TypeL;
		int CreateFlag = (Key >> 15);
		Key &= 0x7FFF;

		CHARACTER* c;

		if (Type >= 152 && Type <= 158)
		{
			c = CreateHellGate(&Data2->ID[0], Key, Type, Data2->PositionX, Data2->PositionY, CreateFlag);
		}
		else
		{
			c = CreateMonster(Type, Data2->PositionX, Data2->PositionY, Key);
		}

		if (c == NULL)
			break;

		OBJECT* o = &c->Object;

		for (int j = 0; j < Data2->s_BuffCount; ++j)
		{
			RegisterBuff(static_cast<eBuffState>(Data2->s_BuffEffectState[j]), o);

			g_ConsoleDebug->Write(MCD_RECEIVE, "ID : %s, Buff : %d", c->ID, static_cast<int>(Data2->s_BuffEffectState[j]));
		}

		c->Object.Angle[2] = ((float)(Data2->Path >> 4) - 1.f) * 45.f;
		c->PositionX = Data2->PositionX;
		c->PositionY = Data2->PositionY;
		c->TargetX = Data2->PositionX;
		c->TargetY = Data2->PositionY;
		o->Kind = KIND_PLAYER;
		c->PK = Data2->Path & 0xf;

		if (c->PK >= PVP_MURDERER2)
			c->Level = 1;

		if (Type >= 152 && Type <= 158)
		{
			char Temp[100];
			strcat(c->ID, GlobalText[485]);
			memcpy(Temp, Data2->ID, MAX_ID_SIZE);
			Temp[MAX_ID_SIZE] = NULL;
			strcat(c->ID, Temp);
			memcpy(c->OwnerID, Data2->ID, MAX_ID_SIZE);
			c->OwnerID[MAX_ID_SIZE] = NULL;

		}

		if (CreateFlag)
		{
			AppearMonster(c);
			CreateEffect(MODEL_MAGIC_CIRCLE1, o->Position, o->Angle, o->Light, 0, o);
			CreateParticle(BITMAP_LIGHTNING + 1, o->Position, o->Angle, o->Light, 2, 1.f, o);

			if (Type >= 152 && Type <= 158)
			{
				o->PriorAnimationFrame = 0.f;
				o->AnimationFrame = 0.f;
				PlayBuffer(SOUND_CHAOS_FALLING_STONE);
			}
		}
		else if (PathFinding2(c->PositionX, c->PositionY, Data2->TargetX, Data2->TargetY, &c->Path))
		{
			c->Movement = true;
		}
		//c->AddUIInfo();

		Offset += (sizeof(PCREATE_SUMMON) - (sizeof(BYTE) * (MAX_BUFF_SLOT_INDEX - Data2->s_BuffCount)));
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x1F [ReceiveCreateSummonViewport(%d)]", Data->Value);
}

void ReceiveDeleteCharacterViewport(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	int Offset = sizeof(PHEADER_DEFAULT);

	for (int i = 0; i < Data->Value; i++)
	{
		LPPDELETE_CHARACTER Data2 = (LPPDELETE_CHARACTER)(ReceiveBuffer + Offset);

		if (Switch_Info != NULL)
		{
			WORD Key = ((WORD)(Data2->KeyH) << 8) + Data2->KeyL;
			if (Key == FIRST_CROWN_SWITCH_NUMBER)
				Switch_Info[0].Reset();
			else
				Switch_Info[1].Reset();
		}

		int Key = ((int)(Data2->KeyH) << 8) + Data2->KeyL;
		int DeleteFlag = (Key >> 15);

		Key &= 0x7FFF;

		int iIndex = g_pPurchaseShopInventory->GetShopCharacterIndex();

		CHARACTER* pCharacter = gmCharacters->GetCharacter(iIndex);

		if (pCharacter && pCharacter->Key == Key)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);
		}

		iIndex = FindCharacterIndex(Key);
		pCharacter = gmCharacters->GetCharacter(iIndex);

		if (pCharacter)
		{
			int buffSize = g_CharacterBuffSize((&pCharacter->Object));

			for (int k = 0; k < buffSize; k++)
			{
				UnRegisterBuff(g_CharacterBuff((&pCharacter->Object), k), &pCharacter->Object);
			}
		}
		DeleteCharacter(Key);

		CHARACTER* pPlayer = FindCharacterTagShopTitle(Key);

		if (pPlayer)
		{
			RemoveShopTitle(pPlayer);
		}
		Offset += sizeof(PDELETE_CHARACTER);
	}
}
int AttackPlayer = 0;

void ReceiveDamage(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_DAMAGE Data = (LPPRECEIVE_DAMAGE)ReceiveBuffer;

	/*DWORD Damage = ((DWORD)(Data->DamageH) << 8) + Data->DamageL;

	if (CharacterAttribute->Life >= Damage)
		CharacterAttribute->Life -= Damage;
	else
		CharacterAttribute->Life = 0;

	DWORD ShieldDamage = ((DWORD)(Data->ShieldDamageH) << 8) + Data->ShieldDamageL;

	if (CharacterAttribute->Shield >= ShieldDamage)
		CharacterAttribute->Shield = ShieldDamage;
	else
		CharacterAttribute->Shield = 0;*/

	CharacterAttribute->Life = Data->btLife;
	CharacterAttribute->Shield = Data->btShield;
}

void ProcessDamageCastle(LPPRECEIVE_ATTACK Data)
{
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	int Success = (Key >> 15);
	Key &= 0x7FFF;

	int Index = FindCharacterIndex(Key);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

	if (pCharacter)
	{
		vec3_t Light;

		OBJECT* o = &pCharacter->Object;

		WORD Damage = (((WORD)(Data->DamageH) << 8) + Data->DamageL);
		int	 DamageType = (Data->DamageType) & 0x3f;
		bool bDoubleEnable = (Data->DamageType >> 6) & 0x01;
		bool bComboEnable = (Data->DamageType >> 7) & 0x01;
		WORD ShieldDamage = (((WORD)(Data->ShieldDamageH) << 8) + Data->ShieldDamageL);

		int accumDamage = ShieldDamage + Damage;
		int	rstDamage = -1;
		float rstScale = 0.8f;
		Vector(0.5f, 0.5f, 0.5f, Light);
		if (accumDamage > 0)
		{
			rstDamage = -2;
			if (accumDamage > 0 && accumDamage < 1000)
			{
				Vector(1.f, 0.f, 0.f, Light);
			}
			else if (accumDamage >= 1000 && accumDamage < 3000)
			{
				Vector(0.95f, 0.7f, 0.15f, Light);
			}
			else if (accumDamage >= 3000)
			{
				Vector(0.95f, 0.7f, 0.15f, Light);
				rstScale = 1.5f;
			}
		}
		else
		{
			if (Key == HeroKey)
			{
				Vector(1.f, 1.f, 1.f, Light);
			}
			else
			{
				Vector(0.5f, 0.5f, 0.5f, Light);
			}
		}

		if (Success)
		{
			SetPlayerShock(pCharacter, Damage);

			Vector(1.f, 0.f, 0.f, Light);
			if (Key == HeroKey)
			{
				if (Damage >= CharacterAttribute->Life)
					CharacterAttribute->Life = 0;
				else
					CharacterAttribute->Life -= Damage;

				if (ShieldDamage >= CharacterAttribute->Shield)
					CharacterAttribute->Shield = 0;
				else
					CharacterAttribute->Shield -= ShieldDamage;

			}

			CreatePoint(o->Position, rstDamage, Light, rstScale);
		}
		else
		{
			if (Key == HeroKey)
			{
				if (Damage >= CharacterAttribute->Life)
					CharacterAttribute->Life = 0;
				else
					CharacterAttribute->Life -= Damage;

				if (ShieldDamage >= CharacterAttribute->Shield)
					CharacterAttribute->Shield = 0;
				else
					CharacterAttribute->Shield -= ShieldDamage;


				if (g_isCharacterBuff(o, eBuff_PhysDefense) && o->Type == MODEL_PLAYER)
				{
					CHARACTER* cm = gmCharacters->GetCharacter(AttackPlayer);
					if (cm)
					{
						OBJECT* om = &cm->Object;
						float fAngle = CreateAngle(om->Position[0], om->Position[1], o->Position[0], o->Position[1]);
						if (fabs((double)fAngle - om->Angle[2]) < 10.f)
						{
							vec3_t Angle = { 0.0f, 0.0f, fAngle + 180.f };
							CreateEffect(MODEL_MAGIC_CAPSULE2, o->Position, Angle, o->Light, 0, o);
						}
					}
				}
#ifdef _PVP_ADD_MOVE_SCROLL
				if (Damage > 0)
					g_MurdererMove.CancelMove();
#endif	// _PVP_ADD_MOVE_SCROLL
			}
			else
			{
				if (pCharacter->MonsterIndex != 275 && rand() % 2 == 0)
					SetPlayerShock(pCharacter, Damage);
			}

			if (DamageType == 4)
			{
				Vector(1.f, 0.f, 1.f, Light);
			}

			CreatePoint(o->Position, rstDamage, Light, rstScale);
		}
		pCharacter->Hit = Damage;
	}
}

void ReceiveAttackDamage(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_ATTACK Data = (LPPRECEIVE_ATTACK)ReceiveBuffer;

	if (gMapManager->InChaosCastle())
	{
		ProcessDamageCastle(Data);
		return;
	}

	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	int Success = (Key >> 15);
	Key &= 0x7FFF;

	int Index = FindCharacterIndex(Key);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

	if (pCharacter)
	{
		vec3_t Light;
		OBJECT* o = &pCharacter->Object;
		DWORD Damage = Data->Damage;
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
		// DamageType
		int	 DamageType = (Data->DamageType) & 0x0f;
		bool bRepeatedly = (Data->DamageType >> 4) & 0x01;
		bool bEndRepeatedly = (Data->DamageType >> 5) & 0x01;
#else //PBG_ADD_NEWCHAR_MONK_SKILL
		int	 DamageType = (Data->DamageType) & 0x3f;
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
		bool bDoubleEnable = (Data->DamageType >> 6) & 0x01;
		bool bComboEnable = (Data->DamageType >> 7) & 0x01;
		DWORD ShieldDamage = Data->ShieldDamage;

		if (Success)
		{
			SetPlayerShock(pCharacter, Damage);

			Vector(1.f, 0.f, 0.f, Light);

			CreatePoint(o->Position, Damage, Light);

			if (ShieldDamage > 0)
			{
				vec3_t nPosShieldDamage;
				Vector(0.8f, 1.f, 0.f, Light);
				nPosShieldDamage[0] = o->Position[0]; nPosShieldDamage[1] = o->Position[1];
				nPosShieldDamage[2] = o->Position[2] + 25.f;
				CreatePoint(nPosShieldDamage, ShieldDamage, Light);
			}

			if (Key == HeroKey)
			{
				if (Damage >= CharacterAttribute->Life)
					CharacterAttribute->Life = 0;
				else
					CharacterAttribute->Life -= Damage;

				if (ShieldDamage >= CharacterAttribute->Shield)
					CharacterAttribute->Shield = 0;
				else
					CharacterAttribute->Shield -= ShieldDamage;

				CharacterAttribute->Life = Data->btLife;

				CharacterAttribute->Shield = Data->btShield;
			}
		}
		else
		{
			if (Key == HeroKey)
			{
				if (Damage >= CharacterAttribute->Life)
					CharacterAttribute->Life = 0;
				else
					CharacterAttribute->Life -= Damage;

				if (ShieldDamage >= CharacterAttribute->Shield)
					CharacterAttribute->Shield = 0;
				else
					CharacterAttribute->Shield -= ShieldDamage;

				CharacterAttribute->Life = Data->btLife;

				CharacterAttribute->Shield = Data->btShield;

				if (g_isCharacterBuff(o, eBuff_PhysDefense) && o->Type == MODEL_PLAYER)
				{
					CHARACTER* cm = gmCharacters->GetCharacter(AttackPlayer);
					if (cm)
					{
						OBJECT* om = &cm->Object;
						float fAngle = CreateAngle(om->Position[0], om->Position[1], o->Position[0], o->Position[1]);
						if (fabs((double)fAngle - om->Angle[2]) < 10.f)
						{
							vec3_t Angle = { 0.0f, 0.0f, fAngle + 180.f };
							CreateEffect(MODEL_MAGIC_CAPSULE2, o->Position, Angle, o->Light, 0, o);
						}
					}
				}
			}
			else
			{
				if (pCharacter->MonsterIndex != 275 && rand() % 2 == 0)
				{
					SetPlayerShock(pCharacter, Damage);
				}
			}
			float scale = 15.f;
			if (Damage == 0)
			{
				if (Key == HeroKey)
				{
					Vector(1.f, 1.f, 1.f, Light);
				}
				else
				{
					Vector(0.5f, 0.5f, 0.5f, Light);
				}
			}
			else
			{
				switch (DamageType)
				{
				case 0:	//	DT_NONE
					if (Key == HeroKey)
					{
						Vector(1.f, 0.f, 0.f, Light);
					}
					else
					{
						Vector(1.f, 0.6f, 0.f, Light);
					}
					break;
				case 1:	//	DT_PERFECT
					scale = 50.f;
					Vector(0.0f, 1.f, 1.f, Light);
					break;
				case 2:	//	DT_EXCELLENT
					scale = 50.f;
					Vector(0.f, 1.f, 0.6f, Light);
					break;
				case 3:	//	DT_CRITICAL
					scale = 50.f;
					Vector(0.f, 0.6f, 1.f, Light);
					break;
				case 4:	//	DT_MIRROR
					Vector(1.f, 0.f, 1.f, Light);
					break;
				case 5: //	DT_POSION
					Vector(0.f, 1.f, 0.f, Light);
					break;
				case 6:	//	DT_DOT
					Vector(0.7f, 0.4f, 1.0f, Light);
					break;
				default:
					Vector(1.f, 1.f, 1.f, Light);
					break;
				}

				if (bComboEnable)
				{
					scale = 50.f;
				}
			}

			if (bRepeatedly || bEndRepeatedly)
			{
				g_CMonkSystem.SetRepeatedly(Damage, DamageType, bDoubleEnable, bEndRepeatedly);
				if (bEndRepeatedly)
				{
					g_CMonkSystem.RenderRepeatedly(Key, o);
				}
			}
			else if (Damage == 0)
			{
				CreatePoint(o->Position, -1, Light);
			}
			else
			{
				if (bComboEnable)
				{
					vec3_t Position, Light2;
					VectorCopy(o->Position, Position);
					Vector(Light[0] - 0.4f, Light[1] - 0.4f, Light[2] - 0.4f, Light2);
					CreatePoint(Position, Damage, Light2, scale);
					Position[2] += 10.f;
					Vector(Light[0] - 0.2f, Light[1] - 0.2f, Light[2] - 0.2f, Light2);
					CreatePoint(Position, Damage, Light2, scale + 5.f);
					Position[2] += 10.f;
					CreatePoint(Position, Damage, Light, scale + 10.f);
				}
				else if (bDoubleEnable)    //  Double Damage
				{
					vec3_t Position, Light2;
					VectorCopy(o->Position, Position);
					Vector(Light[0] - 0.4f, Light[1] - 0.4f, Light[2] - 0.4f, Light2);
					CreatePoint(Position, Damage, Light2, scale);
					Position[2] += 10.f;
					Vector(Light[0] - 0.2f, Light[1] - 0.2f, Light[2] - 0.2f, Light2);
					CreatePoint(Position, Damage, Light2, scale + 5.f);
				}

				CreatePoint(o->Position, Damage, Light, scale);
			}

			if (ShieldDamage > 0)
			{
				vec3_t nPosShieldDamage;
				Vector(0.8f, 1.f, 0.f, Light);
				nPosShieldDamage[0] = o->Position[0]; nPosShieldDamage[1] = o->Position[1];
				nPosShieldDamage[2] = o->Position[2] + 25.f;
				CreatePoint(nPosShieldDamage, ShieldDamage, Light);
			}
		}
		pCharacter->Hit = Damage;
		g_ConsoleDebug->Write(MCD_RECEIVE, "0x15 [ReceiveAttackDamage(%d %d)]", AttackPlayer, Damage);
	}
}

void ReceiveAction(BYTE* ReceiveBuffer, int Size)
{
	LPPRECEIVE_ACTION Data = (LPPRECEIVE_ACTION)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int Index = FindCharacterIndex(Key);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;

		int iTargetKey, iTargetIndex;

		iTargetKey = ((int)(Data->TargetKeyH) << 8) + Data->TargetKeyL;
		iTargetIndex = FindCharacterIndex(iTargetKey);

		if (pCharacter->Helper.Type != MODEL_HELPER + 4)
		{
			pCharacter->Object.Angle[2] = ((float)(Data->Angle) - 1.f) * 45.f;
			pCharacter->Movement = false;

			pCharacter->Object.Position[0] = pCharacter->TargetX * TERRAIN_SCALE + TERRAIN_SCALE * 0.5f;
			pCharacter->Object.Position[1] = pCharacter->TargetY * TERRAIN_SCALE + TERRAIN_SCALE * 0.5f;

			switch (Data->Action)
			{
			case AT_STAND1:
			case AT_STAND2:
				SetPlayerStop(pCharacter);
				break;
			case AT_ATTACK1:
			case AT_ATTACK2:
				SetPlayerAttack(pCharacter);
				pCharacter->AttackTime = 1;
				pCharacter->Object.AnimationFrame = 0;
				pCharacter->TargetCharacter = HeroIndex;
				AttackPlayer = Index;
				break;
			case AT_SKILL_BLOCKING:
				SetAction(o, PLAYER_DEFENSE1);
				PlayBuffer(SOUND_SKILL_DEFENSE);
				break;
			case AT_SIT1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(&pCharacter->Object, PLAYER_SIT1);
				else
					SetAction(&pCharacter->Object, PLAYER_SIT_FEMALE1);
				break;
			case AT_POSE1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(&pCharacter->Object, PLAYER_POSE1);
				else
					SetAction(&pCharacter->Object, PLAYER_POSE_FEMALE1);
				break;
			case AT_HEALING1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(&pCharacter->Object, PLAYER_HEALING1);
				else
					SetAction(&pCharacter->Object, PLAYER_HEALING_FEMALE1);
				break;
			case AT_GREETING1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_GREETING1);
				else
					SetAction(o, PLAYER_GREETING_FEMALE1);
				break;
			case AT_GOODBYE1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_GOODBYE1);
				else
					SetAction(o, PLAYER_GOODBYE_FEMALE1);
				break;
			case AT_CLAP1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_CLAP1);
				else
					SetAction(o, PLAYER_CLAP_FEMALE1);
				break;
			case AT_GESTURE1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_GESTURE1);
				else
					SetAction(o, PLAYER_GESTURE_FEMALE1);
				break;
			case AT_DIRECTION1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_DIRECTION1);
				else
					SetAction(o, PLAYER_DIRECTION_FEMALE1);
				break;
			case AT_UNKNOWN1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_UNKNOWN1);
				else
					SetAction(o, PLAYER_UNKNOWN_FEMALE1);
				break;
			case AT_CRY1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_CRY1);
				else
					SetAction(o, PLAYER_CRY_FEMALE1);
				break;
			case AT_AWKWARD1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_AWKWARD1);
				else
					SetAction(o, PLAYER_AWKWARD_FEMALE1);
				break;
			case AT_SEE1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_SEE1);
				else
					SetAction(o, PLAYER_SEE_FEMALE1);
				break;
			case AT_CHEER1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_CHEER1);
				else
					SetAction(o, PLAYER_CHEER_FEMALE1);
				break;
			case AT_WIN1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_WIN1);
				else
					SetAction(o, PLAYER_WIN_FEMALE1);
				break;
			case AT_SMILE1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_SMILE1);
				else
					SetAction(o, PLAYER_SMILE_FEMALE1);
				break;
			case AT_SLEEP1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_SLEEP1);
				else
					SetAction(o, PLAYER_SLEEP_FEMALE1);
				break;
			case AT_COLD1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_COLD1);
				else
					SetAction(o, PLAYER_COLD_FEMALE1);
				break;
			case AT_AGAIN1:
				if (!gCharacterManager.IsFemale(pCharacter->Class))
					SetAction(o, PLAYER_AGAIN1);
				else
					SetAction(o, PLAYER_AGAIN_FEMALE1);
				break;
			case AT_RESPECT1:
				SetAction(o, PLAYER_RESPECT1);
				break;
			case AT_SALUTE1:
				SetAction(o, PLAYER_SALUTE1);
				break;
			case AT_RUSH1:
				SetAction(o, PLAYER_RUSH1);
				break;
			case AT_SCISSORS:
				SetAction(o, PLAYER_SCISSORS);
				break;
			case AT_ROCK:
				SetAction(o, PLAYER_ROCK);
				break;
			case AT_PAPER:
				SetAction(o, PLAYER_PAPER);
				break;
			case AT_HUSTLE:
				SetAction(o, PLAYER_HUSTLE);
				break;
			case AT_DAMAGE1:
				SetAction(&pCharacter->Object, MONSTER01_SHOCK);
				break;
			case AT_PROVOCATION:
				SetAction(o, PLAYER_PROVOCATION);
				break;
			case AT_LOOK_AROUND:
				SetAction(o, PLAYER_LOOK_AROUND);
				break;
			case AT_CHEERS:
				SetAction(o, PLAYER_CHEERS);
				break;
			case AT_JACK1:
				SetAction(o, PLAYER_JACK_1);
				o->m_iAnimation = 0;
				break;
			case AT_JACK2:
				SetAction(o, PLAYER_JACK_2);
				o->m_iAnimation = 0;
				break;
			case AT_SANTA1_1:
			case AT_SANTA1_2:
			case AT_SANTA1_3:
			{
				SetAction(o, PLAYER_SANTA_1);
				int i = Data->Action % AT_SANTA1_1;
				g_XmasEvent->CreateXmasEventEffect(pCharacter, o, i);
				o->m_iAnimation = 0;
				PlayBuffer(SOUND_XMAS_JUMP_SANTA + i);
			}
			break;
			case AT_SANTA2_1:
			case AT_SANTA2_2:
			case AT_SANTA2_3:
			{
				SetAction(o, PLAYER_SANTA_2);
				int i = Data->Action % AT_SANTA2_1;
				g_XmasEvent->CreateXmasEventEffect(pCharacter, o, i);
				o->m_iAnimation = 0;
				PlayBuffer(SOUND_XMAS_TURN);
			}
			break;
			case AT_RAGEBUFF_1:
			{
				SetAction(o, PLAYER_SKILL_ATT_UP_OURFORCES);
				PlayBuffer(SOUND_RAGESKILL_BUFF_1);
			}
			break;
			case AT_RAGEBUFF_2:
			{
				SetAction(o, PLAYER_SKILL_HP_UP_OURFORCES);
				PlayBuffer(SOUND_RAGESKILL_BUFF_2);
			}
			break;
			default:
				SetAction(&pCharacter->Object, Data->Action);
				break;
			}
		}
	}
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x18 [ReceiveAction(%d)]", Data->Angle);
}

void ReceiveSkillStatus(BYTE* ReceiveBuffer)
{
	LPPMSG_VIEWSKILLSTATE Data = (LPPMSG_VIEWSKILLSTATE)ReceiveBuffer;

	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(Key));

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;

		if (Data->State == 1) // add
		{
			eBuffState bufftype = static_cast<eBuffState>(Data->BuffIndex);

			if (bufftype == eBuffNone || bufftype >= eBuff_Count) return;

			if (g_isCharacterBuff(o, bufftype))
			{
				if ((o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
					|| o->Type == MODEL_SMITH || o->Type == MODEL_NPC_SERBIS || o->Type == MODEL_MERCHANT_MAN
					|| o->Type == MODEL_STORAGE || o->Type == MODEL_NPC_BREEDER)
				{
					if (g_isCharacterBuff(o, eBuff_CrywolfHeroContracted))
					{
						g_TokenCharacterBuff(o, eBuff_CrywolfHeroContracted);
					}
					else
					{
						g_TokenCharacterBuff(o, eBuff_CrywolfHeroContracted);
						g_CharacterUnRegisterBuff(o, eBuff_CrywolfHeroContracted);
					}
				}
			}
			else
			{

				RegisterBuff(bufftype, o);
				if (bufftype == eBuff_CastleRegimentDefense || bufftype == eBuff_CastleRegimentAttack1
					|| bufftype == eBuff_CastleRegimentAttack2 || bufftype == eBuff_CastleRegimentAttack3)
				{
					battleCastle::SettingBattleFormation(pCharacter, bufftype);
				}
				else if (bufftype == eBuff_GMEffect)
				{
					if (pCharacter->m_pParts != NULL)
					{
						DeleteParts(pCharacter);
					}
					pCharacter->EtcPart = PARTS_WEBZEN;
				}

				g_ConsoleDebug->Write(MCD_RECEIVE, "RegisterBuff ID : %s, Buff : %d", pCharacter->ID, static_cast<int>(bufftype));
			}
		}
		else // clear
		{
			eBuffState bufftype = static_cast<eBuffState>(Data->BuffIndex);

			if (bufftype == eBuffNone || bufftype >= eBuff_Count) return;

			UnRegisterBuff(bufftype, o);
			if (bufftype == eBuff_CastleRegimentDefense || bufftype == eBuff_CastleRegimentAttack1
				|| bufftype == eBuff_CastleRegimentAttack2 || bufftype == eBuff_CastleRegimentAttack3)
			{
				battleCastle::DeleteBattleFormation(pCharacter, bufftype);

				g_ConsoleDebug->Write(MCD_RECEIVE, "UnRegisterBuff ID : %s, Buff : %d", pCharacter->ID, static_cast<int>(bufftype));
			}
			else if (bufftype == eBuff_GMEffect)
			{
				DeleteParts(pCharacter);
			}
		}
	}
}

void ReceiveMagicFinish(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_VALUE_KEY Data = (LPPHEADER_DEFAULT_VALUE_KEY)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(Key));

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;

		switch (Data->Value)
		{
		case AT_SKILL_POISON:
		case Skill_Poison_Strengthener:
			UnRegisterBuff(eDeBuff_Poison, o);
			break;
		case AT_SKILL_SLOW:
		case Skill_Ice_Strengthener:
			UnRegisterBuff(eDeBuff_Freeze, o);
			break;
		case AT_SKILL_BLOW_OF_DESTRUCTION:
		case Skill_Strike_of_Destruction_Strengthe:
		case Skill_Strike_of_Destruction_Proficien:
		case Skill_Strike_of_Destruction_Mastery:
			UnRegisterBuff(eDeBuff_BlowOfDestruction, o);
			break;
		case AT_SKILL_ATTACK:
		case Skill_Attack_Increase_Strengthener:
		case Skill_Attack_Increase_Mastery:
			UnRegisterBuff(eBuff_Attack, o);
			break;
		case AT_SKILL_DEFENSE:
		case Skill_Defense_Increase_Strengthener:
		case Skill_Defense_Increase_Mastery:
			UnRegisterBuff(eBuff_Defense, o);
			break;
		case AT_SKILL_STUN:
			UnRegisterBuff(eDeBuff_Stun, o);
			break;
		case AT_SKILL_INVISIBLE:
			UnRegisterBuff(eBuff_Cloaking, o);
			break;
		case AT_SKILL_MANA:
			UnRegisterBuff(eBuff_AddMana, o);
			break;
		case AT_SKILL_BRAND_OF_SKILL:
			UnRegisterBuff(eBuff_AddSkill, o);
			break;
		case AT_SKILL_IMPROVE_AG:
			UnRegisterBuff(eBuff_AddAG, o);
			break;
		case AT_SKILL_ADD_CRITICAL:
		case Skill_Critical_DMG_Increase_PowUp1:
		case Skill_Critical_DMG_Increase_PowUp2:
			UnRegisterBuff(eBuff_AddCriticalDamage, o);
			break;
		case Skill_Critical_DMG_Increase_PowUp3:
			UnRegisterBuff(EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED, o);
			break;
		case Skill_Critical_DMG_Increase_Mastery:
			UnRegisterBuff(EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED, o);
			break;
		case AT_SKILL_VITALITY:
		case Skill_Swell_Life_Strengthener:
			UnRegisterBuff(eBuff_HpRecovery, o); //eBuff_Life
			break;
		case Skill_Swell_Life_Proficiency:
			UnRegisterBuff(EFFECT_GREATER_LIFE_ENHANCED, o); //eBuff_Life
			break;
		case Skill_Swell_Life_Mastery:
			UnRegisterBuff(EFFECT_GREATER_LIFE_MASTERED, o); //eBuff_Life
			break;
		case AT_SKILL_PARALYZE:
			UnRegisterBuff(eDeBuff_Harden, o);
			break;
		case Skill_Fire_Slash:
			UnRegisterBuff(eDeBuff_Defense, o);
			break;
		case AT_SKILL_WIZARDDEFENSE:
			UnRegisterBuff(eBuff_PhysDefense, o);
			break;
		case AT_SKILL_BLAST_POISON:
			UnRegisterBuff(eDeBuff_Poison, o);
			break;

		case AT_SKILL_BLAST_FREEZE:
			UnRegisterBuff(eDeBuff_Freeze, o);
			break;
		case AT_SKILL_MONSTER_MAGIC_DEF:
			SetActionDestroy_Def(o);
			UnRegisterBuff(eBuff_Defense, o);
			break;
		case AT_SKILL_MONSTER_PHY_DEF:
			SetActionDestroy_Def(o);
			UnRegisterBuff(eBuff_PhysDefense, o);
			break;
		case AT_SKILL_ATT_UP_OURFORCES:
			UnRegisterBuff(eBuff_Att_up_Ourforces, o);
			break;
		case AT_SKILL_HP_UP_OURFORCES:
			UnRegisterBuff(eBuff_Hp_up_Ourforces, o);
			break;
		case AT_SKILL_DEF_UP_OURFORCES:
			UnRegisterBuff(eBuff_Def_up_Ourforces, o);
			break;
		}
	}
}

void SetPlayerBow(CHARACTER* c)
{
	OBJECT* so = &c->Object;
	int sc_type_helper = c->Helper.Type;

	if (so->Type == MODEL_PLAYER)
	{
		if (gCharacterManager.GetBaseClass(c->Class) == CLASS_ELF && !c->SafeZone)
		{
			SetAttackSpeed();
			int EquipedBowType = gCharacterManager.GetEquipedBowType(c);

			if (EquipedBowType == BOWTYPE_BOW)
			{
				if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !c->SafeZone)
				{
					SetAction(so, PLAYER_FENRIR_ATTACK_BOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_BOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_BOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_BOW, true);
				}
				else
				{
					if (c->Wing.Type != -1)
						SetAction(so, PLAYER_HIT_FLY_BOW);
					else
						SetAction(so, PLAYER_HIT_BOW);
				}
			}
			else if (EquipedBowType == BOWTYPE_CROSSBOW)
			{
				if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !c->SafeZone)
				{
					SetAction(so, PLAYER_FENRIR_ATTACK_CROSSBOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_CROSSBOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_CROSSBOW, true);
				}
				else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !c->SafeZone)
				{
					SetAction(so, PLAYER_HIT_RIDE_CROSSBOW, true);
				}
				else
				{
					if (c->Wing.Type != -1)
						SetAction(so, PLAYER_HIT_FLY_CROSSBOW);
					else
						SetAction(so, PLAYER_HIT_CROSSBOW);
				}
			}
		}
	}
}

void SetPlayerHighBow(CHARACTER* sc)
{
	int EquipedBowType = gCharacterManager.GetEquipedBowType(sc);

	OBJECT* so = &sc->Object;
	int sc_type_helper = sc->Helper.Type;

	if (EquipedBowType == BOWTYPE_BOW)
	{
		if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
		}
		else
		{
			if (sc->Wing.Type != -1)
				SetAction(so, PLAYER_HIT_FLY_BOW_UP);
			else
				SetAction(so, PLAYER_HIT_BOW_UP);
		}
	}
	else if (EquipedBowType == BOWTYPE_CROSSBOW)
	{
		if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
		}
		else
		{
			if (sc->Wing.Type != -1)
				SetAction(so, PLAYER_HIT_FLY_CROSSBOW_UP);
			else
				SetAction(so, PLAYER_HIT_CROSSBOW_UP);
		}
	}
}

BOOL ReceiveMonsterSkill(BYTE* ReceiveBuffer, int Size, BOOL bEncrypted)
{
	LPPRECEIVE_MONSTERSKILL Data = (LPPRECEIVE_MONSTERSKILL)ReceiveBuffer;

	int SourceKey = Data->SourceKey;
	int TargetKey = Data->TargetKey;
	int Success = (TargetKey >> 15);

	WORD SkillNumber = ((WORD)(Data->MagicH) << 8) + Data->MagicL;

	int Index = FindCharacterIndex(SourceKey);
	int TargetIndex = FindCharacterIndex(TargetKey);


	CHARACTER* sc = gmCharacters->GetCharacter(Index);
	CHARACTER* tc = gmCharacters->GetCharacter(TargetIndex);

	if (sc && tc)
	{
		AttackPlayer = Index;

		OBJECT* so = &sc->Object;
		OBJECT* to = &tc->Object;

		sc->MonsterSkill = SkillNumber;

		sc->SkillSuccess = (Success != 0);
		sc->TargetCharacter = TargetIndex;

		if (SkillNumber > ATMON_SKILL_BIGIN && SkillNumber < ATMON_SKILL_END)
		{
			sc->Object.Angle[2] = CreateAngle(so->Position[0], so->Position[1], to->Position[0], to->Position[1]);
			SetPlayerAttack(sc);
			so->AnimationFrame = 0;
			sc->AttackTime = 1;
		}

		g_ConsoleDebug->Write(MCD_RECEIVE, "0x69 [ReceiveMonsterSkill(Skill : %d | SKey : %d |TKey : %d)]", SkillNumber, SourceKey, TargetKey);
	}
	return (TRUE);
}

BOOL ReceiveMagic(BYTE* ReceiveBuffer, int Size, BOOL bEncrypted)
{
	if (bEncrypted)
	{
		LPPRECEIVE_MAGIC Data = (LPPRECEIVE_MAGIC)ReceiveBuffer;
		int SourceKey = Data->SourceKeyL + (Data->SourceKeyH << 8);
		int TargetKey = Data->TargetKeyL + (Data->TargetKeyH << 8);
		int Success = TargetKey >> 15;
		WORD MagicNumber = Data->MagicL + (Data->MagicH << 8);

		if (MagicNumber != 28 && MagicNumber != 27 && MagicNumber != 417 && MagicNumber != 420 && MagicNumber != 422 && MagicNumber != 423 || Success)
		{
			TargetKey &= 0x7FFFu;
			int srcIndex = FindCharacterIndex(SourceKey);
			int trgIndex = FindCharacterIndex(TargetKey);

			CHARACTER* sc = gmCharacters->GetCharacter(srcIndex);
			CHARACTER* tc = gmCharacters->GetCharacter(trgIndex);

			if (sc && tc)
			{
				AttackPlayer = srcIndex;
				OBJECT* so = &sc->Object;
				OBJECT* to = &tc->Object;

				if (MagicNumber != Skill_Combo)
				{
					if (sc != Hero && MagicNumber != Skill_Teleport && MagicNumber != Skill_Teleport_Ally && to->Visible)
						so->Angle[2] = CreateAngle(so->Position[0], so->Position[1], to->Position[0], to->Position[1]);

					sc->TargetCharacter = trgIndex;

					if (MagicNumber == Skill_Plasma_Storm)
						sc->m_iFenrirSkillTarget = trgIndex;

					sc->SkillSuccess = Success != 0;
					sc->Skill = MagicNumber;
				}

				vec3_t vLight;
				so->start_skill_hide(MagicNumber);

				int sc_type_helper = sc->Helper.Type;

				if ((signed int)MagicNumber > Skill_Killing_Blow)
				{
					if ((signed int)MagicNumber > Skill_Fire_Burst_Strengthener)
					{
						switch (MagicNumber)
						{
						case Skill_Force_Wave_Strengthener:
						LABEL_71:
							if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else
							{
								SetAction(so, PLAYER_HIT_STRIKE, true);
							}
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD1, 0, 0);
							break;
						case Skill_Fire_Burst_Mastery:
							goto LABEL_80;
						case Skill_Critical_DMG_Increase_PowUp1:
						case Skill_Critical_DMG_Increase_PowUp2:
						LABEL_247:
							SetAttackSpeed();
							g_CharacterRegisterBuff(to, eBuff_AddCriticalDamage);

							if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_HAND1, true);
							}

							sc->AttackTime = 1;
							break;
						case Skill_Critical_DMG_Increase_PowUp3:
							SetAttackSpeed();
							g_CharacterRegisterBuff(to, EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED);

							if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
							{
								SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_HAND1, true);
							}
							sc->AttackTime = 1;
							break;
						case Skill_Fire_Scream_Strengthener:
						LABEL_292:
							if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else
							{
								SetAction(so, PLAYER_HIT_STRIKE, true);
							}

							PlayBuffer(SOUND_FIRE_SCREAM, 0, 0);
							break;
						case Skill_Chaotic_Diseier_Strengthener:
						LABEL_321:
							if (so->Type == MODEL_PLAYER)
							{
								if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
								{
									SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
								}
								else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
								{
									SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
								}
								else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
								{
									SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
								}
								else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
								{
									SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
								}
								else
								{
									SetAction(so, PLAYER_HIT_STRIKE, true);
								}
							}
							else
							{
								SetPlayerAttack(sc);
							}
							{
								OBJECT* o = so;
								float Matrix[3][4];
								vec3_t Light, Position, vPos, dp;

								Vector(0.f, 0.f, 0.f, vPos);
								AngleMatrix(o->Angle, Matrix);
								VectorRotate(vPos, Matrix, dp);
								VectorAdd(dp, o->Position, Position);

								Vector(0.5f, 0.5f, 0.5f, Light);
								for (int i = 0; i < 3; ++i)
								{
									CreateEffect(BITMAP_SHINY + 6, Position, o->Angle, Light, 3, o, -1, 0, 0, 0, 0.5f);
								}

								VectorCopy(o->Position, Position);

								for (int i = 0; i < 4; i++)
								{
									Position[0] = (o->Position[0] - 119.f) + (float)(rand() % 240);
									Position[2] = (o->Position[2] + 49.f) + (float)(rand() % 60);
									CreateJoint(BITMAP_2LINE_GHOST, Position, o->Position, o->Angle, 0, o, 20.f, o->PKKey, 0, o->m_bySkillSerialNum);
								}

								CHARACTER* st = gmCharacters->GetCharacter(SelectedCharacter);

								if (sc == Hero && st)
								{
									vec3_t Pos;
									VectorCopy(st->Object.Position, Pos);
									CreateBomb(Pos, true);
								}
								PlayBuffer(SOUND_SKILL_CAOTIC);
							}
							break;
						case Skill_Killing_Blow_Strengthener:
						case Skill_Killing_Blow_Mastery:
							goto LABEL_340;
						case Skill_Defense_Success_Rate_Increase_P:
						case Skill_DefSuccessRate_Increase_Mastery:
						case Skill_Stamina_Increase_Strengthener:
						case 0x23Fu:
						LABEL_343:
							if (sc == Hero)
							{
								if (so->CurrentAction == PLAYER_SKILL_ATT_UP_OURFORCES)
								{
									SendRequestAction(AT_RAGEBUFF_1, ((BYTE)((so->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
								}
								else
								{
									SendRequestAction(AT_RAGEBUFF_2, ((BYTE)((so->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
								}
							}
							{
								OBJECT* _pObj = to;
								if (sc != Hero)
								{
									_pObj = so;
									g_CMonkSystem.RageCreateEffect(_pObj, MagicNumber);
								}
								if (MagicNumber != Skill_Increase_Health && MagicNumber != Skill_Stamina_Increase_Strengthener)
								{
									if (MagicNumber != Skill_Increase_Block && MagicNumber != Skill_Defense_Success_Rate_Increase_P && MagicNumber != Skill_DefSuccessRate_Increase_Mastery)
									{
										if (MagicNumber == Skill_Ignore_Defense || MagicNumber == 575)
										{
											DeleteEffect(BITMAP_LIGHT_RED, _pObj, 1);
											CreateEffect(BITMAP_LIGHT_RED, _pObj->Position, _pObj->Angle, _pObj->Light, 1, _pObj, -1, 0, 0, 0, 1.5f);
										}
									}
									else
									{
										DeleteEffect(BITMAP_LIGHT_RED, _pObj, 2);
										CreateEffect(BITMAP_LIGHT_RED, _pObj->Position, _pObj->Angle, _pObj->Light, 2, _pObj, -1, 0, 0, 0, 1.5f);

										Vector(0.7f, 0.7f, 1.0f, vLight);
										DeleteEffect(MODEL_WINDFOCE, _pObj, 1);
										CreateEffect(MODEL_WINDFOCE, _pObj->Position, _pObj->Angle, vLight, 1, _pObj, -1, 0, 0, 0, 1.0f);
									}
								}
								else
								{
									DeleteEffect(BITMAP_LIGHT_RED, _pObj, 0);
									CreateEffect(BITMAP_LIGHT_RED, _pObj->Position, _pObj->Angle, _pObj->Light, 0, _pObj, -1, 0, 0, 0, 1.5f);
								}
								if ((signed int)MagicNumber > Skill_Defense_Success_Rate_Increase_P)
								{
									switch (MagicNumber)
									{
									case Skill_DefSuccessRate_Increase_Mastery:
										g_CharacterRegisterBuff(_pObj, EFFECT_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED);
										break;
									case Skill_Stamina_Increase_Strengthener:
										g_CharacterRegisterBuff(_pObj, EFFECT_FITNESS_IMPROVED);
										break;
									case 0x23Fu:
										g_CharacterRegisterBuff(_pObj, EFFECT_IGNORE_DEFENSE_RATE_IMPROVED);
										break;
									}
								}
								else
								{
									switch (MagicNumber)
									{
									case Skill_Defense_Success_Rate_Increase_P:
										g_CharacterRegisterBuff(_pObj, EFFECT_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED);
										break;
									case Skill_Ignore_Defense:
										g_CharacterRegisterBuff(_pObj, eBuff_Att_up_Ourforces);
										break;
									case Skill_Increase_Health:
										g_CharacterRegisterBuff(_pObj, eBuff_Hp_up_Ourforces);
										break;
									case Skill_Increase_Block:
										g_CharacterRegisterBuff(_pObj, eBuff_Def_up_Ourforces);
										break;
									}
								}
								sc->AttackTime = 1;
							}
							break;
						case Skill_Beast_Uppercut_Strengthener:
						case Skill_Beast_Uppercut_Mastery:
						LABEL_377:
							sc->AttackTime = 1;
							break;
						case Skill_Chain_Drive_Strengthener:
						LABEL_378:
							sc->AttackTime = 1;
							if (sc != Hero)
								g_CMonkSystem.RageCreateEffect(so, MagicNumber);
							break;
						case Skill_Dragon_Roar_Strengthener:
						LABEL_382:

							Vector(1.0f, 1.0f, 1.0f, vLight);
							if (g_CMonkSystem.SetLowerEffEct())
							{
								CreateEffect(MODEL_VOLCANO_OF_MONK, to->Position, to->Angle, vLight, g_CMonkSystem.GetLowerEffCnt(), to, -1, 0, 0, 0, 0.8f);
							}

							CreateEffect(MODEL_TARGETMON_EFFECT, to->Position, to->Angle, vLight, 0, to, -1, 0, 0, 0, 1.0f);

							StopBuffer(SOUND_RAGESKILL_DRAGONLOWER_ATTACK, true);
							PlayBuffer(SOUND_RAGESKILL_DRAGONLOWER_ATTACK);
							break;
						case Skill_Dark_Side_Strengthener:
						LABEL_385:
							if (sc != Hero)
								g_CMonkSystem.SetDarksideCnt();
							break;
						default:
							break;
						}
					}
					else if (MagicNumber == Skill_Fire_Burst_Strengthener)
					{
					LABEL_80:
						if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
						}
						else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_STRIKE, true);
						}
						sc->AttackTime = 1;
						PlayBuffer(SOUND_ATTACK_FIRE_BUST, 0, 0);
					}
					else
					{
						switch (MagicNumber)
						{
						case Skill_Heal_Strengthener:
						case Skill_Defense_Increase_Strengthener:
						case Skill_Attack_Increase_Strengthener:
						case Skill_Attack_Increase_Mastery:
						case Skill_Defense_Increase_Mastery:
						LABEL_46:
							if (sc->MonsterIndex != 77)
								PlayBuffer(111, 0, 0);
							goto LABEL_48;
						case Skill_Soul_Barrier_Strengthener:
						case Skill_Soul_Barrier_Proficiency:
						LABEL_48:
							if (sc != Hero)
							{
								SetPlayerMagic(sc);
								so->AnimationFrame = 0.0;
							}
							sc->AttackTime = 1;
							break;
						case Skill_Lightning_Strengthener:
						case Skill_Lightning_Strengthener2:
						LABEL_51:
							if (so->Type != MODEL_MONSTER01 + 87)
								PlayBuffer(SOUND_THUNDER01, 0, 0);
							if (to->CurrentAction == PLAYER_POSE1
								|| to->CurrentAction == PLAYER_POSE_FEMALE1
								|| to->CurrentAction == PLAYER_SIT1
								|| to->CurrentAction == PLAYER_SIT_FEMALE1)
							{
								SetPlayerStop(tc);
							}
							goto LABEL_58;
						case Skill_Flame_Strengthener:
						case Skill_Poison_Strengthener:
						case Skill_Ice_Strengthener:
						case Skill_Flame_Strengthener2:
						case Skill_Ice_Strengthener2:
						LABEL_58:
							if (sc != Hero)
							{
								if (so->Type == MODEL_PLAYER)
								{
									SetPlayerMagic(sc);
									so->AnimationFrame = 0.0;
								}
								else
								{
									SetPlayerAttack(sc);
									so->AnimationFrame = 0.0;
								}
							}
							sc->AttackTime = 1;
							break;
						case Skill_Falling_Slash_Strengthener:
						LABEL_89:
							SetAction(so, PLAYER_HIT_SKILL_SWORD1, 1);
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD1, 0, 0);
							break;
						case Skill_Lunge_Strengthener:
						LABEL_90:
							SetAction(so, PLAYER_HIT_SKILL_SWORD2, 1);
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD2, 0, 0);
							break;
						case Skill_Cyclone_Strengthener:
						case Skill_Cyclone_Strengthener2:
						LABEL_92:
							SetAction(so, PLAYER_HIT_SKILL_SWORD4, 1);
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD4, 0, 0);
							break;
						case Skill_Slash_Strengthener:
						LABEL_93:
							if (sc->SwordCount % 2)
								SetAction(so, PLAYER_HIT_TWO_HAND_SWORD3, 1);
							else
								SetAction(so, PLAYER_HIT_SKILL_SWORD5, 1);
							++sc->SwordCount;
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD4, 0, 0);
							break;
						case Skill_Power_Slash_Strengthener:
						LABEL_97:
							SetAction(so, PLAYER_HIT_TWO_HAND_SWORD_TWO, 1);
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD4, 0, 0);
							break;
						case Skill_Death_Stab_Strengthener:
						LABEL_102:
							SetAction(so, PLAYER_HIT_ONETOONE, true);
							if (sc != Hero && so->Type == MODEL_PLAYER)
								so->AnimationFrame = 0.0;
							sc->AttackTime = 1;
							break;
						case Skill_Triple_Shot_Strengthener:
						case Skill_Triple_Shot_Mastery:
						LABEL_106:
							SetPlayerBow(sc);
							sc->AttackTime = 1;
							break;
						case Skill_Fire_Slash_Strengthener:
						LABEL_112:
							if (sc->SkillSuccess)
							{
								DeleteEffect(BITMAP_SKULL, to, 0);
								Vector(1.0, 1.0, 1.0, vLight);
								CreateEffect(BITMAP_SKULL, to->Position, to->Angle, vLight, 0, to);
								PlayBuffer(SOUND_BLOODATTACK, to);

								g_CharacterRegisterBuff(to, eDeBuff_Defense);
							}
							if (so->Type != MODEL_PLAYER)
							{
								SetPlayerAttack(sc);
								so->AnimationFrame = 0.0;
								sc->AttackTime = 1;
							}
							break;
						case Skill_Penetration_Strengthener:
						LABEL_117:
							SetPlayerBow(sc);
							sc->AttackTime = 1;
							break;
						case Skill_Ice_Arrow_Strengthener:
						LABEL_118:
							SetPlayerBow(sc);
							if (sc->SkillSuccess)
							{
								vec3_t Angle;
								DeleteEffect(MODEL_ICE, to, 1);
								VectorCopy(to->Angle, Angle);
								CreateEffect(MODEL_ICE, to->Position, Angle, to->Light, 1, to);
								Angle[2] += 180.f;

								CreateEffect(MODEL_ICE, to->Position, Angle, to->Light, 2, to);
								tc->Movement = false;
								SetPlayerStop(tc);
								g_CharacterRegisterBuff(to, eDeBuff_Harden);
							}
							sc->AttackTime = 1;
							break;
						case Skill_Swell_Life_Strengthener:
						LABEL_274:
							if (!g_isCharacterBuff(to, eBuff_HpRecovery))
							{
								DeleteEffect(BITMAP_LIGHT, to, 1);
								CreateEffect(BITMAP_LIGHT, to->Position, to->Angle, to->Light, 1, to);
							}
							g_CharacterRegisterBuff(to, eBuff_HpRecovery);
							SetAction(so, PLAYER_SKILL_VITALITY, 1);
							sc->AttackTime = 1;
							break;
						case Skill_Swell_Life_Proficiency:
							if (!g_isCharacterBuff(to, EFFECT_GREATER_LIFE_ENHANCED))
							{
								DeleteEffect(BITMAP_LIGHT, to, 1);
								CreateEffect(BITMAP_LIGHT, to->Position, to->Angle, to->Light, 1, to);
							}
							g_CharacterRegisterBuff(to, EFFECT_GREATER_LIFE_ENHANCED);
							SetAction(so, PLAYER_SKILL_VITALITY, 1);
							sc->AttackTime = 1;
							break;
						case Skill_Swell_Life_Mastery:
							if (!g_isCharacterBuff(to, EFFECT_GREATER_LIFE_MASTERED))
							{
								DeleteEffect(BITMAP_LIGHT, to, 1);
								CreateEffect(BITMAP_LIGHT, to->Position, to->Angle, to->Light, 1, to);
							}
							g_CharacterRegisterBuff(to, EFFECT_GREATER_LIFE_MASTERED);
							SetAction(so, PLAYER_SKILL_VITALITY, 1);
							sc->AttackTime = 1;
							break;
						case Skill_Infinity_Arrow_Strengthener:
						LABEL_303:
							CharacterMachine->InfinityArrowAdditionalMana = 10;
							CreateEffect(MODEL_INFINITY_ARROW, so->Position, so->Angle, so->Light, 0, so);
							PlayBuffer(SOUND_INFINITYARROW);
							break;
						case Skill_Chain_Lightning_Strengthener:
						LABEL_304:
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_CHAIN_LIGHTNING, 0, 0);
							break;
						case Skill_Sleep_Strengthener:
						case Skill_Berserker_Strengthener:
						case Skill_Berserker_Proficiency:
						case Skill_Berserker_Mastery:
						LABEL_305:
							if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_FENRIR, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_UNI, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_DINO, true);
							}
							else if (sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_DINO, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_SLEEP, true);
							}
							sc->AttackTime = 1;

							if (MagicNumber != Skill_Sleep && MagicNumber != Skill_Sleep_Strengthener)
							{
								if (MagicNumber == 220)
									PlayBuffer(SOUND_SUMMON_SKILL_BLIND, 0, 0);
								else
									PlayBuffer(SOUND_SUMMON_SKILL_THORNS, 0, 0);
							}
							else
							{
								PlayBuffer(SOUND_SUMMON_SKILL_SLEEP, 0, 0);
							}
							break;
						case Skill_Expansion_of_Wizardry_Power_Up:
						case Skill_Expansion_of_Wizardry_Mastery:
						LABEL_319:
							SetAttackSpeed();
							SetAction(so, PLAYER_SKILL_SWELL_OF_MP, 1);
							Vector(0.3f, 0.2f, 0.9f, vLight);
							CreateEffect(MODEL_SWELL_OF_MAGICPOWER, so->Position, so->Angle, vLight, 0, so);
							PlayBuffer(SOUND_SKILL_SWELL_OF_MAGICPOWER);
							break;
						case Skill_Ignore_Defense:
						case Skill_Increase_Health:
						case Skill_Increase_Block:
							goto LABEL_343;
						case Skill_Beast_Uppercut:
							goto LABEL_377;
						case Skill_Chain_Drive:
						case Skill_Dragon_Slasher:
							goto LABEL_378;
						case Skill_Charge:
							SetAction(so, PLAYER_HIT_RUSH, 1);
							sc->AttackTime = 1;
							PlayBuffer(SOUND_SKILL_SWORD2, 0, 0);
							break;
						case Skill_Dragon_Roar:
							goto LABEL_382;
						case Skill_Dark_Side:
							goto LABEL_385;
						case Skill_Phoenix_Shot:
							to->m_sTargetIndex = trgIndex;
							if (sc != Hero)
							{
								so->m_sTargetIndex = trgIndex;
								g_CMonkSystem.RageCreateEffect(so, MagicNumber);
								/*sub_511360((int)&unk_EBCD98, a1, (int)a2, (int)a3, so, MagicNumber);*/
							}
							Vector(1.0, 1.0, 1.0, vLight);
							CreateEffect(BITMAP_SUPER0001, to->Position, to->Angle, vLight, 0, to, -1, 0, 0, 0, 0.5, -1);
							break;
						default:
							break;
						}
					}
				}
				else if (MagicNumber == Skill_Killing_Blow)
				{
				LABEL_340:
					g_CMonkSystem.SetRageSkillAni(MagicNumber, so);
					so->m_sTargetIndex = trgIndex;
					sc->AttackTime = 1;
					if (sc != Hero)
						g_CMonkSystem.RageCreateEffect(so, MagicNumber);
				}
				else
				{
					switch (MagicNumber)
					{
					case Skill_Summon_Monster:
						SetPlayerAttack(sc);
						break;
					case Skill_Recovery:
					{
						OBJECT* o = so;
						float Matrix[3][4];
						vec3_t vFirePosition;
						vec3_t Light, Position, vPos, Direction;

						if (sc != Hero)
						{
							Vector(0.f, -220.f, 130.f, vPos);
							AngleMatrix(o->Angle, Matrix);
							VectorRotate(vPos, Matrix, Direction);
							VectorAdd(Direction, o->Position, Position);
							Vector(0.7f, 0.6f, 0.f, Light);
							CreateEffect(BITMAP_IMPACT, Position, o->Angle, Light, 0, o);
							SetAction(o, PLAYER_RECOVER_SKILL);
							sc->AttackTime = 1;
						}
						o = to;
						Vector(0.4f, 0.6f, 1.f, Light);
						Vector(0.f, 0.f, 0.f, vPos);
						AngleMatrix(o->Angle, Matrix);
						VectorRotate(vPos, Matrix, Direction);
						VectorAdd(Direction, o->Position, Position);
						Position[2] += 130;
						VectorCopy(o->Position, Position);
						for (int i = 0; i < 19; ++i)
						{
							CreateJoint(BITMAP_FLARE, Position, Position, o->Angle, 47, o, 40, 2);
						}
						Vector(0.3f, 0.2f, 0.1f, Light);


						CreateEffect(MODEL_SUMMON, Position, o->Angle, Light, 0);
						CreateEffect(BITMAP_TWLIGHT, Position, o->Angle, Light, 0);
						CreateEffect(BITMAP_TWLIGHT, Position, o->Angle, Light, 1);
						CreateEffect(BITMAP_TWLIGHT, Position, o->Angle, Light, 2);

						BMD* pModels = gmClientModels->GetModel(so->Type);

						if (pModels)
						{
							int iNumBones = pModels->NumBones;
							for (int i = 0; i < 2; ++i)
							{
								pModels->TransformByObjectBone(vFirePosition, so, rand() % iNumBones);
								CreateEffect(BITMAP_FLARE, vFirePosition, so->Angle, Light, 3);
							}
							PlayBuffer(SOUND_SKILL_RECOVER);
						}
					}
					break;
					case Skill_Magic_Attack_Immunity:
						sc->AttackTime = 1;
						g_CharacterRegisterBuff(so, eBuff_PhysDefense);
						SetPlayerAttack(sc);
						break;
					case Skill_Physical_Attack_Immunity:
						sc->AttackTime = 1;
						g_CharacterRegisterBuff(so, eBuff_Defense);
						SetPlayerAttack(sc);
						break;
					case Skill_Heal:
					case Skill_Greater_Defense:
					case Skill_Greater_Damage:
					case Skill_Summon_Goblin:
					case Skill_Summon_Stone_Golem:
					case Skill_Summon_Assassin:
					case Skill_Summon_Elite_Yeti:
					case Skill_Summon_Dark_Knight:
					case Skill_Summon_Bali:
					case Skill_Summon_Soldier:
						goto LABEL_46;
					case Skill_Soul_Barrier:
						goto LABEL_48;
					case Skill_Lightning:
						goto LABEL_51;
					case Skill_Poison:
					case Skill_Meteorite:
					case Skill_Fire_Ball:
					case Skill_Flame:
					case Skill_Ice:
					case Skill_Power_Wave:
					case Skill_Energy_Ball:
						goto LABEL_58;
					case Skill_Teleport_Ally:
						CreateTeleportBegin(to);

						CreateTeleportEnd(so);
						if (sc == Hero)
							Teleport = false;

						PlayBuffer(SOUND_TELEKINESIS, so);
						break;
					case Skill_Teleport:
						if (gMapManager->currentMap != WD_39KANTURU_3RD || so->Type != MODEL_MONSTER01 + 121)
						{
							CreateTeleportBegin(so);
						}
						else
						{
							so->AlphaTarget = 0.f;
							Vector(1.0f, 1.0f, 1.0f, vLight);
							CreateEffect(BITMAP_SPARK + 1, so->Position, so->Angle, vLight);
							CreateEffect(BITMAP_SPARK + 1, so->Position, so->Angle, vLight);
							PlayBuffer(SOUND_KANTURU_3RD_NIGHTMARE_TELE);
						}
						break;
					case Skill_Force:
						goto LABEL_71;
					case Skill_Fire_Burst:
						goto LABEL_80;
					case Skill_Falling_Slash:
						goto LABEL_89;
					case Skill_Lunge:
						goto LABEL_90;
					case Skill_Uppercut:
						SetAction(so, PLAYER_HIT_SKILL_SWORD3, 1);
						sc->AttackTime = 1;
						PlayBuffer(SOUND_SKILL_SWORD3, 0, 0);
						break;
					case Skill_Cyclone:
						goto LABEL_92;
					case Skill_Slash:
						goto LABEL_93;
					case Skill_Power_Slash:
						goto LABEL_97;
					case Skill_Impale:
						if (sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir)
							SetAction(so, PLAYER_FENRIR_ATTACK_SPEAR, 1);
						else
							SetAction(so, PLAYER_HIT_SKILL_SPEAR, true);
						sc->AttackTime = 1;
						break;
					case Skill_Death_Stab:
						goto LABEL_102;
					case Skill_Triple_Shot:
						goto LABEL_106;
					case Skill_Multi_Shot:
						SetPlayerBow(sc);
						if (sc != Hero && so->Type == MODEL_PLAYER)
							so->AnimationFrame = 0.0;
						sc->AttackTime = 1;
						break;
					case 0x36u:
						SetPlayerBow(sc);
						sc->AttackTime = 1;
						break;
					case Skill_Fire_Slash:
						goto LABEL_112;
					case Skill_Penetration:
						goto LABEL_117;
					case Skill_Ice_Arrow:
						goto LABEL_118;
					case Skill_Crescent_Moon_Slash:
						SetAction(so, PLAYER_HIT_RUSH, 1);
						sc->AttackTime = 1;
						PlayBuffer(SOUND_SKILL_SWORD2, 0, 0);
						break;
					case Skill_Lance:
						if (sc != Hero && so->Type == MODEL_PLAYER)
						{
							SetPlayerMagic(sc);
							so->AnimationFrame = 0.0;
						}
						so->AnimationFrame = 0.0;
						sc->AttackTime = 1;
						break;
					case Skill_Mana_Rays:
						SetAction(so, PLAYER_HIT_DEATH_CANNON, 1);
						so->AnimationFrame = 0.0;
						sc->AttackTime = 1;
						break;
					case Skill_Starfall:
						SetPlayerHighBow(sc);
						sc->AttackTime = 1;
						break;
					case Skill_Fire_Blast:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_STRIKE, true);
						}
						sc->AttackTime = 1;
						PlayBuffer(SOUND_ATTACK_FIRE_BUST, 0, 0);
						break;
					case Skill_Brand:
						g_CharacterRegisterBuff(to, eBuff_AddSkill);
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_SKILL_HAND1, true);
						}
						sc->AttackTime = 1;
						break;
					case Skill_Spiral_Slash:
						SetAction(so, PLAYER_HIT_ONE_FLASH, 1);
						sc->AttackTime = 1;
						PlayBuffer(SOUND_SKILL_SWORD2, 0, 0);
						break;
					case Skill_Stun:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_SKILL_VITALITY, true);
						}
						sc->AttackTime = 1;
						break;
					case Skill_Cancel_Stun:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_REMOVAL, true);
						}
						if (sc->SkillSuccess)
							UnRegisterBuff(eDeBuff_Stun, to);
						sc->AttackTime = 1;
						break;
					case Skill_Invisibility:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_REMOVAL, true);
						}
						if (sc->SkillSuccess
							&& !g_isCharacterBuff(to, eBuff_Cloaking)
							&& so != to)
						{
							g_CharacterRegisterBuff(to, eBuff_Cloaking);
						}
						sc->AttackTime = 1;
						break;
					case Skill_Cancel_Invisibility:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_REMOVAL, true);
						}
						if (sc->SkillSuccess)
							UnRegisterBuff(eBuff_Cloaking, to);

						sc->AttackTime = 1;
						break;
					case Skill_Swell_Mana:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_SKILL_VITALITY, true);
						}

						if (sc->SkillSuccess)
						{
							g_CharacterRegisterBuff(so, eBuff_AddMana);
							CreateEffect(MODEL_MANA_RUNE, so->Position, so->Angle, so->Light);
						}
						sc->AttackTime = 1;
						break;
					case Skill_Abolish_Magic:
						if (sc->SkillSuccess)
						{

							UnRegisterBuff(eBuff_Attack, to);
							UnRegisterBuff(eBuff_Defense, to);
							UnRegisterBuff(eBuff_HpRecovery, to);
							UnRegisterBuff(EFFECT_GREATER_LIFE_ENHANCED, to);
							UnRegisterBuff(eBuff_PhysDefense, to);
							UnRegisterBuff(eBuff_AddCriticalDamage, to);
							UnRegisterBuff(EFFECT_GREATER_CRITICAL_DAMAGE_MASTERED, to);
							UnRegisterBuff(eBuff_AddMana, to);
						}
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC, true);
						}
						else
						{
							SetAction(so, PLAYER_SKILL_VITALITY, true);
						}
						sc->AttackTime = 1;
						break;
					case Skill_Improve_ag:
						if (!g_isCharacterBuff(to, eBuff_AddAG))
						{
							DeleteEffect(BITMAP_LIGHT, to, 2);
							DeleteJoint(BITMAP_JOINT_HEALING, to, 9);

							CreateEffect(BITMAP_LIGHT, to->Position, to->Angle, to->Light, 2, to);
						}

						g_CharacterRegisterBuff(to, eBuff_AddAG);
						if (sc != Hero)
						{
							SetPlayerMagic(sc);
							so->AnimationFrame = 0.0;
						}
						sc->AttackTime = 1;
						break;
					case Skill_Increase_Critical_Damage:
						goto LABEL_247;
					case Skill_Summon:
						if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_TELEPORT, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_TELEPORT, true);
						}
						else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
						{
							SetAction(so, PLAYER_HIT_RIDE_TELEPORT, true);
						}
						else
						{
							SetAction(so, PLAYER_HIT_TELEPORT, true);
						}

						if (so->Type != MODEL_PLAYER)
						{
							SetPlayerAttack(sc);
							so->AnimationFrame = 0.0;
						}
						sc->AttackTime = 1;
						break;
					case Skill_Swell_Life:
						goto LABEL_274;
					case Skill_Fire_Breath:
						if (gMapManager->currentMap == WD_8TARKAN || gMapManager->currentMap == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
							SetAction(so, PLAYER_SKILL_RIDER_FLY, true);
						else
							SetAction(so, PLAYER_SKILL_RIDER, true);
						sc->AttackTime = 1;
						break;
					case Skill_Flame_of_Evil:
						sc->AttackTime = 1;
						break;
					case Skill_Combo:
						CreateEffect(MODEL_COMBO, so->Position, so->Angle, so->Light);
						PlayBuffer(SOUND_COMBO);
						break;
					case Skill_Nova_begin:
						SetAction(so, PLAYER_SKILL_HELL_BEGIN, 1);
						PlayBuffer(SOUND_NUKE1);
						so->m_bySkillCount = 0;
						break;
					case Skill_Nova:
						SetAction(so, PLAYER_SKILL_HELL_START, 1);
						if (sc == Hero)
						{
							MouseRButtonPop = 0;
							MouseRButtonPush = 0;
							MouseRButton = 0;
							MouseRButtonPress = 0;
						}
						sc->AttackTime = 1;
						break;
					case Skill_Fire_Scream:
						goto LABEL_292;
					case Skill_Explosion:
						for (int l = 0; l < 3; ++l)
						{
							vec3_t vPos;

							vPos[0] = to->Position[0] + (rand() % 160);
							vPos[1] = to->Position[1] + (rand() % 160);
							vPos[2] = to->Position[2];
							CreateBomb(vPos, true);
						}
						break;
					case Skill_Infinity_Arrow:
						goto LABEL_303;
					case Skill_Chain_Lightning:
						goto LABEL_304;
					case Skill_Damage_Reflection:
					case Skill_Berserker:
					case Skill_Sleep:
					case 0xDCu:
						goto LABEL_305;
					case Skill_Expansion_of_Wizardry:
						goto LABEL_319;
					case Skill_Doppeganger_Self_Explosion_Skil:
						SetAction(so, MONSTER01_APEAR, true);
						so->m_bActionStart = 0;
						break;
					case Skill_Chaotic_Diseier:
						goto LABEL_321;
					default:
						break;
					}
				}
			}
		}
		else
		{
			g_pChatListBox->AddText("", GlobalText[2249], SEASON3B::TYPE_SYSTEM_MESSAGE);
		}
	}
	else
	{
		SendHackingChecked(0, 25);
	}
	return FALSE;
}

BOOL ReceiveMagicContinue(BYTE* ReceiveBuffer, int Size, BOOL bEncrypted)
{
	if (bEncrypted)
	{
		LPPRECEIVE_MAGIC_CONTINUE Data = (LPPRECEIVE_MAGIC_CONTINUE)ReceiveBuffer;
		int Key = Data->KeyL + (Data->KeyH << 8);
		WORD MagicNumber = Data->MagicL + (Data->MagicH << 8);

		int index = FindCharacterIndex(Key);

		CHARACTER* sc = gmCharacters->GetCharacter(index);

		if (sc)
		{
			OBJECT* so = &sc->Object;
			sc->Skill = MagicNumber;
			so->Angle[2] = (double)Data->Angle / 255.0 * 360.0;

			if (so->Type == MODEL_PLAYER)
			{
				so->start_skill_hide(MagicNumber);

				int sc_type_helper = sc->Helper.Type;

				g_ConsoleDebug->Write(5, "[skill: %d]", MagicNumber);


				if (sc != Hero)
				{
					if ((signed int)MagicNumber > Skill_Phoenix_Shot)
					{
						switch (MagicNumber)
						{
						case Skill_Triple_Shot_Strengthener:
						case Skill_Triple_Shot_Mastery:
						LABEL_11:
							SetPlayerAttack(sc);
							break;
						case Skill_Decay_Strengthener:
						case Skill_Ice_Strengthener:
						LABEL_13:
							SetPlayerMagic(sc);
							break;
						case Skill_Penetration_Strengthener:
						LABEL_14:
							SetPlayerAttack(sc);
							break;
						case Skill_Hellfire_Strengthener:
						LABEL_16:
							SetAction(so, PLAYER_SKILL_HELL);
							break;
						case Skill_Inferno_Strengthener:
						case Skill_Inferno_Strengthener2:
						LABEL_17:
							SetAction(so, PLAYER_SKILL_INFERNO);
							break;
						case Skill_Twisting_Slash_Strengthener:
						case Skill_Twisting_Slash_Mastery:
						case Skill_Twisting_Slash_Strengthener2:
						LABEL_18:
							SetAction(so, PLAYER_HIT_SKILL_WHEEL);
							break;
						case Skill_Fire_Scream_Strengthener:
						case Skill_Fire_Scream_Mastery:
						LABEL_19:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else
							{
								SetAction(so, PLAYER_HIT_STRIKE, true);
							}

							PlayBuffer(SOUND_FIRE_SCREAM);
							break;
						case Skill_Electric_Spark_Strengthener:
						LABEL_26:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_FLASH, true);
							}
							/*if (sc->Helper.Type == (MODEL_HELPER + 37) || so->movementType == 5)
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH);
							else
								SetAction(so, PLAYER_SKILL_FLASH);*/
							break;
						case Skill_Dark_Horse_Strengthener:
						case Skill_Earthshake_Strengthener:
						case Skill_Earthshake_Mastery:
						LABEL_30:
							SetAction(so, PLAYER_HIT_DARKHORSE);
							PlayBuffer(SOUND_EARTH_QUAKE);
							break;
						case Skill_Anger_Blow_Strengthener:
						case Skill_Anger_Blow_Mastery:
						LABEL_31:
							SetAction(so, PLAYER_HIT_SKILL_FURY_STRIKE);
							break;
						case Skill_Strike_of_Destruction_Strengthe:
						LABEL_32:
							SetAction(so, PLAYER_SKILL_BLOW_OF_DESTRUCTION);
							break;
						case Skill_Fire_Slash_Strengthener:
						LABEL_37:
							SetAction(so, PLAYER_HIT_SKILL_WHEEL);
							break;
						case Skill_Power_Slash_Strengthener:
						LABEL_38:
							SetAction(so, PLAYER_HIT_TWO_HAND_SWORD_TWO);
							break;
						case Skill_Death_Stab_Strengthener:
						LABEL_39:
							SetAction(so, PLAYER_HIT_ONETOONE);
							break;
						case Skill_Chaotic_Diseier_Strengthener:
						LABEL_43:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_HIT_RIDE_STRIKE, true);
							}
							else
							{
								SetAction(so, PLAYER_HIT_STRIKE, true);
							}

							float Matrix[3][4];
							vec3_t Light, Position, Pos, Direction;

							Vector(0.f, 0.f, 0.f, Pos);

							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);

							Vector(0.5f, 0.5f, 0.5f, Light);
							for (int i = 0; i < 3; ++i)
							{
								CreateEffect(BITMAP_SHINY + 6, Position, so->Angle, Light, 3, so, -1, 0, 0, 0, 0.5f);
							}

							VectorCopy(so->Position, Position);

							for (int i = 0; i < 4; i++)
							{
								Position[0] = (so->Position[0] - 119.f) + (float)(rand() % 240);
								Position[2] = (so->Position[2] + 49.f) + (float)(rand() % 60);
								CreateJoint(BITMAP_2LINE_GHOST, Position, so->Position, so->Angle, 0, so, 20.f, so->PKKey, 0, so->m_bySkillSerialNum);
							}
							{
								CHARACTER* st = gmCharacters->GetCharacter(SelectedCharacter);

								if (sc == Hero && st)
								{
									vec3_t Pos;
									VectorCopy(st->Object.Position, Pos);
									CreateBomb(Pos, true);
								}
								PlayBuffer(SOUND_SKILL_CAOTIC);
							}

							break;
						case Skill_Drain_Life_Strengthener:
						LABEL_84:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_DRAIN_LIFE_FENRIR, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_DRAIN_LIFE_UNI, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_DRAIN_LIFE_DINO, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_DRAIN_LIFE_FENRIR, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_DRAIN_LIFE, true);
							}
							break;
						case Skill_Lightning_Shock_Strengthener:
						LABEL_103:
							SetAction(so, PLAYER_SKILL_LIGHTNING_SHOCK, true);
							break;
						case Skill_Dragon_Roar_Strengthener:
						LABEL_104:
							if (sc != Hero)
								g_CMonkSystem.RageCreateEffect(so, MagicNumber);
							g_CMonkSystem.SetRageSkillAni(MagicNumber, so);
							break;
						default:
						LABEL_108:
							SetPlayerMagic(sc);
							break;
						}
					}
					else if (MagicNumber == Skill_Phoenix_Shot)
					{
						g_CMonkSystem.SetRageSkillAni(MagicNumber, so);
						//sub_50ECD0(MagicNumber, v82);
					}
					else
					{
						switch (MagicNumber)
						{
						case Skill_Triple_Shot:
							goto LABEL_11;
						case 0x36u:
							SetPlayerAttack(sc);
							goto LABEL_109;
						case Skill_Decay:
						case Skill_Ice_Storm:
							goto LABEL_13;
						case Skill_Penetration:
							goto LABEL_14;
						case Skill_Aqua_Beam:
							SetAction(so, PLAYER_SKILL_FLASH, true);
							goto LABEL_109;
						case Skill_Hellfire:
							goto LABEL_16;
						case Skill_Inferno:
							goto LABEL_17;
						case Skill_Twisting_Slash:
							goto LABEL_18;
						case Skill_Fire_Scream:
							goto LABEL_19;
						case Skill_Electric_Spike:
							goto LABEL_26;
						case Skill_Earthshake:
							goto LABEL_30;
						case Skill_Rageful_Blow:
							goto LABEL_31;
						case Skill_Strike_of_Destruction:
							goto LABEL_32;
						case Skill_Impale:
							if (sc_type_helper == (MODEL_HELPER + 37) || so->movementType == Movement::Fenrir)
							{
								SetAction(so, PLAYER_FENRIR_ATTACK_SPEAR, true);
							}
							else
							{
								SetAction(so, PLAYER_HIT_SKILL_SPEAR, true);
							}
							goto LABEL_109;
						case Skill_Fire_Slash:
							goto LABEL_37;
						case Skill_Power_Slash:
							goto LABEL_38;
						case Skill_Death_Stab:
							goto LABEL_39;
						case Skill_Stun:
							SetAction(so, PLAYER_SKILL_VITALITY, 1);
							goto LABEL_109;
						case Skill_Invisibility:
							SetAction(so, PLAYER_SKILL_VITALITY, 1);
							goto LABEL_109;
						case Skill_Plasma_Storm:
							SetAction_Fenrir_Skill(sc, so);
							goto LABEL_109;
						case Skill_Chaotic_Diseier:
							goto LABEL_43;
						case Skill_Multi_Shot:
						{
							float Matrix[3][4];
							vec3_t Light, Position, Pos, Direction;

							SetPlayerBow(sc);

							Vector(0.f, 20.f, 0.f, Pos);
							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);
							Vector(0.8f, 0.9f, 1.6f, Light);
							CreateEffect(MODEL_MULTI_SHOT3, Position, so->Angle, Light, 0);
							CreateEffect(MODEL_MULTI_SHOT3, Position, so->Angle, Light, 0);

							Vector(0.f, 0.f, 0.f, Pos);
							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);

							CreateEffect(MODEL_MULTI_SHOT1, Position, so->Angle, Light, 0);
							CreateEffect(MODEL_MULTI_SHOT1, Position, so->Angle, Light, 0);
							CreateEffect(MODEL_MULTI_SHOT1, Position, so->Angle, Light, 0);

							Vector(0.f, 20.f, 0.f, Pos);
							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);
							CreateEffect(MODEL_MULTI_SHOT2, Position, so->Angle, Light, 0);
							CreateEffect(MODEL_MULTI_SHOT2, Position, so->Angle, Light, 0);

							Vector(0.f, -120.f, 145.f, Pos);
							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);

							int v42 = 0;
							short Key = -1;

							while (2)
							{
								if (v42 < 400)
								{
									CHARACTER* tc = gmCharacters->GetCharacter(v42);
									if (tc != sc)
									{
										++v42;
										continue;
									}
									Key = v42;
								}
								break;
							}
							CreateEffect(MODEL_BLADE_SKILL, Position, so->Angle, Light, 1, so, Key);
							goto LABEL_109;
						}
						case Skill_Recovery:
						{
							float Matrix[3][4];
							vec3_t vFirePosition;
							vec3_t Light, Position, Pos, Direction;

							Vector(0.4f, 0.6f, 1.f, Light);
							Vector(0.f, 0.f, 0.f, Pos);
							AngleMatrix(so->Angle, Matrix);
							VectorRotate(Pos, Matrix, Direction);
							VectorAdd(Direction, so->Position, Position);
							Position[2] += 130;
							VectorCopy(so->Position, Position);
							for (int i = 0; i < 19; ++i)
							{
								CreateJoint(BITMAP_FLARE, Position, Position, so->Angle, 47, so, 40, 2);
							}
							Vector(0.3f, 0.2f, 0.1f, Light);
							CreateEffect(MODEL_SUMMON, Position, so->Angle, Light, 0);
							CreateEffect(BITMAP_TWLIGHT, Position, so->Angle, Light, 0);
							CreateEffect(BITMAP_TWLIGHT, Position, so->Angle, Light, 1);
							CreateEffect(BITMAP_TWLIGHT, Position, so->Angle, Light, 2);

							BMD* pModels = gmClientModels->GetModel(so->Type);

							if (pModels)
							{
								for (int i = 0; i < 2; ++i)
								{
									int iNumBones = pModels->NumBones;
									pModels->TransformByObjectBone(vFirePosition, so, rand() % iNumBones);
									CreateEffect(BITMAP_FLARE, vFirePosition, so->Angle, Light, 3);
								}
								PlayBuffer(SOUND_SKILL_RECOVER);
							}
						}
						break;
						case Skill_Electric_Surge:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_LIGHTNING_ORB_FENRIR, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_LIGHTNING_ORB_UNI, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_LIGHTNING_ORB_DINO, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_LIGHTNING_ORB_UNI, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_LIGHTNING_ORB, true);
							}
							goto LABEL_109;
						case Skill_Drain_Life:
							goto LABEL_84;
						case Skill_Weakness:
						case Skill_Innovation:
							if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_FENRIR, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_UNI, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_DINO, true);
							}
							else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
							{
								SetAction(so, PLAYER_SKILL_SLEEP_UNI, true);
							}
							else
							{
								SetAction(so, PLAYER_SKILL_SLEEP, true);
							}
							goto LABEL_109;
						case Skill_Explosion_Sahamutt:
						case Skill_Requiem:
						case Skill_Pollution:
							g_SummonSystem.CastSummonSkill(MagicNumber, sc, so, Data->PositionX, Data->PositionY);
							goto LABEL_109;
						case Skill_Flame_Strike:
							SetAction(so, PLAYER_SKILL_FLAMESTRIKE, true);
							goto LABEL_109;
						case Skill_Gigantic_Storm:
							SetAction(so, PLAYER_SKILL_GIGANTICSTORM, true);
							goto LABEL_109;
						case Skill_Lightning_Shock:
							goto LABEL_103;
						case Skill_Dragon_Roar:
							goto LABEL_104;
						default:
							goto LABEL_108;
						}
					}
				LABEL_109:
					so->AnimationFrame = 0.0;
				}
			}
			else
			{
				SetPlayerAttack(sc);
				so->AnimationFrame = 0.0;
			}

			sc->AttackTime = 1;
			sc->TargetCharacter = -1;
			sc->AttackFlag = ATTACK_FAIL;
			sc->SkillX = Data->PositionX;
			sc->SkillY = Data->PositionY;
		}
	}
	else
	{
		SendHackingChecked(0, 30);
	}

	return (TRUE);
}

// ChainLightning
void ReceiveChainMagic(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CHAIN_MAGIC pPacketData = (LPPRECEIVE_CHAIN_MAGIC)ReceiveBuffer;
	CHARACTER* pSourceChar = gmCharacters->GetCharacter(FindCharacterIndex(pPacketData->wUserIndex));

	if (pSourceChar)
	{
		OBJECT* pSourceObject = &pSourceChar->Object;
		OBJECT* pTempObject = NULL;

		switch (pSourceChar->Helper.Type)
		{
		case MODEL_HELPER + 2:
			SetAction(pSourceObject, PLAYER_SKILL_CHAIN_LIGHTNING_UNI);
			break;
		case MODEL_HELPER + 3:
			SetAction(pSourceObject, PLAYER_SKILL_CHAIN_LIGHTNING_DINO);
			break;
		case MODEL_HELPER + 37:
			SetAction(pSourceObject, PLAYER_SKILL_CHAIN_LIGHTNING_FENRIR);
			break;
		default:
			SetAction(pSourceObject, PLAYER_SKILL_CHAIN_LIGHTNING);
			break;
		}

		pSourceChar->Skill = ((WORD)(pPacketData->MagicH) << 8) + pPacketData->MagicL;

		pTempObject = pSourceObject;

		int iOffset = sizeof(PRECEIVE_CHAIN_MAGIC);
		for (int i = 0; i < (int)(pPacketData->byCount); i++)
		{
			LPPRECEIVE_CHAIN_MAGIC_OBJECT pPacketData2 = (LPPRECEIVE_CHAIN_MAGIC_OBJECT)(ReceiveBuffer + iOffset);
			CHARACTER* pTargetChar = gmCharacters->GetCharacter(FindCharacterIndex(pPacketData2->wTargetIndex));

			if (!pTargetChar)
				break;

			OBJECT* pTargetObject = &pTargetChar->Object;

			if (pTempObject != pTargetObject && pTargetObject != NULL && pTargetObject->Live == true)
			{
				vec3_t vAngle;
				Vector(-60.f, 0.f, pSourceObject->Angle[2], vAngle);
				CreateEffect(MODEL_CHAIN_LIGHTNING, pSourceObject->Position, vAngle, pSourceObject->Light, i, pTempObject, -1, 0, 0, 0, 0.f, pPacketData2->wTargetIndex);
			}

			pTempObject = pTargetObject;

			iOffset += sizeof(PRECEIVE_CHAIN_MAGIC_OBJECT);
		}
	}
}

void ReceiveMagicPosition(BYTE* ReceiveBuffer, int Size)
{
	LPPRECEIVE_MAGIC_POSITIONS Data = (LPPRECEIVE_MAGIC_POSITIONS)ReceiveBuffer;
	int SourceKey = ((int)(Data->KeyH) << 8) + Data->KeyL;
	WORD MagicNumber = ((WORD)(Data->MagicH) << 8) + Data->MagicL;

	int SourceIndex = FindCharacterIndex(SourceKey);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(SourceIndex);

	if (pCharacter)
	{
		AttackPlayer = SourceIndex;

		OBJECT* o = &pCharacter->Object;
		CreateMagicShiny(pCharacter);
		PlayBuffer(SOUND_MAGIC);
		SetAction(o, PLAYER_SKILL_HELL);
		pCharacter->Skill = MagicNumber;
		pCharacter->AttackTime = 1;// = 1;

		int Offset = sizeof(PRECEIVE_MAGIC_POSITIONS);
		for (int i = 0; i < Data->Count; i++)
		{
			LPPRECEIVE_MAGIC_POSITION Data2 = (LPPRECEIVE_MAGIC_POSITION)(ReceiveBuffer + Offset);
			int TargetKey = ((int)(Data2->KeyH) << 8) + Data2->KeyL;

			CHARACTER* tc = gmCharacters->GetCharacter(FindCharacterIndex(TargetKey));

			if (tc == NULL)
				break;

			OBJECT* to = &tc->Object;

			if (rand() % 2 == 0)
				SetPlayerShock(tc, tc->Hit);

			if (tc->Hit > 0)
			{
				vec3_t Light;
				Vector(1.f, 0.6f, 0.f, Light);
				CreatePoint(to->Position, tc->Hit, Light);
				if (TargetKey == HeroKey)
				{
					if (tc->Hit >= CharacterAttribute->Life)
						CharacterAttribute->Life = 0;
					else
						CharacterAttribute->Life -= tc->Hit;
				}
			}
			Offset += sizeof(PRECEIVE_MAGIC_POSITION);
		}
	}
}

void ReceiveSkillCount(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_EX_SKILL_COUNT Data = (LPPRECEIVE_EX_SKILL_COUNT)ReceiveBuffer;
	int TargetKey = ((int)(Data->KeyH) << 8) + Data->KeyL;

	CHARACTER* tc = gmCharacters->GetCharacter(FindCharacterIndex(TargetKey));

	if (tc)
	{
		OBJECT* to = &tc->Object;

		if (Data->m_byType == Skill_Nova)
		{
			to->m_bySkillCount = Data->m_byCount;
		}
	}
}

BOOL ReceiveDieExp(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPRECEIVE_DIE Data = (LPPRECEIVE_DIE)ReceiveBuffer;
	int     Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	DWORD   Exp = ((DWORD)(Data->ExpH) << 8) + Data->ExpL;
	int     Damage = Data->Damage;
	int     Success = (Key >> 15);
	Key &= 0x7FFF;

	int Index = FindCharacterIndex(Key);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;
		vec3_t Light;

		Vector(1.f, 0.6f, 0.f, Light);
		if (Success)
		{
			SetPlayerDie(pCharacter);
			CreatePoint(o->Position, Damage, Light);
		}
		else
		{
			Hero->AttackFlag = ATTACK_DIE;
			Hero->Damage = Damage;
			Hero->TargetCharacter = Index;
			CreatePoint(o->Position, Damage, Light);
		}
		pCharacter->Dead = 1;
		pCharacter->Movement = false;

		if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
		{
			g_pMainFrame->SetPreExp_Wide(Master_Level_Data.lMasterLevel_Experince);
			g_pMainFrame->SetGetExp_Wide(Exp);
			Master_Level_Data.lMasterLevel_Experince += Exp;
		}
		else
		{
			g_pMainFrame->SetPreExp(CharacterAttribute->Experience);
			g_pMainFrame->SetGetExp(Exp);
			CharacterAttribute->Experience += Exp;
		}

		if (Exp > 0)
		{

			char Text[100];
			if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
			{
				sprintf(Text, GlobalText[1750], Exp);
			}
			else
				sprintf(Text, GlobalText[486], Exp);
			g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
		}

#ifdef CONSOLE_DEBUG
		g_ConsoleDebug->Write(MCD_RECEIVE, "0x16 [ReceiveDieExp : %d]", Exp);
#endif // CONSOLE_DEBUG	

	}
	return (TRUE);
}

BOOL ReceiveDieExpLarge(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPRECEIVE_DIE2 Data = (LPPRECEIVE_DIE2)ReceiveBuffer;
	int     Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	DWORD   Exp = ((DWORD)(Data->ExpH) << 16) + Data->ExpL;
	int     Damage = Data->Damage;
	int     Success = (Key >> 15);
	Key &= 0x7FFF;

	int Index = FindCharacterIndex(Key);
	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c)
	{
		OBJECT* o = &c->Object;
		vec3_t Light;
		Vector(1.f, 0.6f, 0.f, Light);
		if (Success)
		{
			SetPlayerDie(c);
			CreatePoint(o->Position, Damage, Light);
		}
		else
		{
			Hero->AttackFlag = ATTACK_DIE;
			Hero->Damage = Damage;
			Hero->TargetCharacter = Index;
			CreatePoint(o->Position, Damage, Light);
		}
		c->Dead = 1;
		c->Movement = false;

		if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
		{
			g_pMainFrame->SetPreExp_Wide(Master_Level_Data.lMasterLevel_Experince);
			g_pMainFrame->SetGetExp_Wide(Exp);
			Master_Level_Data.lMasterLevel_Experince += Exp;
		}
		else
		{
			g_pMainFrame->SetPreExp(CharacterAttribute->Experience);
			g_pMainFrame->SetGetExp(Exp);
			CharacterAttribute->Experience += Exp;
		}

		if (Exp > 0)
		{
			char Text[100];

			if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
				sprintf(Text, GlobalText[1750], Exp);
			else
				sprintf(Text, GlobalText[486], Exp);
			g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
		}
	}
	return (TRUE);
}

void FallingStartCharacter(CHARACTER* c, OBJECT* o)
{
	BYTE positionX = (BYTE)(o->Position[0] / TERRAIN_SCALE);
	BYTE positionY = (BYTE)(o->Position[1] / TERRAIN_SCALE);
	int WallIndex = TERRAIN_INDEX_REPEAT(positionX, positionY);
	int Wall = TerrainWall[WallIndex] & TW_ACTION;

	o->m_bActionStart = false;

	if (gMapManager->InChaosCastle() == true && (TerrainWall[WallIndex] & TW_NOGROUND) == TW_NOGROUND)
	{
		c->StormTime = 0;
		o->m_bActionStart = true;
		o->Gravity = rand() % 10 + 10.f;
		o->Velocity -= rand() % 3 + 3.f;

		Vector(0.f, 0.f, 1.f, o->Direction);
		VectorCopy(o->Position, o->m_vDeadPosition);

		PlayBuffer(SOUND_CHAOS_FALLING);
	}
	else if (Wall == TW_ACTION)
	{
		c->StormTime = 0;
		o->m_bActionStart = true;
		o->Gravity = rand() % 10 + 10.f;
		o->Velocity = rand() % 20 + 20.f;
		float angle = rand() % 10 + 85.f;

		if ((TerrainWall[WallIndex + 1] & TW_NOGROUND) == TW_NOGROUND)
		{
			Vector(0.f, 0.f, -angle, o->m_vDownAngle);
		}
		else if ((TerrainWall[WallIndex - 1] & TW_NOGROUND) == TW_NOGROUND)
		{
			Vector(0.f, 0.f, angle, o->m_vDownAngle);
		}

		o->Angle[2] = o->m_vDownAngle[2];
		Vector(rand() % 6 + 8.f, -rand() % 2 + 13.f, 5.f, o->Direction);
		VectorCopy(o->Position, o->m_vDeadPosition);
	}

	if (c == Hero)
	{
		matchEvent::ClearMatchInfo();
	}
}

void ReceiveDie(BYTE* ReceiveBuffer, int Size)
{
	LPPHEADER_DEFAULT_DIE Data = (LPPHEADER_DEFAULT_DIE)ReceiveBuffer;

	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	int Index = FindCharacterIndex(Key);

	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c)
	{
		OBJECT* o = &c->Object;
		c->Dead = 1;
		c->Movement = false;

		WORD SkillType = ((int)(Data->MagicH) << 8) + Data->MagicL;

		c->m_byDieType = SkillType;

		if (gMapManager->InBloodCastle() == true)
		{
			FallingStartCharacter(c, o);
		}
		else if (gMapManager->InChaosCastle() == true)
		{
			FallingStartCharacter(c, o);
		}
		else
		{
			if (c == Hero && g_PortalMgr.IsPortalUsable())
			{
				g_PortalMgr.SaveRevivePosition();
			}

			o->m_bActionStart = false;

			switch (SkillType)
			{
			case AT_SKILL_BLAST_HELL:
			case AT_SKILL_COMBO:
				o->m_bActionStart = true;
				c->StormTime = 0;
				o->Velocity = (rand() % 5 + 10.f) * 0.1f;
				o->m_bySkillCount = 0;

				Vector(0.f, rand() % 15 + 40.f, 0.f, o->Direction);
				VectorCopy(o->Position, o->m_vDeadPosition);
				break;
			}

			if (SkillType == AT_SKILL_BLAST_HELL || SkillType == AT_SKILL_COMBO)
			{
				int TKey = ((int)(Data->TKeyH) << 8) + Data->TKeyL;

				CHARACTER* tc = gmCharacters->GetCharacter(FindCharacterIndex(TKey));

				if (tc)
				{
					OBJECT* to = &tc->Object;
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], to->Position[0], to->Position[1]);
					VectorCopy(o->Angle, o->HeadAngle);
				}
			}
		}
	}
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x17 [ReceiveDie(%d)]", Key);
}

void ReceiveCreateItemViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);

	for (int i = 0; i < Data->Value; i++)
	{
		LPPCREATE_ITEM Data2 = (LPPCREATE_ITEM)(ReceiveBuffer + Offset);

		vec3_t Position;
		Position[0] = (float)(Data2->PositionX + 0.5f) * TERRAIN_SCALE;
		Position[1] = (float)(Data2->PositionY + 0.5f) * TERRAIN_SCALE;
		int Key = ((int)(Data2->KeyH) << 8) + Data2->KeyL;
		int CreateFlag = (Key >> 15);
		Key &= 0x7FFF;

		if (Key < 0 || Key >= MAX_ITEMS)
			Key = 0;

		Items[Key].Key = Key;
		CreateItem(&Items[Key], Data2->Item, Position, CreateFlag);

		Offset += sizeof(PCREATE_ITEM);
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x20 [ReceiveCreateItemViewport]");
}

void ReceiveDeleteItemViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);
	for (int i = 0; i < Data->Value; i++)
	{
		LPPDELETE_CHARACTER Data2 = (LPPDELETE_CHARACTER)(ReceiveBuffer + Offset);
		int Key = ((int)(Data2->KeyH) << 8) + Data2->KeyL;

		if (Key < 0 || Key >= MAX_ITEMS)
			Key = 0;

		Items[Key].Object.Live = false;

		//GMItemDropName->RemoveInfoUI(Key);
		Offset += sizeof(PDELETE_CHARACTER);
	}
}

void ReceiveGetItem(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GET_ITEM Data = (LPPRECEIVE_GET_ITEM)ReceiveBuffer;

	if (Data->Result != NOT_GET_ITEM)
	{
		if (Data->Result == GET_ITEM_ZEN)
		{
			char szMessage[128];
			int backupGold = CharacterMachine->Gold;
			CharacterMachine->Gold = (Data->Item[0] << 24) + (Data->Item[1] << 16) + (Data->Item[2] << 8) + (Data->Item[3]);

			int getGold = CharacterMachine->Gold - backupGold;

			if (getGold > 0)
			{
				sprintf(szMessage, "%d %s %s", getGold, GlobalText[224], GlobalText[918]); //zen ** đã được nhặt
				g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
		}
		else
		{
			if (Data->Result != GET_ITEM_MULTI)
			{
				if (Data->Result >= MAX_EQUIPMENT_INDEX && Data->Result < MAX_MY_INVENTORY_INDEX)
					g_pMyInventory->InsertItem(Data->Result - MAX_EQUIPMENT_INDEX, Data->Item);
			}

			char szItem[64] = { 0, };
			int level = (Items[ItemKey].Item.Level >> 3) & 15;
			GetItemName(Items[ItemKey].Item.Type, level, szItem);

			char szMessage[128];
			sprintf(szMessage, "%s %s", szItem, GlobalText[918]); //tên item đã được nhặt

			if (GMProtect->PickItemTextType())
			{
				g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_GM_MESSAGE);
			}
			else
			{
				g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}

			
		}
#ifdef FOR_WORK
		Items[ItemKey].Object.Live = false;
#endif

		int Type = ConvertItemType(Data->Item);
		if (Type == ITEM_POTION + 13 || Type == ITEM_POTION + 14 || Type == ITEM_POTION + 16 || Type == ITEM_WING + 15 || Type == ITEM_POTION + 22
			|| Type == INDEX_COMPILED_CELE || Type == INDEX_COMPILED_SOUL || Type == ITEM_POTION + 31 || GMJewelOfAction->CheckOfJewelByItem(Type))
			PlayBuffer(SOUND_JEWEL01, &Hero->Object);
		else if (Type == ITEM_POTION + 41)
			PlayBuffer(SOUND_JEWEL02, &Hero->Object);
		else
			PlayBuffer(SOUND_GET_ITEM01, &Hero->Object);
	}
	SendGetItem = -1;

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x22 [ReceiveGetItem(%d)]", Data->Result);
}

void ReceiveDropItem(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_KEY Data = (LPPHEADER_DEFAULT_KEY)ReceiveBuffer;
	if (Data->KeyH)
	{
		if (Data->KeyL < MAX_EQUIPMENT_INDEX || Data->KeyL >= INVENTORY_FULL_SIZE)
		{
			g_pMyInventory->UnequipItem(Data->KeyL);
		}
		else
		{
			g_pMyInventory->DeleteItem(Data->KeyL - MAX_EQUIPMENT_INDEX);
		}

		SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();
	}
	else
	{
		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
	}
	SendDropItem = -1;
}

BOOL ReceiveEquipmentItem(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPHEADER_DEFAULT_SUBCODE_ITEM Data = (LPPHEADER_DEFAULT_SUBCODE_ITEM)ReceiveBuffer;

	if (bEncrypted)
	{
		EquipmentItem = false;

		if (Data->SubCode == 0xFF)
		{
			SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

			if (!g_pTrade->ProcessTradeItemAutoMoveFailure())
			{
				g_pStorageInventory->ProcessStorageItemAutoMoveFailure();
			}
			if (g_pGoblinMixExpansion->IsItemAutoMove())
			{
				g_pGoblinMixExpansion->ProcessItemAutoMoveFailure();
			}
			g_pMyInventory->ProcessItemAutoMoveFailure();
		}
		else
		{
			SEASON3B::CNewUIPickedItem* pPickedItem = SEASON3B::CNewUIInventoryCtrl::GetPickedItem();

			g_pMyInventory->ProcessItemAutoMoveSuccess();

			int iSourceIndex = g_pMyShopInventory->GetSourceIndex();

			if (pPickedItem)
			{
				if (g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory()) == -1)
				{
					if (pPickedItem->GetOwnerInventory() == g_pMyShopInventory->GetInventoryCtrl())
					{
						iSourceIndex += MAX_MY_INVENTORY_INDEX;
					}
				}
				else
				{
					iSourceIndex += MAX_EQUIPMENT_INDEX;
				}
			}

			if (iSourceIndex >= MAX_MY_INVENTORY_INDEX && iSourceIndex < INVENTORY_FULL_SIZE)
			{
				PERSONAL_ITEM_PRICE_TABLE price;
				if (GetPersonalItemPrice(iSourceIndex, price, g_IsPurchaseShop))
				{
					RemovePersonalItemPrice(iSourceIndex, g_IsPurchaseShop);
					if (Data->Index >= MAX_MY_INVENTORY_INDEX)
					{
						AddPersonalItemPrice(Data->Index, price, g_IsPurchaseShop);
					}
				}
			}

			if (g_pGoblinMixExpansion->IsItemAutoMove())
			{
				if (Data->SubCode == 0)
				{
					g_pGoblinMixExpansion->ProcessItemAutoMoveSuccess();
				}
			}

			if (Data->SubCode)
			{
				if (Data->SubCode == REQUEST_EQUIPMENT_TRADE)
				{
					g_pTrade->ProcessToReceiveTradeItems(Data->Index, Data->Item);
				}
				else if (Data->SubCode == REQUEST_EQUIPMENT_STORAGE)
				{
					g_pStorageInventory->ProcessToReceiveStorageItems(Data->Index, Data->Item);
				}
				else if (Data->SubCode == 23) //-- MixExpansion
				{
					SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

					g_pGoblinMixExpansion->InsertItem(Data->Index, Data->Item);
				}
			}
			else
			{
				SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

				int itemindex = Data->Index;
				if (CharacterMachine->CheckEquipment(itemindex) == true)
				{
					g_pMyInventory->EquipItem(itemindex, Data->Item, true);
				}
				else
				{
					if (itemindex < MAX_EQUIPMENT_INDEX || itemindex >= MAX_MY_INVENTORY_INDEX)
					{
						if (itemindex >= MAX_MY_INVENTORY_INDEX && itemindex < MAX_MY_SHOP_INVENTORY_INDEX)
						{
							g_pMyShopInventory->InsertItem(itemindex - MAX_MY_INVENTORY_INDEX, Data->Item);
						}
					}
					else
					{
						if (!g_pTrade->ProcessTradeItemAutoMoveSuccess())
						{
							g_pStorageInventory->ProcessStorageItemAutoMoveSuccess();
						}
						g_pMyInventory->InsertItem(itemindex - MAX_EQUIPMENT_INDEX, Data->Item);
					}
				}
			}
			if (Data->SubCode != REQUEST_EQUIPMENT_CHAOS_MIX
				&& (Data->SubCode < REQUEST_EQUIPMENT_TRAINER_MIX || Data->SubCode > REQUEST_EQUIPMENT_JERRIDON_MIX)
				&& (Data->SubCode != REQUEST_EQUIPMENT_CHAOS_CARD_MIX)
				&& (Data->SubCode != REQUEST_EQUIPMENT_CHERRYBLOSSOM_MIX)
				&& (Data->SubCode < REQUEST_EQUIPMENT_EXTRACT_SEED_MIX || Data->SubCode > REQUEST_EQUIPMENT_DETACH_SOCKET_MIX)
				)
			{
				if (REQUEST_EQUIPMENT_LUCKY_SET_MIX == Data->SubCode || REQUEST_EQUIPMENT_LUCKY_ITEM_MIX == Data->SubCode)
				{
					g_pLuckyItemWnd->GetResult(1, Data->Index, Data->Item);
				}
			}
			else
			{
				SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

				if (Data->Index < MAX_MIX_INVENTORY)
				{
					g_pMixInventory->InsertItem(Data->Index, Data->Item);
				}
			}

			PlayBuffer(SOUND_GET_ITEM01);
		}

		if (g_bPacketAfter_EquipmentItem)
		{
			ReceiveTradeExit(g_byPacketAfter_EquipmentItem);
			g_bPacketAfter_EquipmentItem = FALSE;
		}
		g_ConsoleDebug->Write(MCD_RECEIVE, "0x24 [ReceiveEquipmentItem(%d %d)]", Data->SubCode, Data->Index);
	}
	return TRUE;
}

void ReceiveModifyItem(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE_ITEM Data = (LPPHEADER_DEFAULT_SUBCODE_ITEM)ReceiveBuffer;

	if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
		SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

	int iSrcIndex = Data->Index - MAX_EQUIPMENT_INDEX;

	if (g_pMyInventory->FindItem(iSrcIndex))
		g_pMyInventory->DeleteItem(iSrcIndex);

	g_pMyInventory->InsertItem(iSrcIndex, Data->Item);

	int iType = ConvertItemType(Data->Item);
	if (iType == ITEM_POTION + 28 || iType == ITEM_POTION + 111)
	{
		PlayBuffer(SOUND_KUNDUN_ITEM_SOUND);
	}
	else if (GambleSystem::Instance().IsGambleShop())
	{

	}
	else
	{
		PlayBuffer(SOUND_JEWEL01);
	}
}

#ifdef ITEM_CHAT_SELL_SYSTEM
void ReceivePostChatItem(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE_ITEM_ID Data = (LPPHEADER_DEFAULT_SUBCODE_ITEM_ID)ReceiveBuffer;

	ITEM* pNewItem = g_pNewItemMng->CreateItem(Data->Item);

	if (NULL != pNewItem)
	{
		char TextName[100];
		char TextChat[200];
		memset(TextName, 0, sizeof(TextName));
		memset(TextChat, 0, sizeof(TextChat));

		g_pNewItemTooltip->CreateItemName(pNewItem, TextName);

		if (TextName[0] != 0)
		{
			char CharacterName[MAX_ID_SIZE + 1];
			memset(CharacterName, 0, MAX_ID_SIZE + 1);
			memcpy(CharacterName, (char*)Data->ID, MAX_ID_SIZE);

			sprintf_s(TextChat, "[SELL] ");

			strcat(TextChat, TextName);

			gmHeadChat->AssignChat(CharacterName, TextChat, 0);

			g_pChatListBox->AddText(CharacterName, TextChat, MESSAGE_TYPE::TYPE_WHISPER_MESSAGE, MESSAGE_TYPE::TYPE_ALL_MESSAGE, pNewItem);
		}

		PlayBuffer(SOUND_WHISPER);
	}
}
#endif // ITEM_CHAT_SELL_SYSTEM

BOOL ReceiveTalk(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;

	g_pNewUISystem->HideAll();

	switch (Data->Value)
	{
	case 2:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_STORAGE);
		break;
	case 3:
		g_MixRecipeMgr.SetMixType(SEASON3A::MIXTYPE_GOBLIN_NORMAL);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_MIXINVENTORY);
		break;
	case 4:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_DEVILSQUARE);
		break;
	case 5:
		g_pUIManager->Open(::INTERFACE_SERVERDIVISION);
		break;
	case 6:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_BLOODCASTLE);
		break;
	case 7:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CTrainerMenuMsgBoxLayout));
		break;
	case INDEX_NPC_LAHAP:
	{
		if (COMGEM::isAble())
		{
			g_pNewUISystem->HideAll();
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGemIntegrationMsgBoxLayout));
		}
	}
	break;
	case 0x0C:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_SENATUS);
		break;
	case 0x0D:
		SendRequestBCStatus();
		break;
	case 0x11:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CElpisMsgBoxLayout));
	}
	break;
	case 0x12:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::COsbourneMsgBoxLayout));
	}
	break;
	case 0x13:
	{
		g_MixRecipeMgr.SetMixType(SEASON3A::MIXTYPE_JERRIDON);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_MIXINVENTORY);
	}
	break;
	case 0x14:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_CURSEDTEMPLE_NPC);

		BYTE* cursedtempleenterinfo = (&Data->Value) + 1;
		g_pCursedTempleEnterWindow->SetCursedTempleEnterInfo(cursedtempleenterinfo);
	}
	break;
	case 0x15:
	{
		g_MixRecipeMgr.SetMixType(SEASON3A::MIXTYPE_CHAOS_CARD);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_MIXINVENTORY);
	}
	break;
	case 0x16:
	{
		g_MixRecipeMgr.SetMixType(SEASON3A::MIXTYPE_CHERRYBLOSSOM);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_MIXINVENTORY);
	}
	break;
	case 0x17:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSeedMasterMenuMsgBoxLayout));
	}
	break;
	case 0x18:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSeedInvestigatorMenuMsgBoxLayout));
	}
	break;
	case 0x19:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CResetCharacterPointMsgBoxLayout));
	}
	break;
	case 0x20:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDelgardoMainMenuMsgBoxLayout));
	}
	break;
	case 0x21:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_DUELWATCH);
	}
	break;
	case 0x22:
	{
		GambleSystem::Instance().SetGambleShop();
		g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCSHOP);
	}
	break;
	case 0x23:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_DOPPELGANGER_NPC);
		BYTE* pbtRemainTime = (&Data->Value) + 1;
		g_pDoppelGangerWindow->SetRemainTime(*pbtRemainTime);
	}
	break;
	case 0x24:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC);
	}
	break;
	case 0x25:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA);
	}
	break;
	case 0x26:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CLuckyTradeMenuMsgBoxLayout));
	}
	break;
	case 0x28:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_MIX_EXPANSION);
	}
	break;
	default:
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCSHOP);
	}
	break;
	}
	PlayBuffer(SOUND_CLICK01);
	PlayBuffer(SOUND_INTERFACE01);


	if (!gwinhandle->CheckWndMode())
	{
		int x = 260 * MouseX / GetWindowsX;
		SetCursorPos((x)*g_fScreenRate_x, (MouseY)*g_fScreenRate_y);
	}

	return (TRUE);
}

void ReceiveBuy(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_ITEM Data = (LPPHEADER_DEFAULT_ITEM)ReceiveBuffer;
	if (Data->Index != 255)
	{
		if (Data->Index >= MAX_EQUIPMENT_INDEX && Data->Index < MAX_MY_INVENTORY_INDEX)
		{
			g_pMyInventory->InsertItem(Data->Index - MAX_EQUIPMENT_INDEX, Data->Item);
		}
		else
		{
#ifdef _DEBUG
			__asm { int 3 };
#endif // _DEBUG
		}

		PlayBuffer(SOUND_GET_ITEM01);
	}
	if (Data->Index == 0xfe)
	{
		g_pNewUISystem->HideAll();

		g_pChatListBox->AddText(Hero->ID, GlobalText[732], SEASON3B::TYPE_ERROR_MESSAGE);
	}
	BuyCost = 0;

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x32 [ReceiveBuy(%d)]", Data->Index);
}

void ReceiveSell(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GOLD Data = (LPPRECEIVE_GOLD)ReceiveBuffer;

	if (Data->Flag != 0)
	{
		if (Data->Flag == 0xff)
		{
			SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();

			g_pChatListBox->AddText(Hero->ID, GlobalText[733], SEASON3B::TYPE_ERROR_MESSAGE);
		}
		else if (Data->Flag == 0xfe)
		{
			g_pNewUISystem->HideAll();

			g_pChatListBox->AddText(Hero->ID, GlobalText[733], SEASON3B::TYPE_ERROR_MESSAGE);
		}
		else
		{
			SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();

			CharacterMachine->Gold = Data->Gold;

			PlayBuffer(SOUND_GET_ITEM01);

			g_pNPCShop->SetSellingItem(false);
		}
	}
	else
	{
		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
	}
}

void ReceiveRepair(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_REPAIR_GOLD Data = (LPPRECEIVE_REPAIR_GOLD)ReceiveBuffer;

	if (Data->Gold != 0)
	{
		CharacterMachine->Gold = Data->Gold;
		CharacterMachine->CalculateAll();
		PlayBuffer(SOUND_REPAIR);
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x34 [ReceiveRepair(%d)]", Data->Gold);
}

void ReceiveLevelUp(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_LEVEL_UP Data = (LPPRECEIVE_LEVEL_UP)ReceiveBuffer;
	CharacterAttribute->Level = Data->Level;

#ifdef PROTO_EXTRA
	CharacterAttribute->LevelUpPoint = Data->btLevelUpPoint;
	CharacterAttribute->LifeMax = Data->btMaxLife;
	CharacterAttribute->ManaMax = Data->btMaxMana;
	CharacterAttribute->Life = Data->btMaxLife;
	CharacterAttribute->Mana = Data->btMaxMana;
	CharacterAttribute->Shield = Data->btMaxShield;
	CharacterAttribute->ShieldMax = Data->btMaxShield;
	CharacterAttribute->SkillMana = Data->btSkillManaMax;
	CharacterAttribute->SkillManaMax = Data->btSkillManaMax;
#else
	CharacterAttribute->LevelUpPoint = Data->LevelUpPoint;
	CharacterAttribute->LifeMax = Data->MaxLife;
	CharacterAttribute->ManaMax = Data->MaxMana;
	CharacterAttribute->Life = Data->MaxLife;
	CharacterAttribute->Mana = Data->MaxMana;
	CharacterAttribute->Shield = Data->MaxShield;
	CharacterAttribute->ShieldMax = Data->MaxShield;
	CharacterAttribute->SkillMana = Data->SkillManaMax;
	CharacterAttribute->SkillManaMax = Data->SkillManaMax;
#endif // PROTO_EXTRA

	CharacterAttribute->AddPoint = Data->AddPoint;
	CharacterAttribute->MaxAddPoint = Data->MaxAddPoint;
	CharacterAttribute->wMinusPoint = Data->wMinusPoint;
	CharacterAttribute->wMaxMinusPoint = Data->wMaxMinusPoint;

	unicode::t_char szText[256] = { NULL, };
	DWORD iExp = CharacterAttribute->NextExperince - CharacterAttribute->Experience;
	sprintf(szText, GlobalText[486], iExp);
	g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);

	CharacterMachine->CalculateNextExperince();

	OBJECT* o = &Hero->Object;

	if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 45, o, 80, 2);
		for (int i = 0; i < 19; ++i)
		{
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 46, o, 80, 2);
		}
	}
	else
	{
		for (int i = 0; i < 15; ++i)
		{
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
		}
		CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);
	}
	PlayBuffer(SOUND_LEVEL_UP);

	if (CharacterAttribute->Level == gmProtect->HelperActiveLevel)
	{
		if (gmProtect->HelperActiveAlert)
		{
			SEASON3B::CreateOkMessageBox(GlobalText[3606], -1);
		}
	}
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x05 [ReceiveLevelUp]");
}

void ReceiveAddPoint(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_ADD_POINT Data = (LPPRECEIVE_ADD_POINT)ReceiveBuffer;

#ifdef PROTO_EXTRA
	CharacterAttribute->Strength = Data->btStrength;
	CharacterAttribute->Dexterity = Data->btDexterity;
	CharacterAttribute->Vitality = Data->btVitality;
	CharacterAttribute->Energy = Data->btEnergy;
	CharacterAttribute->Charisma = Data->btCharisma;

	CharacterAttribute->LevelUpPoint = Data->btLevelUpPoint;
	CharacterAttribute->LifeMax = Data->btMaxLife;
	CharacterAttribute->ManaMax = Data->btMaxMana;
	CharacterAttribute->ShieldMax = Data->btMaxShield;
	CharacterAttribute->SkillManaMax = Data->btSkillManaMax;
#else
	if (Data->Result >> 4)
	{
		CharacterAttribute->LevelUpPoint--;
		switch (Data->Result & 0xf)
		{
		case 0:
			CharacterAttribute->Strength++;
			break;
		case 1:
			CharacterAttribute->Dexterity++;
			break;
		case 2:
			CharacterAttribute->Vitality++;
			CharacterAttribute->LifeMax = Data->Max;
			break;
		case 3:
			CharacterAttribute->Energy++;
			CharacterAttribute->ManaMax = Data->Max;
			break;
		case 4:
			CharacterAttribute->Charisma++;
			break;
		}

	}
#endif // PROTO_EXTRA

	CharacterMachine->CalculateAll();
}

void ReceiveLife(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_LIFE Data = (LPPRECEIVE_LIFE)ReceiveBuffer;
	switch (Data->Index)
	{
	case 0xff:
		CharacterAttribute->Life = (CharacterAttribute->Life == 0) ? CharacterAttribute->Life : Data->btLife;
		CharacterAttribute->Shield = Data->btShield;
		break;
	case 0xfe:
		CharacterAttribute->LifeMax = Data->btLife;
		CharacterAttribute->ShieldMax = Data->btShield;
		break;
	case 0xfd:
		EnableUse = 0;
		break;
	default:
	{
		int itemindex = Data->Index - MAX_EQUIPMENT_INDEX;
		ITEM* pItem = g_pMyInventory->FindItem(itemindex);
		if (pItem)
		{
			if (pItem->Durability > 0)
				pItem->Durability--;
			if (pItem->Durability <= 0)
				g_pMyInventory->DeleteItem(itemindex);
		}
	}
	break;
	}
}

void ReceiveMana(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MANA Data = (LPPRECEIVE_MANA)ReceiveBuffer;
	switch (Data->Index)
	{
	case 0xff:
		CharacterAttribute->Mana = Data->btLife;
		CharacterAttribute->SkillMana = Data->btShield;
		break;
	case 0xfe:
		CharacterAttribute->ManaMax = Data->btLife;
		CharacterAttribute->SkillManaMax = Data->btShield;
		break;
	default:
		CharacterAttribute->Mana = Data->btLife;
		/*if (Inventory[Data->Index - 12].Durability > 0)
			Inventory[Data->Index - 12].Durability--;
		if (Inventory[Data->Index - 12].Durability <= 0)
		{
			Inventory[Data->Index - 12].Type = -1;
			Inventory[Data->Index - 12].Number = 0;
		}*/
		break;
	}
}

void ReceivePK(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PK Data = (LPPRECEIVE_PK)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(Key));

	if (pCharacter)
	{
		pCharacter->PK = Data->PK;

#ifdef PK_ATTACK_TESTSERVER_LOG
		PrintPKLog(pCharacter);
#endif // PK_ATTACK_TESTSERVER_LOG

		if (pCharacter->PK >= PVP_MURDERER2)
			pCharacter->Level = 1;
		else
			pCharacter->Level = 0;

		char szTemp[100];
		switch (Data->PK)
		{
		case PVP_HERO2:
		case PVP_HERO1:
			g_pChatListBox->AddText(pCharacter->ID, GlobalText[487], SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case PVP_NEUTRAL:
			g_pChatListBox->AddText(pCharacter->ID, GlobalText[488], SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case PVP_CAUTION:
			g_pChatListBox->AddText(pCharacter->ID, GlobalText[489], SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case PVP_MURDERER1:
			sprintf(szTemp, "%s %d%s", GlobalText[490], 1, GlobalText[491]);
			g_pChatListBox->AddText(pCharacter->ID, szTemp, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case PVP_MURDERER2:
			sprintf(szTemp, "%s %d%s", GlobalText[490], 2, GlobalText[491]);
			g_pChatListBox->AddText(pCharacter->ID, szTemp, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		}

		if (pCharacter == Hero)
		{
			if (Data->PenaltyTime != 0xFFFFFFFF)
			{
				CharacterAttribute->PenaltyTimePK = Data->PenaltyTime;
			}
			else
			{
				CharacterAttribute->PenaltyTimePK = 0;
			}
		}
	}
}

void ReceiveDurability(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_VALUE_KEY Data = (LPPHEADER_DEFAULT_VALUE_KEY)ReceiveBuffer;

	if (CharacterMachine->CheckEquipment(Data->Value))
	{
		CharacterMachine->GetOnlyEquipment(Data->Value)->Durability = Data->KeyH;
	}
	else
	{
		int iLinealPos = Data->Value - MAX_EQUIPMENT_INDEX;

		CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

		if (pPickedItem)
		{
			int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

			if (InvenIndex != -1)
			{
				ITEM* pPickItem = pPickedItem->GetItem();

				if (pPickItem)
				{
					int iSourceIndex = pPickedItem->GetSourceLinealPos();

					iSourceIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(iSourceIndex, InvenIndex);

					if (iSourceIndex == iLinealPos)
					{
						CUSTOM_ITEM_STACK* StackItem = GMProtect->GetItemStack(pPickItem->Type, ((pPickItem->Level >> 3) & 15));

						if (StackItem != NULL)
						{
							pPickItem->Durability = Data->KeyH;

							CNewUIInventoryCtrl::BackupPickedItem();
						}
					}
				}
			}
		}


		ITEM* pItem = g_pMyInventory->FindItem(iLinealPos);

		if (pItem)
		{
			pItem->Durability = Data->KeyH;
		}
	}

	if (Data->KeyL)
	{
		EnableUse = 0;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x2A [ReceiveDurability(%d %d)]", Data->Value, Data->KeyL);
}

BOOL ReceiveHelperItem(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPRECEIVE_HELPER_ITEM Data = (LPPRECEIVE_HELPER_ITEM)ReceiveBuffer;
	CharacterAttribute->AbilityTime[Data->Index] = Data->Time * 24;
	switch (Data->Index)
	{
	case 0:
		CharacterAttribute->Ability |= ABILITY_FAST_ATTACK_SPEED;
		CharacterMachine->CalculateAttackSpeed();
		break;
	case 1:
		CharacterAttribute->Ability |= ABILITY_PLUS_DAMAGE;
		CharacterMachine->CalculateDamage();
		CharacterMachine->CalculateMagicDamage();
		break;
	case 2:
		CharacterAttribute->Ability |= ABILITY_FAST_ATTACK_SPEED2;
		CharacterMachine->CalculateAttackSpeed();
		break;
	}
	EnableUse = 0;

	return (TRUE);
}

void ReceiveWeather(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	int Weather = (Data->Value >> 4);

	if (Weather == 0)
		RainTarget = 0;
	else if (Weather == 1)
		RainTarget = (Data->Value & 15) * 6;
}

void ReceiveEvent(BYTE* ReceiveBuffer)
{
	LPPHEADER_EVENT Data = (LPPHEADER_EVENT)ReceiveBuffer;

	if (SceneFlag == MAIN_SCENE)
	{
		switch (Data->m_byNumber)
		{
		case 1:
			if (Data->m_byValue)
				EnableEvent = 1;
			else
				EnableEvent = 0;
			break;
		case 3:
			if (Data->m_byValue)
				EnableEvent = 3;
			else
				EnableEvent = 0;
			break;
		}
	}
	else
	{
		EnableEvent = 0;
	}
	DeleteBoids();
}

void ReceiveSummonLife(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
	SummonLife = Data->Value;
}

BOOL ReceiveTrade(BYTE* ReceiveBuffer, BOOL bEncrypted)
{
	LPPCHATING Data = (LPPCHATING)ReceiveBuffer;
	g_pTrade->ProcessToReceiveTradeRequest(Data->ID);

	return (TRUE);
}

void ReceiveTradeResult(BYTE* ReceiveBuffer)
{
	LPPTRADE Data = (LPPTRADE)ReceiveBuffer;
	g_pTrade->ProcessToReceiveTradeResult(Data);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x37 [ReceiveTradeResult(%d)]", Data->ID);
}

void ReceiveTradeYourInventoryDelete(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	g_pTrade->ProcessToReceiveYourItemDelete(Data->Value);
}

void ReceiveTradeYourInventory(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_ITEM Data = (LPPHEADER_DEFAULT_ITEM)ReceiveBuffer;
	g_pTrade->ProcessToReceiveYourItemAdd(Data->Index, Data->Item);
}

void ReceiveTradeMyGold(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	g_pTrade->ProcessToReceiveMyTradeGold(Data->Value);
}

void ReceiveTradeYourGold(BYTE* ReceiveBuffer)
{
	LPPHEADER_TRADE_MONEY Data = (LPPHEADER_TRADE_MONEY)ReceiveBuffer;
	g_pTrade->SetYourTradeGold(int(Data->nGold), int(Data->nCoin1), int(Data->nCoin2), int(Data->nCoin3));
}

void ReceiveTradeYourResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	g_pTrade->ProcessToReceiveYourConfirm(Data->Value);
}

void ReceiveTradeExit(BYTE* ReceiveBuffer)
{
	if (EquipmentItem)
	{
		if (!g_bPacketAfter_EquipmentItem)
		{
			memcpy(g_byPacketAfter_EquipmentItem, ReceiveBuffer, sizeof(PHEADER_DEFAULT));
			g_bPacketAfter_EquipmentItem = TRUE;
		}
		return;
	}

	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	g_pTrade->ProcessToReceiveTradeExit(Data->Value);
}

void ReceivePing(BYTE* ReceiveBuffer)
{
	gsteady_clock->runtime_recv_ping(*(int*)(ReceiveBuffer + 4));
}

void ReceiveStorageGold(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_STORAGE_GOLD Data = (LPPRECEIVE_STORAGE_GOLD)ReceiveBuffer;
	if (Data->Result)
	{
		CharacterMachine->StorageGold = Data->StorageGold;
		CharacterMachine->Gold = Data->Gold;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x81 [ReceiveStorageGold(%d %d %d)]", Data->Result, Data->StorageGold, Data->Gold);
}

void ReceiveStorageExit(BYTE* ReceiveBuffer)
{
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x82 [ReceiveStorageExit]");
}

void ReceiveStorageStatus(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;

	g_pStorageInventory->ProcessToReceiveStorageStatus(Data->Value);
}

void ReceiveParty(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_KEY Data = (LPPHEADER_DEFAULT_KEY)ReceiveBuffer;
	PartyKey = ((int)(Data->KeyH) << 8) + Data->KeyL;

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPartyMsgBoxLayout));
}

void ReceivePartyResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[497], SEASON3B::TYPE_ERROR_MESSAGE); //Không thể Tạo nhóm
		break;
	case 1:
		g_pChatListBox->AddText("", GlobalText[498], SEASON3B::TYPE_ERROR_MESSAGE); //Yêu cầu Tạo nhóm bị từ chối
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[499], SEASON3B::TYPE_ERROR_MESSAGE); //Thành viên đã rời khỏi Nhóm
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[500], SEASON3B::TYPE_ERROR_MESSAGE); //"Đối phương đã thoát khỏi trò chơi
		break;
	case 4:
		g_pChatListBox->AddText("", GlobalText[501], SEASON3B::TYPE_ERROR_MESSAGE); //Đối phương đã gia nhập Nhóm khác
		break;
	case 5:
		g_pChatListBox->AddText("", GlobalText[502], SEASON3B::TYPE_ERROR_MESSAGE); //Giải tán Nhóm
		break;
	case 6:
		g_pChatListBox->AddText("", GlobalText[2990], SEASON3B::TYPE_ERROR_MESSAGE); //Không thể tạo nhóm với quân Genus của đối thủ
		break;
	case 7:
		g_pChatListBox->AddText("", GlobalText[2997], SEASON3B::TYPE_ERROR_MESSAGE); //Trong khu vực giao đấu không thể giải tán Nhóm
		break;
	case 8:
		g_pChatListBox->AddText("", GlobalText[2998], SEASON3B::TYPE_ERROR_MESSAGE); //Trong khu vực giao đấu, Nhóm không được kích hoạt
		break;
	}
}

void ReceivePartyList(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PARTY_LISTS Data = (LPPRECEIVE_PARTY_LISTS)ReceiveBuffer;
	int Offset = sizeof(PRECEIVE_PARTY_LISTS);
	PartyNumber = Data->Count;
	for (int i = 0; i < Data->Count; i++)
	{
		LPPRECEIVE_PARTY_LIST Data2 = (LPPRECEIVE_PARTY_LIST)(ReceiveBuffer + Offset);
		PARTY_t* p = &Party[i];
		memcpy(p->Name, Data2->ID, MAX_ID_SIZE);
		p->Name[MAX_ID_SIZE] = NULL;
		p->Number = Data2->Number;
		p->Map = Data2->Map;
		p->x = Data2->x;
		p->y = Data2->y;
		p->currHP = Data2->currHP;
		p->maxHP = Data2->maxHP;
		Offset += sizeof(PRECEIVE_PARTY_LIST);
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x42 [ReceivePartyList(partynum : %d)]", Data->Count);
}

void ReceivePartyInfo(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PARTY_INFOS Data = (LPPRECEIVE_PARTY_INFOS)ReceiveBuffer;
	int Offset = sizeof(PRECEIVE_PARTY_INFOS);
	for (int i = 0; i < Data->Count; i++)
	{
		LPPRECEIVE_PARTY_INFO Data2 = (LPPRECEIVE_PARTY_INFO)(ReceiveBuffer + Offset);
		char stepHP = Data2->value & 0xf;

		PARTY_t* p = &Party[i];

		p->stepHP = min(10, max(0, stepHP));

		Offset += sizeof(PRECEIVE_PARTY_INFO);
	}
}

void ReceivePartyLeave(BYTE* ReceiveBuffer)
{
	PartyNumber = 0;
	g_pChatListBox->AddText("", GlobalText[502], SEASON3B::TYPE_ERROR_MESSAGE); //Giải tán Nhóm

	if (g_iFollowCharacter >= 0)
	{
		bool IsParty = false;
		CHARACTER* c = gmCharacters->GetCharacter(g_iFollowCharacter);

		if (c)
		{
			for (int i = 0; i < PartyNumber; ++i)
			{
				if (strcmp(Party[i].Name, c->ID) == NULL && strlen(Party[i].Name) == strlen(c->ID))
				{
					IsParty = true;
				}
			}
		}

		if (IsParty == false)
		{
			g_iFollowCharacter = -1;
		}
	}
}

void ReceivePartyGetItem(BYTE* ReceiveBuffer) // tin nhắn nhặt item khi pt
{
	LPPRECEIVE_GETITEMINFO_FOR_PARTY Data = (LPPRECEIVE_GETITEMINFO_FOR_PARTY)ReceiveBuffer;
	int Key = ((int)(Data->KeyH) << 8) + Data->KeyL;
	int Index = FindCharacterIndex(Key);
	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c && Hero != c)
	{
		int itemType = Data->ItemInfo & 0x01fff;
		char itemName[100] = { 0, };
		char Text[100] = { 0, };

		if ((Data->ItemInfo & 0x10000))
			sprintf(itemName, "%s ", GlobalText[620]); //Đồ hoàn hảo
		else if ((Data->ItemInfo & 0x20000))
			sprintf(itemName, "%s ", GlobalText[1089]); //Set Item

		int itemLevel = Data->ItemLevel;
		GetItemName(itemType, itemLevel, Text);
		strcat(itemName, Text);

		if ((Data->ItemInfo & 0x02000))
			strcat(itemName, GlobalText[176]); // + Tuyệt chiêu

		if ((Data->ItemInfo & 0x08000))
			strcat(itemName, GlobalText[177]); // + Tính năng

		if ((Data->ItemInfo & 0x04000))
			strcat(itemName, GlobalText[178]); // + May mắn

		sprintf(Text, "%s %s", itemName, GlobalText[918]); //đã được nhặt

		g_pChatListBox->AddText(c->ID, Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

extern int ErrorMessage;

void ReceiveGuild(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_KEY Data = (LPPHEADER_DEFAULT_KEY)ReceiveBuffer;
	GuildPlayerKey = ((int)(Data->KeyH) << 8) + Data->KeyL;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(FindCharacterIndex(GuildPlayerKey));

	if (pCharacter)
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGuildRequestMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(pCharacter->ID);
		pMsgBox->AddMsg(GlobalText[429]); //Xin gia nhập vào Hội.
	}
}

void ReceiveGuildResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[503], SEASON3B::TYPE_ERROR_MESSAGE); //Chủ Hội đã từ chối yêu cầu xin gia nhập của bạn!
		break;
	case 1:
		g_pChatListBox->AddText("", GlobalText[504], SEASON3B::TYPE_ERROR_MESSAGE); //Chúc mừng bạn đã gia nhập Hội
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[505], SEASON3B::TYPE_ERROR_MESSAGE); //Hội đã vượt quá số lượng thành viên cho phép!
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[506], SEASON3B::TYPE_ERROR_MESSAGE); //Đã thoát khỏi trò chơi!
		break;
	case 4:
		g_pChatListBox->AddText("", GlobalText[507], SEASON3B::TYPE_ERROR_MESSAGE); //Người này không phải là Chủ Hội!
		break;
	case 5:
		g_pChatListBox->AddText("", GlobalText[508], SEASON3B::TYPE_ERROR_MESSAGE); //Không thể gia nhập Hội vào lúc này!
		break;
	case 6:
		g_pChatListBox->AddText("", GlobalText[509], SEASON3B::TYPE_ERROR_MESSAGE); //Không thể gửi yêu cầu gia nhập!
		break;
	case 7:
		g_pChatListBox->AddText("", GlobalText[510], SEASON3B::TYPE_ERROR_MESSAGE); //Từ cấp độ 6 trở lên mới có thể gia nhập Hội
		break;
	case 0xA1:
		g_pChatListBox->AddText("", GlobalText[2992], SEASON3B::TYPE_ERROR_MESSAGE); //Chủ Hội không được gia nhập vào Genus
		break;
	case 0xA2:
		g_pChatListBox->AddText("", GlobalText[2995], SEASON3B::TYPE_ERROR_MESSAGE); //Guild Master có thể gia nhập vào Genus khác.
		break;
	case 0xA3:
		g_pChatListBox->AddText("", GlobalText[2996], SEASON3B::TYPE_ERROR_MESSAGE); //Phải gia nhập vào Genus cùng với chủ Hội thì mới có thể đăng ký gia nhập Hội
		break;
	}
}

void ReceiveGuildList(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GUILD_LISTS Data = (LPPRECEIVE_GUILD_LISTS)ReceiveBuffer;
	int Offset = sizeof(PRECEIVE_GUILD_LISTS);

	g_nGuildMemberCount = Data->Count;
	GuildTotalScore = Data->TotalScore;
	GuildTotalScore = max(0, GuildTotalScore);

	g_pGuildInfoWindow->GuildClear();
	g_pGuildInfoWindow->UnionGuildClear();
	g_pGuildInfoWindow->SetRivalGuildName(Data->szRivalGuildName);

	for (int i = 0; i < Data->Count; i++)
	{
		LPPRECEIVE_GUILD_LIST Data2 = (LPPRECEIVE_GUILD_LIST)(ReceiveBuffer + Offset);
		GUILD_LIST_t* p = &GuildList[i];
		memcpy(p->Name, Data2->ID, MAX_ID_SIZE);
		p->Name[MAX_ID_SIZE] = NULL;
		p->Number = Data2->Number;

		if (Data2->CurrentServer != 0xff)
			p->Server = Data2->CurrentServer & 0x7F;
		else
			p->Server = 0xff;

		if (p->Server != 0xff)
		{
			p->Server %= 20;
		}

		p->GuildStatus = Data2->GuildStatus;

		g_pGuildInfoWindow->AddGuildMember(p);

		Offset += sizeof(PRECEIVE_GUILD_LIST);
	}
}

void ReceiveGuildLeave(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:g_pChatListBox->AddText("", GlobalText[511], SEASON3B::TYPE_ERROR_MESSAGE); break; //Thông tin ID không đúng
	case 1:g_pChatListBox->AddText("", GlobalText[512], SEASON3B::TYPE_ERROR_MESSAGE); break; //Đã rời khỏi Hội
	case 2:g_pChatListBox->AddText("", GlobalText[513], SEASON3B::TYPE_ERROR_MESSAGE); break; //Chỉ có Chủ Hội mới có thể giải tán Hội
	case 3:g_pChatListBox->AddText("", GlobalText[514], SEASON3B::TYPE_ERROR_MESSAGE); break; //Không thể rời khỏi Hội
	case 4:g_pChatListBox->AddText("", GlobalText[515], SEASON3B::TYPE_ERROR_MESSAGE); break; //Hội đã bị giải tán
	case 5:g_pChatListBox->AddText("", GlobalText[568], SEASON3B::TYPE_ERROR_MESSAGE); break; //"Bạn đã tham gia Hội
	}
	if (Data->Value == 1 || Data->Value == 4)
	{
		if (Data->Value == 4 && Hero->GuildMarkIndex != -1)
		{
			GuildMark[Hero->GuildMarkIndex].Key = -1;
			GuildMark[Hero->GuildMarkIndex].UnionName[0] = NULL;
			GuildMark[Hero->GuildMarkIndex].GuildName[0] = NULL;
			Hero->GuildMarkIndex = -1;
			//Hero->DelGuildName();
		}

		EnableGuildWar = false;

		g_nGuildMemberCount = -1;
		Hero->GuildStatus = G_NONE;
		Hero->GuildRelationShip = GR_NONE;
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_GUILDINFO);

#ifdef CSK_MOD_MOVE_COMMAND_WINDOW
		g_pMoveCommandWindow->SetCastleOwner(false);
#endif // CSK_MOD_MOVE_COMMAND_WINDOW
	}
	else if (Data->Value == 5)
	{
		SendRequestGuildList();
	}
}

void ReceiveCreateGuildInterface(BYTE* ReceiveBuffer)
{
	g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCGUILDMASTER);
}

void ReceiveCreateGuildMasterInterface(BYTE* ReceiveBuffer)
{
}

void ReceiveDeleteGuildViewport(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_KEY Data = (LPPHEADER_DEFAULT_KEY)ReceiveBuffer;
	int Key = ((int)(Data->KeyH & 0x7f) << 8) + Data->KeyL;
	int Index = FindCharacterIndex(Key);
	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c)
	{
		c->GuildStatus = -1;
		c->GuildType = 0;
		c->ProtectGuildMarkWorldTime = 0.0f;
		c->GuildRelationShip = 0;
		c->GuildMarkIndex = -1;

		//c->DelGuildName();

		g_nGuildMemberCount = -1;

		g_pNewUISystem->Hide(SEASON3B::INTERFACE_GUILDINFO);
	}
}

void ReceiveCreateGuildResult(BYTE* ReceiveBuffer)
{
	LPPMSG_GUILD_CREATE_RESULT Data = (LPPMSG_GUILD_CREATE_RESULT)ReceiveBuffer;

	switch (Data->Value)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[516], SEASON3B::TYPE_ERROR_MESSAGE); //Tên này đã được sử dụng
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[517], SEASON3B::TYPE_ERROR_MESSAGE); //Tên Hội ít nhất phải có 4 ký tự
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[518], SEASON3B::TYPE_ERROR_MESSAGE); //Đối phương đang là thành viên của Hội khác
		break;
	case 4:
		g_pChatListBox->AddText("", GlobalText[940], SEASON3B::TYPE_ERROR_MESSAGE); //Bạn không phải là thành viên của Hội
		break;
	case 5:
		g_pChatListBox->AddText("", GlobalText[941], SEASON3B::TYPE_ERROR_MESSAGE); //Tên Guild không được có khoảng trống
		break;
	case 6:
		g_pChatListBox->AddText("", GlobalText[942], SEASON3B::TYPE_ERROR_MESSAGE); //Không được bỏ trống
		break;
	case 1:
		memset(InputText[0], 0, MAX_ID_SIZE);
		InputLength[0] = 0;
		InputTextMax[0] = MAX_ID_SIZE;
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_NPCGUILDMASTER);
		MouseUpdateTime = 0;
		MouseUpdateTimeMax = 6;
		break;
	}
}

void ReceiveDeclareWar(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_WAR Data = (LPPRECEIVE_WAR)ReceiveBuffer;
	memset(GuildWarName, 0, 8);
	memcpy(GuildWarName, Data->Name, 8);
	GuildWarName[8] = NULL;

	if (Data->Type == 1)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBattleSoccerMsgBoxLayout));
	}
	else
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGuildWarMsgBoxLayout));
	}
}

void ReceiveDeclareWarResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:g_pChatListBox->AddText("", GlobalText[519], SEASON3B::TYPE_ERROR_MESSAGE); break; //Bạn không phải là thành viên của Hội
	case 1:g_pChatListBox->AddText("", GlobalText[520], SEASON3B::TYPE_ERROR_MESSAGE); break; //Tuyên chiến với Hội
	case 2:g_pChatListBox->AddText("", GlobalText[521], SEASON3B::TYPE_ERROR_MESSAGE); break; //Chủ Hội đã thoát khỏi trò chơi
	case 3:g_pChatListBox->AddText("", GlobalText[522], SEASON3B::TYPE_ERROR_MESSAGE); break; //Hội này không tồn tại
	case 4:g_pChatListBox->AddText("", GlobalText[523], SEASON3B::TYPE_ERROR_MESSAGE); break; //Không được tuyên chiến
	case 5:g_pChatListBox->AddText("", GlobalText[524], SEASON3B::TYPE_ERROR_MESSAGE); break; //Chỉ có chủ Hội mới có thể tuyên chiến
	case 6:g_pChatListBox->AddText("", GlobalText[525], SEASON3B::TYPE_ERROR_MESSAGE); break; //Lời khiêu chiến bị từ chối
	}
	if (Data->Value != 1 && !EnableGuildWar)
	{
		InitGuildWar();
	}
}

void ReceiveGuildBeginWar(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_WAR Data = (LPPRECEIVE_WAR)ReceiveBuffer;
	EnableGuildWar = true;

	char Text[100];
	memset(GuildWarName, 0, 8);
	memcpy(GuildWarName, Data->Name, 8);
	GuildWarName[8] = NULL;

	if (Data->Type == 0)
	{
		sprintf(Text, GlobalText[526], GuildWarName); //%s cuộc chiến bắt đầu
	}
	else
	{
		sprintf(Text, GlobalText[533], GuildWarName); //Thất bại!!!
		EnableSoccer = true;
	}

	CreateNotice(Text, 1);
	HeroSoccerTeam = Data->Team;

	for (int i = 0; i < MARK_EDIT; i++)
	{
		MARK_t* p = &GuildMark[i];
		char Temp[8 + 1];
		memset(Temp, 0, 8);
		memcpy(Temp, (char*)Data->Name, 8);
		Temp[8] = NULL;
		if (strcmp(p->GuildName, Temp) == NULL)
		{
			GuildWarIndex = i;
			break;
		}
	}
	if (GuildWarIndex >= 0)
	{
		for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
		{
			CHARACTER* c = gmCharacters->GetCharacter(i);
			GuildTeam(c);
		}
	}
	SetActionClass(Hero, &Hero->Object, PLAYER_RUSH1, AT_RUSH1);
	SendRequestAction(AT_RUSH1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));

	g_pNewUISystem->Show(SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x62 [ReceiveGuildBeginWar(%d)]", Data->Team);
}

void ReceiveGuildEndWar(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	char Text[100];
	int Win = 0;
	switch (Data->Value)
	{
	case 0:
		sprintf(Text, GlobalText[527]); //Thất bại!!!
		break;
	case 1:
		sprintf(Text, GlobalText[528]); Win = 2;
		break;
	case 2:
		sprintf(Text, GlobalText[529]); Win = 2;
		break;
	case 3:
		sprintf(Text, GlobalText[530]);
		break;
	case 4:
		sprintf(Text, GlobalText[531]); Win = 2;
		break;
	case 5:
		sprintf(Text, GlobalText[532]);
		break;
	case 6:
		sprintf(Text, GlobalText[480]); Win = 1;
		break;
	default:
		sprintf(Text, "");
		break;
	}

	g_wtMatchTimeLeft.m_Time = 0;

#ifndef GUILD_WAR_EVENT
	CreateNotice(Text, 1);
#endif

	EnableGuildWar = false;
	EnableSoccer = false;

	GuildWarIndex = -1;
	GuildWarName[0] = NULL;
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		GuildTeam(c);
	}
	switch (Win)
	{
	case 2:
		SetActionClass(Hero, &Hero->Object, PLAYER_WIN1, AT_WIN1);
		SendRequestAction(AT_WIN1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
		break;
	case 0:
		SetActionClass(Hero, &Hero->Object, PLAYER_CRY1, AT_CRY1);
		SendRequestAction(AT_CRY1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
		break;
	}

	g_pNewUISystem->Hide(SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE);
}

void ReceiveGuildWarScore(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_WAR_SCORE Data = (LPPRECEIVE_WAR_SCORE)ReceiveBuffer;
	EnableGuildWar = true;

#ifdef GUILD_WAR_EVENT
	int t1 = Data->Score1 - GuildWarScore[0];
	int t2 = Data->Score2 - GuildWarScore[1];
	if (t1 == 20 || t2 == 20)
	{
		if (t1 > t2)
			g_iGoalEffect = 1;
		else
			g_iGoalEffect = 2;
	}
#endif
	GuildWarScore[0] = Data->Score1;
	GuildWarScore[1] = Data->Score2;
}

void ReceiveGuildIDViewport(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);
	for (int i = 0; i < Data->Value; ++i)
	{
		LPPRECEIVE_GUILD_ID Data2 = (LPPRECEIVE_GUILD_ID)(ReceiveBuffer + Offset);
		int GuildKey = Data2->GuildKey;
		int Key = ((int)(Data2->KeyH & 0x7f) << 8) + Data2->KeyL;
		int Index = FindCharacterIndex(Key);
		CHARACTER* c = gmCharacters->GetCharacter(Index);

		if (c)
		{
			c->GuildStatus = Data2->GuildStatus;
			c->GuildType = Data2->GuildType;
			c->GuildRelationShip = Data2->GuildRelationShip;

#ifdef CSK_MOD_MOVE_COMMAND_WINDOW
			g_pMoveCommandWindow->SetCastleOwner((bool)Data2->btCastleOwner);
#endif // CSK_MOD_MOVE_COMMAND_WINDOW

			if (g_GuildCache.IsExistGuildMark(GuildKey))
			{
				c->GuildMarkIndex = g_GuildCache.GetGuildMarkIndex(GuildKey);
			}
			else
			{
				SendRequestGuildInfo(GuildKey);
				c->GuildMarkIndex = g_GuildCache.MakeGuildMarkIndex(GuildKey);
			}
			GuildTeam(c);

			//c->SetGuildName();
		}

		if (gMapManager->currentMap == WD_30BATTLECASTLE)
		{
			if (g_pSiegeWarfare)
			{
				if (g_pSiegeWarfare->IsCreated() == false)
				{
					g_pSiegeWarfare->InitMiniMapUI();
					g_pSiegeWarfare->SetGuildData(Hero);
					g_pSiegeWarfare->CreateMiniMapUI();
				}
			}
		}

		Offset += sizeof(PRECEIVE_GUILD_ID);
	}
}

void ReceiveGuildInfo(BYTE* ReceiveBuffer)
{
	LPPPRECEIVE_GUILDINFO Data = (LPPPRECEIVE_GUILDINFO)ReceiveBuffer;

	int Index = g_GuildCache.SetGuildMark(Data->GuildKey, Data->UnionName, Data->GuildName, Data->Mark);
}

void ReceiveGuildAssign(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GUILD_ASSIGN pData = (LPPRECEIVE_GUILD_ASSIGN)ReceiveBuffer;

	char szTemp[MAX_GLOBAL_TEXT_STRING] = "Invalid GuildAssign";

	if (pData->byResult == 0x01)
	{
		switch (pData->byType)
		{
		case 0x01:
			strcpy(szTemp, GlobalText[1374]);
			break;
		case 0x02:
			strcpy(szTemp, GlobalText[1375]);
			break;
		case 0x03:
			strcpy(szTemp, GlobalText[1376]);
			break;
		default:
			assert(!"Packet(0xE1)");
			break;
		}
	}
	else
	{
		switch (pData->byResult)
		{
		case GUILD_ANS_NOTEXIST_GUILD:
			strcpy(szTemp, GlobalText[522]);
			break;
		case GUILD_ANS_NOTEXIST_PERMISSION:
			strcpy(szTemp, GlobalText[1386]);
			break;
		case GUILD_ANS_NOTEXIST_EXTRA_STATUS:
			strcpy(szTemp, GlobalText[1326]);
			break;
		case GUILD_ANS_NOTEXIST_EXTRA_TYPE:
			strcpy(szTemp, GlobalText[1327]);
			break;
		default:
			assert(!"Packet(0xE1)");
			break;
		}
	}
	g_pChatListBox->AddText("", szTemp, SEASON3B::TYPE_SYSTEM_MESSAGE);
}

void ReceiveGuildRelationShip(BYTE* ReceiveBuffer)
{
	LPPMSG_GUILD_RELATIONSHIP pData = (LPPMSG_GUILD_RELATIONSHIP)ReceiveBuffer;

	g_pGuildInfoWindow->ReceiveGuildRelationShip(pData->byRelationShipType, pData->byRequestType, pData->byTargetUserIndexH, pData->byTargetUserIndexL);
}

void ReceiveGuildRelationShipResult(BYTE* ReceiveBuffer)
{
	char szTemp[MAX_GLOBAL_TEXT_STRING] = "Invalid GuildRelationShipResult";
	LPPMSG_GUILD_RELATIONSHIP_RESULT pData = (LPPMSG_GUILD_RELATIONSHIP_RESULT)ReceiveBuffer;

	if (pData->byResult == 0x01)
	{
		if (pData->byRelationShipType == 0x01)
		{
			if (pData->byRequestType == 0x01)
			{
				strcpy(szTemp, GlobalText[1381]);
			}
			else if (pData->byRequestType == 0x02)
			{
				strcpy(szTemp, GlobalText[1382]);
			}
			else if (pData->byRequestType == 0x10)
			{
				strcpy(szTemp, GlobalText[1635]);
			}
		}
		else
		{
			if (pData->byRequestType == 0x01)
				strcpy(szTemp, GlobalText[1383]);
			else
				strcpy(szTemp, GlobalText[1384]);
		}
	}
	else if (pData->byResult == 0)
	{
		strcpy(szTemp, GlobalText[1328]);
	}
	else
	{
		switch (pData->byResult)
		{
		case GUILD_ANS_UNIONFAIL_BY_CASTLE:
			strcpy(szTemp, GlobalText[1637]);
			break;
		case GUILD_ANS_NOTEXIST_PERMISSION:
			strcpy(szTemp, GlobalText[1386]);
			break;
		case GUILD_ANS_EXIST_RELATIONSHIP_UNION:
			strcpy(szTemp, GlobalText[1250]);
			break;
		case GUILD_ANS_EXIST_RELATIONSHIP_RIVAL:
			strcpy(szTemp, GlobalText[1251]);
			break;
		case GUILD_ANS_EXIST_UNION:
			strcpy(szTemp, GlobalText[1252]);
			break;
		case GUILD_ANS_EXIST_RIVAL:
			strcpy(szTemp, GlobalText[1253]);
			break;
		case GUILD_ANS_NOTEXIST_UNION:
			strcpy(szTemp, GlobalText[1254]);
			break;
		case GUILD_ANS_NOTEXIST_RIVAL:
			strcpy(szTemp, GlobalText[1255]);
			break;
		case GUILD_ANS_NOT_UNION_MASTER:
			strcpy(szTemp, GlobalText[1333]);
			break;
		case GUILD_ANS_NOT_GUILD_RIVAL:
			strcpy(szTemp, GlobalText[1329]);
			break;
		case GUILD_ANS_CANNOT_BE_UNION_MASTER_GUILD:
			strcpy(szTemp, GlobalText[1331]);
			break;
		case GUILD_ANS_EXCEED_MAX_UNION_MEMBER:
			strcpy(szTemp, GlobalText[1287]);
			break;
		case GUILD_ANS_CANCEL_REQUEST:
			strcpy(szTemp, GlobalText[1268]);
			break;
#ifdef ASG_ADD_GENS_SYSTEM
		case GUILD_ANS_UNION_MASTER_NOT_GENS:
			strcpy(szTemp, GlobalText[2991]);
			break;
		case GUILD_ANS_GUILD_MASTER_NOT_GENS:
			strcpy(szTemp, GlobalText[2992]);
			break;
		case GUILD_ANS_UNION_MASTER_DISAGREE_GENS:
			strcpy(szTemp, GlobalText[2993]);
			break;
#endif	// ASG_ADD_GENS_SYSTEM
		default:
			assert(!"Packet (0xE6)");
			break;
		}
	}
	g_pChatListBox->AddText("", szTemp, SEASON3B::TYPE_SYSTEM_MESSAGE);

	int nCharKey = MAKEWORD(pData->byTargetUserIndexL, pData->byTargetUserIndexH);
	if (nCharKey == HeroKey && pData->byResult == 0x01 && pData->byRelationShipType == 0x01 && pData->byRequestType == 0x02)
		GuildMark[Hero->GuildMarkIndex].UnionName[0] = NULL;
}

void ReceiveBanUnionGuildResult(BYTE* ReceiveBuffer)
{
	LPPMSG_BAN_UNIONGUILD pData = (LPPMSG_BAN_UNIONGUILD)ReceiveBuffer;
	if (pData->byResult == 0x01)
	{
		if (g_pGuildInfoWindow->GetUnionCount() > 2)
		{
			Sleep(500);
			SendRequestUnionList();
		}
		g_pGuildInfoWindow->UnionGuildClear();
	}
	else if (pData->byResult == 0)
	{
		g_pChatListBox->AddText("", GlobalText[1328], SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

void ReceiveUnionViewportNotify(BYTE* ReceiveBuffer)
{
	LPPMSG_UNION_VIEWPORT_NOTIFY_COUNT pData = (LPPMSG_UNION_VIEWPORT_NOTIFY_COUNT)ReceiveBuffer;
	int Offset = sizeof(PMSG_UNION_VIEWPORT_NOTIFY_COUNT);
	for (int i = 0; i < pData->byCount; ++i)
	{
		LPPMSG_UNION_VIEWPORT_NOTIFY pData2 = (LPPMSG_UNION_VIEWPORT_NOTIFY)(ReceiveBuffer + Offset);

		int nGuildMarkIndex = g_GuildCache.GetGuildMarkIndex(pData2->nGuildKey);
		memcpy(GuildMark[nGuildMarkIndex].UnionName, pData2->szUnionName, sizeof(char) * MAX_GUILDNAME);
		GuildMark[nGuildMarkIndex].UnionName[MAX_GUILDNAME] = NULL;

		int nCharKey = MAKEWORD(pData2->byKeyL, pData2->byKeyH);

		int nCharIndex = FindCharacterIndex(nCharKey);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(nCharIndex);

		if (pCharacter)
		{
			pCharacter->GuildRelationShip = pData2->byGuildRelationShip;
		}

		Offset += sizeof(PMSG_UNION_VIEWPORT_NOTIFY);
	}

	/*if (pData->byCount > 0)
	{
		for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
		{
			CHARACTER* c = gmCharacters->GetCharacter(i);

			if (c->Object.Live && c->Object.Kind == KIND_PLAYER)
			{
				c->SetGuildName();
			}
		}
	}*/
}

void ReceiveUnionList(BYTE* ReceiveBuffer)
{
	LPPMSG_UNIONLIST_COUNT pData = (LPPMSG_UNIONLIST_COUNT)ReceiveBuffer;
	g_pGuildInfoWindow->UnionGuildClear();
	if (pData->byResult == 1)
	{
		int Offset = sizeof(PMSG_UNIONLIST_COUNT);
		for (int i = 0; i < pData->byCount; ++i)
		{
			LPPMSG_UNIONLIST pData2 = (LPPMSG_UNIONLIST)(ReceiveBuffer + Offset);

			BYTE tmp[64];
			for (int j = 0; j < 64; ++j)
			{
				if (j % 2 == 0)
					tmp[j] = (pData2->GuildMark[j / 2] >> 4) & 0x0f;
				else
					tmp[j] = pData2->GuildMark[j / 2] & 0x0f;
			}

			g_pGuildInfoWindow->AddUnionList(tmp, pData2->szGuildName, pData2->byMemberCount);

			Offset += sizeof(PMSG_UNIONLIST);
		}
	}
}

void ReceiveSoccerTime(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SOCCER_TIME Data = (LPPRECEIVE_SOCCER_TIME)ReceiveBuffer;
	SoccerTime = Data->Time;
}

void ReceiveSoccerScore(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SOCCER_SCORE Data = (LPPRECEIVE_SOCCER_SCORE)ReceiveBuffer;
	memcpy(SoccerTeamName[0], Data->Name1, 8);
	memcpy(SoccerTeamName[1], Data->Name2, 8);
	SoccerTeamName[0][8] = NULL;
	SoccerTeamName[1][8] = NULL;
	GuildWarScore[0] = Data->Score1;
	GuildWarScore[1] = Data->Score2;

	if (GuildWarScore[0] != 255)
	{
		SoccerObserver = true;
		g_pNewUISystem->Show(SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE);
	}
	else
	{
		SoccerObserver = false;
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE);
	}

	vec3_t Position, Angle, Light;
	Position[0] = (float)(Hero->PositionX + 0.5f) * TERRAIN_SCALE;
	Position[1] = (float)(Hero->PositionY + 0.5f) * TERRAIN_SCALE;
	Position[2] = RequestTerrainHeight(Position[0], Position[1]);
	ZeroMemory(Angle, sizeof(Angle));
	Light[0] = Light[1] = Light[2] = 1.0f;
	CreateEffect(BITMAP_FIRECRACKERRISE, Position, Angle, Light);
}

void ReceiveSoccerGoal(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	char Text[100];
	if (Data->Value == HeroSoccerTeam)
		sprintf(Text, GlobalText[534], GuildMark[Hero->GuildMarkIndex].GuildName);
	else
		sprintf(Text, GlobalText[534], GuildWarName);
	g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
}

void Receive_Master_Level_Exp(BYTE* ReceiveBuffer)
{
	LPPMSG_MASTERLEVEL_INFO Data = (LPPMSG_MASTERLEVEL_INFO)ReceiveBuffer;
	Master_Level_Data.nMLevel = Data->nMLevel;
	Master_Level_Data.lMasterLevel_Experince = 0x0000000000000000;
	Master_Level_Data.lNext_MasterLevel_Experince = 0x0000000000000000;

	Master_Level_Data.lMasterLevel_Experince = ConvertByteToLong(Data->btMExp);
	Master_Level_Data.lNext_MasterLevel_Experince = ConvertByteToLong(Data->btMNextExp);

	Master_Level_Data.nMLevelUpMPoint = Data->nMLPoint;

#ifdef PROTO_EXTRA
	CharacterAttribute->LifeMax = Data->mtMaxLife;
	CharacterAttribute->ManaMax = Data->mtMaxMana;
	CharacterAttribute->ShieldMax = Data->mtMaxShield;
	CharacterAttribute->SkillManaMax = Data->mtMaxSkillMana;
#else
	CharacterAttribute->LifeMax = Data->wMaxLife;
	CharacterAttribute->ManaMax = Data->wMaxMana;
	CharacterAttribute->ShieldMax = Data->wMaxShield;
	CharacterAttribute->SkillManaMax = Data->wMaxSkillMana;
#endif // PROTO_EXTRA
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x50 [Receive_Master_Level_Exp]");
}

void Receive_Master_LevelUp(BYTE* ReceiveBuffer)
{
	LPPMSG_MASTERLEVEL_UP Data = (LPPMSG_MASTERLEVEL_UP)ReceiveBuffer;
	Master_Level_Data.nMLevel = Data->nMLevel;
	Master_Level_Data.nAddMPoint = Data->nAddMPoint;
	Master_Level_Data.nMLevelUpMPoint = Data->nMLevelUpMPoint;
	Master_Level_Data.nMaxPoint = Data->nMaxPoint;

#ifdef PROTO_EXTRA
	CharacterAttribute->LifeMax = Data->mtMaxLife;
	CharacterAttribute->ManaMax = Data->mtMaxMana;
	CharacterAttribute->ShieldMax = Data->mtMaxShield;
	CharacterAttribute->SkillManaMax = Data->mtMaxSkillMana;
#else
	CharacterAttribute->LifeMax = Data->wMaxLife;
	CharacterAttribute->ManaMax = Data->wMaxMana;
	CharacterAttribute->ShieldMax = Data->wMaxShield;
	CharacterAttribute->SkillManaMax = Data->wMaxBP;
#endif // PROTO_EXTRA


	unicode::t_char szText[256] = { NULL, };
	WORD iExp = Master_Level_Data.lNext_MasterLevel_Experince - Master_Level_Data.lMasterLevel_Experince;
	sprintf(szText, GlobalText[1750], iExp);
	g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);

	CharacterMachine->CalulateMasterLevelNextExperience();

	OBJECT* o = &Hero->Object;

	if (gCharacterManager.IsMasterLevel(Hero->Class) == true)
	{
		CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 45, o, 80, 2);
		for (int i = 0; i < 19; ++i)
		{
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 46, o, 80, 2);
		}
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x51 [Receive_Master_LevelUp]");
}

void Receive_Master_LevelGetSkill(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_ANS_MASTERLEVEL_SKILL Data = (LPPMSG_ANS_MASTERLEVEL_SKILL)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 1:
		g_pMasterSkillTree->RegisterSkillInCharacterAttribute(Data->nSkillNum);
		g_pMasterSkillTree->SkillUpgrade(Data->nSkillNum, Data->nSkillLevel, Data->fSkillValue, Data->fSkillNextValue);
		break;
	case 6:
		break;
	case 7:
		break;
	}
	Master_Level_Data.nMLevelUpMPoint = Data->nMLPoint;
#endif // MAIN_UPDATE != 303

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x52 [Receive_Master_LevelGetSkill]");
}

void Receive_Master_LevelSkillTree(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	g_pMasterSkillTree->ClearSkillTreeInfo();
	g_pMasterSkillTree->InitMasterSkillPoint();
	LPPMSG_ANS_MASTERLEVEL_SKILL_LIST_COUNT Data = (LPPMSG_ANS_MASTERLEVEL_SKILL_LIST_COUNT)ReceiveBuffer;
	int lOfs = sizeof(PMSG_ANS_MASTERLEVEL_SKILL_LIST_COUNT);
	for (int i = 0; i < Data->nCount; i++)
	{
		LPPMSG_ANS_MASTERLEVEL_SKILL_LIST pElementMsg = (LPPMSG_ANS_MASTERLEVEL_SKILL_LIST)(ReceiveBuffer + lOfs);
		g_pMasterSkillTree->SetMasterSkillTreeInfo(pElementMsg->byIndex, pElementMsg->byPoint, pElementMsg->fValue, pElementMsg->fNextValue);
		lOfs += sizeof(PMSG_ANS_MASTERLEVEL_SKILL_LIST);
	}
#endif // MAIN_UPDATE != 303

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x53[Receive_Master_LevelSkillTree]");
}

BOOL ReceiveNewJoinMapServer(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_NEW_JOIN_MAP_SERVER Data = (LPPRECEIVE_NEW_JOIN_MAP_SERVER)ReceiveBuffer;

	CharacterAttribute->Level = Data->Level;
	CharacterAttribute->LevelUpPoint = Data->btLevelUpPoint;
	CharacterAttribute->Strength = Data->btStrength;
	CharacterAttribute->Dexterity = Data->btDexterity;
	CharacterAttribute->Vitality = Data->btVitality;
	CharacterAttribute->Energy = Data->btEnergy;
	CharacterAttribute->Charisma = Data->btCharisma;
	CharacterAttribute->Life = Data->btLife;
	CharacterAttribute->LifeMax = Data->btLifeMax;
	CharacterAttribute->Mana = Data->btMana;
	CharacterAttribute->ManaMax = Data->btManaMax;
	CharacterAttribute->SkillMana = Data->btSkillMana;
	CharacterAttribute->SkillManaMax = Data->btSkillManaMax;
	CharacterAttribute->Shield = Data->btShield;
	CharacterAttribute->ShieldMax = Data->btShieldMax;

	CharacterAttribute->wMinusPoint = Data->wMinusPoint;
	CharacterAttribute->wMaxMinusPoint = Data->wMaxMinusPoint;

	if (gCharacterManager.IsMasterLevel(Hero->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		Master_Level_Data.lMasterLevel_Experince = Data->btExp;
		Master_Level_Data.lNext_MasterLevel_Experince = Data->btNextExp;
	}
	else
	{
		CharacterAttribute->Experience = (int)Data->btExp;
		CharacterAttribute->NextExperince = Data->btNextExp;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0xE0 [ReceiveJoinMapServer]");
	return (TRUE);
}

BOOL ReceiveNewCalculate(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_DEFAULT_CHARACTER Data = (LPPRECEIVE_DEFAULT_CHARACTER)ReceiveBuffer;

	CharacterAttribute->Life = Data->Life;
	CharacterAttribute->LifeMax = Data->LifeMax;
	//--
	CharacterAttribute->Mana = Data->Mana;
	CharacterAttribute->ManaMax = Data->ManaMax;
	//--
	CharacterAttribute->SkillMana = Data->SkillMana;
	CharacterAttribute->SkillManaMax = Data->SkillManaMax;
	//--
	CharacterAttribute->Shield = Data->Shield;
	CharacterAttribute->ShieldMax = Data->ShieldMax;
	//--
	CharacterAttribute->AddStrength = Data->AddStrength;
	CharacterAttribute->AddDexterity = Data->AddDexterity;
	CharacterAttribute->AddVitality = Data->AddVitality;
	CharacterAttribute->AddEnergy = Data->AddEnergy;
	CharacterAttribute->AddCharisma = Data->AddCharisma;
	//--
#ifdef SHUTDOWN_CHARACTER_INFO
	CharacterAttribute->Work_PhysiDamageMin = Data->btPhysiDamageMin;
	CharacterAttribute->Work_PhysiDamageMax = Data->btPhysiDamageMax;
	CharacterAttribute->Work_MagicDamageMin = Data->btMagicDamageMin;
	CharacterAttribute->Work_MagicDamageMax = Data->btMagicDamageMax;
	CharacterAttribute->Work_CurseDamageMin = Data->btCurseDamageMin;
	CharacterAttribute->Work_CurseDamageMax = Data->btCurseDamageMax;
	CharacterAttribute->Work_MulPhysiDamage = Data->btMulPhysiDamage;
	CharacterAttribute->Work_DivPhysiDamage = Data->btDivPhysiDamage;
	CharacterAttribute->Work_MulMagicDamage = Data->btMulMagicDamage;
	CharacterAttribute->Work_DivMagicDamage = Data->btDivMagicDamage;
	CharacterAttribute->Work_MulCurseDamage = Data->btMulCurseDamage;
	CharacterAttribute->Work_DivCurseDamage = Data->btDivCurseDamage;
	CharacterAttribute->Work_MagicDamageRate = Data->btMagicDamageRate;
	CharacterAttribute->Work_CurseDamageRate = Data->btCurseDamageRate;
	CharacterAttribute->Work_PhysiSpeed = Data->btPhysiSpeed;
	CharacterAttribute->Work_MagicSpeed = Data->btMagicSpeed;
	CharacterAttribute->Work_AttackSuccessRate = Data->btAttackSuccessRate;
	CharacterAttribute->Work_AttackSuccessRatePvP = Data->btAttackSuccessRatePvP;
	CharacterAttribute->Work_Defense = Data->btDefense;
	CharacterAttribute->Work_DefenseSuccessRate = Data->btDefenseSuccessRate;
	CharacterAttribute->Work_DefenseSuccessRatePvP = Data->btDefenseSuccessRatePvP;
	CharacterAttribute->Work_DamageMultiplier = Data->btDamageMultiplier;
	CharacterAttribute->Work_RFDamageMultiplierA = Data->btRFDamageMultiplierA;
	CharacterAttribute->Work_RFDamageMultiplierB = Data->btRFDamageMultiplierB;
	CharacterAttribute->Work_RFDamageMultiplierC = Data->btRFDamageMultiplierC;
	CharacterAttribute->Work_DarkSpiritAttackDamageMin = Data->btDarkSpiritAttackDamageMin;
	CharacterAttribute->Work_DarkSpiritAttackDamageMax = Data->btDarkSpiritAttackDamageMax;
	CharacterAttribute->Work_DarkSpiritAttackSpeed = Data->btDarkSpiritAttackSpeed;
	CharacterAttribute->Work_DarkSpiritAttackSuccessRate = Data->btDarkSpiritAttackSuccessRate;
#endif // SHUTDOWN_CHARACTER_INFO

	g_ConsoleDebug->Write(MCD_RECEIVE, "0xE1 [ReceiveCalCharacter]");
	return (TRUE);
}

void ReceiveCharacterCoin(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CHARACTER_COIN Data = (LPPRECEIVE_CHARACTER_COIN)ReceiveBuffer;

	CharacterMachine->Coin1 = Data->Coin[0];
	CharacterMachine->Coin2 = Data->Coin[1];
	CharacterMachine->Coin3 = Data->Coin[2];
}

void GCBuyConfirmRecv(BYTE* ReceiveBuffer) // OK
{
	LPPRECEIVE_NPCSHOP_ITEM_BUY Data = (LPPRECEIVE_NPCSHOP_ITEM_BUY)ReceiveBuffer;

	g_pNPCShop->SetStandbyBuyItemKey(Data->btSlot);

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CConfirmBuyMsgBoxLayout));

}

void GCCharacterServerInfoRecv(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CHARACTER_SERVER_INFO Data = (LPPRECEIVE_CHARACTER_SERVER_INFO)ReceiveBuffer;

	if (CharacterMachine)
	{
		CharacterMachine->Lock = Data->Lock;
		CharacterMachine->AccountLevel = Data->AccountLevel;
		strcpy_s(CharacterMachine->AccountExpireDate, Data->AccountExpireDate);
	}
}

void ReceiveServerCommand(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SERVER_COMMAND Data = (LPPRECEIVE_SERVER_COMMAND)ReceiveBuffer;
	switch (Data->Cmd1)
	{
	case 2:
		PlayBuffer(SOUND_MEDAL);
		break;
	case 0:
	{
		vec3_t Position, Angle, Light;
		Position[0] = (float)(Data->Cmd2 + 0.5f) * TERRAIN_SCALE;
		Position[1] = (float)(Data->Cmd3 + 0.5f) * TERRAIN_SCALE;
		Position[2] = RequestTerrainHeight(Position[0], Position[1]);
		ZeroMemory(Angle, sizeof(Angle));
		Light[0] = Light[1] = Light[2] = 1.0f;
		CreateEffect(BITMAP_FIRECRACKER0001, Position, Angle, Light, 0);
	}
	break;
	case 1:
		if (Data->Cmd2 >= 20)
		{
			SEASON3B::CreateOkMessageBox(GlobalText[830 + Data->Cmd2 - 20]);
		}
		else
		{
			SEASON3B::CreateOkMessageBox(GlobalText[650 + Data->Cmd2]);
		}
		break;

	case 3:
		SEASON3B::CreateOkMessageBox(GlobalText[710 + Data->Cmd2]);
		break;
	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[725 + Data->Cmd2]);
		break;
	case 5:
	{
		SEASON3B::CDialogMsgBox* pMsgBox = NULL;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDialogMsgBoxLayout), &pMsgBox);
		if (pMsgBox)
		{
			pMsgBox->AddMsg(g_DialogScript[Data->Cmd2].m_lpszText);
		}
	}
	break;

	case 6:
		SEASON3B::CreateOkMessageBox(GlobalText[449]);
		break;
	case 13:
		SEASON3B::CreateOkMessageBox(GlobalText[1826]);
		break;
	case 14:
	{
		switch (Data->Cmd2)
		{
		case 0:
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CWhiteAngelEventLayout));
			break;

		case 1:
			SEASON3B::CreateOkMessageBox(GlobalText[2024]);
			break;

		case 2:
			SEASON3B::CreateOkMessageBox(GlobalText[2022]);
			break;

		case 3:
			SEASON3B::CreateOkMessageBox(GlobalText[2023]);
			break;

		case 4:
			//ShowCustomMessageBox(GlobalText[858]);
			break;

		case 5:
			SEASON3B::CreateOkMessageBox(GlobalText[2023]);
			break;
		}
	}
	break;
	case 15:
	{
		switch (Data->Cmd2)
		{
		case 0:
			SEASON3B::CreateOkMessageBox(GlobalText[2022]);
			break;
		case 1:
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CHarvestEventLayout));
			break;
		case 2:
			SEASON3B::CreateOkMessageBox(GlobalText[2023]);
			break;
		}
	}
	break;
	case 16:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox = NULL;

		switch (Data->Cmd2)
		{
		case 0:
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSantaTownSantaMsgBoxLayout), &pMsgBox);
			pMsgBox->AddMsg(GlobalText[2588]);
			break;
		case 1:
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSantaTownSantaMsgBoxLayout), &pMsgBox);
			pMsgBox->AddMsg(GlobalText[2585]);
			break;
		case 2:
			SEASON3B::CreateOkMessageBox(GlobalText[2587]);
			break;
		case 3:
			SEASON3B::CreateOkMessageBox(GlobalText[2023]);
			break;
		}
	}
	break;
	case 17:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSantaTownLeaveMsgBoxLayout));
		break;
	case 47:
	case 48:
	case 49:
		SEASON3B::CreateOkMessageBox(GlobalText[1823 + Data->Cmd1 - 47]);
		break;
	case 55:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[39]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 56:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[56]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 57:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[57]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 58:
	{
		WORD Key = ((WORD)(Data->Cmd2) << 8) + Data->Cmd3;
		Key &= 0x7FFF;
		int Index = FindCharacterIndex(Key);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);
		if (pCharacter)
		{
			OBJECT* to = &pCharacter->Object;

			if (to != NULL)
			{
				CreateEffect(MODEL_EFFECT_SKURA_ITEM, to->Position, to->Angle, to->Light, 0, to);
				PlayBuffer(SOUND_CHERRYBLOSSOM_EFFECT0, to);
			}
		}
	}
	break;
	case 59:
	{
		vec3_t Position, Angle, Light;
		Position[0] = (float)(Data->Cmd2 + 0.5f) * TERRAIN_SCALE;
		Position[1] = (float)(Data->Cmd3 + 0.5f) * TERRAIN_SCALE;
		Position[2] = RequestTerrainHeight(Position[0], Position[1]);
		ZeroMemory(Angle, sizeof(Angle));
		Light[0] = Light[1] = Light[2] = 1.0f;
		CreateEffect(BITMAP_FIRECRACKER0001, Position, Angle, Light, 1);
	}
	break;
	case 60:
		g_PortalMgr.ResetRevivePosition();
		break;
	case 61:
		g_PortalMgr.ResetPortalPosition();
		break;
	}
}

void ReceiveMix(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_ITEM Data = (LPPHEADER_DEFAULT_ITEM)ReceiveBuffer;

	switch (Data->Index)
	{
	case 0:
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND) && g_pLuckyItemWnd->GetAct())
		{
			g_pLuckyItemWnd->GetResult(0, Data->Index);
			break;
		}
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		unicode::t_char szText[256] = { 0, };
		switch (g_MixRecipeMgr.GetMixInventoryType())
		{
		case SEASON3A::MIXTYPE_GOBLIN_NORMAL:
		case SEASON3A::MIXTYPE_GOBLIN_CHAOSITEM:
		case SEASON3A::MIXTYPE_GOBLIN_ADD380:
		case SEASON3A::MIXTYPE_EXTRACT_SEED:
		case SEASON3A::MIXTYPE_SEED_SPHERE:
			unicode::_sprintf(szText, GlobalText[594]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
			// 			case SEASON3A::MIXTYPE_TRAINER:
			// 				unicode::_sprintf(szText, GlobalText[1208]);	// ºÎÈ° ½ÇÆÐ
			// 				g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			// 				break;
		case SEASON3A::MIXTYPE_OSBOURNE:
			unicode::_sprintf(szText, GlobalText[2105], GlobalText[2061]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_JERRIDON:
			unicode::_sprintf(szText, GlobalText[2105], GlobalText[2062]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_ELPIS:
			unicode::_sprintf(szText, GlobalText[2112], GlobalText[2063]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_CHAOS_CARD:
			unicode::_sprintf(szText, GlobalText[2112], GlobalText[2265]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_CHERRYBLOSSOM:
			unicode::_sprintf(szText, GlobalText[2112], GlobalText[2560]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		}
	}
	break;
	case 1:
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND) && g_pLuckyItemWnd->GetAct())
		{
			g_pLuckyItemWnd->GetResult(1, 0, Data->Item);
			break;
		}
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		unicode::t_char szText[256] = { 0, };
		switch (g_MixRecipeMgr.GetMixInventoryType())
		{
		case SEASON3A::MIXTYPE_GOBLIN_NORMAL:
		case SEASON3A::MIXTYPE_GOBLIN_CHAOSITEM:
		case SEASON3A::MIXTYPE_GOBLIN_ADD380:
		case SEASON3A::MIXTYPE_EXTRACT_SEED:
		case SEASON3A::MIXTYPE_SEED_SPHERE:
			unicode::_sprintf(szText, GlobalText[595]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
			// 			case SEASON3A::MIXTYPE_TRAINER:
			// 				unicode::_sprintf(szText, GlobalText[1209]);
			// 				g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			// 				break;
		case SEASON3A::MIXTYPE_OSBOURNE:
			unicode::_sprintf(szText, GlobalText[2106], GlobalText[2061]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_JERRIDON:
			unicode::_sprintf(szText, GlobalText[2106], GlobalText[2062]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_ELPIS:
			unicode::_sprintf(szText, GlobalText[2113], GlobalText[2063]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_CHAOS_CARD:
			unicode::_sprintf(szText, GlobalText[2113], GlobalText[2265]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		case SEASON3A::MIXTYPE_CHERRYBLOSSOM:
			unicode::_sprintf(szText, GlobalText[2113], GlobalText[2560]);
			g_pChatListBox->AddText("", szText, SEASON3B::TYPE_SYSTEM_MESSAGE);
			break;
		}

		g_pMixInventory->DeleteAllItems();
		g_pMixInventory->InsertItem(0, Data->Item);

		PlayBuffer(SOUND_MIX01);
		PlayBuffer(SOUND_JEWEL01);
	}
	break;
	case 2:
	case 0x0B:
	{
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_READY);
		g_pChatListBox->AddText("", GlobalText[596], SEASON3B::TYPE_ERROR_MESSAGE);
	}
	break;
	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[649]);
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		break;

	case 9:
		SEASON3B::CreateOkMessageBox(GlobalText[689]);
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		break;

	case 100:
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		g_pMixInventory->DeleteAllItems();
		g_pMixInventory->InsertItem(0, Data->Item);
		break;
	case 3:
	case 5:
	case 7:
	case 8:
	case 0x0A:
	case 0x20:
		if (g_pLuckyItemWnd->GetAct())
		{
			g_pLuckyItemWnd->GetResult(0, Data->Index, Data->Item);
		}
		break;
	default:
		g_pMixInventory->SetMixState(SEASON3B::CNewUIMixInventory::MIX_FINISHED);
		break;
	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x86 [ReceiveMix(%d)]", Data->Index);
}

void ReceiveMixExit(BYTE* ReceiveBuffer)
{
	g_ConsoleDebug->Write(MCD_RECEIVE, "0x87 [ReceiveMixExit]");
}

void ReceiveGemMixResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_JEWEL_MIX Data = (LPPMSG_ANS_JEWEL_MIX)ReceiveBuffer;
	char sBuf[256];
	memset(sBuf, 0, 256);
	switch (Data->m_iResult)
	{
	case 0:
	case 2:
	case 3:
	{
		sprintf(sBuf, "%s%s %s", GlobalText[1801], GlobalText[1816], GlobalText[868]);
		g_pChatListBox->AddText("", sBuf, SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	case 1:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGemIntegrationUnityResultMsgBoxLayout));
	}
	break;
	case 4:
	{
		g_pChatListBox->AddText("", GlobalText[1817], SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	case 5:
	{
		g_pChatListBox->AddText("", GlobalText[1811], SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	}
}

void ReceiveGemUnMixResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_JEWEL_UNMIX Data = (LPPMSG_ANS_JEWEL_UNMIX)ReceiveBuffer;
	char sBuf[256];
	memset(sBuf, 0, 256);

	switch (Data->m_iResult)
	{
	case 0:
	case 5:
	{
		sprintf(sBuf, "%s%s %s", GlobalText[1800], GlobalText[1816], GlobalText[868]);
		g_pChatListBox->AddText("", sBuf, SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	case 1:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGemIntegrationDisjointResultMsgBoxLayout));
	}
	break;
	case 2:
	case 3:
	case 4:
	case 6:
	{
		g_pChatListBox->AddText("", GlobalText[1812], SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	case 7:
	{
		g_pChatListBox->AddText("", GlobalText[1815], SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	case 8:
	{
		g_pChatListBox->AddText("", GlobalText[1811], SEASON3B::TYPE_SYSTEM_MESSAGE);
		COMGEM::GetBack();
	}
	break;
	}
}

void ReceiveMoveToDevilSquareResult(BYTE* ReceiveBuffer)
{
	g_pNewUISystem->Hide(SEASON3B::INTERFACE_DEVILSQUARE);

	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
		break;
	case 1:
		SEASON3B::CreateOkMessageBox(GlobalText[677]);
		break;

	case 2:
		SEASON3B::CreateOkMessageBox(GlobalText[678]);
		break;

	case 3:
		SEASON3B::CreateOkMessageBox(GlobalText[686]);
		break;

	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[687]);
		break;

	case 5:
		SEASON3B::CreateOkMessageBox(GlobalText[679]);
		break;

	case 6:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[39]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	}
}

void ReceiveDevilSquareOpenTime(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	if (0 == Data->Value)
	{
		SEASON3B::CreateOkMessageBox(GlobalText[643]);
	}
	else
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[644], (int)Data->Value);
		SEASON3B::CreateOkMessageBox(strText);
	}
}

void ReceiveDevilSquareCountDown(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;

	if (gMapManager->IsCursedTemple())
	{
		if (Data->Value == TYPE_MATCH_CURSEDTEMPLE_ENTER_CLOSE
			|| Data->Value == TYPE_MATCH_CURSEDTEMPLE_GAME_START)
		{
			g_CursedTemple->SetInterfaceState(false, 0);
			matchEvent::StartMatchCountDown(Data->Value + 1);
		}
	}
	else if (gMapManager->currentMap >= WD_65DOPPLEGANGER1 && gMapManager->currentMap <= WD_68DOPPLEGANGER4)
	{
		if (((BYTE)(Data->Value + 1) >= TYPE_MATCH_DOPPELGANGER_ENTER_CLOSE && (BYTE)(Data->Value + 1) <= TYPE_MATCH_DOPPELGANGER_CLOSE) || (BYTE)(Data->Value + 1) == TYPE_MATCH_NONE)
		{
			matchEvent::StartMatchCountDown((BYTE)(Data->Value + 1));
		}
	}
	else
	{
		matchEvent::StartMatchCountDown(Data->Value + 1);
	}
}

void ReceiveDevilSquareRank(BYTE* ReceiveBuffer)
{
	LPPDEVILRANK Data = (LPPDEVILRANK)ReceiveBuffer;
	matchEvent::SetMatchResult(Data->m_Count, Data->m_MyRank, (MatchResult*)&Data->m_byRank, Data->m_MyRank);
}

void ReceiveMoveToEventMatchResult(BYTE* ReceiveBuffer)
{
	g_pNewUISystem->HideAll();

	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
		break;
	case 1:
		SEASON3B::CreateOkMessageBox(GlobalText[854]);
		break;

	case 2:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[852], GlobalText[1146]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 3:
		SEASON3B::CreateOkMessageBox(GlobalText[686]);
		break;

	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[687]);
		break;

	case 5:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[853], GlobalText[1146], MAX_BLOOD_CASTLE_MEN);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 6:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[867], 6);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 7:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[56]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 8:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[852], GlobalText[1147]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 9:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[853], GlobalText[1147], MAX_CHAOS_CASTLE_MEN);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	}
}

void ReceiveEventZoneOpenTime(BYTE* ReceiveBuffer)
{
	LPPHEADER_MATCH_OPEN_VALUE Data = (LPPHEADER_MATCH_OPEN_VALUE)ReceiveBuffer;
	if (Data->Value == 1)
	{
		if (0 == Data->KeyH)
		{
			SEASON3B::CreateOkMessageBox(GlobalText[643]);
		}
		else
		{
			unicode::t_char strText[128];
			unicode::_sprintf(strText, GlobalText[644], (int)Data->KeyH);
			SEASON3B::CreateOkMessageBox(strText);
		}
	}
	else if (Data->Value == 2)
	{
		unicode::t_char strText[256];
		if (0 == Data->KeyH)
		{
			unicode::_sprintf(strText, GlobalText[850], GlobalText[1146]);
		}
		else
		{
			unicode::_sprintf(strText, GlobalText[851], (int)Data->KeyH, GlobalText[1146]);
		}
		SEASON3B::CreateOkMessageBox(strText);
	}
	else if (Data->Value == 4)
	{
		WORD time = MAKEWORD(Data->KeyL, Data->KeyH);

		if (0 == time)
		{
			char szOpenTime1[256] = { 0, };
			char szOpenTime2[256] = { 0, };

			sprintf(szOpenTime1, GlobalText[850], GlobalText[1147]);
			sprintf(szOpenTime2, GlobalText[1156], GlobalText[1147], Data->KeyM, 100);

			GlobalText.Remove(1154);
			GlobalText.Remove(1155);
			GlobalText.Add(1154, szOpenTime1);
			GlobalText.Add(1155, szOpenTime2);

			SEASON3B::CNewUICommonMessageBox* pMsgBox = NULL;
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CChaosCastleTimeCheckMsgBoxLayout), &pMsgBox);
			if (pMsgBox)
			{
				pMsgBox->AddMsg(GlobalText[1154]);
				pMsgBox->AddMsg(GlobalText[1155]);
			}
		}
		else
		{
			char Text[256];
			int Hour = (int)(time / 60);
			int Mini = (int)(time)-(Hour * 60);

			char szOpenTime[256] = { 0, };

			wsprintf(szOpenTime, GlobalText[1164], Hour);
			wsprintf(Text, GlobalText[851], Mini, GlobalText[1147]);
			strcat(szOpenTime, Text);

			GlobalText.Remove(1154);
			GlobalText.Add(1154, szOpenTime);

			SEASON3B::CNewUICommonMessageBox* pMsgBox = NULL;
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CChaosCastleTimeCheckMsgBoxLayout), &pMsgBox);
			if (pMsgBox)
			{
				pMsgBox->AddMsg(GlobalText[1154]);
			}
		}
	}
	else if (Data->Value == 5)
	{
		unicode::t_char strText[256];
		if (0 == Data->KeyH)
		{
			unicode::_sprintf(strText, GlobalText[850], GlobalText[2369]);
		}
		else
		{
			unicode::_sprintf(strText, GlobalText[851], (int)Data->KeyH, GlobalText[2369]);
		}
		SEASON3B::CreateOkMessageBox(strText);
	}
}

void ReceiveMoveToEventMatchResult2(BYTE* ReceiveBuffer)
{
	g_pNewUISystem->HideAll();

	LPPWHEADER_DEFAULT_WORD Data = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	switch (Data->Value)
	{
	case 0:
		break;
	case 1:
		SEASON3B::CreateOkMessageBox(GlobalText[854]);
		break;

	case 2:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[852], GlobalText[1147]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 3:
		SEASON3B::CreateOkMessageBox(GlobalText[686]);
		break;

	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[687]);
		break;

	case 5:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[853], GlobalText[1147], MAX_CHAOS_CASTLE_MEN);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 6:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[867], 6);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;

	case 7:
		SEASON3B::CreateOkMessageBox(GlobalText[423]);
		break;

	case 8:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[2043], GlobalText[57]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	}
}

void ReceiveSetAttribute(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SET_MAPATTRIBUTE Data = (LPPRECEIVE_SET_MAPATTRIBUTE)ReceiveBuffer;

	g_ErrorReport.Write("Type:%d \r\n", Data->m_byType);

	switch (Data->m_byType)
	{
	case 0:
	{
		if (gMapManager->InBloodCastle() && Data->m_byMapAttr == TW_NOGROUND)
		{
			SetActionObject(gMapManager->currentMap, 36, 0, 1.f);
		}

		for (int k = 0; k < Data->m_byCount; k++)
		{
			int dx = Data->m_vAttribute[(k * 2) + 1].m_byX - Data->m_vAttribute[(k * 2)].m_byX + 1;
			int dy = Data->m_vAttribute[(k * 2) + 1].m_byY - Data->m_vAttribute[(k * 2)].m_byY + 1;

			g_ErrorReport.Write("count:%d, x:%d, y:%d \r\n", Data->m_byCount, dx, dy);

			AddTerrainAttributeRange(Data->m_vAttribute[(k * 2)].m_byX, Data->m_vAttribute[(k * 2)].m_byY, dx, dy, Data->m_byMapAttr, 1 - Data->m_byMapSetType);
		}
	}
	break;

	case 1:
		for (int i = 0; i < Data->m_byCount; ++i)
		{
			if (Data->m_byMapSetType)
			{
				g_ErrorReport.Write("SubTerrainAttribute - count:%d, x:%d, y:%d \r\n", Data->m_byCount, Data->m_vAttribute[i].m_byX, Data->m_vAttribute[i].m_byY);
				SubTerrainAttribute(Data->m_vAttribute[i].m_byX, Data->m_vAttribute[i].m_byY, Data->m_byMapAttr);
			}
			else
			{
				g_ErrorReport.Write("AddTerrainAttribute - count:%d, x:%d, y:%d \r\n", Data->m_byCount, Data->m_vAttribute[i].m_byX, Data->m_vAttribute[i].m_byY);
				AddTerrainAttribute(Data->m_vAttribute[i].m_byX, Data->m_vAttribute[i].m_byY, Data->m_byMapAttr);
			}
		}
		break;
	}
}

void ReceiveMatchGameCommand(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MATCH_GAME_STATE Data = (LPPRECEIVE_MATCH_GAME_STATE)ReceiveBuffer;

	matchEvent::SetMatchGameCommand(Data);
}

void ReceiveDuelRequest(BYTE* ReceiveBuffer)
{
	if (g_MessageBox->IsEmpty() == false)
	{
		return;
	}

	LPPMSG_REQ_DUEL_ANSWER Data = (LPPMSG_REQ_DUEL_ANSWER)ReceiveBuffer;

	g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);

	if (g_pNewUISystem->IsImpossibleDuelInterface() == true)
	{
		g_DuelMgr.SendDuelRequestAnswer(DUEL_ENEMY, FALSE);
		return;
	}

	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDuelMsgBoxLayout));
	PlayBuffer(SOUND_OPEN_DUELWINDOW);
}

void ReceiveDuelStart(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_DUEL_INVITE Data = (LPPMSG_ANS_DUEL_INVITE)ReceiveBuffer;
	char szMessage[256];
	if (Data->nResult == 0)
	{
		g_DuelMgr.EnableDuel(TRUE);
		g_DuelMgr.SetHeroAsDuelPlayer(DUEL_HERO);
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);
		sprintf(szMessage, GlobalText[912], g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY));
		g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_ERROR_MESSAGE);

		g_pNewUISystem->Show(SEASON3B::INTERFACE_DUEL_WINDOW);
		PlayBuffer(SOUND_START_DUEL);
	}
	else if (Data->nResult == 15)
	{
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);
		sprintf(szMessage, GlobalText[913], g_DuelMgr.GetDuelPlayerID(DUEL_ENEMY));
		g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_ERROR_MESSAGE);
	}
	else if (Data->nResult == 16)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDuelCreateErrorMsgBoxLayout));
	}
	else if (Data->nResult == 28)
	{
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);
		sprintf(szMessage, GlobalText[2704], 30);
		g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_ERROR_MESSAGE);
	}
	else if (Data->nResult == 30)
	{
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);
		sprintf(szMessage, GlobalText[1811]);
		g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_ERROR_MESSAGE);
	}
}

void ReceiveDuelEnd(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_DUEL_EXIT Data = (LPPMSG_ANS_DUEL_EXIT)ReceiveBuffer;

	if (Data->nResult == 0)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_DUEL_WINDOW);
		g_DuelMgr.EnableDuel(FALSE);
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL, Data->bIndexH), Data->szID);

		g_pChatListBox->AddText("", GlobalText[914], SEASON3B::TYPE_ERROR_MESSAGE);

		if (g_wtMatchTimeLeft.m_Type == 2)
			g_wtMatchTimeLeft.m_Time = 0;
		g_DuelMgr.EnablePetDuel(FALSE);
	}
	else
	{
	}
}

void ReceiveDuelScore(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_SCORE_BROADCAST Data = (LPPMSG_DUEL_SCORE_BROADCAST)ReceiveBuffer;

	if (g_DuelMgr.IsDuelPlayer(MAKEWORD(Data->bIndexL1, Data->bIndexH1), DUEL_HERO))
	{
		g_DuelMgr.SetScore(DUEL_HERO, Data->btDuelScore1);
		g_DuelMgr.SetScore(DUEL_ENEMY, Data->btDuelScore2);
	}
	else if (g_DuelMgr.IsDuelPlayer(MAKEWORD(Data->bIndexL2, Data->bIndexH2), DUEL_HERO))
	{
		g_DuelMgr.SetScore(DUEL_HERO, Data->btDuelScore2);
		g_DuelMgr.SetScore(DUEL_ENEMY, Data->btDuelScore1);
	}
}

void ReceiveDuelHP(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_HP_BROADCAST Data = (LPPMSG_DUEL_HP_BROADCAST)ReceiveBuffer;

	if (g_DuelMgr.IsDuelPlayer(MAKEWORD(Data->bIndexL1, Data->bIndexH1), DUEL_HERO))
	{
		g_DuelMgr.SetHP(DUEL_HERO, Data->btHP1);
		g_DuelMgr.SetHP(DUEL_ENEMY, Data->btHP2);
		g_DuelMgr.SetSD(DUEL_HERO, Data->btShield1);
		g_DuelMgr.SetSD(DUEL_ENEMY, Data->btShield2);
	}
	else if (g_DuelMgr.IsDuelPlayer(MAKEWORD(Data->bIndexL2, Data->bIndexH2), DUEL_HERO))
	{
		g_DuelMgr.SetHP(DUEL_HERO, Data->btHP2);
		g_DuelMgr.SetHP(DUEL_ENEMY, Data->btHP1);
		g_DuelMgr.SetSD(DUEL_HERO, Data->btShield2);
		g_DuelMgr.SetSD(DUEL_ENEMY, Data->btShield1);
	}
}

void ReceiveDuelChannelList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_DUEL_CHANNELLIST Data = (LPPMSG_ANS_DUEL_CHANNELLIST)ReceiveBuffer;
	for (int i = 0; i < 4; ++i)
	{
		g_DuelMgr.SetDuelChannel(i, Data->channel[i].bStart, Data->channel[i].bWatch, Data->channel[i].szID1, Data->channel[i].szID2);
	}
}

void ReceiveDuelWatchRequestReply(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_DUEL_JOINCNANNEL Data = (LPPMSG_ANS_DUEL_JOINCNANNEL)ReceiveBuffer;
	if (Data->nResult == 0)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_DUELWATCH);

		g_DuelMgr.SetCurrentChannel(Data->nChannelId);
		g_DuelMgr.SetDuelPlayer(DUEL_HERO, MAKEWORD(Data->bIndexL1, Data->bIndexH1), Data->szID1);
		g_DuelMgr.SetDuelPlayer(DUEL_ENEMY, MAKEWORD(Data->bIndexL2, Data->bIndexH2), Data->szID2);
	}
	else if (Data->nResult == 16)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDuelCreateErrorMsgBoxLayout));
	}
	else if (Data->nResult == 27)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDuelWatchErrorMsgBoxLayout));
	}
	else
	{
	}
}

void ReceiveDuelWatcherJoin(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_JOINCNANNEL_BROADCAST Data = (LPPMSG_DUEL_JOINCNANNEL_BROADCAST)ReceiveBuffer;
	g_DuelMgr.AddDuelWatchUser(Data->szID);
}

void ReceiveDuelWatchEnd(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_DUEL_LEAVECNANNEL Data = (LPPMSG_ANS_DUEL_LEAVECNANNEL)ReceiveBuffer;
	if (Data->nResult == 0)
	{
		g_DuelMgr.RemoveAllDuelWatchUser();
	}
	else
	{
	}
}

void ReceiveDuelWatcherQuit(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_LEAVECNANNEL_BROADCAST Data = (LPPMSG_DUEL_LEAVECNANNEL_BROADCAST)ReceiveBuffer;
	g_DuelMgr.RemoveDuelWatchUser(Data->szID);
}

void ReceiveDuelWatcherList(BYTE* ReceiveBuffer)
{
	g_DuelMgr.RemoveAllDuelWatchUser();

	LPPMSG_DUEL_OBSERVERLIST_BROADCAST Data = (LPPMSG_DUEL_OBSERVERLIST_BROADCAST)ReceiveBuffer;
	for (int i = 0; i < Data->nCount; ++i)
	{
		g_DuelMgr.AddDuelWatchUser(Data->user[i].szID);
	}
}

void ReceiveDuelResult(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_RESULT_BROADCAST Data = (LPPMSG_DUEL_RESULT_BROADCAST)ReceiveBuffer;

	char szMessage[256];
	sprintf(szMessage, GlobalText[2689], 10);
	g_pChatListBox->AddText("", szMessage, SEASON3B::TYPE_SYSTEM_MESSAGE);

	SEASON3B::CDuelResultMsgBox* lpMsgBox = NULL;
	SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDuelResultMsgBoxLayout), &lpMsgBox);
	if (lpMsgBox)
	{
		lpMsgBox->SetIDs(Data->szWinner, Data->szLoser);
	}
	PlayBuffer(SOUND_OPEN_DUELWINDOW);
}

void ReceiveDuelRound(BYTE* ReceiveBuffer)
{
	LPPMSG_DUEL_ROUNDSTART_BROADCAST Data = (LPPMSG_DUEL_ROUNDSTART_BROADCAST)ReceiveBuffer;

	if (Data->nFlag == 0)
	{
		g_DuelMgr.SetHP(DUEL_HERO, 100);
		g_DuelMgr.SetHP(DUEL_ENEMY, 100);
		g_DuelMgr.SetSD(DUEL_HERO, 100);
		g_DuelMgr.SetSD(DUEL_ENEMY, 100);
		g_DuelMgr.SetFighterRegenerated(TRUE);
	}
	else if (Data->nFlag == 1)
	{
	}
}

void ReceiveCreateShopTitleViewport(BYTE* ReceiveBuffer)
{
	LPPSHOPTITLE_HEADERINFO Header = (LPPSHOPTITLE_HEADERINFO)ReceiveBuffer;

	int Offset = sizeof(PSHOPTITLE_HEADERINFO);

	for (int i = 0; i < Header->byCount; i++)
	{
		PSHOPTITLE_DATAINFO* pShopTitle = (PSHOPTITLE_DATAINFO*)(ReceiveBuffer + Offset);

		int Key = ((int)(pShopTitle->byIndexH) << 8) + pShopTitle->byIndexL;
		//int key = MAKEWORD(pShopTitle->byIndexL, pShopTitle->byIndexH);

		int index = FindCharacterIndex(Key);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

		if (pCharacter)
		{
			char szShopTitle[40];
			memcpy(szShopTitle, pShopTitle->szTitle, MAX_SHOPTITLE);
			szShopTitle[MAX_SHOPTITLE] = '\0';

			AddShopTitle(Key, pCharacter, (const char*)szShopTitle);
		}
		Offset += sizeof(PSHOPTITLE_DATAINFO);
	}
}

void ReceiveShopTitleChange(BYTE* ReceiveBuffer)
{
	LPPSHOPTITLE_CHANGEINFO Header = (LPPSHOPTITLE_CHANGEINFO)ReceiveBuffer;

	int key = MAKEWORD(Header->byIndexL, Header->byIndexH);
	int index = FindCharacterIndex(key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

	if (pCharacter)
	{
		char szShopTitle[40];
		memcpy(szShopTitle, Header->szTitle, MAX_SHOPTITLE);
		szShopTitle[MAX_SHOPTITLE] = '\0';

		if (strncmp(pCharacter->ID, (const char*)Header->szId, MAX_ID_SIZE) == 0)
		{
			AddShopTitle(key, pCharacter, (const char*)szShopTitle);
		}
		if (pCharacter == Hero)
		{
			unicode::t_string title;
			unicode::t_string temp_title = szShopTitle;

			g_pMyShopInventory->GetTitle(title);

			if (title != temp_title)
			{
				g_pMyShopInventory->ChangeTitle(temp_title);
			}
		}
	}
}

void ReceiveSetPriceResult(BYTE* ReceiveBuffer)
{
	LPPSHOPSETPRICE_RESULTINFO Header = (LPPSHOPSETPRICE_RESULTINFO)ReceiveBuffer;

	if (Header->byResult != 0x01 && g_IsPurchaseShop == PSHOPWNDTYPE_SALE)
	{
		// Header->byResult == 0x06
		if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
		{
			SEASON3B::CNewUIInventoryCtrl::DeletePickedItem();
		}
		RemovePersonalItemPrice(MAX_MY_INVENTORY_INDEX + g_pMyShopInventory->GetTargetIndex(), PSHOPWNDTYPE_SALE);

		SendRequestInventory();

		g_ErrorReport.Write("@ [Fault] ReceiveSetPriceResult (result : %d)\n", Header->byResult);
	}
}

void ReceiveCreatePersonalShop(BYTE* ReceiveBuffer)
{
	LPCREATEPSHOP_RESULSTINFO Header = (LPCREATEPSHOP_RESULSTINFO)ReceiveBuffer;

	if (Header->byResult == 0x01)
	{
		g_pMyShopInventory->ChangePersonal(true);
		g_pMyShopInventory->SetTypeValue(0);

		unicode::t_string title;
		g_pMyShopInventory->GetTitle(title);
		AddShopTitle(Hero->Key, Hero, title.c_str());
	}
	else if (Header->byResult == 0x03)
	{
		g_pMyShopInventory->ChangePersonal(true);
		g_pMyShopInventory->SetTypeValue(Header->byTypeValue);

		//unicode::t_string title;
		//g_pMyShopInventory->GetTitle(title);
		//AddShopTitle(Hero->Key, Hero, title.c_str());
	}
	else
	{
		g_pMyShopInventory->SetTypeValue(0);
		// Header->btResult == 0x03
		g_ErrorReport.Write("@ [Fault] ReceiveCreatePersonalShop (result : %d)\n", Header->byResult);
	}
}

void ReceiveDestroyPersonalShop(BYTE* ReceiveBuffer)
{
	LPDESTROYPSHOP_RESULTINFO Header = (LPDESTROYPSHOP_RESULTINFO)ReceiveBuffer;
	if (Header->byResult == 0x01)
	{
		int key = MAKEWORD(Header->byIndexL, Header->byIndexH);
		int index = FindCharacterIndex(key);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

		if (pCharacter)
		{
			if (pCharacter == Hero)
			{
				g_pMyShopInventory->ChangePersonal(false);
			}
			RemoveShopTitle(pCharacter);
		}
	}
	else
	{
		g_ErrorReport.Write("@ [Fault] ReceiveDestroyPersonalShop (result : %d)\n", Header->byResult);
	}
}

void ReceivePersonalShopItemList(BYTE* ReceiveBuffer)
{
	LPGETPSHOPITEMLIST_HEADERINFO Header = (LPGETPSHOPITEMLIST_HEADERINFO)ReceiveBuffer;

	if (Header->byResult == 0x01)
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE))
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_STORAGE);
		}

		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY))
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);
		}

		g_PersonalShopSeller.Initialize();

		unicode::t_string temp = (char*)Header->szShopTitle;
		g_pPurchaseShopInventory->ChangeTitleText(temp);
		g_pPurchaseShopInventory->GetInventoryCtrl()->RemoveAllItems();

		g_pNewUISystem->Show(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_INVENTORY);
		g_pMyInventory->ChangeMyShopButtonStateOpen();

		RemoveAllPerosnalItemPrice(PSHOPWNDTYPE_PURCHASE);	//. clear item price table
		LPGETPSHOPITEM_DATAINFO pShopItem = (LPGETPSHOPITEM_DATAINFO)(ReceiveBuffer + sizeof(GETPSHOPITEMLIST_HEADERINFO));

		for (int i = 0; i < Header->byCount; i++, pShopItem++)
		{
			if (pShopItem->iItemPrice > 0
				|| pShopItem->JoBValue > 0
				|| pShopItem->JoSValue > 0
				|| pShopItem->JoCValue > 0
				)
			{
				int itemindex = pShopItem->byPos - (MAX_MY_INVENTORY_INDEX);
				g_pPurchaseShopInventory->InsertItem(itemindex, pShopItem->byItemInfo);
				AddPersonalItemPrice(pShopItem->byPos, pShopItem->PersonalShopType, pShopItem->iItemPrice, pShopItem->JoBValue, pShopItem->JoSValue, pShopItem->JoCValue, PSHOPWNDTYPE_PURCHASE);
			}
			else
			{
				g_ConsoleDebug->Write(MCD_ERROR, "[ReceivePersonalShopItemList]Item Cound : %d, Item Index : %d, Item Price : %d", Header->byCount, i, pShopItem->iItemPrice);

				g_ErrorReport.Write("@ ReceivePersonalShopItemList - item price less than zero(%d)\n", pShopItem->iItemPrice);

				g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);

				return;
			}
		}

		int key = MAKEWORD(Header->byIndexL, Header->byIndexH);
		int index = FindCharacterIndex(key);

		if (index != 400)
		{
			g_pPurchaseShopInventory->ChangeShopCharacterIndex(index);
		}
	}
	else
	{
		switch (Header->byResult)
		{
		case 0x03:
		{
			g_pChatListBox->AddText("", GlobalText[1120], SEASON3B::TYPE_ERROR_MESSAGE);
		}
		break;
		case 0x04:
		default:
			g_ErrorReport.Write("@ [Fault] ReceivePersonalShopItemList (result : %d)\n", Header->byResult);
		}

	}

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x05 [ReceivePersonalShopItemList]");
}

void ReceiveRefreshItemList(BYTE* ReceiveBuffer)
{
	LPGETPSHOPITEMLIST_HEADERINFO Header = (LPGETPSHOPITEMLIST_HEADERINFO)ReceiveBuffer;

	if (Header->byResult == 0x01 && g_IsPurchaseShop == PSHOPWNDTYPE_PURCHASE)
	{
		g_pPurchaseShopInventory->GetInventoryCtrl()->RemoveAllItems();

		LPGETPSHOPITEM_DATAINFO pShopItem = (LPGETPSHOPITEM_DATAINFO)(ReceiveBuffer + sizeof(GETPSHOPITEMLIST_HEADERINFO));
		for (int i = 0; i < Header->byCount; i++, pShopItem++)
		{
			int itemindex = pShopItem->byPos - MAX_MY_INVENTORY_INDEX;
			g_pPurchaseShopInventory->InsertItem(itemindex, pShopItem->byItemInfo);

			AddPersonalItemPrice(pShopItem->byPos, pShopItem->PersonalShopType, pShopItem->iItemPrice, pShopItem->JoBValue, pShopItem->JoSValue, pShopItem->JoCValue, PSHOPWNDTYPE_PURCHASE);
		}
	}
	else
	{
		if (Header->byResult == 0x01)
		{
			auto pCurrentInvenCtrl = g_pPurchaseShopInventory->GetInventoryCtrl();

			size_t uiCntInvenCtrl = pCurrentInvenCtrl->GetNumberOfItems();
			g_ErrorReport.Write("@ [Notice] ReceiveRefreshItemList (InventoryCtrl Count Items(%d))\n", uiCntInvenCtrl);
		}
		else
		{
			g_ErrorReport.Write("@ [Fault] ReceiveRefreshItemList (result : %d)\n", Header->byResult);
		}
	}
}

void ReceivePurchaseItem(BYTE* ReceiveBuffer)
{
	LPPURCHASEITEM_RESULTINFO Header = (LPPURCHASEITEM_RESULTINFO)ReceiveBuffer;

	if (Header->byResult == 0x01)
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY))
		{
			RemovePersonalItemPrice(g_pPurchaseShopInventory->GetSourceIndex(), PSHOPWNDTYPE_PURCHASE);
			g_pPurchaseShopInventory->DeleteItem(g_pPurchaseShopInventory->GetSourceIndex());

			int myinvenitemindex = Header->byPos - MAX_EQUIPMENT_INDEX;
			g_pMyInventory->InsertItem(myinvenitemindex, Header->byItemInfo);
		}
		else
		{
			RemoveAllPerosnalItemPrice(PSHOPWNDTYPE_PURCHASE);
			int myinvenitemindex = Header->byPos - MAX_EQUIPMENT_INDEX;
			g_pMyInventory->InsertItem(myinvenitemindex, Header->byItemInfo);
		}
	}
	else if (Header->byResult == 0x06)
	{
		g_pChatListBox->AddText("", GlobalText[1166], SEASON3B::TYPE_ERROR_MESSAGE);
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);
	}
	else
	{
		char strText[255];

		switch (Header->byResult)
		{
		case 0x07:
		{
			g_pChatListBox->AddText("", GlobalText[423], SEASON3B::TYPE_ERROR_MESSAGE);
		}
		break;
		case 0x08:
		{
			g_pChatListBox->AddText("", GlobalText[375], SEASON3B::TYPE_ERROR_MESSAGE);

		}
		break;
		case 0x0B:
			sprintf(strText, GlobalText[580], GlobalText[1806]);
			g_pChatListBox->AddText("", strText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case 0x0C:
			sprintf(strText, GlobalText[580], GlobalText[1807]);
			g_pChatListBox->AddText("", strText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case 0x0D:
			sprintf(strText, GlobalText[580], GlobalText[3316]);
			g_pChatListBox->AddText("", strText, SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case 0x23:
			g_pChatListBox->AddText("", GlobalText[495], SEASON3B::TYPE_ERROR_MESSAGE);
			break;
		case 0x09:
		default:
			g_ErrorReport.Write("@ [Fault] ReceivePurchaseItem (result : %d)\n", Header->byResult);
		}
		SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
	}
}

void NotifySoldItem(BYTE* ReceiveBuffer)
{
	LPSOLDITEM_RESULTINFO Header = (LPSOLDITEM_RESULTINFO)ReceiveBuffer;
	char szId[MAX_ID_SIZE + 2] = { 0, };
	strncpy(szId, (char*)Header->szId, MAX_ID_SIZE);
	szId[MAX_ID_SIZE] = '\0';

	char Text[100];
	sprintf(Text, GlobalText[1122], szId);
	g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
}

void NotifyClosePersonalShop(BYTE* ReceiveBuffer)
{
	if (g_IsPurchaseShop == PSHOPWNDTYPE_PURCHASE)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);

		g_pChatListBox->AddText("", GlobalText[1126], SEASON3B::TYPE_ERROR_MESSAGE);
	}
}

void ReceiveDisplayEffectViewport(BYTE* ReceiveBuffer)
{
	LPDISPLAYEREFFECT_NOTIFYINFO Header = (LPDISPLAYEREFFECT_NOTIFYINFO)ReceiveBuffer;

	int key = MAKEWORD(Header->byIndexL, Header->byIndexH);
	int index = FindCharacterIndex(key);

	CHARACTER* pCharacter = gmCharacters->GetCharacter(index);

	if (pCharacter)
	{
		OBJECT* o = &pCharacter->Object;

		if (o->Kind == KIND_PLAYER)
		{
			switch (Header->byType)
			{
			case 0x01:	//. HP up
			{
#ifdef ENABLE_POTION_EFFECT
				CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 5, o);
#endif // ENABLE_POTION_EFFECT
			}
			break;
			case 0x02:	//. MP up
				break;
			case 0x10:	//. Level up
			{
				if (gCharacterManager.IsMasterLevel(pCharacter->Class) == true)
				{
					CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 45, o, 80, 2);
					for (int i = 0; i < 19; ++i)
					{
						CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 46, o, 80, 2);
					}
				}
				else
				{
					for (int i = 0; i < 15; ++i)
					{
						CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
					}
					CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);
				}
				PlayBuffer(SOUND_LEVEL_UP);
			}
			break;
			case 0x03:
			{
				CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 5, o);
			}
			break;
			case 0x11:
			{
				if (!gMapManager->InChaosCastle())
				{
					CreateEffect(MODEL_SHIELD_CRASH, o->Position, o->Angle, o->Light, 0, o);
					PlayBuffer(SOUND_SHIELDCLASH);
				}
			}
			break;
			}
		}
	}
}

int g_iMaxLetterCount = 0;

void ReceiveFriendList(BYTE* ReceiveBuffer)
{
	g_pWindowMgr->Reset();
	LPFS_FRIEND_LIST_HEADER Header = (LPFS_FRIEND_LIST_HEADER)ReceiveBuffer;
	int iMoveOffset = sizeof(FS_FRIEND_LIST_HEADER);
	char szName[MAX_ID_SIZE + 1] = { 0 };
	for (int i = 0; i < Header->Count; ++i)
	{
		LPFS_FRIEND_LIST_DATA Data = (LPFS_FRIEND_LIST_DATA)(ReceiveBuffer + iMoveOffset);
		strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
		szName[MAX_ID_SIZE] = '\0';
		g_pFriendList->AddFriend(szName, 0, Data->Server);
		iMoveOffset += sizeof(FS_FRIEND_LIST_DATA);
	}
	g_pFriendList->Sort(0);
	g_pFriendList->Sort(1);
	g_pWindowMgr->RefreshMainWndPalList();

	// Ã¤ÆÃ ¼­¹ö »ì¾Æ³²
	g_pWindowMgr->SetServerEnable(TRUE);
	if (g_iChatInputType == 0) SendRequestChangeState(2);

	g_iMaxLetterCount = Header->MaxMemo;

	if (Header->MemoCount > 0)
	{
		char temp[MAX_TEXT_LENGTH + 1];
		sprintf(temp, GlobalText[1072], Header->MemoCount, Header->MaxMemo);
		g_pChatListBox->AddText("", temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

void ReceiveAddFriendResult(BYTE* ReceiveBuffer)
{
	LPFS_FRIEND_RESULT Data = (LPFS_FRIEND_RESULT)ReceiveBuffer;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	char szText[MAX_TEXT_LENGTH + 1] = { 0 };
	strncpy(szText, (const char*)Data->Name, MAX_ID_SIZE);
	szText[MAX_ID_SIZE] = '\0';
	switch (Data->Result)
	{
	case 0x00:
		strcat(szText, GlobalText[1047]);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText);
		break;
	case 0x01:
	{
		g_pChatListBox->AddText("", GlobalText[1075], SEASON3B::TYPE_SYSTEM_MESSAGE);
		g_pFriendList->AddFriend(szName, 0, Data->Server);
		g_pFriendList->Sort();
		g_pWindowMgr->RefreshMainWndPalList();
		g_pFriendMenu->UpdateAllChatWindowInviteList();
	}
	break;
	case 0x03:
		strcpy(szText, GlobalText[1048]);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText);
		break;
	case 0x04:
		strcat(szText, GlobalText[1049]);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText);
		break;
	case 0x05:
		strcpy(szText, GlobalText[1050]);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText);
		break;
	case 0x06:
		strcpy(szText, GlobalText[1068]);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText);
		break;
	default:
		break;
	};
}

void ReceiveRequestAcceptAddFriend(BYTE* ReceiveBuffer)
{
	LPFS_ACCEPT_ADD_FRIEND_RESULT Data = (LPFS_ACCEPT_ADD_FRIEND_RESULT)ReceiveBuffer;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	char szText[MAX_TEXT_LENGTH + 1] = { 0 };
	strncpy(szText, (const char*)Data->Name, MAX_ID_SIZE);
	szText[MAX_ID_SIZE] = '\0';
	strcat(szText, GlobalText[1051]);

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_FRIEND) == false)
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_FRIEND);
	}

	DWORD dwWindowID = g_pWindowMgr->AddWindow(UIWNDTYPE_QUESTION_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, szText, -1);
	if (dwWindowID != 0)
	{
		((CUIQuestionWindow*)g_pWindowMgr->GetWindow(dwWindowID))->SaveID(szName);
	}
	PlayBuffer(SOUND_FRIEND_LOGIN_ALERT);
}

void ReceiveDeleteFriendResult(BYTE* ReceiveBuffer)
{
	LPFS_FRIEND_RESULT Data = (LPFS_FRIEND_RESULT)ReceiveBuffer;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	switch (Data->Result)
	{
	case 0x00:
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1052]);
		break;
	case 0x01:
		g_pFriendList->RemoveFriend(szName);
		g_pWindowMgr->RefreshMainWndPalList();
		break;
	default:
		break;
	};
}

void ReceiveFriendStateChange(BYTE* ReceiveBuffer)
{
	LPFS_FRIEND_STATE_CHANGE Data = (LPFS_FRIEND_STATE_CHANGE)ReceiveBuffer;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	if (Data->Server == 0xFC)
	{
		g_pFriendList->UpdateAllFriendState(0, Data->Server);
		g_pFriendList->Sort();
		g_pWindowMgr->RefreshMainWndPalList();
		g_pFriendMenu->LockAllChatWindow();
		g_pWindowMgr->SetServerEnable(FALSE);
		return;
	}
	g_pFriendList->UpdateFriendState(szName, 0, Data->Server);
	g_pFriendList->Sort();
	g_pWindowMgr->RefreshMainWndPalList();
	g_pFriendMenu->UpdateAllChatWindowInviteList();

	DWORD dwChatRoomUIID = g_pFriendMenu->CheckChatRoomDuplication(szName);
	if (dwChatRoomUIID > 0)
	{
		CUIChatWindow* pWindow = (CUIChatWindow*)g_pWindowMgr->GetWindow(dwChatRoomUIID);
		if (pWindow == NULL);
		else if (Data->Server >= 0xFD/* || Data->Server == 0xFB*/)
		{
			pWindow->Lock(TRUE);
		}
		else
		{
			pWindow->Lock(FALSE);
		}
	}
}

void ReceiveLetterSendResult(BYTE* ReceiveBuffer)
{
	LPFS_SEND_LETTER_RESULT Data = (LPFS_SEND_LETTER_RESULT)ReceiveBuffer;
	switch (Data->Result)
	{
	case 0x00:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1053]);
		break;
	case 0x01:
	{
		if (Data->WindowGuid != 0)
			g_pWindowMgr->SendUIMessage(UI_MESSAGE_CLOSE, Data->WindowGuid, 0);
		char temp[MAX_TEXT_LENGTH + 1];
		sprintf(temp, GlobalText[1046], g_cdwLetterCost);
		g_pChatListBox->AddText("", temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
	break;
	case 0x02:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1061]);
		break;
	case 0x03:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1064]);
		break;
	case 0x04:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1065]);
		break;
	case 0x06:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1068]);
		break;
	case 0x07:
		if (Data->WindowGuid != 0)
			((CUILetterWriteWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid))->SetSendState(FALSE);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[423]);
		break;
	default:
		break;
	};
}

void ReceiveLetter(BYTE* ReceiveBuffer)
{
	//g_pLetterList->ClearLetterList();

	LPFS_LETTER_ALERT Data = (LPFS_LETTER_ALERT)ReceiveBuffer;
	char szDate[16] = { 0 };
	strncpy(szDate, (char*)Data->Date, 10);
	szDate[10] = '\0';
	char szTime[16] = { 0 };
	strncpy(szTime, (char*)Data->Date + 11, 8);
	szTime[8] = '\0';

	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	char szSubject[32 + 1] = { 0 };
	strncpy(szSubject, (const char*)Data->Subject, 32);
	szSubject[32] = '\0';

	switch (Data->Read)
	{
	case 0x02:
		PlayBuffer(SOUND_FRIEND_MAIL_ALERT);
		g_pFriendMenu->SetNewMailAlert(TRUE);
		g_pChatListBox->AddText("", GlobalText[1062], SEASON3B::TYPE_SYSTEM_MESSAGE);
		g_pLetterList->AddLetter(Data->Index, szName, szSubject, szDate, szTime, 0x00);
		g_pLetterList->Sort();
		break;
	case 0x00:
	case 0x01:
		g_pLetterList->AddLetter(Data->Index, szName, szSubject, szDate, szTime, Data->Read);
		g_pLetterList->Sort(2);
		break;
	default:
		break;
	};

	g_pWindowMgr->RefreshMainWndLetterList();

	if (g_pLetterList->GetLetterCount() >= g_iMaxLetterCount)
	{
		g_pChatListBox->AddText("", GlobalText[1073], SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

extern int g_iLetterReadNextPos_x, g_iLetterReadNextPos_y;

void ReceiveLetterText(BYTE* ReceiveBuffer)
{
	LPFS_LETTER_TEXT Data = (LPFS_LETTER_TEXT)ReceiveBuffer;
	Data->Memo[Data->MemoSize] = '\0';

	g_pLetterList->CacheLetterText(Data->Index, Data);

	LETTERLIST_TEXT* pLetter = g_pLetterList->GetLetter(Data->Index);
	if (pLetter == NULL) return;
	pLetter->m_bIsRead = TRUE;
	g_pWindowMgr->RefreshMainWndLetterList();

	char tempTxt[MAX_TEXT_LENGTH + 1];
	sprintf(tempTxt, GlobalText[1054], pLetter->m_szText);
	DWORD dwUIID = 0;
	if (g_iLetterReadNextPos_x == UIWND_DEFAULT)
	{
		dwUIID = g_pWindowMgr->AddWindow(UIWNDTYPE_READLETTER, 100, 100, tempTxt);
	}
	else
	{
		dwUIID = g_pWindowMgr->AddWindow(UIWNDTYPE_READLETTER, g_iLetterReadNextPos_x, g_iLetterReadNextPos_y, tempTxt, 0, UIADDWND_FORCEPOSITION);
		g_iLetterReadNextPos_x = UIWND_DEFAULT;
	}

	CUILetterReadWindow* pWindow = (CUILetterReadWindow*)g_pWindowMgr->GetWindow(dwUIID);
	char szText[1000 + 1] = { 0 };
	strncpy(szText, (const char*)Data->Memo, 1000);
	szText[1000] = '\0';
	pWindow->SetLetter(pLetter, szText);
	g_pWindowMgr->SetLetterReadWindow(pLetter->m_dwLetterID, dwUIID);

	if (strnicmp(pLetter->m_szID, "webzen", MAX_ID_SIZE) == 0)
	{
		pWindow->m_Photo.SetWebzenMail(TRUE);
	}
	else
	{
		BYTE ServerClass = Data->Equipment[EQUIPMENT_HELPER] & 0x7F;
		pWindow->m_Photo.SetClass(gCharacterManager.ChangeServerClassTypeToClientClassType(ServerClass));
		pWindow->m_Photo.SetEquipmentPacket(Data->Equipment);
		pWindow->m_Photo.SetAnimation(Data->PhotoAction + AT_ATTACK1);
		int iAngle = Data->PhotoDir & 0x3F;
		int iZoom = (Data->PhotoDir & 0xC0) >> 6;
		pWindow->m_Photo.SetAngle(iAngle * 6);
		pWindow->m_Photo.SetZoom((iZoom * 10 + 80) / 100.0f);
	}
	pWindow->SendUIMessageDirect(UI_MESSAGE_LISTSCRLTOP, 0, 0);
}

void ReceiveLetterDeleteResult(BYTE* ReceiveBuffer)
{
	LPFS_LETTER_RESULT Data = (LPFS_LETTER_RESULT)ReceiveBuffer;
	switch (Data->Result)
	{
	case 0x00:
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1055]);
		break;
	case 0x01:
		g_pLetterList->RemoveLetter(Data->Index);
		g_pLetterList->RemoveLetterTextCache(Data->Index);
		break;
	default:
		break;
	};

	g_pWindowMgr->RefreshMainWndLetterList();
}

void ReceiveCreateChatRoomResult(BYTE* ReceiveBuffer)
{
	LPFS_CHAT_CREATE_RESULT Data = (LPFS_CHAT_CREATE_RESULT)ReceiveBuffer;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->ID, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	char szIP[16];
	memset(szIP, 0, 16);
	memcpy(szIP, Data->IP, 15);
	switch (Data->Result)
	{
	case 0x00:
		g_pFriendMenu->RemoveRequestWindow(szName);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1069]);
		break;
	case 0x01:
		if (Data->Type == 0)
		{
			g_pFriendMenu->RemoveRequestWindow(szName);
			DWORD dwUIID = g_pWindowMgr->AddWindow(UIWNDTYPE_CHAT, 100, 100, GlobalText[994]);
			((CUIChatWindow*)g_pWindowMgr->GetWindow(dwUIID))->ConnectToChatServer((char*)szIP, Data->RoomNumber, Data->Ticket);
		}
		else if (Data->Type == 1)
		{
			DWORD dwUIID = g_pFriendMenu->CheckChatRoomDuplication(szName);
			if (dwUIID == 0)
			{
				dwUIID = g_pWindowMgr->AddWindow(UIWNDTYPE_CHAT_READY, 100, 100, GlobalText[994]);
				((CUIChatWindow*)g_pWindowMgr->GetWindow(dwUIID))->ConnectToChatServer((char*)szIP, Data->RoomNumber, Data->Ticket);
				g_pWindowMgr->GetWindow(dwUIID)->SetState(UISTATE_READY);
				g_pWindowMgr->SendUIMessage(UI_MESSAGE_BOTTOM, dwUIID, 0);
				if (g_pWindowMgr->GetFriendMainWindow() != NULL)
					g_pWindowMgr->GetFriendMainWindow()->RemoveWindow(dwUIID);
			}
			else if (dwUIID == -1);
			else
			{
				((CUIChatWindow*)g_pWindowMgr->GetWindow(dwUIID))->DisconnectToChatServer();
				((CUIChatWindow*)g_pWindowMgr->GetWindow(dwUIID))->ConnectToChatServer((char*)szIP, Data->RoomNumber, Data->Ticket);

				//				SendRequestCRDisconnectRoom(((CUIChatWindow *)g_pWindowMgr->GetWindow(dwUIID))->GetCurrentSocket());
				//				DWORD dwOldRoomNumber = ((CUIChatWindow *)g_pWindowMgr->GetWindow(dwUIID))->GetRoomNumber();
				//				((CUIChatWindow *)g_pWindowMgr->GetWindow(dwUIID))->ConnectToChatServer((char *)szIP, Data->RoomNumber, Data->Ticket);
				//				g_pChatRoomSocketList->RemoveChatRoomSocket(dwOldRoomNumber);
			}
		}
		else if (Data->Type == 2)
		{
			DWORD dwUIID = g_pWindowMgr->AddWindow(UIWNDTYPE_CHAT_READY, 100, 100, GlobalText[994]);
			((CUIChatWindow*)g_pWindowMgr->GetWindow(dwUIID))->ConnectToChatServer((char*)szIP, Data->RoomNumber, Data->Ticket);
			g_pWindowMgr->GetWindow(dwUIID)->SetState(UISTATE_READY);
			g_pWindowMgr->SendUIMessage(UI_MESSAGE_BOTTOM, dwUIID, 0);
			if (g_pWindowMgr->GetFriendMainWindow() != NULL)
				g_pWindowMgr->GetFriendMainWindow()->RemoveWindow(dwUIID);
		}
		break;
	case 0x02:
		g_pFriendMenu->RemoveRequestWindow(szName);
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1070]);
		break;
	default:
		break;
	};
}

void ReceiveChatRoomInviteResult(BYTE* ReceiveBuffer)
{
	LPFS_CHAT_INVITE_RESULT Data = (LPFS_CHAT_INVITE_RESULT)ReceiveBuffer;
	CUIChatWindow* pChatWindow = (CUIChatWindow*)g_pWindowMgr->GetWindow(Data->WindowGuid);
	if (pChatWindow == NULL) return;

	switch (Data->Result)
	{
	case 0x00:
		pChatWindow->AddChatText(255, GlobalText[1056], 1, 0);
		break;
	case 0x01:
		if (pChatWindow->GetCurrentInvitePal() != NULL)
		{
			char szText[MAX_TEXT_LENGTH + 1] = { 0 };
			strncpy(szText, pChatWindow->GetCurrentInvitePal()->m_szID, MAX_ID_SIZE);
			szText[MAX_ID_SIZE] = '\0';
			strcat(szText, GlobalText[1057]);
			pChatWindow->AddChatText(255, szText, 1, 0);
		}
		else
		{
			assert(!"ReceiveChatRoomInviteResult");
		}
		break;
	case 0x03:
		pChatWindow->AddChatText(255, GlobalText[1074], 1, 0);
		break;
	default:
		break;
	};
}

void ReceiveChatRoomConnectResult(DWORD dwWindowUIID, BYTE* ReceiveBuffer)
{
	LPFS_CHAT_JOIN_RESULT Data = (LPFS_CHAT_JOIN_RESULT)ReceiveBuffer;
	switch (Data->Result)
	{
	case 0x00:
		g_pWindowMgr->AddWindow(UIWNDTYPE_OK_FORCE, UIWND_DEFAULT, UIWND_DEFAULT, GlobalText[1058]);
		break;
	case 0x01:
		break;
	default:
		break;
	};
}

void ReceiveChatRoomUserStateChange(DWORD dwWindowUIID, BYTE* ReceiveBuffer)
{
	LPFS_CHAT_CHANGE_STATE Data = (LPFS_CHAT_CHANGE_STATE)ReceiveBuffer;
	CUIChatWindow* pChatWindow = (CUIChatWindow*)g_pWindowMgr->GetWindow(dwWindowUIID);
	if (pChatWindow == NULL) return;
	char szName[MAX_ID_SIZE + 1] = { 0 };
	strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
	szName[MAX_ID_SIZE] = '\0';
	char szText[MAX_TEXT_LENGTH + 1] = { 0 };
	strncpy(szText, (const char*)Data->Name, MAX_ID_SIZE);
	szText[MAX_ID_SIZE] = '\0';
	switch (Data->Type)
	{
	case 0x00:
		if (pChatWindow->AddChatPal(szName, Data->Index, 0) >= 3)
		{
			strcat(szText, GlobalText[1059]);
			pChatWindow->AddChatText(255, szText, 1, 0);
		}
		break;
	case 0x01:
		if (pChatWindow->GetUserCount() >= 3)
		{
			strcat(szText, GlobalText[1060]);
			pChatWindow->AddChatText(255, szText, 1, 0);
		}
		pChatWindow->RemoveChatPal(szName);
		break;
	default:
		return;
		break;
	};
	if (pChatWindow->GetShowType() == 2)
		pChatWindow->UpdateInvitePalList();
}

void ReceiveChatRoomUserList(DWORD dwWindowUIID, BYTE* ReceiveBuffer)
{
	LPFS_CHAT_USERLIST_HEADER Header = (LPFS_CHAT_USERLIST_HEADER)ReceiveBuffer;
	int iMoveOffset = sizeof(FS_CHAT_USERLIST_HEADER);
	char szName[MAX_ID_SIZE + 1] = { 0 };
	for (int i = 0; i < Header->Count; ++i)
	{
		LPFS_CHAT_USERLIST_DATA Data = (LPFS_CHAT_USERLIST_DATA)(ReceiveBuffer + iMoveOffset);
		strncpy(szName, (const char*)Data->Name, MAX_ID_SIZE);
		szName[MAX_ID_SIZE] = '\0';
		((CUIChatWindow*)g_pWindowMgr->GetWindow(dwWindowUIID))->AddChatPal(szName, Data->Index, 0);
		iMoveOffset += sizeof(FS_CHAT_USERLIST_DATA);
	}
}

void ReceiveChatRoomChatText(DWORD dwWindowUIID, BYTE* ReceiveBuffer)
{
	LPFS_CHAT_TEXT Data = (LPFS_CHAT_TEXT)ReceiveBuffer;
	CUIChatWindow* pChatWindow = (CUIChatWindow*)g_pWindowMgr->GetWindow(dwWindowUIID);
	if (pChatWindow == NULL) return;

	char ChatMsg[MAX_CHATROOM_TEXT_LENGTH] = { '\0' };
	if (Data->MsgSize >= MAX_CHATROOM_TEXT_LENGTH) return;

	memcpy(ChatMsg, Data->Msg, Data->MsgSize);
	BuxConvert((LPBYTE)ChatMsg, Data->MsgSize);

	if (pChatWindow->GetState() == UISTATE_READY)
	{
		g_pFriendMenu->SetNewChatAlert(dwWindowUIID);
		g_pChatListBox->AddText("", GlobalText[1063], SEASON3B::TYPE_SYSTEM_MESSAGE);
		pChatWindow->SetState(UISTATE_HIDE);
		if (g_pWindowMgr->GetFriendMainWindow() != NULL)
		{
			g_pWindowMgr->GetFriendMainWindow()->AddWindow(dwWindowUIID, g_pWindowMgr->GetWindow(dwWindowUIID)->GetTitle());
		}
	}
	else if (pChatWindow->GetState() == UISTATE_HIDE || g_pWindowMgr->GetTopWindowUIID() != dwWindowUIID)
	{
		g_pFriendMenu->SetNewChatAlert(dwWindowUIID);
	}
	pChatWindow->AddChatText(Data->Index, (char*)ChatMsg, 3, 0);
}

void ReceiveChatRoomNoticeText(DWORD dwWindowUIID, BYTE* ReceiveBuffer)
{
	LPFS_CHAT_TEXT Data = (LPFS_CHAT_TEXT)ReceiveBuffer;
	Data->Msg[99] = '\0';
	if (Data->Msg[0] == '\0')
	{
		return;
	}

	g_pChatListBox->AddText("", (char*)Data->Msg, SEASON3B::TYPE_SYSTEM_MESSAGE);
}

void ReceiveOption(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_OPTION Data = (LPPRECEIVE_OPTION)ReceiveBuffer;

	g_pMainFrame->ResetSkillHotKey();

	int iHotKey;
	for (int i = 0; i < 10; ++i)
	{
		int iIndex = i * 2;
		iHotKey = MAKEWORD(Data->HotKey[iIndex + 1], Data->HotKey[iIndex]);

		if (iHotKey != 0xffff)
		{
			for (int j = 0; j < MAX_SKILLS; ++j)
			{
				if (iHotKey == CharacterAttribute->Skill[j])
				{
					g_pMainFrame->SetSkillHotKey(i, j);
					break;
				}
			}
		}
	}

	g_pOption->SetGameOptions(Data->GameOption);

	BYTE byQLevel, byWLevel, byELevel, byRLevel;
	byQLevel = (Data->QWERLevel & 0xFF000000) >> 24;
	byWLevel = (Data->QWERLevel & 0x00FF0000) >> 16;
	byELevel = (Data->QWERLevel & 0x0000FF00) >> 8;
	byRLevel = Data->QWERLevel & 0x000000FF;

	g_pMainFrame->SetItemHotKey(SEASON3B::HOTKEY_Q, Data->KeyQWE[0] + ITEM_POTION, byQLevel);
	g_pMainFrame->SetItemHotKey(SEASON3B::HOTKEY_W, Data->KeyQWE[1] + ITEM_POTION, byWLevel);
	g_pMainFrame->SetItemHotKey(SEASON3B::HOTKEY_E, Data->KeyQWE[2] + ITEM_POTION, byELevel);

	BYTE wChatListBoxSize = (Data->ChatLogBox >> 4) * 3;
	BYTE wChatListBoxBackAlpha = Data->ChatLogBox & 0x0F;

	g_pChatListBox->SetNumberOfShowingLines(wChatListBoxSize);

	g_pChatListBox->SetBackAlpha((wChatListBoxBackAlpha / 10.0f));

	g_pMainFrame->SetItemHotKey(SEASON3B::HOTKEY_R, Data->KeyR + ITEM_POTION, byRLevel);
}

void ReceiveEventChipInfomation(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_EVENT_CHIP_INFO Data = (LPPRECEIVE_EVENT_CHIP_INFO)ReceiveBuffer;

	g_pNewUISystem->HideAll();
	g_bEventChipDialogEnable = Data->m_byType + 1;
	g_shEventChipCount = Data->m_nChipCount;

	if (g_bEventChipDialogEnable == EVENT_SCRATCH_TICKET)
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_GOLD_BOWMAN);
		g_bEventChipDialogEnable = 0;
	}


	if (g_bEventChipDialogEnable == EVENT_LENA)
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_GOLD_BOWMAN_LENA);
		g_bEventChipDialogEnable = 0;

		if (Data->m_shMutoNum[0] != -1 && Data->m_shMutoNum[1] != -1 && Data->m_shMutoNum[2] != -1) {
			memcpy(g_shMutoNumber, Data->m_shMutoNum, sizeof(short) * 3);
		}
	}

	if (g_bEventChipDialogEnable == EVENT_SCRATCH_TICKET)


		if (g_bEventChipDialogEnable == EVENT_SCRATCH_TICKET)
		{
			ZeroMemory(g_strGiftName, sizeof(char) * 64);

			ClearInput(FALSE);
			InputTextMax[0] = 12;
			InputNumber = 1;
			InputEnable = false;
			GoldInputEnable = false;
			InputGold = 0;
			StorageGoldFlag = 0;
			g_bScratchTicket = true;
		}

}

void ReceiveEventChip(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_EVENT_CHIP Data = (LPPRECEIVE_EVENT_CHIP)ReceiveBuffer;
	if (Data->m_unChipCount != 0xFFFFFFFF)
		g_shEventChipCount = Data->m_unChipCount;
}

void ReceiveBuffState(BYTE* ReceiveBuffer)
{
	LPPMSG_ITEMEFFECTCANCEL Data = (LPPMSG_ITEMEFFECTCANCEL)ReceiveBuffer;

	eBuffState bufftype = static_cast<eBuffState>(Data->byBuffType);

	if (bufftype == eBuffNone || bufftype >= eBuff_Count) return;

	if (Data->byEffectOption == 0)
	{
		RegisterBuff(bufftype, (&Hero->Object), Data->wEffectTime);

		if (bufftype == eBuff_HelpNpc)
		{
			g_pChatListBox->AddText("", GlobalText[1828], SEASON3B::TYPE_SYSTEM_MESSAGE);
		}
	}
	else
	{
		UnRegisterBuff(bufftype, (&Hero->Object));
	}
}

void ReceiveMutoNumber(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MUTONUMBER Data = (LPPRECEIVE_MUTONUMBER)ReceiveBuffer;

	if (Data->m_shMutoNum[0] != -1 && Data->m_shMutoNum[1] != -1 && Data->m_shMutoNum[2] != -1)
	{
		memcpy(g_shMutoNumber, Data->m_shMutoNum, sizeof(short) * 3);
	}
}


void ReceiveServerImmigration(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;

	switch (Data->Value)
	{
	case 0:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CServerImmigrationErrorMsgBoxLayout));
		break;
	case 1:
		SEASON3B::CreateOkMessageBox("ReceiveServerImmigration");
		break;
	}
}

void ReceiveScratchResult(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SCRATCH_TICKET_EVENT Data = (LPPRECEIVE_SCRATCH_TICKET_EVENT)ReceiveBuffer;

	switch (Data->m_byIsRegistered)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		SEASON3B::CreateOkMessageBox(GlobalText[886 + Data->m_byIsRegistered]);
		break;

	case 5:
		SEASON3B::CreateOkMessageBox(GlobalText[899]);
		break;
	}

	memcpy(g_strGiftName, Data->m_strGiftName, sizeof(char) * 64);
}

void ReceivePlaySoundEffect(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PLAY_SOUND_EFFECT Data = (LPPRECEIVE_PLAY_SOUND_EFFECT)ReceiveBuffer;

	switch (Data->wEffectNum)
	{
	case 0:
		PlayBuffer(SOUND_RING_EVENT_READY);
		break;
	case 1:
		PlayBuffer(SOUND_RING_EVENT_START);
		break;
	case 2:
		PlayBuffer(SOUND_RING_EVENT_END);
		break;
	default:
		break;
	}
}

void ReceiveEventCount(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_EVENT_COUNT Data = (LPPRECEIVE_EVENT_COUNT)ReceiveBuffer;

	g_csQuest.SetEventCount(Data->m_wEventType, Data->m_wLeftEnterCount);
}

void ReceiveQuestHistory(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_QUEST_HISTORY Data = (LPPRECEIVE_QUEST_HISTORY)ReceiveBuffer;

	g_csQuest.setQuestLists(Data->m_byQuest, Data->m_byCount, Hero->Class);
}

void ReceiveQuestState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_QUEST_STATE Data = (LPPRECEIVE_QUEST_STATE)ReceiveBuffer;

	g_csQuest.setQuestList(Data->m_byQuestIndex, Data->m_byState);
	g_pNewUISystem->HideAll();
	g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCQUEST);
}

void ReceiveQuestResult(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_QUEST_RESULT Data = (LPPRECEIVE_QUEST_RESULT)ReceiveBuffer;

	if (Data->m_byResult == 0)
	{
		g_csQuest.setQuestList(Data->m_byQuestIndex, Data->m_byState);
		g_pNewUISystem->HideAll();
		g_pNewUISystem->Show(SEASON3B::INTERFACE_NPCQUEST);
	}
}

void ReceiveQuestPrize(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_QUEST_REPARATION Data = (LPPRECEIVE_QUEST_REPARATION)ReceiveBuffer;

	WORD Key = ((WORD)(Data->m_byKeyH) << 8) + Data->m_byKeyL;
	Key &= 0x7FFF;
	int Index = FindCharacterIndex(Key);

	switch (Data->m_byReparation)
	{
	case 200:
	{
		CHARACTER* c = gmCharacters->GetCharacter(Index);

		if (c)
		{
			if (c == Hero)
			{
				CharacterAttribute->LevelUpPoint += Data->m_byNumber;
			}

			OBJECT* o = &c->Object;

			vec3_t      Position;
			VectorCopy(o->Position, Position);
			for (int i = 0; i < 15; ++i)
			{
				CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
			}
			CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);
			PlayBuffer(SOUND_CHANGE_UP);
		}
	}
	break;
	case 201:
	{
		CHARACTER* c = gmCharacters->GetCharacter(Index);

		if (c)
		{
			OBJECT* o = &c->Object;
			vec3_t      Position;

			BYTE byClass = gCharacterManager.ChangeServerClassTypeToClientClassType(Data->m_byNumber);
			if (2 != gCharacterManager.GetStepClass(byClass))
				break;

			c->Class = byClass;

			if (Hero == c)
			{
				CharacterAttribute->Class = c->Class;
			}

			VectorCopy(o->Position, Position);
			Position[2] += 200.f;
			for (int i = 0; i < 15; ++i)
			{
				CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
				CreateJoint(BITMAP_FLARE, Position, Position, o->Angle, 10, o, 40, 2);
			}
			CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);

			SetChangeClass(c);
			SetAction(o, PLAYER_SALUTE1);

			PlayBuffer(SOUND_CHANGE_UP);
		}
	}
	break;
	case 202:
	{
		CHARACTER* c = gmCharacters->GetCharacter(Index);
		if (c)
		{
			OBJECT* o = &c->Object;
			vec3_t      Position;

			if (Hero == c)
			{
				CharacterAttribute->LevelUpPoint += Data->m_byNumber;
			}

			VectorCopy(o->Position, Position);
			Position[2] += 200.f;
			for (int i = 0; i < 15; ++i)
			{
				CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
				CreateJoint(BITMAP_FLARE, Position, Position, o->Angle, 10, o, 40, 2);
			}
			CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);

			SetChangeClass(c);
			SetAction(o, PLAYER_SALUTE1);
			PlayBuffer(SOUND_CHANGE_UP);
		}
	}
	break;
	case 203:
	{
		CHARACTER* c = gmCharacters->GetCharacter(Index);
		if (c)
		{
			OBJECT* o = &c->Object;
			vec3_t      Position;

			VectorCopy(o->Position, Position);
			Position[2] += 200.f;
			for (int i = 0; i < 15; ++i)
			{
				CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o, 40, 2);
				CreateJoint(BITMAP_FLARE, Position, Position, o->Angle, 10, o, 40, 2);
			}
			CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 0, o);

			SetChangeClass(c);
			SetAction(o, PLAYER_SALUTE1);

			PlayBuffer(SOUND_CHANGE_UP);
		}
	}
	break;
	case 204:
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

		if (pCharacter)
		{
			BYTE byClass = gCharacterManager.ChangeServerClassTypeToClientClassType(Data->m_byNumber);
			if (gCharacterManager.GetStepClass(byClass) == 3 || gCharacterManager.GetStepClass(byClass) == 4)
			{
				pCharacter->Class = byClass;

				if (Hero == pCharacter)
				{
					CharacterAttribute->Class = byClass;
				}

				CreateEffect(MODEL_CHANGE_UP_EFF, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_CHANGE_UP_NASA, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_CHANGE_UP_CYLINDER, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_INFINITY_ARROW3, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 1, &pCharacter->Object);

				SetChangeClass(pCharacter);
				SetAction(&pCharacter->Object, PLAYER_CHANGE_UP);
				PlayBuffer(SOUND_CHANGE_UP);
			}
		}
	}
	break;
	case 205:
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

		if (pCharacter)
		{
			BYTE byClass = gCharacterManager.ChangeServerClassTypeToClientClassType(Data->m_byNumber);
			if (gCharacterManager.GetStepClass(byClass) == 4)
			{
				pCharacter->Class = byClass;

				if (Hero == pCharacter)
				{
					CharacterAttribute->Class = byClass;
				}

				CreateEffect(MODEL_CHANGE_UP_EFF, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_CHANGE_UP_NASA, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_CHANGE_UP_CYLINDER, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 0, &pCharacter->Object);
				CreateEffect(MODEL_INFINITY_ARROW3, pCharacter->Object.Position, pCharacter->Object.Angle, pCharacter->Object.Light, 1, &pCharacter->Object);

				SetChangeClass(pCharacter);
				SetAction(&pCharacter->Object, PLAYER_CHANGE_UP);
				PlayBuffer(SOUND_CHANGE_UP);
			}
		}
	}
	break;
	default:
		break;
	}
}

void ReceiveQuestMonKillInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_QUEST_MONKILL_INFO pData = (LPPMSG_ANS_QUEST_MONKILL_INFO)ReceiveBuffer;

	g_csQuest.SetKillMobInfo(pData->anKillCountInfo);
}

#ifdef ASG_ADD_TIME_LIMIT_QUEST
void ReceiveQuestLimitResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_QUESTEXP_RESULT pData = (LPPMSG_ANS_QUESTEXP_RESULT)ReceiveBuffer;

	g_pNewUISystem->HideAll();

	switch (pData->m_byResult)
	{
	case QUEST_RESULT_CNT_LIMIT:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CQuestCountLimitMsgBoxLayout));
		break;
	}
}


void ReceiveQuestByItemUseEP(BYTE* ReceiveBuffer)
{
	DWORD* pdwQuestIndex = (DWORD*)(ReceiveBuffer + sizeof(PMSG_NPCTALK_QUESTLIST));
	SendQuestSelection(*pdwQuestIndex, 0);
}
#endif	// ASG_ADD_TIME_LIMIT_QUEST

void ReceiveQuestByEtcEPList(BYTE* ReceiveBuffer)
{
	LPPMSG_NPCTALK_QUESTLIST pData = (LPPMSG_NPCTALK_QUESTLIST)ReceiveBuffer;
	g_QuestMng.SetQuestIndexByEtcList((DWORD*)(ReceiveBuffer + sizeof(PMSG_NPCTALK_QUESTLIST)), pData->m_wQuestCount);
}

void ReceiveQuestByNPCEPList(BYTE* ReceiveBuffer)
{
	LPPMSG_NPCTALK_QUESTLIST pData = (LPPMSG_NPCTALK_QUESTLIST)ReceiveBuffer;
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
	{
		g_pNPCDialogue->ProcessQuestListReceive((DWORD*)(ReceiveBuffer + sizeof(PMSG_NPCTALK_QUESTLIST)), pData->m_wQuestCount);
	}
}

void ReceiveQuestQSSelSentence(BYTE* ReceiveBuffer)
{
	LPPMSG_NPC_QUESTEXP_INFO pData = (LPPMSG_NPC_QUESTEXP_INFO)ReceiveBuffer;

	g_QuestMng.SetCurQuestProgress(pData->m_dwQuestIndex);
}

void ReceiveQuestQSRequestReward(BYTE* ReceiveBuffer)
{
	LPPMSG_NPC_QUESTEXP_INFO pData = (LPPMSG_NPC_QUESTEXP_INFO)ReceiveBuffer;
	g_QuestMng.SetQuestRequestReward(ReceiveBuffer);
	g_QuestMng.SetCurQuestProgress(pData->m_dwQuestIndex);
	g_QuestMng.SetEPRequestRewardState(pData->m_dwQuestIndex, true);
}

void ReceiveQuestCompleteResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_QUESTEXP_COMPLETE pData = (LPPMSG_ANS_QUESTEXP_COMPLETE)ReceiveBuffer;

	switch (pData->m_byResult)
	{
	case 0:
		break;
	case 1:
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS))
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_QUEST_PROGRESS);
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC))
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC);

		g_QuestMng.SetEPRequestRewardState(pData->m_dwQuestIndex, false);
		g_QuestMng.RemoveCurQuestIndexList(pData->m_dwQuestIndex);
		break;

	case 2:
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS))
			g_pQuestProgress->EnableCompleteBtn(false);
		else if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC))
			g_pQuestProgressByEtc->EnableCompleteBtn(false);
		g_pChatListBox->AddText("", GlobalText[2816], SEASON3B::TYPE_ERROR_MESSAGE);
		break;

	case 3:
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS))
			g_pQuestProgress->EnableCompleteBtn(false);
		else if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC))
			g_pQuestProgressByEtc->EnableCompleteBtn(false);
		g_pChatListBox->AddText("", GlobalText[375], SEASON3B::TYPE_ERROR_MESSAGE);
		g_pChatListBox->AddText("", GlobalText[374], SEASON3B::TYPE_ERROR_MESSAGE);
		break;
	}
}

void ReceiveQuestGiveUp(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_QUESTEXP_GIVEUP pData = (LPPMSG_ANS_QUESTEXP_GIVEUP)ReceiveBuffer;
	g_QuestMng.RemoveCurQuestIndexList(pData->m_dwQuestGiveUpIndex);
}

void ReceiveProgressQuestList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_QUESTEXP_PROGRESS_LIST pData = (LPPMSG_ANS_QUESTEXP_PROGRESS_LIST)ReceiveBuffer;
	g_QuestMng.SetCurQuestIndexList((DWORD*)(ReceiveBuffer + sizeof(PMSG_ANS_QUESTEXP_PROGRESS_LIST)),
		int(pData->m_byQuestCount));
}

void ReceiveProgressQuestRequestReward(BYTE* ReceiveBuffer)
{
	LPPMSG_NPC_QUESTEXP_INFO pData = (LPPMSG_NPC_QUESTEXP_INFO)ReceiveBuffer;
	g_QuestMng.SetQuestRequestReward(ReceiveBuffer);
	g_pMyQuestInfoWindow->SetSelQuestRequestReward();
}

void ReceiveProgressQuestListReady(BYTE* ReceiveBuffer)
{
	g_QuestMng.SetQuestIndexByEtcList(NULL, 0);
	SendRequestProgressQuestList();
	SendRequestQuestByEtcEPList();
}


void ReceiveGensJoining(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_ANS_REG_GENS_MEMBER pData = (LPPMSG_ANS_REG_GENS_MEMBER)ReceiveBuffer;
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
		g_pNPCDialogue->ProcessGensJoiningReceive(pData->m_byResult, pData->m_byInfluence);
#endif
}

void ReceiveGensSecession(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_ANS_SECEDE_GENS_MEMBER pData = (LPPMSG_ANS_SECEDE_GENS_MEMBER)ReceiveBuffer;
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
		g_pNPCDialogue->ProcessGensSecessionReceive(pData->m_byResult);
#endif
}

void ReceivePlayerGensInfluence(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_MSG_SEND_GENS_INFO pData = (LPPMSG_MSG_SEND_GENS_INFO)ReceiveBuffer;
	Hero->m_byGensInfluence = pData->m_byInfluence;
	Hero->m_byRankIndex = pData->m_nGensClass;

	//Hero->SetGensMark();

	g_pNewUIGensRanking->SetContribution(pData->m_nContributionPoint);
	g_pNewUIGensRanking->SetRanking(pData->m_nRanking);
	g_pNewUIGensRanking->SetNextContribution(pData->m_nNextContributionPoint);
#endif
}

void ReceiveOtherPlayerGensInfluenceViewport(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_SEND_GENS_MEMBER_VIEWPORT Data = (LPPMSG_SEND_GENS_MEMBER_VIEWPORT)ReceiveBuffer;
	int nOffset = sizeof(PMSG_SEND_GENS_MEMBER_VIEWPORT);
	int i;
	for (i = 0; i < Data->m_byCount; ++i)
	{
		LPPMSG_GENS_MEMBER_VIEWPORT_INFO Data2 = (LPPMSG_GENS_MEMBER_VIEWPORT_INFO)(ReceiveBuffer + nOffset);
		int nKey = ((int)(Data2->m_byNumberH & 0x7f) << 8) + Data2->m_byNumberL;
		int nIndex = FindCharacterIndex(nKey);

		CHARACTER* c = gmCharacters->GetCharacter(nIndex);

		if (c == NULL)
			break;

		c->m_byGensInfluence = Data2->m_byInfluence;
		c->m_byRankIndex = Data2->m_nGensClass;
		c->m_nContributionPoint = Data2->m_nContributionPoint;

		//c->SetGensMark();

		if (::IsStrifeMap(gMapManager->currentMap))
		{
			vec3_t vTemp = { 0.f, 0.f, 0.f };
			if (Hero->m_byGensInfluence == c->m_byGensInfluence)
				CreateEffect(BITMAP_OUR_INFLUENCE_GROUND, c->Object.Position, vTemp, vTemp, 0, &c->Object);
			else
				CreateEffect(BITMAP_ENEMY_INFLUENCE_GROUND, c->Object.Position, vTemp, vTemp, 0, &c->Object);
		}

		nOffset += sizeof(PMSG_GENS_MEMBER_VIEWPORT_INFO);
	}
#endif
}

void ReceiveNPCDlgUIStart(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_NPC_CLICK pData = (LPPMSG_ANS_NPC_CLICK)ReceiveBuffer;
	if (!g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
	{
		g_QuestMng.SetNPC(pData->m_wNPCIndex);
		g_pNPCDialogue->SetContributePoint(pData->m_dwContributePoint);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_NPC_DIALOGUE);
	}
}

void ReceiveReward(BYTE* ReceiveBuffer)
{
#if MAIN_UPDATE > 303
	LPPMSG_GENS_REWARD_CODE pData = (LPPMSG_GENS_REWARD_CODE)ReceiveBuffer;

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
		g_pNPCDialogue->ProcessGensRewardReceive(pData->m_byRewardResult);
#endif
}

void ReceiveUseStateItem(BYTE* ReceiveBuffer)
{
	LPPMSG_USE_STAT_FRUIT Data = (LPPMSG_USE_STAT_FRUIT)ReceiveBuffer;

	BYTE result = Data->result;
	BYTE fruit = Data->btFruitType;
	WORD point = Data->btStatValue;

	unicode::t_char strText[MAX_GLOBAL_TEXT_STRING];

	if (result == 0 || result == 3 || result == 6 || result == 17)
	{
		CharacterAttribute->AddPoint = Data->btValue;
		CharacterAttribute->LevelUpPoint = Data->btLevelUpPoint;
		CharacterAttribute->Strength = Data->btStrength;
		CharacterAttribute->Dexterity = Data->btDexterity;
		CharacterAttribute->Vitality = Data->btVitality;
		CharacterAttribute->Energy = Data->btEnergy;
		CharacterAttribute->Charisma = Data->btCharisma;
	}
	char Text[MAX_GLOBAL_TEXT_STRING];

	switch (result)
	{
	case 0x00:
		if (fruit >= 0 && fruit <= 4)
		{
			int index = 0;

			switch (fruit)
			{
			case 0:
				index = 168;
				break;
			case 1:
				index = 169;
				break;
			case 2:
				index = 167;
				break;
			case 3:
				index = 166;
				break;
			case 4:
				index = 1900;
				break;
			}
			if (index != 0)
			{
				unicode::_sprintf(strText, GlobalText[379], GlobalText[index], point, GlobalText[1412]);
				SEASON3B::CreateOkMessageBox(strText);
			}
		}
		break;
	case 0x01:
		SEASON3B::CreateOkMessageBox(GlobalText[378]);
		break;
	case 0x02:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[1904], GlobalText[1412]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 0x03:
		if (fruit >= 0 && fruit <= 4)
		{
			int  index = 0;

			switch (fruit)
			{
			case 0:
				index = 168;
				break;
			case 1:
				index = 169;
				break;
			case 2:
				index = 167;
				break;
			case 3:
				index = 166;
				break;
			case 4:
				index = 1900;
				break;
			}

			CharacterAttribute->wMinusPoint += point;
			if (index != 0)
			{
				unicode::t_char strText[128];
				unicode::_sprintf(strText, GlobalText[379], GlobalText[index], point, GlobalText[1903]);
				SEASON3B::CreateOkMessageBox(strText);
			}
		}
		break;

	case 0x04:
		SEASON3B::CreateOkMessageBox(GlobalText[1906]);
		break;
	case 0x05:
	{
		unicode::t_char strText[128];
		unicode::_sprintf(strText, GlobalText[1904], GlobalText[1903]);
		SEASON3B::CreateOkMessageBox(strText);
	}
	break;
	case 0x06:
		if (fruit >= 0 && fruit <= 4)
		{
			int  index = 0;
			switch (fruit)
			{
			case 0:
				index = 168;
				break;
			case 1:
				index = 169;
				break;
			case 2:
				index = 167;
				break;
			case 3:
				index = 166;
				break;
			case 4:
				index = 1900;
				break;
			}
			if (index != 0)
			{
				sprintf(Text, GlobalText[379], GlobalText[index], point, GlobalText[1903]);
				SEASON3B::CreateOkMessageBox(Text);
			}
		}
		break;
	case 0x07:
		SEASON3B::CreateOkMessageBox(GlobalText[1906]);
		break;
	case 0x08:
		sprintf(Text, GlobalText[1904], GlobalText[1903]);
		SEASON3B::CreateOkMessageBox(Text);
		break;
	case 0x10:
		SEASON3B::CreateOkMessageBox(GlobalText[1909]);
		break;
	case 0x21:
		SEASON3B::CreateOkMessageBox(GlobalText[1911]);
		break;
	case 0x25:
		SEASON3B::CreateOkMessageBox(GlobalText[1911]);
		break;
	case 0x26:
		SEASON3B::CreateOkMessageBox(GlobalText[1912]);
		break;
	}

	EnableUse = 0;
}

void ReceivePetCommand(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PET_COMMAND Data = (LPPRECEIVE_PET_COMMAND)ReceiveBuffer;
	WORD Key = ((WORD)(Data->m_byKeyH) << 8) + Data->m_byKeyL;

	giPetManager::SetPetCommand(Hero, Key, Data->m_byCommand);
}

void ReceivePetAttack(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PET_ATTACK Data = (LPPRECEIVE_PET_ATTACK)ReceiveBuffer;

	WORD Key = ((WORD)(Data->m_byKeyH) << 8) + Data->m_byKeyL;

	int  index = FindCharacterIndex(Key);

	CHARACTER* sc = gmCharacters->GetCharacter(index);

	if (sc)
	{
		Key = ((WORD)(Data->m_byTKeyH) << 8) + Data->m_byTKeyL;

		giPetManager::SetAttack(sc, Key, Data->m_bySkillType);
	}
}

void ReceivePetInfo(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_PET_INFO Data = (LPPRECEIVE_PET_INFO)ReceiveBuffer;

	PET_INFO Petinfo;
	ZeroMemory(&Petinfo, sizeof(PET_INFO));

	Petinfo.m_dwPetType = Data->m_byPetType;
	Petinfo.m_wLevel = Data->m_byLevel;
	Petinfo.m_wLife = Data->m_byLife;
	Petinfo.m_dwExp1 = Data->m_iExp;
	giPetManager::SetPetInfo(Data->m_byInvType, Data->m_byPos, &Petinfo);
}

void ReceiveWTTimeLeft(BYTE* ReceiveBuffer)
{
	LPPMSG_MATCH_TIMEVIEW Data = (LPPMSG_MATCH_TIMEVIEW)ReceiveBuffer;
	g_wtMatchTimeLeft.m_Time = Data->m_Time;
	g_wtMatchTimeLeft.m_Type = Data->m_Type;
}

void ReceiveWTMatchResult(BYTE* ReceiveBuffer)
{
	LPPMSG_MATCH_RESULT	Data = (LPPMSG_MATCH_RESULT)ReceiveBuffer;
	if (Data->m_Type >= 0 && Data->m_Type < 3)
	{
		g_wtMatchResult.m_Type = Data->m_Type;
		memset(g_wtMatchResult.m_MatchTeamName1, 0, MAX_ID_SIZE);
		memcpy(g_wtMatchResult.m_MatchTeamName1, Data->m_MatchTeamName1, MAX_ID_SIZE);
		g_wtMatchResult.m_Score1 = Data->m_Score1;
		memset(g_wtMatchResult.m_MatchTeamName2, 0, MAX_ID_SIZE);
		memcpy(g_wtMatchResult.m_MatchTeamName2, Data->m_MatchTeamName2, MAX_ID_SIZE);
		g_wtMatchResult.m_Score2 = Data->m_Score2;
	}
}

void ReceiveWTBattleSoccerGoalIn(BYTE* ReceiveBuffer)
{
	/*	LPPMSG_SOCCER_GOALIN Data = (LPPMSG_SOCCER_GOALIN)ReceiveBuffer;

	  vec3_t Position, Angle, Light;
	  Position[0]  = (float)(Data->m_x+0.5f)*TERRAIN_SCALE;
	  Position[1]  = (float)(Data->m_y+0.5f)*TERRAIN_SCALE;
	  Position[2] = RequestTerrainHeight(Position[0],Position[1]);
	  ZeroMemory( Angle, sizeof ( Angle));
	  Light[0] = Light[1] = Light[2] = 1.0f;
		CreateEffect(BITMAP_FIRECRACKERRISE,Position,Angle,Light);*/
}

void ReceiveChangeMapServerInfo(BYTE* ReceiveBuffer)
{
	LPPHEADER_MAP_CHANGESERVER_INFO Data = (LPPHEADER_MAP_CHANGESERVER_INFO)ReceiveBuffer;

	g_pReconnectUI->ReconnectOnMapServerMove(Data->m_vSvrInfo.m_szMapSvrIpAddress, Data->m_vSvrInfo.m_wMapSvrPort);

	if (0 == Data->m_vSvrInfo.m_wMapSvrPort)
	{
		LoadingWorld = 0;

		Teleport = false;
		return;
	}

	g_PortalMgr.Reset();

	g_csMapServer.ConnectChangeMapServer(Data->m_vSvrInfo);
}

void ReceiveChangeMapServerResult(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT Data = (LPPHEADER_DEFAULT)ReceiveBuffer;

	g_pReconnectUI->ReconnectOnMapServerMoveAuth(Data->Value);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0xB1 [ReceiveChangeMapServerResult]");
}

void ReceiveBCStatus(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CASTLESIEGESTATE Data = (LPPMSG_ANS_CASTLESIEGESTATE)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 0x00:
		g_pChatListBox->AddText("", GlobalText[1500], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x01:
	case 0x02:
		g_pNewUISystem->Show(SEASON3B::INTERFACE_GUARDSMAN);
		g_pGuardWindow->SetData(Data);
		break;
	case 0x03:
		g_pChatListBox->AddText("", GlobalText[1501], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x04:
		g_pChatListBox->AddText("", GlobalText[1502], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCReg(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_REGCASTLESIEGE Data = (LPPMSG_ANS_REGCASTLESIEGE)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 0x00:
		g_pChatListBox->AddText("", GlobalText[1503], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x01:
		g_GuardsMan.SetRegStatus(1);
		g_pChatListBox->AddText("", GlobalText[1504], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x02:
		g_pChatListBox->AddText("", GlobalText[1505], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x03:
		g_pChatListBox->AddText("", GlobalText[1506], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x04:
		g_pChatListBox->AddText("", GlobalText[1507], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x05:
		g_pChatListBox->AddText("", GlobalText[1508], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x06:
		g_pChatListBox->AddText("", GlobalText[1509], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x07:
		g_pChatListBox->AddText("", GlobalText[1510], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x08:
		g_pChatListBox->AddText("", GlobalText[1511], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	default:
		assert(!"ReceiveBCReg(0xB2, 0x01)");
		break;
	}
}

void ReceiveBCGiveUp(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_GIVEUPCASTLESIEGE Data = (LPPMSG_ANS_GIVEUPCASTLESIEGE)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 0x00:
		g_pChatListBox->AddText("", GlobalText[1512], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x01:
		SendRequestBCRegInfo();
		SendRequestBCDeclareGuildList();
		g_GuardsMan.SetRegStatus(0);
		g_pChatListBox->AddText("", GlobalText[1513], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x02:
		g_pChatListBox->AddText("", GlobalText[1514], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x03:
		g_pChatListBox->AddText("", GlobalText[1515], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	default:
		assert(!"ReceiveBCGiveUp(0xB2,0x02)");
		break;
	}
}

void ReceiveBCRegInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_GUILDREGINFO Data = (LPPMSG_ANS_GUILDREGINFO)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 0x00:
		g_GuardsMan.SetRegStatus(0);
		break;
	case 0x01:
	{
		g_GuardsMan.SetRegStatus(!Data->btIsGiveUp);
		DWORD dwMarkCount;
		BYTE* pMarkCount = (BYTE*)&dwMarkCount;
		*pMarkCount++ = Data->btGuildMark4;
		*pMarkCount++ = Data->btGuildMark3;
		*pMarkCount++ = Data->btGuildMark2;
		*pMarkCount++ = Data->btGuildMark1;
		g_GuardsMan.SetMarkCount(dwMarkCount);
	}
	break;
	case 0x02:
		g_GuardsMan.SetRegStatus(0);
		break;
	}
}

void ReceiveBCRegMark(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_REGGUILDMARK Data = (LPPMSG_ANS_REGGUILDMARK)ReceiveBuffer;

	switch (Data->btResult)
	{
	case 0x00:
		g_pChatListBox->AddText("", GlobalText[1516], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x01:
	{
		DWORD dwMarkCount;
		BYTE* pMarkCount = (BYTE*)&dwMarkCount;
		*pMarkCount++ = Data->btGuildMark4;
		*pMarkCount++ = Data->btGuildMark3;
		*pMarkCount++ = Data->btGuildMark2;
		*pMarkCount++ = Data->btGuildMark1;
		g_GuardsMan.SetMarkCount(dwMarkCount);
	}
	break;
	case 0x02:
		g_pChatListBox->AddText("", GlobalText[1517], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 0x03:
		g_pChatListBox->AddText("", GlobalText[1518], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCNPCBuy(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_NPCBUY Data = (LPPMSG_ANS_NPCBUY)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[1519], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
		g_SenatusInfo.BuyNewNPC(Data->iNpcNumber, Data->iNpcIndex);
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[1520], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 4:
		g_pChatListBox->AddText("", GlobalText[1616], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCNPCRepair(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_NPCREPAIR Data = (LPPMSG_ANS_NPCREPAIR)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
	{
		g_pChatListBox->AddText("", GlobalText[1519], SEASON3B::TYPE_SYSTEM_MESSAGE);
	}

	break;
	case 1:
	{
		LPPMSG_NPCDBLIST pNPCInfo = NULL;
		pNPCInfo = g_SenatusInfo.GetNPCInfo(Data->iNpcNumber, Data->iNpcIndex);
		pNPCInfo->iNpcHp = Data->iNpcHP;
		pNPCInfo->iNpcMaxHp = Data->iNpcMaxHP;
	}
	break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[1520], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCNPCUpgrade(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_NPCUPGRADE Data = (LPPMSG_ANS_NPCUPGRADE)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[1519], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
	{
		LPPMSG_NPCDBLIST pNPCInfo = g_SenatusInfo.GetNPCInfo(Data->iNpcNumber, Data->iNpcIndex);

		if (pNPCInfo)
		{
			if (Data->iNpcUpType == 1)
				pNPCInfo->iNpcDfLevel = Data->iNpcUpValue;
			else if (Data->iNpcUpType == 2)
				pNPCInfo->iNpcRgLevel = Data->iNpcUpValue;
			else if (Data->iNpcUpType == 3)
				pNPCInfo->iNpcHp = pNPCInfo->iNpcMaxHp = Data->iNpcUpValue;
		}
	}
	break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[1520], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 4:
		g_pChatListBox->AddText("", GlobalText[1521], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 5:
		g_pChatListBox->AddText("", GlobalText[1522], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 6:
		g_pChatListBox->AddText("", GlobalText[1523], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 7:
		g_pChatListBox->AddText("", GlobalText[1524], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveInfoCastleNpc(BYTE* ReceiveBuffer)
{
	LPPMSG_INFO_STATUE_AND_GATE Data = (LPPMSG_INFO_STATUE_AND_GATE)ReceiveBuffer;

	for (int i = 0; i < 4; i++)
	{
		g_SenatusInfo.GATELEVEL_DEFENSE[i] = Data->iNpcDefense1[i];
		g_SenatusInfo.STATUELEVEL_DEFENSE[i] = Data->iNpcDefense2[i];

		g_SenatusInfo.GATELEVEL_HP[i] = Data->iNpcUpMaxHP1[i];
		g_SenatusInfo.STATUELEVEL_HP[i] = Data->iNpcUpMaxHP2[i];
		g_SenatusInfo.STATUELEVEL_RECOVER[i] = Data->iNpcUpRecLevel[i];

		g_ConsoleDebug->Write(5, "[iNpcUpMaxHP1: %d][iNpcUpMaxHP2: %d][iNpcDefense1: %d][iNpcDefense2: %d][iNpcUpRecLevel: %d]", Data->iNpcUpMaxHP1[i], Data->iNpcUpMaxHP2[i], Data->iNpcDefense1[i], Data->iNpcDefense2[i], Data->iNpcUpRecLevel[i]);
	}
}

void ReceiveBCGetTaxInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_TAXMONEYINFO Data = (LPPMSG_ANS_TAXMONEYINFO)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[1525], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
		g_SenatusInfo.SetTaxInfo(Data);
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCChangeTaxRate(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_TAXRATECHANGE Data = (LPPMSG_ANS_TAXRATECHANGE)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[1526], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
		if (Data->btTaxType == 3)
		{
			g_pUIGateKeeper->SetEntranceFee((Data->btTaxRate1 << 24) | (Data->btTaxRate2 << 16) | (Data->btTaxRate3 << 8) | (Data->btTaxRate4));
		}
		else
		{
			g_SenatusInfo.ChangeTaxInfo(Data);
		}
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCWithdraw(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_MONEYDRAWOUT Data = (LPPMSG_ANS_MONEYDRAWOUT)ReceiveBuffer;
	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[1527], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
		g_SenatusInfo.ChangeCastleMoney(Data);
		break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveTaxInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_MAPSVRTAXINFO Data = (LPPMSG_ANS_MAPSVRTAXINFO)ReceiveBuffer;

	if (Data->btTaxType == 1)
	{
		g_nChaosTaxRate = Data->btTaxRate;
	}
	else if (Data->btTaxType == 2)
	{
		g_pNPCShop->SetTaxRate(Data->btTaxRate);
		g_nTaxRate = Data->btTaxRate;
	}
	else
	{
		assert(!"Packet Recv(0xB2(0x1A))");
	}
}

void ReceiveHuntZoneEnter(BYTE* ReceiveBuffer)
{
	LPPMSG_CSHUNTZONEENTER pData = (LPPMSG_CSHUNTZONEENTER)ReceiveBuffer;

	switch (pData->m_byResult)
	{
	case 0:
	{
		g_pUIPopup->CancelPopup();
		g_pUIPopup->SetPopup(GlobalText[860], 1, 50, POPUP_OK, NULL);
	}
	break;

	case 1:
		g_pUIGateKeeper->SetPublic(pData->m_byHuntZoneEnter);
		break;

	case 2:
	{
		g_pUIPopup->CancelPopup();
		g_pUIPopup->SetPopup(GlobalText[1386], 1, 50, POPUP_OK, NULL);
	}
	break;
	}
}

void ReceiveBCNPCList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_NPCDBLIST Data = (LPPMSG_ANS_NPCDBLIST)ReceiveBuffer;
	int Offset = sizeof(PMSG_ANS_NPCDBLIST);

	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[860], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
	{
		for (int i = 0; i < Data->iCount; ++i)
		{
			LPPMSG_NPCDBLIST pNpcInfo = (LPPMSG_NPCDBLIST)(ReceiveBuffer + Offset);
			g_SenatusInfo.SetNPCInfo(pNpcInfo);
			Offset += sizeof(PMSG_NPCDBLIST);
		}
	}
	break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1386], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveBCDeclareGuildList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CSREGGUILDLIST Data = (LPPMSG_ANS_CSREGGUILDLIST)ReceiveBuffer;
	int Offset = sizeof(PMSG_ANS_CSREGGUILDLIST);

	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[860], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
	{
		g_pGuardWindow->ClearDeclareGuildList();
		for (int i = 0; i < Data->iCount; ++i)
		{
			LPPMSG_CSREGGUILDLIST pData2 = (LPPMSG_CSREGGUILDLIST)(ReceiveBuffer + Offset);

			DWORD dwMarkCount;
			BYTE* pMarkCount = (BYTE*)&dwMarkCount;
			*pMarkCount++ = pData2->btRegMarks4;
			*pMarkCount++ = pData2->btRegMarks3;
			*pMarkCount++ = pData2->btRegMarks2;
			*pMarkCount++ = pData2->btRegMarks1;

			g_pGuardWindow->AddDeclareGuildList(pData2->szGuildName, dwMarkCount, pData2->btIsGiveUp, pData2->btSeqNum);

			Offset += sizeof(PMSG_CSREGGUILDLIST);
		}
		g_pGuardWindow->SortDeclareGuildList();
	}
	break;
	}
}

void ReceiveBCGuildList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CSATTKGUILDLIST Data = (LPPMSG_ANS_CSATTKGUILDLIST)ReceiveBuffer;
	int Offset = sizeof(PMSG_ANS_CSATTKGUILDLIST);

	switch (Data->btResult)
	{
	case 0:
		g_pChatListBox->AddText("", GlobalText[860], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 1:
	{
		g_pGuardWindow->ClearGuildList();
		for (int i = 0; i < Data->iCount; ++i)
		{
			LPPMSG_CSATTKGUILDLIST pData2 = (LPPMSG_CSATTKGUILDLIST)(ReceiveBuffer + Offset);

			g_pGuardWindow->AddGuildList(pData2->szGuildName, pData2->btCsJoinSide, pData2->btGuildInvolved, pData2->iGuildScore);

			Offset += sizeof(PMSG_CSATTKGUILDLIST);
		}
	}
	break;
	case 2:
		g_pChatListBox->AddText("", GlobalText[1609], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	case 3:
		g_pChatListBox->AddText("", GlobalText[1609], SEASON3B::TYPE_SYSTEM_MESSAGE);
		break;
	}
}

void ReceiveGateState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GATE_STATE Data = (LPPRECEIVE_GATE_STATE)ReceiveBuffer;
	int Key = ((int)(Data->m_byKeyH) << 8) + Data->m_byKeyL;

	switch (Data->m_byResult)
	{
	case 0:
		break;

	case 1:
		npcGateSwitch::DoInterfaceOpen(Key);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_GATESWITCH);
		break;

	case 2:
		break;

	case 3:
		break;

	case 4:
		break;
	}
}

void ReceiveGateOperator(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GATE_OPERATOR Data = (LPPRECEIVE_GATE_OPERATOR)ReceiveBuffer;
	int Key = ((int)(Data->m_byKeyH) << 8) + Data->m_byKeyL;

	npcGateSwitch::ProcessState(Key, Data->m_byOperator, Data->m_byResult);
}

void ReceiveGateCurrentState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GATE_CURRENT_STATE Data = (LPPRECEIVE_GATE_CURRENT_STATE)ReceiveBuffer;
	int Key = ((int)(Data->m_byKeyH) << 8) + Data->m_byKeyL;
	int Index = FindCharacterIndex(Key);

	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c)
	{
		OBJECT* o = &c->Object;

		if (Data->m_byOperator)
		{
			if (Attacking != -1 && SelectedCharacter == Index)
			{
				Attacking = -1;
			}
			npcGateSwitch::GateOpen(c, o);
		}
		else
		{
			npcGateSwitch::GateClose(c, o);
		}
	}
}

void ReceiveCrownSwitchState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_SWITCH_PROC pData = (LPPRECEIVE_SWITCH_PROC)ReceiveBuffer;

	int iKey = ((int)(pData->m_byIndexH) << 8) + pData->m_byIndexL;
	int iIndex = FindCharacterIndex(iKey);

	CHARACTER* CrownSwitch = gmCharacters->GetCharacter(iIndex);

	if (CrownSwitch == NULL)
	{
		return;
	}
	if (CrownSwitch->ID == NULL)
	{
		return;
	}

	switch (pData->m_byState)
	{
	case 0:
	{
		int iSwitchIndex = ((int)(pData->m_byIndexH) << 8) + pData->m_byIndexL;
		if (iSwitchIndex == FIRST_CROWN_SWITCH_NUMBER)
		{
			Switch_Info[0].Reset();
		}
		else
		{
			Switch_Info[1].Reset();
		}

		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCrownSwitchPopLayout));
	}
	break;

	case 1:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCrownSwitchPushLayout));
		break;

	case 2:
	{
		int iKey = ((int)(pData->m_byKeyH) << 8) + pData->m_byKeyL;
		int iIndex = FindCharacterIndex(iKey);
		CHARACTER* pCha = gmCharacters->GetCharacter(iIndex);
		if (pCha)
		{
			unicode::t_char strText[256];

			SEASON3B::CProgressMsgBox* pMsgBox = NULL;
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCrownSwitchOtherPushLayout), &pMsgBox);
			if (pMsgBox)
			{
				if (pCha != NULL && pCha->ID != NULL)
				{
					unicode::_sprintf(strText, GlobalText[1486], pCha->ID);
				}
				else
				{
					unicode::_sprintf(strText, GlobalText[1487]);
				}
				pMsgBox->AddMsg(strText);

				unicode::_sprintf(strText, GlobalText[1488], CrownSwitch->ID);
				pMsgBox->AddMsg(strText);
			}
		}
	}
	break;
	}
}

int DenyCrownRegistPopupClose(POPUP_RESULT Result)
{
	if (Result & POPUP_RESULT_ESC)
		return 0;
	return 1;
}

void ReceiveCrownRegist(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CROWN_STATE pData = (LPPRECEIVE_CROWN_STATE)ReceiveBuffer;

	g_MessageBox->PopAllMessageBoxes();

	switch (pData->m_byCrownState)
	{
	case 0:
	{
		SEASON3B::CProgressMsgBox* pMsgBox = NULL;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterStartLayout), &pMsgBox);
		if (pMsgBox)
		{
			unicode::t_char strText[256];
			int iTime = (pData->m_dwCrownAccessTime / 1000);
			if (iTime >= 59)
				iTime = 59;
			unicode::_sprintf(strText, GlobalText[1980], GlobalText[1489], iTime);
			pMsgBox->AddMsg(strText);
			pMsgBox->SetElapseTime(60000 - pData->m_dwCrownAccessTime);
		}
	}
	break;

	case 1:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterSuccessLayout));
		break;

	case 2:
	{
		SEASON3B::CProgressMsgBox* pMsgBox = NULL;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterFailLayout), &pMsgBox);
		if (pMsgBox)
		{
			unicode::t_char strText[256];
			int iTime = (pData->m_dwCrownAccessTime / 1000);
			if (iTime >= 59)
				iTime = 59;
			unicode::_sprintf(strText, GlobalText[1980], GlobalText[1491], iTime);
			pMsgBox->AddMsg(strText);
		}
	}
	break;

	case 3:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterOtherLayout));
	}
	break;
	case 4:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterOtherCampLayout));
		break;

	}
}

void ReceiveCrownState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CROWN_STATE pData = (LPPRECEIVE_CROWN_STATE)ReceiveBuffer;

	g_pUIPopup->CancelPopup();
	switch (pData->m_byCrownState)
	{
	case 0:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCrownDefenseRemoveLayout));

		int Index = FindCharacterIndexByMonsterIndex(216);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

		if (pCharacter)
		{
			g_CharacterRegisterBuff((&pCharacter->Object), eBuff_CastleCrown);
		}
	}
	break;

	case 1:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCrownDefenseCreateLayout));

		int Index = FindCharacterIndexByMonsterIndex(216);

		CHARACTER* pCharacter = gmCharacters->GetCharacter(Index);

		if (pCharacter)
		{
			g_CharacterClearBuff((&pCharacter->Object));
		}
	}
	break;

	case 2:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CSealRegisterSuccessLayout));

		break;
	}
}

void ReceiveBattleCastleRegiment(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CROWN_STATE pData = (LPPRECEIVE_CROWN_STATE)ReceiveBuffer;

	switch (pData->m_byCrownState)
	{
	case 0:
		battleCastle::SettingBattleFormation(Hero, static_cast<eBuffState>(0));
		return;
	case 1:
		battleCastle::SettingBattleFormation(Hero, eBuff_CastleRegimentDefense);
		break;
	case 2:
		battleCastle::SettingBattleFormation(Hero, eBuff_CastleRegimentAttack1);
		break;
	case 3:
		battleCastle::SettingBattleFormation(Hero, eBuff_CastleRegimentAttack2);
		break;
	case 4:
		battleCastle::SettingBattleFormation(Hero, eBuff_CastleRegimentAttack3);
		break;
	}
}

void ReceiveBattleCasleSwitchInfo(BYTE* ReceiveBuffer)
{
	LPRECEIVE_CROWN_SWITCH_INFO pData = (LPRECEIVE_CROWN_SWITCH_INFO)ReceiveBuffer;
	Check_Switch(pData);
}

bool Check_Switch(PRECEIVE_CROWN_SWITCH_INFO* Data)
{
	if (Switch_Info == NULL)
	{
		Switch_Info = new CROWN_SWITCH_INFO[2];

		WORD Key = ((WORD)(Data->m_byIndex1) << 8) + Data->m_byIndex2;

		if (Key == FIRST_CROWN_SWITCH_NUMBER)
		{
			Switch_Info[0].m_bySwitchState = Data->m_bySwitchState;
			Switch_Info[0].m_JoinSide = Data->m_JoinSide;
		}
		else
		{
			Switch_Info[1].m_bySwitchState = Data->m_bySwitchState;
			Switch_Info[1].m_JoinSide = Data->m_JoinSide;
		}
	}
	else
	{
		WORD Key = ((WORD)(Data->m_byIndex1) << 8) + Data->m_byIndex2;

		if (Key == FIRST_CROWN_SWITCH_NUMBER)
		{
			Switch_Info[0].m_bySwitchState = Data->m_bySwitchState;
			Switch_Info[0].m_JoinSide = Data->m_JoinSide;
		}
		else
		{
			Switch_Info[1].m_bySwitchState = Data->m_bySwitchState;
			Switch_Info[1].m_JoinSide = Data->m_JoinSide;
		}

		char GName[8 + 1];
		memset(GName, 0, 8 + 1);
		memcpy(GName, (char*)Data->m_szGuildName, 8);

		GName[8] = NULL;

		if (Key == FIRST_CROWN_SWITCH_NUMBER)
		{
			memcpy(&Switch_Info[0].m_szGuildName, GName, 9);
		}
		else
		{
			memcpy(&Switch_Info[1].m_szGuildName, GName, 9);
		}

		char Name[MAX_ID_SIZE + 1];
		memset(Name, 0, MAX_ID_SIZE + 1);
		memcpy(Name, (char*)Data->m_szUserName, MAX_ID_SIZE);
		Name[MAX_ID_SIZE] = NULL;
		if (Key == FIRST_CROWN_SWITCH_NUMBER)
		{
			memcpy(&Switch_Info[0].m_szUserName, Name, MAX_ID_SIZE + 1);
		}
		else
		{
			memcpy(&Switch_Info[1].m_szUserName, Name, MAX_ID_SIZE + 1);
		}

	}
	return true;
}

bool Delete_Switch()
{
	if (Switch_Info != NULL)
	{
		delete[] Switch_Info;
		Switch_Info = NULL;
	}
	return true;
}

void ReceiveBattleCastleStart(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CROWN_STATE pData = (LPPRECEIVE_CROWN_STATE)ReceiveBuffer;

	bool bStartBattleCastle = (bool)(pData->m_byCrownState);

	battleCastle::SetBattleCastleStart(bStartBattleCastle);

	if (bStartBattleCastle)
	{
		g_pSiegeWarfare->InitSkillUI();
	}
	else
	{
		g_pSiegeWarfare->ReleaseSkillUI();
	}
}

void ReceiveBattleCastleProcess(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_BC_PROCESS pData = (LPPRECEIVE_BC_PROCESS)ReceiveBuffer;

	switch (pData->m_byBasttleCastleState)
	{
	case 0:
	{
		char GuildName[8 + 1];
		memset(GuildName, 0, 8 + 1);
		memcpy(GuildName, (char*)pData->m_szGuildName, 8);
		GuildName[8] = NULL;

		char Text[100];
		wsprintf(Text, GlobalText[1496], GuildName);
		CreateNotice(Text, 1);
	}
	break;

	case 1:
	{
		char Text[8 + 1];
		memset(Text, 0, 8 + 1);
		memcpy(Text, (char*)pData->m_szGuildName, 8);
		Text[8] = NULL;
		battleCastle::ChangeBattleFormation(Text, true);

		char Text2[100];
		wsprintf(Text2, GlobalText[1497], Text);
		CreateNotice(Text2, 1);
	}
	break;
	}
}

void ReceiveKillCount(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD pData = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	if (pData->Value == 0)
	{
		if (Hero->BackupCurrentSkill != 255)
		{
			Hero->CurrentSkill = Hero->BackupCurrentSkill;
		}
		Hero->BackupCurrentSkill = 255;
		Hero->GuildMasterKillCount = 0;
	}
	else
	{
		Hero->GuildMasterKillCount = pData->Value;
	}
}

void ReceiveBuildTime(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MONSTER_BUILD_TIME pData = (LPPRECEIVE_MONSTER_BUILD_TIME)ReceiveBuffer;
	int Key = ((int)(pData->m_byKeyH) << 8) + pData->m_byKeyL;
	int Index = FindCharacterIndex(Key);

	CHARACTER* c = gmCharacters->GetCharacter(Index);
	OBJECT* o = &c->Object;

	o->m_byBuildTime = pData->m_byBuildTime;
}

void ReceiveCastleGuildMark(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CASTLE_FLAG pData = (LPPRECEIVE_CASTLE_FLAG)ReceiveBuffer;

	BYTE GuildMark[64];
	for (int i = 0; i < 64; i++)
	{
		if (i % 2 == 0)
			GuildMark[i] = (pData->m_byGuildMark[i / 2] >> 4) & 0x0f;
		else
			GuildMark[i] = pData->m_byGuildMark[i / 2] & 0x0f;
	}
	CreateCastleMark(BITMAP_INTERFACE_MAP, GuildMark);
}

void ReceiveCastleHuntZoneInfo(BYTE* RecevieBuffer)
{
	LPPRECEIVE_CASTLE_HUNTZONE_INFO pData = (LPPRECEIVE_CASTLE_HUNTZONE_INFO)RecevieBuffer;

	if (pData->m_byResult == 0)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGatemanFailMsgBoxLayout));
	}
	else
	{
		g_pUIGateKeeper->SetInfo(pData->m_byResult, (bool)pData->m_byEnable, pData->m_iCurrPrice, pData->m_iUnitPrice, pData->m_iMaxPrice);
		g_pNewUISystem->Show(SEASON3B::INTERFACE_GATEKEEPER);
	}
}

void ReceiveCastleHuntZoneResult(BYTE* ReceiveBuffer)
{
	LPPRECEVIE_CASTLE_HUNTZONE_RESULT pData = (LPPRECEVIE_CASTLE_HUNTZONE_RESULT)ReceiveBuffer;

	if (pData->m_byResult == 0)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CGatemanFailMsgBoxLayout));
	}
}

void ReceiveCatapultState(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_CATAPULT_STATE pData = (LPPRECEIVE_CATAPULT_STATE)ReceiveBuffer;

	if (pData->m_byResult == 1)
	{
		int Key = ((int)(pData->m_byKeyH) << 8) + pData->m_byKeyL;

		g_pNewUISystem->Show(SEASON3B::INTERFACE_CATAPULT);
		g_pCatapultWindow->Init(Key, pData->m_byWeaponType);
	}
	else if (pData->m_byResult == 0)
	{
		g_pChatListBox->AddText("", "ReceiveCatapultState", SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

void ReceiveCatapultFire(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_WEAPON_FIRE pData = (LPPRECEIVE_WEAPON_FIRE)ReceiveBuffer;

	if (pData->m_byResult)
	{
		int Key = ((int)(pData->m_byKeyH) << 8) + pData->m_byKeyL;

		g_pCatapultWindow->DoFire(Key, pData->m_byResult, pData->m_byWeaponType, pData->m_byTargetX, pData->m_byTargetY);
	}
	else if (pData->m_byResult == 0)
	{
		g_pChatListBox->AddText("", "ReceiveCatapultFire", SEASON3B::TYPE_SYSTEM_MESSAGE);
	}
}

void ReceiveCatapultFireToMe(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_BOMBING_ALERT pData = (LPPRECEIVE_BOMBING_ALERT)ReceiveBuffer;

	g_pCatapultWindow->DoFireFixStartPosition(pData->m_byWeaponType, pData->m_byTargetX, pData->m_byTargetY);
}

void ReceivePreviewPort(BYTE* ReceiveBuffer)
{
	LPPWHEADER_DEFAULT_WORD pData = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);
	for (int i = 0; i < pData->Value; i++)
	{
		LPPRECEIVE_PREVIEW_PORT pData2 = (LPPRECEIVE_PREVIEW_PORT)(ReceiveBuffer + Offset);
		WORD Key = ((WORD)(pData2->m_byKeyH) << 8) + pData2->m_byKeyL;
		Key &= 0x7FFF;

		switch (pData2->m_byObjType)
		{
		case 1:
		{
			CHARACTER* c = CreateCharacter(Key, MODEL_PLAYER, pData2->m_byPosX, pData2->m_byPosY, 0);
			OBJECT* o = &c->Object;

			c->Class = gCharacterManager.ChangeServerClassTypeToClientClassType(pData2->m_byTypeH);
			c->Skin = 0;
			c->PK = 0;
			o->Kind = KIND_TMP;

			for (int j = 0; j < pData2->s_BuffCount; ++j)
			{
				RegisterBuff(static_cast<eBuffState>(pData2->s_BuffEffectState[j]), o);
			}

			c->PositionX = pData2->m_byPosX;
			c->PositionY = pData2->m_byPosY;
			c->TargetX = pData2->m_byPosX;
			c->TargetY = pData2->m_byPosY;
			c->Object.Angle[2] = rand() % 360;

			c->m_iDeleteTime = 150;

			ChangeCharacterExt(FindCharacterIndex(Key), pData2->m_byEquipment);

			strcpy(c->ID, "   ");
			c->ID[MAX_ID_SIZE] = NULL;
		}
		break;
		case 2:
		case 3:    //   NPC
		{
			WORD Type = ((WORD)(pData2->m_byTypeH) << 8) + pData2->m_byTypeL;
			CHARACTER* c = CreateMonster(Type, pData2->m_byPosX, pData2->m_byPosY, Key);
			if (c == NULL) break;
			OBJECT* o = &c->Object;

			for (int j = 0; j < pData2->s_BuffCount; ++j)
			{
				RegisterBuff(static_cast<eBuffState>(pData2->s_BuffEffectState[j]), o);
			}

			c->PositionX = pData2->m_byPosX;
			c->PositionY = pData2->m_byPosY;
			c->TargetX = pData2->m_byPosX;
			c->TargetY = pData2->m_byPosY;
			c->Object.Angle[2] = rand() % 360;

			c->m_iDeleteTime = 150;

			if (gMapManager->InBattleCastle() && c->MonsterIndex == 277)
			{
				//  State					
				if (g_isCharacterBuff((&c->Object), eBuff_CastleGateIsOpen))
				{
					SetAction(&c->Object, 1);
					battleCastle::SetCastleGate_Attribute(pData2->m_byPosX, pData2->m_byPosY, 0);
				}
				else if (g_isNotCharacterBuff((&c->Object)))
				{
					SetAction(&c->Object, 0);
					battleCastle::SetCastleGate_Attribute(pData2->m_byPosX, pData2->m_byPosY, 1);
				}
			}
		}
		break;
		}

		Offset += (sizeof(PRECEIVE_PREVIEW_PORT) - (sizeof(BYTE) * (MAX_BUFF_SLOT_INDEX - pData2->s_BuffCount)));
	}
}

void ReceiveMapInfoResult(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MAP_INFO_RESULT pData = (LPPRECEIVE_MAP_INFO_RESULT)ReceiveBuffer;
}

void ReceiveGuildCommand(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_GUILD_COMMAND  pData = (LPPRECEIVE_GUILD_COMMAND)ReceiveBuffer;
	GuildCommander            GCmd = { pData->m_byTeam, pData->m_byX, pData->m_byY, pData->m_byCmd };

	if (g_pSiegeWarfare)
	{
		g_pSiegeWarfare->SetMapInfo(GCmd);
	}
}

void ReceiveGuildMemberLocation(BYTE* ReceiveBuffer)
{
	if (g_pSiegeWarfare->GetCurSiegeWarType() != TYPE_GUILD_COMMANDER)
		return;

	g_pSiegeWarfare->ClearGuildMemberLocation();

	LPPWHEADER_DEFAULT_WORD2 pData = (LPPWHEADER_DEFAULT_WORD2)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD2);

	for (int i = 0; i < pData->Value; i++)
	{
		LPPRECEIVE_MEMBER_LOCATION pData2 = (LPPRECEIVE_MEMBER_LOCATION)(ReceiveBuffer + Offset);

		g_pSiegeWarfare->SetGuildMemberLocation(0, pData2->m_byX, pData2->m_byY);

		Offset += sizeof(PRECEIVE_MEMBER_LOCATION);
	}
}

void ReceiveGuildNpcLocation(BYTE* ReceiveBuffer)
{
	if (g_pSiegeWarfare->GetCurSiegeWarType() != TYPE_GUILD_COMMANDER)
		return;

	LPPWHEADER_DEFAULT_WORD pData = (LPPWHEADER_DEFAULT_WORD)ReceiveBuffer;
	int Offset = sizeof(PWHEADER_DEFAULT_WORD);

	for (int i = 0; i < pData->Value; i++)
	{
		LPPRECEIVE_NPC_LOCATION pData2 = (LPPRECEIVE_NPC_LOCATION)(ReceiveBuffer + Offset);
		g_pSiegeWarfare->SetGuildMemberLocation(pData2->m_byType + 1, pData2->m_byX, pData2->m_byY);

		Offset += sizeof(PRECEIVE_NPC_LOCATION);
	}
}

void ReceiveMatchTimer(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_MATCH_TIMER pData = (LPPRECEIVE_MATCH_TIMER)ReceiveBuffer;

	g_pSiegeWarfare->SetTime(pData->m_byHour, pData->m_byMinute);
}

void ReceiveCrywolfInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_INFO pData = (LPPMSG_ANS_CRYWOLF_INFO)ReceiveBuffer;

	M34CryWolf1st::CheckCryWolf1stMVP(pData->btOccupationState, pData->btCrywolfState);
	g_Direction.m_CMVP.GetCryWolfState(pData->btCrywolfState);

}

void ReceiveCrywolStateAltarfInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_STATE_ALTAR_INFO pData = (LPPMSG_ANS_CRYWOLF_STATE_ALTAR_INFO)ReceiveBuffer;

	M34CryWolf1st::CheckCryWolf1stMVPAltarfInfo(pData->iCrywolfStatueHP,
		pData->btAltarState1,
		pData->btAltarState2,
		pData->btAltarState3,
		pData->btAltarState4,
		pData->btAltarState5);
}

void ReceiveCrywolfAltarContract(BYTE* ReceiveBuffer)
{
	LPPPMSG_ANS_CRYWOLF_ALTAR_CONTRACT pData = (LPPPMSG_ANS_CRYWOLF_ALTAR_CONTRACT)ReceiveBuffer;

	int Key = ((int)(pData->btObjIndexH) << 8) + pData->btObjIndexL;

	if (pData->bResult == 0)
	{
		g_CharacterUnRegisterBuff((&Hero->Object), eBuff_CrywolfHeroContracted);

		int level = CharacterAttribute->Level;
		if (level < 260)
		{
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Dont_Set_Temple1));
			//			M34CryWolf1st::Set_Message_Box(54,0,0);
			//			M34CryWolf1st::Set_Message_Box(55,1,0);
		}
		else
		{
			//			M34CryWolf1st::Set_Message_Box(58,0,0);
			SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Wat_Set_Temple1));
		}
	}
	else if (pData->bResult == 1)
	{
		//		M34CryWolf1st::Set_Message_Box(3,0,0);
		//		M34CryWolf1st::Set_Message_Box(4,1,0);
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Set_Temple));

		M34CryWolf1st::Check_AltarState(Key - 316, pData->btAltarState);

		g_CharacterRegisterBuff((&Hero->Object), eBuff_CrywolfHeroContracted);

		SetAction(&Hero->Object, PLAYER_HEALING_FEMALE1);

		SendRequestAction(AT_HEALING1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
}

void ReceiveCrywolfLifeTime(BYTE* ReceiveBuffer)
{
	LPPPMSG_ANS_CRYWOLF_LEFTTIME pData = (LPPPMSG_ANS_CRYWOLF_LEFTTIME)ReceiveBuffer;

	M34CryWolf1st::SetTime(pData->btHour, pData->btMinute);
	g_pCryWolfInterface->SetTime((int)(pData->btHour), (int)(pData->btMinute));
}

void ReceiveCrywolfTankerHit(BYTE* ReceiveBuffer)
{
	LPPMSG_NOTIFY_REGION_MONSTER_ATTACK pData = (LPPMSG_NOTIFY_REGION_MONSTER_ATTACK)ReceiveBuffer;

	M34CryWolf1st::DoTankerFireFixStartPosition(pData->btSourceX, pData->btSourceY, pData->btPointX, pData->btPointY);
}

void ReceiveCrywolfBenefitPlusChaosRate(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE pData = (LPPMSG_ANS_CRYWOLF_BENEFIT_PLUS_CHAOSRATE)ReceiveBuffer;

	g_MixRecipeMgr.SetPlusChaosRate(pData->btPlusChaosRate);
}

void ReceiveCrywolfBossMonsterInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_BOSSMONSTER_INFO pData = (LPPMSG_ANS_CRYWOLF_BOSSMONSTER_INFO)ReceiveBuffer;

	M34CryWolf1st::Set_BossMonster(pData->btBossHP, pData->btMonster2);
}

void ReceiveCrywolfPersonalRank(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_PERSONAL_RANK pData = (LPPMSG_ANS_CRYWOLF_PERSONAL_RANK)ReceiveBuffer;

	M34CryWolf1st::Set_MyRank(pData->btRank, pData->iGettingExp);
}

void ReceiveCrywolfHeroList(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_CRYWOLF_HERO_LIST_INFO_COUNT pData = (LPPMSG_ANS_CRYWOLF_HERO_LIST_INFO_COUNT)ReceiveBuffer;
	int Offset = sizeof(PMSG_ANS_CRYWOLF_HERO_LIST_INFO_COUNT);
	for (int i = 0; i < pData->btCount; i++)
	{
		LPPMSG_ANS_CRYWOLF_HERO_LIST_INFO pData2 = (LPPMSG_ANS_CRYWOLF_HERO_LIST_INFO)(ReceiveBuffer + Offset);
		Offset += sizeof(PMSG_ANS_CRYWOLF_HERO_LIST_INFO);
		M34CryWolf1st::Set_WorldRank(pData2->iRank, pData2->btHeroClass, pData2->iHeroScore, pData2->szHeroName);
	}
}

void ReceiveKanturu3rdStateInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_KANTURU_STATE_INFO pData = (LPPMSG_ANS_KANTURU_STATE_INFO)ReceiveBuffer;

	g_pKanturu2ndEnterNpc->ReceiveKanturu3rdInfo(pData->btState, pData->btDetailState, pData->btEnter, pData->btUserCount, pData->iRemainTime);
}

void ReceiveKanturu3rdEnterBossMap(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_ENTER_KANTURU_BOSS_MAP pData = (LPPMSG_ANS_ENTER_KANTURU_BOSS_MAP)ReceiveBuffer;
	g_pKanturu2ndEnterNpc->ReceiveKanturu3rdEnter(pData->btResult);
}

void ReceiveKanturu3rdCurrentState(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_KANTURU_CURRENT_STATE pData = (LPPMSG_ANS_KANTURU_CURRENT_STATE)ReceiveBuffer;
	M39Kanturu3rd::Kanturu3rdState(pData->btCurrentState, pData->btCurrentDetailState);
	M39Kanturu3rd::CheckSuccessBattle(pData->btCurrentState, pData->btCurrentDetailState);
}

void ReceiveKanturu3rdState(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_KANTURU_STATE_CHANGE pData = (LPPMSG_ANS_KANTURU_STATE_CHANGE)ReceiveBuffer;

	if (M39Kanturu3rd::IsInKanturu3rd())
	{
		if ((pData->btState == KANTURU_STATE_MAYA_BATTLE
			&& (pData->btDetailState == KANTURU_MAYA_DIRECTION_MONSTER1
				|| pData->btDetailState == KANTURU_MAYA_DIRECTION_MAYA1
				|| pData->btDetailState == KANTURU_MAYA_DIRECTION_MONSTER2
				|| pData->btDetailState == KANTURU_MAYA_DIRECTION_MAYA2
				|| pData->btDetailState == KANTURU_MAYA_DIRECTION_MONSTER3
				|| pData->btDetailState == KANTURU_MAYA_DIRECTION_MAYA3
				))
			|| (pData->btState == KANTURU_STATE_NIGHTMARE_BATTLE
				&& (pData->btDetailState == KANTURU_NIGHTMARE_DIRECTION_BATTLE
					))
			)
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_KANTURU_INFO) == false)
			{
				g_pNewUISystem->Show(SEASON3B::INTERFACE_KANTURU_INFO);
			}
		}
		else
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_KANTURU_INFO) == true)
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_KANTURU_INFO);
			}
		}
		M39Kanturu3rd::Kanturu3rdState(pData->btState, pData->btDetailState);
		M39Kanturu3rd::CheckSuccessBattle(pData->btState, pData->btDetailState);
	}
}

void ReceiveKanturu3rdResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_KANTURU_BATTLE_RESULT pData = (LPPMSG_ANS_KANTURU_BATTLE_RESULT)ReceiveBuffer;
	M39Kanturu3rd::Kanturu3rdResult(pData->btResult);
}

void ReceiveKanturu3rdTimer(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_KANTURU_BATTLE_SCENE_TIMELIMIT pData = (LPPMSG_ANS_KANTURU_BATTLE_SCENE_TIMELIMIT)ReceiveBuffer;
	g_pKanturuInfoWindow->SetTime(pData->btTimeLimit);
}

void RecevieKanturu3rdMayaSKill(BYTE* ReceiveBuffer)
{
	LPPMSG_NOTIFY_KANTURU_WIDE_AREA_ATTACK pData = (LPPMSG_NOTIFY_KANTURU_WIDE_AREA_ATTACK)ReceiveBuffer;
	M39Kanturu3rd::MayaSceneMayaAction(pData->btType);
}

void RecevieKanturu3rdLeftUserandMonsterCount(BYTE* ReceiveBuffer)
{
	LPPMSG_NOTIFY_KANTURU_USER_MONSTER_COUNT pData = (LPPMSG_NOTIFY_KANTURU_USER_MONSTER_COUNT)ReceiveBuffer;
	M39Kanturu3rd::Kanturu3rdUserandMonsterCount(pData->bMonsterCount, pData->btUserCount);
}

void ReceiveCursedTempleEnterInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_CURSED_TEMPLE_USER_COUNT data = (LPPMSG_CURSED_TEMPLE_USER_COUNT)ReceiveBuffer;

	g_pCursedTempleEnterWindow->ReceiveCursedTempleEnterInfo(ReceiveBuffer);
}

void ReceiveCursedTempleEnterResult(BYTE* ReceiveBuffer)
{
	LPPMSG_RESULT_ENTER_CURSED_TEMPLE data = (LPPMSG_RESULT_ENTER_CURSED_TEMPLE)ReceiveBuffer;

	if (data->Result == 0)
	{
		g_pNewUISystem->HideAll();
	}
	g_CursedTemple->UpdateTempleSystemMsg(data->Result);
}

void ReceiveCursedTempleInfo(BYTE* ReceiveBuffer)
{
	g_pCursedTempleWindow->ReceiveCursedTempleInfo(ReceiveBuffer);
	g_CursedTemple->ReceiveCursedTempleInfo(ReceiveBuffer);
}

void ReceiveCursedTempMagicResult(BYTE* ReceiveBuffer)
{
	g_pCursedTempleWindow->ReceiveCursedTempRegisterSkill(ReceiveBuffer);
}

void ReceiveCursedTempSkillEnd(BYTE* ReceiveBuffer)
{
	g_pCursedTempleWindow->ReceiveCursedTempUnRegisterSkill(ReceiveBuffer);
}

void ReceiveCursedTempSkillPoint(BYTE* ReceiveBuffer)
{
	g_pCursedTempleWindow->ReceiveCursedTempSkillPoint(ReceiveBuffer);
}

void ReceiveCursedTempleHolyItemRelics(BYTE* ReceiveBuffer)
{
	g_pCursedTempleWindow->ReceiveCursedTempleHolyItemRelics(ReceiveBuffer);
}

void ReceiveCursedTempleGameResult(BYTE* ReceiveBuffer)
{
	g_pNewUISystem->HideAll();

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM))
	{
		g_pCursedTempleResultWindow->ResetGameResultInfo();
		g_pCursedTempleResultWindow->SetMyTeam(g_pCursedTempleWindow->GetMyTeam());

		g_pNewUISystem->Hide(SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM);
	}

	PlayBuffer(SOUND_CURSEDTEMPLE_GAMESYSTEM5);

	g_pNewUISystem->Show(SEASON3B::INTERFACE_CURSEDTEMPLE_RESULT);

	g_pCursedTempleResultWindow->ReceiveCursedTempleGameResult(ReceiveBuffer);
}

void ReceiveCursedTempleState(BYTE* ReceiveBuffer)
{
	LPPMSG_ILLUSION_TEMPLE_STATE data = (LPPMSG_ILLUSION_TEMPLE_STATE)ReceiveBuffer;

	// 0: wait, 1: wait->ready, 2: ready->play, 3: play->end
	SEASON3A::eCursedTempleState cursedtemple = static_cast<SEASON3A::eCursedTempleState>(data->btIllusionTempleState + 1);

	if (cursedtemple == SEASON3A::eCursedTempleState_Ready)
	{
		g_pNewUISystem->HideAll();

		g_pCursedTempleWindow->ResetCursedTempleSystemInfo();
		g_pCursedTempleWindow->StartTutorialStep();

		PlayBuffer(SOUND_CURSEDTEMPLE_GAMESYSTEM1);

		g_pNewUISystem->Show(SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM);
	}

	g_CursedTemple->ReceiveCursedTempleState(cursedtemple);
}

void ReceiveRaklionStateInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_RAKLION_STATE_INFO pData = (LPPMSG_ANS_RAKLION_STATE_INFO)ReceiveBuffer;
	g_Raklion.SetState(pData->btState, pData->btDetailState);
}

void ReceiveRaklionCurrentState(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_RAKLION_CURRENT_STATE pData = (LPPMSG_ANS_RAKLION_CURRENT_STATE)ReceiveBuffer;
	g_Raklion.SetState(pData->btCurrentState, pData->btCurrentDetailState);
}

void RecevieRaklionStateChange(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_RAKLION_STATE_CHANGE pData = (LPPMSG_ANS_RAKLION_STATE_CHANGE)ReceiveBuffer;
	g_Raklion.SetState(pData->btState, pData->btDetailState);
}

void RecevieRaklionBattleResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_RAKLION_BATTLE_RESULT pData = (LPPMSG_ANS_RAKLION_BATTLE_RESULT)ReceiveBuffer;
}

void RecevieRaklionWideAreaAttack(BYTE* ReceiveBuffer)
{
	LPPMSG_NOTIFY_RAKLION_WIDE_AREA_ATTACK pData = (LPPMSG_NOTIFY_RAKLION_WIDE_AREA_ATTACK)ReceiveBuffer;
}

void RecevieRaklionUserMonsterCount(BYTE* ReceiveBuffer)
{
	LPPMSG_NOTIFY_RAKLION_USER_MONSTER_COUNT pData = (LPPMSG_NOTIFY_RAKLION_USER_MONSTER_COUNT)ReceiveBuffer;
}

void ReceiveCheckSumRequest(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_WORD Data = (LPPHEADER_DEFAULT_WORD)ReceiveBuffer;
	DWORD dwCheckSum = GetCheckSum(Data->Value);
	SendCheckSum(dwCheckSum);

	g_ConsoleDebug->Write(MCD_RECEIVE, "0x03 [ReceiveCheckSumRequest]");
}

void Action(CHARACTER* c, OBJECT* o, bool Now);


void TranslateChattingProtocol(DWORD dwWindowUIID, int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEcrypted);

void DataSend(BYTE* byData, DWORD size) // OK
{
	//CStreamPacketEngine spe;

	//spe.Init(byData[0], byData[1]);

	//if (byData[0] == 0xC1 || byData[0] == 0xC3)
	//	spe.AddData(&byData[3], (size - 3), FALSE);
	//else
	//	spe.AddData(&byData[4], (size - 4), FALSE);

	//spe.Send((byData[0] == 0xC3 || byData[0] == 0xC4));

	//SendPacket_lock->lock();
	CStreamPacketEngine spe;
	if ((BYTE)byData[0] == 0xC2 || (BYTE)byData[0] == 0xC4)
	{
		spe.Init(byData[0], byData[3]);
		for (int n = 4; n < size; n++)
		{
			spe << byData[n];
		}
	}
	else
	{
		spe.Init(byData[0], byData[2]);
		//spe.AddNullData(1);
		for (int n = 3; n < size; n++)
		{
			spe << byData[n];
		}
	}
	spe.Send();
}

void ProtocolCompiler(CWsctlc* pSocketClient, int iTranslation, int iParam)
{
	//if(CurrentProtocolState >= RECEIVE_JOIN_MAP_SERVER)
	//	return;
	int Size = 0;
	int HeadCode = 0;

	while (1)
	{
		BYTE* ReceiveBuffer = pSocketClient->GetReadMsg();

		if (ReceiveBuffer == NULL)
		{
			break;
		}
		else
		{
			BOOL bEncrypted = FALSE;
			BYTE byDec[MAX_SPE_BUFFERSIZE_];
			if (ReceiveBuffer[0] == 0xC1)
			{
				LPPBMSG_HEADER lpHeader = (LPPBMSG_HEADER)ReceiveBuffer;
				HeadCode = lpHeader->HeadCode;
				Size = lpHeader->Size;
			}
			else if (ReceiveBuffer[0] == 0xC2)
			{
				LPPWMSG_HEADER lpHeader = (LPPWMSG_HEADER)ReceiveBuffer;
				HeadCode = lpHeader->HeadCode;
				Size = ((int)(lpHeader->SizeH) << 8) + lpHeader->SizeL;
			}
			else if (ReceiveBuffer[0] == 0xC3 || ReceiveBuffer[0] == 0xC4)
			{
				int iSize;
				if (ReceiveBuffer[0] == 0xC3)
				{
					LPPBMSG_ENCRYPTED lpHeader = (LPPBMSG_ENCRYPTED)ReceiveBuffer;
					Size = lpHeader->Size;
					iSize = g_SimpleModulusSC.Decrypt(byDec + 2, ReceiveBuffer + 2, Size - 2);
				}
				else
				{
					LPWBMSG_ENCRYPTED lpHeader = (LPWBMSG_ENCRYPTED)ReceiveBuffer;
					Size = ((int)(lpHeader->SizeH) << 8) + lpHeader->SizeL;
					iSize = g_SimpleModulusSC.Decrypt(byDec + 2, ReceiveBuffer + 3, Size - 3);
				}
				bEncrypted = TRUE;

				if (iSize < 0)
				{
					SendHackingChecked(0x06, 0);
					g_byPacketSerialRecv++;
					continue;
				}

				if ((g_byPacketSerialRecv) != byDec[2])
				{
					bEncrypted = FALSE;
					g_byPacketSerialRecv = byDec[2];

					g_ErrorReport.Write("Decrypt error : g_byPacketSerialRecv(0x%02X), byDec(0x%02X)\r\n", g_byPacketSerialRecv, byDec[2]);
					g_ErrorReport.Write("Dump : \r\n");
					g_ErrorReport.HexWrite(ReceiveBuffer, Size);
					g_ErrorReport.Write("\r\n");
					g_ErrorReport.HexWrite(byDec + 2, iSize);
				}
				else
				{
					g_byPacketSerialRecv++;
				}

				if (ReceiveBuffer[0] == 0xC3)
				{
					LPPBMSG_HEADER pHeader = (LPPBMSG_HEADER) & (byDec[1]);
					pHeader->Code = 0xC1;
					pHeader->Size = (BYTE)iSize;
					HeadCode = pHeader->HeadCode;
					ReceiveBuffer = (BYTE*)pHeader;
				}
				else
				{
					LPPWMSG_HEADER pHeader = (LPPWMSG_HEADER)byDec;
					pHeader->Code = 0xC2;
					pHeader->SizeH = (BYTE)(iSize / 256);
					pHeader->SizeL = (BYTE)(iSize % 256);
					HeadCode = pHeader->HeadCode;
					ReceiveBuffer = (BYTE*)pHeader;
				}
				Size = iSize;
			}
			TotalPacketSize += Size;
#ifdef SAVE_PACKET
			SOCKET socket = pSocketClient->GetSocket();

			SOCKADDR_IN sockaddr;
			int isockaddrlen = sizeof(sockaddr);
			getpeername(socket, (SOCKADDR*)&sockaddr, &isockaddrlen);

			std::string timeString;
			leaf::GetTimeString(timeString);
			DebugAngel_Write(PACKET_SAVE_FILE, "%s [%d.%d.%d.%d] Receive ", timeString.c_str(),
				(UCHAR)sockaddr.sin_addr.S_un.S_un_b.s_b1, (UCHAR)sockaddr.sin_addr.S_un.S_un_b.s_b2,
				(UCHAR)sockaddr.sin_addr.S_un.S_un_b.s_b3, (UCHAR)sockaddr.sin_addr.S_un.S_un_b.s_b4);
			for (int i = 0; i < 6; i++) {
				DebugAngel_Write(PACKET_SAVE_FILE, "%02X", ReceiveBuffer[i]);
			}
			DebugAngel_Write(PACKET_SAVE_FILE, "\r\n");
#endif

			switch (iTranslation)
			{
			case 1:
				TranslateChattingProtocol(iParam, HeadCode, ReceiveBuffer, Size, bEncrypted);
				break;
			default:
				if (!TranslateProtocol(HeadCode, ReceiveBuffer, Size, bEncrypted))
				{
					g_ErrorReport.Write("Strange packet\r\n");
					g_ErrorReport.HexWrite(ReceiveBuffer, Size);
				}
				break;
			}
		}
	}
}

bool ReceiveRegistedLuckyCoin(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_GET_COIN_COUNT _pData = (LPPMSG_ANS_GET_COIN_COUNT)ReceiveBuffer;

	if (_pData->nCoinCnt >= 0)
	{
		g_pLuckyCoinRegistration->SetRegistCount(_pData->nCoinCnt);
		return true;
	}
	return false;
}

bool ReceiveRegistLuckyCoin(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_REGEIST_COIN _pData = (LPPMSG_ANS_REGEIST_COIN)ReceiveBuffer;

	g_pLuckyCoinRegistration->UnLockLuckyCoinRegBtn();

	switch (_pData->btResult)
	{
	case 0:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CUseRegistLuckyCoinMsgBoxLayout));
		break;
	case 1:
		g_pLuckyCoinRegistration->SetRegistCount(_pData->nCurCoinCnt);
		break;
	case 100:
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CRegistOverLuckyCoinMsgBoxLayout));
		break;
	default:
		return false;
	}

	return true;
}

bool ReceiveRequestExChangeLuckyCoin(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_TREADE_COIN _pData = (LPPMSG_ANS_TREADE_COIN)ReceiveBuffer;

	g_pExchangeLuckyCoinWindow->UnLockExchangeBtn();

	switch (_pData->btResult)
	{
	case 0:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CExchangeLuckyCoinMsgBoxLayout));
	}
	break;
	case 1:
	{
		//g_pNewUISystem->Hide(SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN);
		g_pChatListBox->AddText("", GlobalText[1888], SEASON3B::TYPE_SYSTEM_MESSAGE); //Trao đổi đã được hoàn thành
	}break;
	case 2:
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CExchangeLuckyCoinInvenErrMsgBoxLayout));
	}break;
	default:
		return false;
	}

	return true;
}

bool ReceiveEnterDoppelGangerEvent(BYTE* ReceiveBuffer)
{
	LPPMSG_RESULT_ENTER_DOPPELGANGER Data = (LPPMSG_RESULT_ENTER_DOPPELGANGER)ReceiveBuffer;

	unicode::t_char szText[256] = { 0, };

	switch (Data->btResult)
	{
	case 0:
		break;
	case 1:
		g_pDoppelGangerWindow->LockEnterButton(TRUE);
		break;
	case 2:
		unicode::_sprintf(szText, GlobalText[2864]);
		g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
		g_pDoppelGangerWindow->LockEnterButton(TRUE);
		break;
	case 3:
		unicode::_sprintf(szText, GlobalText[2865]);
		g_pChatListBox->AddText("", szText, SEASON3B::TYPE_ERROR_MESSAGE);
		g_pDoppelGangerWindow->LockEnterButton(TRUE);
		break;
	case 4:
		g_pDoppelGangerWindow->LockEnterButton(FALSE);
		break;
	default:
		return false;
	}

	return true;
}

bool ReceiveDoppelGangerMonsterPosition(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_MONSTER_POSITION Data = (LPPMSG_DOPPELGANGER_MONSTER_POSITION)ReceiveBuffer;
	g_pDoppelGangerFrame->SetMonsterGauge((float)Data->btPosIndex / 22.0f);
	return true;
}

bool ReceiveDoppelGangerState(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_STATE Data = (LPPMSG_DOPPELGANGER_STATE)ReceiveBuffer;

	switch (Data->btDoppelgangerState)
	{
	case 0:
		break;
	case 1:		// wait->ready
		break;
	case 2:		// ready->play
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_DOPPELGANGER_FRAME);

		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDoppelGangerMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2763], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(" ");
		pMsgBox->AddMsg(GlobalText[2764], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(" ");
		pMsgBox->AddMsg(GlobalText[2765], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}
	break;
	case 3:		// play->end
		break;
	}

	return true;
}

bool ReceiveDoppelGangerIcewalkerState(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_ICEWORKER_STATE Data = (LPPMSG_DOPPELGANGER_ICEWORKER_STATE)ReceiveBuffer;

	switch (Data->btIceworkerState)
	{
	case 0:
		g_pDoppelGangerFrame->SetIceWalkerMap(TRUE, (float)(22 - Data->btPosIndex) / 22.0f);
		break;
	case 1:
		g_pDoppelGangerFrame->SetIceWalkerMap(FALSE, 0);
		break;
	}

	return true;
}

bool ReceiveDoppelGangerTimePartyState(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_PLAY_INFO Data = (LPPMSG_DOPPELGANGER_PLAY_INFO)ReceiveBuffer;

	g_pDoppelGangerFrame->SetRemainTime(Data->wRemainSec);
	g_pDoppelGangerFrame->SetPartyMemberRcvd();
	LPPMSG_DOPPELGANGER_USER_POS pUserPos = (LPPMSG_DOPPELGANGER_USER_POS)&Data->UserPosData;
	for (int i = 0; i < Data->btUserCount; ++i)
	{
		g_pDoppelGangerFrame->SetPartyMemberInfo(pUserPos[i].wUserIndex, (float)(22 - pUserPos[i].btPosIndex) / 22.0f);
	}

	return true;
}

bool ReceiveDoppelGangerResult(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_RESULT Data = (LPPMSG_DOPPELGANGER_RESULT)ReceiveBuffer;

	PlayBuffer(SOUND_CHAOS_END);

	g_pDoppelGangerFrame->StopTimer(TRUE);
	g_pDoppelGangerFrame->EnabledDoppelGangerEvent(FALSE);

	switch (Data->btResult)
	{
	case 0:
	{
		g_pDoppelGangerFrame->SetRemainTime(0);

		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDoppelGangerMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2769], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(" ");
		pMsgBox->AddMsg(GlobalText[2770], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		// 			pMsgBox->AddMsg(" ");
		// 			pMsgBox->AddMsg(" ");
		// 			char szText[256] = { 0, };
		// 			sprintf(szText, GlobalText[2771], Data->dwRewardExp);
		// 			pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_BOLD);
	}
	break;
	case 1:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDoppelGangerMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2766], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}
	break;
	case 2:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CDoppelGangerMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2767], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(" ");
		pMsgBox->AddMsg(GlobalText[2768], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}
	break;
	}

	return true;
}

bool ReceiveDoppelGangerMonsterGoal(BYTE* ReceiveBuffer)
{
	LPPMSG_DOPPELGANGER_MONSTER_GOAL Data = (LPPMSG_DOPPELGANGER_MONSTER_GOAL)ReceiveBuffer;

	g_pDoppelGangerFrame->SetMaxMonsters(Data->btMaxGoalCnt);
	g_pDoppelGangerFrame->SetEnteredMonsters(Data->btGoalCnt);

	return true;
}

bool ReceiveMoveMapChecksum(BYTE* ReceiveBuffer)
{
	LPPMSG_MAPMOVE_CHECKSUM Data = (LPPMSG_MAPMOVE_CHECKSUM)ReceiveBuffer;

	g_pMoveCommandWindow->SetMoveCommandKey(Data->dwKeyValue);

	return true;
}

bool ReceiveRequestMoveMap(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_MAPMOVE Data = (LPPMSG_ANS_MAPMOVE)ReceiveBuffer;

	if (Data->btResult == 0)
	{

	}
	else
	{

	}

	// 	MAPMOVE_SUCCESS	
	// 	MAPMOVE_FAILED,	
	// 	MAPMOVE_FAILED_TELEPORT,
	// 	MAPMOVE_FAILED_PSHOP_OPEN,
	// 	MAPMOVE_FAILED_RECALLED,
	// 	MAPMOVE_FAILED_NOT_ENOUGH_EQUIP,
	// 	MAPMOVE_FAILED_WEARING_EQUIP,
	// 	MAPMOVE_FAILED_MURDERER,	
	// 	MAPMOVE_FAILED_NOT_ENOUGH_LEVEL,
	// 	MAPMOVE_FAILED_NOT_ENOUGH_ZEN,	
	// 	MAPMOVE_FORCED_EVENT_END		= 20,
	// 	MAPMOVE_FORCED_GM			

	return true;
}

bool ReceiveEnterEmpireGuardianEvent(BYTE* ReceiveBuffer)
{
	LPPMSG_RESULT_ENTER_EMPIREGUARDIAN Data = (LPPMSG_RESULT_ENTER_EMPIREGUARDIAN)ReceiveBuffer;

	switch (Data->Result)
	{
	case 0:
	{
		g_pEmpireGuardianTimer->SetDay((int)Data->Day);
		g_pEmpireGuardianTimer->SetZone((int)Data->Zone);
		g_pEmpireGuardianTimer->SetRemainTime(Data->RemainTick);

		g_EmpireGuardian1.SetWeather((int)Data->Wheather);
	}break;
	case 1:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2798], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(" ");
		unicode::t_char szText[256] = { NULL, };
		sprintf(szText, GlobalText[2799], (Data->RemainTick / 60000));
		pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 2:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2839], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 3:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2841], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 4:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2842], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 5:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2843], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;

	default:
		return false;
	}

	return true;
}

bool ReceiveRemainTickEmpireGuardian(BYTE* ReceiveBuffer)
{
	LPPMSG_REMAINTICK_EMPIREGUARDIAN Data = (LPPMSG_REMAINTICK_EMPIREGUARDIAN)ReceiveBuffer;

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER) == false)
	{
		g_pNewUISystem->Show(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER);
	}

	g_pEmpireGuardianTimer->SetType((int)Data->Type);
	g_pEmpireGuardianTimer->SetRemainTime((int)Data->RemainTick);
	g_pEmpireGuardianTimer->SetMonsterCount((int)Data->MonsterCount);

	return true;
}

bool ReceiveResultEmpireGuardian(BYTE* ReceiveBuffer)
{
	LPPMSG_CLEAR_RESULT_EMPIREGUARDIAN Data = (LPPMSG_CLEAR_RESULT_EMPIREGUARDIAN)ReceiveBuffer;

	switch (Data->Result)
	{
	case 0:
	{
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		pMsgBox->AddMsg(GlobalText[2803], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(GlobalText[2804], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 1:
	{
		int day = g_pEmpireGuardianTimer->GetDay();
		int zone = g_pEmpireGuardianTimer->GetZone();
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		unicode::t_char szText[256] = { NULL, };
		sprintf(szText, GlobalText[2801], day);
		pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		sprintf(szText, "%d%s", zone, GlobalText[2840]);
		pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	case 2:
	{
		int day = g_pEmpireGuardianTimer->GetDay();
		SEASON3B::CNewUICommonMessageBox* pMsgBox;
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CEmpireGuardianMsgBoxLayout), &pMsgBox);
		unicode::t_char szText[256] = { NULL, };
		sprintf(szText, GlobalText[2801], day);
		pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		pMsgBox->AddMsg(GlobalText[2802], RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
		sprintf(szText, GlobalText[861], Data->Exp);
		pMsgBox->AddMsg(szText, RGBA(255, 255, 255, 255), SEASON3B::MSGBOX_FONT_NORMAL);
	}break;
	}

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER) == true)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER);
	}

	return true;
}

#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM

// (0xD2)(0x01)
bool ReceiveIGS_CashPoint(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_CASHPOINT_ANS Data = (LPPMSG_CASHSHOP_CASHPOINT_ANS)pReceiveBuffer;
	g_InGameShopSystem->SetTotalCash((double)Data->dTotalCash);
	g_InGameShopSystem->SetTotalPoint((double)Data->dTotalPoint);
	g_InGameShopSystem->SetCashCreditCard((double)Data->dCashCredit);
	g_InGameShopSystem->SetCashPrepaid((double)Data->dCashPrepaid);
	g_InGameShopSystem->SetTotalMileage((double)Data->dTotalMileage);
	return true;
}

// (0xD2)(0x02)
bool ReceiveIGS_ShopOpenResult(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_SHOPOPEN_ANS Data = (LPPMSG_CASHSHOP_SHOPOPEN_ANS)pReceiveBuffer;

	g_InGameShopSystem->SetIsRequestShopOpenning(false);

	if ((BYTE)Data->byShopOpenResult == 0)
	{
		return false;
	}

	SendRequestIGS_CashPointInfo();
	char szCode = g_pInGameShop->GetCurrentStorageCode();
	SendRequestIGS_ItemStorageList(1, &szCode);

	g_pNewUISystem->Show(SEASON3B::INTERFACE_INGAMESHOP);

	return true;
}

// (0xD2)(0x03)
bool ReceiveIGS_BuyItem(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_BUYITEM_ANS Data = (LPPMSG_CASHSHOP_BUYITEM_ANS)pReceiveBuffer;

	switch ((BYTE)Data->byResultCode)
	{
	case -2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2953]);
	}
	break;
	case -1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2954]);
	}
	break;
	case 0:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2900], GlobalText[2901]);

		SendRequestIGS_CashPointInfo();

		char szCode = g_pInGameShop->GetCurrentStorageCode();
		SendRequestIGS_ItemStorageList(1, &szCode);
	}
	break;
	case 1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2903]);
	}
	break;
	case 2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2904]);
	}
	break;
	case 3:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2956]);
	}
	break;
	case 4:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2957]);
	}
	break;
	case 5:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[2958]);
	}
	break;
	case 6:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[3052]);
	}break;
	case 7:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[3053]);
	}break;
	case 8:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[3054]);
	}break;
	case 9:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2902], GlobalText[3264]);
	}
	break;
	default:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2945], GlobalText[890]);
	}
	break;
	}

	return true;
}

//----------------------------------------------------------------------------
// (0xD2)(0x04)
bool ReceiveIGS_SendItemGift(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_GIFTSEND_ANS Data = (LPPMSG_CASHSHOP_GIFTSEND_ANS)pReceiveBuffer;

	switch ((BYTE)Data->byResultCode)
	{
	case -2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2953]);
	}
	break;
	case -1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2954]);
	}
	break;
	case 0:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2910], GlobalText[2911]);

		SendRequestIGS_CashPointInfo();
	}
	break;
	case 1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2913]);
	}
	break;
	case 2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2914]);
	}
	break;
	case 3:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2915]);
	}
	break;
	case 4:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2956]);
	}
	break;
	case 5:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2958]);
	}
	break;
	case 6:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2945], GlobalText[2958]);
	}break;
	case 7:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2959]);
	}
	break;
	case 8:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2960]);
	}
	break;
	case 9:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[2961]);
	}
	break;
	case 10:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[3264]);
	}
	break;
	case 20:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2912], GlobalText[3263]);
	}
	break;
	default:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2945], GlobalText[890]);
	}
	break;
	}

	return true;
}

// (0xD2)(0x06)
bool ReceiveIGS_StorageItemListCount(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_STORAGECOUNT Data = (LPPMSG_CASHSHOP_STORAGECOUNT)pReceiveBuffer;
	g_pInGameShop->InitStorage((int)Data->wTotalItemCount, (int)Data->wCurrentItemCount, (int)Data->wTotalPage, (int)Data->wPageIndex);
	return true;
}

// (0xD2)(0x0D)
bool ReceiveIGS_StorageItemList(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_STORAGELIST Data = (LPPMSG_CASHSHOP_STORAGELIST)pReceiveBuffer;

#ifdef KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	if (g_InGameShopSystem->IsShopOpen() == false)
		return false;
#endif // KJH_MOD_SHOP_SCRIPT_DOWNLOAD

	g_pInGameShop->AddStorageItem((int)Data->lStorageIndex, (int)Data->lItemSeq, (int)Data->lStorageGroupCode, (int)Data->lProductSeq, (int)Data->lPriceSeq, (int)Data->dCashPoint, (char)Data->chItemType);
	return true;
}

// (0xD2)(0x0E)
bool ReceiveIGS_StorageGiftItemList(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_GIFTSTORAGELIST Data = (LPPMSG_CASHSHOP_GIFTSTORAGELIST)pReceiveBuffer;

#ifdef KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	if (g_InGameShopSystem->IsShopOpen() == false)
		return false;
#endif // KJH_MOD_SHOP_SCRIPT_DOWNLOAD

	unicode::t_char szID[MAX_ID_SIZE + 1];
	unicode::t_char szMessage[MAX_GIFT_MESSAGE_SIZE];

	strncpy(szID, (unicode::t_char*)Data->chSendUserName, MAX_ID_SIZE + 1);
	strncpy(szMessage, (unicode::t_char*)Data->chMessage, MAX_GIFT_MESSAGE_SIZE);
	szID[MAX_ID_SIZE] = '\0';
	szMessage[MAX_GIFT_MESSAGE_SIZE - 1] = '\0';

	g_pInGameShop->AddStorageItem((int)Data->lStorageIndex, (int)Data->lItemSeq, (int)Data->lStorageGroupCode, (int)Data->lProductSeq, (int)Data->lPriceSeq, (int)Data->dCashPoint, (char)Data->chItemType, szID, szMessage);
	return true;
}

//----------------------------------------------------------------------------
// (0xD2)(0x07)
bool ReceiveIGS_SendCashGift(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_CASHSEND_ANS Data = (LPPMSG_CASHSHOP_CASHSEND_ANS)pReceiveBuffer;
	return true;
}

//----------------------------------------------------------------------------
// (0xD2)(0x08)
bool ReceiveIGS_PossibleBuy(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_ITEMBUY_CONFIRM_ANS Data = (LPPMSG_CASHSHOP_ITEMBUY_CONFIRM_ANS)pReceiveBuffer;
	return true;
}

// (0xD2)(0x09)
bool ReceiveIGS_LeftCountItem(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_ITEMBUY_LEFT_COUNT_ANS Data = (LPPMSG_CASHSHOP_ITEMBUY_LEFT_COUNT_ANS)pReceiveBuffer;
	return true;
}

// (0xD2)(0x0B)
bool ReceiveIGS_UseStorageItem(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_STORAGE_ITEM_USE_ANS Data = (LPPMSG_CASHSHOP_STORAGE_ITEM_USE_ANS)pReceiveBuffer;

	switch ((BYTE)Data->byResult)
	{
	case -2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2967]);
	}
	break;
	case -1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2966]);
	}
	break;
	case 0:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2924], GlobalText[2925]);

		g_pInGameShop->UpdateStorageItemList();
	}
	break;
	case 1:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2962]);
	}
	break;
	case 2:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2963]);
	}
	break;
	case 3:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2964]);
	}
	break;
	case 4:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2965]);
	}break;
	case 21:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2284]);
	}
	break;
	case 22:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[3036]);
	}
	break;
#ifdef LEM_FIX_SERVERMSG_SEALITEM
	case 24:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2928], GlobalText[2610]);
	}
	break;
#endif	// LEM_FIX_SERVERMSG_SEALITEM
	default:
	{
		CMsgBoxIGSCommon* pMsgBox = NULL;
		CreateMessageBox(MSGBOX_LAYOUT_CLASS(CMsgBoxIGSCommonLayout), &pMsgBox);
		pMsgBox->Initialize(GlobalText[2945], GlobalText[890]);
	}
	break;
	}
	return true;
}

// (0xD2)(0x0C)
bool ReceiveIGS_UpdateScript(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_VERSION_UPDATE Data = (LPPMSG_CASHSHOP_VERSION_UPDATE)pReceiveBuffer;

#ifdef KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	g_InGameShopSystem->SetScriptVersion(Data->wSaleZone, Data->wYear, Data->wYearIdentify);
	g_InGameShopSystem->ShopOpenUnLock();
#else // KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) == true)
	{
		SendRequestIGS_CashShopOpen(1);
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_INGAMESHOP);
	}

	g_InGameShopSystem->Release();

	g_InGameShopSystem->SetScriptVersion(Data->wSaleZone, Data->wYear, Data->wYearIdentify);

	if (g_InGameShopSystem->ScriptDownload() == false)
	{
		return false;
	}

	g_InGameShopSystem->Initalize();
	g_pInGameShop->InitZoneBtn();
#endif // KJH_MOD_SHOP_SCRIPT_DOWNLOAD

	return true;
}

// (0xD2)(0x13)
bool ReceiveIGS_EventItemlistCnt(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_EVENTITEM_COUNT Data = (LPPMSG_CASHSHOP_EVENTITEM_COUNT)pReceiveBuffer;

	g_InGameShopSystem->InitEventPackage((int)Data->wEventItemListCount);

	return true;
}

//(0xD2)(0x14)
bool ReceiveIGS_EventItemlist(BYTE* pReceiveBuffer)
{
	LPPMSG_CASHSHOP_EVENTITEM_LIST Data = (LPPMSG_CASHSHOP_EVENTITEM_LIST)pReceiveBuffer;
	g_InGameShopSystem->InsertEventPackage((int*)Data->lPackageSeq);
	return true;
}

// (0xD2)(0x15)
bool ReceiveIGS_UpdateBanner(BYTE* pReceiveBuffer)
{
#ifndef KWAK_FIX_COMPILE_LEVEL4_WARNING_EX
	LPPMSG_CASHSHOP_BANNER_UPDATE Data = (LPPMSG_CASHSHOP_BANNER_UPDATE)pReceiveBuffer;
#endif // KWAK_FIX_COMPILE_LEVEL4_WARNING_EX

#ifdef KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	g_InGameShopSystem->SetBannerVersion(Data->wBannerZone, Data->wYear, Data->wYearIdentify);
#else // KJH_MOD_SHOP_SCRIPT_DOWNLOAD
	if (g_InGameShopSystem->IsShopOpen() == false)
		return false;

#ifdef _DEBUG
	g_InGameShopSystem->SetBannerVersion(583, 0, 0);
#else // _DEBUG
	g_InGameShopSystem->SetBannerVersion(Data->wBannerZone, Data->wYear, Data->wYearIdentify);
#endif // _DEBUG

	if (g_InGameShopSystem->BannerDownload() == false)
	{
		return false;
	}

	g_pInGameShop->InitBanner(g_InGameShopSystem->GetBannerFileName(), g_InGameShopSystem->GetBannerURL());
#endif // KJH_MOD_SHOP_SCRIPT_DOWNLOAD

	return true;
}

#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM


#ifdef PBG_ADD_SECRETBUFF
bool ReceiveFatigueTime(BYTE* pReceiveBuffer)
{
	LPPMSG_FATIGUEPERCENTAGE Data = (LPPMSG_FATIGUEPERCENTAGE)pReceiveBuffer;

	if (g_FatigueTimeSystem->SetFatiguePercentage(Data->btFatiguePercentage))
	{
		g_FatigueTimeSystem->SetIsFatigueSystem(true);
#ifdef PBG_MOD_STAMINA_UI
		g_pNewUIStamina->SetCaution(Data->btFatiguePercentage);
#endif //PBG_MOD_STAMINA_UI
		return true;
	}
	else
		return false;

	return false;
}
#endif //PBG_ADD_SECRETBUFF

#ifdef LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
bool ReceiveEquippingInventoryItem(BYTE* pReceiveBuffer)
{
	LPPMSG_ANS_INVENTORY_EQUIPMENT_ITEM Data = (LPPMSG_ANS_INVENTORY_EQUIPMENT_ITEM)pReceiveBuffer;

	int iResult = (int)Data->btResult;
	int iItemPos = (int)Data->btItemPos;
	iItemPos -= MAX_EQUIPMENT_INDEX;

	if (!(iResult == 254 || iResult == 255))
		return false;

	if (iItemPos < 0 || iItemPos >= MAX_INVENTORY)
		return false;

	ITEM* pItem = g_pMyInventory->FindItem(iItemPos);
	pItem->Durability = iResult;

#ifdef CONSOLE_DEBUG
	g_ConsoleDebug->Write(MCD_RECEIVE, "[0xBF][0x20]  [ReceiveEquippingInventoryItem]");
#endif // CONSOLE_DEBUG

	return true;
}
#endif //LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY


#ifdef KJH_ADD_PERIOD_ITEM_SYSTEM

bool ReceivePeriodItemListCount(BYTE* pReceiveBuffer)
{
	LPPMSG_PERIODITEMEX_ITEMCOUNT Data = (LPPMSG_PERIODITEMEX_ITEMCOUNT)pReceiveBuffer;
	return true;
}

bool ReceivePeriodItemList(BYTE* pReceiveBuffer)
{
	LPPMSG_PERIODITEMEX_ITEMLIST Data = (LPPMSG_PERIODITEMEX_ITEMLIST)pReceiveBuffer;

	if (Data->wItemSlotIndex < MAX_EQUIPMENT_INDEX)
	{
		CharacterMachine->Equipment[Data->wItemSlotIndex].lExpireTime = (long)Data->lExpireDate;
	}
	else
	{
		ITEM* pItem = g_pMyInventory->FindItem(Data->wItemSlotIndex - MAX_EQUIPMENT_INDEX);

		if (pItem == NULL)
			return false;

		pItem->lExpireTime = (long)Data->lExpireDate;
	}

	return true;
}
#endif // KJH_ADD_PERIOD_ITEM_SYSTEM

#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
BOOL ReceiveStraightAttack(BYTE* ReceiveBuffer, int Size, BOOL bEncrypted)
{
	LPPRECEIVE_STRAIGHTATTACK Data = (LPPRECEIVE_STRAIGHTATTACK)ReceiveBuffer;
	int SourceKey = ((int)(Data->SourceKeyH) << 8) + Data->SourceKeyL;
	int TargetKey = ((int)(Data->TargetKeyH) << 8) + Data->TargetKeyL;
	int Success = (TargetKey >> 15);

	WORD AttackNumber = ((WORD)(Data->AttackH) << 8) + Data->AttackL;

	TargetKey &= 0x7FFF;
	int Index = FindCharacterIndex(SourceKey);
	int TargetIndex = FindCharacterIndex(TargetKey);

	if (TargetIndex != MAX_CHARACTERS_CLIENT || AttackNumber == Skill_Dark_Side || AttackNumber == Skill_Dark_Side_Strengthener)
	{

		CHARACTER* sc = gmCharacters->GetCharacter(Index);
		CHARACTER* tc = gmCharacters->GetCharacter(TargetIndex);

		if (sc != NULL && tc != NULL)
		{
			AttackPlayer = Index;
			OBJECT* so = &sc->Object;
			OBJECT* to = &tc->Object;

			if (sc != Hero && to->Visible)
				so->Angle[2] = CreateAngle(so->Position[0], so->Position[1], to->Position[0], to->Position[1]);

			sc->TargetCharacter = TargetIndex;

			sc->SkillSuccess = (Success != 0);
			sc->Skill = AttackNumber;

			so->m_sTargetIndex = TargetIndex;

			if (g_CMonkSystem.IsRageHalfwaySkillAni(AttackNumber))
			{
				if (sc == Hero || (AttackNumber != Skill_Beast_Uppercut && AttackNumber != Skill_Beast_Uppercut_Strengthener && AttackNumber != Skill_Beast_Uppercut_Mastery))
				{
					if (AttackNumber != Skill_Beast_Uppercut && AttackNumber != Skill_Beast_Uppercut_Strengthener && AttackNumber != Skill_Beast_Uppercut_Mastery)
					{
						g_CMonkSystem.SetRageSkillAni(AttackNumber, so);
					}
				}
				else
				{
					g_CMonkSystem.SetRageSkillAni(AttackNumber, so);
					g_CMonkSystem.RageCreateEffect(so, AttackNumber);
				}
				sc->AttackTime = 1;
			}

			if (AttackNumber == Skill_Dark_Side || AttackNumber == Skill_Dark_Side_Strengthener)
			{
				if (sc != Hero)
					g_CMonkSystem.RageCreateEffect(so, AttackNumber);
				g_CMonkSystem.SetRageSkillAni(AttackNumber, so);
				sc->AttackTime = 1;
			}
		}
	}
	else
	{
		SendHackingChecked(0, 74);
	}

	return (TRUE);
}

void ReceiveDarkside(BYTE* ReceiveBuffer)
{
	LPPRECEIVE_DARKSIDE_INDEX Data = (LPPRECEIVE_DARKSIDE_INDEX)ReceiveBuffer;

	if (Data->MagicNumber == Skill_Dark_Side || Data->MagicNumber == Skill_Dark_Side_Strengthener)
	{
		g_CMonkSystem.InitDarksideTarget();
		g_CMonkSystem.SetDarksideTargetIndex(Data->TargerIndex);
	}
}
#endif //PBG_ADD_NEWCHAR_MONK_SKILL

void ReceiveHelperDataRecv(BYTE* ReceiveBuffer)
{
	LPPMSG_HELPER_SETTING Data = (LPPMSG_HELPER_SETTING)ReceiveBuffer;

	gmAIController->ResetConfig();

	if (!Data->result)
	{
		gmAIController->Unpack(Data->data);
	}
}

void ReceiveHelperResult(BYTE* ReceiveBuffer)
{
	LPPMSG_ANS_HELPER_START Data = (LPPMSG_ANS_HELPER_START)ReceiveBuffer;

	if (Data->result == 0)
	{
		if (Data->time)
		{
			unicode::t_char text[0x100] = { 0 };
			sprintf_s(text, GlobalText[3586], Data->money);
			g_pChatListBox->AddText("", text, SEASON3B::TYPE_ERROR_MESSAGE);

			g_pMacroGaugeBar->SetMacroExpense(Data->money);
		}
		else
		{
			gmAIController->Start();
			gfxNotice::addLongMovementText(GlobalText[3562], 20, 150, 2000);
		}
	}
	else if (Data->result == 1)
	{
		gmAIController->Stop();
	}
	else if (Data->result == 2)
	{
		g_pChatListBox->AddText("", GlobalText[3587], SEASON3B::TYPE_ERROR_MESSAGE);
		SendRequestStartHelper(TRUE);
	}
}

void ReceiveInventoryHoly(BYTE* ReceiveBuffer)
{
	LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;

	g_pInventoryJewel->RemoveData();

	int offset = sizeof(PHEADER_DEFAULT_SUBCODE_WORD);

	for (size_t i = 0; i < Data->Value; i++)
	{
		BYTE* btData = (ReceiveBuffer + offset);

		short bItemindex = *(short*)(btData + 8);

		__int64 bItemCount = 0x0000000000000000;

		// Supongamos que btData tiene 12 bytes y necesitas extraer bItemCount de los primeros 6 bytes
		bItemCount |= (__int64)btData[0];
		bItemCount |= (__int64)btData[1] << 8;
		bItemCount |= (__int64)btData[2] << 16;
		bItemCount |= (__int64)btData[3] << 24;
		bItemCount |= (__int64)btData[4] << 32;
		bItemCount |= (__int64)btData[5] << 40;

		g_pInventoryJewel->InsertData(btData[11], bItemindex, btData[10], bItemCount);

		offset += sizeof(PRECEIVE_INVENTORY_HOLY);
	}
}

void ReceiveCharacterLifeInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_CHARACTER_LIFE_INFO Data = (LPPMSG_CHARACTER_LIFE_INFO)ReceiveBuffer;

	WORD Key = ((WORD)(Data->KeyH) << 8) + Data->KeyL;
	Key &= 0x7FFF;

	int _iKey = FindCharacterIndex(Key);
	CHARACTER* pCharacter = gmCharacters->GetCharacter(_iKey);

	if (pCharacter)
	{
		pCharacter->SetLife(Data->CurHp, Data->MaxHp);

#ifdef PLAYER_INFO_SYSTEM
		if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
		{
			pCharacter->SetUIHp(_iKey, pCharacter->GetLife());
		}
#endif // PLAYER_INFO_SYSTEM

	}
	//CharacterAttribute->MagicSpeed = Data->MagicSpeed;
	//CharacterAttribute->AttackSpeed = Data->AttackSpeed;
}

void ReceiveCharacterBaseInfo(BYTE* ReceiveBuffer)
{
	LPPMSG_CHARACTER_BASE_INFO info = (LPPMSG_CHARACTER_BASE_INFO)ReceiveBuffer;

	CharacterAttribute->MagicSpeed = info->MagicSpeed;
	CharacterAttribute->AttackSpeed = info->AttackSpeed;
}

void UpdateInventoryExtend(BYTE* ReceiveBuffer)
{
	LPPMSG_CREATE_INVENTARIO_SEND Data = (LPPMSG_CREATE_INVENTARIO_SEND)ReceiveBuffer;
	//--ExtInventory
	g_pInvenExpansion->SetEnableExpansionInven(Data->ExtInventory, Data->PriceInventoryExt);

	g_pMyInventory->SetInvenPos();

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
	{
		g_pMyInventory->SetExpansionOpenState();
	}
}

void settings_reset_manager(BYTE* ReceiveBuffer, BYTE type)
{
	if (type == 0)
	{
		LPPMSG_RESET_TABLE_INFO Data = (LPPMSG_RESET_TABLE_INFO)ReceiveBuffer;

		GMResetManager->SetResetInfo(Data->CurReset, Data->MaxReset, Data->MinLevel, Data->RewardPoint, Data->ReqMoney, Data->ItemIndex, Data->ItemCount);
	}
	if (type == 1)
	{
		LPPMSG_MASTER_RESET_TABLE_INFO Data = (LPPMSG_MASTER_RESET_TABLE_INFO)ReceiveBuffer;

		GMResetManager->SetMasterResetInfo(Data->CurMasterReset, Data->MaxMasterReset, Data->MinLevel, Data->MinReset, Data->RewardPoint, Data->ReqMoney, Data->ItemIndex, Data->ItemCount);
	}
}

void settings_invasion_kill(BYTE* ReceiveBuffer)
{
	LPPMSG_INVASION_KILL_INFO Data = (LPPMSG_INVASION_KILL_INFO)ReceiveBuffer;

	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->SetMonsterKill(Data->Index, Data->MonsterIndex, Data->Monster_Kill, Data->MonsterCount);
	}
}

void settings_invasion_manager(BYTE* ReceiveBuffer)
{
	LPPMSG_INVASION_DEFAULT_INFO Data = (LPPMSG_INVASION_DEFAULT_INFO)ReceiveBuffer;

	int Offset = sizeof(PMSG_INVASION_DEFAULT_INFO);

	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->SetInvasion(Data->Index, Data->RemainTime, Data->Name);

		for (size_t i = 0; i < (size_t)Data->count; i++)
		{
			LP_INVASION_DEFAULT_MONSTER Data2 = (LP_INVASION_DEFAULT_MONSTER)(ReceiveBuffer + Offset);

			GMInvasionManager->SetMonsterCount(Data->Index, Data2->MonsterIndex, Data2->Monster_Kill, Data2->MonsterCount);

			Offset += sizeof(INVASION_DEFAULT_MONSTER);
		}
	}
}
#include "Protocol.h"
BOOL TranslateProtocol(int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEncrypted)
{
	//===Hook Protocol Custom
	if (ProtocolCoreEx(HeadCode, ReceiveBuffer, Size, bEncrypted))
	{
		return 1;
	}

	switch (HeadCode)
	{
	case 0xF1:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;

		g_ConsoleDebug->Write(MCD_RECEIVE, "Recv [0xF1][0x%02x][0x%02x]", Data->SubCode, Data->Value);

		switch (Data->SubCode)
		{
		case 0x00: //receive join server
			ReceiveJoinServer(ReceiveBuffer);
			break;
		case 0x01: //receive log in
			g_pReconnectUI->ReconnectOnConnectAccount(Data->Value);
			//AddDebugText(ReceiveBuffer,Size);
			switch (Data->Value)
			{
			case 0x20:
				CurrentProtocolState = RECEIVE_LOG_IN_SUCCESS;
				LogIn = 2;
				CheckHack();
				break;
			case 0x00:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_PASSWORD);
				break;
			case 0x01:
				CurrentProtocolState = RECEIVE_LOG_IN_SUCCESS;
				LogIn = 2;
				CheckHack();
				break;
			case 0x02:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_ID);
				break;
			case 0x03:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_ID_CONNECTED);
				break;
			case 0x04:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_SERVER_BUSY);
				break;
			case 0x05:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_ID_BLOCK);
				break;
			case 0x06:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_VERSION);
				g_ErrorReport.Write("Version dismatch. - Login\r\n");
				break;
			case 0x07:
			default:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_CONNECT);
				break;
			case 0x08:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_ERROR);
				break;
			case 0x09:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_NO_PAYMENT_INFO);
				break;
			case 0x0a:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_USER_TIME1);
				break;
			case 0x0b:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_USER_TIME2);
				break;
			case 0x0c:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_PC_TIME1);
				break;
			case 0x0d:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_PC_TIME2);
				break;
			case 0x11:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_ONLY_OVER_15);
				break;
			case 0x40:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_CHARGED_CHANNEL);
				break;
			case 0xc0:
			case 0xd0:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_POINT_DATE);
				break;
			case 0xc1:
			case 0xd1:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_POINT_HOUR);
				break;
			case 0xc2:
			case 0xd2:
				CUIMng::Instance().PopUpMsgWin(RECEIVE_LOG_IN_FAIL_INVALID_IP);
				break;
			}
			break;
		case 0x02:
			if (!ReceiveLogOut(ReceiveBuffer, bEncrypted))
			{
				return (FALSE);
			}
			break;
		case 0x12: //0x02 receive create account
			switch (Data->Value)
			{
			case 0x00:
				CurrentProtocolState = RECEIVE_CREATE_ACCOUNT_FAIL_ID;
				break;
			case 0x01:
				CurrentProtocolState = RECEIVE_CREATE_ACCOUNT_SUCCESS;
				break;
			case 0x02:
				CurrentProtocolState = RECEIVE_CREATE_ACCOUNT_FAIL_RESIDENT;
				break;
			}
			break;
		case 0x03: //receive confirm password
			//AddDebugText(ReceiveBuffer,Size);
			ReceiveConfirmPassword(ReceiveBuffer);
			break;
		case 0x04: //receive confirm password
			//AddDebugText(ReceiveBuffer,Size);
			ReceiveConfirmPassword2(ReceiveBuffer);
			break;
		case 0x05: //receive change password
			ReceiveChangePassword(ReceiveBuffer);
			break;
		}
		break;
	}
	case 0xF3:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}

		g_ConsoleDebug->Write(MCD_RECEIVE, "Recv [0xF3][0x%02x]", subcode);

		switch (subcode)
		{
		case 0x00: //receive characters list
			ReceiveCharacterList(ReceiveBuffer);
			break; 
		case 0x01: //receive create character
			ReceiveCreateCharacter(ReceiveBuffer);
			break;
		case 0x02: //receive delete character
			ReceiveDeleteCharacter(ReceiveBuffer);
			break;
		case 0x03: //receive join map server
			if (!ReceiveJoinMapServer(ReceiveBuffer, bEncrypted))
			{
				return (FALSE);
			}
			break;
		case 0x04: //receive revival
			ReceiveRevival(ReceiveBuffer);
			break;
		case 0x05: //receive level up
			ReceiveLevelUp(ReceiveBuffer);
			break;
		case 0x06: //receive Add Point
			ReceiveAddPoint(ReceiveBuffer);
			break;
		case 0x07: //receive damage
			ReceiveDamage(ReceiveBuffer);
			break;
		case 0x08:
			ReceivePK(ReceiveBuffer);
			break;
		case 0x10: //receive inventory
			if (!ReceiveInventory(ReceiveBuffer, bEncrypted))
			{
				return (FALSE);
			}
			break;
		case 0x11:
			ReceiveMagicList(ReceiveBuffer);
			break;
		case 0x13:
			ReceiveEquipment(ReceiveBuffer);
			break;
		case 0x14:
			ReceiveModifyItem(ReceiveBuffer);
			break;
		case 0x15:
#ifdef ITEM_CHAT_SELL_SYSTEM
			ReceivePostChatItem(ReceiveBuffer);
#endif // ITEM_CHAT_SELL_SYSTEM
			break;
		case 0x20:
			ReceiveSummonLife(ReceiveBuffer);
			break;
		case 0x22:
			ReceiveWTTimeLeft(ReceiveBuffer);
			break;
		case 0x24:
			ReceiveWTMatchResult(ReceiveBuffer);
			break;
		case 0x25:
			ReceiveWTBattleSoccerGoalIn(ReceiveBuffer);
			break;
		case 0x23:
			ReceiveSoccerScore(ReceiveBuffer);
			break;
		case 0x30:
			ReceiveOption(ReceiveBuffer);
			break;
		case 0x40:
			ReceiveServerCommand(ReceiveBuffer);
			break;
		case 0x50:
			Receive_Master_Level_Exp(ReceiveBuffer);
			break;
		case 0x51:
			Receive_Master_LevelUp(ReceiveBuffer);
			break;
		case 0x52:
			Receive_Master_LevelGetSkill(ReceiveBuffer);
			break;
		case 0x53:
			Receive_Master_LevelSkillTree(ReceiveBuffer);
			break;
		case 0xE0:
			ReceiveNewJoinMapServer(ReceiveBuffer);
			break;
		case 0xE1:
			ReceiveNewCalculate(ReceiveBuffer);
			break;
		case 0xE2:
			break;
		case 0xE3:
			//ReceiveNewCalculate(ReceiveBuffer);
			return 1;
		case 0xE4:
			break;
		case 0xE5:
			break;
		case 0xE6:
			if (gNewUIRankingTop)
				gNewUIRankingTop->ReceiveRankingListInfo(ReceiveBuffer);
			return 1;
		case 0xE7:
			if (gNewUIRankingTop)
				gNewUIRankingTop->ReceiveRankingInfo(ReceiveBuffer);
			return 1;
		case 0xE8:
			break;
		case 0xEA:
			ReceiveCharacterCoin(ReceiveBuffer);
			return 1;
		case 0xEB:
			break;
		case 0xEC:
			//ReceiveNewCalculate(ReceiveBuffer);
			return 1;
		case 0xED:
			GCBuyConfirmRecv(ReceiveBuffer);
			break;
		case 0xEE:
			GCCharacterServerInfoRecv(ReceiveBuffer);
			break;
		case 0xF3:
			g_pCharacterInfoWindow->ReceiveAdvanceChar(ReceiveBuffer);
			break;
		}
		break;
	}
	case 0xF4:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x03:
			ReceiveServerConnect(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveServerConnectBusy(ReceiveBuffer);
			break;
		case 0x06:
			ReceiveServerList(ReceiveBuffer);
			break;
#ifdef SOCKET_LAUNCHER
		case 0x08:
			ReceiveServerUpdate(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveServerVersion(ReceiveBuffer);
			break;
#endif // SOCKET_LAUNCHER

		}
	}
	break;
	case 0x00://chat
		ReceiveChat(ReceiveBuffer);
		break;
	case 0x01://chat
		ReceiveChatKey(ReceiveBuffer);
		break;
	case 0x02://chat whisper
		ReceiveChatWhisper(ReceiveBuffer);
		break;
	case 0x03:
		ReceiveCheckSumRequest(ReceiveBuffer);
		break;
	case 0x0B:
		AddDebugText(ReceiveBuffer, Size);
		ReceiveEvent(ReceiveBuffer);
		break;
	case 0x0C:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveChatWhisperResult(ReceiveBuffer);
		break;
	case 0x0D://notice
		ReceiveNotice(ReceiveBuffer);
		break;
	case 0xEC:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}

		switch (subcode)
		{
		case 0x10:
			ReceiveCharacterLifeInfo(ReceiveBuffer);
			break;
		case 0x30:
			ReceiveCharacterBaseInfo(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x0f:
		ReceiveWeather(ReceiveBuffer);
		break;
	case PACKET_MOVE: //move character
		ReceiveMoveCharacter(ReceiveBuffer);
		break;
	case PACKET_POSITION: //move position
		ReceiveMovePosition(ReceiveBuffer);
		break;
	case 0x12: //create characters
		AddDebugText(ReceiveBuffer, Size);
		ReceiveCreatePlayerViewport(ReceiveBuffer, Size);
		break;
	case 0x13: //create monsters
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveCreateMonsterViewport(ReceiveBuffer);
		break;
	case 0x1F: //create monsters
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveCreateSummonViewport(ReceiveBuffer);
		break;
	case 0x45: //create monsters
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveCreateTransformViewport(ReceiveBuffer);
		break;
	case 0x14: //delete characters & monsters
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveDeleteCharacterViewport(ReceiveBuffer);
		break;
	case 0x20: //create item
		ReceiveCreateItemViewport(ReceiveBuffer);
		break;
	case 0x21://delete item
		ReceiveDeleteItemViewport(ReceiveBuffer);
		break;
	case 0x22://get item
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveGetItem(ReceiveBuffer);
		break;
	case 0x23://drop item
		ReceiveDropItem(ReceiveBuffer);
		break;
	case 0x24://equipment item
		AddDebugText(ReceiveBuffer, Size);
		if (!ReceiveEquipmentItem(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x25://change character
		ReceiveChangePlayer(ReceiveBuffer);
		break;
	case PACKET_ATTACK://attack character
		ReceiveAttackDamage(ReceiveBuffer);
		break;
	case 0x18://action character
		ReceiveAction(ReceiveBuffer, Size);
		break;
	case 0x19://magic
		if (!ReceiveMagic(ReceiveBuffer, Size, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x69:
		if (!ReceiveMonsterSkill(ReceiveBuffer, Size, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x1A://magic
		ReceiveMagicPosition(ReceiveBuffer, Size);
		break;
	case 0x1E://magic
		if (!ReceiveMagicContinue(ReceiveBuffer, Size, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x1B://magic
		ReceiveMagicFinish(ReceiveBuffer);
		break;
	case 0x07://setmagicstatus
		ReceiveSkillStatus(ReceiveBuffer);
		break;
	case 0x16://die character(exp)
		if (!ReceiveDieExp(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x9C://die character(exp)
		if (!ReceiveDieExpLarge(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x17://die character
		ReceiveDie(ReceiveBuffer, Size);
		break;
	case 0x2A:
		AddDebugText(ReceiveBuffer, Size);
		ReceiveDurability(ReceiveBuffer);
		break;
	case 0x26:
		ReceiveLife(ReceiveBuffer);
		break;
	case 0x27:
		ReceiveMana(ReceiveBuffer);
		break;
	case 0x28:
		ReceiveDeleteInventory(ReceiveBuffer);
		break;
	case 0x29:
		//AddDebugText(ReceiveBuffer,Size);
		if (!ReceiveHelperItem(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x2C:
		ReceiveUseStateItem(ReceiveBuffer);
		break;
	case 0x30:
		if (!ReceiveTalk(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x31:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeInventory(ReceiveBuffer);
		break;
	case 0x32:
		ReceiveBuy(ReceiveBuffer);
		break;
	case 0x33:
		ReceiveSell(ReceiveBuffer);
		break;
	case 0x34:
		ReceiveRepair(ReceiveBuffer);
		break;
	case 0x36:
		if (!ReceiveTrade(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x37:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeResult(ReceiveBuffer);
		break;
	case 0x38:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeYourInventoryDelete(ReceiveBuffer);
		break;
	case 0x39:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeYourInventory(ReceiveBuffer);
		break;
	case 0x3A:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeMyGold(ReceiveBuffer);
		break;
	case 0x3B:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeYourGold(ReceiveBuffer);
		break;
	case 0x3C:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeYourResult(ReceiveBuffer);
		break;
	case 0x3D:
		//AddDebugText(ReceiveBuffer,Size);
		ReceiveTradeExit(ReceiveBuffer);
		break;
	case 0x1C:
		AddDebugText(ReceiveBuffer, Size);
		if (!ReceiveTeleport(ReceiveBuffer, bEncrypted))
		{
			return (FALSE);
		}
		break;
	case 0x40:
		ReceiveParty(ReceiveBuffer);
		break;
	case 0x41:
		ReceivePartyResult(ReceiveBuffer);
		break;
	case 0x42:
		ReceivePartyList(ReceiveBuffer);
		break;
	case 0x43:
		ReceivePartyLeave(ReceiveBuffer);
		break;
	case 0x44:
		ReceivePartyInfo(ReceiveBuffer);
		break;
	case 0x46:
		ReceiveSetAttribute(ReceiveBuffer);
		break;
	case 0x47:
		ReceivePartyGetItem(ReceiveBuffer);
		break;
	case 0x48:
		ReceiveDisplayEffectViewport(ReceiveBuffer);
		break;
	case 0x50:
		ReceiveGuild(ReceiveBuffer);
		break;
	case 0x51:
		ReceiveGuildResult(ReceiveBuffer);
		break;
	case 0x52:
		ReceiveGuildList(ReceiveBuffer);
		break;
	case 0x53:
		ReceiveGuildLeave(ReceiveBuffer);
		break;
	case 0x54:
		ReceiveCreateGuildInterface(ReceiveBuffer);
		break;
	case 0x55:
		ReceiveCreateGuildMasterInterface(ReceiveBuffer);
		break;
	case 0x56:
		ReceiveCreateGuildResult(ReceiveBuffer);
		break;
	case 0x65:
		ReceiveGuildIDViewport(ReceiveBuffer);
		break;
	case 0x66:
		ReceiveGuildInfo(ReceiveBuffer);
		break;
	case 0x5D:
		ReceiveDeleteGuildViewport(ReceiveBuffer);
		break;
	case 0x60:
		ReceiveDeclareWarResult(ReceiveBuffer);
		break;
	case 0x61:
		ReceiveDeclareWar(ReceiveBuffer);
		break;
	case 0x62:
		ReceiveGuildBeginWar(ReceiveBuffer);
		break;
	case 0x63:
		ReceiveGuildEndWar(ReceiveBuffer);
		break;
	case 0x64:
		ReceiveGuildWarScore(ReceiveBuffer);
		break;
	case 0xAE:
		ReceiveHelperDataRecv(ReceiveBuffer);
		break;
	case 0x4F:
	{
		LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveInventoryHoly(ReceiveBuffer);
			break;
		case 0x02:
			if(g_pGoblinMixExpansion)
				g_pGoblinMixExpansion->ReceiveMixOptions(ReceiveBuffer);
			break;
		case 0x03:
			//g_pGoblinMixExpansion->GetRecipeAdvice(ReceiveBuffer);
			break;
		case 0x04:
			if (g_pGoblinMixExpansion)
				g_pGoblinMixExpansion->ReceiveMixRecipients(ReceiveBuffer);
			break;
		case 0x05:
			if (g_pGoblinMixExpansion)
				g_pGoblinMixExpansion->ReceiveInventory(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xE1:
		ReceiveGuildAssign(ReceiveBuffer);
		break;
	case 0xE5:
		ReceiveGuildRelationShip(ReceiveBuffer);
		break;
	case 0xE6:
		ReceiveGuildRelationShipResult(ReceiveBuffer);
		break;
	case 0xEB:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveBanUnionGuildResult(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x67:
		ReceiveUnionViewportNotify(ReceiveBuffer);
		break;
	case 0xE9:
		ReceiveUnionList(ReceiveBuffer);
		break;
	case 0xBC:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x00:
			ReceiveGemMixResult(ReceiveBuffer);
			break;
		case 0x01:
			ReceiveGemUnMixResult(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x68:
		ReceivePreviewPort(ReceiveBuffer);
		break;
	case 0x71:
		ReceivePing(ReceiveBuffer);
		break;
	case 0x81:
		ReceiveStorageGold(ReceiveBuffer);
		break;
	case 0x82:
		ReceiveStorageExit(ReceiveBuffer);
		break;
	case 0x83:
		ReceiveStorageStatus(ReceiveBuffer);
		break;
	case 0x86:
		ReceiveMix(ReceiveBuffer);
		break;
	case 0x87:
		ReceiveMixExit(ReceiveBuffer);
		break;
	case 0x8E:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveMoveMapChecksum(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveRequestMoveMap(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x90:
		ReceiveMoveToDevilSquareResult(ReceiveBuffer);
		break;
	case 0x91:
		ReceiveEventZoneOpenTime(ReceiveBuffer);
		break;
	case 0x92:
		ReceiveDevilSquareCountDown(ReceiveBuffer);
		break;
	case 0x93:
		ReceiveDevilSquareRank(ReceiveBuffer);
		break;
	case 0x9A:
		ReceiveMoveToEventMatchResult(ReceiveBuffer);
		break;
	case 0x9B:
		ReceiveMatchGameCommand(ReceiveBuffer);
		break;
	case 0x94:
		ReceiveEventChipInfomation(ReceiveBuffer);
		break;
	case 0x95:
		ReceiveEventChip(ReceiveBuffer);
		break;
	case 0x96:
		ReceiveMutoNumber(ReceiveBuffer);
		break;
	case 0x99:
		ReceiveServerImmigration(ReceiveBuffer);
		break;
	case 0x9D:
		ReceiveScratchResult(ReceiveBuffer);
		break;
	case 0x9E:
		ReceivePlaySoundEffect(ReceiveBuffer);
		break;
	case 0x9F:
		ReceiveEventCount(ReceiveBuffer);
		break;
	case 0xA0:
		ReceiveQuestHistory(ReceiveBuffer);
		break;
	case 0xA1:
		ReceiveQuestState(ReceiveBuffer);
		break;
	case 0xA2:
		ReceiveQuestResult(ReceiveBuffer);
		break;
	case 0xA3:
		ReceiveQuestPrize(ReceiveBuffer);
		break;
	case 0xA4:
		ReceiveQuestMonKillInfo(ReceiveBuffer);
		break;
	case 0xF6:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
#ifdef ASG_ADD_TIME_LIMIT_QUEST
		case 0x00:
			ReceiveQuestLimitResult(ReceiveBuffer);
			break;
#endif	// ASG_ADD_TIME_LIMIT_QUEST
		case 0x03:
			ReceiveQuestByEtcEPList(ReceiveBuffer);
			break;
#ifdef ASG_ADD_TIME_LIMIT_QUEST
		case 0x04:
			ReceiveQuestByItemUseEP(ReceiveBuffer);
#endif	// ASG_ADD_TIME_LIMIT_QUEST
		case 0x0A:
			ReceiveQuestByNPCEPList(ReceiveBuffer);
			break;
		case 0x0B:
			ReceiveQuestQSSelSentence(ReceiveBuffer);
			break;
		case 0x0C:
			ReceiveQuestQSRequestReward(ReceiveBuffer);
			break;
		case 0x0D:
			ReceiveQuestCompleteResult(ReceiveBuffer);
			break;
		case 0x0F:
			ReceiveQuestGiveUp(ReceiveBuffer);
			break;
		case 0x1A:
			ReceiveProgressQuestList(ReceiveBuffer);
			break;
		case 0x1B:
			ReceiveProgressQuestRequestReward(ReceiveBuffer);
			break;
		case 0x20:
			ReceiveProgressQuestListReady(ReceiveBuffer);
			break;
		}
	}
	break;
#ifdef ASG_ADD_GENS_SYSTEM
	case 0xF8:
	{
		BYTE bySubcode;

		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			bySubcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			bySubcode = Data->SubCode;
		}

		switch (bySubcode)
		{
		case 0x02:
			ReceiveGensJoining(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveGensSecession(ReceiveBuffer);
			break;
		case 0x07:
			ReceivePlayerGensInfluence(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveOtherPlayerGensInfluenceViewport(ReceiveBuffer);
			break;
#ifdef PBG_ADD_GENSRANKING
		case 0x0A:
			ReceiveReward(ReceiveBuffer);
			break;
#endif //PBG_ADD_GENSRANKING
		}
	}
	break;
#endif	// ASG_ADD_GENS_SYSTEM
	case 0xF9:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveNPCDlgUIStart(ReceiveBuffer);
			break;
		}
	}
	break;

	case 0xA7:
		ReceivePetCommand(ReceiveBuffer);
		break;
	case 0xA8:
		ReceivePetAttack(ReceiveBuffer);
		break;
	case 0xA9:
		ReceivePetInfo(ReceiveBuffer);
		break;

	case 0xAA:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveDuelStart(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveDuelRequest(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveDuelEnd(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveDuelScore(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveDuelHP(ReceiveBuffer);
			break;
		case 0x06:
			ReceiveDuelChannelList(ReceiveBuffer);
			break;
		case 0x07:
			ReceiveDuelWatchRequestReply(ReceiveBuffer);
			break;
		case 0x08:
			ReceiveDuelWatcherJoin(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveDuelWatchEnd(ReceiveBuffer);
			break;
		case 0x0A:
			ReceiveDuelWatcherQuit(ReceiveBuffer);
			break;
		case 0x0B:
			ReceiveDuelWatcherList(ReceiveBuffer);
			break;
		case 0x0C:
			ReceiveDuelResult(ReceiveBuffer);
			break;
		case 0x0D:
			ReceiveDuelRound(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xF7:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x02:
			ReceiveEnterEmpireGuardianEvent(ReceiveBuffer);
			break;

		case 0x04:
			ReceiveRemainTickEmpireGuardian(ReceiveBuffer);
			break;

		case 0x06:
			ReceiveResultEmpireGuardian(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x3F:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveCreateShopTitleViewport(ReceiveBuffer);
			break;
		case 0x01:
			ReceiveSetPriceResult(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveCreatePersonalShop(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveDestroyPersonalShop(ReceiveBuffer);
			break;
		case 0x05:
			ReceivePersonalShopItemList(ReceiveBuffer);
			break;
		case 0x06:
			ReceivePurchaseItem(ReceiveBuffer);
			break;
		case 0x08:
			NotifySoldItem(ReceiveBuffer);
			break;
		case 0x10:
			ReceiveShopTitleChange(ReceiveBuffer);
			break;
		case 0x12:
			NotifyClosePersonalShop(ReceiveBuffer);
			break;
		case 0x13:
			ReceiveRefreshItemList(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0x4E:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x1A:
			UpdateInventoryExtend(ReceiveBuffer);
			break;
		case 0x39:
			settings_reset_manager(ReceiveBuffer, 0);
			break;
		case 0x3A:
			settings_reset_manager(ReceiveBuffer, 1);
			break;
		case 0x3E:
			settings_invasion_kill(ReceiveBuffer);
			break;
		case 0x3F:
			settings_invasion_manager(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xAF:
	{
		LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
		switch (Data->SubCode)
		{
		case 0x01:
			ReceiveMoveToEventMatchResult2(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xB1:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveChangeMapServerInfo(ReceiveBuffer);
			break;

		case 0x01:
			ReceiveChangeMapServerResult(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xB2:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveBCStatus(ReceiveBuffer);
			break;
		case 0x01:
			ReceiveBCReg(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveBCGiveUp(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveBCRegInfo(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveBCRegMark(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveBCNPCBuy(ReceiveBuffer);
			break;
		case 0x06:
			ReceiveBCNPCRepair(ReceiveBuffer);
			break;
		case 0x07:
			ReceiveBCNPCUpgrade(ReceiveBuffer);
			break;
		case 0x08:
			ReceiveBCGetTaxInfo(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveBCChangeTaxRate(ReceiveBuffer);
			break;
		case 0x10:
			ReceiveBCWithdraw(ReceiveBuffer);
			break;
		case 0x1A:
			ReceiveTaxInfo(ReceiveBuffer);
			break;
		case 0x1F:
			ReceiveHuntZoneEnter(ReceiveBuffer);
			break;
		case 0x11:
			ReceiveGateState(ReceiveBuffer);
			break;
		case 0x12:
			ReceiveGateOperator(ReceiveBuffer);
			break;
		case 0x13:
			ReceiveGateCurrentState(ReceiveBuffer);
			break;
		case 0x14:
			ReceiveCrownSwitchState(ReceiveBuffer);
			break;
		case 0x15:
			ReceiveCrownRegist(ReceiveBuffer);
			break;
		case 0x16:
			ReceiveCrownState(ReceiveBuffer);
			break;
		case 0x17:
			ReceiveBattleCastleStart(ReceiveBuffer);
			break;
		case 0x18:
			ReceiveBattleCastleProcess(ReceiveBuffer);
			break;
		case 0x19:
			ReceiveBattleCastleRegiment(ReceiveBuffer);
			break;
		case 0x20:
			ReceiveBattleCasleSwitchInfo(ReceiveBuffer);
			break;
		case 0x1B:
			ReceiveMapInfoResult(ReceiveBuffer);
			break;
		case 0x1D:
			ReceiveGuildCommand(ReceiveBuffer);
			break;
		case 0x1E:
			ReceiveMatchTimer(ReceiveBuffer);
			break;
		case 0x0A:
			ReceiveInfoCastleNpc(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xB3:
		ReceiveBCNPCList(ReceiveBuffer);
		break;
	case 0xB4:
		ReceiveBCDeclareGuildList(ReceiveBuffer);
		break;
	case 0xB5:
		ReceiveBCGuildList(ReceiveBuffer);
		break;
	case 0xB6:
		ReceiveGuildMemberLocation(ReceiveBuffer);
		break;
	case 0xBB:
		ReceiveGuildNpcLocation(ReceiveBuffer);
		break;
	case 0xB7:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveCatapultState(ReceiveBuffer);
			break;

		case 0x01:
			ReceiveCatapultFire(ReceiveBuffer);
			break;

		case 0x02:
			ReceiveCatapultFireToMe(ReceiveBuffer);
			break;

		case 0x03:
			//                ReceiveDamageFrmeCatapult ( ReceiveBuffer );
			break;
		}
	}
	break;

	case 0xB8:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x01:
			ReceiveKillCount(ReceiveBuffer);
			break;

		case 0x02:
			break;
		}
	}
	break;
	case 0xB9:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x01:
			ReceiveBuildTime(ReceiveBuffer);
			break;

		case 0x02:
			ReceiveCastleGuildMark(ReceiveBuffer);
			break;

		case 0x03:
			ReceiveCastleHuntZoneInfo(ReceiveBuffer);
			break;

		case 0x05:
			ReceiveCastleHuntZoneResult(ReceiveBuffer);
			break;
		}
	}
	break;

	case 0xBA:
		ReceiveSkillCount(ReceiveBuffer);
		break;
	case 0xBD:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveCrywolfInfo(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveCrywolStateAltarfInfo(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveCrywolfAltarContract(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveCrywolfLifeTime(ReceiveBuffer);
			break;
		case 0x0C:
			ReceiveCrywolfTankerHit(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveCrywolfBossMonsterInfo(ReceiveBuffer);
			break;
		case 0x07:
			ReceiveCrywolfPersonalRank(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveCrywolfBenefitPlusChaosRate(ReceiveBuffer);
			break;
		case 0x08:
			ReceiveCrywolfHeroList(ReceiveBuffer);
			break;
		}
		break;
	}
	case 0xC0:
		ReceiveFriendList(ReceiveBuffer);
		break;
	case 0xC1:
		ReceiveAddFriendResult(ReceiveBuffer);
		break;
	case 0xC2:
		ReceiveRequestAcceptAddFriend(ReceiveBuffer);
		break;
	case 0xC3:
		ReceiveDeleteFriendResult(ReceiveBuffer);
		break;
	case 0xC4:
		ReceiveFriendStateChange(ReceiveBuffer);
		break;
	case 0xC5:
		ReceiveLetterSendResult(ReceiveBuffer);
		break;
	case 0xC6:
		ReceiveLetter(ReceiveBuffer);
		break;
	case 0xC7:
		ReceiveLetterText(ReceiveBuffer);
		break;
	case 0xC8:
		ReceiveLetterDeleteResult(ReceiveBuffer);
		break;
	case 0xCA:
		ReceiveCreateChatRoomResult(ReceiveBuffer);
		break;
	case 0xCB:
		ReceiveChatRoomInviteResult(ReceiveBuffer);
		break;
	case 0x2D:
		ReceiveBuffState(ReceiveBuffer);
		break;
	case 0xD1:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		switch (subcode)
		{
		case 0x00:
			ReceiveKanturu3rdStateInfo(ReceiveBuffer);
			break;
		case 0x01:
			ReceiveKanturu3rdEnterBossMap(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveKanturu3rdCurrentState(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveKanturu3rdState(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveKanturu3rdResult(ReceiveBuffer);
			break;
		case 0x05:
			ReceiveKanturu3rdTimer(ReceiveBuffer);
			break;
		case 0x06:
			RecevieKanturu3rdMayaSKill(ReceiveBuffer);
			break;
		case 0x07:
			RecevieKanturu3rdLeftUserandMonsterCount(ReceiveBuffer);
			break;
		case 0x10:
			ReceiveRaklionStateInfo(ReceiveBuffer);
			break;
		case 0x11:
			ReceiveRaklionCurrentState(ReceiveBuffer);
			break;
		case 0x12:
			RecevieRaklionStateChange(ReceiveBuffer);
			break;
		case 0x13:
			RecevieRaklionBattleResult(ReceiveBuffer);
			break;
		case 0x14:
			RecevieRaklionWideAreaAttack(ReceiveBuffer);
			break;
		case 0x15:
			RecevieRaklionUserMonsterCount(ReceiveBuffer);
			break;
		}
		break;
	}
	case 0xBF:
	{
		int subcode;
		if (ReceiveBuffer[0] == 0xC1)
		{
			LPPHEADER_DEFAULT_SUBCODE Data = (LPPHEADER_DEFAULT_SUBCODE)ReceiveBuffer;
			subcode = Data->SubCode;
		}
		else
		{
			LPPHEADER_DEFAULT_SUBCODE_WORD Data = (LPPHEADER_DEFAULT_SUBCODE_WORD)ReceiveBuffer;
			subcode = Data->SubCode;
		}

		switch (subcode)
		{
		case 0x00:
			ReceiveCursedTempleEnterResult(ReceiveBuffer);
			break;
		case 0x01:
			ReceiveCursedTempleInfo(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveCursedTempMagicResult(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveCursedTempleEnterInfo(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveCursedTempleGameResult(ReceiveBuffer);
			break;
		case 0x06:
			ReceiveCursedTempSkillPoint(ReceiveBuffer);
			break;
		case 0x07:
			ReceiveCursedTempSkillEnd(ReceiveBuffer);
			break;
		case 0x08:
			ReceiveCursedTempleHolyItemRelics(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveCursedTempleState(ReceiveBuffer);
			break;
		case 0x0a:
			ReceiveChainMagic(ReceiveBuffer);
			break;
		case 0x0B:
			ReceiveRegistedLuckyCoin(ReceiveBuffer);
			break;
		case 0x0C:
			ReceiveRegistLuckyCoin(ReceiveBuffer);
			break;
		case 0x0D:
			ReceiveRequestExChangeLuckyCoin(ReceiveBuffer);
			break;
		case 0x0E:
			ReceiveEnterDoppelGangerEvent(ReceiveBuffer);
			break;
		case 0x0F:
			ReceiveDoppelGangerMonsterPosition(ReceiveBuffer);
			break;
		case 0x10:
			ReceiveDoppelGangerState(ReceiveBuffer);
			break;
		case 0x11:
			ReceiveDoppelGangerIcewalkerState(ReceiveBuffer);
			break;
		case 0x12:
			ReceiveDoppelGangerTimePartyState(ReceiveBuffer);
			break;
		case 0x13:
			ReceiveDoppelGangerResult(ReceiveBuffer);
			break;
		case 0x14:
			ReceiveDoppelGangerMonsterGoal(ReceiveBuffer);
			break;
#ifdef PBG_ADD_SECRETBUFF
		case 0x15:
			ReceiveFatigueTime(ReceiveBuffer);
			break;
#endif //PBG_ADD_SECRETBUFF
#ifdef LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
		case 0x20:
			ReceiveEquippingInventoryItem(ReceiveBuffer);
			break;
#endif //LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
		case 0x51:
			ReceiveHelperResult(ReceiveBuffer);
			break;
		}
	}
	break;
	case 0xDE:
	{
		int subcode = 0;
		if (ReceiveBuffer[0] == 0xC1)
		{
			subcode = *(BYTE*)(ReceiveBuffer + 3);
		}
		else
		{
			subcode = *(BYTE*)(ReceiveBuffer + 4);
		}

		switch (subcode)
		{
		case 0x00:
			ReceiveCharacterCard_New(ReceiveBuffer);
			break;
		}
	}
	break;

#ifdef KJH_PBG_ADD_INGAMESHOP_SYSTEM
	case 0xD2:
	{
		PBMSG_HEADER2* Data = (PBMSG_HEADER2*)ReceiveBuffer;
		switch (Data->m_bySubCode)
		{
		case 0x01:
			ReceiveIGS_CashPoint(ReceiveBuffer);
			break;
		case 0x02:
			ReceiveIGS_ShopOpenResult(ReceiveBuffer);
			break;
		case 0x03:
			ReceiveIGS_BuyItem(ReceiveBuffer);
			break;
		case 0x04:
			ReceiveIGS_SendItemGift(ReceiveBuffer);
			break;
		case 0x06:
			ReceiveIGS_StorageItemListCount(ReceiveBuffer);
			break;
		case 0x07:
			ReceiveIGS_SendCashGift(ReceiveBuffer);
			break;
		case 0x08:
			ReceiveIGS_PossibleBuy(ReceiveBuffer);
			break;
		case 0x09:
			ReceiveIGS_LeftCountItem(ReceiveBuffer);
			break;
		case 0x0B:
			ReceiveIGS_UseStorageItem(ReceiveBuffer);
			break;
		case 0x0C:
			ReceiveIGS_UpdateScript(ReceiveBuffer);
			break;
		case 0x0D:
			ReceiveIGS_StorageItemList(ReceiveBuffer);
			break;
		case 0x0E:
			ReceiveIGS_StorageGiftItemList(ReceiveBuffer);
			break;
		case 0x13:
			ReceiveIGS_EventItemlistCnt(ReceiveBuffer);
			break;
		case 0x14:
			ReceiveIGS_EventItemlist(ReceiveBuffer);
			break;
		case 0x15:
			ReceiveIGS_UpdateBanner(ReceiveBuffer);
			break;
#ifdef KJH_ADD_PERIOD_ITEM_SYSTEM
		case 0x11:
			ReceivePeriodItemListCount(ReceiveBuffer);
			break;
		case 0x12:
			ReceivePeriodItemList(ReceiveBuffer);
			break;
#endif // KJH_ADD_PERIOD_ITEM_SYSTEM
		}
	}break;

#endif // KJH_PBG_ADD_INGAMESHOP_SYSTEM	
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
	case 0x4A:
		ReceiveStraightAttack(ReceiveBuffer, Size, bEncrypted);
		break;
	case 0x4B:
		ReceiveDarkside(ReceiveBuffer);
		break;
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
	default:
		break;
	}

	return (TRUE);
}

void TranslateChattingProtocol(DWORD dwWindowUIID, int HeadCode, BYTE* ReceiveBuffer, int Size, BOOL bEcrypted)
{
	switch (HeadCode)
	{
	case 0x00:
		ReceiveChatRoomConnectResult(dwWindowUIID, ReceiveBuffer);
		break;
	case 0x01:
		ReceiveChatRoomUserStateChange(dwWindowUIID, ReceiveBuffer);
		break;
	case 0x02:
		ReceiveChatRoomUserList(dwWindowUIID, ReceiveBuffer);
		break;
	case 0x04:
		ReceiveChatRoomChatText(dwWindowUIID, ReceiveBuffer);
		break;
	case 0x0D:
		ReceiveChatRoomNoticeText(dwWindowUIID, ReceiveBuffer);
		break;
	default:
		break;
	}
}

bool CheckExceptionBuff(eBuffState buff, OBJECT* o, bool iserase)
{
	if (iserase)
	{
		switch (buff)
		{
		case eBuff_SoulPotion:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterUnRegisterBuff(o, eBuff_CrywolfAltarEnable);
				return false;
			}
		}
		break;

		case eBuff_RemovalMagic:
		{
			std::list<eBuffState> bufflist;

			//debuff
			bufflist.push_back(eDeBuff_Poison);
			bufflist.push_back(eDeBuff_Freeze);
			bufflist.push_back(eDeBuff_Harden);
			bufflist.push_back(eDeBuff_Defense);
			bufflist.push_back(eDeBuff_Stun);
			bufflist.push_back(eDeBuff_Sleep);
			bufflist.push_back(eDeBuff_BlowOfDestruction);

			//buff
			bufflist.push_back(eBuff_HpRecovery);
			bufflist.push_back(eBuff_Attack);
			//bufflist.push_back( eBuff_Life );
			bufflist.push_back(eBuff_Attack);
			bufflist.push_back(eBuff_Defense);
			bufflist.push_back(eBuff_AddAG);
			bufflist.push_back(eBuff_Cloaking);
			bufflist.push_back(eBuff_AddSkill);
			bufflist.push_back(eBuff_PhysDefense);
			bufflist.push_back(eBuff_AddCriticalDamage);
			bufflist.push_back(eBuff_CrywolfAltarOccufied);

			g_CharacterUnRegisterBuffList(o, bufflist);
		}
		break;
		}

		return true;
	}
	else
	{
		switch (buff)
		{
		case eBuff_CastleRegimentAttack3:
		{
			g_CharacterUnRegisterBuff(o, eBuff_CastleRegimentDefense);
		}
		break;

		case eBuff_SoulPotion:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterRegisterBuff(o, eBuff_CrywolfAltarEnable);
				return false;
			}
		}
		break;

		case eBuff_CastleGateIsOpen:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterRegisterBuff(o, eBuff_CrywolfAltarDisable);
				return false;
			}
		}
		break;

		case eBuff_CastleRegimentDefense:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterRegisterBuff(o, eBuff_CrywolfAltarContracted);
				return false;
			}
			else
			{
				g_TokenCharacterBuff(o, eBuff_CastleRegimentDefense);
				return false;
			}
		}
		break;

		case eBuff_CastleRegimentAttack1:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterRegisterBuff(o, eBuff_CrywolfAltarAttempt);
				return false;
			}
			else if (g_isCharacterBuff(o, eBuff_CastleRegimentDefense))
			{
				g_CharacterUnRegisterBuff(o, eBuff_CastleRegimentDefense);
			}
		}
		break;

		case eBuff_RemovalMagic:
		{
			if (o->Type >= MODEL_CRYWOLF_ALTAR1 && o->Type <= MODEL_CRYWOLF_ALTAR5)
			{
				g_CharacterRegisterBuff(o, eBuff_CrywolfAltarOccufied);
				return false;
			}
		}
		break;

		case eBuff_CastleRegimentAttack2:
		{
			if (g_isCharacterBuff(o, eBuff_CastleRegimentDefense))
				g_CharacterUnRegisterBuff(o, eBuff_CastleRegimentDefense);
		}
		break;
		}

		return true;
	}
}

void InsertBuffLogicalEffect(eBuffState buff, OBJECT* o, const int bufftime)
{
	if (o && o == &Hero->Object)
	{
		switch (buff)
		{
		case eBuff_Hellowin1:
		case eBuff_Hellowin2:
		case eBuff_Hellowin3:
		case eBuff_Hellowin4:
		case eBuff_Hellowin5:
		{
			g_RegisterBuffTime(buff, bufftime);

			if (buff == eBuff_Hellowin1)
			{
				CharacterMachine->CalculateAttackSpeed();
			}
			if (buff == eBuff_Hellowin2)
			{
				CharacterMachine->CalculateDamage();
			}
			if (buff == eBuff_Hellowin3)
			{
				CharacterMachine->CalculateDefense();
			}
		}
		break;
		case eBuff_PcRoomSeal1:
		case eBuff_PcRoomSeal2:
		case eBuff_PcRoomSeal3:
		{
			g_RegisterBuffTime(buff, bufftime);
		}
		break;
		case eBuff_Seal1:
		case eBuff_Seal2:
		case eBuff_Seal3:
		case eBuff_Seal4:
		case eBuff_Seal_HpRecovery:
		case eBuff_Seal_MpRecovery:
		case eBuff_GuardCharm:
		case eBuff_ItemGuardCharm:
		case eBuff_AscensionSealMaster:
		case eBuff_WealthSealMaster:
		case eBuff_NewWealthSeal:
		case eBuff_AG_Addition:
		case eBuff_SD_Addition:
		case eBuff_PartyExpBonus:
		{
			g_RegisterBuffTime(buff, bufftime);
		}
		break;
		case eBuff_EliteScroll1:
		case eBuff_EliteScroll2:
		case eBuff_EliteScroll3:
		case eBuff_EliteScroll4:
		case eBuff_EliteScroll5:
		case eBuff_EliteScroll6:
		case eBuff_Scroll_Battle:
		case eBuff_Scroll_Strengthen:
		case eBuff_Scroll_Healing:
		{
			g_RegisterBuffTime(buff, bufftime);

			if (buff == eBuff_EliteScroll1)
			{
				CharacterMachine->CalculateAttackSpeed();
			}
			else if (buff == eBuff_EliteScroll2)
			{
				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_EliteScroll3)
			{
				CharacterMachine->CalculateDamage();
			}
			else if (buff == eBuff_EliteScroll4)
			{
				CharacterMachine->CalculateMagicDamage();
			}
		}
		break;
		case eBuff_SecretPotion1:
		case eBuff_SecretPotion2:
		case eBuff_SecretPotion3:
		case eBuff_SecretPotion4:
		case eBuff_SecretPotion5:
		{
			g_RegisterBuffTime(buff, bufftime);

			CharacterMachine->CalculateAll();
		}
		break;
		case eBuff_CherryBlossom_Liguor:
		case eBuff_CherryBlossom_RiceCake:
		case eBuff_CherryBlossom_Petal:
		{
			g_RegisterBuffTime(buff, bufftime);

			if (buff == eBuff_CherryBlossom_Petal)
			{
				CharacterMachine->CalculateDamage();
			}
		}
		break;
		case eBuff_SwellOfMagicPower:
		case EFFECT_MAGIC_CIRCLE_IMPROVED:
		case EFFECT_MAGIC_CIRCLE_ENHANCED:
		{
			g_RegisterBuffTime(buff, bufftime);

			if (buff == eBuff_SwellOfMagicPower)
			{
				CharacterMachine->CalculateMagicDamage();
			}
		}break;
		case eBuff_BlessingOfXmax:
		case eBuff_CureOfSanta:
		case eBuff_SafeGuardOfSanta:
		case eBuff_StrengthOfSanta:
		case eBuff_DefenseOfSanta:
		case eBuff_QuickOfSanta:
		case eBuff_LuckOfSanta:
		{
			g_RegisterBuffTime(buff, bufftime);

			char _Temp[64] = { 0, };

			if (buff == eBuff_BlessingOfXmax)
			{
				g_pChatListBox->AddText("", GlobalText[2591], SEASON3B::TYPE_SYSTEM_MESSAGE);
				CharacterMachine->CalculateDamage();
				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_StrengthOfSanta)
			{
				sprintf(_Temp, GlobalText[2594], 30);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);

				CharacterMachine->CalculateDamage();
			}
			else if (buff == eBuff_DefenseOfSanta)
			{
				sprintf(_Temp, GlobalText[2595], 100);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);

				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_QuickOfSanta)
			{
				sprintf(_Temp, GlobalText[2598], 15);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);

				CharacterMachine->CalculateAttackSpeed();
			}
			else if (buff == eBuff_LuckOfSanta)
			{
				sprintf(_Temp, GlobalText[2599], 10);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
			else if (buff == eBuff_CureOfSanta)
			{
				sprintf(_Temp, GlobalText[2592], 500);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
			else if (buff == eBuff_SafeGuardOfSanta)
			{
				sprintf(_Temp, GlobalText[2593], 500);
				g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
		}
		break;
		case eBuff_DuelWatch:
		{
			g_pNewUISystem->HideAll();
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MAINFRAME);
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_BUFF_WINDOW);
			g_pNewUISystem->Show(SEASON3B::INTERFACE_DUELWATCH_MAINFRAME);
			g_pNewUISystem->Show(SEASON3B::INTERFACE_DUELWATCH_USERLIST);
		}
		break;
		case eBuff_HonorOfGladiator:
		{
			g_RegisterBuffTime(buff, bufftime);
		}
		break;
		}

		if (bufftime > 0)
		{
			g_UnRegisterBuffTime(buff);
			g_RegisterBuffTime(buff, bufftime);
		}
	}
}

void ClearBuffLogicalEffect(eBuffState buff, OBJECT* o)
{
	if (o && o == &Hero->Object)
	{
		switch (buff)
		{
		case eBuff_Hellowin1:
		case eBuff_Hellowin2:
		case eBuff_Hellowin3:
		case eBuff_Hellowin4:
		case eBuff_Hellowin5:
		{
			g_UnRegisterBuffTime(buff);

			if (buff == eBuff_Hellowin1)
			{
				CharacterMachine->CalculateAttackSpeed();
			}
			else if (buff == eBuff_Hellowin2)
			{
				int iBaseClass = gCharacterManager.GetBaseClass(Hero->Class);

				if (iBaseClass == CLASS_WIZARD || iBaseClass == CLASS_DARK || iBaseClass == CLASS_SUMMONER)
				{
					CharacterMachine->CalculateMagicDamage();
				}
				else
				{
					CharacterMachine->CalculateDamage();
				}
			}
			else if (buff == eBuff_Hellowin3)
			{
				CharacterMachine->CalculateDefense();
			}
		}
		break;
		case eBuff_PcRoomSeal1:
		case eBuff_PcRoomSeal2:
		case eBuff_PcRoomSeal3:
		{
			g_UnRegisterBuffTime(buff);
		}
		break;
		case eBuff_Seal1:
		case eBuff_Seal2:
		case eBuff_Seal3:
		case eBuff_Seal4:
		case eBuff_Seal_HpRecovery:
		case eBuff_Seal_MpRecovery:
		case eBuff_GuardCharm:
		case eBuff_ItemGuardCharm:
		case eBuff_AscensionSealMaster:
		case eBuff_WealthSealMaster:
		case eBuff_NewWealthSeal:
		case eBuff_AG_Addition:
		case eBuff_SD_Addition:
		case eBuff_PartyExpBonus:
		{
			g_UnRegisterBuffTime(buff);
		}
		break;
		case eBuff_EliteScroll1:
		case eBuff_EliteScroll2:
		case eBuff_EliteScroll3:
		case eBuff_EliteScroll4:
		case eBuff_EliteScroll5:
		case eBuff_EliteScroll6:
		case eBuff_Scroll_Battle:
		case eBuff_Scroll_Strengthen:
		case eBuff_Scroll_Healing:
		{
			g_UnRegisterBuffTime(buff);

			if (buff == eBuff_EliteScroll1)
			{
				CharacterMachine->CalculateAttackSpeed();
			}
			else if (buff == eBuff_EliteScroll2)
			{
				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_EliteScroll3)
			{
				CharacterMachine->CalculateDamage();
			}
			else if (buff == eBuff_EliteScroll4)
			{
				CharacterMachine->CalculateMagicDamage();
			}
		}
		break;
		case eBuff_SecretPotion1:
		case eBuff_SecretPotion2:
		case eBuff_SecretPotion3:
		case eBuff_SecretPotion4:
		case eBuff_SecretPotion5:
		{
			g_UnRegisterBuffTime(buff);

			CharacterMachine->CalculateAll();
		}
		break;
		case eBuff_CherryBlossom_Liguor:
		case eBuff_CherryBlossom_RiceCake:
		case eBuff_CherryBlossom_Petal:
		{
			g_UnRegisterBuffTime(buff);

			if (buff == eBuff_CherryBlossom_Petal)
			{
				CharacterMachine->CalculateDamage();
			}
		}
		break;
		case eBuff_SwellOfMagicPower:
		case EFFECT_MAGIC_CIRCLE_IMPROVED:
		case EFFECT_MAGIC_CIRCLE_ENHANCED:
		{
			g_UnRegisterBuffTime(buff);

			CharacterMachine->CalculateMagicDamage();
		}
		break;
		case eBuff_BlessingOfXmax:
		case eBuff_CureOfSanta:
		case eBuff_SafeGuardOfSanta:
		case eBuff_StrengthOfSanta:
		case eBuff_DefenseOfSanta:
		case eBuff_QuickOfSanta:
		case eBuff_LuckOfSanta:
		{
			g_UnRegisterBuffTime(buff);

			if (buff == eBuff_BlessingOfXmax)
			{
				CharacterMachine->CalculateDamage();
				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_StrengthOfSanta)
			{
				CharacterMachine->CalculateDamage();
			}
			else if (buff == eBuff_DefenseOfSanta)
			{
				CharacterMachine->CalculateDefense();
			}
			else if (buff == eBuff_QuickOfSanta)
			{
				CharacterMachine->CalculateAttackSpeed();
			}
		}
		break;
		case eBuff_DuelWatch:
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_DUELWATCH_MAINFRAME);
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_DUELWATCH_USERLIST);
			g_pNewUISystem->Show(SEASON3B::INTERFACE_MAINFRAME);
			g_pNewUISystem->Show(SEASON3B::INTERFACE_BUFF_WINDOW);
		}
		break;
		case eBuff_HonorOfGladiator:
		{
			g_UnRegisterBuffTime(buff);
		}
		break;
		}
		g_UnRegisterBuffTime(buff);
	}
}

void InsertBuffPhysicalEffect(eBuffState buff, OBJECT* o)
{
	switch (buff)
	{
	case eBuff_CursedTempleProdection:
	{
		if (o->Type == MODEL_PLAYER)
		{
			DeleteEffect(MODEL_CURSEDTEMPLE_PRODECTION_SKILL, o);
			DeleteEffect(MODEL_SHIELD_CRASH, o);
			DeleteEffect(BITMAP_SHOCK_WAVE, o);
			vec3_t  Light;
			Vector(0.3f, 0.3f, 0.8f, Light);
			CreateEffect(MODEL_CURSEDTEMPLE_PRODECTION_SKILL, o->Position, o->Angle, Light, 0, o);
			CreateEffect(MODEL_SHIELD_CRASH, o->Position, o->Angle, Light, 1, o);
			CreateEffect(BITMAP_SHOCK_WAVE, o->Position, o->Angle, Light, 10, o);
		}
	}
	break;
	case eBuff_PhysDefense:
	{
		if (o->Type == MODEL_PLAYER)
		{
			PlayBuffer(SOUND_SOULBARRIER);
			DeleteJoint(MODEL_SPEARSKILL, o, 0);
			for (int j = 0; j < 4; ++j)
			{
				CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 0, o, 50.0f);
			}
		}
	}
	break;
	case eBuff_AddAG:
	{
		DeleteEffect(BITMAP_LIGHT, o, 2);
		CreateEffect(BITMAP_LIGHT, o->Position, o->Angle, o->Light, 2, o);
	}
	break;

	case eBuff_HpRecovery:
	case EFFECT_GREATER_LIFE_ENHANCED:
	case EFFECT_GREATER_LIFE_MASTERED:
	{
		DeleteEffect(BITMAP_LIGHT, o, 1);
		CreateEffect(BITMAP_LIGHT, o->Position, o->Angle, o->Light, 1, o);
	}
	break;
	case eDeBuff_Harden:
	{
		vec3_t Angle;
		VectorCopy(o->Angle, Angle);

		DeleteEffect(MODEL_ICE, o, 1);
		CreateEffect(MODEL_ICE, o->Position, Angle, o->Light, 1, o);

		Angle[2] += 180.f;
		CreateEffect(MODEL_ICE, o->Position, Angle, o->Light, 2, o);
	}
	break;
	case eDeBuff_Defense:
	{
		vec3_t Light = { 1.0f, 1.f, 1.f };
		DeleteEffect(BITMAP_SKULL, o, 0);
		CreateEffect(BITMAP_SKULL, o->Position, o->Angle, Light, 0, o);

		PlayBuffer(SOUND_BLOODATTACK, o);
	}
	break;
	case eDeBuff_Stun:
	{
		DeleteEffect(BITMAP_SKULL, o, 5);

		vec3_t Angle;
		vec3_t Position;
		VectorCopy(o->Position, Position);
		Vector(0.f, 0.f, 90.f, Angle);
		CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 8, o, 30.0f);
		Position[2] -= 10.f;
		Vector(0.f, 0.f, 180.f, Angle);
		CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 8, o, 30.0f);
		Position[2] -= 10.f;
		Vector(0.f, 0.f, 270.f, Angle);
		CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 8, o, 30.0f);
	}
	break;
	case eDeBuff_NeilDOT:
	{
		g_SummonSystem.CreateDamageOfTimeEffect(AT_SKILL_SUMMON_REQUIEM, o);
	}
	break;
	case eDeBuff_SahamuttDOT:
	{
		g_SummonSystem.CreateDamageOfTimeEffect(AT_SKILL_SUMMON_EXPLOSION, o);
	}
	break;
	case eBuff_Thorns:
	{
		vec3_t vLight;
		Vector(0.9f, 0.6f, 0.1f, vLight);
		CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 43, o, 50.f, 0, 0, 0, 0, vLight);
	}
	break;
	case eDeBuff_Blind:
	{
		vec3_t vLight;
		Vector(1.0f, 1.0f, 1.0f, vLight);
		CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o->Position, o->Angle, vLight, 3, o);
	}
	break;
	case eDeBuff_Sleep:
	{
		vec3_t vLight;
		Vector(0.7f, 0.1f, 0.9f, vLight);
		CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o->Position, o->Angle, vLight, 4, o);
	}
	break;
	case eDeBuff_AttackDown:
	{
		vec3_t vLight;
		Vector(1.4f, 0.2f, 0.2f, vLight);
		CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, vLight, 1, o, -1, 0, 0, 0, 0.5f);
		CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, vLight, 1, o, -1, 0, 0, 0, 1.f);
	}
	break;

	case eDeBuff_DefenseDown:
	{
		vec3_t vLight;
		Vector(0.25f, 1.0f, 0.7f, vLight);
		CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, vLight, 2, o, -1, 0, 0, 0, 0.5f);
		CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, vLight, 2, o, -1, 0, 0, 0, 1.f);
	}
	break;
	case eBuff_Berserker:
	{
		vec3_t vLight[2];
		Vector(0.9f, 0.0f, 0.1f, vLight[0]);
		Vector(1.0f, 1.0f, 1.0f, vLight[1]);
		for (int i = 0; i < 4; ++i)
		{
			DeleteEffect(BITMAP_ORORA, o, i);
			CreateEffect(BITMAP_ORORA, o->Position, o->Angle, vLight[0], i, o);
			if (i == 2 || i == 3)
			{
				DeleteEffect(BITMAP_SPARK + 2, o, i);
				CreateEffect(BITMAP_SPARK + 2, o->Position, o->Angle, vLight[1], i, o);
			}
		}
		DeleteEffect(BITMAP_LIGHT_MARKS, o);
		CreateEffect(BITMAP_LIGHT_MARKS, o->Position, o->Angle, vLight[0], 0, o);
	}
	break;
	case eDeBuff_FlameStrikeDamage:
	{
		CreateEffect(BITMAP_FIRE_CURSEDLICH, o->Position, o->Angle, o->Light, 2, o);
	}
	break;
	case eDeBuff_GiganticStormDamage:
	{
		DeleteEffect(BITMAP_JOINT_THUNDER, o, 0);
		CreateEffect(BITMAP_JOINT_THUNDER, o->Position, o->Angle, o->Light, 0, o);
	}
	break;
	case eDeBuff_LightningShockDamage:
	{
		CreateEffect(MODEL_LIGHTNING_SHOCK, o->Position, o->Angle, o->Light, 2, o);
	}
	break;
	case eDeBuff_BlowOfDestruction:
	{
		CreateEffect(MODEL_ICE, o->Position, o->Angle, o->Light);
	}
	break;
	case eBuff_Doppelganger_Ascension:
	{
		PlayBuffer(SOUND_SKILL_BERSERKER);
		CreateEffect(MODEL_CHANGE_UP_EFF, o->Position, o->Angle, o->Light, 2, o);
		CreateEffect(MODEL_CHANGE_UP_CYLINDER, o->Position, o->Angle, o->Light, 2, o);
		CreateEffect(MODEL_INFINITY_ARROW3, o->Position, o->Angle, o->Light, 1, o);
	}
	break;
	case eBuff_SD_Addition:
	{
		DeleteEffect(MODEL_EFFECT_SD_AURA, o);
		CreateEffect(MODEL_EFFECT_SD_AURA, o->Position, o->Angle, o->Light, 0, o);
	}
	break;
	case eBuff_AG_Addition:
	{
		DeleteParticle(BITMAP_AG_ADDITION_EFFECT);

		for (int i = 0; i < 5; ++i)
		{
			CreateParticle(BITMAP_AG_ADDITION_EFFECT, o->Position, o->Angle, o->Light, 0, 1.0f, o);
			CreateParticle(BITMAP_AG_ADDITION_EFFECT, o->Position, o->Angle, o->Light, 1, 1.0f, o);
			CreateParticle(BITMAP_AG_ADDITION_EFFECT, o->Position, o->Angle, o->Light, 2, 1.0f, o);
		}
	}
	break;
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
	case eBuff_Att_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 1);
		CreateEffect(BITMAP_LIGHT_RED, o->Position, o->Angle, o->Light, 1, o, -1, 0, 0, 0, 1.5f);
	}
	break;
	case eBuff_Hp_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 0);
		CreateEffect(BITMAP_LIGHT_RED, o->Position, o->Angle, o->Light, 0, o, -1, 0, 0, 0, 1.5f);
	}
	break;
	case eBuff_Def_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 2);
		CreateEffect(BITMAP_LIGHT_RED, o->Position, o->Angle, o->Light, 2, o, -1, 0, 0, 0, 1.5f);
	}
	break;
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
	}
}

void ClearBuffPhysicalEffect(eBuffState buff, OBJECT* o)
{
	switch (buff)
	{
	case eBuff_CursedTempleProdection:
	{
		if (g_isCharacterBuff(o, eBuff_CursedTempleProdection))
		{
			DeleteEffect(MODEL_CURSEDTEMPLE_PRODECTION_SKILL, o);
		}
	}
	break;

	case eBuff_Defense:
	{
		DeleteJoint(MODEL_SPEARSKILL, o, 4);
		DeleteJoint(MODEL_SPEARSKILL, o, 9);
	}
	break;

	case eBuff_AddAG:
	{
		DeleteEffect(BITMAP_LIGHT, o, 2);
	}
	break;

	case eBuff_HpRecovery:
	case EFFECT_GREATER_LIFE_ENHANCED:
	case EFFECT_GREATER_LIFE_MASTERED:
	{
		DeleteEffect(BITMAP_LIGHT, o, 1);
	}
	break;

	case eBuff_PhysDefense:
	{
		if (o->Type == MODEL_PLAYER)
		{
			DeleteJoint(MODEL_SPEARSKILL, o, 0);
		}
	}
	break;

	case eBuff_HelpNpc:
	{
		DeleteJoint(MODEL_SPEARSKILL, o, 4);
		DeleteJoint(MODEL_SPEARSKILL, o, 9);
	}
	break;

	case eBuff_RemovalMagic:
	{
		DeleteEffect(BITMAP_LIGHT, o, 2);
		DeleteEffect(BITMAP_LIGHT, o, 1);
		DeleteEffect(BITMAP_SKULL, o, 0);
		DeleteJoint(MODEL_SPEARSKILL, o, 4);
		DeleteJoint(MODEL_SPEARSKILL, o, 9);
		DeleteJoint(MODEL_SPEARSKILL, o, 0);
	}
	break;
	case eDeBuff_CursedTempleRestraint:
	{
		if (g_isCharacterBuff(o, eDeBuff_CursedTempleRestraint))
		{
			DeleteEffect(MODEL_CURSEDTEMPLE_RESTRAINT_SKILL, o);
		}
	}
	break;

	case eDeBuff_Defense:
	{
		DeleteEffect(BITMAP_SKULL, o, 0);
	}
	break;

	case eDeBuff_Stun:
	{
		DeleteEffect(BITMAP_SKULL, o, 5);
	}
	break;

	case eDeBuff_NeilDOT:
	{
		g_SummonSystem.RemoveDamageOfTimeEffect(AT_SKILL_SUMMON_REQUIEM, o);
	}
	break;

	case eDeBuff_SahamuttDOT:
	{
		g_SummonSystem.RemoveDamageOfTimeEffect(AT_SKILL_SUMMON_EXPLOSION, o);
	}
	break;

	case eBuff_Thorns:
	{
		DeleteJoint(BITMAP_FLARE, o, 43);
	}
	break;

	case eDeBuff_Sleep:
	{
		DeleteEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 4);
	}
	break;

	case eDeBuff_Blind:
	{
		DeleteEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 3);
	}
	break;

	case eDeBuff_AttackDown:
		DeleteEffect(BITMAP_SHINY + 6, o, 1);
		DeleteEffect(BITMAP_PIN_LIGHT, o, 1);
		break;

	case eDeBuff_DefenseDown:
		DeleteEffect(BITMAP_SHINY + 6, o, 2);
		DeleteEffect(BITMAP_PIN_LIGHT, o, 2);
		break;
	case eBuff_Berserker:
	{
		for (int i = 0; i < 4; ++i)
		{
			DeleteEffect(BITMAP_ORORA, o, i);
			if (i == 2 || i == 3)
				DeleteEffect(BITMAP_SPARK + 2, o, i);
		}
		DeleteEffect(BITMAP_LIGHT_MARKS, o);
	}
	break;
	case eDeBuff_FlameStrikeDamage:
	{
		// 			DeleteEffect(BITMAP_FIRE_CURSEDLICH, o, 2);
	}
	break;
	case eDeBuff_GiganticStormDamage:
	{
		// 			DeleteEffect(BITMAP_JOINT_THUNDER, o, 0);
	}
	break;
	case eDeBuff_LightningShockDamage:
	{
		// 			DeleteEffect(MODEL_LIGHTNING_SHOCK, o, 2);
	}
	break;
	case eDeBuff_BlowOfDestruction:
	{

	}
	break;
	case eBuff_SwellOfMagicPower:
	case EFFECT_MAGIC_CIRCLE_IMPROVED:
	case EFFECT_MAGIC_CIRCLE_ENHANCED:
	{
		DeleteEffect(MODEL_SWELL_OF_MAGICPOWER_BUFF_EFF, o, 0);
	}break;
	case eBuff_Doppelganger_Ascension:
	{
		// 			DeleteEffect(BITMAP_JOINT_THUNDER, o, 0);
	}
	break;
	case eBuff_SD_Addition:
	{
		DeleteEffect(MODEL_EFFECT_SD_AURA, o);
	}
	break;
	case eBuff_AG_Addition:
	{
		DeleteParticle(BITMAP_AG_ADDITION_EFFECT);
	}
	break;
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
	case eBuff_Att_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 1);
	}
	break;
	case eBuff_Hp_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 0);
	}
	break;
	case eBuff_Def_up_Ourforces:
	{
		DeleteEffect(BITMAP_LIGHT_RED, o, 2);
	}
	break;
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
	}
}

void RegisterBuff(eBuffState buff, OBJECT* o, const int bufftime)
{
	eBuffClass buffclasstype = g_IsBuffClass(buff);

	if (buffclasstype == eBuffClass_Count) return;

	if (!o)
	{
		return;
	}

	if (gMapManager->InChaosCastle() && (o && o != &Hero->Object))
	{
		return;
	}

	InsertBuffPhysicalEffect(buff, o);

	if (CheckExceptionBuff(buff, o, false))
	{
		g_CharacterRegisterBuff(o, buff);
		InsertBuffLogicalEffect(buff, o, bufftime);
	}
}

void UnRegisterBuff(eBuffState buff, OBJECT* o)
{
	eBuffClass buffclasstype = g_IsBuffClass(buff);

	if (buffclasstype == eBuffClass_Count)
		return;

	ClearBuffPhysicalEffect(buff, o);

	if (CheckExceptionBuff(buff, o, true))
	{
		g_CharacterUnRegisterBuff(o, buff);
		ClearBuffLogicalEffect(buff, o);
	}
}

