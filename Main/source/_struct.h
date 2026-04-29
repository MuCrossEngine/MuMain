#ifndef ___STRUCT_H__
#define ___STRUCT_H__

#pragma once

#include "w_Buff.h"

#include "w_ObjectInfo.h"
class OBJECT;
#include "w_CharacterInfo.h"
class CHARACTER;

namespace SEASON3B
{
	class CNewUIItemMng;


	typedef struct tagEQUIPMENT_ITEM
	{
		int index;
		int x;
		int y;
		int width;
		int height;
		DWORD dwBgImage;
		BOOL CheckRender;
	} EQUIPMENT_ITEM;
}

#define MAX_MONSTERSKILL_NUM	10

namespace info
{
	struct Script_Item
	{
		char Name[30];
		bool TwoHand;
		WORD Level;
		BYTE m_byItemSlot;
		BYTE m_bySkillIndex;
		BYTE Width;
		BYTE Height;
		BYTE DamageMin;
		BYTE DamageMax;
		BYTE SuccessfulBlocking;
		BYTE Defense;
		BYTE MagicDefense;
		BYTE WeaponSpeed;
		BYTE WalkSpeed;
		BYTE Durability;
		BYTE MagicDur;
		BYTE MagicPower;
		WORD RequireStrength;
		WORD RequireDexterity;
		WORD RequireEnergy;
		WORD  RequireVitality;
		WORD RequireCharisma;
		WORD RequireLevel;
		BYTE Value;
		int  iZen;
		BYTE  AttType;
		BYTE RequireClass[MAX_CLASS];
		BYTE Resistance[MAX_RESISTANCE];
	};
	
	struct Script_Silde 
	{
		typedef std::vector<char*>     SildeVECTOR;
		SildeVECTOR					   Sildelist;
	};

	struct Script_Dialog
	{
		char m_lpszText[MAX_LENGTH_DIALOG];
		int m_iNumAnswer;
		int m_iLinkForAnswer[MAX_ANSWER_FOR_DIALOG];
		int m_iReturnForAnswer[MAX_ANSWER_FOR_DIALOG];
		char m_lpszAnswer[MAX_ANSWER_FOR_DIALOG][MAX_LENGTH_ANSWER];
	};
	
	struct Script_Credit
	{
		BYTE	byClass;
		char	szName[32];
		//char	szName[CRW_NAME_MAX];
	};

	struct Script_Movereq
	{
		int index;
		char szMainMapName[32];			//. Main map name
		char szSubMapName[32];			//. Substitute map name
		int	iReqLevel;					//. required level
		int iReqZen;					//. required zen
		int iGateNum;					//. Gate number
	};

	struct Script_Quest_Class_Act
	{
		BYTE    chLive;
		BYTE    byQuestType;
		WORD    wItemType;
		BYTE    byItemSubType;
		BYTE	byItemLevel;
		BYTE    byItemNum;
		BYTE    byRequestType;
		BYTE    byRequestClass[MAX_CLASS];
		short   shQuestStartText[4];
	};

	struct Script_Quest_Class_Request
	{
		BYTE    byLive;
		BYTE    byType;
		WORD    wCompleteQuestIndex;
		WORD    wLevelMin;
		WORD    wLevelMax;
		WORD	wRequestStrength;
		DWORD   dwZen;
		short   shErrorText;
	};

	struct Script_Quest
	{
		short   shQuestConditionNum;
		short   shQuestRequestNum;
		WORD    wNpcType;
		char    strQuestName[32];

		Script_Quest_Class_Act     QuestAct[MAX_QUEST_CONDITION];
		Script_Quest_Class_Request QuestRequest[MAX_QUEST_REQUEST];
	};
};

//infomation start
typedef struct
{
	BYTE Flag;
	BYTE Map;
	BYTE x1;
	BYTE y1;
	BYTE x2;
	BYTE y2;
	WORD Target;
	BYTE Angle;
	WORD Level;
	WORD m_wMaxLevel;
} GATE_ATTRIBUTE;

typedef struct
{
	char m_lpszText[MAX_LENGTH_DIALOG];
	int m_iNumAnswer;
	int m_iLinkForAnswer[MAX_ANSWER_FOR_DIALOG];
	int m_iReturnForAnswer[MAX_ANSWER_FOR_DIALOG];
	char m_lpszAnswer[MAX_ANSWER_FOR_DIALOG][MAX_LENGTH_ANSWER];
} DIALOG_SCRIPT;//Script_Dialog

