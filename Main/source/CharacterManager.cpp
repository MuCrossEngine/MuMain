// CharacterManager.cpp: implementation of the CCharacterManager class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CharacterManager.h"
#include "CSItemOption.h"
#include "GlobalText.h"
#include "SkillManager.h"
#include "ZzzInfomation.h"
#include "./Utilities/Log/ErrorReport.h"
#include "./Utilities/Log/muConsoleDebug.h"

CCharacterManager gCharacterManager;

CCharacterManager::CCharacterManager() // OK
{
}

CCharacterManager::~CCharacterManager() // OK
{
}

BYTE CCharacterManager::GetBaseClass(const BYTE byClass)
{
	return (byClass & 0xF);
}

BYTE CCharacterManager::ChangeServerClassTypeToClientClassType(const BYTE byServerClassType)
{
	return ((((int)byServerClassType >> 1) & 1) << 6) | (32 * (((int)byServerClassType >> 2) & 1)) | ((int)byServerClassType >> 4) | (16 * (((int)byServerClassType >> 3) & 1));
}

bool CCharacterManager::IsSecondClass(const BYTE byClass)
{
	return (((int)byClass >> 4) & 1) != 0;
}

bool CCharacterManager::IsThirdClass(const BYTE byClass)
{
	return (((int)byClass >> 5) & 1) != 0;
}

bool CCharacterManager::IsFourthClass(const BYTE byClass)
{
	return (((int)byClass >> 6) & 1) != 0;
}

BYTE CCharacterManager::GetStepClass(const BYTE byClass)
{
	if (IsFourthClass(byClass))
		return 4;
	if (IsThirdClass(byClass))
		return 3;
	if (IsSecondClass(byClass))
		return 2;
	return 1;
}

const char* CCharacterManager::GetCharacterClassText(const BYTE byClass)
{
	BYTE byCharacterClass = this->GetCharacterClass(byClass);

	switch (byCharacterClass)
	{
	case Dark_Wizard: //-- Soul Master
		return GlobalText[20];
	case Soul_Master: //-- Soul Master
		return GlobalText[25];
	case Grand_Master: //-- Grand Master
		return GlobalText[1669];
	case Soul_Wizard: //-- Grand Master
		return GlobalText[3186];
	case Dark_Knight: //-- Dark Knight
		return GlobalText[21];
	case Blade_Knight: //-- Blade Knight
		return GlobalText[26];
	case Blade_Master: //-- Blade Master
		return GlobalText[1668];
	case Dragon_Knight: //-- Blade Master
		return GlobalText[3187];
	case Fairy_Elf: //-- Elf
		return GlobalText[22];
	case Muse_Elf: //-- Muse Elf
		return GlobalText[27];
	case High_Elf: //-- High Elf
		return GlobalText[1670];
	case Noble_Elves: //-- High Elf
		return GlobalText[3188];
	case Magic_Gladiator: //-- Magic Gladiator
		return GlobalText[23];
	case Duel_Master: //-- Dual Master
		return GlobalText[1671];
	case Magic_Knight: //-- Dual Master
		return GlobalText[3189];
	case Dark_Lord: //-- Dark Lord
		return GlobalText[24];
	case Lord_Emperor: //-- Lord Emperor
		return GlobalText[1672];
	case Empire_Road: //-- Lord Emperor
		return GlobalText[3190];
	case Summoner: //-- Summoner
		return GlobalText[1687];
	case Bloody_Summoner: //-- Bloody Summoner
		return GlobalText[1688];
	case Dimension_Master: //-- Dimension Master
		return GlobalText[1689];
	case Dimension_Summoner: //-- Dimension Master
		return GlobalText[3191];
	case Rage_Fighter: //-- Rage Fighter
		return GlobalText[3150];
	case Fist_Master: //-- Fist Master
		return GlobalText[3151];
	case Fist_Blazer: //-- Fist Master
		return GlobalText[3192];
	case Grow_Lancer: //-- Grow Lancer
		return GlobalText[3175];
	case Mirage_Lancer: //-- Mirage Lancer
		return GlobalText[3176];
	case Shining_Lancer: //-- Shining Lancer
		return GlobalText[3177];
	case Runer_Wizzard:
		return GlobalText[3179];
	case Rune_Spell:
		return GlobalText[3181];
	case Rune_Grand:
		return GlobalText[3182];
	case Majestic_Rune:
		return GlobalText[3183];
	case Slayer:
		return GlobalText[3193];
	case Royal_Slayer:
		return GlobalText[3194];
	case Master_Slayer:
		return GlobalText[3195];
	case Slaughterer:
		return GlobalText[3196];
	case Gun_Crusher:
		return GlobalText[3200];
	case Gun_Breaker:
		return GlobalText[3201];
	case Master_Gun_Breaker:
		return GlobalText[3202];
	case Heist_Gun_Crasher:
		return GlobalText[3203];
	case Mage_Kundun:
		return GlobalText[3218];
	case Light_Master:
		return GlobalText[3219];
	case Shine_Wizard:
		return GlobalText[3220];
	case Luminous_Wizard:
		return GlobalText[3221];
	case Mage_Lemuria:
		return GlobalText[3213];
	case Wo_Mage:
		return GlobalText[3214];
	case Arch_Mage:
		return GlobalText[3215];
	case Mystic_Mage:
		return GlobalText[3216];
	case Illusion_Knight:
		return GlobalText[3209];
	case Mirage_Knight:
		return GlobalText[3210];
	case Illusion_Master:
		return GlobalText[3211];
	case Mystic_Knight:
		return GlobalText[3212];
	default:
		return GlobalText[2305]; //-- Error Code :
		break;
	}
	return GlobalText[2305]; //-- Error Code :
}

