///////////////////////////////////////////////////////////////////////////////
// 3D 특수효과 관련 함수
//
// *** 함수 레벨: 3
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAi.h"
#include "ZzzEffect.h"
#include "DSPlaySound.h"
#include "WSClient.h"
#include "CSChaosCastle.h"
#include "GMBattleCastle.h"
#include "GMHuntingGround.h"
#include "GMAida.h"
#include "GMCrywolf1st.h"
#include "GM3rdChangeUp.h"
#include "MapManager.h"
#include "w_MapHeaders.h"
#include "NewUISystem.h"


int RainTarget = 0;
int RainCurrent = 0;

static  int RainSpeed = 30;
static  int RainAngle = 0;
static  int RainPosition = 0;

void CreateBonfire(vec3_t Position, vec3_t Angle)
{
	Position[0] += timefac((float)(rand() % 16 - 8));
	Position[1] += timefac((float)(rand() % 16 - 8));
	Position[2] += timefac((float)(rand() % 16 - 8));
	vec3_t Light;
	Vector(1.f, 1.f, 1.f, Light);
	CreateParticleSync(BITMAP_FIRE, Position, Angle, Light, rand() % 4);

	if (rand() % 4 == 0)
	{
		CreateParticleSync(BITMAP_SPARK, Position, Angle, Light);
		vec3_t a;
		Vector(-(float)(rand() % 60 + 30), 0.f, (float)(rand() % 360), a);
		CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, a);
	}
	float Luminosity = (float)(rand() % 6 + 6) * 0.1f;
	Vector(Luminosity, Luminosity * 0.6f, Luminosity * 0.4f, Light);
	AddTerrainLight(Position[0], Position[1], Light, 4, PrimaryTerrainLight);
}

void CreateFire(int Type, OBJECT* o, float x, float y, float z)
{
	vec3_t Light;
	vec3_t p, Position;
	float Luminosity;
	float Matrix[3][4];

	AngleMatrix(o->Angle, Matrix);
	Vector(x, y, z, p);
	VectorRotate(p, Matrix, Position);
	VectorAdd(o->Position, Position, Position);

	Position[0] += timefac((float)(rand() % 16 - 8));
	Position[1] += timefac((float)(rand() % 16 - 8));
	Position[2] += timefac((float)(rand() % 16 - 8));

	switch (Type)
	{
	case 0:
		Luminosity = (float)(rand() % 6 + 6) * 0.1f;
		Vector(Luminosity, Luminosity * 0.6f, Luminosity * 0.4f, Light);
		if (rand() % 2 == 0)
			CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light, rand() % 4);
		AddTerrainLight(Position[0], Position[1], Light, 4, PrimaryTerrainLight);
		break;
	case 1:
		if (rand() % 2 == 0)
			CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, o->Light);
		break;
	case 2:
		if (rand() % 2 == 0)
			CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, o->Light, 2);
		break;
	}
}

void CheckSkull(OBJECT* o)
{
	vec3_t Position;
	VectorCopy(Hero->Object.Position, Position);
	if (Hero->Object.CurrentAction >= PLAYER_WALK_MALE && Hero->Object.CurrentAction <= PLAYER_RUN_RIDE_WEAPON
		|| (Hero->Object.CurrentAction == PLAYER_RAGE_UNI_RUN || Hero->Object.CurrentAction == PLAYER_RAGE_UNI_RUN_ONE_RIGHT))
	{
		if (o->Direction[0] < 0.1f)
		{
			float dx = Position[0] - o->Position[0];
			float dy = Position[1] - o->Position[1];
			float Distance = sqrtf(dx * dx + dy * dy);
			if (Distance < 50.f)
			{
				Vector(-dx * 0.4f, -dy * 0.4f, 0.f, o->Direction);
				o->HeadAngle[1] = -dx * 4.f;
				o->HeadAngle[0] = -dy * 4.f;
				PlayBuffer(SOUND_BONE2, o);
			}
		}
	}
	VectorScale(o->Direction, timepow(0.6f), o->Direction);
	VectorScale(o->HeadAngle, timepow(0.6f), o->HeadAngle);
	interpolatesum(o->Position, o->Direction);
	interpolatesum(o->Angle, o->HeadAngle);
}