typedef struct _MASTER_LEVEL_DATA
{
	BYTE Width;
	int Ability[8][4];
}MASTER_LEVEL_DATA;

typedef struct _MINI_MAP
{
	BYTE Kind;
	int	 Location[2];
	int  Rotation;
	char Name[100];
}MINI_MAP;

typedef struct tagITEM
{
/*+00*/		short	Type;
/*+02*/		int		Level;
/*+06*/		BYTE	Part;
/*+07*/		BYTE	Class;
/*+08*/		bool	TwoHand;
/*+09*/		WORD	DamageMin;
/*+11*/		WORD	DamageMax;

/*+592*/	BYTE	Kind1;
/*+593*/	BYTE	Kind2;
/*+594*/	BYTE	Kind3;

/*+13*/		BYTE	SuccessfulBlocking;
/*+14*/		WORD	Defense;
/*+16*/		WORD	MagicDefense;
/*+18*/		WORD	MagicPower;
			WORD 	AttackPower;
/*+19*/		BYTE	WeaponSpeed;
/*+20*/		WORD	WalkSpeed;
/*+22*/		BYTE	Durability;
/*+23*/		BYTE	Option1;
/*+24*/		BYTE	ExtOption;
/*+25*/		WORD	RequireStrength;
/*+27*/		WORD	RequireDexterity;
/*+29*/		WORD	RequireEnergy;
/*+31*/		WORD	RequireVitality;
/*+33*/		WORD	RequireCharisma;
/*+35*/		WORD	RequireLevel;
/*+37*/		BYTE	SpecialNum;
/*+38*/		WORD	Special[MAX_ITEM_SPECIAL];
/*+54*/		BYTE	SpecialValue[MAX_ITEM_SPECIAL];
/*+62*/		DWORD	Key;
/*+66*/		BYTE	bySelectedSlotIndex;
	union
	{
	/*+67*/		BYTE  x;
	/*+67*/		BYTE  lineal_pos;
	};
	union
	{
	/*+68*/		BYTE  y;
	/*+68*/		BYTE  ex_src_type;
	};
/*+69*/		WORD	Jewel_Of_Harmony_Option;
/*+71*/		WORD	Jewel_Of_Harmony_OptionLevel;
/*+73*/		bool	option_380;
/*+74*/		BYTE	bySocketOption[MAX_SOCKETS];
/*+79*/		BYTE	SocketCount;
/*+80*/		BYTE	SocketSeedID[MAX_SOCKETS];
/*+85*/		BYTE	SocketSphereLv[MAX_SOCKETS];
/*+90*/		BYTE	SocketSeedSetOption;
/*+91*/		int		Number;
/*+95*/		BYTE	Color;
/*+96*/		BYTE	byColorState;
/*+97*/		bool	bPeriodItem;
/*+98*/		bool	bExpiredPeriod;
/*+99*/		long	lExpireTime;
private:
	friend class SEASON3B::CNewUIItemMng;
	int   RefCount;
public:
	bool check_status(int _type)
	{
		if (Type < 0 || Type > MAX_ITEM)
			return false;
		if (_type)
		{
			if (_type == 1)
			{
				if ((Option1 & 0x3F) > 0)
					return true;
			}
			else if (_type == 2 && option_380 == true)
			{
				return true;
			}
		}
		else if (ExtOption % 4 != 0)
		{
			return true;
		}
		return false;
	}
} ITEM;

typedef struct
{
	WORD Life;
	WORD MoveSpeed;
	WORD AttackSpeed;
	WORD AttackDamageMin;
	WORD AttackDamageMax;
	WORD Defense;
	WORD MagicDefense;
	WORD AttackRating;
	WORD SuccessfulBlocking;
} MONSTER_ATTRIBUTE;

typedef struct
{
	WORD				Type;
	char				Name[32];
	WORD				Level;
	MONSTER_ATTRIBUTE	Attribute;
} MONSTER_SCRIPT;

typedef struct
{
	short             Type;
	BYTE              Level;
	int               Experince;
	MONSTER_ATTRIBUTE Attribute;
} MONSTER;

