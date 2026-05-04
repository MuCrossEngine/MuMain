#include "stdafx.h"
#include "jpexs.h"
#include "UIControls.h"
#include "MapManager.h"
#include "NewUISystem.h"
#include "ZzzInterface.h"
#include "MultiLanguage.h"
#include "UIGuildInfo.h"
#include "GMBattleCastle.h"
#include "PersonalShopTitleImp.h"
#include "CGMHeadChat.h"
#include "CGMProtect.h"
#include "ZzzLodTerrain.h"

#include "CGMFontLayer.h"

template <size_t N>
static void CopyChatBuffer(char (&destination)[N], const char* source)
{
	if (source == NULL)
	{
		destination[0] = '\0';
		return;
	}

	strncpy(destination, source, N - 1);
	destination[N - 1] = '\0';
}

CGMHeadChat::CGMHeadChat() :Chat(MAX_CHAT)
{
}

CGMHeadChat::~CGMHeadChat()
{
}

CGMHeadChat* CGMHeadChat::Instance()
{
	static CGMHeadChat s_Instance;
	return &s_Instance;
}

void CGMHeadChat::AddGuildName(CHAT* c, CHARACTER* Owner)
{
	if (IsShopInViewport(Owner))
	{
		std::string summary;
		GetShopTitleSummary(Owner, summary);
		strncpy(c->szShopTitle, summary.c_str(), sizeof(c->szShopTitle) - 1);
		c->szShopTitle[sizeof(c->szShopTitle) - 1] = '\0';
	}
	else
	{
		c->szShopTitle[0] = '\0';
	}

	CHARACTER* pCharacter = Owner;

	if (gmProtect->m_RenderCharacterName && pCharacter->GetKind() == KIND_PLAYER)
	{
		if (pCharacter->GuildMarkIndex >= 0 && GuildMark[pCharacter->GuildMarkIndex].UnionName[0])
		{
			snprintf(c->Union, sizeof(c->Union), "%s>", GuildMark[pCharacter->GuildMarkIndex].UnionName);
		}
		else
		{
			c->Union[0] = NULL;
		}

		if (pCharacter->GuildMarkIndex >= 0)
		{
			c->GuildColor = 0;

			if (c->Union[0] != NULL)
				snprintf(c->Guild, sizeof(c->Guild), "<%s - %s", GuildMark[Owner->GuildMarkIndex].GuildName, c->Union);
			else
				snprintf(c->Guild, sizeof(c->Guild), "<%s>", GuildMark[Owner->GuildMarkIndex].GuildName);
		}
		else
		{
			c->GuildColor = 0;
			c->Guild[0] = NULL;
		}
	}
	else
	{
		if (Owner->GuildMarkIndex >= 0 && GuildMark[Owner->GuildMarkIndex].UnionName[0])
		{
			if (Owner->GuildRelationShip == GR_UNION)
				snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1295]);
			if (Owner->GuildRelationShip == GR_UNIONMASTER)
			{
				if (Owner->GuildStatus == G_MASTER)
					snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1296]);
				else
					snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1295]);
			}
			else if (Owner->GuildRelationShip == GR_RIVAL)
			{
				if (Owner->GuildStatus == G_MASTER)
					snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1298]);
				else
					snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1297]);
			}
			else if (Owner->GuildRelationShip == GR_RIVALUNION)
				snprintf(c->Union, sizeof(c->Union), "<%s> %s", GuildMark[Owner->GuildMarkIndex].UnionName, GlobalText[1299]);
			else
				snprintf(c->Union, sizeof(c->Union), "<%s>", GuildMark[Owner->GuildMarkIndex].UnionName);
		}
		else
		{
			c->Union[0] = NULL;
		}

		if (Owner->GuildMarkIndex >= 0)
		{
			c->GuildColor = Owner->GuildTeam;

			if (Owner->GuildStatus == G_PERSON)
				snprintf(c->Guild, sizeof(c->Guild), "[%s] %s", GuildMark[Owner->GuildMarkIndex].GuildName, GlobalText[1330]);
			else if (Owner->GuildStatus == G_MASTER)
				snprintf(c->Guild, sizeof(c->Guild), "[%s] %s", GuildMark[Owner->GuildMarkIndex].GuildName, GlobalText[1300]);
			else if (Owner->GuildStatus == G_SUB_MASTER)
				snprintf(c->Guild, sizeof(c->Guild), "[%s] %s", GuildMark[Owner->GuildMarkIndex].GuildName, GlobalText[1301]);
			else if (Owner->GuildStatus == G_BATTLE_MASTER)
				snprintf(c->Guild, sizeof(c->Guild), "[%s] %s", GuildMark[Owner->GuildMarkIndex].GuildName, GlobalText[1302]);
			else
				snprintf(c->Guild, sizeof(c->Guild), "[%s]", GuildMark[Owner->GuildMarkIndex].GuildName);
		}
		else
		{
			c->GuildColor = 0;
			c->Guild[0] = NULL;
		}
	}
}

