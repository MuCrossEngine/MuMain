// w_ObjectInfo.cpp: implementation of the OBJECT class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SkillManager.h"
#include "w_ObjectInfo.h"

void CInterpolateContainer::GetCurrentValue(vec3_t& v3Out, float fCurrentRate, VEC_INTERPOLATES& vecInterpolates)
{
	for (auto& interpolateFactor : vecInterpolates)
	{
		if (interpolateFactor.fRateStart <= fCurrentRate && interpolateFactor.fRateEnd > fCurrentRate)
		{
			VectorInterpolation_F(v3Out, interpolateFactor.v3Start, interpolateFactor.v3End, (interpolateFactor.fRateEnd - interpolateFactor.fRateStart), (fCurrentRate - interpolateFactor.fRateStart));
			return;
		}
	}
	/*VEC_INTERPOLATES::iterator	iterBegin = vecInterpolates.begin();
	VEC_INTERPOLATES::iterator	iterEnd = vecInterpolates.end();
	VEC_INTERPOLATES::iterator	iter_;
	
	INTERPOLATE_FACTOR* pCurFactor = NULL;
	
	bool	bFindInterpolateFactor = false;
	
	for (iter_ = iterBegin; iter_ < iterEnd; ++iter_)
	{
		INTERPOLATE_FACTOR& interpolateFactor = (*iter_);
	
		if (interpolateFactor.fRateStart <= fCurrentRate && interpolateFactor.fRateEnd > fCurrentRate)
		{
			bFindInterpolateFactor = true;
			pCurFactor = &interpolateFactor;
			break;
		}
	}
	
	if (bFindInterpolateFactor == true)
	{
		VectorInterpolation_F(v3Out,
			pCurFactor->v3Start,
			pCurFactor->v3End,
			pCurFactor->fRateEnd - pCurFactor->fRateStart,
			fCurrentRate - pCurFactor->fRateStart);
	}*/
}

void CInterpolateContainer::GetCurrentValueF(float& fOut, float fCurrentRate, VEC_INTERPOLATES_F& vecInterpolates)
{
	for (auto& interpolateFactor : vecInterpolates)
	{
		if (interpolateFactor.fRateStart <= fCurrentRate && interpolateFactor.fRateEnd > fCurrentRate)
		{
			float t = (fCurrentRate - interpolateFactor.fRateStart) / (interpolateFactor.fRateEnd - interpolateFactor.fRateStart);
			LInterpolationF(fOut, interpolateFactor.fStart, interpolateFactor.fEnd, t);
			return;
		}
	}
	/*VEC_INTERPOLATES_F::iterator	iterBegin = vecInterpolates.begin();
	VEC_INTERPOLATES_F::iterator	iterEnd = vecInterpolates.end();
	VEC_INTERPOLATES_F::iterator	iter_;

	INTERPOLATE_FACTOR_F* pCurFactor = NULL;

	bool	bFindInterpolateFactor = false;

	for (iter_ = iterBegin; iter_ < iterEnd; ++iter_)
	{
		INTERPOLATE_FACTOR_F& interpolateFactor = (*iter_);

		if (interpolateFactor.fRateStart <= fCurrentRate && interpolateFactor.fRateEnd > fCurrentRate)
		{
			bFindInterpolateFactor = true;
			pCurFactor = &interpolateFactor;
			break;
		}
	}

	if (bFindInterpolateFactor == true)
	{
		LInterpolationF(fOut,
			pCurFactor->fStart,
			pCurFactor->fEnd,
			(float)(fCurrentRate - pCurFactor->fRateStart) / (float)(pCurFactor->fRateEnd - pCurFactor->fRateStart));
	}*/
}

OBJECT::OBJECT()
{
	Initialize();
}

OBJECT::~OBJECT()
{
	Destroy();
}

