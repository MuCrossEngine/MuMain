///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CGMProtect.h"
#include "UIManager.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzLodTerrain.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzTexture.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "ZzzAI.h"
#include "ZzzEffect.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "ZzzScene.h"
#include "./Utilities/Log/ErrorReport.h"
#include "CSQuest.h"
#include "Local.h"
#include "zzzMixInventory.h"
#include "PersonalShopTitleImp.h"
#include "GOBoid.h"
#include "CSItemOption.h"
#include "CSChaosCastle.h"
#include "GMHellas.h"
#include "npcBreeder.h"
#include "GIPetManager.h"
#include "CSParts.h"
#include "npcGateSwitch.h"
#include "GMBattleCastle.h"
#include "CComGem.h"
#include "GMCrywolf1st.h"
#include "CDirection.h"
#include "ChangeRingManager.h"
#include "MixMgr.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "NewUIInventoryCtrl.h"
#include "GM3rdChangeUp.h"
#include "w_CursedTemple.h"
#include "SocketSystem.h"
#include "PortalMgr.h"
#include "NewUISystem.h"
#include "ServerListManager.h"
#include <time.h>
#include "MapManager.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "CGMItemDropName.h"
#include "jpexs.h"
#include "CGMJewelOfAction.h"


extern CUITextInputBox* g_pSingleTextInputBox;
extern int g_iChatInputType;
extern CUIGuildListBox* g_pGuildListBox;

int			g_nTaxRate = 0;
int			g_nChaosTaxRate = 0;

char         g_GuildNotice[3][128];
GUILD_LIST_t GuildList[MAX_GUILDS];
int			 g_nGuildMemberCount = 0;
int			 GuildTotalScore = 0;
int			 GuildPlayerKey = 0;
int			 SelectMarkColor = 0;
MARK_t		 GuildMark[MAX_MARKS];

PARTY_t Party[MAX_PARTYS];
int     PartyNumber = 0;
int     PartyKey = 0;

ITEM_t Items[MAX_ITEMS];
ITEM   PickItem;
ITEM   TargetItem;

ITEM Inventory[MAX_INVENTORY];
ITEM ShopInventory[MAX_SHOP_INVENTORY];
ITEM g_PersonalShopInven[MAX_PERSONALSHOP_INVEN];
ITEM g_PersonalShopBackup[MAX_PERSONALSHOP_INVEN];
bool g_bEnablePersonalShop = false;
int g_iPShopWndType = PSHOPWNDTYPE_NONE;
POINT g_ptPersonalShop = { 0, 0 };
int g_iPersonalShopMsgType = 0;
char g_szPersonalShopTitle[64] = { 0, };

CHARACTER g_PersonalShopSeller;
bool g_bIsTooltipOn = false;

int   CheckSkill = -1;
ITEM* CheckInventory = NULL;

bool  EquipmentSuccess = false;
bool  CheckShop;
int   CheckX;
int   CheckY;
ITEM* SrcInventory;
int   SrcInventoryIndex;
int   DstInventoryIndex;

int  AllRepairGold = 0;
int  StorageGoldFlag = 0;

int ListCount = 0;
int GuildListPage = 0;

int  g_bEventChipDialogEnable = EVENT_NONE;
int  g_shEventChipCount = 0;
short g_shMutoNumber[3] = { -1, -1, -1 };

bool g_bServerDivisionAccept = false;
char g_strGiftName[64];

bool RepairShop = false;
int  RepairEnable = 0;
int AskYesOrNo = 0;

char OkYesOrNo = -1;

int g_iKeyPadEnable = 0;
WORD g_wStoragePassword = 0;
short	g_nKeyPadMapping[10];
char	g_lpszKeyPadInput[2][MAX_KEYPADINPUT + 1];

BYTE g_byItemUseType = 0;

static  const int DEFAULT_DEVILSQUARELEVEL[6][2] = { {15, 130}, { 131, 180}, { 181, 230}, {231, 280}, { 281, 330}, { 331, 99999} };
static  const int DARKCLASS_DEVILSQUARELEVEL[6][2] = { {15, 110}, { 111, 160}, { 161, 210}, {211, 260}, { 261, 310}, { 311, 99999} };
int g_iDevilSquareLevel[6][2];

static  const int g_iChaosCastleLevel[12][2] = {
						{ 15, 49 }, { 50, 119 }, { 120, 179 }, { 180, 239 }, { 240, 299 }, { 300, 999 },
						{ 15, 29 }, { 30,  99 }, { 100, 159 }, { 160, 219 }, { 220, 279 }, { 280, 999 }
};
static  const int g_iChaosCastleZen[6] = { 25, 80, 150, 250, 400, 650 };

BYTE BuyItem[4];

static  int iStateNum = 4;

extern bool Teleport;

#ifdef _PVP_ADD_MOVE_SCROLL
extern CMurdererMove g_MurdererMove;
#endif	// _PVP_ADD_MOVE_SCROLL

int getLevelGeneration(int level, unsigned int* color)
{
	int lvl;
	if (level >= 300)
	{
		lvl = 300;
		*color = (255 << 24) + (255 << 16) + (153 << 8) + (255);
	}
	else if (level >= 200)
	{
		lvl = 200;
		*color = (255 << 24) + (255 << 16) + (230 << 8) + (210);
	}
	else if (level >= 100)
	{
		lvl = 100;
		*color = (255 << 24) + (24 << 16) + (201 << 8) + (0);
	}
	else if (level >= 50)
	{
		lvl = 50;
		*color = (255 << 24) + (0 << 16) + (150 << 8) + (255);
	}
	else
	{
		lvl = 10;
		*color = (255 << 24) + (0 << 16) + (0 << 8) + (255);
	}
	return lvl;
}

SIZE Size[30];

int RenderTextList(int sx, int sy, int TextNum, int Tab, int iSort = RT3_SORT_CENTER)
{
	int TextWidth = 0;
	float fsy = sy;
	for (int i = 0; i < TextNum; i++)
	{
		if (TextBold[i])
		{
			g_pRenderText->SetFont(g_hFontBold);
		}
		else
		{
			g_pRenderText->SetFont(g_hFont);
		}

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[i], lstrlen(TextList[i]), &Size[i]);

		if (TextWidth < Size[i].cx)
		{
			TextWidth = Size[i].cx;
		}
	}

	if (Tab == 0)
	{
		sx -= (TextWidth + Tab) * gwinhandle->GetScreenX() / WindowWidth / 2;
	}

	for (int i = 0; i < TextNum; i++)
	{
		g_pRenderText->SetTextColor(0xffffffff);

		switch (TextListColor[i])
		{
		case TEXT_COLOR_WHITE:
		case TEXT_COLOR_DARKRED:
		case TEXT_COLOR_DARKBLUE:
		case TEXT_COLOR_DARKYELLOW:
			glColor3f(1.f, 1.f, 1.f);
			break;
		case TEXT_COLOR_BLUE:
			glColor3f(0.5f, 0.7f, 1.f);
			break;
		case TEXT_COLOR_GRAY:
			glColor3f(0.4f, 0.4f, 0.4f);
			break;
		case TEXT_COLOR_GREEN_BLUE:
			glColor3f(1.f, 1.f, 1.f);
			break;
		case TEXT_COLOR_RED:
			glColor3f(1.f, 0.2f, 0.1f);
			break;
		case TEXT_COLOR_YELLOW:
			glColor3f(1.f, 0.8f, 0.1f);
			break;
		case TEXT_COLOR_GREEN:
			glColor3f(0.1f, 1.f, 0.5f);
			break;
		case TEXT_COLOR_PURPLE:
			glColor3f(1.f, 0.1f, 1.f);
			break;
		}
		if (TEXT_COLOR_DARKRED == TextListColor[i])
		{
			g_pRenderText->SetBgColor(160, 0, 0, 255);
		}
		else if (TEXT_COLOR_DARKBLUE == TextListColor[i])
		{
			g_pRenderText->SetBgColor(0, 0, 160, 255);
		}
		else if (TEXT_COLOR_DARKYELLOW == TextListColor[i])
		{
			g_pRenderText->SetBgColor(160, 102, 0, 255);
		}
		else if (TEXT_COLOR_GREEN_BLUE == TextListColor[i])
		{
			g_pRenderText->SetTextColor(0, 255, 0, 255);
			g_pRenderText->SetBgColor(60, 60, 200, 255);
		}
		else
		{
			g_pRenderText->SetBgColor(0, 0, 0, 0);
		}

		if (TextBold[i])
		{
			g_pRenderText->SetFont(g_hFontBold);
		}
		else
		{
			g_pRenderText->SetFont(g_hFont);
		}
		SIZE TextSize;
		g_pRenderText->RenderText(sx, fsy, TextList[i], TextWidth + Tab, 0, iSort, &TextSize);
		fsy += TextSize.cy;
	}
	return TextWidth + Tab;
}

void HexToDWORD(DWORD HexColor)
{
	BYTE R = (HexColor >> 24) & 0xFF;  // Rojo (primer byte)
	BYTE G = (HexColor >> 16) & 0xFF;  // Verde (segundo byte)
	BYTE B = (HexColor >> 8) & 0xFF;   // Azul (tercer byte)

	glColor3ub(R, G, B);
}

void HexToDWORD(BYTE R, BYTE G, BYTE B)
{
	glColor3f((R / 255.0), (G / 255.0), (B / 255.0));
}

void HookColorOldFont(int TextColor)
{
	DWORD HexColor;

	g_pRenderText->SetTextColor(0xFFFFFFFF);

	switch (TextColor)
	{
	case 0:
	case 5:
	case 7:
	case 8:
		glColor3f(1.0, 1.0, 1.0);
		break;
	case 1:
		glColor3f(0.5, 0.69999999, 1.0);
		break;
	case 10:
		glColor3f(0.40000001, 0.40000001, 0.40000001);
		break;
	case 9:
		glColor3f(1.0, 1.0, 1.0);
		break;
	case 2:
		glColor3f(1.0, 0.2, 0.1);
		break;
	case 3:
		glColor3f(1.0, 0.80000001, 0.1);
		break;
	case 4:
		glColor3f(0.1, 1.0, 0.5);
		break;
	case 6:
		glColor3f(1.0, 0.1, 1.0);
		break;
	case 11:
		glColor3f(0.80000001, 0.5, 0.80000001);
		break;
	case 12:
		glColor3f(0.69999999, 0.40000001, 1.0);
		break;
	case 13:
		glColor3f(0.89999998, 0.41999999, 0.039999999);
		break;
	default:
		switch (TextColor)
		{
		case 14:
			HexColor = 0x800080FF;
			break;
		case 15:
			HexColor = 0xFFFF00FF;
			break;
		case 16:
			HexColor = 0x00B0F0FF;
			break;
		case 17:
			HexColor = 0x00B050FF;
			break;
		case 18:
			HexColor = 0xCCFF00FF;
			break;
		case 19:
			HexColor = 0x00FFFFFF;
			break;
		case 20:
			HexColor = 0xBCBCBCFF;
			break;
		case 21:
			HexColor = 0xC1C469FF;
			break;
		default:
			HexColor = 0xFFFFFFFF;
			break;
		}
		HexToDWORD(HexColor);
		break;
	}

	switch (TextColor)
	{
	case 5:
		g_pRenderText->SetBgColor(0xA0u, 0, 0, 0xFFu);
		break;
	case 7:
		g_pRenderText->SetBgColor(0, 0, 0xA0u, 0xFFu);
		break;
	case 8:
		g_pRenderText->SetBgColor(0xA0u, 0x66u, 0, 0xFFu);
		break;
	case 9:
		g_pRenderText->SetBgColor(0x3Cu, 0x3Cu, 0xC8u, 0xFFu);
		g_pRenderText->SetTextColor(0, 0xFFu, 0, 0xFFu);
		break;
	default:
		g_pRenderText->SetBgColor(0);
		break;
	}
}

void HookColorFont(int TextColor)
{
	DWORD HexColor;

	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetTextColor(0xFFFFFFFF);

	switch (TextColor)
	{
	case 0:
		HexColor = 0xFFFFFFFF;
		break;
	case 1:
		HexColor = 0x7FB2FFFF;
		break;
	case 2:
		HexColor = 0xFF3319FF;
		break;
	case 3:
		HexColor = 0xFFCC19FF;
		break;
	case 4:
		HexColor = 0x19C81CFF;
		break;
	case 5:
		HexColor = 0xA00000FF;
		break;
	case 6:
		HexColor = 0xFF19FFFF;
		break;
	case 7:
		HexColor = 0x0000A0FF;
		break;
	case 8:
		HexColor = 0xA06600FF;
		break;
	case 9:
		HexColor = 0x00FF00FF;
		break;
	case 10:
		HexColor = 0xB2B2B2FF;
		break;
	case 11:
		HexColor = 0xCC7FCCFF;
		break;
	case 12:
		HexColor = 0xB266FFFF;
		break;
	case 13:
		HexColor = 0xE56B0AFF;
		break;
	case 14:
		HexColor = 0x800080FF;
		break;
	case 15:
		HexColor = 0xFFFF00FF;
		break;
	case 16:
		HexColor = 0x00B0F0FF;
		break;
	case 17:
		HexColor = 0x00B050FF;
		break;
	case 18:
		HexColor = 0xCCFF00FF;
		break;
	case 19:
		HexColor = 0x00FFFFFF;
		break;
	case 20:
		HexColor = 0xBCBCBCFF;
		break;
	case 21:
		HexColor = 0xC1C469FF;
		break;
	default:
		HexColor = 0xFFFFFFFF;
		break;
	}

	HexToDWORD(HexColor);
}

void RenderTipTextList(const int sx, const int sy, int TextNum, int Tab, int iSort, int iRenderPoint, BOOL bUseBG, bool bRender3d)
{
	float fscale = 0.0;
	SIZE TextSize[2] = { 0, 0, 0, 0 };
	int TextLine[2] = { 0, 0 };
	int EmptyLine[2] = { 0, 0 };
	int SteepLine[2] = { -1, -1 };

	float fWidth[2] = { 0, 0 };
	float fHeight[2] = { 0, 0 };

	bool Success = false;

	if (g_csItemOption.m_ByLineIni != 0 && g_csItemOption.m_ByLineEnd != 0)
	{
		Success = true;
		SteepLine[0] = g_csItemOption.m_ByLineIni;
		SteepLine[1] = g_csItemOption.m_ByLineEnd;

		g_csItemOption.m_ByLineIni = 0;
		g_csItemOption.m_ByLineEnd = 0;
	}

	int current_left = 0;

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL))
		fscale = 1.3;
	else
		fscale = 1.1;

	for (int i = 0; i < TextNum; ++i)
	{
		if (TextList[i][0] == '\0')
		{
			TextNum = i; break;
		}

		if (TextBold[i])
			g_pRenderText->SetFont(g_hFontBold);
		else
			g_pRenderText->SetFont(g_hFont);

		current_left = 0;

		if (Success && (i >= SteepLine[0] && i < SteepLine[1]))
		{
			current_left = 1;
		}

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[i], lstrlen(TextList[i]), &TextSize[current_left]);

		float iHeight = 0;

		if (fWidth[current_left] < TextSize[current_left].cx)
		{
			fWidth[current_left] = TextSize[current_left].cx;
		}

		if (TextList[i][0] == 0x0A || (TextList[i][0] == ' ' && TextList[i][1] == 0x00))
		{
			++EmptyLine[current_left];
			iHeight = (float)TextSize[current_left].cy;

			if (TextList[i][0] == 0x0A)
			{
				iHeight *= 0.5f;
			}
		}
		else
		{
			++TextLine[current_left];
			iHeight = (TextSize[current_left].cy);
		}
		fHeight[current_left] += (iHeight * fscale);
	}

	if (TextNum == 0)
		return;

	ITEM* dwIndentItem = NULL;

	uint16_t max_vh_increase = 0;
	uint16_t max_uw_increase = 0;

	if (bRender3d)
	{
		dwIndentItem = g_pNewItemTooltip->GetItemTooltip();

		if (dwIndentItem)
		{
			Script_Item* pItemAttr = GMItemMng->find(dwIndentItem->Type);

			if (pItemAttr->Height > 1)
			{
				max_vh_increase = pItemAttr->Getheight2() * g_fScreenRate_y;
				max_uw_increase = pItemAttr->Getwidth2() * g_fScreenRate_x;
			}
			else
			{
				max_vh_increase = (pItemAttr->Height * 40) * g_fScreenRate_y;
				max_uw_increase = (pItemAttr->Width * 40) * g_fScreenRate_x;
			}
		}

		fHeight[0] += max_vh_increase;

		if (fWidth[0] < max_uw_increase)
		{
			fWidth[0] = max_uw_increase;
		}
		else
		{
			fWidth[0] += max_uw_increase / 2.f;
			max_uw_increase = fWidth[0];
		}
	}

	int pos[2] = { 0, 0 };

	for (int i = 0; i < 2; i++)
	{
		if (!Success && i == 1)
			continue;

		if (Tab > 0)
		{
			fWidth[i] = Tab * 2;
		}

		fWidth[i] += 12.0;
		fHeight[i] += 4.0;

		pos[i] = (sx * g_fScreenRate_x) - fWidth[i] / 2.f;

		if (pos[i] < 0)
		{
			pos[i] = 4.f;
		}

		if ((pos[i] + fWidth[i]) > (int)WindowWidth)
		{
			pos[i] = (int)WindowWidth - fWidth[i] - 1;
		}
	}

	if (Success)
	{
		if (fHeight[0] < fHeight[1])
			fHeight[0] = fHeight[1];

		fHeight[1] = fHeight[0];

		if ((pos[0] + fWidth[0]) + fWidth[1] > WindowWidth)
			pos[0] = WindowWidth - ((fWidth[1] + fWidth[0]) + 6.f);
		pos[1] = (pos[0] + fWidth[0] + 6.f);
	}

	float fsy[2] = { 0.f, 0.0 };

	switch (iRenderPoint)
	{
	case STRP_BOTTOMCENTER:
		fsy[0] = (sy * g_fScreenRate_y) - fHeight[0];
		fsy[1] = (sy * g_fScreenRate_y) - fHeight[0];
		break;
	default:
		fsy[0] = (sy * g_fScreenRate_y);
		fsy[1] = (sy * g_fScreenRate_y);
		break;
	}

	if (fsy[0] < 1.f || fsy[1] < 1.f)
	{
		fsy[0] = (1.f * g_fScreenRate_y);
		fsy[1] = (1.f * g_fScreenRate_y);
	}

	EnableAlphaTest(true);

	if (bUseBG == TRUE && TextNum > 0)
	{
		for (int i = 0; i < 2; i++)
		{
			if (!Success && i == 1)
				continue;

			if (gmProtect->shutdown_tooltipold)
				g_pNameWindow->RenderTooltip((float)pos[i], fsy[i] - 8.0, (float)fWidth[i], fHeight[i] + 14.f);
			else
				g_pNameWindow->RenderTooltip_old((float)pos[i], fsy[i] - 8.0, (float)fWidth[i], fHeight[i] + 14.f);
		}
	}

	if (bRender3d)
	{
		if (dwIndentItem)
		{
			Script_Item* pItemAttr = GMItemMng->find(dwIndentItem->Type);


			float renderframex = pos[0];
			float renderframey = fsy[0];

			renderframex += ((max_uw_increase - pItemAttr->Getwidth() * g_fScreenRate_x) / 2.0);
			renderframey += (max_vh_increase - pItemAttr->Getheight() * g_fScreenRate_y) / 2.0;

			SEASON3B::begin3D();

			RenderItem3D(renderframex / g_fScreenRate_x, renderframey / g_fScreenRate_y, pItemAttr->Getwidth(), pItemAttr->Getheight(), dwIndentItem->Type, dwIndentItem->Level, dwIndentItem->Option1, dwIndentItem->ExtOption);

			SEASON3B::endrender3D();
			//fWidth[0] / g_fScreenRate_x

			fsy[0] += max_vh_increase;

			EnableAlphaTest(true);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	for (int i = 0; i < TextNum; i++)
	{
		if (TextBold[i])
			g_pRenderText->SetFont(g_hFontBold);
		else
			g_pRenderText->SetFont(g_hFont);

		g_pRenderText->SetBgColor(0);

		if (gmProtect->shutdown_tooltipold)
		{
			HookColorFont(TextListColor[i]);
		}
		else
		{
			HookColorOldFont(TextListColor[i]);
		}

		float iHeight = 0;

		current_left = 0;

		if (Success && (i >= SteepLine[0] && i < SteepLine[1]))
		{
			current_left = 1;
		}

		if (TextList[i][0] == 0x0A || (TextList[i][0] == ' ' && TextList[i][1] == 0x00))
		{
			g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[i], lstrlen(TextList[i]), &TextSize[current_left]);
			iHeight = (float)TextSize[current_left].cy;

			if (TextList[i][0] == 0x0A)
			{
				iHeight *= 0.5f;
			}
		}
		else
		{

			SIZE iTextSize;
			g_pRenderText->RenderTextClipped(pos[current_left], fsy[current_left], TextList[i], fWidth[current_left], 0, iSort, &iTextSize);
			iHeight = iTextSize.cy;
		}
		fsy[current_left] += (iHeight * fscale);
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	DisableAlphaBlend();
}

bool IsCanUseItem()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE) || g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool IsCanTrade()
{
	if (g_pUIManager->IsOpen(INTERFACE_PERSONALSHOPSALE) || g_pUIManager->IsOpen(INTERFACE_PERSONALSHOPPURCHASE))
	{
		return false;
	}
	return true;
}

int TextNum = 0;
int SkipNum = 0;

bool IsRequireClassRenderItem(const short sType)
{
	if (
		sType == ITEM_HELPER + 19
		|| sType == ITEM_HELPER + 29
		|| sType == ITEM_WING + 26
		|| (sType >= ITEM_WING + 30 && sType <= ITEM_WING + 31)
		|| (sType >= ITEM_HELPER + 43 && sType <= ITEM_HELPER + 45)
		|| sType == ITEM_HELPER + 10
		|| (sType >= ITEM_HELPER + 39 && sType <= ITEM_HELPER + 42)
		|| sType == ITEM_HELPER + 37
		|| sType == ITEM_WING + 15
		|| sType == ITEM_WING + 32
		|| sType == ITEM_WING + 33
		|| sType == ITEM_WING + 34
		)
	{
		return false;
	}

	if ((sType >= ITEM_HELPER + 43 && sType <= ITEM_HELPER + 45)
		|| (sType >= ITEM_HELPER + 46 && sType <= ITEM_HELPER + 48)
		|| (sType >= ITEM_HELPER + 125 && sType <= ITEM_HELPER + 127)
		|| (sType == ITEM_POTION + 54)
		|| (sType >= ITEM_POTION + 58 && sType <= ITEM_POTION + 62)
		|| (sType == ITEM_POTION + 53)
		|| (sType >= ITEM_POTION + 70 && sType <= ITEM_POTION + 71)
		|| (sType >= ITEM_POTION + 72 && sType <= ITEM_POTION + 77)
		|| (sType >= ITEM_TYPE_CHARM_MIXWING + EWS_BEGIN
			&& sType <= ITEM_TYPE_CHARM_MIXWING + EWS_END)
		|| (sType == ITEM_HELPER + 59)
		|| (sType >= ITEM_HELPER + 54 && sType <= ITEM_HELPER + 58)
		|| (sType >= ITEM_POTION + 78 && sType <= ITEM_POTION + 82)
		|| (sType == ITEM_HELPER + 60)
		|| (sType == ITEM_HELPER + 61)
		|| (sType == ITEM_POTION + 91)
		|| (sType == ITEM_POTION + 94)
		|| (sType >= ITEM_POTION + 92 && sType <= ITEM_POTION + 93)
		|| (sType == ITEM_POTION + 95)
		|| (sType >= ITEM_HELPER + 62 && sType <= ITEM_HELPER + 63)
		|| (sType >= ITEM_POTION + 97 && sType <= ITEM_POTION + 98)
		|| (sType == ITEM_POTION + 96)
		|| (sType == ITEM_HELPER + 64 || sType == ITEM_HELPER + 65)
		|| (sType == ITEM_HELPER + 67)
		|| (sType == ITEM_HELPER + 68)
		|| (sType == ITEM_HELPER + 76)
		|| (sType == ITEM_HELPER + 122)
		|| (sType == ITEM_HELPER + 69)
		|| (sType == ITEM_HELPER + 70)
		|| (sType == ITEM_HELPER + 71 || sType == ITEM_HELPER + 72 || sType == ITEM_HELPER + 73 || sType == ITEM_HELPER + 74 || sType == ITEM_HELPER + 75)
		|| (sType == ITEM_HELPER + 80)
		|| (sType == ITEM_HELPER + 106)
		|| sType == ITEM_HELPER + 81
		|| sType == ITEM_HELPER + 82
		|| sType == ITEM_HELPER + 93
		|| sType == ITEM_HELPER + 94
		|| sType == ITEM_HELPER + 121
		|| (sType >= ITEM_POTION + 145 && sType <= ITEM_POTION + 150)
#ifdef LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
		|| g_pMyInventory->IsInvenItem(sType)
#endif //LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
		|| (sType == ITEM_POTION + 133)
		)
	{
		return false;
	}

	return true;
}