typedef struct
{
/*+00*/	char Name[32];
/*+32*/	WORD Level;
/*+34*/	WORD Damage;
/*+36*/	WORD Mana;
/*+38*/	WORD AbilityGuage;
/*+40*/	DWORD Distance;
/*+44*/	int Delay;
/*+48*/	int Energy;
/*+52*/	WORD Charisma;
/*+54*/	BYTE MasteryType;
/*+55*/	BYTE SkillUseType;
/*+56*/	DWORD SkillBrand;
/*+60*/	BYTE KillCount;
/*+61*/	BYTE RequireDutyClass[MAX_DUTY_CLASS];
/*+64*/	BYTE RequireClass[MAX_CLASS];
/*+71*/	BYTE SkillRank;
/*+72*/	WORD Magic_Icon;
/*+74*/	BYTE TypeSkill;
/*+76*/	int Strength;
/*+80*/	int Dexterity;
/*+84*/	BYTE ItemSkill;
/*+85*/	BYTE IsDamage;
/*+86*/	WORD Effect;
} SKILL_ATTRIBUTE;

typedef struct
{
	WORD Strength;
	WORD Dexterity;
	WORD Vitality;
	WORD Energy;
	WORD Life;
	WORD Mana;
	WORD Shield;
	BYTE LevelLife;
	BYTE LevelMana;
	BYTE VitalityToLife;
	BYTE EnergyToMana;
	BYTE ClassSkill[MAX_SKILLS];
} CLASS_ATTRIBUTE;

enum CharacterExperience
{
	LEVEL_DATA_NORMAL,
	LEVEL_DATA_MASTER,
	LEVEL_DATA_MAJESTIC,
	LEVEL_DATA_MAX
};

class CHARACTER_ATTRIBUTE
{
public:
	CHARACTER_ATTRIBUTE() {
		memset(Name, 0, sizeof(Name));
		this->Init();
	}

	char Name[MAX_ID_SIZE+1];
	BYTE Class;
	BYTE Skin;
	WORD Level;
	DWORD Experience;
	DWORD NextExperince;
	DWORD MaxCharacterLevel;
	DWORD Strength;
	DWORD Dexterity;
	DWORD Vitality;
	DWORD Energy;
	DWORD Charisma;
	DWORD Life;
	DWORD Mana;
	DWORD LifeMax;
	DWORD ManaMax;
	DWORD Shield;
	DWORD ShieldMax;
	DWORD AttackRatingPK;
	DWORD SuccessfulBlockingPK;
	DWORD AddStrength;
	DWORD AddDexterity;
	DWORD AddVitality;
	DWORD AddEnergy;
	DWORD AddLifeMax;
	DWORD AddManaMax;
	DWORD AddCharisma;
	DWORD SkillMana;
	DWORD SkillManaMax;
	BYTE Ability;
	WORD AbilityTime[3];

	short AddPoint;
	short MaxAddPoint;

	DWORD wMinusPoint;
	DWORD wMaxMinusPoint;
	DWORD AttackSpeed;
	DWORD AttackRating;
	DWORD AttackDamageMinRight;
	DWORD AttackDamageMaxRight;
	DWORD AttackDamageMinLeft;
	DWORD AttackDamageMaxLeft;
	DWORD MagicSpeed;
	DWORD MagicDamageMin;
	DWORD MagicDamageMax;
	DWORD CurseDamageMin;
	DWORD CurseDamageMax;
	DWORD CriticalDamage;
	DWORD SuccessfulBlocking;
	DWORD Defense;
	DWORD MagicDefense;
	DWORD WalkSpeed;
	DWORD LevelUpPoint;
	//--
#ifdef SHUTDOWN_CHARACTER_INFO
	DWORD Work_PhysiDamageMin;
	DWORD Work_PhysiDamageMax;
	DWORD Work_MagicDamageMin;
	DWORD Work_MagicDamageMax;
	DWORD Work_CurseDamageMin;
	DWORD Work_CurseDamageMax;
	DWORD Work_MulPhysiDamage;
	DWORD Work_DivPhysiDamage;
	DWORD Work_MulMagicDamage;
	DWORD Work_DivMagicDamage;
	DWORD Work_MulCurseDamage;
	DWORD Work_DivCurseDamage;
	DWORD Work_MagicDamageRate;
	DWORD Work_CurseDamageRate;
	DWORD Work_PhysiSpeed;
	DWORD Work_MagicSpeed;
	DWORD Work_AttackSuccessRate;
	DWORD Work_AttackSuccessRatePvP;
	DWORD Work_Defense;
	DWORD Work_DefenseSuccessRate;
	DWORD Work_DefenseSuccessRatePvP;
	DWORD Work_DamageMultiplier;
	DWORD Work_RFDamageMultiplierA;
	DWORD Work_RFDamageMultiplierB;
	DWORD Work_RFDamageMultiplierC;
	DWORD Work_DarkSpiritAttackDamageMin;
	DWORD Work_DarkSpiritAttackDamageMax;
	DWORD Work_DarkSpiritAttackSpeed;
	DWORD Work_DarkSpiritAttackSuccessRate;
	DWORD Work_SkillMagicDamage;
	DWORD Work_SkillPhysiDamage;
#endif // SHUTDOWN_CHARACTER_INFO
	BYTE SkillNumber;
	BYTE SkillMasterNumber;
	WORD Skill[MAX_SKILLS];
	BYTE SkillLevel[MAX_SKILLS];
	int  SkillDelay[MAX_SKILLS];
	DWORD PenaltyTimePK;
public:
	void Init();
	__int64 character_experience(CharacterExperience Type, int Character_Level);
#ifdef SHUTDOWN_CHARACTER_INFO
	void GetMagicDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
	void GetMagicDamage(DWORD* _DamageMin, DWORD* _DamageMax, DWORD* _DamageRate); // OK
	void GetPhisyDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
	void GetSkillDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
	void GetCurseDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
	void GetCurseDamage(DWORD* _DamageMin, DWORD* _DamageMax, DWORD* _DamageRate); // OK
	void GetDarkSpiritDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
	void GetDarkHorseDamage(DWORD* _DamageMin, DWORD* _DamageMax); // OK
#endif // SHUTDOWN_CHARACTER_INFO
};