void CGMHeadChat::AddChat(CHAT* c, const char* Text, int Flag)
{
	int Time = 0;
	int Length = (int)strlen(Text);
	switch (Flag)
	{
	case 0:
		Time = Length * 2 + 160;
		break;
	case 1:
		Time = 1000;
		g_pChatListBox->AddText(c->ID, Text, SEASON3B::TYPE_CHAT_MESSAGE);
		break;
	}

#ifdef PLAYER_INFO_SYSTEM
	std::string ChatText;

	if (Length >= (20 * g_pMultiLanguage->GetNumByteForOneCharUTF8()))
	{
		CutText(Text, c->Text[0], c->Text[1], Length);
		c->LifeTime[0] = Time;
		c->LifeTime[1] = Time;

		ChatText = SEASON3B::FontHTML(Text, 0, true, false);
	}
	else
	{
		CopyChatBuffer(c->Text[0], Text);
		c->LifeTime[0] = Time;

		if (c->LifeTime[1] > 0)
		{
			ChatText = "<font face='$NormalFont' size='12' color='#ffffff'><b>";
			ChatText += c->Text[1];
			ChatText += "</b><br><b>";
			ChatText += c->Text[0];
			ChatText += "</b></font>";
		}
		else
		{
			ChatText = SEASON3B::FontHTML(c->Text[0], 0, true, false);
		}
	}

	CHARACTER* pCharacter = c->Owner;

	if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER) && !ChatText.empty())
	{
		c->RemoveUIChat = true;
		pCharacter->AddUIChat(ChatText.c_str());
	}
#else
	if (Length >= (20 * g_pMultiLanguage->GetNumByteForOneCharUTF8()))
	{
		CutText(Text, c->Text[0], c->Text[1], Length);
		c->LifeTime[0] = Time;
		c->LifeTime[1] = Time;
	}
	else
	{
		CopyChatBuffer(c->Text[0], Text);
		c->LifeTime[0] = Time;
	}

#endif // PLAYER_INFO_SYSTEM
}

void CGMHeadChat::AssignChat(char* ID, const char* Text, int Flag)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(i);
		OBJECT* o = &pCharacter->Object;
		if (o->Live && (o->Kind == KIND_PLAYER || o->Kind == KIND_MONSTER))
		{
			if (strcmp(pCharacter->ID, ID) == NULL)
			{
				CreateChat(ID, Text, pCharacter, Flag);
				return;
			}
		}
	}
}

void CGMHeadChat::CreateChat(char* ID, const char* Text, CHARACTER* Owner, int Flag, int SetColor)
{
	OBJECT* o = &Owner->Object;

	if (!o->Live || !o->Visible)
		return;

	int Color;
	if (SetColor != -1)
	{
		Color = SetColor;
	}
	else
	{
		Color = Owner->PK;
		if (o->Kind == KIND_NPC)
		{
			Color = 0;
		}
	}

	CHARACTER* pCharacter = Owner;

	for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* c = &Chat[i];

		if (c->Owner == Owner)
		{
			CopyChatBuffer(c->ID, ID);
			c->Color = Color;

			AddGuildName(c, Owner);

#ifdef PLAYER_INFO_SYSTEM
			if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
			{
				pCharacter->AddUIInfo();

				pCharacter->SetIDColor();

				pCharacter->SetGensMark();

				pCharacter->SetGuildName();
			}
#endif // PLAYER_INFO_SYSTEM

			if (strlen(Text) == 0)
			{
				c->IDLifeTime = 10;
			}
			else
			{
				if (c->LifeTime[0] > 0)
				{
					CopyChatBuffer(c->Text[1], c->Text[0]);
					c->LifeTime[1] = c->LifeTime[0];
				}

				c->Owner = Owner;

				AddChat(c, Text, Flag);
			}
			return;
		}
	}

	for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* c = &Chat[i];

		if (c->IDLifeTime <= 0 && c->LifeTime[0] <= 0)
		{
			c->Owner = Owner;
			CopyChatBuffer(c->ID, ID);
			c->Color = Color;
			AddGuildName(c, Owner);

#ifdef PLAYER_INFO_SYSTEM
			if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
			{
				pCharacter->AddUIInfo();

				pCharacter->SetIDColor();

				pCharacter->SetGensMark();

				pCharacter->SetGuildName();
			}
#endif // PLAYER_INFO_SYSTEM

			if (strlen(Text) == 0)
			{
				c->IDLifeTime = 100;
			}
			else
			{
				AddChat(c, Text, Flag);
			}
			return;
		}
	}
}