void RequireClass(Script_Item* pItem)
{
	if (pItem == NULL)
		return;

	BYTE byFirstClass = gCharacterManager.GetBaseClass(Hero->Class);
	BYTE byStepClass = gCharacterManager.GetStepClass(Hero->Class);

	int iTextColor = 0;

	TextListColor[TextNum + 2] = TextListColor[TextNum + 3] = TEXT_COLOR_WHITE;
	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	int iCount = 0;
	for (int i = 0; i < MAX_CLASS; ++i)
	{
		if (pItem->RequireClass[i] == 1)
		{
			iCount++;
		}
	}
	if (iCount == MAX_CLASS)
		return;

	for (int i = 0; i < MAX_CLASS; ++i)
	{
		BYTE byRequireClass = pItem->RequireClass[i];

		if (byRequireClass == 0)
			continue;

		if (i == byFirstClass && byRequireClass <= byStepClass)
		{
			iTextColor = TEXT_COLOR_WHITE;
		}
		else
		{
			iTextColor = TEXT_COLOR_DARKRED;
		}

		switch (i)
		{
		case CLASS_WIZARD:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[20]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 2)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[25]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1669]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
		case CLASS_KNIGHT:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[21]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 2)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[26]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1668]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
		case CLASS_ELF:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[22]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 2)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[27]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1670]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
		case CLASS_DARK:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[23]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1671]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
		case CLASS_DARK_LORD:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[24]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1672]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
		case CLASS_SUMMONER:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1687]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 2)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1688]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[1689]);
				TextListColor[TextNum] = iTextColor;
			}

			TextBold[TextNum] = false;	TextNum++;
		}
		break;
#ifdef PBG_ADD_NEWCHAR_MONK
		case CLASS_RAGEFIGHTER:
		{
			if (byRequireClass == 1)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[3150]);
				TextListColor[TextNum] = iTextColor;
			}
			else if (byRequireClass == 3)
			{
				sprintf(TextList[TextNum], GlobalText[61], GlobalText[3151]);
				TextListColor[TextNum] = iTextColor;
			}
			TextBold[TextNum] = false;	TextNum++;
		}
		break;
#endif //PBG_ADD_NEWCHAR_MONK
		}
	}
}

const int iMaxLevel = 15;
const int iMaxColumn = 17;
int g_iCurrentItem = -1;
int g_iItemInfo[iMaxLevel + 1][iMaxColumn];

void RenderHelpLine(int iColumnType, const char* pPrintStyle, int& TabSpace, const char* pGapText, int Pos_y, int iType)
{
	int iCurrMaxLevel = iMaxLevel;

	if (iType == 5)
		iCurrMaxLevel = 0;

	for (int Level = 0; Level <= iCurrMaxLevel; ++Level)
	{
		sprintf(TextList[TextNum], pPrintStyle, g_iItemInfo[Level][iColumnType]);
		if (g_iItemInfo[Level][_COLUMN_TYPE_CAN_EQUIP] == TRUE)
		{
			TextListColor[Level] = TEXT_COLOR_WHITE;
		}
		else
		{
			TextListColor[Level] = TEXT_COLOR_RED;
		}
		TextBold[Level] = false;
		++TextNum;
	}

	SIZE TextSize;
	RenderTipTextList(TabSpace, Pos_y, TextNum, 0, RT3_SORT_CENTER, FALSE);

	if (pGapText == NULL)
	{
		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[TextNum - 1], lstrlen(TextList[TextNum - 1]), &TextSize);
	}
	else
	{
		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), pGapText, lstrlen(pGapText), &TextSize);
	}
	TabSpace += int(TextSize.cx / g_fScreenRate_x);
	if (iType == 6)
	{
		TabSpace += 5;
	}
	TextNum -= iCurrMaxLevel + 1;
}

void RenderHelpCategory(int iColumnType, int Pos_x, int Pos_y)
{
	const char* pText = NULL;

	switch (iColumnType)
	{
	case _COLUMN_TYPE_LEVEL:
		pText = GlobalText[161];
		break;
	case _COLUMN_TYPE_ATTMIN: case _COLUMN_TYPE_ATTMAX:
		pText = GlobalText[162];
		break;
	case _COLUMN_TYPE_MAGIC:
		pText = GlobalText[163];
		break;
	case _COLUMN_TYPE_CURSE:
		pText = GlobalText[1144];
		break;
	case _COLUMN_TYPE_PET_ATTACK:
		pText = GlobalText[1239];
		break;
	case _COLUMN_TYPE_DEFENCE:
		pText = GlobalText[164];
		break;
	case _COLUMN_TYPE_DEFRATE:
		pText = GlobalText[165];
		break;
	case _COLUMN_TYPE_REQSTR:
		pText = GlobalText[166];
		break;
	case _COLUMN_TYPE_REQDEX:
		pText = GlobalText[167];
		break;
	case _COLUMN_TYPE_REQENG:
		pText = GlobalText[168];
		break;
	case _COLUMN_TYPE_REQCHA:
		pText = GlobalText[1900];
		break;
	case _COLUMN_TYPE_REQVIT:
		pText = GlobalText[169];
		break;
	case _COLUMN_TYPE_REQNLV:
		pText = GlobalText[1931];
		break;
	default:
		break;
	}
	if (pText != NULL)
	{
		sprintf(TextList[TextNum], pText);
		TextListColor[TextNum] = TEXT_COLOR_BLUE;
		TextNum++;
	}
	RenderTipTextList(Pos_x, Pos_y, TextNum, 0, RT3_SORT_RIGHT, FALSE);
	TextNum = 0;
}

void ComputeItemInfo(int iHelpItem)
{
	if (g_iCurrentItem == iHelpItem)
		return;
	else
		g_iCurrentItem = iHelpItem;

	Script_Item* p = GMItemMng->find(ItemHelp);

	for (int Level = 0; Level <= iMaxLevel; Level++)
	{
		int RequireStrength = 0;
		int RequireDexterity = 0;
		int RequireEnergy = 0;
		int RequireCharisma = 0;
		int RequireVitality = 0;
		int RequireLevel = 0;
		int DamageMin = p->DamageMin;
		int DamageMax = p->DamageMax;
		int Defense = p->Defense;
		int Magic = p->MagicPower;
		int Blocking = p->SuccessfulBlocking;

		if (DamageMin > 0)
		{
			DamageMin += (min(9, Level) * 3);
			switch (Level - 9)
			{
			case 6: DamageMin += 9; break;	// +15
			case 5: DamageMin += 8; break;	// +14
			case 4: DamageMin += 7; break;	// +13
			case 3: DamageMin += 6; break;	// +12
			case 2: DamageMin += 5; break;	// +11
			case 1: DamageMin += 4; break;	// +10
			default: break;
			};
		}
		if (DamageMax > 0)
		{
			DamageMax += (min(9, Level) * 3);
			switch (Level - 9)
			{
			case 6: DamageMax += 9; break;	// +15
			case 5: DamageMax += 8; break;	// +14
			case 4: DamageMax += 7; break;	// +13
			case 3: DamageMax += 6; break;	// +12
			case 2: DamageMax += 5; break;	// +11
			case 1: DamageMax += 4; break;	// +10
			default: break;
			};
		}

		if (Magic > 0)
		{
			Magic += (min(9, Level) * 3);	// ~ +9
			switch (Level - 9)
			{
			case 6: Magic += 9; break;		// +15
			case 5: Magic += 8; break;		// +14
			case 4: Magic += 7; break;	    // +13
			case 3: Magic += 6; break;		// +12
			case 2: Magic += 5; break;		// +11
			case 1: Magic += 4; break;		// +10
			default: break;
			};
			Magic /= 2;

			if (IsCepterItem(ItemHelp) == false)
			{
				Magic += Level * 2;
			}
		}

		if (Defense > 0)
		{
			if (ItemHelp >= ITEM_SHIELD && ItemHelp < ITEM_SHIELD + MAX_ITEM_INDEX)
			{
				Defense += Level;
			}
			else
			{
				Defense += (min(9, Level) * 3);	// ~ +9
				switch (Level - 9)
				{
				case 6: Defense += 9; break;	// +15
				case 5: Defense += 8; break;	// +14
				case 4: Defense += 7; break;	// +13
				case 3: Defense += 6; break;	// +12
				case 2: Defense += 5; break;	// +11
				case 1: Defense += 4; break;	// +10
				default: break;
				};
			}
		}
		if (Blocking > 0)
		{
			Blocking += (min(9, Level) * 3);	// ~ +9
			switch (Level - 9)
			{
			case 6: Blocking += 9; break;	// +15
			case 5: Blocking += 8; break;	// +14
			case 4: Blocking += 7; break;	// +13
			case 3: Blocking += 6; break;	// +12
			case 2: Blocking += 5; break;	// +11
			case 1: Blocking += 4; break;	// +10
			default: break;
			};
		}

		if (p->RequireLevel)
		{
			RequireLevel = p->RequireLevel;
		}
		else
		{
			RequireLevel = 0;
		}

		if (p->RequireStrength)
		{
			RequireStrength = 20 + p->RequireStrength * (p->Level + Level * 3) * 3 / 100;
		}
		else
		{
			RequireStrength = 0;
		}

		if (p->RequireDexterity)	RequireDexterity = 20 + p->RequireDexterity * (p->Level + Level * 3) * 3 / 100;
		else RequireDexterity = 0;

		if (p->RequireVitality)	RequireVitality = 20 + p->RequireVitality * (p->Level + Level * 3) * 3 / 100;
		else RequireVitality = 0;

		if (p->RequireEnergy)
		{
			if (ItemHelp >= ITEM_STAFF + 21 && ItemHelp <= ITEM_STAFF + 29)
			{
				RequireEnergy = 20 + p->RequireEnergy * (p->Level + Level * 1) * 3 / 100;
			}
			else
			{
				if ((p->RequireLevel > 0) && (ItemHelp >= ITEM_ETC && ItemHelp < ITEM_ETC + MAX_ITEM_INDEX))
				{
					RequireEnergy = 20 + (p->RequireEnergy) * (p->RequireLevel) * 4 / 100;
				}
				else
				{
					RequireEnergy = 20 + p->RequireEnergy * (p->Level + Level * 3) * 4 / 100;
				}
			}
		}
		else
		{
			RequireEnergy = 0;
		}

		if (p->RequireCharisma)	RequireCharisma = 20 + p->RequireCharisma * (p->Level + Level * 3) * 3 / 100;
		else RequireCharisma = 0;

		g_iItemInfo[Level][_COLUMN_TYPE_LEVEL] = Level;
		g_iItemInfo[Level][_COLUMN_TYPE_ATTMIN] = DamageMin;
		g_iItemInfo[Level][_COLUMN_TYPE_ATTMAX] = DamageMax;

		if (ItemHelp >= ITEM_STAFF + 21 && ItemHelp <= ITEM_STAFF + 29)
		{
			g_iItemInfo[Level][_COLUMN_TYPE_CURSE] = Magic;
		}
		else
		{
			g_iItemInfo[Level][_COLUMN_TYPE_MAGIC] = Magic;
		}

		g_iItemInfo[Level][_COLUMN_TYPE_PET_ATTACK] = Magic;
		g_iItemInfo[Level][_COLUMN_TYPE_DEFENCE] = Defense;
		g_iItemInfo[Level][_COLUMN_TYPE_DEFRATE] = Blocking;
		g_iItemInfo[Level][_COLUMN_TYPE_REQSTR] = RequireStrength;
		g_iItemInfo[Level][_COLUMN_TYPE_REQDEX] = RequireDexterity;
		g_iItemInfo[Level][_COLUMN_TYPE_REQENG] = RequireEnergy;
		g_iItemInfo[Level][_COLUMN_TYPE_REQCHA] = RequireCharisma;
		g_iItemInfo[Level][_COLUMN_TYPE_REQVIT] = RequireVitality;
		g_iItemInfo[Level][_COLUMN_TYPE_REQNLV] = RequireLevel;

		if (IsCepterItem(ItemHelp) == true)

		{
			g_iItemInfo[Level][_COLUMN_TYPE_MAGIC] = 0;
		}
		else
		{
			g_iItemInfo[Level][_COLUMN_TYPE_PET_ATTACK] = 0;
		}

		WORD Strength, Dexterity, Energy, Vitality, Charisma;

		Strength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;
		Dexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
		Energy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
		Vitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;
		Charisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;

		if (RequireStrength <= Strength
			&& RequireDexterity <= Dexterity
			&& RequireEnergy <= Energy
			&& RequireVitality <= Vitality
			&& RequireCharisma <= Charisma
			&& RequireLevel <= CharacterAttribute->Level)
			g_iItemInfo[Level][_COLUMN_TYPE_CAN_EQUIP] = TRUE;
		else
			g_iItemInfo[Level][_COLUMN_TYPE_CAN_EQUIP] = FALSE;
	}
}

unsigned int getGoldColor(DWORD Gold)
{
	if (Gold >= 10000000)
	{
		return  (255 << 24) + (0 << 16) + (0 << 8) + (255);
	}
	else if (Gold >= 1000000)
	{
		return  (255 << 24) + (0 << 16) + (150 << 8) + (255);
	}
	else if (Gold >= 100000)
	{
		return  (255 << 24) + (24 << 16) + (201 << 8) + (0);
	}

	return  (255 << 24) + (150 << 16) + (220 << 8) + (255);
}

void ConvertGold(double dGold, unicode::t_char* szText, int iDecimals /*= 0*/)
{
	unicode::t_char szTemp[256];
	int iCipherCnt = 0;
	DWORD dwValueTemp = (DWORD)dGold;

	while (dwValueTemp / 1000 > 0)
	{
		iCipherCnt = iCipherCnt + 3;
		dwValueTemp = dwValueTemp / 1000;
	}

	unicode::_sprintf(szText, "%d", dwValueTemp);

	while (iCipherCnt > 0)
	{
		dwValueTemp = (DWORD)dGold;
		dwValueTemp = (dwValueTemp % (int)pow(10.f, (float)iCipherCnt)) / (int)pow(10.f, (float)(iCipherCnt - 3));
		unicode::_sprintf(szTemp, ",%03d", dwValueTemp);
		strcat(szText, szTemp);
		iCipherCnt = iCipherCnt - 3;
	}

	if (iDecimals > 0)
	{
		dwValueTemp = (int)(dGold * pow(10.f, (float)iDecimals)) % (int)pow(10.f, (float)iDecimals);
		unicode::_sprintf(szTemp, ".%d", dwValueTemp);
		strcat(szText, szTemp);
	}
}

void ConvertGold64(__int64 Gold, unicode::t_char* Text)
{
	int Gold1 = Gold % 1000;
	int Gold2 = Gold % 1000000 / 1000;
	int Gold3 = Gold % 1000000000 / 1000000;
	int Gold4 = Gold % 1000000000000 / 1000000000;
	int Gold5 = Gold % 1000000000000000 / 1000000000000;
	int Gold6 = Gold / 1000000000000000;
	if (Gold >= 1000000000000000)
		unicode::_sprintf(Text, "%d,%03d,%03d,%03d,%03d,%03d", Gold6, Gold5, Gold4, Gold3, Gold2, Gold1);
	else if (Gold >= 1000000000000)
		unicode::_sprintf(Text, "%d,%03d,%03d,%03d,%03d", Gold5, Gold4, Gold3, Gold2, Gold1);
	else if (Gold >= 1000000000)
		unicode::_sprintf(Text, "%d,%03d,%03d,%03d", Gold4, Gold3, Gold2, Gold1);
	else if (Gold >= 1000000)
		unicode::_sprintf(Text, "%d,%03d,%03d", Gold3, Gold2, Gold1);
	else if (Gold >= 1000)
		unicode::_sprintf(Text, "%d,%03d", Gold2, Gold1);
	else
		unicode::_sprintf(Text, "%d", Gold1);
}

void ConvertTaxGold(DWORD Gold, char* Text)
{
	Gold += ((LONGLONG)Gold * g_pNPCShop->GetTaxRate()) / 100;

	int Gold1 = Gold % 1000;
	int Gold2 = Gold % 1000000 / 1000;
	int Gold3 = Gold % 1000000000 / 1000000;
	int Gold4 = Gold / 1000000000;
	if (Gold >= 1000000000)
		sprintf(Text, "%d,%03d,%03d,%03d", Gold4, Gold3, Gold2, Gold1);
	else if (Gold >= 1000000)
		sprintf(Text, "%d,%03d,%03d", Gold3, Gold2, Gold1);
	else if (Gold >= 1000)
		sprintf(Text, "%d,%03d", Gold2, Gold1);
	else
		sprintf(Text, "%d", Gold1);
}

void ConvertChaosTaxGold(DWORD Gold, char* Text)
{
	Gold += ((LONGLONG)Gold * g_nChaosTaxRate) / 100;

	int Gold1 = Gold % 1000;
	int Gold2 = Gold % 1000000 / 1000;
	int Gold3 = Gold % 1000000000 / 1000000;
	int Gold4 = Gold / 1000000000;
	if (Gold >= 1000000000)
		sprintf(Text, "%d,%03d,%03d,%03d", Gold4, Gold3, Gold2, Gold1);
	else if (Gold >= 1000000)
		sprintf(Text, "%d,%03d,%03d", Gold3, Gold2, Gold1);
	else if (Gold >= 1000)
		sprintf(Text, "%d,%03d", Gold2, Gold1);
	else
		sprintf(Text, "%d", Gold1);
}

int ConvertRepairGold(int Gold, int Durability, int MaxDurability, short Type, char* Text)
{
	Gold = min(Gold, 400000000);

	float   repairGold = (float)Gold;
	float   persent = 1.f - (float)(Durability / (float)MaxDurability);
	bool    doubleP = false;

	if (persent > 0)
	{
		float fRoot = (float)sqrt((double)repairGold);
		float fRootRoot = (float)sqrt(sqrt((double)repairGold));
		repairGold = 3.f * fRoot * fRootRoot;

		if (doubleP)
		{
			repairGold *= 2;
		}

		repairGold *= persent;
		repairGold++;

		if (Durability <= 0)
		{
			if (Type == ITEM_HELPER + 4 || Type == ITEM_HELPER + 5)
			{
				repairGold *= 2;
			}
			else
			{
				repairGold += repairGold * 0.4f;
			}
		}
	}
	else
	{
		repairGold = 0;
	}

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP) == true && g_pNPCShop->IsRepairShop())
	{
		Gold = (int)(repairGold);
	}
	else if ((g_pMyInventory->IsVisible()) && (!g_pNPCShop->IsVisible()))
	{
		Gold = (int)(repairGold + (repairGold * 1.5f));
	}
	else
	{
		Gold = (int)(repairGold);
	}

	if (Gold >= 1000)
	{
		Gold = (Gold / 100) * 100;
	}
	else if (Gold >= 100)
	{
		Gold = (Gold / 10) * 10;
	}

	ConvertGold(Gold, Text);

	return  Gold;
}

void RepairAllGold(void)
{
	char    text[100];

	AllRepairGold = 0;

	for (int i = 0; i < MAX_EQUIPMENT; ++i)
	{
		if (CharacterMachine->Equipment[i].Type != -1)
		{
			ITEM* ip = &CharacterMachine->Equipment[i];
			Script_Item* p = GMItemMng->find(ip->Type);

			int Level = (ip->Level >> 3) & 15;
			int maxDurability = calcMaxDurability(ip, p, Level);

			if (IsRepairBan(ip) == true)
			{
				continue;
			}

			//. check durability
			if (ip->Durability < maxDurability)
			{
				int gold = ConvertRepairGold(ItemValue(ip, 2), ip->Durability, maxDurability, ip->Type, text);

				if (Check_LuckyItem(ip->Type))
					gold = 0;
				AllRepairGold += gold;
			}
		}
	}

	ITEM* pItem = NULL;

	for (int i = 0; i < (int)(g_pMyInventory->GetNumberOfItems()); ++i)
	{
		pItem = g_pMyInventory->GetItem(i);

		if (pItem)
		{
			Script_Item* p = GMItemMng->find(pItem->Type);

			int Level = (pItem->Level >> 3) & 15;
			int maxDurability = calcMaxDurability(pItem, p, Level);

			if (pItem->Type >= ITEM_POTION + 55 && pItem->Type <= ITEM_POTION + 57)
			{
				continue;
			}
			//. item filtering
			if ((pItem->Type >= ITEM_HELPER && pItem->Type <= ITEM_HELPER + 5) || pItem->Type == ITEM_HELPER + 10 || pItem->Type == ITEM_HELPER + 31)
				continue;
			if (pItem->Type == ITEM_BOW + 7 || pItem->Type == ITEM_BOW + 15 || pItem->Type >= ITEM_POTION)
				continue;
			if (pItem->Type >= ITEM_WING + 7 && pItem->Type <= ITEM_WING + 19)
				continue;
			if ((pItem->Type >= ITEM_HELPER + 14 && pItem->Type <= ITEM_HELPER + 19) || pItem->Type == ITEM_POTION + 21)
				continue;
			if (pItem->Type == ITEM_HELPER + 20)
				continue;
			if (pItem->Type == ITEM_HELPER + 38)
				continue;

			if (pItem->Type >= ITEM_HELPER + 46 && pItem->Type <= ITEM_HELPER + 48)
			{
				continue;
			}
			if (pItem->Type >= ITEM_HELPER + 125 && pItem->Type <= ITEM_HELPER + 127)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 145 && pItem->Type <= ITEM_POTION + 150)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 58 && pItem->Type <= ITEM_POTION + 62)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 53)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 43 || pItem->Type == ITEM_HELPER + 44 || pItem->Type == ITEM_HELPER + 45)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 70 && pItem->Type <= ITEM_POTION + 71)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 72 && pItem->Type <= ITEM_POTION + 77)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 59)
			{
				continue;
			}
			if (pItem->Type >= ITEM_HELPER + 54 && pItem->Type <= ITEM_HELPER + 58)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 60)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 61)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 91)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 92 && pItem->Type <= ITEM_POTION + 93)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 95)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 95)
			{
				continue;
			}
			if (pItem->Type >= ITEM_HELPER + 62 && pItem->Type <= ITEM_HELPER + 63)
			{
				continue;
			}
			if (pItem->Type >= ITEM_POTION + 97 && pItem->Type <= ITEM_POTION + 98)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 140)
			{
				continue;
			}
			if (pItem->Type == ITEM_POTION + 96)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 64 || pItem->Type == ITEM_HELPER + 65)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 67)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 80)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 106)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 123)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 68)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 76)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 122)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 69)
				continue;
			if (pItem->Type == ITEM_HELPER + 70)
				continue;

			if (pItem->Type == ITEM_HELPER + 37)
				continue;

			if (pItem->Type == ITEM_HELPER + 66)
				continue;

			if (pItem->Type == ITEM_HELPER + 71
				|| pItem->Type == ITEM_HELPER + 72
				|| pItem->Type == ITEM_HELPER + 73
				|| pItem->Type == ITEM_HELPER + 74
				|| pItem->Type == ITEM_HELPER + 75)
				continue;

			if (pItem->Type == ITEM_HELPER + 81)
				continue;
			if (pItem->Type == ITEM_HELPER + 82)
				continue;
			if (pItem->Type == ITEM_HELPER + 93)
				continue;
			if (pItem->Type == ITEM_HELPER + 94)
				continue;

			if (pItem->Type >= ITEM_TYPE_CHARM_MIXWING + EWS_BEGIN && pItem->Type <= ITEM_TYPE_CHARM_MIXWING + EWS_END)
			{
				continue;
			}
			if (pItem->Type == ITEM_HELPER + 97 || pItem->Type == ITEM_HELPER + 98 || pItem->Type == ITEM_POTION + 91)
				continue;

			if (pItem->Type == ITEM_HELPER + 121)
				continue;

