///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CGMProtect.h"
#ifndef __ANDROID__
#include <imm.h>
#endif
#include "UIManager.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzLodTerrain.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzTexture.h"
#include "ZzzAI.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "ZzzOpenData.h"
#include "ZzzEffect.h"
#include "ZzzScene.h"
#include "ZzzPath.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "./Utilities/Log/ErrorReport.h"
#include "MatchEvent.h"
#include "CSQuest.h"
#include "PersonalShopTitleImp.h"
#include "zzzopenglUtil.h"
#include "CSItemOption.h"
#include "npcBreeder.h"
#include "GIPetManager.h"
#include "CSParts.h"
#include "GMBattleCastle.h"
#include "GMCrywolf1st.h"
#include "UIMapName.h"	// rozy
#include "GMCrywolf1st.h"
#include "CDirection.h"
#include "GM_Kanturu_3rd.h"
#include "GM_Kanturu_2nd.h"
#include "MapManager.h"
#include "Event.h"
#include "./Utilities/Log/muConsoleDebug.h"
#include "NewUISystem.h"
#include "w_CursedTemple.h"
#include "UIControls.h"
#include "GM3rdChangeUp.h"
#include "PartyManager.h"
#include "NewUICommonMessageBox.h"
#include "SummonSystem.h"
#include "GMSwampOfQuiet.h"
#include "w_MapHeaders.h"
#include "DuelMgr.h"
#include "ChangeRingManager.h"
#include "NewUIGensRanking.h"
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
#include "MonkSystem.h"
#endif //PBG_ADD_NEWCHAR_MONK_SKILL

#include "ProtocolSend.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "ScaleForm.h"


int NoticeCount = 0;
int NoticeTime = 300;
int NoticeInverse = 0;
NOTICE Notice[MAX_NOTICE];


extern CUITextInputBox* g_pSingleTextInputBox;
extern CUITextInputBox* g_pSinglePasswdInputBox;
extern int g_iChatInputType;
extern BOOL g_bUseChatListBox;
extern DWORD g_dwMouseUseUIID;
extern CUIMapName* g_pUIMapName;	// rozy
extern bool bCheckNPC;

extern void SetPlayerBow(CHARACTER* c);
#ifdef _PVP_ADD_MOVE_SCROLL
extern CMurdererMove g_MurdererMove;
#endif	// _PVP_ADD_MOVE_SCROLL
extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern int  AlphaBlendType;
const int ciSystemColor = 240;
extern char* g_lpszMp3[NUM_MUSIC];
extern vec3_t MousePosition, MouseTarget;
extern void RegisterBuff(eBuffState buff, OBJECT* o, const int bufftime = 0);
extern void UnRegisterBuff(eBuffState buff, OBJECT* o);
extern BYTE m_CrywolfState;
MovementSkill g_MovementSkill;

const   float   AutoMouseLimitTime = (1.f * 60.f * 60.f);
int   LoadingWorld = 0;
int   ItemHelp = 0;
int   MouseUpdateTime = 0;
int   MouseUpdateTimeMax = 6;
bool  WhisperEnable = true;
bool  ChatWindowEnable = true;
int   InputFrame = 0;
int   EditFlag = EDIT_NONE;
char  ColorTable[][10] = { "White","Black","Red","Yellow","Green","Cyan","Blue","Magenta" };

int   SelectMonster = 0;
int   SelectModel = 0;
int   SelectMapping = 0;
int   SelectColor = 0;
int   SelectWall = 0;
float SelectMappingAngle = 0.f;
bool  DebugEnable = true;
int   SelectedItem = -1;
int   SelectedNpc = -1;
int   SelectedCharacter = -1;
int   SelectedOperate = -1;
int   Attacking = -1;

int   g_iFollowCharacter = -1;

bool g_bAutoGetItem = false;

float LButtonPopTime = 0.f;
float LButtonPressTime = 0.f;
float RButtonPopTime = 0.f;
float RButtonPressTime = 0.f;

//  
int   BrushSize = 0;
int   HeroTile;
int   TargetNpc = -1;
int   TargetType;
int TargetX;
int TargetY;
float TargetAngle;
int   FontHeight = -1;

OBJECT* TradeNpc = NULL;

bool  DontMove = false;
bool  ServerHide = true;
bool  SkillEnable;
bool  MouseOnWindow;
int   KeyState[256];

int    TerrainWallType[TERRAIN_SIZE * TERRAIN_SIZE];
float  TerrainWallAngle[TERRAIN_SIZE * TERRAIN_SIZE];
OBJECT* PickObject = NULL;
vec3_t PickObjectAngle;
float  PickObjectHeight;
bool   PickObjectLockHeight = false;
bool   EnableRandomObject = false;
float  WallAngle;

bool LockInputStatus = false;

bool GuildInputEnable = false;
bool TabInputEnable = false;
bool GoldInputEnable = false;
bool InputEnable = true;
bool g_bScratchTicket = false;

int  InputGold = 0;
int  InputNumber = 2;
int  InputTextWidth = 110;
int  InputIndex = 0;
int  InputResidentNumber = 6;
int  InputTextMax[12] = { MAX_ID_SIZE,MAX_ID_SIZE,MAX_ID_SIZE,30,30,10,14,20,40 };
char InputText[12][256];
char InputTextIME[12][4];
char InputTextHide[12];
int  InputLength[12];
char MacroText[10][256];
int MacroTime = 0;
int  WhisperIDCurrent = 2;
char WhisperID[MAX_WHISPER_ID][256];
DWORD g_dwOneToOneTick = 0;

bool g_bGMObservation = false;

BYTE DebugText[10][256];
int  DebugTextLength[10];
char DebugTextCount = 0;

int ItemKey = 0;
int ActionTarget = -1;

#ifdef LEM_FIX_USER_LOGOUT
bool g_bExit = false;
#endif // LEM_FIX_USER_LOGOUT [lem_2010.8.18]
bool EnableFastInput = false;
WORD g_byLastSkillSerialNumber = 0;


#ifdef PK_ATTACK_TESTSERVER_LOG
void PrintPKLog(CHARACTER* pCha)
{
	static DWORD dwStartTime = 0;
	static SHORT sKey = 0;

	if (pCha->Key == sKey && (timeGetTime() - dwStartTime < 1000))
	{
		return;
	}

	if (pCha)
	{
		if (pCha->PK >= PVP_MURDERER2 && pCha->Object.Type == KIND_PLAYER)
		{
			g_ErrorReport.Write("!!!!!!!!!!!!!!!!! PK !!!!!!!!!!!!!!!\n");
			g_ErrorReport.WriteCurrentTime();
			g_ErrorReport.Write(" ID(%s) PK(%d) GuildName(%s)\n", pCha->ID, pCha->PK, GuildMark[pCha->GuildMarkIndex].GuildName);
#ifdef CONSOLE_DEBUG
			g_ConsoleDebug->Write(MCD_ERROR, "[!!! PK !!! : ID(%s) PK(%d) GuildName(%s)\n]", pCha->ID, pCha->PK, GuildMark[pCha->GuildMarkIndex].GuildName);
#endif // CONSOLE_DEBUG

			sKey = pCha->Key;
			dwStartTime = timeGetTime();
		}
	}
}
#endif // PK_ATTACK_TESTSERVER_LOG

bool PressKey(int Key)
{
	if (SEASON3B::IsPress(Key))
	{
		if (KeyState[Key] == false)
		{
			KeyState[Key] = true;
			return true;
		}
	}
	else
	{
		KeyState[Key] = false;
	}

	return false;
}

void ClearInput(BOOL bClearWhisperTarget)
{
	InputIndex = 0;
	InputResidentNumber = -1;
	for (int i = 0; i < 10; i++)
	{
		if (i == 1 && bClearWhisperTarget == FALSE)
			continue;

		for (int j = 0; j < 256; j++)
			InputText[i][j] = NULL;
		InputLength[i] = 0;
		InputTextHide[i] = 0;
	}

	if (g_iChatInputType == 1)
	{
		g_pSingleTextInputBox->SetText(NULL);
		g_pSinglePasswdInputBox->SetText(NULL);
	}
}

void SetPositionIME_Wnd(float x, float y)
{
	float xRatio_Input = x / 640.f;
	float yRatio_Input = y / 480.f;

	COMPOSITIONFORM comForm;
	comForm.dwStyle = CFS_POINT;
	comForm.ptCurrentPos.x = (long)(WindowWidth * xRatio_Input);
	comForm.ptCurrentPos.y = (long)(WindowHeight * yRatio_Input);
	SetRect(&comForm.rcArea, 0, 0, WindowWidth, WindowHeight);

	HWND hWnd = ImmGetDefaultIMEWnd(gwinhandle->GethWnd());
	::SendMessage(hWnd, WM_IME_CONTROL, IMC_SETCOMPOSITIONWINDOW, (LPARAM)&comForm);
}

DWORD   g_dwOldConv = IME_CMODE_ALPHANUMERIC;
DWORD   g_dwOldSent = IME_SMODE_AUTOMATIC;
DWORD   g_dwCurrConv = 0;

void SaveIME_Status()
{
	HIMC  data;

	data = ::ImmGetContext(gwinhandle->GethWnd());

	::ImmGetConversionStatus(data, &g_dwOldConv, &g_dwOldSent);
	::ImmReleaseContext(gwinhandle->GethWnd(), data);
}

//  
void SetIME_Status(bool halfShape)
{
	if (g_iChatInputType == 1) return;

	HIMC  data;
	DWORD dwConv, dwSent;

	data = ::ImmGetContext(gwinhandle->GethWnd());

	//  �ݰ�.
	dwConv = g_dwOldConv;
	dwSent = g_dwOldSent;
	if (halfShape)
	{
		dwConv = IME_CMODE_ALPHANUMERIC;
		dwSent = IME_SMODE_NONE;
	}
	::ImmSetConversionStatus(data, dwConv, dwSent);
	::ImmReleaseContext(gwinhandle->GethWnd(), data);
}

bool CheckIME_Status(bool change, int mode)
{
	if (g_iChatInputType == 1) return false;

	HIMC  data;
	DWORD dwConv, dwSent;
	bool  bIme = false;

	data = ::ImmGetContext(gwinhandle->GethWnd());

	::ImmGetConversionStatus(data, &dwConv, &dwSent);

	if (dwConv != IME_CMODE_ALPHANUMERIC || dwSent != IME_SMODE_NONE)
	{
		bIme = true;

		if ((mode & IME_CONVERSIONMODE) == IME_CONVERSIONMODE)
		{
			g_dwOldConv = dwConv;
		}
		if ((mode & IME_SENTENCEMODE) == IME_SENTENCEMODE)
		{
			g_dwOldSent = dwSent;
		}

		if (change)
		{
			dwConv = IME_CMODE_ALPHANUMERIC;
			dwSent = IME_SMODE_NONE;
			::ImmSetConversionStatus(data, dwConv, dwSent);
		}
	}
	::ImmReleaseContext(gwinhandle->GethWnd(), data);

	g_dwCurrConv = dwConv;

	return   bIme;
}

void RenderIME_Status()
{
	char    Text[100];
	if ((g_dwOldConv & IME_CMODE_NATIVE) == IME_CMODE_NATIVE)
	{
		sprintf(Text, "ENGLISH");
	}
	else
	{
		sprintf(Text, "ENGLISH");
	}

	g_pRenderText->SetTextColor(255, 230, 210, 255);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->RenderText(100, 100, Text);

	HIMC  data;
	DWORD dwConv, dwSent;

	data = ::ImmGetContext(gwinhandle->GethWnd());

	::ImmGetConversionStatus(data, &dwConv, &dwSent);
	::ImmReleaseContext(gwinhandle->GethWnd(), data);

	sprintf(Text, "Sentence Mode = %d", dwSent);
	g_pRenderText->RenderText(100, 110, Text);

	sprintf(Text, "Old Sentence Mode = %d", g_dwOldSent);
	g_pRenderText->RenderText(100, 120, Text);

	sprintf(Text, "LockInputStatus=%d", LockInputStatus);
	g_pRenderText->RenderText(100, 130, Text);
}


void RenderInputText(int x, int y, int Index, int Gold)
{
	return;

	if (g_iChatInputType == 1)
	{
		return;
	}
	else if (g_iChatInputType == 0)
	{
		g_pRenderText->SetTextColor(255, 230, 210, 255);
		g_pRenderText->SetBgColor(0);

		SIZE* Size;
		char Text[256];
		if (InputTextHide[Index] == 1)
		{
			int iTextSize = 0;
			for (unsigned int i = 0; i < strlen(InputText[Index]); i++)
			{
				Text[i] = '*';
				iTextSize = i;
			}
			Text[iTextSize] = NULL;
		}
		else if (InputTextHide[Index] == 2)
		{
			int iTextSize = 0;
			for (unsigned int i = 0; i < 7; i++)
			{
				Text[i] = InputText[Index][i];
				iTextSize = i;
			}
			for (unsigned int i = 7; i < strlen(InputText[Index]); i++)
			{
				Text[i] = '*';
				iTextSize = i;
			}
			Text[iTextSize] = NULL;
		}
		else
		{
			strcpy(Text, InputText[Index]);
		}
		SIZE TextSize;
		g_pRenderText->RenderText(x, y, Text, InputTextWidth, 0, RT3_SORT_LEFT, &TextSize);
		Size = &TextSize;

		if (Index == InputIndex)
			SetPositionIME_Wnd(x + Size->cx, y);

		if (Index == InputIndex && (InputFrame++) % 2 == 0)
		{
			EnableAlphaTest();
			if (strlen(InputTextIME[Index]) > 0)
			{
				if (InputTextHide[Index] == 1)
					g_pRenderText->RenderText(x + Size->cx, y, "**");
				else
					g_pRenderText->RenderText(x + Size->cx, y, InputTextIME[Index]);
			}
			else
				g_pRenderText->RenderText(x + Size->cx, y, "_");
		}
	}
}

void RenderTipText(int sx, int sy, const char* Text)
{
	SIZE TextSize = { 0, 0 };
	g_pRenderText->SetFont(g_hFont);
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), Text, lstrlen(Text), &TextSize);

	int BackupAlphaBlendType = AlphaBlendType;
	EnableAlphaTest();
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	RenderColor((float)sx - 2, (float)sy - 3, (float)TextSize.cx / g_fScreenRate_x + 4, (float)1);	// ��
	RenderColor((float)sx - 2, (float)sy - 3, (float)1, (float)TextSize.cy / g_fScreenRate_y + 4);	// ��
	RenderColor((float)sx - 2 + TextSize.cx / g_fScreenRate_x + 3, (float)sy - 3, (float)1, (float)TextSize.cy / g_fScreenRate_y + 4);
	RenderColor((float)sx - 2, (float)sy - 3 + TextSize.cy / g_fScreenRate_y + 3, (float)TextSize.cx / g_fScreenRate_x + 4, (float)1);

	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	RenderColor((float)sx - 1, (float)sy - 2, (float)TextSize.cx / g_fScreenRate_x + 2, (float)TextSize.cy / g_fScreenRate_y + 2);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->RenderText(sx, sy, Text);

	switch (BackupAlphaBlendType)
	{
	case 0:
		DisableAlphaBlend();
		break;
	case 1:
		EnableLightMap();
		break;
	case 2:
		EnableAlphaTest(true);
		break;
	case 3:
		EnableAlphaBlend();
		break;
	case 4:
		EnableAlphaBlendMinus();
		break;
	case 5:
		EnableAlphaBlend2();
		break;
	default:
		DisableAlphaBlend();
		break;
	}
}

void ScrollNotice()
{
	if (NoticeCount > MAX_NOTICE - 1)
	{
		NoticeCount = MAX_NOTICE - 1;
		for (int i = 1; i < MAX_NOTICE; i++)
		{
			Notice[i - 1].Color = Notice[i].Color;
			strcpy(Notice[i - 1].Text, Notice[i].Text);
		}
	}
}

void ClearNotice(void)
{
	memset(Notice, 0, sizeof(NOTICE) * MAX_NOTICE);
}

void CreateNotice(char* Text, int Color)
{
#ifdef SHUTDOWN_SCALEFORM_INFO
	gfxNotice::addNoticeText(Text, Color);
#else
	SIZE Size;
	g_pRenderText->SetFont(g_hFontBold);
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), Text, lstrlen(Text), &Size);

	ScrollNotice();
	Notice[NoticeCount].Color = Color;
	if (Size.cx < 256)
	{
		strcpy(Notice[NoticeCount++].Text, Text);
	}
	else
	{
		char Temp1[256];
		char Temp2[256];
		CutText(Text, Temp1, Temp2, strlen(Text));
		strcpy(Notice[NoticeCount++].Text, Temp2);
		ScrollNotice();
		Notice[NoticeCount].Color = Color;
		strcpy(Notice[NoticeCount++].Text, Temp1);
	}
	NoticeTime = standlimit(300);
#endif // SHUTDOWN_SCALEFORM_INFO
}

void MoveNotices()
{
#ifdef SHUTDOWN_SCALEFORM_INFO
	gfxinit->Update(0);
#else
	if (NoticeTime-- <= 0)
	{
		NoticeTime = standlimit(300);
		CreateNotice("", 0);
	}
#endif // SHUTDOWN_SCALEFORM_INFO
}

void RenderNotices()
{
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) == true)
		return;
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM

#ifdef SHUTDOWN_SCALEFORM_INFO
	gfxinit->Render(0);
#else
	EnableAlphaTest();
	glColor3f(1.f, 1.f, 1.f);


	int RenderFrameX = pos_center() + 320;
	int RenderFrameY = pos_botton() + 300;

	g_pRenderText->SetFont(g_hFontBold);

	for (int i = 0; i < MAX_NOTICE; i++)
	{
		NOTICE* n = &Notice[i];
		if (n->Color == 0)
		{
			g_pRenderText->SetBgColor(0, 0, 0, 128);

			if ((NoticeInverse % standlimit(10)) >= 5)
			{
				g_pRenderText->SetTextColor(0xFFu, 0xC8u, 0x50u, 0xFFu);
			}
			else
			{
				g_pRenderText->SetTextColor(0xFFu, 0xC8u, 0x50u, 0x80u);
			}
		}
		else
		{
			g_pRenderText->SetTextColor(100, 255, 200, 255);
			g_pRenderText->SetBgColor(0, 0, 0, 128);
		}

		if (g_bUseChatListBox == TRUE)
			g_pRenderText->RenderText(RenderFrameX, RenderFrameY + i * 13, n->Text, 0, 0, RT3_WRITE_CENTER);
		else
			g_pRenderText->RenderText(RenderFrameX, RenderFrameY + i * 13, n->Text, 0, 0, RT3_WRITE_CENTER);
	}
	NoticeInverse++;
#endif // SHUTDOWN_SCALEFORM_INFO
}

void CutText(const char* Text, char* Text1, char* Text2, int Length)
{

	if (g_pMultiLanguage->IsCharUTF8(Text))
	{
		std::wstring wstrText = L"";
		g_pMultiLanguage->ConvertCharToWideStr(wstrText, Text);
		int iClosestBlankFromCenter = g_pMultiLanguage->GetClosestBlankFromCenter(wstrText);

		std::wstring wstrText1, wstrText2;
		wstrText1 = wstrText.substr(iClosestBlankFromCenter + 1, std::string::npos);
		wstrText2 = wstrText.substr(0, iClosestBlankFromCenter + 1);

		std::string strText1, strText2;
		g_pMultiLanguage->ConvertWideCharToStr(strText1, wstrText1.c_str(), CP_UTF8);
		g_pMultiLanguage->ConvertWideCharToStr(strText2, wstrText2.c_str(), CP_UTF8);

		strncpy(Text1, strText1.c_str(), strText1.length() + 1);
		strncpy(Text2, strText2.c_str(), strText2.length() + 1);

		return;
	}

	int Cut = 0;

	for (int j = 0; j < Length;)
	{
		if ((j >= Length / 2 - 2 && Text[j] == ' ') || j >= Length / 2 + 2)
		{
			Cut = j;
			break;
		}

		j += _mbclen((unsigned char*)&(Text[j]));
	}

	int iTextSize = 0;
	for (int j = 0; j < Cut; j++)
	{
		Text2[j] = Text[j];
		iTextSize = j;
	}
	Text2[iTextSize + 1] = NULL;
	for (int j = Cut; j < Length; j++)
	{
		Text1[j - Cut] = Text[j];
		iTextSize = j;
	}
	Text1[iTextSize - Cut + 1] = NULL;
}

int     WhisperID_Num = 0;
char    WhisperRegistID[MAX_ID_SIZE + 1][10];

void RegistWhisperID(int lvl, char* text)
{
	int level = CharacterAttribute->Level;

	if (level < lvl)
	{
		bool noMatch = true;
		for (int i = 0; i < 10; ++i)
		{
			if (strcmp(text, WhisperRegistID[i]) == 0)
			{
				noMatch = false;
				break;
			}
		}

		if (noMatch)
		{
			strcpy(WhisperRegistID[WhisperID_Num], text);
			WhisperID_Num++;

			if (WhisperID_Num >= 10)
			{
				WhisperID_Num = 0;
			}
		}
	}
}

void ClearWhisperID(void)
{
	ZeroMemory(WhisperRegistID, sizeof(char) * (MAX_ID_SIZE + 1) * 10);
}


int RenderDebugText(int y)
{
	char Text[100];
	int Width = 16;
	for (int i = 0; i < min(DebugTextCount, 10); i++)
	{
		int  Type = 0;
		int  Count = 0;
		int  x = 0;
		bool Hex = true;
		int  SizeByte = 1;
		for (int j = 0; j < DebugTextLength[i]; j++)
		{
			glColor3f(0.6f, 0.6f, 0.6f);
			if (j == 0)
			{
				if (DebugText[i][j] == 0xc2) SizeByte = 2;
			}
			if (j == 2)
			{
				if (SizeByte == 1)
				{
					Type = DebugText[i][j];
					glColor3f(1.f, 1.f, 1.f);
					if (DebugText[i][j] == 0x00)
					{
						x = Width * 4;
					}
				}
			}
			if (j == 3)
			{
				if (SizeByte == 2)
				{
					Type = DebugText[i][j];
					glColor3f(1.f, 1.f, 1.f);
				}
			}

			SIZE TextSize;
			if (Hex)
			{
				sprintf(Text, "%0.2x", DebugText[i][j]);
				g_pRenderText->RenderText(x, y, Text, 0, 0, RT3_SORT_CENTER, &TextSize);
			}
			else
			{
				sprintf(Text, "%c", DebugText[i][j]);
				g_pRenderText->RenderText(x, y, Text, 0, 0, RT3_SORT_CENTER, &TextSize);
			}
			if (Hex)
			{
				x += Width;
			}
			else
			{
				x += TextSize.cx;
			}
			Count++;
		}
		y += 12;
	}
	return y;
}

int SearchArrow()
{
	int iIndex = -1;
	if (gCharacterManager.GetBaseClass(CharacterAttribute->Class) == CLASS_ELF)
	{
		int EquipedBowType = gCharacterManager.GetEquipedBowType();

		if (EquipedBowType == BOWTYPE_BOW)
		{
			iIndex = g_pMyInventory->FindItemReverseKind2(ItemKind2::ARROW);
		}
		else if (EquipedBowType == BOWTYPE_CROSSBOW)
		{
			iIndex = g_pMyInventory->FindItemReverseKind2(ItemKind2::BOLT);
		}

		if (iIndex == -1 && (EquipedBowType == BOWTYPE_BOW || EquipedBowType == BOWTYPE_CROSSBOW))
		{
			iIndex = g_pMyInventory->FindItemReverseKind2(ItemKind2::QUIVER);
		}
	}
	return iIndex;
}

int SearchArrowCount()
{
	int Count = 0;
	if (gCharacterManager.GetBaseClass(CharacterAttribute->Class) == CLASS_ELF)
	{
		int Arrow = 0;
		int EquipedBowType = gCharacterManager.GetEquipedBowType();

		if (EquipedBowType == BOWTYPE_BOW)
		{
			Arrow = ITEM_BOW + 15;
		}
		else if (EquipedBowType == BOWTYPE_CROSSBOW)
		{
			Arrow = ITEM_BOW + 7;
		}
		Count = g_pMyInventory->GetNumItemByType(Arrow);
	}
	return Count;
}

bool CheckTile(CHARACTER* c, int PositionX, int PositionY, float Range)
{
	if (c)
	{
		float dx = c->PositionX - PositionX;
		float dy = c->PositionY - PositionY;
		float Distance = sqrtf(dx * dx + dy * dy);

		return (Range >= Distance);
	}
	return false;
}

bool CheckTile(CHARACTER* c, float Range)
{
	if (c)
	{
		float dx = (double)(c->PositionX - TargetX);
		float dy = (double)(c->PositionY - TargetY);
		float Distance = sqrtf(dx * dx + dy * dy);
		return (Range >= Distance);
	}
	return false;
}

bool CheckTile(CHARACTER* c, OBJECT* o, float Range)
{
	if (c == NULL)	return false;
	if (o == NULL)	return false;

	float dx = o->Position[0] - (TargetX * TERRAIN_SCALE + TERRAIN_SCALE * 0.5f);
	float dy = o->Position[1] - (TargetY * TERRAIN_SCALE + TERRAIN_SCALE * 0.5f);

	float Distance = sqrtf(dx * dx + dy * dy);
	if (Distance <= TERRAIN_SCALE * Range)
	{
		return true;
	}
	return false;
}

bool CheckDistance(CHARACTER* sc, CHARACTER* tc, float Range)
{
	if (sc)
	{
		float dx = sc->Object.Position[0] - tc->Object.Position[0];
		float dy = sc->Object.Position[1] - tc->Object.Position[1];
		float Distance = sqrtf(dx * dx + dy * dy);
		return (Range >= Distance);
	}
	return false;
}

bool CheckWall(int sx1, int sy1, int sx2, int sy2, bool NoGround)
{
	int Index = TERRAIN_INDEX_REPEAT(sx1, sy1);

	int nx1, ny1, d1, d2, len1, len2;
	int px1 = sx2 - sx1;
	int py1 = sy2 - sy1;

	if (px1 < 0)
	{
		px1 = -px1;
		nx1 = -1;
	}
	else
	{
		nx1 = 1;
	}

	if (py1 < 0)
	{
		py1 = -py1;
		ny1 = -TERRAIN_SIZE;
	}
	else
	{
		ny1 = TERRAIN_SIZE;
	}

	if (px1 > py1)
	{
		len1 = px1;
		len2 = py1;
		d1 = ny1;
		d2 = nx1;
	}
	else
	{
		len1 = py1;
		len2 = px1;
		d1 = nx1;
		d2 = ny1;
	}

	int error = 0, count = 0;

	do
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

		int _type;
		if (pCharacter)
			_type = pCharacter->Object.Type;
		else
			_type = 0;

		if ((_type != MODEL_MONSTER01 + 183
			&& _type != MODEL_MONSTER01 + 184
			&& _type != MODEL_MONSTER01 + 186
			&& _type != MODEL_MONSTER01 + 187)
			&& (TerrainWall[Index] >= TW_NOMOVE
				&& (TerrainWall[Index] & TW_ACTION)
				&& (TerrainWall[Index] & TW_HEIGHT)
				&& (TerrainWall[Index] & TW_CAMERA_UP)))
		{
			return false;
		}
		if (NoGround && (TerrainWall[Index] & TW_NOGROUND))
			return false;

		error += len2;
		if (error > len1 / 2)
		{
			Index += d1;
			error -= len1;
		}
		Index += d2;
	} while (++count <= len1);

	return true;
}

bool CheckAttack_Fenrir(CHARACTER* pCharacter)
{
	bool success = false;

	if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
		return false;

	if (gMapManager->InChaosCastle() == true && pCharacter != Hero)
		return true;

	if (::IsStrifeMap(gMapManager->currentMap) && pCharacter != Hero && pCharacter->m_byGensInfluence != Hero->m_byGensInfluence)
	{
		if (!strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[pCharacter->GuildMarkIndex].GuildName)
			|| g_pPartyManager->IsPartyMember(SelectedCharacter))
		{
			if (SEASON3B::IsRepeat(VK_CONTROL) == 1)
				return 1;
		}
		if (strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[pCharacter->GuildMarkIndex].GuildName))
		{
			if (!g_pPartyManager->IsPartyMember(SelectedCharacter))
				return 1;
		}
	}

	if (pCharacter->Object.Kind == KIND_MONSTER)
	{
		if (EnableGuildWar && EnableSoccer)
			success = true;
		else
			success = EnableGuildWar == false;
	}
	else if (pCharacter->Object.Kind == KIND_PLAYER)
	{
		if (battleCastle::IsBattleCastleStart())
		{
			if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK
				|| Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2
				|| Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK2
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK3)
				&& (pCharacter->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || pCharacter->EtcPart == PARTS_DEFENSE_TEAM_MARK))
			{
				return g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == false;
			}
			if ((Hero->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || Hero->EtcPart == PARTS_DEFENSE_TEAM_MARK)
				&& (pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK
					|| pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2
					|| pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK2
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK3))
			{
				return g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == 0;
			}
			if (g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack1)
				|| g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack2)
				|| g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack3))
			{
				if (!g_isCharacterBuff((&pCharacter->Object), eBuff_CastleRegimentAttack1)
					&& !g_isCharacterBuff((&pCharacter->Object), eBuff_CastleRegimentAttack2)
					&& !g_isCharacterBuff((&pCharacter->Object), eBuff_CastleRegimentAttack3))
				{
					return g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == 0;
				}
			}
			else if (g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentDefense)
				&& !g_isCharacterBuff((&pCharacter->Object), eBuff_CastleRegimentDefense))
			{
				return g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == 0;
			}
		}

		if (pCharacter->GuildRelationShip != GR_RIVAL && pCharacter->GuildRelationShip != GR_RIVALUNION)
		{
			if (!EnableGuildWar
				|| pCharacter->PK < PVP_MURDERER2
				|| pCharacter->GuildMarkIndex == -1
				|| strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[pCharacter->GuildMarkIndex].GuildName))
			{
				if (g_DuelMgr.IsDuelEnabled())
				{
					success = g_DuelMgr.IsDuelPlayer(pCharacter, DUEL_ENEMY, TRUE) != 0;
				}
				else if (EnableGuildWar)
				{
					success = pCharacter->GuildTeam == GR_RIVAL && pCharacter != Hero;
				}
				else if (pCharacter->PK < PVP_MURDERER2 && (SEASON3B::IsRepeat(VK_CONTROL) != true || pCharacter == Hero))
				{
					success = false;
					if (gMapManager->IsCursedTemple())
					{
						if (!g_CursedTemple->IsPartyMember(SelectedCharacter))
							success = true;
					}
				}
				else
				{
					success = true;
				}
			}
			else
			{
				success = false;
			}
		}
		else
		{
			success = true;
		}
	}
	else
	{
		success = false;
	}

	return success;
}

bool CheckAttack()
{
	if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
		return false;

	if (IsGMCharacter() && IsNonAttackGM() == true)
		return false;

	CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);
	if (!pCharacter)
		return false;

	if (gMapManager->InChaosCastle() == true && pCharacter != Hero)
		return true;

	bool Success = false;
	if (::IsStrifeMap(gMapManager->currentMap)
		&& pCharacter != Hero
		&& pCharacter->m_byGensInfluence != Hero->m_byGensInfluence)
	{
		if (g_pCommandWindow->GetMouseCursor() == CURSOR_IDSELECT)
		{
			Success = false;
		}
		else if (SEASON3B::IsRepeat(VK_MENU) == true)
		{
			Success = false;
		}
		else if (SEASON3B::IsRepeat(VK_CONTROL) == true)
		{
			if (EnableGuildWar)
				Success = (pCharacter->GuildTeam == 2 && pCharacter != Hero);
			else
				Success = true;
		}
		else if (strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[pCharacter->GuildMarkIndex].GuildName))
		{
			if (pCharacter->GuildRelationShip != GR_UNION && pCharacter->GuildRelationShip != GR_UNIONMASTER)
			{
				if (EnableGuildWar)
				{
					Success = (pCharacter->GuildTeam == 2 && pCharacter != Hero);
				}
				else
				{
					if (g_pPartyManager->IsPartyMember(SelectedCharacter))
						Success = (pCharacter->GuildRelationShip == GR_RIVAL || pCharacter->GuildRelationShip == GR_RIVALUNION);
					else
						Success = true;
				}
			}
			else
			{
				Success = false;
			}
		}
		else
		{
			if (g_pPartyManager->IsPartyMember(SelectedCharacter))
			{
				Success = false;
			}
			else if (EnableGuildWar)
			{
				Success = (pCharacter->GuildTeam == 2 && pCharacter != Hero);
			}
			else
			{
				Success = (pCharacter->GuildRelationShip == GR_NONE);
			}
		}
	}
	else if (pCharacter->Object.Kind == KIND_MONSTER)
	{
		if (EnableGuildWar && EnableSoccer)
		{
			Success = true;
		}
		else if (EnableGuildWar)
		{
			Success = false;
		}
		else
		{
			Success = (g_isCharacterBuff((&Hero->Object), eBuff_DuelWatch) == false);
		}
	}
	else if (pCharacter->Object.Kind == KIND_PLAYER)
	{
		if (battleCastle::IsBattleCastleStart())
		{
			if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK
				|| Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2
				|| Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK2
				|| Hero->EtcPart == PARTS_ATTACK_TEAM_MARK3)
				&& (pCharacter->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || pCharacter->EtcPart == PARTS_DEFENSE_TEAM_MARK))
			{
				return (g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == false);
			}

			if ((Hero->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || Hero->EtcPart == PARTS_DEFENSE_TEAM_MARK)
				&& (pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK
					|| pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2
					|| pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK2
					|| pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK3))
			{
				return (g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == false);
			}

			if (g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack1)
				|| g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack2)
				|| g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentAttack3))
			{
				OBJECT* o = &pCharacter->Object;

				if (!g_isCharacterBuff(o, eBuff_CastleRegimentAttack1)
					&& !g_isCharacterBuff(o, eBuff_CastleRegimentAttack2)
					&& !g_isCharacterBuff(o, eBuff_CastleRegimentAttack3))
				{
					return (g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == false);
				}
			}
			else if (g_isCharacterBuff((&Hero->Object), eBuff_CastleRegimentDefense)
				&& !g_isCharacterBuff((&pCharacter->Object), eBuff_CastleRegimentDefense))
			{
				return (g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking) == false);
			}
		}

		if (pCharacter->GuildRelationShip != GR_RIVAL && pCharacter->GuildRelationShip != GR_RIVALUNION)
		{
			if (!EnableGuildWar
				|| pCharacter->PK < PVP_MURDERER2
				|| pCharacter->GuildMarkIndex == -1
				|| strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[pCharacter->GuildMarkIndex].GuildName))
			{
				if (g_DuelMgr.IsDuelEnabled())
				{
					Success = (g_DuelMgr.IsDuelPlayer(pCharacter, DUEL_ENEMY, TRUE));
				}
				else if (g_isCharacterBuff((&Hero->Object), eBuff_DuelWatch))
				{
					Success = false;
				}
				else if (EnableGuildWar)
				{
					Success = (pCharacter->GuildTeam == 2 && pCharacter != Hero);
				}
				else if (pCharacter->PK < PVP_MURDERER2 && (SEASON3B::IsRepeat(VK_CONTROL) != true || pCharacter == Hero))
				{
					if (gMapManager->IsCursedTemple())
					{
						if (!g_CursedTemple->IsPartyMember(SelectedCharacter))
							Success = true;
					}
				}
				else
				{
					Success = true;
				}
			}
			else
			{
				Success = false;
			}
		}
		else
		{
			Success = true;
		}
	}
	else
	{
		Success = false;
	}

	return Success;
}

