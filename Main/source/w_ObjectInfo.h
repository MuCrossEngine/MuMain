// w_ObjectInfo.h: interface for the ObjectInfo class.
//////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include "./Math/ZzzMathLib.h"
#include "steady_clock.h"

class	CInterpolateContainer
{
public:
	struct INTERPOLATE_FACTOR
	{
		float fRateStart;
		float fRateEnd;
		vec3_t v3Start;
		vec3_t v3End;

		INTERPOLATE_FACTOR(float fRateStart_, float fRateEnd_, vec3_t& v3Start_, vec3_t& v3End_)
			: fRateStart(fRateStart_), fRateEnd(fRateEnd_) {
			VectorCopy(v3Start_, v3Start);
			VectorCopy(v3End_, v3End);
		}
		
		INTERPOLATE_FACTOR() {}
	};
	
	struct INTERPOLATE_FACTOR_F
	{
		float	fRateStart;
		float	fRateEnd;
		float	fStart;
		float	fEnd;

		INTERPOLATE_FACTOR_F(float fRateStart_, float fRateEnd_, float fStart_, float fEnd_)
			: fRateStart(fRateStart_), fRateEnd(fRateEnd_), fStart(fStart_), fEnd(fEnd_) {}
		
		INTERPOLATE_FACTOR_F() {}
	};

public:
	typedef std::vector<INTERPOLATE_FACTOR>		VEC_INTERPOLATES;
	typedef std::vector<INTERPOLATE_FACTOR_F>	VEC_INTERPOLATES_F;
	VEC_INTERPOLATES						m_vecInterpolatesAngle;
	VEC_INTERPOLATES						m_vecInterpolatesPos;
	VEC_INTERPOLATES_F						m_vecInterpolatesScale;
	VEC_INTERPOLATES_F						m_vecInterpolatesAlpha;	
		
	public:
		void GetAngleCurrent(vec3_t& v3Out, float fCurrentRate) {
			GetCurrentValue(v3Out, fCurrentRate, m_vecInterpolatesAngle);
		}

		void GetPosCurrent(vec3_t& v3Out, float fCurrentRate) {
			GetCurrentValue(v3Out, fCurrentRate, m_vecInterpolatesPos);
		}

		void GetScaleCurrent(float& fOut, float fCurrentRate) {
			GetCurrentValueF(fOut, fCurrentRate, m_vecInterpolatesScale);
		}

		void GetAlphaCurrent(float& fOut, float fCurrentRate) {
			GetCurrentValueF(fOut, fCurrentRate, m_vecInterpolatesAlpha);
		}

		/*
		void GetAngleCurrent(vec3_t& v3Out, float fCurrentRate )
		{
			GetCurrentValue( v3Out, fCurrentRate, m_vecInterpolatesAngle );
		}
		void GetPosCurrent(vec3_t& v3Out, float fCurrentRate )
		{
			GetCurrentValue( v3Out, fCurrentRate, m_vecInterpolatesPos );
		}

		void GetScaleCurrent(float& fOut, float fCurrentRate )
		{
			GetCurrentValueF( fOut, fCurrentRate, m_vecInterpolatesScale );
		}

		void GetAlphaCurrent(float& fOut, float fCurrentRate )
		{
			GetCurrentValueF( fOut, fCurrentRate, m_vecInterpolatesAlpha );
		}*/
		void ClearContainer() {
			m_vecInterpolatesPos.clear();
			m_vecInterpolatesAngle.clear();
			m_vecInterpolatesScale.clear();
			m_vecInterpolatesAlpha.clear();
		}

	public:
		CInterpolateContainer(){};
		~CInterpolateContainer(){};
private:
	void GetCurrentValue(vec3_t& v3Out, float fCurrentRate, VEC_INTERPOLATES& vecInterpolates);
	void GetCurrentValueF(float& fOut, float fCurrentRate, VEC_INTERPOLATES_F& vecInterpolates);
};

typedef struct tagMU_POINTF
{
	float x;
	float y;
} MU_POINTF;

typedef struct tagSIZEF
{
	float cx;
	float cy;
} SIZEF;
	
typedef struct
{
	vec3_t StartPos;
	vec3_t XAxis;
	vec3_t YAxis;
	vec3_t ZAxis;
} OBB_t;

class OBJECT
{
public:
	OBJECT();
	virtual ~OBJECT();

public:
	void Initialize();
	void Destroy();

	void CreateBoneTransform(int NumBones);
	void DeleteBoneTransform();
	vec34_t* GetBoneTransform();
	vec4_t* GetBoneTransform(int index);

