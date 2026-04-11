// SkillManager.cpp: implementation of the CSkillManager class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillManager.h"
#include "CharacterManager.h"
#include "MapManager.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "NewUISystem.h"

CSkillManager gSkillManager;

CSkillManager::CSkillManager() // OK
{
}

CSkillManager::~CSkillManager() // OK
{
}

bool CSkillManager::FindHeroSkill(ActionSkillType eSkillType)
{
	for (int i = 0; i < CharacterAttribute->SkillNumber; ++i)
	{
		if (CharacterAttribute->Skill[i] == eSkillType)
		{
			return true;
		}
	}
	return false;
}

void CSkillManager::GetSkillInformation(int iType, int iLevel, char* lpszName, int* piMana, int* piDistance, int* piSkillMana)
{
	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];
	if (lpszName)
	{
		strcpy(lpszName, p->Name);
	}
	if (piMana)
	{
#if MAIN_UPDATE > 303
		if (iType >= Skill_Durability_Reduction && iType <= Skill_Increase_Triple_Damage_Chance)
		{
			static double level_var[20] = {
				1.0331,
				1.0453,
				1.0566,
				1.0671999,
				1.077,
				1.086,
				1.0943,
				1.1019,
				1.1089,
				1.1152,
				1.1209,
				1.126,
				1.1306,
				1.1346999,
				1.1382999,
				1.1415,
				1.1442,
				1.1466,
				1.1486,
				1.1502
			};
			*piMana = p->Mana * level_var[g_pMasterSkillTree->GetSkillLevel(iType)];
		}
		else
		{
			*piMana = p->Mana;
		}
#else
		* piMana = p->Mana;
#endif // MAIN_UPDATE != 303
	}
	if (piDistance)
	{
		*piDistance = p->Distance;
	}
	if (piSkillMana)
	{
		*piSkillMana = p->AbilityGuage;
	}
}

void CSkillManager::GetSkillInformation_Energy(int iType, int* piEnergy)
{
	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	if (piEnergy)
	{
		if (p->Energy == 0)
		{
			*piEnergy = 0;
		}
		else
		{
			*piEnergy = 20 + (p->Energy * p->Level * 4 / 100);

			if (iType == AT_SKILL_SUMMON_EXPLOSION || iType == AT_SKILL_SUMMON_REQUIEM) {
				*piEnergy = 20 + (p->Energy * p->Level * 3 / 100);
			}

			if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_KNIGHT) {
				*piEnergy = 10 + (p->Energy * p->Level * 4 / 100);
			}
		}
	}

}

void CSkillManager::GetSkillInformation_Charisma(int iType, int* piCharisma)
{

	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	if (piCharisma)
	{
		*piCharisma = p->Charisma;
	}

}

void CSkillManager::GetSkillInformation_Damage(int iType, int* piDamage)
{

	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	if (piDamage)
	{
		*piDamage = p->Damage;
	}
}

float CSkillManager::GetSkillDistance(int Index, CHARACTER* c)
{
	float Distance = (float)(SkillAttribute[Index].Distance);

	if (c != NULL && c->Helper.Type == (MODEL_HELPER + 4))
	{
		Distance += 2.0;
	}

	return Distance;
}

bool CSkillManager::CheckSkillDelay(int SkillIndex)
{
	int Skill = CharacterAttribute->Skill[SkillIndex];

	int Delay = SkillAttribute[Skill].Delay;

	if (!CheckAttack() && (Skill == AT_SKILL_GIANTSWING || Skill == AT_SKILL_DRAGON_LOWER || Skill == AT_SKILL_DRAGON_KICK
		|| Skill == Skill_Phoenix_Shot))
	{
		return false;
	}

	if (Delay > 0)
	{
		if (CharacterAttribute->SkillDelay[SkillIndex] > 0)
		{
			return false;
		}

		int iCharisma;
		GetSkillInformation_Charisma(Skill, &iCharisma);
		if ((unsigned int)iCharisma > (CharacterAttribute->Charisma + CharacterAttribute->AddCharisma))
		{
			return false;
		}

		CharacterAttribute->SkillDelay[SkillIndex] = Delay;
	}
	return true;
}