int	getTargetCharacterKey(CHARACTER* c, int selected, int Key)
{
	int targetkey = -1;

	if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
	{
		targetkey = -1;
	}
	else
	{
		CHARACTER* sc = gmCharacters->GetCharacter(selected);

		if (c == Hero && sc)
		{
			//if (Key && sc->Key != Key)
			//{
				//sc->Key = Key;
			//}

			if (gMapManager->InChaosCastle() == true)
			{
				targetkey = sc->Key;
			}
			else if (!EnableGuildWar
				|| sc->PK < PVP_MURDERER2
				|| sc->GuildMarkIndex == -1
				|| strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[sc->GuildMarkIndex].GuildName))
			{
				if (g_DuelMgr.IsDuelEnabled())
				{
					if (g_DuelMgr.IsDuelPlayer(sc, DUEL_ENEMY, TRUE))
						targetkey = sc->Key;
					else
						targetkey = -1;
				}
				else if (sc->GuildRelationShip != GR_RIVAL && sc->GuildRelationShip != GR_RIVALUNION)
				{
					if (EnableGuildWar)
					{
						if (sc->GuildTeam != GR_RIVAL || sc == Hero)
							targetkey = -1;
						else
							targetkey = sc->Key;
					}
					else if (!::IsStrifeMap(gMapManager->currentMap)
						|| sc == Hero
						|| sc->m_byGensInfluence == Hero->m_byGensInfluence
						|| SEASON3B::IsRepeat(VK_MENU))
					{
						if ((signed int)sc->PK < PVP_MURDERER2 && (SEASON3B::IsRepeat(VK_CONTROL) != true || sc == Hero))
						{
							if (gMapManager->IsCursedTemple())
							{
								if (g_CursedTemple->IsPartyMember(selected))
									targetkey = -1;
								else
									targetkey = sc->Key;
							}
							else
							{
								targetkey = -1;
							}
						}
						else
						{
							targetkey = sc->Key;
						}
					}
					else if (sc->GuildRelationShip || g_pPartyManager->IsPartyMember(SelectedCharacter))
					{
						if (strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[c->GuildMarkIndex].GuildName)
							&& !g_pPartyManager->IsPartyMember(SelectedCharacter))
						{
							targetkey = -1;
						}
						else if (SEASON3B::IsRepeat(VK_CONTROL) == true)
						{
							targetkey = sc->Key;
						}
						else
						{
							targetkey = -1;
						}
					}
					else
					{
						targetkey = sc->Key;
					}
				}
				else
				{
					targetkey = sc->Key;
				}
			}
			else
			{
				targetkey = -1;
			}
		}
		else
		{
			targetkey = -1;
		}
	}

	return targetkey;
}

void LetHeroStop(CHARACTER* pCharacter, BOOL bSetMovementFalse)
{
	BYTE PathX[1] = { (BYTE)(Hero->PositionX) };
	BYTE PathY[1] = { (BYTE)(Hero->PositionY) };

	SendCharacterMove(Hero->Key, Hero->Object.Angle[2], 1, PathX, PathY, TargetX, TargetY);

	if (pCharacter)
	{
		if (bSetMovementFalse == TRUE)
			pCharacter->Movement = false;
	}
}

bool CastWarriorSkill(CHARACTER* c, OBJECT* o, ITEM* p, int iSkill)
{
	bool Success = false;

	if (c != NULL)
	{
		if (o != NULL)
		{
			if (p != NULL)
			{
				Success = false;
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter != NULL)
				{
					TargetX = pCharacter->PositionX;
					TargetY = pCharacter->PositionY;
					memset(&g_MovementSkill, 0, sizeof(g_MovementSkill));

					g_MovementSkill.m_bMagic = FALSE;
					g_MovementSkill.m_iSkill = iSkill;
					g_MovementSkill.m_iTarget = SelectedCharacter;

					if (pCharacter)
						g_MovementSkill.m_iKey = pCharacter->Key;
					else
						g_MovementSkill.m_iKey = 0;

					float Distance = gSkillManager.GetSkillDistance(iSkill, c);

					if (gMapManager->InBloodCastle() && (iSkill >= Skill_Falling_Slash && iSkill <= Skill_Slash || iSkill >= Skill_Cyclone_Strengthener && iSkill <= Skill_Lunge_Strengthener))
					{
						Distance = 1.8f;
					}

					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						UseSkillWarrior(c, o);
						Success = true;
					}
					else if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
					{
						c->Movement = true;
						c->MovementType = MOVEMENT_SKILL;
						Success = true;
						SendMove(c, o);
						g_pNewUIMiniMap->StopMove();
					}
				}
				else
				{
					Success = false;
				}
			}
			else
			{
				Success = false;
			}
		}
		else
		{
			Success = false;
		}
	}
	else
	{
		Success = false;
	}

	return (Success);
}

bool SkillWarrior(CHARACTER* c, ITEM* p)
{
	OBJECT* o = &c->Object;

	if (o->Type == MODEL_PLAYER)
	{
		if (o->CurrentAction == PLAYER_DEFENSE1)
			return false;
		if (o->CurrentAction >= PLAYER_HIT_SKILL_SWORD1 && o->CurrentAction <= PLAYER_HIT_SKILL_SWORD4 || o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD_TWO)
			return false;
	}
	else if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
	{
		return false;
	}

	int iMana, iSkillMana;
	int Skill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];

	if (Skill == Skill_Fire_Breath
		|| Skill == Skill_Death_Stab
		|| Skill == Skill_Death_Stab_Strengthener
		|| Skill == Skill_Impale
		&& (Hero->Helper.Type == (MODEL_HELPER + 2)
			|| Hero->Helper.Type == (MODEL_HELPER + 3)
			|| Hero->Helper.Type == (MODEL_HELPER + 4)
			|| Hero->Helper.Type == (MODEL_HELPER + 37)
			|| o->movementType == Movement::Uniria
			|| o->movementType == Movement::Dinorant
			|| o->movementType == Movement::Horse
			|| o->movementType == Movement::Fenrir
			)
		|| Skill == Skill_Force
		|| Skill == Skill_Force_Wave_Strengthener
		|| Skill == Skill_Fire_Burst
		|| Skill == Skill_Fire_Burst_Mastery
		|| Skill == Skill_Fire_Burst_Strengthener
		|| Skill == Skill_Crescent_Moon_Slash
		|| Skill == Skill_Spiral_Slash
		|| Skill == Skill_Fire_Blast)
	{
		if (Skill > Skill_Spiral_Slash)
		{
			if (Skill != Skill_Death_Stab_Strengthener)
			{
			LABEL_49:
				gSkillManager.GetSkillInformation(Skill, 1, NULL, &iMana, NULL, &iSkillMana);
				if (CharacterAttribute->Mana < (unsigned int)iMana)
				{
					int Index = g_pMyInventory->FindManaItemIndex();

					if (Index != -1)
					{
						SendRequestUse(Index, 0);
					}
					return false;
				}
				if ((unsigned int)iSkillMana > CharacterAttribute->SkillMana)
					return false;
				if (!gSkillManager.CheckSkillDelay(g_MovementSkill.m_iSkill))
					return false;
				if (CheckAttack())
					return CastWarriorSkill(c, o, p, Skill);
				goto LABEL_68;
			}
		}
		else
		{
			if (Skill == Skill_Spiral_Slash)
			{
				if (Hero->Weapon[0].Type < MODEL_SWORD || Hero->Weapon[0].Type >= MODEL_SWORD + MAX_ITEM_INDEX)
					return false;
				goto LABEL_49;
			}
			if (Skill != Skill_Death_Stab)
			{
				if (Skill == Skill_Impale && (o->Type != MODEL_PLAYER || Hero->Weapon[0].Type == -1 || Hero->Weapon[0].Type < MODEL_SPEAR || Hero->Weapon[0].Type >= (MODEL_SPEAR + MAX_ITEM_INDEX)))
				{
					return false;
				}
				goto LABEL_49;
			}
		}
		if (Hero->Weapon[0].Type == -1
			|| Hero->Weapon[0].Type >= MODEL_STAFF && Hero->Weapon[0].Type < (MODEL_STAFF + MAX_ITEM_INDEX))
		{
			return 0;
		}
		goto LABEL_49;
	}

LABEL_68:
	bool Success = false;

	for (int i = 0; i < p->SpecialNum; ++i)
	{
		bool temp = false;
		for (int j = CharacterAttribute->Skill[Hero->CurrentSkill]; j; j = SkillAttribute[j].SkillBrand)
		{
			if (j == p->Special[i])
			{
				temp = true;
				break;
			}
		}
		if (temp)
		{
			gSkillManager.GetSkillInformation(p->Special[i], 1, NULL, &iMana, NULL);

			if (CharacterAttribute->Mana < (unsigned int)iMana)
			{
				int Index = g_pMyInventory->FindManaItemIndex();
				if (Index != -1)
				{
					SendRequestUse(Index, 0);
				}
				continue;
			}
			if (!gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
			{
				continue;
			}

			int SkillChar = CharacterAttribute->Skill[Hero->CurrentSkill];

			if (SkillChar > Skill_Fire_Breath)
			{
				if (SkillChar < Skill_Cyclone_Strengthener || SkillChar > Skill_Lunge_Strengthener && SkillChar != Skill_Cyclone_Strengthener2)
					continue;
				goto LABEL_109;
			}
			if (SkillChar == Skill_Fire_Breath)
				goto LABEL_109;
			if (SkillChar != Skill_Defense)
			{
				if (SkillChar <= Skill_Defense || SkillChar > Skill_Slash)
					continue;
			LABEL_109:
				if (CheckAttack())
					Success = CastWarriorSkill(c, o, p, SkillChar);
				continue;
			}
			c->Movement = false;
			if (o->Type == MODEL_PLAYER)
				SetAction(o, PLAYER_DEFENSE1);
			else
				SetPlayerAttack(c);

			SendRequestMagic(SkillChar, Hero->Key);

			Success = true;
		}
	}
	if (CharacterAttribute->Skill[Hero->CurrentSkill] != Skill_Fire_Scream
		&& CharacterAttribute->Skill[Hero->CurrentSkill] != Skill_Fire_Scream_Strengthener
		&& Skill != Skill_Chaotic_Diseier_Strengthener
		&& Skill != Skill_Chaotic_Diseier)
	{
		return Success;
	}

	gSkillManager.GetSkillInformation(CharacterAttribute->Skill[Hero->CurrentSkill], 1, NULL, &iMana, NULL);

	if (CharacterAttribute->Mana >= (unsigned int)iMana)
	{
		float Distance = gSkillManager.GetSkillDistance(CharacterAttribute->Skill[Hero->CurrentSkill], c);

		if (CheckTile(c, TargetX, TargetY, Distance))
		{
			o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
			WORD TKey = 0xffff;

			if (g_MovementSkill.m_iTarget != -1)
			{
				TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
			}
		}
		return Success;
	}
	int Index = g_pMyInventory->FindManaItemIndex();

	if (Index != -1)
	{
		SendRequestUse(Index, 0);
	}
	return Success;
}

void UseSkillWarrior(CHARACTER* c, OBJECT* o)
{
	int Skill;
	if (g_MovementSkill.m_bMagic)
		Skill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];
	else
		Skill = g_MovementSkill.m_iSkill;

	LetHeroStop();
	c->Movement = false;

	if (o->Type != MODEL_PLAYER)
	{
		SetPlayerAttack(c);
		goto LABEL_40;
	}
	SetAttackSpeed();

	if (Skill <= Skill_Fire_Burst)
	{
		if (Skill >= Skill_Force)
			goto LABEL_40;
		switch (Skill)
		{
		case Skill_Impale:
			if (c->Helper.Type == MODEL_HELPER + 37 || o->movementType == Movement::Fenrir)
				SetAction(o, PLAYER_FENRIR_ATTACK_SPEAR, true);
			else
				SetAction(o, PLAYER_HIT_SKILL_SPEAR, true);
			goto LABEL_40;
		case Skill_Death_Stab:
			goto LABEL_19;
		case Skill_Fire_Breath:
			if (gMapManager->currentMap == WD_8TARKAN || gMapManager->currentMap == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
				SetAction(o, PLAYER_SKILL_RIDER_FLY, 1);
			else
				SetAction(o, PLAYER_SKILL_RIDER, 1);
			goto LABEL_40;
		case Skill_Crescent_Moon_Slash:
			SetAction(o, PLAYER_HIT_RUSH, 1);
			goto LABEL_40;
		default:
			goto LABEL_34;
		}
		goto LABEL_34;
	}
	if (Skill > Skill_Cyclone_Strengthener)
	{
		switch (Skill)
		{
		case Skill_Death_Stab_Strengthener:
		LABEL_19:
			SetAction(o, PLAYER_HIT_ONETOONE, 1);
			goto LABEL_40;
		case Skill_Fire_Scream_Strengthener:
		case Skill_Chaotic_Diseier_Strengthener:
		case Skill_Fire_Burst_Strengthener:
		case Skill_Force_Wave_Strengthener:
		case Skill_Fire_Burst_Mastery:
			goto LABEL_40;
		case Skill_Cyclone_Strengthener2:
			goto LABEL_30;
		case Skill_Slash_Strengthener:
			SetAction(o, PLAYER_HIT_SKILL_SWORD5, 1);
			goto LABEL_40;
		case Skill_Falling_Slash_Strengthener:
			SetAction(o, PLAYER_HIT_SKILL_SWORD1, 1);
			goto LABEL_40;
		case Skill_Lunge_Strengthener:
			SetAction(o, PLAYER_HIT_SKILL_SWORD2, 1);
			goto LABEL_40;
		default:
			break;
		}
	LABEL_34:
		if (c->Helper.Type != (MODEL_HELPER + 37) || c->SafeZone)
			SetAction(o, g_MovementSkill.m_iSkill + PLAYER_HIT_SWORD_LEFT1, 1);
		else
			SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC, 1);
		goto LABEL_40;
	}
	if (Skill == Skill_Cyclone_Strengthener)
	{
	LABEL_30:
		SetAction(o, PLAYER_HIT_SKILL_SWORD4, 1);
		goto LABEL_40;
	}
	if (Skill != Skill_Fire_Blast && Skill != Skill_Fire_Scream && Skill != Skill_Chaotic_Diseier)
		goto LABEL_34;

LABEL_40:
	vec3_t Light;
	Vector(1.f, 1.f, 1.f, Light);

	if (Skill != Skill_Force
		&& Skill != Skill_Fire_Burst
		&& Skill != Skill_Force_Wave_Strengthener
		&& Skill != Skill_Fire_Blast
		&& Skill != Skill_Fire_Scream
		&& Skill != Skill_Fire_Scream_Strengthener
		&& Skill != Skill_Killing_Blow
		&& Skill != Skill_Beast_Uppercut
		&& Skill != Skill_Killing_Blow_Strengthener
		&& Skill != Skill_Beast_Uppercut_Strengthener
		&& Skill != Skill_Killing_Blow_Mastery
		&& Skill != Skill_Beast_Uppercut_Mastery
		&& Skill != Skill_Fire_Burst_Mastery
		&& Skill != Skill_Fire_Burst_Strengthener)
	{
		CreateParticle(BITMAP_SHINY + 2, o->Position, o->Angle, Light, 0, 0.f, o);
		PlayBuffer(SOUND_BRANDISH_SWORD01 + rand() % 2);
	}

	CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

	if (pCharacter)
	{
		c->TargetPosition[0] = pCharacter->Object.Position[0];
		c->TargetPosition[1] = pCharacter->Object.Position[1];
		c->TargetPosition[2] = pCharacter->Object.Position[2];

		o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
		SendRequestMagic(Skill, pCharacter->Key);
	}

	c->AttackTime = 1;

	if (!g_isCharacterBuff(o, eDeBuff_Harden)
		&& c->Helper.Type != (MODEL_HELPER + 4)
		&& Skill != Skill_Fire_Scream
		&& Skill != Skill_Fire_Scream_Strengthener)
	{
		BYTE positionX = TargetX;
		BYTE positionY = TargetY;
		bool IsAttackMonster = true;

		pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

		if (pCharacter && (pCharacter->MonsterIndex == 277
			|| pCharacter->MonsterIndex == 283
			|| pCharacter->MonsterIndex == 278
			|| pCharacter->MonsterIndex == 288))
		{
			IsAttackMonster = false;
		}
		if (gMapManager->InBloodCastle() == true || Skill == Skill_Force || Skill == Skill_Force_Wave_Strengthener || Skill == Skill_Spiral_Slash || Skill == Skill_Crescent_Moon_Slash || !IsAttackMonster)
		{
			int angle = abs((int)(o->Angle[2] / 45.0));

			switch (angle)
			{
			case 0:
				++positionY;
				break;
			case 1:
				--positionX;
				++positionY;
				break;
			case 2:
				--positionX;
				break;
			case 3:
				--positionX;
				--positionY;
				break;
			case 4:
				--positionY;
				break;
			case 5:
				++positionX;
				--positionY;
				break;
			case 6:
				++positionX;
				break;
			case 7:
				++positionX;
				++positionY;
				break;
			default:
				break;
			}
		}
		int TargetIndex = TERRAIN_INDEX(positionX, positionY);

		if (!(TerrainWall[TargetIndex] & TW_NOMOVE)
			&& !(TerrainWall[TargetIndex] & TW_NOGROUND)
			&& Skill != Skill_Impale
			&& Skill != Skill_Death_Stab
			&& Skill != Skill_Fire_Breath
			&& Skill != Skill_Force
			&& Skill != Skill_Force_Wave_Strengthener
			&& Skill != Skill_Fire_Burst
			&& Skill != Skill_Fire_Blast
			&& Skill != Skill_Death_Stab_Strengthener
			&& Skill != Skill_Fire_Burst_Mastery
			&& Skill != Skill_Fire_Burst_Strengthener)
		{
			SendPosition(positionX, positionY);
		}
	}
}

void UseSkillWizard(CHARACTER* c, OBJECT* o)
{
	int Skill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];

	if (Skill != Skill_Death_Stab
		&& Skill != Skill_Impale
		&& Skill != Skill_Death_Stab_Strengthener
		&& (Skill != Skill_Mana_Rays
			|| Hero->Weapon[0].Type >= MODEL_STAFF && Hero->Weapon[0].Type < (MODEL_STAFF + MAX_ITEM_INDEX)))
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

		if (pCharacter)
		{
			c->TargetPosition[0] = pCharacter->Object.Position[0];
			c->TargetPosition[1] = pCharacter->Object.Position[1];
			c->TargetPosition[2] = pCharacter->Object.Position[2];
			o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
		}
		switch (Skill)
		{
		case Skill_Mana_Rays:
			SendRequestMagic(Skill, g_MovementSkill.m_iKey);
			SetAction(o, PLAYER_HIT_DEATH_CANNON);
			SetAttackSpeed();
			LetHeroStop();
			break;
		case Skill_Poison:
		case Skill_Meteorite:
		case Skill_Lightning:
		case Skill_Fire_Ball:
		case Skill_Ice:
		case Skill_Power_Wave:
		case Skill_Energy_Ball:
		case Skill_Lance:
		case Skill_Lightning_Strengthener:
		case Skill_Poison_Strengthener:
		case Skill_Ice_Strengthener:
		case Skill_Lightning_Strengthener2:
		case Skill_Ice_Strengthener2:
			SendRequestMagic(Skill, g_MovementSkill.m_iKey);
			SetPlayerMagic(c);
			LetHeroStop();
			break;
		case Skill_Cometfall:
		case Skill_Blast_Strengthener:
		case Skill_Blast_Strengthener2:
		{
			unsigned __int16 TKey = -1;
			if (g_MovementSkill.m_iTarget != -1)
			{
				TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
			}
			SendRequestMagicContinue(Skill, (int)(c->TargetPosition[0] / 100.f), (int)(c->TargetPosition[1] / 100.f), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, TKey, 0);
			SetPlayerMagic(c);
			LetHeroStop();
		}
		break;
		default:
			return;
		}
	}
}

void UseSkillElf(CHARACTER* c, OBJECT* o)
{
	LetHeroStop();

	CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

	if (pCharacter)
	{
		c->TargetPosition[0] = pCharacter->Object.Position[0];
		c->TargetPosition[1] = pCharacter->Object.Position[1];
		c->TargetPosition[2] = pCharacter->Object.Position[2];
		o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
	}

	int Skill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];

	if (Skill > Skill_Heal_Strengthener)
	{
		switch (Skill)
		{
		case Skill_Defense_Increase_Strengthener:
		case Skill_Attack_Increase_Strengthener:
		case Skill_Attack_Increase_Mastery:
		case Skill_Defense_Increase_Mastery:
			goto LABEL_7;
		case Skill_Ice_Arrow_Strengthener:
		LABEL_12:
		{
			const WORD notDexterity = 646;
			WORD Dexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;

			if (Dexterity >= notDexterity)
			{
				if (CheckArrow())
				{
					SendRequestMagic(Skill, g_MovementSkill.m_iKey);
					SetPlayerAttack(c);
				}
			}
		}
		break;
		default:
			return;
		}
	}
	else if (Skill == Skill_Heal_Strengthener)
	{
	LABEL_7:
		SendRequestMagic(Skill, g_MovementSkill.m_iKey);
		SetPlayerMagic(c);
	}
	else
	{
		switch (Skill)
		{
		case Skill_Heal:
		case Skill_Greater_Defense:
		case Skill_Greater_Damage:
		case Skill_Recovery:
			goto LABEL_7;
		case Skill_Ice_Arrow:
			goto LABEL_12;
		case Skill_Starfall:
			if (CheckArrow())
			{
				SendRequestMagic(Skill, g_MovementSkill.m_iKey);
				SetPlayerHighBowAttack(c);
			}
			break;
		default:
			return;
		}
	}
}

void UseSkillSummon(CHARACTER* pCha, OBJECT* pObj)
{
	int iSkill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];

	CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

	if (iSkill > Skill_Berserker_Proficiency)
	{
		if (iSkill != Skill_Berserker_Mastery)
			return;
		goto LABEL_45;
	}
	if (iSkill >= Skill_Berserker_Strengthener)
	{
	LABEL_45:
		LetHeroStop();

		switch (pCha->Helper.Type)
		{
		case (MODEL_HELPER + 2):
			SetAction(pObj, PLAYER_SKILL_SLEEP_UNI);
			break;
		case (MODEL_HELPER + 3):
			SetAction(pObj, PLAYER_SKILL_SLEEP_DINO);
			break;
		case (MODEL_HELPER + 37):
			SetAction(pObj, PLAYER_SKILL_SLEEP_FENRIR);
			break;
		default:
			SetAction(pObj, PLAYER_SKILL_SLEEP);
			break;
		}
		SendRequestMagic(iSkill, HeroKey);
		return;
	}
	switch (iSkill)
	{
	case Skill_Drain_Life:
	case Skill_Electric_Surge:
	case Skill_Drain_Life_Strengthener:
		LetHeroStop();
		if (iSkill != Skill_Drain_Life && iSkill != Skill_Drain_Life_Strengthener)
		{
			if (iSkill == Skill_Electric_Surge)
			{
				switch (pCha->Helper.Type)
				{
				case MODEL_HELPER + 2:
					SetAction(pObj, PLAYER_SKILL_LIGHTNING_ORB_UNI);
					break;
				case MODEL_HELPER + 3:
					SetAction(pObj, PLAYER_SKILL_LIGHTNING_ORB_DINO);
					break;
				case MODEL_HELPER + 37:
					SetAction(pObj, PLAYER_SKILL_LIGHTNING_ORB_FENRIR);
					break;
				default:
					SetAction(pObj, PLAYER_SKILL_LIGHTNING_ORB);
					break;
				}
			}
		}
		else
		{
			switch (pCha->Helper.Type)
			{
			case MODEL_HELPER + 2:
				SetAction(pObj, PLAYER_SKILL_DRAIN_LIFE_UNI);
				break;
			case MODEL_HELPER + 3:
				SetAction(pObj, PLAYER_SKILL_DRAIN_LIFE_DINO);
				break;
			case MODEL_HELPER + 37:
				SetAction(pObj, PLAYER_SKILL_DRAIN_LIFE_FENRIR);
				break;
			default:
				SetAction(pObj, PLAYER_SKILL_DRAIN_LIFE);
				break;
			}
		}
		if (pCharacter)
		{
			SendRequestMagicContinue(iSkill, (int)(pCharacter->TargetPosition[0] / 100.f), (int)(pCharacter->TargetPosition[1] / 100.f), (BYTE)(pObj->Angle[2] / 360.f * 256.f), 0, 0, g_MovementSkill.m_iKey, 0);
		}
		break;
	case Skill_Chain_Lightning:
	case Skill_Chain_Lightning_Strengthener:
		LetHeroStop();
		switch (pCha->Helper.Type)
		{
		case MODEL_HELPER + 2:
			SetAction(pObj, PLAYER_SKILL_CHAIN_LIGHTNING_UNI);
			break;
		case MODEL_HELPER + 3:
			SetAction(pObj, PLAYER_SKILL_CHAIN_LIGHTNING_DINO);
			break;
		case MODEL_HELPER + 37:
			SetAction(pObj, PLAYER_SKILL_CHAIN_LIGHTNING_FENRIR);
			break;
		default:
			SetAction(pObj, PLAYER_SKILL_CHAIN_LIGHTNING);
			break;
		}
		if (pCharacter)
		{
			SendRequestMagicContinue(iSkill, (int)(pCharacter->TargetPosition[0] / 100.f), (int)(pCharacter->TargetPosition[1] / 100.f), (BYTE)(pObj->Angle[2] / 360.f * 256.f), 0, 0, g_MovementSkill.m_iKey, 0);
		}
		break;
	case Skill_Damage_Reflection:
	case Skill_Sleep:
	case 220:
	case Skill_Sleep_Strengthener:
		LetHeroStop();
		if (pCharacter)
		{
			pCha->TargetPosition[0] = pCharacter->Object.Position[0];
			pCha->TargetPosition[1] = pCharacter->Object.Position[1];
			pCha->TargetPosition[2] = pCharacter->Object.Position[2];

			pObj->Angle[2] = CreateAngle(pObj->Position[0], pObj->Position[1], pCha->TargetPosition[0], pCha->TargetPosition[1]);
			SendRequestMagic(iSkill, g_MovementSkill.m_iKey);
		}
		break;
	case Skill_Berserker:
		goto LABEL_45;
	case Skill_Weakness:
	case Skill_Innovation:
		LetHeroStop();
		SendRequestMagicContinue(iSkill, pCha->PositionX, pCha->PositionY, (BYTE)(pObj->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, 0);
		switch (pCha->Helper.Type)
		{
		case MODEL_HELPER + 2:
			SetAction(pObj, PLAYER_SKILL_SLEEP_UNI);
			break;
		case MODEL_HELPER + 3:
			SetAction(pObj, PLAYER_SKILL_SLEEP_DINO);
			break;
		case MODEL_HELPER + 37:
			SetAction(pObj, PLAYER_SKILL_SLEEP_FENRIR);
			break;
		default:
			SetAction(pObj, PLAYER_SKILL_SLEEP);
			break;
		}
		break;
	default:
		return;
	}
}

#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
void UseSkillRagefighter(CHARACTER* pCha, OBJECT* pObj)
{
	int iSkill;

	if (g_MovementSkill.m_bMagic)
		iSkill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];
	else
		iSkill = g_MovementSkill.m_iSkill;

	ITEM* pLeftRing = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];
	ITEM* pRightRing = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];

	if (g_CMonkSystem.IsChangeringNotUseSkill(pLeftRing->Type, pRightRing->Type, pLeftRing->Level, pRightRing->Level) == false)
	{
		if (g_CMonkSystem.IsRideNotUseSkill(iSkill, pCha->Helper.Type) == false)
		{
			if (g_CMonkSystem.IsSwordformGlovesUseSkill(iSkill) == true)
			{
				LetHeroStop();
				pCha->Movement = false;

				if (pObj->Type == MODEL_PLAYER)
				{
					g_CMonkSystem.SetRageSkillAni(iSkill, pObj);
					SetAttackSpeed();
				}

				CHARACTER* pCharacter;

				if (iSkill > Skill_Killing_Blow_Strengthener)
				{
					switch (iSkill)
					{
					case Skill_Killing_Blow_Mastery:
						goto LABEL_13;
					case Skill_Beast_Uppercut_Strengthener:
					case Skill_Beast_Uppercut_Mastery:
					LABEL_44:
					{
						WORD wTargetKey = 0;
						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							wTargetKey = g_MovementSkill.m_iKey;
						}

						SendRequestRageAtt(iSkill, wTargetKey);
						g_CMonkSystem.InitConsecutiveState(3.0f, 7.0f, 0);
					}
					break;
					case Skill_Chain_Drive_Strengthener:
					LABEL_49:
					{
						WORD wTargetKey = 0;
						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							wTargetKey = g_MovementSkill.m_iKey;
						}

						SendRequestRageAtt(iSkill, wTargetKey);
						g_CMonkSystem.InitConsecutiveState(3.0f, 12.0f);
						pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
						g_CMonkSystem.RageCreateEffect(pObj, iSkill);
					}
					break;
					case Skill_Dark_Side_Strengthener:
					LABEL_59:
					{
						WORD wTargetKey = 0;
						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							wTargetKey = g_MovementSkill.m_iKey;
						}

						SendRequestDarkside(iSkill, wTargetKey);

						SendRequestRageAtt(iSkill, wTargetKey);

						pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;

						g_CMonkSystem.RageCreateEffect(pObj, iSkill);
					}
					break;
					case Skill_Dragon_Roar_Strengthener:
					LABEL_64:
					{
						WORD TKey = 0xffff;
						BYTE angle = (BYTE)((((pObj->Angle[2] + 180.f) / 360.f) * 255.f));

						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							TKey = g_MovementSkill.m_iKey;
						}
						BYTE byValue = GetDestValue((pCha->PositionX), (pCha->PositionY), TargetX, TargetY);
						SendRequestMagicContinue(iSkill, pCha->PositionX, pCha->PositionY, ((pObj->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);

						pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
						g_CMonkSystem.RageCreateEffect(pObj, iSkill);
					}
					break;
					case Skill_Defense_Success_Rate_Increase_P:
					case Skill_DefSuccessRate_Increase_Mastery:
					case Skill_Stamina_Increase_Strengthener:
					case 575:
					LABEL_69:
					{
						SendRequestMagic(iSkill, HeroKey);
						if (rand() % 2 == 0)
						{
							SetAction(pObj, PLAYER_SKILL_ATT_UP_OURFORCES);
							PlayBuffer(SOUND_RAGESKILL_BUFF_1);
						}
						else
						{
							SetAction(pObj, PLAYER_SKILL_HP_UP_OURFORCES);
							PlayBuffer(SOUND_RAGESKILL_BUFF_2);
						}
						g_CMonkSystem.RageCreateEffect(pObj, iSkill);
					}
					break;
					default:
						return;
					}
				}
				else if (iSkill == Skill_Killing_Blow_Strengthener)
				{
				LABEL_13:
					WORD wTargetKey = 0;
					pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

					if (pCharacter)
					{
						wTargetKey = g_MovementSkill.m_iKey;
						pCha->TargetPosition[0] = pCharacter->Object.Position[0];
						pCha->TargetPosition[1] = pCharacter->Object.Position[1];
						pCha->TargetPosition[2] = pCharacter->Object.Position[2];
					}

					pObj->Angle[2] = CreateAngle(pObj->Position[0], pObj->Position[1], pCha->TargetPosition[0], pCha->TargetPosition[1]);
					SendRequestMagic(iSkill, wTargetKey);

					BYTE positionX = (BYTE)(pCha->TargetPosition[0] / TERRAIN_SCALE);
					BYTE positionY = (BYTE)(pCha->TargetPosition[1] / TERRAIN_SCALE);

					bool IsAttackMonster = true;

					if (pCharacter && (pCharacter->MonsterIndex == 277
						|| pCharacter->MonsterIndex == 283
						|| pCharacter->MonsterIndex == 278
						|| pCharacter->MonsterIndex == 288))
					{
						IsAttackMonster = false;
					}

					if (gMapManager->InBloodCastle() == true || iSkill == Skill_Charge || !IsAttackMonster)
					{
						int angle = abs((int)(pObj->Angle[2] / 45.0));

						switch (angle)
						{
						case 0:
							++positionY;
							break;
						case 1:
							--positionX;
							++positionY;
							break;
						case 2:
							--positionX;
							break;
						case 3:
							--positionX;
							--positionY;
							break;
						case 4:
							--positionY;
							break;
						case 5:
							++positionX;
							--positionY;
							break;
						case 6:
							++positionX;
							break;
						case 7:
							++positionX;
							++positionY;
							break;
						default:
							break;
						}
					}
					int TargetIndex = TERRAIN_INDEX(positionX, positionY);

					vec3_t vDis;
					Vector(0.0f, 0.0f, 0.0f, vDis);
					VectorSubtract(pCha->TargetPosition, pCha->Object.Position, vDis);
					VectorNormalize(vDis);
					VectorScale(vDis, TERRAIN_SCALE, vDis);
					VectorSubtract(pCha->TargetPosition, vDis, vDis);
					BYTE CharPosX = (BYTE)(vDis[0] / TERRAIN_SCALE);
					BYTE CharPosY = (BYTE)(vDis[1] / TERRAIN_SCALE);

					if ((TerrainWall[TargetIndex] & TW_NOMOVE) != TW_NOMOVE && (TerrainWall[TargetIndex] & TW_NOGROUND) != TW_NOGROUND)
						SendPosition(CharPosX, CharPosY);
					pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
					g_CMonkSystem.RageCreateEffect(pObj, iSkill);
				}
				else
				{
					switch (iSkill)
					{
					case Skill_Killing_Blow:
					case Skill_Charge:
						goto LABEL_13;
					case Skill_Beast_Uppercut:
						goto LABEL_44;
					case Skill_Chain_Drive:
						goto LABEL_49;
					case Skill_Dragon_Slasher:
					{
						WORD wTargetKey = 0;
						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							wTargetKey = g_MovementSkill.m_iKey;
						}

						SendRequestRageAtt(iSkill, wTargetKey);
						g_CMonkSystem.InitConsecutiveState(3.0, 0.0, 0);
						pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
						g_CMonkSystem.RageCreateEffect(pObj, iSkill);
						return;
					}
					case Skill_Dark_Side:
						goto LABEL_59;
					case Skill_Dragon_Roar:
						goto LABEL_64;
					case Skill_Ignore_Defense:
					case Skill_Increase_Health:
					case Skill_Increase_Block:
						goto LABEL_69;
					case Skill_Plasma_Storm:
					{
						pObj->Angle[2] = CreateAngle(pObj->Position[0], pObj->Position[1], pCha->TargetPosition[0], pCha->TargetPosition[1]);
						gSkillManager.CheckSkillDelay(Hero->CurrentSkill);

						BYTE pos = CalcTargetPos(pObj->Position[0], pObj->Position[1], (int)(pCha->TargetPosition[0]), (int)(pCha->TargetPosition[1]));
						WORD TKey = getTargetCharacterKey(pCha, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						pCha->m_iFenrirSkillTarget = g_MovementSkill.m_iTarget;

						SendRequestMagicContinue(iSkill, (pCha->PositionX), (pCha->PositionY), (BYTE)(pObj->Angle[2] / 360.f * 256.f), 0, pos, TKey, &pObj->m_bySkillSerialNum);
						pCha->Movement = 0;
						if (pObj->Type == MODEL_PLAYER)
						{
							SetAction_Fenrir_Skill(pCha, pObj);
						}
					}
					break;
					case Skill_Phoenix_Shot:
					{
						BYTE angle = (signed int)((pObj->Angle[2] + 180.0) / 360.0 * 255.0);
						WORD TKey = 0xffff;
						pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);
						if (pCharacter)
						{
							TKey = g_MovementSkill.m_iKey;
						}

						BYTE byValue = GetDestValue(pCha->PositionX, pCha->PositionY, TargetX, TargetY);
						SendRequestMagicContinue(iSkill, pCha->PositionX, pCha->PositionY, ((pObj->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);

						pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
						pObj->Angle[2] = CreateAngle(pObj->Position[0], pObj->Position[1], pCha->TargetPosition[0], pCha->TargetPosition[1]);
						g_CMonkSystem.RageCreateEffect(&pCha->Object, iSkill);
					}
					break;
					default:
						return;
					}
				}
			}
		}
	}
}