#ifdef LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
			if (g_pMyInventory->IsInvenItem(pItem->Type))
				continue;

#endif //LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY

			if (pItem->Type >= ITEM_WING + 130 && pItem->Type <= ITEM_WING + 134)
				continue;
			if (pItem->Type == ITEM_HELPER + 109)
				continue;
			if (pItem->Type == ITEM_HELPER + 110)
				continue;
			if (pItem->Type == ITEM_HELPER + 111)
				continue;
			if (pItem->Type == ITEM_HELPER + 112)
				continue;
			if (pItem->Type == ITEM_HELPER + 113)
				continue;
			if (pItem->Type == ITEM_HELPER + 114)
				continue;
			if (pItem->Type == ITEM_HELPER + 115)
				continue;
			if (pItem->Type == ITEM_HELPER + 107)
				continue;
			if (Check_ItemAction(pItem, eITEM_REPAIR))
				continue;

			//. check durability
			if (pItem->Durability < maxDurability)
			{
				int gold = ConvertRepairGold(ItemValue(pItem, 2), pItem->Durability, maxDurability, pItem->Type, text);

				if (Check_LuckyItem(pItem->Type))
					gold = 0;

				AllRepairGold += gold;
			}
		}
	}
}


int InventoryStartX;
int InventoryStartY;
int ShopInventoryStartX;
int ShopInventoryStartY;
int TradeInventoryStartX;
int TradeInventoryStartY;
int CharacterInfoStartX;
int CharacterInfoStartY;
int GuildStartX;
int GuildStartY;
int GuildListStartX;
int GuildListStartY;
int SommonTable[] = { 2,7,14,8,9,41 };

char ChaosEventName[][100] = {
	"È÷µÅ© °íÇâ ¿©Çà±Ç",
	"ÆæÆ¼¾ö4 ÄÄÇ»ÅÍ",
	"µðÁöÅ»Ä«¸Þ¶ó",
	"·ÎÁöÅØ ¹«¼± ¸¶¿ì½º+Å°º¸µå ¼¼Æ®",
	"256M ·¥",
	"6°³¿ ÀâÁö ±¸µ¶±Ç",
	"¹®È­»óÇ°±Ç(¸¸¿ø)",
	"¹Â ¸Ó±×ÄÅ",
	"¹Â T¼ÅÃ÷",
	"¹Â 10½Ã°£ ¹«·áÀÌ¿ë±Ç"
};

WORD calcMaxDurability(const ITEM* pItem, Script_Item* item_info, int Level)
{
	WORD maxDurability = item_info->Durability;

	if (pItem->Type >= ITEM_STAFF && pItem->Type < ITEM_STAFF + MAX_ITEM_INDEX)
	{
		maxDurability = item_info->MagicDur;
	}
	for (int i = 0; i < Level && pItem->Type < (ITEM_HELPER + 51); ++i)
	{
		if (i < 14)
		{
			if (i < 13)
			{
				if (i < 12)
				{
					if (i < 11)
					{
						if (i < 10)
						{
							if (i < 9)
							{
								if (i < 4)
									++maxDurability;
								else
									maxDurability += 2;
							}
							else
							{
								maxDurability += 3;
							}
						}
						else
						{
							maxDurability += 4;
						}
					}
					else
					{
						maxDurability += 5;
					}
				}
				else
				{
					maxDurability += 6;
				}
			}
			else
			{
				if (maxDurability + 7 < 255)
					maxDurability += 7;
				else
					maxDurability = 255;
			}
		}
		else
		{
			if (maxDurability + 8 < 255)
				maxDurability += 8;
			else
				maxDurability = 255;
		}
	}

	if (pItem->Type == ITEM_HELPER + 4 || pItem->Type == ITEM_HELPER + 5)
	{
		maxDurability = 255;
	}

	if ((pItem->ExtOption % 0x04) != EXT_A_SET_OPTION && (pItem->ExtOption % 0x04) != EXT_B_SET_OPTION)
	{
		if ((pItem->Option1 & 0x3F) > 0
			&& (pItem->Type < ITEM_WING + 3 || pItem->Type > ITEM_WING + 6)
			&& pItem->Type != ITEM_SWORD + 19
			&& pItem->Type != ITEM_BOW + 18
			&& pItem->Type != ITEM_STAFF + 10
			&& pItem->Type != ITEM_HELPER + 30
			&& (pItem->Type < ITEM_WING + 36 || pItem->Type > ITEM_WING + 40)
			&& (pItem->Type < ITEM_WING + 42 || pItem->Type > ITEM_WING + 43)
			&& pItem->Type != ITEM_MACE + 13
			&& (pItem->Type < ITEM_WING + 49 || pItem->Type > ITEM_WING + 50)
			&& pItem->Type != ITEM_STAFF + 36)
		{
			maxDurability += 15;
		}
	}
	else
	{
		maxDurability += 20;
	}

	if (Check_LuckyItem(pItem->Type))
	{
		maxDurability = 255;
	}

	return  maxDurability;
}

void GetItemName(int iType, int iLevel, char* Text)
{
	Script_Item* p = GMItemMng->find(iType);

	if (iType >= ITEM_POTION + 23 && iType <= ITEM_POTION + 26)
	{
		if (iType == ITEM_POTION + 23)
		{
			switch (iLevel)
			{
			case 0:
				sprintf(Text, "%s", p->Name);
				break;
			case 1:
				sprintf(Text, "%s", GlobalText[906]);
				break;
			}
		}
		else if (iType == ITEM_POTION + 24)
		{
			switch (iLevel)
			{
			case 0: sprintf(Text, "%s", p->Name); break;
			case 1: sprintf(Text, "%s", GlobalText[907]); break;
			}
		}
		else
		{
			sprintf(Text, "%s", p->Name);
		}
	}
	else if (iType == ITEM_POTION + 12)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", GlobalText[100]); break; //Zen
		case 1:	sprintf(Text, "%s", GlobalText[101]); break; //Trái tim
		case 2:	sprintf(Text, "%s", ChaosEventName[p->Durability]); break;
		}
	}
	else if (iType == ITEM_POTION + 11)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1: sprintf(Text, "%s", GlobalText[105]); break; //Ngôi sao may mắn
		case 2: sprintf(Text, "%s", GlobalText[106]); break; //Pháo hoa
		case 3: sprintf(Text, "%s", GlobalText[107]); break; //Trái tim tình yêu
		case 5: sprintf(Text, "%s", GlobalText[109]); break; //Huy chương Bạc
		case 6: sprintf(Text, "%s", GlobalText[110]); break; //Huy chương Vàng
		case 7: sprintf(Text, "%s", GlobalText[111]); break; //Hộp của Vua
			break;
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
			sprintf(Text, "%s +%d", GlobalText[115], iLevel - 7);  //Hộp Kundun
			break;
		case 13:
			sprintf(Text, GlobalText[117]); break; //Trái tim Chúa tể
		case 14:
			sprintf(Text, GlobalText[1650]); break; //Bình tăng Mana
			break;

		case 15:
			sprintf(Text, GlobalText[1651]); break; //Bình tăng HP
			break;
		}
	}
	else if (iType == ITEM_HELPER + 15)
	{
		switch (iLevel)
		{
		case 0:sprintf(Text, "%s %s", GlobalText[168], p->Name); break;
		case 1:sprintf(Text, "%s %s", GlobalText[169], p->Name); break;
		case 2:sprintf(Text, "%s %s", GlobalText[167], p->Name); break;
		case 3:sprintf(Text, "%s %s", GlobalText[166], p->Name); break;
		case 4:sprintf(Text, "%s %s", GlobalText[1900], p->Name); break;
		}
	}
	else if (iType == ITEM_HELPER + 14)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1: sprintf(Text, "%s", GlobalText[1235]); break; //Huy hiệu Hoàng Tộc
		}
	}
	else if (iType == ITEM_HELPER + 31)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s %s", GlobalText[1187], p->Name); break; //Chiến Mã
		case 1: sprintf(Text, "%s %s", GlobalText[1214], p->Name); break; //Quạ Tinh
		}
	}
	else if (iType == ITEM_POTION + 21)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1: sprintf(Text, "%s", GlobalText[810]); break;
		case 2: sprintf(Text, "%s", GlobalText[1098]); break;
		case 3: sprintf(Text, "%s", GlobalText[1290]); break;
		}
	}
	else if (iType == ITEM_HELPER + 19)
	{
		sprintf(Text, "%s", GlobalText[809]);
	}
	else if (iType == ITEM_HELPER + 20)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1: sprintf(Text, "%s", GlobalText[922]); break;
		case 2: sprintf(Text, "%s", GlobalText[928]); break;
		case 3: sprintf(Text, "%s", GlobalText[929]); break;
		}
	}
	else if (iType == ITEM_POTION + 9)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1:	sprintf(Text, "%s", GlobalText[108]); break;
		}
	}
	else if (iType == ITEM_WING + 11)
	{
		sprintf(Text, "%s %s", SkillAttribute[30 + iLevel].Name, GlobalText[102]);
	}
	else if (iType == ITEM_WING + 32)
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType == ITEM_WING + 33)
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType == ITEM_WING + 34)
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType == ITEM_WING + 35)
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType >= ITEM_POTION + 45 && iType <= ITEM_POTION + 50)
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType == ITEM_POTION + 32)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1:	sprintf(Text, "%s", GlobalText[2012]); break;
		}
	}
	else if (iType == ITEM_POTION + 33)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1:	sprintf(Text, "%s", GlobalText[2013]); break;
		}
	}
	else if (iType == ITEM_POTION + 34)
	{
		switch (iLevel)
		{
		case 0: sprintf(Text, "%s", p->Name); break;
		case 1:	sprintf(Text, "%s", GlobalText[2014]); break;
		}
	}
	else if (iType == ITEM_HELPER + 10)
	{
		for (int i = 0; i < MAX_MONSTER; i++)
		{
			if (SommonTable[iLevel] == MonsterScript[i].Type)
			{
				sprintf(Text, "%s %s", MonsterScript[i].Name, GlobalText[103]);
			}
		}
	}
	else if (iType >= ITEM_WING + 3 && iType <= ITEM_WING + 6)
	{
		if (iLevel == 0)
			sprintf(Text, "%s", p->Name);
		else
			sprintf(Text, "%s +%d", p->Name, iLevel);
	}
	else if ((iType >= ITEM_WING + 36 && iType <= ITEM_WING + 40) || (iType >= ITEM_WING + 42 && iType <= ITEM_WING + 43) || (iType == ITEM_WING + 50))
	{
		if (iLevel == 0)
			sprintf(Text, "%s", p->Name);
		else
			sprintf(Text, "%s +%d", p->Name, iLevel);
	}
	else if (iType == ITEM_SWORD + 19 || iType == ITEM_BOW + 18 || iType == ITEM_STAFF + 10 || iType == ITEM_MACE + 13)
	{
		if (iLevel == 0)
			sprintf(Text, "%s", p->Name);
		else
			sprintf(Text, "%s +%d", p->Name, iLevel);
	}
	else if (COMGEM::NOGEM != COMGEM::Check_Jewel_Com(iType))
	{
		sprintf(Text, "%s +%d", p->Name, iLevel + 1);
	}
	else if (iType == INDEX_COMPILED_CELE)
	{
		sprintf(Text, "%s +%d", GlobalText[1806], iLevel + 1);
	}
	else if (iType == INDEX_COMPILED_SOUL)
	{
		sprintf(Text, "%s +%d", GlobalText[1807], iLevel + 1);
	}
	else if ((iType >= ITEM_WING + 60 && iType <= ITEM_WING + 65)
		|| (iType >= ITEM_WING + 70 && iType <= ITEM_WING + 74)
		|| (iType >= ITEM_WING + 100 && iType <= ITEM_WING + 129))
	{
		sprintf(Text, "%s", p->Name);
	}
	else if (iType == ITEM_POTION + 7)
	{
		int iTextIndex = 0;
		iTextIndex = (iLevel == 0) ? 1413 : 1414;
		sprintf(Text, "%s", GlobalText[iTextIndex]);
	}
	else
	{
		if (iLevel == 0)
			sprintf(Text, "%s", p->Name);
		else
			sprintf(Text, "%s +%d", p->Name, iLevel);
	}
}

void GetSpecialOptionText(int Type, char* Text, WORD Option, BYTE Value, int iMana)
{
	switch (Option)
	{
	case AT_IMPROVE_DAMAGE: //-- 80
		sprintf(Text, GlobalText[(88)], Value); // ~Additional Dmg +%d
		break;
	case AT_IMPROVE_MAGIC: //-- 81
		sprintf(Text, GlobalText[(89)], Value); // ~Additional Wizardry Dmg +%d
		break;
	case AT_IMPROVE_CURSE: //-- 82
		sprintf(Text, GlobalText[(1697)], Value); // ~Additional Curse Spell +%d
		break;
	case AT_IMPROVE_BLOCKING: //-- 83
		sprintf(Text, GlobalText[(90)], Value); // ~Additional defense rate +%d
		break;
	case AT_IMPROVE_DEFENSE: //-- 84
		sprintf(Text, GlobalText[(91)], Value); // ~Additional defense +%d
		break;
	case AT_LUCK: //-- 85
		sprintf(Text, GlobalText[(87)]); // ~Luck (success rate of Jewel of Soul +25%%)
		break;
	case AT_LIFE_REGENERATION: //-- 86
		if (Type < ITEM_HELPER + 14 || Type > ITEM_HELPER + 18)
		{
			sprintf(Text, GlobalText[(92)], Value); // ~Automatic HP recovery %d%%
		}
		break;
	case AT_IMPROVE_LIFE: //-- 87
		sprintf(Text, GlobalText[(622)]); // ~Increase Max HP +4%%
		break;
	case AT_IMPROVE_MANA: //-- 88
		sprintf(Text, GlobalText[(623)]); // ~Increase Max Mana +4%%
		break;
	case AT_DECREASE_DAMAGE: //-- 89
		sprintf(Text, GlobalText[(624)]); // ~Damage Decrease +4%%
		break;
	case AT_REFLECTION_DAMAGE:
		sprintf(Text, GlobalText[(625)]); // ~Reflect Damage +5%%
		break;
	case AT_IMPROVE_BLOCKING_PERCENT:
		sprintf(Text, GlobalText[(626)]); // ~Defense success rate +10%%
		break;
	case AT_IMPROVE_GAIN_GOLD:
		sprintf(Text, GlobalText[(627)]); // ~Increases acquisition rate of Zen after hunting monsters +30%%
		break;
	case AT_EXCELLENT_DAMAGE:
		sprintf(Text, GlobalText[(628)]); // ~Excellent Damage rate +10%%
		break;
	case AT_IMPROVE_DAMAGE_LEVEL:
		sprintf(Text, GlobalText[(629)]); // ~Increase Damage +level/20
		break;
	case AT_IMPROVE_DAMAGE_PERCENT:
		sprintf(Text, GlobalText[(630)], Value); // ~Increase Damage +%d%%
		break;
	case AT_IMPROVE_MAGIC_LEVEL:
		sprintf(Text, GlobalText[(631)]); // ~Increase Wizardry Dmg +level/20
		break;
	case AT_IMPROVE_MAGIC_PERCENT:
		sprintf(Text, GlobalText[(632)], Value); // ~Increase Wizardry Damage +%d%%
		break;
	case AT_IMPROVE_ATTACK_SPEED:
		sprintf(Text, GlobalText[(633)], Value); // ~Increase Attacking(Wizardry)speed +%d
		break;
	case AT_IMPROVE_GAIN_LIFE:
		sprintf(Text, GlobalText[(634)]); // ~Increases acquisition rate of Life after hunting monsters +life/8
		break;
	case AT_IMPROVE_GAIN_MANA:
		sprintf(Text, GlobalText[(635)]); // ~Increases acquisition rate of Mana after hunting monsters +Mana/8
		break;
	case AT_IMPROVE_HP_MAX:
		sprintf(Text, GlobalText[(740)], Value); // ~HP +%d increased
		break;
	case AT_IMPROVE_MP_MAX:
		sprintf(Text, GlobalText[(741)], Value); // ~Mana +%d increased
		break;
	case AT_ONE_PERCENT_DAMAGE:
		sprintf(Text, GlobalText[(742)], Value); // ~Ignor opponent's defensive power by %d%%
		break;
	case AT_IMPROVE_AG_MAX:
		sprintf(Text, GlobalText[(743)], Value); // ~Max AG +%d increased
		break;
	case AT_DAMAGE_ABSORB:
		sprintf(Text, GlobalText[(744)], Value); // ~Absorb %d%% additional damage
		break;
	case AT_DAMAGE_REFLECTION:
		sprintf(Text, GlobalText[(1673)], Value); // ~Return's the enemy's attack power in %d%%
		break;
	case AT_RECOVER_FULL_LIFE: //-- 107
		sprintf(Text, GlobalText[(1674)], Value); // ~Complete recovery of life in %d%% rate
		break;
	case AT_RECOVER_FULL_MANA: //-- 108
		sprintf(Text, GlobalText[(1675)], Value); // ~Complete recover of Mana in %d%% rate
		break;
	case AT_IMPROVE_EVADE: //-- 110
		sprintf(Text, GlobalText[(746)]); // ~Parrying 10%% increased
		break;
	case 168u:
		sprintf(Text, GlobalText[(985)], Value); // ~Increase Strength +%d
		break;
	case 169u:
		sprintf(Text, GlobalText[(986)], Value); // ~Increase Agility +%d
		break;
	case 170u:
		sprintf(Text, GlobalText[(988)], Value); // ~Increase Energy +%d
		break;
	case 171u:
		sprintf(Text, GlobalText[(987)], Value); // ~Increase Stamina +%d
		break;
	case 172u:
		sprintf(Text, GlobalText[(954)], Value); // ~Increase Command +%d
		break;
	case 176u:
		sprintf(Text, GlobalText[(577)], Value); // ~Increase %d%% of Damage
		break;
	case 178u:
		sprintf(Text, GlobalText[(959)], Value); // ~Increase Defense +%d
		break;
	case Skill_Defense:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(80)], iMana);
		break;
	case Skill_Uppercut:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(83)], iMana);
		break;
	case Skill_Fire_Breath:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(745)], iMana);
		break;
	case 54u:
	case Skill_Recovery:
	case Skill_Multi_Shot:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(920)], iMana);
		break;
	case Skill_Force_Wave:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(1186)], iMana);
		break;
	case Skill_Plasma_Storm:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(1928)], iMana);
		break;
	case Skill_Potion_of_Soul:
		sprintf(Text, GlobalText[(1087)], Value);
		break;
	case 205u:
		sprintf(Text, GlobalText[(1088)], Value);
		break;
	case Skill_Explosion_Sahamutt:
		sprintf(Text, GlobalText[(1695)], iMana);
		break;
	case Skill_Requiem:
		sprintf(Text, GlobalText[(1696)], iMana);
		break;
	case Skill_Pollution:
		sprintf(Text, GlobalText[(1789)], iMana);
		break;
	case Skill_Phoenix_Shot:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(3158)], iMana);
		break;
	case Skill_Slash:
	case Skill_Slash_Strengthener:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(85)], iMana);
		break;
	case Skill_Falling_Slash:
	case Skill_Falling_Slash_Strengthener:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(81)], iMana);
		break;
	case Skill_Lunge:
	case Skill_Lunge_Strengthener:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(82)], iMana);
		break;
	case Skill_Triple_Shot:
	case Skill_Triple_Shot_Strengthener:
	case Skill_Triple_Shot_Mastery:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(86)], iMana);
		break;
	case Skill_Cyclone:
	case Skill_Cyclone_Strengthener:
	case Skill_Cyclone_Strengthener2:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(84)], iMana);
		break;
	case Skill_Power_Slash:
	case Skill_Power_Slash_Strengthener:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(98)], iMana);
		break;
	case Skill_Force:
	case Skill_Force_Wave_Strengthener:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(1210)], iMana);
		break;
	case Skill_Earthshake:
	case Skill_Dark_Horse_Strengthener:
	case Skill_Earthshake_Strengthener:
	case Skill_Earthshake_Mastery:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(1189)], iMana);
		break;
	case Skill_Killing_Blow:
	case Skill_Killing_Blow_Strengthener:
	case Skill_Killing_Blow_Mastery:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(3153)], iMana);
		break;
	case Skill_Beast_Uppercut:
	case Skill_Beast_Uppercut_Strengthener:
	case Skill_Beast_Uppercut_Mastery:
		gSkillManager.GetSkillInformation(Option, 1, 0, &iMana, 0, 0);
		sprintf(Text, GlobalText[(3154)], iMana);
		break;
	default:
		return;
	}
}

