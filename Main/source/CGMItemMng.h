#if !defined(AFX_GMITEMMNG_H__3FF63040_0D46_4FFC_BFD9_86B9CE56C47A__INCLUDED_)
#define AFX_GMITEMMNG_H__3FF63040_0D46_4FFC_BFD9_86B9CE56C47A__INCLUDED_
#pragma once
#define MAX_ITEM_LINE			8192

namespace ItemKind1
{
	enum
	{
		WEAPON = 1,
		PENDANT = 2,
		ARMOR = 3,
		RING = 4,
		PET = 5,
		WINGS = 6,
		JEWEL = 9,
		SKILL = 10,
		EVENT_INVENTORY = 11,
		MUUN_INVENTORY = 12,

		MASTERY_WEAPON = 14,
		MASTERY_ARMOR_1 = 15,
		MASTERY_ARMOR_2 = 18,

		EARRING = 19,
		GUARDIAN_MOUNT = 20,
		SOUL_CONVERTER = 21,
		SOUL_CONVERTER1 = 21,
		SOUL_CONVERTER2 = 22,
		SOUL_CONVERTER3 = 23,
		SOUL_CONVERTER4 = 24,
		PENTAGRAM_OPTION = 25,
		ERRTEL_OPTION = 26,
		MASTERY_SHILED = 27,
	};
};

namespace ItemKind2
{
	enum
	{
		NONE = 0,
		SWORD = 1,
		MAGIC_SWORD = 2,
		FIST = 3, //-- claws RF
		AXE = 4,
		MACE = 5,
		SCEPTER = 6,
		LANCE = 7,
		BOW = 8,
		CROSSBOW = 9,
		ARROW = 10,
		BOLT = 11,
		STAFF = 12,
		STICK = 13,
		BOOK = 14,
		SHIELD = 15,
		HELM = 16,
		ARMOR = 17,
		PANTS = 18,
		GLOVES = 19,
		BOOTS = 20,
		//--
		MINI_WINGS = 22,
		WINGS_LVL_1 = 23,
		WINGS_LVL_2 = 24,
		WINGS_LVL_3 = 25,
		LORD_CAPE = 26,
		CLOAK_LVL_2 = 27,
		MONSTER_WINGS = 28,
		WINGS_CONQUERORS = 60,
		WINGS_ANGEL_AND_DEVIL = 62,
		WINGS_LVL_4 = 76,
		WINGS_POWER = 80,
		//--
		PHYSICAL_PENDANT = 29,
		WIZARD_PENDANT = 30,
		RING = 31,
		TRANSFORMATION_RING_1 = 32,
		TRANSFORMATION_RING_2 = 33,
		SHOP_PENDANT = 34,
		SHOP_RING = 35,
		SKILL_SCROLL_1 = 36,
		SKILL_PARCHMENT = 37,
		SKILL_ORB = 38,
		SKILL_SCROLL_2 = 39,
		SEED = 40,
		SPHERE = 41,
		SEED_SPHERE = 42,
		PENTAGRAM = 43,
		ERRTEL = 44,
		HELPER = 45,
		HELPER_MOUNT = 46,
		INVENTORY_ITEM = 47,
		BUFF_SCROLL = 48,
		BUFF_ELIXIR = 49,
		BUFF_POTION = 50,
		POTION = 51,
		EVENT_TICKET = 53,
		LUCKY_TICKET = 54,
		QUEST = 55,
		JEWEL = 56,
		GEMSTONE = 58,
		REFINING_STONE = 59,
		DECORATION_RING = 61,
		MUUN = 63,
		MUUN_MOUNT = 64,
		WIZARDRY_STONE = 67,
		SUMMON_ITEM = 68,
		EXPANSION_RADIANCE = 70,
		BOX_1 = 71,
		BOX_2 = 72,
		BUFF_TALISMAN = 73,
		BUFF_ITEM = 74,
		QUIVER = 75,
		EARRING = 77,
		RUNIC_MACE = 78,
		GUARDIAN_MOUNT = 79,
		SHORT_SWORD = 81,
		MAGIC_GUN = 84,
		MAGIC_STICK = 89,
		MAGIC_ORB = 90,
	};
};

namespace ItemKind3
{
	enum
	{
		NONE = 0,
		COMMON = 1,
		SOCKET = 2,
		PVP = 3,
		LUCKY = 4,
		BOUND = 5,
		ARCHANGEL_WEAPON = 6,
		CHAOS_WEAPON = 7,
		BLESSED_ARCHANGEL_WEAPON = 8,
		PET_GAIN_EXPERIENCE = 9,
	};
};