void AttackRagefighter(CHARACTER* pCha, int nSkill, float fDistance)
{
	OBJECT* pObj = &pCha->Object;

	int iMana, iSkillMana;
	gSkillManager.GetSkillInformation(nSkill, 1, NULL, &iMana, NULL, &iSkillMana);

	if (CharacterAttribute->Mana < (unsigned int)iMana)
	{
		int Index = g_pMyInventory->FindManaItemIndex();

		if (Index != -1)
		{
			SendRequestUse(Index, 0);
		}
		return;
	}

	if ((unsigned int)iSkillMana > CharacterAttribute->SkillMana || !gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
		return;

	bool bSuccess = CheckTarget(pCha);
	bool bCheckAttack = CheckAttack();
	g_MovementSkill.m_bMagic = TRUE;
	g_MovementSkill.m_iSkill = Hero->CurrentSkill;

	if (bCheckAttack)
	{
		g_MovementSkill.m_iTarget = SelectedCharacter;
		CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);
		g_MovementSkill.m_iKey = tc ? tc->Key : 0;
	}
	else
	{
		g_MovementSkill.m_iTarget = -1;
		g_MovementSkill.m_iKey = 0;
	}
	if (!bSuccess)
		return;

	if (nSkill > Skill_Beast_Uppercut_Mastery)
	{
		switch (nSkill)
		{
		case Skill_Chain_Drive_Strengthener:
		case Skill_Dragon_Roar_Strengthener:
			goto LABEL_38;
		case Skill_Dark_Side_Strengthener:
			goto LABEL_49;
		case Skill_Defense_Success_Rate_Increase_P:
		case Skill_DefSuccessRate_Increase_Mastery:
		case Skill_Stamina_Increase_Strengthener:
		case 575:
		LABEL_50:
			UseSkillRagefighter(pCha, pObj);
			break;
		default:
			return;
		}
		return;
	}
	if (nSkill >= Skill_Killing_Blow_Mastery)
	{
	LABEL_38:
		CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);

		if (tc)
		{
			fDistance = gSkillManager.GetSkillDistance(nSkill, pCha);
			pCha->TargetCharacter = SelectedCharacter;
			if (tc->Dead == 0)
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

				if (pCharacter)
				{
					TargetX = pCharacter->PositionX;
					TargetY = pCharacter->PositionY;
					if (bCheckAttack)
					{
						bool bNoneWall = CheckWall(pCha->PositionX, pCha->PositionY, TargetX, TargetY, 1);
						if (CheckTile(pCha, TargetX, TargetY, fDistance) && !pCha->SafeZone && bNoneWall)
						{
							UseSkillRagefighter(pCha, pObj);
						}
						else if (PathFinding2(pCha->PositionX, pCha->PositionY, TargetX, TargetY, &pCha->Path, fDistance))
						{
							pCha->Movement = true;
							pCha->MovementType = MOVEMENT_SKILL;
							g_pNewUIMiniMap->StopMove();
							SendMove(pCha, pObj);
						}
					}
				}
			}
		}
		return;
	}
	if (nSkill > Skill_Dark_Side)
	{
		if (nSkill > Skill_Phoenix_Shot)
		{
			if (nSkill < Skill_Killing_Blow_Strengthener || nSkill > Skill_Beast_Uppercut_Strengthener)
				return;
			goto LABEL_38;
		}
		if (nSkill < Skill_Charge)
		{
			switch (nSkill)
			{
			case Skill_Dragon_Roar:
			case Skill_Dragon_Slasher:
				goto LABEL_38;
			case Skill_Ignore_Defense:
			case Skill_Increase_Health:
			case Skill_Increase_Block:
				goto LABEL_50;
			default:
				return;
			}
			return;
		}
		goto LABEL_38;
	}
	if (nSkill == Skill_Dark_Side)
	{
	LABEL_49:
		UseSkillRagefighter(pCha, pObj);
		return;
	}
	if (nSkill != Skill_Plasma_Storm)
	{
		if (nSkill <= 259 || nSkill > Skill_Chain_Drive)
			return;
		goto LABEL_38;
	}
	if (!gMapManager->InChaosCastle())
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

		if (pCharacter)
		{
			TargetX = pCharacter->PositionX;
			TargetY = pCharacter->PositionY;
			if (CheckTile(pCha, TargetX, TargetY, fDistance))
			{
				if (g_MovementSkill.m_iTarget == -1)
					pCha->m_iFenrirSkillTarget = -1;
				else
					UseSkillRagefighter(pCha, pObj);
			}
			else if (g_MovementSkill.m_iTarget == -1)
			{
				Attacking = -1;
			}
			else if (PathFinding2(pCha->PositionX, pCha->PositionY, TargetX, TargetY, &pCha->Path, fDistance))
			{
				pCha->Movement = true;
			}
		}
	}
}

bool UseSkillRagePosition(CHARACTER* pCha)
{
	OBJECT* pObj = &pCha->Object;

	if (pObj->CurrentAction == PLAYER_SKILL_GIANTSWING || pObj->CurrentAction == PLAYER_SKILL_STAMP || pObj->CurrentAction == PLAYER_SKILL_DRAGONKICK)
	{
		if (g_CMonkSystem.IsConsecutiveAtt(pObj->AnimationFrame))
		{
			int iSkill = g_MovementSkill.m_bMagic ? CharacterAttribute->Skill[g_MovementSkill.m_iSkill] : g_MovementSkill.m_iSkill;
			return g_CMonkSystem.SendAttackPacket(pCha, g_MovementSkill.m_iTarget, iSkill);
		}

		if (pObj->CurrentAction == PLAYER_SKILL_STAMP && pObj->AnimationFrame >= 2.0f)
		{
			pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
			g_CMonkSystem.RageCreateEffect(pObj, AT_SKILL_STAMP);
		}
	}
	else if (pObj->CurrentAction == PLAYER_SKILL_DARKSIDE_READY)
	{
		int AttTime = (int)(2.5f / gmClientModels->GetModel(pObj->Type)->Actions[PLAYER_SKILL_DARKSIDE_READY].PlaySpeed);
		if (pCha->AttackTime >= AttTime)
		{
			if (g_CMonkSystem.SendDarksideAtt(pObj))
			{
				pCha->AttackTime = 1;
				return true;
			}
		}
		return false;
	}
	else if (pObj->CurrentAction == PLAYER_SKILL_THRUST)
	{
		pObj->Angle[2] = CreateAngle(pObj->Position[0], pObj->Position[1], pCha->TargetPosition[0], pCha->TargetPosition[1]);
		pObj->m_sTargetIndex = g_MovementSkill.m_iTarget;
	}
	else
	{
		g_CMonkSystem.InitConsecutiveState();
		g_CMonkSystem.InitEffectOnce();
	}
	return false;
}
#endif //PBG_ADD_NEWCHAR_MONK_SKILL

void ReloadArrow()
{
	int Index = SearchArrow();

	if (Index != -1)
	{
		ITEM* rp = NULL;
		ITEM* lp = NULL;

		bool Success = false;

		if (gCharacterManager.GetBaseClass(CharacterAttribute->Class) == CLASS_ELF && SEASON3B::CNewUIInventoryCtrl::GetPickedItem() == NULL)
		{
			rp = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT];
			lp = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];
			Success = true;
		}

		if (Success)
		{
			int EquipedBowType = gCharacterManager.GetEquipedBowType(rp);
			/*if ((gCharacterManager.GetEquipedBowType(lp) == BOWTYPE_BOW) && (rp->Type == -1))
			{
				ITEM* pItem = g_pMyInventory->FindItem(Index);

				int InvenIndex = g_pMyInventory->FindInvenIndex(Index);
				SEASON3B::CNewUIInventoryCtrl::CreatePickedItem(g_pMyInventory->GetInventoryCtrl(InvenIndex), pItem);

				if (pItem)
				{
					SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, Index + MAX_EQUIPMENT, pItem, REQUEST_EQUIPMENT_INVENTORY, EQUIPMENT_WEAPON_RIGHT);
				}
				g_pMyInventory->DeleteItem(Index);
#ifdef NEW_CHAT_SPIT
				g_pSystemLogBox->AddText(GlobalText[250], SEASON3B::TYPE_SYSTEM_MESSAGE);
#else
				g_pChatListBox->AddText("", GlobalText[250], SEASON3B::TYPE_SYSTEM_MESSAGE);
#endif
			}
			else */if ((EquipedBowType == BOWTYPE_CROSSBOW || EquipedBowType == BOWTYPE_BOW) && (lp->Type == -1))
			{
				ITEM* pItem = g_pMyInventory->FindItem(Index);

				int InvenIndex = g_pMyInventory->FindInvenIndex(Index);
				SEASON3B::CNewUIInventoryCtrl::CreatePickedItem(g_pMyInventory->GetInventoryCtrl(InvenIndex), pItem);

				if (pItem)
				{
					SendRequestEquipmentItem(REQUEST_EQUIPMENT_INVENTORY, Index + MAX_EQUIPMENT, pItem, REQUEST_EQUIPMENT_INVENTORY, EQUIPMENT_WEAPON_LEFT);
				}
				g_pMyInventory->DeleteItem(Index);
				g_pChatListBox->AddText("", GlobalText[250], SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
		}
	}
	else
	{
		if (g_pChatListBox->CheckChatRedundancy(GlobalText[251]) == FALSE)
		{
			g_pChatListBox->AddText("", GlobalText[251], SEASON3B::TYPE_ERROR_MESSAGE);
		}
	}
}

bool CheckArrow()
{
	if (gCharacterManager.GetEquipedBowType() == BOWTYPE_CROSSBOW)
	{
		int weaponkind = GMItemMng->GetKind2(CharacterMachine->Equipment[1].Type);

		if ((weaponkind != ItemKind2::BOLT && weaponkind != ItemKind2::QUIVER) || (CharacterMachine->Equipment[1].Durability <= 0))
		{
			ReloadArrow();
			return false;
		}
	}
	else if (gCharacterManager.GetEquipedBowType() == BOWTYPE_BOW)
	{
		int weaponkind = GMItemMng->GetKind2(CharacterMachine->Equipment[1].Type);

		if ((weaponkind != ItemKind2::ARROW && weaponkind != ItemKind2::QUIVER) || (CharacterMachine->Equipment[1].Durability <= 0))
		{
			ReloadArrow();
			return false;
		}
	}
	//int Right = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;
	//int Left = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
	//
	//if (gCharacterManager.GetEquipedBowType() == BOWTYPE_CROSSBOW)
	//{
	//	if ((Left != ITEM_BOW + 7) || (CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Durability <= 0))
	//	{
	//		ReloadArrow();
	//		return false;
	//	}
	//}
	//else if (gCharacterManager.GetEquipedBowType() == BOWTYPE_BOW)
	//{
	//	if ((Right != ITEM_BOW + 15) || (CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Durability <= 0))
	//	{
	//		ReloadArrow();
	//		return false;
	//	}
	//}
	return true;
}

bool SkillElf(CHARACTER* c, ITEM* p)
{
	OBJECT* o = &c->Object;
	bool Success = 0;

	for (int i = 0; i < p->SpecialNum; i++)
	{
		bool have = false;

		for (int j = CharacterAttribute->Skill[Hero->CurrentSkill]; j; j = SkillAttribute[j].SkillBrand)
		{
			if (p->Special[i] == j)
			{
				have = true;
				break;
			}
		}
		if (have)
		{
			int iMana, iSkillMana;
			int Spe_Num = CharacterAttribute->Skill[Hero->CurrentSkill];

			gSkillManager.GetSkillInformation(Spe_Num, 1, NULL, &iMana, NULL, &iSkillMana);

			if (g_isCharacterBuff(o, eBuff_InfinityArrow)
				|| g_isCharacterBuff(o, EFFECT_INFINITY_ARROW_IMPROVED))
			{
				iMana += CharacterMachine->InfinityArrowAdditionalMana;
			}
			if (CharacterAttribute->Mana <= (unsigned int)iMana)
			{
				int Index = g_pMyInventory->FindManaItemIndex();
				if (Index != -1)
				{
					SendRequestUse(Index, 0);
				}
				continue;
			}
			if ((unsigned int)iSkillMana > CharacterAttribute->SkillMana)
				return false;
			if (!gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
				return false;

			float Distance = gSkillManager.GetSkillDistance(Spe_Num, c);

			if (Spe_Num > Skill_Triple_Shot_Strengthener)
			{
				if (Spe_Num != Skill_Triple_Shot_Mastery)
					continue;
			}
			else if (Spe_Num != Skill_Triple_Shot_Strengthener && Spe_Num != AT_SKILL_CROSSBOW)
			{
				if (Spe_Num == AT_SKILL_BLAST_CROSSBOW4)
				{
					if (CheckArrow())
					{
						if (CheckTile(c, o, Distance))
						{

							o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
							BYTE byValue = GetDestValue((c->PositionX), (c->PositionY), TargetX, TargetY);

							BYTE angle = (BYTE)((((o->Angle[2] + 180.f) / 360.f) * 255.f));
							WORD TKey = 0xffff;
							if (g_MovementSkill.m_iTarget != -1)
							{
								TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
							}
							SendRequestMagicContinue(Spe_Num, (c->PositionX),
								(c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);
							SetPlayerAttack(c);
							if (o->Type != MODEL_PLAYER)
							{
								CreateArrows(c, o, NULL, FindHotKey((c->Skill)), Spe_Num - AT_SKILL_PIERCING);
							}

							Success = true;
						}
					}
					else
					{
						i = p->SpecialNum;
					}
				}
				continue;
			}
			if (CheckArrow())
			{
				if (CheckTile(c, o, Distance))
				{
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					WORD TKey = 0xffff;
					if (g_MovementSkill.m_iTarget != -1)
					{
						TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
					}
					SendRequestMagicContinue(Spe_Num, (c->PositionX),
						(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, TKey, 0);
					SetPlayerAttack(c);
					if (o->Type != MODEL_PLAYER && o->Kind != KIND_PLAYER)
						CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 1);
					Success = true;
				}
			}
			else
			{
				i = p->SpecialNum;
			}
		}
	}
	return Success;
}

void ActionMovementAttack(CHARACTER* c, OBJECT* o)
{
	float Range = 1.8f;
	int Right = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;
	int Left = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;

	if (Right >= ITEM_SPEAR && Right < ITEM_SPEAR + MAX_ITEM_INDEX)
		Range = 2.2f;
	if (gCharacterManager.GetEquipedBowType())
		Range = 6.f;

	if (c->MonsterIndex == 9)
		Range = 7.f;

	if (ActionTarget == -1)
		return;

	if (o->Type == MODEL_PLAYER)
	{
		if (o->CurrentAction >= PLAYER_HIT_FIST && o->CurrentAction <= PLAYER_RIDE_SKILL
			&& o->CurrentAction != PLAYER_STOP_RIDE_HORSE
			&& o->CurrentAction != PLAYER_STOP_TWO_HAND_SWORD_TWO
			&& o->CurrentAction == PLAYER_FENRIR_SKILL_ONE_RIGHT
			&& o->CurrentAction == PLAYER_RAGE_FENRIR_ONE_RIGHT
			)
		{
			return;
		}
	}
	else if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
	{
		return;
	}

	if (!c->SafeZone)
	{
		CHARACTER* tc = gmCharacters->GetCharacter(ActionTarget);
		if (tc)
		{
			float newRange = Range * TERRAIN_SCALE;

			if (CheckDistance(c, tc, newRange) == true)
			{
				MouseUpdateTime = MouseUpdateTimeMax;
				SetPlayerAttack(c);
				c->AttackTime = 1;
				VectorCopy(tc->Object.Position, c->TargetPosition);
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

				LetHeroStop(NULL, 0);
				c->Movement = false;

				c->TargetCharacter = ActionTarget;
				int Dir = ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8);
				c->Skill = 0;
				SendRequestAttack(tc->Key, Dir);
			}
			else if (gCharacterManager.GetBaseClass(c->Class) == CLASS_ELF)
			{
				if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Range, 2))
				{
					c->Movement = true;
				}
			}
		}
	}
}

void ActionMovementSkill(CHARACTER* c, OBJECT* o)
{
	float Range = 1.8f;

	int iSkill = 0;

	if (g_MovementSkill.m_bMagic)
		iSkill = CharacterAttribute->Skill[g_MovementSkill.m_iSkill];
	else
		iSkill = g_MovementSkill.m_iSkill;

	float Distance = gSkillManager.GetSkillDistance(iSkill, c);

	CHARACTER* tc = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

	if (tc && tc->Dead == 0)
	{
		TargetX = tc->PositionX;
		TargetY = tc->PositionY;

		if (iSkill <= 262)
		{
			if (iSkill < 260)
			{
				switch (iSkill)
				{
				case 47:
					if (Hero->Helper.Type == MODEL_HELPER + 2)
						goto LABEL_41;
					return;
				case 19:
				case 20:
				case 21:
				case 22:
				case 23:
				case 43:
				case 44:
				case 49:
				case 57:
				case 60:
				case 61:
				case 74:
					goto LABEL_41;
				case 1:
				case 2:
				case 3:
				case 4:
				case 7:
				case 11:
				case 17:
				case 45:
				case 73:
					goto LABEL_49;
				case 46:
				case 51:
					goto LABEL_59;
				case 26:
				case 27:
				case 28:
					goto LABEL_69;
				case 55:
					if (!CheckTile(c, TargetX, TargetY, Distance) || c->SafeZone)
					{
						if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
							c->Movement = true;
					}
					else if (CheckWall(c->PositionX, c->PositionY, TargetX, TargetY, true))
					{
						AttackKnight(c, iSkill, Distance);
					}
					break;
				default:
					return;
				}
				return;
			}
			goto LABEL_89;
		}
		if (iSkill > 552)
		{
			switch (iSkill)
			{
			case 554:
			case 555:
			case 558:
			case 560:
				goto LABEL_89;
			default:
				return;
			}
		}
		else
		{
			if (iSkill >= 551)
			{
			LABEL_89:
				if (CheckTile(c, TargetX, TargetY, Distance) && !c->SafeZone)
				{
					if (CheckWall(c->PositionX, c->PositionY, TargetX, TargetY, true))
					{
						UseSkillRagefighter(c, o);
					}
					else if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
					{
						c->Movement = true;
					}
				}
				return;
			}
			switch (iSkill)
			{
			case 326:
			case 327:
			case 328:
			case 329:
			case 336:
			case 479:
			case 508:
			case 509:
			case 514:
			LABEL_41:
				if (!CheckTile(c, TargetX, TargetY, Distance) || c->SafeZone)
				{
					if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
						c->Movement = true;
				}
				else
				{
					UseSkillWarrior(c, o);
				}
				break;
			case 379:
			case 384:
			case 389:
			case 480:
			case 489:
			LABEL_49:
				if (!CheckTile(c, TargetX, TargetY, Distance) || c->SafeZone)
				{
					if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
						c->Movement = true;
				}
				else if (CheckWall(c->PositionX, c->PositionY, TargetX, TargetY, false))
				{
					UseSkillWizard(c, o);
				}
				break;
			case 424:
			LABEL_59:
				if (!CheckTile(c, TargetX, TargetY, Distance) || c->SafeZone)
				{
					if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
						c->Movement = true;
				}
				else if (CheckWall(c->PositionX, c->PositionY, TargetX, TargetY, false))
				{
					UseSkillElf(c, o);
				}
				break;
			case 413:
			case 417:
			case 420:
			case 422:
			case 423:
			LABEL_69:
				if (!CheckTile(c, TargetX, TargetY, Distance) || c->SafeZone)
				{
					if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
						c->Movement = true;
				}
				else if (CheckWall(c->PositionX, c->PositionY, TargetX, TargetY, false))
				{
					UseSkillElf(c, o);
				}
				break;
			case 264:
			case 265:
			case 269:
			case 270:
				goto LABEL_89;
			default:
				return;
			}
		}
	}
}

void ActionMovementGet(CHARACTER* c, OBJECT* o)
{
	if (!g_bAutoGetItem)
	{
		if (!CheckTile(c, TargetX, TargetY, 1.5))
			return;
		MouseUpdateTimeMax = 6;
	}

	if (Items[ItemKey].Item.Type == ITEM_POTION + 15)
	{
		SendRequestGetItem(ItemKey);
	}
	else if (g_pMyInventory->FindEmptySlot(&Items[ItemKey].Item) == -1)
	{
		unicode::t_char Text[256];
		unicode::_sprintf(Text, GlobalText[375]);

		g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);

		OBJECT* pItem = &(Items[ItemKey].Object);
		pItem->Position[2] = RequestTerrainHeight(pItem->Position[0], pItem->Position[1]) + 3.f;
		pItem->Gravity = 50.f;
	}
	else
	{
		SendRequestGetItem(ItemKey);
	}
}

void ActionMovementTalk(CHARACTER* c, OBJECT* o)
{
	MouseUpdateTimeMax = 12;

	if (M34CryWolf1st::Get_State_Only_Elf() != true || M34CryWolf1st::IsCyrWolf1st() != true)
	{
		SetPlayerStop(c);
		c->Movement = false;
	}

	CHARACTER* tc = gmCharacters->GetCharacter(TargetNpc);

	if (tc)
	{
		if (tc->Object.Kind == KIND_NPC)
		{
			int level = CharacterAttribute->Level;

			if (tc->MonsterIndex == 238 && level < 10)
			{
				char Text[100];
				sprintf(Text, GlobalText[663], CHAOS_MIX_LEVEL);
				g_pChatListBox->AddText("", Text, SEASON3B::TYPE_SYSTEM_MESSAGE);
				return;
			}

			if (tc->MonsterIndex != 243 && tc->MonsterIndex != 246 && tc->MonsterIndex != 251 && tc->MonsterIndex != 416 && tc->MonsterIndex != 578)
			{
				g_pNPCShop->SetRepairShop(false);
			}
			else
			{
				g_pNPCShop->SetRepairShop(true);
			}

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYQUEST))
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MYQUEST);
			}
			if (g_csQuest.IsInit())
			{
				SendRequestQuestHistory();
			}
			if (M34CryWolf1st::Get_State_Only_Elf() != true || M34CryWolf1st::IsCyrWolf1st() != true)
			{
				if (M34CryWolf1st::IsCyrWolf1st() == true)
				{
					if (tc->Object.Type < MODEL_CRYWOLF_ALTAR1 || tc->Object.Type > MODEL_CRYWOLF_ALTAR5)
					{
						if (tc->Object.Type == MODEL_NPC_QUARREL)
							SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CMapEnterWerwolfMsgBoxLayout));
						SendRequestTalk(tc->Key);
					}
				}
				else
				{
					if (SEASON3A::CGM3rdChangeUp::Instance().IsBalgasBarrackMap())
					{
						SendRequestTalk(tc->Key);
						SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CMapEnterGateKeeperMsgBoxLayout));
					}
					else if (tc->MonsterIndex < 468 || tc->MonsterIndex > 475)
					{
						if (tc->MonsterIndex == 478 || tc->MonsterIndex == 540 || tc->MonsterIndex == 547 || tc->MonsterIndex == 579)
						{
							SendRequestTalk(tc->Key);
						}
						else if (M38Kanturu2nd::Is_Kanturu2nd() == true)
						{
							if (!g_pKanturu2ndEnterNpc->IsNpcAnimation())
							{
								SendRequestTalk(tc->Key);
							}
						}
						else if (gMapManager->IsCursedTemple())
						{
							if (!g_CursedTemple->IsGaugebarEnabled())
							{
								if (tc->MonsterIndex == 380 || ((tc->MonsterIndex == 383 || tc->MonsterIndex == 384) && g_pCursedTempleWindow->CheckInventoryHolyItem(Hero)))
								{
									g_CursedTemple->SetGaugebarEnabled(true);
								}
								g_pCursedTempleWindow->CheckTalkProgressNpc(tc->MonsterIndex, tc->Key);
							}
						}
						else
						{
							SendRequestTalk(tc->Key);
						}
					}
					else
					{
						char _Temp[32] = { 0, };
						SendRequestTalk(tc->Key);

						if (tc->MonsterIndex == 470)
						{
							sprintf(_Temp, GlobalText[2596], 100);
							g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
						}
						else if (tc->MonsterIndex == 471)
						{
							sprintf(_Temp, GlobalText[2597], 100);
							g_pChatListBox->AddText("", _Temp, SEASON3B::TYPE_SYSTEM_MESSAGE);
						}
					}
				}
			}
			else if (tc->Object.Type < MODEL_CRYWOLF_ALTAR1 || tc->Object.Type > MODEL_CRYWOLF_ALTAR5)
			{
				SendRequestTalk(tc->Key);
			}
			else
			{
				int Num = tc->Object.Type - MODEL_CRYWOLF_ALTAR1;
				if (gCharacterManager.GetBaseClass(Hero->Class) != CLASS_ELF || M34CryWolf1st::Get_AltarState_State(Num))
				{
					if (gCharacterManager.GetBaseClass(Hero->Class) != CLASS_ELF || M34CryWolf1st::Get_AltarState_State(Num) != true)
					{
						SendRequestTalk(tc->Key);
					}
					else
					{
						SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Ing_Set_Temple));
					}
				}
				else
				{
					BYTE State = (m_AltarState[Num] & 0x0f);
					if (State > 0)
						SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Get_Temple));
					else
						SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CCry_Wolf_Destroy_Set_Temple));
				}
			}
			bCheckNPC = (tc->MonsterIndex == 229);
			if (tc->MonsterIndex == 226)
			{
				ITEM* pItem = &CharacterMachine->Equipment[EQUIPMENT_HELPER];

				if (pItem->Type == ITEM_HELPER + 4 || pItem->Kind3 == ItemKind3::PET_GAIN_EXPERIENCE)
					SendRequestPetInfo(PET_TYPE_DARK_HORSE, 0, EQUIPMENT_HELPER);

				pItem = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];
				if (pItem->Type == ITEM_HELPER + 5)
					SendRequestPetInfo(PET_TYPE_DARK_SPIRIT, 0, EQUIPMENT_WEAPON_LEFT);
			}
		}
		TargetNpc = -1;
	}
}