bool CreateDevilSquareRain(PARTICLE* o, int Index)
{
	if (gMapManager->InDevilSquare() == false
		&& gMapManager->currentMap != WD_34CRYWOLF_1ST
		)
	{
		return false;
	}

	if (gMapManager->currentMap == WD_34CRYWOLF_1ST && weather != 1)
		return false;

	o->Type = BITMAP_RAIN_01;
	if (Index < 300)
	{
		Vector(
			Hero->Object.Position[0] + (float)(rand() % 1600 - 800),
			Hero->Object.Position[1] + (float)(rand() % 1400 - 500),
			Hero->Object.Position[2] + (float)(rand() % 200 + 300),
			o->Position);
	}
	else
	{
		Vector(
			Hero->Object.Position[0] + (float)(rand() % 1600 - 800),
			Hero->Object.Position[1] + (float)(rand() % 300 + 1000) - RainPosition,
			Hero->Object.Position[2] + (float)(rand() % 200 + 300),
			o->Position);
	}
	if (rand() % 2 == 0)
	{
		Vector((float)(-(rand() % 20 + 20)), 0.f, 0.f, o->Angle);
	}
	else
	{
		Vector((float)(-(rand() % 20 + 30 + RainAngle)), 0.f, 0.f, o->Angle);
	}
	vec3_t Velocity;
	float Matrix[3][4];

	Vector(0.f, 0.f, -(float)(rand() % 40 + RainSpeed), Velocity);

	AngleMatrix(o->Angle, Matrix);
	VectorRotate(Velocity, Matrix, o->Velocity);

	return true;
}

bool CreateChaosCastleRain(PARTICLE* o, int Index)
{
	if (gMapManager->InChaosCastle() == false)
		return false;

	o->Type = BITMAP_RAIN_01;
	o->TurningForce[0] = 1.f;
	o->TurningForce[1] = 30.f + rand() % 10;
	if (Index < 300)
	{
		Vector(
			Hero->Object.Position[0] + (float)(rand() % 1600 - 800),
			Hero->Object.Position[1] + (float)(rand() % 1400 - 500),
			Hero->Object.Position[2] + (float)(rand() % 200 + 300),
			o->Position);
	}
	else
	{
		Vector(
			Hero->Object.Position[0] + (float)(rand() % 1600 - 800),
			Hero->Object.Position[1] + (float)(rand() % 300 + 1000) - RainPosition,
			Hero->Object.Position[2] + (float)(rand() % 200 + 300),
			o->Position);
	}
	if (rand() % 2 == 0)
	{
		Vector((float)(-(rand() % 20 + 20)), 0.f, 0.f, o->Angle);
	}
	else
	{
		Vector((float)(-(rand() % 20 + 30 + RainAngle)), 0.f, 0.f, o->Angle);
	}
	vec3_t Velocity;
	Vector(0.f, 0.f, -(float)(rand() % 40 + RainSpeed + 20), Velocity);
	float Matrix[3][4];
	AngleMatrix(o->Angle, Matrix);
	VectorRotate(Velocity, Matrix, o->Velocity);

	return true;
}


bool CreateLorenciaLeaf(PARTICLE* o)
{
	if (gMapManager->currentMap != WD_0LORENCIA)
		return false;

	o->Type = BITMAP_LEAF1;
	vec3_t Position;
	Vector(Hero->Object.Position[0] + (float)(rand() % 1600 - 800), Hero->Object.Position[1] + (float)(rand() % 1400 - 500), Hero->Object.Position[2] + (float)(rand() % 300 + 50), Position);
	VectorCopy(Position, o->Position);
	VectorCopy(Position, o->StartPosition);
	o->Velocity[0] = -(float)(rand() % 64 + 64) * 0.1f;

	if (Position[1] < CameraPosition[1] + 400.f)
		o->Velocity[0] = -o->Velocity[0] + 3.2f;

	o->Velocity[1] = (float)(rand() % 32 - 16) * 0.1f;
	o->Velocity[2] = (float)(rand() % 32 - 16) * 0.1f;
	o->TurningForce[0] = (float)(rand() % 16 - 8) * 0.1f;
	o->TurningForce[1] = (float)(rand() % 64 - 32) * 0.1f;
	o->TurningForce[2] = (float)(rand() % 16 - 8) * 0.1f;

	return true;
}

bool CreateHeavenRain(PARTICLE* o, int index)
{
	if (gMapManager->currentMap != WD_10HEAVEN)
		return false;

	int Rainly = RainCurrent * MAX_LEAVES / 100;
	if (index < Rainly)
	{
		o->Type = BITMAP_RAIN_01;
		Vector(Hero->Object.Position[0] + (float)(rand() % 1600 - 800), Hero->Object.Position[1] + (float)(rand() % 1400 - 500),
			Hero->Object.Position[2] + (float)(rand() % 200 + 200),
			o->Position);
		Vector(-30.f, 0.f, 0.f, o->Angle);
		vec3_t Velocity;
		Vector(0.f, 0.f, -(float)(rand() % 24 + 20), Velocity);
		float Matrix[3][4];
		AngleMatrix(o->Angle, Matrix);
		VectorRotate(Velocity, Matrix, o->Velocity);
	}
	return true;
}