void RenderRepairInfo(int sx, int sy, ITEM* ip, bool Sell)
{
	if (IsRepairBan(ip) == true)
	{
		return;
	}
	if (ip->Type >= MODEL_TYPE_CHARM_MIXWING + EWS_BEGIN && ip->Type <= MODEL_TYPE_CHARM_MIXWING + EWS_END)
	{
		return;
	}
	if (ip->Type == ITEM_HELPER + 107)
	{
		return;
	}
	if (ip->Type == ITEM_HELPER + 104)
	{
		return;
	}
	if (ip->Type == ITEM_HELPER + 105)
	{
		return;
	}
	if (ip->Type == ITEM_HELPER + 103)
	{
		return;
	}
	if (ip->Type == ITEM_POTION + 133)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 109)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 110)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 111)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 112)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 113)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 114)
	{
		return;
	}
	if (ip->Type == MODEL_HELPER + 115)
	{
		return;
	}
	if (ip->Type == MODEL_POTION + 112)
	{
		return;
	}
	if (ip->Type == MODEL_POTION + 113)
	{
		return;
	}
	if (ip->Type == ITEM_POTION + 120)
	{
		return;
	}
	if (ip->Type == ITEM_POTION + 121)
	{
		return;
	}
	if (ip->Type == ITEM_POTION + 122)
	{
		return;
	}
	if (ITEM_POTION + 123 == ip->Type)
	{
		return;
	}
	if (ITEM_POTION + 124 == ip->Type)
	{
		return;
	}
	if (ITEM_POTION + 134 <= ip->Type && ip->Type <= ITEM_POTION + 139)
	{
		return;
	}

	if (ITEM_WING + 130 <= ip->Type &&
		ip->Type <= ITEM_WING + 135
		)
	{
		return;
	}

	if (ITEM_POTION + 114 <= ip->Type && ip->Type <= ITEM_POTION + 119)
	{
		return;
	}
	if (ITEM_POTION + 126 <= ip->Type && ip->Type <= ITEM_POTION + 129)
	{
		return;
	}
	if (ITEM_POTION + 130 <= ip->Type && ip->Type <= ITEM_POTION + 132)
	{
		return;
	}
	if (ITEM_HELPER + 121 == ip->Type)
	{
		return;
	}

	Script_Item* p = GMItemMng->find(ip->Type);

	TextNum = 0;
	SkipNum = 0;
	for (int i = 0; i < 30; i++)
	{
		TextList[i][0] = NULL;
	}

	int Level = (ip->Level >> 3) & 15;
	int Color;

	if (ip->Type == ITEM_POTION + 13 || ip->Type == ITEM_POTION + 14 || ip->Type == ITEM_WING + 15
		|| GMJewelOfAction->CheckOfJewelByItem(ip->Type))
	{
		Color = TEXT_COLOR_YELLOW;
	}
	else if (COMGEM::isCompiledGem(ip))
	{
		Color = TEXT_COLOR_YELLOW;
	}
	else if (ip->Type == ITEM_STAFF + 10 || ip->Type == ITEM_SWORD + 19 || ip->Type == ITEM_BOW + 18 || ip->Type == ITEM_MACE + 13)
	{
		Color = TEXT_COLOR_PURPLE;
	}
	else if (ip->Type == ITEM_POTION + 17 || ip->Type == ITEM_POTION + 18 || ip->Type == ITEM_POTION + 19)
	{
		Color = TEXT_COLOR_YELLOW;
	}
	else if (ip->SpecialNum > 0 && (ip->Option1 & 63) > 0)
	{
		Color = TEXT_COLOR_GREEN;
	}
	else if (Level >= 7)
	{
		Color = TEXT_COLOR_YELLOW;
	}
	else
	{
		if (ip->SpecialNum > 0)
		{
			Color = TEXT_COLOR_BLUE;
		}
		else
		{
			Color = TEXT_COLOR_WHITE;
		}
	}

	if ((ip->Type >= ITEM_WING + 3 && ip->Type <= ITEM_WING + 6)
		|| ip->Type >= ITEM_HELPER + 30
		|| (ip->Type >= ITEM_WING + 36 && ip->Type <= ITEM_WING + 40)
		|| (ip->Type >= ITEM_WING + 42 && ip->Type <= ITEM_WING + 43)
		|| (ip->Type >= ITEM_WING + 49 && ip->Type <= ITEM_WING + 50)
		)
	{
		if (Level >= 7)
		{
			Color = TEXT_COLOR_YELLOW;
		}
		else
		{
			if (ip->SpecialNum > 0)
			{
				Color = TEXT_COLOR_BLUE;
			}
			else
			{
				Color = TEXT_COLOR_WHITE;
			}
		}
	}

	if (ip->Type < ITEM_POTION)
	{
		int maxDurability;

		sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

		char Text[100];

		maxDurability = calcMaxDurability(ip, p, Level);
		if (ip->Durability < maxDurability)
		{
			RepairEnable = 2;

			int iGold = ItemValue(ip, 2);
			if (iGold == -1)
				return;
			ConvertRepairGold(iGold, ip->Durability, maxDurability, ip->Type, Text);
			sprintf(TextList[TextNum], GlobalText[238], Text);

			TextListColor[TextNum] = Color;
		}
		else
		{
			RepairEnable = 1;
			sprintf(TextList[TextNum], GlobalText[238], "0");
			TextListColor[TextNum] = Color;
		}
		TextBold[TextNum] = true;
		TextNum++;

		//        RepairEnable = 1;
	}
	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	if (ip->Type == ITEM_WING + 11)
	{
		sprintf(TextList[TextNum], "%s %s", SkillAttribute[30 + Level].Name, GlobalText[102]);
	}
	else if (ip->Type == ITEM_HELPER + 10)
	{
		for (int i = 0; i < MAX_MONSTER; i++)
		{
			if (SommonTable[Level] == MonsterScript[i].Type)
			{
				sprintf(TextList[TextNum], "%s %s", MonsterScript[i].Name, GlobalText[103]);
				break;
			}
		}
	}
	else if ((ip->Type == ITEM_HELPER + 4) || (ip->Type == ITEM_HELPER + 5))
	{
		sprintf(TextList[TextNum], "%s", p->Name);
	}
	else if ((ip->Type >= ITEM_WING + 3 && ip->Type <= ITEM_WING + 6)
		|| ip->Type >= ITEM_HELPER + 30
		|| (ip->Type >= ITEM_WING + 36 && ip->Type <= ITEM_WING + 40)
		|| (ip->Type >= ITEM_WING + 42 && ip->Type <= ITEM_WING + 43)
		|| (ip->Type >= ITEM_WING + 49 && ip->Type <= ITEM_WING + 50)
		)
	{
		if (Level == 0)
			sprintf(TextList[TextNum], "%s", p->Name);
		else
			sprintf(TextList[TextNum], "%s +%d", p->Name, Level);
	}
	else
	{
		if ((ip->Option1 & 63) > 0)
		{
			if (Level == 0)
				sprintf(TextList[TextNum], "%s %s", GlobalText[620], p->Name);
			else
				sprintf(TextList[TextNum], "%s %s +%d", GlobalText[620], p->Name, Level);
		}
		else
		{
			if (Level == 0)
				sprintf(TextList[TextNum], "%s", p->Name);
			else
				sprintf(TextList[TextNum], "%s +%d", p->Name, Level);
		}
	}
	TextListColor[TextNum] = Color; TextBold[TextNum] = true; TextNum++;
	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	if (ip->Type < ITEM_POTION)
	{
		if (ip->bPeriodItem == false)
		{
			int maxDurability = calcMaxDurability(ip, p, Level);

			sprintf(TextList[TextNum], GlobalText[71], ip->Durability, maxDurability);

			TextListColor[TextNum] = TEXT_COLOR_WHITE;
			TextBold[TextNum] = false;
			TextNum++;
		}
	}

	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	SIZE TextSize = { 0, 0 };

	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[0], 1, &TextSize);

	int Height = ((TextNum - SkipNum) * TextSize.cy + SkipNum * TextSize.cy / 2) * gwinhandle->GetScreenY() / WindowHeight;
	if (sy - Height >= 0)
		sy -= Height;
	else
		sy += p->Height * INVENTORY_SCALE;

	RenderTipTextList(sx, sy, TextNum, 0);
}

bool GetAttackDamage(int* iMinDamage, int* iMaxDamage)
{
	int AttackDamageMin;
	int AttackDamageMax;

	ITEM* r = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* l = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];

	/*if (PickItem.Number > 0 && SrcInventory == Inventory)
	{
		switch (SrcInventoryIndex)
		{
		case EQUIPMENT_WEAPON_RIGHT:
			r = &PickItem;
			break;
		case EQUIPMENT_WEAPON_LEFT:
			l = &PickItem;
			break;
		}
	}*/

	int EquipedBowType = gCharacterManager.GetEquipedBowType();

	if (EquipedBowType == BOWTYPE_CROSSBOW || EquipedBowType == BOWTYPE_BOW)
	{
		AttackDamageMin = CharacterAttribute->AttackDamageMinRight;
		AttackDamageMax = CharacterAttribute->AttackDamageMaxRight;
	}
	else if (r->Type == -1)
	{
		AttackDamageMin = CharacterAttribute->AttackDamageMinLeft;
		AttackDamageMax = CharacterAttribute->AttackDamageMaxLeft;
	}
	else if (r->Type >= ITEM_STAFF && r->Type < ITEM_SHIELD)
	{
		AttackDamageMin = CharacterAttribute->AttackDamageMinLeft;
		AttackDamageMax = CharacterAttribute->AttackDamageMaxLeft;
	}
	else
	{
		AttackDamageMin = CharacterAttribute->AttackDamageMinRight;
		AttackDamageMax = CharacterAttribute->AttackDamageMaxRight;
	}

	bool Alpha = false;
	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_KNIGHT || gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK)
	{
		if (l->Type >= ITEM_SWORD && l->Type < ITEM_STAFF + MAX_ITEM_INDEX && r->Type >= ITEM_SWORD && r->Type < ITEM_STAFF + MAX_ITEM_INDEX)
		{
			Alpha = true;
			AttackDamageMin = ((CharacterAttribute->AttackDamageMinRight * 55) / 100 + (CharacterAttribute->AttackDamageMinLeft * 55) / 100);
			AttackDamageMax = ((CharacterAttribute->AttackDamageMaxRight * 55) / 100 + (CharacterAttribute->AttackDamageMaxLeft * 55) / 100);
		}
	}
	else if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		int weaponkind = GMItemMng->GetKind2(r->Type);
		if (weaponkind == ItemKind2::BOW || weaponkind == ItemKind2::CROSSBOW)
		{
			weaponkind = GMItemMng->GetKind2(l->Type);
			if ((weaponkind == ItemKind2::ARROW || weaponkind == ItemKind2::BOLT || weaponkind == ItemKind2::QUIVER) && ((l->Level >> 3) & 15) >= 1)
			{
				Alpha = true;
			}
		}
	}
	else if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_RAGEFIGHTER)
	{
		if (l->Type >= ITEM_SWORD && l->Type < ITEM_MACE + MAX_ITEM_INDEX && r->Type >= ITEM_SWORD && r->Type < ITEM_MACE + MAX_ITEM_INDEX)
		{
			Alpha = true;
			AttackDamageMin = ((CharacterAttribute->AttackDamageMinRight + CharacterAttribute->AttackDamageMinLeft) * 60 / 100);
			AttackDamageMax = ((CharacterAttribute->AttackDamageMaxRight + CharacterAttribute->AttackDamageMaxLeft) * 65 / 100);
		}
	}

	if (CharacterAttribute->Ability & ABILITY_PLUS_DAMAGE)
	{
		AttackDamageMin += 15;
		AttackDamageMax += 15;
	}


	*iMinDamage = AttackDamageMin;
	*iMaxDamage = AttackDamageMax;

	return Alpha;
}

void RenderSkillInfo(int sx, int sy, int Type, int SkillNum, int iRenderPoint /*= STRP_NONE*/)
{
	char lpszName[256];
	int  iMinDamage, iMaxDamage;
	int  HeroClass = gCharacterManager.GetBaseClass(Hero->Class);
	int  iMana, iDistance, iSkillMana;
	int  TextNum = 0;
	int  SkipNum = 0;

	if (giPetManager::RenderPetCmdInfo(sx, sy, Type))
		return;

	int  AttackDamageMin, AttackDamageMax;
	int  iSkillMinDamage, iSkillMaxDamage;

	int  SkillType = CharacterAttribute->Skill[Type];
	gCharacterManager.GetMagicSkillDamage(CharacterAttribute->Skill[Type], &iMinDamage, &iMaxDamage);
	gCharacterManager.GetSkillDamage(CharacterAttribute->Skill[Type], &iSkillMinDamage, &iSkillMaxDamage);

	GetAttackDamage(&AttackDamageMin, &AttackDamageMax);

	iSkillMinDamage += AttackDamageMin;
	iSkillMaxDamage += AttackDamageMax;
	gSkillManager.GetSkillInformation(CharacterAttribute->Skill[Type], 1, lpszName, &iMana, &iDistance, &iSkillMana);

	if (CharacterAttribute->Skill[Type] == AT_SKILL_STRONG_PIER && Hero->Weapon[0].Type != -1)
	{
		for (int i = 0; i < CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].SpecialNum; i++)
		{
			if (CharacterMachine->Equipment[0].Special[i] == AT_SKILL_LONG_SPEAR)
			{
				wsprintf(lpszName, "%s", GlobalText[1200]);
				break;
			}
		}
	}

	//sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;
	sprintf(TextList[TextNum], "%s", lpszName);
	TextListColor[TextNum] = TEXT_COLOR_BLUE; TextBold[TextNum] = true; TextNum++;
	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	WORD Dexterity;
	WORD Energy;
	WORD Strength;
	WORD Vitality;
	WORD Charisma;

	Dexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
	Energy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
	Strength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;
	Vitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;
	Charisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;

	int skillattackpowerRate = 0;

	StrengthenCapability rightinfo, leftinfo;

	ITEM* rightweapon = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* leftweapon = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];

	int rightlevel = (rightweapon->Level >> 3) & 15;

	if (rightlevel >= rightweapon->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&rightinfo, rightweapon, 1);
	}

	int leftlevel = (leftweapon->Level >> 3) & 15;

	if (leftlevel >= leftweapon->Jewel_Of_Harmony_OptionLevel)
	{
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&leftinfo, leftweapon, 1);
	}

	if (rightinfo.SI_isSP)
	{
		skillattackpowerRate += rightinfo.SI_SP.SI_skillattackpower;
		skillattackpowerRate += rightinfo.SI_SP.SI_magicalpower;
	}
	if (leftinfo.SI_isSP)
	{
		skillattackpowerRate += leftinfo.SI_SP.SI_skillattackpower;
	}

	if (HeroClass == CLASS_WIZARD || HeroClass == CLASS_SUMMONER)
	{
		if (CharacterAttribute->Skill[Type] == AT_SKILL_WIZARDDEFENSE
			|| CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Strengthener
			|| CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Proficiency
			|| CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Mastery
			)
		{
			int iDamageShield;
			if (CharacterAttribute->Skill[Type] == AT_SKILL_WIZARDDEFENSE)
			{
				iDamageShield = (int)(10 + (Dexterity / 50.f) + (Energy / 200.f));
			}
			else if (CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Strengthener
				|| CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Proficiency
				|| CharacterAttribute->Skill[Type] == Skill_Soul_Barrier_Mastery)
			{
				iDamageShield = (int)(10 + (Dexterity / 50.f) + (Energy / 200.f)) + ((CharacterAttribute->Skill[Type] - Skill_Soul_Barrier_Strengthener) * 5);
			}

			int iDeleteMana = (int)(CharacterAttribute->ManaMax * 0.02f);
			int iLimitTime = (int)(60 + (Energy / 40.f));

			sprintf(TextList[TextNum], GlobalText[578], iDamageShield);
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;

			sprintf(TextList[TextNum], GlobalText[880], iDeleteMana);
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;

			sprintf(TextList[TextNum], GlobalText[881], iLimitTime);
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
		}
		else if (SkillType != AT_SKILL_SWELL_OF_MAGICPOWER && SkillType != AT_SKILL_ALICE_SLEEP)
		{
			WORD bySkill = CharacterAttribute->Skill[Type];
			if (!(AT_SKILL_STUN <= bySkill && bySkill <= AT_SKILL_MANA) && !(AT_SKILL_ALICE_THORNS <= bySkill && bySkill <= AT_SKILL_ALICE_ENERVATION) && bySkill != AT_SKILL_TELEPORT && bySkill != AT_SKILL_TELEPORT_B)
			{
#ifdef SHUTDOWN_CHARACTER_INFO
				DWORD _iMinDamage = iMinDamage;
				DWORD _iMaxDamage = iMaxDamage;

				if (AT_SKILL_SUMMON_EXPLOSION <= bySkill && bySkill <= AT_SKILL_SUMMON_POLLUTION)
				{
					CharacterAttribute->GetCurseDamage(&_iMinDamage, &_iMaxDamage);
					sprintf(TextList[TextNum], GlobalText[1692], _iMinDamage, _iMaxDamage);
				}
				else
				{
					CharacterAttribute->GetMagicDamage(&_iMinDamage, &_iMaxDamage);
					sprintf(TextList[TextNum], GlobalText[170], _iMinDamage, _iMaxDamage);
				}
#else
				if (AT_SKILL_SUMMON_EXPLOSION <= bySkill && bySkill <= AT_SKILL_SUMMON_POLLUTION)
				{
					gCharacterManager.GetCurseSkillDamage(bySkill, &iMinDamage, &iMaxDamage);
					sprintf(TextList[TextNum], GlobalText[1692], iMinDamage, iMaxDamage);
				}
				else
					sprintf(TextList[TextNum], GlobalText[170], iMinDamage + skillattackpowerRate, iMaxDamage + skillattackpowerRate);
#endif // SHUTDOWN_CHARACTER_INFO


				TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
			}
		}
	}
	if (HeroClass == CLASS_KNIGHT || HeroClass == CLASS_DARK || HeroClass == CLASS_ELF || HeroClass == CLASS_DARK_LORD
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
		|| HeroClass == CLASS_RAGEFIGHTER
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
		)
	{
		switch (CharacterAttribute->Skill[Type])
		{
		case AT_SKILL_TELEPORT:
		case AT_SKILL_TELEPORT_B:
		case AT_SKILL_WIZARDDEFENSE:
		case AT_SKILL_BLOCKING:
		case AT_SKILL_VITALITY:
		case AT_SKILL_HEALING:
		case AT_SKILL_DEFENSE:
		case AT_SKILL_ATTACK:
		case AT_SKILL_SUMMON:
		case AT_SKILL_SUMMON + 1:
		case AT_SKILL_SUMMON + 2:
		case AT_SKILL_SUMMON + 3:
		case AT_SKILL_SUMMON + 4:
		case AT_SKILL_SUMMON + 5:
		case AT_SKILL_SUMMON + 6:
		case AT_SKILL_SUMMON + 7:
		case AT_SKILL_IMPROVE_AG:
		case AT_SKILL_STUN:
		case AT_SKILL_REMOVAL_STUN:
		case AT_SKILL_MANA:
		case AT_SKILL_INVISIBLE:
		case AT_SKILL_REMOVAL_INVISIBLE:
		case AT_SKILL_REMOVAL_BUFF:
			break;
		case AT_SKILL_PARTY_TELEPORT:
		case AT_SKILL_ADD_CRITICAL:
			break;
		case AT_SKILL_DARK_HORSE:
			sprintf(TextList[TextNum], GlobalText[1237]);
			TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
			break;
		case AT_SKILL_BRAND_OF_SKILL:
			break;
		case AT_SKILL_PLASMA_STORM_FENRIR:
		case AT_SKILL_RECOVER:
		case AT_SKILL_ATT_UP_OURFORCES:
		case AT_SKILL_HP_UP_OURFORCES:
		case AT_SKILL_DEF_UP_OURFORCES:
			break;
		default:
			sprintf(TextList[TextNum], GlobalText[879], iSkillMinDamage, iSkillMaxDamage + skillattackpowerRate);
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
			break;
		}
	}

	if (CharacterAttribute->Skill[Type] == AT_SKILL_PLASMA_STORM_FENRIR)
	{
		int iSkillDamage;
		gSkillManager.GetSkillInformation_Damage(AT_SKILL_PLASMA_STORM_FENRIR, &iSkillDamage);

		if (HeroClass == CLASS_KNIGHT || HeroClass == CLASS_DARK)
		{
			iSkillMinDamage = (Strength / 3) + (Dexterity / 5) + (Vitality / 5) + (Energy / 7) + iSkillDamage;
		}
		else if (HeroClass == CLASS_WIZARD || HeroClass == CLASS_SUMMONER)
		{
			iSkillMinDamage = (Strength / 5) + (Dexterity / 5) + (Vitality / 7) + (Energy / 3) + iSkillDamage;
		}
		else if (HeroClass == CLASS_ELF)
		{
			iSkillMinDamage = (Strength / 5) + (Dexterity / 3) + (Vitality / 7) + (Energy / 5) + iSkillDamage;
		}
		else if (HeroClass == CLASS_DARK_LORD)
		{
			iSkillMinDamage = (Strength / 5) + (Dexterity / 5) + (Vitality / 7) + (Energy / 3) + (Charisma / 3) + iSkillDamage;
		}
#ifdef PBG_ADD_NEWCHAR_MONK
		else if (HeroClass == CLASS_RAGEFIGHTER)
		{
			iSkillMinDamage = (Strength / 5) + (Dexterity / 5) + (Vitality / 3) + (Energy / 7) + iSkillDamage;
		}
#endif //PBG_ADD_NEWCHAR_MONK
		iSkillMaxDamage = iSkillMinDamage + 30;

		sprintf(TextList[TextNum], GlobalText[879], iSkillMinDamage, iSkillMaxDamage + skillattackpowerRate);
		TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
	}

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_ELF)
	{
		bool Success = true;
		switch (CharacterAttribute->Skill[Type])
		{
		case AT_SKILL_HEALING:
			sprintf(TextList[TextNum], GlobalText[171], Energy / 5 + 5);
			break;
		case AT_SKILL_DEFENSE:
			sprintf(TextList[TextNum], GlobalText[172], Energy / 8 + 2);
			break;
		case AT_SKILL_ATTACK:
			sprintf(TextList[TextNum], GlobalText[173], Energy / 7 + 3);
			break;
		case AT_SKILL_RECOVER:
		{
			int Cal = Energy / 4;
			sprintf(TextList[TextNum], GlobalText[1782], (int)((float)Cal + (float)CharacterAttribute->Level));
		}
		break;
		default:Success = false;
		}
		if (Success)
		{
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
		}
	}

	if (SkillType != AT_SKILL_SWELL_OF_MAGICPOWER)
	{
		if (iDistance)
		{
			sprintf(TextList[TextNum], GlobalText[174], iDistance);
			TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
		}
	}

	sprintf(TextList[TextNum], GlobalText[175], iMana);
	TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
	if (iSkillMana > 0)
	{
		sprintf(TextList[TextNum], GlobalText[360], iSkillMana);
		TextListColor[TextNum] = TEXT_COLOR_WHITE; TextBold[TextNum] = false; TextNum++;
	}
	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_KNIGHT)
	{
		if (CharacterAttribute->Skill[Type] == Skill_Impale)
		{
			sprintf(TextList[TextNum], GlobalText[96]);
			TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
		}

		if (Hero->byExtensionSkill == 1 && CharacterAttribute->Level >= 220)
		{
			if ((CharacterAttribute->Skill[Type] >= AT_SKILL_SWORD1 && CharacterAttribute->Skill[Type] <= AT_SKILL_SWORD5)
				|| CharacterAttribute->Skill[Type] == AT_SKILL_WHEEL || CharacterAttribute->Skill[Type] == AT_SKILL_FURY_STRIKE
				|| CharacterAttribute->Skill[Type] == AT_SKILL_ONETOONE)
			{
				sprintf(TextList[TextNum], GlobalText[99]);
				TextListColor[TextNum] = TEXT_COLOR_DARKRED;
				TextBold[TextNum] = false;
				TextNum++;
			}
			else if (CharacterAttribute->Skill[Type] == AT_SKILL_BLOW_OF_DESTRUCTION)
			{
				sprintf(TextList[TextNum], GlobalText[2115]);
				TextListColor[TextNum] = TEXT_COLOR_DARKRED;
				TextBold[TextNum] = false;
				TextNum++;
			}
		}
	}

	BYTE MasteryType = gSkillManager.GetSkillMasteryType(CharacterAttribute->Skill[Type]);
	if (MasteryType != 255)
	{
		sprintf(TextList[TextNum], GlobalText[1080 + MasteryType]);
		TextListColor[TextNum] = TEXT_COLOR_BLUE; TextBold[TextNum] = false; TextNum++;
	}

	int SkillUseType;
	int BrandType = SkillAttribute[SkillType].SkillBrand;
	SkillUseType = SkillAttribute[SkillType].SkillUseType;
	if (SkillUseType == SKILL_USE_TYPE_BRAND)
	{
		sprintf(TextList[TextNum], GlobalText[1480], SkillAttribute[BrandType].Name);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
		sprintf(TextList[TextNum], GlobalText[1481], SkillAttribute[BrandType].Damage);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}
	SkillUseType = SkillAttribute[SkillType].SkillUseType;
	if (SkillUseType == SKILL_USE_TYPE_MASTER)
	{
		sprintf(TextList[TextNum], GlobalText[1482]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
		sprintf(TextList[TextNum], GlobalText[1483], SkillAttribute[SkillType].KillCount);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_DARK_LORD)
	{
		if (CharacterAttribute->Skill[Type] == AT_SKILL_PARTY_TELEPORT && PartyNumber <= 0)
		{
			sprintf(TextList[TextNum], GlobalText[1185]);
			TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
		}
	}

	if (CharacterAttribute->Skill[Type] == AT_SKILL_PLASMA_STORM_FENRIR)
	{
		sprintf(TextList[TextNum], GlobalText[1926]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
		sprintf(TextList[TextNum], GlobalText[1927]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}

	if (CharacterAttribute->Skill[Type] == AT_SKILL_INFINITY_ARROW)
	{
		sprintf(TextList[1], lpszName);
		TextListColor[1] = TEXT_COLOR_BLUE; TextBold[1] = true;
		sprintf(TextList[2], "\n");
		sprintf(TextList[3], GlobalText[2040]);
		TextListColor[3] = TEXT_COLOR_DARKRED; TextBold[3] = false;
		sprintf(TextList[4], GlobalText[175], iMana);
		TextListColor[4] = TEXT_COLOR_WHITE; TextBold[4] = false;
		sprintf(TextList[5], GlobalText[360], iSkillMana);
		TextListColor[5] = TEXT_COLOR_WHITE; TextBold[5] = false;
		TextNum = 6; SkipNum = 2;
	}

	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;


	if (!GMProtect->CastleSkillEnable() && (CharacterAttribute->Skill[Type] == AT_SKILL_RUSH || CharacterAttribute->Skill[Type] == Skill_Fire_Blast
		|| CharacterAttribute->Skill[Type] == AT_SKILL_DEEPIMPACT || CharacterAttribute->Skill[Type] == AT_SKILL_JAVELIN
		|| CharacterAttribute->Skill[Type] == AT_SKILL_ONEFLASH || CharacterAttribute->Skill[Type] == AT_SKILL_DEATH_CANNON
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
		|| CharacterAttribute->Skill[Type] == AT_SKILL_OCCUPY
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
		))
	{
		sprintf(TextList[TextNum], GlobalText[2047]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}

	if (CharacterAttribute->Skill[Type] == AT_SKILL_STUN || CharacterAttribute->Skill[Type] == AT_SKILL_REMOVAL_STUN
		|| CharacterAttribute->Skill[Type] == AT_SKILL_INVISIBLE || CharacterAttribute->Skill[Type] == AT_SKILL_REMOVAL_INVISIBLE
		|| CharacterAttribute->Skill[Type] == AT_SKILL_REMOVAL_BUFF)
	{
		sprintf(TextList[TextNum], GlobalText[2048]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}
	if (CharacterAttribute->Skill[Type] == Skill_Impale)
	{
		sprintf(TextList[TextNum], GlobalText[2049]);
		TextListColor[TextNum] = TEXT_COLOR_DARKRED; TextBold[TextNum] = false; TextNum++;
	}

	if (SkillType == AT_SKILL_SWELL_OF_MAGICPOWER)
	{
		sprintf(TextList[TextNum], GlobalText[2054]);
		TextListColor[TextNum] = TEXT_COLOR_BLUE; TextBold[TextNum] = false; TextNum++;
	}
	sprintf(TextList[TextNum], "\n"); TextNum++; SkipNum++;

	SIZE TextSize = { 0, 0 };
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[0], 1, &TextSize);

	if (iRenderPoint == STRP_NONE)
	{
		int Height = ((TextNum - SkipNum) * TextSize.cy + SkipNum * TextSize.cy / 2) / g_fScreenRate_y;
		sy -= Height;
	}
	RenderTipTextList(sx, sy, TextNum, 0, RT3_SORT_CENTER, iRenderPoint);
}

void RenderItemName(ITEM_t* item, int Number, OBJECT* pObj, int ItemLevel, int ItemOption, int ItemExtOption, bool Sort)
{
	float _ScreenX, _ScreenY;

	GLfloat fCurColor[4];

	glGetFloatv(GL_CURRENT_COLOR, fCurColor);

	if (gmProtect->m_RenderItemNameDrop == 1)
	{
		SIZE RenderBoxSize;

		g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), item->Name, strlen(item->Name), &RenderBoxSize);

		if (Sort == true)
		{
			_ScreenX = pObj->ScreenX;
			_ScreenY = (pObj->ScreenY - 15);
		}
		else
		{
			_ScreenX = MouseRenderX;
			_ScreenY = (MouseRenderY - 15);
		}
		float RenderSizeX = (RenderBoxSize.cx + 32.f);
		float RenderSizeY = (RenderBoxSize.cy + (RenderBoxSize.cy));

		_ScreenY -= (RenderSizeY * 0.5);
		_ScreenX -= (RenderSizeX * 0.5);

		EnableAlphaTest();

		glColor4f(1.f, 1.f, 1.f, 1.f);

		jpexs::RenderBoxjpexs_dds(IMAGE_HUD_PLAYER, _ScreenX, _ScreenY, RenderSizeX, RenderSizeY);

		g_pRenderText->SetFont(g_hFontBold);

		HookColorFont(item->RenderColor);

		if (Sort == true)
			_ScreenX = pObj->ScreenX;
		else
			_ScreenX = MouseRenderX;

		g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, item->Name, 0, RenderSizeY, RT3_WRITE_CENTER, NULL);

	}
	else
	{
		g_pRenderText->SetBgColor(0, 0, 0, 0xFF);

		if (item->RenderTextBold != TRUE)
			g_pRenderText->SetFont(g_hFont);
		else
			g_pRenderText->SetFont(g_hFontBold);

		g_pRenderText->SetTextColor(0xFFFFFFFF);

		EnableAlphaTest();

		HookColorFont(item->RenderColor);

		GLfloat _fCurColor[4] = { 1.f, 1.f, 1.f, 0.0f };

		glGetFloatv(GL_CURRENT_COLOR, _fCurColor);

		if (_fCurColor[0] < 0.9f || _fCurColor[1] < 0.9f || _fCurColor[2] < 0.9f)
		{
			g_pRenderText->SetTextColor(_fCurColor[0] * 255, _fCurColor[1] * 255, _fCurColor[2] * 255, 255);
		}

		if (Sort == true)
		{
			_ScreenX = pObj->ScreenX;
			_ScreenY = (pObj->ScreenY - 15);
		}
		else
		{
			_ScreenX = MouseRenderX;
			_ScreenY = (MouseRenderY - 15);
		}

		if (ItemExtOption % 4 == 1 || ItemExtOption % 4 == 2)
		{
			glColor3f(1.0, 1.0, 1.0);
			g_pRenderText->SetTextColor(0, 0xFFu, 0, 0xFFu);
			g_pRenderText->SetBgColor(0x3Cu, 0x3Cu, 0xC8u, 0xFFu);
		}

		g_pRenderText->SetBgColor(0, 0, 0, 0xFFu);
		g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, item->Name, 0, 0, RT3_WRITE_CENTER, 0);
		g_pRenderText->SetTextColor(0xFFu, 0xE6u, 0xC8u, 0xFFu);
		g_pRenderText->SetBgColor(0, 0, 0, 0xFFu);
	}
	glColor4fv(fCurColor);
}