void OBJECT::Initialize()
{
	m_bpcroom = false;
	Live = false;
	bBillBoard = false;
	m_bCollisionCheck = false;
	m_bRenderShadow = false;
	EnableShadow = false;
	LightEnable = false;
	m_bActionStart = false;
	Visible = false;
	AlphaEnable = false;
	EnableBoneMatrix = false;
	ContrastEnable = false;
	ChromeEnable = false;
	m_bRenderAfterCharacter = false;

	AI = 0;
	CurrentAction = 0;
	PriorAction = 0;

	ExtState = 0;
	Teleport = 0;
	Kind = 0;
	Skill = 0;
	m_byNumCloth = 0;
	m_byHurtByOneToOne = 0;
	WeaponLevel = 0;
	DamageTime = 0;
	m_byBuildTime = 0;
	m_bySkillCount = 0;
	m_bySkillSerialNum = 0;
	Block = 0;

	ScreenX = 0;
	ScreenY = 0;
	PKKey = 0;
	Weapon = 0;

	Type = 0;
	SubType = 0;
	m_iAnimation = 0;
	HiddenMesh = 0;
	LifeTime = 0;
	BlendMesh = 0;
	AttackPoint[0] = 0;
	AttackPoint[1] = 0;
	RenderType = 0;
	InitialSceneFrame = 0;
	LinkBone = 0;

	m_dwTime = 0;

	Scale = 0.0f;
	BlendMeshLight = 0.0f;
	BlendMeshTexCoordU = 0.0f;
	BlendMeshTexCoordV = 0.0f;
	Timer = 0.0f;
	m_fEdgeScale = 0.0f;
	Velocity = 0.0f;
	CollisionRange = 0.0f;
	ShadowScale = 0.0f;
	Gravity = 0.0f;
	Distance = 0.0f;
	AnimationFrame = 0.0f;
	PriorAnimationFrame = 0.0f;
	AlphaTarget = 0.0f;
	Alpha = 0.0f;

	IdentityMatrix(Matrix);

	IdentityVector3D(Light);
	IdentityVector3D(Direction);
	IdentityVector3D(m_vPosSword);
	IdentityVector3D(StartPosition);
	IdentityVector3D(BoundingBoxMin);
	IdentityVector3D(BoundingBoxMax);
	IdentityVector3D(m_vDownAngle);
	IdentityVector3D(m_vDeadPosition);
	IdentityVector3D(Position);
	IdentityVector3D(Angle);
	IdentityVector3D(HeadAngle);
	IdentityVector3D(HeadTargetAngle);
	IdentityVector3D(EyeLeft);
	IdentityVector3D(EyeRight);
	IdentityVector3D(EyeLeft2);
	IdentityVector3D(EyeRight2);
	IdentityVector3D(EyeLeft3);
	IdentityVector3D(EyeRight3);

	IdentityVector3D(OBB.StartPos);
	IdentityVector3D(OBB.XAxis);
	IdentityVector3D(OBB.YAxis);
	IdentityVector3D(OBB.ZAxis);

	m_pCloth = NULL;
	BoneTransform = NULL;

	Owner = NULL;
	Prior = NULL;
	Next = NULL;

	LinkIndex = -1;
	BackupScale = 0.0;
	life_time_work = 0;
	runtime_whelskill = 0;
	movementType = Movement::None;
	time_animation_hide = std::chrono::steady_clock::now();
}

void OBJECT::Destroy()
{
	this->DeleteBoneTransform();
	this->movementType = Movement::None;
	this->LinkIndex = -1;
}

void OBJECT::CreateBoneTransform(int NumBones)
{
	this->DeleteBoneTransform();
	this->BoneTransform = new vec34_t[NumBones];
}

void OBJECT::DeleteBoneTransform()
{
	SAFE_DELETE_ARRAY(this->BoneTransform);
}

vec34_t* OBJECT::GetBoneTransform()
{
	return this->BoneTransform;
}

vec4_t* OBJECT::GetBoneTransform(int index)
{
	BMD* pModel = gmClientModels->GetModel(Type);

	if (pModel && this->BoneTransform)
	{
		if (index >= 0 && index < pModel->NumBones)
		{
			return this->BoneTransform[index];
		}
	}
	return NULL;
}

