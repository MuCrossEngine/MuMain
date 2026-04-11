#include "stdafx.h"
#include "MonkSystem.h"
#include "CGMItemMng.h"

CGMItemMng GMItemPtr;

Script_Item::Script_Item()
{
	Type = -1;
	Index = -1;
	SubIndex = -1;
	Kind1 = 0;
	Kind2 = 0;
	Kind3 = 0;
	TwoHand = false;
	Level = 0;
	m_byItemSlot = -1;
	m_wSkillIndex = 0;
	Width = 0;
	Height = 0;
	DamageMin = 0;
	DamageMax = 0;
	SuccessfulBlocking = 0;
	Defense = 0;
	MagicDefense = 0;
	WeaponSpeed = 0;
	WalkSpeed = 0;
	Durability = 0;
	MagicDur = 0;
	MagicPower = 0;
	RequireStrength = 0;
	RequireDexterity = 0;
	RequireEnergy = 0;
	RequireVitality = 0;
	RequireCharisma = 0;
	RequireLevel = 0;
	Value = 0;
	iZen = 0;
	AttType = 0;

	Drop = false;
	Trade = false;
	StorePersonal = false;
	WhareHouse = false;
	SellNpc = false;
	Expensive = false;
	Repair = false;
	Overlap = 0;
	PcFlag = 0;
	MuunFlag = 0;
	PowerATTK = 0;

	memset(Name, 0, sizeof(Name));
	memset(Resistance, 0, sizeof(Resistance));
	memset(RequireClass, 0, sizeof(RequireClass));
}

Script_Item::~Script_Item()
{
}

BOOL Script_Item::IsKind1(int Kind)
{
	return this->Kind1 == Kind;
}

BOOL Script_Item::IsKind2(int Kind)
{
	return this->Kind2 == Kind;
}

BOOL Script_Item::IsKind3(int Kind)
{
	return this->Kind3 == Kind;
}

bool Script_Item::IsChaosItem()
{
	return IsKind3(ItemKind3::CHAOS_WEAPON);
}

bool Script_Item::IsPentagramItem()
{
	return IsKind1(ItemKind1::PENTAGRAM_OPTION);
}

bool Script_Item::IsWeaponDivine()
{
	return IsKind3(ItemKind3::ARCHANGEL_WEAPON) || IsKind3(ItemKind3::BLESSED_ARCHANGEL_WEAPON);
}

bool Script_Item::IsItemSocket()
{
	return IsKind3(ItemKind3::SOCKET);
}

bool Script_Item::IsItemCepter()
{
	return IsKind2(ItemKind2::SCEPTER);
}

bool Script_Item::IsMagicSword()
{
	return IsKind2(ItemKind2::MAGIC_SWORD);
}

bool Script_Item::IsWeaponCros()
{
	return IsKind2(ItemKind2::CROSSBOW);
}

bool Script_Item::IsWeaponBows()
{
	return IsKind2(ItemKind2::BOW);
}

bool Script_Item::IsWeaponBook()
{
	return IsKind2(ItemKind2::BOOK);
}

bool Script_Item::IsWeaponElf()
{
	return IsKind2(ItemKind2::ARROW) || IsKind2(ItemKind2::BOLT) || IsKind2(ItemKind2::QUEST);
}

bool Script_Item::IsLuckyItem()
{
	return IsKind3(ItemKind3::LUCKY);
}

bool Script_Item::IsEquipmentWing()
{
	if (IsKind2(ItemKind2::MINI_WINGS) || IsKind2(ItemKind2::MONSTER_WINGS)
		|| IsKind2(ItemKind2::WINGS_LVL_1) || IsKind2(ItemKind2::WINGS_LVL_2)
		|| IsKind2(ItemKind2::WINGS_LVL_3) || IsKind2(ItemKind2::WINGS_LVL_4)
		|| IsKind2(ItemKind2::WINGS_POWER) || IsKind2(ItemKind2::WINGS_CONQUERORS)
		|| IsKind2(ItemKind2::WINGS_ANGEL_AND_DEVIL)
		|| IsKind2(ItemKind2::LORD_CAPE) || IsKind2(ItemKind2::CLOAK_LVL_2)
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Script_Item::IsMonsterWing()
{
	return IsKind2(ItemKind2::MONSTER_WINGS);
}

bool Script_Item::IsItemWingLvl1()
{
	return IsKind2(ItemKind2::WINGS_LVL_1);
}

bool Script_Item::IsItemWingLvl2()
{
	return IsKind2(ItemKind2::WINGS_LVL_2);
}

bool Script_Item::IsItemWingLvl3()
{
	return IsKind2(ItemKind2::WINGS_LVL_3);
}

bool Script_Item::IsItemWingLvl4()
{
	return IsKind2(ItemKind2::WINGS_LVL_4);
}

bool Script_Item::IsItemWingLvl5()
{
	return false;
}

float Script_Item::Getwidth()
{
	return (this->Width * 20);
}

float Script_Item::Getheight()
{
	return (this->Height * 20);
}

float Script_Item::Getwidth2()
{
	return (this->Width * 30.f);
}

float Script_Item::Getheight2()
{
	return (this->Height * 30.f);
}


CGMItemMng::CGMItemMng() : ItemMemory(MAX_ITEM_LINE)
{
}

CGMItemMng::~CGMItemMng()
{
	ItemMemory.clear();
}


void CGMItemMng::OpenFile(char* FileName)
{
	BYTE* Pack = NULL;
	int Size = sizeof(Script_Item);

	int MaxLine = PackFileDecrypt(FileName, &Pack, 0, Size, 0xE2F1);

	if (MaxLine != 0 && Pack)
	{
		BYTE* pSeek = Pack;
		for (int i = 0; i < MaxLine; i++)
		{
			int index = *(int*)pSeek;
			Script_Item info = (*(Script_Item*)pSeek);
			ItemMemory[index] = info;

			if (info.Type >= ITEM_SWORD + 36 && info.Type < ITEM_AXE && info.Kind2 == ItemKind2::FIST)
			{
				g_CMonkSystem.RegistItem(info.Type);
			}
			pSeek += Size;
		}
	}
	g_CMonkSystem.SetSwordformGlovesItemType();
	SAFE_DELETE_ARRAY(Pack);
}

Script_Item* CGMItemMng::find(int index)
{
	if (index >= 0 && index < MAX_ITEM_LINE)
	{
		return &ItemMemory[index];
	}
	else
	{
		static Script_Item pItem;
		return &pItem;
	}
}

int CGMItemMng::GetKind1(int index)
{
	return this->find(index)->Kind1;
}

int CGMItemMng::GetKind2(int index)
{
	return this->find(index)->Kind2;
}

int CGMItemMng::GetKind3(int index)
{
	return this->find(index)->Kind3;
}

bool CGMItemMng::IsLuckyItem(int index)
{
	return this->find(index)->IsLuckyItem();
}