BYTE CCharacterManager::GetSkinModelIndex(const BYTE byClass)
{
	int CharacterClass;

	if (CharacterIsEvo3(byClass))
		CharacterClass = MAX_CLASS_PLAYER * ((((int)byClass >> 6) & 1) + 2 * (((int)byClass >> 5) & 1));
	else
		CharacterClass = MAX_CLASS_PLAYER * ((((int)byClass >> 6) & 1) + (((int)byClass >> 5) & 1) + (((int)byClass >> 4) & 1));

	CharacterClass += (byClass & 0xF);

	return CharacterClass;
}

bool CCharacterManager::IsMasterLevel(const BYTE byClass)
{
	return this->IsThirdClass(byClass) || this->IsFourthClass(byClass);
}

bool CCharacterManager::IsFemale(const BYTE byClass)
{
	BYTE byFirstClass = this->GetBaseClass(byClass);

	return byFirstClass == Fairy_Elf
		|| byFirstClass == Summoner
		|| byFirstClass == Grow_Lancer
		|| byFirstClass == Runer_Wizzard
		|| byFirstClass == Slayer
		|| byFirstClass == Mage_Lemuria
		|| byFirstClass == Alquimista1;
}

bool CCharacterManager::CharacterIsEvo3(const BYTE byClass)
{
	BYTE byFirstClass = this->GetBaseClass(byClass);

	return byFirstClass == Magic_Gladiator
		|| byFirstClass == Dark_Lord
		|| byFirstClass == Rage_Fighter
		|| byFirstClass == Grow_Lancer;
}