namespace Movement
{
	enum
	{
		None = -1,
		Demon = 0,
		Rudolf,
		Panda,
		Unicornio,
		Skeleton,
		Fenrir,
		Horse,
		Uniria,
		MuunPet,
		Angel,
		Satan,
		Dinorant,
		Guardian,
		Muun2Pet,
	};
};

class Script_Item
{
public:
	Script_Item();
	~Script_Item();

/*+000*/ int Type;
/*+004*/ short Index;
/*+006*/ short SubIndex;
/*+528*/ char Name[64];
/*+592*/ BYTE Kind1;
/*+593*/ BYTE Kind2;
/*+594*/ BYTE Kind3;
/*+595*/ bool TwoHand;            // (+30)
/*+596*/ WORD Level;              // (+32)
/*+598*/ BYTE m_byItemSlot;       // (+34)
/*+600*/ WORD m_wSkillIndex;      // (+36)
/*+602*/ BYTE Width;              // (+38)
/*+603*/ BYTE Height;             // (+39)
/*+604*/ WORD DamageMin;          // (+40)
/*+606*/ WORD DamageMax;          // (+41)
/*+608*/ BYTE SuccessfulBlocking; // (+42)
/*+610*/ WORD Defense;            // (+43)
/*+612*/ WORD MagicDefense;       // (+44)
/*+614*/ BYTE WeaponSpeed;        // (+45)
/*+615*/ BYTE WalkSpeed;          // (+46)
/*+616*/ BYTE Durability;         // (+47)
/*+617*/ BYTE MagicDur;           // (+48)
/*+620*/ DWORD MagicPower;        // (+49)
/*+624*/ WORD RequireStrength;    // (+50)
/*+626*/ WORD RequireDexterity;   // (+52)
/*+628*/ WORD RequireEnergy;      // (+54)
/*+630*/ WORD RequireVitality;    // (+56)
/*+632*/ WORD RequireCharisma;    // (+58)
/*+634*/ WORD RequireLevel;       // (+60)
/*+636*/ BYTE Value;              // (+64)
/*+640*/ int iZen;                // (+68)
/*+644*/ BYTE AttType;            // (+72)
/*+645*/ BYTE RequireClass[MAX_CLASS_PLAYER];
/*+659*/ BYTE Resistance[MAX_RESISTANCE];
/*+667*/ bool Drop;
/*+668*/ bool Trade;
/*+669*/ bool StorePersonal;
/*+670*/ bool WhareHouse;
/*+671*/ bool SellNpc;
/*+672*/ bool Expensive;
/*+673*/ bool Repair;
/*+676*/ WORD Overlap;
/*+678*/ WORD PcFlag;
/*+680*/ WORD MuunFlag;
/*+686*/ DWORD PowerATTK;
public:
	BOOL IsKind1(int Kind);
	BOOL IsKind2(int Kind);
	BOOL IsKind3(int Kind);
	bool IsChaosItem();
	bool IsPentagramItem();
	bool IsWeaponDivine();
	bool IsItemSocket();
	bool IsItemCepter();
	bool IsMagicSword();
	bool IsWeaponCros();
	bool IsWeaponBows();
	bool IsWeaponBook();
	bool IsWeaponElf();
	bool IsLuckyItem();
	bool IsEquipmentWing();

	bool IsMonsterWing();
	bool IsItemWingLvl1();
	bool IsItemWingLvl2();
	bool IsItemWingLvl3();
	bool IsItemWingLvl4();
	bool IsItemWingLvl5();

	float Getwidth();
	float Getheight();
	float Getwidth2();
	float Getheight2();

};

class CGMItemMng
{
	typedef std::vector<Script_Item> type_script_item;
public:
	CGMItemMng();
	virtual~CGMItemMng();

	void OpenFile(char* FileName);
	Script_Item* find(int index);
	int GetKind1(int index);
	int GetKind2(int index);
	int GetKind3(int index);
	bool IsLuckyItem(int index);
private:
	type_script_item ItemMemory;
};

extern CGMItemMng GMItemPtr;


#define GMItemMng						(&GMItemPtr)
#endif //AFX_GMITEMMNG_H__3FF63040_0D46_4FFC_BFD9_86B9CE56C47A__INCLUDED_