///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzInfomation.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzScene.h"
#include "ZzzInterface.h"
#include "ReadScript.h"
#include "zzzInventory.h"
#include "zzzopenglutil.h"
#include "./Utilities/Log/ErrorReport.h"
#include "zzzLodTerrain.h"
#include "CSItemOption.h"
#include "GIPetManager.h"
#include "GMHellas.h"
#include "CComGem.h"
#include "NewUIInventoryCtrl.h"
#include "SocketSystem.h"
#include "NewUISystem.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "CGMProtect.h"
#include "CGMItemWings.h"

CLASS_ATTRIBUTE     ClassAttribute[MAX_CLASS];
MONSTER_SCRIPT      MonsterScript[MAX_MONSTER];
GATE_ATTRIBUTE* GateAttribute = NULL;
SKILL_ATTRIBUTE* SkillAttribute = NULL;
CHARACTER_MACHINE* CharacterMachine = NULL;
CHARACTER_ATTRIBUTE* CharacterAttribute = NULL;


#ifdef STATE_LIMIT_TIME
static  int     g_iWorldStateTime = 0;
static  DWORD   g_dwWorldStateBack = 0;
#endif// STATE_LIMIT_TIME

CGlobalText GlobalText;
CGlobalText TextGlobal;

void SaveTextFile(char* FileName)
{
	FILE* fp = fopen(FileName, "wb");

	int Size = MAX_GLOBAL_TEXT_STRING;
	BYTE* Buffer = new BYTE[Size];
	for (int i = 0; i < MAX_TEXTS; i++)
	{
		memcpy(Buffer, GlobalText[i], Size);
		BuxConvert(Buffer, Size);
		fwrite(Buffer, Size, 1, fp);
	}
	delete[] Buffer;
	fclose(fp);
}

char AbuseFilter[MAX_FILTERS][20];
char AbuseNameFilter[MAX_NAMEFILTERS][20];
int  AbuseFilterNumber = 0;
int  AbuseNameFilterNumber = 0;

void OpenFilterFile(char* FileName)
{
	FILE* fp = fopen(FileName, "rb");
	if (fp == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return;
	}

	int Size = 20;
	BYTE* Buffer = new BYTE[Size * MAX_FILTERS];
	fread(Buffer, Size * MAX_FILTERS, 1, fp);
	DWORD dwCheckSum;
	fread(&dwCheckSum, sizeof(DWORD), 1, fp);
	fclose(fp);

	{
		BYTE* pSeek = Buffer;
		for (int i = 0; i < MAX_FILTERS; i++)
		{
			BuxConvert(pSeek, Size);
			memcpy(AbuseFilter[i], pSeek, Size);
			if (AbuseFilter[i][0] == NULL)
			{
				AbuseFilterNumber = i;
				break;
			}
			pSeek += Size;
		}
	}

	delete[] Buffer;
	fclose(fp);
}