BYTE CCharacterManager::GetCharacterClass(const BYTE byClass)
{
	unsigned __int8 byCharacterClass = 0;
	unsigned __int8 byFirstClass  = (byClass & 0xF);
	unsigned __int8 bySecondClass = ((signed int)byClass >> 4) & 1;
	unsigned __int8 byThirdClass  = ((signed int)byClass >> 5) & 1;
	unsigned __int8 byFourthClass = ((signed int)byClass >> 6) & 1;

	switch (byFirstClass)
	{
	case Dark_Wizard:
		if (byFourthClass)
		{
			byCharacterClass = Soul_Wizard;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Grand_Master;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Soul_Master;
		}
		else
		{
			byCharacterClass = Dark_Wizard;
		}
		break;
	case Dark_Knight:
		if (byFourthClass)
		{
			byCharacterClass = Dragon_Knight;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Blade_Master;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Blade_Knight;
		}
		else
		{
			byCharacterClass = Dark_Knight;
		}
		break;
	case Fairy_Elf:
		if (byFourthClass)
		{
			byCharacterClass = Noble_Elves;
		}
		else if (byThirdClass)
		{
			byCharacterClass = High_Elf;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Muse_Elf;
		}
		else
		{
			byCharacterClass = Fairy_Elf;
		}
		break;
	case Magic_Gladiator:
		if (byFourthClass)
		{
			byCharacterClass = Magic_Knight;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Duel_Master;
		}
		else
		{
			byCharacterClass = Magic_Gladiator;
		}
		break;
	case Dark_Lord:
		if (byFourthClass)
		{
			byCharacterClass = Empire_Road;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Lord_Emperor;
		}
		else
		{
			byCharacterClass = Dark_Lord;
		}
		break;
	case Summoner:
		if (byFourthClass)
		{
			byCharacterClass = Dimension_Summoner;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Dimension_Master;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Bloody_Summoner;
		}
		else
		{
			byCharacterClass = Summoner;
		}
		break;
	case Rage_Fighter:
		if (byFourthClass)
		{
			byCharacterClass = Fist_Blazer;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Fist_Master;
		}
		else
		{
			byCharacterClass = Rage_Fighter;
		}
		break;
	case Grow_Lancer:
		if (byFourthClass)
		{
			byCharacterClass = Shining_Lancer;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Mirage_Lancer;
		}
		else
		{
			byCharacterClass = Grow_Lancer;
		}
		break;
	case Runer_Wizzard:
		if (byFourthClass)
		{
			byCharacterClass = Majestic_Rune;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Rune_Grand;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Rune_Spell;
		}
		else
		{
			byCharacterClass = Runer_Wizzard;
		}
		break;
	case Slayer:
		if (byFourthClass)
		{
			byCharacterClass = Slaughterer;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Master_Slayer;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Royal_Slayer;
		}
		else
		{
			byCharacterClass = Slayer;
		}
		break;
	case Gun_Crusher:
		if (byFourthClass)
		{
			byCharacterClass = Heist_Gun_Crasher;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Master_Gun_Breaker;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Gun_Breaker;
		}
		else
		{
			byCharacterClass = Gun_Crusher;
		}
		break;

	case Mage_Kundun:
		if (byFourthClass)
		{
			byCharacterClass = Luminous_Wizard;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Shine_Wizard;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Light_Master;
		}
		else
		{
			byCharacterClass = Mage_Kundun;
		}
		break;
	case Mage_Lemuria:
		if (byFourthClass)
		{
			byCharacterClass = Mystic_Mage;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Arch_Mage;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Wo_Mage;
		}
		else
		{
			byCharacterClass = Mage_Lemuria;
		}
		break;
	case Illusion_Knight:
		if (byFourthClass)
		{
			byCharacterClass = Mystic_Knight;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Illusion_Master;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Mirage_Knight;
		}
		else
		{
			byCharacterClass = Illusion_Knight;
		}
		break;
	case Alquimista1:
		if (byFourthClass)
		{
			byCharacterClass = Alquimista4;
		}
		else if (byThirdClass)
		{
			byCharacterClass = Alquimista3;
		}
		else if (bySecondClass)
		{
			byCharacterClass = Alquimista2;
		}
		else
		{
			byCharacterClass = Alquimista1;
		}
		break;
	}
	return byCharacterClass;
}

int CCharacterManager::GetEquipedBowType(CHARACTER* pChar)
{
	int weapon01 = (pChar->Weapon[0].Type - MODEL_ITEM);
	int weaponkind = GMItemMng->GetKind2(weapon01);

	if (weaponkind == ItemKind2::BOW)
	{
		return BOWTYPE_BOW;
	}
	else if (weaponkind == ItemKind2::CROSSBOW)
	{
		return BOWTYPE_CROSSBOW;
	}

	return BOWTYPE_NONE;
}

int CCharacterManager::GetEquipedBowType()
{
	int weapon01 = (CharacterMachine->Equipment[0].Type);
	int weaponkind = GMItemMng->GetKind2(weapon01);

	if (weaponkind == ItemKind2::BOW)
	{
		return BOWTYPE_BOW;
	}
	else if (weaponkind == ItemKind2::CROSSBOW)
	{
		return BOWTYPE_CROSSBOW;
	}

	return BOWTYPE_NONE;
}

int CCharacterManager::GetEquipedBowType(ITEM* pItem)
{
	int weaponkind = GMItemMng->GetKind2(pItem->Type);

	if (weaponkind == ItemKind2::BOW)
	{
		return BOWTYPE_BOW;
	}
	else if (weaponkind == ItemKind2::CROSSBOW)
	{
		return BOWTYPE_CROSSBOW;
	}

	return BOWTYPE_NONE;
}

