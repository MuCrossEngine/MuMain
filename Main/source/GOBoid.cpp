//////////////////////////////////////////////////////////////////////////
//  
//  GOBoid.cpp
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "zzzInfomation.h"
#include "ZzzOpenglUtil.h"
#include "zzzBmd.h"
#include "GOBoid.h"
#include "zzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAI.h"
#include "ZzzEffect.h"
#include "zzzOpenData.h"
#include "CSChaosCastle.h"
#include "GMHellas.h"
#include "GMAida.h"
#include "CDirection.h"
#include "DSPlaySound.h"
#include "MapManager.h"
#include "CameraMove.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "CGMItemEffect.h"
#include "Utilities/Log/muConsoleDebug.h"

int EnableEvent = 0;

std::map<int, PET_GOBOID> goBoidList;




PET_GOBOID* GoBoidFind(int ItemIndex)
{
	std::map<int, PET_GOBOID>::iterator li = goBoidList.find(ItemIndex);

	if (li != goBoidList.end())
	{
		return &li->second;
	}
	return NULL;
}

bool isGoBoidItem(int ItemIndex)
{
	std::map<int, PET_GOBOID>::iterator li = goBoidList.find(ItemIndex);

	if (li != goBoidList.end())
	{
		return true;
	}
	return false;
}

void GoBoidREG(int ItemIndex, int nType, int Movement, float scale)
{
	PET_GOBOID info;
	info.nType = ItemIndex;
	info.RenderIndex = nType;
	info.RenderScale = scale;
	info.MovementType = Movement;
	goBoidList.insert(std::pair<int, PET_GOBOID>(ItemIndex, info));
}

void CreateGoboId(int ItemIndex, vec3_t Position, OBJECT* Owner, int SubType, int LinkBone)
{
	if (NULL == Owner)
		return;

	PET_GOBOID* fw = GoBoidFind(ItemIndex);

	if (fw)
	{
		gGoboidManager->CreateBug(fw->RenderIndex, Position, Owner, SubType, LinkBone);
	}
}

void CreateSubGoboId(int ItemIndex, vec3_t Position, OBJECT* Owner, OBJECT* o, int SubType, int LinkBone)
{
	if (NULL == Owner)
		return;

	PET_GOBOID* fw = GoBoidFind(ItemIndex);

	if (fw)
	{
		o->LinkIndex = ItemIndex;
		CreateBugSub(fw->RenderIndex, Position, Owner, o, SubType, LinkBone);
	}
}

bool IsBug(ITEM* pItem)
{
	if (pItem != NULL)
	{
		if ((pItem->Type == ITEM_HELPER + 0
			|| pItem->Type == ITEM_HELPER + 1
			|| pItem->Type == ITEM_HELPER + 2
			|| pItem->Type == ITEM_HELPER + 3
			|| pItem->Type == ITEM_HELPER + 4
			|| pItem->Type == ITEM_HELPER + 37
			))
		{
			return true;
		}
		else
		{
			if (isGoBoidItem(pItem->Type))
			{
				return true;
			}
		}
	}

	return false;
}

bool CreateBugSub(int Type, vec3_t Position, OBJECT* Owner, OBJECT* o, int SubType, int LinkBone)
{
	if (gMapManager->InChaosCastle() == true || !Owner || !o)
		return false;

	if (o->Live == false)
	{
		o->Type = Type;
		o->Live = true;
		o->Visible = false;
		o->LightEnable = true;
		o->ContrastEnable = false;
		o->AlphaEnable = false;
		o->EnableBoneMatrix = false;
		o->Owner = Owner;
		o->SubType = SubType;
		o->HiddenMesh = -1;
		o->BlendMesh = -1;
		o->BlendMeshLight = 1.f;
		o->Scale = 0.7f;
		o->LifeTime = 30;
		o->Alpha = 0.f;
		o->AlphaTarget = 1.f;

		VectorCopy(Position, o->Position);
		VectorCopy(Owner->Angle, o->Angle);
		Vector(3.f, 3.f, 3.f, o->Light);

		o->PriorAnimationFrame = 0.f;
		o->AnimationFrame = 0.f;
		o->Velocity = 0.5f;
		switch (o->Type)
		{
		case MODEL_FENRIR_BLACK:
		case MODEL_FENRIR_BLUE:
		case MODEL_FENRIR_RED:
		case MODEL_FENRIR_GOLD:
			o->Scale = 1.f;
			break;
		case MODEL_DARK_HORSE:
			o->Scale = 1.1f;
			break;
		case MODEL_PEGASUS:
		case MODEL_UNICON:
			o->Scale = 0.9f;
			break;
		case MODEL_HELPER:
			o->BlendMesh = 1;
			Vector(Owner->Position[0] + (float)(rand() % 512 - 256),
				Owner->Position[1] + (float)(rand() % 512 - 256),
				Owner->Position[2] + (float)(rand() % 128 + 128), o->Position);
			break;
		case MODEL_HELPER + 1:
			Vector(Owner->Position[0] + (float)(rand() % 128 - 64), Owner->Position[1] + (float)(rand() % 128 - 64), Owner->Position[2], o->Position);
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + (float)(rand() % 100);
			break;
		}

		if (LinkBone == 0)
		{
			o->movementType = Movement::None;

			if (o->Type == MODEL_FENRIR_BLACK
				|| o->Type == MODEL_FENRIR_BLUE
				|| o->Type == MODEL_FENRIR_RED
				|| o->Type == MODEL_FENRIR_GOLD
				)
			{
				o->movementType = Movement::Fenrir;
			}
			else if (o->Type == MODEL_DARK_HORSE)
			{
				o->movementType = Movement::Horse;
			}
			else if (o->Type == MODEL_DARK_HORSE)
			{
				o->movementType = Movement::Horse;
			}
			else if (o->Type == MODEL_UNICON)
			{
				o->movementType = Movement::Uniria;
			}
			else if (o->Type == MODEL_PEGASUS)
			{
				o->movementType = Movement::Dinorant;
			}
			else if (o->Type == MODEL_HELPER)
			{
				o->movementType = Movement::Angel;
			}
			else
			{
				PET_GOBOID* fL = GoBoidFind((o->Type - MODEL_ITEM));

				if (fL)
				{
					o->movementType = fL->MovementType;
					o->Scale = fL->RenderScale;
				}
			}
			Owner->movementType = o->movementType;
		}
		else
		{
			PET_GOBOID* fL = GoBoidFind((o->Type - MODEL_ITEM));

			if (fL)
			{
				o->movementType = fL->MovementType;
				o->Scale = fL->RenderScale;
			}
		}

		o->LinkIndex = (o->Type - MODEL_ITEM);
		o->BackupScale = o->Scale;
		return FALSE;
	}
	return TRUE;
}