void OpenNameFilterFile(char* FileName)
{
	FILE* fp = fopen(FileName, "rb");
	if (fp == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return;
	}
	int Size = 20;
	BYTE* Buffer = new BYTE[Size * MAX_NAMEFILTERS];

	fread(Buffer, Size * MAX_NAMEFILTERS, 1, fp);

	DWORD dwCheckSum;
	fread(&dwCheckSum, sizeof(DWORD), 1, fp);
	fclose(fp);
	if (dwCheckSum != GenerateCheckSum2(Buffer, Size * MAX_NAMEFILTERS, 0x2BC1))
	{
		char Text[256];
		sprintf(Text, "%s - File corrupted.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
	else
	{
		BYTE* pSeek = Buffer;
		for (int i = 0; i < MAX_NAMEFILTERS; i++)
		{
			BuxConvert(pSeek, Size);
			memcpy(AbuseNameFilter[i], pSeek, Size);
			if (AbuseNameFilter[i][0] == NULL)
			{
				AbuseNameFilterNumber = i;
				break;
			}
			pSeek += Size;
		}
	}
	delete[] Buffer;
}

///////////////////////////////////////////////////////////////////////////////
// gate
///////////////////////////////////////////////////////////////////////////////

void OpenGateScript(char* FileName)
{
	FILE* fp = fopen(FileName, "rb");
	if (fp != NULL)
	{
		int Size = sizeof(GATE_ATTRIBUTE);
		BYTE* Buffer = new BYTE[Size];

		for (int i = 0; i < MAX_GATES; i++)
		{
			fread(Buffer, Size, 1, fp);
			BuxConvert(Buffer, Size);
			memcpy(&GateAttribute[i], Buffer, Size);
		}
		delete[] Buffer;
		fclose(fp);
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}


void OpenMonsterSkillScript(char* FileName)
{
	memset(MonsterSkill, -1, sizeof(Script_Skill));

	FILE* fp = fopen(FileName, "rb");
	if (fp != NULL)
	{
		int Size = (sizeof(Script_Skill) + sizeof(int));
		BYTE* Buffer = new BYTE[Size];
		int FileCount = 0;
		fread(&FileCount, sizeof(int), 1, fp);

		for (int i = 0; i < FileCount; i++)
		{
			fread(Buffer, Size, 1, fp);
			BuxConvert(Buffer, Size);

			int dummy = -1;
			int Seek = 0;
			memcpy(&dummy, Buffer + Seek, sizeof(int));
			Seek += sizeof(int);

			memcpy(MonsterSkill[dummy].Skill_Num, Buffer + Seek, sizeof(int) * MAX_MONSTERSKILL_NUM);

			Seek += (sizeof(int) * MAX_MONSTERSKILL_NUM);
			memcpy(&MonsterSkill[dummy].Slot, Buffer + Seek, sizeof(int));
		}
		delete[] Buffer;
		fclose(fp);
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}

void OpenNpcScript(char* FileName)
{
	if ((SMDFile = fopen(FileName, "rb")) == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return;
	}
	SMDToken Token;
	while (true)
	{
		Token = (*GetToken)();
		if (Token == END) break;
		if (Token == NAME && strcmp("end", TokenString) == NULL) break;
		if (Token == NUMBER)
		{
			int Type, x, y, Dir;
			Type = (int)TokenNumber;
			Token = (*GetToken)(); x = (int)TokenNumber;
			Token = (*GetToken)(); y = (int)TokenNumber;
			Token = (*GetToken)(); Dir = (int)TokenNumber;
		}
	}
	fclose(SMDFile);
}

void OpenSkillScript(char* FileName)
{
	//copy_new_file(FileName, "Data\\Local\\Skill2.bmd");

	FILE* fp = fopen(FileName, "rb");

	if (fp != NULL)
	{
		int Size = sizeof(SKILL_ATTRIBUTE);

		// ÀÐ±â
		BYTE* Buffer = new BYTE[Size * MAX_SKILLS];
		fread(Buffer, Size * MAX_SKILLS, 1, fp);
		// crc Ã¼Å©
		DWORD dwCheckSum;
		fread(&dwCheckSum, sizeof(DWORD), 1, fp);
		fclose(fp);
		if (dwCheckSum != GenerateCheckSum2(Buffer, Size * MAX_SKILLS, 0x5A18))
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", FileName);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
		else
		{
			BYTE* pSeek = Buffer;
			for (int i = 0; i < MAX_SKILLS; i++)
			{

				BuxConvert(pSeek, Size);
				memcpy(&SkillAttribute[i], pSeek, Size);

				pSeek += Size;
			}
		}
		delete[] Buffer;
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}

BOOL IsValidateSkillIdx(INT iSkillIdx)
{
	if (iSkillIdx >= MAX_SKILLS || iSkillIdx < 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IsCorrectSkillType(INT iSkillSeq, eTypeSkill iSkillTypeIdx)
{
	if (IsValidateSkillIdx(iSkillSeq) == FALSE)
	{
		return FALSE;
	}

	SKILL_ATTRIBUTE& CurSkillAttribute = SkillAttribute[iSkillSeq];

	if (CurSkillAttribute.TypeSkill == (BYTE)iSkillTypeIdx)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL IsCorrectSkillType_FrendlySkill(INT iSkillSeq)
{
	return IsCorrectSkillType(iSkillSeq, eTypeSkill_FrendlySkill);
}

BOOL IsCorrectSkillType_Buff(INT iSkillSeq)
{
	return IsCorrectSkillType(iSkillSeq, eTypeSkill_Buff);
}

BOOL IsCorrectSkillType_DeBuff(INT iSkillSeq)
{
	return IsCorrectSkillType(iSkillSeq, eTypeSkill_DeBuff);
}

BOOL IsCorrectSkillType_CommonAttack(INT iSkillSeq)
{
	return IsCorrectSkillType(iSkillSeq, eTypeSkill_CommonAttack);
}

///////////////////////////////////////////////////////////////////////////////
// dialog
///////////////////////////////////////////////////////////////////////////////

int g_iCurrentDialog = -1;
DIALOG_SCRIPT g_DialogScript[MAX_DIALOG];

void OpenDialogFile(char* FileName)
{
	FILE* fp = fopen(FileName, "rb");
	if (fp == NULL)
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", FileName);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		return;
	}
	int Size = sizeof(DIALOG_SCRIPT);
	BYTE* Buffer = new BYTE[Size];
	for (int i = 0; i < MAX_DIALOG; i++)
	{
		fread(Buffer, Size, 1, fp);
		BuxConvert(Buffer, Size);
		memcpy(&g_DialogScript[i], Buffer, Size);
	}
	delete[] Buffer;
	fclose(fp);
}

///////////////////////////////////////////////////////////////////////////////
// item
///////////////////////////////////////////////////////////////////////////////

int ConvertItemType(BYTE* Item)
{
	return (32 * (Item[5] & 0xF0) + Item[0] + 2 * (Item[3] & 0x80));
}

void PrintItem(char* FileName)
{
	FILE* fp = fopen(FileName, "wt");
	fprintf(fp, "                ÀÌ¸§  ÃÖ¼Ò°ø°Ý·Â ÃÖ´ë°ø°Ý·Â ¹æ¾î·Â ¹æ¾îÀ² ÇÊ¿äÈû ÇÊ¿ä¹ÎÃ¸ ÇÊ¿ä¿¡³ÊÁö\n");
	//fprintf(fp,"                ÀÌ¸§    Ä«¿À½º¼º°øÈ®·ü\n");
	bool Excellent = true;
	for (int i = 0; i < 16 * MAX_ITEM_INDEX; i++)
	{
		if ((i & 0x1FF) == 0)
		{
			fprintf(fp, "------------------------------------------------------------------------------------------------------\n");
		}
		Script_Item* p = GMItemMng->find(i);
		if (p->Name[0] != NULL)
		{
			int Plus;
			if (i >= 12 * MAX_ITEM_INDEX)
				Plus = 1;
			else
				Plus = 10;
			for (int j = 0; j < Plus; j++)
			{
				int Level = j;
				int RequireStrength = 0;
				int RequireDexterity = 0;
				int RequireEnergy = 0;
				int DamageMin = p->DamageMin;
				int DamageMax = p->DamageMax;
				int Defense = p->Defense;
				int SuccessfulBlocking = p->SuccessfulBlocking;
				if (DamageMin > 0)
				{
					if (Excellent)
					{
						if (p->Level)
							DamageMin += p->DamageMin * 25 / p->Level + 5;
					}
					DamageMin += Level * 3;
				}
				if (DamageMax > 0)
				{
					if (Excellent)
					{
						if (p->Level)
							DamageMax += p->DamageMin * 25 / p->Level + 5;
					}
					DamageMax += Level * 3;
				}
				if (Defense > 0)
				{
					if (i >= ITEM_SHIELD && i < ITEM_SHIELD + MAX_ITEM_INDEX)
					{
						Defense += Level;
					}
					else
					{
						if (Excellent)
						{
							if (p->Level)
								Defense += p->Defense * 12 / p->Level + 4 + p->Level / 5;
						}
						Defense += Level * 3;
					}
				}
				if (SuccessfulBlocking > 0)
				{
					if (Excellent)
					{
						if (p->Level)
							SuccessfulBlocking += p->SuccessfulBlocking * 25 / p->Level + 5;
					}
					SuccessfulBlocking += Level * 3;
				}
				int ItemLevel = p->Level;
				if (Excellent)
					ItemLevel = p->Level + 25;
				if (p->RequireStrength)
					RequireStrength = 20 + p->RequireStrength * (ItemLevel + Level * 3) * 3 / 100;
				else
					RequireStrength = 0;
				if (p->RequireDexterity)
					RequireDexterity = 20 + p->RequireDexterity * (ItemLevel + Level * 3) * 3 / 100;
				else
					RequireDexterity = 0;
				if (p->RequireEnergy)
				{
					RequireEnergy = 20 + p->RequireEnergy * (ItemLevel + Level * 3) * 4 / 10;
				}
				else
				{
					RequireEnergy = 0;
				}
				if (i >= ITEM_STAFF && i < ITEM_STAFF * MAX_ITEM_INDEX)
				{
					DamageMin = DamageMin / 2 + Level * 2;
					DamageMax = 0;
				}
				ITEM ip;
				ip.Type = i;
				ItemConvert(&ip, j << 3, Excellent, 0);

				ItemValue(&ip, 0);

				if (j == 0)
					fprintf(fp, "%20s %8d %8d %8d %8d %8d %8d %8d %8d %8d\n", p->Name, DamageMin, DamageMax, Defense, SuccessfulBlocking, RequireStrength, RequireDexterity, RequireEnergy, p->WeaponSpeed, ItemValue(&ip));
				//fprintf(fp,"%20s %4d%%",p->Name, iRate);
				else
					fprintf(fp, "%17s +%d %8d %8d %8d %8d %8d %8d %8d %8d %8d\n", "", Level, DamageMin, DamageMax, Defense, SuccessfulBlocking, RequireStrength, RequireDexterity, RequireEnergy, p->WeaponSpeed, ItemValue(&ip));
				//fprintf(fp,"%4d%%<+%d>",iRate,Level);
			}
			fprintf(fp, "\n");
		}
	}
	fclose(fp);
}

BYTE getSkillIndexByBook(int Type)
{
	int SkillIndex = Type - ITEM_ETC + 1;
	switch (SkillIndex)
	{
	case 17: SkillIndex = AT_SKILL_BLAST_POISON; break;
	case 18: SkillIndex = AT_SKILL_BLAST_FREEZE; break;
	case 19: SkillIndex = AT_SKILL_BLAST_HELL;   break;
	}

	return SkillIndex;
}

bool IsCepterItem(int iType)
{
	if ((iType >= ITEM_MACE + 8 && iType <= ITEM_MACE + 15) || iType == ITEM_MACE + 17 || iType == ITEM_MACE + 18)
	{
		return true;
	}
	return false;
}

void ItemConvert(ITEM* pItem, BYTE Attribute1, BYTE Attribute2, BYTE Attribute3)
{
	int is_excellent; // [sp+54h] [bp-28h]@1
	int excelAddValue; // [sp+5Ch] [bp-20h]@1
	int excelWing; // [sp+68h] [bp-14h]@1
	char is_ancient; // [sp+6Fh] [bp-Dh]@1
	int Level; // [sp+74h] [bp-8h]@1
	Script_Item* item_info; // [sp+78h] [bp-4h]@22


	pItem->Level = Attribute1;
	Level = ((signed int)Attribute1 >> 3) & 0xF;
	is_excellent = Attribute2 & 0x3F;
	excelWing = Attribute2 & 0x3F;
	excelAddValue = 0;
	is_ancient = 0;

	CUSTOM_WING_INFO* customWings = GMItemWings->Find(pItem->Type);

	if (pItem->Type >= (ITEM_WING + 3) && pItem->Type <= (ITEM_WING + 6)
		|| pItem->Type >= (ITEM_WING + 36) && pItem->Type <= (ITEM_WING + 40)
		|| pItem->Type >= (ITEM_WING + 42) && pItem->Type <= (ITEM_WING + 43)
		|| pItem->Type == (ITEM_SWORD + 19)
		|| pItem->Type == (ITEM_BOW + 18)
		|| pItem->Type == (ITEM_STAFF + 10)
		|| pItem->Type == (ITEM_MACE + 13)
		|| pItem->Type == (ITEM_HELPER + 30)
		|| pItem->Type >= (ITEM_WING + 130) && pItem->Type <= (ITEM_WING + 134)
		|| pItem->Type >= (ITEM_WING + 49) && pItem->Type <= (ITEM_WING + 50)
		|| pItem->Type == (ITEM_WING + 135)
		|| pItem->Type == (ITEM_STAFF + 36)
		|| customWings != NULL)
	{
		is_excellent = 0;
	}

	if ((Attribute3 % 0x4) == EXT_A_SET_OPTION || (Attribute3 % 0x4) == EXT_B_SET_OPTION)
	{
		is_excellent = 1;
		is_ancient = true;
	}

	item_info = GMItemMng->find(pItem->Type);

	int Weapon02Kind1 = item_info->Kind1;
	int Weapon02Kind2 = item_info->Kind2;
	int Weapon02Kind3 = item_info->Kind3;

	int RequireStrength = item_info->RequireStrength;
	int RequireDexterity = item_info->RequireDexterity;
	int RequireEnergy = item_info->RequireEnergy;
	int RequireVitality = item_info->RequireVitality;
	int RequireCharisma = item_info->RequireCharisma;
	int RequireLevel = item_info->RequireLevel;


	pItem->TwoHand = item_info->TwoHand;
	pItem->WeaponSpeed = item_info->WeaponSpeed;
	pItem->DamageMin = item_info->DamageMin;
	pItem->DamageMax = item_info->DamageMax;
	pItem->SuccessfulBlocking = item_info->SuccessfulBlocking;
	pItem->Defense = item_info->Defense;
	pItem->MagicDefense = item_info->MagicDefense;
	pItem->WalkSpeed = item_info->WalkSpeed;
	pItem->MagicPower = item_info->MagicPower;
	pItem->AttackPower = item_info->PowerATTK;

	pItem->Kind1 = Weapon02Kind1;
	pItem->Kind2 = Weapon02Kind2;
	pItem->Kind3 = Weapon02Kind3;

	int setItemDropLevel = item_info->Level + 30;

	switch (pItem->Type)
	{
	case ITEM_MACE + 6:
		excelAddValue = 15;
		break;
	case ITEM_BOW + 6:
		excelAddValue = 30;
		break;
	case ITEM_STAFF + 7:
		excelAddValue = 25;
		break;
	}

	if (item_info->DamageMin > 0)
	{
		if (is_excellent > 0)
		{
			if (item_info->Level)
			{
				if (excelAddValue)
					pItem->DamageMin += excelAddValue;
				else
					pItem->DamageMin += (unsigned int)(25 * item_info->DamageMin / item_info->Level) + 5;
			}
		}

		if (is_ancient)
		{
			pItem->DamageMin += setItemDropLevel / 40 + 5;
		}

		WORD damage = 0;

		if (Level <= 9)
			damage = (Attribute1 >> 3) & 0xF;
		else
			damage = 9;

		pItem->DamageMin += 3 * damage;

		switch (Level)
		{
		case 15:
			pItem->DamageMin += 9;
		case 14:
			pItem->DamageMin += 8;
		case 13:
			pItem->DamageMin += 7;
		case 12:
			pItem->DamageMin += 6;
		case 11:
			pItem->DamageMin += 5;
		case 10:
			pItem->DamageMin += 4;
			break;
		default:
			break;
		}
	}
	if (item_info->DamageMax > 0)
	{
		if (is_excellent > 0)
		{
			if (item_info->Level)
			{
				if (excelAddValue)
					pItem->DamageMax += excelAddValue;
				else
					pItem->DamageMax += (unsigned int)(25 * item_info->DamageMin / item_info->Level) + 5;
			}
		}

		if (is_ancient)
		{
			pItem->DamageMax += setItemDropLevel / 40 + 5;
		}

		WORD damage = 0;

		if (Level <= 9)
			damage = (Attribute1 >> 3) & 0xF;
		else
			damage = 9;

		pItem->DamageMax += 3 * damage;

		switch (Level)
		{
		case 15:
			pItem->DamageMax += 9;
		case 14:
			pItem->DamageMax += 8;
		case 13:
			pItem->DamageMax += 7;
		case 12:
			pItem->DamageMax += 6;
		case 11:
			pItem->DamageMax += 5;
		case 10:
			pItem->DamageMax += 4;
			break;
		default:
			break;
		}
	}

	if (item_info->MagicPower > 0)
	{
		if (is_excellent > 0)
		{
			if (item_info->Level)
			{
				if (excelAddValue)
					pItem->MagicPower += excelAddValue;
				else
					pItem->MagicPower += (unsigned int)(25 * item_info->MagicPower / item_info->Level) + 5;
			}
		}

		if (is_ancient)
		{
			pItem->MagicPower += setItemDropLevel / 60 + 5;
		}

		WORD damage = 0;

		if (Level <= 9)
			damage = (Attribute1 >> 3) & 0xF;
		else
			damage = 9;

		pItem->MagicPower += 3 * damage;

		switch (Level)
		{
		case 15:
			pItem->MagicPower += 9;
		case 14:
			pItem->MagicPower += 8;
		case 13:
			pItem->MagicPower += 7;
		case 12:
			pItem->MagicPower += 6;
		case 11:
			pItem->MagicPower += 5;
		case 10:
			pItem->MagicPower += 4;
			break;
		default:
			break;
		}

		pItem->MagicPower /= 2;

		if (IsCepterItem(pItem->Type) == false)
		{
			pItem->MagicPower += Level * 2;
		}
	}

	if (item_info->SuccessfulBlocking > 0)
	{
		if (is_excellent > 0 && item_info->Level)
		{
			pItem->SuccessfulBlocking += (unsigned int)(25 * item_info->SuccessfulBlocking / item_info->Level) + 5;
		}

		WORD damage = 0;

		if (Level <= 9)
			damage = (Attribute1 >> 3) & 0xF;
		else
			damage = 9;

		pItem->SuccessfulBlocking += 3 * damage;

		switch (Level)
		{
		case 15:
			pItem->SuccessfulBlocking += 9;
		case 14:
			pItem->SuccessfulBlocking += 8;
		case 13:
			pItem->SuccessfulBlocking += 7;
		case 12:
			pItem->SuccessfulBlocking += 6;
		case 11:
			pItem->SuccessfulBlocking += 5;
		case 10:
			pItem->SuccessfulBlocking += 4;
			break;
		default:
			break;
		}
	}

	if (pItem->Type == ITEM_HELPER + 30)
	{
		item_info->Defense = 15;
		pItem->Defense = 15;
	}
	if (item_info->Defense > 0)
	{
		WORD defense = 0;

		if (pItem->Type < ITEM_SHIELD || pItem->Type >= ITEM_HELM)
		{
			if (is_excellent > 0 && item_info->Level)
				pItem->Defense += 12 * item_info->Defense / item_info->Level + item_info->Level / 5 + 4;

			if (is_ancient)
				pItem->Defense += 3 * pItem->Defense / setItemDropLevel + setItemDropLevel / 30 + 2;

			if ((pItem->Type < ITEM_WING + 3 || pItem->Type > ITEM_WING + 6) && pItem->Type != ITEM_WING + 42)
			{
				if (pItem->Type != ITEM_HELPER + 30 && pItem->Type != ITEM_WING + 49)
				{
					if (customWings != NULL)
					{
						if (Level <= 9)
							defense = ((signed int)Attribute1 >> 3) & 0xF;
						else
							defense = 9;
						pItem->Defense += customWings->DefenseConstA * defense;
					}
					else if (Weapon02Kind2 != ItemKind2::WINGS_LVL_3)
					{
						if (Level <= 9)
							defense = ((signed int)Attribute1 >> 3) & 0xF;
						else
							defense = 9;
						pItem->Defense += 3 * defense;
					}
					else
					{
						if (Level <= 9)
							defense = ((signed int)Attribute1 >> 3) & 0xF;
						else
							defense = 9;
						pItem->Defense += 4 * defense;
					}
				}
				else
				{
					if (Level <= 9)
						defense = ((signed int)Attribute1 >> 3) & 0xF;
					else
						defense = 9;
					pItem->Defense += 2 * defense;
				}
			}
			else
			{
				if (Level <= 9)
					defense = ((signed int)Attribute1 >> 3) & 0xF;
				else
					defense = 9;
				pItem->Defense += 2 * defense;
			}

			if (customWings != NULL || Weapon02Kind2 != ItemKind2::WINGS_LVL_3)
			{
				switch (Level)
				{
				case 15:
					pItem->Defense += 9;
				case 14:
					pItem->Defense += 8;
				case 13:
					pItem->Defense += 7;
				case 12:
					pItem->Defense += 6;
				case 11:
					pItem->Defense += 5;
				case 10:
					pItem->Defense += 4;
					break;
				default:
					break;
				}
			}
			else
			{
				switch (Level)
				{
				case 15:
					pItem->Defense += 10;
				case 14:
					pItem->Defense += 9;
				case 13:
					pItem->Defense += 8;
				case 12:
					pItem->Defense += 7;
				case 11:
					pItem->Defense += 6;
				case 10:
					pItem->Defense += 5;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			pItem->Defense += Level;

			if (is_ancient)
				pItem->Defense += (unsigned int)(20 * pItem->Defense / setItemDropLevel) + 2;
		}
	}

	if ((signed int)item_info->MagicDefense > 0)
	{
		WORD magicdefense = 0;
		if (Level <= 9)
			magicdefense = ((signed int)Attribute1 >> 3) & 0xF;
		else
			magicdefense = 9;

		pItem->MagicDefense += 3 * magicdefense;

		switch (Level)
		{
		case 15:
			pItem->MagicDefense += 9;
		case 14:
			pItem->MagicDefense += 8;
		case 13:
			pItem->MagicDefense += 7;
		case 12:
			pItem->MagicDefense += 6;
		case 11:
			pItem->MagicDefense += 5;
		case 10:
			pItem->MagicDefense += 4;
			break;
		default:
			break;
		}
	}

	int item_level = item_info->Level;

	if (is_excellent)
		item_level = item_info->Level + 25;
	else if (is_ancient)
		item_level = item_info->Level + 30;

	int addValue = 4;

	if ((pItem->Type >= ITEM_WING + 3 && pItem->Type <= ITEM_WING + 6) || pItem->Type == ITEM_WING + 42)
	{
		addValue = 5;
	}

	if (item_info->RequireLevel && ((pItem->Type >= ITEM_SWORD && pItem->Type < ITEM_WING)
		|| (pItem->Type == ITEM_HELPER + 37)
		|| (pItem->Type >= ITEM_WING + 7 && pItem->Type <= ITEM_WING + 40)
		|| (pItem->Type >= ITEM_WING + 43 && pItem->Type < ITEM_HELPER)
		&& (pItem->Type != ITEM_WING + 49)
		))
	{
		pItem->RequireLevel = item_info->RequireLevel;
	}
	else if (item_info->RequireLevel && ((pItem->Type >= ITEM_WING && pItem->Type <= ITEM_WING + 7)
		|| (pItem->Type >= ITEM_WING + 41 && pItem->Type <= ITEM_WING + 42)
		|| (pItem->Type == ITEM_WING + 49)
		|| pItem->Type >= ITEM_HELPER))
	{
		pItem->RequireLevel = item_info->RequireLevel + Level * addValue;
	}
	else
	{
		if (customWings != NULL)
			pItem->RequireLevel = item_info->RequireLevel;
		else
			pItem->RequireLevel = 0;
	}

	if (RequireStrength > 0)
	{
		if (item_info->Level < 220)
		{
			pItem->RequireStrength = (3 * (3 * Level + item_level) * RequireStrength / 100 + 20);
		}
		else
		{
			if (is_excellent)
				pItem->RequireStrength = (RequireStrength + RequireStrength * 0.75f + RequireStrength * 0.02f * Level);
			else
				pItem->RequireStrength = (RequireStrength + RequireStrength * 0.02f * Level);
		}
	}
	else
	{
		pItem->RequireStrength = 0;
	}

	if (RequireDexterity > 0)
	{
		if (item_info->Level < 220)
		{
			pItem->RequireDexterity = (3 * (3 * Level + item_level) * RequireDexterity / 100 + 20);
		}
		else
		{
			if (is_excellent)
				pItem->RequireDexterity = (RequireDexterity + RequireDexterity * 0.75f + RequireDexterity * 0.02f * Level);
			else
				pItem->RequireDexterity = (RequireDexterity + RequireDexterity * 0.02f * Level);
		}
	}
	else
	{
		pItem->RequireDexterity = 0;
	}

	if (RequireEnergy > 0)
	{
		if (item_info->Level < 220)
		{
			if (Weapon02Kind2 == ItemKind2::BOOK)
				pItem->RequireEnergy = (3 * (Level + item_level) * RequireEnergy / 100 + 20);
			else if (RequireLevel > 0 && (Weapon02Kind2 == ItemKind2::SKILL_SCROLL_1 || Weapon02Kind2 == ItemKind2::SKILL_PARCHMENT))
				pItem->RequireEnergy = (4 * RequireLevel * RequireEnergy / 100 + 20);
			else
				pItem->RequireEnergy = (RequireEnergy * (3 * Level + item_level) * 4 / 100 + 20);
		}
		else
		{
			if (is_excellent)
				pItem->RequireEnergy = (RequireEnergy * 2 + RequireEnergy * 0.035f * Level);
			else
				pItem->RequireEnergy = (RequireEnergy + RequireEnergy * 0.035f * Level);
		}
	}
	else
	{
		pItem->RequireEnergy = 0;
	}

	if (RequireVitality > 0)
	{
		if (item_info->Level < 220)
		{
			pItem->RequireVitality = (3 * (3 * Level + item_level) * RequireVitality / 100 + 20);
		}
		else
		{
			if (is_excellent)
				pItem->RequireVitality = (RequireVitality + RequireVitality * 0.75f + RequireVitality * 0.02f * Level);
			else
				pItem->RequireVitality = (RequireVitality + RequireVitality * 0.02f * Level);
		}
	}
	else
	{
		pItem->RequireVitality = 0;
	}

	if (pItem->Type != ITEM_HELPER + 5)
	{
		if (RequireCharisma > 0)
			pItem->RequireCharisma = (3 * (3 * Level + item_level) * RequireCharisma / 100 + 20);
		else
			pItem->RequireCharisma = 0;
	}
	else
	{
		pItem->RequireCharisma = (185 + (item_info->RequireCharisma * 15));
	}

	if (Weapon02Kind1 == ItemKind1::SKILL)
	{
		pItem->RequireStrength = RequireStrength;
		pItem->RequireDexterity = RequireDexterity;
		pItem->RequireEnergy = RequireEnergy;
		pItem->RequireVitality = RequireVitality;
		pItem->RequireCharisma = RequireCharisma;

		if (pItem->Type == ITEM_WING + 11)
		{
			switch (Level)
			{
			case 0:
				pItem->RequireEnergy = 30;
				break;
			case 1:
				pItem->RequireEnergy = 60;
				break;
			case 2:
				pItem->RequireEnergy = 90;
				break;
			case 3:
				pItem->RequireEnergy = 130;
				break;
			case 4:
				pItem->RequireEnergy = 170;
				break;
			case 5:
				pItem->RequireEnergy = 210;
				break;
			case 6:
				pItem->RequireEnergy = 300;
				break;
			case 7:
				pItem->RequireEnergy = 500;
				break;
			default:
				pItem->RequireEnergy = 0;
				break;
			}
		}
	}

	if (pItem->Type == ITEM_HELPER + 10)
	{
		if (Level <= 2)
			pItem->RequireLevel = 20;
		else
			pItem->RequireLevel = 50;
	}

	if (pItem->Type >= ITEM_HELM + 29 && pItem->Type <= ITEM_HELM + 33
		|| pItem->Type >= ITEM_ARMOR + 29 && pItem->Type <= ITEM_ARMOR + 33
		|| pItem->Type >= ITEM_PANTS + 29 && pItem->Type <= ITEM_PANTS + 33
		|| pItem->Type >= ITEM_GLOVES + 29 && pItem->Type <= ITEM_GLOVES + 33
		|| pItem->Type >= ITEM_BOOTS + 29 && pItem->Type <= ITEM_BOOTS + 33
		|| pItem->Type == ITEM_SWORD + 22
		|| pItem->Type == ITEM_SWORD + 23
		|| pItem->Type == ITEM_STAFF + 12
		|| pItem->Type == ITEM_BOW + 21
		|| pItem->Type == ITEM_MACE + 14
		|| pItem->Type == ITEM_STAFF + 19
		|| pItem->Type == ITEM_HELM + 43
		|| pItem->Type == ITEM_ARMOR + 43
		|| pItem->Type == ITEM_PANTS + 43
		|| pItem->Type == ITEM_GLOVES + 43
		|| pItem->Type == ITEM_BOOTS + 43
		|| Check_LuckyItem(pItem->Type))
	{
		is_excellent = 0;
	}

	if (is_excellent > 0 && pItem->RequireLevel > 0 && pItem->Type != ITEM_HELPER + 37)
	{
		pItem->RequireLevel += 20;
	}
	pItem->SpecialNum = 0;

	if (customWings != NULL)
	{
		if (excelWing & 0x01)
		{
			pItem->SpecialValue[pItem->SpecialNum] = customWings->NewOptionValue1;
			pItem->Special[pItem->SpecialNum] = customWings->NewOptionIndex1;
			pItem->SpecialNum++;
		}
		if ((excelWing >> 1) & 0x01)
		{
			pItem->SpecialValue[pItem->SpecialNum] = customWings->NewOptionValue2;
			pItem->Special[pItem->SpecialNum] = customWings->NewOptionIndex2;
			pItem->SpecialNum++;
		}
		if ((excelWing >> 2) & 0x01)
		{
			pItem->SpecialValue[pItem->SpecialNum] = customWings->NewOptionValue3;
			pItem->Special[pItem->SpecialNum] = customWings->NewOptionIndex3;
			pItem->SpecialNum++;
		}
		if ((excelWing >> 3) & 0x01)
		{
			pItem->SpecialValue[pItem->SpecialNum] = customWings->NewOptionValue4;
			pItem->Special[pItem->SpecialNum] = customWings->NewOptionIndex4;
			pItem->SpecialNum++;
		}
	}
	else if ((pItem->Type < ITEM_WING + 3 || pItem->Type > ITEM_WING + 6) && pItem->Type != ITEM_WING + 42)
	{
		if (pItem->Type != ITEM_HELPER + 30 && pItem->Type != ITEM_WING + 49)
		{
			if (pItem->Type >= ITEM_WING + 36 && pItem->Type <= ITEM_WING + 40 || pItem->Type == ITEM_WING + 43 || pItem->Type == ITEM_WING + 50)
			{
				if (Attribute2 & 1)
				{
					pItem->SpecialValue[pItem->SpecialNum] = 5;
					pItem->Special[pItem->SpecialNum++] = AT_ONE_PERCENT_DAMAGE;
				}
				if ((excelWing >> 1) & 1)
				{
					pItem->SpecialValue[pItem->SpecialNum] = 5;
					pItem->Special[pItem->SpecialNum++] = AT_ONE_PERCENT_DAMAGE;
				}
				if ((excelWing >> 2) & 1)
				{
					pItem->SpecialValue[pItem->SpecialNum] = 5;
					pItem->Special[pItem->SpecialNum++] = AT_RECOVER_FULL_LIFE;
				}
				if ((excelWing >> 3) & 1)
				{
					pItem->SpecialValue[pItem->SpecialNum] = 5;
					pItem->Special[pItem->SpecialNum++] = AT_RECOVER_FULL_MANA;
				}
			}
		}
		else
		{
			if (Attribute2 & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5 * Level + 50;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_HP_MAX;
			}
			if ((excelWing >> 1) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5 * Level + 50;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MP_MAX;
			}
			if ((excelWing >> 2) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 3;
				pItem->Special[pItem->SpecialNum++] = AT_ONE_PERCENT_DAMAGE;
			}
			if ((excelWing >> 3) & 1 && pItem->Type != ITEM_WING + 49)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5 * Level + 10;
				pItem->Special[pItem->SpecialNum++] = 172;
			}
		}
	}
	else
	{
		if (Attribute2 & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 5 * Level + 50;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_HP_MAX;
		}
		if ((excelWing >> 1) & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 5 * Level + 50;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MP_MAX;
		}
		if ((excelWing >> 2) & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 3;
			pItem->Special[pItem->SpecialNum++] = AT_ONE_PERCENT_DAMAGE;
		}
		if ((excelWing >> 3) & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 50;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_AG_MAX;
		}
		if ((excelWing >> 4) & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 5;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
		}
	}

	if (((signed int)Attribute1 >> 7) & 1 && item_info->m_wSkillIndex)
		pItem->Special[pItem->SpecialNum++] = item_info->m_wSkillIndex;

	if (((signed int)Attribute1 >> 2) & 1)
	{
		if (customWings != NULL)
		{
			pItem->Special[pItem->SpecialNum++] = AT_LUCK;
		}
		else
		{
			if (pItem->Type >= ITEM_SWORD && pItem->Type < ITEM_WING
				&& pItem->Type != ITEM_BOW + 7
				&& pItem->Type != ITEM_BOW + 15)
			{
				pItem->Special[pItem->SpecialNum++] = AT_LUCK;
			}
			if (pItem->Type >= ITEM_WING && pItem->Type <= ITEM_WING + 6
				|| pItem->Type >= ITEM_WING + 41 && pItem->Type <= ITEM_WING + 42)
			{
				pItem->Special[pItem->SpecialNum++] = AT_LUCK;
			}

			if (pItem->Type == ITEM_HELPER + 30 || pItem->Type == ITEM_WING + 49)
				pItem->Special[pItem->SpecialNum++] = AT_LUCK;

			if (pItem->Type >= ITEM_WING + 36 && pItem->Type <= ITEM_WING + 40
				|| pItem->Type == ITEM_WING + 43
				|| pItem->Type == ITEM_WING + 50)
			{
				pItem->Special[pItem->SpecialNum++] = AT_LUCK;
			}
		}
	}

	int Option3 = (Attribute1 & 3) + 4 * ((Attribute2 & 0x40) >> 6);

	if (Option3)
	{
		if (pItem->Type == ITEM_HELPER + 3)
		{
			if (Option3 & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5;
				pItem->Special[pItem->SpecialNum++] = AT_DAMAGE_ABSORB;
			}
			if ((Option3 >> 1) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 50;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_AG_MAX;
			}
			if ((Option3 >> 2) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
			}
		}
		else
		{
			if (pItem->Type >= ITEM_SWORD && pItem->Type < ITEM_STAFF
				&& pItem->Type != ITEM_BOW + 7
				&& pItem->Type != ITEM_BOW + 15)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
				pItem->RequireStrength += 5 * Option3;
			}
			if (pItem->Type >= ITEM_STAFF && pItem->Type < ITEM_SHIELD)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
				if (Weapon02Kind2 != ItemKind2::BOOK)
					pItem->Special[pItem->SpecialNum] = AT_IMPROVE_MAGIC;
				else
					pItem->Special[pItem->SpecialNum] = AT_IMPROVE_CURSE;
				++pItem->SpecialNum;
				pItem->RequireStrength += 5 * Option3;
			}
			if (pItem->Type >= ITEM_SHIELD && pItem->Type < ITEM_HELM)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 5 * Option3;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_BLOCKING;
				pItem->RequireStrength += 5 * Option3;
			}
			if (pItem->Type >= ITEM_HELM && pItem->Type < ITEM_WING)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DEFENSE;
				pItem->RequireStrength += 5 * Option3;
			}
			if (pItem->Type >= ITEM_HELPER + 8 && pItem->Type < ITEM_POTION && pItem->Type != ITEM_HELPER + 30)
			{
				if (pItem->Type == ITEM_HELPER + 24)
				{
					pItem->SpecialValue[pItem->SpecialNum] = Option3;
					pItem->Special[pItem->SpecialNum++] = 204;
				}
				else if (pItem->Type == ITEM_HELPER + 28)
				{
					pItem->SpecialValue[pItem->SpecialNum] = Option3;
					pItem->Special[pItem->SpecialNum++] = AT_SKILL_HELLOWIN_EVENT_1;
				}
				else
				{
					pItem->SpecialValue[pItem->SpecialNum] = Option3;
					pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
				}
			}
			if (pItem->Type == ITEM_WING)
			{
				pItem->SpecialValue[pItem->SpecialNum] = Option3;
				pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
			}
			else if (pItem->Type != ITEM_WING + 1 && pItem->Type != ITEM_WING + 41)
			{
				switch (pItem->Type)
				{
				case ITEM_WING + 2:
					pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
					pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					break;
				case ITEM_WING + 3:
					if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					break;
				case ITEM_WING + 4:
					if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 5:
					if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 6:
					if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
					}
					break;
				case ITEM_HELPER + 30:
					pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
					pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					break;
				case ITEM_WING + 49:
					if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 42:
					pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
					if ((excelWing >> 5) & 1)
						pItem->Special[pItem->SpecialNum] = AT_IMPROVE_MAGIC;
					else
						pItem->Special[pItem->SpecialNum] = AT_IMPROVE_CURSE;
					++pItem->SpecialNum;
					break;
				case ITEM_WING + 36:
					if ((excelWing >> 4) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DEFENSE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 37:
					if ((excelWing >> 4) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
					}
					else if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DEFENSE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 38:
					if ((excelWing >> 4) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DEFENSE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				case ITEM_WING + 39:
					if ((excelWing >> 4) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				default:
					if (customWings != NULL)
					{
						if ((excelWing >> 4) & 0x01)
						{
							pItem->SpecialValue[pItem->SpecialNum] = Option3 * customWings->OptionValue1;
							pItem->Special[pItem->SpecialNum++] = customWings->OptionIndex1;
						}
						else if ((excelWing >> 5) & 0x01)
						{
							pItem->SpecialValue[pItem->SpecialNum] = Option3 * customWings->OptionValue2;
							pItem->Special[pItem->SpecialNum++] = customWings->OptionIndex2;
						}
						else
						{
							pItem->SpecialValue[pItem->SpecialNum] = Option3 * customWings->OptionValue3;
							pItem->Special[pItem->SpecialNum++] = customWings->OptionIndex3;
						}
					}
					else if (pItem->Type != ITEM_WING + 40 && pItem->Type != ITEM_WING + 50)
					{
						if (pItem->Type == ITEM_WING + 43)
						{
							if ((excelWing >> 4) & 1)
							{
								pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
								pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
							}
							else if ((excelWing >> 5) & 1)
							{
								pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
								pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_CURSE;
							}
							else
							{
								pItem->SpecialValue[pItem->SpecialNum] = Option3;
								pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
							}
						}
					}
					else if ((excelWing >> 4) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE;
					}
					else if ((excelWing >> 5) & 1)
					{
						pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
						pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DEFENSE;
					}
					else
					{
						pItem->SpecialValue[pItem->SpecialNum] = Option3;
						pItem->Special[pItem->SpecialNum++] = AT_LIFE_REGENERATION;
					}
					break;
				}
			}
			else
			{
				pItem->SpecialValue[pItem->SpecialNum] = 4 * Option3;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC;
			}
		}
	}

	if (pItem->Type == ITEM_HELPER + 4)
	{
		giPetManager::SetPetItemConvert(pItem, giPetManager::GetPetInfo(pItem));
	}

	if (pItem->Type >= ITEM_SHIELD && pItem->Type < ITEM_WING
		|| pItem->Type >= ITEM_HELPER + 8 && pItem->Type <= ITEM_HELPER + 9
		|| pItem->Type >= ITEM_HELPER + 21 && pItem->Type <= ITEM_HELPER + 24
		|| Weapon02Kind2 == ItemKind2::RING)
	{
		if (((signed int)Attribute2 >> 5) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_LIFE;
		if (((signed int)Attribute2 >> 4) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MANA;
		if (((signed int)Attribute2 >> 3) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_DECREASE_DAMAGE;
		if (((signed int)Attribute2 >> 2) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_REFLECTION_DAMAGE;
		if (((signed int)Attribute2 >> 1) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_BLOCKING_PERCENT;
		if (Attribute2 & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_GAIN_GOLD;
	}

	if (pItem->Type >= ITEM_SWORD && pItem->Type < ITEM_SHIELD
		|| pItem->Type >= ITEM_HELPER + 12 && pItem->Type <= ITEM_HELPER + 13
		|| pItem->Type >= ITEM_HELPER + 25 && pItem->Type <= ITEM_HELPER + 28
		|| Weapon02Kind2 == ItemKind2::PHYSICAL_PENDANT
		|| Weapon02Kind2 == ItemKind2::WIZARD_PENDANT)
	{
		if (((signed int)Attribute2 >> 5) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_EXCELLENT_DAMAGE;

		if (((pItem->Type < ITEM_STAFF || pItem->Type >= ITEM_SHIELD)
			&& pItem->Type != ITEM_HELPER + 12
			&& pItem->Type != ITEM_HELPER + 25
			&& pItem->Type != ITEM_HELPER + 27)
			|| Weapon02Kind2 == ItemKind2::PHYSICAL_PENDANT)
		{
			if (((signed int)Attribute2 >> 4) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = (BYTE)((float)CharacterAttribute->Level / (float)20);
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_LEVEL;
			}
			if (((signed int)Attribute2 >> 3) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 2;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
			}
		}
		else
		{
			if (((signed int)Attribute2 >> 4) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = (BYTE)((float)CharacterAttribute->Level / (float)20);
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC_LEVEL;
			}
			if (((signed int)Attribute2 >> 3) & 1)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 2;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
			}
		}
		if (((signed int)Attribute2 >> 2) & 1)
		{
			pItem->SpecialValue[pItem->SpecialNum] = 7;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
		}
		if (((signed int)Attribute2 >> 1) & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_GAIN_LIFE;
		if (Attribute2 & 1)
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_GAIN_MANA;
	}
	if (pItem->Type == ITEM_HELPER + 20)
	{
		if (Level)
		{
			if (Level == 3)
			{
				pItem->SpecialValue[pItem->SpecialNum] = 10;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
				pItem->SpecialValue[pItem->SpecialNum] = 10;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
				pItem->SpecialValue[pItem->SpecialNum] = 10;
				pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
			}
		}
		else
		{
			pItem->SpecialValue[pItem->SpecialNum] = 10;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
			pItem->SpecialValue[pItem->SpecialNum] = 10;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
			pItem->SpecialValue[pItem->SpecialNum] = 10;
			pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
		}
	}
	if (pItem->Type == ITEM_HELPER + 107)
	{
		pItem->SpecialValue[pItem->SpecialNum] = 15;
		pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_MAGIC_PERCENT;
		pItem->SpecialValue[pItem->SpecialNum] = 15;
		pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_DAMAGE_PERCENT;
		pItem->SpecialValue[pItem->SpecialNum] = 10;
		pItem->Special[pItem->SpecialNum++] = AT_IMPROVE_ATTACK_SPEED;
	}

	pItem->Part = item_info->m_byItemSlot;
}

int ItemValue(ITEM* ip, int goldType)
{
	if (ip->Type == -1)
		return 0;

	Script_Item* p = GMItemMng->find(ip->Type);

	__int64 Gold = 0;

	if (p->iZen != 0)
	{
		Gold = p->iZen;

		if (goldType)
		{
			Gold = Gold / 3;
		}

		if (Gold >= 1000)
			Gold = Gold / 100 * 100;
		else if (Gold >= 100)
			Gold = Gold / 10 * 10;

		return (int)Gold;
	}

	int Type = ip->Type / MAX_ITEM_INDEX;
	int Level = (ip->Level >> 3) & 15;
	bool Excellent = false;
	for (int i = 0; i < ip->SpecialNum; i++)
	{
		switch (ip->Special[i])
		{
		case AT_IMPROVE_LIFE:
		case AT_IMPROVE_MANA:
		case AT_DECREASE_DAMAGE:
		case AT_REFLECTION_DAMAGE:
		case AT_IMPROVE_BLOCKING_PERCENT:
		case AT_IMPROVE_GAIN_GOLD:
		case AT_EXCELLENT_DAMAGE:
		case AT_IMPROVE_DAMAGE_LEVEL:
		case AT_IMPROVE_DAMAGE_PERCENT:
		case AT_IMPROVE_MAGIC_LEVEL:
		case AT_IMPROVE_MAGIC_PERCENT:
		case AT_IMPROVE_ATTACK_SPEED:
		case AT_IMPROVE_GAIN_LIFE:
		case AT_IMPROVE_GAIN_MANA:
			Excellent = true;
			break;
		}
	}
	int Level2 = p->Level + Level * 3;
	if (Excellent)
		Level2 += 25;

	if (ip->Type == ITEM_BOW + 7)
	{
		int sellMoney = 0;

		switch (Level)
		{
		case    0:
			sellMoney = 100;
			break;
		case    1:
			sellMoney = 1400;
			break;
		case    2:
			sellMoney = 2200;
			break;
		case    3:
			sellMoney = 3000;
			break;
		}
		if (p->Durability > 0)
			Gold = (sellMoney * ip->Durability / p->Durability);//+(170*(Level*2));
	}
	else if (ip->Type == ITEM_BOW + 15)
	{
		int sellMoney = 0;

		switch (Level)
		{
		case    0:
			sellMoney = 70;
			break;
		case    1:
			sellMoney = 1200;
			break;
		case    2:
			sellMoney = 2000;
			break;
		case    3:
			sellMoney = 2800;
			break;
		}
		if (p->Durability > 0)
			Gold = (sellMoney * ip->Durability / p->Durability);//+(80*(Level*2));
	}
	else if (COMGEM::isCompiledGem(ip))
	{
		Gold = COMGEM::CalcItemValue(ip);
	}
	else if (ip->Type == ITEM_POTION + 13)
	{
		Gold = 9000000;
	}
	else if (ip->Type == ITEM_POTION + 14)
	{
		Gold = 6000000;
	}
	else if (ip->Type == ITEM_WING + 15)
	{
		Gold = 810000;
	}
	else if (ip->Type == ITEM_POTION + 16)
	{
		Gold = 45000000;
	}
	else if (ip->Type == ITEM_POTION + 22)
	{
		Gold = 36000000;
	}
	else if (ip->Type == ITEM_POTION + 141)
	{
		Gold = 224000 * 3;
	}
	else if (ip->Type == ITEM_POTION + 142)
	{
		Gold = 182000 * 3;
	}
	else if (ip->Type == ITEM_POTION + 143)
	{
		Gold = 157000 * 3;
	}
	else if (ip->Type == ITEM_POTION + 144)
	{
		Gold = 121000 * 3;
	}

	else if (ip->Type == ITEM_HELPER + 14)
	{
		switch (Level)
		{
		case 0: Gold = 180000;  break;
		case 1: Gold = 7500000; break;
		}
	}
	else if (ip->Type == ITEM_HELPER + 3)
	{
		Gold = 960000;
		for (int i = 0; i < ip->SpecialNum; i++)
		{
			switch (ip->Special[i])
			{
			case AT_DAMAGE_ABSORB:
			case AT_IMPROVE_AG_MAX:
			case AT_IMPROVE_ATTACK_SPEED:
				Gold += 300000;
				break;
			}
		}
	}
	else if (ip->Type == ITEM_HELPER + 15)
	{
		Gold = 33000000;
	}
	else if (ip->Type == ITEM_HELPER + 16 || ip->Type == ITEM_HELPER + 17)
	{
		switch (Level)
		{
		case 1: Gold = 10000; break;
		case 2: Gold = 50000; break;
		case 3: Gold = 100000; break;
		case 4: Gold = 300000; break;
		case 5: Gold = 500000; break;
		case 6: Gold = 800000; break;
		case 7: Gold = 1000000; break;
		case 8: Gold = 1200000; break;
		default:
			Gold = 0;
		}
	}
	else if (ip->Type == ITEM_HELPER + 18)
	{
		Gold = (long long)(200000 + 20000 * (Level - 1));
		if (Level == 1)
		{
			Gold = 50000;
		}
		Gold *= 3;
	}
	else if (ip->Type == ITEM_POTION + 28)
	{
		Gold = 200000 * 3;
	}
	else if (ip->Type == ITEM_POTION + 29)
	{
		Gold = (long long)(ip->Durability * 10000) * 3;
	}
	else if (ip->Type == ITEM_POTION + 111)
	{
		Gold = 200000 * 3;
	}
	else if (ip->Type == ITEM_POTION + 110)
	{
		Gold = (long long)(ip->Durability * 10000) * 3;
	}
	else if (ITEM_POTION + 102 == ip->Type)
	{
		Gold = 10000 * 3;
	}
	else if (ITEM_POTION + 109 == ip->Type)
	{
		Gold = 10000 * 3;
	}
	else if (ITEM_POTION + 101 == ip->Type || ITEM_POTION + 103 <= ip->Type && ip->Type <= ITEM_POTION + 108)
	{
		Gold = (long long)((long long)ip->Durability * 10000) * 3;
	}
	else if (ip->Type == ITEM_HELPER + 29)
	{
		Gold = 5000;
	}
	else if (ip->Type == ITEM_POTION + 21)
	{
		if (Level == 0)
			Gold = 9000;
		else if (Level == 1)
			Gold = 9000;
		else if (Level == 2)
			Gold = 3000 * 3;
		else if (Level == 3)
		{
			Gold = (long long)ip->Durability * 3900;
		}
	}
	else if (ip->Type == ITEM_POTION + 17)
	{
		int iValue[8] = { 30000, 10000, 50000, 100000, 300000, 500000, 800000, 1000000 };
		Gold = iValue[min(max(0, Level), 7)];
	}
	else if (ip->Type == ITEM_POTION + 18)
	{
		int iValue[8] = { 30000, 15000, 75000, 150000, 450000, 750000, 1200000, 1500000 };
		Gold = iValue[min(max(0, Level), 7)];
	}
	else if (ip->Type == ITEM_POTION + 19)
	{
		int iValue[8] = { 120000, 60000, 84000, 120000, 180000, 240000, 300000, 180000 };
		Gold = iValue[min(max(0, Level), 7)];
	}
	else if (ip->Type == ITEM_HELPER + 49 || ip->Type == ITEM_HELPER + 50 || ip->Type == ITEM_HELPER + 51)
	{
		switch (Level)
		{
		case 1: Gold = 500000; break;
		case 2: Gold = 600000; break;
		case 3: Gold = 800000; break;
		case 4: Gold = 1000000; break;
		case 5: Gold = 1200000; break;
		case 6: Gold = 1400000; break;
		default:
			Gold = 3000 * 3;
		}
	}
	else if (ip->Type == ITEM_POTION + 23 || ip->Type == ITEM_POTION + 24 || ip->Type == ITEM_POTION + 25 || ip->Type == ITEM_POTION + 26
		|| ip->Type == ITEM_POTION + 65 || ip->Type == ITEM_POTION + 66 || ip->Type == ITEM_POTION + 67 || ip->Type == ITEM_POTION + 68
		)
	{
		Gold = 9000;
	}
	else if (ip->Type == ITEM_POTION + 9 && Level == 1)
	{
		Gold = 1000;
	}
	else if (ip->Type == ITEM_POTION + 20)
	{
		Gold = 900;
	}
	else if (ip->Type == ITEM_POTION + 51)
	{
		Gold = 200000;
	}
	else if (ip->Type == ITEM_POTION + 63)
	{
		Gold = 200000;
	}
	else if (ip->Type == ITEM_POTION + 52)
	{
		Gold = 33 * 3;
	}
	else if (ip->Type == ITEM_POTION + 10)
	{
		Gold = 750;
	}
	else if (ip->Type == ITEM_POTION + 31)
	{
		Gold = 60000000;
	}
	else if (ip->Type == ITEM_POTION + 7)
	{
		switch (Level)
		{
		case 0: Gold = (long long)900000 * ip->Durability; break;
		case 1: Gold = (long long)450000 * ip->Durability; break;
		}
	}
	else if (ip->Type == ITEM_HELPER + 7)
	{
		switch (Level)
		{
		case 0: Gold = 1500000; break;
		case 1: Gold = 1200000; break;
		}
	}
	else if (ip->Type == ITEM_HELPER + 11)
	{
		switch (Level)
		{
		case 0: Gold = 100000; break;
		case 1: Gold = 2400000; break;
		}
	}
	else if (ip->Type >= ITEM_POTION + 35 && ip->Type <= ITEM_POTION + 37)
	{
		switch (ip->Type)
		{
		case ITEM_POTION + 35:
			Gold = 2000;
			break;
		case ITEM_POTION + 36:
			Gold = 4000;
			break;
		case ITEM_POTION + 37:
			Gold = 6000;
			break;
		}
		Gold *= ip->Durability;
	}
	else if (ip->Type >= ITEM_POTION + 38 && ip->Type <= ITEM_POTION + 40)
	{
		switch (ip->Type)
		{
		case ITEM_POTION + 38:
			Gold = 2500;
			break;
		case ITEM_POTION + 39:
			Gold = 5000;
			break;
		case ITEM_POTION + 40:
			Gold = 7500;
			break;
		}
		Gold *= ip->Durability;
	}
	else if (ip->Type == ITEM_POTION + 100)
	{
		Gold = 100 * 3;
		Gold *= ip->Durability;
	}
	else if (ip->Type == ITEM_POTION + 90 || ip->Type == ITEM_POTION + 85 || ip->Type == ITEM_POTION + 86 || ip->Type == ITEM_POTION + 87)
	{
		Gold = 300;
		Gold *= ip->Durability;
	}

	else if (p->Value > 0)
	{
		Gold = p->Value * p->Value * 10 / 12;

		if (ip->Type == ITEM_POTION + 3 || ip->Type == ITEM_POTION + 6)
		{
			Gold = 1500;
		}

		if (ip->Type >= ITEM_POTION && ip->Type <= ITEM_POTION + 8)
		{
			if (Level > 0)
				Gold *= (__int64)pow((double)2, (double)Level);

			Gold = Gold / 10 * 10;
			Gold *= (__int64)ip->Durability;

			if (goldType)
			{
				Gold = Gold / 3;
				Gold = Gold / 10 * 10;
			}
			return (int)Gold;
		}
	}
	else if (ip->Type == ITEM_HELPER + 20)
	{
		if (Level == 0)
			Gold = 3000;
		else if (Level == 1)
			Gold = 0;
	}
	else if (ip->Type == ITEM_HELPER + 52 || ip->Type == ITEM_HELPER + 53)
	{
		Gold = 3000000;
	}
	else if (ip->Type == ITEM_HELPER + 31)
	{
		switch (Level)
		{
		case 0: Gold = 10000000 * 3; break;
		case 1: Gold = 5000000 * 3;  break;
		}
	}
	else if (((Type == 12 && (ip->Type > ITEM_WING + 6
		&& !(ip->Type >= ITEM_WING + 36 && ip->Type <= ITEM_WING + 43)
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		&& (ip->Type != ITEM_WING + 50)
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		)) || Type == 13 || Type == 15))
	{
		Gold = (long long)100 + Level2 * Level2 * Level2;

		for (int i = 0; i < ip->SpecialNum; i++)
		{
			switch (ip->Special[i])
			{
			case AT_LIFE_REGENERATION:
				Gold += Gold * ip->SpecialValue[i];
				break;
			}
		}
	}
	else if (ip->Type == ITEM_POTION + 53)
	{
		Gold = 0;
	}
	else
	{
		switch (Level)
		{
		case 5:Level2 += 4; break;
		case 6:Level2 += 10; break;
		case 7:Level2 += 25; break;
		case 8:Level2 += 45; break;
		case 9:Level2 += 65; break;
		case 10:Level2 += 95; break;
		case 11:Level2 += 135; break;
		case 12:Level2 += 185; break;
		case 13:Level2 += 245; break;
		case 14:Level2 += 305; break;
		case 15:Level2 += 365; break;
		}
		if ((Type == 12 && ip->Type <= ITEM_WING + 6) || ip->Type == ITEM_HELPER + 30 || (ip->Type >= ITEM_WING + 36 && ip->Type <= ITEM_WING + 43)
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
			|| (ip->Type == ITEM_WING + 50)
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
			)
		{
			Gold = (long long)(40000000 + ((40 + Level2) * Level2 * Level2 * 11));
		}
		else
		{
			Gold = (long long)100 + (40 + Level2) * Level2 * Level2 / 8;
		}
		if (Type >= 0 && Type <= 6)
		{
			if (!p->TwoHand)
				Gold = Gold * 80 / 100;
		}
		for (int i = 0; i < ip->SpecialNum; i++)
		{
			switch (ip->Special[i])
			{
			case AT_SKILL_BLOCKING:
			case AT_SKILL_SWORD1:
			case AT_SKILL_SWORD2:
			case AT_SKILL_SWORD3:
			case AT_SKILL_SWORD4:
			case AT_SKILL_SWORD5:
			case Skill_Slash_Strengthener:
			case AT_SKILL_CROSSBOW:
			case AT_SKILL_RECOVER:
			case AT_SKILL_GAOTIC:
			case AT_SKILL_MULTI_SHOT:
			case Skill_Chaotic_Diseier_Strengthener:
			case Skill_Killing_Blow_Strengthener:
			case Skill_Beast_Uppercut_Strengthener:
			case Skill_Killing_Blow_Mastery:
			case Skill_Beast_Uppercut_Mastery:
			case Skill_Triple_Shot_Strengthener:
			case Skill_Power_Slash_Strengthener:
			case AT_SKILL_ICE_BLADE:
			case AT_SKILL_THRUST:
			case AT_SKILL_STAMP:
				Gold += (long long)((double)Gold * 1.5f);
				break;
			case AT_IMPROVE_DAMAGE:
			case AT_IMPROVE_MAGIC:
			case AT_IMPROVE_CURSE:
			case AT_IMPROVE_DEFENSE:
			case AT_LIFE_REGENERATION:
				if ((Type == 12 && ip->Type <= ITEM_WING + 6)

					|| (ip->Type >= ITEM_WING + 36 && ip->Type <= ITEM_WING + 43)
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
					|| (ip->Type >= ITEM_WING + 49 && ip->Type <= ITEM_WING + 50)
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
					)    //  ³¯°³.
				{
					int iOption = ip->SpecialValue[i];
					if (AT_LIFE_REGENERATION == ip->Special[i])
					{
						iOption *= 4;
					}
					switch (iOption)
					{
					case 4:Gold += (__int64)((double)Gold * 6 / 10); break;
					case 8:Gold += (__int64)((double)Gold * 14 / 10); break;
					case 12:Gold += (__int64)((double)Gold * 28 / 10); break;
					case 16:Gold += (__int64)((double)Gold * 56 / 10); break;
					}
				}
				else
				{
					switch (ip->SpecialValue[i])
					{
					case 4:Gold += (__int64)((double)Gold * 6 / 10); break;
					case 8:Gold += (__int64)((double)Gold * 14 / 10); break;
					case 12:Gold += (__int64)((double)Gold * 28 / 10); break;
					case 16:Gold += (__int64)((double)Gold * 56 / 10); break;
					}
				}
				break;
			case AT_IMPROVE_BLOCKING:
				switch (ip->SpecialValue[i])
				{
				case 5:Gold += (__int64)((double)Gold * 6 / 10); break;
				case 10:Gold += (__int64)((double)Gold * 14 / 10); break;
				case 15:Gold += (__int64)((double)Gold * 28 / 10); break;
				case 20:Gold += (__int64)((double)Gold * 56 / 10); break;
				}
				break;
			case AT_LUCK:
				Gold += (__int64)((double)Gold * 25 / 100);
				break;
			case AT_IMPROVE_LIFE:
			case AT_IMPROVE_MANA:
			case AT_DECREASE_DAMAGE:
			case AT_REFLECTION_DAMAGE:
			case AT_IMPROVE_BLOCKING_PERCENT:
			case AT_IMPROVE_GAIN_GOLD:
			case AT_EXCELLENT_DAMAGE:
			case AT_IMPROVE_DAMAGE_LEVEL:
			case AT_IMPROVE_DAMAGE_PERCENT:
			case AT_IMPROVE_MAGIC_LEVEL:
			case AT_IMPROVE_MAGIC_PERCENT:
			case AT_IMPROVE_GAIN_LIFE:
			case AT_IMPROVE_GAIN_MANA:
			case AT_IMPROVE_ATTACK_SPEED:
				Gold += Gold;
				break;
			case AT_IMPROVE_EVADE:
				Gold += Gold;
				break;
			case AT_IMPROVE_HP_MAX:
			case AT_IMPROVE_MP_MAX:
			case AT_ONE_PERCENT_DAMAGE:
			case AT_IMPROVE_AG_MAX:
			case AT_DAMAGE_ABSORB:
			case AT_DAMAGE_REFLECTION:
			case AT_RECOVER_FULL_LIFE:
			case AT_RECOVER_FULL_MANA:
				Gold += (__int64)((double)Gold * 25 / 100);
				break;
			}
		}
		Gold += g_SocketItemMgr.CalcSocketBonusItemValue(ip, Gold);
	}
	Gold = min(Gold, 3000000000);

	if (goldType == 2)
	{
		if (Gold >= 1000)
		{
			Gold = (Gold / 100) * 100;
		}
		else if (Gold >= 100)
		{
			Gold = (Gold / 10) * 10;
		}
	}

	if (goldType)
	{
		Gold = Gold / 3;
	}

	if (ip->Type == ITEM_HELPER + 32)
		Gold = (long long)ip->Durability * 50;
	else if (ip->Type == ITEM_HELPER + 33)
		Gold = (long long)ip->Durability * 100;
	else if (ip->Type == ITEM_HELPER + 34)
		Gold = (long long)ip->Durability * 1000;
	else if (ip->Type == ITEM_HELPER + 35)
		Gold = (long long)ip->Durability * 10000;
	else if (ip->Type == ITEM_HELPER + 36)
		Gold = 30000;
	else if (ip->Type == ITEM_HELPER + 37)
		Gold = 50000;

	if (ip->Type >= ITEM_POTION + 45 && ip->Type <= ITEM_POTION + 50)
	{
		Gold = (long long)ip->Durability * 50;
	}

	if (ip->Type == ITEM_HELPER + 71 || ip->Type == ITEM_HELPER + 72 || ip->Type == ITEM_HELPER + 73 || ip->Type == ITEM_HELPER + 74 || ip->Type == ITEM_HELPER + 75)
	{
		Gold = 2000000;
	}

	if ((ip->Type == ITEM_HELPER + 4) || (ip->Type == ITEM_HELPER + 5))
	{
		PET_INFO* pPetInfo = giPetManager::GetPetInfo(ip);

		if (pPetInfo->m_dwPetType == PET_TYPE_NONE)
			return -1;

		Gold = giPetManager::GetPetItemValue(pPetInfo);
	}

	switch (ip->Type)
	{
	case ITEM_POTION + 112:
	case ITEM_POTION + 113:
	case ITEM_POTION + 121:
	case ITEM_POTION + 122:
	case ITEM_POTION + 123:
	case ITEM_POTION + 124:
	case ITEM_HELPER + 80:
	case ITEM_HELPER + 76:
	case ITEM_HELPER + 64:
	case ITEM_HELPER + 65:
	case ITEM_HELPER + 109:
	case ITEM_HELPER + 110:
	case ITEM_HELPER + 111:
	case ITEM_HELPER + 112:
	case ITEM_HELPER + 113:
	case ITEM_HELPER + 114:
	case ITEM_HELPER + 115:
		Gold = 1000;
		break;
	case ITEM_HELPER + 122:
	case ITEM_HELPER + 123:
		Gold = 2000;
		break;
	case ITEM_WING + 130:
	case ITEM_WING + 131:
	case ITEM_WING + 132:
	case ITEM_WING + 133:
	case ITEM_WING + 134:
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
	case ITEM_WING + 135:
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		Gold = 80;
		break;
	}

	if (ip->Type == ITEM_HELPER + 10 || ip->Type == ITEM_HELPER + 20 || ip->Type == ITEM_HELPER + 29)
		goto EXIT_CALCULATE;
	if (ip->Type == ITEM_BOW + 7 || ip->Type == ITEM_BOW + 15 || ip->Type >= ITEM_POTION)
		goto EXIT_CALCULATE;
	if (ip->Type >= ITEM_WING + 7 && ip->Type <= ITEM_WING + 19)
		goto EXIT_CALCULATE;
	if ((ip->Type >= ITEM_HELPER + 14 && ip->Type <= ITEM_HELPER + 19) || ip->Type == ITEM_POTION + 21)
		goto EXIT_CALCULATE;
	if (ip->Type == ITEM_POTION + 7 || ip->Type == ITEM_HELPER + 7 || ip->Type == ITEM_HELPER + 11)
		goto EXIT_CALCULATE;
	if ((ip->Type >= ITEM_HELPER + 49) && (ip->Type <= ITEM_HELPER + 51))
		goto EXIT_CALCULATE;

	switch (ip->Type)
	{
	case ITEM_POTION + 112:	goto EXIT_CALCULATE;
		// MODEL_POTION+113
	case ITEM_POTION + 113:	goto EXIT_CALCULATE;
	case ITEM_POTION + 121:	goto EXIT_CALCULATE;
	case ITEM_POTION + 122:	goto EXIT_CALCULATE;
	case ITEM_POTION + 123:	goto EXIT_CALCULATE;
	case ITEM_POTION + 124:	goto EXIT_CALCULATE;
	case ITEM_WING + 130:
	case ITEM_WING + 131:
	case ITEM_WING + 132:
	case ITEM_WING + 133:
	case ITEM_WING + 134:
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
	case ITEM_WING + 135:
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 122:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 123:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 80:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 76:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 64:
	case ITEM_HELPER + 65:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 109:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 110:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 111:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 112:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 113:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 114:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 115:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 128:
	case ITEM_HELPER + 129:
	case ITEM_HELPER + 134:
		goto EXIT_CALCULATE;
	case ITEM_HELPER + 130:
	case ITEM_HELPER + 131:
	case ITEM_HELPER + 132:
	case ITEM_HELPER + 133:
		goto EXIT_CALCULATE;
	}

	if (goldType == 1 && !(ip->Type >= ITEM_HELPER + 32 && ip->Type <= ITEM_HELPER + 37))
	{
		//        char Text[100];
				//int repairGold = ConvertRepairGold(Gold,ip->Durability, p->Durability, ip->Type, Text);
		DWORD maxDurability = calcMaxDurability(ip, p, Level);
		float persent = 1.f - ((float)ip->Durability / (float)maxDurability);
		DWORD repairGold = (DWORD)(Gold * 0.6f * persent);

		if (ip->Type == ITEM_HELPER + 31)
			repairGold = 0;

		Gold -= repairGold;
	}
EXIT_CALCULATE:

	if (Gold >= 1000)
	{
		Gold = (Gold / 100) * 100;
	}
	else if (Gold >= 100)
	{
		Gold = (Gold / 10) * 10;
	}
	if (Check_LuckyItem(ip->Type))
		Gold = 0;

	return (int)Gold;
}

bool IsRequireEquipItem(ITEM* pItem)
{
	if (pItem == NULL) {
		return false;
	}

	Script_Item* pItemAttr = GMItemMng->find(pItem->Type);

	bool bEquipable = false;

	BYTE byFirstClass = Hero->GetBaseClass();

	if (pItemAttr->RequireClass[byFirstClass])
	{
		bEquipable = true;
	}
	else if (byFirstClass == CLASS_DARK && (pItemAttr->RequireClass[CLASS_WIZARD] && pItemAttr->RequireClass[CLASS_KNIGHT]))
	{
		bEquipable = true;
	}

	BYTE byStepClass = Hero->GetStepClass();

	if (pItemAttr->RequireClass[byFirstClass] > byStepClass)
	{
		return false;
	}

	if (bEquipable == false)
		return false;

	WORD wStrength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;
	WORD wDexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
	WORD wEnergy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
	WORD wVitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;
	WORD wCharisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;
	WORD wLevel = CharacterAttribute->Level;

	int iItemLevel = (pItem->Level >> 3) & 15;

	int iDecNeedStrength = 0, iDecNeedDex = 0;

	extern JewelHarmonyInfo* g_pUIJewelHarmonyinfo;
	if (iItemLevel >= pItem->Jewel_Of_Harmony_OptionLevel)
	{
		StrengthenCapability SC;
		g_pUIJewelHarmonyinfo->GetStrengthenCapability(&SC, pItem, 0);

		if (SC.SI_isNB)
		{
			iDecNeedStrength = SC.SI_NB.SI_force;
			iDecNeedDex = SC.SI_NB.SI_activity;
		}
	}

	if (pItem->RequireStrength - iDecNeedStrength > wStrength)
		return false;
	if (pItem->RequireDexterity - iDecNeedDex > wDexterity)
		return false;
	if (pItem->RequireEnergy > wEnergy)
		return false;
	if (pItem->RequireVitality > wVitality)
		return false;
	if (pItem->RequireCharisma > wCharisma)
		return false;
	if (pItem->RequireLevel > wLevel)
		return false;

	if (pItem->Type == ITEM_HELPER + 5)
	{
		PET_INFO* pPetInfo = giPetManager::GetPetInfo(pItem);
		WORD wRequireCharisma = static_cast<WORD>((185 + pPetInfo->m_wLevel * 15) & 0xFFFF);
		if (wRequireCharisma > wCharisma)
		{
			return false;
		}
	}

	return bEquipable;
}

void PlusSpecial(DWORD* Value, int Special, ITEM* Item)
{
	if (Item->Type == -1)
	{
		return;
	}

	if (IsRequireEquipItem(Item))
	{
		for (int i = 0; i < Item->SpecialNum; i++)
		{
			if (Item->Special[i] == Special && Item->Durability != 0)
				*Value += Item->SpecialValue[i];
		}
	}
}

void PlusSpecialPercent(DWORD* Value, int Special, ITEM* Item, WORD Percent)
{
	if (Item->Type == -1) return;

	if (IsRequireEquipItem(Item))
	{
		for (int i = 0; i < Item->SpecialNum; i++)
		{
			if (Item->Special[i] == Special && Item->Durability != 0)
				*Value += *Value * Percent / 100;
		}
	}
}

void PlusSpecialPercent2(DWORD* Value, int Special, ITEM* Item)
{
	if (Item->Type == -1) return;

	if (IsRequireEquipItem(Item))
	{
		for (int i = 0; i < Item->SpecialNum; i++)
		{
			if (Item->Special[i] == Special && Item->Durability != 0)
				*Value += (unsigned short)(*Value * (Item->SpecialValue[i] / 100.f));
		}
	}
}

WORD ItemDefense(ITEM* Item)
{
	if (Item->Type == -1)
		return 0;

	DWORD Defense = Item->Defense;
	PlusSpecial(&Defense, AT_IMPROVE_DEFENSE, Item);

	return Defense;
}

WORD ItemMagicDefense(ITEM* Item)
{
	if (Item->Type == -1)
		return 0;

	return Item->MagicDefense;
}

WORD ItemWalkSpeed(ITEM* Item)
{
	if (Item->Type == -1)
		return 0;

	return Item->WalkSpeed;
}

extern int HeroKey;
int EditMonsterNumber = 0;
int MonsterKey = 0;

void OpenMonsterScript(char* FileName)
{
	if ((SMDFile = fopen(FileName, "rb")) == NULL)
		return;
	SMDToken Token;
	while (true)
	{
		Token = (*GetToken)();//¹øÈ£
		if (Token == END)
			break;
		if (Token == NAME && strcmp("end", TokenString) == NULL)
			break;
		MONSTER_SCRIPT* m = &MonsterScript[EditMonsterNumber++];
		m->Type = (int)TokenNumber;
		Token = (*GetToken)();
		Token = (*GetToken)();
		strcpy(m->Name, TokenString);
	}
	fclose(SMDFile);
}

char* getMonsterName(int type)
{
	for (int i = 0; i < MAX_MONSTER; ++i)
	{
		if (MonsterScript[i].Type == type)
		{
			return MonsterScript[i].Name;
		}
	}
	return NULL;
}

void MonsterConvert(MONSTER* m, int Level)
{
	MONSTER_SCRIPT* p = &MonsterScript[m->Type];
	MONSTER_ATTRIBUTE* c = &p->Attribute;
	m->Level = Level;
	m->Attribute.AttackSpeed = c->AttackSpeed;
	m->Attribute.AttackDamageMin = c->AttackDamageMin / 2 + c->AttackDamageMin / 2 * (m->Level) / 9;
	m->Attribute.AttackDamageMax = c->AttackDamageMax / 2 + c->AttackDamageMax / 2 * (m->Level) / 9;
	m->Attribute.Defense = c->Defense / 2 + c->Defense / 2 * (m->Level) / 9;
	m->Attribute.AttackRating = c->AttackRating / 2 + c->AttackRating / 2 * (m->Level) / 9;
	m->Attribute.SuccessfulBlocking = c->SuccessfulBlocking / 2 + c->SuccessfulBlocking / 2 * (m->Level) / 9;
}

void CreateClassAttribute(int Class, int Strength, int Dexterity, int Vitality, int Energy, int Life, int Mana, int LevelLife, int LevelMana, int VitalityToLife, int EnergyToMana)
{
	CLASS_ATTRIBUTE* c = &ClassAttribute[Class];
	c->Strength = Strength;
	c->Dexterity = Dexterity;
	c->Vitality = Vitality;
	c->Energy = Energy;
	c->Life = Life;
	c->Mana = Mana;
	c->LevelLife = LevelLife;
	c->LevelMana = LevelMana;
	c->VitalityToLife = VitalityToLife;
	c->EnergyToMana = EnergyToMana;
	c->Shield = 0;
}

void CreateClassAttributes()
{
	CreateClassAttribute(0, 18, 18, 15, 30, 80, 60, 1, 2, 1, 2);
	CreateClassAttribute(1, 28, 20, 25, 10, 110, 20, 2, 1, 2, 1);
	CreateClassAttribute(2, 50, 50, 50, 30, 110, 30, 110, 30, 6, 3);
	CreateClassAttribute(3, 30, 30, 30, 30, 120, 80, 1, 1, 2, 2);
	CreateClassAttribute(4, 30, 30, 30, 30, 120, 80, 1, 1, 2, 2);
	CreateClassAttribute(5, 50, 50, 50, 30, 110, 30, 110, 30, 6, 3);
#ifdef PBG_ADD_NEWCHAR_MONK
	CreateClassAttribute(6, 32, 27, 25, 20, 100, 40, 1, 3, 1, 1);
#endif //PBG_ADD_NEWCHAR_MONK
}

float CalcDurabilityPercent(BYTE dur, BYTE maxDur, int Level, int option, int ExtOption)
{
	int maxDurability = maxDur;
	for (int i = 0; i < ((Level >> 3) & 15); i++)
	{
		if (i >= 4)
		{
			maxDurability += 2;
		}
		else
		{
			maxDurability++;
		}
	}

	if ((ExtOption % 0x04) == EXT_A_SET_OPTION || (ExtOption % 0x04) == EXT_B_SET_OPTION)
	{
		maxDurability += 20;
	}
	else if ((option & 63) > 0)
	{
		maxDurability += 15;
	}

	float   durP = 1.f - (dur / (float)maxDurability);

	if (durP > 0.8f)
	{
		return  0.5f;
	}
	else if (durP > 0.7f)
	{
		return  0.3f;
	}
	else if (durP > 0.5f)
	{
		return  0.2f;
	}
	else
	{
		return  0.f;
	}
	return 0.f;
}

void CHARACTER_MACHINE::Init()
{
	CreateClassAttributes();

	Character.MaxCharacterLevel = MAX_CHARACTER_LEVEL;
	Character.PenaltyTimePK = 0;

	for (int i = 0; i < MAX_NEW_EQUIPMENT; i++)
	{
		Equipment[i].Type = -1;
	}

	Gold = 0;
	StorageGold = 0;
	PacketSerial = 0;
	AccountLevel = 0;
	InfinityArrowAdditionalMana = 0;


	PowerLevelUp = false;
	ATTKPowerValue = 0;

#ifdef SHUTDOWN_CHARACTER_INFO
	Character.Work_PhysiDamageMin = 0;
	Character.Work_PhysiDamageMax = 0;
	Character.Work_MagicDamageMin = 0;
	Character.Work_MagicDamageMax = 0;
	Character.Work_CurseDamageMin = 0;
	Character.Work_CurseDamageMax = 0;
	Character.Work_MulPhysiDamage = 0;
	Character.Work_DivPhysiDamage = 0;
	Character.Work_MulMagicDamage = 0;
	Character.Work_DivMagicDamage = 0;
	Character.Work_MulCurseDamage = 0;
	Character.Work_DivCurseDamage = 0;
	Character.Work_MagicDamageRate = 0;
	Character.Work_CurseDamageRate = 0;
	Character.Work_PhysiSpeed = 0;
	Character.Work_MagicSpeed = 0;
	Character.Work_AttackSuccessRate = 0;
	Character.Work_AttackSuccessRatePvP = 0;
	Character.Work_Defense = 0;
	Character.Work_DefenseSuccessRate = 0;
	Character.Work_DefenseSuccessRatePvP = 0;
	Character.Work_DamageMultiplier = 0;
	Character.Work_RFDamageMultiplierA = 0;
	Character.Work_RFDamageMultiplierB = 0;
	Character.Work_RFDamageMultiplierC = 0;
	Character.Work_DarkSpiritAttackDamageMin = 0;
	Character.Work_DarkSpiritAttackDamageMax = 0;
	Character.Work_DarkSpiritAttackSpeed = 0;
	Character.Work_DarkSpiritAttackSuccessRate = 0;
	Character.Work_SkillMagicDamage = 0;
	Character.Work_SkillPhysiDamage = 0;
#endif // ATTRIBUTE_CHARACTER_INFO
}

void CHARACTER_MACHINE::InitAddValue()
{
	Character.AddStrength = 0;
	Character.AddDexterity = 0;
	Character.AddVitality = 0;
	Character.AddEnergy = 0;
	Character.AddCharisma = 0;
	Character.AddLifeMax = 0;
	Character.AddManaMax = 0;
}

void CHARACTER_MACHINE::SetCharacter(BYTE Class)
{
	CLASS_ATTRIBUTE* c = &ClassAttribute[Class];
	Character.Class = Class;
	Character.Level = 1;
	Character.Strength = c->Strength;
	Character.Dexterity = c->Dexterity;
	Character.Vitality = c->Vitality;
	Character.Energy = c->Energy;
	Character.Life = c->Life;
	Character.Mana = c->Mana;
	Character.LifeMax = c->Life;
	Character.ManaMax = c->Mana;
	Character.Shield = c->Shield;
	Character.ShieldMax = c->Shield;

	InitAddValue();

	for (int j = 0; j < MAX_SKILLS; j++)
	{
		Character.Skill[j] = 0;
	}
}

void CHARACTER_MACHINE::InputEnemyAttribute(MONSTER* e)
{
	memcpy(&Enemy, e, sizeof(MONSTER));
}

void CHARACTER_MACHINE::CalculateDamage()
{
	DWORD    DamageMin, DamageMax;

	ITEM* Right = &Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* Left = &Equipment[EQUIPMENT_WEAPON_LEFT];
	ITEM* Amulet = &Equipment[EQUIPMENT_AMULET];
	ITEM* RRing = &Equipment[EQUIPMENT_RING_RIGHT];
	ITEM* LRing = &Equipment[EQUIPMENT_RING_LEFT];

	DWORD Strength, Dexterity, Energy;

	Strength = Character.Strength + Character.AddStrength;
	Dexterity = Character.Dexterity + Character.AddDexterity;
	Energy = Character.Energy + Character.AddEnergy;
#ifdef PBG_ADD_NEWCHAR_MONK
	DWORD Vitality = Character.Vitality + Character.AddVitality;
#endif //PBG_ADD_NEWCHAR_MONK

	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	if (((gCharacterManager.GetEquipedBowType(Right) == BOWTYPE_BOW) && (Right->Durability != 0)) || ((gCharacterManager.GetEquipedBowType(Right) == BOWTYPE_CROSSBOW) && (Right->Durability != 0)))
	{
		Character.AttackDamageMinRight = Dexterity / 7 + Strength / 14;
		Character.AttackDamageMaxRight = Dexterity / 4 + Strength / 8;
		Character.AttackDamageMinLeft = Dexterity / 7 + Strength / 14;
		Character.AttackDamageMaxLeft = Dexterity / 4 + Strength / 8;
	}
	else
	{
		switch (CharacterClass)
		{
		case CLASS_ELF:
			Character.AttackDamageMinRight = (Strength + Dexterity) / 7;
			Character.AttackDamageMaxRight = (Strength + Dexterity) / 4;
			Character.AttackDamageMinLeft = (Strength + Dexterity) / 7;
			Character.AttackDamageMaxLeft = (Strength + Dexterity) / 4;
			break;

		case CLASS_KNIGHT:
		{
			int minValue = 7;
			int maxValue = 4;

			minValue = 6; maxValue = 4;

			Character.AttackDamageMinRight = Strength / minValue;
			Character.AttackDamageMaxRight = Strength / maxValue;
			Character.AttackDamageMinLeft = Strength / minValue;
			Character.AttackDamageMaxLeft = Strength / maxValue;
		}
		break;

		case CLASS_DARK:
		{
			int minValue[2] = { 7, 10 };
			int maxValue[2] = { 4, 5 };

			minValue[0] = 6; minValue[1] = 12;
			maxValue[0] = 4; maxValue[1] = 9;

			Character.AttackDamageMinRight = (Strength / minValue[0]) + Energy / minValue[1];
			Character.AttackDamageMaxRight = (Strength / maxValue[0]) + Energy / maxValue[1];
			Character.AttackDamageMinLeft = (Strength / minValue[0]) + Energy / minValue[1];
			Character.AttackDamageMaxLeft = (Strength / maxValue[0]) + Energy / maxValue[1];
		}
		break;
		case CLASS_DARK_LORD:
			Character.AttackDamageMinRight = Strength / 7 + Energy / 14;
			Character.AttackDamageMaxRight = Strength / 5 + Energy / 10;
			Character.AttackDamageMinLeft = Strength / 7 + Energy / 14;
			Character.AttackDamageMaxLeft = Strength / 5 + Energy / 10;
			break;

		case CLASS_SUMMONER:
			Character.AttackDamageMinRight = (Strength + Dexterity) / 7;
			Character.AttackDamageMaxRight = (Strength + Dexterity) / 4;
			Character.AttackDamageMinLeft = (Strength + Dexterity) / 7;
			Character.AttackDamageMaxLeft = (Strength + Dexterity) / 4;
			break;
#ifdef PBG_ADD_NEWCHAR_MONK
		case CLASS_RAGEFIGHTER:
		{
			Character.AttackDamageMinRight = Strength / 7 + Vitality / 15;
			Character.AttackDamageMaxRight = Strength / 5 + Vitality / 12;
			Character.AttackDamageMinLeft = Strength / 7 + Vitality / 15;
			Character.AttackDamageMaxLeft = Strength / 5 + Vitality / 12;
		}
		break;
#endif //PBG_ADD_NEWCHAR_MONK
		default:
			Character.AttackDamageMinRight = Strength / 8;
			Character.AttackDamageMaxRight = Strength / 4;
			Character.AttackDamageMinLeft = Strength / 8;
			Character.AttackDamageMaxLeft = Strength / 4;
			break;
		}
	}

	if (Equipment[EQUIPMENT_WING].Type != -1)
	{
		Script_Item* p = GMItemMng->find(Equipment[EQUIPMENT_WING].Type);

		if (p->Durability != 0)
		{
			float   percent = CalcDurabilityPercent(Equipment[EQUIPMENT_WING].Durability, p->Durability, Equipment[EQUIPMENT_WING].Level, 0, 0);//Equipment[EQUIPMENT_WING].Option1);

			DamageMin = 0; DamageMax = 0;
			PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, &Equipment[EQUIPMENT_WING]);
			PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, &Equipment[EQUIPMENT_WING]);

			DamageMin = DamageMin - (WORD)(DamageMin * percent);
			DamageMax = DamageMax - (WORD)(DamageMax * percent);

			Character.AttackDamageMinRight += DamageMin;
			Character.AttackDamageMaxRight += DamageMax;
			Character.AttackDamageMinLeft += DamageMin;
			Character.AttackDamageMaxLeft += DamageMax;
		}
	}

	if (Right->Type != -1 && Right->Durability != 0)
	{
		Script_Item* p = GMItemMng->find(Right->Type);
		float   percent = CalcDurabilityPercent(Right->Durability, p->Durability, Right->Level, Right->Option1, Right->ExtOption);

		DamageMin = Right->DamageMin;
		DamageMax = Right->DamageMax;

		PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, Right);
		PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, Right);

		DamageMin = DamageMin - (WORD)(DamageMin * percent);
		DamageMax = DamageMax - (WORD)(DamageMax * percent);

		if (Right->Type >= ITEM_STAFF && Right->Type <= ITEM_STAFF + MAX_ITEM_INDEX)
		{
			Character.AttackDamageMinLeft += (WORD)(DamageMin);
			Character.AttackDamageMaxLeft += (WORD)(DamageMax);
		}
		else
		{
			Character.AttackDamageMinRight += DamageMin;
			Character.AttackDamageMaxRight += DamageMax;
		}

		PlusSpecial(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_LEVEL, Right);
		PlusSpecial(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_LEVEL, Right);
		PlusSpecialPercent(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_PERCENT, Right, 2);
		PlusSpecialPercent(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_PERCENT, Right, 2);
	}

	if (Left->Type != -1 && Left->Durability != 0)
	{
		Script_Item* p = GMItemMng->find(Left->Type);
		float   percent = CalcDurabilityPercent(Left->Durability, p->Durability, Left->Level, Left->Option1, Left->ExtOption);

		DamageMin = Left->DamageMin;
		DamageMax = Left->DamageMax;

		PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, Left);
		PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, Left);

		DamageMin = DamageMin - (WORD)(DamageMin * percent);
		DamageMax = DamageMax - (WORD)(DamageMax * percent);

		if (Left->Type >= ITEM_STAFF && Left->Type <= ITEM_STAFF + MAX_ITEM_INDEX)
		{
			Character.AttackDamageMinLeft += (WORD)(DamageMin);
			Character.AttackDamageMaxLeft += (WORD)(DamageMax);
		}
		else
		{
			Character.AttackDamageMinLeft += DamageMin;
			Character.AttackDamageMaxLeft += DamageMax;
		}

		PlusSpecial(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_LEVEL, Left);
		PlusSpecial(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_LEVEL, Left);
		PlusSpecialPercent(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_PERCENT, Left, 2);
		PlusSpecialPercent(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_PERCENT, Left, 2);
	}

	if (Amulet->Type != -1 && Amulet->Durability != 0)
	{
		PlusSpecial(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_LEVEL, Amulet);
		PlusSpecial(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_LEVEL, Amulet);
		PlusSpecialPercent(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_PERCENT, Amulet, 2);
		PlusSpecialPercent(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_PERCENT, Amulet, 2);

		PlusSpecial(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_LEVEL, Amulet);
		PlusSpecial(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_LEVEL, Amulet);
		PlusSpecialPercent(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_PERCENT, Amulet, 2);
		PlusSpecialPercent(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_PERCENT, Amulet, 2);
	}
	if (RRing->Type != -1 && RRing->Durability != 0)
	{
		PlusSpecialPercent(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_PERCENT, RRing, RRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_PERCENT, RRing, RRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_PERCENT, RRing, RRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_PERCENT, RRing, RRing->SpecialValue[1]);
	}
	if (LRing->Type != -1 && LRing->Durability != 0)
	{
		PlusSpecialPercent(&Character.AttackDamageMinRight, AT_IMPROVE_DAMAGE_PERCENT, LRing, LRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMaxRight, AT_IMPROVE_DAMAGE_PERCENT, LRing, LRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMinLeft, AT_IMPROVE_DAMAGE_PERCENT, LRing, LRing->SpecialValue[1]);
		PlusSpecialPercent(&Character.AttackDamageMaxLeft, AT_IMPROVE_DAMAGE_PERCENT, LRing, LRing->SpecialValue[1]);
	}

	DWORD    Damage = 0;

	memset(g_csItemOption.m_bySetOptionListOnOff, 0, sizeof(BYTE) * 16);

	g_csItemOption.PlusSpecial(&Damage, AT_SET_OPTION_IMPROVE_ATTACK_MIN);
	Character.AttackDamageMinLeft += Damage;
	Character.AttackDamageMinRight += Damage;

	Damage = 0;
	g_csItemOption.PlusSpecial(&Damage, AT_SET_OPTION_IMPROVE_ATTACK_MAX);
	Character.AttackDamageMaxLeft += Damage;
	Character.AttackDamageMaxRight += Damage;

	Damage = 0;
	g_csItemOption.PlusSpecialLevel(&Damage, Dexterity, AT_SET_OPTION_IMPROVE_ATTACK_1);
	Character.AttackDamageMinRight += Damage;
	Character.AttackDamageMaxRight += Damage;
	Character.AttackDamageMinLeft += Damage;
	Character.AttackDamageMaxLeft += Damage;

	Damage = 0;
	g_csItemOption.PlusSpecialLevel(&Damage, Strength, AT_SET_OPTION_IMPROVE_ATTACK_2);
	Character.AttackDamageMinRight += Damage;
	Character.AttackDamageMaxRight += Damage;
	Character.AttackDamageMinLeft += Damage;
	Character.AttackDamageMaxLeft += Damage;

	if ((Right->Type >= ITEM_BOW && Right->Type < ITEM_BOW + MAX_ITEM_INDEX) && (Left->Type >= ITEM_BOW && Left->Type < ITEM_BOW + MAX_ITEM_INDEX))
	{
		int LLevel = ((Left->Level >> 3) & 15);
		int RLevel = ((Right->Level >> 3) & 15);

		if (Left->Type == ITEM_BOW + 7 && LLevel >= 1)
		{
			Character.AttackDamageMinRight += (WORD)(Character.AttackDamageMinRight * ((LLevel * 2 + 1) * 0.01f) + 1);
			Character.AttackDamageMaxRight += (WORD)(Character.AttackDamageMaxRight * ((LLevel * 2 + 1) * 0.01f) + 1);
		}
		else if (Right->Type == ITEM_BOW + 15 && RLevel >= 1)
		{
			Character.AttackDamageMinLeft += (WORD)(Character.AttackDamageMinLeft * ((RLevel * 2 + 1) * 0.01f) + 1);
			Character.AttackDamageMaxLeft += (WORD)(Character.AttackDamageMaxLeft * ((RLevel * 2 + 1) * 0.01f) + 1);
		}
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll3))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 74);
		Character.AttackDamageMinRight += Item_data.m_byValue1;
		Character.AttackDamageMaxRight += Item_data.m_byValue1;
		Character.AttackDamageMinLeft += Item_data.m_byValue1;
		Character.AttackDamageMaxLeft += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin2))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 47);
		Character.AttackDamageMinRight += Item_data.m_byValue1;
		Character.AttackDamageMaxRight += Item_data.m_byValue1;
		Character.AttackDamageMinLeft += Item_data.m_byValue1;
		Character.AttackDamageMaxLeft += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_Petal))
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 87);
		Character.AttackDamageMinRight += Item_data.m_byValue1;
		Character.AttackDamageMaxRight += Item_data.m_byValue1;
		Character.AttackDamageMinLeft += Item_data.m_byValue1;
		Character.AttackDamageMaxLeft += Item_data.m_byValue1;
	}
	Character.AttackDamageMinRight += g_SocketItemMgr.m_StatusBonus.m_iAttackDamageMinBonus;
	Character.AttackDamageMaxRight += g_SocketItemMgr.m_StatusBonus.m_iAttackDamageMaxBonus;
	Character.AttackDamageMinLeft += g_SocketItemMgr.m_StatusBonus.m_iAttackDamageMinBonus;
	Character.AttackDamageMaxLeft += g_SocketItemMgr.m_StatusBonus.m_iAttackDamageMaxBonus;
	if (g_isCharacterBuff((&Hero->Object), eBuff_BlessingOfXmax))	//Å©¸®½º¸¶½ºÀÇ Ãàº¹
	{
		int _Temp = 0;
		_Temp = Character.Level / 3 + 45;

		Character.AttackDamageMinRight += _Temp;
		Character.AttackDamageMaxRight += _Temp;
		Character.AttackDamageMinLeft += _Temp;
		Character.AttackDamageMaxLeft += _Temp;
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_StrengthOfSanta))	//»êÅ¸ÀÇ °­È­
	{
		int _Temp = 30;

		Character.AttackDamageMinRight += _Temp;
		Character.AttackDamageMaxRight += _Temp;
		Character.AttackDamageMinLeft += _Temp;
		Character.AttackDamageMaxLeft += _Temp;

	}
}