void ActionMovementOperate(CHARACTER* c, OBJECT* o)
{
	if (max(abs((Hero->PositionX) - TargetX), abs((Hero->PositionY) - TargetY)) <= 1)
	{
		bool ActionHealing = false;
		bool ActionSitDown = false;
		bool ActionPoseLef = false;

		if (gMapManager->currentMap == WD_0LORENCIA)
		{
			switch (TargetType)
			{
			case MODEL_POSE_BOX:
				ActionPoseLef = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case MODEL_TREE01 + 6:
				ActionSitDown = true;
				break;
			case MODEL_FURNITURE01 + 5:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case MODEL_FURNITURE01 + 6:
				ActionSitDown = true;
				break;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_1DUNGEON)
		{
			switch (TargetType)
			{
			case 60:
				ActionPoseLef = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 59:
				ActionSitDown = true;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_2DEVIAS)
		{
			switch (TargetType)
			{
			case 91:
				ActionPoseLef = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 22:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 25:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 40:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 45:
				ActionSitDown = true;
				break;
			case 55:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			case 73:
				ActionSitDown = true;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_3NORIA)
		{
			switch (TargetType)
			{
			case 8:
				ActionSitDown = true;
				break;
			case 38:
				ActionHealing = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_7ATLANSE)
		{
			switch (TargetType)
			{
			case 39:
				ActionPoseLef = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_8TARKAN)
		{
			switch (TargetType)
			{
			case 78:
				ActionSitDown = true;
				break;
			}
		}
		else if (gMapManager->InBattleCastle())
		{
			if (TargetType == 77)
			{
				if (battleCastle::GetGuildMaster(Hero))
				{
					ActionSitDown = true;
					Hero->Object.Angle[2] = TargetAngle;
				}
			}
			else if (TargetType == 84)
			{
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
			}
		}
		else if (M38Kanturu2nd::Is_Kanturu2nd())
		{
			if (TargetType == 3)
				ActionSitDown = true;
		}
		else if (gMapManager->currentMap == WD_51HOME_6TH_CHAR)
		{
			switch (TargetType)
			{
			case 103:
				ActionSitDown = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			}
		}
		else if (gMapManager->currentMap == WD_79UNITEDMARKETPLACE)
		{
			switch (TargetType)
			{
			case 67:
				ActionPoseLef = true;
				Hero->Object.Angle[2] = TargetAngle;
				break;
			}
		}
		if (ActionHealing)
		{
			if (!gCharacterManager.IsFemale(c->Class))
				SetAction(o, PLAYER_HEALING1);
			else
				SetAction(o, PLAYER_HEALING_FEMALE1);
			SendRequestAction(AT_HEALING1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
		}
		else
		{
			BYTE PathX[1];
			BYTE PathY[1];
			PathX[0] = TargetX;
			PathY[0] = TargetY;

			SendCharacterMove(Hero->Key, Hero->Object.Angle[2], 1, PathX, PathY, TargetX, TargetY);

			c->Path.PathNum = 0;
			if (ActionPoseLef)
			{
				if (!gCharacterManager.IsFemale(c->Class))
					SetAction(o, PLAYER_POSE1);
				else
					SetAction(o, PLAYER_POSE_FEMALE1);
				SendRequestAction(AT_POSE1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
			}
			if (ActionSitDown)
			{
				if ((!c->SafeZone) && (c->Helper.Type == MODEL_HELPER + 37 || c->Helper.Type == MODEL_HELPER + 2 || c->Helper.Type == MODEL_HELPER + 3 || c->Helper.Type == MODEL_HELPER + 4))
					return;

				if (!gCharacterManager.IsFemale(c->Class))
					SetAction(o, PLAYER_SIT1);
				else
					SetAction(o, PLAYER_SIT_FEMALE1);
				SendRequestAction(AT_SIT1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
			}
			PlayBuffer(SOUND_DROP_ITEM01, &Hero->Object);
		}
	}
}

void Action(CHARACTER* c, OBJECT* o, bool Now)
{
	if (c->MovementType == MOVEMENT_ATTACK)
	{
		ActionMovementAttack(c, o);
	}
	if (c->MovementType == MOVEMENT_SKILL)
	{
		ActionMovementSkill(c, o);
	}
	if (c->MovementType == MOVEMENT_GET)
	{
		ActionMovementGet(c, o);
	}
	if (c->MovementType == MOVEMENT_TALK)
	{
		ActionMovementTalk(c, o);
	}
	if (c->MovementType == MOVEMENT_OPERATE)
	{
		ActionMovementOperate(c, o);
	}
}

void CloseNPCGMWindow()
{
	if (!g_pUIManager->IsOpen(INTERFACE_NPCGUILDMASTER))
		return;
	g_pUIManager->Close(INTERFACE_NPCGUILDMASTER);
}

void SendMove(CHARACTER* c, OBJECT* o)
{
	if (g_pNewUISystem->IsImpossibleSendMoveInterface() == true)
	{
		return;
	}

	if (g_isCharacterBuff(o, eDeBuff_Harden))
	{
		return;
	}
	if (g_isCharacterBuff(o, eDeBuff_CursedTempleRestraint))
	{
		return;
	}

	if (c->Path.PathNum <= 2)
	{
		MouseUpdateTimeMax = 0;
	}
	else if (c->Path.PathNum == 3)
	{
		MouseUpdateTimeMax = 5;
	}
	else
	{
		MouseUpdateTimeMax = 10 + (c->Path.PathNum - 2) * 3;
	}

	SendCharacterMove(Hero->Key, o->Angle[2], c->Path.PathNum, &c->Path.PathX[0], &c->Path.PathY[0], TargetX, TargetY);

	c->Movement = true;

	g_pNewUISystem->UpdateSendMoveInterface();

	if (g_bEventChipDialogEnable)
	{
		SendRequestEventChipExit();

		if (g_bEventChipDialogEnable == EVENT_SCRATCH_TICKET)
		{
			ClearInput(FALSE);
			InputEnable = false;
			GoldInputEnable = false;
			InputGold = 0;
			StorageGoldFlag = 0;
			g_bScratchTicket = false;
		}
		g_bEventChipDialogEnable = EVENT_NONE;

		if (!gwinhandle->CheckWndMode())
		{
			int x = 260 * MouseX / GetWindowsX;
			SetCursorPos((x)*g_fScreenRate_x, (MouseY)*g_fScreenRate_y);
		}

		MouseUpdateTimeMax = 6;
		MouseLButton = false;
	}
}

int StandTime = 0;
int HeroAngle = 0;

bool CheckMacroLimit(char* Text)
{
	int  length;
	char string[256];

	memcpy(string, Text + 3, sizeof(char) * (256 - 2));
	string[255] = 0;

	length = strlen(GlobalText[258]);
	if (strcmp(string, GlobalText[258]) == 0 || strcmp(string, GlobalText[259]) == 0 || stricmp(string, "/trade") == 0)
	{
		return  true;
	}
	if (strcmp(string, GlobalText[256]) == 0 || stricmp(string, "/party") == 0 || stricmp(string, "/pt") == 0)
	{
		return  true;
	}
	if (strcmp(string, GlobalText[254]) == 0 || stricmp(string, "/guild") == 0)
	{
		return  true;
	}
	if (strcmp(string, GlobalText[248]) == 0 || stricmp(string, "/GuildWar") == 0)
	{
		return  true;
	}
	if (strcmp(string, GlobalText[249]) == 0 || stricmp(string, "/BattleSoccer") == 0)
	{
		return  true;
	}

	return  false;
}

bool CheckCommand(char* Text, bool bMacroText)
{
	if (g_ConsoleDebug->CheckCommand(Text) == true)
	{
		return true;
	}

	if (bMacroText == false && LogOut == false)
	{
		char Name[256];
		int iTextSize = 0;
		for (int i = 0; i < 256 && Text[i] != ' ' && Text[i] != '\0'; i++)
		{
			Name[i] = Text[i];
			iTextSize = i;
		}
		Name[iTextSize] = NULL;

		if (!g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE))
		{
			if (strcmp(Name, GlobalText[258]) == NULL || strcmp(Name, GlobalText[259]) == NULL || stricmp(Name, "/trade") == NULL)
			{
				if (gMapManager->InChaosCastle() == true)
				{
					g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);

					return false;
				}

				if (::IsStrifeMap(gMapManager->currentMap))
				{
					g_pChatListBox->AddText("", GlobalText[3147], SEASON3B::TYPE_SYSTEM_MESSAGE);
					return false;
				}

				int level = CharacterAttribute->Level;

				if (level < TRADELIMITLEVEL)
				{
					g_pChatListBox->AddText("", GlobalText[478], SEASON3B::TYPE_SYSTEM_MESSAGE);
					return true;
				}
				if (SelectedCharacter != -1)
				{
					CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
					if (c)
					{
						OBJECT* o = &c->Object;
						if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
							abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
							abs((c->PositionY) - (Hero->PositionY)) <= 1)
						{
							if (IsShopInViewport(c))
							{
								g_pChatListBox->AddText("", GlobalText[493], SEASON3B::TYPE_ERROR_MESSAGE);
								return true;
							}
							SendRequestTrade(c->Key);
						}
					}
				}
				else for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
				{
					CHARACTER* c = gmCharacters->GetCharacter(i);
					OBJECT* o = &c->Object;

					if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
						abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
						abs((c->PositionY) - (Hero->PositionY)) <= 1)
					{
						if (IsShopInViewport(c))
						{
							g_pChatListBox->AddText("", GlobalText[493], SEASON3B::TYPE_SYSTEM_MESSAGE);
							return true;
						}

						BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						if (abs(Dir1 - Dir2) == 4)
						{
							SendRequestTrade(c->Key);
							break;
						}
					}
				}
				return true;
			}
		}

		if (strcmp(Text, GlobalText[688]) == 0)
		{
			return false;
		}

		if (strcmp(Text, GlobalText[1117]) == 0 || stricmp(Text, "/personalshop") == 0)
		{
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}

			int level = CharacterAttribute->Level;
			if (level >= 6)
			{
				g_pNewUISystem->Show(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
			}
			else
			{
				char szError[48] = "";
				sprintf(szError, GlobalText[1123], 6);
				g_pChatListBox->AddText("", szError, SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
			return true;
		}
		if (strstr(Text, GlobalText[1118]) != NULL || strstr(Text, "/purchase") != NULL)
		{
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}

			if (::IsStrifeMap(gMapManager->currentMap))
			{
				g_pChatListBox->AddText("", GlobalText[3147], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}

			if (
				g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND)
				)
			{
				g_pChatListBox->AddText("", GlobalText[1121], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}

			char szCmd[24]; char szId[MAX_ID_SIZE];

			if (sscanf(Text, "%s %s", szCmd, szId) != 0)
			{
				if (SelectedCharacter != -1)
				{
					CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
					OBJECT* o = &c->Object;
					if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change))
						SendRequestOpenPersonalShop(c->Key, c->ID);
				}
				else
				{
					for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
					{
						CHARACTER* c = gmCharacters->GetCharacter(i);
						if (strlen(szId) > 0 && c->Object.Live)
						{
							if (strcmp(c->ID, szId) == 0)
							{
								SendRequestOpenPersonalShop(c->Key, c->ID);
							}
						}
						else
						{
							OBJECT* o = &c->Object;
							if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change))
							{
								BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
								BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
								if (abs(Dir1 - Dir2) == 4)
								{
									SendRequestOpenPersonalShop(c->Key, c->ID);
									break;
								}
							}
						}
					}
				}
			}

			return true;
		}

		if (strcmp(Text, GlobalText[1136]) == 0)
		{
			ShowShopTitles();
			g_pChatListBox->AddText("", GlobalText[1138], SEASON3B::TYPE_SYSTEM_MESSAGE);
		}

		if (strcmp(Text, GlobalText[1137]) == 0)
		{
			HideShopTitles();
			g_pChatListBox->AddText("", GlobalText[1139], SEASON3B::TYPE_ERROR_MESSAGE);
		}
		if (strcmp(Text, GlobalText[908]) == 0 || stricmp(Text, "/duelstart") == 0)
		{
#ifndef GUILD_WAR_EVENT
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}
#endif// UILD_WAR_EVENT
			if (!g_DuelMgr.IsDuelEnabled())
			{
				int iLevel = CharacterAttribute->Level;
				if (iLevel < 30)
				{
					char szError[48] = "";
					sprintf(szError, GlobalText[2704], 30);
					g_pChatListBox->AddText("", szError, SEASON3B::TYPE_ERROR_MESSAGE);
					return 3;
				}
				else
					if (SelectedCharacter != -1)
					{
						CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
						OBJECT* o = &c->Object;
						if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
							abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
							abs((c->PositionY) - (Hero->PositionY)) <= 1) {

							SendRequestDuelStart(c->Key, c->ID);
						}
					}
					else for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
					{
						CHARACTER* c = gmCharacters->GetCharacter(i);
						OBJECT* o = &c->Object;

						if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
							abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
							abs((c->PositionY) - (Hero->PositionY)) <= 1)
						{
							BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
							BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
							if (abs(Dir1 - Dir2) == 4)
							{
								SendRequestDuelStart(c->Key, c->ID);
								break;
							}
						}
					}
			}
			else
			{
				g_pChatListBox->AddText("", GlobalText[915], SEASON3B::TYPE_SYSTEM_MESSAGE);
			}
		}
		if (strcmp(Text, GlobalText[909]) == 0 || stricmp(Text, "/duelend") == 0)
		{
#ifndef GUILD_WAR_EVENT
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}
#endif// GUILD_WAR_EVENT
			if (g_DuelMgr.IsDuelEnabled())
			{
				SendRequestDuelEnd();
			}
		}
		if (strcmp(Text, GlobalText[254]) == NULL || stricmp(Text, "/guild") == NULL)
		{
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}
			if (Hero->GuildStatus != G_NONE)
			{
				g_pChatListBox->AddText("", GlobalText[255], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return true;
			}

			if (SelectedCharacter != -1)
			{
				CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
				OBJECT* o = &c->Object;
				if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
					abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
					abs((c->PositionY) - (Hero->PositionY)) <= 1)
					SendRequestGuild(c->Key);
			}
			else for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* c = gmCharacters->GetCharacter(i);
				OBJECT* o = &c->Object;

				if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
					abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
					abs((c->PositionY) - (Hero->PositionY)) <= 1)
				{
					BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
					BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
					if (abs(Dir1 - Dir2) == 4)
					{
						SendRequestGuild(c->Key);
						break;
					}
				}
			}
			return true;
		}
		if (!strcmp(Text, GlobalText[1354]) || !stricmp(Text, "/union") ||
			!strcmp(Text, GlobalText[1356]) || !stricmp(Text, "/rival") ||
			!strcmp(Text, GlobalText[1357]) || !stricmp(Text, "/rivaloff"))
		{
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}
			if (Hero->GuildStatus == G_NONE)
			{
				g_pChatListBox->AddText("", GlobalText[1355], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return true;
			}

			if (SelectedCharacter != -1)
			{
				CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
				if (c)
				{
					OBJECT* o = &c->Object;
					if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
						abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
						abs((c->PositionY) - (Hero->PositionY)) <= 1)
					{
						if (!strcmp(Text, GlobalText[1354]) || !stricmp(Text, "/union"))
						{
							SendRequestGuildRelationShip(0x01, 0x01, HIBYTE(c->Key), LOBYTE(c->Key));
						}
						else if (!strcmp(Text, GlobalText[1356]) || !stricmp(Text, "/rival"))
						{
							SendRequestGuildRelationShip(0x02, 0x01, HIBYTE(c->Key), LOBYTE(c->Key));
						}
						else
						{
							SetAction(&Hero->Object, PLAYER_RESPECT1);
							SendRequestAction(AT_RESPECT1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
							SendRequestGuildRelationShip(0x02, 0x02, HIBYTE(c->Key), LOBYTE(c->Key));
						}
					}
				}
			}
			else for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* c = gmCharacters->GetCharacter(i);
				if (c)
				{
					OBJECT* o = &c->Object;

					if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) &&
						abs((c->PositionX) - (Hero->PositionX)) <= 1 &&
						abs((c->PositionY) - (Hero->PositionY)) <= 1)
					{
						BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						if (abs(Dir1 - Dir2) == 4) {
							if (!strcmp(Text, GlobalText[1354]) || !stricmp(Text, "/union"))
							{
								SendRequestGuildRelationShip(0x01, 0x01, HIBYTE(c->Key), LOBYTE(c->Key));
							}
							else if (!strcmp(Text, GlobalText[1356]) || !stricmp(Text, "/rival"))
							{
								SendRequestGuildRelationShip(0x02, 0x01, HIBYTE(c->Key), LOBYTE(c->Key));
							}
							else
							{
								SetAction(&Hero->Object, PLAYER_RESPECT1);
								SendRequestAction(AT_RESPECT1, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
								SendRequestGuildRelationShip(0x02, 0x02, HIBYTE(c->Key), LOBYTE(c->Key));
							}
							break;
						}
					}
				}
			}
			return true;
		}
		if (strcmp(Text, GlobalText[256]) == NULL || stricmp(Text, "/party") == NULL || stricmp(Text, "/pt") == NULL)
		{
			if (gMapManager->InChaosCastle() == true)
			{
				g_pChatListBox->AddText("", GlobalText[1150], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return false;
			}
			if (PartyNumber > 0 && strcmp(Party[0].Name, Hero->ID) != NULL)
			{
				g_pChatListBox->AddText("", GlobalText[257], SEASON3B::TYPE_SYSTEM_MESSAGE);
				return true;
			}

			if (SelectedCharacter != -1)
			{
				CHARACTER* c = gmCharacters->GetCharacter(SelectedCharacter);
				if (c)
				{
					OBJECT* o = &c->Object;
					if (o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) && abs((c->PositionX) - (Hero->PositionX)) <= 1 && abs((c->PositionY) - (Hero->PositionY)) <= 1)
						SendRequestParty(c->Key);
				}
			}
			else for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* c = gmCharacters->GetCharacter(i);
				if (c)
				{
					OBJECT* o = &c->Object;
					if (o->Live && o->Kind == KIND_PLAYER && c != Hero && (o->Type == MODEL_PLAYER || c->Change) && abs((c->PositionX) - (Hero->PositionX)) <= 1 && abs((c->PositionY) - (Hero->PositionY)) <= 1)
					{
						BYTE Dir1 = (BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						BYTE Dir2 = (BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8;
						if (abs(Dir1 - Dir2) == 4)
						{
							SendRequestParty(c->Key);
							break;
						}
					}
				}
			}
			return true;
		}
		if (stricmp(Text, "/charactername") == NULL)
		{
			if (IsGMCharacter() == true)
			{
				g_bGMObservation = !g_bGMObservation;
				return true;
			}
		}

		for (int i = 0; i < 10; i++)
		{
			char Name[256];
			if (i != 9)
				sprintf(Name, "/%d", i + 1);
			else
				sprintf(Name, "/%d", 0);
			if (Text[0] == Name[0] && Text[1] == Name[1])
			{
				if (CheckMacroLimit(Text) == true)
				{
					return  false;
				}

				int iTextSize = 0;
				for (int j = 3; j < (int)strlen(Text); j++)
				{
					MacroText[i][j - 3] = Text[j];
					iTextSize = j;
				}
				if (iTextSize >= 3)
					MacroText[i][iTextSize - 3] = NULL;
				PlayBuffer(SOUND_CLICK01);
				return true;
			}
		}

		char lpszFilter[] = "/filter";
		if ((strlen(GlobalText[753]) > 0 && strncmp(Text, GlobalText[753], strlen(GlobalText[753])) == NULL)
			|| (strncmp(Text, lpszFilter, strlen(lpszFilter)) == NULL))
		{
			g_pChatListBox->SetFilterText(Text);
		}
	}
#ifdef CSK_FIX_MACRO_MOVEMAP
	else if (bMacroText == true)
	{
		char Name[256];

		int iTextSize = 0;
		for (int i = 0; i < 256 && Text[i] != ' ' && Text[i] != '\0'; i++)
		{
			Name[i] = Text[i];
			iTextSize = i;
		}
		Name[iTextSize] = NULL;

		if (strcmp(Name, GlobalText[260]) == 0 || stricmp(Name, "/move") == 0)
		{
			if (IsGMCharacter() == true || FindText2(Hero->ID, "webzen") == true)
			{
				return false;
			}
			return true;
		}
	}
#endif // CSK_FIX_MACRO_MOVEMAP

	if (IsIllegalMovementByUsingMsg(Text))
		return TRUE;

	for (int i = 0; i < 16 * MAX_ITEM_INDEX; ++i)
	{
		Script_Item* p = GMItemMng->find(i);

		if (p->Width != 0)
		{
			char Name[256];
			sprintf(Name, "/%s", p->Name);

			if (stricmp(Text, Name) == NULL)
			{
				g_csItemOption.ClearOptionHelper();

				g_pNewUISystem->Show(SEASON3B::INTERFACE_ITEM_EXPLANATION);

				ItemHelp = i;
				PlayBuffer(SOUND_CLICK01);
				return true;
			}
		}
	}

	g_csItemOption.CheckRenderOptionHelper(Text);

	return (IsIllegalMovementByUsingMsg(Text)) ? TRUE : FALSE;
	}

bool FindText(const char* Text, const char* Token, bool First)
{
	int LengthToken = (int)strlen(Token);
	int Length = (int)strlen(Text) - LengthToken;
	if (First)
		Length = 0;
	if (Length < 0)
		return false;

	unsigned char* lpszCheck = (unsigned char*)Text;
	for (int i = 0; i <= Length; i += _mbclen(lpszCheck + i))
	{
		bool Success = true;
		for (int j = 0; j < LengthToken; j++)
		{
			if (Text[i + j] != Token[j])
			{
				Success = false;
				break;
			}
		}
		if (Success)
			return true;
	}
	return false;
}


bool FindTextABS(const char* Text, const char* Token, bool First)
{
	int LengthToken = (int)strlen(Token);
	int Length = (int)strlen(Text) - LengthToken;
	if (First)
		Length = 0;
	if (Length < 0)
		return false;

	unsigned char* lpszCheck = (unsigned char*)Text;
	for (int i = 0; i <= Length; i += _mbclen(lpszCheck + i))
	{
		bool Success = true;
		for (int j = 0; j < LengthToken; j++)
		{
			if (Text[i + j] != Token[j])
			{
				Success = false;
				break;
			}
		}
		if (Success)
			return true;
	}
	return false;
}

void SetActionClass(CHARACTER* c, OBJECT* o, int Action, int ActionType)
{
	if ((o->CurrentAction >= PLAYER_STOP_MALE && o->CurrentAction <= PLAYER_STOP_RIDE_WEAPON)
		|| o->CurrentAction == PLAYER_STOP_TWO_HAND_SWORD_TWO
		|| o->CurrentAction == PLAYER_RAGE_UNI_STOP_ONE_RIGHT
		|| o->CurrentAction == PLAYER_STOP_RAGEFIGHTER)
	{
		if (!gCharacterManager.IsFemale(c->Class) || (Action >= PLAYER_RESPECT1 && Action <= PLAYER_RUSH1))
			SetAction(o, Action);
		else
			SetAction(o, Action + 1);
		SendRequestAction(ActionType, ((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
}

void CheckChatText(char* Text)
{
	CHARACTER* c = Hero;
	OBJECT* o = &c->Object;
	if (FindText(Text, GlobalText[270]) || FindText(Text, GlobalText[271]) || FindText(Text, GlobalText[272]) || FindText(Text, GlobalText[273]) || FindText(Text, GlobalText[274]) || FindText(Text, GlobalText[275]) || FindText(Text, GlobalText[276]) || FindText(Text, GlobalText[277]))
	{
		SetActionClass(c, o, PLAYER_GREETING1, AT_GREETING1);
		SendRequestAction(AT_GREETING1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[278]) || FindText(Text, GlobalText[279]) || FindText(Text, GlobalText[280]))
	{
		SetActionClass(c, o, PLAYER_GOODBYE1, AT_GOODBYE1);
		SendRequestAction(AT_GOODBYE1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[281]) || FindText(Text, GlobalText[282]) || FindText(Text, GlobalText[283]) || FindText(Text, GlobalText[284]) || FindText(Text, GlobalText[285]) || FindText(Text, GlobalText[286]))
	{
		SetActionClass(c, o, PLAYER_CLAP1, AT_CLAP1);
		SendRequestAction(AT_CLAP1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[287]) || FindText(Text, GlobalText[288]) || FindText(Text, GlobalText[289]) || FindText(Text, GlobalText[290]))
	{
		SetActionClass(c, o, PLAYER_GESTURE1, AT_GESTURE1);
		SendRequestAction(AT_GESTURE1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[292]) || FindText(Text, GlobalText[293]) || FindText(Text, GlobalText[294]) || FindText(Text, GlobalText[295]))
	{
		SetActionClass(c, o, PLAYER_DIRECTION1, AT_DIRECTION1);
		SendRequestAction(AT_DIRECTION1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[296]) || FindText(Text, GlobalText[297]) || FindText(Text, GlobalText[298]) || FindText(Text, GlobalText[299]) || FindText(Text, GlobalText[300]) || FindText(Text, GlobalText[301]) || FindText(Text, GlobalText[302]))
	{
		SetActionClass(c, o, PLAYER_UNKNOWN1, AT_UNKNOWN1);
		SendRequestAction(AT_UNKNOWN1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, ";") || FindText(Text, GlobalText[303]) || FindText(Text, GlobalText[304]) || FindText(Text, GlobalText[305]))
	{
		SetActionClass(c, o, PLAYER_AWKWARD1, AT_AWKWARD1);
		SendRequestAction(AT_AWKWARD1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, "��.��") || FindText(Text, "��.��") || FindText(Text, "T_T") || FindText(Text, GlobalText[306]) || FindText(Text, GlobalText[307]) || FindText(Text, GlobalText[308]) || FindText(Text, GlobalText[309]))
	{
		SetActionClass(c, o, PLAYER_CRY1, AT_CRY1);
		SendRequestAction(AT_CRY1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, "��.��") || FindText(Text, "��.,��") || FindText(Text, "��,.��") || FindText(Text, "-.-") || FindText(Text, "-_-") || FindText(Text, GlobalText[310]) || FindText(Text, GlobalText[311]))
	{
		SetActionClass(c, o, PLAYER_SEE1, AT_SEE1);
		SendRequestAction(AT_SEE1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, "^^") || FindText(Text, "^.^") || FindText(Text, "^_^") || FindText(Text, GlobalText[312]) || FindText(Text, GlobalText[313]) || FindText(Text, GlobalText[314]) || FindText(Text, GlobalText[315]) || FindText(Text, GlobalText[316]))
	{
		SetActionClass(c, o, PLAYER_SMILE1, AT_SMILE1);
		SendRequestAction(AT_SMILE1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[318]) || FindText(Text, GlobalText[319]) || FindText(Text, GlobalText[320]) || FindText(Text, GlobalText[321]))
	{
		SetActionClass(c, o, PLAYER_CHEER1, AT_CHEER1);
		SendRequestAction(AT_CHEER1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[322]) || FindText(Text, GlobalText[323]) || FindText(Text, GlobalText[324]) || FindText(Text, GlobalText[325]))
	{
		SetActionClass(c, o, PLAYER_WIN1, AT_WIN1);
		SendRequestAction(AT_WIN1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[326]) || FindText(Text, GlobalText[327]) || FindText(Text, GlobalText[328]) || FindText(Text, GlobalText[329]))
	{
		SetActionClass(c, o, PLAYER_SLEEP1, AT_SLEEP1);
		SendRequestAction(AT_SLEEP1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[330]) || FindText(Text, GlobalText[331]) || FindText(Text, GlobalText[332]) || FindText(Text, GlobalText[333]) || FindText(Text, GlobalText[334]))
	{
		SetActionClass(c, o, PLAYER_COLD1, AT_COLD1);
		SendRequestAction(AT_COLD1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[335]) || FindText(Text, GlobalText[336]) || FindText(Text, GlobalText[337]) || FindText(Text, GlobalText[338]))
	{
		SetActionClass(c, o, PLAYER_AGAIN1, AT_AGAIN1);
		SendRequestAction(AT_AGAIN1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[339]) || FindText(Text, GlobalText[340]) || FindText(Text, GlobalText[341]))
	{
		SetActionClass(c, o, PLAYER_RESPECT1, AT_RESPECT1);
		SendRequestAction(AT_RESPECT1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[342]) || FindText(Text, GlobalText[343]) || FindText(Text, "/��") || FindText(Text, "��^"))
	{
		SetActionClass(c, o, PLAYER_SALUTE1, AT_SALUTE1);
		SendRequestAction(AT_SALUTE1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[344]) || FindText(Text, GlobalText[345]) || FindText(Text, GlobalText[346]) || FindText(Text, GlobalText[347]))
	{
		SetActionClass(c, o, PLAYER_RUSH1, AT_RUSH1);
		SendRequestAction(AT_RUSH1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[783]) || FindText(Text, "hustle"))
	{
		SetActionClass(c, o, PLAYER_HUSTLE, AT_HUSTLE);
		SendRequestAction(AT_HUSTLE, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[291]))
	{
		SetActionClass(c, o, PLAYER_PROVOCATION, AT_PROVOCATION);
		SendRequestAction(AT_PROVOCATION, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[317]))
	{
		SetActionClass(c, o, PLAYER_CHEERS, AT_CHEERS);
		SendRequestAction(AT_CHEERS, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[348]))
	{
		SetActionClass(c, o, PLAYER_LOOK_AROUND, AT_LOOK_AROUND);
		SendRequestAction(AT_LOOK_AROUND, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[2041]))
	{
		SetActionClass(c, o, PLAYER_KOREA_HANDCLAP, AT_JACK1);
		SendRequestAction(AT_JACK1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[2042]))
	{
		SetActionClass(c, o, PLAYER_POINT_DANCE, AT_JACK2);
		SendRequestAction(AT_JACK2, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
	}
	else if (FindText(Text, GlobalText[2228]))
	{
		ITEM* pItem_rr = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];
		ITEM* pItem_rl = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];

		if (pItem_rr->Type == ITEM_HELPER + 40 || pItem_rl->Type == ITEM_HELPER + 40)
		{
			if (rand() % 2 == 0)
			{
				SetAction(o, PLAYER_JACK_1);
				SendRequestAction(AT_JACK1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
			}
			else
			{
				SetAction(o, PLAYER_JACK_2);
				SendRequestAction(AT_JACK2, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
			}

			o->m_iAnimation = 0;
		}

	}
	else if (FindText(Text, GlobalText[2243]))
	{
		ITEM* pItem_rr = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];
		ITEM* pItem_rl = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];

		if (pItem_rr->Type == ITEM_HELPER + 41 || pItem_rl->Type == ITEM_HELPER + 41)
		{
			if (o->CurrentAction != PLAYER_SANTA_1 && o->CurrentAction != PLAYER_SANTA_2)
			{
				int i = rand() % 3;
				if (rand() % 2)
				{
					SetAction(o, PLAYER_SANTA_1);
					SendRequestAction(AT_SANTA1_1 + i, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
					PlayBuffer(SOUND_XMAS_JUMP_SANTA + i);
				}
				else
				{
					SetAction(o, PLAYER_SANTA_2);
					SendRequestAction(AT_SANTA2_1 + i, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
					PlayBuffer(SOUND_XMAS_TURN);
				}

				g_XmasEvent->CreateXmasEventEffect(c, o, i);
			}

			o->m_iAnimation = 0;
		}
	}
}

bool CheckTarget(CHARACTER* c)
{
	CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

	if (pCharacter)
	{
		TargetX = (int)(pCharacter->Object.Position[0] / TERRAIN_SCALE);
		TargetY = (int)(pCharacter->Object.Position[1] / TERRAIN_SCALE);
		VectorCopy(pCharacter->Object.Position, c->TargetPosition);
		return true;
	}
	else
	{
		RenderTerrain(true);
		if (RenderTerrainTile(SelectXF, SelectYF, (int)SelectXF, (int)SelectYF, 1.f, 1, true))
		{
			VectorCopy(CollisionPosition, c->TargetPosition);
			TargetX = (int)(c->TargetPosition[0] / TERRAIN_SCALE);
			TargetY = (int)(c->TargetPosition[1] / TERRAIN_SCALE);
			return true;
		}
	}
	return false;
}


BYTE MakeSkillSerialNumber(BYTE* pSerialNumber)
{
	if (pSerialNumber == NULL) return 0;

	++g_byLastSkillSerialNumber;
	if (g_byLastSkillSerialNumber > 50) g_byLastSkillSerialNumber = 1;

	*pSerialNumber = g_byLastSkillSerialNumber;
	return g_byLastSkillSerialNumber;
}

void AttackElf(CHARACTER* c, int Skill, float Distance)
{
	OBJECT* o = &c->Object;
	int ClassIndex = gCharacterManager.GetBaseClass(c->Class);

	int iMana, iSkillMana;
	gSkillManager.GetSkillInformation(Skill, 1, NULL, &iMana, NULL, &iSkillMana);
	if (g_isCharacterBuff(o, eBuff_InfinityArrow))
		iMana += CharacterMachine->InfinityArrowAdditionalMana;
	if ((unsigned int)iMana > CharacterAttribute->Mana)
	{
		int Index = g_pMyInventory->FindManaItemIndex();

		if (Index != -1)
		{
			SendRequestUse(Index, 0);
		}
		return;
	}

	if ((unsigned int)iSkillMana > CharacterAttribute->SkillMana)
	{
		return;
	}
	if (!gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
	{
		return;
	}

	int iEnergy;
	gSkillManager.GetSkillInformation_Energy(Skill, &iEnergy);

	if ((unsigned int)iEnergy > (CharacterAttribute->Energy + CharacterAttribute->AddEnergy))
	{
		return;
	}

	bool Success = CheckTarget(c);


	switch (Skill)
	{
	case AT_SKILL_SUMMON:
	case AT_SKILL_SUMMON + 1:
	case AT_SKILL_SUMMON + 2:
	case AT_SKILL_SUMMON + 3:
	case AT_SKILL_SUMMON + 4:
	case AT_SKILL_SUMMON + 5:
	case AT_SKILL_SUMMON + 6:
#ifdef ADD_ELF_SUMMON
	case AT_SKILL_SUMMON + 7:
#endif // ADD_ELF_SUMMON
	case Skill_Summon_Satyros:
		if (gMapManager->currentMap != WD_10HEAVEN && gMapManager->InChaosCastle() == false)
			if (!g_Direction.m_CKanturu.IsMayaScene())
			{
				SendRequestMagic(Skill, HeroKey);
				SetPlayerMagic(c);
			}
		return;

	case AT_SKILL_PIERCING:
	case Skill_Penetration_Strengthener:
		if ((o->Type == MODEL_PLAYER) && (gCharacterManager.GetEquipedBowType(Hero) != BOWTYPE_NONE))
		{
			if (!CheckArrow())
				break;
			if (CheckTile(c, o, Distance))
			{
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

				WORD TKey = 0xffff;
				if (g_MovementSkill.m_iTarget != -1)
				{
					TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				}
				SendRequestMagicContinue(Skill, (c->PositionX),
					(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, TKey, 0);
				SetPlayerAttack(c);
				if (o->Type != MODEL_PLAYER)
				{
					CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 0, (c->Skill));
				}
			}
		}
		break;
	}

	CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

	if (pCharacter)
	{
		ZeroMemory(&g_MovementSkill, sizeof(g_MovementSkill));
		g_MovementSkill.m_bMagic = TRUE;
		g_MovementSkill.m_iSkill = Hero->CurrentSkill;
		g_MovementSkill.m_iTarget = SelectedCharacter;
		g_MovementSkill.m_iKey = pCharacter->Key;
	}
	if (!CheckTile(c, o, Distance))
	{
		if (SelectedCharacter != -1 && (Skill == Skill_Heal
			|| Skill == Skill_Greater_Damage
			|| Skill == Skill_Greater_Defense
			|| CheckAttack()
			|| Skill == Skill_Summoned_Monster_Power_Up1
			|| Skill == Skill_Summoned_Monster_Power_Up2
			|| Skill == Skill_Summoned_Monster_Power_Up3
			|| Skill == Skill_Cure
			|| Skill == Skill_Party_Healing
			|| Skill == Skill_Party_Healing_Strengthener
			|| Skill == Skill_Heal_Strengthener
			|| Skill == Skill_Fire_Scream
			|| Skill == Skill_Fire_Scream_Strengthener
			|| Skill == Skill_Fire_Scream_Mastery
			|| Skill == Skill_Bless
			|| Skill == Skill_Bless_Strengthener))
		{
			if (pCharacter && pCharacter->Object.Kind == KIND_PLAYER)
			{
				if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
				{
					c->Movement = true;
					c->MovementType = MOVEMENT_SKILL;
					g_pNewUIMiniMap->StopMove();
					SendMove(c, o);
				}
			}
		}

		if (Skill != AT_SKILL_STUN && Skill != AT_SKILL_REMOVAL_STUN && Skill != AT_SKILL_MANA && Skill != AT_SKILL_INVISIBLE && Skill != AT_SKILL_REMOVAL_INVISIBLE
			&& Skill != AT_SKILL_PLASMA_STORM_FENRIR
			)
			return;
	}
	bool Wall = CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY);

	if (Wall)
	{
		if (SelectedCharacter != -1 && pCharacter)
		{
			if (pCharacter->Object.Kind == KIND_PLAYER)
			{
				switch (Skill)
				{
				case Skill_Heal:
				case Skill_Greater_Damage:
				case Skill_Greater_Defense:
				case Skill_Summoned_Monster_Power_Up1:
				case Skill_Summoned_Monster_Power_Up2:
				case Skill_Summoned_Monster_Power_Up3:
				case Skill_Cure:
				case Skill_Party_Healing:
				case Skill_Party_Healing_Strengthener:
				case Skill_Heal_Strengthener:
				case Skill_Defense_Increase_Strengthener:
				case Skill_Attack_Increase_Strengthener:
				case Skill_Defense_Increase_Mastery:
				case Skill_Attack_Increase_Mastery:
				case Skill_Bless:
				case Skill_Bless_Strengthener:
					UseSkillElf(c, o);
					return;
				}
			}
			if (CheckAttack())
			{
				if ((Skill == Skill_Ice_Arrow || Skill == Skill_Starfall
					|| Skill == Skill_Poison_Arrow
					|| Skill == Skill_Ice_Arrow_Strengthener
					|| Skill == Skill_Poison_Arrow_Strengthener) && ((o->Type == MODEL_PLAYER) && (gCharacterManager.GetEquipedBowType(Hero) != BOWTYPE_NONE)))
				{
					UseSkillElf(c, o);
				}
			}
		}
	}

	switch (Skill)
	{
	case AT_SKILL_INFINITY_ARROW:
	case Skill_Infinity_Arrow_Strengthener:
	{
		if (g_isCharacterBuff((&Hero->Object), eBuff_InfinityArrow) == false)
		{
			SendRequestMagic(Skill, HeroKey);
			if ((c->Helper.Type == MODEL_HELPER + 37)
				|| (c->Helper.Type == MODEL_HELPER + 2)
				|| (c->Helper.Type == MODEL_HELPER + 3)
				|| (c->Helper.Type == MODEL_HELPER + 4))
				SetPlayerMagic(c);
			else
				SetAction(o, PLAYER_RUSH1);

			c->Movement = 0;
		}
	}
	break;

	case AT_SKILL_HELLOWIN_EVENT_1:
	case AT_SKILL_HELLOWIN_EVENT_2:
	case AT_SKILL_HELLOWIN_EVENT_3:
	case AT_SKILL_HELLOWIN_EVENT_4:
	case AT_SKILL_HELLOWIN_EVENT_5:
		SendRequestMagic(Skill, HeroKey);
		c->Movement = 0;
		break;
	case AT_SKILL_RECOVER:
	{
		vec3_t Light, Position, P, dp;

		float Matrix[3][4];

		Vector(0.f, -220.f, 130.f, P);
		AngleMatrix(o->Angle, Matrix);
		VectorRotate(P, Matrix, dp);
		VectorAdd(dp, o->Position, Position);
		Vector(0.7f, 0.6f, 0.f, Light);
		CreateEffect(BITMAP_IMPACT, Position, o->Angle, Light, 0, o);
		SetAction(o, PLAYER_RECOVER_SKILL);

		if (pCharacter && pCharacter->Object.Kind == KIND_PLAYER)
		{
			SendRequestMagic(Skill, pCharacter->Key);
		}
		else
		{
			SendRequestMagic(Skill, HeroKey);
		}
	}
	break;
	case AT_SKILL_HEALING:
	case AT_SKILL_ATTACK:
	case AT_SKILL_DEFENSE:
	case Skill_Summoned_Monster_Power_Up1:
	case Skill_Summoned_Monster_Power_Up2:
	case Skill_Summoned_Monster_Power_Up3:
	case Skill_Cure:
	case Skill_Party_Healing:
	case Skill_Party_Healing_Strengthener:
	case Skill_Bless:
	case Skill_Bless_Strengthener:
	case Skill_Heal_Strengthener:
	case Skill_Defense_Increase_Strengthener:
	case Skill_Attack_Increase_Strengthener:
	case Skill_Defense_Increase_Mastery:
	case Skill_Attack_Increase_Mastery:
		SendRequestMagic(Skill, HeroKey);
		SetPlayerMagic(c);
		return;
	case AT_SKILL_MULTI_SHOT:
	case Skill_Multi_Shot_Strengthener:
	case Skill_Multi_Shot_Mastery:
	{
		if (!CheckArrow())
			break;

		if (gCharacterManager.GetEquipedBowType_Skill() == BOWTYPE_NONE)
			return;
		o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

		if (CheckTile(c, o, Distance))
		{
			BYTE PathX[1];
			BYTE PathY[1];
			PathX[0] = (c->PositionX);
			PathY[0] = (c->PositionY);

			SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

			BYTE byValue = GetDestValue((c->PositionX), (c->PositionY), TargetX, TargetY);

			BYTE angle = (BYTE)((((o->Angle[2] + 180.f) / 360.f) * 255.f));
			WORD TKey = 0xffff;
			if (g_MovementSkill.m_iTarget != -1)
			{
				TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
			}

			SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);
			SetAttackSpeed();

			c->Movement = 0;

			SetPlayerBow(c);
			vec3_t Light, Position, P, dp;

			float Matrix[3][4];
			Vector(0.f, 20.f, 0.f, P);
			AngleMatrix(o->Angle, Matrix);
			VectorRotate(P, Matrix, dp);
			VectorAdd(dp, o->Position, Position);
			Vector(0.8f, 0.9f, 1.6f, Light);
			CreateEffect(MODEL_MULTI_SHOT3, Position, o->Angle, Light, 0);
			CreateEffect(MODEL_MULTI_SHOT3, Position, o->Angle, Light, 0);

			Vector(0.f, -20.f, 0.f, P);
			Vector(0.f, 0.f, 0.f, P);
			AngleMatrix(o->Angle, Matrix);
			VectorRotate(P, Matrix, dp);
			VectorAdd(dp, o->Position, Position);

			CreateEffect(MODEL_MULTI_SHOT1, Position, o->Angle, Light, 0);
			CreateEffect(MODEL_MULTI_SHOT1, Position, o->Angle, Light, 0);
			CreateEffect(MODEL_MULTI_SHOT1, Position, o->Angle, Light, 0);

			Vector(0.f, 20.f, 0.f, P);
			AngleMatrix(o->Angle, Matrix);
			VectorRotate(P, Matrix, dp);
			VectorAdd(dp, o->Position, Position);
			CreateEffect(MODEL_MULTI_SHOT2, Position, o->Angle, Light, 0);
			CreateEffect(MODEL_MULTI_SHOT2, Position, o->Angle, Light, 0);

			Vector(0.f, -120.f, 145.f, P);
			AngleMatrix(o->Angle, Matrix);
			VectorRotate(P, Matrix, dp);
			VectorAdd(dp, o->Position, Position);

			short Key = -1;
			for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			{
				CHARACTER* tc = gmCharacters->GetCharacter(i);
				if (tc == c)
				{
					Key = i;
					break;
				}
			}

			CreateEffect(MODEL_BLADE_SKILL, Position, o->Angle, Light, 1, o, Key);
			PlayBuffer(SOUND_SKILL_MULTI_SHOT);
		}
	}
	break;
	case AT_SKILL_IMPROVE_AG:
		SendRequestMagic(Skill, HeroKey);
		SetPlayerMagic(c);
		c->Movement = 0;
		break;
	case AT_SKILL_PLASMA_STORM_FENRIR:
	{
		if (CheckAttack())
		{
			g_MovementSkill.m_iTarget = SelectedCharacter;

			CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);
			g_MovementSkill.m_iKey = (tc) ? tc->Key : 0;
		}
		else
		{
			g_MovementSkill.m_iTarget = -1;
			g_MovementSkill.m_iKey = 0;
		}

		int TargetX = (int)(c->TargetPosition[0] / TERRAIN_SCALE);
		int TargetY = (int)(c->TargetPosition[1] / TERRAIN_SCALE);
		if (CheckTile(c, o, Distance))
		{
			BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
			WORD TKey = 0xffff;
			if (g_MovementSkill.m_iTarget != -1)
			{
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

				gSkillManager.CheckSkillDelay(Hero->CurrentSkill);

				TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				c->m_iFenrirSkillTarget = g_MovementSkill.m_iTarget;
				SendRequestMagicContinue(Skill, (c->PositionX),
					(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, pos, TKey, &o->m_bySkillSerialNum);
				c->Movement = 0;

				if (o->Type == MODEL_PLAYER)
				{
					SetAction_Fenrir_Skill(c, o);
				}
			}
			else
			{
				c->m_iFenrirSkillTarget = -1;
			}
		}
		else
		{
			if (g_MovementSkill.m_iTarget != -1)
			{
				if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance * 1.2f))
				{
					c->Movement = true;
				}
			}
			else
			{
				Attacking = -1;
			}
		}
	}
	break;
	}
}

void AttackKnight(CHARACTER* c, int Skill, float Distance)
{
	OBJECT* o = &c->Object;
	int ClassIndex = gCharacterManager.GetBaseClass(c->Class);

	int iMana, iSkillMana;
	gSkillManager.GetSkillInformation(Skill, 1, NULL, &iMana, NULL, &iSkillMana);

	int iTypeL = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
	int iTypeR = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;

	if (iTypeR != -1 && (iTypeR < ITEM_STAFF || iTypeR >= (ITEM_STAFF + MAX_ITEM_INDEX)) && (iTypeL < ITEM_STAFF || iTypeL >= (ITEM_STAFF + MAX_ITEM_INDEX))
		|| Skill == Skill_Swell_Life
		|| Skill == Skill_Critical_DMG_Increase_PowUp1
		|| Skill == Skill_Critical_DMG_Increase_PowUp2
		|| Skill == Skill_Critical_DMG_Increase_PowUp3
		|| Skill == Skill_Increase_Critical_Damage
		|| Skill == Skill_Summon
		|| Skill == Skill_Electric_Spark_Strengthener
		|| Skill == Skill_Electric_Spike
		|| Skill == Skill_Earthshake_Mastery
		|| Skill == Skill_Dark_Horse_Strengthener
		|| Skill == Skill_Earthshake
		|| Skill == Skill_Swell_Life_Strengthener
		|| Skill == Skill_Earthshake_Strengthener
		|| Skill == Skill_Swell_Life_Proficiency
		|| Skill == Skill_Brand
		|| Skill == Skill_Plasma_Storm
		|| Skill == Skill_Fire_Scream
		|| Skill == Skill_Fire_Scream_Strengthener
		|| Skill == Skill_Gigantic_Storm
		|| Skill == Skill_Chaotic_Diseier_Strengthener
		|| Skill == Skill_Chaotic_Diseier)
	{
		bool Success = true;

		if (Skill == Skill_Summon && PartyNumber <= 0)
			Success = false;

		if (g_csItemOption.Special_Option_Check(Skill) == false)
			Success = false;

		if (Skill == Skill_Summon && g_DuelMgr.IsDuelEnabled())
			Success = false;

		if (Skill == Skill_Summon && (IsDoppelGanger1() || IsDoppelGanger2() || IsDoppelGanger3() || IsDoppelGanger4()))
			Success = false;

		if (Skill == Skill_Earthshake || Skill == Skill_Dark_Horse_Strengthener || Skill == Skill_Earthshake_Mastery || Skill == Skill_Earthshake_Strengthener)
		{
			BYTE t_DarkLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;
			if (t_DarkLife == 0)
				Success = false;
		}

		if (gMapManager->InChaosCastle())
		{
			if (Skill == Skill_Earthshake
				|| Skill == Skill_Earthshake_Mastery
				|| Skill == Skill_Dark_Horse_Strengthener
				|| Skill == Skill_Earthshake_Strengthener
				|| Skill == Skill_Fire_Breath
				|| (Skill >= AT_PET_COMMAND_DEFAULT && Skill <= AT_PET_COMMAND_TARGET)
				|| Skill == Skill_Plasma_Storm
				)
			{
				Success = false;
			}
		}
		else if (Skill == Skill_Earthshake
			|| Skill == Skill_Dark_Horse_Strengthener
			|| Skill == Skill_Earthshake_Mastery
			|| Skill == Skill_Earthshake_Strengthener
			)
		{
			BYTE t_DarkLife = 0;
			t_DarkLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;
			if (t_DarkLife == 0)
				Success = false;
		}

		if ((unsigned int)iMana > CharacterAttribute->Mana)
		{
			int itemindex = g_pMyInventory->FindManaItemIndex();

			if (itemindex != -1)
			{
				SendRequestUse(itemindex, 0);
			}
			Success = false;
		}

		if (Success && (unsigned int)iSkillMana > CharacterAttribute->SkillMana)
			Success = false;

		if (Success && !gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
			Success = false;

		int iEnergy;
		gSkillManager.GetSkillInformation_Energy(Skill, &iEnergy);

		if ((unsigned int)iEnergy > (CharacterAttribute->Energy + CharacterAttribute->AddEnergy))
			Success = false;

		if (ClassIndex == CLASS_DARK_LORD)
		{
			int iCharisma;
			gSkillManager.GetSkillInformation_Charisma(Skill, &iCharisma);

			if ((unsigned int)iCharisma > (CharacterAttribute->Charisma + CharacterAttribute->AddCharisma))
				Success = false;
		}

		if (Success)
		{
			int targetMovement = c->GetMovement();

			if (Skill > Skill_Twisting_Slash_Strengthener)
			{
				switch (Skill)
				{
				case Skill_Twisting_Slash_Mastery:
				case Skill_Twisting_Slash_Strengthener2:
					goto LABEL_93;
				case Skill_Fire_Slash_Strengthener:
				LABEL_98:
				{
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

					DWORD Strength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;

					if (Strength >= 596 && CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);

						WORD TKey = 0xffff;
						if (g_MovementSkill.m_iTarget != -1)
						{
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
						}
						SendRequestMagicContinue(Skill, c->PositionX, c->PositionY, (BYTE)(o->Angle[2] / 360.f * 256.f), byValue, 0, TKey, 0);
						SetAttackSpeed();
						SetAction(o, PLAYER_HIT_SKILL_WHEEL);
						c->Movement = 0;
					}
				}
				return;
				case Skill_Power_Slash_Strengthener:
				LABEL_106:

					if (c->Helper.Type < (MODEL_HELPER + 2) || c->Helper.Type > (MODEL_HELPER + 4) && c->Helper.Type != (MODEL_HELPER + 37))
					{
						o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

						if (CheckTile(c, TargetX, TargetY, Distance))
						{
							BYTE PathX[1] = { (BYTE)(c->PositionX) };
							BYTE PathY[1] = { (BYTE)(c->PositionY) };
							SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

							BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
							BYTE angle = (signed int)((o->Angle[2] + 180.0) / 360.0 * 255.0);

							WORD TKey = 0xffff;
							if (g_MovementSkill.m_iTarget != -1)
							{
								TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
							}

							SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (o->Angle[2] / 360.0 * 255.0), byValue, angle, TKey, 0);
							SetAttackSpeed();

							if (c->Helper.Type != (MODEL_HELPER + 37) || c->SafeZone)
								SetAction(o, PLAYER_HIT_TWO_HAND_SWORD_TWO, 1);
							else
								SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC, 1);
							c->Movement = 0;
						}
					}
					return;
				case Skill_Chaotic_Diseier_Strengthener:
				LABEL_120:
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE PathX[1] = { (BYTE)(c->PositionX) };
						BYTE PathY[1] = { (BYTE)(c->PositionY) };
						SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
						BYTE angle = (signed int)((o->Angle[2] + 180.0) / 360.0 * 255.0);

						WORD TKey = 0xffff;

						if (g_MovementSkill.m_iTarget != -1)
						{
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
							if (TKey == 0xFFFF)
							{
								CHARACTER* tc = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

								if (tc)
									TKey = tc->Key;
							}
						}
						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);

						SetAttackSpeed();
						c->Movement = 0;

						if (targetMovement == Movement::Fenrir && !c->SafeZone)
						{
							SetAction(o, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, true);
						}
						else if ((targetMovement == Movement::Uniria 
							|| targetMovement == Movement::Dinorant
							|| targetMovement == Movement::Horse
							)&& !c->SafeZone)
						{
							SetAction(o, PLAYER_HIT_RIDE_STRIKE, true);
						}
						else
						{
							SetAction(o, PLAYER_HIT_STRIKE, true);
						}

						float Matrix[3][4];
						vec3_t Light, Position, vPos, Direction;

						Vector(0.f, 0.f, 0.f, vPos);
						AngleMatrix(o->Angle, Matrix);
						VectorRotate(vPos, Matrix, Direction);
						VectorAdd(Direction, o->Position, Position);
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

						CHARACTER* sc = gmCharacters->GetCharacter(SelectedCharacter);

						if (sc && c == Hero)
						{
							VectorCopy(sc->Object.Position, vPos);
							CreateBomb(vPos, true, 6);
						}
						PlayBuffer(SOUND_SKILL_CAOTIC, 0, 0);
					}
					else if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
					{
						c->Movement = 1;
					}
					return;
				case Skill_Critical_DMG_Increase_PowUp1:
				case Skill_Critical_DMG_Increase_PowUp2:
				case Skill_Critical_DMG_Increase_PowUp3:
				LABEL_167:
					SendRequestMagic(Skill, HeroKey);
					c->Movement = 0;
					return;
				case Skill_Fire_Scream_Strengthener:
				LABEL_177:
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
						BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], (int)(c->TargetPosition[0]), (int)(c->TargetPosition[1]));

						WORD TKey = 0xffff;
						if (g_MovementSkill.m_iTarget != -1)
						{
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
						}

						o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
						CheckClientArrow(o);

						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), byValue, pos, TKey, 0);
						SetAttackSpeed();


						if ((targetMovement == Movement::Dinorant
							|| targetMovement == Movement::Uniria
							|| targetMovement == Movement::Horse)
							&& !c->SafeZone
							)
						{
							SetAction(o, PLAYER_HIT_RIDE_STRIKE, 1);
						}
						else if (targetMovement == Movement::Fenrir && !c->SafeZone)
						{
							SetAction(o, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE, 1);
						}
						else
						{
							SetAction(o, PLAYER_HIT_STRIKE, 1);
						}

						c->Movement = 0;
					}
					else
					{
						Attacking = -1;
					}
					return;
				case Skill_Dark_Horse_Strengthener:
				case Skill_Earthshake_Strengthener:
				case Skill_Earthshake_Mastery:
				LABEL_193:
					if (targetMovement != Movement::Horse || c->SafeZone)
						return;
					goto LABEL_196;
				case Skill_Electric_Spark_Strengthener:
				LABEL_196:
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
						BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], (int)(c->TargetPosition[0]), (int)(c->TargetPosition[1]));
						WORD TKey = 0xffff;
						if (g_MovementSkill.m_iTarget != -1)
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), byValue, pos, TKey, 0);
						SetAttackSpeed();

						if (Skill != Skill_Electric_Spike && Skill != Skill_Electric_Spark_Strengthener)
						{
							if (Skill == Skill_Earthshake || Skill == Skill_Dark_Horse_Strengthener || Skill == Skill_Earthshake_Mastery || Skill == Skill_Earthshake_Strengthener)
							{
								SetAction(o, PLAYER_HIT_DARKHORSE, 1);
								PlayBuffer(SOUND_EARTH_QUAKE, 0, 0);
							}
						}
						else if (targetMovement != Movement::Horse || c->SafeZone)
						{
							if (targetMovement != Movement::Fenrir || c->SafeZone)
								SetAction(o, PLAYER_SKILL_FLASH, 1);
							else
								SetAction(o, PLAYER_FENRIR_ATTACK_DARKLORD_FLASH, 1);
						}
						else
						{
							SetAction(o, PLAYER_HIT_RIDE_ATTACK_FLASH, 1);
						}
						c->Movement = 0;
					}
					else
					{
						Attacking = -1;
					}
					break;
				case Skill_Swell_Life_Strengthener:
				case Skill_Swell_Life_Proficiency:
				LABEL_219:
					SendRequestMagic(Skill, HeroKey);
					SetAction(o, PLAYER_SKILL_VITALITY, 1);
					c->Movement = 0;
					break;
				case Skill_Anger_Blow_Strengthener:
				case Skill_Anger_Blow_Mastery:
				LABEL_224:

					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], (int)(c->TargetPosition[0]), (int)(c->TargetPosition[1]));
						WORD TKey = 0xffff;

						if (g_MovementSkill.m_iTarget != -1)
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, pos, TKey, 0);
						SetAction(o, PLAYER_HIT_SKILL_FURY_STRIKE, 1);
						c->Movement = 0;
					}
					else
					{
						Attacking = -1;
					}
					break;
				case Skill_Strike_of_Destruction_Strengthe:
				LABEL_232:
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], (int)(c->TargetPosition[0]), (int)(c->TargetPosition[1]));
						WORD TKey = 0xffff;

						if (g_MovementSkill.m_iTarget != -1)
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						SendRequestMagicContinue(Skill, TargetX, TargetY, (BYTE)(o->Angle[2] / 360.f * 256.f), 0, pos, TKey, 0);

						SetAction(o, PLAYER_SKILL_BLOW_OF_DESTRUCTION, 1);
						c->Movement = 0;
					}
					else
					{
						Attacking = -1;
					}
					break;
				default:
					return;
				}
			}
			else if (Skill == Skill_Twisting_Slash_Strengthener)
			{
			LABEL_93:
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
				BYTE PathX[1] = { (BYTE)(c->PositionX) };
				BYTE PathY[1] = { (BYTE)(c->PositionY) };
				SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

				WORD TKey = 0xffff;
				if (g_MovementSkill.m_iTarget != -1)
				{
					TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				}

				SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, TKey, 0);
				SetAttackSpeed();
				SetAction(o, PLAYER_HIT_SKILL_WHEEL);
				c->Movement = 0;
			}
			else
			{
				switch (Skill)
				{
				case Skill_Twisting_Slash:
					goto LABEL_93;
				case Skill_Fire_Slash:
					goto LABEL_98;
				case Skill_Power_Slash:
					goto LABEL_106;
				case Skill_Chaotic_Diseier:
					goto LABEL_120;
				case Skill_Flame_Strike:
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE PathX[1] = { (BYTE)(c->PositionX) };
						BYTE PathY[1] = { (BYTE)(c->PositionY) };
						SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
						BYTE angle = (signed int)((o->Angle[2] + 180.0) / 360.0 * 255.0);
						WORD TKey = 0xffff;

						if (g_MovementSkill.m_iTarget != -1)
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);
						SetAttackSpeed();
						SetAction(o, PLAYER_SKILL_FLAMESTRIKE, 1);
						c->Movement = 0;
					}
					return;
				case Skill_Gigantic_Storm:
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						BYTE PathX[1] = { (BYTE)(c->PositionX) };
						BYTE PathY[1] = { (BYTE)(c->PositionY) };
						SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

						BYTE byValue = GetDestValue(c->PositionX, c->PositionY, TargetX, TargetY);
						BYTE angle = (signed int)((o->Angle[2] + 180.0) / 360.0 * 255.0);
						WORD TKey = 0xffff;

						if (g_MovementSkill.m_iTarget != -1)
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);

						SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);
						SetAttackSpeed();
						SetAction(o, PLAYER_SKILL_GIGANTICSTORM, 1);
						c->Movement = 0;
					}
					return;
				case Skill_Summon:
					if (!gMapManager->IsCursedTemple() || g_pMyInventory->IsItem(ITEM_POTION + 64, true))
					{
						SendRequestMagic(Skill, HeroKey);
						c->Movement = 0;
					}
					return;
				case Skill_Increase_Critical_Damage:
					goto LABEL_167;
				case Skill_Brand:
					SendRequestMagic(Skill, HeroKey);
					c->Movement = 0;
					return;
				case Skill_Fire_Scream:
					goto LABEL_177;
				case Skill_Earthshake:
					goto LABEL_193;
				case Skill_Electric_Spike:
					goto LABEL_196;
				case Skill_Swell_Life:
					goto LABEL_219;
				case Skill_Rageful_Blow:
					goto LABEL_224;
				case Skill_Strike_of_Destruction:
					goto LABEL_232;
				case Skill_Plasma_Storm:
					if (CheckTile(c, TargetX, TargetY, Distance))
					{
						WORD TKey = -1;
						BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], (int)(c->TargetPosition[0]), (int)(c->TargetPosition[1]));

						if (g_MovementSkill.m_iTarget == -1)
						{
							c->m_iFenrirSkillTarget = -1;
						}
						else
						{
							o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
							gSkillManager.CheckSkillDelay(Hero->CurrentSkill);
							TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
							c->m_iFenrirSkillTarget = g_MovementSkill.m_iTarget;

							SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, pos, TKey, &o->m_bySkillSerialNum);
							c->Movement = 0;
							if (o->Type == MODEL_PLAYER)
							{
								SetAction_Fenrir_Skill(c, o);
							}
						}
					}
					else if (g_MovementSkill.m_iTarget == -1)
					{
						Attacking = -1;
					}
					else if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance, 2))
					{
						c->Movement = 1;
					}
					break;
				default:
					return;
				}
			}
		}
	}
}