void CGMHeadChat::SetBooleanPosition(CHAT* c)
{
	CHARACTER* pCharacter = c->Owner;

#ifdef PLAYER_INFO_SYSTEM
	if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
	{
		pCharacter->UpdatePosition(-1);
	}
	else
	{
		BOOL bResult[5];
		SIZE Size[5];
		memset(&Size[0], 0, sizeof(SIZE) * 5);

		if (g_isCharacterBuff((&pCharacter->Object), eBuff_GMEffect) || (pCharacter->CtlCode == CTLCODE_20OPERATOR) || (pCharacter->CtlCode == CTLCODE_08OPERATOR))
		{
			g_pRenderText->SetFont(g_hFontBold);
			bResult[0] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->ID, lstrlen(c->ID), &Size[0]);
			g_pRenderText->SetFont(g_hFont);
		}
		else
		{
			bResult[0] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->ID, lstrlen(c->ID), &Size[0]);
		}

		bResult[1] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Text[0], lstrlen(c->Text[0]), &Size[1]);
		bResult[2] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Text[1], lstrlen(c->Text[1]), &Size[2]);
		bResult[3] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Union, lstrlen(c->Union), &Size[3]);
		bResult[4] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Guild, lstrlen(c->Guild), &Size[4]);

		Size[0].cx += 3;

		if (c->LifeTime[1] > 0)
			c->Width = max(max(max(Size[0].cx, Size[1].cx), max(Size[2].cx, Size[3].cx)), Size[4].cx);

		else if (c->LifeTime[0] > 0)
			c->Width = max(max(Size[0].cx, Size[1].cx), max(Size[3].cx, Size[4].cx));
		else
			c->Width = max(max(Size[0].cx, Size[3].cx), Size[4].cx);
		c->Height = FontHeight * (bResult[0] + bResult[1] + bResult[2] + bResult[3] + bResult[4]);

		if (lstrlen(c->szShopTitle) > 0)
		{
			SIZE sizeT[2];
			g_pRenderText->SetFont(g_hFontBold);

			if (g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->szShopTitle, lstrlen(c->szShopTitle), &sizeT[0]) && g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), GlobalText[1104], lstrlen(GlobalText[1104]), &sizeT[1]))
			{
				if (c->Width < sizeT[0].cx + sizeT[1].cx)
					c->Width = sizeT[0].cx + sizeT[1].cx;
				c->Height += max(sizeT[0].cy, sizeT[1].cy);
			}
			g_pRenderText->SetFont(g_hFont);
		}
	}
#else
	BOOL bResult[5];
	SIZE Size[5];
	memset(&Size[0], 0, sizeof(SIZE) * 5);

	if (g_isCharacterBuff((&pCharacter->Object), eBuff_GMEffect) || (pCharacter->CtlCode == CTLCODE_20OPERATOR) || (pCharacter->CtlCode == CTLCODE_08OPERATOR))
	{
		g_pRenderText->SetFont(g_hFontBold);
		bResult[0] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->ID, lstrlen(c->ID), &Size[0]);
		g_pRenderText->SetFont(g_hFont);
	}
	else
	{
		bResult[0] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->ID, lstrlen(c->ID), &Size[0]);
	}

	if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
	{
		bResult[1] = GMFontLayer->_GetTextExtentPoint32(c->Text[0], lstrlen(c->Text[0]), &Size[1]);
		bResult[2] = GMFontLayer->_GetTextExtentPoint32(c->Text[1], lstrlen(c->Text[1]), &Size[2]);
		bResult[3] = GMFontLayer->_GetTextExtentPoint32(c->Union, lstrlen(c->Union), &Size[3]);
		bResult[4] = GMFontLayer->_GetTextExtentPoint32(c->Guild, lstrlen(c->Guild), &Size[4]);
	}
	else
	{
		bResult[1] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Text[0], lstrlen(c->Text[0]), &Size[1]);
		bResult[2] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Text[1], lstrlen(c->Text[1]), &Size[2]);
		bResult[3] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Union, lstrlen(c->Union), &Size[3]);
		bResult[4] = g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->Guild, lstrlen(c->Guild), &Size[4]);
	}

	Size[0].cx += 3;

	if (c->LifeTime[1] > 0)
		c->Width = max(max(max(Size[0].cx, Size[1].cx), max(Size[2].cx, Size[3].cx)), Size[4].cx);
	else if (c->LifeTime[0] > 0)
		c->Width = max(max(Size[0].cx, Size[1].cx), max(Size[3].cx, Size[4].cx));
	else
		c->Width = max(max(Size[0].cx, Size[3].cx), Size[4].cx);

	c->Height = FontHeight * (bResult[0] + bResult[1] + bResult[2] + bResult[3] + bResult[4]);

	if (lstrlen(c->szShopTitle) > 0)
	{
		SIZE sizeT[2];
		g_pRenderText->SetFont(g_hFontBold);

		if (g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), c->szShopTitle, lstrlen(c->szShopTitle), &sizeT[0]) 
			&& g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), GlobalText[1104], lstrlen(GlobalText[1104]), &sizeT[1]))
		{
			if (c->Width < sizeT[0].cx + sizeT[1].cx)
				c->Width = sizeT[0].cx + sizeT[1].cx;
			c->Height += max(sizeT[0].cy, sizeT[1].cy);
		}
		g_pRenderText->SetFont(g_hFont);
	}