void CHARACTER_MACHINE::CalculateCriticalDamage()
{
	Character.CriticalDamage = 0;
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_WEAPON_RIGHT]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_WEAPON_LEFT]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_HELM]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_ARMOR]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_PANTS]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_GLOVES]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_BOOTS]);
	PlusSpecial(&Character.CriticalDamage, AT_LUCK, &Equipment[EQUIPMENT_WING]);
}

void CHARACTER_MACHINE::CalculateMagicDamage()
{
	DWORD Energy;
	Energy = Character.Energy + Character.AddEnergy;
	Character.MagicDamageMin = Energy / 9;
	Character.MagicDamageMax = Energy / 4;

	ITEM* Right = &Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* Left = &Equipment[EQUIPMENT_WEAPON_LEFT];
	ITEM* Amulet = &Equipment[EQUIPMENT_AMULET];
	ITEM* RRing = &Equipment[EQUIPMENT_RING_RIGHT];
	ITEM* LRing = &Equipment[EQUIPMENT_RING_LEFT];
	float   percent;
	DWORD    DamageMin = 0;
	DWORD    DamageMax = 0;

	// ³¯°³
	if (Equipment[EQUIPMENT_WING].Type != -1)
	{
		Script_Item* p = GMItemMng->find(Equipment[EQUIPMENT_WING].Type);
		ITEM* ipWing = &Equipment[EQUIPMENT_WING];
		percent = CalcDurabilityPercent(ipWing->Durability, p->Durability, ipWing->Level, 0);//ipWing->Option1);

		DamageMin = 0;
		DamageMax = 0;
		PlusSpecial(&DamageMin, AT_IMPROVE_MAGIC, &Equipment[EQUIPMENT_WING]);
		PlusSpecial(&DamageMax, AT_IMPROVE_MAGIC, &Equipment[EQUIPMENT_WING]);

		DamageMin = DamageMin - (WORD)(DamageMin * percent);
		DamageMax = DamageMax - (WORD)(DamageMax * percent);

		Character.MagicDamageMin += DamageMin;
		Character.MagicDamageMax += DamageMax;
	}

	if (Right->Type != -1 && Right->Durability != 0)
	{
		Script_Item* p = GMItemMng->find(Right->Type);
		percent = CalcDurabilityPercent(Right->Durability, p->Durability, Right->Level, Right->Option1, Right->ExtOption);
		DamageMin = 0; DamageMax = 0;

		if (Right->Type == ITEM_SWORD + 21
			|| Right->Type == ITEM_SWORD + 31
			|| Right->Type == ITEM_SWORD + 23
			|| Right->Type == ITEM_SWORD + 25
			|| Right->Type == ITEM_SWORD + 28
			)
		{
			PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, Right);
			PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, Right);
		}
		else
		{
			PlusSpecial(&DamageMin, AT_IMPROVE_MAGIC, Right);
			PlusSpecial(&DamageMax, AT_IMPROVE_MAGIC, Right);
		}

		Character.MagicDamageMin += DamageMin - (WORD)(DamageMin * percent);
		Character.MagicDamageMax += DamageMax - (WORD)(DamageMax * percent);

		PlusSpecial(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_LEVEL, Right);
		PlusSpecial(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_LEVEL, Right);
		PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, Right, 2);
		PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, Right, 2);
	}

	if (Left->Type != -1 && Left->Durability != 0)
	{
		// CalculateCurseDamage()
		if (CLASS_SUMMONER != gCharacterManager.GetBaseClass(Character.Class))
		{
			Script_Item* p = GMItemMng->find(Left->Type);
			percent = CalcDurabilityPercent(Left->Durability, p->Durability, Left->Level, Left->Option1, Left->ExtOption);
			DamageMin = 0; DamageMax = 0;

			if (Left->Type >= ITEM_SWORD && Left->Type < ITEM_SHIELD)
			{
				if (Left->Type == ITEM_SWORD + 28)
				{
					PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, Left);
					PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, Left);
				}
				else
				{
					PlusSpecial(&DamageMin, AT_IMPROVE_MAGIC, Left);
					PlusSpecial(&DamageMax, AT_IMPROVE_MAGIC, Left);
				}

				if (gCharacterManager.GetBaseClass(Character.Class) != CLASS_DARK)
				{
					Character.MagicDamageMin += DamageMin - (WORD)(DamageMin * percent);
					Character.MagicDamageMax += DamageMax - (WORD)(DamageMax * percent);
				}

				PlusSpecial(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_LEVEL, Left);
				PlusSpecial(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_LEVEL, Left);
				PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, Left, 2);
				PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, Left, 2);
			}
		}
	}

	if (Left->Type != -1 && Left->Durability != 0)
	{
		if (CLASS_SUMMONER != gCharacterManager.GetBaseClass(Character.Class))
		{
			Script_Item* p = GMItemMng->find(Left->Type);
			percent = CalcDurabilityPercent(Left->Durability, p->Durability, Left->Level, Left->Option1, Left->ExtOption);
			DamageMin = 0; DamageMax = 0;

			if (Right->Type == ITEM_SWORD + 28)
			{
				PlusSpecial(&DamageMin, AT_IMPROVE_DAMAGE, Right);
				PlusSpecial(&DamageMax, AT_IMPROVE_DAMAGE, Right);
			}
			else
			{
				PlusSpecial(&DamageMin, AT_IMPROVE_MAGIC, Right);
				PlusSpecial(&DamageMax, AT_IMPROVE_MAGIC, Right);
			}

			if (gCharacterManager.GetBaseClass(Character.Class) != CLASS_DARK
				)
			{
				Character.MagicDamageMin += DamageMin - (WORD)(DamageMin * percent);
				Character.MagicDamageMax += DamageMax - (WORD)(DamageMax * percent);
			}

			PlusSpecial(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_LEVEL, Left);
			PlusSpecial(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_LEVEL, Left);
			PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, Left, 2);
			PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, Left, 2);
		}
	}

	if (Amulet->Type != -1 && Amulet->Durability != 0)
	{
		PlusSpecial(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_LEVEL, Amulet);
		PlusSpecial(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_LEVEL, Amulet);
		PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, Amulet, 2);
		PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, Amulet, 2);
	}

	if (RRing->Type != -1 && RRing->Durability != 0)
	{
		PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, RRing, RRing->SpecialValue[0]);
		PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, RRing, RRing->SpecialValue[0]);
	}
	if (LRing->Type != -1 && LRing->Durability != 0)
	{
		PlusSpecialPercent(&Character.MagicDamageMin, AT_IMPROVE_MAGIC_PERCENT, LRing, LRing->SpecialValue[0]);
		PlusSpecialPercent(&Character.MagicDamageMax, AT_IMPROVE_MAGIC_PERCENT, LRing, LRing->SpecialValue[0]);
	}

	DWORD MagicDamage = 0;

	MagicDamage = 0;
	g_csItemOption.PlusSpecialLevel(&MagicDamage, Energy, AT_SET_OPTION_IMPROVE_MAGIC);
	Character.MagicDamageMin += MagicDamage;
	Character.MagicDamageMax += MagicDamage;

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll4))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 75);
		Character.MagicDamageMin += Item_data.m_byValue1;
		Character.MagicDamageMax += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin2))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 47);
		Character.MagicDamageMin += Item_data.m_byValue1;
		Character.MagicDamageMax += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_CherryBlossom_Petal))
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 87);
		Character.MagicDamageMin += Item_data.m_byValue1;
		Character.MagicDamageMax += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_SwellOfMagicPower)
		|| (g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_IMPROVED))
		|| (g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_ENHANCED))
		)
	{
		int iAdd_MP = (CharacterAttribute->Energy / 9) * 0.2f;
		Character.MagicDamageMin += iAdd_MP;
	}
	Character.MagicDamageMin += g_SocketItemMgr.m_StatusBonus.m_iSkillAttackDamageBonus;
	Character.MagicDamageMax += g_SocketItemMgr.m_StatusBonus.m_iSkillAttackDamageBonus;
	Character.MagicDamageMin += g_SocketItemMgr.m_StatusBonus.m_iMagicPowerBonus;
	Character.MagicDamageMax += g_SocketItemMgr.m_StatusBonus.m_iMagicPowerBonus;

	if (g_isCharacterBuff((&Hero->Object), eBuff_StrengthOfSanta))
	{
		int _Temp = 30;

		Character.MagicDamageMin += _Temp;
		Character.MagicDamageMax += _Temp;
	}
}