void AttackWizard(CHARACTER* c, int Skill, float Distance)
{
	OBJECT* o = &c->Object;
	int ClassIndex = c->GetBaseClass();

	int iEnergy, iMana, iSkillMana;
	if (Skill != Skill_Nova_begin && Skill != Skill_Nova)
	{
		gSkillManager.GetSkillInformation_Energy(Skill, &iEnergy);
		gSkillManager.GetSkillInformation(Skill, 1, NULL, &iMana, NULL, &iSkillMana);
	}
	else
	{
		gSkillManager.GetSkillInformation_Energy(Skill_Nova, &iEnergy);
		gSkillManager.GetSkillInformation(Skill_Nova, 1, NULL, &iMana, NULL, &iSkillMana);

		if (Skill == Skill_Nova)
			iSkillMana = 0;
	}

	if ((unsigned int)iEnergy > (CharacterAttribute->Energy + CharacterAttribute->AddEnergy))
		return;

	if ((unsigned int)iMana > CharacterAttribute->Mana)
	{
		int Index = g_pMyInventory->FindManaItemIndex();
		if (Index != -1)
		{
			SendRequestUse(Index, 0);
		}
		return;
	}

	if ((unsigned int)iSkillMana > CharacterAttribute->SkillMana)
	{
		if (Skill == Skill_Nova_begin || Skill == Skill_Nova)
		{
			MouseRButtonPop = false;
			MouseRButtonPush = false;
			MouseRButton = false;
			MouseRButtonPress = 0;
		}
		return;
	}

	if (!gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
		return;

	bool Success = CheckTarget(c);

	switch (Skill)
	{
	case Skill_Soul_Barrier_Strengthener:
	case Skill_Soul_Barrier_Proficiency:
	case Skill_Soul_Barrier_Mastery:
	case Skill_Soul_Barrier:
		if (g_pPartyManager->IsPartyMember(SelectedCharacter))
		{
			c->TargetCharacter = SelectedCharacter;
			CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

			if (pCharacter)
			{
				memset(&g_MovementSkill, 0, sizeof(g_MovementSkill));
				g_MovementSkill.m_bMagic = TRUE;
				g_MovementSkill.m_iSkill = Hero->CurrentSkill;
				g_MovementSkill.m_iTarget = SelectedCharacter;
				g_MovementSkill.m_iKey = pCharacter->Key;
			}

			if (!CheckTile(c, TargetX, TargetY, Distance))
			{
				if (SelectedCharacter != -1 && PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
				{
					c->Movement = true;
					c->MovementType = MOVEMENT_SKILL;
					g_pNewUIMiniMap->StopMove();
					SendMove(c, o);
				}
				return;
			}

			SendRequestMagic(Skill, g_MovementSkill.m_iKey);
			SetPlayerMagic(c);
		}
		else
		{
			SendRequestMagic(Skill, HeroKey);
			SetPlayerMagic(c);
			Attacking = -1;
		}
		break;
	case AT_SKILL_BLAST_HELL_BEGIN:
	{
		SendRequestMagic(Skill, HeroKey);

		SetAttackSpeed();
		SetAction(o, PLAYER_SKILL_HELL_BEGIN);
		c->Movement = 0;
	}
	return;
	case AT_SKILL_BLAST_HELL:
	case Skill_Nova_Strengthener:
	{
		int iTargetKey = getTargetCharacterKey(c, SelectedCharacter, 0);
		if (iTargetKey == -1) {
			iTargetKey = HeroKey;
		}
		SendRequestMagic(Skill, iTargetKey);

		SetAttackSpeed();
		SetAction(o, PLAYER_SKILL_HELL_START);
		c->Movement = 0;
	}
	return;
	case AT_SKILL_HELL:
	case Skill_Hellfire_Strengthener:
	{
		SendRequestMagicContinue(Skill, (c->PositionX),
			(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, 0);
		SetAttackSpeed();
		SetAction(o, PLAYER_SKILL_HELL);
		c->Movement = 0;
	}
	return;
	case AT_SKILL_INFERNO:
	case Skill_Inferno_Strengthener:
	case Skill_Inferno_Strengthener2:
	{
		SendRequestMagicContinue(Skill, (c->PositionX),
			(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, 0);
		SetAttackSpeed();
		SetAction(o, PLAYER_SKILL_INFERNO);
		c->Movement = 0;
	}
	return;
	case AT_SKILL_PLASMA_STORM_FENRIR:
	{
		if (CheckAttack())
		{
			g_MovementSkill.m_iTarget = SelectedCharacter;

			CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);
			g_MovementSkill.m_iKey = (tc) ? tc->Key : 0;
		}
		else
		{
			g_MovementSkill.m_iTarget = -1;
			g_MovementSkill.m_iKey = 0;
		}

		int TargetX = (int)(c->TargetPosition[0] / TERRAIN_SCALE);
		int TargetY = (int)(c->TargetPosition[1] / TERRAIN_SCALE);
		if (CheckTile(c, o, Distance))//&& CheckAttack())
		{
			BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
			WORD TKey = 0xffff;
			if (g_MovementSkill.m_iTarget != -1)
			{
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

				gSkillManager.CheckSkillDelay(Hero->CurrentSkill);

				TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				c->m_iFenrirSkillTarget = g_MovementSkill.m_iTarget;
				SendRequestMagicContinue(Skill, (c->PositionX),
					(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, pos, TKey, &o->m_bySkillSerialNum);
				c->Movement = 0;

				if (o->Type == MODEL_PLAYER)
				{
					SetAction_Fenrir_Skill(c, o);
				}
			}
			else
			{
				c->m_iFenrirSkillTarget = -1;
			}
		}
		else
		{
			if (g_MovementSkill.m_iTarget != -1)
			{
				if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance * 1.2f))
				{
					c->Movement = true;
				}
			}
			else
			{
				Attacking = -1;
			}
		}
	}
	return;
	case AT_SKILL_BLOCKING:
	case AT_SKILL_SWORD1:
	case AT_SKILL_SWORD2:
	case AT_SKILL_SWORD3:
	case AT_SKILL_SWORD4:
	case AT_SKILL_SWORD5:
	case Skill_Impale:
	case Skill_Slash_Strengthener:
		return;
	case AT_SKILL_SWELL_OF_MAGICPOWER:
		if (g_isCharacterBuff((&Hero->Object), eBuff_SwellOfMagicPower) == false)
		{
			SendRequestMagic(Skill, HeroKey);
			c->Movement = 0;
		}
		break;
	case Skill_Expansion_of_Wizardry_Power_Up:
		if (g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_IMPROVED) == false)
		{
			SendRequestMagic(Skill, HeroKey);
			c->Movement = 0;
		}
		break;
	case Skill_Expansion_of_Wizardry_Mastery:
		if (g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_ENHANCED) == false)
		{
			SendRequestMagic(Skill, HeroKey);
			c->Movement = 0;
		}
		break;
	}

	CHARACTER* pChar = gmCharacters->GetCharacter(SelectedCharacter);

	if (pChar)
	{
		ZeroMemory(&g_MovementSkill, sizeof(g_MovementSkill));
		g_MovementSkill.m_bMagic = TRUE;
		g_MovementSkill.m_iSkill = Hero->CurrentSkill;
		if (CheckAttack())
		{
			g_MovementSkill.m_iTarget = SelectedCharacter;
			g_MovementSkill.m_iKey = pChar->Key;
		}
		else
		{
			g_MovementSkill.m_iTarget = -1;
			g_MovementSkill.m_iKey = 0;
		}
	}

	if (!CheckTile(c, o, Distance))
	{
		if (SelectedCharacter != -1 && CheckAttack())
		{
			if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
			{
				c->Movement = true;
				c->MovementType = MOVEMENT_SKILL;
				g_pNewUIMiniMap->StopMove();
				SendMove(c, o);
			}
		}

		if (Skill != AT_SKILL_STUN
			&& Skill != AT_SKILL_REMOVAL_STUN
			&& Skill != AT_SKILL_MANA
			&& Skill != AT_SKILL_INVISIBLE
			&& Skill != AT_SKILL_REMOVAL_INVISIBLE
			&& Skill != AT_SKILL_PLASMA_STORM_FENRIR
			&& Skill != AT_SKILL_ALICE_BERSERKER
			&& Skill != AT_SKILL_ALICE_WEAKNESS
			&& Skill != AT_SKILL_ALICE_ENERVATION
			&& Skill != Skill_Berserker_Strengthener
			&& Skill != Skill_Berserker_Proficiency
			&& Skill != Skill_Berserker_Mastery
			&& Skill != Skill_Weakness_Strengthener
			&& Skill != Skill_Innovation_Strengthener
			)
			return;
	}

	bool Wall = CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY);
	if (Wall)
	{
		if (SelectedCharacter != -1)
		{
			if (CheckAttack())
			{
				UseSkillWizard(c, o);
			}
		}
		if (Success)
		{
			o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
			switch (Skill)
			{
			case AT_SKILL_STORM:
			case Skill_Evil_Spirit_Strengthener:
			case Skill_Evil_Spirit_Strengthener2:
			case Skill_Evil_Spirit:
			{
				SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, &o->m_bySkillSerialNum);
				SetPlayerMagic(c);
			}
			return;
			case AT_SKILL_FLASH:
			{
				SendRequestMagicContinue(Skill, (c->PositionX),
					(c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, 0);
				SetAttackSpeed();

				if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
					SetAction(o, PLAYER_SKILL_FLASH);
				else
					SetAction(o, PLAYER_SKILL_FLASH);

				c->Movement = 0;
				StandTime = 0;
			}
			return;
			case AT_SKILL_FLAME:
			case Skill_Flame_Strengthener:
			case Skill_Flame_Strengthener2:
				SendRequestMagicContinue(Skill, (BYTE)(c->TargetPosition[0] / TERRAIN_SCALE), (BYTE)(c->TargetPosition[1] / TERRAIN_SCALE), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, 0xffff, 0);
				SetPlayerMagic(c);
				return;
			case AT_SKILL_BLAST_POISON:
			case Skill_Decay_Strengthener:
			case Skill_Ice_Storm_Strengthener:
			case Skill_Ice_Storm_Mastery:
			case AT_SKILL_BLAST_FREEZE:
			{
				WORD TKey = 0xffff;
				if (g_MovementSkill.m_iTarget != -1)
				{
					TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				}
				SendRequestMagicContinue(Skill, (BYTE)(c->TargetPosition[0] / TERRAIN_SCALE), (BYTE)(c->TargetPosition[1] / TERRAIN_SCALE), (BYTE)(o->Angle[2] / 360.f * 256.f), 0, 0, TKey, 0);
				SetPlayerMagic(c);
				c->Movement = 0;
			}
			return;
			}
		}
	}
	if (ClassIndex == CLASS_WIZARD && Success)
	{
		switch (Skill)
		{
		case AT_SKILL_TELEPORT_B:
			if (gMapManager->IsCursedTemple()
				&& !g_pMyInventory->IsItem(ITEM_POTION + 64, true))
			{
				return;
			}
			if (SelectedCharacter != -1)
			{
				if (!g_pPartyManager->IsPartyMember(SelectedCharacter))
					return;

				if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
				{
					return;
				}

				CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);
				OBJECT* to = &tc->Object;
				bool Success = false;
				if (to->Type == MODEL_PLAYER)
				{
					if (to->CurrentAction != PLAYER_SKILL_TELEPORT)
						Success = true;
				}
				else
				{
					if (to->CurrentAction != MONSTER01_SHOCK)
						Success = true;
				}
				if (Success && to->Teleport != TELEPORT_BEGIN && to->Teleport != TELEPORT && to->Alpha >= 0.7f)
				{
					int Wall, indexX, indexY, TargetIndex, count = 0;
					int PositionX = (int)(c->Object.Position[0] / TERRAIN_SCALE);
					int PositionY = (int)(c->Object.Position[1] / TERRAIN_SCALE);

					while (1)
					{
						indexX = rand() % 3;
						indexY = rand() % 3;

						if (indexX != 1 || indexY != 1)
						{
							TargetX = (PositionX - 1) + indexX;
							TargetY = (PositionY - 1) + indexY;

							TargetIndex = TERRAIN_INDEX(TargetX, TargetY);

							Wall = TerrainWall[TargetIndex];

							if ((Wall & TW_ACTION) == TW_ACTION)
							{
								Wall -= TW_ACTION;
							}
							if (gMapManager->currentMap == WD_30BATTLECASTLE)
							{
								int ax = (Hero->PositionX);
								int ay = (Hero->PositionY);
								if ((ax >= 150 && ax <= 200) && (ay >= 180 && ay <= 230))
									Wall = 0;
							}
							if (Wall == 0) break;

							count++;
						}

						if (count > 10) return;
					}
					to->Angle[2] = CreateAngle(to->Position[0], to->Position[1], tc->TargetPosition[0], tc->TargetPosition[1]);
					bool bResult;
					SendRequestMagicTeleportB(&bResult, tc->Key, TargetX, TargetY);
					if (bResult)
					{
						SetPlayerTeleport(tc);
					}
				}
			}
			return;

		case AT_SKILL_TELEPORT:
		{
			if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem()
				|| g_isCharacterBuff(o, eDeBuff_Stun)
				|| g_isCharacterBuff(o, eDeBuff_Sleep)
				)
			{
				return;
			}

#ifdef PBG_ADD_NEWCHAR_MONK
			WORD byHeroPriorSkill = g_pSkillList->GetHeroPriorSkill();
#else //PBG_ADD_NEWCHAR_MONK
			BYTE byHeroPriorSkill = g_pSkillList->GetHeroPriorSkill();
#endif //PBG_ADD_NEWCHAR_MONK
			if (c == Hero && byHeroPriorSkill == AT_SKILL_BLAST_HELL)
			{
				g_pSkillList->SetHeroPriorSkill(AT_SKILL_TELEPORT);
				SendRequestMagic(byHeroPriorSkill, HeroKey);
				SetAttackSpeed();
				SetAction(&Hero->Object, PLAYER_SKILL_HELL_BEGIN);
				Hero->Movement = 0;
				return;
		}

			bool Success = false;
			if (o->Type == MODEL_PLAYER)
			{
				if (o->CurrentAction != PLAYER_SKILL_TELEPORT)
					Success = true;
			}
			else
			{
				if (o->CurrentAction != MONSTER01_SHOCK)
					Success = true;
			}
			if (Success && o->Teleport != TELEPORT_BEGIN && o->Teleport != TELEPORT && o->Alpha >= 0.7f)
			{
				int TargetIndex = TERRAIN_INDEX_REPEAT(TargetX, TargetY);
				int Wall = TerrainWall[TargetIndex];
				if ((Wall & TW_ACTION) == TW_ACTION) Wall -= TW_ACTION;
				if ((Wall & TW_HEIGHT) == TW_HEIGHT) Wall -= TW_HEIGHT;
				if (Wall == 0)
				{
					o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
					bool bResult;
					SendRequestMagicTeleport(&bResult, 0, TargetX, TargetY);
					if (bResult)
					{
						if (g_isCharacterBuff(o, eDeBuff_Stun))
						{
							UnRegisterBuff(eDeBuff_Stun, o);
						}

						if (g_isCharacterBuff(o, eDeBuff_Sleep))
						{
							UnRegisterBuff(eDeBuff_Sleep, o);
						}

						SetPlayerTeleport(c);
					}
				}
			}
		}
		return;
	}
}
	else if (ClassIndex == CLASS_SUMMONER && Success)
	{
		if (g_SummonSystem.SendRequestSummonSkill(Skill, c, o) == TRUE)
		{
			return;
		}

		int iEnergy;
		gSkillManager.GetSkillInformation_Energy(Skill, &iEnergy);
		if ((unsigned int)iEnergy > (CharacterAttribute->Energy + CharacterAttribute->AddEnergy))
		{
			return;
		}

		g_MovementSkill.m_bMagic = TRUE;
		g_MovementSkill.m_iSkill = Hero->CurrentSkill;
		g_MovementSkill.m_iTarget = SelectedCharacter;

		CHARACTER* tc = gmCharacters->GetCharacter(SelectedCharacter);
		g_MovementSkill.m_iKey = (tc) ? tc->Key : 0;

		switch (Skill)
		{
		case AT_SKILL_ALICE_THORNS:
		{
			CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

			if (SelectedCharacter == -1 || (pCharacter && pCharacter->Object.Kind != KIND_PLAYER))
			{
				LetHeroStop();

				switch (c->Helper.Type)
				{
				case MODEL_HELPER + 2:
					SetAction(o, PLAYER_SKILL_SLEEP_UNI);
					break;
				case MODEL_HELPER + 3:
					SetAction(o, PLAYER_SKILL_SLEEP_DINO);
					break;
				case MODEL_HELPER + 37:
					SetAction(o, PLAYER_SKILL_SLEEP_FENRIR);
					break;
				default:
					SetAction(o, PLAYER_SKILL_SLEEP);
					break;
				}
				SendRequestMagic(Skill, HeroKey);
			}
			else if (pCharacter && pCharacter->Dead == 0 && pCharacter->Object.Kind == KIND_PLAYER)
			{
				CHARACTER* tc = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

				if (tc)
				{
					TargetX = (int)(tc->Object.Position[0] / TERRAIN_SCALE);
					TargetY = (int)(tc->Object.Position[1] / TERRAIN_SCALE);

					if (CheckTile(c, o, Distance) && c->SafeZone == false)
					{
						bool bNoneWall = CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY);
						if (bNoneWall)
						{
							UseSkillSummon(c, o);
						}
					}
					else
					{
						if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
						{
							c->Movement = true;
						}
					}
				}
			}
		}
		break;
		case AT_SKILL_ALICE_BERSERKER:
		case AT_SKILL_ALICE_WEAKNESS:
		case AT_SKILL_ALICE_ENERVATION:
		case Skill_Berserker_Strengthener:
		case Skill_Berserker_Proficiency:
		case Skill_Berserker_Mastery:

		case Skill_Weakness_Strengthener:
		case Skill_Innovation_Strengthener:
			UseSkillSummon(c, o);
			break;
		case AT_SKILL_LIGHTNING_SHOCK:
		case Skill_Lightning_Shock_Strengthener:
		{
			o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

			if (CheckTile(c, o, Distance))
			{
				BYTE PathX[1];
				BYTE PathY[1];
				PathX[0] = (c->PositionX);
				PathY[0] = (c->PositionY);

				SendCharacterMove(c->Key, o->Angle[2], 1, &PathX[0], &PathY[0], TargetX, TargetY);

				BYTE byValue = GetDestValue((c->PositionX), (c->PositionY), TargetX, TargetY);

				BYTE angle = (BYTE)((((o->Angle[2] + 180.f) / 360.f) * 255.f));
				WORD TKey = 0xffff;
				if (g_MovementSkill.m_iTarget != -1)
				{
					TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				}
				SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), ((o->Angle[2] / 360.f) * 255), byValue, angle, TKey, 0);
				SetAttackSpeed();
				SetAction(o, PLAYER_SKILL_LIGHTNING_SHOCK);
				c->Movement = 0;
			}
		}
		break;
		}

		if (SelectedCharacter == -1)
		{
			return;
		}

		switch (Skill)
		{
		case Skill_Drain_Life_Strengthener:
		case Skill_Drain_Life_Mastery:
		case Skill_Chain_Lightning_Strengthener:
		case AT_SKILL_ALICE_DRAINLIFE:
		case AT_SKILL_ALICE_LIGHTNINGORB:
		case AT_SKILL_ALICE_CHAINLIGHTNING:
		{
			c->TargetCharacter = SelectedCharacter;
			CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

			if (pCharacter)
			{
				if (pCharacter->Dead == 0)
				{
					pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

					if (pCharacter)
					{
						TargetX = (int)(pCharacter->Object.Position[0] / TERRAIN_SCALE);
						TargetY = (int)(pCharacter->Object.Position[1] / TERRAIN_SCALE);

						if (CheckAttack() == true)
						{
							if (CheckTile(c, o, Distance) && c->SafeZone == false)
							{
								bool bNoneWall = CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY);
								if (bNoneWall)
								{
									UseSkillSummon(c, o);
								}
							}
							else
							{
								if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
								{
									c->Movement = true;
								}
							}
						}
					}
				}
			}
		}
		break;
		case Skill_Sleep_Strengthener:
		case AT_SKILL_ALICE_SLEEP:
		case AT_SKILL_ALICE_BLIND:
		{
			CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

			if (pCharacter)
			{
				if (pCharacter->Object.Kind == KIND_PLAYER)
				{
					if (gMapManager->InChaosCastle() == true
						|| gMapManager->IsCursedTemple() == true
						|| (gMapManager->InBattleCastle() == true && battleCastle::IsBattleCastleStart() == true)
						|| g_DuelMgr.IsDuelEnabled()
						)
					{
					}
					else
					{
						break;
					}
				}
				if (pCharacter->Dead == 0)
				{
					pCharacter = gmCharacters->GetCharacter(g_MovementSkill.m_iTarget);

					if (pCharacter)
					{
						TargetX = (int)(pCharacter->Object.Position[0] / TERRAIN_SCALE);
						TargetY = (int)(pCharacter->Object.Position[1] / TERRAIN_SCALE);

						if (CheckAttack() == true)
						{
							if (CheckTile(c, o, Distance) && c->SafeZone == false)
							{
								bool bNoneWall = CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY);
								if (bNoneWall)
								{
									UseSkillSummon(c, o);
								}
							}
							else
							{
								if (PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path, Distance))
								{
									c->Movement = true;
								}
							}
						}
					}
				}
			}
		}
		break;
		}
	}
}