bool MoveBug(OBJECT* o, bool bForceRender)
{
	if (o->Live)
	{
		if (SceneFlag == MAIN_SCENE)
		{
			if (!(World >= WD_65DOPPLEGANGER1 && World <= WD_68DOPPLEGANGER4) && (!o->Owner->Live || o->Owner->Kind != KIND_PLAYER))
			{
				o->Live = false;
				return TRUE;
			}
		}

		Alpha(o);

		BMD* b = gmClientModels->GetModel(o->Type);

		if (b == NULL)
			return true;

		int    j;
		float  FlyRange = 0.0f;
		vec3_t Light, Position;
		vec3_t TargetPosition;

		VectorCopy(o->Owner->Position, TargetPosition);

		switch (o->movementType)
		{
		case Movement::Fenrir:
		{
			if ((TerrainWall[TERRAIN_INDEX_REPEAT((int)(o->Owner->Position[0] / TERRAIN_SCALE), (int)(o->Owner->Position[1] / TERRAIN_SCALE))] & TW_SAFEZONE) == TW_SAFEZONE && bForceRender == FALSE)
			{
				o->Alpha = 0.f;
				break;
			}
			if (o->Owner->IsHideFenrir() || o->Owner->animation_hide_traditional())
			{
				o->Alpha = 0.f;
				break;
			}

			if (o->Owner->Teleport == TELEPORT_BEGIN || o->Owner->Teleport == TELEPORT)
			{
				o->Alpha -= (0.1f);
				if (o->Alpha < 0)
					o->Alpha = 0.f;
			}
			else
			{
				o->Alpha = 1.f;
			}

			VectorCopy(o->Owner->HeadAngle, o->HeadAngle);
			VectorCopy(o->Owner->Position, o->Position);
			VectorCopy(o->Owner->Angle, o->Angle);

			if ((o->Owner->CurrentAction >= PLAYER_FENRIR_ATTACK && o->Owner->CurrentAction <= PLAYER_FENRIR_ATTACK_BOW)
				|| IsAliceRideAction_Fenrir(o->Owner->CurrentAction) == true
				|| o->Owner->CurrentAction == PLAYER_RAGE_FENRIR_ATTACK_RIGHT)
			{
				SetAction(o, FENRIR_ATTACK);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_FENRIR_SKILL && o->Owner->CurrentAction <= PLAYER_FENRIR_SKILL_ONE_LEFT)
			{
				SetAction(o, FENRIR_ATTACK_SKILL);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_FENRIR_DAMAGE && o->Owner->CurrentAction <= PLAYER_FENRIR_DAMAGE_ONE_LEFT)
			{
				SetAction(o, FENRIR_DAMAGE);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_FENRIR_STAND && o->Owner->CurrentAction <= PLAYER_FENRIR_STAND_WEAPON_LEFT)
			{
				SetAction(o, FENRIR_STAND);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction == PLAYER_DIE1)
			{
				SetAction(o, FENRIR_STAND);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_RAGE_FENRIR_DAMAGE && o->Owner->CurrentAction <= PLAYER_RAGE_FENRIR_DAMAGE_ONE_LEFT)
			{
				SetAction(o, FENRIR_DAMAGE);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_RAGE_FENRIR && o->Owner->CurrentAction <= PLAYER_RAGE_FENRIR_ONE_LEFT)
			{
				SetAction(o, FENRIR_ATTACK_SKILL);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_RAGE_FENRIR_STAND && o->Owner->CurrentAction <= PLAYER_RAGE_FENRIR_STAND_ONE_LEFT)
			{
				SetAction(o, FENRIR_STAND);
				o->Velocity = 0.4f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_SKILL_THRUST && o->Owner->CurrentAction <= PLAYER_SKILL_HP_UP_OURFORCES)
			{
				SetAction(o, FENRIR_STAND);
				o->Velocity = 0.4f;
			}
			else
			{
				if (o->Owner->CurrentAction >= PLAYER_FENRIR_WALK && o->Owner->CurrentAction <= PLAYER_FENRIR_WALK_WEAPON_LEFT)
				{
					SetAction(o, FENRIR_WALK);
					o->Velocity = 1.0f;
				}
				else if (o->Owner->CurrentAction >= PLAYER_FENRIR_RUN && o->Owner->CurrentAction <= PLAYER_FENRIR_RUN_ONE_LEFT_ELF)
				{
					SetAction(o, FENRIR_RUN);
					o->Velocity = 0.6f;
				}
				else if (o->Owner->CurrentAction >= PLAYER_RAGE_FENRIR_RUN && o->Owner->CurrentAction <= PLAYER_RAGE_FENRIR_RUN_ONE_LEFT)
				{
					SetAction(o, FENRIR_RUN);
					o->Velocity = 0.6f;
				}
				else if (o->Owner->CurrentAction >= PLAYER_RAGE_FENRIR_WALK && o->Owner->CurrentAction <= PLAYER_RAGE_FENRIR_WALK_TWO_SWORD)
				{
					SetAction(o, FENRIR_WALK);
					o->Velocity = 1.0f;
				}

				Vector(1.f, 1.f, 1.f, Light);
				if (World == WD_10HEAVEN)
				{
					bool   bWave = false;
					vec3_t p = { 120.f, 0.f, 32.f };

					if (o->AnimationFrame > 1.f && o->AnimationFrame < 1.2f)
					{
						b->TransformPosition(BoneTransform[22], p, Position);	// R Hand
						Position[0] += 40.f;
						bWave = true;
					}
					else if (o->AnimationFrame > 4.8f && o->AnimationFrame <= 5.0f)
					{
						b->TransformPosition(BoneTransform[44], p, Position);	// R Foot
						Position[0] += 40.f;
						Position[2] += 700.f;
						bWave = true;
					}

					if (bWave)
					{
						Vector(Position[0], Position[1], Position[2], p);
						CreateEffectSync(BITMAP_SHOCK_WAVE, p, o->Angle, Light, 1);

						for (int i = 0; i < rand() % 5 + 5; ++i)
						{
							Vector(Position[0] + (float)(rand() % 50 - 25),
								Position[1] + (float)(rand() % 50 - 25),
								Position[2] + (float)(rand() % 16 - 8) - 10, p);
							CreateParticleSync(BITMAP_SMOKE, p, o->Angle, Light);
						}
					}
				}
				else if (rand() % 3 == 0 && !gMapManager->InHellas())
				{
					if (o->Owner && !g_isCharacterBuff(o->Owner, eBuff_Cloaking))
					{
						Vector(o->Position[0] + (float)(rand() % 64 - 32), o->Position[1] + (float)(rand() % 64 - 32), o->Position[2] + (float)(rand() % 32 - 16), Position);
						if (World == WD_2DEVIAS)
						{
							CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, Light);
						}
						else if (World != WD_10HEAVEN && !g_Direction.m_CKanturu.IsMayaScene())
						{
							CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, Light);
						}
					}
				}
			}
		}
		break;
		case Movement::Horse:
		{
			if ((TerrainWall[TERRAIN_INDEX_REPEAT((int)(o->Owner->Position[0] / TERRAIN_SCALE), (int)(o->Owner->Position[1] / TERRAIN_SCALE))] & TW_SAFEZONE) == TW_SAFEZONE && bForceRender == FALSE)
			{
				o->Alpha = 0.f;
				break;
			}
			if (o->Owner->IsHideHorser() || o->Owner->animation_hide_traditional())
			{
				o->Alpha = 0.f;
				break;
			}

			b->BoneHead = 7;
			VectorCopy(o->Owner->HeadAngle, o->HeadAngle);
			VectorCopy(o->Owner->Position, o->Position);
			VectorCopy(o->Owner->Angle, o->Angle);

			if (o->Owner->CurrentAction == PLAYER_HIT_DARKHORSE)
			{
				SetAction(o, 3);
				o->Velocity = 0.34f;
			}
			else if (o->Owner->CurrentAction == PLAYER_RUN_RIDE_HORSE)
			{
				Vector(1.f, 1.f, 1.f, Light);

				SetAction(o, 1);
				if (World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
				{
					bool   bWave = false;
					vec3_t p = { 120.f, 0.f, rand() % 64 - 32.f };

					if (o->AnimationFrame > 1.f && o->AnimationFrame < 1.2f)
					{
						b->TransformPosition(BoneTransform[19], p, Position);
						bWave = true;
					}
					else if (o->AnimationFrame > 1.1f && o->AnimationFrame < 1.4f)
					{
						b->TransformPosition(BoneTransform[25], p, Position);
						bWave = true;
					}
					else if (o->AnimationFrame > 1.3f && o->AnimationFrame < 1.6f)
					{
						b->TransformPosition(BoneTransform[32], p, Position);
						bWave = true;
					}
					else if (o->AnimationFrame > 1.5f && o->AnimationFrame <= 1.8f)
					{
						b->TransformPosition(BoneTransform[38], p, Position);
						bWave = true;
					}

					if (bWave)
					{
						Vector(Position[0] + (float)(rand() % 16 - 8), Position[1] + (float)(rand() % 16 - 8), Position[2], p);
						CreateEffectSync(BITMAP_SHOCK_WAVE, p, o->Angle, Light, 1);

						for (int i = 0; i < rand() % 5 + 5; ++i)
						{
							Vector(Position[0] + (float)(rand() % 50 - 25), Position[1] + (float)(rand() % 50 - 25), Position[2] + (float)(rand() % 16 - 8) - 10, p);
							CreateParticleSync(BITMAP_SMOKE, p, o->Angle, Light);
						}
					}
				}
				else if (rand() % 2 == 0 && !gMapManager->InHellas())
				{
					if (o->Owner && !g_isCharacterBuff(o->Owner, eBuff_Cloaking))
					{
						Vector(o->Position[0] + (float)(rand() % 64 - 32), o->Position[1] + (float)(rand() % 64 - 32), o->Position[2] + (float)(rand() % 32 - 16), Position);

						if (World == WD_2DEVIAS)
						{
							CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, Light);
						}
						else if (World != WD_10HEAVEN && !g_Direction.m_CKanturu.IsMayaScene())
						{
							CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, Light);
						}
					}
				}
				o->Velocity = 0.34f;
			}
			else if (o->Owner->CurrentAction >= PLAYER_HIT_RIDE_STRIKE && o->Owner->CurrentAction <= PLAYER_HIT_RIDE_ATTACK_MAGIC)
			{
				SetAction(o, 2);
				o->Velocity = 0.34f;
			}
			else if (o->Owner->CurrentAction == PLAYER_IDLE1_DARKHORSE)
			{
				SetAction(o, 5);
				o->Velocity = 1.0f;
			}
			else if (o->Owner->CurrentAction == PLAYER_IDLE2_DARKHORSE)
			{
				SetAction(o, 6);
				o->Velocity = 1.0f;
			}
			else
			{
				SetAction(o, 0);
				o->LifeTime = (0);
				o->WeaponLevel = 0;
				o->Velocity = 0.3f;
			}

			if (o->CurrentAction != 1
				&& o->Owner
				&& !g_isCharacterBuff(o->Owner, eBuff_Cloaking)
				&& rand() % 3 == 0)
			{
				vec3_t p = { 50.f, -4.f, 0.f };
				b->TransformPosition(BoneTransform[27], p, Position);

				if (World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3)
				{
					CreateParticleSync(BITMAP_BUBBLE, Position, o->Angle, o->Light);
				}
				else
				{
					CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, o->Light);
				}
			}

			if (o->CurrentAction == 3)
			{
				vec3_t Pos1, Pos2, p, p2;

				Vector(60.f, 0.f, 0.f, Pos1);
				Vector(0.f, 0.f, 0.f, Pos2);
				Vector(1.f, 1.f, 1.f, Light);
				b->TransformPosition(BoneTransform[9], Pos1, p);
				b->TransformPosition(BoneTransform[9], Pos2, p2);
				CreateBlur(Hero, p, p2, Light, 0);
			}
			if (o->CurrentAction == 3)
			{
				RenderDarkHorseSkill(o, b);
			}
			if (o->Owner->ExtState == 1)
			{
				vec3_t p;
				vec3_t Angle = { 0.f, 0.f, 0.f };
				float  Matrix[3][4];

				Vector(0.f, 50, 0.f, p);
				for (int i = 0; i < 4; ++i)
				{
					Angle[2] += 90.f;
					AngleMatrix(Angle, Matrix);
					VectorRotate(p, Matrix, Position);
					VectorAdd(o->Position, Position, Position);

					for (int j = 0; j < 3; ++j)
					{
						CreateJointSync(BITMAP_FLARE, Position, Position, Angle, 0, o, 40, 2);
					}
				}
			}
			o->Owner->ExtState = 0;
			o->Live = o->Owner->Live;
		}
		break;
		case Movement::Uniria:
		case Movement::Dinorant:
		{
			if ((TerrainWall[TERRAIN_INDEX_REPEAT((int)(o->Owner->Position[0] / TERRAIN_SCALE), (int)(o->Owner->Position[1] / TERRAIN_SCALE))] & TW_SAFEZONE) == TW_SAFEZONE
				&& bForceRender == FALSE)
			{
				o->Alpha = 0.f;
				break;
			}

			if (o->Owner->IsHideRiders() || o->Owner->animation_hide_traditional())
			{
				o->Alpha = 0.f;
				break;
			}

			if (o->Owner->Teleport == TELEPORT_BEGIN || o->Owner->Teleport == TELEPORT)
			{
				o->Alpha -= (0.1f);
				if (o->Alpha < 0)
					o->Alpha = 0.f;
			}
			else
			{
				o->Alpha = 1.f;
			}

			VectorCopy(o->Owner->Position, o->Position);

			if (o->movementType == Movement::Dinorant)
			{
				if (World == WD_8TARKAN || World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
				{
					o->Position[2] -= (10.f);
				}
				else if (World != -1)
				{
					o->Position[2] -= (30.f);
				}
			}
			VectorCopy(o->Owner->Angle, o->Angle);
			if (o->Owner->CurrentAction >= PLAYER_WALK_MALE && o->Owner->CurrentAction <= PLAYER_RUN_RIDE_WEAPON
				|| o->Owner->CurrentAction == PLAYER_FLY_RIDE || o->Owner->CurrentAction == PLAYER_FLY_RIDE_WEAPON
				|| o->Owner->CurrentAction == PLAYER_RAGE_UNI_RUN || o->Owner->CurrentAction == PLAYER_RAGE_UNI_RUN_ONE_RIGHT
				)
			{
				if (o->movementType == Movement::Dinorant)
				{
					if (World == WD_8TARKAN || World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
						SetAction(o, 3);
					else
						SetAction(o, 2);
				}
				else
				{
					SetAction(o, 2);
				}

				if (rand() % 2 == 0 && World != WD_10HEAVEN)
				{
					if (!g_Direction.m_CKanturu.IsMayaScene())
					{
						if (o->Owner && !g_isCharacterBuff(o->Owner, eBuff_Cloaking))
						{
							Vector(1.f, 1.f, 1.f, Light);
							Vector(o->Position[0] + (float)(rand() % 64 - 32), o->Position[1] + (float)(rand() % 64 - 32), o->Position[2] + (float)(rand() % 32 - 16), Position);
							if (World == 2)
								CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, Light);
							else
								CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, Light);
						}
					}
				}
			}
			else if (o->Owner->CurrentAction == PLAYER_SKILL_RIDER || o->Owner->CurrentAction == PLAYER_SKILL_RIDER_FLY)
			{
				if (World == WD_8TARKAN || World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
					SetAction(o, 7);
				else
					SetAction(o, 6);
			}
			else if ((o->Owner->CurrentAction >= PLAYER_HIT_FIST && o->Owner->CurrentAction <= PLAYER_HIT_RIDE_CROSSBOW) || IsAliceRideAction_UniDino(o->Owner->CurrentAction) == true)
			{
				if (o->movementType == Movement::Dinorant)
				{
					if (World == WD_8TARKAN || World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
						SetAction(o, 5);
					else
						SetAction(o, 4);
				}
				else
				{
					SetAction(o, 3);
				}
			}
			else
			{
				if (o->movementType == Movement::Dinorant)
				{
					if (World == WD_8TARKAN || World == WD_10HEAVEN || g_Direction.m_CKanturu.IsMayaScene())
						SetAction(o, 1);
					else
						SetAction(o, 0);
				}
				else
				{
					SetAction(o, 0);
				}
			}
			o->Velocity = 0.34f;
			o->Live = o->Owner->Live;
		}
		break;
		case Movement::Angel:
		{
			if (o->Owner && !g_isCharacterBuff(o->Owner, eBuff_Cloaking))
			{
				vec3_t Light;
				FlyRange = 150.f;
				Vector(0.4f, 0.4f, 0.4f, Light);
				for (j = 0; j < 4; j++)
				{
					Vector((float)(rand() % 16 - 8), (float)(rand() % 16 - 8), (float)(rand() % 16 - 8), Position);
					VectorAdd(Position, o->Position, Position);
					CreateParticleSync(BITMAP_SPARK, Position, o->Angle, Light, 1);
				}
			}
		}
		break;
		case Movement::Muun2Pet:
		{
			vec3_t Range;
			float MoveRange = 150.f;

			if (o->Owner->Teleport == TELEPORT_BEGIN || o->Owner->Teleport == TELEPORT)
			{
				o->Alpha -= (0.1f);
				if (o->Alpha < 0)
					o->Alpha = 0.f;
			}
			else
			{
				o->Alpha = 1.f;
			}

			VectorSubtract(TargetPosition, o->Position, Range);

			float Distance = Range[0] * Range[0] + Range[1] * Range[1];

			VectorCopy(o->Position, Position);

			float diff1 = ((TargetPosition[0] > Position[0]) ? TargetPosition[0] - Position[0] : Position[0] - TargetPosition[0]);
			float diff2 = ((TargetPosition[1] > Position[1]) ? TargetPosition[1] - Position[1] : Position[1] - TargetPosition[1]);

			if (Distance >= MoveRange * MoveRange)
			{
				float Speed, Angle, Angle2;

				Angle = atan(diff2 / diff1) * 180 / Q_PI;
				Angle2 = atan2(diff1, diff2) * 180 / Q_PI;

				if (Distance >= (MoveRange * 2) * (MoveRange * 2))
				{
					Speed = 15.0f;
					o->Velocity = 0.89000002f;
				}
				else if (Distance >= (MoveRange + (MoveRange / 2)) * (MoveRange + (MoveRange / 2)))
				{
					Speed = 13.0f;
					o->Velocity = 0.69000002f;
				}
				else
				{
					Speed = 10.0f;
					o->Velocity = 0.40000001f;
				}

				double MoveX = Speed * (cos(Angle * Q_PI / 180));
				double MoveY = Speed * (sin(Angle * Q_PI / 180));

				if (Position[0] < TargetPosition[0] && Position[1] < TargetPosition[1])
				{
					o->Position[0] += (float)timefac(MoveX);
					o->Position[1] += (float)timefac(MoveY);
					o->Angle[2] = 180;
					o->Angle[2] -= (Angle2);
				}
				else if (Position[0] < TargetPosition[0] && Position[1] > TargetPosition[1])
				{
					o->Position[0] += (float)timefac(MoveX);
					o->Position[1] -= (float)timefac(MoveY);
					o->Angle[2] = (Angle2);
				}
				else if (Position[0] > TargetPosition[0] && Position[1] > TargetPosition[1])
				{
					o->Position[0] -= (float)timefac(MoveX);
					o->Position[1] -= (float)timefac(MoveY);
					o->Angle[2] = 360;
					o->Angle[2] -= (Angle2);
				}
				else if (Position[0] > TargetPosition[0] && Position[1] < TargetPosition[1])
				{
					o->Position[0] -= (float)timefac(MoveX);
					o->Position[1] += (float)timefac(MoveY);
					o->Angle[2] = 180;
					o->Angle[2] += (Angle2);
				}
				o->CurrentAction = 2;
			}
			else if (Distance < 70 * 70)
			{
				if ((o->Owner->CurrentAction < 15 || o->Owner->CurrentAction > 37))
				{
					vec3_t Direction;
					o->CurrentAction = 2;

					AngleMatrix(o->Angle, o->Matrix);
					VectorRotate(o->Direction, o->Matrix, Direction);
					interpolatesum(o->Position, Direction);
				}
			}
			else
			{
				if (GetTickCount() <= o->Timer + 800)
				{
					o->CurrentAction = 1;
				}
				else if (GetTickCount() <= o->Timer + 12000)
				{
					o->CurrentAction = 0;
				}
				else
				{
					o->Timer = GetTickCount();
				}
			}

			o->Position[2] = o->Owner->Position[2];

			if (!(rand() % 100))
			{
				float Speed = 0;
				if (Distance >= MoveRange * MoveRange)
				{
					Speed = -(float)(rand() % 64 + 128) * 0.1f;
				}
				else
				{
					Speed = -(float)(rand() % 64 + 16) * 0.1f;
					o->Angle[2] = (float)(rand() % 360);
				}
				o->Direction[0] = 0.f;
				o->Direction[1] = Speed;
				o->Direction[2] = (float)(rand() % 64 - 32) * 0.1f;
			}

			int LootPosition = 900;
			if (Distance >= LootPosition * LootPosition)
			{
				if (o->Position[0] < o->Owner->Position[0])
				{
					float radial = Range[0] + rand() % 360;
					o->Position[0] += timefac(radial);
				}
				else if (o->Position[0] > o->Owner->Position[0])
				{
					float radial = Range[0] - rand() % 360;
					o->Position[0] += timefac(radial);
				}
				else if (o->Position[1] < o->Owner->Position[1])
				{
					float radial = Range[1] + rand() % 360;
					o->Position[1] += timefac(radial);
				}
				else if (o->Position[1] > o->Owner->Position[1])
				{
					float radial = Range[1] - rand() % 360;
					o->Position[1] += timefac(radial);
				}
			}
			if (o->Position[2] < o->Owner->Position[2] + 100.f)
			{
				o->Direction[2] += timefac(1.5f);
			}
			if (o->Position[2] > o->Owner->Position[2] + 200.f)
			{
				o->Direction[2] -= timefac(1.5f);
			}
		}
		break;
		}

		float myPlaySpeed = (o->Velocity);

		b->CurrentAction = o->CurrentAction;
		b->PlayAnimation(&o->AnimationFrame, &o->PriorAnimationFrame, &o->PriorAction, myPlaySpeed, o->Position, o->Angle);

		if (o->movementType == Movement::Angel || o->Type == MODEL_HELPER + 1)
		{
			vec3_t Range;
			VectorSubtract(TargetPosition, o->Position, Range);
			float Distance = Range[0] * Range[0] + Range[1] * Range[1];
			if (Distance >= FlyRange * FlyRange)
			{
				float Angle = CreateAngle(o->Position[0], o->Position[1], TargetPosition[0], TargetPosition[1]);
				o->Angle[2] = TurnAngle2(o->Angle[2], Angle, 20.f);
			}
			AngleMatrix(o->Angle, o->Matrix);
			vec3_t Direction;
			VectorRotate(o->Direction, o->Matrix, Direction);
			interpolatesum(o->Position, Direction);

			o->Position[2] += timefac((float)(rand() % 16 - 8));

			if (rand() % 32 == 0 && checkNormalizer)
			{
				float Speed = 0;
				if (Distance >= FlyRange * FlyRange)
					Speed = -(float)(rand() % 64 + 128) * 0.1f;
				else
				{
					Speed = -(float)(rand() % 64 + 16) * 0.1f;
					o->Angle[2] = (float)(rand() % 360);
				}
				o->Direction[0] = 0.f;
				o->Direction[1] = Speed;
				o->Direction[2] = (float)(rand() % 64 - 32) * 0.1f;
			}
			if (o->Position[2] < o->Owner->Position[2] + 100.f)
				o->Direction[2] += timefac(1.5f);

			if (o->Position[2] > o->Owner->Position[2] + 200.f)
				o->Direction[2] -= timefac(1.5f);
		}
	}
	return TRUE;
}