void CHARACTER_MACHINE::CalculateCurseDamage()
{
	if (CLASS_SUMMONER != gCharacterManager.GetBaseClass(Character.Class))
		return;

	DWORD wEnergy = Character.Energy + Character.AddEnergy;
	Character.CurseDamageMin = wEnergy / 9;
	Character.CurseDamageMax = wEnergy / 4;

	ITEM* pEquipWing = &Equipment[EQUIPMENT_WING];
	if (pEquipWing->Type != -1)
	{
		Script_Item* pAttribute = GMItemMng->find(pEquipWing->Type);
		DWORD wDamageMin = 0;
		DWORD wDamageMax = 0;

		::PlusSpecial(&wDamageMin, AT_IMPROVE_CURSE, pEquipWing);
		::PlusSpecial(&wDamageMax, AT_IMPROVE_CURSE, pEquipWing);

		float fPercent = ::CalcDurabilityPercent(pEquipWing->Durability,
			pAttribute->Durability, pEquipWing->Level, 0);
		wDamageMin -= DWORD(wDamageMin * fPercent);
		wDamageMax -= DWORD(wDamageMax * fPercent);

		Character.CurseDamageMin += wDamageMin;
		Character.CurseDamageMax += wDamageMax;
	}

	ITEM* pEquipLeft = &Equipment[EQUIPMENT_WEAPON_LEFT];
	if (pEquipLeft->Type != -1 && pEquipLeft->Durability != 0)
	{
		Script_Item* pAttribute = GMItemMng->find(pEquipLeft->Type);
		DWORD wDamageMin = 0;
		DWORD wDamageMax = 0;

		::PlusSpecial(&wDamageMin, AT_IMPROVE_CURSE, pEquipLeft);
		::PlusSpecial(&wDamageMax, AT_IMPROVE_CURSE, pEquipLeft);

		float fPercent = CalcDurabilityPercent(pEquipLeft->Durability,
			pAttribute->Durability, pEquipLeft->Level, pEquipLeft->Option1,
			pEquipLeft->ExtOption);

		Character.CurseDamageMin += wDamageMin - DWORD(wDamageMin * fPercent);
		Character.CurseDamageMax += wDamageMax - DWORD(wDamageMax * fPercent);

		PlusSpecial(&Character.CurseDamageMin, AT_IMPROVE_MAGIC_LEVEL, pEquipLeft);
		PlusSpecial(&Character.CurseDamageMax, AT_IMPROVE_MAGIC_LEVEL, pEquipLeft);
		PlusSpecialPercent(&Character.CurseDamageMin, AT_IMPROVE_MAGIC_PERCENT, pEquipLeft, 2);
		PlusSpecialPercent(&Character.CurseDamageMax, AT_IMPROVE_MAGIC_PERCENT, pEquipLeft, 2);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_StrengthOfSanta))
	{
		int _Temp = 30;

		Character.CurseDamageMin += _Temp;
		Character.CurseDamageMax += _Temp;
	}
}