bool CreateDeviasSnow(PARTICLE* o)
{
	vec3_t Velocity;
	float Matrix[3][4];

	if (gMapManager->currentMap != WD_2DEVIAS)
		return false;

	o->Type = BITMAP_LEAF1;
	o->Scale = 5.f;
	if (rand() % 10 == 0)
	{
		o->Type = BITMAP_LEAF2;
		o->Scale = 10.f;
	}
	Vector(Hero->Object.Position[0] + (float)(rand() % 1600 - 800), Hero->Object.Position[1] + (float)(rand() % 1400 - 500),
		Hero->Object.Position[2] + (float)(rand() % 200 + 200),
		o->Position);
	Vector(-30.f, 0.f, 0.f, o->Angle);

	Vector(0.f, 0.f, -(float)(rand() % 16 + 8), Velocity);
	AngleMatrix(o->Angle, Matrix);
	VectorRotate(Velocity, Matrix, o->Velocity);

	return true;
}

bool CreateAtlanseLeaf(PARTICLE* o)
{
	if (gMapManager->currentMap != WD_3NORIA && gMapManager->currentMap != WD_7ATLANSE)
		return false;

	o->Type = BITMAP_LEAF1;
	vec3_t Position;
	Vector(Hero->Object.Position[0] + (float)(rand() % 1600 - 800), Hero->Object.Position[1] + (float)(rand() % 1400 - 500),
		Hero->Object.Position[2] + (float)(rand() % 300 + 50),
		Position);
	VectorCopy(Position, o->Position);
	VectorCopy(Position, o->StartPosition);
	o->Velocity[0] = -(float)(rand() % 64 + 64) * 0.1f;
	if (Position[1] < CameraPosition[1] + 400.f) o->Velocity[0] = -o->Velocity[0] + 3.2f;
	o->Velocity[1] = (float)(rand() % 32 - 16) * 0.1f;
	o->Velocity[2] = (float)(rand() % 32 - 16) * 0.1f;
	o->TurningForce[0] = (float)(rand() % 16 - 8) * 0.1f;
	o->TurningForce[1] = (float)(rand() % 64 - 32) * 0.1f;
	o->TurningForce[2] = (float)(rand() % 16 - 8) * 0.1f;

	return true;
}

bool MoveDevilSquareRain(PARTICLE* o)
{
	if (gMapManager->InDevilSquare() == false && gMapManager->currentMap != WD_34CRYWOLF_1ST)
		return false;

	if (gMapManager->currentMap == WD_34CRYWOLF_1ST && weather != 1)
		return false;

	interpolatesum2(o->Position, o->Velocity, o->Position);
	float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);

	if (o->Position[2] < Height)
	{
		o->Live = false;
		o->Position[2] = Height + 10.f;
		if (rand() % 4 == 0)
			CreateParticleSync(BITMAP_RAIN_02, o->Position, o->Angle, o->Light);
		else
			CreateParticleSync(BITMAP_RAIN_03, o->Position, o->Angle, o->Light);
	}
	return true;
}

bool MoveChaosCastleRain(PARTICLE* o)
{
	if (gMapManager->InChaosCastle() == false)
		return false;

	interpolatesum2(o->Position, o->Velocity, o->Position);
	float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);

	if (o->Position[2] < Height && (TERRAIN_ATTRIBUTE(o->Position[0], o->Position[1]) & TW_NOGROUND) != TW_NOGROUND)
	{
		o->Live = false;
		o->Position[2] = Height + 10.f;
		if (rand() % 4 == 0)
			CreateParticleSync(BITMAP_RAIN_02, o->Position, o->Angle, o->Light);
		else
			CreateParticleSync(BITMAP_RAIN_03, o->Position, o->Angle, o->Light);
	}
	return true;
}