void CSkillManager::CalcSkillDelay(int time)
{
	int iSkillNumber;

	if ((signed int)CharacterAttribute->SkillNumber >= 650)
		iSkillNumber = 650;
	else
		iSkillNumber = CharacterAttribute->SkillNumber;

	for (int i = 0; i < iSkillNumber; ++i)
	{
		if (CharacterAttribute->SkillDelay[i] > 0)
		{
			CharacterAttribute->SkillDelay[i] -= time;
			if (CharacterAttribute->SkillDelay[i] <= 0)
			{
				CharacterAttribute->SkillDelay[i] = 0;
			}
		}
	}
}

bool CSkillManager::IsMultiTargetSkill(int SkillIndex)
{
	return SkillIndex == Skill_Nova 
		|| SkillIndex == Skill_Nova_Strengthener
		|| SkillIndex == Skill_Beast_Uppercut
		|| SkillIndex == Skill_Beast_Uppercut_Strengthener
		|| SkillIndex == Skill_Beast_Uppercut_Mastery
		|| SkillIndex == Skill_Dark_Side
		|| SkillIndex == Skill_Dark_Side_Strengthener;
}

BYTE CSkillManager::GetSkillMasteryType(int iType)
{
	BYTE MasteryType = 255;
	SKILL_ATTRIBUTE* p = &SkillAttribute[iType];

	MasteryType = p->MasteryType;

	return MasteryType;
}

int CSkillManager::MasterSkillToBaseSkillIndex(int iMasterSkillIndex)
{
	return iMasterSkillIndex;
}

bool CSkillManager::skillVScharactorCheck(const DemendConditionInfo& basicInfo, const DemendConditionInfo& heroInfo)
{
	if (basicInfo <= heroInfo)
	{
		return true;
	}
	return false;
}

bool CSkillManager::DemendConditionCheckSkill(WORD SkillType)
{
	if (SkillType >= MAX_SKILLS)
	{
		return false;
	}

	if ((true == gMapManager->IsEmpireGuardian()) && (SkillType == AT_SKILL_TELEPORT_B || SkillType == AT_SKILL_TELEPORT))
	{
		return false;
	}

	if (SkillAttribute[SkillType].Energy == 0)
	{
		return true;
	}

	SkillType = MasterSkillToBaseSkillIndex(SkillType);

	bool result = true;

	DemendConditionInfo BasicCharacterInfo;

	BasicCharacterInfo.SkillLevel = SkillAttribute[SkillType].Level;
	BasicCharacterInfo.SkillStrength = SkillAttribute[SkillType].Strength;
	BasicCharacterInfo.SkillDexterity = SkillAttribute[SkillType].Dexterity;
	BasicCharacterInfo.SkillVitality = 0;
	BasicCharacterInfo.SkillEnergy = (20 + (SkillAttribute[SkillType].Energy * SkillAttribute[SkillType].Level) * 0.04);
	BasicCharacterInfo.SkillCharisma = SkillAttribute[SkillType].Charisma;

	DemendConditionInfo HeroCharacterInfo;

	HeroCharacterInfo.SkillLevel = CharacterMachine->Character.Level;
	HeroCharacterInfo.SkillStrength = CharacterMachine->Character.Strength + CharacterMachine->Character.AddStrength;
	HeroCharacterInfo.SkillDexterity = CharacterMachine->Character.Dexterity + CharacterMachine->Character.AddDexterity;
	HeroCharacterInfo.SkillVitality = CharacterMachine->Character.Vitality + CharacterMachine->Character.AddVitality;
	HeroCharacterInfo.SkillEnergy = CharacterMachine->Character.Energy + CharacterMachine->Character.AddEnergy;
	HeroCharacterInfo.SkillCharisma = CharacterMachine->Character.Charisma + CharacterMachine->Character.AddCharisma;

	result = skillVScharactorCheck(BasicCharacterInfo, HeroCharacterInfo);

	return result;
}