void CHARACTER_MACHINE::CalculateAttackRating()
{
	DWORD Strength, Dexterity, Charisma;

	Strength = Character.Strength + Character.AddStrength;
	Dexterity = Character.Dexterity + Character.AddDexterity;
	Charisma = Character.Charisma + Character.AddCharisma;

	if (gCharacterManager.GetBaseClass(Character.Class) == CLASS_DARK_LORD)
	{
		Character.AttackRating = static_cast<DWORD>(((Character.Level * 5) + (Dexterity * 5) / 2) + (Strength / 6) + (Charisma / 10) & 0xFFFF);
	}
	else if (gCharacterManager.GetBaseClass(Character.Class) == CLASS_RAGEFIGHTER)
	{
		Character.AttackRating = ((Character.Level * 3) + (Dexterity * 5) / 4) + (Strength / 6);
	}
	else
	{
		Character.AttackRating = static_cast<WORD>((((Character.Level * 5) + (Dexterity * 3) / 2) + (Strength / 4)) & 0xFFFF);
	}

	g_csItemOption.PlusSpecial(&Character.AttackRating, AT_SET_OPTION_IMPROVE_ATTACKING_PERCENT);

	Character.AttackRating += g_SocketItemMgr.m_StatusBonus.m_iAttackRateBonus;
}