typedef struct _MASTER_LEVEL_VALUE
{
	short		 nMLevel;
	__int64		 lMasterLevel_Experince;
	__int64		 lNext_MasterLevel_Experince;

	short		nAddMPoint;
	short      	nMLevelUpMPoint;
	short		nTotalMPoint;
	short		nMaxPoint;
} MASTER_LEVEL_VALUE;

//infomation end

typedef struct _OBJECT_BLOCK
{
	unsigned char Index;
	OBJECT        *Head;
	OBJECT        *Tail;
	bool          Visible;

	_OBJECT_BLOCK()
	{
		Index = 0;
		Head = NULL;
		Tail = NULL;
		Visible = false;
	}

} OBJECT_BLOCK;

typedef struct
{
	bool	 Live;
	int	     Type;
	OBJECT   *Owner;
} OPERATE;

typedef struct //-- size: 848
{
/*+000*/ WORD   Key;
/*+002*/ ITEM   Item;
/*+112*/ OBJECT Object;
/*+760*/ char   Name[80];
/*+840*/ DWORD  RenderColor;
/*+844*/ BOOL   RenderTextBold;
}ITEM_t;


typedef struct Script_Skill
{
	int Skill_Num[MAX_MONSTERSKILL_NUM];
	int Slot;
} Script_Skill;


typedef struct
{
public:
	bool		Live;
	int			Type;
	int         TexType;
	int			SubType;
	BYTE        RenderType;
	BYTE        RenderFace;
	float		Scale;
	vec3_t		Position;
	vec3_t      StartPosition;
	vec3_t		Angle;
	vec3_t		HeadAngle;
	vec3_t		Light;
	OBJECT      *Target;
	vec3_t      TargetPosition;
	BYTE        byOnlyOneRender;
	int			NumTails;
	int			MaxTails;
	vec3_t		Tails[MAX_TAILS][4];
	steady_clock_	LifeTime;
	int				life_time_work;

	bool        Collision;
	float		Velocity;
	vec3_t		Direction;
	short       PKKey;
	WORD		Skill;
	BYTE		Weapon;
	float		MultiUse;
	bool        bTileMapping;
	BYTE        m_byReverseUV;
	bool        m_bCreateTails;
	int			TargetIndex[5];
	BYTE		m_bySkillSerialNum;
	int			m_iChaIndex;
	short int	m_sTargetIndex;
} JOINT;
//character end

typedef struct
{
	bool	Live;
	int		Type;
	int		TexType;
	int		SubType;
	float	Scale;
	vec3_t	Position;
	vec3_t	Angle;
	vec3_t	Light;
	float	Alpha;
	steady_clock_	LifeTime;

	OBJECT* Target;
	float	Rotation;
	int		Frame;
	bool	bEnableMove;
	float	Gravity;
	vec3_t	Velocity;
	vec3_t	TurningForce;
	vec3_t	StartPosition;
	int		iNumBone;
	bool	bRepeatedly;
	float	fRepeatedlyHeight;
} PARTICLE;