void AttackCommon(CHARACTER* c, int Skill, float Distance)
{
	OBJECT* o = &c->Object;

	int ClassIndex = gCharacterManager.GetBaseClass(c->Class);

	int iMana, iSkillMana;
	gSkillManager.GetSkillInformation(Skill, 1, NULL, &iMana, NULL, &iSkillMana);

	if (o->Type == MODEL_PLAYER)
	{
		bool Success = true;

		if ((unsigned int)iMana > CharacterAttribute->Mana)
		{
			int Index = g_pMyInventory->FindManaItemIndex();
			if (Index != -1)
			{
				SendRequestUse(Index, 0);
			}
			Success = false;
		}
		if (Success && (unsigned int)iSkillMana > CharacterAttribute->SkillMana)
		{
			Success = false;
		}
		if (Success && !gSkillManager.CheckSkillDelay(Hero->CurrentSkill))
		{
			Success = false;
		}

		int iEnergy;
		gSkillManager.GetSkillInformation_Energy(Skill, &iEnergy);
		if (Success && (unsigned int)iEnergy > (CharacterAttribute->Energy + CharacterAttribute->AddEnergy))
		{
			Success = false;
		}

		switch (Skill)
		{
		case    AT_SKILL_STUN:
		{
			//	                        if ( CheckTile( c, o, Distance ) )
			{
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);

				int TargetX = (int)(c->TargetPosition[0] / TERRAIN_SCALE);
				int TargetY = (int)(c->TargetPosition[1] / TERRAIN_SCALE);
				BYTE byValue = GetDestValue((c->PositionX), (c->PositionY), TargetX, TargetY);

				BYTE pos = CalcTargetPos(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
				WORD TKey = 0xffff;
				if (g_MovementSkill.m_iTarget != -1)
				{
					TKey = getTargetCharacterKey(c, g_MovementSkill.m_iTarget, g_MovementSkill.m_iKey);
				}
				SendRequestMagicContinue(Skill, (c->PositionX), (c->PositionY), (BYTE)(o->Angle[2] / 360.f * 256.f), byValue, pos, TKey, 0);
				SetAttackSpeed();

				if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
				{
					SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
				}
				else if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
				{
					SetAction(o, PLAYER_SKILL_RIDER);
				}
				else if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
				{
					SetAction(o, PLAYER_SKILL_RIDER_FLY);
				}
				else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
				{
					SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
				}
				else
				{
					SetAction(o, PLAYER_SKILL_VITALITY);
				}
				c->Movement = 0;
			}
		}
		break;

		case    AT_SKILL_REMOVAL_STUN:
		{
			if (SelectedCharacter == -1)
			{
				SendRequestMagic(Skill, HeroKey);
			}
			else
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter)
				{
					SendRequestMagic(Skill, pCharacter->Key);
				}
			}


			if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
			{
				SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
			}
			else
				if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
				{
					SetAction(o, PLAYER_SKILL_RIDER);
				}
				else
					if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
					{
						SetAction(o, PLAYER_SKILL_RIDER_FLY);
					}
					else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
					{
						SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
					}
					else
					{
						SetAction(o, PLAYER_HIT_REMOVAL);
					}
			c->Movement = 0;
		}
		break;

		case    AT_SKILL_MANA:
			if (SelectedCharacter == -1)
			{
				SendRequestMagic(Skill, HeroKey);
			}
			else
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter)
				{
					SendRequestMagic(Skill, pCharacter->Key);
				}
			}

			if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
			{
				SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
			}
			else if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER);
			}
			else if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER_FLY);
			}
			else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
			{
				SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
			}
			else
				SetAction(o, PLAYER_SKILL_VITALITY);
			c->Movement = 0;
			break;

		case    AT_SKILL_INVISIBLE:

			if (SelectedCharacter == -1)
			{
				SendRequestMagic(Skill, HeroKey);
			}
			else
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter && pCharacter->Object.Kind == KIND_PLAYER)
				{
					SendRequestMagic(Skill, pCharacter->Key);
				}
			}

			if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
			{
				SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
			}
			else if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER);
			}
			else if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER_FLY);
			}
			else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
			{
				SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
			}
			else
				SetAction(o, PLAYER_SKILL_VITALITY);
			c->Movement = 0;
			break;

		case AT_SKILL_REMOVAL_INVISIBLE:
			if (SelectedCharacter == -1)
			{
				SendRequestMagic(Skill, HeroKey);
			}
			else
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter)
				{
					SendRequestMagic(Skill, pCharacter->Key);
				}
			}

			if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
			{
				SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
			}
			else if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER);
			}
			else if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER_FLY);
			}
			else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
			{
				SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
			}
			else
				SetAction(o, PLAYER_HIT_REMOVAL);
			c->Movement = 0;
			break;

		case AT_SKILL_REMOVAL_BUFF:

			if (SelectedCharacter == -1)
			{
				SendRequestMagic(Skill, HeroKey);
			}
			else
			{
				CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

				if (pCharacter)
				{
					SendRequestMagic(Skill, pCharacter->Key);
				}
			}

			if (c->Helper.Type == MODEL_HELPER + 4 && !c->SafeZone)
			{
				SetAction(o, PLAYER_HIT_RIDE_ATTACK_MAGIC);
			}
			else if (c->Helper.Type == MODEL_HELPER + 2 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER);
			}
			else if (c->Helper.Type == MODEL_HELPER + 3 && !c->SafeZone)
			{
				SetAction(o, PLAYER_SKILL_RIDER_FLY);
			}
			else if (c->Helper.Type == MODEL_HELPER + 37 && !c->SafeZone)
			{
				SetAction(o, PLAYER_FENRIR_ATTACK_MAGIC);
			}
			else
				SetAction(o, PLAYER_SKILL_VITALITY);
			c->Movement = 0;
			break;
		}
	}
}

bool SkillKeyPush(int Skill)
{
	if (Skill == AT_SKILL_BLAST_HELL && MouseLButtonPush)
	{
		return true;
	}
	return false;
}

void Attack(CHARACTER* c)
{
	if ((MouseOnWindow || !SEASON3B::CheckMouseIn(0, 0, GetScreenWidth(), pos_botton(428.f))))
	{
		if (MouseLButtonPush)
		{
			MouseRButtonPop = false;
			MouseRButtonPush = false;
			MouseRButton = false;
			MouseRButtonPress = 0;
			return;
		}
	}

	if (!g_isCharacterBuff((&c->Object), eDeBuff_Stun)
		&& !g_isCharacterBuff((&c->Object), eDeBuff_Sleep)
		&& !g_isCharacterBuff((&Hero->Object), eBuff_DuelWatch))
	{
		OBJECT* o = &c->Object;
		int ClassIndex = gCharacterManager.GetBaseClass(c->Class);

		if (o->Teleport == TELEPORT_BEGIN)
		{
			o->Alpha -= 0.1f;
			if (o->Alpha < 0.1f)
			{
				o->Teleport = TELEPORT;
			}
		}
		bool Success = false;
		bool CursedTempleSkillSuccess = false;
		int Skill = CharacterAttribute->Skill[Hero->CurrentSkill];
		float Distance = gSkillManager.GetSkillDistance(Skill, c);

		if (EnableFastInput)
		{
			if (MouseLButtonPush || MouseLButton)
			{
				MouseLButtonPush = false;
				Success = true;
			}
		}
		else
		{
			if (MouseRButtonPress)
			{
				if (MouseRButtonPop || SkillKeyPush(Skill) || MouseRButtonPress > 0x1F4)
				{
					MouseRButtonPop = false;
					MouseRButtonPush = false;
					MouseRButton = false;
					MouseRButtonPress = 0;
					Success = true;
				}
				else
				{
					++MouseRButtonPress;
				}
			}
			else if ((MouseRButtonPush || MouseRButton) && !MouseLButtonPush && !MouseLButton && !c->Movement)
			{
				if (Skill == Skill_Nova)
				{
					if (o->Teleport != TELEPORT_END && o->Teleport)
						return;

					int iReqEng = 0;
					gSkillManager.GetSkillInformation_Energy(Skill, &iReqEng);

					if (CharacterAttribute->Energy + CharacterAttribute->AddEnergy < (unsigned int)iReqEng)
						return;

					MouseRButtonPress = 1;
					Hero->Object.m_bySkillCount = 0;
					Skill = Skill_Nova_begin;
				}
				SEASON3B::CNewUIInventoryCtrl::BackupPickedItem();
				MouseRButtonPush = false;
				Success = true;
			}

			if (g_pOption->IsAutoAttack() && gMapManager->currentMap != WD_6STADIUM && !gMapManager->InChaosCastle() && Attacking == 2 && SelectedCharacter != -1)
				Success = true;

			if (Success)
			{
				RButtonPressTime = ((WorldTime - RButtonPopTime) / CLOCKS_PER_SEC);
				if (RButtonPressTime >= AutoMouseLimitTime)
				{
					MouseRButtonPush = false;
					MouseRButton = false;
					Success = FALSE;
				}
			}
			else
			{
				RButtonPopTime = WorldTime;
				RButtonPressTime = 0.f;
			}
		}

		if (Success)
		{
			g_iFollowCharacter = -1;
			if (!g_isCharacterBuff((&Hero->Object), eBuff_Cloaking) && giPetManager::SendPetCommand(c, Hero->CurrentSkill) == true)
			{
				return;
			}

			if (gMapManager->IsCursedTemple())
			{
				if (g_pCursedTempleWindow->IsCursedTempleSkillKey(SelectedCharacter))
				{
					CursedTempleSkillSuccess = true;
				}
				else
				{
					if (g_CursedTemple->IsPartyMember(SelectedCharacter) == true)
					{
						if (!IsCorrectSkillType_FrendlySkill(Skill) && !IsCorrectSkillType_Buff(Skill))
							return;
					}
					else
					{
						if ((IsCorrectSkillType_FrendlySkill(Skill) || IsCorrectSkillType_Buff(Skill)) && SelectedCharacter != -1)
						{
							return;
						}
					}
					if (!CheckSkillUseCondition(o, Skill))
						return;
				}
			}
			else if (!CheckSkillUseCondition(o, Skill))
			{
				return;
			}

			if (Skill >= AT_SKILL_SUMMON_EXPLOSION && Skill <= AT_SKILL_SUMMON_POLLUTION)
			{
				CheckTarget(c);
				if (!CheckTile(c, o, Distance))
					return;
			}
		}

		if (gSkillManager.DemendConditionCheckSkill(Skill) && Success && c->Dead == 0)
		{
			if (!c->SafeZone && CursedTempleSkillSuccess)
			{
				g_pCursedTempleWindow->SetCursedTempleSkill(c, o, SelectedCharacter);
				return;
			}
			if ((!c->SafeZone || gMapManager->InBloodCastle() == true || gMapManager->InChaosCastle() == true) && (!c->SafeZone
				|| Skill == Skill_Heal
				|| Skill == Skill_Greater_Defense
				|| Skill == Skill_Greater_Damage
				|| Skill == Skill_Soul_Barrier
				|| Skill == Skill_Swell_Life
				|| Skill == Skill_Infinity_Arrow
				|| Skill == Skill_Expansion_of_Wizardry
				|| Skill == Skill_Recovery
				|| Skill == Skill_Berserker
				|| Skill == Skill_Berserker_Strengthener
				|| Skill == Skill_Berserker_Proficiency
				|| Skill == Skill_Berserker_Mastery
				|| Skill == Skill_Defense_Increase_Strengthener
				|| Skill == Skill_Attack_Increase_Strengthener
				|| Skill == Skill_Soul_Barrier_Strengthener
				|| Skill == Skill_Heal_Strengthener
				|| Skill == Skill_Swell_Life_Strengthener
				|| Skill == Skill_Swell_Life_Proficiency
				|| Skill == Skill_Expansion_of_Wizardry_Power_Up
				|| Skill == Skill_Expansion_of_Wizardry_Mastery
				|| Skill == Skill_Soul_Barrier_Proficiency
				|| Skill == Skill_Attack_Increase_Mastery
				|| Skill == Skill_Defense_Increase_Mastery
				|| Skill == Skill_Infinity_Arrow_Strengthener
				|| Skill == Skill_Improve_ag
				|| Skill == Skill_Increase_Critical_Damage
				|| Skill == Skill_Critical_DMG_Increase_PowUp1
				|| Skill == Skill_Critical_DMG_Increase_PowUp2
				|| Skill == Skill_Critical_DMG_Increase_PowUp3
				|| Skill == Skill_Summon
				|| Skill >= Skill_Stun && Skill <= Skill_Abolish_Magic
				|| Skill == Skill_Plasma_Storm
				|| Skill >= Skill_Ignore_Defense && Skill <= Skill_Increase_Block
				|| Skill == Skill_Defense_Success_Rate_Increase_P
				|| Skill == Skill_DefSuccessRate_Increase_Mastery
				|| Skill == 575)
				)
			{
				if (g_pOption->IsAutoAttack() && gMapManager->currentMap != WD_6STADIUM && gMapManager->InChaosCastle() == false)
				{
					if (ClassIndex != CLASS_ELF
						|| Skill == Skill_Triple_Shot
						|| Skill == Skill_Multi_Shot
						|| Skill == Skill_Triple_Shot_Strengthener
						|| Skill == AT_SKILL_BOW
						|| Skill == Skill_Penetration
						|| Skill == AT_SKILL_BLAST_CROSSBOW4
						|| Skill == Skill_Plasma_Storm
						|| Skill == Skill_Penetration_Strengthener
						|| Skill == Skill_Triple_Shot_Mastery)
					{
						if (ClassIndex != CLASS_KNIGHT || Skill != AT_SKILL_VITALITY && Skill != Skill_Swell_Life_Strengthener && Skill != Skill_Swell_Life_Proficiency)
						{
							if (ClassIndex != CLASS_DARK_LORD || Skill != AT_SKILL_ADD_CRITICAL && Skill != AT_SKILL_PARTY_TELEPORT && Skill != Skill_Critical_DMG_Increase_PowUp1 && Skill != Skill_Critical_DMG_Increase_PowUp2 && Skill != Skill_Critical_DMG_Increase_PowUp3)
							{
								if (ClassIndex || Skill != Skill_Nova_begin && Skill != Skill_Nova)
								{
									if (Skill < Skill_Stun || Skill > Skill_Abolish_Magic)
									{
										if (Skill == Skill_Brand)
										{
											Attacking = -1;
										}
										else if (Skill != Skill_Damage_Reflection
											&& Skill != Skill_Sleep_Strengthener
											&& Skill != Skill_Berserker
											&& Skill != Skill_Sleep
											&& Skill != Skill_Berserker_Strengthener
											&& Skill != Skill_Berserker_Proficiency
											&& Skill != Skill_Berserker_Mastery
											&& Skill != AT_SKILL_ALICE_BLIND
											&& Skill != Skill_Weakness
											&& Skill != Skill_Innovation)
										{
											if ((Skill < Skill_Ignore_Defense || Skill > Skill_Increase_Block)
												&& Skill != Skill_Defense_Success_Rate_Increase_P
												&& Skill != Skill_DefSuccessRate_Increase_Mastery && Skill != 575)
												Attacking = 2;
											else
												Attacking = -1;
										}
										else
										{
											Attacking = -1;
										}
									}
									else
									{
										Attacking = -1;
									}
								}
								else
								{
									Attacking = -1;
								}
							}
							else
							{
								Attacking = -1;
							}
						}
						else
						{
							Attacking = -1;
						}
					}
					else
					{
						Attacking = -1;
					}
				}
				if (o->Type == MODEL_PLAYER)
				{
					if (o->CurrentAction < PLAYER_STOP_MALE
						|| o->CurrentAction > PLAYER_STOP_RIDE_WEAPON
						&& o->CurrentAction != PLAYER_STOP_TWO_HAND_SWORD_TWO
						&& o->CurrentAction != PLAYER_SKILL_HELL_BEGIN
						&& o->CurrentAction != PLAYER_DARKLORD_STAND
						&& o->CurrentAction != PLAYER_STOP_RIDE_HORSE
						&& o->CurrentAction != PLAYER_FENRIR_STAND
						&& o->CurrentAction != PLAYER_FENRIR_STAND_WEAPON_TWO
						&& o->CurrentAction != PLAYER_FENRIR_STAND_WEAPON_RIGHT
						&& o->CurrentAction != PLAYER_FENRIR_STAND_WEAPON_LEFT
						&& !(o->CurrentAction >= PLAYER_RAGE_FENRIR_STAND && o->CurrentAction <= PLAYER_RAGE_FENRIR_STAND_ONE_LEFT)
						&& o->CurrentAction != PLAYER_RAGE_UNI_STOP_ONE_RIGHT
						&& o->CurrentAction != PLAYER_STOP_RAGEFIGHTER
						&& o->CurrentAction != PLAYER_STOP_FLY_BOW
						)
					{
						MouseRButtonPress = 0;
						return;
					}
				}
				else
				{
					if (o->CurrentAction < MONSTER01_STOP1 || o->CurrentAction > MONSTER01_STOP2)
					{
						return;
					}
				}

				if (ClassIndex && Skill != Skill_Phoenix_Shot)
				{
					CheckTarget(c);

					if (CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY))
					{
						for (int i = EQUIPMENT_WEAPON_RIGHT; i <= EQUIPMENT_WEAPON_LEFT; i++)
						{
							if (ClassIndex == CLASS_KNIGHT || ClassIndex == CLASS_DARK || ClassIndex == CLASS_DARK_LORD || ClassIndex == CLASS_RAGEFIGHTER)
							{
								bool bOk = false;
								if (//c->Helper.Type == MODEL_HELPER + 2
									//|| c->Helper.Type == MODEL_HELPER + 3
									//|| c->Helper.Type == MODEL_HELPER + 4
									//|| c->Helper.Type == MODEL_HELPER + 37
									c->Helper.Type == MODEL_HELPER + 4
									|| (c->Helper.Type == MODEL_HELPER + 37 && Skill == Skill_Plasma_Storm)
									)
								{
									if (Skill > Skill_Chaotic_Diseier)
									{
										switch (Skill)
										{
										case Skill_Death_Stab_Strengthener:
										case Skill_Fire_Burst_Strengthener:
										case Skill_Force_Wave_Strengthener:
										case Skill_Dark_Horse_Strengthener:
										case Skill_Earthshake_Strengthener:
										case Skill_Fire_Burst_Mastery:
										case Skill_Earthshake_Mastery:
										case Skill_Fire_Scream_Strengthener:
										case Skill_Electric_Spark_Strengthener:
										case Skill_Chaotic_Diseier_Strengthener:
											goto LABEL_203;
										default:
											break;
										}
									}
									else if (Skill == Skill_Chaotic_Diseier)
									{
									LABEL_203:
										bOk = true;
									}
									else
									{
										switch (Skill)
										{
										case Skill_Death_Stab:
										case Skill_Impale:
										case Skill_Fire_Breath:
										case Skill_Force:
										case Skill_Fire_Burst:
										case Skill_Earthshake:
										case Skill_Electric_Spike:
										case Skill_Mana_Rays:
										case Skill_Fire_Blast:
										case Skill_Plasma_Storm:
										case Skill_Fire_Scream:
											goto LABEL_203;
										default:
											break;
										}
									}
								}
								else
								{
									bOk = true;
								}

								if (bOk)
								{
									g_MovementSkill.m_bMagic = TRUE;
									g_MovementSkill.m_iSkill = Hero->CurrentSkill;

									if (CheckAttack())
									{
										g_MovementSkill.m_iTarget = SelectedCharacter;
									}
									else
									{
										g_MovementSkill.m_iTarget = -1;
									}

									if (SkillWarrior(c, &CharacterMachine->Equipment[i]))
										return;
								}
							}
							if (ClassIndex == CLASS_ELF && SkillElf(c, &CharacterMachine->Equipment[i]))
								return;
						}
					}
					else
					{
						ZeroMemory(&g_MovementSkill, sizeof(g_MovementSkill));
						g_MovementSkill.m_iTarget = -1;
					}
				}


				if (Skill == Skill_Dark_Horse_Strengthener
					|| Skill == Skill_Earthshake_Strengthener
					|| Skill == Skill_Earthshake_Mastery
					|| Skill == Skill_Earthshake
					)
				{
					AttackKnight(c, Skill, Distance);
				}
				else
				{
					if (ClassIndex == CLASS_ELF)
					{
						AttackElf(c, Skill, Distance);
					}
					if (ClassIndex == CLASS_KNIGHT || ClassIndex == CLASS_DARK || ClassIndex == CLASS_DARK_LORD)
					{
						AttackKnight(c, Skill, Distance);
					}
					if (ClassIndex == CLASS_RAGEFIGHTER)
					{
						AttackRagefighter(c, Skill, Distance);
					}
					if (ClassIndex == CLASS_WIZARD || ClassIndex == CLASS_DARK || ClassIndex == CLASS_SUMMONER)
					{
						AttackWizard(c, Skill, Distance);
					}
					if ((Skill >= Skill_Stun && Skill <= Skill_Abolish_Magic))
					{
						AttackCommon(c, Skill, Distance);
					}
				}
			}
		}
	}
}


BOOL g_bWhileMovingZone = FALSE;
DWORD g_dwLatestZoneMoving = 0;

void CheckGate()
{
	if ((g_pMyInventory->IsItem(ITEM_POTION + 64, true)) || (gMapManager->IsCursedTemple() && g_pMyInventory->IsItem(ITEM_POTION + 64, false)))
	{
		return;
	}


	ITEM* pEquipedWing = &CharacterMachine->Equipment[EQUIPMENT_WING];
	BYTE Kind1 = GMItemMng->GetKind1(pEquipedWing->Type);
	BYTE Kind2 = GMItemMng->GetKind2(pEquipedWing->Type);

	for (int i = 0; i < MAX_GATES; i++)
	{
		GATE_ATTRIBUTE* gs = &GateAttribute[i];
		if (gs->Flag == 1 && gs->Map == gMapManager->currentMap)
		{
			if ((Hero->PositionX) >= gs->x1 && (Hero->PositionY) >= gs->y1 &&
				(Hero->PositionX) <= gs->x2 && (Hero->PositionY) <= gs->y2)
			{
				GATE_ATTRIBUTE* gt = &GateAttribute[gs->Target];

				if ((LoadingWorld) == 0 && Hero->JumpTime == 0)
				{
					bool Success = false;
					int Level;

					if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK || gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD
						|| gCharacterManager.GetBaseClass(Hero->Class) == CLASS_RAGEFIGHTER
						)
						Level = gs->Level * 2 / 3;
					else
						Level = gs->Level;

					if (i == 28)
					{
						if (CharacterAttribute->Level >= Level)
							Success = true;
					}
					else
					{
						Success = true;
					}


					if (Success)
					{
						if (((i >= 45 && i <= 49) || (i >= 55 && i <= 56)) &&
							((CharacterMachine->Equipment[EQUIPMENT_HELPER].Type >= ITEM_HELPER + 2 && CharacterMachine->Equipment[EQUIPMENT_HELPER].Type <= ITEM_HELPER + 3)))
						{
							g_pChatListBox->AddText("", GlobalText[261], SEASON3B::TYPE_ERROR_MESSAGE);
						}
						else if ((62 <= i && i <= 65) &&
							!(Kind1 == ItemKind1::WINGS
								|| Kind2 == ItemKind2::MINI_WINGS
								|| Kind2 == ItemKind2::WINGS_LVL_1
								|| Kind2 == ItemKind2::WINGS_LVL_2
								|| Kind2 == ItemKind2::WINGS_LVL_3
								|| Kind2 == ItemKind2::LORD_CAPE
								|| Kind2 == ItemKind2::CLOAK_LVL_2
								|| Kind2 == ItemKind2::MONSTER_WINGS
								|| Kind2 == ItemKind2::WINGS_CONQUERORS
								|| Kind2 == ItemKind2::WINGS_ANGEL_AND_DEVIL
								|| Kind2 == ItemKind2::WINGS_LVL_4
								|| Kind2 == ItemKind2::WINGS_POWER
								))
						{
							g_pChatListBox->AddText("", GlobalText[263], SEASON3B::TYPE_ERROR_MESSAGE);

							if (CharacterAttribute->Level < Level)
							{
								char Text[100];
								sprintf(Text, GlobalText[350], Level);
								g_pChatListBox->AddText("", Text, SEASON3B::TYPE_ERROR_MESSAGE);
							}
						}

						else if ((62 <= i && i <= 65) && (CharacterMachine->Equipment[EQUIPMENT_HELPER].Type == ITEM_HELPER + 2))
						{
							g_pChatListBox->AddText("", GlobalText[569], SEASON3B::TYPE_ERROR_MESSAGE);
						}
						else if (CharacterAttribute->Level < Level)
						{
							LoadingWorld = 50;
							char Text[100];
							sprintf(Text, GlobalText[350], Level);
							g_pChatListBox->AddText("", Text, SEASON3B::TYPE_ERROR_MESSAGE);
							//							return;
						}
						else
						{
							bool bResult;
							if ((LoadingWorld) <= 30)
							{
								SendRequestMagicTeleport(&bResult, i, 0, 0);
							}

							if (!bResult)
							{
								g_bWhileMovingZone = FALSE;
							}
							else
							{
								LoadingWorld = 9999999;

								if (gt->Map == WD_30BATTLECASTLE || gt->Map == WD_31HUNTING_GROUND)
								{
									SaveOptions();
									SaveMacro("Data\\Macro.txt");
								}

								SelectedItem = -1;
								SelectedNpc = -1;
								SelectedCharacter = -1;
								SelectedOperate = -1;
								Attacking = -1;

								if ((gs->Map == WD_0LORENCIA && gt->Map == WD_30BATTLECASTLE) || (gs->Map == WD_30BATTLECASTLE && gt->Map == WD_0LORENCIA))
								{
									g_dwLatestZoneMoving = GetTickCount();
									g_bWhileMovingZone = FALSE;
								}
								else
								{
									g_bWhileMovingZone = TRUE;
								}

								StandTime = 0;
							}
						}
					}
				}
			}
		}
	}
}

void MoveEffect(OBJECT* o, int iIndex);