void CHARACTER_MACHINE::CalculateAttackRatingPK()
{
	WORD Dexterity;
	Dexterity = Character.Dexterity + Character.AddDexterity;
	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	float tmpf = 0.f;
	switch (CharacterClass)
	{
	case CLASS_KNIGHT:
		tmpf = (float)Character.Level * 3 + (float)Dexterity * 4.5f;
		break;
	case CLASS_DARK_LORD:
		tmpf = (float)Character.Level * 3 + (float)Dexterity * 4.f;
		break;
	case CLASS_ELF:
		tmpf = (float)Character.Level * 3 + (float)Dexterity * 0.6f;
		break;
	case CLASS_DARK:
	case CLASS_SUMMONER:
		tmpf = (float)Character.Level * 3 + (float)Dexterity * 3.5f;
		break;
	case CLASS_WIZARD:
		tmpf = (float)Character.Level * 3 + (float)Dexterity * 4.f;
		break;
#ifdef PBG_ADD_NEWCHAR_MONK
	case CLASS_RAGEFIGHTER:
		tmpf = (float)Character.Level * 2.6f + (float)Dexterity * 3.6f;
		break;
#endif //PBG_ADD_NEWCHAR_MONK
	}

	Character.AttackRatingPK = (WORD)tmpf;
}