bool RenderBug(OBJECT* o, bool bForceRender)
{
	if (o->Live)
	{
		OBJECT* to = o->Owner;
		o->Visible = (bForceRender == FALSE ? TestFrustrum2D(o->Position[0] * 0.01f, o->Position[1] * 0.01f, -20.f) : true);

		if (o->Visible && o->Alpha > 0)
		{
			if (o->Owner->Type != MODEL_PLAYER && o->Type != MODEL_HELPER)
				return TRUE;

			if (bForceRender == false && o->Owner->animation_hide_traditional())
			{
				return TRUE;
			}

			if (bForceRender)
			{
				o->Scale = o->Scale;
				o->Scale = (float)((to->Scale / 0.01) * (o->BackupScale * 0.01));
			}
			else if (SceneFlag == CHARACTER_SCENE)
			{
				o->Scale = (float)((to->Scale / 0.01) * (o->BackupScale * 0.01));
			}
			else
			{
				o->Scale = (float)((to->Scale / 0.01) * (o->BackupScale * 0.01));
			}

			int State = 0;
			if (g_isCharacterBuff(o->Owner, eBuff_Cloaking))
			{
				State = 10;
			}

			RenderObject(o, false, 0, State);

			vec3_t Light;
			float  Luminosity = (float)(rand() % 30 + 70) * 0.01f;

			switch (o->Type)
			{
			case MODEL_HELPER:
				Vector(Luminosity * 0.5f, Luminosity * 0.8f, Luminosity * 0.6f, Light);
				CreateSprite(BITMAP_LIGHT, o->Position, 1.f, Light, o);
				break;
			default:
			{
				BMD* pModel = gmClientModels->GetModel(o->Type);
				if (pModel)
				{
					GMItemEffect->RenderPetEffect((o->Type - MODEL_ITEM), pModel, o);
				}
			}
			break;
			}
		}
	}
	return TRUE;
}