int GetScreenWidth()
{
	int iWidth = 0;
	int WinOpen0 = (int)pos_right(640.f);
	int WinOpen1 = (int)WinOpen0 - (WIN_WINDOW_SIZEX);
	int WinOpen2 = (int)WinOpen1 - (WIN_WINDOW_SIZEX);
	int WinOpen3 = (int)WinOpen2 - (WIN_WINDOW_SIZEX);

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY) && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYSHOP_INVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_JEWEL)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND))
	{
		iWidth = WinOpen2;
	}
	else
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
			&& (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYQUEST)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)))
		{
			iWidth = WinOpen2;
		}
		else
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER) && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PET))
			{
				iWidth = WinOpen2;
			}
			else
			{
				if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_REFINERY))
				{
					iWidth = WinOpen2;
				}
				else
				{
					if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PARTY)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCGUILDMASTER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GUILDINFO)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GUARDSMAN)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_SENATUS)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GATEKEEPER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYQUEST)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_SERVERDIVISION)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_COMMAND)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCQUEST)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GATESWITCH)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CATAPULT)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DEVILSQUARE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_BLOODCASTLE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GOLD_BOWMAN)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GOLD_BOWMAN_LENA)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DUELWATCH)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DOPPELGANGER_NPC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GENSRANKING)

						)
					{
						iWidth = WinOpen1;
					}
					else
					{
						if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL))
						{
							if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL_SUB))
								iWidth = WinOpen2;
							else
								iWidth = WinOpen1;
						}
						else
						{
							iWidth = WinOpen0;
						}
					}
				}
			}
		}
	}

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
	{
		if (g_pInvenExpansion->IsThirdInterface(0))
			iWidth = WinOpen3;
		else
			iWidth = WinOpen2;
	}
	else
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
		{
			iWidth = WinOpen3;
		}
	}
	return iWidth;
}

int GetScreenWidth2()
{
	int iWidth = 0;
	int WinOpen0 = 640;
	int WinOpen1 = 450;
	int WinOpen2 = 260;
	int WinOpen3 = 70;

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY) && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCSHOP)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_TRADE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYSHOP_INVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_JEWEL)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND))
	{
		iWidth = WinOpen2;
	}
	else
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
			&& (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYQUEST)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)))
		{
			iWidth = WinOpen2;
		}
		else
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER) && g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PET))
			{
				iWidth = WinOpen2;
			}
			else
			{
				if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_REFINERY))
				{
					iWidth = WinOpen2;
				}
				else
				{
					if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PARTY)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCGUILDMASTER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GUILDINFO)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GUARDSMAN)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_SENATUS)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GATEKEEPER)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MYQUEST)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_SERVERDIVISION)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_COMMAND)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPCQUEST)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GATESWITCH)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CATAPULT)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DEVILSQUARE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_BLOODCASTLE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GOLD_BOWMAN)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GOLD_BOWMAN_LENA)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DUELWATCH)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_DOPPELGANGER_NPC)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA)
						|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_GENSRANKING)

						)
					{
						iWidth = WinOpen1;
					}
					else
					{
						if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL))
						{
							if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL_SUB))
								iWidth = WinOpen2;
							else
								iWidth = WinOpen1;
						}
						else
						{
							iWidth = WinOpen0;
						}
					}
				}
			}
		}
	}

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
	{
		if (g_pInvenExpansion->IsThirdInterface(0))
			iWidth = WinOpen3;
		else
			iWidth = WinOpen2;
	}
	else
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
		{
			iWidth = WinOpen3;
		}
	}
	return iWidth;
}

void ClearInventory()
{
	for (int i = 0; i < MAX_EQUIPMENT; i++)
	{
		CharacterMachine->Equipment[i].Type = -1;
		CharacterMachine->Equipment[i].Number = 0;
	}
	for (int i = 0; i < MAX_INVENTORY; i++)
	{
		Inventory[i].Type = -1;
		//Inventory[i].Number = 0;
	}
	for (int i = 0; i < MAX_SHOP_INVENTORY; i++)
	{
		ShopInventory[i].Type = -1;
		ShopInventory[i].Number = 0;
	}

	COMGEM::Init();
}

int CompareItem(ITEM item1, ITEM item2)
{
	int equal = 0;
	if (item1.Type != item2.Type)
	{
		return  2;
	}
	else
		if (item1.Class == item2.Class && item1.Type == item2.Type)
		{
			int level1 = (item1.Level >> 3) & 15;
			int level2 = (item2.Level >> 3) & 15;
			int option1 = (item1.Option1 & 63);
			int option2 = (item2.Option1 & 63);
			int skill1 = (item1.Level >> 7) & 1;
			int skill2 = (item2.Level >> 7) & 1;

			equal = 1;
			if (level1 == level2)
			{
				equal = 0;
			}
			else
				if (level1 < level2)
				{
					equal = -1;
				}
			if (equal == 0)
			{
				if (skill1 < skill2)
				{
					equal = -1;
				}
				else if (skill1 > skill2)
				{
					equal = 1;
				}
			}
			if (equal == 0)
			{
				if (option1 < option2)
				{
					equal = -1;
				}
				else if (option1 > option2)
				{
					equal = 1;
				}
			}
			if (equal == 0)
			{
				if (item1.SpecialNum < item2.SpecialNum)
				{
					equal = -1;
				}
				else if (item1.SpecialNum > item2.SpecialNum)
				{
					equal = 1;
				}
				else
				{
					int     Num = max(item1.SpecialNum, item2.SpecialNum);
					int     addOption1 = 0;
					int     addOptionV1 = 0;
					int     addOption2 = 0;
					int     addOptionV2 = 0;
					for (int i = 0; i < Num; ++i)
					{
						switch (item1.Special[i])
						{
						case AT_IMPROVE_DAMAGE:
						case AT_IMPROVE_MAGIC:
						case AT_IMPROVE_CURSE:
						case AT_IMPROVE_BLOCKING:
						case AT_IMPROVE_DEFENSE:
							addOption1 = 1;
							addOptionV1 = item1.SpecialValue[i];
							break;
						}
						switch (item2.Special[i])
						{
						case AT_IMPROVE_DAMAGE:
						case AT_IMPROVE_MAGIC:
						case AT_IMPROVE_CURSE:
						case AT_IMPROVE_BLOCKING:
						case AT_IMPROVE_DEFENSE:
							addOption2 = 1;
							addOptionV2 = item2.SpecialValue[i];
							break;
						}
					}

					if (addOption1 < addOption2 || addOptionV1 < addOptionV2)
					{
						equal = -1;
					}
					else if (addOption1 != addOption2 && addOptionV1 != addOptionV2)
					{
						equal = 1;
					}
				}
			}
			if (equal == 0)
			{
				if (item1.Durability < item2.Durability)
					equal = -1;
				if (item1.Durability > item2.Durability)
					equal = 1;
			}
		}
	return  equal;
}

bool EquipmentItem = false;
extern int BuyCost;

bool IsPartChargeItem(ITEM* pItem)
{
	if ((pItem->Type >= ITEM_HELPER + 46 && pItem->Type <= ITEM_HELPER + 48)
		|| (pItem->Type == ITEM_POTION + 54)
		|| (pItem->Type >= ITEM_POTION + 58 && pItem->Type <= ITEM_POTION + 62)
		|| (pItem->Type >= ITEM_POTION + 145 && pItem->Type <= ITEM_POTION + 150)
		|| (pItem->Type >= ITEM_HELPER + 125 && pItem->Type <= ITEM_HELPER + 127)
		|| pItem->Type == ITEM_POTION + 53
		|| (pItem->Type >= ITEM_HELPER + 43 && pItem->Type <= ITEM_HELPER + 45)
		|| (pItem->Type >= ITEM_POTION + 70 && pItem->Type <= ITEM_POTION + 71)
		|| (pItem->Type >= ITEM_POTION + 72 && pItem->Type <= ITEM_POTION + 77)
		|| (pItem->Type == ITEM_HELPER + 59)
		|| (pItem->Type >= ITEM_HELPER + 54 && pItem->Type <= ITEM_HELPER + 58)
		|| (pItem->Type >= ITEM_POTION + 78 && pItem->Type <= ITEM_POTION + 82)
		|| (pItem->Type == ITEM_HELPER + 60)
		|| (pItem->Type == ITEM_HELPER + 61)
		|| (pItem->Type == ITEM_POTION + 91)
		|| (pItem->Type >= ITEM_POTION + 92 && pItem->Type <= ITEM_POTION + 93)
		|| (pItem->Type == ITEM_POTION + 95)
		|| (pItem->Type == ITEM_POTION + 94)
		|| (pItem->Type >= ITEM_HELPER + 62 && pItem->Type <= ITEM_HELPER + 63)
		|| (pItem->Type >= ITEM_POTION + 97 && pItem->Type <= ITEM_POTION + 98)
		|| (pItem->Type == ITEM_POTION + 96)
		|| (pItem->Type == ITEM_HELPER + 64 || pItem->Type == ITEM_HELPER + 65)
		|| (pItem->Type == ITEM_HELPER + 69)
		|| (pItem->Type == ITEM_HELPER + 70)
		|| pItem->Type == ITEM_HELPER + 81
		|| pItem->Type == ITEM_HELPER + 82
		|| pItem->Type == ITEM_HELPER + 93
		|| pItem->Type == ITEM_HELPER + 94
		|| pItem->Type == ITEM_HELPER + 107
		|| pItem->Type == ITEM_HELPER + 104
		|| pItem->Type == ITEM_HELPER + 105
		|| pItem->Type == ITEM_HELPER + 103
		|| pItem->Type == ITEM_POTION + 133
		|| pItem->Type == ITEM_HELPER + 109
		|| pItem->Type == ITEM_HELPER + 110
		|| pItem->Type == ITEM_HELPER + 111
		|| pItem->Type == ITEM_HELPER + 112
		|| pItem->Type == ITEM_HELPER + 113
		|| pItem->Type == ITEM_HELPER + 114
		|| pItem->Type == ITEM_HELPER + 115
		|| pItem->Type == ITEM_POTION + 112
		|| pItem->Type == ITEM_POTION + 113
		|| pItem->Type == ITEM_POTION + 120
		|| pItem->Type == ITEM_POTION + 123
		|| pItem->Type == ITEM_POTION + 124
		|| pItem->Type == ITEM_POTION + 134
		|| pItem->Type == ITEM_POTION + 135
		|| pItem->Type == ITEM_POTION + 136
		|| pItem->Type == ITEM_POTION + 137
		|| pItem->Type == ITEM_POTION + 138
		|| pItem->Type == ITEM_POTION + 139
		|| pItem->Type == ITEM_WING + 130
		|| pItem->Type == ITEM_WING + 131
		|| pItem->Type == ITEM_WING + 132
		|| pItem->Type == ITEM_WING + 133
		|| pItem->Type == ITEM_WING + 134
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		|| pItem->Type == ITEM_WING + 135
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		|| pItem->Type == ITEM_HELPER + 116
		|| pItem->Type == ITEM_HELPER + 106
		|| pItem->Type == ITEM_HELPER + 124
		|| pItem->Type == ITEM_POTION + 114
		|| pItem->Type == ITEM_POTION + 115
		|| pItem->Type == ITEM_POTION + 116
		|| pItem->Type == ITEM_POTION + 117
		|| pItem->Type == ITEM_POTION + 118
		|| pItem->Type == ITEM_POTION + 119
		|| pItem->Type == ITEM_POTION + 126
		|| pItem->Type == ITEM_POTION + 127
		|| pItem->Type == ITEM_POTION + 128
		|| pItem->Type == ITEM_POTION + 129
		|| pItem->Type == ITEM_POTION + 130
		|| pItem->Type == ITEM_POTION + 131
		|| pItem->Type == ITEM_POTION + 132
		|| pItem->Type == ITEM_HELPER + 121
		|| pItem->Type == ITEM_POTION + 140
		)
	{
		return true;
	}

	return false;
}

bool IsHighValueItem(ITEM* pItem)
{
	int iLevel = (pItem->Level >> 3) & 15;

	if (
		pItem->Type == ITEM_HELPER + 3 ||
		pItem->Type == ITEM_POTION + 13 ||
		pItem->Type == ITEM_POTION + 14 ||
		pItem->Type == ITEM_POTION + 16 ||
		pItem->Type == ITEM_POTION + 22 ||
		pItem->Type == ITEM_WING + 15 ||
		pItem->Type == ITEM_POTION + 31 ||
		pItem->Type == ITEM_WING + 30 ||
		pItem->Type == ITEM_WING + 31 ||
		(pItem->Type >= ITEM_WING && pItem->Type <= ITEM_WING + 6) ||
		pItem->Type == ITEM_HELPER + 4 ||
		pItem->Type == ITEM_HELPER + 5 ||
		pItem->Type == ITEM_HELPER + 30 ||
		(pItem->Type >= ITEM_WING + 36 && pItem->Type <= ITEM_WING + 43) ||
		((pItem->ExtOption % 0x04) == EXT_A_SET_OPTION || (pItem->ExtOption % 0x04) == EXT_B_SET_OPTION) ||
		pItem->Type == ITEM_SWORD + 19 ||
		pItem->Type == ITEM_STAFF + 10 ||
		pItem->Type == ITEM_BOW + 18 ||
		pItem->Type == ITEM_MACE + 13 ||
		pItem->Type == ITEM_HELPER + 14 ||
		pItem->Type == ITEM_HELPER + 15 ||
		pItem->Type == ITEM_HELPER + 19 ||
		pItem->Type == ITEM_HELPER + 31 ||
		(pItem->Type >= ITEM_POTION + 41 && pItem->Type <= ITEM_POTION + 44) ||
		(iLevel > 6 && pItem->Type < ITEM_WING) ||
		(pItem->Option1 & 63) > 0 ||
		(pItem->Type >= ITEM_HELPER + 34 && pItem->Type <= ITEM_HELPER + 37)
		|| pItem->Type == ITEM_HELPER + 52
		|| pItem->Type == ITEM_HELPER + 53
		|| pItem->Type == ITEM_POTION + 121
		|| pItem->Type == ITEM_POTION + 122
		|| pItem->Type == ITEM_WING + 130
		|| pItem->Type == ITEM_WING + 131
		|| pItem->Type == ITEM_WING + 132
		|| pItem->Type == ITEM_WING + 133
		|| pItem->Type == ITEM_WING + 134
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		|| pItem->Type == ITEM_WING + 135
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		|| pItem->Type == ITEM_HELPER + 80
		|| pItem->Type == ITEM_HELPER + 76
		|| pItem->Type == ITEM_HELPER + 122
		|| pItem->Type == ITEM_HELPER + 123
		|| pItem->Type == ITEM_HELPER + 64
		|| pItem->Type == ITEM_HELPER + 65
		|| pItem->Type == ITEM_HELPER + 109
		|| pItem->Type == ITEM_HELPER + 110
		|| pItem->Type == ITEM_HELPER + 111
		|| pItem->Type == ITEM_HELPER + 112
		|| pItem->Type == ITEM_HELPER + 113
		|| pItem->Type == ITEM_HELPER + 114
		|| pItem->Type == ITEM_HELPER + 115
		|| pItem->Type == ITEM_POTION + 112
		|| pItem->Type == ITEM_POTION + 113
		|| (pItem->Type == ITEM_HELPER + 20 && iLevel == 0)
#ifdef LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
		|| (g_pMyInventory->IsInvenItem(pItem->Type) && pItem->Durability == 255)
#endif //LJH_ADD_SYSTEM_OF_EQUIPPING_ITEM_FROM_INVENTORY
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		|| (pItem->Type >= ITEM_WING + 49 && pItem->Type <= ITEM_WING + 50)
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
#ifdef KJH_FIX_SELL_LUCKYITEM
		|| (Check_ItemAction(pItem, eITEM_SELL) && pItem->Durability > 0)
#endif // KJH_FIX_SELL_LUCKYITEM
		|| (COMGEM::isCompiledGem(pItem))
		|| GMJewelOfAction->CheckOfJewelByItem(pItem->Type)
		)
	{
		if (true == pItem->bPeriodItem && false == pItem->bExpiredPeriod)
		{
			return false;
		}
		else if (pItem->Type == ITEM_HELPER + 80
			|| pItem->Type == ITEM_HELPER + 76
			|| pItem->Type == ITEM_HELPER + 64
			|| pItem->Type == ITEM_HELPER + 65
			|| pItem->Type == ITEM_HELPER + 122
			|| pItem->Type == ITEM_HELPER + 123
			)
		{
			if (true == pItem->bPeriodItem && true == pItem->bExpiredPeriod)
			{
				return true;
			}
			else
				return false;
		}
		return true;
	}

	return false;
}

bool IsPersonalShopBan(ITEM* pItem)
{
	bool result;

	if (pItem)
	{
		if (pItem->bPeriodItem)
		{
			result = true;
		}
		else if ((pItem->bPeriodItem || pItem->Type != ITEM_HELPER + 64)
			&& pItem->Type != ITEM_HELPER + 65
			&& pItem->Type != ITEM_HELPER + 80
			&& pItem->Type != ITEM_HELPER + 76
			&& pItem->Type != ITEM_HELPER + 122
			&& pItem->Type != ITEM_HELPER + 123
			&& ((!g_pMyInventory->IsInvenItem(pItem->Type))
				|| pItem->Durability != 0xff)
			&& (pItem->Type != ITEM_HELPER + 20 || (pItem->Level >> 3) & 0xF))
		{
			if (pItem->Type == ITEM_HELPER + 38
				|| pItem->Type == ITEM_HELPER + 39
				|| pItem->Type == ITEM_POTION + 21 && ((pItem->Level >> 3) & 0xF) != 3
				|| pItem->Type >= ITEM_POTION + 23 && pItem->Type <= ITEM_POTION + 26
				|| pItem->Type == ITEM_HELPER + 19
				|| pItem->Type == ITEM_POTION + 11 && ((pItem->Level >> 3) & 0xF) == 13
				|| pItem->Type >= ITEM_HELPER + 43 && pItem->Type <= ITEM_HELPER + 45
				|| pItem->Type == ITEM_HELPER + 20 && (pItem->Level >> 3) & 0xF
				|| pItem->Type == ITEM_POTION + 65
				|| pItem->Type == ITEM_POTION + 66
				|| pItem->Type == ITEM_POTION + 67
				|| pItem->Type == ITEM_POTION + 68
				|| pItem->Type == ITEM_HELPER + 97
				|| pItem->Type == ITEM_HELPER + 98
				|| pItem->Type == ITEM_POTION + 91
				|| pItem->Type == ITEM_HELPER + 80
				|| pItem->Type == ITEM_HELPER + 76
				|| pItem->Type == ITEM_HELPER + 122
				|| pItem->Type == ITEM_HELPER + 123
				|| pItem->Type >= ITEM_HELPER + 117 && pItem->Type <= ITEM_HELPER + 120
				|| pItem->Type == ITEM_HELPER + 109
				|| pItem->Type == ITEM_HELPER + 110
				|| pItem->Type == ITEM_HELPER + 111
				|| pItem->Type == ITEM_HELPER + 112
				|| pItem->Type == ITEM_HELPER + 113
				|| pItem->Type == ITEM_HELPER + 114
				|| pItem->Type == ITEM_HELPER + 115
				|| pItem->Type >= ITEM_POTION + 151 && pItem->Type <= ITEM_POTION + 156
				|| (g_pMyInventory->IsInvenItem(pItem->Type))
				|| pItem->Type >= ITEM_POTION + 163 && pItem->Type <= ITEM_POTION + 168
				|| IsPartChargeItem(pItem))
			{
				result = true;
			}
			else
			{
				result = Check_ItemAction(pItem, eITEM_PERSONALSHOP, 0) != false;
			}
		}
		else
		{
			result = false;
		}

		/*if (g_pMyInventory->IsInvenItem(pItem->Type) == false)
		{
			Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

			if (ItemInfo->StorePersonal == true)
			{
				result = false;
			}
		}*/
	}
	else
	{
		result = false;
	}

	return result;
}