bool MoveHeavenRain(PARTICLE* o)
{
	if (gMapManager->currentMap != WD_0LORENCIA && gMapManager->currentMap != WD_10HEAVEN)
		return false;

	if (o->Type == BITMAP_RAIN_01)
	{
		interpolatesum2(o->Position, o->Velocity, o->Position);
		float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);

		if (o->Position[2] < Height)
		{
			o->Live = false;
			o->Position[2] = Height + 10.f;
			if (gMapManager->currentMap != WD_10HEAVEN)
				CreateParticleSync(BITMAP_RAIN_02, o->Position, o->Angle, o->Light);
		}
	}
	else
	{

		o->Velocity[0] += timefac((float)(rand() % 16 - 8) * 0.1f);
		o->Velocity[1] += timefac((float)(rand() % 16 - 8) * 0.1f);
		o->Velocity[2] += timefac((float)(rand() % 16 - 8) * 0.1f);

		interpolatesum2(o->Position, o->Velocity, o->Position);

		o->TurningForce[0] += timefac(((float)(rand() % 8 - 4) * 0.02f));
		o->TurningForce[1] += timefac(((float)(rand() % 16 - 8) * 0.02f));
		o->TurningForce[2] += timefac(((float)(rand() % 8 - 4) * 0.02f));

		interpolatesum2(o->Angle, o->TurningForce, o->Angle);

		vec3_t Range;
		VectorSubtract(o->StartPosition, o->Position, Range);
		float Length = Range[0] * Range[0] + Range[1] * Range[1] + Range[2] * Range[2];

		if (Length >= 200000.f)
			o->Live = false;
	}

	return true;
}

void MoveEtcLeaf(PARTICLE* o)
{
	float Height = RequestTerrainHeight(o->Position[0], o->Position[1]);
	if (o->Position[2] <= Height)
	{
		o->Position[2] = Height;
		o->Light[0] -= timefac(0.05f);
		o->Light[1] -= timefac(0.05f);
		o->Light[2] -= timefac(0.05f);
		if (o->Light[0] <= 0.f)
			o->Live = false;
	}
	else
	{
		o->Velocity[0] += timefac((float)(rand() % 16 - 8) * 0.1f);
		o->Velocity[1] += timefac((float)(rand() % 16 - 8) * 0.1f);
		o->Velocity[2] += timefac((float)(rand() % 16 - 8) * 0.1f);
		interpolatesum(o->Position, o->Velocity);
	}
}

bool MoveLeaves()
{
	int iMaxLeaves = (gMapManager->InDevilSquare() == true) ? MAX_LEAVES : 80;

	if (gMapManager->currentMap == WD_10HEAVEN)
	{
		RainTarget = MAX_LEAVES / 2;
	}
	else if (gMapManager->InChaosCastle() == true)
	{
		RainTarget = MAX_LEAVES / 2;
		iMaxLeaves = 80;
	}
	else if (gMapManager->InBattleCastle())
	{
		iMaxLeaves = 40;
	}
	else if (gMapManager->currentMap == WD_55LOGINSCENE)
	{
		iMaxLeaves = 80;
	}
	else if (M34CryWolf1st::IsCyrWolf1st())
	{
		if (weather == 1)
			iMaxLeaves = 60;
		else
			if (weather == 2)
				iMaxLeaves = 50;
	}
	if (checkNormalizer)
	{
		if (RainCurrent > RainTarget)
			RainCurrent--;
		else if (RainCurrent < RainTarget)
			RainCurrent++;
	}

	RainSpeed = (int)sinf(WorldTime * 0.001f) * 10 + 30;
	RainAngle = (int)sinf(WorldTime * 0.0005f + 50.f) * 20;
	if (checkNormalizer)
	{
		RainPosition += 20;
		RainPosition %= 2000;
	}

	for (int i = 0; i < iMaxLeaves; i++)
	{
		PARTICLE* o = &Leaves[i];
		if (!o->Live)
		{
			Vector(1.f, 1.f, 1.f, o->Light);
			o->Live = true;

			if (CreateDevilSquareRain(o, i))
				continue;
			if (CreateChaosCastleRain(o, i))
				continue;
			if (CreateLorenciaLeaf(o))
				continue;
			if (CreateHeavenRain(o, i))
				continue;
			if (CreateDeviasSnow(o))
				continue;
			if (CreateAtlanseLeaf(o))
				continue;
			if (battleCastle::CreateFireSnuff(o))
				continue;
			if (M31HuntingGround::CreateMist(o))
				continue;
			if (M33Aida::CreateMist(o))
				continue;
			if (weather == 2 && M34CryWolf1st::CreateMist(o))
				continue;
			if (SEASON3A::CGM3rdChangeUp::Instance().CreateFireSnuff(o))
				continue;
			if (g_Raklion.CreateSnow(o))
				continue;
			if (g_SantaTown.CreateSnow(o))
				continue;
			if (g_PKField.CreateFireSpark(o))
				continue;
			if (g_DoppelGanger2.CreateFireSpark(o))
				continue;
			if (g_EmpireGuardian1.CreateRain(o))
				continue;
			if (g_EmpireGuardian2.CreateRain(o))
				continue;
			if (g_EmpireGuardian3.CreateRain(o))
				continue;
			if (g_UnitedMarketPlace.CreateRain(o))
				continue;
		}
		else
		{
			if (MoveDevilSquareRain(o))
				continue;
			if (MoveChaosCastleRain(o))
				continue;
			if (MoveHeavenRain(o))
				continue;
			if (g_UnitedMarketPlace.MoveRain(o))
				continue;
			MoveEtcLeaf(o);
		}
	}
	return true;
}


