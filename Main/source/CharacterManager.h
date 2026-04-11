// CharacterManager.h: interface for the CCharacterManager class.
//////////////////////////////////////////////////////////////////////
#pragma once


class CCharacterManager
{
public:
	CCharacterManager();
	virtual ~CCharacterManager();
	BYTE ChangeServerClassTypeToClientClassType(const BYTE byServerClassType);
	BYTE GetSkinModelIndex(const BYTE byClass);

	BYTE GetBaseClass(const BYTE byClass);
	bool IsSecondClass(const BYTE byClass);
	bool IsThirdClass(const BYTE byClass);
	bool IsFourthClass(const BYTE byClass);
	bool IsMasterLevel(const BYTE byClass);
	bool IsFemale(const BYTE byClass);
	bool CharacterIsEvo3(const BYTE byClass);
	BYTE GetCharacterClass(const BYTE byClass);
	BYTE GetStepClass(const BYTE byClass);
	BYTE GetTextureHQCharacterClass(unsigned __int8 byClass);
	const char* GetCharacterClassText(const BYTE byClass);

	int GetEquipedBowType(CHARACTER *pChar);
	int GetEquipedBowType();
	int GetEquipedBowType(ITEM* pItem);
	int GetEquipedBowType_Skill( );
	bool IsEquipedWing();
	void GetMagicSkillDamage( int iType, int *piMinDamage, int *piMaxDamage);
	void GetCurseSkillDamage(int iType, int *piMinDamage, int *piMaxDamage);
	void GetSkillDamage( int iType, int *piMinDamage, int *piMaxDamage);
public:

};

extern CCharacterManager gCharacterManager;