void RenderDarkHorseSkill(OBJECT* pObject, BMD* pModel)
{
	float  Matrix[3][4];
	vec3_t Angle, p, Position;

	if (pObject && pModel)
	{
		pObject->WeaponLevel++;
		pObject->LifeTime += timefac(1.f);

		if (pObject->LifeTime.duration(10))
		{
			CreateEffect(BITMAP_SHOCK_WAVE, pObject->Position, pObject->Angle, pObject->Light);
		}
		if (pObject->AnimationFrame < 8.0 || pObject->AnimationFrame > 9.5)
		{
			if (pObject->LifeTime.numeral(20))
			{
				CreateEffect(MODEL_SKILL_FURY_STRIKE, pObject->Position, pObject->Angle, pObject->Light, 0, pObject, -1, 0, 2);
				pObject->LifeTime = -3;
				pObject->WeaponLevel = -3;
			}
		}
		else
		{
			if (pObject->LifeTime.duration(2))
			{
				Vector(0.f, 150.f * (pObject->LifeTime._runvalue / 2), 0.f, p);
				Vector(0.f, 0.f, (float)(rand() % 360), Angle);
				for (int i = 0; i < 6; ++i)
				{
					Angle[2] += 60.f;
					AngleMatrix(Angle, Matrix);
					VectorRotate(p, Matrix, Position);
					VectorAdd(pObject->Position, Position, Position);
					CreateEffect(MODEL_GROUND_STONE + rand() % 2, Position, pObject->Angle, pObject->Light);
				}
			}
			EarthQuake = (rand() % 3 - 3) * (0.7f);
		}
	}
}

void RenderSkillEarthQuake(CHARACTER* c, OBJECT* o, BMD* b, int iMaxSkill)
{
	if (c == NULL)
		return;

	if (o == NULL)
		return;

	if (b == NULL)
		return;

	float  Matrix[3][4];
	vec3_t Angle, p, Position;
	CHARACTER* pCharacter = gmCharacters->GetCharacter(c->TargetCharacter);

	if (pCharacter)
	{
		OBJECT& TargetO = pCharacter->Object;

		o->WeaponLevel++;

		if (o->WeaponLevel == iMaxSkill - 2 ||
			o->WeaponLevel == iMaxSkill - 1 ||
			o->WeaponLevel == iMaxSkill - 0)
		{
			Vector(0.f, 40.f * (o->WeaponLevel / 2), 0.f, p);
			Vector(0.f, 0.f, (float)(rand() % 360), Angle);
			for (int i = 0; i < 6; ++i)
			{
				Angle[2] += 60.f;
				AngleMatrix(Angle, Matrix);
				VectorRotate(p, Matrix, Position);
				VectorAdd(TargetO.Position, Position, Position);

				int iCurrentStone = MODEL_GROUND_STONE;

				CreateEffect(iCurrentStone + 1, Position, TargetO.Angle, TargetO.Light);
			}
		}

		if (o->WeaponLevel == iMaxSkill - 1)
		{
			EarthQuake = (rand() % 3 - 3) * 0.7f;
			CreateEffect(MODEL_SKILL_FURY_STRIKE, TargetO.Position, TargetO.Angle, TargetO.Light, 0, o, -1, 0, 2);
		}

		if (o->WeaponLevel > iMaxSkill)
		{
			o->WeaponLevel = 0;
		}
	}
}

void DeleteBoids()
{
	for (int i = 0; i < MAX_BOIDS; i++)
	{
		OBJECT* o = &Boids[i];
		o->Live = false;
	}
}

int CreateDragon(OBJECT* o, int index)
{
	if (World != WD_10HEAVEN)
		return 0;

	if (index < 3)
	{
		o->Live = true;
		OpenMonsterModel(31);
		o->Type = MODEL_MONSTER01 + 31;
		o->Scale = (float)(rand() % 3 + 6) * 0.05f;
		o->Alpha = 1.f;
		o->AlphaTarget = o->Alpha;
		o->Velocity = (float)(rand() % 10 + 10) * 0.02f;
		o->Gravity = (float)(rand() % 10 + 10) * 0.05f;
		o->LightEnable = true;
		o->AlphaEnable = false;
		o->SubType = 0;
		o->HiddenMesh = -1;
		o->BlendMesh = -1;
		//o->LifeTime    = 128+rand()%128;
		//o->Timer       = (float)(rand()%10)*0.1f;
		o->CurrentAction = MONSTER01_DIE + 1;
		SetAction(o, o->CurrentAction);
		Vector(0.f, 0.f, (float)(rand() % 360), o->Angle);
		Vector(Hero->Object.Position[0] + (float)(rand() % 4000 - 2000),
			Hero->Object.Position[1] + (float)(rand() % 4000 - 2000),
			Hero->Object.Position[2] - 600.f, o->Position);
	}
	else
	{
		o->Type = MODEL_SPEARSKILL;
		o->Velocity = 2.2f;
		o->LightEnable = false;
		o->LifeTime = 240 * 40.0;
		o->AI = BOID_GROUND;
		Vector(1.f, 1.f, 1.f, o->Light);

		o->AlphaEnable = true;
		o->Scale = 0.8f;
		o->ShadowScale = 10.f;
		o->HiddenMesh = -1;
		o->BlendMesh = -1;
		o->Timer = (float)(rand() % 314) * 0.01f;
		Vector(Hero->Object.Position[0] + (float)(rand() % 1024 - 512),
			Hero->Object.Position[1] + (float)(rand() % 1024 - 512),
			Hero->Object.Position[2], o->Position);

		o->Position[2] = Hero->Object.Position[2];

		if (gMapManager->InBloodCastle() == true)
		{
			CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 3, o, 25.0f);
		}
		else
		{
			CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 1, o, 25.0f);
		}
		Vector(0.f, 0.f, 0.f, o->Angle);
		o->Angle[2] = (float)(rand() % 360);
	}
	return 1;
}