bool IsTradeBan(ITEM* pItem)
{
	bool result;

	if (pItem)
	{
		if (pItem->bPeriodItem)
		{
			result = true;
		}
		else
		{
			if (!pItem->bPeriodItem && (pItem->Type == (ITEM_HELPER + 38)
				|| pItem->Type == (ITEM_HELPER + 39)
				|| pItem->Type == (ITEM_POTION + 21) && ((pItem->Level >> 3) & 0xF) != 3
				|| pItem->Type >= (ITEM_POTION + 23) && pItem->Type <= (ITEM_POTION + 26)
				|| pItem->Type == (ITEM_HELPER + 19)
				|| pItem->Type == (ITEM_POTION + 11) && ((pItem->Level >> 3) & 0xF) == 13
				|| pItem->Type >= (ITEM_HELPER + 43) && pItem->Type <= (ITEM_HELPER + 45)
				|| pItem->Type == (ITEM_HELPER + 20) && (pItem->Level >> 3) & 0xF
				|| pItem->Type == (ITEM_POTION + 64)
				|| pItem->Type == (ITEM_POTION + 65)
				|| pItem->Type == (ITEM_POTION + 66)
				|| pItem->Type == (ITEM_POTION + 67)
				|| pItem->Type == (ITEM_POTION + 68)
				|| pItem->Type == (ITEM_HELPER + 97)
				|| pItem->Type == (ITEM_HELPER + 98)
				|| pItem->Type == (ITEM_POTION + 91)
				|| pItem->Type == (ITEM_HELPER + 80)
				|| pItem->Type == (ITEM_HELPER + 76)
				|| pItem->Type == (ITEM_HELPER + 122)
				|| pItem->Type == (ITEM_HELPER + 123)
				|| pItem->Type >= (ITEM_HELPER + 117) && pItem->Type <= (ITEM_HELPER + 120)
				|| pItem->Type >= (ITEM_POTION + 151) && pItem->Type <= (ITEM_POTION + 156)
				|| (g_pMyInventory->IsInvenItem(pItem->Type))
				|| pItem->Type >= (ITEM_POTION + 163) && (pItem->Type <= ITEM_POTION + 168)
				|| IsPartChargeItem(pItem)))
			{
				result = true;
			}
			else if (pItem->Type == (ITEM_POTION + 52))
			{
				result = !g_isCharacterBuff((&Hero->Object), eBuff_GMEffect)
					&& Hero->CtlCode != CTLCODE_20OPERATOR
					&& Hero->CtlCode != CTLCODE_08OPERATOR;
			}
			else
			{
				result = Check_ItemAction(pItem, eITEM_TRADE, false) != 0;
			}

			/*if (pItem->Type != (ITEM_POTION + 52) && !g_pMyInventory->IsInvenItem(pItem->Type))
			{
				Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

				if (ItemInfo->Trade == true)
				{
					return false;
				}
			}*/
		}
	}
	else
	{
		result = false;
	}

	return result;
}

bool IsStoreBan(ITEM* pItem)
{
	bool result; // al@27

	if (pItem->Type >= ITEM_POTION + 23 && pItem->Type <= ITEM_POTION + 26
		|| pItem->Type == ITEM_POTION + 21 && ((pItem->Level >> 3) & 0xF) != 3
		|| pItem->Type == ITEM_HELPER + 19
		|| pItem->Type == ITEM_POTION + 11 && ((pItem->Level >> 3) & 0xF) == 13
		|| pItem->Type >= ITEM_HELPER + 43 && pItem->Type <= 6701
		|| pItem->Type == ITEM_HELPER + 93
		|| pItem->Type == ITEM_HELPER + 94
		|| pItem->Type == ITEM_HELPER + 20 && (pItem->Level >> 3) & 0xF
		|| pItem->Type == ITEM_POTION + 65
		|| pItem->Type == ITEM_POTION + 66
		|| pItem->Type == ITEM_POTION + 67
		|| pItem->Type == ITEM_POTION + 68
		|| pItem->Type == ITEM_HELPER + 70 && pItem->Durability == 1
		|| pItem->Type >= ITEM_POTION + 151 && pItem->Type <= ITEM_POTION + 156
		|| pItem->bPeriodItem == 1
		|| (g_pMyInventory->IsInvenItem(pItem->Type))
		&& pItem->Durability == 0xfe
		|| pItem->Type >= ITEM_POTION + 163 && pItem->Type <= ITEM_POTION + 168)
	{
		result = true;
	}
	else
	{
		result = Check_ItemAction(pItem, eITEM_STORE, 0) != 0;
	}

	/*Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

	if (ItemInfo->WhareHouse == true)
	{
		result = false;
	}*/

	return result;
}

sItemAct Set_ItemActOption(int _nIndex, int _nOption)
{
	sItemAct	sItem;
	// eITEM_PERSONALSHOP = °³ÀÎ»óÁ¡, eITEM_STORE = Ã¢°í, eITEM_TRADE = °Å·¡, eITEM_DROP = ¹ö¸®±â, eITEM_SELL = ÆÇ¸Å, eITEM_REPAIR = ¼ö¸®
	int	nItemOption[][eITEM_END] = { 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, -1, 0, 0, 0, 0, 0 };


	sItem.s_nItemIndex = _nIndex;

	for (int i = 0; i < eITEM_END; i++)
	{
		sItem.s_bType[i] = nItemOption[_nOption][i];
	}
	return sItem;
}


bool Check_ItemAction(ITEM* _pItem, ITEMSETOPTION _eAction, bool _bType)
{
	std::vector<sItemAct>			sItem;
	std::vector<sItemAct>::iterator li;
	int		i = 0;

	for (i = 0; i < 12; i++)	sItem.push_back(Set_ItemActOption(ITEM_HELPER + 135 + i, 0));
	for (i = 0; i < 2; i++)	sItem.push_back(Set_ItemActOption(ITEM_POTION + 160 + i, 0));
	for (i = 0; i < 12; i++)
	{
		sItem.push_back(Set_ItemActOption(ITEM_ARMOR + 62 + i, 1));
		sItem.push_back(Set_ItemActOption(ITEM_HELM + 62 + i, 1));
		sItem.push_back(Set_ItemActOption(ITEM_BOOTS + 62 + i, 1));
		sItem.push_back(Set_ItemActOption(ITEM_GLOVES + 62 + i, 1));
		sItem.push_back(Set_ItemActOption(ITEM_PANTS + 62 + i, 1));
	}

	for (li = sItem.begin(); li != sItem.end(); li++)
	{
		if (li->s_nItemIndex == _pItem->Type)
		{
			_bType = (li->s_bType[_eAction]) ^ (!_bType);
			return _bType;
		}
	}

	return false;
}

bool Check_LuckyItem(int _nIndex, int _nType)
{
	int nItemTabIndex = (_nIndex + _nType) % 512;

	if (_nIndex >= ITEM_HELM && _nIndex <= ITEM_WING)
	{
		return (nItemTabIndex >= 62 && nItemTabIndex <= 72);
	}

	return false;
}

bool IsLuckySetItem(int iType)
{
	int iItemIndex = iType % MAX_ITEM_INDEX;

	if (iType >= ITEM_HELM && iType <= ITEM_WING)
	{
		return (iItemIndex >= 62 && iItemIndex <= 72);
	}

	return false;
}

bool IsDropBan(ITEM* pItem)
{
	bool result;

	if (pItem->bPeriodItem
		|| pItem->Type != (ITEM_POTION + 54)
		&& pItem->Type != (ITEM_HELPER + 64)
		&& pItem->Type != (ITEM_HELPER + 65)
		&& pItem->Type != (ITEM_HELPER + 80)
		&& pItem->Type != (ITEM_HELPER + 76)
		&& pItem->Type != (ITEM_HELPER + 122)
		&& pItem->Type != (ITEM_HELPER + 123)
		&& (pItem->Type != (ITEM_HELPER + 20) || pItem->bPeriodItem || (pItem->Level >> 3) & 0xF))
	{
		if (pItem->Type != ITEM_POTION + 123 && pItem->Type != ITEM_POTION + 124)
		{
			if (pItem->Type >= ITEM_POTION + 23 && pItem->Type <= ITEM_POTION + 26
				|| pItem->Type >= ITEM_POTION + 65 && pItem->Type <= ITEM_POTION + 68
				|| pItem->Type == ITEM_HELPER + 97
				|| pItem->Type == ITEM_HELPER + 98
				|| pItem->Type == ITEM_POTION + 91
				|| pItem->Type == ITEM_HELPER + 80
				|| pItem->Type == ITEM_HELPER + 76
				|| pItem->Type == ITEM_HELPER + 122
				|| pItem->Type == ITEM_HELPER + 123
				|| pItem->Type == ITEM_POTION + 121
				|| pItem->Type == ITEM_POTION + 122
				|| pItem->Type >= ITEM_POTION + 151 && pItem->Type <= ITEM_POTION + 156
				|| (g_pMyInventory->IsInvenItem(pItem->Type))
				|| pItem->Type >= ITEM_POTION + 163 && pItem->Type <= ITEM_POTION + 168
				|| pItem->Type == ITEM_POTION + 96
				|| pItem->Type == ITEM_HELPER + 20 && !((pItem->Level >> 3) & 0xF)
				|| IsPartChargeItem(pItem))
			{
				result = true;
			}
			else
			{
				result = Check_ItemAction(pItem, eITEM_DROP, 0) != 0;
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}

	/*Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

	if (ItemInfo->Drop == true)
	{
		result = false;
	}*/

	return result;
}

bool IsSellingBan(ITEM* pItem)
{
	int Level = (pItem->Level >> 3) & 15;

	if (pItem->Type != ITEM_POTION + 112
		&& pItem->Type != ITEM_POTION + 113
		&& pItem->Type != ITEM_POTION + 121
		&& pItem->Type != ITEM_POTION + 122
		&& pItem->Type != ITEM_POTION + 123
		&& pItem->Type != ITEM_POTION + 124
		&& pItem->Type != ITEM_WING + 130
		&& pItem->Type != ITEM_WING + 131
		&& pItem->Type != ITEM_WING + 132
		&& pItem->Type != ITEM_WING + 133
		&& pItem->Type != ITEM_WING + 134
		&& pItem->Type != ITEM_WING + 135
		&& pItem->Type != ITEM_HELPER + 80
		&& pItem->Type != ITEM_HELPER + 76
		&& pItem->Type != ITEM_HELPER + 122
		&& pItem->Type != ITEM_HELPER + 123
		&& pItem->Type != ITEM_HELPER + 64
		&& pItem->Type != ITEM_HELPER + 65
		&& pItem->Type != ITEM_HELPER + 109
		&& pItem->Type != ITEM_HELPER + 110
		&& pItem->Type != ITEM_HELPER + 111
		&& pItem->Type != ITEM_HELPER + 112
		&& pItem->Type != ITEM_HELPER + 113
		&& pItem->Type != ITEM_HELPER + 114
		&& pItem->Type != ITEM_HELPER + 115
		&& (!g_pMyInventory->IsInvenItem(pItem->Type)
			|| pItem->Durability == 254)
		&& (pItem->Type != ITEM_HELPER + 20 || Level)
		&& pItem->Type != ITEM_HELPER + 106
		&& pItem->Type != ITEM_HELPER + 107
		|| pItem->bPeriodItem != true
		|| pItem->bExpiredPeriod != true)
	{
		if (pItem->Type == ITEM_POTION + 11
			|| pItem->Type == ITEM_HELPER + 20 && (Level == 1 || Level == 2)
			|| pItem->bPeriodItem == true && !pItem->bExpiredPeriod && pItem->Type == 6676 && !Level
			|| pItem->Type == ITEM_HELPER + 20 && (Level == 1 || Level == 2)
			|| pItem->Type == ITEM_HELPER + 19
			|| pItem->Type == ITEM_POTION + 20 && Level >= 1 && Level <= 5
			|| pItem->Type == ITEM_POTION + 151
			|| pItem->Type == ITEM_POTION + 152
			|| IsLuckySetItem(pItem->Type) == 1 && pItem->Durability > 0
			|| !IsLuckySetItem(pItem->Type) && Check_ItemAction(pItem, eITEM_SELL))
		{
			return true;
		}
	}

	if (true == pItem->bPeriodItem && true == pItem->bExpiredPeriod)
	{
		return true;
	}

	/*Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

	if (ItemInfo->SellNpc == true)
	{
		return false;
	}*/

	return false;
}

bool IsRepairBan(ITEM* pItem)
{
	if (g_ChangeRingMgr->CheckRepair(pItem->Type) == true)
	{
		return true;
	}
	if (IsPartChargeItem(pItem) == true || ((pItem->Type >= ITEM_TYPE_CHARM_MIXWING + EWS_BEGIN) && (pItem->Type <= ITEM_TYPE_CHARM_MIXWING + EWS_END)))
	{
		return true;
	}

	if ((pItem->Type >= ITEM_POTION + 55 && pItem->Type <= ITEM_POTION + 57)
		|| pItem->Type == MODEL_HELPER + 43
		|| pItem->Type == MODEL_HELPER + 44
		|| pItem->Type == MODEL_HELPER + 45
		|| (pItem->Type >= ITEM_HELPER && pItem->Type <= ITEM_HELPER + 3)
		|| pItem->Type == ITEM_BOW + 7
		|| pItem->Type == ITEM_BOW + 15
		|| pItem->Type >= ITEM_POTION
		|| (pItem->Type >= ITEM_WING + 7 && pItem->Type <= ITEM_WING + 19)
		|| (pItem->Type >= ITEM_HELPER + 14 && pItem->Type <= ITEM_HELPER + 19)
		|| pItem->Type == ITEM_POTION + 21
		|| pItem->Type == ITEM_HELPER + 4
		|| pItem->Type == ITEM_HELPER + 5
		|| pItem->Type == ITEM_HELPER + 38
		|| pItem->Type == ITEM_HELPER + 67
		|| pItem->Type == ITEM_HELPER + 80
		|| pItem->Type == ITEM_HELPER + 76
		|| pItem->Type == ITEM_HELPER + 122
		|| pItem->Type == ITEM_HELPER + 123
		|| pItem->Type == ITEM_HELPER + 106
		|| pItem->Type == ITEM_POTION + 84
		|| pItem->Type == ITEM_POTION + 85
		|| pItem->Type == ITEM_POTION + 86
		|| pItem->Type == ITEM_POTION + 87
		|| pItem->Type == ITEM_POTION + 88
		|| pItem->Type == ITEM_POTION + 89
		|| pItem->Type == ITEM_POTION + 90
		|| g_pMyInventory->IsInvenItem(pItem->Type)
		|| pItem->Type == ITEM_HELPER + 7
		|| pItem->Type == ITEM_HELPER + 10
		|| pItem->Type == ITEM_HELPER + 11
		|| pItem->Type == ITEM_HELPER + 20
		|| pItem->Type == ITEM_HELPER + 29
		|| pItem->Type == ITEM_HELPER + 32
		|| pItem->Type == ITEM_HELPER + 33
		|| pItem->Type == ITEM_HELPER + 34
		|| pItem->Type == ITEM_HELPER + 35
		|| pItem->Type == ITEM_HELPER + 36
		|| pItem->Type == ITEM_HELPER + 37
		|| pItem->Type == ITEM_HELPER + 49
		|| pItem->Type == ITEM_HELPER + 50
		|| pItem->Type == ITEM_HELPER + 51
		|| pItem->Type == ITEM_HELPER + 66
		|| pItem->Type == ITEM_HELPER + 71
		|| pItem->Type == ITEM_HELPER + 72
		|| pItem->Type == ITEM_HELPER + 73
		|| pItem->Type == ITEM_HELPER + 74
		|| pItem->Type == ITEM_HELPER + 75
		)
	{
		return true;
	}

	if (Check_ItemAction(pItem, eITEM_REPAIR))
		return true;

	/*Script_Item* ItemInfo = GMItemMng->find(pItem->Type);

	if (ItemInfo->Repair == false)
	{
		return true;
	}*/

	return false;
}

bool IsWingItem(ITEM* pItem)
{
	switch (pItem->Type)
	{
	case ITEM_WING:
	case ITEM_WING + 1:
	case ITEM_WING + 2:
	case ITEM_WING + 3:
	case ITEM_WING + 4:
	case ITEM_WING + 5:
	case ITEM_WING + 6:
	case ITEM_HELPER + 30:
	case ITEM_WING + 36:
	case ITEM_WING + 37:
	case ITEM_WING + 38:
	case ITEM_WING + 39:
	case ITEM_WING + 40:
	case ITEM_WING + 41:
	case ITEM_WING + 42:
	case ITEM_WING + 43:
	case ITEM_WING + 130:
	case ITEM_WING + 131:
	case ITEM_WING + 132:
	case ITEM_WING + 133:
	case ITEM_WING + 134:
	case ITEM_WING + 49:
	case ITEM_WING + 50:
	case ITEM_WING + 135:
		return true;
	}

	return false;
}

OBJECT ObjectSelect;


void RenderItem3D(float sx, float sy, float Width, float Height, int Type, int Level, int Option1, int ExtOption, bool PickUp, float _renderposx, float _renderposy, float _rendersizex, float _rendersizey, float Scale)
{
	if (Type != -1)
	{
		bool Success = false;

		if (_renderposx == 0.0)
			_renderposx = sx;

		if (_renderposy == 0.0)
			_renderposy = sy;

		if (_rendersizex == 0.0)
			_rendersizex = Width;

		if (_rendersizey == 0.0)
			_rendersizey = Height;

		if ((g_pPickedItem == NULL || PickUp) && SEASON3B::CheckMouseIn(_renderposx, _renderposy, _rendersizex, _rendersizey))
		{
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) || (g_pNewUISystem->CheckMouseUse() == false))
			{
				Success = true;
			}
		}

		if (g_pInGameShop->IsInGameShopRect(_renderposx, _renderposy))
		{
			g_pInGameShop->SetConvertInvenCoord(Type, Width, Height);
			g_pInGameShop->SetRateScale(Type);
			sx = g_pInGameShop->GetConvertPos().x + sx;
			sy = g_pInGameShop->GetConvertPos().y + sy;
			Width = g_pInGameShop->GetConvertSize().x;
			Height = g_pInGameShop->GetConvertSize().y;
		}

		float pos[2];
		float ItemScale;
		vec3_t ItemAngle;

		int itemLevel = Level >> 3;
		int modelType = Type + MODEL_SWORD;

		ITEM_POSITION* RectItem3D = g_pNewItemScript->FindItemRelative(Type);

		pos[0] = RectItem3D->X;
		pos[1] = RectItem3D->Y;

		VectorCopy(RectItem3D->angle, ItemAngle);

		CUSTOM_PET_STACK* pLStack = GMProtect->GetPetAction(Type);

		if (pLStack && pLStack->RenderType != -1)
		{
			Type = pLStack->RenderType;

			modelType = Type + MODEL_SWORD;
		}

		if (Scale > 0.0)
			ItemScale = Scale;
		else
			ItemScale = RectItem3D->scale;

		if (Type != ITEM_HELPER + (14) || itemLevel != 1)
		{
			if (Type != ITEM_HELPER + (11) || itemLevel != 1)
			{
				if (Type == ITEM_HELPER + (19))
				{
					if (itemLevel)
					{
						if (itemLevel == 1)
						{
							modelType = MODEL_SWORD + 19;
							pos[0] = 0.7;
							pos[1] = 0.8;
							Vector(180.0, 270.0, 15.0, ItemAngle);
							ItemScale = 0.001;
						}
						else if (itemLevel == 2)
						{
							modelType = MODEL_BOW + 18;
							pos[0] = 0.7;
							pos[1] = 0.7;
							Vector(90.0, 180.0, 20.0, ItemAngle);
							ItemScale = 0.0015;
						}
					}
					else
					{
						modelType = MODEL_STAFF + 10;
						pos[0] = 0.5;
						pos[1] = 0.5;
						Vector(180.0, 270.0, 15.0, ItemAngle);
						ItemScale = 0.001;
					}
				}
				else if (Type == ITEM_HELPER + (20))
				{
					if (itemLevel)
					{
						if (itemLevel > 0 && itemLevel <= 3)
						{
							modelType = MODEL_EVENT + 14;
							pos[0] = 0.5;
							pos[1] = 0.8;
							Vector(255.0, 160.0, 0.0, ItemAngle);
							ItemScale = 0.0028;
						}
					}
					else
					{
						modelType = MODEL_EVENT + 15;
						Vector(270.0, 0.0, 0.0, ItemAngle);
					}
				}
				else if (Type != ITEM_HELPER + (71) && Type != ITEM_HELPER + (72) && Type != ITEM_HELPER + (73) && Type != ITEM_HELPER + (74) && Type != ITEM_HELPER + (75))
				{
					if (Type != ITEM_POTION + (9) || itemLevel != 1)
					{
						if (Type != ITEM_POTION + (12) || itemLevel != 1)
						{
							if (Type == ITEM_POTION + (21))
							{
								switch (itemLevel)
								{
								case 1:
									modelType = MODEL_EVENT + 11;
									pos[0] = 0.4;
									pos[1] = 0.8;
									Vector(-90.0, -20.0, -20.0, ItemAngle);
									ItemScale = 0.0015;
									break;
								case 2:
									modelType = MODEL_EVENT + 11;
									pos[0] = 0.4;
									pos[1] = 0.8;
									Vector(270.0, 0.0, 0.0, ItemAngle);
									ItemScale = 0.0015;
									break;
								case 3:
									pos[0] = 0.5;
									pos[1] = 0.5;
									Vector(-90.0, -10.0, 0.0, ItemAngle);
									ItemScale = 0.002;
									break;
								}
							}
							else if (Type != ITEM_POTION + (23) || itemLevel != 1)
							{
								if (Type != ITEM_POTION + (24) || itemLevel != 1)
								{
									if (Type != ITEM_POTION + (32) || itemLevel != 1)
									{
										if (Type != ITEM_POTION + (33) || itemLevel != 1)
										{
											if (Type != ITEM_POTION + (34) || itemLevel != 1)
											{
												if (Type < ITEM_POTION + (46) || Type > ITEM_POTION + (48))
												{
													if (Type == ITEM_POTION + (100))
													{
														if (g_pLuckyCoinRegistration->GetItemRotation())
															Success = true;
													}
													else if (Type != ITEM_ARMOR + (59) && Type != ITEM_ARMOR + (83))
													{
														switch (Type)
														{
														case ITEM_ARMOR + 60:
															modelType = MODEL_ARMORINVEN_61;
															break;
														case ITEM_ARMOR + 61:
															modelType = MODEL_ARMORINVEN_62;
															break;
														case ITEM_ARMOR + 73:
															modelType = MODEL_ARMORINVEN_74;
															break;
														}
													}
													else
													{
														modelType = MODEL_ARMORINVEN_60;
													}
												}
												else
												{
													modelType = MODEL_POTION + 46;
												}
											}
											else
											{
												modelType = MODEL_EVENT + 23;
												pos[0] = 0.5;
												pos[1] = 0.57;
											}
										}
										else
										{
											modelType = MODEL_EVENT + 22;
											pos[0] = 0.5;
											pos[1] = 0.57;
										}
									}
									else
									{
										modelType = MODEL_EVENT + 21;
										pos[0] = 0.5;
										pos[1] = 0.45;
									}
								}
								else
								{
									modelType = MODEL_EVENT + 13;
									pos[0] = 0.5;
									pos[1] = 0.8;
									Level = -1;
								}
							}
							else
							{
								modelType = MODEL_EVENT + 12;
								Level = -1;
								ItemScale = 0.0012;
								Vector(250.0, 140.0, 0.0, ItemAngle);
							}
						}
						else
						{
							Vector(270.0, -90.0, 0.0, ItemAngle);
							ItemScale = 0.0035;
						}
					}
					else
					{
						modelType = MODEL_EVENT + 7;
						pos[0] = 0.5;
						pos[1] = 0.8;
						Vector(270.0, 0.0, 0.0, ItemAngle);
						ItemScale = 0.0025;
					}
				}
				else if (!Success)
				{
					//ItemAngle[1] = flt_62367C4 * 0.2000000029802322;
				}
			}
			else
			{
				modelType = MODEL_EVENT + 18;
				pos[0] = 0.5;
				pos[1] = 0.5;
			}
		}
		else
		{
			modelType = MODEL_EVENT + 16;
			pos[0] = 0.55;
			pos[1] = 0.85;
			Vector(-90.0, 0.0, 0.0, ItemAngle);
			ItemScale = 0.002;
		}

		if (Type == ITEM_POTION + 11)
		{
			if (itemLevel > 6)
			{
				if (itemLevel == 13)
				{
					modelType = MODEL_EVENT + 6;
					pos[0] = 0.5;
					pos[1] = 0.5;
					Vector(270.0, 90.0, 0.0, ItemAngle);
					ItemScale = 0.0039;
				}
				else if (itemLevel > 13 && itemLevel <= 15)
				{
					modelType = MODEL_EVENT + 7;
					pos[0] = 0.5;
					pos[1] = 0.8;
					Vector(270.0, 0.0, 0.0, ItemAngle);
					ItemScale = 0.0025;
				}
				else if (itemLevel >= 8)
				{
					modelType = MODEL_EVENT + 10;
					Vector(270.0, -10.0, 0.0, ItemAngle);
					ItemScale = 0.001;
				}
			}
			else
			{
				switch (itemLevel)
				{
				case 1:
					modelType = MODEL_EVENT + 4;
					Vector(270.0, -10.0, 0.0, ItemAngle);
					ItemScale = 0.0025;
					break;
				case 2:
					modelType = MODEL_EVENT + 5;
					Vector(270.0, 0.0, 0.0, ItemAngle);
					ItemScale = 0.0025;
					break;
				case 3:
					modelType = MODEL_EVENT + 6;
					pos[0] = 0.5;
					pos[1] = 0.5;
					Vector(270.0, 90.0, 0.0, ItemAngle);
					ItemScale = 0.0039;
					break;
				case 5:
					modelType = MODEL_EVENT + 8;
					Vector(270.0, -10.0, 0.0, ItemAngle);
					ItemScale = 0.0015;
					break;
				case 6:
					modelType = MODEL_EVENT + 9;
					Vector(270.0, -10.0, 0.0, ItemAngle);
					ItemScale = 0.0019;
					break;
				}
			}
		}

		vec3_t Position;

		int x = (Width * pos[0]) + sx;
		int y = (Height * pos[1]) + sy;

		CreateScreenVector(x, y, Position, false);

		VectorCopy(ItemAngle, ObjectSelect.Angle);

		RenderObjectScreen(modelType, Level, Option1, ExtOption, Position, Success, PickUp, ItemScale);
	}
}