//inventory start
typedef struct
{
	char Name[MAX_ID_SIZE+1];
	BYTE Number;
	BYTE Server;
	BYTE GuildStatus;
} GUILD_LIST_t;

typedef struct
{
	int  Key;
	char UnionName[8+1];
	char GuildName[8+1];
	BYTE Mark[64];
} MARK_t;

typedef struct
{
/*00*/	char    Name[MAX_ID_SIZE + 1];
/*11*/	BYTE    Number;
/*12*/	BYTE    Map;
/*13*/	BYTE    x;
/*14*/	BYTE    y;
/*16*/	int     currHP;
/*20*/	int     maxHP;
/*24*/	BYTE    stepHP;
/*28*/	int     index;
} PARTY_t;
//inventory end

//interface start
typedef struct
{
	char      ID[MAX_ID_SIZE+1];
	char      Text[256];
	int       Type;
	int       LifeTime;
	int       Width;
} WHISPER;

typedef struct
{
	bool m_bMagic;
	int m_iSkill;
	int m_iTarget;
	int m_iKey;
} MovementSkill;
//interface end

//matchevent start
typedef struct
{
	char	m_lpID[MAX_ID_SIZE];
	int		m_iScore;
	DWORD	m_dwExp;
	int		m_iZen;
} DevilSquareRank;
//matchevent end

//gmhellas start
typedef struct 
{
	char m_strName[64];
	vec3_t  m_vPos;
}ObjectDescript;
//gmhellas end

//csquest start
typedef struct 
{
	BYTE    chLive;
	BYTE    byQuestType;
	WORD    wItemType;
	BYTE    byItemSubType;
	BYTE	byItemLevel;
	BYTE    byItemNum;
	BYTE    byRequestType;
	BYTE    byRequestClass[MAX_CLASS];
	short   shQuestStartText[4];
} QUEST_CLASS_ACT;

typedef struct 
{
	BYTE    byLive;
	BYTE    byType;
	WORD    wCompleteQuestIndex;
	WORD    wLevelMin;
	WORD    wLevelMax;
	WORD	wRequestStrength;
	DWORD   dwZen;
	short   shErrorText;
} QUEST_CLASS_REQUEST;

typedef struct 
{
	short   shQuestConditionNum;
	short   shQuestRequestNum;
	WORD	wNpcType;
	unicode::t_char strQuestName[32];
	QUEST_CLASS_ACT     QuestAct[MAX_QUEST_CONDITION];
	QUEST_CLASS_REQUEST QuestRequest[MAX_QUEST_REQUEST];
} QUEST_ATTRIBUTE;

typedef struct 
{
	BYTE    byFlag;
	BYTE    byCount;
} QUEST_FLAG_BUFFER;

typedef struct 
{
	int iCrastGold;
	int iCrastSilver;
	int iCrastBronze;
} QUEST_CRAST;
//csquest end

//csmapinterface start 
typedef struct
{
	BYTE    bIndex;
	BYTE    x;
	BYTE    y;
}VisibleUnitLocation;

typedef struct 
{
	BYTE    byTeam;
	BYTE    byX;
	BYTE    byY;
	BYTE    byCmd;
	BYTE    byLifeTime;
}GuildCommander;
//csmapinterface end

//csitemsetoption start
typedef	struct 
{
	BYTE	byOption[2];
	BYTE	byMixItemLevel[2];
}ITEM_SET_TYPE;

typedef struct 
{
	char strSetName[64];
	BYTE byStandardOption[6][2];
	BYTE byStandardOptionValue[6][2];
	BYTE byExtOption[2];
	BYTE byExtOptionValue[2];
	BYTE byOptionCount;
	BYTE byFullOption[5];
	BYTE byFullOptionValue[5];
	BYTE byRequireClass[MAX_CLASS_PLAYER];
}ITEM_SET_OPTION;
//csitemsetoption end

//cseventmatch start
typedef struct
{
	char	m_lpID[MAX_ID_SIZE];
	int		m_iScore;
	DWORD	m_dwExp;
	int		m_iZen;
}MatchResult;
//cseventmatch end

//cdirection start
struct DirectionMonster
{
	int		m_Index;
	int		m_iActionCheck;
	bool	m_bAngleCheck;
};
//cdirection end
#endif ___STRUCT_H__