#endif // PLAYER_INFO_SYSTEM
}


void CGMHeadChat::RenderBooleans()
{
	g_pRenderText->SetFont(g_hFont);

	for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* ci = &Chat[i];

		if (ci->IDLifeTime > 0 || ci->LifeTime[0] > 0)
		{
			vec3_t Position;
			int ScreenX, ScreenY;

			CHARACTER* pCharacter = ci->Owner;

			float add_boundin = 0.0;

			if (ci->Owner != NULL)
			{
				OBJECT* o = &ci->Owner->Object;

				if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
				{
					if (o->Type >= MODEL_LITTLESANTA && o->Type <= MODEL_LITTLESANTA_END || o->Type == MODEL_MERCHANT_MAN)
					{
						add_boundin += 50.f;
					}
					else
					{
						add_boundin += 90.f;
					}
				}
				else
				{
					if (o->Type >= MODEL_LITTLESANTA && o->Type <= MODEL_LITTLESANTA_END || o->Type == MODEL_MERCHANT_MAN)
					{
						add_boundin += 20.f;
					}
					else
					{
						add_boundin += 40.f;
				}
				}

				Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + add_boundin, Position);

				Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + o->BoundingBoxMax[2] + add_boundin;

				Projection2(Position, &ScreenX, &ScreenY);
			}
			else
			{
				Projection2(ci->Position, &ScreenX, &ScreenY);
			}
			SetBooleanPosition(ci);

			ci->x = ScreenX - (ci->Width * 0.5f);
			ci->y = ScreenY - ci->Height;
		}
	}

	/*for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* ci = &Chat[i];
		if (ci->IDLifeTime > 0 || ci->LifeTime[0] > 0)
		{
			for (int j = 0; j < MAX_CHAT; j++)
			{
				CHAT* cj = &Chat[j];
				if (i != j && (cj->IDLifeTime > 0 || cj->LifeTime[0] > 0))
				{
					if (ci->x + ci->Width > cj->x && ci->x<cj->x + cj->Width &&
						ci->y + ci->Height>cj->y && ci->y < cj->y + cj->Height)
					{
						if (ci->y < cj->y + cj->Height / 2)
							ci->y = cj->y - ci->Height;
						else
							ci->y = cj->y + cj->Height;
					}
				}
			}
		}
	}*/

	for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* ci = &Chat[i];
		if (ci->IDLifeTime > 0 || ci->LifeTime[0] > 0)
		{
			CHARACTER* pCharacter = ci->Owner;

			if (gmProtect->m_RenderCharacterName == 0 || (gmProtect->m_RenderCharacterName && !(pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER)))
			{
				//. Fit to screen
				if (ci->x < 0)
					ci->x = 0;
				if (ci->x >= (int)WindowWidth - ci->Width)
					ci->x = WindowWidth - ci->Width;
				if (ci->y < 0)
					ci->y = 0;
				if (ci->y >= (int)WindowHeight - ci->Height)
					ci->y = WindowHeight - ci->Height;
			}

			RenderBoolean(ci->x, ci->y, ci);
		}
	}
}

void RenderGensMark(BYTE _GensInfluence, BYTE _GensRankInfo, int _ScreenX, int _ScreenY)
{
	if (_GensInfluence == GENS_TYPE_NONE || _GensRankInfo <= 0 || _GensRankInfo > 14)
	{
		return;
	}
	float u, v, uw, vh;

	int _ImgIndex = (14 - _GensRankInfo);

	if (_GensInfluence == GENS_TYPE_BARNERT)
	{
		switch (_ImgIndex)
		{
		case 0:
			u = 312, v = 129, uw = 362, vh = 198;
			break;
		case 1:
			u = 468, v = 129, uw = 518, vh = 198;
			break;
		case 2:
			u = 416, v = 129, uw = 466, vh = 198;
			break;
		case 3:
			u = 364, v = 129, uw = 414, vh = 198;
			break;
		case 4:
			u = 208, v = 129, uw = 258, vh = 198;
			break;
		case 5:
			u = 333, v = 0, uw = 383, vh = 69;
			break;
		case 6:
			u = 645, v = 0, uw = 695, vh = 69;
			break;
		case 7:
			u = 260, v = 129, uw = 310, vh = 198;
			break;
		case 8:
			u = 801, v = 0, uw = 851, vh = 69;
			break;
		case 9:
			u = 853, v = 0, uw = 903, vh = 69;
			break;
		case 10:
			u = 905, v = 0, uw = 955, vh = 69;
			break;
		case 11:
			u = 697, v = 0, uw = 747, vh = 69;
			break;
		case 12:
			u = 957, v = 0, uw = 1007, vh = 69;
			break;
		case 13:
			u = 52, v = 129, uw = 102, vh = 198;
			break;
		default:
			u = 312, v = 129, uw = 362, vh = 198;
			break;
		}
	}
	else
	{
		switch (_ImgIndex)
		{
		case 0:
			u = 104, v = 129, uw = 154, vh = 198;
			break;
		case 1:
			u = 156, v = 129, uw = 206, vh = 198;
			break;
		case 2:
			u = 749, v = 0, uw = 799, vh = 69;
			break;
		case 3:
			u = 0, v = 129, uw = 50, vh = 198;
			break;
		case 4:
			u = 593, v = 0, uw = 643, vh = 69;
			break;
		case 5:
			u = 385, v = 0, uw = 435, vh = 69;
			break;
		case 6:
			u = 541, v = 0, uw = 591, vh = 69;
			break;
		case 7:
			u = 177, v = 0, uw = 227, vh = 69;
			break;
		case 8:
			u = 229, v = 0, uw = 279, vh = 69;
			break;
		case 9:
			u = 73, v = 0, uw = 123, vh = 69;
			break;
		case 10:
			u = 125, v = 0, uw = 175, vh = 69;
			break;
		case 11:
			u = 281, v = 0, uw = 331, vh = 69;
			break;
		case 12:
			u = 437, v = 0, uw = 487, vh = 69;
			break;
		case 13:
			u = 489, v = 0, uw = 539, vh = 69;
			break;
		default:
			u = 104, v = 129, uw = 154, vh = 198;
			break;
		}
	}
	jpexs::Renderjpexs_dds(IMAGE_HUD_PLAYER, scale_jpexs(_ScreenX, -2100), scale_jpexs(_ScreenY, -1060), 51, 74, u, v, uw, vh);
}