void RenderObjectScreen(int Type, int ItemLevel, int Option1, int ExtOption, vec3_t Target, int Select, bool PickUp, float Scale)
{
	vec3_t Direction, Position;
	int Level = (ItemLevel >> 3) & 15;

	VectorSubtract(Target, MousePosition, Direction);

	if (PickUp)
		VectorMA(MousePosition, (float)ScaleMA(0.07), Direction, Position);
	else
		VectorMA(MousePosition, (float)ScaleMA(0.10), Direction, Position);

	if (Select == TRUE)
	{
		static float lasted_rotation = 0.0f;
		ObjectSelect.Angle[1] = lasted_rotation + timefac(14);
		lasted_rotation = ObjectSelect.Angle[1];
	}

	ObjectSelect.Type = Type;

	if ((ObjectSelect.Type < MODEL_HELM || ObjectSelect.Type >= MODEL_WING)
		&& ObjectSelect.Type != MODEL_ARMORINVEN_60
		&& ObjectSelect.Type != MODEL_ARMORINVEN_61
		&& ObjectSelect.Type != MODEL_ARMORINVEN_62
		&& ObjectSelect.Type != MODEL_ARMORINVEN_74)
	{
		if (ObjectSelect.Type == MODEL_POTION + 12)
		{
			if (Level)
			{
				if (Level == 2)
				{
					ObjectSelect.Type = MODEL_EVENT + 1;
					Type = MODEL_EVENT + 1;
				}
			}
			else
			{
				ObjectSelect.Type = MODEL_EVENT;
				Type = MODEL_EVENT;
			}
		}
	}
	else
	{
		ObjectSelect.Type = MODEL_PLAYER;
	}

	BMD* pModel = gmClientModels->GetModel(ObjectSelect.Type);

	if (!pModel)
		return;

	pModel->CurrentAction = 0;
	ObjectSelect.AnimationFrame = 0;
	ObjectSelect.PriorAnimationFrame = 0;
	ObjectSelect.PriorAction = 0;

	if (Type < MODEL_HELM || Type >= MODEL_ARMOR)
	{
		if (Type < MODEL_ARMOR || Type >= MODEL_PANTS)
		{
			if (Type < MODEL_GLOVES || Type >= MODEL_BOOTS)
			{
				if (Type < MODEL_PANTS || Type >= MODEL_GLOVES)
				{
					pModel->BodyHeight = 0.0;
				}
				else
				{
					pModel->BodyHeight = -50.0;
				}
			}
			else
			{
				pModel->BodyHeight = -70.0;
			}
		}
		else
		{
			pModel->BodyHeight = -100.0;
			if (Check_LuckyItem(Type - MODEL_ITEM, 0))
			{
				pModel->BodyHeight -= 13.0;
			}
		}
	}
	else
	{
		pModel->BodyHeight = -160.0;
		if (Check_LuckyItem(Type - MODEL_ITEM, 0))
			pModel->BodyHeight -= 10.0;

		if (Type == MODEL_HELM + 65 || Type == MODEL_HELM + 70)
			Position[0] += 0.04;
	}

	if (Type == MODEL_ARMORINVEN_60 || Type == MODEL_ARMORINVEN_61 || Type == MODEL_ARMORINVEN_62)
	{
		pModel->BodyHeight -= 100;
	}

	pModel->Animation(BoneTransform, ObjectSelect.AnimationFrame, ObjectSelect.PriorAnimationFrame, ObjectSelect.PriorAction, ObjectSelect.Angle, ObjectSelect.HeadAngle, false, false);

	CHARACTER Armor;

	OBJECT* o = &Armor.Object;
	o->Type = Type;

	ItemObjectAttribute(o);

	o->LightEnable = false;

	Armor.Class = 2;

	int ScreenPos_X = 0;
	int ScreenPos_Y = 0;
	Projection(Position, &ScreenPos_X, &ScreenPos_Y);

	if (g_pInGameShop->IsInGameShopRect(ScreenPos_X, ScreenPos_Y))
	{
		float Rate = g_pInGameShop->GetRateScale();
		o->Scale = Scale * Rate;
	}
	else
	{
		o->Scale = Scale;
	}

	vec3_t Light;
	Vector(1.f, 1.f, 1.f, Light);
	VectorCopy(Position, o->Position);

	if (GMItemMng->IsLuckyItem(Type - MODEL_ITEM))
		RenderPartObject(o, Type, NULL, Light, o->Alpha, ItemLevel, Option1, MAX_MODELS, true, true, true, 0, RENDER_TEXTURE);
	else
		RenderPartObject(o, Type, NULL, Light, o->Alpha, ItemLevel, Option1, ExtOption, true, true, true, 0, RENDER_TEXTURE);
}

void InventoryColor(ITEM* p)
{
	switch (p->Color)
	{
	case 0:
		glColor3f(1.f, 1.f, 1.f);
		break;
	case 1:
		glColor3f(0.8f, 0.8f, 0.8f);
		break;
	case 2:
		glColor3f(0.6f, 0.7f, 1.f);
		break;
	case 3:
		glColor3f(1.f, 0.2f, 0.1f);
		break;
	case 4:
		glColor3f(0.5f, 1.f, 0.6f);
		break;
	case 5:
		glColor4f(0.8f, 0.7f, 0.f, 1.f);
		break;
	case 6:
		glColor4f(0.8f, 0.5f, 0.f, 1.f);
		break;
	case 7:
		glColor4f(0.8f, 0.3f, 0.3f, 1.f);
		break;
	case 8:
		glColor4f(1.0f, 0.f, 0.f, 1.f);
		break;
	case 99:
		glColor3f(1.f, 0.2f, 0.1f);
		break;
	}
}

void RenderEqiupmentBox()
{
	int StartX = InventoryStartX;
	int StartY = InventoryStartY;
	float x, y, Width, Height;

	EnableAlphaTest();

	//helper
	Width = 40.f; Height = 40.f; x = 15.f; y = 46.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_HELPER]);
	RenderBitmap(BITMAP_INVENTORY + 15, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);
	//wing
	Width = 60.f; Height = 40.f; x = 115.f; y = 46.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WING]);
	RenderBitmap(BITMAP_INVENTORY + 14, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);
	if (gCharacterManager.GetBaseClass(CharacterAttribute->Class) != CLASS_DARK)
	{
		//helmet
		Width = 40.f; Height = 40.f; x = 75.f; y = 46.f;
		InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_HELM]);
		RenderBitmap(BITMAP_INVENTORY + 3, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);
	}
	//weapon right
	Width = 40.f; Height = 60.f; x = 15.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT]);
	RenderBitmap(BITMAP_INVENTORY + 6, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);

	//armor upper
	Width = 40.f; Height = 60.f; x = 75.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_ARMOR]);
	RenderBitmap(BITMAP_INVENTORY + 4, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);

	//weapon left
	Width = 40.f; Height = 60.f; x = 134.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT]);
	RenderBitmap(BITMAP_INVENTORY + 16, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);

	//glove
	if (gCharacterManager.GetBaseClass(CharacterAttribute->Class) != CLASS_RAGEFIGHTER)
	{
		Width = 40.f; Height = 40.f; x = 15.f; y = 152.f;
		InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_GLOVES]);
		RenderBitmap(BITMAP_INVENTORY + 7, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);
	}

	//armor lower
	Width = 40.f; Height = 40.f; x = 75.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_PANTS]);
	RenderBitmap(BITMAP_INVENTORY + 5, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);

	//boot
	Width = 40.f; Height = 40.f; x = 134.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_BOOTS]);
	RenderBitmap(BITMAP_INVENTORY + 8, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 64.f, Height / 64.f);
	//necklace
	Width = 20.f; Height = 20.f; x = 55.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_AMULET]);
	RenderBitmap(BITMAP_INVENTORY + 9, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 32.f, Height / 32.f);
	//ring
	Width = 20.f; Height = 20.f; x = 55.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT]);
	RenderBitmap(BITMAP_INVENTORY + 10, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 32.f, Height / 32.f);
	//ring
	Width = 20.f; Height = 20.f; x = 115.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_RING_LEFT]);
	RenderBitmap(BITMAP_INVENTORY + 10, x + StartX, y + StartY, Width, Height, 0.f, 0.f, Width / 32.f, Height / 32.f);
}

void RenderEqiupmentPart3D(int Index, float sx, float sy, float Width, float Height)
{
	ITEM* p = &CharacterMachine->Equipment[Index];
	if (p->Type != -1)
	{
		if (p->Number > 0)
			RenderItem3D(sx, sy, Width, Height, p->Type, p->Level, p->Option1, p->ExtOption, false);
	}
}

void RenderEqiupment3D()
{
	int StartX = InventoryStartX;
	int StartY = InventoryStartY;
	float x, y, Width, Height;

	//helper
	Width = 40.f; Height = 40.f; x = 15.f; y = 46.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_HELPER]);
	RenderEqiupmentPart3D(EQUIPMENT_HELPER, StartX + x, StartY + y, Width, Height);
	//wing
	Width = 60.f; Height = 40.f; x = 115.f; y = 46.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WING]);
	RenderEqiupmentPart3D(EQUIPMENT_WING, StartX + x, StartY + y, Width, Height);
	//helmet
	Width = 40.f; Height = 40.f; x = 75.f; y = 46.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_HELM]);
	RenderEqiupmentPart3D(EQUIPMENT_HELM, StartX + x, StartY + y, Width, Height);
	//armor upper
	Width = 40.f; Height = 60.f; x = 75.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_ARMOR]);
	RenderEqiupmentPart3D(EQUIPMENT_ARMOR, StartX + x, StartY + y - 10, Width, Height);
	//armor lower
	Width = 40.f; Height = 40.f; x = 75.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_PANTS]);
	RenderEqiupmentPart3D(EQUIPMENT_PANTS, StartX + x, StartY + y, Width, Height);
	//weapon right
	Width = 40.f; Height = 60.f; x = 15.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT]);
	RenderEqiupmentPart3D(EQUIPMENT_WEAPON_RIGHT, StartX + x, StartY + y, Width, Height);
	//weapon left
	Width = 40.f; Height = 60.f; x = 134.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT]);
	RenderEqiupmentPart3D(EQUIPMENT_WEAPON_LEFT, StartX + x, StartY + y, Width, Height);
	//glove
	Width = 40.f; Height = 40.f; x = 15.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_GLOVES]);
	RenderEqiupmentPart3D(EQUIPMENT_GLOVES, StartX + x, StartY + y, Width, Height);
	//boot
	Width = 40.f; Height = 40.f; x = 134.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_BOOTS]);
	RenderEqiupmentPart3D(EQUIPMENT_BOOTS, StartX + x, StartY + y, Width, Height);
	//necklace
	Width = 20.f; Height = 20.f; x = 55.f; y = 89.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_AMULET]);
	RenderEqiupmentPart3D(EQUIPMENT_AMULET, StartX + x, StartY + y, Width, Height);
	//ring
	Width = 20.f; Height = 20.f; x = 55.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT]);
	RenderEqiupmentPart3D(EQUIPMENT_RING_RIGHT, StartX + x, StartY + y, Width, Height);
	//ring
	Width = 20.f; Height = 20.f; x = 115.f; y = 152.f;
	InventoryColor(&CharacterMachine->Equipment[EQUIPMENT_RING_LEFT]);
	RenderEqiupmentPart3D(EQUIPMENT_RING_LEFT, StartX + x, StartY + y, Width, Height);
}

bool CheckEmptyInventory(ITEM* Inv, int InvWidth, int InvHeight)
{
	bool Empty = true;
	for (int y = 0; y < InvHeight; y++)
	{
		for (int x = 0; x < InvWidth; x++)
		{
			int Index = y * InvWidth + x;
			ITEM* p = &Inv[Index];
			if (p->Type != -1 /*&& p->Number > 0*/)
			{
				Empty = false;
			}
		}
	}
	return Empty;
}

void InitPartyList()
{
	PartyNumber = 0;
	PartyKey = 0;
}

void MoveServerDivisionInventory()
{
	if (!g_pUIManager->IsOpen(INTERFACE_SERVERDIVISION))
		return;
	int x = (int)GetWindowsX - 190;
	int y = 0;
	int Width, Height;

	if (MouseX >= (int)(x) && MouseX < (int)(x + 190) &&
		MouseY >= (int)(y) && MouseY < (int)(y + 256 + 177))
	{
		MouseOnWindow = true;
	}

	Width = 16; Height = 16; x = InventoryStartX + 25; y = 240;
	if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height && MouseLButtonPush)
	{
		g_bServerDivisionAccept ^= true;

		MouseLButtonPush = false;
		MouseLButton = false;
	}

	if (g_bServerDivisionAccept)
	{
		Width = 120; Height = 24; x = (float)InventoryStartX + 35; y = 320;
		if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height && MouseLButtonPush)
		{
			MouseLButtonPush = false;
			MouseLButton = false;
			AskYesOrNo = 4;
			OkYesOrNo = -1;

			ShowCheckBox(1, 448, MESSAGE_CHECK);
		}
	}

	Width = 120; Height = 24; x = (float)InventoryStartX + 35; y = 350;
	if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height && MouseLButtonPush)
	{
		MouseLButtonPush = false;
		MouseLButton = false;
		MouseUpdateTime = 0;
		MouseUpdateTimeMax = 6;

		SendExitInventory();
		g_pUIManager->CloseAll();
	}

	Width = 24; Height = 24; x = InventoryStartX + 25; y = InventoryStartY + 395;
	if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
	{
		if (MouseLButtonPush)
		{
			MouseLButtonPush = false;
			MouseUpdateTime = 0;
			MouseUpdateTimeMax = 6;

			g_bEventChipDialogEnable = EVENT_NONE;

			SendExitInventory();
			g_pUIManager->CloseAll();
		}
	}
}

void HideKeyPad(void)
{
	g_iKeyPadEnable = 0;
}

int CheckMouseOnKeyPad(void)
{
	int Width, Height, WindowX, WindowY;
	Width = 213; Height = 2 * 5 + 6 * 40; WindowX = ((int)GetWindowsX - Width) / 2; WindowY = 60 + 40;//60 220

	int iButtonTop = 50;

	for (int i = 0; i < 11; ++i)
	{
		int xButton = i % 5;
		int yButton = i / 5;

		int xLeft = WindowX + 10 + xButton * 40;
		int yTop = WindowY + iButtonTop + yButton * 40;
		if (xLeft <= MouseX && MouseX < xLeft + 32 &&
			yTop <= MouseY && MouseY < yTop + 32)
		{
			return (i);
		}
	}
	// Ok, Cancel ( 11 - 12)
	int yTop = WindowY + iButtonTop + 2 * 40 + 5;

	for (int i = 0; i < 2; ++i)
	{
		int xLeft = WindowX + 52 + i * 78;
		if (xLeft <= MouseX && MouseX < xLeft + 70 &&
			yTop <= MouseY && MouseY < yTop + 21)
		{
			return (11 + i);
		}
	}

	return (-1);
}

bool g_bPadPushed = false;

void MovePersonalShop()
{
	if ((g_pUIManager->IsOpen(INTERFACE_PERSONALSHOPSALE) || g_pUIManager->IsOpen(INTERFACE_PERSONALSHOPPURCHASE)) && g_iPShopWndType == PSHOPWNDTYPE_SALE)
	{
		if (g_iPersonalShopMsgType == 1)
		{
			if (OkYesOrNo == 1)
			{
				g_iPersonalShopMsgType = 0;
				OkYesOrNo = -1;
			}
			else if (OkYesOrNo == 2)
			{
				g_iPersonalShopMsgType = 0;
				OkYesOrNo = -1;
			}
		}
		g_ptPersonalShop.x = GetWindowsX - 190 * 2;
		g_ptPersonalShop.y = 0;

		int Width = 56, Height = 24;
		int ButtonX = g_ptPersonalShop.x + 30, ButtonY = g_ptPersonalShop.y + 396;
		if (MouseX >= ButtonX && MouseX < ButtonX + Width && MouseY >= ButtonY && MouseY < ButtonY + Height && MouseLButtonPush)
		{
			MouseLButtonPush = false;
			if (!IsExistUndecidedPrice() && strlen(g_szPersonalShopTitle) > 0)
			{
				if (g_bEnablePersonalShop)
				{
					SendRequestCreatePersonalShop(g_szPersonalShopTitle);
					g_pUIManager->Close(INTERFACE_INVENTORY);
				}
				else
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPersonalshopCreateMsgBoxLayout));
				}
			}
			else
			{
				g_pChatListBox->AddText("", GlobalText[1119], SEASON3B::TYPE_ERROR_MESSAGE);
			}
		}

		ButtonX = g_ptPersonalShop.x + 105;
		if (MouseX >= ButtonX && MouseX < ButtonX + Width && MouseY >= ButtonY && MouseY < ButtonY + Height && MouseLButtonPush)
		{
			MouseLButtonPush = false;
			if (g_bEnablePersonalShop)
			{
				SendRequestDestoryPersonalShop();
			}
		}

		Width = 150;
		ButtonX = g_ptPersonalShop.x + 20;
		ButtonY = g_ptPersonalShop.y + 65;
		if (MouseX >= ButtonX && MouseX < ButtonX + Width && MouseY >= ButtonY && MouseY < ButtonY + Height && MouseLButtonPush)
		{
			OpenPersonalShopMsgWnd(1);
		}
	}
}

void ClosePersonalShop()
{
	if (g_iPShopWndType == PSHOPWNDTYPE_PURCHASE)
	{
		memcpy(g_PersonalShopInven, g_PersonalShopBackup, sizeof(ITEM) * MAX_PERSONALSHOP_INVEN);
		if (IsShopInViewport(Hero))
		{
			std::string title;
			GetShopTitle(Hero, title);
			strcpy(g_szPersonalShopTitle, title.c_str());
		}
		else
		{
			g_szPersonalShopTitle[0] = '\0';
		}
		if (g_PersonalShopSeller.Key)
		{
			SendRequestClosePersonalShop(g_PersonalShopSeller.Key, g_PersonalShopSeller.ID);
		}
	}

	g_PersonalShopSeller.Initialize();

	g_iPShopWndType = PSHOPWNDTYPE_NONE;
}

void ClearPersonalShop()
{
	g_bEnablePersonalShop = false;
	g_iPShopWndType = PSHOPWNDTYPE_NONE;
	g_iPersonalShopMsgType = 0;
	g_szPersonalShopTitle[0] = '\0';

	RemoveAllShopTitle();
}

bool IsExistUndecidedPrice()
{
	bool bResult = true;

	for (int i = 0; i < MAX_PERSONALSHOP_INVEN; ++i)
	{
		int iIndex;
		PERSONAL_ITEM_PRICE_TABLE price_table;
		ITEM* pItem = g_pMyShopInventory->FindItem(i);

		if (pItem)
		{
			bResult = false;

			iIndex = MAX_MY_INVENTORY_INDEX + (pItem->y * COL_PERSONALSHOP_INVEN) + pItem->x;

			if (GetPersonalItemPrice(iIndex, price_table, g_IsPurchaseShop) == false)
			{
				return true;
			}

			if (price_table.Price <= 0
				&& price_table.JewelOfBless <= 0
				&& price_table.JewelOfSoul <= 0
				&& price_table.JewelOfChaos <= 0)
			{
				return true;
			}
		}
		else
		{
			continue;
		}
	}

	return bResult;
}

void OpenPersonalShopMsgWnd(int iMsgType)
{
	if (iMsgType == 1)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPersonalShopNameMsgBoxLayout));
	}
	else if (iMsgType == 2)
	{
		SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CPersonalShopItemValueMsgBoxLayout));
	}
}

bool IsCorrectShopTitle(const char* szShopTitle)
{
	int j = 0;
	char TmpText[2048];
	for (int i = 0; i < (int)strlen(szShopTitle); ++i)
	{
		if (szShopTitle[i] != 32)
		{
			TmpText[j] = szShopTitle[i];
			j++;
		}
	}
	TmpText[j] = 0;

	for (int i = 0; i < AbuseFilterNumber; i++)
	{
		if (FindText(TmpText, AbuseFilter[i]))
		{
			return false;
		}
	}

	int len = strlen(szShopTitle);
	int count = 0;

	for (int i = 0; i < len; i++)
	{
		if (szShopTitle[i] == 0x20) {
			count++;
			if (i == 1 && count >= 2) return false;
		}
		else {
			count = 0;
		}
	}
	if (count >= 2)
		return false;
	return true;
}