int CreateAtlanseFish(OBJECT* o)
{
	if (World != WD_7ATLANSE && World != WD_67DOPPLEGANGER3)
		return 0;

	if (Hero->Object.Position[1] * 0.01f < 128)
	{
		o->Type = MODEL_FISH01 + 1 + rand() % 2;
		o->Gravity = 15;
		o->AlphaEnable = true;
		o->Scale = 0.8f;
		o->ShadowScale = 10.f;
		o->HiddenMesh = -1;
		o->BlendMesh = -1;
		o->Timer = (float)(rand() % 314) * 0.01f;
		if (rand() % 100 < 90)
		{
			o->Velocity = 0.3f;
		}
		else
		{
			o->Velocity = 0.25f;
		}
		o->LightEnable = false;
		Vector(0.f, 0.f, 0.f, o->Angle);
		Vector(1.f, 1.f, 1.f, o->Light);
		Vector(Hero->Object.Position[0] + (float)(rand() % 1024 - 512),
			Hero->Object.Position[1] + (float)(rand() % 1024 - 512),
			Hero->Object.Position[2], o->Position);

		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + (float)(rand() % 200 + 150);
	}
	else
	{
		o->Live = false;
	}
	return 1;
}

void MoveBat(OBJECT* o)
{
	o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
	o->Position[2] += timefac(-absf(sinf(o->Timer)) * 150.f + 350.f);
	o->Timer += timefac(0.2f);
}

void MoveButterFly(OBJECT* o)
{
	if (rand() % 32 == 0)
	{
		o->Angle[2] = (float)(rand() % 360);
		o->Direction[2] = ((float)(rand() % 15 - 7) * 1.f);
	}
	o->Direction[2] += timefac((float)(rand() % 15 - 7) * 0.2f);
	float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);

	if (o->Position[2] < Height + 50.f)
	{
		o->Direction[2] *= timepow(0.8f);
		o->Direction[2] += timefac(1.f);
	}
	if (o->Position[2] > Height + 300.f)
	{
		o->Direction[2] *= timepow(0.8f);
		o->Direction[2] -= timefac(1.f);
	}
	o->Position[2] += timefac((float)(rand() % 15 - 7) * 0.3f);
}

void MoveBird(OBJECT* o)
{
	if (o->AI == BOID_FLY)
	{
		if ((int)WorldTime % 8192 < 2048)
		{
			vec3_t Range;
			VectorSubtract(o->Position, Hero->Object.Position, Range);
			float Distance = sqrtf(Range[0] * Range[0] + Range[1] * Range[1]);
			if (Distance >= 200.f && Distance <= 400.f)
			{
				o->AI = BOID_DOWN;
			}
		}
		o->Velocity = 1.f;
		o->Position[2] += timefac((float)(rand() % 16 - 8));

		if (o->Position[2] < 200.f)
			o->Direction[2] = 10.f;
		else if (o->Position[2] > 600.f)
			o->Direction[2] = -10.f;
	}
	if (o->AI == BOID_DOWN)
	{
		o->Direction[2] = -20.f;
		float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);
		if (o->Position[2] < Height)
		{
			o->AI = BOID_UP;
			o->Velocity = 1.1f;
			o->Direction[2] = 20.f;
			o->CurrentAction = 0;
		}
	}
	if (o->AI == BOID_GROUND)
	{
		if (Hero->Object.CurrentAction >= PLAYER_WALK_MALE || rand() % 256 == 0)
		{
			o->AI = BOID_UP;
			o->Velocity = 1.1f;
			o->Direction[2] = 20.f;
			o->CurrentAction = 0;
		}
	}
	if (o->AI == BOID_UP)
	{
		o->Position[2] += timefac((float)(rand() % 16 - 8));
		o->Velocity -= timefac(0.005f);
		if (o->Velocity <= 1.f)
		{
			o->AI = BOID_FLY;
		}
	}
}

void MoveHeavenBug(OBJECT* o, int index)
{
	double iFrame = gsteady_clock->GetDeltAccumulated();

	o->Position[0] += timefac(o->Velocity * (float)sinf(o->Angle[2]));
	o->Position[1] -= timefac(o->Velocity * (float)cosf(o->Angle[2]));
	double angle = 0.01f * cosf((float)(34571 + iFrame + index * 41273) * 0.0003f) * sinf((float)(17732 + iFrame + index * 5161) * 0.0003f);
	o->Angle[2] += timefac(angle);

	float dx = o->Position[0] - Hero->Object.Position[0];
	float dy = o->Position[1] - Hero->Object.Position[1];
	float Range = sqrtf(dx * dx + dy * dy);

	if (Range >= 1500.f)
		o->Live = false;
	if (rand() % 5120 == 0)
		o->Live = false;

	if (gMapManager->InBloodCastle())
	{
		if (o->LifeTime <= 0)
		{
			o->Live = false;
		}
	}
}

void MoveEagle(OBJECT* o)
{
	float fSeedAngle = WorldTime * 0.001f;
	float fFlyRange = o->Gravity;
	float fAngle = 0;

	if (o->SubType == 0 && rand() % 120 == 0)
	{
		o->SubType = 1;
		o->AnimationFrame = 0;
	}
	else if (o->SubType == 1 && o->AnimationFrame == 24)
	{
		o->SubType = 2;
	}
	else if (o->SubType == 2 && o->AnimationFrame == 0)
	{
		o->SubType = 0;
	}
	else if (o->SubType == 0)
	{
		o->AnimationFrame = 0;
	}

	if (o->AI == BOID_FLY)
	{
		o->HeadAngle[0] = cosf(fSeedAngle) * fFlyRange;
		o->HeadAngle[1] = sinf(fSeedAngle) * fFlyRange;
		fAngle = CreateAngle(o->Position[0], o->Position[1], o->Position[0] + o->HeadAngle[0], o->Position[1] + o->HeadAngle[1]);

		if (o->HeadAngle[2] == 0 && o->HeadAngle[0] > o->HeadAngle[1])
		{
			o->HeadAngle[2] = 1;
		}
		else if (o->HeadAngle[2] == 1 && o->HeadAngle[0] < o->HeadAngle[1])
		{
			o->HeadAngle[2] = 2;
		}
		else if (o->HeadAngle[2] == 2 && o->HeadAngle[0] > o->HeadAngle[1])
		{
			o->AI = BOID_GROUND;
			o->HeadAngle[2] = 0;

			o->HeadAngle[0] = sinf(fSeedAngle) * fFlyRange;
			o->HeadAngle[1] = cosf(fSeedAngle) * fFlyRange;
			fAngle = CreateAngle(o->Position[0], o->Position[1], o->Position[0] + o->HeadAngle[0], o->Position[1] + o->HeadAngle[1]);
		}
	}
	else if (o->AI == BOID_GROUND)
	{
		o->HeadAngle[0] = sinf(fSeedAngle) * fFlyRange;
		o->HeadAngle[1] = cosf(fSeedAngle) * fFlyRange;
		fAngle = CreateAngle(o->Position[0], o->Position[1], o->Position[0] + o->HeadAngle[0], o->Position[1] + o->HeadAngle[1]);

		if (o->HeadAngle[2] == 0 && o->HeadAngle[0] < o->HeadAngle[1])
		{
			o->HeadAngle[2] = 1;
		}
		else if (o->HeadAngle[2] == 1 && o->HeadAngle[0] > o->HeadAngle[1])
		{
			o->HeadAngle[2] = 2;
		}
		else if (o->HeadAngle[2] == 2 && o->HeadAngle[0] < o->HeadAngle[1])
		{
			o->AI = BOID_FLY;
			o->HeadAngle[2] = 0;

			o->HeadAngle[0] = cosf(fSeedAngle) * fFlyRange;
			o->HeadAngle[1] = sinf(fSeedAngle) * fFlyRange;
			fAngle = CreateAngle(o->Position[0], o->Position[1], o->Position[0] + o->HeadAngle[0], o->Position[1] + o->HeadAngle[1]);
		}
	}

	o->Position[0] += timefac(o->HeadAngle[0]);
	o->Position[1] += timefac(o->HeadAngle[1]);
	o->Position[2] += timefac(sinf(WorldTime * 0.0005f) * 1.0f);
	o->Angle[1] += timefac(sinf(WorldTime * 0.001f) * 0.4f);
	o->Angle[2] = fAngle + 270;
}

void MoveTornado(OBJECT* o)
{
	o->Scale = 1.0f;
	if (rand() % 500 == 0)
	{
		o->HeadAngle[0] = (rand() % 314) / 100.0f;
	}
	o->Position[0] += timefac(sinf(o->HeadAngle[0]) * 2.0f);
	o->Position[1] += timefac(cosf(o->HeadAngle[0]) * 2.0f);
	o->Angle[2] = 0;

	if (o->BlendMeshLight < 1.0f)
		o->BlendMeshLight += timefac(0.1f);
}