void CGMHeadChat::RenderBoolean(int x, int y, CHAT* c)
{
	CHARACTER* pCharacter = c->Owner;

	if (g_isCharacterBuff((&pCharacter->Object), eBuff_CrywolfNPCHide))
	{
		return;
	}

	if (pCharacter != Hero && battleCastle::IsBattleCastleStart() == true && g_isCharacterBuff((&pCharacter->Object), eBuff_Cloaking))
	{
		if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK))
		{
			if (!(pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK || pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK))
			{
				return;
			}
		}
		else if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2 || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK2))
		{
			if (!(pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2 || pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK2))
			{
				return;
			}
		}
		else if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3 || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK3))
		{
			if (!(pCharacter->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3 || pCharacter->EtcPart == PARTS_ATTACK_TEAM_MARK3))
			{
				return;
			}
		}
		else if ((Hero->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || Hero->EtcPart == PARTS_DEFENSE_TEAM_MARK))
		{
			if (!(pCharacter->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || pCharacter->EtcPart == PARTS_DEFENSE_TEAM_MARK))
			{
				return;
			}
		}
	}

	EnableAlphaTest();
	glColor3f(1.f, 1.f, 1.f);

	if (FontHeight > 32)
		FontHeight = 32;

	POINT RenderPos = { x, y };
	SIZE RenderBoxSize = { c->Width, c->Height };
	int iLineHeight = FontHeight;

	if (gmProtect->m_RenderCharacterName && (pCharacter->GetKind() == KIND_PLAYER || pCharacter->GetKind() == KIND_MONSTER))
	{
		bool bGmMode = false;

		iLineHeight = GMFontLayer->getmetrics();

		if (::IsStrifeMap(gMapManager->currentMap) && Hero->m_byGensInfluence != pCharacter->m_byGensInfluence && !::IsGMCharacter())
		{
			if (!pCharacter->m_nContributionPoint)
			{
				return;
			}

			int _TempX = (int)(c->x + c->Width * 0.5f + 20.0f);

			if (pCharacter->m_byGensInfluence == 1 || pCharacter->m_byGensInfluence == 2)
			{
				g_pNewUIGensRanking->RanderMark(_TempX / g_fScreenRate_x, y / g_fScreenRate_y, pCharacter->m_byGensInfluence, pCharacter->m_byRankIndex, SEASON3B::CNewUIGensRanking::MARK_BOOLEAN, (float)RenderPos.y / g_fScreenRate_y);
			}
		}

		if (g_isCharacterBuff((&pCharacter->Object), eBuff_GMEffect) || (pCharacter->CtlCode == CTLCODE_20OPERATOR) || (pCharacter->CtlCode == CTLCODE_08OPERATOR))
		{
			bGmMode = true;
		}

		RenderPos.x += (RenderBoxSize.cx * 0.5);
		RenderPos.y -= iLineHeight;

		if (pCharacter->GetKind() == KIND_PLAYER && MODEL_PLAYER == pCharacter->Object.Type)
		{
			if (gmProtect->RenderGuildLogo == 1)
			{
				if (pCharacter->GuildStatus != G_NONE && pCharacter->GuildMarkIndex != -1)
				{
					CreateGuildMark(pCharacter->GuildMarkIndex, true);
					RenderBitmap(BITMAP_GUILD, (float)RenderPos.x + 60.f, (float)(RenderPos.y - iLineHeight) + 5, 32.f, 32.f, 0.0, 0.0, 1.f, 1.f, false, false);
				}
			}

			if (pCharacter->m_byGensInfluence != GENS_TYPE_NONE)
			{
				RenderGensMark(pCharacter->m_byGensInfluence, pCharacter->m_byRankIndex, RenderPos.x, (RenderPos.y + iLineHeight));
			}
		}

		g_pRenderText->SetTextColor(pCharacter->GetColorID());

		if (GMProtect->IsRenderWave(c->ID))
		{
			GMFontLayer->RenderWave(RenderPos.x, RenderPos.y, c->ID, 0, iLineHeight, RT3_WRITE_CENTER, NULL);
		}
		else
		{
			GMFontLayer->RenderText(RenderPos.x, RenderPos.y, c->ID, 0, iLineHeight, RT3_WRITE_CENTER);
		}
		RenderPos.y -= iLineHeight;

		if (c->Guild && c->Guild[0])
		{
			g_pRenderText->SetTextColor(pCharacter->GetRelationShipTextColor());
			GMFontLayer->RenderText(RenderPos.x, RenderPos.y, c->Guild, 0, iLineHeight, RT3_WRITE_CENTER);
		}

		RenderPos.y -= iLineHeight;

#if MAIN_UPDATE <= 303
		if (pCharacter->GetKind() == KIND_PLAYER)
		{
			int iSelectedCharacter = gmCharacters->GetCharacterIndex(pCharacter);

			if (SelectedCharacter != -1 && iSelectedCharacter == SelectedCharacter)
			{
				g_pBuffWindow->RenderBuffStatus(RenderPos.x / g_fScreenRate_x, RenderPos.y / g_fScreenRate_y, &pCharacter->Object);
			}
		}
#endif

		DWORD dwTextColor[2];
		BYTE byAlpha[2] = { 255, 255 };

		if (c->LifeTime[0] > 0 && c->LifeTime[0] < 10)
		{
			byAlpha[0] = 128;
		}
		if (c->LifeTime[1] > 0 && c->LifeTime[1] < 10)
		{
			byAlpha[1] = 128;
		}

		if (bGmMode)
		{
			dwTextColor[0] = RGBA(250, 200, 50, byAlpha[0]);
			dwTextColor[1] = RGBA(250, 200, 50, byAlpha[1]);
		}
		else
		{
			dwTextColor[0] = RGBA(230, 220, 200, byAlpha[0]);
			dwTextColor[1] = RGBA(230, 220, 200, byAlpha[1]);
		}

		if (c->LifeTime[1] > 0 || c->LifeTime[0] > 0)
		{
			float RenderPosY = RenderPos.y;
			float RenderPosX = RenderPos.x;
			float RenderSizeY = (iLineHeight * 2);
			float RenderSizeX = (RenderBoxSize.cx + 16.f);

			if (c->LifeTime[1] > 0)
			{
				RenderSizeY += iLineHeight;
			}
			RenderPosX -= (RenderSizeX / 2.f);
			RenderPosY -= (RenderSizeY / 2.f);

			g_pNameWindow->RenderTooltip(RenderPosX, RenderPosY, RenderSizeX, RenderSizeY);
		}

		if (c->LifeTime[1] > 0)
		{
			g_pRenderText->SetTextColor(dwTextColor[0]);
			GMFontLayer->RenderText(RenderPos.x, RenderPos.y, c->Text[0], 0, iLineHeight, RT3_WRITE_CENTER);

			RenderPos.y -= iLineHeight;
			g_pRenderText->SetTextColor(dwTextColor[1]);
			GMFontLayer->RenderText(RenderPos.x, RenderPos.y, c->Text[1], 0, iLineHeight, RT3_WRITE_CENTER);
		}
		else if (c->LifeTime[0] > 0)
		{
			RenderPos.y -= (iLineHeight / 2.f);
			g_pRenderText->SetTextColor(dwTextColor[0]);
			GMFontLayer->RenderText(RenderPos.x, RenderPos.y, c->Text[0], 0, iLineHeight, RT3_WRITE_CENTER);
		}
	}
	else
	{
		if (IsShopInViewport(pCharacter))
		{
			SIZE TextSize;
			g_pRenderText->SetFont(g_hFontBold);
			g_pRenderText->SetBgColor(GetShopBGColor(pCharacter));

			g_pRenderText->SetTextColor(GetShopTextColor(pCharacter));
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, GlobalText[1104], 0, iLineHeight, RT3_SORT_LEFT, &TextSize);
			RenderPos.x += TextSize.cx;

			g_pRenderText->SetTextColor(GetShopText2Color(pCharacter));
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->szShopTitle, RenderBoxSize.cx - TextSize.cx, iLineHeight, RT3_SORT_LEFT, &TextSize);
			g_pRenderText->SetFont(g_hFont);

			RenderPos.x = x;
			RenderPos.y += iLineHeight;
		}
		else if (::IsStrifeMap(gMapManager->currentMap) && Hero->m_byGensInfluence != pCharacter->m_byGensInfluence && !::IsGMCharacter())
		{
			if (!pCharacter->m_nContributionPoint)
				return;

			if (KIND_PLAYER == pCharacter->GetKind() && MODEL_PLAYER == pCharacter->Object.Type)
			{
				int _TempX = (int)(c->x + c->Width * 0.5f + 20.0f);

				if (pCharacter->m_byGensInfluence == 1 || pCharacter->m_byGensInfluence == 2)
				{
					g_pNewUIGensRanking->RanderMark(_TempX / g_fScreenRate_x, y / g_fScreenRate_y, pCharacter->m_byGensInfluence, pCharacter->m_byRankIndex, SEASON3B::CNewUIGensRanking::MARK_BOOLEAN, (float)RenderPos.y / g_fScreenRate_y);
				}
			}
		}

		bool bGmMode = false;

		if (g_isCharacterBuff((&pCharacter->Object), eBuff_GMEffect) || (pCharacter->CtlCode == CTLCODE_20OPERATOR) || (pCharacter->CtlCode == CTLCODE_08OPERATOR))
		{
			bGmMode = true;
			g_pRenderText->SetBgColor(30, 30, 30, 200);
			g_pRenderText->SetTextColor(200, 255, 255, 255);
		}

		if (pCharacter == Hero)
		{
			g_pRenderText->SetBgColor(60, 100, 0, 150);
			g_pRenderText->SetTextColor(200, 255, 0, 255);
		}
		else if (pCharacter->GuildMarkIndex == Hero->GuildMarkIndex)
		{
			g_pRenderText->SetBgColor(GetGuildRelationShipBGColor(GR_UNION));
			g_pRenderText->SetTextColor(GetGuildRelationShipTextColor(GR_UNION));
		}
		else
		{
			g_pRenderText->SetBgColor(GetGuildRelationShipBGColor(pCharacter->GuildRelationShip));
			g_pRenderText->SetTextColor(GetGuildRelationShipTextColor(pCharacter->GuildRelationShip));
		}

		if (c->Union && c->Union[0])
		{
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->Union, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT);
			RenderPos.y += iLineHeight;
		}
		if (c->Guild && c->Guild[0])
		{
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->Guild, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT);
			RenderPos.y += iLineHeight;
		}

		if (bGmMode)
		{
			g_pRenderText->SetTextColor(100, 250, 250, 255);
		}
		else
		{
			SetPlayerColor(c->Color);
		}

		if (c->x <= MouseX && MouseX < (int)(c->x + c->Width * gwinhandle->GetScreenX() / WindowWidth) &&
			c->y <= MouseY && MouseY < (int)(c->y + c->Height * gwinhandle->GetScreenY() / WindowHeight) &&
			InputEnable && Hero->SafeZone && strcmp(c->ID, Hero->ID) != NULL && (DWORD)WorldTime % 24 < 12)
		{
			unsigned int Temp = g_pRenderText->GetBgColor();
			g_pRenderText->SetBgColor(g_pRenderText->GetTextColor());
			g_pRenderText->SetTextColor(Temp);
		}