void MoveHero()
{
	int Angle;
	CHARACTER* c = Hero;
	OBJECT* o = &c->Object;

	if (o->CurrentAction == PLAYER_CHANGE_UP)
		return;

	if (g_Direction.IsDirection())
	{
		SetPlayerStop(c);
		return;
	}

	if (SelectedCharacter != -1 && g_isCharacterBuff((&Hero->Object), eBuff_CrywolfHeroContracted))
	{
		return;
	}

	if (c->Dead > 0 || g_isCharacterBuff(o, eDeBuff_Stun) || g_isCharacterBuff(o, eDeBuff_Sleep) || !o->Live)
	{
		return;
	}

	if (LoadingWorld > 0 && (--LoadingWorld == 9999998 || LoadingWorld == 29))
		SetPlayerStop(c);

	if (LoadingWorld <= 30)
	{
		if (g_pWindowMgr->GetAddFriendWindow() > 0)
		{
			if (MouseRButtonPush)
			{
				if (!::IsStrifeMap(gMapManager->currentMap))
				{
					CUITextInputWindow* pWindow = (CUITextInputWindow*)g_pWindowMgr->GetWindow(g_pWindowMgr->GetAddFriendWindow());
					CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

					if (pWindow != NULL && pCharacter)
					{
						pWindow->SetText(pCharacter->ID);
					}
				}
			}
		}

		int HeroX = GetScreenWidth() / 2;
		int HeroY = 180;

		bool bLookAtMouse = true;
		bool NoAutoAttacking = false;

		if (g_isCharacterBuff(o, eDeBuff_Stun) || g_isCharacterBuff(o, eDeBuff_Sleep) || o->CurrentAction == PLAYER_POINT_DANCE || o->CurrentAction == PLAYER_SKILL_GIANTSWING)
		{
			Angle = (int)o->Angle[2];
			bLookAtMouse = false;
		}
		else
		{
			float vAngle = CreateAngle((float)HeroX, (float)HeroY, (float)MouseX, (float)MouseY);
			Angle = ((int)(o->Angle[2] + vAngle) + 315) % 360;

			if (Angle < 120)
				Angle = 120;

			if (Angle > 240)
				Angle = 240;

			Angle = (Angle + 180) % 360;
		}

		o->HeadTargetAngle[2] = 0.f;

		if ((g_pOption->IsAutoAttack()
			&& Attacking != -1
			&& gMapManager->currentMap != WD_6STADIUM
			&& gMapManager->InChaosCastle() == false)
			|| o->CurrentAction == PLAYER_HIT_SKILL_FURY_STRIKE
			|| o->CurrentAction == PLAYER_SKILL_GIANTSWING
			)
		{
			bLookAtMouse = false;
		}

		if (bLookAtMouse)
		{
			int mousePosY = MouseY;

			if (mousePosY > gwinhandle->GetScreenY())
			{
				mousePosY = gwinhandle->GetScreenY();
			}
			o->HeadTargetAngle[0] = (float)Angle;
			o->HeadTargetAngle[1] = (HeroY - mousePosY) * 0.05f;

			NoAutoAttacking = true;
		}
		else
		{
			o->HeadTargetAngle[0] = 0;
			o->HeadTargetAngle[1] = 0;
		}

		bool stuck_movement = false;

		if (g_pNewUIMiniMap && g_pNewUIMiniMap->movement_automatic())
		{
			stuck_movement = true;
		}

		if (c->Movement)
		{
			if (g_isCharacterBuff(o, eDeBuff_Harden) || g_isCharacterBuff(o, eDeBuff_CursedTempleRestraint))
			{
				SetPlayerStop(c);
			}
			else
			{
				SetPlayerWalk(c);

				if (MovePath(c))
				{
					c->Movement = false;

					if (stuck_movement)
					{
						SetPlayerStop(c);
					}

					HeroAngle = (int)o->Angle[2];
					StandTime = 0;

					if (c->MovementType == MOVEMENT_OPERATE)
					{
						Action(c, o, false);
					}
					else
					{
						if (CheckArrow() || gCharacterManager.GetBaseClass(Hero->Class) != CLASS_ELF)
							Action(c, o, false);
						else
							SetPlayerStop(Hero);
					}
				}
				else
				{
					g_CharacterUnRegisterBuff(o, eBuff_CrywolfHeroContracted);
					MoveCharacterPosition(c);
				}
			}
		}
		else
		{
			if (StandTime++ >= ((int)standlimit(40)) && !MouseOnWindow && Hero->Dead == 0
				&& o->CurrentAction != PLAYER_POSE1
				&& o->CurrentAction != PLAYER_POSE_FEMALE1
				&& o->CurrentAction != PLAYER_SIT1
				&& o->CurrentAction != PLAYER_SIT_FEMALE1)
			{
				if (NoAutoAttacking)
				{
					if (o->CurrentAction != PLAYER_HIT_TELEPORT
						&& o->CurrentAction != PLAYER_HIT_RIDE_TELEPORT
						&& o->CurrentAction != PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT
						&& o->CurrentAction != PLAYER_SKILL_ATT_UP_OURFORCES
						&& o->CurrentAction != PLAYER_SKILL_HP_UP_OURFORCES
						&& Hero->AttackTime == (0))
					{
						StandTime = 0;
						HeroAngle = -(int)(CreateAngle((float)MouseX, (float)MouseY, (float)HeroX, (float)HeroY)) + 360 + 45;
						HeroAngle %= 360;
						BYTE Angle1 = ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8);
						BYTE Angle2 = ((BYTE)(((float)HeroAngle + 22.5f) / 360.f * 8.f + 1.f) % 8);
						if (Angle1 != Angle2)
						{
							if (o->CurrentAction != PLAYER_HIT_SKILL_SWORD2)
							{
								Hero->Object.Angle[2] = (float)HeroAngle;
							}
							SendRequestAction(AT_STAND1, ((BYTE)((HeroAngle + 22.5f) / 360.f * 8.f + 1.f) % 8));
						}
					}
				}
			}
			UseSkillRagePosition(c);
		}
		CheckGate();

		if (!MouseOnWindow && !g_pNewUISystem->CheckMouseUse())
		{
			bool Success = false;

			if (MouseUpdateTime >= MouseUpdateTimeMax && !EnableFastInput)
			{
				if (MouseLButtonPush)
				{
					MouseLButtonPush = false;
					Success = true;
				}
				if (MouseLButton)
					Success = true;

				if (g_pOption->IsAutoAttack()
					&& gMapManager->currentMap != WD_6STADIUM
					&& !gMapManager->InChaosCastle()
					&& Attacking == 1
					&& SelectedCharacter != -1)
				{
					Success = true;
				}

				if (!Success || g_isCharacterBuff(o, eDeBuff_Stun) || g_isCharacterBuff(o, eDeBuff_Sleep))
				{
					LButtonPopTime = WorldTime;
					LButtonPressTime = 0.f;
				}
				else
				{
					g_iFollowCharacter = -1;
					LButtonPressTime = ((WorldTime - LButtonPopTime) / CLOCKS_PER_SEC);
					if (LButtonPressTime >= AutoMouseLimitTime)
					{
						MouseLButtonPush = false;
						MouseLButton = false;
						Success = false;
					}
				}
			}

			CHARACTER* followCharacter = gmCharacters->GetCharacter(g_iFollowCharacter);

			if (followCharacter)
			{
				if (followCharacter->Object.Live)
				{
					c->MovementType = MOVEMENT_MOVE;
					ActionTarget = g_iFollowCharacter;
					g_pNewUIMiniMap->StopMove();
					TargetX = (int)(followCharacter->Object.Position[0] / TERRAIN_SCALE);
					TargetY = (int)(followCharacter->Object.Position[1] / TERRAIN_SCALE);

					if (PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
						SendMove(c, o);
				}
				else
				{
					g_iFollowCharacter = -1;
				}
			}
			else if (Success
				&& (o->CurrentAction != PLAYER_SHOCK
					&& (o->Teleport != TELEPORT_BEGIN
						&& o->Teleport != TELEPORT
						&& o->Alpha >= 0.7f)
					&& (o->CurrentAction < PLAYER_HIT_FIST || o->CurrentAction > PLAYER_RIDE_SKILL)
					&& (o->CurrentAction < PLAYER_SKILL_SLEEP || o->CurrentAction > PLAYER_SKILL_LIGHTNING_SHOCK)
					&& o->CurrentAction != PLAYER_RECOVER_SKILL
					&& (o->CurrentAction < PLAYER_SKILL_THRUST || o->CurrentAction>258)
					|| (o->CurrentAction >= PLAYER_STOP_TWO_HAND_SWORD_TWO && o->CurrentAction <= PLAYER_RUN_TWO_HAND_SWORD_TWO)
					|| (o->CurrentAction >= PLAYER_DARKLORD_STAND && o->CurrentAction <= PLAYER_RUN_RIDE_HORSE)
					|| (o->CurrentAction >= PLAYER_FENRIR_RUN && o->CurrentAction <= PLAYER_FENRIR_WALK_WEAPON_LEFT)
					|| (o->CurrentAction >= PLAYER_RAGE_FENRIR_WALK && o->CurrentAction <= PLAYER_RAGE_FENRIR_STAND_ONE_LEFT)))
			{
				int RightType = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;
				int LeftType = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;

				SEASON3B::CNewUIPickedItem* pPickedItem = SEASON3B::CNewUIInventoryCtrl::GetPickedItem();

				if (!pPickedItem && RightType == -1 &&
					((LeftType >= ITEM_SWORD && LeftType < ITEM_SPEAR) || (LeftType >= ITEM_STAFF && LeftType < ITEM_SHIELD)
						&& (LeftType < ITEM_STAFF + 21 || LeftType > ITEM_STAFF + 29)))
				{
					if (g_pMyInventory->IsEquipable(EQUIPMENT_WEAPON_LEFT, &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT]))
					{
						memcpy(&PickItem, &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT], sizeof(ITEM));
						CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type = -1;
						CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Level = 0;
						SetCharacterClass(Hero);
						SrcInventory = Inventory;
						SrcInventoryIndex = EQUIPMENT_WEAPON_LEFT;
						DstInventoryIndex = EQUIPMENT_WEAPON_RIGHT;
						SendRequestEquipmentItem(0, SrcInventoryIndex, 0, DstInventoryIndex);
					}
				}
				MouseUpdateTime = 0;
				Success = false;

				if (!c->SafeZone)
					Success = CheckAttack();

				if (Success)
				{
					if (c->Movement && c->MovementType == MOVEMENT_MOVE && c->GetBaseClass() == CLASS_ELF
						&& gCharacterManager.GetEquipedBowType(&CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT]))
					{
						SendPosition((c->PositionX), (c->PositionY));
					}

					CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

					if (pCharacter)
					{
						Attacking = 1;
						c->MovementType = MOVEMENT_ATTACK;
						ActionTarget = SelectedCharacter;

						TargetX = (int)(pCharacter->Object.Position[0] / TERRAIN_SCALE);
						TargetY = (int)(pCharacter->Object.Position[1] / TERRAIN_SCALE);

						if (CheckWall((c->PositionX), (c->PositionY), TargetX, TargetY))
						{
							if (!PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
							{
								if (CheckArrow() == false)
								{
									return;
								}
								Action(c, o, true);
							}
							else
							{
								if ((gCharacterManager.GetEquipedBowType() != BOWTYPE_NONE) || (c->MonsterIndex == 9))
								{
									if (CheckArrow() == false)
									{
										return;
									}
									Action(c, o, true);
								}
								else
								{
									SendMove(c, o);
								}
							}
						}
					}
				}
				else if (SelectedOperate != -1 && (c->SafeZone || (c->Helper.Type < MODEL_HELPER + 2 || c->Helper.Type>MODEL_HELPER + 4 || c->Helper.Type != MODEL_HELPER + 37)))
				{
					TargetX = (int)(Operates[SelectedOperate].Owner->Position[0] / TERRAIN_SCALE);
					TargetY = (int)(Operates[SelectedOperate].Owner->Position[1] / TERRAIN_SCALE);
					int wall = TerrainWall[TERRAIN_INDEX(TargetX, TargetY)];

					if ((wall == TW_HEIGHT || wall < TW_CHARACTER) && !c->Movement)
					{
						c->MovementType = MOVEMENT_OPERATE;
						TargetType = Operates[SelectedOperate].Owner->Type;
						TargetAngle = Operates[SelectedOperate].Owner->Angle[2];
						if (PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
							SendMove(c, o);
						else
							Action(c, o, true);
					}
				}
				else if (SelectedNpc != -1 && !g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP) && !g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE))
				{
					if (g_isCharacterBuff(o, eBuff_CrywolfNPCHide) == false)
						c->MovementType = MOVEMENT_TALK;
					g_pNewUIMiniMap->StopMove();

					ActionTarget = SelectedNpc;

					CHARACTER* pCharacter = gmCharacters->GetCharacter(ActionTarget);

					if (pCharacter)
					{
						TargetX = (int)(pCharacter->Object.Position[0] / TERRAIN_SCALE);
						TargetY = (int)(pCharacter->Object.Position[1] / TERRAIN_SCALE);
						TargetNpc = ActionTarget;
						TargetType = pCharacter->Object.Type;
						TargetAngle = pCharacter->Object.Angle[2];

						if (TargetType == MODEL_KANTURU2ND_ENTER_NPC)
						{
							vec3_t vHero, vTarget, vSubstract;
							VectorCopy(o->Position, vHero);
							vHero[2] = 0.f;
							VectorCopy(pCharacter->Object.Position, vTarget);
							vTarget[2] = 0.f;
							VectorSubtract(vHero, vTarget, vSubstract);
							float fLength = VectorLength(vSubstract);
							if (fLength <= 550.f)
							{
								Action(c, o, true);
							}
							else
							{
								if (PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
									SendMove(c, o);
								else
									Action(c, o, true);
							}
						}
						else
						{
							if (PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
								SendMove(c, o);
							else
								Action(c, o, true);
						}
					}
				}
				else if (SelectedItem != -1)
				{
					g_pNewUIMiniMap->StopMove();
					c->MovementType = MOVEMENT_GET;
					ItemKey = SelectedItem;
					TargetX = (int)(Items[SelectedItem].Object.Position[0] / TERRAIN_SCALE);
					TargetY = (int)(Items[SelectedItem].Object.Position[1] / TERRAIN_SCALE);
					if (PathFinding2((c->PositionX), (c->PositionY), TargetX, TargetY, &c->Path))
						SendMove(c, o);
					else
					{
						Action(c, o, true);
						c->MovementType = MOVEMENT_MOVE;
					}
				}
				else if (!SEASON3B::IsRepeat(VK_SHIFT))
				{
					RenderTerrain(true);
					bool Success = RenderTerrainTile(SelectXF, SelectYF, (int)SelectXF, (int)SelectYF, 1.f, 1, true);

					if (Success && c->Object.Live)
					{
						TargetX = (BYTE)(CollisionPosition[0] / TERRAIN_SCALE);
						TargetY = (BYTE)(CollisionPosition[1] / TERRAIN_SCALE);

						WORD CurrAtt = TerrainWall[TargetY * 256 + TargetX];
						if (CurrAtt >= TW_NOGROUND && (CurrAtt & TW_ACTION) != TW_ACTION && (CurrAtt & TW_HEIGHT) != TW_HEIGHT)
							DontMove = true;
						else
							DontMove = false;

						int xPos = (int)(0.01f * o->Position[0]);
						int yPos = (int)(0.01f * o->Position[1]);

						g_pNewUIMiniMap->StopMove();

						if (!c->Movement || (abs((c->PositionX) - xPos) < 2 && abs((c->PositionY) - yPos) < 2))
						{
							if ((c->PositionX != TargetX || c->PositionY != TargetY || !c->Movement) && PathFinding2(c->PositionX, c->PositionY, TargetX, TargetY, &c->Path))
							{
								c->MovementType = MOVEMENT_MOVE;
								SendMove(c, o);

								vec3_t vLight, vPos;
								Vector(1.f, 1.f, 0.f, vLight);
								VectorCopy(CollisionPosition, vPos);
								DeleteEffect(MODEL_MOVE_TARGETPOSITION_EFFECT);

								int iTerrainIndex = TERRAIN_INDEX((int)SelectXF, (int)SelectYF);
								if ((TerrainWall[iTerrainIndex] & TW_NOMOVE) != TW_NOMOVE)
								{
									CreateEffect(MODEL_MOVE_TARGETPOSITION_EFFECT, vPos, o->Angle, vLight, 0, o, -1, 0, 0, 0, 0.6f);
								}
							}
							else
							{
								MouseUpdateTime = MouseUpdateTimeMax;
								MouseUpdateTime = 0;
							}
						}
					}
				}
			}
			MouseUpdateTime++;
		}

		Attack(Hero);
		int Index = ((int)Hero->Object.Position[1] / (int)TERRAIN_SCALE) * 256 + ((int)Hero->Object.Position[0] / (int)TERRAIN_SCALE);
		if (Index < 0)
			Index = 0;
		else if (Index > 65535)
			Index = 65535;
		HeroTile = TerrainMappingLayer1[Index];
	}
}

int SelectItem()
{
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		OBJECT* o = &Items[i].Object;
		if (o->Live && o->Visible)
		{
			o->LightEnable = true;
			Vector(0.2f, 0.2f, 0.2f, o->Light);
		}
	}
	float Luminosity = 1.5f;

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		OBJECT* o = &Items[i].Object;
		if (o->Live && o->Visible)
		{
			if (CollisionDetectLineToOBB(MousePosition, MouseTarget, o->OBB))
			{
				{
					o->LightEnable = false;
					Vector(Luminosity, Luminosity, Luminosity, o->Light);
					return i;
				}
			}
		}
	}
	return -1;
}

int SelectCharacter(BYTE Kind)
{
	bool Main = true;
	if (SceneFlag == CHARACTER_SCENE)
		Main = false;

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		OBJECT* o = &c->Object;

		if ((Kind & o->Kind) == o->Kind && o->Live && o->Visible && o->Alpha > 0.f)
		{
			o->LightEnable = true;
			switch (c->Level)
			{
			case 0:Vector(0.2f, 0.2f, 0.2f, o->Light); break;
			case 1:Vector(-0.4f, -0.4f, -0.4f, o->Light); break;
			case 2:Vector(0.2f, -0.6f, -0.6f, o->Light); break;
			case 3:Vector(1.5f, 1.5f, 1.5f, o->Light); break;
			case 4:Vector(0.3f, 0.2f, -0.5f, o->Light); break;
			}
			if (c->PK >= PVP_MURDERER2)
			{
				Vector(-0.4f, -0.4f, -0.4f, o->Light);
			}
		}
	}
	vec3_t Light;
	Vector(0.8f, 0.8f, 0.8f, Light);
	int iSelected = -1;
	float fNearestDist = 1000000000000.0f;

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		OBJECT* o = &c->Object;

		if (o->Live && o->Visible && o->Alpha > 0.f && c->Dead == 0 && !g_isCharacterBuff(o, eBuff_CrywolfNPCHide))
		{
			if (o->Kind == KIND_PLAYER)
			{
				for (int j = 0; j < PartyNumber; ++j)
				{
					PARTY_t* p = &Party[j];

					if (p->index != -2)
						continue;
					if (p->index > -1)
						continue;

					int length = max(strlen(p->Name), max(1, strlen(c->ID)));

					if (!strncmp(p->Name, c->ID, length))
					{
						p->index = i;
						break;
					}
				}
			}

			if (Main && c == Hero)
			{
				continue;
			}

			if (c->m_bIsSelected == false)
			{
				continue;
			}

			if ((Kind & o->Kind) == o->Kind)
			{
				if (o->Type == MODEL_SMELTING_NPC)
				{
					o->OBB.StartPos[2] += 300.0f;
				}
				else if (o->Type == MODEL_MONSTER01 + 118 || o->Type == MODEL_MONSTER01 + 119)
				{
					o->OBB.StartPos[2] += 200.0f;
				}
				else if (o->Type == MODEL_KANTURU2ND_ENTER_NPC)
				{
					o->OBB.StartPos[0] -= 100.f;
					o->OBB.StartPos[2] += 100.0f;
					o->OBB.XAxis[0] += 100.f;
					o->OBB.ZAxis[2] += 100.f;
				}

				if (CollisionDetectLineToOBB(MousePosition, MouseTarget, o->OBB))
				{
					vec3_t vSub;
					VectorSubtract(o->Position, CameraPosition, vSub);

					float fNewDist = DotProduct(vSub, vSub);

					if (fNewDist < fNearestDist)
					{
						BOOL bCanTalk = TRUE;
						if (gMapManager->currentMap == WD_0LORENCIA || gMapManager->currentMap == WD_2DEVIAS)
						{
							int Index = ((int)o->Position[1] / (int)TERRAIN_SCALE) * 256 + ((int)o->Position[0] / (int)TERRAIN_SCALE);
							if ((gMapManager->currentMap == WD_0LORENCIA && TerrainMappingLayer1[Index] == 4) ||
								(gMapManager->currentMap == WD_2DEVIAS && TerrainMappingLayer1[Index] == 3))
							{
								if (TerrainMappingLayer1[Index] != HeroTile && (gMapManager->currentMap == WD_2DEVIAS && HeroTile != 11))
									bCanTalk = FALSE;
							}
						}
						if (bCanTalk == TRUE)
						{
							iSelected = i;
							fNearestDist = fNewDist;
						}
					}
				}
			}
		}
	}

	for (int j = 0; j < PartyNumber; ++j)
	{
		PARTY_t* p = &Party[j];

		if (p->index >= 0) continue;

		int length = max(strlen(p->Name), max(1, strlen(Hero->ID)));

		if (!strncmp(p->Name, Hero->ID, length))
		{
			p->index = -3;
		}
		else
		{
			p->index = -1;
		}
	}

	return iSelected;
}

int SelectOperate()
{
	for (int i = 0; i < MAX_OPERATES; i++)
	{
		OPERATE* n = &Operates[i];
		OBJECT* o = n->Owner;
		if (n->Live && o->Visible && o->HiddenMesh == -1)
		{
			float* Light = &o->Light[0];
			Vector(0.2f, 0.2f, 0.2f, Light);
		}
	}
	if (IsBattleCastleStart() && gMapManager->currentMap == WD_30BATTLECASTLE)
		return -1;

	for (int i = 0; i < MAX_OPERATES; i++)
	{
		OPERATE* n = &Operates[i];
		OBJECT* o = n->Owner;
		if (n->Live && o->Visible)
		{
			float* Light = &o->Light[0];
			if (CollisionDetectLineToOBB(MousePosition, MouseTarget, o->OBB))
			{
				Vector(1.5f, 1.5f, 1.5f, Light);
				return i;
			}
		}
	}
	return -1;
}

void SelectObjects()
{
	BYTE CKind_1, CKind_2;

	if (!g_pOption->IsAutoAttack() || World == WD_6STADIUM || gMapManager->InChaosCastle())
	{
		SelectedCharacter = -1;
		Attacking = -1;
	}
	else
	{
		CHARACTER* sc = gmCharacters->GetCharacter(SelectedCharacter);

		if (sc)
		{
			if ((signed int)sc->Dead > 0 || sc->GetKind() != KIND_MONSTER)
			{
				SelectedCharacter = -1;
				Attacking = -1;
			}
			if (Attacking == -1)
			{
				SelectedCharacter = -1;
			}
			else if (MouseLButton
				|| MouseLButtonPush
				|| MouseRButton
				|| MouseRButtonPush
				|| (signed int)Hero->Dead > 0)
			{
				SelectedCharacter = -1;
			}
		}
		else
		{
			SelectedCharacter = -1;
			Attacking = -1;
		}
	}

	SelectedItem = -1;
	SelectedNpc = -1;
	SelectedOperate = -1;

	if (MouseOnWindow || g_pNewUISystem->CheckMouseUse() || !SEASON3B::CheckMouseIn(0, 0, GetScreenWidth(), pos_botton(429)))
	{
		g_pPartyManager->SearchPartyMember();
	}
	else if (SEASON3B::IsRepeat(VK_MENU))
	{
		if (!SEASON3B::CNewUIInventoryCtrl::GetPickedItem())
			SelectedItem = SelectItem();

		if (SelectedItem == -1)
		{
			SelectedNpc = SelectCharacter(KIND_NPC);
			if (SelectedNpc == -1)
			{
				SelectedCharacter = SelectCharacter(KIND_MONSTER | KIND_EDIT);
				if (SelectedCharacter == -1)
				{
					SelectedCharacter = SelectCharacter(KIND_PLAYER);
					if (SelectedCharacter == -1)
						SelectedOperate = SelectOperate();
				}
			}
		}
		else
		{
			g_pPartyManager->SearchPartyMember();
		}
	}
	else
	{
		CKind_1 = KIND_MONSTER | KIND_EDIT;
		CKind_2 = KIND_PLAYER;

		if (Hero->GetBaseClass() == CLASS_ELF || Hero->GetBaseClass() == CLASS_WIZARD)
		{
			int Skill = CharacterAttribute->Skill[Hero->CurrentSkill];

			if (((Skill >= AT_SKILL_HEALING) && (Skill <= AT_SKILL_ATTACK))
				|| Skill == Skill_Cure
				|| Skill == Skill_Party_Healing
				|| Skill == Skill_Party_Healing_Strengthener
				|| Skill == Skill_Heal_Strengthener
				|| Skill == Skill_Attack_Increase_Strengthener
				|| Skill == Skill_Defense_Increase_Strengthener
				|| Skill == Skill_Summoned_Monster_Power_Up1
				|| Skill == Skill_Summoned_Monster_Power_Up2
				|| Skill == Skill_Summoned_Monster_Power_Up3
				|| Skill == Skill_Bless
				|| Skill == Skill_Bless_Strengthener
				|| Skill == AT_SKILL_TELEPORT_B
				|| Skill == Skill_Soul_Barrier
				|| Skill == Skill_Soul_Barrier_Strengthener
				|| Skill == Skill_Soul_Barrier_Proficiency
				|| Skill == Skill_Soul_Barrier_Mastery
				|| Skill == Skill_Attack_Increase_Mastery
				|| Skill == Skill_Defense_Increase_Mastery
				)
			{
				CKind_1 = KIND_PLAYER;
				CKind_2 = KIND_MONSTER | KIND_EDIT;
			}
		}

		if (g_pPartyListWindow && g_pPartyListWindow->GetSelectedCharacter() != -1)
		{
			g_pPartyManager->SearchPartyMember();
		}
		else
		{
			if (SelectedCharacter == -1)
			{
				SelectedCharacter = SelectCharacter(CKind_1);
			}
			if (SelectedCharacter == -1)
			{
				SelectedCharacter = SelectCharacter(CKind_2);
				if (SelectedCharacter == -1)
				{
					SelectedNpc = SelectCharacter(KIND_NPC);
					if (SelectedNpc == -1)
					{
						if (SEASON3B::CNewUIInventoryCtrl::GetPickedItem() == NULL)
						{
							SelectedItem = SelectItem();
						}
						if (SelectedItem == -1)
						{
							SelectedOperate = SelectOperate();
						}
					}
				}
			}
			else if (Attacking != -1)
			{
				g_pPartyManager->SearchPartyMember();
			}
		}
	}

	if (SelectedCharacter == -1)
	{
		Attacking = -1;
	}

	//if (g_pPartyListWindow)
	//{
		//g_pPartyListWindow->SetListBGColor();
	//}
}

int FindHotKey(int Skill)
{
	int SkillIndex = 0;

	for (int i = 0; i < MAX_MAGIC; i++)
	{
		if (CharacterAttribute->Skill[i] == Skill)
		{
			SkillIndex = i;
			break;
		}
	}

	return SkillIndex;
}

void SendMacroChat(char* Text)
{
	if (!CheckCommand(Text, true))
	{
		if ((Hero->GetMovement() == Movement::Dinorant) || Hero->SafeZone)
			CheckChatText(Text);
		if (CheckAbuseFilter(Text))
			SendChat(GlobalText[570]);
		else
			SendChat(Text);
#ifdef _DEBUG
		MacroTime = 10;
#else
		MacroTime = standlimit(100);
#endif
	}
	}

void MoveInterface()
{
	if (g_Direction.IsDirection())
	{
		return;
	}

	if (battleCastle::IsBattleCastleStart() == true)
	{
		if (CharacterAttribute->SkillNumber > 0)
		{
			UseBattleMasterSkill();
		}
	}

	//int x, y, Width, Height;

	if (MacroTime == 0)
	{
		if (gMapManager->InChaosCastle() == false)
		{
			if (SEASON3B::IsRepeat(VK_MENU))
			{
				for (int i = 0; i < 9; i++)
				{
					if (SEASON3B::IsRepeat('1' + i))
					{
						SendMacroChat(MacroText[i]);
					}
				}
				if (SEASON3B::IsRepeat('0'))
				{
					SendMacroChat(MacroText[9]);
				}
			}
		}
	}

	if (Hero->Dead == 0)
	{
		g_pMainFrame->UseHotKeyItemRButton();
	}

	/*if (g_pUIManager->IsInputEnable())
	{
		if (g_iChatInputType == 0)
		{
			x = 186; y = 415;
			Width = 190; Height = 29;

			if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
			{
				if (MouseLButtonPush)
				{
					MouseLButtonPush = false;
					InputIndex = 0;
					MouseUpdateTime = 0;
					MouseUpdateTimeMax = 6;
					PlayBuffer(SOUND_CLICK01);
				}
			}
			x = 186 + 190; y = 415;
			Width = 58; Height = 29;
			if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
			{
				if (MouseLButtonPush)
				{
					MouseLButtonPush = false;
					InputIndex = 1;
					MouseUpdateTime = 0;
					MouseUpdateTimeMax = 6;
					PlayBuffer(SOUND_CLICK01);
				}
			}
		}
		Width = 20; Height = 29; x = 186 + 190 + 58; y = 415;
		if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
		{
			if (MouseLButtonPush)
			{
				MouseLButtonPush = false;
				if (WhisperEnable)
					WhisperEnable = false;
				else
					WhisperEnable = true;
				MouseUpdateTime = 0;
				MouseUpdateTimeMax = 6;
				PlayBuffer(SOUND_CLICK01);
			}
		}
	}*/
}

bool IsCanBCSkill(int Type)
{
	if (!GMProtect->CastleSkillEnable()
		&& (Type == Skill_Crescent_Moon_Slash
			|| Type == Skill_Lance
			|| Type == Skill_Starfall
			|| Type == Skill_Spiral_Slash
			|| Type == Skill_Mana_Rays
			|| Type == Skill_Fire_Blast
			|| Type == Skill_Charge))
	{
		if (!gMapManager->InBattleCastle() || !battleCastle::IsBattleCastleStart())
		{
			return false;
		}
	}

	return true;
}

bool CheckSkillUseCondition(OBJECT* o, int Type)
{
	if (IsCanBCSkill(Type) == false)
	{
		return false;
	}

	return SkillAttribute[Type].SkillUseType != SKILL_USE_TYPE_BRAND || g_isCharacterBuff(o, eBuff_AddSkill);
}

void GetTime(DWORD time, std::string& timeText, bool isSecond)
{
	char buff[100];

	if (isSecond)
	{
		DWORD day = time / (1440 * 60);
		DWORD oClock = (time - (day * (1440 * 60))) / 3600;
		DWORD minutes = (time - ((oClock * 3600) + (day * (1440 * 60)))) / 60;
		DWORD second = time % 60;

		if (day != 0)
		{
			sprintf(buff, "%d %s %d %s %d %s %d %s", day, GlobalText[2298], oClock, GlobalText[2299], minutes, GlobalText[2300], second, GlobalText[2301]);
			timeText = buff;
		}
		else if (day == 0 && oClock != 0)
		{
			sprintf(buff, "%d %s %d %s %d %s", oClock, GlobalText[2299], minutes, GlobalText[2300], second, GlobalText[2301]);
			timeText = buff;
		}
		else if (day == 0 && oClock == 0 && minutes != 0)
		{
			sprintf(buff, "%d %s %d %s", minutes, GlobalText[2300], second, GlobalText[2301]);
			timeText = buff;
		}
		else if (day == 0 && oClock == 0 && minutes == 0)
		{
			timeText = GlobalText[2308];
		}
	}
	else
	{
		DWORD day = time / 1440;
		DWORD oClock = (time - (day * 1440)) / 60;
		DWORD minutes = time % 60;

		if (day != 0)
		{
			sprintf(buff, "%d %s %d %s %d %s", day, GlobalText[2298], oClock, GlobalText[2299], minutes, GlobalText[2300]);
			timeText = buff;
		}
		else if (day == 0 && oClock != 0)
		{
			sprintf(buff, "%d %s %d %s", oClock, GlobalText[2299], minutes, GlobalText[2300]);
			timeText = buff;
		}
		else if (day == 0 && oClock == 0 && minutes != 0)
		{
			sprintf(buff, "%d %s", minutes, GlobalText[2300]);
			timeText = buff;
		}
	}
}

void RenderBar(float x, float y, float Width, float Height, float Bar, bool Disabled, bool clipping)
{
	if (clipping)
	{
		if (x < 0)
			x = 0;
		if (y < 0)
			y = 0;
		if (x + Width + 4 > gwinhandle->GetScreenX())
			x = gwinhandle->GetScreenX() - (Width + 1 + 4);
		if (y + Height + 4 > gwinhandle->GetScreenY() - 47)
			y = gwinhandle->GetScreenY() - 47 - (Height + 1 + 4);
	}

	EnableAlphaTest();
	glColor4f(0.f, 0.f, 0.f, 0.5f);
	RenderColor(x + 1, y + 1, Width + 4, Height + 4);

	EnableAlphaBlend();
	if (Disabled)
		glColor3f(0.2f, 0.0f, 0.0f);
	else
		glColor3f(0.f, 0.2f, 0.2f);
	RenderColor(x, y, Width + 4, Height + 4);
	if (Disabled)
		glColor3f(50.f / 255.f, 10 / 255.f, 0.f);
	else
		glColor3f(0.f / 255.f, 50 / 255.f, 50.f / 255.f);
	RenderColor(x + 2, y + 2, Width, Height);
	if (Disabled)
		glColor3f(200.f / 255.f, 50 / 255.f, 0.f);
	else
		glColor3f(0.f / 255.f, 200 / 255.f, 50.f / 255.f);
	RenderColor(x + 2, y + 2, Bar, Height);

	DisableAlphaBlend();
}

void RenderSwichState()
{
	char Buff[300];

	if (Switch_Info == NULL)
		return;

	int ax = (Hero->PositionX);
	int ay = (Hero->PositionY);

	if (!(ax >= 150 && ax <= 200 && ay >= 180 && ay <= 230))
	{
		Delete_Switch();
		return;
	}

	for (int i = 0; i < 2; i++)
	{
		if (Switch_Info[i].m_bySwitchState > 0)
		{
			sprintf(Buff, "%s%d / %s / %s", GlobalText[1981], i + 1, Switch_Info[i].m_szGuildName, Switch_Info[i].m_szUserName);
			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetTextColor(255, 255, 255, 255);
			g_pRenderText->SetBgColor(0);
			g_pRenderText->RenderText(0, gwinhandle->GetScreenY() - 85 + (i * 15), Buff, gwinhandle->GetScreenX(), 0, RT3_SORT_CENTER);
		}
	}
}

void RenderInterface(bool Render)
{
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	RenderOutSides();

	RenderPartyHP();

	RenderSwichState();
	battleCastle::RenderBuildTimes();

	g_pUIMapName->Render();		// rozy

	M39Kanturu3rd::RenderKanturu3rdinterface();
}

void RenderOutSides()
{
	if (gMapManager->currentMap == WD_8TARKAN)
	{
		EnableAlphaTest();
		glColor4f(1.f, 1.f, 1.f, 0.5f);
		EnableAlphaBlend();
		glColor3f(0.3f, 0.3f, 0.25f);
		float WindX = (float)((int)WorldTime % 100000) * 0.0002f;
		RenderBitmapUV(BITMAP_CHROME + 2, 0.f, 0.f, gwinhandle->GetScreenX(), gwinhandle->GetScreenY(), WindX, 0.f, 0.3f, 0.3f);
		float WindX2 = (float)((int)WorldTime % 100000) * 0.001f;
		RenderBitmapUV(BITMAP_CHROME + 3, 0.f, 0.f, gwinhandle->GetScreenX(), gwinhandle->GetScreenY(), WindX2, 0.f, 3.f, 2.f);
	}
	else if (IsKarutanMap())
	{
		EnableAlphaTest();
		EnableAlphaBlend();
		glColor3f(0.3f, 0.3f, 0.25f);
		float fWindX = (float)((int)WorldTime % 100000) * 0.004f;
		RenderBitmapUV(BITMAP_CHROME + 3, 0.f, 0.f, gwinhandle->GetScreenX(), gwinhandle->GetScreenY(), fWindX, 0.f, 3.f, 2.f);
	}
	else if (WD_34CRYWOLF_1ST == gMapManager->currentMap)
	{
		if (ashies == true)
		{
			M34CryWolf1st::RenderBaseSmoke();
			ashies = false;
		}
	}
	else if (gMapManager->currentMap == WD_56MAP_SWAMP_OF_QUIET)
	{
		SEASON3C::GMSwampOfQuiet::RenderBaseSmoke();
	}
	else if (IsIceCity())
	{
		g_Raklion.RenderBaseSmoke();
	}
	else
	{
		battleCastle::RenderBaseSmoke();
	}

	TheMapProcess().RenderFrontSideVisual();

	glColor3f(1.f, 1.f, 1.f);
}

void MoveTournamentInterface()
{
	static unsigned int s_effectCount = 0;
	int Width = 70, Height = 20;
	int WindowX = (gwinhandle->GetScreenX() - Width) / 2;
	int WindowY = (gwinhandle->GetScreenY() - Height) / 2 + 50;
	if (MouseLButtonPush)
	{
		float wRight = WindowX + Width;
		float wBottom = WindowY + Height;

		if (WindowY <= MouseY && MouseY <= WindowY + Height &&
			WindowX <= MouseX && MouseX <= WindowX + Width)
		{
			g_wtMatchResult.Clear();
			g_wtMatchTimeLeft.m_Time = 0;
		}
	}

	if (g_iGoalEffect)
	{
		for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
			MoveBattleSoccerEffect(gmCharacters->GetCharacter(i));
		g_iGoalEffect = 0;
	}
}