void MoveBoidGroup(OBJECT* o, int index)
{
	if (o->AI != BOID_GROUND)
	{
		if (o->Type != MODEL_BUTTERFLY01 || rand() % 4 == 0)
			MoveBoid(o, index, Boids, MAX_BOIDS);
		AngleMatrix(o->Angle, o->Matrix);
		vec3_t p, Direction;
		if (World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3)
		{
			if (o->Timer < 5.f)
			{
				if (index < 35)
				{
					Vector(o->Velocity * (float)(rand() % 16 + 8), 0.f, o->Direction[2], Direction);
				}
				else
				{
					Vector(o->Velocity * (float)(rand() % 16 + 16), 0.f, o->Direction[2], Direction);
				}
				o->Gravity = 15;
			}
			else
			{
				Vector(o->Velocity * (float)(rand() % 32 + 32), 0.f, o->Direction[2], Direction);
				o->Gravity = 5;
			}
			o->Timer += timefac(0.1f);
			if (o->Timer >= 10)
			{
				o->Timer = 0.f;
			}
		}
		else
		{
			Vector(o->Velocity * 25.f, 0.f, o->Direction[2], Direction);
		}
		VectorRotate(Direction, o->Matrix, p);
		interpolatesum(o->Position, p);
		o->Direction[0] = o->Position[0] + 3.f * p[0];
		o->Direction[1] = o->Position[1] + 3.f * p[1];

		float dx = o->Position[0] - Hero->Object.Position[0];
		float dy = o->Position[1] - Hero->Object.Position[1];
		float Range = sqrtf(dx * dx + dy * dy);
		float FlyDistance = 1500.f;
		if (o->Type == MODEL_MONSTER01 + 31)
		{
			FlyDistance = 4000.f;
		}
		else if (o->Type == MODEL_MONSTER01 + 33)
		{
			FlyDistance = 3000.f;
		}
		else if (World == WD_51HOME_6TH_CHAR
			|| World == WD_77NEW_LOGIN_SCENE
			|| World == WD_78NEW_CHARACTER_SCENE
			);
		else
		{
			if (rand() % 512 == 0)
				o->Live = false;
		}
		if (World == WD_77NEW_LOGIN_SCENE ||
			World == WD_78NEW_CHARACTER_SCENE
			);
		else
			if (Range >= FlyDistance)
				o->Live = false;
	}
}

void MoveBoids()
{
	if (EnableEvent != 0)
	{
		OBJECT* o = &Hero->Object;
		vec3_t Position, Angle, Light;
		if (rand() % 40 == 0 && checkNormalizer)
		{
			Vector(Hero->Object.Position[0] + (float)(rand() % 600 - 200), Hero->Object.Position[1] + (float)(rand() % 400 + 200), Hero->Object.Position[2] + 300.f, Position);
			Vector(0.f, 0.f, 0.f, Angle);
			Vector(1.f, 1.f, 1.f, Light);
			CreateEffect(MODEL_FIRE, Position, Angle, Light, 3);
			PlayBuffer(SOUND_METEORITE01);
		}
		Vector(-0.3f, -0.3f, -0.2f, Light);
		AddTerrainLight(o->Position[0], o->Position[1], Light, 16, PrimaryTerrainLight);
	}

	int Index = TERRAIN_INDEX_REPEAT((int)(Hero->Object.Position[0] / TERRAIN_SCALE), (int)(Hero->Object.Position[1] / TERRAIN_SCALE));

	int dragon_create_max = 0;

	for (int i = 0; i < MAX_BOIDS; i++)
	{
		bool bOut = false;
		bool bCreate = true;
		switch (World)
		{
		case WD_7ATLANSE:
		case WD_67DOPPLEGANGER3:
			break;
		case WD_10HEAVEN:
			if (i >= 13)
			{
				bOut = true;
			}
			break;
		case WD_51HOME_6TH_CHAR:
			if (i >= 2)
			{
				bOut = TRUE;
			}
			break;
		case WD_77NEW_LOGIN_SCENE:
		case WD_78NEW_CHARACTER_SCENE:
			if (i >= 20)
			{
				bOut = TRUE;
			}
			break;
		default:
			if (gMapManager->InHellas() == true)
			{
				if (i > 1)
				{
					bOut = true;
				}
				if ((rand() % 10))
				{
					bCreate = false;
				}
				break;
			}

			if (i >= 5)
			{
				bOut = true;
			}
			break;
		}

		if (EnableEvent != 0 && dragon_create_max <= 3)
		{
			bOut = false;
		}

		if (bOut)
		{
			break;
		}

		OBJECT* o = &Boids[i];

		if (!o->Live && bCreate && checkNormalizer)
		{
			if (EnableEvent != 0 && dragon_create_max <= 3)
			{
				dragon_create_max++;
				if (rand() % 300 == 0)
				{
					o->Live = true;
					OpenMonsterModel(31);
					o->Type = MODEL_MONSTER01 + 31;
					o->Scale = (float)(rand() % 3 + 6) * 0.1f;
					o->Alpha = 1.f;
					o->AlphaTarget = 1.f;
					o->Velocity = 0.5f;
					o->LightEnable = true;
					o->AlphaEnable = false;
					o->SubType = 0;
					o->HiddenMesh = -1;
					o->BlendMesh = -1;
					o->LifeTime = 128 + rand() % 128;
					o->Timer = (float)(rand() % 10) * 0.1f;
					Vector(0.f, 0.f, -90.f, o->Angle);
					if (EnableEvent == 3)
					{
						o->SubType = 1;
					}
					Vector(Hero->Object.Position[0] + (float)(rand() % 600 - 100), Hero->Object.Position[1] + (float)(rand() % 400 + 200), Hero->Object.Position[2] + 300.f, o->Position);
				}
			}
			else if (World == WD_0LORENCIA
				|| World == WD_1DUNGEON
				|| World == WD_3NORIA
				|| World == WD_4LOSTTOWER
				|| World == WD_10HEAVEN
				|| ((World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3) && (TerrainWall[Index] == 0 || TerrainWall[Index] == TW_CHARACTER))
				|| gMapManager->InBloodCastle()
				|| gMapManager->InHellas()
				|| (World == WD_51HOME_6TH_CHAR && i < 1 && rand() % 500 == 0 && Hero->SafeZone != true)
				)
			{
				int iCreateBoid = 0;

				o->Initialize();

				o->Live = true;
				o->Velocity = 1.f;
				o->LightEnable = true;
				o->LifeTime = 0;
				o->SubType = 0;
				Vector(0.5f, 0.5f, 0.5f, o->Light);
				o->Alpha = 0.f;
				o->AlphaTarget = 1.f;
				o->Gravity = 13;
				o->AI = 0;
				o->CurrentAction = 0;

				if (World == WD_0LORENCIA)
					o->Type = MODEL_BIRD01;
				else if (World == WD_1DUNGEON || World == WD_4LOSTTOWER)
					o->Type = MODEL_BAT01;
				else if (World == WD_3NORIA)
				{
					o->Type = MODEL_BUTTERFLY01;
					o->Velocity = 0.3f;
					o->LightEnable = false;
					Vector(1.f, 1.f, 1.f, o->Light);
				}
				else if (gMapManager->InBloodCastle() == true)
				{
					o->Type = MODEL_CROW;
				}
				else if (World == WD_51HOME_6TH_CHAR)
				{
					o->Type = MODEL_MAP_TORNADO;
					o->Velocity = 0.0f;
					o->BlendMeshLight = 0.0f;
					o->HeadAngle[0] = (rand() % 314) / 100.0f;
					o->Position[1] += 5.0f;
					o->m_bRenderAfterCharacter = true;
				}
				else
				{
					if (iCreateBoid == 0)
					{
						iCreateBoid = CreateDragon(o, i);
					}
					if (iCreateBoid == 0)
					{
						iCreateBoid = CreateAtlanseFish(o);
					}
					if (iCreateBoid == 0)
					{
						iCreateBoid = CreateBigMon(o);
					}
				}
				if (iCreateBoid != 1)
				{
					o->AlphaEnable = true;
					o->Scale = 0.8f;
					o->ShadowScale = 10.f;
					o->HiddenMesh = -1;
					o->BlendMesh = -1;
					o->Timer = (float)(rand() % 314) * 0.01f;
					Vector(Hero->Object.Position[0] + (float)(rand() % 1024 - 512), Hero->Object.Position[1] + (float)(rand() % 1024 - 512), Hero->Object.Position[2], o->Position);
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + (float)(rand() % 200 + 150);
					Vector(0.f, 0.f, 0.f, o->Angle);
				}
			}
		}

		if (o->Live)
		{
			BMD* pModel = gmClientModels->GetModel(o->Type);

			if (pModel)
			{
				float PlaySpeed = 1.f;

				if (o->Type == MODEL_MONSTER01 + 31 || o->Type == MODEL_MONSTER01 + 33)
				{
					PlaySpeed = 0.5f;
				}

				if (EnableEvent != 0 && o->Type == MODEL_MONSTER01 + 31)
				{
					vec3_t Position, Direction;

#ifdef fps_new_system
					PlaySpeed *= gsteady_clock->GetNormalizerFps();
#endif // fps_new_system
					SetAction(o, MONSTER01_DIE + 1);
					pModel->CurrentAction = o->CurrentAction;
					pModel->PlayAnimation(&o->AnimationFrame, &o->PriorAnimationFrame, &o->PriorAction, PlaySpeed, o->Position, o->Angle);
					AngleMatrix(o->Angle, o->Matrix);

					Vector(o->Scale * 40.f, 0.f, 0.f, Position);
					VectorRotate(Position, o->Matrix, Direction);
					interpolatesum(o->Position, Direction);

					float heroTerrain = RequestTerrainHeight(Hero->GetPositionX(), Hero->GetPositionY());
					float reqTerrain = RequestTerrainHeight(o->Position[0], o->Position[1]);

					if (heroTerrain - reqTerrain >= 20.f)
					{
						o->Position[2] = heroTerrain + 300.f;
					}
					else
					{
						o->Position[2] = reqTerrain + 300.f;
					}

					o->Position[2] += (-(absf(sinf(o->Timer))) * 100.f + 100.f);

					o->Timer += timefac(o->Scale * 0.05f);
					o->LifeTime-= timefac(1.f);

					if (o->LifeTime <= 0)
						o->Live = false;
					if (rand() % 128 == 0)
						PlayBuffer(SOUND_MONSTER + 124);
				}
				else
				{
					pModel->CurrentAction = o->CurrentAction;

					if (World == WD_51HOME_6TH_CHAR
						|| World == WD_77NEW_LOGIN_SCENE
						|| World == WD_78NEW_CHARACTER_SCENE)
					{
						if (pModel->GetAction(o->CurrentAction))
							PlaySpeed = pModel->GetAction(pModel->CurrentAction)->PlaySpeed;
					}

					pModel->PlayAnimation(&o->AnimationFrame, &o->PriorAnimationFrame, &o->PriorAction, PlaySpeed, o->Position, o->Angle);

					switch (o->Type)
					{
					case MODEL_BAT01:
						MoveBat(o);
						break;
					case MODEL_BUTTERFLY01:
						MoveButterFly(o);
						break;
					case MODEL_BIRD01:
					case MODEL_CROW:
						MoveBird(o);
						break;
					case MODEL_SPEARSKILL:
						MoveHeavenBug(o, i);
						break;
					case MODEL_MONSTER01 + 33:
						MoveBigMon(o);
						break;
					case MODEL_EAGLE:
						MoveEagle(o);
						break;
					case MODEL_MAP_TORNADO:
						MoveTornado(o);
						break;
					}

					MoveBoidGroup(o, i);

					if (o->LifeTime <= 0 && (World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3) && TerrainWall[Index] == TW_SAFEZONE)
					{
						o->Angle[2] += (180.f);

						if (o->Angle[2] >= 360.f)
							o->Angle[2] -= 360.f;

						o->LifeTime = 10;
						o->SubType++;
					}

					if (!(o->Type == MODEL_EAGLE || o->Type == MODEL_MAP_TORNADO) && o->SubType >= 2)
						o->Live = false;

					o->LifeTime--;

					float dx = o->Position[0] - Hero->Object.Position[0];
					float dy = o->Position[1] - Hero->Object.Position[1];
					float Range = sqrtf(dx * dx + dy * dy);

					if (Range < 600)
					{
						if (o->Type == MODEL_BIRD01)
						{
							if (rand() % 512 == 0)
								PlayBuffer(SOUND_BIRD01, o);
							if (rand() % 512 == 0)
								PlayBuffer(SOUND_BIRD02, o);
						}
						else if (o->Type == MODEL_BAT01)
						{
							if (rand() % 256 == 0)
								PlayBuffer(SOUND_BAT01, o);
						}
						else if (o->Type == MODEL_CROW)
						{
							if (TerrainWall[Index] == TW_SAFEZONE)
							{
								if (rand() % 128 == 0)
									PlayBuffer(SOUND_CROW, o);
							}
						}
					}
				}
				Alpha(o);
			}
		}
	}
}

