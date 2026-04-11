// w_CharacterInfo.cpp: implementation of the CHARACTER class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "w_CharacterInfo.h"
#include "ScaleForm.h"
#include "UIGuildInfo.h"
#include "SkillManager.h"
#include "ZzzLodTerrain.h"
#include "CharacterManager.h"
#include "PersonalShopTitleImp.h"

extern CHARACTER* Hero;
extern MARK_t GuildMark[MAX_MARKS];

CHARACTER::CHARACTER()
{
	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	Object.Initialize();

	Blood = false;
	Ride = false;
	SkillSuccess = false;
	m_bFixForm = false;
	Foot[0] = false;
	Foot[1] = false;
	SafeZone = false;
	Change = false;
	HideShadow = false;
	m_bIsSelected = false;
	Decoy = false;

	Class = 0;
	Skin = 0;
	CtlCode = 0;
	ExtendState = 0;
	EtcPart = 0;
	GuildStatus = 0;
	GuildType = 0;
	GuildRelationShip = 0;
	GuildSkill = 0;
	GuildMasterKillCount = 0;
	BackupCurrentSkill = 0;
	GuildTeam = 0;
	m_byGensInfluence = 0;
	PK = 0;
	AttackFlag = 0;
	AttackTime = 0;
	TargetAngle = 0;
	Dead = 0;
	Run = 0;
	Skill = 0;
	SwordCount = 0;
	byExtensionSkill = 0;
	m_byDieType = 0;
	StormTime = 0;
	JumpTime = 0;
	TargetX = 0;
	TargetY = 0;
	SkillX = 0;
	SkillY = 0;
	Appear = 0;
	CurrentSkill = 0;
	CastRenderTime = 0;
	m_byFriend = 0;
	MonsterSkill = 0;

	for (int i = 0; i < 32; ++i)
		ID[i] = 0;

	Movement = 0;
	MovementType = 0;
	CollisionTime = 0;
	GuildMarkIndex = 0;
	Key = 0;
	TargetCharacter = 0;

	Level = 0;
	MonsterIndex = 0;
	Damage = 0;
	Hit = 0;
	MoveSpeed = 0;

	Action = 0;
	ExtendStateTime = (float)(100) / REFERENCE_FPS;
	LongRangeAttack = 0;
	SelectItem = 0;
	Item = 0;
	FreezeType = 0;
	PriorPositionX = 0;
	PriorPositionY = 0;
	PositionX = 0;
	PositionY = 0;
	m_iDeleteTime = 0;
	m_iFenrirSkillTarget = -1;

	ProtectGuildMarkWorldTime = 0.0f;
	AttackRange = 0.0f;
	Freeze = 0.0f;
	Duplication = 0.0f;
	Rot = 0.0f;
	IdentityVector3D(TargetPosition);
	IdentityVector3D(Light);
	m_pParts = NULL;
#ifdef SYSTEM_FLAG_NAT
	m_pFlagParts = NULL;
#endif // SYSTEM_FLAG_NAT

	m_pPet = NULL;

	InitPetInfo(PET_TYPE_DARK_HORSE);
	InitPetInfo(PET_TYPE_DARK_SPIRIT);

	for (int i = 0; i < 32; ++i)
	{
		OwnerID[i] = 0;
	}

	m_pPostMoveProcess = NULL;
	m_pTempParts = NULL;
	m_iTempKey = 0;
	m_CursedTempleCurSkill = AT_SKILL_CURSED_TEMPLE_PRODECTION;
	m_CursedTempleCurSkillPacket = false;
	m_byRankIndex = 0;
	m_nContributionPoint = 0;

	for (int k = 0; k < MAX_BODYPART; ++k)
	{
		BodyPart[k].Type = -1;
		BodyPart[k].Level = 0;
		BodyPart[k].Option1 = 0;
		BodyPart[k].ExtOption = 0;
	}

	for (int l = 0; l < 2; ++l)
	{
		Weapon[l].Type = -1;
		Weapon[l].Level = 0;
		Weapon[l].Option1 = 0;
	}

	Wing.Type = -1;
	Wing.Level = 0;
	Wing.Option1 = 0;

	Core.Type = -1;
	Core.Level = 0;
	Core.Option1 = 0;

	player_active_info = false;

	this->critical_work.intervalSet(1.6);
}

void CHARACTER::Destroy()
{

}

void CHARACTER::InitPetInfo(int iPetType)
{
	memset(&m_PetInfo[iPetType], 0, sizeof(PET_INFO));
	m_PetInfo[iPetType].m_dwPetType = PET_TYPE_NONE;
}