int CCharacterManager::GetEquipedBowType_Skill()
{
	int Weapon01Kind2 = GMItemMng->GetKind2((CharacterMachine->Equipment[0].Type));
	int Weapon02Kind2 = GMItemMng->GetKind2((CharacterMachine->Equipment[1].Type));

	if (Weapon02Kind2 == ItemKind2::ARROW || Weapon02Kind2 == ItemKind2::BOLT || Weapon02Kind2 == ItemKind2::QUIVER)
	{
		if (Weapon01Kind2 == ItemKind2::BOW && (Weapon02Kind2 == ItemKind2::ARROW || Weapon02Kind2 == ItemKind2::QUIVER))
		{
			return BOWTYPE_BOW;
		}
		else if (Weapon01Kind2 == ItemKind2::CROSSBOW && (Weapon02Kind2 == ItemKind2::BOLT || Weapon02Kind2 == ItemKind2::QUIVER))
		{
			return BOWTYPE_CROSSBOW;
		}
	}

	return BOWTYPE_NONE;
}

bool CCharacterManager::IsEquipedWing()
{
	ITEM* pEquippedItem = &CharacterMachine->Equipment[EQUIPMENT_WING];

	if ((pEquippedItem->Type >= ITEM_WING && pEquippedItem->Type <= ITEM_WING + 6)
		|| (pEquippedItem->Type >= ITEM_WING + 36 && pEquippedItem->Type <= ITEM_WING + 43)
		|| (pEquippedItem->Type == ITEM_HELPER + 30)
		|| (ITEM_WING + 130 <= pEquippedItem->Type && pEquippedItem->Type <= ITEM_WING + 134)
		|| (pEquippedItem->Type >= ITEM_WING + 49 && pEquippedItem->Type <= ITEM_WING + 50) || (pEquippedItem->Type == ITEM_WING + 135)
		)
	{
		return true;
	}

	return false;
}

void CCharacterManager::GetMagicSkillDamage(int iType, int* piMinDamage, int* piMaxDamage)
{
	if (AT_SKILL_SUMMON_EXPLOSION <= iType && iType <= AT_SKILL_SUMMON_REQUIEM)
	{
		*piMinDamage = CharacterMachine->Character.MagicDamageMin;
		*piMaxDamage = CharacterMachine->Character.MagicDamageMax;
		return;
	}

	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	int Damage = p->Damage;

	*piMinDamage = CharacterMachine->Character.MagicDamageMin + Damage;
	*piMaxDamage = CharacterMachine->Character.MagicDamageMax + Damage + Damage / 2;

	Damage = 0;
	g_csItemOption.PlusSpecial((DWORD*)&Damage, AT_SET_OPTION_IMPROVE_MAGIC_POWER);
	if (Damage != 0)
	{
		float fratio = 1.f + (float)Damage / 100.f;
		*piMinDamage *= fratio;
		*piMaxDamage *= fratio;
	}

	Damage = 0;
	g_csItemOption.ClearListOnOff();
	g_csItemOption.PlusMastery(&Damage, p->MasteryType);
	g_csItemOption.PlusSpecial((DWORD*)&Damage, AT_SET_OPTION_IMPROVE_SKILL_ATTACK);
	*piMinDamage += Damage;
	*piMaxDamage += Damage;
}

void CCharacterManager::GetCurseSkillDamage(int iType, int* piMinDamage, int* piMaxDamage)
{
	if (CLASS_SUMMONER != gCharacterManager.GetBaseClass(CharacterMachine->Character.Class))
		return;

	if (AT_SKILL_SUMMON_EXPLOSION <= iType && iType <= AT_SKILL_SUMMON_REQUIEM)
	{
		SKILL_ATTRIBUTE* p = &SkillAttribute[iType];
		*piMinDamage = CharacterMachine->Character.CurseDamageMin + p->Damage;
		*piMaxDamage = CharacterMachine->Character.CurseDamageMax + p->Damage + p->Damage / 2;
	}
	else
	{
		*piMinDamage = CharacterMachine->Character.CurseDamageMin;
		*piMaxDamage = CharacterMachine->Character.CurseDamageMax;
	}
}

void CCharacterManager::GetSkillDamage(int iType, int* piMinDamage, int* piMaxDamage)
{
	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	int Damage = p->Damage;

	*piMinDamage = Damage;
	*piMaxDamage = Damage + Damage / 2;

	Damage = 0;
	g_csItemOption.ClearListOnOff();
	g_csItemOption.PlusMastery(&Damage, p->MasteryType);
	g_csItemOption.PlusSpecial((DWORD*)&Damage, AT_SET_OPTION_IMPROVE_SKILL_ATTACK);
	*piMinDamage += Damage;
	*piMaxDamage += Damage;
}