extern DWORD g_dwActiveUIID;
extern DWORD g_dwMouseUseUIID;

bool IsStrifeMap(int nMapIndex)
{
	bool bStrifeMap = false;

#if MAIN_UPDATE > 303
	if (!g_ServerListManager->IsNonPvP())
	{
		ENUM_WORLD aeStrife[1] = { WD_63PK_FIELD };

		for (int i = 0; i < 1; ++i)
		{
			if (aeStrife[i] == nMapIndex)
			{
				bStrifeMap = true;
				break;
			}
		}
	}
#endif
	return bStrifeMap;
}

unsigned int MarkColor[16];

void CreateGuildMark(int nMarkIndex, bool blend)
{
	BITMAP_t* b = &Bitmaps[BITMAP_GUILD];
	int Width, Height;
	Width = (int)b->Width;
	Height = (int)b->Height;
	BYTE* Buffer = b->Buffer.data();
	int alpha = 128;
	if (blend)
	{
		alpha = 0;
	}

	for (int i = 0; i < 16; i++)
	{
		switch (i)
		{
		case 0:
			MarkColor[i] = (alpha << 24) + (0 << 16) + (0 << 8) + (0);
			break;
		case 1:
			MarkColor[i] = (255 << 24) + (0 << 16) + (0 << 8) + (0);
			break;
		case 2:
			MarkColor[i] = (255 << 24) + (128 << 16) + (128 << 8) + (128);
			break;
		case 3:
			MarkColor[i] = (255 << 24) + (255 << 16) + (255 << 8) + (255);
			break;
		case 4:
			MarkColor[i] = (255 << 24) + (0 << 16) + (0 << 8) + (255);
			break;
		case 5:
			MarkColor[i] = (255 << 24) + (0 << 16) + (128 << 8) + (255);
			break;
		case 6:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (255);
			break;
		case 7:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (128);
			break;
		case 8:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (0);
			break;
		case 9:
			MarkColor[i] = (255 << 24) + (128 << 16) + (255 << 8) + (0);
			break;
		case 10:
			MarkColor[i] = (255 << 24) + (255 << 16) + (255 << 8) + (0);
			break;
		case 11:
			MarkColor[i] = (255 << 24) + (255 << 16) + (128 << 8) + (0);
			break;
		case 12:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (0);
			break;
		case 13:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (128);
			break;
		case 14:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (255);
			break;
		case 15:
			MarkColor[i] = (255 << 24) + (128 << 16) + (0 << 8) + (255);
			break;
		}
	}
	BYTE* MarkBuffer = GuildMark[nMarkIndex].Mark;

	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			*((unsigned int*)(Buffer)) = MarkColor[MarkBuffer[0]];
			Buffer += 4;
			MarkBuffer++;
		}
	}

	glBindTexture(GL_TEXTURE_2D, b->TextureNumber);

	glTexImage2D(GL_TEXTURE_2D, 0, b->Components, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, b->Buffer.data());
}

void CreateCastleMark(int Type, BYTE* buffer, bool blend)
{
	if (buffer == NULL) return;

	BITMAP_t* b = &Bitmaps[Type];

	int Width, Height;

	Width = (int)b->Width;
	Height = (int)b->Height;
	BYTE* Buffer = b->Buffer.data();

	int alpha = 128;
	if (blend)
	{
		alpha = 0;
	}

	for (int i = 0; i < 16; i++)
	{
		switch (i)
		{
		case 0:
			MarkColor[i] = (alpha << 24) + (0 << 16) + (0 << 8) + (0);
			break;
		case 1:
			MarkColor[i] = (255 << 24) + (0 << 16) + (0 << 8) + (0);
			break;
		case 2:
			MarkColor[i] = (255 << 24) + (128 << 16) + (128 << 8) + (128);
			break;
		case 3:
			MarkColor[i] = (255 << 24) + (255 << 16) + (255 << 8) + (255);
			break;
		case 4:
			MarkColor[i] = (255 << 24) + (0 << 16) + (0 << 8) + (255);
			break;//»¡
		case 5:
			MarkColor[i] = (255 << 24) + (0 << 16) + (128 << 8) + (255);
			break;//
		case 6:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (255);
			break;//³ë
		case 7:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (128);
			break;//
		case 8:
			MarkColor[i] = (255 << 24) + (0 << 16) + (255 << 8) + (0);
			break;//ÃÊ
		case 9:
			MarkColor[i] = (255 << 24) + (128 << 16) + (255 << 8) + (0);
			break;//
		case 10:
			MarkColor[i] = (255 << 24) + (255 << 16) + (255 << 8) + (0);
			break;//Ã»
		case 11:
			MarkColor[i] = (255 << 24) + (255 << 16) + (128 << 8) + (0);
			break;//
		case 12:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (0);
			break;//ÆÄ
		case 13:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (128);
			break;//
		case 14:
			MarkColor[i] = (255 << 24) + (255 << 16) + (0 << 8) + (255);
			break;//º¸
		case 15:
			MarkColor[i] = (255 << 24) + (128 << 16) + (0 << 8) + (255);
			break;//
		}
	}
	BYTE MarkBuffer[32 * 32];

	int offset = 0;

	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			offset = (j / 4) + ((i / 4) * 8);
			MarkBuffer[j + (i * 32)] = buffer[offset];
		}
	}

	offset = 0;
	int offset2 = 0;

	for (int i = 0; i < Height; ++i)
	{
		for (int j = 0; j < Width; ++j)
		{
			if (j >= (Width / 2 - 16) && j < (Width / 2 + 16) && i >= (Height / 2 - 16) && i < (Height / 2 + 16))
			{
				*((unsigned int*)(Buffer + offset)) = MarkColor[MarkBuffer[offset2]];
				offset2++;
			}
			else if (j<3 || j>(Width - 4) || i<10 || i>(Height - 10))
			{
				*((unsigned int*)(Buffer + offset)) = (255 << 24) + (0 << 16) + ((int)(50 + i / 100.f * 160) << 8) + (50 + i / 100.f * 255);
			}
			else
			{
				*((unsigned int*)(Buffer + offset)) = (255 << 24) + (i << 16) + (i << 8) + (i);
			}
			offset += 4;
		}
	}
	glBindTexture(GL_TEXTURE_2D, b->TextureNumber);

	glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, b->Buffer.data());
}


void RenderGuildColor(float x, float y, int SizeX, int SizeY, int Index)
{
	RenderBitmap(BITMAP_INVENTORY + 18, x - 1, y - 1, (float)SizeX + 2, (float)SizeY + 2, 0.f, 0.f, SizeX / 32.f, SizeY / 30.f);

	BITMAP_t* b = &Bitmaps[BITMAP_GUILD];

	int Width, Height;

	Width = (int)b->Width;
	Height = (int)b->Height;

	BYTE* buff = b->Buffer.data();
	BYTE* Buffer = buff;
	unsigned int Color = MarkColor[Index];

	if (Index == 0)
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				*((unsigned int*)(Buffer)) = 255 << 24;
				Buffer += 4;
			}
		}
		Color = (255 << 24) + (128 << 16) + (128 << 8) + (128);
		Buffer = buff;
		for (int i = 0; i < 8; i++)
		{
			*((unsigned int*)(Buffer)) = Color;
			Buffer += 8 * 4 + 4;
		}
		Buffer = buff + 7 * 4;
		for (int i = 0; i < 8; i++)
		{
			*((unsigned int*)(Buffer)) = Color;
			Buffer += 8 * 4 - 4;
		}
	}
	else
	{
		for (int i = 0; i < Height; i++)
		{
			for (int j = 0; j < Width; j++)
			{
				*((unsigned int*)(Buffer)) = Color;
				Buffer += 4;
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, b->TextureNumber);

	glTexImage2D(GL_TEXTURE_2D, 0, b->Components, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, b->Buffer.data());
	RenderBitmap(BITMAP_GUILD, x, y, (float)SizeX, (float)SizeY);
}

void RenderGuildList(int StartX, int StartY)
{
	GuildListStartX = StartX;
	GuildListStartY = StartY;

	glColor3f(1.f, 1.f, 1.f);

	DisableAlphaBlend();
	float x, y, Width, Height;
	Width = 190.f; Height = 256.f; x = (float)StartX; y = (float)StartY;
	RenderBitmap(BITMAP_INVENTORY, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 256.f);
	Width = 190.f; Height = 177.f; x = (float)StartX; y = (float)StartY + 256;
	RenderBitmap(BITMAP_INVENTORY + 1, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 256.f);

	EnableAlphaTest();

	g_pRenderText->SetBgColor(20, 20, 20, 255);
	g_pRenderText->SetTextColor(220, 220, 220, 255);
	g_pRenderText->SetFont(g_hFontBold);

	char Text[100];
	if (Hero->GuildMarkIndex == -1)
		sprintf(Text, GlobalText[180]);
	else
		sprintf(Text, "%s (Score:%d)", GuildMark[Hero->GuildMarkIndex].GuildName, GuildTotalScore);

	g_pRenderText->RenderText(StartX + 95 - 60, StartY + 12, Text, 120 * WindowWidth / GetWindowsX, true, 3);

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(230, 230, 230, 255);
	g_pRenderText->SetFont(g_hFont);

	if (g_nGuildMemberCount == 0)
	{
		g_pRenderText->RenderText(StartX + 20, StartY + 50, GlobalText[185]);
		g_pRenderText->RenderText(StartX + 20, StartY + 65, GlobalText[186]);
		g_pRenderText->RenderText(StartX + 20, StartY + 80, GlobalText[187]);
	}
	g_pRenderText->SetBgColor(0, 0, 0, 128);
	g_pRenderText->SetTextColor(100, 255, 200, 255);
	g_pRenderText->RenderText(StartX + (int)Width / 2, StartY + 44, g_GuildNotice[0], 0, 0, RT3_WRITE_CENTER);
	g_pRenderText->RenderText(StartX + (int)Width / 2, StartY + 58, g_GuildNotice[1], 0, 0, RT3_WRITE_CENTER);

	int yGuildStart = 72;
	int Number = g_nGuildMemberCount;

	if (g_nGuildMemberCount >= MAX_GUILD_LINE)
		Number = MAX_GUILD_LINE;
}

//#define MAX_LENGTH_CMB	( 26)
#define NUM_LINE_CMB	( 7)

void RenderServerDivision()
{
	if (!g_pUIManager->IsOpen(INTERFACE_SERVERDIVISION))
		return;

	float Width, Height, x, y;

	glColor3f(1.f, 1.f, 1.f);
	EnableAlphaTest();

	InventoryStartX = GetWindowsX - 190;
	InventoryStartY = 0;
	Width = 213; Height = 40; x = (float)InventoryStartX; y = (float)InventoryStartY;
	SEASON3B::RenderInventoryInterface((int)x, (int)y, 1);

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(255, 230, 210, 255);

	g_pRenderText->SetFont(g_hFontBold);
	x = InventoryStartX + (190 / 2.f);
	y = 50;
	for (int i = 462; i < 470; ++i)
	{
		g_pRenderText->RenderText(x, y, GlobalText[i], 0, 0, RT3_WRITE_CENTER);
		y += 20;
	}

	g_pRenderText->SetFont(g_hFontBold);
	Width = 16; Height = 16; x = (float)InventoryStartX + 25; y = 240;
	if (g_bServerDivisionAccept)
	{
		g_pRenderText->SetTextColor(212, 150, 0, 255);
		RenderBitmap(BITMAP_INVENTORY_BUTTON + 11, x, y, Width, Height, 0.f, 0.f, 24 / 32.f, 24 / 32.f);
	}
	else
	{
		g_pRenderText->SetTextColor(223, 191, 103, 255);
		RenderBitmap(BITMAP_INVENTORY_BUTTON + 10, x, y, Width, Height, 0.f, 0.f, 24 / 32.f, 24 / 32.f);
	}
	g_pRenderText->RenderText((int)(x + Width + 3), (int)(y + 5), GlobalText[447]);
	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetTextColor(255, 230, 210, 255);

	Width = 120; Height = 24; x = (float)InventoryStartX + 35; y = 350;//(Width/2.f); y = 231;
	RenderBitmap(BITMAP_INTERFACE + 10, (float)x, (float)y, (float)Width, (float)Height, 0.f, 0.f, 213.f / 256.f);
	g_pRenderText->RenderText((int)(x + (Width / 2)), (int)(y + 5), GlobalText[229], 0, 0, RT3_WRITE_CENTER);

	Width = 120; Height = 24; x = (float)InventoryStartX + 35; y = 320;//(Width/2.f); y = 231;
	if (g_bServerDivisionAccept)
		glColor3f(1.f, 1.f, 1.f);
	else
		glColor3f(0.5f, 0.5f, 0.5f);
	RenderBitmap(BITMAP_INTERFACE + 10, (float)x, (float)y, (float)Width, (float)Height, 0.f, 0.f, 213.f / 256.f);
	g_pRenderText->RenderText((int)(x + (Width / 2)), (int)(y + 5), GlobalText[228], 0, 0, RT3_WRITE_CENTER);

	glColor3f(1.f, 1.f, 1.f);
}

BYTE CaculateFreeTicketLevel(int iType)
{
	int iChaLevel = CharacterAttribute->Level;
	int iChaClass = gCharacterManager.GetBaseClass(Hero->Class);
	int iChaExClass = gCharacterManager.IsSecondClass(Hero->Class);

	int iItemLevel = 0;

	if (iType == FREETICKET_TYPE_DEVILSQUARE)
	{
		if (iChaClass == CLASS_DARK)
		{
			if (iChaLevel >= 15 && iChaLevel <= 110)
			{
				iItemLevel = 0;
			}
			else if (iChaLevel >= 111 && iChaLevel <= 160)
			{
				iItemLevel = 8;
			}
			else if (iChaLevel >= 161 && iChaLevel <= 210)
			{
				iItemLevel = 16;
			}
			else if (iChaLevel >= 211 && iChaLevel <= 260)
			{
				iItemLevel = 24;
			}
			else if (iChaLevel >= 261 && iChaLevel <= 310)
			{
				iItemLevel = 32;
			}
			else if (iChaLevel >= 311 && iChaLevel <= 400)
			{
				iItemLevel = 40;
			}
		}
		else
		{
			if (iChaLevel >= 15 && iChaLevel <= 130)
			{
				iItemLevel = 0;
			}
			else if (iChaLevel >= 131 && iChaLevel <= 180)
			{
				iItemLevel = 8;
			}
			else if (iChaLevel >= 181 && iChaLevel <= 230)
			{
				iItemLevel = 16;
			}
			else if (iChaLevel >= 231 && iChaLevel <= 280)
			{
				iItemLevel = 24;
			}
			else if (iChaLevel >= 281 && iChaLevel <= 330)
			{
				iItemLevel = 32;
			}
			else if (iChaLevel >= 331 && iChaLevel <= 400)
			{
				iItemLevel = 40;
			}
		}
		return (iItemLevel >> 3) & 15;
	}
	else if (iType == FREETICKET_TYPE_BLOODCASTLE)
	{
		if (iChaClass == CLASS_DARK)
		{
			if (iChaLevel >= 15 && iChaLevel <= 60)
			{
				iItemLevel = 0;
			}
			else if (iChaLevel >= 61 && iChaLevel <= 110)
			{
				iItemLevel = 8;
			}
			else if (iChaLevel >= 111 && iChaLevel <= 160)
			{
				iItemLevel = 16;
			}
			else if (iChaLevel >= 161 && iChaLevel <= 210)
			{
				iItemLevel = 24;
			}
			else if (iChaLevel >= 211 && iChaLevel <= 260)
			{
				iItemLevel = 32;
			}
			else if (iChaLevel >= 261 && iChaLevel <= 310)
			{
				iItemLevel = 40;
			}
			else if (iChaLevel >= 311 && iChaLevel <= 400)
			{
				iItemLevel = 48;
			}
		}
		else
		{
			if (iChaLevel >= 15 && iChaLevel <= 80)
			{
				iItemLevel = 0;
			}
			else if (iChaLevel >= 81 && iChaLevel <= 130)
			{
				iItemLevel = 8;
			}
			else if (iChaLevel >= 131 && iChaLevel <= 180)
			{
				iItemLevel = 16;
			}
			else if (iChaLevel >= 181 && iChaLevel <= 230)
			{
				iItemLevel = 24;
			}
			else if (iChaLevel >= 231 && iChaLevel <= 280)
			{
				iItemLevel = 32;
			}
			else if (iChaLevel >= 281 && iChaLevel <= 330)
			{
				iItemLevel = 40;
			}
			else if (iChaLevel >= 331 && iChaLevel <= 400)
			{
				iItemLevel = 48;
			}
		}
		return (iItemLevel >> 3) & 15;
	}
	else if (iType == FREETICKET_TYPE_CURSEDTEMPLE)
	{
		if (g_pCursedTempleEnterWindow->CheckEnterLevel(iItemLevel))
		{
			return (iItemLevel >> 3) & 15;
		}
	}
	else if (iType == FREETICKED_TYPE_CHAOSCASTLE)
	{
		if (g_pCursedTempleEnterWindow->CheckEnterLevel(iItemLevel))
		{
			return (iItemLevel >> 3) & 15;
		}
	}
	return 0;
}

void SEASON3B::RenderGoldRect(float fPos_x, float fPos_y, float fWidth, float fHeight, int iFillType)
{
	if (iFillType == 1)
	{
		EnableAlphaTest();
		glColor4ub(146, 144, 141, 200);
		RenderColor(fPos_x, fPos_y, fWidth, fHeight);
		EndRenderColor();
	}
	else if (iFillType == 2)
	{
		EnableAlphaTest();
		glColor4f(0.0, 0.0, 0.0, 0.40000001);
		RenderColor(fPos_x, fPos_y, fWidth, fHeight);
		EndRenderColor();
	}
	else if (iFillType == 3)
	{
		EnableAlphaTest();
		glColor4f(0.0, 0.0, 0.0, 0.60000001);
		RenderColor(fPos_x, fPos_y, fWidth, fHeight);
		EndRenderColor();
	}
	else if (iFillType == 4)
	{
		EnableAlphaTest();
		glColor4f(0.0, 0.0, 0.0, 0.80000001);
		RenderColor(fPos_x, fPos_y, fWidth, fHeight);
		EndRenderColor();
	}

	RenderBitmap(BITMAP_INVENTORY + 19, fPos_x, fPos_y, fWidth, 2, 10 / 256.f, 5 / 16.f, 170.f / 256.f, 2.f / 16.f);

	RenderBitmap(BITMAP_INVENTORY + 19, fPos_x, fPos_y + fHeight, fWidth + 1, 2, 10 / 256.f, 5 / 16.f, 170.f / 256.f, 2.f / 16.f);

	RenderBitmap(BITMAP_INVENTORY, fPos_x, fPos_y, 2, fHeight, 1.f / 256.f, 5 / 16.f, 2.f / 256.f, 125.f / 256.f);

	RenderBitmap(BITMAP_INVENTORY, fPos_x + fWidth, fPos_y, 2, fHeight, 1.f / 256.f, 5 / 16.f, 2.f / 256.f, 125.f / 256.f);
}

void SEASON3B::RenderInventoryInterface(int StartX, int StartY, int Flag)
{
	float x, y, Width, Height;
	Width = 190.f; Height = 256.f; x = (float)StartX; y = (float)StartY;

	RenderBitmap(BITMAP_INVENTORY, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 256.f);

	Width = 190.f; Height = 177.f; x = (float)StartX; y = (float)StartY + 256;
	RenderBitmap(BITMAP_INVENTORY + 1, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 256.f);

	if (Flag)
	{
		Width = 190.f; Height = 10.f; x = (float)StartX; y = (float)StartY + Flag;
		RenderBitmap(BITMAP_INVENTORY + 19, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 16.f);
	}
}

void SEASON3B::SetButtonImagen(CNewUIButton* button, int imgindex1, int imgindex2, float su, float sv, float uw, float vh)
{
	button->UnRegisterButtonState();
	button->RegisterButtonState(BUTTON_STATE_UP, imgindex1, 0, su, sv, uw, vh);
	button->RegisterButtonState(BUTTON_STATE_DOWN, imgindex2, 0, su, sv, uw, vh);
	button->ChangeFrame(BUTTON_STATE_UP);

}

void SEASON3B::SetButtonPosition(CNewUIButton* button, int StartX, int StartY, int type)
{
	if (gmProtect->checkold_school())
	{
		if (type == 1) //-- close
		{
			button->SetPos(StartX + 20.f, StartY + 395.f);
		}
		else if (type == 2) //-- pshop open
		{
			button->SetPos(StartX + 76.f, StartY + 395.f);
		}
		else if (type == 3) //-- repair
		{
			button->SetPos(StartX + 48.f, StartY + 395.f);
		}
		else if (type == 4) //-- jewel
		{
			button->SetPos(StartX + 104.f, StartY + 395.f);
		}
		else if (type == 5) //-- quest
		{
			button->SetPos(StartX + 48.f, StartY + 395.f);
		}
		else if (type == 6) //-- pet
		{
			button->SetPos(StartX + 76.f, StartY + 395.f);
		}
		else if (type == 7) //-- money insert
		{
			button->SetPos(StartX + 26.f, StartY + 395.f);
		}
		else if (type == 8) //-- money take
		{
			button->SetPos(StartX + 54.f, StartY + 395.f);
		}
		else if (type == 9) //-- unlock
		{
			button->SetPos(StartX + 82.f, StartY + 395.f);
		}
		else if (type == 10) //-- lock
		{
			button->SetPos(StartX + 82.f, StartY + 395.f);
		}
		else if (type == 12) //-- pshop closed
		{
			button->SetPos(StartX + 76.f, StartY + 395.f);
		}
	}
}

void SEASON3B::SetButtonInterface(CNewUIButton* button, int StartX, int StartY, int type, bool move)
{
	if (gmProtect->checkold_school())
	{
		if (type == 1) //-- close
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON, BITMAP_INVENTORY_BUTTON + 1);
			button->SetSize(24, 24);
			if (move) button->SetPos(StartX + 20.f, StartY + 395.f);
		}
		else if (type == 2) //-- pshop open
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 2, BITMAP_INVENTORY_BUTTON + 3);
			button->SetSize(24, 24);
			button->SetPos(StartX + 76.f, StartY + 395.f);
		}
		else if (type == 3) //-- repair
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 4, BITMAP_INVENTORY_BUTTON + 5);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 48.f, StartY + 395.f);
		}
		else if (type == 4) //-- jewel
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 6, BITMAP_INVENTORY_BUTTON + 7);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 104.f, StartY + 395.f);
		}
		else if (type == 5) //-- quest
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 8, BITMAP_INVENTORY_BUTTON + 9);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 48.f, StartY + 395.f);
		}
		else if (type == 6) //-- pet
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 18, BITMAP_INVENTORY_BUTTON + 19);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 76.f, StartY + 395.f);
		}
		else if (type == 7) //-- money insert
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 21, BITMAP_INVENTORY_BUTTON + 22);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 26.f, StartY + 395.f);
		}
		else if (type == 8) //-- money take
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 21, BITMAP_INVENTORY_BUTTON + 22);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 54.f, StartY + 395.f);
		}
		else if (type == 9) //-- unlock
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 14, BITMAP_INVENTORY_BUTTON + 17);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 82.f, StartY + 395.f);
		}
		else if (type == 10) //-- lock
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 16, BITMAP_INVENTORY_BUTTON + 15);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 82.f, StartY + 395.f);
		}
		else if (type == 12) //-- pshop closed
		{
			SetButtonImagen(button, BITMAP_INVENTORY_BUTTON + 3, BITMAP_INVENTORY_BUTTON + 2);
			button->SetSize(24, 24);

			if (move) button->SetPos(StartX + 76.f, StartY + 395.f);
		}
	}
}