bool OBJECT::IsHideFenrir()
{
	if (Type == MODEL_PLAYER
		&& ((this->CurrentAction >= PLAYER_FENRIR_ATTACK
			&& this->CurrentAction <= PLAYER_FENRIR_WALK_WEAPON_LEFT)
			|| (this->CurrentAction >= PLAYER_RAGE_FENRIR
				&& this->CurrentAction <= PLAYER_RAGE_FENRIR_ATTACK_RIGHT)
			|| this->CurrentAction == PLAYER_SKILL_SLEEP_FENRIR
			|| this->CurrentAction == PLAYER_SKILL_CHAIN_LIGHTNING_FENRIR
			|| this->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_FENRIR
			|| this->CurrentAction == PLAYER_SKILL_DRAIN_LIFE_FENRIR
			|| this->CurrentAction == PLAYER_SKILL_SUMMON_FENRIR
			))
	{
		return false;
	}
	return true;
}

bool OBJECT::IsHideHorser()
{
	if (Type == MODEL_PLAYER
		&& (this->CurrentAction == PLAYER_STOP_RIDE_HORSE
			|| this->CurrentAction == PLAYER_RUN_RIDE_HORSE
			|| this->CurrentAction == PLAYER_HIT_RIDE_HORSE_SWORD
			|| this->CurrentAction == PLAYER_HIT_DARKHORSE
			|| this->CurrentAction == PLAYER_IDLE1_DARKHORSE
			|| this->CurrentAction == PLAYER_IDLE2_DARKHORSE
			|| this->CurrentAction == PLAYER_HIT_RIDE_STRIKE
			|| this->CurrentAction == PLAYER_HIT_RIDE_ATTACK_MAGIC
			|| this->CurrentAction == PLAYER_HIT_RIDE_CROSSBOW
			|| this->CurrentAction == PLAYER_HIT_RIDE_BOW
			))
	{
		return false;
	}
	return true;
}

bool OBJECT::IsHideRiders()
{
	if (Type == MODEL_PLAYER
		&& (this->CurrentAction == PLAYER_STOP_RIDE
			|| this->CurrentAction == PLAYER_STOP_RIDE_WEAPON
			|| this->CurrentAction == PLAYER_RUN_RIDE
			|| this->CurrentAction == PLAYER_RUN_RIDE_WEAPON
			|| this->CurrentAction == PLAYER_HIT_RIDE_SWORD
			|| this->CurrentAction == PLAYER_HIT_RIDE_TWO_HAND_SWORD
			|| this->CurrentAction == PLAYER_HIT_RIDE_SPEAR
			|| this->CurrentAction == PLAYER_HIT_RIDE_SCYTHE
			|| this->CurrentAction == PLAYER_HIT_RIDE_BOW
			|| this->CurrentAction == PLAYER_HIT_RIDE_CROSSBOW
			|| this->CurrentAction == PLAYER_SKILL_RIDER
			|| this->CurrentAction == PLAYER_SKILL_RIDER_FLY
			|| this->CurrentAction == PLAYER_HIT_SKILL_SPEAR
			|| this->CurrentAction == PLAYER_FLY_RIDE
			|| this->CurrentAction == PLAYER_FLY_RIDE_WEAPON
			|| this->CurrentAction == PLAYER_HIT_RIDE_STRIKE
			|| this->CurrentAction == PLAYER_HIT_RIDE_TELEPORT
			|| this->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH
			|| this->CurrentAction == PLAYER_HIT_RIDE_ATTACK_MAGIC
			|| this->CurrentAction == PLAYER_HIT_RIDE_BOW_UP
			|| this->CurrentAction == PLAYER_HIT_RIDE_CROSSBOW_UP
			|| this->CurrentAction == PLAYER_RIDE_SKILL
			|| this->CurrentAction == PLAYER_RAGE_UNI_ATTACK
			|| this->CurrentAction == PLAYER_RAGE_UNI_ATTACK_ONE_RIGHT
			|| this->CurrentAction == PLAYER_RAGE_UNI_RUN
			|| this->CurrentAction == PLAYER_RAGE_UNI_RUN_ONE_RIGHT
			|| this->CurrentAction == PLAYER_RAGE_UNI_STOP_ONE_RIGHT
			))
	{
		return false;
	}
	return true;
}