void RenderBoids(bool bAfterCharacter)
{
	for (int i = 0; i < MAX_BOIDS; i++)
	{
		OBJECT* o = &Boids[i];
		if (o->m_bRenderAfterCharacter != bAfterCharacter)
			continue;

		if (MODEL_SPEARSKILL != o->Type)
		{
			o->Angle[2] += (90.f);
		}
		if (o->Live)
		{
			o->Visible = TestFrustrum2D(o->Position[0] * 0.01f, o->Position[1] * 0.01f, -20.f);

			if (o->Visible)
			{
				if (MODEL_SPEARSKILL == o->Type)
				{
					continue;
				}

				if (World == WD_77NEW_LOGIN_SCENE && i == 0 && !CCameraMove::GetInstancePtr()->IsTourMode())
				{
					continue;
				}

				RenderObject(o, true);

				BMD* b = gmClientModels->GetModel(o->Type);

				if (b)
				{
					vec3_t p, Position, Light;
					switch (o->Type)
					{
					case MODEL_MONSTER01 + 31:
						if (o->SubType == 1)
						{
							float Bright = 1.0f;
							RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_METAL | RENDER_BRIGHT, Bright);
							RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_CHROME | RENDER_BRIGHT, Bright);
						}
						if (EnableEvent != 0)
						{
							Vector(0.f, -50.f, 0.f, p);
							b->TransformPosition(BoneTransform[11], p, Position);
							Vector(1.f, 0.f, 0.f, Light);
							CreateSprite(BITMAP_LIGHTNING + 1, Position, 1.f, Light, o);
							Vector(1.f, 1.f, 1.f, Light);
							CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
						}
						break;
					case MODEL_BUTTERFLY01:
					{
						float  Luminosity = (float)(rand() % 32 + 64) * 0.01f;
						Vector(Luminosity * 0.2f, Luminosity * 0.4f, Luminosity * 0.4f, Light);
						CreateSprite(BITMAP_LIGHT, o->Position, 1.f, Light, o);
					}
					break;
					case MODEL_MAP_TORNADO:
					{
						CreateParticleSync(BITMAP_CLOUD, o->Position, o->Angle, o->Light, 18, o->Scale, o);
					}
					break;
					case MODEL_CROW:
						Vector(-5.f, 0.f, 0.f, p);
						b->TransformPosition(BoneTransform[1], p, Position, true);

						float Luminosity = (float)(rand() % 32 + 128) * 0.01f;
						Vector(Luminosity * 1.f, Luminosity * 0.2f, 0.f, Light);
						CreateSprite(BITMAP_LIGHT, Position, 0.1f, Light, o);

						Vector(5.f, 0.f, 0.f, p);
						b->TransformPosition(BoneTransform[1], p, Position, true);
						CreateSprite(BITMAP_LIGHT, Position, 0.1f, Light, o);
						break;
					}
					if (World != WD_10HEAVEN)
					{
						EnableAlphaTest();
						if (o->Type == MODEL_EAGLE)
						{
							if (o->ShadowScale == 0)
								glColor4f(0.f, 0.f, 0.f, 0.0f);
							else
								glColor4f(0.f, 0.f, 0.f, 1.0f);
						}
						else
							glColor4f(0.f, 0.f, 0.f, 0.2f);

						if (World == WD_51HOME_6TH_CHAR && o->Type == MODEL_MAP_TORNADO);
						else
						{
							VectorCopy(o->Position, Position);
							Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
							VectorCopy(Position, b->BodyOrigin);

							b->RenderBodyShadow();
						}
					}
				}
			}
		}
		if (MODEL_SPEARSKILL != o->Type)
		{
			o->Angle[2] -= 90.f;
		}
	}
}

void RenderFishs()
{
	if (g_pOption->GetRenderEffect() == false)
	{
		return;
	}

	for (int i = 0; i < MAX_FISHS; i++)
	{
		OBJECT* o = &Fishs[i];
		o->Angle[2] += 90.f;

		if (o->Live)
		{
			o->Visible = TestFrustrum2D(o->Position[0] * 0.01f, o->Position[1] * 0.01f, -20.f);

			if (o->Visible && o->Type != -1)
			{
				RenderObject(o);

				if (!(o->Type == MODEL_FISH01 + 7 || o->Type == MODEL_FISH01 + 8))
				{
					if (World != WD_10HEAVEN)
					{
						EnableAlphaTest();
						glColor4f(0.f, 0.f, 0.f, 0.2f);
						BMD* b = gmClientModels->GetModel(o->Type);
						if (b)
						{
							vec3_t Position;
							VectorCopy(o->Position, Position);
							Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
							VectorCopy(Position, b->BodyOrigin);
							b->RenderBodyShadow();
						}
					}
				}
			}
		}
		o->Angle[2] -= 90.f;
	}
}