void CHARACTER::PostMoveProcess_Active(UINT uiLimitCount)
{
	if (m_pPostMoveProcess != NULL)
	{
		delete m_pPostMoveProcess; m_pPostMoveProcess = NULL;
	}

	m_pPostMoveProcess = new ST_POSTMOVE_PROCESS();
	m_pPostMoveProcess->bProcessingPostMoveEvent = false;
	m_pPostMoveProcess->uiProcessingCount_PostMoveEvent = uiLimitCount;
}

UINT CHARACTER::PostMoveProcess_GetCurProcessCount()
{
	return m_pPostMoveProcess->uiProcessingCount_PostMoveEvent;
}

bool CHARACTER::PostMoveProcess_IsProcessing()
{
	if (m_pPostMoveProcess == NULL)
	{
		return false;
	}
	return (0 <= m_pPostMoveProcess->uiProcessingCount_PostMoveEvent);
}

bool CHARACTER::PostMoveProcess_Process()
{
	if (m_pPostMoveProcess == NULL)
	{
		return false;
	}

	UINT uiCurretProcessingCount_ = m_pPostMoveProcess->uiProcessingCount_PostMoveEvent--;

	if (0 >= uiCurretProcessingCount_)
	{
		delete m_pPostMoveProcess;
		m_pPostMoveProcess = NULL;
		return false;
	}
	return true;
}

PET_INFO* CHARACTER::GetEquipedPetInfo(int iPetType)
{
	return &(m_PetInfo[iPetType]);
}

int CHARACTER::GetBaseClass()
{
	return gCharacterManager.GetBaseClass(Class);
}

bool CHARACTER::IsSecondClass()
{
	return gCharacterManager.IsSecondClass(Class);
}

bool CHARACTER::IsThirdClass()
{
	return gCharacterManager.IsThirdClass(Class);
}

bool CHARACTER::IsMasterLevel()
{
	return gCharacterManager.IsMasterLevel(Class);
}

BYTE CHARACTER::GetStepClass()
{
	return gCharacterManager.GetStepClass(Class);
}

BYTE CHARACTER::GetSkinModelIndex()
{
	return gCharacterManager.GetSkinModelIndex(Class);
}

BYTE CHARACTER::GetCharacterClass()
{
	return gCharacterManager.GetCharacterClass(Class);
}

short CHARACTER::GetSkinBodyIndex(int evol)
{
	int _firstclass = this->GetBaseClass();

	return MODEL_BODY_ARMOR + _firstclass + (evol * MAX_CLASS_PLAYER);
}

BYTE CHARACTER::ChangeServerClassTypeToClientClassType(const BYTE byServerClassType)
{
	Class = gCharacterManager.ChangeServerClassTypeToClientClassType(byServerClassType);

	return Class;
}

int CHARACTER::GetLife()
{
	return (m_LifeCur / (float)m_LifeMax) * 100.f;
}

float CHARACTER::GetPercenteLife()
{
	return (m_LifeCur / (float)m_LifeMax);
}

void CHARACTER::SetLife(DWORD Cur, DWORD Max)
{
	m_LifeCur = Cur;
	m_LifeMax = Max;
}

int CHARACTER::GetKind()
{
	return this->Object.Kind;
}

BYTE CHARACTER::GetLive()
{
	return (unsigned __int8)this->Object.Live;
}

int CHARACTER::GetMovement()
{
	return this->Object.movementType;
}

float CHARACTER::GetPositionX()
{
	return this->Object.Position[0];
}

float CHARACTER::GetPositionY()
{
	return this->Object.Position[1];
}

WORD CHARACTER::GetCurrentAction()
{
	return this->Object.CurrentAction;
}

DWORD CHARACTER::GetColorID()
{
	BYTE Style = this->PK;

	if (Object.Kind == KIND_NPC)
	{
		Style = 0;
	}

	DWORD Hex = 0xFFFFFF;

	switch (Style)
	{
	case 0:
		Hex = 0xFF00FF96;
		break;
	case PVP_HERO2:
		Hex = 0xFFEE8C56;
		break;
	case PVP_HERO1:
		Hex = 0xFFEEB18C;
		break;
	case PVP_NEUTRAL:
		Hex = 0xFF00DCFF;
		break;
	case PVP_CAUTION:
		Hex = 0xFF0096FF;
		break;
	case PVP_MURDERER1:
		Hex = 0xFF0044FF;
		break;
	default:
		Hex = 0xFF0000FF;
		break;
	}
	return Hex;
}

DWORD CHARACTER::GetRelationShipTextColor()
{
	DWORD Color = 0xFF00FFC8;

	if (this == Hero)
	{
		Color = 0XFF00FFC8;
	}
	else if (GuildRelationShip == GR_NONE)
	{
		Color = 0xFFFFFFFF;
	}
	else if (GuildRelationShip == GR_RIVAL || GuildRelationShip == GR_RIVALUNION)
	{
		Color = 0xFF0000FF;
	}

	return Color;
}