bool OBJECT::animation_hide_traditional()
{
	if (Type == MODEL_PLAYER)
	{
		auto runtimeThread = std::chrono::steady_clock::now();
		double difTime = std::chrono::duration<double>(runtimeThread - this->time_animation_hide).count();
		return (difTime < 2.f);
	}
	return false;
}

void OBJECT::start_skill_hide(int skill)
{
	if (Type == MODEL_PLAYER
		&& (skill == Skill_Inferno
			|| skill == Skill_Twisting_Slash
			|| skill == Skill_Rageful_Blow
			|| skill == Skill_Death_Stab
			|| skill == Skill_Crescent_Moon_Slash
			|| skill == Skill_Lance
			|| skill == Skill_Starfall
			|| (skill == Skill_Impale 
				&& this->movementType != Movement::Fenrir
				&& this->movementType != Movement::Horse
				&& this->movementType != Movement::Uniria
				&& this->movementType != Movement::Dinorant
				)
			|| skill == Skill_Swell_Life
			|| skill == Skill_Fire_Breath
			|| skill == Skill_Strike_of_Destruction
			|| skill == Skill_Flame_Strike
			|| skill == Skill_Gigantic_Storm
			|| skill == Skill_Lightning_Shock
			|| skill == Skill_Killing_Blow
			|| skill == Skill_Beast_Uppercut
			|| skill == Skill_Chain_Drive
			|| skill == Skill_Dark_Side
			|| skill == Skill_Dragon_Roar
			|| skill == Skill_Dragon_Slasher
			|| skill == Skill_Ignore_Defense
			|| skill == Skill_Increase_Health
			|| skill == Skill_Increase_Block
			|| skill == Skill_Charge
			|| skill == Skill_Phoenix_Shot
			|| skill == Skill_Cyclone_Strengthener
			|| skill == Skill_Slash_Strengthener
			|| skill == Skill_Falling_Slash_Strengthener
			|| skill == Skill_Lunge_Strengthener
			|| skill == Skill_Twisting_Slash_Strengthener
			|| skill == Skill_Twisting_Slash_Mastery
			|| skill == Skill_Twisting_Slash_Strengthener2
			|| skill == Skill_Death_Stab_Strengthener
			|| skill == Skill_Death_Stab_Proficiency
			|| skill == Skill_Death_Stab_Mastery
			|| skill == Skill_Strike_of_Destruction_Strengthe
			|| skill == Skill_Strike_of_Destruction_Proficien
			|| skill == Skill_Strike_of_Destruction_Mastery
			|| skill == Skill_Swell_Life_Strengthener
			|| skill == Skill_Swell_Life_Proficiency
			|| skill == Skill_Swell_Life_Mastery
		))
	{
		this->time_animation_hide = std::chrono::steady_clock::now();
	}
}

bool OBJECT::animation_fenrir_walk()
{
	if ((this->CurrentAction >= PLAYER_FENRIR_WALK && this->CurrentAction <= PLAYER_FENRIR_WALK_WEAPON_LEFT)
		|| (this->CurrentAction >= PLAYER_RAGE_FENRIR_WALK && this->CurrentAction <= PLAYER_RAGE_FENRIR_WALK_TWO_SWORD))
	{
		return true;
	}
	return false;
}

bool OBJECT::animation_fenrir_walk_run()
{
	if ((this->CurrentAction >= PLAYER_FENRIR_RUN && this->CurrentAction <= PLAYER_FENRIR_RUN_ONE_LEFT_ELF)
		|| (this->CurrentAction >= PLAYER_RAGE_FENRIR_RUN && this->CurrentAction <= PLAYER_RAGE_FENRIR_RUN_ONE_LEFT))
	{
		return true;
	}
	return false;
}