void RenderLeaves()
{
	if (g_pOption->GetRenderEffect() == false)
	{
		return;
	}

	if (World == WD_2DEVIAS
		|| World == WD_7ATLANSE
		|| World == WD_10HEAVEN
		|| IsIceCity()
		|| IsSantaTown()
		|| gMapManager->IsPKField()
		|| IsDoppelGanger2()
		|| gMapManager->IsEmpireGuardian1()
		|| gMapManager->IsEmpireGuardian2()
		|| gMapManager->IsEmpireGuardian3()
		|| IsUnitedMarketPlace()
		|| World == WD_95DEBENTER1
		|| World == WD_96DEBENTER2
		|| World == WD_100URUK_MOUNTAIN
		|| World == WD_101URUK_MOUNTAIN
		|| World == WD_110NARS1
		|| World == WD_111NARS2
		|| World == WD_113NIXIESLAKE
		|| World == WD_114INSTANCE_LABYRINTH
		|| World == WD_121PLACE_OF_QUALITY
		|| World == WD_122SWAMP_OF_DARKNESS
		|| IsKuberaMine()
		|| IsAtlansAbbyss()
		|| IsScorchedCanyon()
		|| IsIcarusRed()
		|| IsTempleOfArnil()
		|| IsBlazeKethotum()
		|| IsKanturuUnderGround()
		//|| sub_D0D8EC(iWorld) 138
		|| !gMapManager->InChaosCastle(World) && (gMapManager->InBattleCastle(World) || World == WD_55LOGINSCENE || World == WD_42CHANGEUP3RD_2ND))
	{
		EnableAlphaBlend();
	}
	else
	{
		EnableAlphaTest(true);
	}

	glColor3f(1.f, 1.f, 1.f);

	int iMaxLeaves = MAX_LEAVES;
	int index = TERRAIN_INDEX_REPEAT(Hero->PositionX, Hero->PositionY);

	switch (World)
	{
	case WD_100URUK_MOUNTAIN:
	case WD_101URUK_MOUNTAIN:
		if ((TerrainWall[index] & 0x800) != 0)
			iMaxLeaves = 80;
		else if ((TerrainWall[index] & 0x1000) != 0)
			iMaxLeaves = 100;
		else
			iMaxLeaves = 0;
		break;
	case WD_110NARS1:
	case WD_111NARS2:
		if ((TerrainWall[index] & 0x800) == 0)
			iMaxLeaves = 0;
		break;
	case WD_122SWAMP_OF_DARKNESS:
		if ((TerrainWall[index] & 0x800) != 0)
			iMaxLeaves = 80;
		else
			iMaxLeaves = 100;
		break;
	case WD_133TEMPLE_OF_ARNIL:
		iMaxLeaves = 80;
		break;
	}
	glColor3f(1.000000, 1.000000, 1.000000);

	for (int i = 0; i < iMaxLeaves; i++)

	{
		PARTICLE* o = &Leaves[i];
		if (o->Live && Bitmaps.FindTexture(o->Type))
		{
			BindTexture(o->Type);
			if (gMapManager->currentMap == WD_2DEVIAS || IsIceCity() || IsSantaTown())
			{
				RenderSprite(o->Type, o->Position, o->Scale, o->Scale, o->Light);
			}
			else
			{
				glPushMatrix();
				glTranslatef(o->Position[0], o->Position[1], o->Position[2]);
				float Matrix[3][4];
				AngleMatrix(o->Angle, Matrix);

				if (gMapManager->InChaosCastle() == true)
					RenderPlane3D(o->TurningForce[0], o->TurningForce[1], Matrix);
				else
				{
					if (o->Type == BITMAP_RAIN_01)
					{
						if (gMapManager->currentMap == WD_34CRYWOLF_1ST)
						{
							if (weather == 1)
								RenderPlane3D(1.f, 20.f, Matrix);
						}
						else
							RenderPlane3D(1.f, 20.f, Matrix);
					}
					else if (o->Type == BITMAP_FIRE_SNUFF)
						RenderPlane3D(o->Scale * 2.f, o->Scale * 4.f, Matrix);
					else
					{
						RenderPlane3D(3.f, 3.f, Matrix);
					}
				}
				glPopMatrix();
			}
		}
	}
}

