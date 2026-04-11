// w_CharacterInfo.h: interface for the CHARACTER class.
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_W_CHARACTER_H__95647591_5047_48A4_81AE_E88B5F17EE94__INCLUDED_)
#define AFX_W_CHARACTER_H__95647591_5047_48A4_81AE_E88B5F17EE94__INCLUDED_
#pragma once

#include "steady_clock.h"

#define DEFAULT_MAX_POSTMOVEPCOCESS		15

typedef struct _PATH_t
{
	BYTE CurrentPath;
	BYTE CurrentPathFloat;
	BYTE PathNum;
	BYTE PathX[MAX_PATH_FIND];
	BYTE PathY[MAX_PATH_FIND];
	bool Success;
	bool Error;
	BYTE x,y;
	BYTE Direction;
	BYTE Run;
	int Count;

	_PATH_t()
	{
		CurrentPath = 0;
		CurrentPathFloat = 0;
		PathNum = 0;

		for ( int i = 0; i < MAX_PATH_FIND; ++i )
		{
			PathX[i] = 0;
			PathY[i] = 0;
		}

		Success = 0;
		Error = 0;
		x = 0, y = 0;
		Direction = 0;
		Run = 0;
		Count = 0;
	}
} PATH_t;

typedef struct _PART_t
{
	short Type;
	BYTE  Level;
	BYTE  Option1;
	BYTE  ExtOption;
	BYTE  LinkBone;
	BYTE  ClothType;
	BYTE  CurrentAction;
	WORD  PriorAction;
	float AnimationFrame;
	float PriorAnimationFrame;
	float PlaySpeed;
	BYTE m_byNumCloth;
	void *m_pCloth[2];

	_PART_t()
	{
		Type = 0;
		Level = 0;
		Option1 = 0;
		ExtOption = 0;
		LinkBone = 0;
		CurrentAction = 0;
		PriorAction = 0;
		AnimationFrame = 0;
		PriorAnimationFrame = 0;
		PlaySpeed = 0;
		m_byNumCloth = 0;
		m_pCloth[0] = NULL;
		m_pCloth[1] = NULL;

		ClothType = 0;
	}
} PART_t;

typedef struct 
{
	DWORD m_dwPetType;
	DWORD m_dwExp1;
	DWORD m_dwExp2;
	WORD m_wLevel;
	WORD m_wLife;
	WORD m_wDamageMin;
	WORD m_wDamageMax;
	WORD m_wAttackSpeed;
	WORD m_wAttackSuccess;
} PET_INFO;


struct ST_POSTMOVE_PROCESS 
{
	bool bProcessingPostMoveEvent;
	UINT uiProcessingCount_PostMoveEvent;

	ST_POSTMOVE_PROCESS()
	{
		bProcessingPostMoveEvent = false;
		uiProcessingCount_PostMoveEvent = 0;
	}

	~ST_POSTMOVE_PROCESS()
	{
	}
};