void MoveBattleSoccerEffect(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	if (o->Live)
	{
		if (g_iGoalEffect == 1)
		{
			if (c->GuildTeam == 1)
			{
				vec3_t Position, Angle;
				for (int i = 0; i < 36; ++i)
				{
					Angle[0] = -10.f;
					Angle[1] = 0.f;
					Angle[2] = 45.f;
					float x_bias = cosf(Q_PI / 180.f * 10.0f * i);
					float y_bias = sinf(Q_PI / 180.f * 10.0f * i);
					Position[0] = c->Object.Position[0] + x_bias * TERRAIN_SCALE;
					Position[1] = c->Object.Position[1] + y_bias * TERRAIN_SCALE;
					Position[2] = c->Object.Position[2];
					CreateJoint(BITMAP_FLARE, Position, Position, Angle, 22, &c->Object, 35);
				}
			}
		}
		else if (g_iGoalEffect == 2)
		{
			if (c->GuildTeam == 2)
			{
				vec3_t Position, Angle;
				for (int i = 0; i < 36; ++i)
				{
					Angle[0] = -10.f;
					Angle[1] = 0.f;
					Angle[2] = 45.f;
					float x_bias = cosf(Q_PI / 180.f * 10.0f * i);
					float y_bias = sinf(Q_PI / 180.f * 10.0f * i);
					Position[0] = c->Object.Position[0] + x_bias * TERRAIN_SCALE;
					Position[1] = c->Object.Position[1] + y_bias * TERRAIN_SCALE;
					Position[2] = c->Object.Position[2];
					CreateJoint(BITMAP_FLARE, Position, Position, Angle, 22, &c->Object, 35);
				}
			}
		}
	}
}

void RenderTournamentInterface()
{
	int Width = 300, Height = 2 * 5 + 6 * 30;
	int WindowX = (gwinhandle->GetScreenX() - Width) / 2;
	int WindowY = 120 + 0;
	float x = 0.0f, y = 0.0f;
	char t_Str[20];
	strcpy(t_Str, "");

	if (g_wtMatchTimeLeft.m_Time)
	{
		int t_valueSec = g_wtMatchTimeLeft.m_Time % 60;
		int t_valueMin = g_wtMatchTimeLeft.m_Time / 60;
		if (t_valueMin <= 10)
		{
			g_pRenderText->SetFont(g_hFontBig);
			g_pRenderText->SetTextColor(255, 10, 10, 255);
			g_pRenderText->SetBgColor(0);

			if (g_wtMatchTimeLeft.m_Type == 3)
			{
				g_pRenderText->SetTextColor(255, 255, 10, 255);
				sprintf(t_Str, GlobalText[1392], t_valueSec);
			}
			else
			{
				if (g_wtMatchTimeLeft.m_Time < 60)
				{
					g_pRenderText->SetTextColor(255, 255, 10, 255);
				}
				if (t_valueSec < 10)
				{
					sprintf(t_Str, GlobalText[1390], t_valueMin, t_valueSec);
				}
				else
				{
					sprintf(t_Str, GlobalText[1391], t_valueMin, t_valueSec);
				}
			}
			x += (float)GetScreenWidth() / 2; y += 350;
			g_pRenderText->RenderText((int)x, (int)y, t_Str, 0, 0, RT3_WRITE_CENTER); x++; y++;

			g_pRenderText->SetTextColor(0xffffffff);
			g_pRenderText->RenderText((int)x, (int)y, t_Str, 0, 0, RT3_WRITE_CENTER);

			g_pRenderText->SetFont(g_hFont);
			g_pRenderText->SetTextColor(255, 255, 255, 255);
		}
	}

	if (!strcmp(g_wtMatchResult.m_MatchTeamName1, ""))
	{
		return;
	}

	Width = 300;
	Height = 2 * 5 + 5 * 40; WindowX = (gwinhandle->GetScreenX() - Width) / 2;
	WindowY = 120 + 0;
	int yPos = WindowY;
	RenderBitmap(BITMAP_INTERFACE + 22, (float)WindowX, (float)yPos, (float)Width, (float)5, 0.f, 0.f, Width / 512.f, 5.f / 8.f);
	yPos += 5;

	for (int i = 0; i < 5; ++i)
	{
		RenderBitmap(BITMAP_INTERFACE + 21, WindowX, (float)yPos,
			Width, 40.f, 0.f, 0.0f, 213.f / 256.f, 40.f / 64.f);
		yPos += 40.f;
	}
	RenderBitmap(BITMAP_INTERFACE + 22, (float)WindowX, (float)yPos, (float)Width, (float)5, 0.f, 0.f, Width / 512.f, 5.f / 8.f);

	EnableAlphaBlend();
	glColor4f(1.f, 1.f, 1.f, 1.f);
	g_pRenderText->SetFont(g_hFontBig);
	g_pRenderText->SetTextColor(200, 240, 255, 255);
	sprintf(t_Str, GlobalText[1393]);
	g_pRenderText->RenderText(WindowX + Width / 2 - 50, WindowY + 20, t_Str);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	sprintf(t_Str, GlobalText[1394]);
	g_pRenderText->SetTextColor(255, 255, 10, 255);
	g_pRenderText->RenderText(WindowX + Width / 2 - 13, WindowY + 50, t_Str);
	g_pRenderText->SetTextColor(255, 255, 255, 255);

	float t_temp = 0.0f;
	sprintf(t_Str, "%s", g_wtMatchResult.m_MatchTeamName1);
	t_temp = (MAX_ID_SIZE - strlen(t_Str)) * 5;
	g_pRenderText->RenderText(WindowX + 10 + t_temp, WindowY + 50, t_Str);
	sprintf(t_Str, "%s", g_wtMatchResult.m_MatchTeamName2);
	t_temp = (MAX_ID_SIZE - strlen(t_Str)) * 5;
	g_pRenderText->RenderText(WindowX + Width - 120 + t_temp, WindowY + 50, t_Str);

	sprintf(t_Str, "(%d)", g_wtMatchResult.m_Score1);
	g_pRenderText->RenderText(WindowX + 45, WindowY + 75, t_Str);
	sprintf(t_Str, "(%d)", g_wtMatchResult.m_Score2);
	g_pRenderText->RenderText(WindowX + Width - 85, WindowY + 75, t_Str);

	if (g_wtMatchResult.m_Score1 == g_wtMatchResult.m_Score2)
	{
		g_pRenderText->SetFont(g_hFontBig);
		g_pRenderText->SetTextColor(255, 255, 10, 255);
		sprintf(t_Str, GlobalText[1395]);
		g_pRenderText->RenderText(WindowX + Width / 2 - 35, WindowY + 115, t_Str);
		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
	}
	else if (g_wtMatchResult.m_Score1 > g_wtMatchResult.m_Score2)
	{
		g_pRenderText->SetFont(g_hFontBig);
		g_pRenderText->SetTextColor(255, 255, 10, 10);
		sprintf(t_Str, GlobalText[1396]);
		g_pRenderText->RenderText(WindowX + 47, WindowY + 115, t_Str);
		g_pRenderText->SetTextColor(255, 10, 10, 255);
		sprintf(t_Str, GlobalText[1397]);
		g_pRenderText->RenderText(WindowX + Width - 82, WindowY + 115, t_Str);
		g_pRenderText->SetFont(g_hFont);
	}
	else
	{
		g_pRenderText->SetFont(g_hFontBig);
		g_pRenderText->SetTextColor(255, 255, 10, 10);
		sprintf(t_Str, GlobalText[1397]);
		g_pRenderText->RenderText(WindowX + 47, WindowY + 115, t_Str);
		g_pRenderText->SetTextColor(255, 10, 10, 255);
		sprintf(t_Str, GlobalText[1396]);
		g_pRenderText->RenderText(WindowX + Width - 82, WindowY + 115, t_Str);
		g_pRenderText->SetFont(g_hFont);
	}
	g_pRenderText->SetFont(g_hFont);

	Width = 70;
	Height = 20;
	x = (gwinhandle->GetScreenX() - Width) / 2;
	y = (gwinhandle->GetScreenY() - Height) / 2 + 50;
	if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
	{
		RenderBitmap(BITMAP_INTERFACE + 12, (float)x, (float)y, (float)Width, (float)Height, 0.f, 0.f, Width / 128.f, Height / 32.f);
	}
	else
	{
		RenderBitmap(BITMAP_INTERFACE + 11, (float)x, (float)y, (float)Width, (float)Height, 0.f, 0.f, Width / 128.f, Height / 32.f);
	}

	glColor3f(1.f, 1.f, 1.f);
	DisableAlphaBlend();
}

void RenderTimes()
{
	if (MacroTime > 0)
	{
		float Width = 50.f;
		float Height = 2.f;
		float x = (GetScreenWidth() - Width) / 2;
		float y = pos_botton() + 392.0;

		int Time = (double)MacroTime * (int)Width / 100;
		EnableAlphaTest();
		g_pRenderText->RenderText((int)x, (int)y, "Macro Time");
		RenderBar(x, y + 12, Width, Height, (float)Time);
	}
	glColor3f(1.f, 1.f, 1.f);

	matchEvent::RenderTime();
}

void RenderHPBar(int ScreenX, int ScreenY, float Width, int stepHP, bool scale)
{
	EnableAlphaTest();
	glColor4f(0.f, 0.f, 0.f, 0.5f);
	RenderColor((float)(ScreenX + 1), (float)(ScreenY + 1), Width + 4.f, 5.f, 0.0, 0, scale);

	EnableAlphaBlend();
	glColor3f(0.2f, 0.0f, 0.0f);
	RenderColor((float)ScreenX, (float)ScreenY, Width + 4.f, 5.f, 0.0, 0, scale);

	glColor3f(50.f / 255.f, 10 / 255.f, 0.f);
	RenderColor((float)(ScreenX + 2), (float)(ScreenY + 2), Width, 1.f, 0.0, 0, scale);


	glColor3f(250.f / 255.f, 10 / 255.f, 0.f);
	for (int k = 0; k < stepHP; ++k)
	{
		RenderColor((float)(ScreenX + 2 + (k * 4)), (float)(ScreenY + 2), 3.f, 2.f, 0.0, 0, scale);
	}
	DisableAlphaBlend();
}

#include "jpexs.h"

void RenderPartyHP()
{
	if (PartyNumber > 0)
	{

		if (gmProtect->PartyHpBarType == 1)
		{
			for (int j = 0; j < PartyNumber; ++j)
			{
				PARTY_t* p = &Party[j];

				if (p->index <= -1)
					continue;

				CHARACTER* c = gmCharacters->GetCharacter(p->index);
				if (c != NULL)
				{
					OBJECT* o = &c->Object;
					vec3_t      Position;
					int         ScreenX, ScreenY;
					Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 100.f, Position);
					Projection(Position, &ScreenX, &ScreenY);
					RenderBitmap(BITMAP_CURSOR + 7, (float)(ScreenX - 8), (float)(ScreenY - 32), 24.0, 24.0, 0.0, 0.40000001, 1.4, 1.4, 1, 1, 0.0);
				}
			}
		}
		else if (gmProtect->PartyHpBarType != 2)
		{
			float   Width = 38.f;
			char    Text[100];

			for (int j = 0; j < PartyNumber; ++j)
			{
				PARTY_t* p = &Party[j];

				if (p->index <= -1)
					continue;

				CHARACTER* c = gmCharacters->GetCharacter(p->index);
				if (c != NULL)
				{
					OBJECT* o = &c->Object;
					vec3_t      Position;
					int         ScreenX, ScreenY;

					Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 100.f, Position);
					Projection(Position, &ScreenX, &ScreenY);
					ScreenX -= (int)(Width / 2);

					if ((MouseX >= ScreenX && MouseX < ScreenX + Width && MouseY >= ScreenY - 2 && MouseY < ScreenY + 6))
					{
						sprintf(Text, "HP : %d0%%", p->stepHP);
						g_pRenderText->SetTextColor(255, 230, 210, 255);
						g_pRenderText->RenderText(ScreenX, ScreenY - 6, Text);
					}

					int stepHP = min(10, p->stepHP);
					RenderHPBar(ScreenX, ScreenY, Width, stepHP, true);
				}
			}
			DisableAlphaBlend();
			glColor3f(1.f, 1.f, 1.f);
		}
	}


	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	for (size_t i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(i);

		OBJECT* o = &pCharacter->Object;

		if (o->Live && (
#if MAIN_UPDATE > 303
			o->Kind == KIND_PLAYER ||
#endif
			o->Kind == KIND_MONSTER) && pCharacter->Dead == 0 && pCharacter != Hero)
		{
			vec3_t      Position;
			int ScreenX = 0, ScreenY = 0;

			Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + 100.f, Position);
			Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + o->BoundingBoxMax[2] + 100.f;
			Projection2(Position, &ScreenX, &ScreenY);

			if (gmProtect->m_RenderCharacterName == 0)
			{
				ScreenX -= 19.f;

				int stepHP = (10 * pCharacter->GetPercenteLife());
				RenderHPBar(ScreenX, ScreenY, 38.f, stepHP, false);
			}
			else
			{
				float fLife = 104;

				if (pCharacter == Hero)
					fLife *= CharacterMachine->GetRealLife();
				else
					fLife *= pCharacter->GetPercenteLife();

				if (fLife > 0.0f)
				{
					jpexs::Renderjpexs_dds(IMAGE_HUD_PLAYER, scale_jpexs(ScreenX, -1080), scale_jpexs(ScreenY, 120), 109, 11, 661, 71, 770, 82); //-- bar life

					if (pCharacter->GetKind() == KIND_PLAYER && pCharacter->Object.Type == MODEL_PLAYER)
					{
						jpexs::RenderRepetR2jpexs_dds(IMAGE_HUD_PLAYER, scale_jpexs(ScreenX, -1020), scale_jpexs(ScreenY, 180), fLife, 5, 1015, 64, 1019, 69);
					}
					else if (pCharacter->GetKind() == KIND_MONSTER)
					{
						jpexs::RenderRepetR2jpexs_dds(IMAGE_HUD_PLAYER, scale_jpexs(ScreenX, -1020), scale_jpexs(ScreenY, 180), fLife, 5, 1009, 64, 1013, 69);
					}
				}
			}
		}
	}
}

extern int g_iKeyPadEnable;

void RenderCursor()
{
	EnableAlphaTest();
	glColor3f(1.f, 1.f, 1.f);

	if (Hero == NULL)
	{
		RenderBitmap(BITMAP_CURSOR, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		return;
	}

	float u = 0.f;
	float v = 0.f;
	int Frame = (int)(WorldTime * 0.01f) % 6;

	if (Frame == 1 || Frame == 3 || Frame == 5)
		u = 0.5f;
	if (Frame == 2 || Frame == 3 || Frame == 4)
		v = 0.5f;

	if (g_iKeyPadEnable || ErrorMessage)
	{
		RenderBitmap(BITMAP_CURSOR, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
	}
	else if (SelectedItem != -1)
	{
		RenderBitmap(BITMAP_CURSOR + 3, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
	}
	else if (SelectedNpc != -1)
	{
		if (M38Kanturu2nd::Is_Kanturu2nd())
		{
			RenderBitmap(BITMAP_CURSOR2, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		}
		else
		{
			RenderBitmap(BITMAP_CURSOR + 4, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f, u, v, 0.5f, 0.5f);
		}
	}
	else if (SelectedOperate != -1)
	{
		if ((gMapManager->currentMap == WD_0LORENCIA && Operates[SelectedOperate].Owner->Type == MODEL_POSE_BOX) ||
			(gMapManager->currentMap == WD_1DUNGEON && Operates[SelectedOperate].Owner->Type == 60) ||
			(gMapManager->currentMap == WD_2DEVIAS && Operates[SelectedOperate].Owner->Type == 91) ||
			(gMapManager->currentMap == WD_3NORIA && Operates[SelectedOperate].Owner->Type == 38)
			)
			RenderBitmap(BITMAP_CURSOR + 6, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		else
			RenderBitmap(BITMAP_CURSOR + 7, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
	}
	else if ((!Hero->SafeZone/*||EnableEdit*/) && SelectedCharacter != -1)
	{
		if (CheckAttack() && !MouseOnWindow)
		{
			if (gMapManager->InBattleCastle())
			{
				RenderBitmap(BITMAP_CURSOR2, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
			}
			else
			{
				RenderBitmap(BITMAP_CURSOR + 2, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
			}
		}
		else
			RenderBitmap(BITMAP_CURSOR, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
	}
	else if (g_pNewUISystem && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_COMMAND) && g_pCommandWindow)
	{
		if (g_pCommandWindow->GetMouseCursor() == CURSOR_IDSELECT)
		{
			RenderBitmap(BITMAP_INTERFACE_EX + 29, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		}
		else if (g_pCommandWindow->GetMouseCursor() == CURSOR_NORMAL)
		{
			RenderBitmap(BITMAP_CURSOR, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		}
		else if (g_pCommandWindow->GetMouseCursor() == CURSOR_PUSH)
		{
			RenderBitmap(BITMAP_CURSOR + 1, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		}
	}
	else if (g_pNewUISystem && ((g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY)
		&& g_pMyInventory->GetRepairMode() == SEASON3B::CNewUIMyInventory::REPAIR_MODE_ON)
		|| (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP)
			&& g_pNPCShop->GetShopState() == SEASON3B::CNewUINPCShop::SHOP_STATE_REPAIR)
		))
	{
		if (MouseLButton == false)
			RenderBitmap(BITMAP_CURSOR + 5, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		else
			RenderBitmapRotate(BITMAP_CURSOR + 5, (float)MouseX + 5.f, (float)MouseY + 18.f, 24.f, 24.f, 45.f);
	}
	else if (RepairEnable == 2)
	{
		if (sin((double)WorldTime * 0.02f) > 0)
			RenderBitmapRotate(BITMAP_CURSOR + 5, (float)MouseX + 10.f, (float)MouseY + 10.f, 24.f, 24.f, 0.f);
		else
			RenderBitmapRotate(BITMAP_CURSOR + 5, (float)MouseX + 5.f, (float)MouseY + 18.f, 24.f, 24.f, 45.f);
	}
	else
	{
		if (!MouseLButton)
			RenderBitmap(BITMAP_CURSOR, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		else
		{
			if (DontMove)
				RenderBitmap(BITMAP_CURSOR + 8, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
			else
				RenderBitmap(BITMAP_CURSOR + 1, (float)MouseX - 2.f, (float)MouseY - 2.f, 24.f, 24.f);
		}
	}
}

#ifdef ENABLE_EDIT
//-- 005BBCF0
void BackSelectModel()
{
	for (int i = 1; i < 20; i++)
	{
		if (SelectModel - i < 0)
			break;
		if (gmClientModels->GetModel(SelectModel - i)->NumMeshs > 0)
		{
			SelectModel -= i;
			break;
		}
	}
}

//-- 005BBD60
void ForwardSelectModel()
{
	for (int i = 1; i < 20; i++)
	{
		if (gmClientModels->GetModel(SelectModel + i)->NumMeshs > 0)
		{
			SelectModel += i;
			break;
		}
	}
}

extern int  EditMonsterNumber;

extern int MonsterKey;
//-- 005BBDC0
void EditObjects()
{
	if (EditFlag == EDIT_MONSTER)
	{
		if (MouseLButtonPush)
		{
			MouseLButtonPush = false;
			bool Success = RenderTerrainTile(SelectXF, SelectYF, (int)SelectXF, (int)SelectYF, 1.f, 1, true);

			if (Success)
			{
				CHARACTER* c = CreateMonster(MonsterScript[SelectMonster].Type, (BYTE)(CollisionPosition[0] / TERRAIN_SCALE), (BYTE)(CollisionPosition[1] / TERRAIN_SCALE), MonsterKey++);
				c->Object.Kind = KIND_EDIT;
			}
		}
		if (MouseRButtonPush)
		{
			MouseRButtonPush = false;

			CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedCharacter);

			if (pCharacter)
			{
				pCharacter->Object.Live = false;
			}
		}
	}
	if (EditFlag == EDIT_OBJECT)
	{
		if (MouseRButtonPush)
		{
			MouseRButtonPush = false;
			bool Success = RenderTerrainTile(SelectXF, SelectYF, (int)SelectXF, (int)SelectYF, 1.f, 1, true);
			if (Success)
			{
				OBJECT* o = CreateObject(SelectModel, CollisionPosition, PickObjectAngle);
				int Scale = (int)TERRAIN_SCALE;
				o->Position[0] = (float)((int)o->Position[0] / Scale + 0.5f) * Scale;
				o->Position[1] = (float)((int)o->Position[1] / Scale + 0.5f) * Scale;
			}
		}
		if (MouseLButtonPush)
		{
			MouseLButtonPush = false;
			if (!PickObject)
			{
				if (MouseX < 100 && MouseY < 100)
				{
					PickObject = CreateObject(SelectModel, MouseTarget, PickObjectAngle);
				}
				else
				{
					PickObject = CollisionDetectObjects(PickObject);
					if (PickObject)
						PickObjectHeight = PickObject->Position[2] - RequestTerrainHeight(PickObject->Position[0], PickObject->Position[1]);
				}
			}
		}
		if (PickObject)
		{
			if (MouseLButton)
			{
				bool Success = RenderTerrainTile(SelectXF, SelectYF, (int)SelectXF, (int)SelectYF, 1.f, 1, true);
				if (Success)
				{
					VectorCopy(CollisionPosition, PickObject->Position);
					if (PickObjectLockHeight)
					{
						int Scale = (int)TERRAIN_SCALE / 2;
						PickObject->Position[0] = (float)((int)PickObject->Position[0] / Scale * Scale);
						PickObject->Position[1] = (float)((int)PickObject->Position[1] / Scale * Scale);
					}
					else
						PickObject->Position[2] += PickObjectHeight;
				}
				if (HIBYTE(GetAsyncKeyState('Q')) == 128)
					PickObject->Angle[0] -= 5.f;
				if (HIBYTE(GetAsyncKeyState('E')) == 128)
					PickObject->Angle[0] += 5.f;
				if (HIBYTE(GetAsyncKeyState('A')) == 128)
					PickObject->Angle[2] += 30.f;
				if (HIBYTE(GetAsyncKeyState('D')) == 128)
					PickObject->Angle[2] -= 30.f;
				if (HIBYTE(GetAsyncKeyState('W')) == 128)
					PickObjectHeight += 5.f;
				if (HIBYTE(GetAsyncKeyState('S')) == 128)
					PickObjectHeight -= 5.f;
				if (HIBYTE(GetAsyncKeyState('R')) == 128)
					PickObject->Scale += 0.02f;
				if (HIBYTE(GetAsyncKeyState('F')) == 128)
					PickObject->Scale -= 0.02f;
				if (MouseX >= gwinhandle->GetScreenX() - 100 && MouseY < 100)
				{
					DeleteObject(PickObject, &ObjectBlock[PickObject->Block]);
					PickObject = NULL;
				}
			}
			else
			{
				VectorCopy(PickObject->Angle, PickObjectAngle);
				CreateObject(PickObject->Type, PickObject->Position, PickObject->Angle, PickObject->Scale);
				if (EnableRandomObject)
				{
					vec3_t Position, Angle;
					for (int i = 0; i < 9; i++)
					{
						VectorCopy(PickObject->Position, Position);
						VectorCopy(PickObject->Angle, Angle);
						Position[0] += (float)(rand() % 2000 - 1000);
						Position[1] += (float)(rand() % 2000 - 1000);
						Position[2] = RequestTerrainHeight(Position[0], Position[1]);
						Angle[2] = (float)(rand() % 360);
						CreateObject(PickObject->Type, Position, Angle, PickObject->Scale + (float)(rand() % 16 - 8) * 0.01f);
					}
				}
				DeleteObject(PickObject, &ObjectBlock[PickObject->Block]);
				PickObject = NULL;
			}
		}
	}

	if (EditFlag == EDIT_HEIGHT)
	{
		if (MouseLButton)
		{
			AddTerrainHeight(CollisionPosition[0], CollisionPosition[1], -10.f, BrushSize + 1, BackTerrainHeight);
			CreateTerrainNormal();
			CreateTerrainLight();
		}
		if (MouseRButton)
		{
			AddTerrainHeight(CollisionPosition[0], CollisionPosition[1], 10.f, BrushSize + 1, BackTerrainHeight);
			CreateTerrainNormal();
			CreateTerrainLight();
		}
	}
	if (EditFlag == EDIT_LIGHT)
	{
		vec3_t Light;
		if (MouseLButton)
		{
			switch (SelectColor)
			{
			case 0:
				Vector(0.1f, 0.1f, 0.1f, Light);
				break;
			case 1:
				Vector(-0.1f, -0.1f, -0.1f, Light);
				break;
			case 2:
				Vector(0.05f, -0.05f, -0.05f, Light);
				break;
			case 3:
				Vector(0.05f, 0.05f, -0.05f, Light);
				break;
			case 4:
				Vector(-0.05f, 0.05f, -0.05f, Light);
				break;
			case 5:
				Vector(-0.05f, 0.05f, 0.05f, Light);
				break;
			case 6:
				Vector(-0.05f, -0.05f, 0.05f, Light);
				break;
			case 7:
				Vector(0.05f, -0.05f, 0.05f, Light);
				break;
			}
			AddTerrainLightClip(CollisionPosition[0], CollisionPosition[1], Light, BrushSize + 1, TerrainLight);
			CreateTerrainLight();
		}
		if (MouseRButton)
		{
			int mx = (int)(CollisionPosition[0] / TERRAIN_SCALE);
			int my = (int)(CollisionPosition[1] / TERRAIN_SCALE);
			for (int y = my - 2; y <= my + 2; y++)
			{
				for (int x = mx - 2; x <= mx + 2; x++)
				{
					int Index1 = TERRAIN_INDEX_REPEAT(x, y);
					int Index2 = TERRAIN_INDEX_REPEAT(x - 1, y);
					int Index3 = TERRAIN_INDEX_REPEAT(x + 1, y);
					int Index4 = TERRAIN_INDEX_REPEAT(x, y - 1);
					int Index5 = TERRAIN_INDEX_REPEAT(x, y + 1);
					for (int i = 0; i < 3; i++)
					{
						TerrainLight[Index1][i] = (TerrainLight[Index1][i] + TerrainLight[Index2][i] + TerrainLight[Index3][i] + TerrainLight[Index4][i] + TerrainLight[Index5][i]) * 0.2f;
					}
				}
			}
			CreateTerrainLight();
		}
	}
	if (EditFlag == EDIT_MAPPING)
	{
		int sx = gwinhandle->GetScreenX() - 30;
		int sy = 0;
		for (int i = 0; i < 14; i++)
		{
			if (MouseX >= sx && MouseY >= sy + i * 30 && MouseX < sx + 30 && MouseY < sy + i * 30 + 30)
			{
				if (MouseLButton)
				{
					SelectMapping = i;
					return;
				}
			}
		}
		int x = (int)SelectXF;
		int y = (int)SelectYF;
		int Index1 = TERRAIN_INDEX_REPEAT(x, y);
		int Index2 = TERRAIN_INDEX_REPEAT(x + 1, y);
		int Index3 = TERRAIN_INDEX_REPEAT(x + 1, y + 1);
		int Index4 = TERRAIN_INDEX_REPEAT(x, y + 1);
		if (Bitmaps[BITMAP_MAPTILE + SelectMapping].Components != 4)
		{
			if (MouseLButton)
			{
				if (CurrentLayer == 0)
				{
					for (int i = y - BrushSize; i <= y + BrushSize; i++)
					{
						for (int j = x - BrushSize; j <= x + BrushSize; j++)
						{
							TerrainMappingLayer1[TERRAIN_INDEX_REPEAT(j, i)] = SelectMapping;
						}
					}
				}
				if (CurrentLayer == 1)
				{
					for (int i = y - 1; i <= y + 1; i++)
					{
						for (int j = x - 1; j <= x + 1; j++)
						{
							TerrainMappingLayer2[TERRAIN_INDEX_REPEAT(j, i)] = SelectMapping;
						}
					}
					TerrainMappingAlpha[Index1] += 0.1f;
					TerrainMappingAlpha[Index2] += 0.1f;
					TerrainMappingAlpha[Index3] += 0.1f;
					TerrainMappingAlpha[Index4] += 0.1f;
					if (TerrainMappingAlpha[Index1] > 1.f) TerrainMappingAlpha[Index1] = 1.f;
					if (TerrainMappingAlpha[Index2] > 1.f) TerrainMappingAlpha[Index2] = 1.f;
					if (TerrainMappingAlpha[Index3] > 1.f) TerrainMappingAlpha[Index3] = 1.f;
					if (TerrainMappingAlpha[Index4] > 1.f) TerrainMappingAlpha[Index4] = 1.f;
				}
			}
			if (MouseRButton)
			{
				if (CurrentLayer == 1)
				{
					for (int i = y - 1; i <= y + 1; i++)
					{
						for (int j = x - 1; j <= x + 1; j++)
						{
							TerrainMappingLayer2[TERRAIN_INDEX_REPEAT(j, i)] = 255;
						}
					}
					TerrainMappingAlpha[Index1] = 0.f;
					TerrainMappingAlpha[Index2] = 0.f;
					TerrainMappingAlpha[Index3] = 0.f;
					TerrainMappingAlpha[Index4] = 0.f;
				}
			}
		}
	}
}
#endif

int TotalPacketSize = 0;
int OldTime = timeGetTime();
extern int MixItemValue;
extern int MixItemValueOld;

void RenderDebugWindow()
{
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->SetBgColor(0, 0, 0, 255);

	if (timeGetTime() - OldTime >= 1000)
	{
		OldTime = timeGetTime();
		TotalPacketSize = 0;
	}

#ifdef ENABLE_EDIT
	char Text[256];
	if (EditFlag == EDIT_MAPPING)
	{
		int sx = gwinhandle->GetScreenX() - 30;
		int sy = 0;
		for (int i = 0; i < 14; i++)
		{
			if (i == SelectMapping)
				glColor3f(1.f, 1.f, 1.f);
			else
				glColor3f(0.8f, 0.8f, 0.8f);
			RenderBitmap(BITMAP_MAPTILE + i, (float)(sx), (float)(sy + i * 30), 30.f, 30.f);
		}
		if (CurrentLayer == 0)
			g_pRenderText->RenderText(gwinhandle->GetScreenX() - 100, sy, "Background");
		else
			g_pRenderText->RenderText(gwinhandle->GetScreenX() - 100, sy, "Layer1");
		sprintf(Text, "Brush Size: %d", BrushSize * 2 + 1);
		g_pRenderText->RenderText(gwinhandle->GetScreenX() - 100, sy + 11, Text);
	}
	glColor3f(1.f, 1.f, 1.f);
	if (EditFlag == EDIT_OBJECT)
	{
		g_pRenderText->RenderText(gwinhandle->GetScreenX() - 100, 0, "Garbage");
		g_pRenderText->RenderText(0, 0, gmClientModels->GetModel(SelectModel)->Name);
	}
	if (EditFlag == EDIT_MONSTER)
	{
		for (int i = 0; i < EditMonsterNumber; i++)
		{
			if (i == SelectMonster)
				glColor3f(1.f, 0.8f, 0.f);
			else
				glColor3f(1.f, 1.f, 1.f);
			sprintf(Text, "%2d: %s", MonsterScript[i].Type, MonsterScript[i].Name);
			g_pRenderText->RenderText(gwinhandle->GetScreenX() - 100, i * 10, Text);
		}
	}
	if (EditFlag == EDIT_LIGHT)
	{
		for (int i = 0; i < 8; i++)
		{
			if (i == SelectColor)
				glColor3f(1.f, 0.8f, 0.f);
			else
				glColor3f(1.f, 1.f, 1.f);

			g_pRenderText->RenderText(gwinhandle->GetScreenX() - 64, i * 10, ColorTable[i]);
		}
	}
#endif //ENABLE_EDIT
}

void MouseRButtonReset()
{
	MouseRButtonPop = false;
	MouseRButtonPush = false;
	MouseRButton = false;
	MouseRButtonPress = 0;
}

bool IsGMCharacter()
{
	if ((Hero->Object.Kind == KIND_PLAYER
		&& Hero->Object.Type == MODEL_PLAYER
		&& Hero->Object.SubType == MODEL_GM_CHARACTER)
		|| (g_isCharacterBuff((&Hero->Object), eBuff_GMEffect))
		|| (Hero->CtlCode & CTLCODE_08OPERATOR)
		|| (Hero->CtlCode & CTLCODE_20OPERATOR))
	{
		return true;
	}
	return false;
}

bool IsNonAttackGM()
{
	if (Hero->CtlCode & CTLCODE_04FORTV || Hero->CtlCode & CTLCODE_08OPERATOR)
	{
		return true;
	}

	return false;
}

bool IsIllegalMovementByUsingMsg(const char* szChatText)
{
	bool bCantFly = false;
	bool bCantSwim = false;
	bool bEquipChangeRing = false;

	bool bMoveAtlans = false;
	bool bMoveIcarus = false;

	char szChatTextUpperChars[256];
	strcpy(szChatTextUpperChars, szChatText);
	_strupr(szChatTextUpperChars);

	short pEquipedRightRingType = (&CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT])->Type;
	short pEquipedLeftRingType = (&CharacterMachine->Equipment[EQUIPMENT_RING_LEFT])->Type;
	short pEquipedHelperType = (&CharacterMachine->Equipment[EQUIPMENT_HELPER])->Type;
	short pEquipedWingType = (&CharacterMachine->Equipment[EQUIPMENT_WING])->Type;


	if ((pEquipedWingType == -1 && pEquipedHelperType != ITEM_HELPER + 3 &&
		pEquipedHelperType != ITEM_HELPER + 37 && pEquipedHelperType != ITEM_HELPER + 4) ||
		pEquipedHelperType == ITEM_HELPER + 2)
	{
		bCantFly = true;
	}
	if (pEquipedHelperType == ITEM_HELPER + 2 || pEquipedHelperType == ITEM_HELPER + 3)
	{
		bCantSwim = true;
	}
	if (g_ChangeRingMgr->CheckMoveMap(pEquipedLeftRingType, pEquipedRightRingType))
	{
		bEquipChangeRing = true;
	}

	if ((strstr(szChatTextUpperChars, "/MOVE") != NULL) ||
		(strlen(GlobalText[260]) > 0 && strstr(szChatTextUpperChars, GlobalText[260]) != NULL))
	{
		std::list<SEASON3B::CMoveCommandData::MOVEINFODATA*> m_listMoveInfoData;
		m_listMoveInfoData = SEASON3B::CMoveCommandData::GetInstance()->GetMoveCommandDatalist();

		std::list<SEASON3B::CMoveCommandData::MOVEINFODATA*>::iterator li = m_listMoveInfoData.begin();


		while (li != m_listMoveInfoData.end())
		{
			char cMapNameUpperChars[256];
			strcpy(cMapNameUpperChars, (*li)->_ReqInfo.szSubMapName);
			_strupr(cMapNameUpperChars);

			if (strstr(szChatText, ((*li)->_ReqInfo.szMainMapName)) != NULL ||
				strstr(szChatTextUpperChars, cMapNameUpperChars) != NULL)
				break;

			li++;
		}

		if (li != m_listMoveInfoData.end())
		{
			if (stricmp((*li)->_ReqInfo.szMainMapName, GlobalText[37]) == 0)
			{
				bMoveAtlans = true;
			}
			else if (stricmp((*li)->_ReqInfo.szMainMapName, GlobalText[55]) == 0)
			{
				bMoveIcarus = true;
			}
		}
	}

	if (bCantSwim && bMoveAtlans)
	{
		g_pChatListBox->AddText("", GlobalText[261], SEASON3B::TYPE_SYSTEM_MESSAGE);
		return true;
	}

	if ((bCantFly || bEquipChangeRing) && bMoveIcarus)
	{
		g_pChatListBox->AddText("", GlobalText[263], SEASON3B::TYPE_SYSTEM_MESSAGE);
		return true;
	}

	return false;
}