#if MAIN_UPDATE <= 303
		if (pCharacter->GetKind() == KIND_PLAYER)
		{
			int iSelectedCharacter = gmCharacters->GetCharacterIndex(pCharacter);

			if (SelectedCharacter != -1 && iSelectedCharacter == SelectedCharacter)
			{
				g_pBuffWindow->RenderBuffStatus(c->x + (c->Width / 2.f) / g_fScreenRate_x, c->y / g_fScreenRate_y, &pCharacter->Object);
			}
		}
#endif
		if (bGmMode)
		{
			g_pRenderText->SetFont(g_hFontBold);

			if (GMProtect->IsRenderWave(c->ID))
			{
				g_pRenderText->RenderWave(RenderPos.x, RenderPos.y, c->ID, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT, NULL, strlen(c->ID), false);
			}
			else
			{
				g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->ID, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT);
			}

			RenderPos.y += iLineHeight;
			g_pRenderText->SetFont(g_hFont);
		}
		else
		{
			if (GMProtect->IsRenderWave(c->ID))
			{
				g_pRenderText->RenderWave(RenderPos.x, RenderPos.y, c->ID, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT, NULL, strlen(c->ID), false);
			}
			else
			{
				g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->ID, RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT);
			}
			RenderPos.y += iLineHeight;
		}

		if (c->GuildColor == 0)
			g_pRenderText->SetBgColor(10, 30, 50, 150);
		else if (c->GuildColor == 1)
			g_pRenderText->SetBgColor(30, 50, 0, 150);
		else if (bGmMode)
			g_pRenderText->SetBgColor(30, 30, 30, 200);
		else
			g_pRenderText->SetBgColor(50, 0, 0, 150);

		DWORD dwTextColor[2];
		BYTE byAlpha[2] = { 255, 255 };
		if ((c->LifeTime[0] > 0 && c->LifeTime[0] < 10))
			byAlpha[0] = 128;
		if ((c->LifeTime[1] > 0 && c->LifeTime[1] < 10))
			byAlpha[1] = 128;

		if (bGmMode)
		{
			dwTextColor[0] = RGBA(250, 200, 50, byAlpha[0]);
			dwTextColor[1] = RGBA(250, 200, 50, byAlpha[1]);
		}
		else
		{
			dwTextColor[0] = RGBA(230, 220, 200, byAlpha[0]);
			dwTextColor[1] = RGBA(230, 220, 200, byAlpha[1]);
		}

		if (c->LifeTime[1] > 0)
		{
			g_pRenderText->SetTextColor(dwTextColor[1]);
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->Text[1], RenderBoxSize.cx, iLineHeight, RT3_SORT_LEFT);
			RenderPos.y += iLineHeight;

			g_pRenderText->SetTextColor(dwTextColor[0]);
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->Text[0], RenderBoxSize.cx, iLineHeight);
		}
		else if (c->LifeTime[0] > 0)
		{
			g_pRenderText->SetTextColor(dwTextColor[0]);
			g_pRenderText->RenderTextClipped(RenderPos.x, RenderPos.y, c->Text[0], RenderBoxSize.cx, iLineHeight);
		}

		if (pCharacter->GetKind() == KIND_PLAYER && MODEL_PLAYER == pCharacter->Object.Type)
		{
			const int nGensMarkHeight = 18;
			int nGensMarkPosY = (RenderPos.y - y - nGensMarkHeight) / 2 + y;

			x /= g_fScreenRate_x;
			if (gmProtect->RenderGuildLogo != 0)
			{
				if (pCharacter->GuildStatus != G_NONE && pCharacter->GuildMarkIndex != -1)
				{
					x -= 20.f;
					CreateGuildMark(pCharacter->GuildMarkIndex, true);
					RenderBitmap(BITMAP_GUILD, (float)x, (float)y / g_fScreenRate_y, 18.f, 18.f, 0.0, 0.0, 1.f, 1.f);
				}
			}

			if (c->LifeTime[1] > 0)
			{
				RenderPos.y -= iLineHeight;
			}

			if (pCharacter->m_byGensInfluence == 1 || pCharacter->m_byGensInfluence == 2)
			{
				g_pNewUIGensRanking->RanderMark(x, y / g_fScreenRate_y, pCharacter->m_byGensInfluence, pCharacter->m_byRankIndex, SEASON3B::CNewUIGensRanking::MARK_BOOLEAN, (float)RenderPos.y / g_fScreenRate_y);
			}
		}
	}
}