class CHARACTER
{
public:
	CHARACTER();
	virtual ~CHARACTER();
public:
/*+004*/	bool	Blood;
/*+005*/	bool	Ride; //-- no used
/*+006*/	bool	SkillSuccess;
/*+008*/	BOOL	m_bFixForm;
/*+012*/	bool	Foot[2];
/*+014*/	bool	SafeZone;
/*+015*/	bool	Change;
/*+016*/	bool	HideShadow;
/*+017*/	bool	m_bIsSelected;
/*+018*/	bool	Decoy; //-- no used
/*+019*/	BYTE	Class;
/*+020*/	BYTE	Skin;
/*+021*/	BYTE	CtlCode;
/*+022*/	BYTE	ExtendState;
/*+023*/	BYTE	EtcPart;
/*+024*/	BYTE	GuildStatus;
/*+025*/	BYTE	GuildType;
/*+026*/	BYTE	GuildRelationShip;
/*+027*/	BYTE	GuildSkill;
/*+028*/	BYTE	GuildMasterKillCount;
/*+029*/	BYTE	BackupCurrentSkill;
/*+030*/	BYTE	GuildTeam;
/*+031*/	BYTE	m_byGensInfluence;	//0 - None, 1 - D, 2 - V
/*+032*/	BYTE	PK;
/*+033*/	BYTE	AttackFlag;
/*+034*/	steady_clock_ AttackTime;
/*+035*/	BYTE	TargetAngle;
/*+036*/	steady_clock_ Dead;
/*+037*/	steady_clock_ Run;
/*+038*/	WORD	Skill;
/*+040*/	BYTE	SwordCount;
/*+041*/	BYTE	byExtensionSkill;
/*+042*/	BYTE	m_byDieType;
/*+043*/	BYTE	StormTime;
/*+044*/	BYTE	JumpTime;
/*+045*/	BYTE	TargetX;
/*+046*/	BYTE	TargetY;
/*+047*/	BYTE	SkillX;
/*+048*/	BYTE	SkillY;
/*+049*/	steady_clock_ Appear;
/*+050*/	BYTE	CurrentSkill;
/*+051*/	BYTE	CastRenderTime;
/*+052*/	BYTE	m_byFriend;
/*+054*/	WORD	MonsterSkill;
/*+056*/	char	ID[64];
/*+120*/	bool	Movement;
/*+121*/	BYTE	MovementType;
/*+122*/	BYTE	CollisionTime; //-- no used
/*+124*/	short	GuildMarkIndex;
/*+126*/	WORD	Key;
/*+128*/	short	TargetCharacter;
/*+130*/	WORD	Level;
/*+132*/	WORD	MonsterIndex;
/*+134*/	WORD	Damage;
/*+136*/	WORD	Hit;
/*+138*/	WORD	MoveSpeed;
/*+140*/	int		Action;
/*+144*/	CheckerTime ExtendStateTime;
/*+148*/	int		LongRangeAttack;
/*+152*/	int		SelectItem;
/*+156*/	int		Item;
/*+160*/	int		FreezeType;
/*+164*/	int		PriorPositionX;
/*+168*/	int		PriorPositionY;
/*+172*/	int		PositionX;
/*+176*/	int		PositionY;
/*+180*/	steady_clock_ m_iDeleteTime;
/*+184*/	int		m_iFenrirSkillTarget;
/*+188*/	float	ProtectGuildMarkWorldTime;
/*+192*/	float	AttackRange; //-- no used
/*+196*/	float	Freeze;
/*+200*/	float	Duplication; //-- no used
/*+204*/	float	Rot;
/*+208*/	vec3_t	TargetPosition;
/*+220*/	vec3_t	Light;
/*+232*/	PART_t	BodyPart[MAX_BODYPART];
/*+448*/	PART_t	Weapon[2];
/*+520*/	PART_t	Wing;
/*+556*/	PART_t	Helper;
/*+556*/	PART_t	MuunHelper[2];
/*+592*/	PART_t	Flag;
#ifdef SHUTDOWN_CORE_WING4
/*+592*/	PART_t	Core;
#endif // SHUTDOWN_CORE_WING4
/*+628*/	PATH_t	Path;
/*+672*/	void*	m_pParts;
/*+676*/	void*	m_pPet;
/*+680*/	PET_INFO	m_PetInfo[2];
/*+728*/	char	OwnerID[32];
private:
/*+760*/	ST_POSTMOVE_PROCESS* m_pPostMoveProcess;
public:
#ifdef SYSTEM_FLAG_NAT
/*+764*/	void* m_pFlagParts;
#endif // SYSTEM_FLAG_NAT
/*+764*/	void*	m_pTempParts;
/*+768*/	int		m_iTempKey;
/*+772*/	WORD	m_CursedTempleCurSkill;
/*+774*/	bool	m_CursedTempleCurSkillPacket;
/*+776*/	OBJECT	Object;
/*+1424*/	BYTE	m_byRankIndex;
/*+1428*/	int		m_nContributionPoint;



CheckerTime critical_work;

/*+1428*/	int		m_Reset;
/*+1428*/	int		m_MasterReset;

/*+1428*/	DWORD	m_LifeCur;
/*+1428*/	DWORD	m_LifeMax;

bool player_active_info;
public:
	void Initialize();
	void Destroy();
	void InitPetInfo(int iPetType);
	void PostMoveProcess_Active(UINT uiLimitCount);
	UINT PostMoveProcess_GetCurProcessCount();
	bool PostMoveProcess_IsProcessing();
	bool PostMoveProcess_Process();
	PET_INFO* GetEquipedPetInfo(int iPetType);

	int GetBaseClass();
	bool IsSecondClass();
	bool IsThirdClass();
	bool IsMasterLevel();
	BYTE GetStepClass();
	BYTE GetSkinModelIndex();
	BYTE GetCharacterClass();
	short GetSkinBodyIndex(int evol);

	BYTE ChangeServerClassTypeToClientClassType(const BYTE byServerClassType);

	int GetLife();
	float GetPercenteLife();
	void SetLife(DWORD Cur, DWORD Max);
	int GetKind();
	BYTE GetLive();
	int GetMovement();
	float GetPositionX();
	float GetPositionY();
	WORD GetCurrentAction();



	DWORD GetColorID();
	DWORD GetRelationShipTextColor();
	DWORD GetIDColor();
	DWORD GetGuildRelationShipTextColor();
	void AddUIInfo();
	void SetUIInfo(int _iVisibleType);
	void DelUIInfo();
	void AddUIChat(const char* Text);
	void DelUIChat();
	void UpdatePosition(int _iKey, bool _bshowNames = true);
	void UpdatePosition(int _iKey, int& _ScreenX, int& _ScreenY);
	void SetGensMark();

	void SetGuildName();
	void DelGuildName();
	void SetInfoStore();
	void OpenStore(const char* _strStore, const char* _strID, const char* _strStoreTitle);
	void CloseStore();
	void SetUIHp(int _iKey, int Percento);
	void SetVisibleHP(int _bVisibleHP);
	void SetIDColor();

	bool GetInfoGuild(char* _strGuildName, DWORD* _iGuildNameColor);
};

#endif // !defined(AFX_W_CHARACTERINFO_H__95647591_5047_48A4_81AE_E88B5F17EE94__INCLUDED_)