void CHARACTER_MACHINE::CalculateAttackSpeed()
{
	/*DWORD Dexterity = Character.Dexterity + Character.AddDexterity;

	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	if (CharacterClass == CLASS_ELF)
	{
		Character.AttackSpeed = Dexterity / 50;
		Character.MagicSpeed = Dexterity / 50;
	}
	else if (CharacterClass == CLASS_KNIGHT || CharacterClass == CLASS_DARK)
	{
		Character.AttackSpeed = Dexterity / 15;
		Character.MagicSpeed = Dexterity / 20;
	}

	else if (CharacterClass == CLASS_DARK_LORD)
	{
		Character.AttackSpeed = Dexterity / 10;
		Character.MagicSpeed = Dexterity / 10;
	}
	else if (CharacterClass == CLASS_SUMMONER)
	{
		Character.AttackSpeed = Dexterity / 20;
		Character.MagicSpeed = Dexterity / 20;
	}
	else if (CharacterClass == CLASS_RAGEFIGHTER)
	{
		Character.AttackSpeed = Dexterity / 9;
		Character.MagicSpeed = Dexterity / 9;
	}
	else
	{
		Character.AttackSpeed = Dexterity / 20;
		Character.MagicSpeed = Dexterity / 10;
	}

	DWORD MaxAttackSpeed = GMProtect->GetAttackSpeedForClass(CharacterClass);

	MaxAttackSpeed &= 0xFFFF;

	if (Character.AttackSpeed > MaxAttackSpeed)
		Character.AttackSpeed = MaxAttackSpeed;

	if (Character.MagicSpeed > MaxAttackSpeed)
		Character.MagicSpeed = MaxAttackSpeed;

	ITEM* r = &Equipment[EQUIPMENT_WEAPON_RIGHT];
	ITEM* l = &Equipment[EQUIPMENT_WEAPON_LEFT];
	ITEM* g = &Equipment[EQUIPMENT_GLOVES];
	ITEM* rl = &Equipment[EQUIPMENT_RING_LEFT];
	ITEM* rr = &Equipment[EQUIPMENT_RING_RIGHT];
	ITEM* phelper = &Equipment[EQUIPMENT_HELPER];

	SEASON3B::CNewUIPickedItem* pPickedItem = SEASON3B::CNewUIInventoryCtrl::GetPickedItem();
	if (pPickedItem && pPickedItem->GetOwnerInventory() == NULL)
	{
		ITEM* pItem = pPickedItem->GetItem();

		if (pItem)
		{
			int iSourceIndex = pPickedItem->GetSourceLinealPos();
			switch (iSourceIndex)
			{
			case EQUIPMENT_WEAPON_RIGHT:
				r = pItem;
				break;
			case EQUIPMENT_WEAPON_LEFT:
				l = pItem;
				break;
			case EQUIPMENT_GLOVES:
				g = pItem;
				break;
			case EQUIPMENT_RING_LEFT:
				rl = pItem;
				break;
			case EQUIPMENT_RING_RIGHT:
				rr = pItem;
				break;
			case EQUIPMENT_HELPER:
				phelper = pItem;
				break;

			}
		}
	}

	bool Right = false;
	bool Left = false;
	bool Glove = false;
	bool RingRight = false;
	bool RingLeft = false;
	bool Helper = false;
	if ((r->Type != ITEM_BOW + 7 && r->Type != ITEM_BOW + 15) && r->Type >= ITEM_SWORD && r->Type < ITEM_STAFF + MAX_ITEM_INDEX) {
		if (IsRequireEquipItem(r) && r->Durability != 0)
		{
			Right = true;
		}
	}

	if ((l->Type != ITEM_BOW + 7 && l->Type != ITEM_BOW + 15) && l->Type >= ITEM_SWORD && l->Type < ITEM_STAFF + MAX_ITEM_INDEX) {
		if (IsRequireEquipItem(l) && l->Durability != 0)
		{
			Left = true;
		}
	}

	if (g->Durability != 0 && g->Type != -1) {
		if (IsRequireEquipItem(g))
		{
			Glove = true;
		}
	}

	if (rl->Durability != 0 && (rl->Type == ITEM_HELPER + 20 || rl->Type == ITEM_HELPER + 107))
	{
		if (IsRequireEquipItem(rl))
		{
			if (false == rl->bPeriodItem || false == rl->bExpiredPeriod)
			{
				switch (rl->Level >> 3)
				{
				case 0:
				case 3:
					RingRight = true;
					break;
				}
			}
		}
	}

	if (rr->Durability != 0 && (rr->Type == ITEM_HELPER + 20 || rr->Type == ITEM_HELPER + 107))
	{
		if (IsRequireEquipItem(rr))
		{
			if (false == rr->bPeriodItem || false == rr->bExpiredPeriod)
			{
				switch (rr->Level >> 3)
				{
				case 0:
				case 3:
					RingLeft = true;
					break;
				}
			}
		}
	}

	if (phelper->Durability != 0 &&
		(phelper->Type == ITEM_HELPER + 64
			|| phelper->Type == ITEM_HELPER + 123
			)
		)
	{
		if (IsRequireEquipItem(phelper))
		{
			if (false == phelper->bPeriodItem || false == phelper->bExpiredPeriod)
			{
				Helper = true;
			}
		}
	}

	int RightWeaponSpeed = r->WeaponSpeed;
	int LeftWeaponSpeed = l->WeaponSpeed;

	if (r->Durability <= 0)
		RightWeaponSpeed = 0;
	if (l->Durability <= 0)
		LeftWeaponSpeed = 0;

	if (Right && Left)
	{
		Character.AttackSpeed += (RightWeaponSpeed + LeftWeaponSpeed) / 2;
		Character.MagicSpeed += (RightWeaponSpeed + LeftWeaponSpeed) / 2;
	}
	else if (Right)
	{
		Character.AttackSpeed += RightWeaponSpeed;
		Character.MagicSpeed += RightWeaponSpeed;
	}
	else if (Left)
	{
		Character.AttackSpeed += LeftWeaponSpeed;
		Character.MagicSpeed += LeftWeaponSpeed;
	}

	if (RingRight)
	{
		Character.AttackSpeed += 10;
		Character.MagicSpeed += 10;
		CharacterAttribute->Ability |= ABILITY_FAST_ATTACK_RING;
	}
	else if (RingLeft)
	{
		Character.AttackSpeed += 10;
		Character.MagicSpeed += 10;
		CharacterAttribute->Ability |= ABILITY_FAST_ATTACK_RING;
	}
	else if (CharacterAttribute->Ability & ABILITY_FAST_ATTACK_RING)
	{
		CharacterAttribute->Ability &= (~ABILITY_FAST_ATTACK_RING);
	}

	if (Glove)
	{
		Character.AttackSpeed += g->WeaponSpeed;
		Character.MagicSpeed += g->WeaponSpeed;
	}

	if (Helper)
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_HELPER + 64);
		Character.AttackSpeed += Item_data.m_byValue2;
		Character.MagicSpeed += Item_data.m_byValue2;
	}

	if (Helper)
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_HELPER + 123);
		Character.AttackSpeed += Item_data.m_byValue2;
		Character.MagicSpeed += Item_data.m_byValue2;
	}

	if (Character.Ability & ABILITY_FAST_ATTACK_SPEED)
	{
		Character.AttackSpeed += 20;
		Character.MagicSpeed += 20;
	}
	else if (Character.Ability & ABILITY_FAST_ATTACK_SPEED2)
	{
		Character.AttackSpeed += 20;
		Character.MagicSpeed += 20;
	}
	ITEM* Amulet = &Equipment[EQUIPMENT_AMULET];

	if ((false == Amulet->bPeriodItem || false == Amulet->bExpiredPeriod) && Amulet->Type == ITEM_HELPER + 114)
	{
		int _Temp = 7;

		Character.AttackSpeed += _Temp;
		Character.MagicSpeed += _Temp;
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll1))
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 72);
		Character.AttackSpeed += Item_data.m_byValue1;
		Character.MagicSpeed += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin1))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 46);
		Character.AttackSpeed += Item_data.m_byValue1;
		Character.MagicSpeed += Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_QuickOfSanta))
	{
		int _Temp = 15;
		Character.AttackSpeed += _Temp;
		Character.MagicSpeed += _Temp;
	}
	PlusSpecial(&Character.AttackSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_WEAPON_RIGHT]);
	PlusSpecial(&Character.MagicSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_WEAPON_RIGHT]);
	PlusSpecial(&Character.AttackSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_WEAPON_LEFT]);
	PlusSpecial(&Character.MagicSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_WEAPON_LEFT]);
	PlusSpecial(&Character.AttackSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_AMULET]);
	PlusSpecial(&Character.MagicSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_AMULET]);
	PlusSpecial(&Character.AttackSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_HELPER]);
	PlusSpecial(&Character.MagicSpeed, AT_IMPROVE_ATTACK_SPEED, &Equipment[EQUIPMENT_HELPER]);
	Character.AttackSpeed += g_SocketItemMgr.m_StatusBonus.m_iAttackSpeedBonus;
	Character.MagicSpeed += g_SocketItemMgr.m_StatusBonus.m_iAttackSpeedBonus;*/
}

void CHARACTER_MACHINE::CalculateSuccessfulBlockingPK()
{
	WORD Dexterity;
	Dexterity = Character.Dexterity + Character.AddDexterity;
	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	float tmpf = 0.f;
	switch (CharacterClass)
	{
	case CLASS_WIZARD:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.25f;
		break;
	case CLASS_KNIGHT:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.5f;
		break;
	case CLASS_ELF:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.1f;
		break;
	case CLASS_DARK:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.25f;
		break;
	case CLASS_DARK_LORD:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.5f;
		break;
	case CLASS_SUMMONER:
		tmpf = (float)Character.Level * 2 + (float)Dexterity * 0.5f;
		break;
#ifdef PBG_ADD_NEWCHAR_MONK
	case CLASS_RAGEFIGHTER:
		tmpf = (float)Character.Level * 1.5f + (float)Dexterity * 0.2f;
		break;
#endif //PBG_ADD_NEWCHAR_MONK
	}
	Character.SuccessfulBlockingPK = (WORD)tmpf;
}

void CHARACTER_MACHINE::CalculateSuccessfulBlocking()
{
	DWORD Dexterity;

	Dexterity = Character.Dexterity + Character.AddDexterity;

	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	if (CharacterClass == CLASS_ELF || CharacterClass == CLASS_SUMMONER)
	{
		Character.SuccessfulBlocking = Dexterity / 4;
	}
	else if (CharacterClass == CLASS_DARK_LORD)
	{
		Character.SuccessfulBlocking = Dexterity / 7;
	}
#ifdef PBG_ADD_NEWCHAR_MONK
	else if (CharacterClass == CLASS_RAGEFIGHTER)
	{
		Character.SuccessfulBlocking = Dexterity / 10;
	}
#endif //PBG_ADD_NEWCHAR_MONK
	else
	{
		Character.SuccessfulBlocking = Dexterity / 3;
	}

	ITEM* Left = &Equipment[EQUIPMENT_WEAPON_LEFT];
	if (Left->Type != -1 && Left->Durability != 0)
	{
		Script_Item* p = GMItemMng->find(Left->Type);
		float percent = CalcDurabilityPercent(Left->Durability, p->Durability, Left->Level, Left->Option1, Left->ExtOption);

		DWORD SuccessBlocking = 0;
		SuccessBlocking = Left->SuccessfulBlocking - (DWORD)(Left->SuccessfulBlocking * percent);

		Character.SuccessfulBlocking += SuccessBlocking;

		SuccessBlocking = 0;
		PlusSpecial(&SuccessBlocking, AT_IMPROVE_BLOCKING, Left);

		Character.SuccessfulBlocking += SuccessBlocking - (DWORD)(SuccessBlocking * percent);

		PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, Left, 10);
	}

	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_HELM], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_ARMOR], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_PANTS], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_GLOVES], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_BOOTS], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_RING_LEFT], 10);
	PlusSpecialPercent(&Character.SuccessfulBlocking, AT_IMPROVE_BLOCKING_PERCENT, &Equipment[EQUIPMENT_RING_RIGHT], 10);

	Character.SuccessfulBlocking *= g_SocketItemMgr.m_StatusBonus.m_fDefenceRateBonus;
}

void CHARACTER_MACHINE::CalculateDefense()
{
	DWORD Dexterity, Vitality;

	Dexterity = Character.Dexterity + Character.AddDexterity;
	Vitality = Character.Vitality + Character.AddVitality;

	int CharacterClass = gCharacterManager.GetBaseClass(Character.Class);

	if (CharacterClass == CLASS_ELF)
	{
		Character.Defense = Dexterity / 10;
	}
	else if (CharacterClass == CLASS_KNIGHT || CharacterClass == CLASS_SUMMONER)
	{
		Character.Defense = Dexterity / 3;
	}
	else if (CharacterClass == CLASS_WIZARD)
	{
		Character.Defense = Dexterity / 4;
	}
	else if (CharacterClass == CLASS_DARK_LORD)
	{
		Character.Defense = Dexterity / 7;
	}
#ifdef PBG_ADD_NEWCHAR_MONK
	else if (CharacterClass == CLASS_RAGEFIGHTER)
	{
		Character.Defense = Dexterity / 8;
	}
#endif //PBG_ADD_NEWCHAR_MONK
	else
	{
		Character.Defense = Dexterity / 5;
	}

	DWORD    Defense = 0;
	for (int i = EQUIPMENT_WEAPON_LEFT; i <= EQUIPMENT_WING; ++i)
	{
		if (Equipment[i].Durability != 0)
		{
			DWORD defense = ItemDefense(&Equipment[i]);

			Script_Item* p = GMItemMng->find(Equipment[i].Type);
			float percent;
			if (i == EQUIPMENT_WING)
			{
				percent = CalcDurabilityPercent(Equipment[i].Durability, p->Durability, Equipment[i].Level, 0);//Equipment[i].Option1);
			}
			else
			{
				percent = CalcDurabilityPercent(Equipment[i].Durability, p->Durability, Equipment[i].Level, Equipment[i].Option1, Equipment[i].ExtOption);
			}

			defense -= (DWORD)(defense * percent);

			Defense += defense;
		}
	}
	Character.Defense += Defense;

	if (g_bAddDefense)
	{
		float addDefense = 0.f;
		switch (EquipmentLevelSet)
		{
		case 10:
			addDefense = 0.05f;
			break;
		case 11:
			addDefense = 0.1f;
			break;
		case 12:
			addDefense = 0.15f;
			break;
		case 13:
			addDefense = 0.2f;
			break;
		case 14:
			addDefense = 0.25f;
			break;
		case 15:
			addDefense = 0.3f;
			break;
		}
		Character.Defense += (DWORD)(Character.Defense * addDefense);
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_EliteScroll2))
	{
		const ITEM_ADD_OPTION& Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 73);
		Character.Defense += (DWORD)Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_Hellowin3))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 48);
		Character.Defense += (DWORD)Item_data.m_byValue1;
	}
	if (g_isCharacterBuff((&Hero->Object), eBuff_BlessingOfXmax))
	{
		int _Temp = 0;
		_Temp = Character.Level / 5 + 50;

		Character.Defense += _Temp;
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_DefenseOfSanta))
	{
		int _Temp = 100;
		Character.Defense += _Temp;
	}

	g_csItemOption.PlusSpecial(&Character.Defense, AT_SET_OPTION_IMPROVE_DEFENCE);
	g_csItemOption.PlusSpecialLevel(&Character.Defense, Dexterity, AT_SET_OPTION_IMPROVE_DEFENCE_3);
	g_csItemOption.PlusSpecialLevel(&Character.Defense, Vitality, AT_SET_OPTION_IMPROVE_DEFENCE_4);

	if (Equipment[EQUIPMENT_WEAPON_LEFT].Type >= ITEM_SHIELD && Equipment[EQUIPMENT_WEAPON_LEFT].Type < ITEM_SHIELD + MAX_ITEM_INDEX)
	{
		g_csItemOption.PlusSpecialPercent(&Character.Defense, AT_SET_OPTION_IMPROVE_SHIELD_DEFENCE);
	}

	PlusSpecial(&Character.Defense, AT_SET_OPTION_IMPROVE_DEFENCE, &Equipment[EQUIPMENT_HELPER]);
	if (Equipment[EQUIPMENT_WING].Type == ITEM_HELPER + 30
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		|| Equipment[EQUIPMENT_WING].Type == ITEM_WING + 49
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
		)
	{
		PlusSpecial(&Character.Defense, AT_SET_OPTION_IMPROVE_DEFENCE, &Equipment[EQUIPMENT_WING]);
	}

	Character.Defense += g_SocketItemMgr.m_StatusBonus.m_iDefenceBonus;

	if (Equipment[EQUIPMENT_WEAPON_LEFT].Type != -1 && Equipment[EQUIPMENT_WEAPON_LEFT].Durability != 0)
	{
		Character.Defense += Equipment[EQUIPMENT_WEAPON_LEFT].Defense * g_SocketItemMgr.m_StatusBonus.m_iShieldDefenceBonus * 0.01f;
	}
}