void CGMHeadChat::MoveChat()
{
	for (int i = 0; i < MAX_CHAT; i++)
	{
		CHAT* c = &Chat[i];

		if (c->IDLifeTime > 0)
			c->IDLifeTime -= FPS_ANIMATION_FACTOR;

		if (c->LifeTime[0] > 0)
			c->LifeTime[0] -= FPS_ANIMATION_FACTOR;

		if (c->LifeTime[1] > 0)
			c->LifeTime[1] -= FPS_ANIMATION_FACTOR;

		CHARACTER* pCharacter = c->Owner;

		if (pCharacter != NULL && (!pCharacter->Object.Live || !pCharacter->Object.Visible))
		{
			c->IDLifeTime = 0;
			c->LifeTime[0] = 0;
			c->LifeTime[1] = 0;
		}

		if (c->LifeTime[0] > 0)
		{
			DisableShopTitleDraw(c->Owner);
		}

		if (pCharacter != NULL)
		{
			if (c->LifeTime[0] <= 0)
			{
				EnableShopTitleDraw(c->Owner);
			}
		}
	}
}

void CGMHeadChat::SetPlayerColor(BYTE PK)
{
	switch (PK)
	{
	case 0:
		g_pRenderText->SetTextColor(150, 255, 240, 255);
		break;//npc
	case 1:
		g_pRenderText->SetTextColor(100, 120, 255, 255);
		break;
	case 2:
		g_pRenderText->SetTextColor(140, 180, 255, 255);
		break;
	case 3:
		g_pRenderText->SetTextColor(200, 220, 255, 255);
		break;//normal
	case 4:
		g_pRenderText->SetTextColor(255, 150, 60, 255);
		break; //pk1
	case 5:
		g_pRenderText->SetTextColor(255, 80, 30, 255);
		break;  //pk2
	default:
		g_pRenderText->SetTextColor(255, 0, 0, 255);
		break;    //pk3
	}
}

bool CGMHeadChat::EnableView(CHAT* c)
{
	if (c->Owner != NULL)
	{
		return true;
	}
	return false;
}