	bool IsHideFenrir();
	bool IsHideHorser();
	bool IsHideRiders();
	bool animation_hide_traditional();
	void start_skill_hide(int skill);
	bool animation_fenrir_walk();
	bool animation_fenrir_walk_run();
public:
/*+004*/	bool			Live;
/*+005*/	bool			bBillBoard;
/*+006*/	bool			m_bCollisionCheck;
/*+007*/	bool			m_bRenderShadow;
/*+008*/	bool			EnableShadow;
/*+009*/	bool			LightEnable;
/*+010*/	bool			m_bActionStart;
/*+011*/	bool			m_bRenderAfterCharacter;
/*+012*/	bool			Visible;
/*+013*/	bool			AlphaEnable;
/*+014*/	bool			EnableBoneMatrix;
/*+015*/	bool			ContrastEnable;
/*+016*/	bool			ChromeEnable;
/*+017*/	BYTE			AI;
/*+018*/	WORD			CurrentAction;
/*+020*/	WORD			PriorAction;
/*+022*/	BYTE			ExtState;
/*+023*/	BYTE			Teleport;
/*+024*/	BYTE			Kind;
/*+026*/	WORD			Skill;
/*+028*/	BYTE			m_byNumCloth;
/*+029*/	BYTE			m_byHurtByOneToOne;
/*+030*/	BYTE			WeaponLevel;
/*+031*/	BYTE			DamageTime;
/*+032*/	BYTE			m_byBuildTime;
/*+033*/	BYTE			m_bySkillCount;
/*+034*/	BYTE			m_bySkillSerialNum;
/*+035*/	BYTE			Block;
/*+036*/	void*			m_pCloth;
/*+040*/	short			ScreenX;
/*+042*/	short			ScreenY;
/*+044*/	short			PKKey;
/*+046*/	short			Weapon;
/*+048*/	int				Type;
/*+052*/	int				SubType;
/*+056*/	int				m_iAnimation;
/*+060*/	int				HiddenMesh;
/*+064*/	steady_clock_	LifeTime;
			int				life_time_work;
/*+068*/	int				BlendMesh;
/*+072*/	int				AttackPoint[2];
/*+080*/	int				RenderType;
/*+084*/	int				InitialSceneFrame;
/*+088*/	int				LinkBone;
/*+092*/	DWORD			m_dwTime;
/*+096*/	float			Scale;
/*+096*/	float			BackupScale;
/*+100*/	float			BlendMeshLight;
/*+104*/	float			BlendMeshTexCoordU;
/*+108*/	float			BlendMeshTexCoordV;
/*+112*/	float			Timer;
/*+116*/	float			m_fEdgeScale;
/*+120*/	float			Velocity;
/*+124*/	float			CollisionRange;
/*+128*/	float			ShadowScale;
/*+132*/	float			Gravity;
/*+136*/	float			Distance;
/*+140*/	float			AnimationFrame;
/*+144*/	float			PriorAnimationFrame;
/*+148*/	float			AlphaTarget;
/*+152*/	float			Alpha;
/*+156*/	vec3_t			Light;
/*+168*/	vec3_t			Direction;
/*+180*/	vec3_t			m_vPosSword;
/*+192*/	vec3_t			StartPosition;
/*+204*/	vec3_t			BoundingBoxMin;
/*+216*/	vec3_t			BoundingBoxMax;
/*+228*/	vec3_t			m_vDownAngle;
/*+240*/	vec3_t			m_vDeadPosition;
/*+252*/	vec3_t			Position;
/*+264*/	vec3_t			Angle;
/*+276*/	vec3_t			HeadAngle;
/*+288*/	vec3_t			HeadTargetAngle;
/*+300*/	vec3_t			EyeLeft;
/*+312*/	vec3_t			EyeRight;
/*+324*/	vec3_t			EyeLeft2;
/*+336*/	vec3_t			EyeRight2;
/*+348*/	vec3_t			EyeLeft3;
/*+360*/	vec3_t			EyeRight3;
/*+372*/	vec34_t			Matrix;
/*+420*/	vec34_t*		BoneTransform;
/*+424*/	OBB_t			OBB;
/*+472*/	OBJECT*			Owner;
/*+476*/	OBJECT*			Prior;
/*+480*/	OBJECT*			Next;
/*+484*/	Buff			m_BuffMap;
/*+520*/	short int		m_sTargetIndex;
/*+524*/	BOOL			m_bpcroom;
/*+540*/	vec3_t			m_v3PrePos1;
/*+552*/	vec3_t			m_v3PrePos2;
/*+564*/	vec3_t			m_v3PrePos3;
/*+576*/	CInterpolateContainer	m_Interpolates;
//--
			int				LinkIndex;
			int				runtime_whelskill;
			int				movementType;
			std::chrono::steady_clock::time_point time_animation_hide;
};