DWORD CHARACTER::GetIDColor()
{

	BYTE Style = this->PK;

	if (Object.Kind == KIND_NPC)
	{
		Style = 0;
	}

	DWORD Hex = 0xFFFFFF;

	switch (Style)
	{
	case 0:
		Hex = 0x96FFF0;
		break;
	case PVP_HERO2:
		Hex = 0x568CEE;
		break;
	case PVP_HERO1:
		Hex = 0x8CB1EE;
		break;
	case PVP_NEUTRAL:
		Hex = 0xFFDC00;
		break;
	case PVP_CAUTION:
		Hex = 0xFF9600;
		break;
	case PVP_MURDERER1:
		Hex = 0xFF4400;
		break;
	default:
		Hex = 0xFF0000;
		break;
	}
	return Hex;
}

DWORD CHARACTER::GetGuildRelationShipTextColor()
{
	DWORD Color = 0xC8FF00;

	if (this == Hero)
	{
		Color = 0xC8FF00;
	}
	else if (GuildRelationShip == GR_NONE)
	{
		Color = 0xFFFFFF;
	}
	else if (GuildRelationShip == GR_RIVAL || GuildRelationShip == GR_RIVALUNION)
	{
		Color = 0xFF0000;
	}

	return Color;
}

void CHARACTER::AddUIInfo()
{
}

void CHARACTER::SetUIInfo(int _iVisibleType)
{
}

void CHARACTER::DelUIInfo()
{
}

void CHARACTER::AddUIChat(const char* Text)
{
}

void CHARACTER::DelUIChat()
{
}

void CHARACTER::UpdatePosition(int _iKey, bool _bshowNames)
{
	if (player_active_info && (this->GetKind() == KIND_PLAYER || this->GetKind() == KIND_MONSTER))
	{
		vec3_t Position;
		OBJECT* o = &Object;

		float add_boundin = 0.0;
		int _ScreenX, _ScreenY;


		if (_iKey == -1)
		{
			_iKey = gmCharacters->GetCharacterIndex(this);
		}

		if (o->Kind == KIND_PLAYER)
		{
			if (o->Type >= MODEL_LITTLESANTA && o->Type <= MODEL_LITTLESANTA_END || o->Type == MODEL_MERCHANT_MAN)
			{
				add_boundin = 30.f;
			}
			else
			{
				add_boundin = 40.0;
			}

			if (this->SafeZone == false && this->Wing.Type != -1)
			{
				add_boundin += 30.f;
			}
		}
		else
		{
			add_boundin = 40.0;
		}

		Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + add_boundin, Position);

		Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + o->BoundingBoxMax[2] + add_boundin;

		Projection2(Position, &_ScreenX, &_ScreenY);

		char _strGuildName[100];
		DWORD _iGuildNameColor = 0xFFFFFFFF;

		memset(_strGuildName, 0, sizeof(_strGuildName));

		GetInfoGuild(_strGuildName, &_iGuildNameColor);
	}
}

void CHARACTER::UpdatePosition(int _iKey, int& _ScreenX, int& _ScreenY)
{
	vec3_t Position;
	OBJECT* o = &Object;

	float add_boundin = 0.0;
	if (o->Kind == KIND_PLAYER)
	{
		if (o->Type >= MODEL_LITTLESANTA && o->Type <= MODEL_LITTLESANTA_END || o->Type == MODEL_MERCHANT_MAN)
		{
			add_boundin = 30.f;
		}
		else
		{
			add_boundin = 40.0;
		}

		if (this->SafeZone == false && this->Wing.Type != -1)
		{
			add_boundin += 30.f;
		}
	}
	else
	{
		add_boundin = 40.0;
	}

	Vector(o->Position[0], o->Position[1], o->Position[2] + o->BoundingBoxMax[2] + add_boundin, Position);

	Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + o->BoundingBoxMax[2] + add_boundin;

	Projection2(Position, &_ScreenX, &_ScreenY);

	_ScreenY -= 30;
}

void CHARACTER::SetGensMark()
{
}

void CHARACTER::SetGuildName()
{
}

void CHARACTER::DelGuildName()
{
}

void CHARACTER::SetInfoStore()
{
}

void CHARACTER::OpenStore(const char* _strStore, const char* _strID, const char* _strStoreTitle)
{
}

void CHARACTER::CloseStore()
{
}

void CHARACTER::SetUIHp(int _iKey, int Percento)
{
}

void CHARACTER::SetVisibleHP(int _bVisibleHP)
{
}

void CHARACTER::SetIDColor()
{
}

bool CHARACTER::GetInfoGuild(char* _strGuildName, DWORD* _iGuildNameColor)
{
	return false;
}