void MoveFishs()
{
	for (int i = 0; i < MAX_FISHS; i++)
	{
		if (World != WD_7ATLANSE && World != WD_8TARKAN
			&& gMapManager->InHellas() == false
			&& World != WD_34CRYWOLF_1ST
			&& World != WD_67DOPPLEGANGER3
			)
		{
			if (i >= 3)
				continue;
		}

		OBJECT* o = &Fishs[i];
		if (!o->Live && checkNormalizer)
		{
			Vector(Hero->Object.Position[0] + (float)(rand() % 1024 - 512),
				Hero->Object.Position[1] + (float)(rand() % 1024 - 512),
				Hero->Object.Position[2], o->Position);
			int Index = TERRAIN_INDEX_REPEAT((int)(o->Position[0] / TERRAIN_SCALE), (int)(o->Position[1] / TERRAIN_SCALE));
			if ((World == WD_0LORENCIA && TerrainMappingLayer1[Index] == 5) ||
				(World == WD_1DUNGEON && TerrainWall[Index] < TW_NOGROUND) ||
				(World == WD_6STADIUM && TerrainWall[Index] < TW_NOGROUND) ||
				((World == WD_7ATLANSE
					|| World == WD_67DOPPLEGANGER3
					) && (TerrainWall[Index] == 0 || TerrainWall[Index] == TW_CHARACTER)) ||
				(gMapManager->InHellas() && (TerrainWall[Index] == 0 || TerrainWall[Index] == TW_CHARACTER)) ||
				(M33Aida::IsInAida() && (TerrainWall[Index] == 0 || TerrainWall[Index] == TW_CHARACTER)) ||
				((World == WD_8TARKAN
					|| World == WD_34CRYWOLF_1ST
					) && (TerrainWall[Index] == 0 || TerrainWall[Index] == TW_CHARACTER))
				)
			{
				o->Live = true;
				o->Alpha = 0.f;
				o->AlphaTarget = 1.f;
				o->BlendMesh = -1;
				Vector(0.5f, 0.5f, 0.5f, o->Light);
				Vector(0.f, 0.f, 0.f, o->Angle);
				o->LightEnable = true;
				o->AlphaEnable = true;
				o->SubType = 0;
				o->HiddenMesh = -1;
				o->LifeTime = rand() % 128;
				o->Scale = (float)(rand() % 4 + 4) * 0.1f;
				o->Gravity = 13;
				o->bBillBoard = true;
				switch (World)
				{
				case WD_0LORENCIA:
					o->Type = MODEL_FISH01;
					o->AlphaTarget = (float)(rand() % 2 + 2) * 0.1f;
					o->Velocity = 0.6f / o->Scale;
					break;
				case WD_1DUNGEON:
					o->Type = MODEL_RAT01;
					o->Velocity = 0.6f / o->Scale;
					break;
				case WD_6STADIUM:
					o->Type = MODEL_BUG01;
					o->Velocity = 0.6f / o->Scale;
					break;
				case WD_33AIDA:
				case WD_8TARKAN:
					o->Type = MODEL_BUG01 + 1;
					o->Scale = (float)(rand() % 4 + 8) * 0.1f;
					o->Velocity = 2.5f / o->Scale;
					o->Gravity = 9;
					o->LifeTime = 100;
					VectorCopy(o->Position, o->EyeLeft);
					CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 4, o, 30.f);
					break;
				case WD_34CRYWOLF_1ST:
					if (Hero->SafeZone != true)
					{
						o->Type = MODEL_SCOLPION;
						o->Scale = (float)(rand() % 4 + 8) * 0.1f;
						o->Velocity = (((rand() % 8 + 1)) * 0.1f) / o->Scale;
						VectorCopy(o->Position, o->EyeLeft);
						o->Gravity = 1;
						o->LifeTime = 100;
						CreateJoint(BITMAP_SCOLPION_TAIL, o->Position, o->Position, o->Angle, 0, o, 30.f);
					}
					else
						o->Live = false;
					break;
				case WD_7ATLANSE:
				case WD_67DOPPLEGANGER3:
					o->Scale = (float)(rand() % 2 + 8) * 0.1f;
					if (Hero->Object.Position[1] * 0.01f < 128)
					{
						o->Type = MODEL_FISH01 + 1 + 2 + rand() % 4;
						o->Velocity = 1.f / o->Scale;
						if (rand() % 2 == 0)
							o->Gravity = 2;
						else
							o->Gravity = 3;
					}
					else
					{
						o->Type = MODEL_FISH01 + 1 + 6 + rand() % 2;
						if (o->Type == MODEL_FISH01 + 7 || o->Type == MODEL_FISH01 + 8)
						{
							o->BlendMesh = 0;
							o->BlendMeshLight = 1.f;
						}
						o->Velocity = 0.5f / o->Scale;
						if (rand() % 2 == 0)
							o->Gravity = 1;
						else
							o->Gravity = 2;
					}
					o->Timer = (float)(rand() % 32) * 0.1f;
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + (float)(rand() % 150 + 50);
					break;
				}

				if (gMapManager->InHellas())
				{
					o->Type = -1;
					o->Scale = (float)(rand() % 4 + 8) * 0.1f;
					o->Velocity = 2.5f / o->Scale;
					o->Gravity = 9;
					o->LifeTime = 70;
					CreateJoint(BITMAP_FLARE + 1, o->Position, o->Position, o->Angle, 8, o, 50.f);
				}
			}
		}

		if (o->Live)
		{
			if (o->Type != -1)
			{
				BMD* b = gmClientModels->GetModel(o->Type);
				if (b)
				{
					float myPlaySpeed = (o->Velocity * 0.5f);
					b->CurrentAction = o->CurrentAction;
					b->PlayAnimation(&o->AnimationFrame, &o->PriorAnimationFrame, &o->PriorAction, myPlaySpeed, o->Position, o->Angle);
				}
			}
			if (o->Type == MODEL_FISH01 + 7 || o->Type == MODEL_FISH01 + 8)
			{
				o->BlendMeshLight = sinf(o->Timer) * 0.4f + 0.5f;
				o->Timer += timefac(0.1f);
			}

			if ((o->Type >= MODEL_FISH01 && o->Type <= MODEL_FISH01 + 10) || o->LifeTime > 0)
			{
				if (o->Type == MODEL_BUG01 || o->Type == MODEL_SCOLPION)
					SetAction(o, 1);
				else
					SetAction(o, 0);

				MoveBoid(o, i, Fishs, MAX_FISHS);
				AngleMatrix(o->Angle, o->Matrix);
				vec3_t Position, Direction;
				Vector(o->Velocity * (float)(rand() % 4 + 6), 0.f, 0.f, Position);
				VectorRotate(Position, o->Matrix, Direction);
				interpolatesum2(o->Position, Direction, o->Position);

				if (World != 7 || gMapManager->InHellas() == false || World != WD_67DOPPLEGANGER3)
				{
					o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
				}
				o->Direction[0] = o->Position[0] + 3.f * Direction[0];
				o->Direction[1] = o->Position[1] + 3.f * Direction[1];
				int Index = TERRAIN_INDEX_REPEAT((int)(o->Position[0] / TERRAIN_SCALE), (int)(o->Position[1] / TERRAIN_SCALE));


				if (World == WD_8TARKAN || gMapManager->InHellas() || M33Aida::IsInAida())
				{
					if (TerrainWall[Index] == 1 || TerrainWall[Index] >= TW_NOGROUND)
					{
						o->Angle[2] += (180.f);
						if (o->Angle[2] >= 360.f)
							o->Angle[2] -= 360.f;
						o->SubType++;
					}
					else
					{
						if (o->SubType > 0)
							o->SubType--;
					}

					if (gMapManager->InHellas() && o->LifeTime <= 1)
					{
						o->Live = false;
					}
				}
				else if ((o->Type == MODEL_FISH01 && TerrainMappingLayer1[Index] != 5) ||
					(o->Type == MODEL_RAT01 && TerrainWall[Index] >= TW_NOGROUND) ||
					((World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3) && (TerrainWall[Index] == 1 || TerrainWall[Index] >= TW_NOGROUND)))
				{
					o->Angle[2] += (180.f);
					if (o->Angle[2] >= 360.f)
						o->Angle[2] -= 360.f;
					o->SubType++;
				}
				else
				{
					if (o->SubType > 0)
						o->SubType--;
				}
				if (o->SubType >= 2)
					o->Live = false;

				if (o->Type == MODEL_BUG01 + 1
					|| o->Type == MODEL_SCOLPION
					)
				{
					VectorCopy(o->Position, o->EyeLeft);
				}

				float dx = o->Position[0] - Hero->Object.Position[0];
				float dy = o->Position[1] - Hero->Object.Position[1];
				float Range = sqrtf(dx * dx + dy * dy);
				if (Range >= 1500.f)
					o->Live = false;

				if (Range < 600.f)
					if (o->Type == MODEL_RAT01 && rand() % 256 == 0)
						PlayBuffer(SOUND_RAT01, o);
			}
			else
			{
				SetAction(o, 0);
			}

			o->LifeTime-= timefac(1.f);

			if (o->LifeTime <= 0)
			{
				if (o->Type != MODEL_BUG01)
				{
					if (rand() % 64 == 0)
						o->LifeTime = rand() % 128;
				}
			}
			Alpha(o);
		}
	}
}

void CGoboidManager::CreateBug(int Type, vec3_t Position, OBJECT* Owner, int SubType, int targetIndex)
{
	if (NULL == Owner)
		return;

	if (targetIndex >= 0 && targetIndex < 3)
	{
		if (gMapManager->InChaosCastle() == true)
			return;

		if (Owner->Type != MODEL_PLAYER && Type != MODEL_HELPER)
			return;

		for (int i = 0; i < MAX_BUTTERFLES; i++)
		{
			OBJECT* pObj = &petList[targetIndex][i];
			if (CreateBugSub(Type, Position, Owner, pObj, SubType, targetIndex) == FALSE)
				return;
		}
	}
}

void CGoboidManager::DeleteBug(OBJECT* Owner, int targetIndex)
{
	if (NULL == Owner)
		return;

	if (targetIndex >= 0 && targetIndex < 3)
	{
		for (int i = 0; i < MAX_BUTTERFLES; i++)
		{
			OBJECT* o = &petList[targetIndex][i];

			if (o->Live)
			{
				if (o->Owner == Owner)
				{
					o->Live = false;
					o->movementType = Movement::None;

					if (targetIndex == 0)
					{
						Owner->movementType = Movement::None;
					}
				}
			}
		}
	}
	else if (targetIndex == -1)
	{
		for (targetIndex = 0; targetIndex < 3; targetIndex++)
		{
			for (int i = 0; i < MAX_BUTTERFLES; i++)
			{
				OBJECT* o = &petList[targetIndex][i];

				if (o->Live)
				{
					if (o->Owner == Owner)
					{
						o->Live = false;
						o->movementType = Movement::None;
						Owner->movementType = Movement::None;
					}
				}
			}
		}
	}
}

void CGoboidManager::MoveBugs()
{
	for (int LinkBone = 0; LinkBone < 3; LinkBone++)
	{
		for (int i = 0; i < MAX_BUTTERFLES; i++)
		{
			OBJECT* o = &petList[LinkBone][i];

			if (MoveBug(o) == FALSE)
			{
				break;
			}
		}
	}
}

void CGoboidManager::RenderBugs()
{
	for (int LinkBone = 0; LinkBone < 3; LinkBone++)
	{
		for (int i = 0; i < MAX_BUTTERFLES; i++)
		{
			OBJECT* o = &petList[LinkBone][i];

			if (RenderBug(o) == FALSE)
			{
				break;
			}
		}
	}
}

CGoboidManager* CGoboidManager::Instance()
{
	static CGoboidManager sInstance;
	return &sInstance;
}