void CHARACTER_MACHINE::CalculateMagicDefense()
{
	for (int i = EQUIPMENT_HELM; i <= EQUIPMENT_WING; ++i)
	{
		if (Equipment[i].Durability != 0)
		{
			Character.MagicDefense = ItemMagicDefense(&Equipment[i]);
		}
	}
}

void CHARACTER_MACHINE::CalculateWalkSpeed()
{
	if (Equipment[EQUIPMENT_BOOTS].Durability != 0)
	{
		Character.WalkSpeed = ItemWalkSpeed(&Equipment[EQUIPMENT_BOOTS]);
	}

	if (Equipment[EQUIPMENT_WING].Durability != 0)
	{
		Character.WalkSpeed += ItemWalkSpeed(&Equipment[EQUIPMENT_WING]);
	}
}

void CHARACTER_MACHINE::CalculateNextExperince()
{
	if (Character.MaxCharacterLevel > 400)
	{
		// Normalizamos el nivel actual proporcionalmente al sistema de nivel 400
		float normalized_level = static_cast<float>(Character.Level) / Character.MaxCharacterLevel * 400.0f;

		// Cálculo de la experiencia base utilizando el nivel normalizado, en punto flotante para precisión
		float baseExperience = (9.0f + normalized_level) * normalized_level * normalized_level * 10.0f;

		// Ajuste adicional si el nivel normalizado es mayor que 255
		if (normalized_level > 255.0f)
		{
			float LevelOver_N = normalized_level - 255.0f;
			baseExperience += (9.0f + LevelOver_N) * LevelOver_N * LevelOver_N * 1000.0f;
		}

		// Convertir el resultado final de experiencia a DWORD para evitar pérdida de precisión
		Character.Experience = Character.NextExperince;
		Character.NextExperince = static_cast<DWORD>(baseExperience);
	}
	else
	{
		Character.Experience = Character.NextExperince;
		Character.NextExperince = (9 + Character.Level) * (Character.Level) * (Character.Level) * 10;

		if (Character.Level > 255)
		{
			int LevelOver_N = Character.Level - 255;
			Character.NextExperince += (9 + LevelOver_N) * LevelOver_N * LevelOver_N * 1000;
		}
	}
}

void CHARACTER_MACHINE::CalulateMasterLevelNextExperience()
{
	Master_Level_Data.lMasterLevel_Experince = Master_Level_Data.lNext_MasterLevel_Experince;

	__int64 iTotalLevel = (__int64)CharacterAttribute->Level + (__int64)Master_Level_Data.nMLevel + 1;
	__int64 iTOverLevel = iTotalLevel - 255;
	__int64 iBaseExperience = 0;

	__int64 iData_Master =
		(
			(
				(__int64)9 + (__int64)iTotalLevel
				)
			* (__int64)iTotalLevel
			* (__int64)iTotalLevel
			* (__int64)10
			)
		+
		(
			(
				(__int64)9 + (__int64)iTOverLevel
				)
			* (__int64)iTOverLevel
			* (__int64)iTOverLevel
			* (__int64)1000
			);

	Master_Level_Data.lNext_MasterLevel_Experince = (iData_Master - (__int64)3892250000) / (__int64)2;
}

bool CHARACTER_MACHINE::IsZeroDurability()
{
	for (int i = EQUIPMENT_WEAPON_RIGHT; i < MAX_EQUIPMENT; ++i)
	{
		if (Equipment[i].Durability == 0)
		{
			return  true;
		}
	}

	return  false;
}

void CHARACTER_MACHINE::CalculateBasicState()
{
	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion1))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 78);
		Character.AddStrength += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion2))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 79);
		Character.AddDexterity += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion3))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 80);
		Character.AddVitality += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion4))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 81);
		Character.AddEnergy += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion5))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 82);
		Character.AddCharisma += (WORD)Item_data.m_byValue1;
	}

	if (g_isCharacterBuff((&Hero->Object), eBuff_Hp_up_Ourforces))
	{
#ifdef PBG_MOD_RAGEFIGHTERSOUND
		WORD _AddStat = (WORD)(30 + (WORD)((Character.Energy - 132) / 10));
#else //PBG_MOD_RAGEFIGHTERSOUND
		WORD _AddStat = (WORD)(30 + (WORD)((Character.Energy - 380) / 10));
#endif //PBG_MOD_RAGEFIGHTERSOUND
		if (_AddStat > 200)
		{
			_AddStat = 200;
		}
		Character.AddVitality += _AddStat;
	}
}

void CHARACTER_MACHINE::getAllAddStateOnlyExValues(int& iAddStrengthExValues, int& iAddDexterityExValues, int& iAddVitalityExValues, int& iAddEnergyExValues, int& iAddCharismaExValues)
{
	if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion1))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 78);
		iAddStrengthExValues += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion2))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 79);
		iAddDexterityExValues += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion3))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 80);
		iAddVitalityExValues += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion4))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 81);
		iAddEnergyExValues += (WORD)Item_data.m_byValue1;
	}
	else if (g_isCharacterBuff((&Hero->Object), eBuff_SecretPotion5))
	{
		ITEM_ADD_OPTION Item_data = g_pItemAddOptioninfo->GetItemAddOtioninfo(ITEM_POTION + 82);
		iAddCharismaExValues += (WORD)Item_data.m_byValue1;
	}

	iAddStrengthExValues += g_SocketItemMgr.m_StatusBonus.m_iStrengthBonus;
	iAddDexterityExValues += g_SocketItemMgr.m_StatusBonus.m_iDexterityBonus;
	iAddVitalityExValues += g_SocketItemMgr.m_StatusBonus.m_iVitalityBonus;
	iAddEnergyExValues += g_SocketItemMgr.m_StatusBonus.m_iEnergyBonus;
}

bool CHARACTER_MACHINE::CheckEquipment(int index)
{
	bool success = false;

	if (index < MAX_EQUIPMENT)
	{
		success = true;
	}
	else
	{
		index -= 224;

		if (index >= MAX_EQUIPMENT && index < MAX_NEW_EQUIPMENT)
		{
			success = true;
		}
	}
	return success;
}

ITEM* CHARACTER_MACHINE::GetOnlyEquipment(int index)
{
	ITEM* pEquippedItem = NULL;

	if (index < MAX_EQUIPMENT)
	{
		pEquippedItem = &this->Equipment[index];
	}
	else
	{
		index -= 224;

		if (index >= MAX_EQUIPMENT && index < MAX_NEW_EQUIPMENT)
		{
			pEquippedItem = &this->Equipment[index];
		}
	}

	return pEquippedItem;
}

int CHARACTER_MACHINE::ConvertLevel(int level)
{
	if (Character.MaxCharacterLevel > 400)
	{
		return ((level / 400.0) * Character.MaxCharacterLevel);
	}
	else
	{
		return level;
	}
}

float CHARACTER_MACHINE::GetRealLife()
{
	float dataMax = static_cast<float>(Character.LifeMax);
	float data = static_cast<float>(min(max(0, CharacterAttribute->Life), dataMax));

	return (data / dataMax);
}

float CHARACTER_MACHINE::GetRealMana()
{
	float dataMax = static_cast<float>(Character.ManaMax);
	float data = static_cast<float>(min(max(0, CharacterAttribute->Mana), dataMax));

	return (data / dataMax);
}

float CHARACTER_MACHINE::GetRealShield()
{
	float dataMax = static_cast<float>(Character.ShieldMax);
	float data = static_cast<float>(min(max(0, CharacterAttribute->Shield), dataMax));

	return (data / dataMax);
}

float CHARACTER_MACHINE::GetRealSkillMP()
{
	float dataMax = static_cast<float>(Character.SkillManaMax);
	float data = static_cast<float>(min(max(0, CharacterAttribute->SkillMana), dataMax));

	return (data / dataMax);
}

void CHARACTER_MACHINE::CalculateAll()
{
	CalculateBasicState();
	g_csItemOption.CheckItemSetOptions();
	g_csItemOption.ClearListOnOff();

	InitAddValue();

	g_SocketItemMgr.CheckSocketSetOption();
	g_SocketItemMgr.CalcSocketStatusBonus();

	this->Character.AddStrength += g_SocketItemMgr.m_StatusBonus.m_iStrengthBonus;
	this->Character.AddDexterity += g_SocketItemMgr.m_StatusBonus.m_iDexterityBonus;
	this->Character.AddVitality += g_SocketItemMgr.m_StatusBonus.m_iVitalityBonus;
	this->Character.AddEnergy += g_SocketItemMgr.m_StatusBonus.m_iEnergyBonus;

	CalculateBasicState();

	DWORD wAddStrength = 0, wAddDexterity = 0, wAddEnergy = 0, wAddVitality = 0, wStrengthResult = 0, wDexterityResult = 0, wEnergyResult = 0, wVitalityResult = 0, wAddCharisma = 0, wCharismaResult = 0;

	g_csItemOption.getAllAddStateOnlyAddValue(&wAddStrength, &wAddDexterity, &wAddEnergy, &wAddVitality, &wAddCharisma);

	int iAddStrengthByExValues = 0, iAddDexterityByExValues = 0, iAddEnergyByExValues = 0, iAddVitalityByExValues = 0, iAddCharismaExValues = 0;

	getAllAddStateOnlyExValues(iAddStrengthByExValues, iAddDexterityByExValues, iAddVitalityByExValues, iAddEnergyByExValues, iAddCharismaExValues);

	wAddStrength += iAddStrengthByExValues;
	wAddDexterity += iAddDexterityByExValues;
	wAddEnergy += iAddEnergyByExValues;
	wAddVitality += iAddVitalityByExValues;

	wStrengthResult = this->Character.Strength + wAddStrength;
	wDexterityResult = this->Character.Dexterity + wAddDexterity;
	wEnergyResult = this->Character.Energy + wAddEnergy;
	wVitalityResult = this->Character.Vitality + wAddVitality;

	wCharismaResult = this->Character.Charisma + wAddCharisma;

	g_csItemOption.getAllAddOptionStatesbyCompare(&Character.AddStrength, &Character.AddDexterity, &Character.AddEnergy, &Character.AddVitality, &Character.AddCharisma, wStrengthResult, wDexterityResult, wEnergyResult, wVitalityResult, wCharismaResult);
	g_csItemOption.CheckItemSetOptions();

	if ((this->Equipment[EQUIPMENT_WING].Type + MODEL_ITEM) == MODEL_HELPER + 30)
	{
		PlusSpecial(&Character.AddCharisma, AT_SET_OPTION_IMPROVE_CHARISMA, &this->Equipment[EQUIPMENT_WING]);
	}

	CalculateBasicState();
	CalculateDamage();
	CalculateMagicDamage();
	CalculateCurseDamage();
	CalculateAttackRating();
	CalculateAttackSpeed();
	CalculateSuccessfulBlocking();
	CalculateDefense();
	CalculateMagicDefense();
	CalculateWalkSpeed();
	//CalculateNextExperince();
	CalculateSuccessfulBlockingPK();
	CalculateAttackRatingPK();

	MONSTER_ATTRIBUTE* c = &Enemy.Attribute;
	FinalAttackDamageRight = AttackDamageRight - c->Defense;
	FinalAttackDamageLeft = AttackDamageLeft - c->Defense;
	int EnemyAttackDamage = c->AttackDamageMin + rand() % (c->AttackDamageMax - c->AttackDamageMin + 1);
	FinalHitPoint = EnemyAttackDamage - Character.Defense;
	FinalAttackRating = Character.AttackRating - c->SuccessfulBlocking;
	FinalDefenseRating = Character.SuccessfulBlocking - c->AttackRating;

	if (FinalAttackDamageRight < 0)
		FinalAttackDamageRight = 0;

	if (FinalAttackDamageLeft < 0)
		FinalAttackDamageLeft = 0;

	if (FinalHitPoint < 0)
		FinalHitPoint = 0;

	if (FinalAttackRating < 0)
		FinalAttackRating = 0;
	else if (FinalAttackRating > 100)
		FinalAttackRating = 100;

	if (FinalDefenseRating < 0)
		FinalDefenseRating = 0;
	else if (FinalDefenseRating > 100)
		FinalDefenseRating = 100;

	if (rand() % 100 < FinalAttackRating)
		FinalSuccessAttack = true;
	else
		FinalSuccessAttack = false;

	if (rand() % 100 < FinalDefenseRating)
		FinalSuccessDefense = true;
	else
		FinalSuccessDefense = false;
}

#ifdef SHUTDOWN_CHARACTER_INFO
void CHARACTER_ATTRIBUTE::Init()
{
}

__int64 CHARACTER_ATTRIBUTE::character_experience(CharacterExperience Type, int Character_Level)
{
	__int64 baseExperiencei64 = 0;

	if (Type == LEVEL_DATA_NORMAL)
	{
		if (this->MaxCharacterLevel > MAX_CHARACTER_LEVEL)
		{
			float normalized_level = static_cast<float>(Character_Level) / this->MaxCharacterLevel * (float)MAX_CHARACTER_LEVEL;

			float baseExperience = (9.0f + normalized_level) * normalized_level * normalized_level * 10.0f;

			if (normalized_level > 255.0f)
			{
				float LevelOver_N = normalized_level - 255.0f;
				baseExperience += (9.0f + LevelOver_N) * LevelOver_N * LevelOver_N * 1000.0f;
			}
			baseExperiencei64 = static_cast<DWORD>(baseExperience);
		}
		else
		{
			DWORD LevelExperience = (9.0f + Character_Level) * Character_Level * Character_Level * 10.0f;

			if (Character_Level > 255)
			{
				int LevelOver_N = Character_Level - 255;
				LevelExperience += (9.0f + LevelOver_N) * LevelOver_N * LevelOver_N * 1000.0f;
			}
			baseExperiencei64 = LevelExperience;
		}
	}
	else if (Type == LEVEL_DATA_MASTER)
	{
		__int64 iTotalLevel = Character_Level + MAX_CHARACTER_LEVEL;

		__int64 iTOverLevel = iTotalLevel - 255;

		baseExperiencei64 = ((((iTotalLevel + 9) * iTotalLevel) * iTotalLevel) * 10);

		baseExperiencei64 += ((((iTOverLevel + 9) * iTOverLevel) * iTOverLevel) * 1000);

		baseExperiencei64 = (baseExperiencei64 - 3892250000) / 2;
	}

	return baseExperiencei64;
}

void CHARACTER_ATTRIBUTE::GetMagicDamage(DWORD* _DamageMin, DWORD* _DamageMax)
{
	(*_DamageMin) = this->Work_MagicDamageMin;

	(*_DamageMax) = this->Work_MagicDamageMax;

	(*_DamageMin) += this->Work_SkillMagicDamage;

	(*_DamageMax) += this->Work_SkillMagicDamage + (this->Work_SkillMagicDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulMagicDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulMagicDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivMagicDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivMagicDamage) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetMagicDamage(DWORD* _DamageMin, DWORD* _DamageMax, DWORD* _DamageRate)
{
	(*_DamageMin) = this->Work_MagicDamageMin;

	(*_DamageMax) = this->Work_MagicDamageMax;

	(*_DamageMin) += this->Work_SkillMagicDamage;

	(*_DamageMax) += this->Work_SkillMagicDamage + (this->Work_SkillMagicDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulMagicDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulMagicDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivMagicDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivMagicDamage) * 0.01;

	(*_DamageRate) = ((*_DamageMax) * this->Work_MagicDamageRate) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetPhisyDamage(DWORD* _DamageMin, DWORD* _DamageMax) // OK
{
	(*_DamageMin) = this->Work_PhysiDamageMin;

	(*_DamageMax) = this->Work_PhysiDamageMax;

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivPhysiDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivPhysiDamage) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetSkillDamage(DWORD* _DamageMin, DWORD* _DamageMax) // OK
{
	(*_DamageMin) = this->Work_PhysiDamageMin;

	(*_DamageMax) = this->Work_PhysiDamageMax;

	(*_DamageMin) += this->Work_SkillPhysiDamage;

	(*_DamageMax) += this->Work_SkillPhysiDamage + (this->Work_SkillPhysiDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivPhysiDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivPhysiDamage) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetCurseDamage(DWORD* _DamageMin, DWORD* _DamageMax) // OK
{
	(*_DamageMin) = this->Work_CurseDamageMin;

	(*_DamageMax) = this->Work_CurseDamageMax;

	(*_DamageMin) += this->Work_SkillMagicDamage;

	(*_DamageMax) += this->Work_SkillMagicDamage + (this->Work_SkillMagicDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulCurseDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulCurseDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivCurseDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivCurseDamage) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetCurseDamage(DWORD* _DamageMin, DWORD* _DamageMax, DWORD* _DamageRate) // OK
{
	(*_DamageMin) = this->Work_CurseDamageMin;

	(*_DamageMax) = this->Work_CurseDamageMax;

	(*_DamageMin) += this->Work_SkillMagicDamage;

	(*_DamageMax) += this->Work_SkillMagicDamage + (this->Work_SkillMagicDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulCurseDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulCurseDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivCurseDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivCurseDamage) * 0.01;

	(*_DamageRate) = ((*_DamageMax) * this->Work_CurseDamageRate) * 0.01;
}

void CHARACTER_ATTRIBUTE::GetDarkSpiritDamage(DWORD* _DamageMin, DWORD* _DamageMax) // OK
{
	(*_DamageMin) = this->Work_DarkSpiritAttackDamageMin;

	(*_DamageMax) = this->Work_DarkSpiritAttackDamageMax;
}

void CHARACTER_ATTRIBUTE::GetDarkHorseDamage(DWORD* _DamageMin, DWORD* _DamageMax) // OK
{
	(*_DamageMin) = this->Work_PhysiDamageMin;

	(*_DamageMax) = this->Work_PhysiDamageMax;

	(*_DamageMin) += this->Work_SkillPhysiDamage;

	(*_DamageMax) += this->Work_SkillPhysiDamage + (this->Work_SkillPhysiDamage * 0.5);

	(*_DamageMin) += ((*_DamageMin) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMax) += ((*_DamageMax) * this->Work_MulPhysiDamage) * 0.01;

	(*_DamageMin) -= ((*_DamageMin) * this->Work_DivPhysiDamage) * 0.01;

	(*_DamageMax) -= ((*_DamageMax) * this->Work_DivPhysiDamage) * 0.01;
}
#endif // SHUTDOWN_CHARACTER_INFO