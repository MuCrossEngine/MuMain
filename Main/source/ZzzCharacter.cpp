///////////////////////////////////////////////////////////////////////////////
// 케릭터 관련 함수
// 케릭터 랜더링, 움직임등을 처리
//
// *** 함수 레벨: 3
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "_enum.h"
#ifndef __ANDROID__
#include <eh.h>
#endif
#include "UIManager.h"
#include "GuildCache.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAI.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "ZzzEffect.h"
#include "ZzzOpenData.h"
#include "ZzzScene.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "PhysicsManager.h"
#include "GOBoid.h"
#include "CSItemOption.h"
#include "CSChaosCastle.h"
#include "GIPetManager.h"
#include "GMHellas.h"
#include "CSParts.h"
#include "GMBattleCastle.h"
#include "GMHuntingGround.h"
#include "GMCryingWolf2nd.h"
#include "BoneManager.h"
#include "GMAida.h"
#include "GMCryWolf1st.h"
#include "GM_Kanturu_1st.h"
#include "GM_Kanturu_2nd.h"
#include "CDirection.h"
#include "GM_Kanturu_3rd.h"
#include "GM3rdChangeUp.h"
#include "Input.h"
#include "ChangeRingManager.h"
#include "Event.h"
#include "PartyManager.h"
#include "GMNewTown.h"
#include "w_CursedTemple.h"
#include "SummonSystem.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "GMSwampOfQuiet.h"
#include "w_MapHeaders.h"
#include "w_PetProcess.h"
#include "DuelMgr.h"
#include "CGMProtect.h"
#include "MonkSystem.h"
#include "CGMMonsterMng.h"
#include "CGMItemEffect.h"
#include "CGMItemWings.h"
#include "CGMHeadChat.h"
#include "CGMPhysicsManager.h"

CHARACTER* Hero;
CHARACTER CharacterView;
Script_Skill MonsterSkill[MODEL_MONSTER_END];
extern CKanturuDirection KanturuDirection;
float g_fBoneSave[10][3][4];
vec3_t BossHeadPosition;
extern int HeroIndex;

static char vec_list[35] =
{
	 5,  6, 33, 53, 35, 49, 50, 45, 46, 41,
	42, 37, 38, 11, 31, 13, 27, 28, 23, 24,
	19, 20, 15, 16, 54, 55, 62, 69, 70, 77,
	 2, 79, 81, 84, 86
};

static char wingLeft[15][2] =
{
	{0,2},  {2,3}, {2,4},  {4,5}, {5,6}, {4,7}, {7,8}, {4,9}, {9,10}, {4,11}, {11,12},
	{6,5}, {8,7}, {10,9},{12,11}
};

static char wingRight[15][2] =
{
	{0,13}, {13,14}, {13,15}, {15,16}, {16,17}, {15,18}, {18,19}, {15,20}, {20,21}, {15,22}, {22,23},
	{17,16},{19,18}, {21,20}, {23,22}
};

static char arm_leg_Left[4][2] =
{
	{29,28}, {28,27}, {34,33}, {33,30},
};

static  char    arm_leg_Right[4][2] =
{
	{26,25}, {25,24}, {32,31}, {31,30},
};

static  char    g_chStar[10] =
{
	10, 18, 37, 38, 51, 52, 58, 59, 66, 24
};

int  EquipmentLevelSet = 0;
bool g_bAddDefense = false;
int g_iLimitAttackTime = 15;
extern float  ParentMatrix[3][4];
extern int CurrentSkill;


static float   g_fStopTime = 0.f;
static int     g_iOldPositionX = 0;
static int     g_iOldPositionY = 0;

BOOL PlayMonsterSoundGlobal(OBJECT* pObject);
void UnRegisterBuff(eBuffState buff, OBJECT* o);
void RegisterBuff(eBuffState buff, OBJECT* o, const int bufftime = 0);

int GetFenrirType(CHARACTER* c)
{
	if (c->Helper.Option1 == 0x01)
		return FENRIR_TYPE_BLACK;
	else if (c->Helper.Option1 == 0x02)
		return FENRIR_TYPE_BLUE;
	else if (c->Helper.Option1 == 0x04)
		return FENRIR_TYPE_GOLD;

	return FENRIR_TYPE_RED;
}

void FallingMonster(CHARACTER* c, OBJECT* o)
{
	float AngleY = 0.0;
	o->Gravity += timefac(2.5f);
	o->Angle[0] -= timefac(4.f);
	o->m_bActionStart = true;
	o->Direction[1] += timefac(o->Direction[0]);

	if (o->Gravity >= 1.f)
		AngleY = o->Angle[2];

	if (o->Angle[0] <= -90.0f)
		o->Angle[0] = -90.0f;

	o->Angle[2] = AngleY;

	if (o->Gravity >= 150.0f)
		o->Gravity = 150.0f;
	o->Position[2] -= timefac(o->Gravity);
}

BOOL PlayMonsterSound(OBJECT* pObject)
{
	if (PlayMonsterSoundGlobal(pObject))
	{
		return TRUE;
	}
	if (SEASON3B::GMNewTown::IsCurrentMap())
	{
		SEASON3B::GMNewTown::PlayMonsterSound(pObject);
		return TRUE;
	}
	if (SEASON3C::GMSwampOfQuiet::IsCurrentMap())
	{
		SEASON3C::GMSwampOfQuiet::PlayMonsterSound(pObject);
		return TRUE;
	}
	if (TheMapProcess().PlayMonsterSound(pObject) == true)
	{
		return TRUE;
	}
	return FALSE;
}

void SetPlayerStop(CHARACTER* c)
{
	OBJECT* o = &c->Object;

	c->Run = 0;

	bool movement_normal_hide = true;

	if (o->animation_hide_traditional())
	{
		movement_normal_hide = false;
	}

	if (o->Type == MODEL_PLAYER)
	{
		if (o->movementType == Movement::Fenrir && !c->SafeZone && movement_normal_hide)
		{
			if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
			{
				if (c->Weapon[0].Type == -1 || c->Weapon[1].Type == -1)
				{
					if (c->Weapon[0].Type == -1 || c->Weapon[1].Type != -1)
					{
						if (c->Weapon[0].Type != -1 || c->Weapon[1].Type == -1)
							SetAction(o, PLAYER_RAGE_FENRIR_STAND, 1);
						else
							SetAction(o, PLAYER_RAGE_FENRIR_STAND_ONE_LEFT, 1);
					}
					else
					{
						SetAction(o, PLAYER_RAGE_FENRIR_STAND_ONE_RIGHT, 1);
					}
				}
				else
				{
					SetAction(o, PLAYER_RAGE_FENRIR_STAND_TWO_SWORD, 1);
				}
			}
			else if (c->Weapon[0].Type == -1 || c->Weapon[1].Type == -1)
			{
				if (c->Weapon[0].Type == -1 || c->Weapon[1].Type != -1)
				{
					if (c->Weapon[0].Type != -1 || c->Weapon[1].Type == -1)
						SetAction(o, PLAYER_FENRIR_STAND, 1);
					else
						SetAction(o, PLAYER_FENRIR_STAND_WEAPON_LEFT, 1);
				}
				else
				{
					SetAction(o, PLAYER_FENRIR_STAND_WEAPON_RIGHT, 1);
				}
			}
			else
			{
				SetAction(o, PLAYER_FENRIR_STAND_WEAPON_TWO, 1);
			}
		}
		else if (o->movementType == Movement::Horse && !c->SafeZone && movement_normal_hide)
		{
			if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
			{
				SetAction(o, PLAYER_STOP_RIDE_HORSE, 1);
			}
			else
			{
				SetAction(o, PLAYER_STOP_RIDE_HORSE, 1);
			}
		}
		else if ((o->movementType == Movement::Uniria || o->movementType == Movement::Dinorant) && !c->SafeZone && movement_normal_hide)
		{
			if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
			{
				if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
					SetAction(o, PLAYER_RAGE_UNI_STOP_ONE_RIGHT, 1);
				else
					SetAction(o, PLAYER_STOP_RIDE_WEAPON, 1);
			}
			else
			{
				SetAction(o, PLAYER_STOP_RIDE, 1);
			}
		}
		else if (c->m_pPet != NULL && c->SafeZone && !gMapManager->InChaosCastle())
		{
			SetAction(o, PLAYER_DARKLORD_STAND, 1);
		}
		else
		{
			bool Fly = false;

			if (o->SubType != MODEL_CURSEDTEMPLE_ALLIED_PLAYER && o->SubType != MODEL_CURSEDTEMPLE_ILLUSION_PLAYER && !c->SafeZone && c->Wing.Type != -1)
			{
				Fly = true;
			}

			int Index = TERRAIN_INDEX_REPEAT((int)(o->Position[0] / TERRAIN_SCALE), (int)(o->Position[1] / TERRAIN_SCALE));

			if (SceneFlag == MAIN_SCENE && (World == WD_7ATLANSE || gMapManager->InHellas() || World == WD_67DOPPLEGANGER3) && !(TerrainWall[Index] & TW_SAFEZONE))
			{
				Fly = true;
			}
			if (c->MonsterIndex == 257)
				Fly = true;

			if (Fly)
			{
				if (g_isCharacterBuff(o, eBuff_CrywolfHeroContracted))
				{
					if (o->CurrentAction != PLAYER_HEALING_FEMALE1)
					{
						SetAction(o, PLAYER_HEALING_FEMALE1);
						SendRequestAction(AT_HEALING1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
					}
				}
				else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
				{
					SetAction(o, PLAYER_STOP_FLY_CROSSBOW);
				}
				else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
				{
					SetAction(o, PLAYER_STOP_FLY_BOW);
				}
				else
				{
					SetAction(o, PLAYER_STOP_FLY);
				}
			}
			else if (g_isCharacterBuff(o, eBuff_CrywolfHeroContracted))
			{
				if (o->CurrentAction != PLAYER_HEALING_FEMALE1)
				{
					SetAction(o, PLAYER_HEALING_FEMALE1, 1);
					SendRequestAction(AT_HEALING1, ((BYTE)((o->Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8));
				}
			}
			else if ((c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
				&& (!c->SafeZone || gMapManager->InBloodCastle()))
			{
				if (c->Weapon[0].Type < MODEL_ITEM || c->Weapon[0].Type >= MODEL_MACE + MAX_ITEM_INDEX)
				{
					if (c->Weapon[0].Type != MODEL_SPEAR + 1 && c->Weapon[0].Type != MODEL_SPEAR + 2)
					{
						if (c->Weapon[0].Type < MODEL_SPEAR || c->Weapon[0].Type >= MODEL_SPEAR + MAX_ITEM_INDEX)
						{
							if (c->Weapon[0].Type < MODEL_STAFF + 14 || c->Weapon[0].Type > MODEL_STAFF + 20)
							{
								if (c->Weapon[0].Type < MODEL_STAFF || c->Weapon[0].Type >= MODEL_STAFF + MAX_ITEM_INDEX)
								{
									if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
									{
										SetAction(o, PLAYER_STOP_BOW, 1);
									}
									else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
									{
										SetAction(o, PLAYER_STOP_CROSSBOW, 1);
									}
									else if (c->GetBaseClass() == CLASS_ELF)
									{
										SetAction(o, PLAYER_STOP_FEMALE, 1);
									}
									else if (c->GetBaseClass() == CLASS_SUMMONER)
									{
										SetAction(o, PLAYER_STOP_SUMMONER, 1);
									}
									else if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
									{
										SetAction(o, PLAYER_STOP_RAGEFIGHTER, 1);
									}
									else
									{
										SetAction(o, PLAYER_STOP_MALE, 1);
									}
								}
								else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
								{
									SetAction(o, PLAYER_STOP_SCYTHE, 1);
								}
								else
								{
									SetAction(o, PLAYER_STOP_SWORD, 1);
								}
							}
							else
							{
								SetAction(o, PLAYER_STOP_WAND, 1);
							}
						}
						else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
						{
							SetAction(o, PLAYER_STOP_SCYTHE, 1);
						}
						else
						{
							SetAction(o, PLAYER_STOP_SWORD, 1);
						}
					}
					else
					{
						SetAction(o, PLAYER_STOP_SPEAR, 1);
					}
				}
				else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
				{
					if (c->Weapon[0].Type != MODEL_SWORD + 21
						&& c->Weapon[0].Type != MODEL_SWORD + 31
						&& c->Weapon[0].Type != MODEL_SWORD + 23
						&& c->Weapon[0].Type != MODEL_SWORD + 25)
					{
						SetAction(o, PLAYER_STOP_TWO_HAND_SWORD, 1);
					}
					else
					{
						SetAction(o, PLAYER_STOP_TWO_HAND_SWORD_TWO, 1);
					}
				}
				else
				{
					SetAction(o, PLAYER_STOP_SWORD, 1);
				}
			}
			else if (c->GetBaseClass() == CLASS_ELF)
			{
				SetAction(o, PLAYER_STOP_FEMALE, 1);
			}
			else if (c->GetBaseClass() != CLASS_SUMMONER || gMapManager->InChaosCastle())
			{
				if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
					SetAction(o, PLAYER_STOP_RAGEFIGHTER, 1);
				else
					SetAction(o, PLAYER_STOP_MALE, 1);
			}
			else
			{
				SetAction(o, PLAYER_STOP_SUMMONER, 1);
			}
		}
	}
	else
	{
		int Index = TERRAIN_INDEX_REPEAT((c->PositionX), (c->PositionY));
		if (o->Type == MODEL_MONSTER01 + 32 && (TerrainWall[Index] & TW_SAFEZONE))
			SetAction(o, PLAYER_STOP_SCYTHE, 1);
		else
			SetAction(o, PLAYER_SET, 1);
	}

	if ((rand() % 16) == 0)
	{
		if (o->Type == MODEL_PLAYER && ((o->SubType < MODEL_SKELETON1) || o->SubType > MODEL_SKELETON3))
		{
			if (o->movementType == Movement::Fenrir && !c->SafeZone && movement_normal_hide)
			{
				if ((rand() % 3) == 0)
				{
					PlayBuffer(SOUND_FENRIR_IDLE_1 + rand() % 2, o);
				}
			}
		}
		else
		{
			BMD* pModels = gmClientModels->GetModel(o->Type);

			if (pModels && pModels->Sounds[0] != -1)
			{
				int offset = 0;
				if (o->SubType == 9)
					offset += 5;

				PlayBuffer(SOUND_MONSTER + offset + pModels->Sounds[rand() % 2], o);
			}
		}
	}
}

void SetPlayerWalk(CHARACTER* c)
{
	OBJECT* o = &c->Object;

	if (c->SafeZone)
		c->Run = 0;

	int bootsType = c->GetSkinModelIndex() + MODEL_BODY_BOOTS;
	int glovesType = c->GetSkinModelIndex() + MODEL_BODY_GLOVES;

	bool movement_normal_hide = true;

	if (o->animation_hide_traditional())
	{
		movement_normal_hide = false;
	}

	if (c->MonsterIndex < 534 || c->MonsterIndex > 539)
	{
		if (!c->SafeZone
			&& c->Run < 40
			&& (c->GetBaseClass() == CLASS_DARK
				|| c->GetBaseClass() == CLASS_DARK_LORD
				|| c->GetBaseClass() == CLASS_RAGEFIGHTER
				|| World != WD_7ATLANSE
				&& !gMapManager->InHellas()
				&& World != WD_67DOPPLEGANGER3
				&& c->BodyPart[BODYPART_BOOTS].Type != bootsType
				&& c->BodyPart[BODYPART_BOOTS].Level >= 5
				|| (World == WD_7ATLANSE || gMapManager->InHellas() || World == WD_67DOPPLEGANGER3)
				&& c->BodyPart[BODYPART_GLOVES].Type != glovesType
				&& c->BodyPart[BODYPART_GLOVES].Level >= 5
				|| (World != WD_7ATLANSE && !gMapManager->InHellas() && World != WD_67DOPPLEGANGER3 && (c->BodyPart[BODYPART_BOOTS].Type != bootsType
					&& c->BodyPart[BODYPART_BOOTS].Level >= 5))
				|| (World == WD_7ATLANSE || gMapManager->InHellas() || World == WD_67DOPPLEGANGER3 && (c->BodyPart[BODYPART_GLOVES].Type != glovesType
					&& c->BodyPart[BODYPART_GLOVES].Level >= 5))
				|| (o->movementType == Movement::Fenrir && movement_normal_hide)
				|| o->SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER
				|| o->SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER))
		{
			c->Run += timefac(1.f);
		}
	}
	else
	{
		c->Run = 0;
	}

	if (o->Type == MODEL_PLAYER)
	{
		for (int i = PLAYER_WALK_MALE; i <= PLAYER_WALK_CROSSBOW; i++)
		{
			Action_t* pAction = gmClientModels->GetPlayerModelAction(i);

			if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
				pAction->PlaySpeed = 0.32f;
			else
				pAction->PlaySpeed = 0.33f;

			if (g_isCharacterBuff(o, eDeBuff_Freeze))
				pAction->PlaySpeed *= timepow(0.5f);
			else if (g_isCharacterBuff(o, eDeBuff_BlowOfDestruction))
				pAction->PlaySpeed *= timepow(0.33f);
		}

		gmClientModels->GetPlayerModelAction(PLAYER_RAGE_UNI_RUN)->PlaySpeed = 0.34f;
		gmClientModels->GetPlayerModelAction(PLAYER_RAGE_UNI_RUN_ONE_RIGHT)->PlaySpeed = 0.34f;

		for (int i = PLAYER_RUN; i <= PLAYER_RUN_RIDE_WEAPON; i++)
		{
			Action_t* pAction = gmClientModels->GetPlayerModelAction(i);

			if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
				pAction->PlaySpeed = 0.28f;
			else
				pAction->PlaySpeed = 0.34f;

			if (g_isCharacterBuff(o, eDeBuff_Freeze))
				pAction->PlaySpeed *= timepow(0.5f);
			else if (g_isCharacterBuff(o, eDeBuff_BlowOfDestruction))
				pAction->PlaySpeed *= timepow(0.33f);
		}

		gmClientModels->GetPlayerModelAction(PLAYER_CHANGE_UP)->PlaySpeed = 0.049f;
		gmClientModels->GetPlayerModelAction(PLAYER_RUN_RIDE_HORSE)->PlaySpeed = 0.33f;
		gmClientModels->GetPlayerModelAction(PLAYER_DARKLORD_WALK)->PlaySpeed = 0.33f;
		gmClientModels->GetPlayerModelAction(PLAYER_WALK_WAND)->PlaySpeed = 0.44f;
		gmClientModels->GetPlayerModelAction(PLAYER_RUN_WAND)->PlaySpeed = 0.76f;

		if (g_isCharacterBuff(o, eDeBuff_Freeze))
		{
			gmClientModels->GetPlayerModelAction(PLAYER_RUN_RIDE_HORSE)->PlaySpeed *= timepow(0.5f);
			gmClientModels->GetPlayerModelAction(PLAYER_DARKLORD_WALK)->PlaySpeed *= timepow(0.5f);
		}
		else if (g_isCharacterBuff(o, eDeBuff_BlowOfDestruction))
		{
			gmClientModels->GetPlayerModelAction(PLAYER_RUN_RIDE_HORSE)->PlaySpeed *= timepow(0.33f);
			gmClientModels->GetPlayerModelAction(PLAYER_DARKLORD_WALK)->PlaySpeed *= timepow(0.33f);
		}

		int sc_type_helper = c->Helper.Type;

		if ((sc_type_helper == MODEL_HELPER + 37 || o->movementType == Movement::Fenrir) && !c->SafeZone && movement_normal_hide)
		{
			if (c->Run >= FENRIR_RUN_DELAY)
			{
				SetAction_Fenrir_Run(c, o);
			}
			else
			{
				SetAction_Fenrir_Walk(c, o);
			}
		}
		else if ((sc_type_helper == MODEL_HELPER + 4 || o->movementType == Movement::Horse) && !c->SafeZone && movement_normal_hide)
		{
			SetAction(o, PLAYER_RUN_RIDE_HORSE, true);
		}
		else if ((sc_type_helper == MODEL_HELPER + 2 || o->movementType == Movement::Uniria) && !c->SafeZone && movement_normal_hide)
		{
			if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
			{
				if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
					SetAction(o, PLAYER_RAGE_UNI_RUN_ONE_RIGHT, true);
				else
					SetAction(o, PLAYER_RAGE_UNI_RUN, true);
			}
			else if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
			{
				SetAction(o, PLAYER_RUN_RIDE_WEAPON, true);
			}
			else
			{
				SetAction(o, PLAYER_RUN_RIDE, true);
			}
		}
		else if ((sc_type_helper == MODEL_HELPER + 3 || o->movementType == Movement::Dinorant) && !c->SafeZone && movement_normal_hide)
		{
			if (World != WD_8TARKAN && World != WD_10HEAVEN)
			{
				if (!g_Direction.m_CKanturu.IsMayaScene())
				{
					if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
					{
						if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
							SetAction(o, PLAYER_RAGE_UNI_RUN_ONE_RIGHT, true);
						else
							SetAction(o, PLAYER_RAGE_UNI_RUN, true);
					}
					else if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
					{
						SetAction(o, PLAYER_RUN_RIDE_WEAPON, true);
					}
					else
					{
						SetAction(o, PLAYER_RUN_RIDE, true);
					}
				}
			}
		}
		else
		{
			if (c->m_pPet != NULL && c->SafeZone)
			{
				SetAction(o, PLAYER_DARKLORD_WALK, true);
			}
			else
			{
				if (o->SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER || o->SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER || c->SafeZone || c->Wing.Type == -1)
				{
					if (c->SafeZone || World != WD_7ATLANSE && !gMapManager->InHellas() && World != WD_67DOPPLEGANGER3)
					{
						if ((c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1) && (!c->SafeZone || gMapManager->InBloodCastle()))
						{
							if (c->Run >= 40)
							{
								if (c->Weapon[0].Type < MODEL_SWORD || c->Weapon[0].Type >= MODEL_MACE + MAX_ITEM_INDEX)
								{
									if (c->Weapon[0].Type < MODEL_STAFF + 14 || c->Weapon[0].Type > MODEL_STAFF + 20)
									{
										if (c->Weapon[0].Type < MODEL_STAFF || c->Weapon[0].Type >= MODEL_STAFF + MAX_ITEM_INDEX)
										{
											if (c->Weapon[0].Type < MODEL_SPEAR || c->Weapon[0].Type >= MODEL_SPEAR + MAX_ITEM_INDEX)
											{
												if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
												{
													SetAction(o, PLAYER_RUN_BOW, true);
												}
												else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
												{
													SetAction(o, PLAYER_RUN_CROSSBOW, true);
												}
												else
												{
													SetAction(o, PLAYER_RUN, true);
												}
											}
											else
											{
												SetAction(o, PLAYER_RUN_SPEAR, true);
											}
										}
										else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
										{
											SetAction(o, PLAYER_RUN_SPEAR, true);
										}
										else
										{
											SetAction(o, PLAYER_RUN_SWORD, true);
										}
									}
									else
									{
										SetAction(o, PLAYER_RUN_WAND, true);
									}
								}
								else if (c->Weapon[1].Type < MODEL_SWORD || c->Weapon[1].Type >= MODEL_MACE + MAX_ITEM_INDEX)
								{
									if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
									{
										if (c->Weapon[0].Type != MODEL_SWORD + 21
											&& c->Weapon[0].Type != MODEL_SWORD + 31
											&& c->Weapon[0].Type != MODEL_SWORD + 23
											&& c->Weapon[0].Type != MODEL_SWORD + 25)
										{
											SetAction(o, PLAYER_RUN_TWO_HAND_SWORD, true);
										}
										else
										{
											SetAction(o, PLAYER_RUN_TWO_HAND_SWORD_TWO, true);
										}
									}
									else
									{
										SetAction(o, PLAYER_RUN_SWORD, true);
									}
								}
								else if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
								{
									SetAction(o, PLAYER_RUN, true);
								}
								else
								{
									SetAction(o, PLAYER_RUN_TWO_SWORD, true);
								}
							}
							else if (c->Weapon[0].Type < MODEL_SWORD || c->Weapon[0].Type >= MODEL_MACE + MAX_ITEM_INDEX)
							{
								if (c->Weapon[0].Type < MODEL_STAFF + 14 || c->Weapon[0].Type > MODEL_STAFF + 20)
								{
									if (c->Weapon[0].Type < MODEL_STAFF || c->Weapon[0].Type >= MODEL_STAFF + MAX_ITEM_INDEX)
									{
										if (c->Weapon[0].Type != MODEL_SPEAR + 1 && c->Weapon[0].Type != MODEL_SPEAR + 2 + MAX_ITEM_INDEX)
										{
											if (c->Weapon[0].Type < MODEL_SPEAR || c->Weapon[0].Type >= MODEL_SPEAR + MAX_ITEM_INDEX)
											{
												if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
												{
													SetAction(o, PLAYER_WALK_BOW, true);
												}
												else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
												{
													SetAction(o, PLAYER_WALK_CROSSBOW, true);
												}
												else if (gCharacterManager.IsFemale(c->Class))
												{
													SetAction(o, PLAYER_WALK_FEMALE, true);
												}
												else
												{
													SetAction(o, PLAYER_WALK_MALE, true);
												}
											}
											else
											{
												SetAction(o, PLAYER_WALK_SCYTHE, true);
											}
										}
										else
										{
											SetAction(o, PLAYER_WALK_SPEAR, true);
										}
									}
									else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
									{
										SetAction(o, PLAYER_WALK_SCYTHE, true);
									}
									else
									{
										SetAction(o, PLAYER_WALK_SWORD, true);
									}
								}
								else
								{
									SetAction(o, PLAYER_WALK_WAND, true);
								}
							}
							else if (GMItemMng->find((c->Weapon[0].Type - MODEL_ITEM))->TwoHand)
							{
								if (c->Weapon[0].Type != MODEL_SWORD + 21
									&& c->Weapon[0].Type != MODEL_SWORD + 31
									&& c->Weapon[0].Type != MODEL_SWORD + 23
									&& c->Weapon[0].Type != MODEL_SWORD + 25)
								{
									SetAction(o, PLAYER_WALK_TWO_HAND_SWORD, true);
								}
								else
								{
									SetAction(o, PLAYER_WALK_TWO_HAND_SWORD_TWO, true);
								}
							}
							else
							{
								SetAction(o, PLAYER_WALK_SWORD, true);
							}
						}
						else if (c->Run < 40)
						{
							if (gCharacterManager.IsFemale(c->Class))
							{
								if (c->GetBaseClass() == CLASS_SUMMONER && gMapManager->InChaosCastle())
									SetAction(o, PLAYER_WALK_MALE, true);
								else
									SetAction(o, PLAYER_WALK_FEMALE, true);
							}
							else
							{
								SetAction(o, PLAYER_WALK_MALE, true);
							}
						}
						else
						{
							SetAction(o, PLAYER_RUN, true);
						}
					}
					else if (c->Run < 40)
					{
						SetAction(o, PLAYER_WALK_SWIM, true);
					}
					else
					{
						SetAction(o, PLAYER_RUN_SWIM, true);
					}
				}
				else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
				{
					SetAction(o, PLAYER_FLY_CROSSBOW, true);
				}
				else
				{
					SetAction(o, PLAYER_FLY, true);
				}
			}
		}
	}
	else
	{
		SetAction(o, MONSTER01_WALK, 1);
		PlayMonsterSound(o);
	}

	if (o->Type == MODEL_MONSTER01 + 27)
	{
		PlayBuffer(SOUND_BONE2, o);
	}
	else if (o->movementType == Movement::Horse && !c->SafeZone && movement_normal_hide)
	{
		PlayBuffer(SOUND_RUN_DARK_HORSE_1 + rand() % 3, o);
	}
	else if (o->movementType == Movement::Fenrir && !c->SafeZone && movement_normal_hide)
	{
		if (o->animation_fenrir_walk())
		{
			PlayBuffer(SOUND_FENRIR_RUN_1 + rand() % 2, o);
		}
		else if (o->animation_fenrir_walk_run())
		{
			PlayBuffer(SOUND_FENRIR_RUN_1 + rand() % 3, o);
		}
	}
	else if ((c == Hero && rand() % 64 == 0) || (c != Hero && rand() % 16 == 0))
	{
		if (o->Type != MODEL_PLAYER || (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3))
		{
			if (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3)
				PlayBuffer(SOUND_BONE1, o);

			BMD* pModels = gmClientModels->GetModel(o->Type);

			if (pModels && pModels->Sounds[0] != -1)
			{
				int offset = 0;
				if (o->SubType == 9)
					offset += 5;

				PlayBuffer(SOUND_MONSTER + offset + pModels->Sounds[rand() % 2], o);
			}
		}
	}
}

void SetAttackSpeed()
{
	int attack_speed = min(CharacterAttribute->AttackSpeed, 3276);
	int magic_speed = min(CharacterAttribute->MagicSpeed, 3276);

	int current_skill_id = CharacterAttribute->Skill[Hero->CurrentSkill];

	if (gCharacterManager.GetBaseClass(Hero->Class) == Dark_Knight)
	{
		if (current_skill_id == Skill_Death_Stab
			|| current_skill_id == Skill_Death_Stab_Mastery
			|| current_skill_id == Skill_Death_Stab_Proficiency
			|| current_skill_id == Skill_Death_Stab_Strengthener)
		{
			attack_speed = min(CharacterAttribute->AttackSpeed, 1086);
			magic_speed = min(CharacterAttribute->MagicSpeed, 1086);
		}
		else if (current_skill_id == Skill_Twisting_Slash
			|| current_skill_id == Skill_Twisting_Slash_Mastery
			|| current_skill_id == Skill_Twisting_Slash_Strengthener
			|| current_skill_id == Skill_Twisting_Slash_Strengthener2)
		{

			attack_speed = min(CharacterAttribute->AttackSpeed, 3170);
			magic_speed = min(CharacterAttribute->MagicSpeed, 3170);
		}
	}
	else if (Hero->GetBaseClass() == Dark_Wizard || Hero->GetBaseClass() == Magic_Gladiator)
	{
		if (current_skill_id == Skill_Evil_Spirit ||
			current_skill_id == AT_SKILL_STORM ||
			current_skill_id == AT_SKILL_BLAST ||
			current_skill_id == AT_SKILL_BLAST_POISON ||
			current_skill_id == AT_SKILL_BLAST_FREEZE ||
			current_skill_id == AT_SKILL_FLAME ||
			current_skill_id == Skill_Wind_Tome_Strengthener)
		{
			if (magic_speed > 450 && magic_speed < 480)
				magic_speed = 450;
			else if (magic_speed > 600 && magic_speed < 690)
				magic_speed = 600;
			else if (magic_speed > 850 && magic_speed < 1105)
				magic_speed = 850;
			else if (magic_speed > 1350 && magic_speed < 2355)
				magic_speed = 1350;
			else if (magic_speed > 2850)
				magic_speed = 2850;
		}
		else if (current_skill_id == AT_SKILL_REDUCEDEFENSE)
		{
			if (attack_speed > 1368)
				attack_speed = 1368;
		}
		else if (current_skill_id == Skill_Twisting_Slash
			|| current_skill_id == Skill_Twisting_Slash_Mastery
			|| current_skill_id == Skill_Twisting_Slash_Strengthener
			|| current_skill_id == Skill_Twisting_Slash_Strengthener2)
		{

			attack_speed = min(CharacterAttribute->AttackSpeed, 3170);
			magic_speed = min(CharacterAttribute->MagicSpeed, 3170);
		}
	}
	else if (gCharacterManager.GetBaseClass(Hero->Class) == Fairy_Elf)
	{
		if (current_skill_id == AT_SKILL_CROSSBOW)
		{
			if (attack_speed > 508)
				attack_speed = 500;
		}
		else if (current_skill_id == AT_SKILL_PARALYZE ||
			current_skill_id == AT_SKILL_PIERCING)
		{
			if (attack_speed > 400)
				attack_speed = 400;
		}
	}
	else if (gCharacterManager.GetBaseClass(Hero->Class) == Dark_Lord)
	{
		if (current_skill_id == Skill_Fire_Scream)
		{
			if (attack_speed > 249 && attack_speed < 264)
				attack_speed = 249;
			else if (attack_speed > 324 && attack_speed < 367)
				attack_speed = 324;
			else if (attack_speed > 449 && attack_speed < 575)
				attack_speed = 449;
			else if (attack_speed > 699 && attack_speed < 1200)
				attack_speed = 699;
			else if (attack_speed > 1449)
				attack_speed = 1449;
		}
	}

	float AttackSpeed1 = attack_speed * 0.004f;
	float AttackSpeed2 = attack_speed * 0.002f;
	float MagicSpeed1 = magic_speed * 0.004f;
	float MagicSpeed2 = magic_speed * 0.002f;

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_FIST)->PlaySpeed = 0.6f + AttackSpeed1;

	for (int i = PLAYER_HIT_SWORD_RIGHT1; i <= PLAYER_HIT_RIDE_CROSSBOW; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.25f + AttackSpeed1;
	}

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SWORD1)->PlaySpeed = 0.30f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SWORD2)->PlaySpeed = 0.30f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SWORD3)->PlaySpeed = 0.27f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SWORD4)->PlaySpeed = 0.30f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SWORD5)->PlaySpeed = 0.24f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_WHEEL)->PlaySpeed = 0.24f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_ONETOONE)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_SPEAR)->PlaySpeed = 0.30f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_RIDER)->PlaySpeed = 0.3f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_RIDER_FLY)->PlaySpeed = 0.3f + AttackSpeed1;

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_TWO_HAND_SWORD_TWO)->PlaySpeed = 0.25f + AttackSpeed1;

	for (int i = PLAYER_HIT_BOW; i <= PLAYER_HIT_FLY_CROSSBOW; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.30f + AttackSpeed1;
	for (int i = PLAYER_HIT_RIDE_BOW; i <= PLAYER_HIT_RIDE_CROSSBOW; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.30f + AttackSpeed1;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_ELF1)->PlaySpeed = 0.25f + MagicSpeed1;

	for (int i = PLAYER_SKILL_HAND1; i <= PLAYER_SKILL_WEAPON2; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.29f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_TELEPORT)->PlaySpeed = 0.30f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_FLASH)->PlaySpeed = 0.40f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_INFERNO)->PlaySpeed = 0.60f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_HELL)->PlaySpeed = 0.50f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_RIDE_SKILL)->PlaySpeed = 0.30f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_HELL_BEGIN)->PlaySpeed = 0.50f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_STRIKE)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_STRIKE)->PlaySpeed = 0.2f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_HORSE_SWORD)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_ATTACK_FLASH)->PlaySpeed = 0.40f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_ATTACK_MAGIC)->PlaySpeed = 0.3f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE)->PlaySpeed = 0.2f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_SWORD)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_FLASH)->PlaySpeed = 0.40f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_TWO_SWORD)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_MAGIC)->PlaySpeed = 0.37f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_CROSSBOW)->PlaySpeed = 0.30f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_SPEAR)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_ONE_SWORD)->PlaySpeed = 0.25f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_BOW)->PlaySpeed = 0.30f + AttackSpeed1;

	for (int i = PLAYER_HIT_BOW_UP; i <= PLAYER_HIT_RIDE_CROSSBOW_UP; ++i)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.30f + AttackSpeed1;
	}

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_ONE_FLASH)->PlaySpeed = 0.4f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RUSH)->PlaySpeed = 0.3f + AttackSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_DEATH_CANNON)->PlaySpeed = 0.2f + AttackSpeed1;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SLEEP)->PlaySpeed = 0.3f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SLEEP_UNI)->PlaySpeed = 0.3f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SLEEP_DINO)->PlaySpeed = 0.3f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SLEEP_FENRIR)->PlaySpeed = 0.3f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_LIGHTNING_ORB)->PlaySpeed = 0.4f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_LIGHTNING_ORB_UNI)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_LIGHTNING_ORB_DINO)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_LIGHTNING_ORB_FENRIR)->PlaySpeed = 0.25f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_CHAIN_LIGHTNING)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_CHAIN_LIGHTNING_UNI)->PlaySpeed = 0.15f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_CHAIN_LIGHTNING_DINO)->PlaySpeed = 0.15f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_CHAIN_LIGHTNING_FENRIR)->PlaySpeed = 0.15f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAIN_LIFE)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAIN_LIFE_UNI)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAIN_LIFE_DINO)->PlaySpeed = 0.25f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAIN_LIFE_FENRIR)->PlaySpeed = 0.25f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_GIGANTICSTORM)->PlaySpeed = 0.55f + MagicSpeed1;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_FLAMESTRIKE)->PlaySpeed = 0.69f + MagicSpeed2;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_LIGHTNING_SHOCK)->PlaySpeed = 0.35f + MagicSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SUMMON)->PlaySpeed = 0.25f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SUMMON_UNI)->PlaySpeed = 0.25f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SUMMON_DINO)->PlaySpeed = 0.25f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SUMMON_FENRIR)->PlaySpeed = 0.25f;


	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_BLOW_OF_DESTRUCTION)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_RECOVER_SKILL)->PlaySpeed = 0.33f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_SWELL_OF_MP)->PlaySpeed = 0.2f;

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_FURY_STRIKE)->PlaySpeed = 0.38f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_VITALITY)->PlaySpeed = 0.34f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_HELL_START)->PlaySpeed = 0.30f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_TELEPORT)->PlaySpeed = 0.28f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_TELEPORT)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_DARKHORSE)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_REMOVAL)->PlaySpeed = 0.28f;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_THRUST)->PlaySpeed = 0.4f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_STAMP)->PlaySpeed = 0.4f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_GIANTSWING)->PlaySpeed = 0.4f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DARKSIDE_READY)->PlaySpeed = 0.3f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DARKSIDE_ATTACK)->PlaySpeed = 0.3f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAGONKICK)->PlaySpeed = 0.4f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_DRAGONLORE)->PlaySpeed = 0.3f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_ATT_UP_OURFORCES)->PlaySpeed = 0.35f;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_HP_UP_OURFORCES)->PlaySpeed = 0.35f;

	gmClientModels->GetPlayerModelAction(PLAYER_RAGE_FENRIR_ATTACK_RIGHT)->PlaySpeed = 0.25f + AttackSpeed2;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_PHOENIX_SHOT)->PlaySpeed = 0.74;
}

void SetPlayerHighBowAttack(CHARACTER* sc)
{
	OBJECT* so = &sc->Object;

	int sc_type_helper = sc->Helper.Type;

	int EquipedBowType = gCharacterManager.GetEquipedBowType(sc);

	if (so->Type == MODEL_PLAYER)
	{
		SetAttackSpeed();

		if (EquipedBowType == BOWTYPE_BOW)
		{
			if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_BOW_UP, true);
			}
			else
			{
				if (sc->Wing.Type != -1)
					SetAction(so, PLAYER_HIT_FLY_BOW_UP);
				else
					SetAction(so, PLAYER_HIT_BOW_UP);
			}
		}
		else if (EquipedBowType == BOWTYPE_CROSSBOW)
		{
			if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
			}
			else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
			{
				SetAction(so, PLAYER_HIT_RIDE_CROSSBOW_UP, true);
			}
			else
			{
				if (sc->Wing.Type != -1)
					SetAction(so, PLAYER_HIT_FLY_CROSSBOW_UP);
				else
					SetAction(so, PLAYER_HIT_CROSSBOW_UP);
			}
		}
	}

	sc->SwordCount++;
}

void SetPlayerAttack(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	if (o->Type == MODEL_PLAYER)
	{
		SetAttackSpeed();

		int sc_type_helper = c->Helper.Type;
		int sc_type_weapon01 = c->Weapon[0].Type;
		int sc_type_weapon02 = c->Weapon[1].Type;

		if ((sc_type_helper == MODEL_HELPER + 37 || o->movementType == Movement::Fenrir) && !c->SafeZone)
		{
			if (c->GetBaseClass() == CLASS_DARK_LORD)
			{
				SetAction(o, PLAYER_FENRIR_ATTACK_DARKLORD_SWORD, 1);
			}
			else
			{
				if (sc_type_weapon01 < MODEL_SPEAR || sc_type_weapon01 >= MODEL_SPEAR + 5)
				{
					if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
					{
						SetAction(o, PLAYER_FENRIR_ATTACK_BOW, 1);
					}
					else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
					{
						SetAction(o, PLAYER_FENRIR_ATTACK_CROSSBOW, 1);
					}
					else if (sc_type_weapon01 == -1 || sc_type_weapon02 == -1)
					{
						if (sc_type_weapon01 == -1 || sc_type_weapon02 != -1)
						{
							if (sc_type_weapon01 != -1 || sc_type_weapon02 == -1 || c->GetBaseClass() != CLASS_RAGEFIGHTER)
							{
								if (sc_type_weapon01 != -1 || sc_type_weapon02 == -1)
									SetAction(o, PLAYER_FENRIR_ATTACK, 1);
								else
									SetAction(o, PLAYER_FENRIR_ATTACK_ONE_SWORD, 1);
							}
							else
							{
								SetAction(o, PLAYER_RAGE_FENRIR_ATTACK_RIGHT, 1);
							}
						}
						else
						{
							SetAction(o, PLAYER_FENRIR_ATTACK_ONE_SWORD, 1);
						}
					}
					else
					{
						SetAction(o, PLAYER_FENRIR_ATTACK_TWO_SWORD, 1);
					}
				}
				else
				{
					SetAction(o, PLAYER_FENRIR_ATTACK_SPEAR, 1);
				}
			}
		}
		else if ((sc_type_helper == MODEL_HELPER + 2 || o->movementType == Movement::Uniria || sc_type_helper == MODEL_HELPER + 3 || o->movementType == Movement::Dinorant) && !c->SafeZone)
		{
			if (sc_type_weapon01 < MODEL_SPEAR || sc_type_weapon01 >= MODEL_SPEAR + 5)
			{
				if (sc_type_weapon01 < MODEL_SPEAR + 5 || sc_type_weapon01 >= MODEL_BOW)
				{
					if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
					{
						SetAction(o, PLAYER_HIT_RIDE_BOW, 1);
					}
					else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
					{
						SetAction(o, PLAYER_HIT_RIDE_CROSSBOW, 1);
					}
					else if (sc_type_weapon01 == -1)
					{
						if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
							SetAction(o, PLAYER_RAGE_UNI_ATTACK, 1);
						else
							SetAction(o, PLAYER_HIT_RIDE_SWORD, 1);
					}
					else if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
					{
						if (GMItemMng->find((sc_type_weapon01 - MODEL_ITEM))->TwoHand)
							SetAction(o, PLAYER_RAGE_UNI_ATTACK_ONE_RIGHT, 1);
						else
							SetAction(o, PLAYER_RAGE_UNI_ATTACK, 1);
					}
					else if (GMItemMng->find((sc_type_weapon01 - MODEL_ITEM))->TwoHand)
					{
						SetAction(o, PLAYER_HIT_RIDE_TWO_HAND_SWORD, 1);
					}
					else
					{
						SetAction(o, PLAYER_HIT_RIDE_SWORD, 1);
					}
				}
				else
				{
					SetAction(o, PLAYER_HIT_RIDE_SCYTHE, 1);
				}
			}
			else
			{
				SetAction(o, PLAYER_HIT_RIDE_SPEAR, 1);
			}
		}
		else if ((sc_type_helper == MODEL_HELPER + 4 || o->movementType == Movement::Horse) && !c->SafeZone)
		{
			if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
			{
				SetAction(o, PLAYER_HIT_RIDE_BOW, 1);
			}
			else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
			{
				SetAction(o, PLAYER_HIT_RIDE_CROSSBOW, 1);
			}
			else
			{
				SetAction(o, PLAYER_HIT_RIDE_HORSE_SWORD, 1);
			}
		}
		else
		{
			if (sc_type_weapon01 != -1 || sc_type_weapon02 != -1)
			{
				if (sc_type_weapon01 < MODEL_SWORD || sc_type_weapon01 >= MODEL_MACE + MAX_ITEM_INDEX)
				{
					if (sc_type_weapon02 < MODEL_SWORD || sc_type_weapon02 >= MODEL_MACE + MAX_ITEM_INDEX)
					{
						if (sc_type_weapon01 < MODEL_STAFF || sc_type_weapon01 >= MODEL_STAFF + MAX_ITEM_INDEX)
						{
							if (sc_type_weapon01 != MODEL_SPEAR + 1 && sc_type_weapon01 != MODEL_SPEAR + 2)
							{
								if (sc_type_weapon01 < MODEL_SPEAR || sc_type_weapon01 >= MODEL_SPEAR + MAX_ITEM_INDEX)
								{
									if (gCharacterManager.GetEquipedBowType(c) == 1)
									{
										if (c->Wing.Type == -1)
											SetAction(o, PLAYER_HIT_BOW, 1);
										else
											SetAction(o, PLAYER_HIT_FLY_BOW, 1);
									}
									else if (gCharacterManager.GetEquipedBowType(c) == 2)
									{
										if (c->Wing.Type == -1)
											SetAction(o, PLAYER_HIT_CROSSBOW, 1);
										else
											SetAction(o, PLAYER_HIT_FLY_CROSSBOW, 1);
									}
									else
									{
										SetAction(o, PLAYER_HIT_FIST, 1);
									}
								}
								else
								{
									SetAction(o, c->SwordCount % 3 + PLAYER_HIT_SCYTHE1, 1);
								}
							}
							else
							{
								SetAction(o, PLAYER_HIT_SPEAR1, 1);
							}
						}
						else if (GMItemMng->find((sc_type_weapon01 - MODEL_ITEM))->TwoHand)
						{
							SetAction(o, (rand() % 2) + PLAYER_SKILL_WEAPON1, 1);
						}
						else
						{
							SetAction(o, (rand() % 2) + PLAYER_HIT_SWORD_RIGHT1, 1);
						}
					}
					else
					{
						SetAction(o, (rand() % 2) + PLAYER_HIT_SWORD_LEFT1, 1);
					}
				}
				else if (GMItemMng->find((sc_type_weapon01 - MODEL_ITEM))->TwoHand)
				{
					if (sc_type_weapon01 != MODEL_SWORD + 21
						&& sc_type_weapon01 != MODEL_SWORD + 31
						&& sc_type_weapon01 != MODEL_SWORD + 23
						&& sc_type_weapon01 != MODEL_SWORD + 25)
					{
						SetAction(o, c->SwordCount % 3 + PLAYER_HIT_TWO_HAND_SWORD1, 1);
					}
					else
					{
						SetAction(o, PLAYER_HIT_TWO_HAND_SWORD_TWO, 1);
					}
				}
				else if (sc_type_weapon02 < MODEL_SWORD || sc_type_weapon02 >= MODEL_MACE + MAX_ITEM_INDEX)
				{
					SetAction(o, c->SwordCount % 2 + PLAYER_HIT_SWORD_RIGHT1, 1);
				}
				else
				{
					switch (c->SwordCount % 4)
					{
					case 0:
						SetAction(o, PLAYER_HIT_SWORD_RIGHT1, 1);
						break;
					case 1:
						SetAction(o, PLAYER_HIT_SWORD_LEFT1, 1);
						break;
					case 2:
						SetAction(o, PLAYER_HIT_SWORD_RIGHT2, 1);
						break;
					case 3:
						SetAction(o, PLAYER_HIT_SWORD_LEFT2, 1);
						break;
					}
				}
			}
			else
			{
				SetAction(o, PLAYER_HIT_FIST, 1);
			}
		}
	}
	else if (o->Type == 39)
	{
		CreateEffect(MODEL_SAW, o->Position, o->Angle, o->Light);
		PlayBuffer(SOUND_TRAP01, o);
	}
	else if (o->Type == 40)
	{
		SetAction(&c->Object, 1);
		PlayBuffer(SOUND_TRAP01, o);
	}
	else if (o->Type == 51)
	{
		CreateEffect(BITMAP_FIRE + 1, o->Position, o->Angle, o->Light);
		PlayBuffer(SOUND_FLAME);
	}
	else
	{
		if (o->Type == MODEL_MONSTER01 + 32)
		{
			int Action = rand() % 8;
			if (Action > 2)
				SetAction(&c->Object, MONSTER01_ATTACK1 + rand() % 2);
			else if (Action > 0)
				SetAction(&c->Object, MONSTER01_ATTACK3);
			else
				SetAction(&c->Object, MONSTER01_ATTACK4);
		}
		else
		{
			bool Success = true;

			if (SetCurrentAction_HellasMonster(c, o) == true)
				Success = false;
			if (battleCastle::SetCurrentAction_BattleCastleMonster(c, o) == true)
				Success = false;
			if (M31HuntingGround::SetCurrentActionHuntingGroundMonster(c, o) == true)
				Success = false;
			if (M33Aida::SetCurrentActionAidaMonster(c, o) == true)
				Success = false;
			if (M34CryWolf1st::SetCurrentActionCrywolfMonster(c, o) == true)
				Success = false;
			if (M37Kanturu1st::SetCurrentActionKanturu1stMonster(c, o) == true)
				Success = false;
			if (M38Kanturu2nd::Set_CurrentAction_Kanturu2nd_Monster(c, o) == true)
				Success = false;
			if (M39Kanturu3rd::SetCurrentActionKanturu3rdMonster(c, o) == true)
				Success = false;
			if (SEASON3A::CGM3rdChangeUp::Instance().SetCurrentActionBalgasBarrackMonster(c, o))
				Success = false;
			if (g_CursedTemple->SetCurrentActionMonster(c, o) == true)
				Success = false;
			if (SEASON3B::GMNewTown::SetCurrentActionMonster(c, o) == true)
				Success = false;
			if (SEASON3C::GMSwampOfQuiet::SetCurrentActionMonster(c, o) == true)
				Success = false;
			if (TheMapProcess().SetCurrentActionMonster(c, o) == true)
				Success = false;

			if (Success)
			{
				if (c->SwordCount % 3 == 0)
					SetAction(&c->Object, MONSTER01_ATTACK1);
				else
					SetAction(&c->Object, MONSTER01_ATTACK2);
				c->SwordCount++;
			}
		}
	}

	if (World == WD_73NEW_LOGIN_SCENE || c->Object.AnimationFrame != 0.f)
	{
		c->SwordCount++;
	}
	else
	{
		if (o->Type == MODEL_PLAYER && (o->SubType < MODEL_SKELETON1 || o->SubType > MODEL_SKELETON3))
		{
			if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_BOW)
			{
				PlayBuffer(SOUND_BOW01, o);
			}
			else if (gCharacterManager.GetEquipedBowType(c) == BOWTYPE_CROSSBOW)
			{
				PlayBuffer(SOUND_CROSSBOW01, o);
			}
			else if (c->Weapon[0].Type >= MODEL_BOW + 13 && c->Weapon[0].Type < MODEL_BOW + 15)
				PlayBuffer(SOUND_MAGIC, o);
			else if (c->Weapon[0].Type == MODEL_SWORD + 10 || c->Weapon[0].Type == MODEL_SPEAR)
				PlayBuffer(SOUND_BRANDISH_SWORD03, o);
			else if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
				PlayBuffer(SOUND_BRANDISH_SWORD01 + rand() % 2, o);
		}
		else
		{
			if (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3)
			{
				PlayBuffer(SOUND_BRANDISH_SWORD01 + rand() % 2, o);
			}
			BMD* pModels = gmClientModels->GetModel(o->Type);

			if (pModels && pModels->Sounds[2] != -1)
			{
				int offset = ((int)(o->SubType == 9) * 5);

				PlayBuffer(SOUND_MONSTER + offset + pModels->Sounds[2 + rand() % 2], o);
			}
		}
		c->SwordCount++;
	}
}

void SetPlayerMagic(CHARACTER* sc)
{
	OBJECT* so = &sc->Object;
	if (so->Type == MODEL_PLAYER)
	{
		SetAttackSpeed();

		int sc_type_helper = sc->Helper.Type;

		if ((sc_type_helper == MODEL_HELPER + 37 || so->movementType == Movement::Fenrir) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_FENRIR_ATTACK_MAGIC);
		}
		else if ((sc_type_helper == MODEL_HELPER + 2 || so->movementType == Movement::Uniria) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_RIDE_SKILL);
		}
		else if ((sc_type_helper == MODEL_HELPER + 3 || so->movementType == Movement::Dinorant) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_RIDE_SKILL);
		}
		else if ((sc_type_helper == MODEL_HELPER + 4 || so->movementType == Movement::Horse) && !sc->SafeZone)
		{
			SetAction(so, PLAYER_HIT_RIDE_ATTACK_MAGIC);
		}
		else
		{
			if (gCharacterManager.IsFemale(sc->Class))
				SetAction(so, PLAYER_SKILL_ELF1);
			else
				SetAction(so, (rand() % 2) + PLAYER_SKILL_HAND1);
		}
	}
	else
	{
		if ((bool)(sc->SwordCount % 3))
			SetAction(so, MONSTER01_ATTACK2);
		else
			SetAction(so, MONSTER01_ATTACK1);
		sc->SwordCount++;
	}
}

void SetPlayerTeleport(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	if (o->Type == MODEL_PLAYER)
		SetAction(o, PLAYER_SKILL_TELEPORT);
	else
		SetAction(o, MONSTER01_SHOCK);
}

void SetPlayerShock(CHARACTER* c, int Hit)
{
	if (c->Dead == 0)
	{
		OBJECT* o = &c->Object;
		int sc_type_helper = c->Helper.Type;

		if (!(sc_type_helper == MODEL_HELPER + 2 || o->movementType == Movement::Uniria || sc_type_helper == MODEL_HELPER + 3 || o->movementType == Movement::Dinorant))
		{
			if (!(sc_type_helper == MODEL_HELPER + 4 || o->movementType == Movement::Horse))
			{
				if (o->CurrentAction != PLAYER_HIT_SKILL_FURY_STRIKE)
				{
					if (o->CurrentAction != PLAYER_SKILL_VITALITY && o->CurrentAction != PLAYER_SKILL_HELL_BEGIN)
					{
						if (o->CurrentAction != PLAYER_SKILL_ATT_UP_OURFORCES)
						{
							if (o->CurrentAction != PLAYER_SKILL_HP_UP_OURFORCES && o->CurrentAction != PLAYER_SKILL_GIANTSWING)
							{
								if (o->CurrentAction != PLAYER_SKILL_DRAGONLORE)
								{
									if (o->CurrentAction != PLAYER_SKILL_PHOENIX_SHOT && Hit > 0)
									{
										if (o->Type == MODEL_PLAYER)
										{
											if (sc_type_helper == MODEL_HELPER + 37 || o->movementType == Movement::Fenrir)
											{
												SetAction_Fenrir_Damage(c, o);

												if (!(rand() % 3))
												{
													PlayBuffer((rand() % 2) + SOUND_FENRIR_DAMAGE_1, o);
												}
											}
											else
											{
												SetAction(o, PLAYER_SHOCK, true);
											}
											c->Movement = false;
										}
										else if (o->CurrentAction < MONSTER01_ATTACK1 || o->CurrentAction > MONSTER01_ATTACK2)
										{
											SetAction(o, MONSTER01_SHOCK, 1);
										}
										if (o->AnimationFrame == 0.0)
										{
											PlayMonsterSound(o);

											if (o->Type == MODEL_PLAYER && (o->SubType < MODEL_SKELETON1 || o->SubType > MODEL_SKELETON3))
											{
												if (gCharacterManager.IsFemale(c->Class))
												{
													PlayBuffer((rand() % 2) + SOUND_HUMAN_SCREAM01, o, 0);
												}
												else if (c->GetBaseClass() == CLASS_DARK_LORD && rand() % 5)
												{
													if ((sc_type_helper != MODEL_HELPER + 37 && o->movementType != 5))
														PlayBuffer(SOUND_DARKLORD_PAIN, o, 0);
												}
												else
												{
													PlayBuffer((rand() % 3) + 105, o, 0);
												}
											}
											else if (o->SubType < MODEL_SKELETON1 || o->SubType > MODEL_SKELETON3)
											{
												BMD* b = gmClientModels->GetModel(o->Type);

												if (b && o->Type != MODEL_MONSTER01 + 14 && b->Sounds[2] != -1)
												{
													int offset = 0;
													if (o->SubType == 9)
													{
														offset = 5;
													}
													PlayBuffer(SOUND_MONSTER + offset + b->Sounds[2 + rand() % 2], o);
												}
											}
											else
											{
												PlayBuffer(SOUND_BONE1, o, 0);
											}
										}

										if (o->Type == MODEL_MONSTER01 + 61)
										{
											vec3_t Position;
											for (int i = 0; i < 3; ++i)
											{
												if ((rand() % 2) == 0)
												{
													Position[0] = o->Position[0] + (rand() % 128 - 64);
													Position[1] = o->Position[1];
													Position[2] = o->Position[2] + 200 + (rand() % 50);
													CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, o->Light);
												}
											}
											PlayBuffer(SOUND_HIT_GATE);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void SetPlayerDie(CHARACTER* c)
{
	OBJECT* o = &c->Object;

	if (c == Hero)
	{
		CharacterAttribute->Life = 0;
	}

	if (c->Object.Type == MODEL_PLAYER)
	{
		if (o->SubType < MODEL_SKELETON1 || o->SubType > MODEL_SKELETON3)
		{
			SetAction(o, PLAYER_DIE1);
		}
		else
		{
			o->Live = false;
			CreateEffect(MODEL_BONE1, o->Position, o->Angle, o->Light);
			for (int j = 0; j < 10; j++)
			{
				CreateEffect(MODEL_BONE2, o->Position, o->Angle, o->Light);
			}
			PlayBuffer(SOUND_BONE2, o);
		}
	}
	else
	{
		switch (o->Type)
		{
		case MODEL_MONSTER01 + 30:
		{
			o->Live = false;
			CreateEffect(MODEL_BONE1, o->Position, o->Angle, o->Light);
			for (int j = 0; j < 10; j++)
				CreateEffect(MODEL_BONE2, o->Position, o->Angle, o->Light);
			PlayBuffer(SOUND_BONE2, o);
		}
		break;
		case MODEL_MONSTER01 + 25:
		{
			o->Live = false;
			for (int j = 0; j < 8; j++)
			{
				CreateEffect(MODEL_BIG_STONE1, o->Position, o->Angle, o->Light);
				CreateEffect(MODEL_BIG_STONE2, o->Position, o->Angle, o->Light);
			}
			PlayBuffer(SOUND_BONE2, o);
		}
		break;
		case MODEL_MONSTER01 + 110:
		case MODEL_MONSTER01 + 115:
		{
			if (World == WD_39KANTURU_3RD && g_Direction.m_CKanturu.m_iKanturuState == KANTURU_STATE_MAYA_BATTLE)
			{
				FallingMonster(c, o);
				SetAction(&c->Object, MONSTER01_STOP2);
			}
			else
				SetAction(&c->Object, MONSTER01_DIE);
		}
		break;
		case MODEL_MONSTER01 + 118:
		case MODEL_MONSTER01 + 119:
		{
			if (World == WD_39KANTURU_3RD && g_Direction.m_CKanturu.m_iMayaState >= KANTURU_MAYA_DIRECTION_MAYA3)
				SetAction(&c->Object, MONSTER01_STOP2);
			else
				SetAction(&c->Object, MONSTER01_DIE);
		}
		break;
		case MODEL_MONSTER01 + 191:
		{
			if (c->Object.CurrentAction != MONSTER01_APEAR)
			{
				SetAction(&c->Object, MONSTER01_DIE);
			}
		}
		break;
		default:
			SetAction(&c->Object, MONSTER01_DIE);
			break;
		}
	}

	if (c->Object.AnimationFrame == 0.f)
	{
		PlayMonsterSound(o);
		if (gMapManager->InChaosCastle() == true)
		{
			PlayBuffer(SOUND_CHAOS_MOB_BOOM01 + rand() % 2, o);
		}
		else
		{
			if (o->Type != MODEL_PLAYER || (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3))
			{
				BMD* b = gmClientModels->GetModel(o->Type);

				if (b && b->Sounds[4] != -1)
				{
					int offset = 0;
					if (o->SubType == 9)
					{
						offset = 5;
					}
					PlayBuffer(SOUND_MONSTER + offset + b->Sounds[4], o);
				}
			}
			else
			{
				if (!gCharacterManager.IsFemale(c->Class))
				{
					if (c->GetBaseClass() == CLASS_DARK_LORD)
						PlayBuffer(SOUND_DARKLORD_DEAD, o);
					else
						PlayBuffer(SOUND_HUMAN_SCREAM04, o);
				}
				else
				{
					PlayBuffer(SOUND_FEMALE_SCREAM02, o);
				}

				if (c->Helper.Type == MODEL_HELPER + 37 || o->movementType == Movement::Fenrir)
					PlayBuffer(SOUND_FENRIR_DEATH, o);
			}
		}
		c->Object.AnimationFrame = 0.001f;
	}
}

void CalcAddPosition(OBJECT* o, float x, float y, float z, vec3_t Position)
{
	vec3_t p;
	float Matrix[3][4];

	AngleMatrix(o->Angle, Matrix);
	Vector(x, y, z, p);
	VectorRotate(p, Matrix, Position);
	VectorAdd(Position, o->Position, Position);
}

void AttackEffect(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	BMD* b = gmClientModels->GetModel(o->Type);

	if (!b)
	{
		return;
	}

	int i;
	vec3_t Angle, Light;
	vec3_t p, Position;
	float Luminosity = (float)(rand() % 6 + 2) * 0.1f;
	Vector(0.f, 0.f, 0.f, p);
	Vector(1.f, 1.f, 1.f, Light);

	if (gMapManager->InHellas())
	{
		OBJECT* to = NULL;
		CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
		if (tc)
		{
			to = &tc->Object;
			if (AttackEffect_HellasMonster(c, tc, o, to, b) == true)
				return;
		}

	}
	if (M31HuntingGround::AttackEffectHuntingGroundMonster(c, o, b) == true
		|| battleCastle::AttackEffect_BattleCastleMonster(c, o, b) == true
		|| M33Aida::AttackEffectAidaMonster(c, o, b) == true
		|| M34CryingWolf2nd::AttackEffectCryingWolf2ndMonster(c, o, b) == true
		|| M37Kanturu1st::AttackEffectKanturu1stMonster(c, o, b) == true
		|| M38Kanturu2nd::AttackEffect_Kanturu2nd_Monster(c, o, b) == true
		|| M39Kanturu3rd::AttackEffectKanturu3rdMonster(c, o, b) == true
		|| M34CryWolf1st::AttackEffectCryWolf1stMonster(c, o, b) == true
		|| SEASON3A::CGM3rdChangeUp::Instance().AttackEffectBalgasBarrackMonster(c, o, b)
		|| g_CursedTemple->AttackEffectMonster(c, o, b) == true
		|| SEASON3B::GMNewTown::AttackEffectMonster(c, o, b) == true
		|| SEASON3C::GMSwampOfQuiet::AttackEffectMonster(c, o, b) == true
		|| TheMapProcess().AttackEffectMonster(c, o, b) == true)
	{
		return;
	}

	switch (c->MonsterIndex)
	{
	case 162:
	case 164:
	case 166:
	case 168:
	case 170:
	case 172:
	case 426:
		break;
	case 89:
	case 95:
	case 112:
	case 118:
	case 124:
	case 130:
	case 143:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if ((rand() % 2) == 0)
			{
				if (c->AttackTime == 1)
				{
					CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light, 1);
				}
			}
			else
			{
				Vector(o->Position[0] + rand() % 1024 - 512, o->Position[1] + rand() % 1024 - 512, o->Position[2], Position);
				CreateEffectSync(MODEL_FIRE, Position, o->Angle, o->Light);
				PlayBuffer(SOUND_METEORITE01);
			}
		}
		break;
	case 68:
		break;
	case 69:
		break;
	case 70:
		if (c->AttackTime == 5)
		{
			CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
			if (tc)
			{
				OBJECT* to = &tc->Object;
				for (int i = 0; i < 20; ++i)
				{
					CreateEffect(BITMAP_BLIZZARD, to->Position, to->Angle, Light);
				}
			}
		}
		break;
	case 301:
	case 71:
	case 74:
		if (c->Object.CurrentAction == MONSTER01_ATTACK1 || c->Object.CurrentAction == MONSTER01_ATTACK2)
		{
			if (c->AttackTime == 5)
			{
				CreateInferno(o->Position);
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light);
			}
		}
		break;
	case 72:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 14)
			{
				vec3_t Angle = { 0.0f, 0.0f, 0.0f };
				int iCount = 36;

				for (int i = 0; i < iCount; ++i)
				{
					//Angle[2] = ( float)i * 10.0f;
					Angle[0] = (float)(rand() % 360);
					Angle[1] = (float)(rand() % 360);
					Angle[2] = (float)(rand() % 360);
					vec3_t Position;
					VectorCopy(o->Position, Position);
					Position[2] += 100.f;
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, Position, Angle, 1, NULL, 60.f, 0, 0);
				}
			}
		}
		break;
	case 73:
	case 75:
		if (c->Object.CurrentAction == MONSTER01_ATTACK1)
		{
			if (c->AttackTime == 11)
			{
				CreateInferno(o->Position);
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light);

				for (int k = 0; k < 5; ++k)
				{
					Vector(1.f, 0.5f, 0.f, Light);
					VectorCopy(o->Angle, Angle);
					Angle[0] += 45.f;
					VectorCopy(o->Position, Position);
					Position[0] += (float)(rand() % 1001 - 500);
					Position[1] += (float)(rand() % 1001 - 500);
					Position[2] += 500.f;
					VectorCopy(Position, o->StartPosition);
					CreateEffect(MODEL_PIERCING + 1, Position, Angle, Light, 1, o);
				}
			}
		}
		else
		{
			if (c->AttackTime == 13)
			{
				Vector(1.f, 0.5f, 0.f, Light);
				Vector(-50.f, 100.f, 0.f, p);
				VectorCopy(o->Angle, Angle);
				Angle[0] += 45.f;
				b->TransformPosition(o->BoneTransform[11], p, Position, true);
				VectorCopy(Position, o->StartPosition);
				CreateEffect(MODEL_PIERCING + 1, Position, Angle, Light, 1, o);
				PlayBuffer(SOUND_METEORITE01);
			}
			else if (c->AttackTime == 9)
			{
				Vector(1.f, 0.5f, 0.f, Light);
				Vector(0.f, 0.f, 0.f, p);
				VectorCopy(o->Angle, Angle);
				Angle[0] += 45.f;
				b->TransformPosition(o->BoneTransform[11], p, Position, true);
			}
		}

		break;
	case 77:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 2 || c->AttackTime == 6)
			{
				vec3_t Angle = { 0.0f, 0.0f, 0.0f };
				int iCount = 40;
				for (i = 0; i < iCount; ++i)
				{
					//Angle[2] = ( float)i * 10.0f;
					Angle[0] = (float)(rand() % 360);
					Angle[1] = (float)(rand() % 360);
					Angle[2] = (float)(rand() % 360);
					vec3_t Position;
					VectorCopy(o->Position, Position);
					Position[2] += 100.f;
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, Position, Angle, 3, NULL, 50.f, 0, 0);
				}
			}
		}
		break;
	case 63:
	case 61:
		if (c->MonsterIndex == 63)
		{
			if (c->AttackTime == 1)
			{
				CreateInferno(o->Position);
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light);
			}
			if ((c->Skill) == AT_SKILL_BOSS)
			{
				if (c->MonsterIndex == 63)
				{
					Vector(o->Position[0] + rand() % 800 - 400, o->Position[1] + rand() % 800 - 400, o->Position[2], Position);
					CreateEffect(MODEL_SKILL_BLAST, Position, o->Angle, o->Light);
				}
				if (c->AttackTime == 14)
				{
					for (int i = 0; i < 18; i++)
					{
						VectorCopy(o->Angle, Angle);
						Angle[2] += i * 20.f;
						CreateEffect(MODEL_STAFF + 8, o->Position, Angle, o->Light);
					}
				}
			}
		}
		else
		{
			if (c->AttackTime == 1)
			{
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light);
			}
		}
		break;
	case 66:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 1)
			{
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light, 1);
			}
		}
		break;
	case 54:
	case 57:
	case 151:
		if (c->AttackTime == 1)
		{
			Vector(60.f, -110.f, 0.f, p);
			b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
			CreateEffect(MODEL_ARROW_BOMB, o->Position, o->Angle, o->Light, 0, o);
			if (c->MonsterIndex == 57)
			{
				vec3_t Angle;
				VectorCopy(o->Angle, Angle);
				Angle[2] += 20.f;
				CreateEffect(MODEL_ARROW_BOMB, o->Position, Angle, o->Light, 0, o);
				Angle[2] -= 40.f;
				CreateEffect(MODEL_ARROW_BOMB, o->Position, Angle, o->Light, 0, o);
			}
		}
		break;
	case 53:
	case 58:
	case 59:
		if (c->AttackTime == 1)
		{
			CreateInferno(o->Position);
		}
		if (c->AttackTime == 14)
		{
			if (c->MonsterIndex == 59)
			{
				if ((c->Skill) == AT_SKILL_BOSS)
				{
					for (i = 0; i < 18; i++)
					{
						VectorCopy(o->Angle, Angle);
						Angle[2] += i * 20.f;
						CreateEffect(MODEL_STAFF + 8, o->Position, Angle, o->Light);
					}
				}
			}
		}
		break;
	case 49:
		if (c->AttackTime.residual_duration(5, 1))
		{
			b->TransformPosition(o->BoneTransform[63], p, Position, true);
			CreateEffect(BITMAP_BOSS_LASER + 1, Position, o->Angle, o->Light);
		}
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 1)
			{
				VectorCopy(o->Angle, Angle); Angle[2] += 20.f;
				VectorCopy(o->Position, p); p[2] += 50.f;
				Luminosity = (15 - (double)c->AttackTime) * 0.1f;
				Vector(Luminosity * 0.3f, Luminosity * 0.6f, Luminosity * 1.f, Light);

				for (int i = 0; i < 9; i++)
				{
					Angle[2] += 40.f;
					CreateEffect(BITMAP_BOSS_LASER, p, Angle, Light);
				}
			}
		}
		break;
	case 42:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 1)
			{
				Vector(0.f, 0.f, 0.f, p);
				b->TransformPosition(o->BoneTransform[11], p, Position, true);
				Vector(o->Angle[0] - 20.f, o->Angle[1], o->Angle[2] - 30.f, Angle);
				CreateEffect(MODEL_FIRE, Position, Angle, o->Light, 2);
				Vector(o->Angle[0] - 30.f, o->Angle[1], o->Angle[2], Angle);
				CreateEffect(MODEL_FIRE, Position, Angle, o->Light, 2);
				Vector(o->Angle[0] - 20.f, o->Angle[1], o->Angle[2] + 30.f, Angle);
				CreateEffect(MODEL_FIRE, Position, Angle, o->Light, 2);
				PlayBuffer(SOUND_METEORITE01);
			}
			Vector(o->Position[0] + rand() % 1024 - 512, o->Position[1] + rand() % 1024 - 512, o->Position[2], Position);
			CreateEffectSync(MODEL_FIRE, Position, o->Angle, o->Light);
			PlayBuffer(SOUND_METEORITE01);
		}
		break;
	case 35:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 1)
			{
				for (int i = 0; i < 18; i++)
				{
					Vector(0.f, 0.f, i * 20.f, Angle);
					CreateEffect(MODEL_FIRE, o->Position, Angle, o->Light, 1, o);
				}
				PlayBuffer(SOUND_METEORITE01);
			}
		}
		break;
	case 38:
	case 67:
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			if (c->AttackTime == 1)
			{
				CreateEffect(MODEL_CIRCLE, o->Position, o->Angle, o->Light);
				CreateEffect(MODEL_CIRCLE_LIGHT, o->Position, o->Angle, o->Light);
				PlayBuffer(SOUND_HELLFIRE);
			}
			Vector(o->Position[0] + rand() % 1024 - 512, o->Position[1] + rand() % 1024 - 512, o->Position[2], Position);
			CreateEffectSync(MODEL_FIRE, Position, o->Angle, o->Light);
			PlayBuffer(SOUND_METEORITE01);
		}
		break;
	case 103://함정
		if ((c->Skill) == AT_SKILL_BOSS)
		{
			Vector(o->Position[0] + rand() % 1024 - 512, o->Position[1] + rand() % 1024 - 512, o->Position[2], Position);
			CreateEffectSync(MODEL_FIRE, Position, o->Angle, o->Light);
			PlayBuffer(SOUND_METEORITE01);
		}
		break;
	case 45://물고기
		for (int i = 0; i < 4; i++)
		{
			Vector((float)(rand() % 32 - 16), (float)(rand() % 32 - 16), (float)(rand() % 32 - 16), p);
			b->TransformPosition(o->BoneTransform[2], p, Position, true);
			CreateParticleSync(BITMAP_BUBBLE, Position, o->Angle, Light);
			CreateParticleSync(BITMAP_BLOOD + 1, Position, o->Angle, Light);
		}
		break;
	default:
		break;
	}

	if (c->TargetCharacter >= 0 && c->TargetCharacter < MAX_CHARACTERS_CLIENT)
	{
		CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
		OBJECT* to = &tc->Object;

		if ((c->Skill) == AT_SKILL_ENERGYBALL)
		{
			switch (c->MonsterIndex)
			{
			case 163:
			case 165:
			case 167:
			case 169:
			case 171:
			case 173:
			case 427:
				if (c->Weapon[0].Type == MODEL_BOW + 19)
				{
					if (c->AttackTime == 8)
					{
						CreateArrows(c, o, o, 0, 0, 0);
					}
				}
				else if (c->Object.CurrentAction == MONSTER01_ATTACK1)
				{
					if (c->AttackTime == 15)
					{
						CalcAddPosition(o, -20.f, -90.f, 100.f, Position);
						CreateEffect(BITMAP_BOSS_LASER, Position, o->Angle, Light, 0, o);
					}
				}
				else if (c->Object.CurrentAction == MONSTER01_ATTACK2)
				{
					if (c->AttackTime == 8)
					{
						if (rand() % 2 == 0)
						{
							CreateEffect(MODEL_SKILL_BLAST, to->Position, o->Angle, o->Light, 0, o);
							CreateEffect(MODEL_SKILL_BLAST, to->Position, o->Angle, o->Light, 0, o);
						}
						else
						{
							Vector(0.8f, 0.5f, 0.1f, Light);
							CreateEffect(MODEL_FIRE, to->Position, o->Angle, Light, 6);
							CreateEffect(MODEL_FIRE, to->Position, o->Angle, Light, 6);
						}
					}
				}
				break;
			case 89:
			case 95:
			case 112:
			case 118:
			case 124:
			case 130:
			case 143:
				if (c->AttackTime == 14)
				{
					Vector(0.f, 0.f, 0.f, p);
					b->TransformPosition(o->BoneTransform[33], p, Position, true);
					VectorCopy(o->Angle, Angle);
					CreateEffect(MODEL_PIERCING + 1, Position, Angle, Light, 1);
					CreateJoint(BITMAP_JOINT_THUNDER, Position, Position, Angle, 2, to, 50.f);
				}
				break;

			case 87:
			case 93:
			case 99:
			case 116:
			case 122:
			case 128:
			case 141:
				if (c->AttackTime == (13))
				{
					Vector(1.0f, 1.0f, 1.0f, Light);
					Vector(60.f, 30.f, 0.f, p);
					b->TransformPosition(o->BoneTransform[6], p, Position, true);

					Vector(o->Angle[0], o->Angle[1], o->Angle[2], Angle);
					CreateEffect(MODEL_FIRE, Position, Angle, o->Light, 5);
				}
				break;

			case 77:
				if (c->AttackTime == 14)
				{
					Vector(0.f, 0.f, 0.f, p);
					b->TransformPosition(g_fBoneSave[2], p, Position, true);
					VectorCopy(o->Angle, Angle);
					CreateEffect(MODEL_PIERCING + 1, Position, Angle, Light, 1);
					CreateJoint(BITMAP_JOINT_THUNDER, Position, Position, Angle, 2, to, 50.f);
				}
				break;
			case 73:
			case 75:
				if (c->Object.CurrentAction == MONSTER01_ATTACK2)
				{
					if (c->AttackTime == 13)
					{
						Vector(1.f, 0.5f, 0.f, Light);
						Vector(-50.f, 100.f, 0.f, p);
						VectorCopy(o->Angle, Angle);
						Angle[0] += 45.f;
						b->TransformPosition(o->BoneTransform[11], p, Position, true);
						CreateEffect(MODEL_PIERCING + 1, Position, Angle, Light, 1);
						CreateJoint(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 2, to, 50.f);
					}
				}
				break;
			case 69:
				if (c->AttackTime == 1)
				{
					for (int i = 0; i < 4; ++i)
					{
						CreateJoint(BITMAP_FLARE, o->Position, o->Position, Angle, 7, to, 50.f);
						CreateJoint(BITMAP_FLARE, Position, Position, Angle, 7, to, 50.f);
					}
				}
				break;
			case 61:
				for (int i = 0; i < 6; i++)
				{
					int Hand = 0;
					if (i >= 3)
						Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);//에러
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 2, to, 50.f);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 2, to, 10.f);
				}

				if (c->AttackTime == 1)
					PlayBuffer(SOUND_EVIL);

				for (int i = 0; i < 4; i++)
				{
					int Hand = 0;
					if (i >= 2)
						Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_LASER + 1, Position, to->Position, Angle, 0, to, 50.f);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, o->Light);
				}
				break;
			case 46:
				if (c->AttackTime == 1)
					PlayBuffer(SOUND_EVIL);

				for (int i = 0; i < 4; i++)
				{
					int Hand = 0;
					if (i >= 2)
						Hand = 1;
					Vector(0.f, 0.f, 0.f, Angle);
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);
					CreateJointSync(BITMAP_BLUR + 1, Position, to->Position, Angle, 1, to, 50.f);
					CreateJointSync(BITMAP_BLUR + 1, Position, to->Position, Angle, 1, to, 10.f);
				}
				break;
			case 37:
				if (c->AttackTime == 1)
					PlayBuffer(SOUND_EVIL);

				for (int i = 0; i < 4; i++)
				{
					int Hand = 0;
					if (i >= 2) Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_LASER + 1, Position, to->Position, Angle, 0, to, 50.f);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, o->Light);
				}
				break;
			case 66:
			{
				if (c->AttackTime == 1)
					PlayBuffer(SOUND_THUNDER01);
				float fAngle = (float)(45.f - ((int)c->AttackTime * 3 + (int)WorldTime / 10) % 90) + 180.f;

				for (int i = 0; i < 4; i++)
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[i % 2].LinkBone], p, Position, true);
					Vector(0.f, 0.f, fAngle, Angle);
					CreateJointSync(BITMAP_JOINT_LASER + 1, Position, to->Position, Angle, 1, to, 50.f);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, o->Light);
					fAngle += 270.f;
				}
			}
			break;

			default:
				break;
			}
		}
		else if ((c->Skill) == AT_SKILL_THUNDER)
		{
			switch (c->MonsterIndex)
			{
			case 89:
			case 95:
			case 112:
			case 118:
			case 124:
			case 130:
			case 143:
			{
				if (c->AttackTime == 1)
					PlayBuffer(SOUND_THUNDER01);

				float fAngle = (float)(45.f - ((int)c->AttackTime * 3 + (int)WorldTime / 10) % 90) + 180.f;

				for (int i = 0; i < 4; i++)
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[i % 2].LinkBone], p, Position, true);
					Vector(0.f, 0.f, fAngle, Angle);
					CreateJointSync(BITMAP_JOINT_LASER + 1, Position, to->Position, Angle, 1, to, 50.f);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, o->Light);
					fAngle += 270.f;
				}
			}
			break;

			case 77://불사조공격
				if (c->AttackTime > 8)
				{
					Vector(0.f, 0.f, 0.f, p);
					b->TransformPosition(g_fBoneSave[0], p, Position, true);
					for (int i = 0; i < 4; ++i)
					{
						Vector(0.f, 0.f, (float)(rand() % 360), Angle);
						CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 80.f);
					}
				}
				break;
			case 37://데빌
				if (c->AttackTime == 1)
					PlayBuffer(SOUND_EVIL);

				for (int i = 0; i < 4; i++)
				{
					int Hand = 0;
					if (i >= 2) Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_LASER + 1, Position, to->Position, Angle, 0, to, 50.f);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, o->Light);
				}
				break;
			case 34:

				for (int i = 0; i < 4; i++)
				{
					int Hand = 0;
					if (i >= 2) Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 50.f);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 10.f);
					CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				}
				break;
			case 48://리자드킹
				for (int i = 0; i < 6; i++)
				{
					int Hand = 0;
					if (i >= 3)
						Hand = 1;
					b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], p, Position, true);//에러
					Vector(0.f, 0.f, (float)(rand() % 360), Angle);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 2, to, 50.f);
					CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 2, to, 10.f);
				}
				break;
			case 39:
				if (o->Type == MODEL_PLAYER)
				{
					Vector(0.f, 0.f, 0.f, p);
				}
				else
				{
					Vector(0.f, -130.f, 0.f, p);
				}
				b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
				Vector(-60.f, 0.f, o->Angle[2], Angle);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 50.f);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 10.f);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				break;
			case 386:
			case 389:
			case 392:
			case 395:
			case 398:
			case 401:
			{
				Vector(8.f, 0.f, 0.f, Light);
				b->TransformPosition(o->BoneTransform[17], p, Position, true);
				Vector(-60.f, 0.f, o->Angle[2], Angle);
				//CreateJoint(BITMAP_JOINT_THUNDER,Position,to->Position,Angle,0,to,50.f);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 21, to, 50.f);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);

				b->TransformPosition(o->BoneTransform[41], p, Position, true);
				Vector(-60.f, 0.f, o->Angle[2], Angle);
				//CreateJoint(BITMAP_JOINT_THUNDER,Position,to->Position,Angle,0,to,50.f);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 21, to, 50.f);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
			}
			break;
			// 플레이어 이거나 기타 몬스터가 전기(번개)를 사용했을시
			default:
				if (b->NumBones < c->Weapon[0].LinkBone)
					break;

				if (o->Type == MODEL_PLAYER)
				{
					Vector(0.f, 0.f, 0.f, p);
				}
				else
				{
					Vector(0.f, -130.f, 0.f, p);
				}
				b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
				Vector(-60.f, 0.f, o->Angle[2], Angle);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 50.f);
				CreateJointSync(BITMAP_JOINT_THUNDER, Position, to->Position, Angle, 0, to, 10.f);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				break;
			}
		}
		else
		{
		}
	}
}

bool CheckMonsterSkill(CHARACTER* pCharacter, OBJECT* pObject)
{
	if (pCharacter->MonsterIndex == 364)
	{
		M39Kanturu3rd::MayaSceneMayaAction(pCharacter->MonsterSkill);
		return true;
	}

	int iCheckAttackAni = -1;

	for (int i = 0; i < MAX_MONSTERSKILL_NUM; i++)
	{
		if (MonsterSkill[pCharacter->MonsterIndex].Skill_Num[i] == pCharacter->MonsterSkill)
		{
			iCheckAttackAni = i;
			break;
		}
		else
		{
			iCheckAttackAni = -1;
		}
	}

	switch (iCheckAttackAni)
	{
	case 0:
		SetAction(pObject, MONSTER01_ATTACK1);
		break;
	case 1:
		SetAction(pObject, MONSTER01_ATTACK2);
		break;
	case 2:
		SetAction(pObject, MONSTER01_ATTACK3);
		break;
	case 3:
		SetAction(pObject, MONSTER01_ATTACK4);
		break;
	case 4:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	case 5:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	case 6:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	case 7:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	case 8:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	case 9:
		SetAction(pObject, MONSTER01_ATTACK5);
		break;
	}

	pCharacter->MonsterSkill = -1;


	if (iCheckAttackAni < 0)
	{
		SetAction(pObject, MONSTER01_ATTACK1);
	}

	if (iCheckAttackAni > 4)
		return false;

	return true;
}

bool CharacterAnimation(CHARACTER* c, OBJECT* o)
{
	bool Play = false;
	BMD* b = gmClientModels->GetModel(o->Type);

	if (b)
	{
		float PlaySpeed = 0.f;
		if (b->NumActions > 0)
		{
			PlaySpeed = b->Actions[b->CurrentAction].PlaySpeed;

			if (PlaySpeed < 0.f)
				PlaySpeed = 0.f;
			if (c->Change && o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
				PlaySpeed *= timepow(1.5f);
			if (o->CurrentAction == PLAYER_SKILL_VITALITY && o->AnimationFrame > 6.f)
			{
				PlaySpeed /= timepow(2.f);
			}
			else if ((o->CurrentAction == PLAYER_HIT_TELEPORT || o->CurrentAction == PLAYER_HIT_RIDE_TELEPORT
				|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT
				) && o->AnimationFrame > 5.5f)
			{
				PlaySpeed /= timepow(10.f);
			}
			else if (c->GetBaseClass() == CLASS_DARK_LORD &&
				(o->CurrentAction == PLAYER_SKILL_FLASH || o->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH
					|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_FLASH
					) && (o->AnimationFrame > 1.f && o->AnimationFrame < 3.f)
				)
			{
				if (g_pPartyManager->IsPartyMemberChar(c) == false)
				{
					PlaySpeed /= timepow(2.f);
				}
				else
				{
					PlaySpeed /= timepow(8.f);
				}
			}
			if (o->CurrentAction == PLAYER_SKILL_HELL_BEGIN)
			{
				PlaySpeed /= timepow(2.f);
			}
			if (o->Type != MODEL_PLAYER)
			{
				switch (o->Type)
				{
				case MODEL_MONSTER01 + 64:
					if (o->CurrentAction == MONSTER01_DIE && o->AnimationFrame > 6)
						PlaySpeed *= timepow(4.0f);
					break;
				case MODEL_FACE:
				case MODEL_FACE + 1:
				case MODEL_FACE + 2:
				case MODEL_FACE + 3:
				case MODEL_FACE + 4:
				case MODEL_FACE + 5:
				case MODEL_FACE + 6:
					PlaySpeed *= timepow(2.0f);
					break;
				}
			}
			if (o->Type == MODEL_MONSTER01 + 87)
			{
				if (o->CurrentAction == MONSTER01_DIE)
					PlaySpeed /= timepow(2.f);
			}
		}

		if (g_isCharacterBuff(o, eDeBuff_Stun) || g_isCharacterBuff(o, eDeBuff_Sleep))
		{
			return false;
		}

		Play = b->PlayAnimation(&o->AnimationFrame, &o->PriorAnimationFrame, &o->PriorAction, PlaySpeed, o->Position, o->Angle);

		if (o->CurrentAction == PLAYER_CHANGE_UP)
		{
			if (Play == false)
				SetPlayerStop(c);
		}

		if (o->CurrentAction == PLAYER_RECOVER_SKILL)
		{
			if (Play == false)
				SetPlayerStop(c);
		}
	}
	return Play;
}

int GetHandOfWeapon(OBJECT* o)
{
	int Hand = 0;
	if (o->Type == MODEL_PLAYER)
	{
		if (o->CurrentAction == PLAYER_HIT_SWORD_LEFT1 || o->CurrentAction == PLAYER_HIT_SWORD_LEFT2)
			Hand = 1;
	}

	return (Hand);
}

bool AttackStage(CHARACTER* c, OBJECT* o)
{
	int Hand = GetHandOfWeapon(o);

	int iSkill = (c->Skill);

	if (c == Hero || c->Object.Kind == KIND_PLAYER)
		g_iLimitAttackTime = GMProtect->GetAttackTimeForClass(c->GetBaseClass());
	else
		g_iLimitAttackTime = 15;

	switch (iSkill)
	{
	case Skill_Death_Stab:
	case Skill_Death_Stab_Strengthener:
	case Skill_Death_Stab_Proficiency:
	case Skill_Death_Stab_Mastery:
	{
		BMD* pModel = gmClientModels->GetModel(o->Type);

		if (pModel)
		{
			if (pModel->Bones[c->Weapon[Hand].LinkBone].Dummy || c->Weapon[Hand].LinkBone >= pModel->NumBones)
			{
				break;
			}

			if (c->AttackTime == 8)
			{
				if (SceneFlag != LOG_IN_SCENE)
					PlayBuffer(SOUND_SKILL_SWORD2);
			}

			if (c->AttackTime > 2 && c->AttackTime <= 8)
			{
				for (int j = 0; j < 3; ++j)
				{
					vec3_t CurPos, TempPos;
					VectorCopy(o->Position, CurPos);
					CurPos[2] += 120.0f;

					GetNearRandomPos(CurPos, 300, TempPos);
					float fDistance = 1400.0f;
					TempPos[0] += (-fDistance * sinf(o->Angle[2] * Q_PI / 180.0f));
					TempPos[1] += (fDistance * cosf(o->Angle[2] * Q_PI / 180.0f));
					CreateJointSync(MODEL_SPEARSKILL, TempPos, TempPos, o->Angle, 2, o, 40.0f);
				}
			}
			if (c->AttackTime <= 8)
			{
				float fDistance = 300.0f;
				vec4_t* mtBone = o->GetBoneTransform(c->Weapon[Hand].LinkBone);

				if (mtBone)
				{
					vec3_t Position2 = { 0.0f, 0.0f, 0.0f };
					pModel->TransformPosition(mtBone, Position2, o->m_vPosSword, true);
					o->m_vPosSword[0] += timefac(fDistance * sinf(o->Angle[2] * Q_PI / 180.0f));
					o->m_vPosSword[1] += timefac(-fDistance * cosf(o->Angle[2] * Q_PI / 180.0f));
				}
			}
			if (c->AttackTime > 6 && c->AttackTime <= 12)
			{
				vec3_t Position;
				vec3_t Position2 = { 0.0f, 0.0f, 0.0f };

				vec4_t* mtBone = o->GetBoneTransform(c->Weapon[Hand].LinkBone);

				if (mtBone)
				{
					pModel->TransformPosition(mtBone, Position2, Position, true);

					float fDistance = 100.0f + (float)((double)c->AttackTime - 8.f) * 10.0f;
					Position[0] += timeNormalizer(fDistance * sinf(o->Angle[2] * Q_PI / 180.0f));
					Position[1] += timeNormalizer(-fDistance * cosf(o->Angle[2] * Q_PI / 180.0f));

					vec3_t Light = { 1.0f, 1.0f, 1.0f };
					CreateEffectSync(MODEL_SPEAR, Position, o->Angle, Light, 1, o);
					CreateEffectSync(MODEL_SPEAR, Position, o->Angle, Light, 1, o);

					CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);

					if (tc)
					{
						OBJECT* to = &tc->Object;
						if (c->AttackTime > 10 && to->Live)
						{
							to->m_byHurtByOneToOne = 35;
						}
					}
				}
			}
			if (c->AttackTime >= 12)
			{
				c->AttackTime = (g_iLimitAttackTime);
			}
		}
	}
	break;
	case Skill_Impale:
	{
		BMD* b = gmClientModels->GetModel(o->Type);

		if (b)
		{
			vec3_t p;
			if (c->AttackTime == 10)
			{
				PlayBuffer(SOUND_RIDINGSPEAR);
			}
			else if (c->AttackTime == 4)
			{
				vec3_t Light = { 1.0f, 1.0f, .5f };
				vec3_t Position2 = { 0.0f, 0.0f, 0.0f };
				b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], Position2, p, true);
				CreateEffect(MODEL_SPEAR + 1, p, o->Angle, Light, c->Weapon[Hand].Type, o);
			}
			else if (c->AttackTime == 8)
			{
				vec3_t Position;
				memcpy(Position, o->Position, sizeof(vec3_t));
				Position[0] += 50.0f * sinf(o->Angle[2] * Q_PI / 180.0f);
				Position[1] += -50.0f * cosf(o->Angle[2] * Q_PI / 180.0f);
				Position[2] += 110.0f;
				vec3_t Light = { 1.0f, 1.0f, 1.0f };
				CreateEffect(MODEL_SPEAR, Position, o->Angle, Light, 0, o);
				CreateEffect(MODEL_SPEAR, Position, o->Angle, Light, 0, o);
			}
			if (c->AttackTime > 13 && c->AttackTime <= 14)
			{
				for (int i = 0; i < 3; ++i)
				{
					vec3_t Position;
					memcpy(Position, o->Position, sizeof(vec3_t));
					Position[0] += 145.0f * sinf(o->Angle[2] * Q_PI / 180.0f);
					Position[1] += -145.0f * cosf(o->Angle[2] * Q_PI / 180.0f);
					Position[2] += 110.0f;

					vec3_t Light = { .3f, .3f, .3f };
					if (!(c->AttackTime == 11))
					{
						Position[0] += (rand() % 60 - 30);
						Position[1] += (rand() % 60 - 30);
						Position[2] += (rand() % 60 - 30);
					}
					CreateEffectSync(MODEL_SPEARSKILL, Position, o->Angle, Light, c->Weapon[Hand].Type, o);
				}
			}
		}
	}
	break;
	case AT_SKILL_PIERCING:
		if (o->Type == MODEL_PLAYER && o->CurrentAction >= PLAYER_HIT_FIST && o->CurrentAction <= PLAYER_RIDE_SKILL)
		{
			if (o->AnimationFrame >= 5.f)
			{
				o->PriorAnimationFrame = 4.f;
				o->AnimationFrame = 5.f;
			}
		}

		if (c->AttackTime == 3)
		{
			CreateEffect(BITMAP_GATHERING, o->Position, o->Angle, o->Light, 0, o);
			PlayBuffer(SOUND_PIERCING, o);
		}
		c->AttackTime = 15;
		break;
	case Skill_Fire_Slash_Strengthener:
	case Skill_Fire_Slash_Mastery:
	case Skill_Fire_Slash:
		if (o->Type == MODEL_PLAYER)
		{
			SetAction(o, PLAYER_HIT_SKILL_WHEEL);

			if (c->AttackTime >= 1 && c->AttackTime <= 2)
			{
				vec3_t Angle;
				Vector(1.f, 0.f, 0.f, Angle);
				CreateEffectSync(BITMAP_GATHERING, o->Position, o->Angle, o->Light, 1, o);
			}
			if (o->AnimationFrame >= 3.f)
			{
				o->PKKey = getTargetCharacterKey(c, SelectedCharacter, 0);

				PlayBuffer(SOUND_SKILL_SWORD3);

				if (SkillAttribute[iSkill].Distance == 3)
				{
					CreateEffect(BITMAP_SWORD_FORCE, o->Position, o->Angle, o->Light, 1, o, o->PKKey, FindHotKey(iSkill));
				}
				else
				{
					CreateEffect(BITMAP_SWORD_FORCE, o->Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey(iSkill));
				}

				float AttackSpeed1 = CharacterAttribute->AttackSpeed * 0.004f;
				gmClientModels->GetPlayerModelAction(PLAYER_HIT_SKILL_WHEEL)->PlaySpeed = 0.54f + AttackSpeed1;
				c->AttackTime = 15;
			}
		}
		g_iLimitAttackTime = 15;
		break;
	case Skill_Power_Slash_Strengthener:
	case AT_SKILL_ICE_BLADE:
		if (o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD_TWO)
		{
			vec3_t Angle;
			VectorCopy(o->Angle, Angle);

			Angle[2] -= 40.f;
			CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light, 2, o);
			Angle[2] += 20.f;
			CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light, 2, o);
			Angle[2] += 20.f;
			CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light, 2, o);
			Angle[2] += 20.f;
			CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light, 2, o);
			Angle[2] += 20.f;
			CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light, 2, o);

			PlayBuffer(SOUND_SKILL_SWORD3);

			c->AttackTime = 15;
		}
		g_iLimitAttackTime = 15;
		break;
	case AT_SKILL_BLAST_HELL:
	case Skill_Nova_Strengthener:
		if (o->AnimationFrame >= 14.f && o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_SKILL_HELL_START)
		{
			c->AttackTime = 15;
		}
		break;
	case Skill_Swell_Life_Strengthener:
	case Skill_Swell_Life_Proficiency:
	case Skill_Swell_Life_Mastery:
	case AT_SKILL_VITALITY:
		if (c->AttackTime > 9 && o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_SKILL_VITALITY)
		{
			c->AttackTime = 15;
		}
		break;
	case    AT_SKILL_IMPROVE_AG:
		if (o->AnimationFrame >= 5.f && ((o->Type == MODEL_PLAYER && o->CurrentAction >= PLAYER_HIT_FIST && o->CurrentAction <= PLAYER_RIDE_SKILL) ||
			((o->Type >= MODEL_MONSTER01 && o->Type < MODEL_MONSTER_END) && o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)))
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_STRONG_PIER:
		if (o->AnimationFrame >= 3.f && o->Type == MODEL_PLAYER && (o->CurrentAction == PLAYER_HIT_STRIKE || o->CurrentAction == PLAYER_HIT_RIDE_STRIKE
			|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE))
		{
			c->AttackTime = 15;
		}
		break;
	case Skill_Fire_Burst_Strengthener:
	case Skill_Fire_Burst_Mastery:
	case AT_SKILL_LONGPIER_ATTACK:
		if (o->AnimationFrame >= 3.f && o->Type == MODEL_PLAYER && (o->CurrentAction == PLAYER_HIT_STRIKE || o->CurrentAction == PLAYER_HIT_RIDE_STRIKE
			|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE))
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_THUNDER_STRIKE:
		if (o->AnimationFrame >= 5.5f && o->Type == MODEL_PLAYER && (o->CurrentAction == PLAYER_SKILL_FLASH || o->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH
			|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_FLASH))
		{
			c->AttackTime = 15;
		}
		else
		{
			c->AttackTime = (10);
		}
		break;
	case Skill_Earthshake_Strengthener:
	case Skill_Earthshake_Mastery:
	case AT_SKILL_DARK_HORSE:
		if (o->AnimationFrame >= 5.f && o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_HIT_DARKHORSE)
		{
			c->AttackTime = 15;
		}
		break;
	case    AT_SKILL_PARTY_TELEPORT:
		if (c->AttackTime > 5 && o->Type == MODEL_PLAYER && (o->CurrentAction == PLAYER_HIT_TELEPORT || o->CurrentAction == PLAYER_HIT_RIDE_TELEPORT
			|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT))
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_RIDER:
		if (o->AnimationFrame >= 5.f && o->Type == MODEL_PLAYER && (o->CurrentAction == PLAYER_SKILL_RIDER || o->CurrentAction == PLAYER_SKILL_RIDER_FLY))
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_STUN:
	case AT_SKILL_MANA:
		break;
	case AT_SKILL_INVISIBLE:
		c->AttackTime = 15;
		break;
	case AT_SKILL_REMOVAL_STUN:
	case AT_SKILL_REMOVAL_INVISIBLE:
		c->AttackTime = 15;
		break;
	case AT_SKILL_REMOVAL_BUFF:
		if (o->AnimationFrame >= 3.5f)
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_RUSH:
	case AT_SKILL_OCCUPY:
		if (o->AnimationFrame > 5.f)
		{
			c->AttackTime = 15;
		}
		{
			vec3_t Position;
			vec3_t	Angle;

			VectorCopy(o->Position, Position);
			Position[0] += rand() % 30 - 15.f;
			Position[1] += rand() % 30 - 15.f;
			Position[2] += 20.f;
			for (int i = 0; i < 4; i++)
			{
				Vector((float)(rand() % 60 + 60 + 90), 0.f, o->Angle[2], Angle);
				CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, Angle);

				if (iSkill == AT_SKILL_OCCUPY)
				{
					CreateParticleSync(BITMAP_FIRE, Position, Angle, o->Light, 18, 1.5f);
				}
				else
				{
					CreateParticleSync(BITMAP_FIRE, Position, Angle, o->Light, 2, 1.5f);
				}
			}
		}
		break;
	case AT_SKILL_ONEFLASH:
		if (o->AnimationFrame > 5.f)
		{
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 23, NULL, 40.f, 0);
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 23, NULL, 40.f, 1);
			CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 23, NULL, 40.f, 4);
			c->AttackTime = 15;

			PlayBuffer(SOUND_BCS_ONE_FLASH);
		}
		else if (o->AnimationFrame > 2.3f && o->AnimationFrame < 2.6f)
		{
			CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 23, NULL, 40.f, 2);
			CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 23, NULL, 40.f, 3);
			PlayBuffer(SOUND_BCS_ONE_FLASH);
		}
		g_iLimitAttackTime = 15;
		break;
	case Skill_Fire_Blast:
		if (o->AnimationFrame >= 3.f
			&& o->Type == MODEL_PLAYER
			&& (o->CurrentAction == PLAYER_HIT_STRIKE || o->CurrentAction == PLAYER_HIT_RIDE_STRIKE || o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE)
			)
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_DEATH_CANNON:
		if (o->AnimationFrame >= 3.f && o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_HIT_DEATH_CANNON)
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_FLAME_STRIKE:
	{
		c->AttackTime = 15;
	}
	break;
	case AT_SKILL_GIGANTIC_STORM:
		if (o->AnimationFrame > 7.f)
		{
			c->AttackTime = 15;
		}
		break;
	case AT_SKILL_LIGHTNING_SHOCK:
	case AT_SKILL_BLOW_OF_DESTRUCTION:
	case Skill_Strike_of_Destruction_Strengthe:
	case Skill_Strike_of_Destruction_Proficien:
	case Skill_Strike_of_Destruction_Mastery:
	case Skill_Lightning_Shock_Strengthener:
	{
		c->AttackTime = 15;
	}
	break;
	case AT_SKILL_ATT_UP_OURFORCES:
	case AT_SKILL_HP_UP_OURFORCES:
	case AT_SKILL_DEF_UP_OURFORCES:
	{
		c->AttackTime = 15;
	}
	break;
	case AT_SKILL_DRAGON_KICK:
	{
		c->AttackTime = 1;
	}
	break;
	case AT_SKILL_GIANTSWING:
	case AT_SKILL_STAMP:
	{
		o->m_sTargetIndex = c->TargetCharacter;
	}
	break;
	default:
		if (o->AnimationFrame >= 1.f && o->Type == MODEL_PLAYER && o->CurrentAction == PLAYER_HIT_SKILL_FURY_STRIKE)
		{
			c->AttackTime = 15;
		}
		else if (o->AnimationFrame >= 5.f && ((o->Type == MODEL_PLAYER && o->CurrentAction >= PLAYER_HIT_FIST && o->CurrentAction <= PLAYER_RIDE_SKILL) ||
			((o->Type >= MODEL_MONSTER01 && o->Type < MODEL_MONSTER_END) && o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)))
		{
			int RightType = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;
			int LeftType = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;

			if (c->AttackTime >= 1 && LeftType == ITEM_BOW + 21 && o->Type == MODEL_PLAYER)
			{
				for (int i = 0; i < 18; i++)
				{
					CreateParticle(BITMAP_SPARK + 1, o->Position, o->Angle, o->Light, 12, 2.0f);
				}
			}
			c->AttackTime = 15;
		}
		break;
	}
	return true;
}

void DeleteCloth(CHARACTER* c, OBJECT* o, PART_t* p2)
{
	if (o && o->m_pCloth)
	{
		CPhysicsCloth* pCloth = (CPhysicsCloth*)o->m_pCloth;
		for (int i = 0; i < o->m_byNumCloth; ++i)
		{
			pCloth[i].Destroy();
		}
		delete[] pCloth;
		o->m_pCloth = NULL;
		o->m_byNumCloth = 0;
	}

	if (c)
	{
		for (int i = 0; i < MAX_BODYPART; i++)
		{
			PART_t* p = &c->BodyPart[i];
			if (p->m_pCloth[0])
			{
				for (int i = 0; i < p->m_byNumCloth; ++i)
				{
					CPhysicsCloth* pCloth = (CPhysicsCloth*)p->m_pCloth[i];
					pCloth->Destroy();
					delete pCloth;
					p->m_pCloth[i] = NULL;
				}
			}
		}
	}

	if (p2)
	{
		if (p2->m_pCloth[0])
		{
			for (int i = 0; i < p2->m_byNumCloth; ++i)
			{
				CPhysicsCloth* pCloth = (CPhysicsCloth*)p2->m_pCloth[i];
				pCloth->Destroy();
				delete pCloth;
				p2->m_pCloth[i] = NULL;
			}
		}
	}
}

void FallingCharacter(CHARACTER* c, OBJECT* o)
{
	float Matrix[3][4];
	vec3_t  Position, p;

	Vector(0.f, 0.f, 0.f, Position);
	Vector(0.f, o->Direction[1], 0.f, p);
	AngleMatrix(o->m_vDownAngle, Matrix);
	VectorRotate(p, Matrix, Position);

	o->Direction[1] += timefac(o->Direction[0]);
	o->Gravity += timefac(o->Velocity);
	o->Velocity -= timefac(o->Direction[2]);
	o->Angle[0] -= timefac(5.f);

	o->Position[0] = o->m_vDeadPosition[0] + Position[0];
	o->Position[1] = o->m_vDeadPosition[1] + Position[1];
	o->Position[2] = o->m_vDeadPosition[2] + o->Gravity;
}

void PushingCharacter(CHARACTER* c, OBJECT* o)
{
	if (c->StormTime > 0)
	{
		o->Angle[2] += timefac(c->StormTime * 10);
		c->StormTime--;
	}
	if (c->JumpTime > 0)
	{
		float Speed = 0.2f;
		if (o->Type == MODEL_MONSTER01 + 52)
		{
			Speed = 0.07f;
		}
		if (gMapManager->InChaosCastle() == true)
		{
			o->Position[0] += timefac((((float)c->TargetX) * TERRAIN_SCALE - o->Position[0]) * Speed);
			o->Position[1] += timefac((((float)c->TargetY) * TERRAIN_SCALE - o->Position[1]) * Speed);
			c->JumpTime++;
			if (c->JumpTime > 15)
			{
				SetPlayerStop(c);

				o->Position[0] = c->TargetX * TERRAIN_SCALE;
				o->Position[1] = c->TargetY * TERRAIN_SCALE;

				c->PositionX = c->TargetX;
				c->PositionY = c->TargetY;

				c->JumpTime = 0;
			}
		}
		else
		{
			o->Position[0] += timefac((((float)c->TargetX + 0.5f) * TERRAIN_SCALE - o->Position[0]) * Speed);
			o->Position[1] += timefac((((float)c->TargetY + 0.5f) * TERRAIN_SCALE - o->Position[1]) * Speed);

			if (o->Type != MODEL_BALL)
				o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);

			if (c->JumpTime++ > 15)
			{
				if (o->Type == MODEL_MONSTER01 + 52)
					SetPlayerStop(c);

				c->JumpTime = 0;
			}
		}
	}
	if (o->Type == MODEL_BALL)
	{
		o->Position[2] += timefac(o->Gravity);
		o->Gravity -= timefac(6.f);
		float Height = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
		if (o->Position[2] < Height)
		{
			o->Position[2] = Height;
			o->Gravity = -o->Gravity * 0.4f;
		}
		VectorAdd(o->Angle, o->Direction, o->Angle);
		VectorScale(o->Direction, timepow(0.8f), o->Direction);
	}
}

void DeadCharacterBuff(OBJECT* o)
{
	g_CharacterUnRegisterBuff(o, eDeBuff_Stun);
	g_CharacterUnRegisterBuff(o, eBuff_Cloaking);
	g_CharacterUnRegisterBuff(o, eDeBuff_Harden);
	g_CharacterUnRegisterBuff(o, eDeBuff_Sleep);
}

void DeadCharacter(CHARACTER* c, OBJECT* o, BMD* b)
{
	if (c->Dead <= 0)
		return;

	DeadCharacterBuff(o);

	if (o->Type == MODEL_MONSTER01 + 32 && World == WD_7ATLANSE)
	{
		c->Rot += timefac(0.05f);
	}
	else if (o->Type == MODEL_MONSTER01 + 64)
	{
		if (o->LifeTime >= 100)
			c->Rot += timefac(0.01f);
		else
			c->Rot += timefac(0.02f);
	}
	else
		c->Rot += timefac(0.02f);

	float RotTime = 1.f;
	if (c->Rot >= RotTime)
	{
		if (o->Type != MODEL_MONSTER01 + 116)
		{
			o->Alpha = 1.f - (c->Rot - RotTime);
		}

		if (o->Alpha >= 0.01f)
		{
			o->Position[2] -= timefac(0.4f);
		}
		else if (c != Hero)
		{
			o->Live = false;
			c->m_byDieType = 0;
			o->m_bActionStart = false;
			o->m_bySkillCount = 0;

			BoneManager::UnregisterBone(c);
		}
		DeleteCloth(c, o);
	}

	if (gMapManager->InBloodCastle() == true && o->m_bActionStart)
	{
		FallingCharacter(c, o);
	}
	else if (gMapManager->InChaosCastle() == true)
	{
		int startDeadTime = 25;
		if (o->m_bActionStart)
		{
			FallingCharacter(c, o);
			startDeadTime = 15;
		}
		if (c->Dead <= startDeadTime && c->Dead >= startDeadTime - 10 && (c->Dead % 2))
		{
			vec3_t Position;

			VectorCopy(o->Position, Position);

			Position[0] += rand() % 160 - 80.f;
			Position[1] += rand() % 160 - 80.f;
			Position[2] += rand() % 160 - 80.f + 50.f;
			CreateBomb(Position, true);
		}
	}
	else
	{
		switch (c->m_byDieType)
		{
		case AT_SKILL_BLAST_HELL:
		case AT_SKILL_COMBO:
			if (c->Dead < 15)
			{
				o->Direction[1] -= timefac(o->Velocity);
				o->Velocity += timefac(1.f);

				if (o->Direction[1] < 0)
				{
					o->Direction[1] = 0.f;
				}
				MoveParticle(o, o->HeadAngle);
			}

			if (c->Dead <= 30 && c->m_byDieType == AT_SKILL_BLAST_HELL)
			{
				vec3_t Light, p, Position;
				Vector(0.3f, 0.3f, 1.f, Light);
				Vector(0.f, 0.f, 0.f, p);
				for (int i = 0; i < 10; i++)
				{
					b->TransformPosition(o->BoneTransform[rand() % 32], p, Position, true);
					CreateParticleSync(BITMAP_LIGHT, Position, o->Angle, Light, 5, 0.5f + (rand() % 100) / 50.f);
				}
			}
			break;
		}
	}
	if (SceneFlag == MAIN_SCENE && (World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3))
	{
		for (int i = 0; i < 4; i++)
		{
			vec3_t Position;
			Vector((float)(rand() % 128 - 64), (float)(rand() % 128 - 64), (float)(rand() % 256), Position);
			VectorAdd(Position, o->Position, Position);
			CreateParticleSync(BITMAP_BUBBLE, Position, o->Angle, o->Light);
		}
	}
}

void CalcStopTime(void)
{
	if (gMapManager->InChaosCastle() == false) return;

	if ((Hero->PositionX) != g_iOldPositionX || (Hero->PositionY) != g_iOldPositionY)
	{
		g_iOldPositionX = (Hero->PositionX);
		g_iOldPositionY = (Hero->PositionY);

		g_fStopTime = WorldTime;
		return;
	}

	float fStopTime = ((WorldTime - g_fStopTime) / CLOCKS_PER_SEC);
	if (fStopTime >= 10)
	{
		int index = TERRAIN_INDEX_REPEAT(g_iOldPositionX, g_iOldPositionY);

		if ((TerrainWall[index] & TW_NOGROUND) == TW_NOGROUND)
		{
			SendRequestCheckPosition(g_iOldPositionX, g_iOldPositionY);
		}
		g_fStopTime = WorldTime;
	}
}

void HeroAttributeCalc(CHARACTER* c)
{
	if (c != Hero) return;

	if (CharacterAttribute->AbilityTime[0] > 0)
	{
		CharacterAttribute->AbilityTime[0]--;
	}
	if (CharacterAttribute->AbilityTime[0] == 0)
	{
		CharacterAttribute->Ability &= (~ABILITY_FAST_ATTACK_SPEED);
		CharacterMachine->CalculateAttackSpeed();
	}
	if (CharacterAttribute->AbilityTime[1] > 0)
	{
		CharacterAttribute->AbilityTime[1]--;
	}
	if (CharacterAttribute->AbilityTime[1] == 0)
	{
		CharacterAttribute->Ability &= (~ABILITY_PLUS_DAMAGE);
		g_csItemOption.ClearListOnOff();
		CharacterMachine->CalculateDamage();
		CharacterMachine->CalculateMagicDamage();
		CharacterMachine->CalculateCurseDamage();
	}
	if (CharacterAttribute->AbilityTime[2] > 0)
	{
		CharacterAttribute->AbilityTime[2]--;
	}
	if (CharacterAttribute->AbilityTime[2] == 0)
	{
		CharacterAttribute->Ability &= (~ABILITY_FAST_ATTACK_SPEED2);
		CharacterMachine->CalculateAttackSpeed();
	}

}

void OnlyNpcChatProcess(CHARACTER* c, OBJECT* o)
{
	if (o->Kind == KIND_NPC && (rand() % 2) == 0)
	{
		switch (o->Type)
		{
		case MODEL_MERCHANT_GIRL:
			if (gMapManager->InBattleCastle() == false)
			{
				gmHeadChat->CreateChat(c->ID, GlobalText[1974], c);
			}
			break;
		case MODEL_ELF_WIZARD:
			gmHeadChat->CreateChat(c->ID, GlobalText[1975], c);
			break;
		case MODEL_MASTER:
			gmHeadChat->CreateChat(c->ID, GlobalText[1976], c);
			break;
		case MODEL_PLAYER:
			if (c->MonsterIndex == 257)
				gmHeadChat->CreateChat(c->ID, GlobalText[1827], c);
			break;
		}
	}
}

void PlayerNpcStopAnimationSetting(CHARACTER* c, OBJECT* o)
{
	int action = rand() % 100;

	if (o->CurrentAction != PLAYER_STOP_MALE)
	{
		SetAction(&c->Object, PLAYER_STOP_MALE);
	}
	else
	{
		if (action < 80)
		{
			SetAction(&c->Object, PLAYER_STOP_MALE);
		}
		else if (action < 85)
		{
			SetAction(&c->Object, PLAYER_CLAP1);
		}
		else if (action < 90)
		{
			SetAction(&c->Object, PLAYER_CHEER1);
		}
		else if (action < 95)
		{
			SetAction(&c->Object, PLAYER_SEE1);
		}
		else if (action < 100)
		{
			SetAction(&c->Object, PLAYER_UNKNOWN1);
		}

		int TextIndex = 0;
		if (World == WD_2DEVIAS)
		{
			int iTextIndices[] = { 904, 905 };
			int iNumTextIndices = 2;

			static int s_iTextIndex = iTextIndices[0];
			if (o->CurrentAction != o->PriorAction)
				s_iTextIndex = iTextIndices[rand() % iNumTextIndices];
			TextIndex = s_iTextIndex;
		}
		else if (World == WD_0LORENCIA)
		{
			int iTextIndices[] = { 823 };
			int iNumTextIndices = 1;
			static int s_iTextIndex = iTextIndices[0];
			if (o->CurrentAction != o->PriorAction)
				s_iTextIndex = iTextIndices[rand() % iNumTextIndices];
			TextIndex = s_iTextIndex;
		}

		if (TextIndex != 0)
		{
			char szText[512];
			wsprintf(szText, GlobalText[TextIndex]);
			gmHeadChat->CreateChat(c->ID, szText, c);
		}
	}
}

void PlayerStopAnimationSetting(CHARACTER* c, OBJECT* o)
{
	if (o->CurrentAction != PLAYER_DIE1 && o->CurrentAction != PLAYER_DIE2)
	{
		if (o->CurrentAction == PLAYER_STOP_FLY_BOW || (o->CurrentAction < PLAYER_WALK_MALE
			|| o->CurrentAction >= PLAYER_PROVOCATION && o->CurrentAction <= PLAYER_CHEERS
			|| o->CurrentAction >= PLAYER_IDLE1_DARKHORSE && o->CurrentAction <= PLAYER_IDLE2_DARKHORSE
			|| o->CurrentAction >= PLAYER_SKILL_THRUST
			&& o->CurrentAction <= PLAYER_RAGE_FENRIR_ATTACK_RIGHT
			&& (o->CurrentAction < PLAYER_RAGE_FENRIR_RUN || o->CurrentAction > PLAYER_RAGE_FENRIR_RUN_ONE_LEFT)
			&& (o->CurrentAction < PLAYER_RAGE_UNI_RUN || o->CurrentAction > PLAYER_RAGE_UNI_STOP_ONE_RIGHT)
			|| o->CurrentAction == PLAYER_STOP_RAGEFIGHTER
			|| o->CurrentAction >= PLAYER_HIT_FIST
			&& o->CurrentAction <= PLAYER_SHOCK
			&& o->CurrentAction != PLAYER_WALK_TWO_HAND_SWORD_TWO
			&& o->CurrentAction != PLAYER_RUN_TWO_HAND_SWORD_TWO
			&& o->CurrentAction != PLAYER_FLY_RIDE
			&& o->CurrentAction != PLAYER_FLY_RIDE_WEAPON
			&& o->CurrentAction != PLAYER_SKILL_HELL_BEGIN
			&& o->CurrentAction != PLAYER_DARKLORD_WALK
			&& o->CurrentAction != PLAYER_RUN_RIDE_HORSE
			&& (o->CurrentAction < PLAYER_FENRIR_RUN || o->CurrentAction > PLAYER_FENRIR_RUN_ONE_LEFT_ELF)
			&& o->CurrentAction != PLAYER_RECOVER_SKILL)
			)
		{
			SetPlayerStop(c);
		}
		if (o->CurrentAction == PLAYER_SKILL_HELL_BEGIN)
			o->AnimationFrame = 0.0;
	}
	else if (!c->Blood)
	{
		c->Blood = true;
		CreateBlood(o);
	}
}

void EtcStopAnimationSetting(CHARACTER* c, OBJECT* o)
{
	if (o->Type == MODEL_WARCRAFT)
	{
		o->CurrentAction = 1;
	}
	else if (battleCastle::StopBattleCastleMonster(c, o) == true)
	{
		CharacterAnimation(c, o);
		return;
	}
	else if (World == WD_1DUNGEON && o->Type == 40)
	{
		SetAction(&c->Object, 0);
	}
	else if (o->Type >= MODEL_MONSTER01 && o->Type < MODEL_MONSTER_END || GMMonsterMng->IsMonsterByIndex(c->MonsterIndex) == true)
	{
		if (o->CurrentAction == MONSTER01_DIE)
		{
			if (!c->Blood)
			{
				c->Blood = true;
				CreateBlood(o);
			}
			return;
		}
		else if (o->CurrentAction == MONSTER01_STOP2 || o->CurrentAction == MONSTER01_SHOCK
			|| o->CurrentAction == MONSTER01_ATTACK1 || o->CurrentAction == MONSTER01_ATTACK2
			|| o->CurrentAction == MONSTER01_ATTACK3 || o->CurrentAction == MONSTER01_ATTACK4
			|| o->CurrentAction == MONSTER01_ATTACK5
			)
		{
			SetAction(o, MONSTER01_STOP1);
		}

		if (o->CurrentAction == MONSTER01_APEAR && (o->Type == MODEL_MONSTER01 + 118 || o->Type == MODEL_MONSTER01 + 119 || o->Type == MODEL_MONSTER01 + 150))
		{
			if (o->Type == MODEL_MONSTER01 + 150)
			{
				o->CurrentAction = MONSTER01_STOP1;
			}
			SetAction(o, MONSTER01_STOP1);
		}
	}
	else
	{
		switch (o->Type)
		{
		case MODEL_WEDDING_NPC:
			if ((rand() % 16 < 4) && o->SubType == 0)
			{
				SetAction(o, 1);
				o->SubType = 1;
			}
			else
			{
				SetAction(o, 0);
				o->SubType = 0;
			}
			break;
		case MODEL_SMITH:
		case MODEL_SCIENTIST:
			if (rand() % 16 < 12)
				SetAction(o, 0);
			else
				SetAction(o, rand() % 2 + 1);
			break;
		case MODEL_FACE:
		case MODEL_FACE + 1:
		case MODEL_FACE + 2:
		case MODEL_FACE + 3:
		case MODEL_FACE + 4:
		case MODEL_FACE + 5:
#ifdef PBG_ADD_NEWCHAR_MONK
		case MODEL_FACE + 6:
#endif //PBG_ADD_NEWCHAR_MONK
			break;
		case MODEL_ELBELAND_SILVIA:
		case MODEL_ELBELAND_RHEA:
			if (rand() % 5 < 4 || o->CurrentAction == 1)
				SetAction(o, 0);
			else
				SetAction(o, 1);
			break;
		case MODEL_NPC_DEVIN:
			if (rand() % 5 < 4)
				SetAction(o, 0);
			else
				SetAction(o, 1);
			break;

		case MODEL_SEED_MASTER:
			if (rand() % 3 < 2 || o->CurrentAction != 0)
				SetAction(o, 0);
			else
			{
				SetAction(o, rand() % 3 + 1);
			}
			break;
		case MODEL_SEED_INVESTIGATOR:
			if (rand() % 3 < 2 || o->CurrentAction == 1)
				SetAction(o, 0);
			else
				SetAction(o, 1);
			break;
		case MODEL_LITTLESANTA:
		case MODEL_LITTLESANTA + 1:
		case MODEL_LITTLESANTA + 2:
		case MODEL_LITTLESANTA + 3:
			if (rand() % 5 < 2)
				SetAction(o, 0);
			else
				SetAction(o, rand() % 3 + 2);
			break;

		case MODEL_LITTLESANTA + 4:
		case MODEL_LITTLESANTA + 5:
		case MODEL_LITTLESANTA + 6:
		case MODEL_LITTLESANTA + 7:
			if (rand() % 5 < 2)
				SetAction(o, 1);
			else
				SetAction(o, rand() % 3 + 2);
			break;
		case MODEL_XMAS2008_SANTA_NPC:
			if (rand() % 3 < 2 || (o->CurrentAction == 1 || o->CurrentAction == 2))
			{
				SetAction(o, 0);
			}
			else
			{
				SetAction(o, rand() % 2 + 1);
			}
			break;
		case MODEL_XMAS2008_SNOWMAN_NPC:
			SetAction(o, 0);
			break;
		case MODEL_GAMBLE_NPC_MOSS:
			if (rand() % 5 < 4 || o->CurrentAction == 1)
			{
				SetAction(o, 0);
			}
			else
			{
				SetAction(o, 1);
			}
			break;
		case MODAL_GENS_NPC_DUPRIAN:
		case MODAL_GENS_NPC_BARNERT:
			if (rand() % 5 < 4)
				SetAction(o, 0);
			else
				SetAction(o, rand() % 2 + 1);
			break;
		case MODEL_UNITEDMARKETPLACE_RAUL:
		case MODEL_UNITEDMARKETPLACE_JULIA:
			if (rand() % 5 < 4)
				SetAction(o, 0);
			else
				SetAction(o, rand() % 2 + 1);
			break;
		case MODEL_UNITEDMARKETPLACE_CHRISTIN:
		case MODEL_KARUTAN_NPC_REINA:
			if (rand() % 5 < 3)
				SetAction(o, 0);
			else
				SetAction(o, rand() % 2 + 1);
			break;
		case MODEL_KARUTAN_NPC_VOLVO:
			if (rand() % 5 < 4)
				SetAction(o, 0);
			else
				SetAction(o, 1);
			break;
		case MODEL_LUCKYITEM_NPC:
			if (rand() % 5 < 4 || o->CurrentAction == 1)
				SetAction(o, 0);
			else
				SetAction(o, 1);
			break;
		default:
			SetAction(o, rand() % 2);
			break;
		}
	}
}

void AnimationCharacter(CHARACTER* c, OBJECT* o, BMD* b)
{
	bool bEventNpc = false;
	if (o->Kind == KIND_NPC && (World == WD_0LORENCIA || World == WD_2DEVIAS) && o->Type == MODEL_PLAYER && (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3))
	{
		if (World == WD_0LORENCIA)
			Vector(0.f, 0.f, 90.f, o->Angle);
		else
			Vector(0.f, 0.f, 0.f, o->Angle);

		bEventNpc = true;
	}

	OnlyNpcChatProcess(c, o);

	bool Play = CharacterAnimation(c, o);

	if (!Play)
	{
		c->LongRangeAttack = -1;
		c->Foot[0] = false;
		c->Foot[1] = false;
		if (o->Type == MODEL_PLAYER)
		{
			if (bEventNpc)
				PlayerNpcStopAnimationSetting(c, o);
			else
				PlayerStopAnimationSetting(c, o);
		}
		else
		{
			EtcStopAnimationSetting(c, o);
		}

		if (o->CurrentAction == MONSTER01_STOP1 || o->CurrentAction == MONSTER01_STOP2)
			PlayMonsterSound(o);

		if (o->Type == MODEL_WARCRAFT)
		{
			o->AnimationFrame = 8.f;
		}
	}
	if (b && b->GetAction(o->CurrentAction))
	{

		switch (o->Type)
		{
		case MODEL_DEVIAS_TRADER:
			if (b->CurrentAnimationFrame == b->Actions[o->CurrentAction].NumAnimationKeys - 1)
			{
				if (rand() % 32 == 0)
					SetAction(o, 1);
				else
					SetAction(o, 0);
			}
			break;
		case MODEL_MONSTER01 + 128:
			if (o->CurrentAction <= 1 && b->CurrentAnimationFrame == b->Actions[o->CurrentAction].NumAnimationKeys - 1)
			{
				if (rand() % 10 == 0)
					SetAction(o, 1);
				else
					SetAction(o, 0);
			}
			break;
		}
	}
}

void CreateWeaponBlur(CHARACTER* c, OBJECT* o, BMD* b)
{
	if ((o->AnimationFrame >= 3.f || (o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD_TWO) ))
	{
		int Hand = 0;
		int Hand2 = 0;
		int BlurType = 0;
		int BlurMapping = 0;
		int Type = c->Weapon[Hand].Type;
		int Level = c->Weapon[Hand].Level;

		if (o->Type == MODEL_PLAYER)
		{
			if (o->CurrentAction == PLAYER_HIT_SWORD_LEFT1 || o->CurrentAction == PLAYER_HIT_SWORD_LEFT2)
			{
				Hand = 1;
				Hand2 = 1;
			}
		}
		if (o->Type == MODEL_PLAYER)
		{
			if (o->CurrentAction == PLAYER_HIT_ONE_FLASH || o->CurrentAction == PLAYER_HIT_RUSH)
			{
				BlurType = 1;
				BlurMapping = 2;
			}
			else if (o->CurrentAction == PLAYER_HIT_SKILL_SWORD2 || o->CurrentAction == PLAYER_HIT_SKILL_SWORD3 || o->CurrentAction == PLAYER_HIT_SKILL_SWORD4)
			{
				BlurType = 1;
				if (Type == MODEL_SWORD + 14 || Type == MODEL_SWORD + 21 || Type == MODEL_SWORD + 31)
					BlurMapping = 1;
				else
					BlurMapping = 2;
			}
			else if (o->CurrentAction == PLAYER_HIT_STRIKE)
			{
				BlurType = 1;
				BlurMapping = 2;
			}
			else if (o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB
				|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_UNI
				|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_DINO
				|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_FENRIR)
			{
				BlurType = 1;
				BlurMapping = 1;
			}
			else if (o->CurrentAction == PLAYER_SKILL_BLOW_OF_DESTRUCTION && o->AnimationFrame >= 2.f && o->AnimationFrame <= 8.f)
			{
				BlurType = 1;
				BlurMapping = 2;
			}
			else if (o->CurrentAction == PLAYER_HIT_SKILL_SWORD5)
			{
				BlurType = 1;
				if (Type == MODEL_MACE + 5)
					BlurMapping = 1;
				else
					BlurMapping = 2;
			}
			else if (Type >= MODEL_SWORD && Type < MODEL_SWORD + MAX_ITEM_INDEX)
			{
				if ((o->CurrentAction >= PLAYER_HIT_SWORD_RIGHT1 && o->CurrentAction <= PLAYER_HIT_TWO_HAND_SWORD3) || o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD_TWO )
				{
					BlurType = 1;
					if (Type == MODEL_SWORD + 17)
					{
						BlurMapping = 6;
					}
					else if (o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD3 || o->CurrentAction == PLAYER_HIT_TWO_HAND_SWORD_TWO)
					{
						if (Type == MODEL_SWORD + 25)
							BlurMapping = 2;
						else
							BlurMapping = 1;
					}
				}
			}
			else if (Type == MODEL_AXE + 3 || Type >= MODEL_AXE + 5 && Type < MODEL_MACE + MAX_ITEM_INDEX)
			{
				if (o->CurrentAction >= PLAYER_HIT_SKILL_SWORD1 && o->CurrentAction <= PLAYER_HIT_SKILL_SWORD5)
				{
					BlurType = 1;
					BlurMapping = 2;
				}
			}
			else if (Type >= MODEL_SPEAR && Type < MODEL_SPEAR + MAX_ITEM_INDEX)
			{
				if (o->CurrentAction >= PLAYER_HIT_SPEAR1 && o->CurrentAction <= PLAYER_HIT_SCYTHE3)
				{
					BlurType = 3;
					if (Type == MODEL_SPEAR + 10)
					{
						BlurType = 1;
						BlurMapping = 0;
					}
					else if (o->CurrentAction == PLAYER_HIT_SCYTHE3)
						BlurMapping = 1;
				}
			}
		}
		else
		{
			if (c->MonsterIndex == 71 || c->MonsterIndex == 74 || c->MonsterIndex == 301)
			{
				if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
				{
					BlurType = 1;
					BlurMapping = 6;
				}
			}
			else if (o->Type == MODEL_MONSTER01 + 66)
			{
				if (o->CurrentAction == MONSTER01_ATTACK1)
				{
					BlurType = 5;
					BlurMapping = 2;
					Hand = 0;
					Hand2 = 1;
				}
			}
			else if (o->Type == MODEL_MONSTER01 + 67)
			{
				if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
				{
					BlurType = 4;
					BlurMapping = 0;
					Level = 99;
				}
			}
			else if (o->Type == MODEL_MONSTER01 + 69)
			{
				if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
				{
					if (o->SubType == 9)
					{
						BlurType = 1;
						BlurMapping = 2;
					}
					else
					{
						BlurType = 1;
						BlurMapping = 0;
						Level = 99;
						Type = 0;
					}
				}
			}
			else if (Type >= MODEL_SWORD && Type < MODEL_SWORD + MAX_ITEM_INDEX)
			{
				if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
					BlurType = 1;
			}
		}
		if (BlurType > 0)
		{
			vec3_t  Light;
			vec3_t  Pos1, Pos2;
			vec3_t  p, p2;
			switch (BlurType)
			{
			case 1:
				Vector(0.f, -20.f, 0.f, Pos1);
				break;
			case 2:
				Vector(0.f, -80.f, 0.f, Pos1);
				break;
			case 3:
				Vector(0.f, -100.f, 0.f, Pos1);
				break;
			}
			Vector(0.f, -120.f, 0.f, Pos2);

			if (BlurType == 4)
			{
				Vector(0.f, 0.f, 0.f, Pos1);
				Vector(0.f, -200.f, 0.f, Pos2);
			}
			else if (BlurType == 5)
			{
				Vector(0.f, 0.f, 0.f, Pos1);
				Vector(0.f, -20.f, 0.f, Pos2);
			}

			if (Type == MODEL_SWORD + 13 || Type == MODEL_MACE + 6 || Type == MODEL_SPEAR + 9)
			{
				Vector(1.f, 0.2f, 0.2f, Light);
			}
			else if (Level == 99)
			{
				Vector(0.3f, 0.2f, 1.f, Light);
			}
			else if (BlurMapping == 0)
			{
				if (Level >= 7)
				{
					Vector(1.f, 0.6f, 0.2f, Light);
				}
				else if (Level >= 5)
				{
					Vector(0.2f, 0.4f, 1.f, Light);
				}
				else if (Level >= 3)
				{
					Vector(1.f, 0.2f, 0.2f, Light);
				}
				else
				{
					Vector(0.8f, 0.8f, 0.8f, Light);
				}
			}
			else
			{
				Vector(1.f, 1.f, 1.f, Light);
			}

			if ((o->Type != MODEL_PLAYER || Type == MODEL_SWORD + 3 || Type == MODEL_SWORD + 6 || Type == MODEL_SWORD + 9 || Type == MODEL_SWORD + 11 || Type == MODEL_SPEAR + 4)
				&& o->Type != MODEL_MONSTER01 + 66 && o->Type != MODEL_MONSTER01 + 67 && o->Type != MODEL_MONSTER01 + 69
				)
			{
				b->TransformPosition(o->BoneTransform[c->Weapon[Hand].LinkBone], Pos1, p, true);
				b->TransformPosition(o->BoneTransform[c->Weapon[Hand2].LinkBone], Pos2, p2, true);
				CreateBlur(c, p, p2, Light, BlurMapping);
			}
#ifdef PBG_ADD_NEWCHAR_MONK
			else if (g_CMonkSystem.IsSwordformGloves(Type))
			{
				g_CMonkSystem.MoveBlurEffect(c, o, b);
			}
#endif //PBG_ADD_NEWCHAR_MONK
			else
			{
				float inter = 10.f;
				float animationFrame = o->AnimationFrame - b->Actions[b->CurrentAction].PlaySpeed;
				float priorAnimationFrame = o->PriorAnimationFrame;
				float animationSpeed = b->Actions[b->CurrentAction].PlaySpeed / inter;

				for (int i = 0; i < (int)(inter); ++i)
				{
					b->Animation(BoneTransform, animationFrame, priorAnimationFrame, o->PriorAction, o->Angle, o->HeadAngle);

					b->TransformPosition(BoneTransform[c->Weapon[Hand].LinkBone], Pos1, p, false);
					b->TransformPosition(BoneTransform[c->Weapon[Hand2].LinkBone], Pos2, p2, false);

					if (o->Type == MODEL_MONSTER01 + 66 && i % 2)
					{
						CreateParticle(BITMAP_FIRE + 3, p2, o->Angle, Light, 12);
					}

					if (c->Weapon[0].Type != -1 || c->Weapon[1].Type != -1)
						CreateBlur(c, p, p2, Light, BlurMapping, true);

					animationFrame += animationSpeed;
				}
			}
		}
		M31HuntingGround::MoveHuntingGroundBlurEffect(c, o, b);
		M34CryingWolf2nd::MoveCryingWolf2ndBlurEffect(c, o, b);
		M34CryWolf1st::MoveCryWolf1stBlurEffect(c, o, b);
		M33Aida::MoveAidaBlurEffect(c, o, b);
		M37Kanturu1st::MoveKanturu1stBlurEffect(c, o, b);
		M38Kanturu2nd::Move_Kanturu2nd_BlurEffect(c, o, b);
		M39Kanturu3rd::MoveKanturu3rdBlurEffect(c, o, b);
		SEASON3A::CGM3rdChangeUp::Instance().MoveBalgasBarrackBlurEffect(c, o, b);
		g_CursedTemple->MoveBlurEffect(c, o, b);
		SEASON3B::GMNewTown::MoveBlurEffect(c, o, b);
		SEASON3C::GMSwampOfQuiet::MoveBlurEffect(c, o, b);
		TheMapProcess().MoveBlurEffect(c, o, b);
	}
	else
	{
		VectorCopy(o->Position, o->StartPosition);
	}
}

void MoveCharacter(CHARACTER* c, OBJECT* o)
{
	if (o->Type == MODEL_WARCRAFT)
	{
		char Text[100];
		char ID[100];
		wsprintf(ID, "%s .", c->ID);
		wsprintf(Text, GlobalText[1176], c->Level);
		strcat(ID, Text);
		AddObjectDescription(ID, o->Position);
	}

	BMD* b = gmClientModels->GetModel(o->Type);

	if (b)
	{
		VectorCopy(o->Position, b->BodyOrigin);
		b->BodyScale = o->Scale;
		b->CurrentAction = o->CurrentAction;

		CalcStopTime();
		HeroAttributeCalc(c);
		PushingCharacter(c, o);
		DeadCharacter(c, o, b);
		Alpha(o);

		if (c->Freeze > 0.f)
		{
			c->Freeze -= timefac(0.03f);
		}

		AnimationCharacter(c, o, b);

		if (c->Dead > 0)
		{
			c->Dead += timefac(1.f);
			if (c->Dead >= 15)
			{
				SetPlayerDie(c);
			}
			if (gMapManager->InBloodCastle() && o->m_bActionStart)
			{
				SetPlayerDie(c);
			}
		}

		vec3_t p, Position, Light;
		float Luminosity = (float)(rand() % 6 + 2) * 0.1f;

		Vector(0.f, 0.f, 0.f, p);
		Vector(1.f, 1.f, 1.f, Light);

		if (gMapManager->InBattleCastle() == false && o->m_byHurtByOneToOne > 0)
		{
			vec3_t pos1, pos2;

			Vector(0.f, 0.f, 0.f, p);
			for (int i = 0; i < b->NumBones; ++i)
			{
				if (!b->Bones[i].Dummy)
				{
					int iParent = b->Bones[i].Parent;
					if (iParent > -1 && iParent < b->NumBones)
					{
						b->TransformPosition(o->BoneTransform[i], p, pos1, true);
						b->TransformPosition(o->BoneTransform[iParent], p, pos2, true);

						GetNearRandomPos(pos1, 20, pos1);
						GetNearRandomPos(pos2, 20, pos2);
						CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 20.f);
					}
				}
			}
			o->m_byHurtByOneToOne--;
		}

		if ((o->CurrentAction == PLAYER_HIT_TELEPORT || o->CurrentAction == PLAYER_HIT_RIDE_TELEPORT
			|| o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT
			) && o->AnimationFrame > 5.5f)
		{
			Vector(0.f, 0.f, 0.f, p);
			Vector(0.3f, 0.5f, 1.f, Light);
			b->TransformPosition(o->BoneTransform[42], p, Position, true);
			CreateParticleSync(BITMAP_LIGHT, Position, o->Angle, Light);
		}

		if (c->AttackTime >= 1.f)
		{
			AttackStage(c, o);
			AttackEffect(c);
			c->AttackTime += timefac(1.f);
		}

		if (c->AttackTime >= g_iLimitAttackTime)
		{
			c->AttackTime = 0;
			o->PKKey = getTargetCharacterKey(c, SelectedCharacter, 0);

			switch ((c->Skill))
			{
			case AT_SKILL_SUMMON:
			case AT_SKILL_SUMMON + 1:
			case AT_SKILL_SUMMON + 2:
			case AT_SKILL_SUMMON + 3:
			case AT_SKILL_SUMMON + 4:
			case AT_SKILL_SUMMON + 5:
			case AT_SKILL_SUMMON + 6:
#ifdef ADD_ELF_SUMMON
			case AT_SKILL_SUMMON + 7:
#endif// ADD_ELF_SUMMON
			case Skill_Summon_Satyros:
				CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, o->Light, 3, o);
				break;
			case Skill_Anger_Blow_Strengthener:
			case Skill_Anger_Blow_Mastery:
			case AT_SKILL_FURY_STRIKE:
				o->Weapon = c->Weapon[0].Type - MODEL_SWORD;
				o->WeaponLevel = (BYTE)c->Weapon[0].Level;
				CreateEffect(MODEL_SKILL_FURY_STRIKE, o->Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey((c->Skill)));
				PlayBuffer(SOUND_FURY_STRIKE1);
				break;
			case Skill_Strike_of_Destruction_Strengthe:
			case Skill_Strike_of_Destruction_Proficien:
			case Skill_Strike_of_Destruction_Mastery:
			case AT_SKILL_BLOW_OF_DESTRUCTION:
				o->Weapon = c->Weapon[0].Type - MODEL_SWORD;
				o->WeaponLevel = (BYTE)c->Weapon[0].Level;
				Vector(0.f, 0.f, 0.f, o->Light);
				o->Light[0] = (float)(c->SkillX + 0.5f) * TERRAIN_SCALE;
				o->Light[1] = (float)(c->SkillY + 0.5f) * TERRAIN_SCALE;
				o->Light[2] = o->Position[2];
				CreateEffect(MODEL_BLOW_OF_DESTRUCTION, o->Position, o->Angle, o->Light, 0, o);
				PlayBuffer(SOUND_SKILL_BLOWOFDESTRUCTION);
				break;
			case Skill_Fire_Slash_Strengthener:
			case Skill_Fire_Slash_Mastery:
			case Skill_Fire_Slash:
				o->Weapon = c->Weapon[0].Type - MODEL_SWORD;
				o->WeaponLevel = (BYTE)c->Weapon[0].Level;
				break;
			case Skill_Power_Slash_Strengthener:
			case AT_SKILL_ICE_BLADE:
				o->Weapon = c->Weapon[0].Type - MODEL_SWORD;
				o->WeaponLevel = (BYTE)c->Weapon[0].Level;
				break;
			case Skill_Swell_Life_Strengthener:
			case Skill_Swell_Life_Proficiency:
			case Skill_Swell_Life_Mastery:
			case AT_SKILL_VITALITY:
			{
				vec3_t Angle = { 0.0f, 0.0f, 0.0f };

				for (int i = 0; i < 36; ++i)
				{
					Angle[0] = -10.f;
					Angle[1] = 0.f;
					Angle[2] = i * 10.f;
					vec3_t Position;
					VectorCopy(o->Position, Position);
					Position[2] += 100.f;
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, Position, Angle, 2, o, 60.f, 0, 0);

					if ((i % 20) == 0)
					{
						CreateEffect(BITMAP_MAGIC + 1, o->Position, Angle, o->Light, 4, o);
					}
				}
			}
			PlayBuffer(SOUND_SWELLLIFE);
			break;
			case AT_SKILL_STUN:
				CreateJoint(BITMAP_FLASH, o->Position, o->Position, o->Angle, 7, NULL);
				PlayBuffer(SOUND_BMS_STUN);
				break;
			case AT_SKILL_REMOVAL_STUN:
			{
				CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
				if (tc)
				{
					OBJECT* to = &tc->Object;
					if (to != o)
					{
						VectorCopy(to->Position, Position);
						Position[2] += 1200.f;
						CreateJoint(BITMAP_FLASH, Position, Position, to->Angle, 0, to, 120.f);
						PlayBuffer(SOUND_BMS_STUN_REMOVAL);
					}
				}
			}
			break;
			case AT_SKILL_MANA:
			{
				vec3_t Angle = { 0.0f, 0.0f, 0.0f };
				int iCount = 36;

				for (int i = 0; i < iCount; ++i)
				{
					Angle[0] = -10.f;
					Angle[1] = 0.f;
					Angle[2] = i * 10.f;
					vec3_t Position;
					VectorCopy(o->Position, Position);
					Position[2] += 100.f;
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, Position, Angle, 21, o, 60.f, 0, 0);
					if ((i % 20) == 0)
						CreateEffect(BITMAP_MAGIC + 1, o->Position, Angle, Light, 10, o);
				}
			}
			PlayBuffer(SOUND_BMS_MANA);
			break;
			case AT_SKILL_INVISIBLE:
			{
				CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
				if (tc)
				{
					OBJECT* to = &tc->Object;

					DeleteJoint(MODEL_SPEARSKILL, to, 4);
					DeleteJoint(MODEL_SPEARSKILL, to, 9);
					CreateEffect(BITMAP_MAGIC + 1, to->Position, to->Angle, to->Light, 6, to);
					PlayBuffer(SOUND_BMS_INVISIBLE);
				}
			}
			break;
			case AT_SKILL_REMOVAL_INVISIBLE:
			{
				CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);
				if (tc)
				{
					OBJECT* to = &tc->Object;
					if (to != o)
					{
						VectorCopy(to->Position, Position);
						Position[2] += 1200.f;
						CreateJoint(BITMAP_FLASH, Position, Position, to->Angle, 1, to, 120.f);

						PlayBuffer(SOUND_BMS_STUN_REMOVAL);
					}
				}
			}
			break;
			case AT_SKILL_REMOVAL_BUFF:
			{
				vec3_t Angle;
				vec3_t Position;
				VectorCopy(o->Position, Position);
				Position[2] += 100.f;

				std::list<eBuffState> bufflist;

				//debuff
				bufflist.push_back(eDeBuff_Poison);
				bufflist.push_back(eDeBuff_Freeze);
				bufflist.push_back(eDeBuff_Harden);
				bufflist.push_back(eDeBuff_Defense);
				bufflist.push_back(eDeBuff_Stun);
				bufflist.push_back(eDeBuff_Sleep);
				bufflist.push_back(eDeBuff_BlowOfDestruction);

				//buff
				bufflist.push_back(eBuff_HpRecovery);
				bufflist.push_back(eBuff_Attack);
				//bufflist.push_back( eBuff_Life );
				//bufflist.push_back( eBuff_Attack );
				bufflist.push_back(eBuff_Defense);
				bufflist.push_back(eBuff_AddAG);
				bufflist.push_back(eBuff_Cloaking);
				bufflist.push_back(eBuff_AddSkill);
				bufflist.push_back(eBuff_PhysDefense);
				bufflist.push_back(eBuff_AddCriticalDamage);
				bufflist.push_back(eBuff_CrywolfAltarOccufied);

				g_CharacterUnRegisterBuffList(o, bufflist);

				Vector(0.f, 0.f, 45.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 5, o, 170.0f);
				Position[2] -= 10.f;
				Vector(0.f, 0.f, 135.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 6, o, 170.0f);
				Position[2] -= 10.f;
				Vector(0.f, 0.f, 225.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 7, o, 170.0f);

				Vector(0.f, 0.f, 90.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 5, o, 170.0f);
				Position[2] -= 10.f;
				Vector(0.f, 0.f, 180.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 6, o, 170.0f);
				Position[2] -= 10.f;
				Vector(0.f, 0.f, 270.f, Angle);
				CreateJoint(MODEL_SPEARSKILL, Position, Position, Angle, 7, o, 170.0f);
			}

			PlayBuffer(SOUND_BMS_MAGIC_REMOVAL);
			break;
			case AT_SKILL_IMPROVE_AG:
			{
				vec3_t Angle = { -45.f, 0.f, 45.f };
				vec3_t Light = { 1.f, 1.f, 1.f };
				vec3_t Position;

				Position[0] = o->Position[0] + sinf(45 * 0.1f) * 80.f;
				Position[1] = o->Position[1] + cosf(45 * 0.1f) * 80.f;
				Position[2] = o->Position[2] + 300;
				CreateJoint(BITMAP_JOINT_HEALING, Position, Position, Angle, 10, o, 15.f);
				Angle[2] = 405.f;
				CreateJoint(BITMAP_JOINT_HEALING, Position, Position, Angle, 10, o, 15.f);

				Angle[2] = 225.f;
				Position[0] = o->Position[0] + cosf(225 * 0.1f) * 80.f;
				Position[1] = o->Position[1] + sinf(225 * 0.1f) * 80.f;
				Position[2] = o->Position[2] + 300;
				CreateJoint(BITMAP_JOINT_HEALING, Position, Position, Angle, 10, o, 15.f);
				Angle[2] = 450.f;
				CreateJoint(BITMAP_JOINT_HEALING, Position, Position, Angle, 10, o, 15.f);
			}
			PlayBuffer(SOUND_SWELLLIFE);
			break;
			case AT_SKILL_ADD_CRITICAL:
				Vector(0.f, 0.f, 0.f, p);
				Vector(1.f, 0.6f, 0.3f, Light);
				if (c->Weapon[0].Type != MODEL_BOW + 15)
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 0);
				}
				if (c->Weapon[1].Type != MODEL_BOW + 7 && (c->Weapon[1].Type < MODEL_SHIELD || c->Weapon[1].Type >= MODEL_SHIELD + MAX_ITEM_INDEX))
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[1].LinkBone], p, Position, true);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 1);
				}
				PlayBuffer(SOUND_CRITICAL, o);
				break;
			case AT_SKILL_PARTY_TELEPORT:
				CreateEffect(MODEL_CIRCLE, o->Position, o->Angle, o->Light, 2, o);
				CreateEffect(MODEL_CIRCLE_LIGHT, o->Position, o->Angle, o->Light, 3);

				PlayBuffer(SOUND_PART_TELEPORT, o);
				break;
			case AT_SKILL_THUNDER_STRIKE:
				CalcAddPosition(o, 0.f, -90.f, -50.f, Position);
				if (o->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH)
				{
					Position[2] += 80.f;
				}
				else if (o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_FLASH)
				{
					Position[2] += 40.f;
				}

				CreateEffect(BITMAP_FLARE_FORCE, Position, o->Angle, o->Light, 0, o);

				PlayBuffer(SOUND_ELEC_STRIKE, o);
				break;
			case AT_SKILL_RIDER:
				CreateEffect(BITMAP_SHOTGUN, o->Position, o->Angle, o->Light, 0, o, o->PKKey, CurrentSkill);
				PlayBuffer(SOUND_SKILL_SWORD3);
				break;
			case Skill_Twisting_Slash:
			case Skill_Twisting_Slash_Strengthener:
			case Skill_Twisting_Slash_Strengthener2:
			case Skill_Twisting_Slash_Mastery:
				o->Weapon = c->Weapon[0].Type - MODEL_SWORD;
				o->WeaponLevel = (BYTE)c->Weapon[0].Level;
				if (o->runtime_whelskill < 1)
					CreateEffect(MODEL_SKILL_WHEEL1, o->Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey((c->Skill)));

				if (SceneFlag != LOG_IN_SCENE)
					PlayBuffer(SOUND_SKILL_SWORD4);
				c->PostMoveProcess_Active(g_iLimitAttackTime);
				break;
			case Skill_Hellfire_Strengthener:
			case AT_SKILL_HELL:
				CreateEffect(MODEL_CIRCLE, o->Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey((c->Skill)));
				CreateEffect(MODEL_CIRCLE_LIGHT, o->Position, o->Angle, o->Light);

				if (gMapManager->InHellas() == true)
				{
					AddWaterWave((c->PositionX), (c->PositionY), 2, -1500);
				}

				PlayBuffer(SOUND_HELLFIRE);
				break;
			case AT_SKILL_BLAST_HELL:
			case Skill_Nova_Strengthener:
				CreateEffect(MODEL_CIRCLE, o->Position, o->Angle, o->Light, 1, o);
				StopBuffer(SOUND_NUKE1, true);
				PlayBuffer(SOUND_NUKE2);
				break;
			case AT_SKILL_BLAST_POISON:
			case Skill_Decay_Strengthener:
				Position[0] = (float)(c->SkillX + 0.5f) * TERRAIN_SCALE;
				Position[1] = (float)(c->SkillY + 0.5f) * TERRAIN_SCALE;
				Position[2] = RequestTerrainHeight(Position[0], Position[1]);

				Vector(0.8f, 0.5f, 0.1f, Light);
				CreateEffect(MODEL_FIRE, Position, o->Angle, Light, 6, NULL, 0);
				CreateEffect(MODEL_FIRE, Position, o->Angle, Light, 6, NULL, 0);
				PlayBuffer(SOUND_DEATH_POISON1);

				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}
				break;
			case Skill_Ice_Storm_Strengthener:
			case Skill_Ice_Storm_Mastery:
			case AT_SKILL_BLAST_FREEZE:
			{
				vec3_t TargetPosition, Pos;
				TargetPosition[0] = (float)(c->SkillX + 0.5f) * TERRAIN_SCALE;
				TargetPosition[1] = (float)(c->SkillY + 0.5f) * TERRAIN_SCALE;
				TargetPosition[2] = RequestTerrainHeight(TargetPosition[0], TargetPosition[1]);

				for (int i = 0; i < 10; ++i)
				{
					Pos[0] = TargetPosition[0];
					Pos[1] = TargetPosition[1];
					Pos[2] = TargetPosition[2] + (rand() % 50) * i;
					CreateEffect(MODEL_BLIZZARD, Pos, o->Angle, Light, 0, NULL, i + 1);
				}
				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}

				PlayBuffer(SOUND_SUDDEN_ICE1);
			}
			break;
			case Skill_Flame:
			case Skill_Flame_Strengthener:
			case Skill_Flame_Strengthener2:
				Position[0] = (float)(c->SkillX + 0.5f) * TERRAIN_SCALE;
				Position[1] = (float)(c->SkillY + 0.5f) * TERRAIN_SCALE;
				Position[2] = RequestTerrainHeight(Position[0], Position[1]);
				CreateEffect(BITMAP_FLAME, Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey(c->Skill));
				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}
				if (SceneFlag != LOG_IN_SCENE)
					PlayBuffer(SOUND_FLAME);
				break;
			case AT_SKILL_STORM:
				CreateEffect(MODEL_STORM, o->Position, o->Angle, Light, 0, o, o->PKKey, FindHotKey(AT_SKILL_STORM));
				PlayBuffer(SOUND_STORM);
				break;
			case Skill_Fire_Scream_Strengthener:
			case Skill_Fire_Scream_Mastery:
			case Skill_Fire_Scream:
			{
				int SkillIndex = FindHotKey((c->Skill));
				OBJECT* pObj = o;
				vec3_t ap, P, dp;
				float BkO = pObj->Angle[2];

				VectorCopy(pObj->Position, ap);
				CreateEffect(MODEL_DARK_SCREAM, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);
				CreateEffect(MODEL_DARK_SCREAM_FIRE, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);

				Vector(80.f, 0.f, 0.f, P);

				pObj->Angle[2] += 10.f;

				AngleMatrix(pObj->Angle, pObj->Matrix);
				VectorRotate(P, pObj->Matrix, dp);
				VectorAdd(dp, pObj->Position, pObj->Position);
				CreateEffect(MODEL_DARK_SCREAM, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);
				CreateEffect(MODEL_DARK_SCREAM_FIRE, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);

				VectorCopy(ap, pObj->Position);
				VectorCopy(pObj->Position, ap);
				Vector(-80.f, 0.f, 0.f, P);
				pObj->Angle[2] -= 20.f;

				AngleMatrix(pObj->Angle, pObj->Matrix);
				VectorRotate(P, pObj->Matrix, dp);
				VectorAdd(dp, pObj->Position, pObj->Position);
				CreateEffect(MODEL_DARK_SCREAM, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);
				CreateEffect(MODEL_DARK_SCREAM_FIRE, pObj->Position, pObj->Angle, pObj->Light, 0, pObj, pObj->PKKey, SkillIndex);
				VectorCopy(ap, pObj->Position);
				pObj->Angle[2] = BkO;

				if (((c->Helper.Type >= MODEL_HELPER + 2 && c->Helper.Type <= MODEL_HELPER + 4) || o->movementType == 6 || o->movementType == 7 || o->movementType == 11)
					&& !c->SafeZone)
				{
					SetAction(o, PLAYER_HIT_RIDE_STRIKE);
				}
				else if ((c->Helper.Type == MODEL_HELPER + 37 || o->movementType == 5) && !c->SafeZone)
				{
					SetAction(o, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE);
				}
				else
				{
					SetAction(o, PLAYER_HIT_STRIKE);
				}

				PlayBuffer(SOUND_FIRE_SCREAM);
			}
			break;
			case AT_SKILL_FLASH:
				CalcAddPosition(o, -20.f, -90.f, 100.f, Position);
				CreateEffect(BITMAP_BOSS_LASER, Position, o->Angle, Light, 0, o, o->PKKey, FindHotKey(AT_SKILL_FLASH));
				PlayBuffer(SOUND_FLASH);
				break;
			case Skill_Blast_Strengthener:
			case Skill_Blast_Strengthener2:
			case Skill_Cometfall:
				Position[0] = (float)(c->SkillX + 0.5f) * TERRAIN_SCALE;
				Position[1] = (float)(c->SkillY + 0.5f) * TERRAIN_SCALE;
				Position[2] = RequestTerrainHeight(Position[0], Position[1]);
				{
					int SkillIndex = FindHotKey((c->Skill));
					CreateEffect(MODEL_SKILL_BLAST, Position, o->Angle, o->Light, 0, o, o->PKKey, SkillIndex);
					CreateEffect(MODEL_SKILL_BLAST, Position, o->Angle, o->Light, 0, o, o->PKKey, SkillIndex);
				}

				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}
				break;
			case AT_SKILL_INFERNO:
			case Skill_Inferno_Strengthener:
			case Skill_Inferno_Strengthener2:
				CreateInferno(o->Position);
				CreateEffect(MODEL_SKILL_INFERNO, o->Position, o->Angle, o->Light, 0, o, o->PKKey, FindHotKey(c->Skill));
				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}
				break;
			case Skill_Evil_Spirit_Strengthener:
			case Skill_Evil_Spirit_Strengthener2:
			case Skill_Evil_Spirit:
				vec3_t Position;
				VectorCopy(o->Position, Position);
				Position[2] += 100.f;

				for (int i = 0; i < 4; i++)
				{
					vec3_t Angle;
					Vector(0.f, 0.f, i * 90.f, Angle);

					int SkillIndex = FindHotKey((c->Skill));
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, o->Position, Angle, 0, o, 80.f, o->PKKey, SkillIndex, o->m_bySkillSerialNum);
					CreateJoint(BITMAP_JOINT_SPIRIT, Position, o->Position, Angle, 0, o, 20.f);
				}
				if (c == Hero)
				{
					++CharacterMachine->PacketSerial;
				}
				PlayBuffer(SOUND_EVIL);
				break;
			case AT_SKILL_PLASMA_STORM_FENRIR:
			{
				PlayBuffer(SOUND_FENRIR_SKILL);
				CHARACTER* p_temp_c;
				OBJECT* p_o[MAX_FENRIR_SKILL_MONSTER_NUM];
				int iMonsterNum = 0;

				for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
				{
					p_temp_c = gmCharacters->GetCharacter(i);

					if (p_temp_c->Object.Live == TRUE && p_temp_c->Object.Kind == KIND_MONSTER && p_temp_c->Object.CurrentAction != MONSTER01_DIE)
					{
						float dx = c->Object.Position[0] - p_temp_c->Object.Position[0];
						float dy = c->Object.Position[1] - p_temp_c->Object.Position[1];
						float fDistance = sqrtf(dx * dx + dy * dy) / TERRAIN_SCALE;
						if (fDistance <= gSkillManager.GetSkillDistance(AT_SKILL_PLASMA_STORM_FENRIR))
						{
							p_o[iMonsterNum] = &p_temp_c->Object;
							iMonsterNum++;
						}
					}
					else if (p_temp_c->Object.Live == TRUE && p_temp_c->Object.Kind == KIND_PLAYER && p_temp_c->Object.CurrentAction != PLAYER_DIE1)
					{
						if (CheckAttack_Fenrir(p_temp_c) == true && CInput::Instance().IsKeyDown(VK_LCONTROL))
						{
							float dx = c->Object.Position[0] - p_temp_c->Object.Position[0];
							float dy = c->Object.Position[1] - p_temp_c->Object.Position[1];
							float fDistance = sqrtf(dx * dx + dy * dy) / TERRAIN_SCALE;
							if (fDistance <= gSkillManager.GetSkillDistance(AT_SKILL_PLASMA_STORM_FENRIR))
							{
								p_o[iMonsterNum] = &p_temp_c->Object;
								iMonsterNum++;
							}
						}
					}

					if (iMonsterNum >= 10)
						break;
				}

				vec3_t vAngle;
				int iAngle = rand() % 360;

				if (c->m_iFenrirSkillTarget != -1 && c->m_iFenrirSkillTarget < MAX_CHARACTERS_CLIENT)
				{
					CHARACTER* p_tc = gmCharacters->GetCharacter(c->m_iFenrirSkillTarget);
					OBJECT* p_to = &p_tc->Object;

					for (int j = 0; j < 2; j++)
					{
						CalcAddPosition(o, 0.f, -140.f, 130.f, Position);
						Vector((float)(rand() % 360), 0.0f, (float)(rand() % 360), vAngle);

						CreateJoint(MODEL_FENRIR_SKILL_THUNDER, Position, p_to->Position, vAngle, 0 + GetFenrirType(c), p_to, 100.f);
						CreateJoint(MODEL_FENRIR_SKILL_THUNDER, Position, p_to->Position, vAngle, 3 + GetFenrirType(c), p_to, 80.f);
					}

					for (int i = 0; i < iMonsterNum; i++)
					{
						for (int j = 0; j < 2; j++)
						{
							CalcAddPosition(o, 0.f, -140.f, 130.f, Position);
							Vector((float)(rand() % 360), 0.0f, (float)(rand() % 360), vAngle);

							CreateJoint(MODEL_FENRIR_SKILL_THUNDER, Position, p_o[i]->Position, vAngle, 0 + GetFenrirType(c), p_o[i], 100.f);
							CreateJoint(MODEL_FENRIR_SKILL_THUNDER, Position, p_o[i]->Position, vAngle, 4 + GetFenrirType(c), p_o[i], 80.f);
						}
					}

					for (int k = 0; k < 6; k++)
					{
						CalcAddPosition(o, 0.f, 10.f + (rand() % 40 - 20), 130.f, Position);
						Vector((float)(rand() % 360), 0.0f, (float)(rand() % 360), vAngle);
						CreateJoint(BITMAP_FLARE_FORCE, Position, Position, vAngle, 11 + GetFenrirType(c), NULL, 60.f);
					}
				}
				break;
			}
			case AT_SKILL_RUSH:
				CreateEffect(MODEL_SWORD_FORCE, o->Position, o->Angle, Light, 0, o);
				PlayBuffer(SOUND_BCS_RUSH);
				break;
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
			case AT_SKILL_OCCUPY:
			{
				CreateEffect(MODEL_SWORD_FORCE, o->Position, o->Angle, Light, 2, o);
				PlayBuffer(SOUND_BCS_RUSH);
			}
			break;
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
			case AT_SKILL_ONEFLASH:
				break;
			case AT_SKILL_BRAND_OF_SKILL:
				Vector(0.f, 0.f, 0.f, p);
				Vector(1.f, 1.f, 1.f, Light);
				if (c->Weapon[0].Type != MODEL_BOW + 15)
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 0);
				}
				if (c->Weapon[1].Type != MODEL_BOW + 7 && (c->Weapon[1].Type < MODEL_SHIELD || c->Weapon[1].Type >= MODEL_SHIELD + MAX_ITEM_INDEX))
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[1].LinkBone], p, Position, true);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 1);
				}
				CreateEffect(MODEL_MANA_RUNE, o->Position, o->Angle, o->Light);

				PlayBuffer(SOUND_BCS_BRAND_OF_SKILL);
				break;
			case AT_SKILL_ALICE_BERSERKER:
			case Skill_Berserker_Mastery:
			case Skill_Berserker_Proficiency:
			case Skill_Berserker_Strengthener:
				Vector(1.0f, 0.1f, 0.2f, Light);
				CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, Light, 11, o);
				CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o->Position, o->Angle, Light, 0, o);
				CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT2, o->Position, o->Angle, Light, 0, o);
				PlayBuffer(SOUND_SKILL_BERSERKER);
				break;
			case AT_SKILL_ALICE_WEAKNESS:
			case Skill_Weakness_Strengthener:
				Vector(2.0f, 0.1f, 0.1f, Light);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 7.0f);
				Vector(2.0f, 0.4f, 0.3f, Light);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 2.0f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 1.0f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 0.2f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 0.1f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT2, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT22, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT222, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				Vector(1.4f, 0.2f, 0.2f, Light);
				CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 0.5f);
				CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 1.f);
				PlayBuffer(SOUND_SKILL_WEAKNESS);
				break;
			case AT_SKILL_ALICE_ENERVATION:
			case Skill_Innovation_Strengthener:
				Vector(0.25f, 1.0f, 0.7f, Light);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 7.0f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 2.0f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 1.0f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 0.2f);
				CreateEffect(BITMAP_MAGIC_ZIN, o->Position, o->Angle, Light, 2, NULL, -1, 0, 0, 0, 0.1f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT2, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT22, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_SUMMONER_CASTING_EFFECT222, o->Position, o->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 0.5f);
				CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, Light, 0, NULL, -1, 0, 0, 0, 1.f);
				PlayBuffer(SOUND_SKILL_ENERVATION);
				break;
			case AT_SKILL_FLAME_STRIKE:
			case Skill_Flame_Strike_Mastery:
			case Skill_Flame_Strike_Strengthener:
			{
				//DeleteEffect(MODEL_EFFECT_FLAME_STRIKE, o, 0);
				CreateEffect(MODEL_EFFECT_FLAME_STRIKE, o->Position, o->Angle, o->Light, 0, o);
				PlayBuffer(SOUND_SKILL_FLAME_STRIKE);
			}
			break;
			case AT_SKILL_GIGANTIC_STORM:
			case Skill_Gigantic_Storm_Strengthener:
			{
				vec34_t Matrix;
				vec3_t vAngle, vDirection, vPosition;
				float fAngle;
				for (int i = 0; i < 5; ++i)
				{
					Vector(0.f, 200.f, 0.f, vDirection);
					fAngle = o->Angle[2] + i * 72.f;
					Vector(0.f, 0.f, fAngle, vAngle);
					AngleMatrix(vAngle, Matrix);
					VectorRotate(vDirection, Matrix, vPosition);
					VectorAdd(vPosition, o->Position, vPosition);

					CreateEffect(BITMAP_JOINT_THUNDER, vPosition, o->Angle, o->Light);
				}
				PlayBuffer(SOUND_SKILL_GIGANTIC_STORM);
			}
			break;
			case AT_SKILL_LIGHTNING_SHOCK:
			case Skill_Lightning_Shock_Strengthener:
			{
				vec3_t vLight;
				Vector(1.0f, 1.0f, 1.0f, vLight);

				CreateEffect(MODEL_LIGHTNING_SHOCK, o->Position, o->Angle, vLight, 0, o);
				PlayBuffer(SOUND_SKILL_LIGHTNING_SHOCK);
			}
			break;
			case AT_SKILL_THRUST:
			{
				o->Angle[2] = CreateAngle(o->Position[0], o->Position[1], c->TargetPosition[0], c->TargetPosition[1]);
				o->m_sTargetIndex = c->TargetCharacter;
			}
			break;
			case AT_SKILL_STAMP:
			{
				o->m_sTargetIndex = c->TargetCharacter;
			}
			break;
			}

			if (c->TargetCharacter == -1)
			{
				BYTE Skill = 0;
				if ((c->Skill) == AT_SKILL_CROSSBOW || (c->Skill) == Skill_Triple_Shot_Strengthener || (c->Skill) == Skill_Triple_Shot_Mastery)
					Skill = 1;
				if ((o->Type == MODEL_PLAYER &&
					(o->CurrentAction == PLAYER_HIT_BOW || o->CurrentAction == PLAYER_HIT_CROSSBOW ||
						o->CurrentAction == PLAYER_HIT_FLY_BOW || o->CurrentAction == PLAYER_HIT_FLY_CROSSBOW ||
						o->CurrentAction == PLAYER_FENRIR_ATTACK_BOW || o->CurrentAction == PLAYER_FENRIR_ATTACK_CROSSBOW ||
						o->CurrentAction == PLAYER_HIT_RIDE_BOW || o->CurrentAction == PLAYER_HIT_RIDE_CROSSBOW)) ||
					o->Type != MODEL_PLAYER && o->Kind == KIND_PLAYER
					)
				{
					if (AT_SKILL_MULTI_SHOT != (c->Skill))
						CreateArrows(c, o, NULL, FindHotKey((c->Skill)), Skill, (c->Skill));
				}

				if (o->Type == MODEL_MONSTER01 + 22 || o->Type == MODEL_MONSTER01 + 35 || o->Type == MODEL_MONSTER01 + 40
					|| o->Type == MODEL_MONSTER01 + 46)
				{
					CreateArrows(c, o, NULL, 0, 0);
				}
			}
			else
			{
				CHARACTER* tc = gmCharacters->GetCharacter(c->TargetCharacter);

				if (!tc)
					return;

				OBJECT* to = &tc->Object;
				if (o->Type == MODEL_PLAYER &&
					(o->CurrentAction == PLAYER_HIT_BOW || o->CurrentAction == PLAYER_HIT_CROSSBOW ||
						o->CurrentAction == PLAYER_HIT_FLY_BOW || o->CurrentAction == PLAYER_HIT_FLY_CROSSBOW ||
						o->CurrentAction == PLAYER_HIT_RIDE_BOW || o->CurrentAction == PLAYER_HIT_RIDE_CROSSBOW
						|| o->CurrentAction == PLAYER_FENRIR_ATTACK_BOW || o->CurrentAction == PLAYER_FENRIR_ATTACK_CROSSBOW	//^ 펜릴 스킬 관련(요정 화살 나가게 하는 것)
						))
				{
					if (AT_SKILL_MULTI_SHOT != (c->Skill))
						CreateArrows(c, o, to, FindHotKey((c->Skill)), 0, (c->Skill));
				}

				if (o->Type == MODEL_MONSTER01 + 22 || o->Type == MODEL_MONSTER01 + 35 || o->Type == MODEL_MONSTER01 + 40)
				{
					CreateArrows(c, o, to, 0, 0);
				}

				if (tc->Hit >= 1)
				{
					if (to->Type != MODEL_MONSTER01 + 7)
					{
						for (int i = 0; i < 10; i++)
						{
							Vector(to->Position[0] + (float)(rand() % 64 - 32), to->Position[1] + (float)(rand() % 64 - 32), to->Position[2] + (float)(rand() % 64 + 90), Position);
							CreateParticle(BITMAP_BLOOD + 1, Position, o->Angle, Light);
						}
					}

					if (to->Type == MODEL_MONSTER01 + 60)
					{
						for (int i = 0; i < 5; i++)
						{
							if ((rand() % 2) == 0)
							{
								Position[0] = to->Position[0];
								Position[1] = to->Position[1];
								Position[2] = to->Position[2] + 50 + rand() % 30;

								CreateEffect(MODEL_STONE_COFFIN + 1, Position, o->Angle, o->Light);
							}
						}
						PlayBuffer(SOUND_HIT_CRISTAL);
					}
					battleCastle::RenderMonsterHitEffect(to);
				}
				if (o->CurrentAction >= PLAYER_HIT_SKILL_SWORD1 && o->CurrentAction <= PLAYER_HIT_SKILL_SWORD5)
				{
					CreateSpark(0, tc, to->Position, o->Angle);
				}
				vec3_t Angle;
				VectorCopy(o->Angle, Angle);
				Angle[2] = CreateAngle(o->Position[0], o->Position[1], to->Position[0], to->Position[1]);
				switch ((c->Skill))
				{
				case Skill_Triple_Shot_Strengthener:
				case Skill_Triple_Shot_Mastery:
				case AT_SKILL_CROSSBOW:
					CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 1);
				case Skill_Penetration_Strengthener:
				case AT_SKILL_PIERCING:
					CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 0, (c->Skill));
					break;
				case Skill_Poison_Arrow:
				case Skill_Poison_Arrow_Strengthener:
					CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 0, (c->Skill));
					break;
				case Skill_Ice_Arrow_Strengthener:
				case AT_SKILL_PARALYZE:
					CreateArrows(c, o, NULL, FindHotKey((c->Skill)), 0, (c->Skill));
					break;
				case AT_SKILL_DEEPIMPACT:
					CreateArrows(c, o, to, FindHotKey((c->Skill)), 0, (c->Skill));
					PlayBuffer(SOUND_BCS_DEEP_IMPACT);
					break;
				case Skill_Cure:
				case Skill_Party_Healing:
				case Skill_Party_Healing_Strengthener:
				case Skill_Heal_Strengthener:
				case AT_SKILL_HEALING:
					CreateEffect(BITMAP_MAGIC + 1, to->Position, to->Angle, to->Light, 1, to);
					break;
				case Skill_Defense_Increase_Strengthener:
				case Skill_Defense_Increase_Mastery:
				case AT_SKILL_DEFENSE:
					if (c->SkillSuccess)
					{
						if (g_isCharacterBuff(o, eBuff_Cloaking))
						{
							break;
						}

						CreateEffect(BITMAP_MAGIC + 1, to->Position, to->Angle, to->Light, 2, to);

						if (!g_isCharacterBuff(to, eBuff_Defense))
						{
							g_CharacterRegisterBuff(to, eBuff_Defense);

							for (int j = 0; j < 5; ++j)
							{
								CreateJoint(MODEL_SPEARSKILL, to->Position, to->Position, to->Angle, 4, to, 20.0f, -1, 0, 0, c->TargetCharacter);
							}
						}
						else if (!SearchJoint(MODEL_SPEARSKILL, to, 4)
							&& !SearchJoint(MODEL_SPEARSKILL, to, 9))
						{
							for (int j = 0; j < 5; ++j)
							{
								CreateJoint(MODEL_SPEARSKILL, to->Position, to->Position, to->Angle, 4, to, 20.0f, -1, 0, 0, c->TargetCharacter);
							}
						}
					}
					break;
				case AT_SKILL_ATTACK:
				case Skill_Attack_Increase_Strengthener:
				case Skill_Attack_Increase_Mastery:
					if (!g_isCharacterBuff(o, eBuff_Cloaking))
					{
						CreateEffect(BITMAP_MAGIC + 1, to->Position, to->Angle, to->Light, 3, to);
						if (c->SkillSuccess)
						{
							g_CharacterRegisterBuff(to, eBuff_Attack);
						}
					}
					break;
				case AT_SKILL_SLOW:
					CreateEffect(MODEL_ICE, to->Position, o->Angle, Light);

					for (int i = 0; i < 5; i++)
						CreateEffect(MODEL_ICE_SMALL, to->Position, o->Angle, o->Light);

					if (c->SkillSuccess)
					{
						if (!g_isCharacterBuff(to, eDeBuff_Freeze))
						{
							g_CharacterRegisterBuff(to, eDeBuff_Freeze);
						}
					}
					PlayBuffer(SOUND_ICE);
					break;
				case Skill_Soul_Barrier_Strengthener:
				case Skill_Soul_Barrier_Proficiency:
				case Skill_Soul_Barrier_Mastery:
				case Skill_Soul_Barrier:
					if (o->Type == MODEL_MONSTER01 + 55)
					{
						g_CharacterRegisterBuff(o, eBuff_PhysDefense);
					}
					else
					{
						if (g_isCharacterBuff(to, eBuff_Cloaking))
							break;
						g_CharacterRegisterBuff(to, eBuff_PhysDefense);

						PlayBuffer(SOUND_SOULBARRIER);
						DeleteJoint(MODEL_SPEARSKILL, to, 0);
						for (int j = 0; j < 5; ++j)
						{
							CreateJoint(MODEL_SPEARSKILL, to->Position, to->Position, to->Angle, 0, to, 20.0f);
						}
					}
					break;
				case AT_SKILL_POISON:
				case Skill_Poison_Strengthener:
					if (o->Type == MODEL_PLAYER)
						CreateEffect(MODEL_POISON, to->Position, o->Angle, o->Light);
					Vector(0.4f, 0.6f, 1.f, Light);

					for (int i = 0; i < 10; i++)
						CreateParticle(BITMAP_SMOKE, to->Position, o->Angle, Light, 1);

					if (c->SkillSuccess)
					{
						g_CharacterRegisterBuff(to, eDeBuff_Poison);
					}
					PlayBuffer(SOUND_HEART);
					break;
				case AT_SKILL_METEO:
				case Skill_Meteor_Strengthener:
				case Skill_Meteor_Mastery:
					CreateEffect(MODEL_FIRE, to->Position, to->Angle, o->Light);
					PlayBuffer(SOUND_METEORITE01);
					break;
				case AT_SKILL_JAVELIN:
					CreateEffect(MODEL_SKILL_JAVELIN, o->Position, o->Angle, o->Light, 0, to);
					CreateEffect(MODEL_SKILL_JAVELIN, o->Position, o->Angle, o->Light, 1, to);
					CreateEffect(MODEL_SKILL_JAVELIN, o->Position, o->Angle, o->Light, 2, to);

					PlayBuffer(SOUND_BCS_JAVELIN);
					break;
				case AT_SKILL_DEATH_CANNON:
					Vector(0.f, 0.f, o->Angle[2], Angle);
					VectorCopy(o->Position, Position);

					Position[2] += 130.f;
					CreateJoint(BITMAP_JOINT_FORCE, Position, Position, Angle, 4, NULL, 40.f);
					PlayBuffer(SOUND_BCS_DEATH_CANON);
					break;
				case Skill_Fire_Blast:
					CreateEffect(MODEL_PIER_PART, o->Position, o->Angle, o->Light, 2, to);
					PlayBuffer(SOUND_BCS_SPACE_SPLIT);
					break;
				case AT_SKILL_FIREBALL:
					CreateEffect(MODEL_FIRE, o->Position, Angle, o->Light, 1, to);
					PlayBuffer(SOUND_METEORITE01);
					break;
				case AT_SKILL_FLAME:
				case Skill_Flame_Strengthener:
				case Skill_Flame_Strengthener2:
					Position[0] = to->Position[0];
					Position[1] = to->Position[1];
					Position[2] = RequestTerrainHeight(Position[0], Position[1]);
					CreateEffect(BITMAP_FLAME, Position, o->Angle, o->Light, 5, o, o->PKKey, FindHotKey(AT_SKILL_FLAME));
					PlayBuffer(SOUND_FLAME);
					break;
				case AT_SKILL_POWERWAVE:
					if (o->Type == MODEL_MONSTER01 + 18)
					{
						Angle[2] += 10.f;
						CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light);
						Angle[2] -= 20.f;
						CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light);
						Angle[2] += 10.f;
					}
					CreateEffect(MODEL_MAGIC2, o->Position, Angle, o->Light);
					PlayBuffer(SOUND_MAGIC);
					break;
				case Skill_Force_Wave_Strengthener:
				case AT_SKILL_STRONG_PIER:
					CreateEffect(MODEL_WAVES, o->Position, o->Angle, o->Light, 1);
					CreateEffect(MODEL_WAVES, o->Position, o->Angle, o->Light, 1);
					CreateEffect(MODEL_PIERCING2, o->Position, o->Angle, o->Light);
					PlayBuffer(SOUND_ATTACK_SPEAR);
					break;
				case Skill_Fire_Burst_Strengthener:
				case Skill_Fire_Burst_Mastery:
				case Skill_Fire_Burst:
				{
					vec3_t Angle = { 0.f, 0.f, o->Angle[2] };
					vec3_t Pos = { 0.f, 0.f, (to->BoundingBoxMax[2] / 1.f) };

					Vector(80.f, 0.f, 20.f, p);
					b->TransformPosition(o->BoneTransform[0], p, Position, true);
					Angle[2] = o->Angle[2] + 90;
					CreateEffect(MODEL_PIER_PART, Position, Angle, Pos, 0, to);
					Pos[2] -= to->BoundingBoxMax[2] / 2;
					Angle[2] = o->Angle[2];
					CreateEffect(MODEL_PIER_PART, Position, Angle, Pos, 0, to);
					Angle[2] = o->Angle[2] - 90;
					CreateEffect(MODEL_PIER_PART, Position, Angle, Pos, 0, to);

					Vector(1.f, 0.6f, 0.3f, Light);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 0);
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 1);
				}
				break;
				case AT_SKILL_ENERGYBALL:
					switch (c->MonsterIndex)
					{
					case 37:
					case 46:
					case 61:
					case 66:
					case 69:
					case 70:
					case 73:
					case 75:
					case 77:
					case 89:
					case 95:
					case 112:
					case 118:
					case 124:
					case 130:
					case 143:
					case 87:
					case 93:
					case 99:
					case 116:
					case 122:
					case 128:
					case 141:
					case 163:
					case 165:
					case 167:
					case 169:
					case 171:
					case 173:
					case 427:
					case 303:
					case 293:
						break;
					default:
						if (o->Type == MODEL_MONSTER01 + 12)
						{
							CreateEffect(MODEL_SNOW1, o->Position, Angle, o->Light, 0, to);
						}
						else if (o->Type == MODEL_MONSTER01 + 134)
						{
							CreateEffect(MODEL_WOOSISTONE, o->Position, Angle, o->Light, 0, to);
						}
						else if (o->Type == MODEL_MONSTER01 + 138)
						{
							vec3_t vLight;
							Vector(1.0f, 1.0f, 1.0f, vLight);
							CreateEffect(MODEL_EFFECT_SAPITRES_ATTACK, o->Position, o->Angle, vLight, 0, to);
						}
						else
						{
							CreateEffect(BITMAP_ENERGY, o->Position, Angle, o->Light, 0, to);
							PlayBuffer(SOUND_MAGIC);
						}
						break;
					}
					break;
				case AT_SKILL_ALICE_LIGHTNINGORB:
				{
					vec3_t vLight;
					Vector(1.0f, 1.0f, 1.0f, vLight);
					CreateEffect(MODEL_LIGHTNING_ORB, o->Position, o->Angle, vLight, 0, to);
					PlayBuffer(SOUND_SUMMON_SKILL_LIGHTORB);
				}
				break;
				case AT_SKILL_ALICE_SLEEP:
				case AT_SKILL_ALICE_BLIND:
				case Skill_Sleep_Strengthener:
				case AT_SKILL_ALICE_THORNS:
				{
					int iSkillIndex = (c->Skill);
					vec3_t vLight;

					if (iSkillIndex == AT_SKILL_ALICE_SLEEP || iSkillIndex == Skill_Sleep_Strengthener)
					{
						Vector(0.7f, 0.3f, 0.8f, vLight);
					}
					else if (iSkillIndex == AT_SKILL_ALICE_BLIND)
					{
						Vector(1.0f, 1.0f, 1.0f, vLight);
					}
					else if (iSkillIndex == AT_SKILL_ALICE_THORNS)
					{
						Vector(0.8f, 0.5f, 0.2f, vLight);
					}
					if (iSkillIndex == AT_SKILL_ALICE_SLEEP || iSkillIndex == AT_SKILL_ALICE_THORNS || iSkillIndex == Skill_Sleep_Strengthener)
					{
						CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, vLight, 11, o);
					}
					else if (iSkillIndex == AT_SKILL_ALICE_BLIND)
					{
						CreateEffect(BITMAP_MAGIC + 1, o->Position, o->Angle, vLight, 12, o);
					}

					if (iSkillIndex == AT_SKILL_ALICE_SLEEP || iSkillIndex == Skill_Sleep_Strengthener)
					{
						Vector(0.8f, 0.3f, 0.9f, vLight);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, to->Position, to->Angle, vLight, 0, to);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT2, to->Position, to->Angle, vLight, 0, to);
					}
					else if (iSkillIndex == AT_SKILL_ALICE_BLIND)
					{
						Vector(1.0f, 1.0f, 1.0f, vLight);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, to->Position, to->Angle, vLight, 1, to);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT2, to->Position, to->Angle, vLight, 1, to);
					}
					else if (iSkillIndex == AT_SKILL_ALICE_THORNS)
					{
						Vector(0.8f, 0.5f, 0.2f, vLight);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, to->Position, to->Angle, vLight, 2, to);
						CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT2, to->Position, to->Angle, vLight, 2, to);
					}
				}
				break;
				case Skill_Chain_Lightning_Strengthener:
				case AT_SKILL_ALICE_CHAINLIGHTNING:
				{
					PlayBuffer(SOUND_SKILL_CHAIN_LIGHTNING);
				}
				break;
				case Skill_Drain_Life_Strengthener:
				case Skill_Drain_Life_Mastery:
				case AT_SKILL_ALICE_DRAINLIFE:
				{
					CHARACTER* pTargetChar = gmCharacters->GetCharacter(c->TargetCharacter);
					OBJECT* pSourceObj = o;
					pSourceObj->Owner = &(pTargetChar->Object);

					CreateEffect(MODEL_ALICE_DRAIN_LIFE, pSourceObj->Position, pSourceObj->Angle, pSourceObj->Light, 0, pSourceObj);
					PlayBuffer(SOUND_SKILL_DRAIN_LIFE);
				}
				break;
				}

				VectorCopy(to->Position, Position);
				Position[2] += 120.f;

				int Hand = 0;
				if (o->CurrentAction == PLAYER_HIT_SWORD_LEFT1 || o->CurrentAction == PLAYER_HIT_SWORD_LEFT2)
					Hand = 1;

				if (tc == Hero)
				{
					Vector(1.f, 0.f, 0.f, Light);
				}
				else
				{
					Vector(1.f, 0.6f, 0.f, Light);
				}

				switch (c->AttackFlag)
				{
				case ATTACK_DIE:
					CreateJoint(BITMAP_JOINT_ENERGY, to->Position, to->Position, o->Angle, 0, o, 20.f);
					CreateJoint(BITMAP_JOINT_ENERGY, to->Position, to->Position, o->Angle, 1, o, 20.f);
					break;
				}
				switch ((c->Skill))
				{
				case AT_SKILL_HEALING:
				case AT_SKILL_ATTACK:
				case AT_SKILL_DEFENSE:
				case Skill_Bless:
				case Skill_Summon_Satyros:
				case Skill_Bless_Strengthener:

				case Skill_Cure:
				case Skill_Party_Healing:
				case Skill_Party_Healing_Strengthener:
				case Skill_Defense_Increase_Strengthener:
				case Skill_Defense_Increase_Mastery:
				case Skill_Attack_Increase_Strengthener:
				case Skill_Attack_Increase_Mastery:

				case AT_SKILL_SUMMON:
				case AT_SKILL_SUMMON + 1:
				case AT_SKILL_SUMMON + 2:
				case AT_SKILL_SUMMON + 3:
				case AT_SKILL_SUMMON + 4:
				case AT_SKILL_SUMMON + 5:
				case AT_SKILL_SUMMON + 6:
				case AT_SKILL_SUMMON + 7:
				case AT_SKILL_ONETOONE:
				case Skill_Impale:

				case Skill_Soul_Barrier_Strengthener:
				case Skill_Soul_Barrier_Proficiency:
				case Skill_Soul_Barrier_Mastery:
				case AT_SKILL_WIZARDDEFENSE:

				case Skill_Swell_Life_Strengthener:
				case Skill_Swell_Life_Proficiency:
				case Skill_Swell_Life_Mastery:
				case AT_SKILL_VITALITY:
				case Skill_Nova_Strengthener:
				case AT_SKILL_BLAST_HELL:
				case Skill_Critical_DMG_Increase_PowUp1:
				case Skill_Critical_DMG_Increase_PowUp2:
				case Skill_Critical_DMG_Increase_PowUp3:
				case Skill_Critical_DMG_Increase_Mastery:
				case AT_SKILL_ADD_CRITICAL:

				case AT_SKILL_PARTY_TELEPORT:
				case AT_SKILL_STUN:
				case AT_SKILL_REMOVAL_STUN:
				case AT_SKILL_MANA:
				case AT_SKILL_INVISIBLE:
				case AT_SKILL_REMOVAL_BUFF:
				case AT_SKILL_BRAND_OF_SKILL:
					break;
				default:
					if (68 <= c->MonsterIndex && c->MonsterIndex <= 75)
					{
					}
					else
					{
						ITEM* r = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT];
						ITEM* l = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT];

						if ((r->Type >= ITEM_BOW && r->Type < ITEM_BOW + MAX_ITEM_INDEX) && (l->Type >= ITEM_BOW && l->Type < ITEM_BOW + MAX_ITEM_INDEX))
						{
							PlayBuffer(SOUND_ATTACK01 + 5 + rand() % 4, o);
						}
						else
						{
							PlayBuffer(SOUND_ATTACK01 + rand() % 4, o);
						}
					}
					break;
				}
			}

			c->Skill = 0;
			c->Damage = 0;
			c->AttackFlag = ATTACK_FAIL;
		}

		if (c->m_iDeleteTime > 0)
		{
			c->m_iDeleteTime -= timefac(1.0f);
		}
		if (c->m_iDeleteTime != -128 && c->m_iDeleteTime <= 0)
		{
			c->m_iDeleteTime = -128;
			DeleteCharacter(c, o);
		}
		battleCastle::SetBuildTimeLocation(o);

		CreateWeaponBlur(c, o, b);

		switch (o->Type)
		{
		case MODEL_BALL:
			CreateFire(0, o, 0.f, 0.f, 0.f);
			break;
		}

		giPetManager::MovePet(c);

		if (c->Dead > 0)
		{
			if (g_isCharacterBuff(o, eBuff_BlessPotion))
				g_CharacterUnRegisterBuff(o, eBuff_BlessPotion);
			if (g_isCharacterBuff(o, eBuff_SoulPotion))
				g_CharacterUnRegisterBuff(o, eBuff_SoulPotion);
		}
	}
}

void PlayWalkSound()
{
	OBJECT* o = &Hero->Object;

	if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
	{
		if (World == WD_2DEVIAS && (HeroTile != 3 && HeroTile < 10))
		{
			PlayBuffer(SOUND_HUMAN_WALK_SNOW);
		}
		else if (World == WD_0LORENCIA && HeroTile == 0)
		{
			PlayBuffer(SOUND_HUMAN_WALK_GRASS);
		}
		else if (World == WD_3NORIA && HeroTile == 0)
		{
			PlayBuffer(SOUND_HUMAN_WALK_GRASS);
		}
		else if ((World == WD_7ATLANSE || gMapManager->InHellas() || World == WD_67DOPPLEGANGER3) && !Hero->SafeZone)
		{
			PlayBuffer(SOUND_HUMAN_WALK_SWIM);
		}
		else if (IsIceCity())
		{
			PlayBuffer(SOUND_HUMAN_WALK_SNOW);
		}
		else if (IsSantaTown())
		{
			PlayBuffer(SOUND_HUMAN_WALK_SNOW);
		}
		else
		{
			PlayBuffer(SOUND_HUMAN_WALK_GROUND);
		}
	}
}

bool CheckFullSet(CHARACTER* c)
{
	int tmpLevel = 10;
	bool Success = true;
	int  start = 5, end = 1;

	EquipmentLevelSet = 0;

	bool bHero = false;

	if (c == Hero)
	{
		bHero = true;
		start = EQUIPMENT_BOOTS;
		end = EQUIPMENT_HELM;
	}

	if (c->GetBaseClass() == CLASS_DARK)
	{
		end = EQUIPMENT_ARMOR;
	}

	if (bHero == true)
	{
		for (int i = start; i >= end; i--)
		{
#ifdef PBG_ADD_NEWCHAR_MONK
			if ((c->GetBaseClass() == CLASS_RAGEFIGHTER) && (i == EQUIPMENT_GLOVES))
				continue;
#endif //PBG_ADD_NEWCHAR_MONK

			if (CharacterMachine->Equipment[i].Type == -1)
			{
				Success = false;
				break;
			}
		}

		if (Success)
		{
			int Type = CharacterMachine->Equipment[EQUIPMENT_BOOTS].Type % MAX_ITEM_INDEX;
			tmpLevel = (CharacterMachine->Equipment[EQUIPMENT_BOOTS].Level >> 3) & 15;
			for (int i = start; i >= end; i--)
			{
#ifdef PBG_ADD_NEWCHAR_MONK
				if ((c->GetBaseClass() == CLASS_RAGEFIGHTER) && (i == EQUIPMENT_GLOVES))
					continue;
#endif //PBG_ADD_NEWCHAR_MONK
				int Level = (CharacterMachine->Equipment[i].Level >> 3) & 15;
				if (Level < 9)
				{
					EquipmentLevelSet = 0;
					Success = false;
					break;
				}
				if (Type != (CharacterMachine->Equipment[i].Type % MAX_ITEM_INDEX))
				{
					EquipmentLevelSet = 0;
					Success = false;
					break;
				}

				if (Level >= 9 && tmpLevel >= Level)
				{
					tmpLevel = Level;
					EquipmentLevelSet = Level;
				}
			}
		}

		g_bAddDefense = true;

		if (c->GetBaseClass() == CLASS_DARK && Success)
		{
			if (CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 15
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 20
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 23
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 32
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 37
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 47
				&& CharacterMachine->Equipment[EQUIPMENT_ARMOR].Type != ITEM_ARMOR + 48
				)
			{
				g_bAddDefense = false;
			}
		}
	}
	else
	{
		for (int i = 5; i >= end; i--)
		{
			if (c->BodyPart[i].Type == -1)
			{
				Success = false;
				break;
			}
		}

		if (Success)
		{
			int Type = (c->BodyPart[5].Type - MODEL_ITEM) % MAX_ITEM_INDEX;
			tmpLevel = c->BodyPart[5].Level & 0xf;

			for (int i = 5; i >= end; i--)
			{
				int Level = c->BodyPart[i].Level & 0xf;
				if (Level < 9)
				{
					EquipmentLevelSet = 0;
					Success = false;
					break;
				}
				if (Type != (c->BodyPart[i].Type - MODEL_ITEM) % MAX_ITEM_INDEX)
				{
					EquipmentLevelSet = 0;
					Success = false;
					break;
				}

				if (Level >= 9 && tmpLevel >= Level)
				{
					tmpLevel = Level;
					EquipmentLevelSet = Level;
				}
			}
		}

		g_bAddDefense = true;

		if (c->GetBaseClass() == CLASS_DARK && Success)
		{
			if (c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 15
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 20
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 23
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 32
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 37
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 47
				&& c->BodyPart[BODYPART_ARMOR].Type != ITEM_ARMOR + 48
				)
			{
				g_bAddDefense = false;
			}
		}
	}

	if (gMapManager->InChaosCastle())
	{
		ChangeChaosCastleUnit(c);
	}
	return Success;
}

void MoveEye(OBJECT* o, BMD* b, int Right, int Left, int Right2, int Left2, int Right3, int Left3)
{
	vec3_t p;
	Vector(0.f, 0.f, 0.f, p);
	b->TransformPosition(o->BoneTransform[Right], p, o->EyeRight, true);

	Vector(0.f, 0.f, 0.f, p);
	b->TransformPosition(o->BoneTransform[Left], p, o->EyeLeft, true);

	if (Right2 != -1)
		b->TransformPosition(o->BoneTransform[Right2], p, o->EyeRight2, true);
	if (Left2 != -1)
		b->TransformPosition(o->BoneTransform[Left2], p, o->EyeLeft2, true);
	if (Right3 != -1)
		b->TransformPosition(o->BoneTransform[Right3], p, o->EyeRight3, true);
	if (Left3 != -1)
		b->TransformPosition(o->BoneTransform[Left3], p, o->EyeLeft3, true);
}

void MonsterDieSandSmoke(OBJECT* o)
{
	if (o->CurrentAction == MONSTER01_DIE && o->AnimationFrame >= 8.f && o->AnimationFrame < 9.f)
	{
		vec3_t Position;
		for (int i = 0; i < 20; i++)
		{
			Vector(1.f, 1.f, 1.f, o->Light);
			Position[0] = o->Position[0] + (float)(rand() % 64 - 32);
			Position[1] = o->Position[1] + (float)(rand() % 64 - 32);
			Position[2] = o->Position[2] + (float)(rand() % 32 - 16);
			CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, o->Light, 1);
		}
	}
}

void MonsterMoveSandSmoke(OBJECT* o)
{
	if (o->CurrentAction == MONSTER01_WALK)
	{
		vec3_t Position;
		Vector(o->Position[0] + rand() % 200 - 100, o->Position[1] + rand() % 200 - 100, o->Position[2], Position);
		CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, o->Light);
	}
}

void MoveCharacterVisual(CHARACTER* c, OBJECT* o)
{
	BMD* b = gmClientModels->GetModel(o->Type);
	if (b->NumActions == 0)
	{
		VectorCopy(o->BoundingBoxMin, o->OBB.StartPos);
		o->OBB.XAxis[0] = (o->BoundingBoxMax[0] - o->BoundingBoxMin[0]);
		o->OBB.YAxis[1] = (o->BoundingBoxMax[1] - o->BoundingBoxMin[1]);
		o->OBB.ZAxis[2] = (o->BoundingBoxMax[2] - o->BoundingBoxMin[2]);
		interpolatesum2(o->OBB.StartPos, o->Position, o->OBB.StartPos);

		o->OBB.XAxis[1] = 0.f;
		o->OBB.XAxis[2] = 0.f;
		o->OBB.YAxis[0] = 0.f;
		o->OBB.YAxis[2] = 0.f;
		o->OBB.ZAxis[0] = 0.f;
		o->OBB.ZAxis[1] = 0.f;
		return;
	}
	VectorCopy(o->Position, b->BodyOrigin);
	b->BodyScale = o->Scale;
	b->CurrentAction = o->CurrentAction;

	if (o->Visible)
	{
		int Index = TERRAIN_INDEX_REPEAT((c->PositionX), (c->PositionY));
		if ((TerrainWall[Index] & TW_SAFEZONE) == TW_SAFEZONE)
			c->SafeZone = true;
		else
			c->SafeZone = false;

		if (!g_isCharacterBuff(o, eDeBuff_Harden) && !g_isCharacterBuff(o, eDeBuff_Stun)
			&& !g_isCharacterBuff(o, eDeBuff_Sleep)
			&& !g_isCharacterBuff(o, eBuff_Att_up_Ourforces)
			&& !g_isCharacterBuff(o, eBuff_Hp_up_Ourforces)
			&& !g_isCharacterBuff(o, eBuff_Def_up_Ourforces)
			)
		{
			if (o->Type != MODEL_PLAYER)
				MoveHead(c);

			if (c != Hero && c->Dead == 0 && rand() % 32 == 0)
			{
				o->HeadTargetAngle[0] = (float)(rand() % 128 - 64);
				o->HeadTargetAngle[1] = (float)(rand() % 32 - 16);
				for (int i = 0; i < 2; i++)
				{
					if (o->HeadTargetAngle[i] < 0)
					{
						o->HeadTargetAngle[i] += 360.f;
					}
				}
			}

			for (int j = 0; j < 2; j++)
				o->HeadAngle[j] = TurnAngle2(o->HeadAngle[j], o->HeadTargetAngle[j], FarAngle(o->HeadAngle[j], o->HeadTargetAngle[j]) * 0.2f);
		}

		vec3_t p, Position;
		vec3_t Light;
		float Luminosity = 0.8f;
		if (c->Appear > 0)
		{
			c->Appear -= timefac(1.f);
			for (int i = 0; i < 20; i++)
			{
				Vector(1.f, 1.f, 1.f, o->Light);
				Vector(o->Position[0] + (float)(rand() % 64 - 32), o->Position[1] + (float)(rand() % 64 - 32), o->Position[2] + (float)(rand() % 32 - 16), Position);
				if (rand() % 10 == 0)
					CreateParticleSync(BITMAP_SMOKE + 1, Position, o->Angle, o->Light, 1);
				if (rand() % 10 == 0)
					CreateEffectSync(MODEL_STONE1 + rand() % 2, o->Position, o->Angle, o->Light);
			}
		}
		if (c->PK < PVP_MURDERER2)
		{
			for (int j = 0; j < 2; j++)
			{
				if (c->Weapon[j].Type == MODEL_SWORD + 12)
				{
					Vector(Luminosity, Luminosity * 0.8f, Luminosity * 0.5f, Light);
					AddTerrainLight(o->Position[0], o->Position[1], Light, 3, PrimaryTerrainLight);
				}
				else if (c->Weapon[j].Type == MODEL_SWORD + 19 || c->Weapon[j].Type == MODEL_BOW + 18 || c->Weapon[j].Type == MODEL_STAFF + 10 || c->Weapon[j].Type == MODEL_MACE + 13)
				{
					Vector(Luminosity * 0.8f, Luminosity * 0.5f, Luminosity * 0.3f, Light);
					AddTerrainLight(o->Position[0], o->Position[1], Light, 2, PrimaryTerrainLight);
				}
			}
		}
		if (c->Freeze > 0.f)
		{
			if (c->FreezeType == BITMAP_ICE && rand() % 4 == 0)
			{
				Vector(o->Position[0] + (float)(rand() % 100 - 50), o->Position[1] + (float)(rand() % 100 - 50), o->Position[2] + (float)(rand() % 180), Position);
				//CreateParticle(BITMAP_SHINY,Position,o->Angle,o->Light);
				//CreateParticle(BITMAP_SHINY,Position,o->Angle,o->Light,1);
			}
			if (c->FreezeType == BITMAP_BURN)
			{
				Vector(0.f, 0.f, -20.f, Position);
				for (int i = 1; i < b->NumBones;)
				{
					if (!b->Bones[i].Dummy)
					{
						b->TransformPosition(o->BoneTransform[i], Position, p);
						//CreateParticle(BITMAP_FIRE+1,p,o->Angle,o->Light,rand()%4);
					}
					int d = (int)(4.f / c->Freeze);
					if (d < 1)
						d = 1;
					i += d;
				}
			}
		}
		Vector(1.f, 1.f, 1.f, Light);
		Vector(0.f, 0.f, 0.f, p);
		Luminosity = (float)(rand() % 8 + 2) * 0.1f;
		bool Smoke = false;
		switch (o->Type)
		{
		case MODEL_PLAYER:
			if (SceneFlag == MAIN_SCENE && (World == WD_7ATLANSE || World == WD_67DOPPLEGANGER3) && (int)WorldTime % 10000 < 1000)
			{
				Vector(0.f, 20.f, -10.f, p);
				b->TransformPosition(o->BoneTransform[b->BoneHead], p, Position, true);
				CreateParticleSync(BITMAP_BUBBLE, Position, o->Angle, Light);
			}
			Vector(1.f, 1.f, 1.f, Light);
			Vector(-15.f, 0.f, 0.f, p);
			if (gMapManager->InDevilSquare() == true)
			{
				if (rand() % 4 == 0)
				{
					b->TransformPosition(o->BoneTransform[26], p, Position, true);
					CreateParticleSync(BITMAP_RAIN_03, Position, o->Angle, Light);
				}
				if (rand() % 4 == 0)
				{
					b->TransformPosition(o->BoneTransform[35], p, Position, true);
					CreateParticleSync(BITMAP_RAIN_03, Position, o->Angle, Light);
				}
			}
			if (o->CurrentAction == PLAYER_SKILL_HELL_BEGIN || o->CurrentAction == PLAYER_SKILL_HELL_START)
			{
				if (o->BoneTransform != NULL)
				{
					Vector(0.3f, 0.3f, 1.f, Light);

					for (int i = 0; i < 40; i += 2)
					{
						if (!b->Bones[i].Dummy && i < b->NumBones)
						{
							b->TransformPosition(o->BoneTransform[i], p, Position, true);

							for (int j = 0; j < o->m_bySkillCount + 1; ++j)
							{
								CreateParticleSync(BITMAP_LIGHT, Position, o->Angle, Light, 6, 1.3f + (o->m_bySkillCount * 0.08f));
							}
						}
					}
				}
				VectorCopy(o->Position, Position);
				CreateForce(o, Position);
			}

			if (o->CurrentAction == PLAYER_SKILL_HELL)
			{
				for (int i = 0; i < 10; i++)
				{
					b->TransformPosition(o->BoneTransform[rand() % b->NumBones], p, Position, true);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
				}
			}

			{
				Vector(0.f, -30.f, 0.f, p);
				vec4_t* bone = o->GetBoneTransform(c->Weapon[0].LinkBone);

				if (bone)
					b->TransformPosition(bone, p, Position, true);
			}
			break;
		case MODEL_MONSTER01 + 49:
			break;
		case MODEL_MONSTER01 + 50:
			break;
		case MODEL_MONSTER01 + 51:
		{
			vec3_t pos1, pos2;

			for (int i = 2; i < 5; ++i)
			{
				b->TransformPosition(o->BoneTransform[i], p, pos1, true);
				b->TransformPosition(o->BoneTransform[i + 1], p, pos2, true);
				CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);
			}

			for (int i = 9; i < 11; ++i)
			{
				if (i == 9)
					b->TransformPosition(o->BoneTransform[2], p, pos1, true);
				else
					b->TransformPosition(o->BoneTransform[i], p, pos1, true);
				b->TransformPosition(o->BoneTransform[i + 1], p, pos2, true);
				CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);
			}

			b->TransformPosition(o->BoneTransform[2], p, pos1, true);
			b->TransformPosition(o->BoneTransform[18], p, pos2, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[22], p, pos1, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos2, pos1, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[23], p, pos2, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[24], p, pos1, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos2, pos1, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[25], p, pos2, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[18], p, pos1, true);
			b->TransformPosition(o->BoneTransform[31], p, pos2, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[32], p, pos1, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos2, pos1, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[33], p, pos2, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 14.f);

			b->TransformPosition(o->BoneTransform[34], p, pos1, true);
			CreateJointSync(BITMAP_JOINT_THUNDER, pos2, pos1, o->Angle, 7, NULL, 14.f);
		}
		break;
		case MODEL_MONSTER01 + 52:
			o->BlendMeshTexCoordU = -(float)((int)(WorldTime) % 10000) * 0.0004f;
			break;
		case MODEL_MONSTER01 + 55:
			o->BlendMeshTexCoordV = (float)((int)(WorldTime) % 10000) * 0.0001f;
			break;
		case MODEL_MONSTER01 + 54:
			break;
		case MODEL_MONSTER01 + 48:
			if (0 == ((int)rand() % 5))
			{
				Position[0] = o->Position[0] + ((rand() % 21) - 10) * ((float)TERRAIN_SIZE / 70);
				Position[1] = o->Position[1] + ((rand() % 21) - 10) * ((float)TERRAIN_SIZE / 70);
				CreatePointer(BITMAP_BLOOD, Position, o->Angle[0], o->Light, 0.65f);
			}
			break;
		case MODEL_MONSTER01 + 88:
		case MODEL_MONSTER01 + 45:
			MoveEye(o, b, 8, 9);
			MonsterMoveSandSmoke(o);
			//MonsterDieSandSmoke(o);
			break;
		case MODEL_MONSTER01 + 44://
			MoveEye(o, b, 8, 9);
			if (c->MonsterIndex == 63)
			{
				char    body[2] = { 30,0 };
				char    head = 1;
				vec3_t  vec[35];
				vec3_t  angle;
				vec3_t  dist;
				vec3_t  p;

				Vector(0.f, 0.f, 0.f, angle);
				Vector(0.f, 0.f, 0.f, p);

				for (int i = 0; i < 35; ++i)
				{
					b->TransformPosition(o->BoneTransform[vec_list[i]], p, vec[i], true);
				}

				char start, end;
				float scale = 1.0f;

				for (int i = 0; i < 15; ++i)
				{
					if (i >= 11)
					{
						scale = 0.5f;
					}

					start = wingLeft[i][0];
					end = wingLeft[i][1];

					dist[0] = MoveHumming(vec[end], angle, vec[start], 360.0f);
					CreateParticleSync(BITMAP_FLAME, vec[start], angle, dist, 2, scale);

					start = wingRight[i][0];
					end = wingRight[i][1];

					dist[0] = MoveHumming(vec[end], angle, vec[start], 360.0f);
					CreateParticleSync(BITMAP_FLAME, vec[start], angle, dist, 2, scale);
				}

				for (int i = 0; i < 4; ++i)
				{
					start = arm_leg_Left[i][0];
					end = arm_leg_Left[i][1];

					dist[0] = MoveHumming(vec[end], angle, vec[start], 360.0f);
					CreateParticleSync(BITMAP_FLAME, vec[start], angle, dist, 2, 0.6f);

					start = arm_leg_Right[i][0];
					end = arm_leg_Right[i][1];

					dist[0] = MoveHumming(vec[end], angle, vec[start], 360.0f);
					CreateParticleSync(BITMAP_FLAME, vec[start], angle, dist, 2, 0.6f);
				}

				if ((int)WorldTime % 2 == 0)
				{
					start = body[0];
					end = body[1];

					dist[0] = MoveHumming(vec[end], angle, vec[start], 360.0f);
					CreateParticleSync(BITMAP_FLAME, vec[start], angle, dist, 2, 1.3f);

					CreateParticleSync(BITMAP_FLAME, vec[head], angle, dist, 3, 0.5f);
				}

				Vector(-1.3f, -1.3f, -1.3f, Light);
				AddTerrainLight(o->Position[0], o->Position[1], Light, 3, PrimaryTerrainLight);
			}
			else
			{
				vec3_t  pos, angle;
				Vector(0.f, 0.f, 0.f, angle);

				Luminosity = (float)sinf(WorldTime * 0.002f) * 0.3f + 0.7f;

				Vector(Luminosity, Luminosity * 0.5f, Luminosity * 0.5f, Light);

				b->TransformPosition(o->BoneTransform[55], p, pos, true);
				b->TransformPosition(o->BoneTransform[62], p, Position, true);
				MoveHumming(pos, angle, Position, 360.0f);
				CreateParticleSync(BITMAP_FLAME, Position, angle, Light, 1, 0.2f);

				b->TransformPosition(o->BoneTransform[70], p, pos, true);
				b->TransformPosition(o->BoneTransform[77], p, Position, true);
				MoveHumming(pos, angle, Position, 360.0f);
				CreateParticleSync(BITMAP_FLAME, Position, angle, Light, 1, 0.2f);

				MonsterMoveSandSmoke(o);
				MonsterDieSandSmoke(o);
			}
			break;
		case MODEL_MONSTER01 + 43:
			MoveEye(o, b, 11, 12);
			MonsterMoveSandSmoke(o);
			//MonsterDieSandSmoke(o);
			break;
		case MODEL_MONSTER01 + 42://
			MoveEye(o, b, 24, 25);
			if (o->SubType == 1)
			{
				b->TransformPosition(o->BoneTransform[6], p, Position, true);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
				b->TransformPosition(o->BoneTransform[13], p, Position, true);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
				Vector(-1.3f, -1.3f, -1.3f, Light);
				AddTerrainLight(o->Position[0], o->Position[1], Light, 3, PrimaryTerrainLight);
			}
			else
			{
				MonsterMoveSandSmoke(o);
				MonsterDieSandSmoke(o);
			}
			break;
		case MODEL_MONSTER01 + 41:
			MoveEye(o, b, 8, 9);
			MonsterMoveSandSmoke(o);
			MonsterDieSandSmoke(o);
			break;
		case MODEL_MONSTER01 + 39:
			MoveEye(o, b, 28, 27);
			break;
		case MODEL_MONSTER01 + 37:
			if (o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
			{
				o->BlendMeshLight += 0.1f;
				if (o->BlendMeshLight > 1.f)
					o->BlendMeshLight = 1.f;
			}
			else
			{
				o->BlendMeshLight -= 0.1f;
				if (o->BlendMeshLight < 0.f)
					o->BlendMeshLight = 0.f;
			}
			break;
		case MODEL_MONSTER01 + 29:
			o->BlendMesh = 3;
			o->BlendMeshTexCoordV = -(float)((int)(WorldTime) % 1000) * 0.001f;
			if (rand() % 2 == 0)
			{
				Vector(0.f, 0.f, 0.f, p);
				b->TransformPosition(o->BoneTransform[2], p, Position, true);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
			}
			break;
		case MODEL_MONSTER01 + 27:
			o->BlendMeshTexCoordV = -(float)((int)(WorldTime) % 1000) * 0.001f;
			break;
		case MODEL_MONSTER01 + 26:
			o->BlendMeshTexCoordU = -(float)((int)(WorldTime) % 10000) * 0.0001f;
			break;
		case MODEL_MONSTER01 + 32:
			Vector(0.f, 0.f, 0.f, p);
			Vector(0.6f, 1.f, 0.8f, Light);
			if (o->CurrentAction == MONSTER01_ATTACK1)
			{
				b->TransformPosition(o->BoneTransform[33], p, Position, true);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				Vector(1.f, 0.6f, 1.f, Light);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
			}
			if (o->CurrentAction == MONSTER01_ATTACK2)
			{
				b->TransformPosition(o->BoneTransform[20], p, Position, true);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				Vector(1.f, 0.6f, 1.f, Light);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
			}
			if (o->CurrentAction == MONSTER01_ATTACK3)
			{
				b->TransformPosition(o->BoneTransform[41], p, Position, true);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				Vector(1.f, 0.6f, 1.f, Light);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
			}
			if (o->CurrentAction == MONSTER01_ATTACK4)
			{
				b->TransformPosition(o->BoneTransform[49], p, Position, true);
				CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
				Vector(1.f, 0.6f, 1.f, Light);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
			}
			if (o->CurrentAction == MONSTER01_DIE && o->AnimationFrame < 12.f)
			{
				Vector(0.1f, 0.8f, 0.6f, Light);

				for (int i = 0; i < 20; i++)
				{
					b->TransformPosition(o->BoneTransform[rand() % b->NumBones], p, Position, true);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
				}
			}
			break;
		case MODEL_MONSTER01 + 11:
			o->BlendMeshLight = (float)(rand() % 10) * 0.1f;
			if (c->Level == 2)
			{
				for (int i = 0; i < 10; i++)
				{
					b->TransformPosition(o->BoneTransform[rand() % b->NumBones], p, Position, true);
					CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light);
				}
				Vector(Luminosity * 1.f, Luminosity * 0.2f, Luminosity * 0.f, Light);
				AddTerrainLight(o->Position[0], o->Position[1], Light, 2, PrimaryTerrainLight);
			}
			break;
		case MODEL_MONSTER01 + 15:
			o->BlendMeshTexCoordV = -(float)((int)(WorldTime) % 2000) * 0.0005f;
			break;
		case MODEL_MIX_NPC:
			if (rand() % 64 == 0)
				PlayBuffer(SOUND_NPC + 1);
			break;
		case MODEL_ELF_WIZARD:
			if (rand() % 256 == 0)
				PlayBuffer(SOUND_NPC);
			break;
		case MODEL_SMITH:
			if (g_isCharacterBuff(o, eBuff_CrywolfNPCHide))
				break;
			if (o->CurrentAction == 0 && o->AnimationFrame >= 5.f && o->AnimationFrame <= 10.f)
				PlayBuffer(SOUND_NPC);
			o->BlendMesh = 4;
			o->BlendMeshLight = Luminosity;
			Vector(Luminosity * 1.f, Luminosity * 0.4f, Luminosity * 0.f, Light);
			AddTerrainLight(o->Position[0], o->Position[1], Light, 3, PrimaryTerrainLight);
			Vector(1.f, 1.f, 1.f, Light);
			Vector(0.f, 0.f, 0.f, p);
			if (o->CurrentAction == 0 && o->AnimationFrame >= 5.f && o->AnimationFrame <= 6.f)
			{
				b->TransformPosition(o->BoneTransform[17], p, Position, true);
				vec3_t Angle;
				for (int i = 0; i < 4; i++)
				{
					Vector((float)(rand() % 60 + 60 + 90), 0.f, (float)(rand() % 30), Angle);
					CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, Angle);
					CreateParticleSync(BITMAP_SPARK, Position, Angle, Light);
				}
			}
			break;
		case MODEL_DEVIAS_TRADER:
			Vector(1.f, 1.f, 1.f, Light);
			Vector(0.f, 5.f, 10.f, p);
			if (o->CurrentAction == 0)
			{
				Vector(Luminosity * 0.5f, Luminosity * 0.3f, Luminosity * 0.f, Light);
				AddTerrainLight(o->Position[0], o->Position[1], Light, 3, PrimaryTerrainLight);
				b->TransformPosition(o->BoneTransform[37], p, Position, true);
				vec3_t Angle;
				for (int i = 0; i < 4; i++)
				{
					Vector((float)(rand() % 60 + 60 + 30), 0.f, (float)(rand() % 30), Angle);
					CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, Angle);
					if (rand() % 2)
						CreateParticleSync(BITMAP_SPARK, Position, Angle, Light);
				}
			}
			break;
		case MODEL_WEDDING_NPC:
			if (o->CurrentAction == 1)
			{
				if (o->AnimationFrame > 4.5f && o->AnimationFrame <= 4.8f)
				{
					CreateEffect(BITMAP_FIRECRACKER0001, o->Position, o->Angle, o->Light, 0);
				}
			}
			break;
		case MODEL_MONSTER01 + 2:
			if (o->CurrentAction == MONSTER01_ATTACK1 && o->AnimationFrame <= 4.f)
			{
				vec3_t Light;
				Vector(1.f, 1.f, 1.f, Light);
				Vector(0.f, (float)(rand() % 32 + 32), 0.f, p);
				b->TransformPosition(o->BoneTransform[7], p, Position, true);
				CreateParticle(BITMAP_FIRE, Position, o->Angle, Light, 1);
			}
		case MODEL_MONSTER01 + 3:
		case MODEL_MONSTER01 + 6:
		case MODEL_MONSTER01 + 20:
			if (o->Type == MODEL_MONSTER01 + 20)
			{
				Vector(0.f, 0.f, 0.f, p);
				b->TransformPosition(o->BoneTransform[7], p, Position, true);
				Vector(Luminosity * 1.f, Luminosity * 0.4f, Luminosity * 0.2f, Light);
				CreateSprite(BITMAP_LIGHT, Position, 1.f, Light, o);
			}
			if (c->Dead == 0 && rand() % 4 == 0)
			{
				Vector(o->Position[0] + (float)(rand() % 64 - 32),
					o->Position[1] + (float)(rand() % 64 - 32),
					o->Position[2] + (float)(rand() % 32 - 16), Position);
				if (World == WD_2DEVIAS)
					CreateParticle(BITMAP_SMOKE, Position, o->Angle, Light);
				else
					CreateParticle(BITMAP_SMOKE + 1, Position, o->Angle, Light);
			}
			break;
		case MODEL_MONSTER01 + 33:
			if (c->Dead == 0 && c->Level == 1 && rand() % 4 == 0)
			{
				Vector(o->Position[0] + (float)(rand() % 64 - 32),
					o->Position[1] + (float)(rand() % 64 - 32),
					o->Position[2] + (float)(rand() % 32 - 16), Position);
				CreateParticle(BITMAP_SMOKE + 1, Position, o->Angle, Light);
			}
			break;
		case MODEL_MONSTER01 + 5:
			MonsterDieSandSmoke(o);
			break;
		case MODEL_MONSTER01 + 12:
		case MODEL_MONSTER01 + 13:
			if (rand() % 4 == 0)
			{
				Vector(0.f, 0.f, 0.f, p);
				b->TransformPosition(o->BoneTransform[22], p, Position, true);
				CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, o->Light);
			}
			break;
		case MODEL_MONSTER01:
			if ((o->CurrentAction == MONSTER01_STOP1 &&
				(o->AnimationFrame >= 15.f && o->AnimationFrame <= 20.f)))
				Smoke = true;
			if ((o->CurrentAction == MONSTER01_STOP2 &&
				(o->AnimationFrame >= 20.f && o->AnimationFrame <= 25.f)))
				Smoke = true;
			if ((o->CurrentAction == MONSTER01_WALK &&
				((o->AnimationFrame >= 2.f && o->AnimationFrame <= 3.f) ||
					(o->AnimationFrame >= 5.f && o->AnimationFrame <= 6.f))))
				Smoke = true;
			if (Smoke)
			{
				if (rand() % 2 == 0)
				{
					Vector(0.f, -4.f, 0.f, p);
					b->TransformPosition(o->BoneTransform[24], p, Position, true);
					CreateParticleSync(BITMAP_SMOKE, Position, o->Angle, o->Light);
				}
			}
			break;
		default:
		{
			if (MoveHellasMonsterVisual(o, b)) break;
			if (battleCastle::MoveBattleCastleMonsterVisual(o, b)) break;
			if (M31HuntingGround::MoveHuntingGroundMonsterVisual(o, b)) break;
			if (M34CryingWolf2nd::MoveCryingWolf2ndMonsterVisual(o, b)) break;
			if (M34CryWolf1st::MoveCryWolf1stMonsterVisual(c, o, b)) break;
			if (M33Aida::MoveAidaMonsterVisual(o, b)) break;
			if (M37Kanturu1st::MoveKanturu1stMonsterVisual(c, o, b)) break;
			if (M38Kanturu2nd::Move_Kanturu2nd_MonsterVisual(c, o, b)) break;
			if (M39Kanturu3rd::MoveKanturu3rdMonsterVisual(o, b)) break;
			if (SEASON3A::CGM3rdChangeUp::Instance().MoveBalgasBarrackMonsterVisual(c, o, b))	break;
			if (g_NewYearsDayEvent->MoveMonsterVisual(c, o, b) == true) break;
			if (g_CursedTemple->MoveMonsterVisual(o, b) == true) break;
			if (SEASON3B::GMNewTown::MoveMonsterVisual(o, b) == true) break;
			if (SEASON3C::GMSwampOfQuiet::MoveMonsterVisual(o, b) == true) break;
			if (g_09SummerEvent->MoveMonsterVisual(c, o, b)) break;
			if (g_EmpireGuardian1.MoveMonsterVisual(c, o, b) == true) break;
			if (g_EmpireGuardian2.MoveMonsterVisual(c, o, b) == true) break;
			if (g_EmpireGuardian3.MoveMonsterVisual(c, o, b) == true) break;
			if (g_EmpireGuardian4.MoveMonsterVisual(c, o, b) == true) break;
			if (TheMapProcess().MoveMonsterVisual(o, b) == true) break;
		}
		break;
		}

		if (o->Type == MODEL_PLAYER && c == Hero)
		{
			if ((o->CurrentAction >= PLAYER_WALK_MALE && o->CurrentAction <= PLAYER_RUN_RIDE_WEAPON)
				|| (o->CurrentAction == PLAYER_WALK_TWO_HAND_SWORD_TWO || o->CurrentAction == PLAYER_RUN_TWO_HAND_SWORD_TWO)
				|| (o->CurrentAction == PLAYER_RUN_RIDE_HORSE)
#ifdef PBG_ADD_NEWCHAR_MONK_ANI
				|| (o->CurrentAction == PLAYER_RAGE_UNI_RUN || o->CurrentAction == PLAYER_RAGE_UNI_RUN_ONE_RIGHT)
#endif //PBG_ADD_NEWCHAR_MONK_ANI
				)
			{
				Vector(0.f, 0.f, 0.f, p);
				if (o->AnimationFrame >= 1.5f && !c->Foot[0])
				{
					c->Foot[0] = true;
					PlayWalkSound();
				}
				if (o->AnimationFrame >= 4.5f && !c->Foot[1])
				{
					c->Foot[1] = true;
					PlayWalkSound();
				}
			}
		}

		if ((o->CurrentAction == PLAYER_RUN_RIDE
#ifdef PBG_ADD_NEWCHAR_MONK_ANI
			|| o->CurrentAction == PLAYER_RAGE_UNI_RUN
			|| o->CurrentAction == PLAYER_RAGE_UNI_RUN_ONE_RIGHT
#endif //PBG_ADD_NEWCHAR_MONK_ANI
			|| o->CurrentAction == PLAYER_RUN_RIDE_WEAPON || o->CurrentAction == PLAYER_RUN_SWIM || o->CurrentAction == PLAYER_WALK_SWIM || o->CurrentAction == PLAYER_FLY || o->CurrentAction == PLAYER_FLY_CROSSBOW || o->CurrentAction == PLAYER_RUN_RIDE_HORSE) &&
			o->Type == MODEL_PLAYER && gMapManager->InHellas())
		{
			vec3_t Light = { 0.3f, 0.3f, 0.3f };
			VectorCopy(o->Position, Position);
			float  Matrix[3][4];
			Vector(0.f, -40.f, 0.f, p);
			AngleMatrix(o->Angle, Matrix);
			VectorRotate(p, Matrix, Position);
			VectorAdd(o->Position, Position, Position);
			Position[0] += rand() % 64 - 32.f;
			Position[1] += rand() % 64 - 32.f;
			Position[2] += 50.f;
			CreateParticleSync(BITMAP_WATERFALL_5, Position, o->Angle, Light, 1);
		}
	}
}

float CharacterMoveSpeed(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	float Speed = (float)c->MoveSpeed;
	if (o->Type == MODEL_PLAYER && o->Kind == KIND_PLAYER)
	{
		bool isholyitem = false;

		isholyitem = g_pCursedTempleWindow->CheckInventoryHolyItem(c);

		if (isholyitem)
		{
			c->Run = 40;
			Speed = 8;
			return Speed;
		}

		if ((c->Helper.Type == MODEL_HELPER + 37 || o->movementType == 5) && !c->SafeZone && !isholyitem)
		{
			if (c->Run < FENRIR_RUN_DELAY / 2)
				Speed = 15;
			else if (c->Run < FENRIR_RUN_DELAY)
				Speed = 16;
			else
			{
				if (c->Helper.Option1 > 0)
					Speed = 19;
				else
					Speed = 17;
			}
		}
		else if ((c->Helper.Type == MODEL_HELPER + 4 || o->movementType == 6) && !c->SafeZone && !isholyitem)
		{
			c->Run = 40;
			Speed = 17;
		}
		else if (!(c->Object.SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER || c->Object.SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER)
			&& (c->Wing.Type != -1 || (c->Helper.Type >= MODEL_HELPER + 2 && c->Helper.Type <= MODEL_HELPER + 3)) && !c->SafeZone
			&& !isholyitem)
		{
			if (c->Wing.Type == MODEL_WING + 5 || c->Wing.Type == MODEL_WING + 36 )
			{
				c->Run = 40;
				Speed = 16;
			}
			else
			{
				c->Run = 40;
				Speed = 15;
			}
		}
		else if (!isholyitem)
		{
			if (c->Run < 40)
				Speed = 12;
			else
				Speed = 15;
		}
	}
#ifndef GUILD_WAR_EVENT
	if (gMapManager->InChaosCastle() == true)
	{
		c->Run = 40;
		Speed = 15;
	}
#endif// GUILD_WAR_EVENT

	if (g_isCharacterBuff((&c->Object), eDeBuff_Freeze))
	{
		Speed *= timepow(0.5f);
	}
	else if (g_isCharacterBuff((&c->Object), eDeBuff_BlowOfDestruction))
	{
		Speed *= timepow(0.33f);
	}
	if (g_isCharacterBuff((&c->Object), eBuff_CursedTempleQuickness))
	{
		c->Run = 40;
		Speed = 20;
	}

	return Speed;
}

void MoveCharacterPosition(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	float Matrix[3][4];
	vec3_t v, Velocity;

	AngleMatrix(o->Angle, Matrix);
	Vector(0.f, -CharacterMoveSpeed(c), 0.f, v);
	VectorRotate(v, Matrix, Velocity);
	interpolatesum(o->Position, Velocity);

	if (World == -1 || c->Helper.Type != MODEL_HELPER + 3 || c->SafeZone)
	{
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
	}
	else
	{
		if (World == WD_8TARKAN || World == WD_10HEAVEN)
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 90.f;
		else
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
	}
	if (o->Type == MODEL_MONSTER01 + 2)
	{
		o->Position[2] += timefac(-absf(sinf(o->Timer)) * 70.f + 70.f);
	}
	o->Timer += timefac(0.15);
}

void MoveMonsterClient(CHARACTER* c, OBJECT* o)
{
	if (c == Hero)
		return;

	if (c->Dead == 0)
	{
		if (c->MonsterIndex == 384 || c->MonsterIndex == 383)
		{
			c->Movement = false;
		}

		if (!c->Movement)
		{
			if (c->Appear == 0 && o->Type != MODEL_MONSTER01 + 7 && ((c->PositionX) != c->TargetX || (c->PositionY) != c->TargetY))
			{
				int iDefaultWall = TW_CHARACTER;

				if (World >= WD_65DOPPLEGANGER1 && World <= WD_68DOPPLEGANGER4)
				{
					iDefaultWall = TW_NOMOVE;
				}

				if (PathFinding2((c->PositionX), (c->PositionY), c->TargetX, c->TargetY, &c->Path, 0.0f, iDefaultWall))
				{
					c->Movement = true;
				}
			}
		}
		else
		{
			if (o->Type == MODEL_MONSTER01 + 121 && (o->CurrentAction == MONSTER01_ATTACK1
				|| o->CurrentAction == MONSTER01_ATTACK2
				|| o->CurrentAction == MONSTER01_ATTACK3
				|| o->CurrentAction == MONSTER01_ATTACK4
				|| o->CurrentAction == MONSTER01_ATTACK5))
			{
				c->Movement = true;
				SetAction(o, o->CurrentAction);
			}
			else
			{
				SetPlayerWalk(c);
			}

			if (MovePath(c))
			{
				c->Movement = false;
				SetPlayerStop(c);
				c->Object.Angle[2] = ((float)(c->TargetAngle) - 1.f) * 45.f;
			}
			MoveCharacterPosition(c);
		}
	}
	else
	{
		if (o->Type == MODEL_MONSTER01 + 2)
		{
			o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
		}
	}
}

void MoveCharacterClient(CHARACTER* cc)
{
	OBJECT* co = &cc->Object;
	if (co->Live)
	{
		if (World == WD_77NEW_LOGIN_SCENE)
		{
			float fDistance_x = CameraPosition[0] - co->Position[0];
			float fDistance_y = CameraPosition[1] - co->Position[1];
			float fDistance = sqrtf(fDistance_x * fDistance_x + fDistance_y * fDistance_y);
			if (!TestFrustrum2D(co->Position[0] * 0.01f, co->Position[1] * 0.01f, -100.f) || fDistance > 3800.f)
				return;
		}
		co->Visible = TestFrustrum2D(co->Position[0] * 0.01f, co->Position[1] * 0.01f, -20.f);

		MoveMonsterClient(cc, co);
		MoveCharacter(cc, co);
		MoveCharacterVisual(cc, co);

		battleCastle::MoveBattleCastleMonster(cc, co);
	}
}

void MoveCharactersClient()
{
	for (int i = 0; i < TERRAIN_SIZE * TERRAIN_SIZE; i++)
	{
		if ((TerrainWall[i] & TW_CHARACTER) == TW_CHARACTER)
		{
			TerrainWall[i] -= TW_CHARACTER;
		}
	}

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* tc = gmCharacters->GetCharacter(i);
		OBJECT* to = &tc->Object;
		if (to->Live && tc->Dead == 0 && to->Kind != KIND_TRAP)
		{
			int Index = TERRAIN_INDEX_REPEAT((tc->PositionX), (tc->PositionY));
			TerrainWall[Index] |= TW_CHARACTER;
		}

		to->Visible = TestFrustrum2D(to->Position[0] * 0.01f, to->Position[1] * 0.01f, -20.f);
	}

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		MoveCharacterClient(gmCharacters->GetCharacter(i));
	}
	MoveBlurs();
}

void RenderGuild(OBJECT* o, int Type, vec3_t vPos)
{
	EnableAlphaTest();
	EnableCullFace();
	glColor3f(1.f, 1.f, 1.f);
	BindTexture(BITMAP_GUILD);
	glPushMatrix();

	float Matrix[3][4];
	vec3_t Angle;
	VectorCopy(o->Angle, Angle);
	Angle[2] += 135.f;
	Angle[1] += 45.f;
	Angle[0] += 80.f;
	AngleMatrix(Angle, Matrix);
	Matrix[0][3] = 20.f;
	Matrix[1][3] = -5.f;
	if (Type == MODEL_ARMOR + 20 && Type != -1)
	{
		Matrix[2][3] = -18.f;//-5
	}
	else
	{
		Matrix[2][3] = -10.f;//-5
	}

	if (vPos != NULL)
	{
		Matrix[0][3] = vPos[0];
		Matrix[1][3] = vPos[1];
		Matrix[2][3] = vPos[2];
	}

	R_ConcatTransforms(o->BoneTransform[26], Matrix, ParentMatrix);
	glTranslatef(o->Position[0], o->Position[1], o->Position[2]);
	RenderPlane3D(5.f, 7.f, ParentMatrix);

	glPopMatrix();
	DisableCullFace();
}

OBJECT g_ItemObject[MAX_ITEM + MODEL_ITEM];

void RenderBrightEffect(BMD* b, int Bitmap, int Link, float Scale, vec3_t Light, OBJECT* o)
{
	vec3_t p, Position;
	Vector(0.f, 0.f, 0.f, p);
	b->TransformPosition(BoneTransform[Link], p, Position, true);
	CreateSprite(Bitmap, Position, Scale, Light, o);
}


void RenderLinkObject(float x, float y, float z, CHARACTER* c, PART_t* f, int Type, int Level, int Option1, bool Link, bool Translate, int RenderType, bool bRightHandItem)
{
	OBB_t OBB;
	float Matrix[3][4];
	vec3_t Angle, p, Position;

	OBJECT* pObject = &c->Object;
	BMD* pModel = gmClientModels->GetModel(Type);

	if (pModel == NULL
		|| (pObject->SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER || pObject->SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER)
		&& (Type >= MODEL_WING && Type <= (MODEL_WING + 6)
			|| Type >= (MODEL_WING + 36) && Type <= (MODEL_WING + 43)
			//|| Type >= (ITEM_WING + 130) && Type <= (ITEM_WING + 134)
			|| Type >= (MODEL_WING + 49) && Type <= (MODEL_WING + 50)
			|| Type == (MODEL_WING + 135))
		|| Type == (MODEL_HELPER + 30)
		|| Type == (MODEL_WING + 49)
		|| Type == (MODEL_WING + 135)
		//|| Type == (ITEM_WING + 130)
		|| Type >= (MODEL_STAFF + 21) && Type <= (MODEL_STAFF + 29)
		|| Type == (MODEL_STAFF + 40)
		|| g_isCharacterBuff(pObject, eBuff_Cloaking))
	{
		return;
	}

	pModel->ContrastEnable = pObject->ContrastEnable;
	pModel->BodyScale = pObject->Scale;
	pModel->CurrentAction = f->CurrentAction;
	pModel->BodyHeight = 0.f;

	OBJECT* Object = &g_ItemObject[Type];
	Object->Type = Type;

	ItemObjectAttribute(Object);
	pModel->LightEnable = Object->LightEnable;
	pModel->LightEnable = false;
	g_CharacterCopyBuff(Object, pObject);

	if (Type == (MODEL_WING + 39))
	{
		f->PlaySpeed = 0.15f;
	}

	if (c->MonsterIndex >= 529 && c->MonsterIndex <= 539)
	{
		if (World == WD_65DOPPLEGANGER1)
			RenderType = RENDER_DOPPELGANGER | RENDER_TEXTURE;
		else
			RenderType = RENDER_DOPPELGANGER | RENDER_BRIGHT | RENDER_TEXTURE;
	}

	int WeaponKind = GMItemMng->GetKind2(Type - MODEL_ITEM);

	if (Link)
	{
		if (c->MonsterIndex < 132 || c->MonsterIndex > 134)
		{
			if (Type == (MODEL_WING + 50))
			{
				Angle[0] = 0.0;
				Angle[1] = 90.0;
				Angle[2] = 0.0;
				AngleMatrix(Angle, Matrix);
				Matrix[0][3] = 10.0;
				Matrix[1][3] = -15.0;
				Matrix[2][3] = 0.0;
			}
			else if (Type < (MODEL_WING + 40))
			{
				if ((Type < (MODEL_BOW + 8) || Type >= (MODEL_BOW + 15)) && (Type < (MODEL_BOW + 16) || Type >= (MODEL_BOW + 17)) && (Type < (MODEL_BOW + 18) || Type >= (MODEL_BOW + 20))
					|| (Type > (MODEL_BOW + 25) && WeaponKind == ItemKind2::BOW))
				{
					if (Type != MODEL_15GRADE_ARMOR_OBJ_ARMLEFT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_ARMRIGHT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_BODYLEFT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_BODYRIGHT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_BOOTLEFT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_BOOTRIGHT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_HEAD
						&& Type != MODEL_15GRADE_ARMOR_OBJ_PANTLEFT
						&& Type != MODEL_15GRADE_ARMOR_OBJ_PANTRIGHT)
					{
						if (Type == MODEL_SWORD_35_WING)
						{
							Angle[0] = x;
							Angle[1] = y;
							Angle[2] = z;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 15.0;
							Matrix[2][3] = 0.0;
							Matrix[1][3] = -5.0;
						}
						else
						{
							if (Type == (MODEL_STAFF + 9))
							{
								Angle[0] = 110.0;
								Angle[1] = 180.0;
								Angle[2] = 90.0;
								AngleMatrix(Angle, Matrix);
							}
							else
							{
								Angle[0] = 70.0;
								Angle[1] = 0.0;
								Angle[2] = 90.0;
								AngleMatrix(Angle, Matrix);
							}
							switch (Type)
							{
							case (MODEL_BOW + 20):
								Angle[0] = -60.0;
								Angle[1] = 0.0;
								Angle[2] = -80.0;
								AngleMatrix(Angle, Matrix);
								Matrix[0][3] = -5.0;
								Matrix[1][3] = 20.0;
								Matrix[2][3] = 0.0;
								break;
							case (MODEL_BOW + 23):
								Angle[0] = -60.0;
								Angle[1] = 0.0;
								Angle[2] = -80.0;
								AngleMatrix(Angle, Matrix);
								Matrix[0][3] = -5.0;
								Matrix[1][3] = 20.0;
								Matrix[2][3] = -5.0;
								break;
							case (MODEL_BOW + 24):
								Angle[0] = 90.0;
								Angle[1] = 0.0;
								Angle[2] = -80.0;
								AngleMatrix(Angle, Matrix);
								Matrix[0][3] = 10.0;
								Matrix[1][3] = 20.0;
								Matrix[2][3] = -5.0;
								break;
							default:
								if (Type < MODEL_BOW || Type >= (MODEL_STAFF))
								{
									if (Type == (MODEL_STAFF + 9))
									{
										Matrix[0][3] = -10.0;
										Matrix[1][3] = 5.0;
										Matrix[2][3] = -10.0;
									}
									else if (Type == (MODEL_STAFF + 8))
									{
										Matrix[0][3] = -10.0;
										Matrix[1][3] = 5.0;
										Matrix[2][3] = 10.0;
									}
									else if (Type < MODEL_SHIELD || Type >= MODEL_HELM)
									{
										Matrix[0][3] = -20.0;
										Matrix[1][3] = 5.0;
										Matrix[2][3] = 40.0;
									}
									else if (Type == (MODEL_SHIELD + 16))
									{
										Angle[0] = 30.0;
										Angle[1] = 0.0;
										Angle[2] = 90.0;
										AngleMatrix(Angle, Matrix);
										Matrix[0][3] = -20.0;
										Matrix[1][3] = 0.0;
										Matrix[2][3] = -20.0;
									}
									else if (Type != (MODEL_SHIELD + 14) && Type != (MODEL_SHIELD + 15) && Type != (MODEL_SHIELD + 27))
									{
										if (Type == (MODEL_SHIELD + 6))
										{
											Angle[0] = 30.0;
											Angle[1] = 0.0;
											Angle[2] = 90.0;
											AngleMatrix(Angle, Matrix);
											Matrix[0][3] = -15.0;
											Matrix[1][3] = 0.0;
											Matrix[2][3] = -25.0;
										}
										else
										{
											Matrix[0][3] = -10.0;
											Matrix[1][3] = 0.0;
											Matrix[2][3] = 0.0;
										}
									}
									else
									{
										Angle[0] = 50.0;
										Angle[1] = 0.0;
										Angle[2] = 90.0;
										AngleMatrix(Angle, Matrix);
										Matrix[0][3] = -28.0;
										Matrix[1][3] = 0.0;
										Matrix[2][3] = -25.0;
									}
								}
								else
								{
									if (Type > (MODEL_BOW + 25) && WeaponKind == ItemKind2::BOW)
									{
										Angle[0] = -60.0;
										Angle[1] = 0.0;
										Angle[2] = -80.0;
										AngleMatrix(Angle, Matrix);
										Matrix[0][3] = -5.0;
										Matrix[1][3] = 20.0;
										Matrix[2][3] = 0.0;
									}
									else if (Type > (MODEL_BOW + 25) && WeaponKind == ItemKind2::CROSSBOW)
									{
										Angle[0] = 0.0;
										Angle[1] = 20.0;
										Angle[2] = 180.0;
										AngleMatrix(Angle, Matrix);
										Matrix[0][3] = -10.0;
										Matrix[1][3] = 8.0;
										Matrix[2][3] = 40.0;
									}
									else
									{
										Matrix[0][3] = -10.0;
										Matrix[1][3] = 5.0;
										Matrix[2][3] = 10.0;
									}
								}
								break;
							}
						}
					}
					else
					{
						switch (Type)
						{
						case MODEL_15GRADE_ARMOR_OBJ_ARMLEFT:
							Angle[0] = 0.0;
							Angle[1] = -90.0;
							Angle[2] = 0.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 30.0;
							Matrix[1][3] = 0.0;
							Matrix[2][3] = 20.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_ARMRIGHT:
							Angle[0] = 0.0;
							Angle[1] = -90.0;
							Angle[2] = 0.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 30.0;
							Matrix[1][3] = 0.0;
							Matrix[2][3] = -20.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_BODYLEFT:
							Angle[0] = 0.0;
							Angle[1] = -90.0;
							Angle[2] = 0.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 5.0;
							Matrix[1][3] = -20.0;
							Matrix[2][3] = 0.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_BODYRIGHT:
							Angle[0] = 0.0;
							Angle[1] = -90.0;
							Angle[2] = 0.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 5.0;
							Matrix[1][3] = -20.0;
							Matrix[2][3] = 0.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_BOOTLEFT:
							Angle[0] = 0.0;
							Angle[1] = 90.0;
							Angle[2] = 180.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 20.0;
							Matrix[1][3] = 15.0;
							Matrix[2][3] = -10.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_BOOTRIGHT:
							Angle[0] = 0.0;
							Angle[1] = 90.0;
							Angle[2] = 180.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 20.0;
							Matrix[1][3] = 15.0;
							Matrix[2][3] = 10.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_HEAD:
							Angle[0] = 180.0;
							Angle[1] = -90.0;
							Angle[2] = 0.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 28.0;
							Matrix[1][3] = 20.0;
							Matrix[2][3] = 0.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_PANTLEFT:
							Angle[0] = 0.0;
							Angle[1] = 90.0;
							Angle[2] = 180.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 25.0;
							Matrix[1][3] = 5.0;
							Matrix[2][3] = -5.0;
							break;
						case MODEL_15GRADE_ARMOR_OBJ_PANTRIGHT:
							Angle[0] = 0.0;
							Angle[1] = 90.0;
							Angle[2] = 180.0;
							AngleMatrix(Angle, Matrix);
							Matrix[0][3] = 25.0;
							Matrix[1][3] = 5.0;
							Matrix[2][3] = 5.0;
							break;
						default:
							break;
						}
					}
				}
				else
				{
					Angle[0] = 0.0;
					Angle[1] = 20.0;
					Angle[2] = 180.0;
					AngleMatrix(Angle, Matrix);
					Matrix[0][3] = -10.0;
					Matrix[1][3] = 8.0;
					Matrix[2][3] = 40.0;
				}
			}
			else
			{
				Angle[0] = 0.0;
				Angle[1] = 90.0;
				Angle[2] = 0.0;
				AngleMatrix(Angle, Matrix);
				Matrix[0][3] = -47.0;
				Matrix[1][3] = -7.0;
				Matrix[2][3] = 0.0;
			}
		}
		else if (Type != (MODEL_STAFF + 10) && Type != (MODEL_SWORD + 19))
		{
			if (Type == (MODEL_BOW + 18))
			{
				Angle[0] = 10.0;
				Angle[1] = 0.0;
				Angle[2] = 0.0;
				AngleMatrix(Angle, Matrix);
				Matrix[0][3] = 0.0;
				Matrix[1][3] = 110.0;
				Matrix[2][3] = 80.0;
			}
		}
		else
		{
			Angle[0] = 90.0;
			Angle[1] = 0.0;
			Angle[2] = 90.0;
			AngleMatrix(Angle, Matrix);
			Matrix[0][3] = 0.0;
			Matrix[1][3] = 80.0;
			Matrix[2][3] = 120.0;
		}

#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
		if (g_CMonkSystem.IsRagefighterCommonWeapon(c->Class, Type))
		{
			Matrix[1][3] += 10.0;
			Matrix[2][3] += 25.0;
			pModel->BodyScale = 0.9f;
		}
#endif //PBG_ADD_NEWCHAR_MONK_ITEM

		if (!bRightHandItem && (Type < MODEL_SHIELD || Type >= MODEL_HELM) && Type != (MODEL_BOW + 20))
		{
			float RotX, RotY, RotZ;

#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
			if (g_CMonkSystem.IsRagefighterCommonWeapon(c->Class, Type))
			{
				Matrix[1][3] += 7.0;
			}
#endif //PBG_ADD_NEWCHAR_MONK_ITEM

			RotX = Matrix[0][3];
			RotY = Matrix[1][3];
			RotZ = Matrix[2][3];

			Angle[2] = -(Angle[2]);
			AngleMatrix(Angle, Matrix);

			Matrix[0][3] = -(RotX);
			Matrix[1][3] = RotY;
			Matrix[2][3] = RotZ;
		}

		vec4_t* mtBone = pObject->GetBoneTransform(f->LinkBone);
		if (!mtBone)
			return;

		R_ConcatTransforms(mtBone, Matrix, ParentMatrix);
		pModel->BodyOrigin[0] = c->Object.Position[0];
		pModel->BodyOrigin[1] = c->Object.Position[1];
		pModel->BodyOrigin[2] = c->Object.Position[2];
		Object->Angle[0] = 0.0;
		Object->Angle[1] = 0.0;
		Object->Angle[2] = 0.0;
	}
	else
	{
		Vector(x, y, z, p);
		BMD* Owner = gmClientModels->GetModel(pObject->Type);
		if (!Owner)
			return;

		vec4_t* mtBone = pObject->GetBoneTransform(f->LinkBone);
		if (!mtBone)
			return;

		Owner->RotationPosition(mtBone, p, Position);
		pModel->BodyOrigin[0] = c->Object.Position[0] + Position[0];
		pModel->BodyOrigin[1] = c->Object.Position[1] + Position[1];
		pModel->BodyOrigin[2] = c->Object.Position[2] + Position[2];
		Object->Angle[0] = 0;
		Object->Angle[1] = 0;
		Object->Angle[2] = 0;
	}

	if (Type == MODEL_BOSS_HEAD)
	{
		pModel->BodyOrigin[0] = pModel->BodyOrigin[0] + BossHeadPosition[0];
		pModel->BodyOrigin[1] = pModel->BodyOrigin[1] + BossHeadPosition[1];
		pModel->BodyOrigin[2] = pModel->BodyOrigin[2] + BossHeadPosition[2];

		pModel->BoneHead = 0;
		Object->Angle[0] = 0.0;
		Object->Angle[1] = 0.0;
		Object->Angle[2] = WorldTime;
	}

	if ((c->Skill == Skill_Penetration || c->Skill == Skill_Penetration_Strengthener)
		&& pObject->Type == MODEL_PLAYER
		&& pObject->CurrentAction >= PLAYER_HIT_FIST
		&& pObject->CurrentAction <= PLAYER_RIDE_SKILL
		&& pObject->AnimationFrame >= 5.f
		&& pObject->AnimationFrame <= 10.f)
	{
		f->PriorAnimationFrame = 2.f;
		f->AnimationFrame = 3.2f;
	}
	if (f->Type == (MODEL_WING + 6) && c->SafeZone)
	{
		pModel->CurrentAction = 1;
	}

	if ((!Link || (Type < MODEL_BOW || Type >= MODEL_BOW + MAX_ITEM_INDEX) || Type == (MODEL_BOW + 23) || (Type > (MODEL_BOW + 25) && WeaponKind == ItemKind2::BOW))
		&& !g_isCharacterBuff(pObject, eDeBuff_Stun)
		&& !g_isCharacterBuff(pObject, eDeBuff_Sleep))
	{
		float myPlaySpeed = f->PlaySpeed;
#ifdef fps_new_system
		myPlaySpeed *= gsteady_clock->GetNormalizerFps();
#endif // fps_new_system
		pModel->PlayAnimation(&f->AnimationFrame, &f->PriorAnimationFrame, &f->PriorAction, myPlaySpeed, Position, Object->Angle);
	}

	vec3_t Temp;
	Object->Position[0] = pModel->BodyOrigin[0];
	Object->Position[1] = pModel->BodyOrigin[1];
	Object->Position[2] = pModel->BodyOrigin[2];
	pModel->Animation(BoneTransform, f->AnimationFrame, f->PriorAnimationFrame, f->PriorAction, Object->Angle, Object->Angle, true, true);

#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
	if (!g_CMonkSystem.IsRagefighterCommonWeapon(c->Class, Type) || Link)
	{
		pModel->Transform(BoneTransform, Temp, Temp, &OBB, Translate, 0.0);
	}
	else
	{
		float _KnightScale = 0.9f;
		if (Type == (MODEL_MACE + 2))
			pModel->InterpolationTrans(BoneTransform[0], BoneTransform[2], _KnightScale);
		pModel->Transform(BoneTransform, Temp, Temp, &OBB, Translate, _KnightScale);
	}
#else
	pModel->Transform(BoneTransform, Temp, Temp, &OBB, Translate);
#endif //PBG_ADD_NEWCHAR_MONK_ITEM

	pModel->HideSkin = false;
	if (c->MonsterIndex == 67 || c->MonsterIndex == 137)
	{
		RenderType |= (RENDER_EXTRA | RENDER_TEXTURE);
	}
	else
	{
		RenderType |= RENDER_TEXTURE;
	}

	RenderPartObjectEffect(Object, Type, c->Light, pObject->Alpha, Level << 3, Option1, false, 0, RenderType);

	float Luminosity;
	vec3_t Light;
	Luminosity = (float)(rand() % 30 + 70) * 0.005f;
	switch (Type)
	{
	case MODEL_BOW + 4:
	case MODEL_BOW + 5:
	case MODEL_BOW + 6:
	case MODEL_BOW + 17:
		if (Type == MODEL_BOW + 6)
		{
			Vector(Luminosity * 0.6f, Luminosity * 1.f, Luminosity * 0.8f, Light);

			for (int i = 13; i <= 18; i++)
				RenderBrightEffect(pModel, BITMAP_SHINY + 1, i, 1.f, Light, pObject);
		}
		else if (Type == MODEL_BOW + 17)
		{
			Vector(Luminosity * 0.5f, Luminosity * 0.5f, Luminosity * 0.8f, Light);

			for (int i = 13; i <= 18; i++)
				RenderBrightEffect(pModel, BITMAP_SHINY + 1, i, 1.f, Light, pObject);

			for (int i = 5; i <= 8; i++)
				RenderBrightEffect(pModel, BITMAP_SHINY + 1, i, 1.f, Light, pObject);
		}
		else
		{
			Vector(Luminosity * 1.f, Luminosity * 0.6f, Luminosity * 0.2f, Light);
			RenderBrightEffect(pModel, BITMAP_SHINY + 1, 2, 1.f, Light, pObject);
			RenderBrightEffect(pModel, BITMAP_SHINY + 1, 6, 1.f, Light, pObject);
		}
		break;
	case MODEL_STAFF + 10:
		Vector(Luminosity * 1.f, Luminosity * 0.3f, Luminosity * 0.1f, Light);

		for (int i = 0; i < 10; ++i)
		{
			vec3_t Light2;
			Vector(0.4f, 0.4f, 0.4f, Light2);
			Vector(i * 30.f - 180.f, -40.f, 0.f, p);
			pModel->TransformPosition(BoneTransform[0], p, Position, true);

			if ((rand() % 3) == 0)
			{
				CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light2, pObject, (float)(rand() % 360));
			}
			CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
		}
		break;
	case MODEL_BOW + 21:
	{
		Vector(0.8f, 0.8f, 0.2f, Light);
		Vector(0.f, 0.f, 0.f, p);
		if (rand() % 2 == 1)
		{
			pModel->TransformPosition(BoneTransform[4], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 11, 0.8f);

			pModel->TransformPosition(BoneTransform[12], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 11, 0.8f);
		}
		Vector(0.5f, 0.5f, 0.1f, Light);
		pModel->TransformPosition(BoneTransform[7], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		pModel->TransformPosition(BoneTransform[15], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		Vector(20.f, 0.f, 0.f, p);
		Vector(1.0f, 1.0f, 0.4f, Light);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);
	}
	break;
	case MODEL_MACE + 14:
	{
		Vector(1.0f, 0.3f, 0.0f, Light);

		float fRendomPos = (float)(rand() % 60) / 20.0f - 1.5f;
		float fRendomScale = (float)(rand() % 10) / 20.0f + 1.4f;
		Vector(0.f, -100.f + fRendomPos, fRendomPos, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale - 0.3f, Light, pObject, 20.0f);

		fRendomPos = (float)(rand() % 60) / 20.0f - 1.5f;
		fRendomScale = (float)(rand() % 10) / 20.0f + 1.0f;
		Vector(0.f, -100.f + fRendomPos, fRendomPos, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, fRendomScale + 0.3f, Light, pObject);

		fRendomPos = (float)(rand() % 40) / 20.0f - 1.0f;
		fRendomScale = (float)(rand() % 8) / 20.0f + 0.4f;
		Vector(0.f, 100.f + fRendomPos, fRendomPos, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale - 0.2f, Light, pObject, 90.0f);
	}
	break;
	case MODEL_SWORD + 22:
	{
		float fLight = (float)sinf((WorldTime) * 0.4f) * 0.25f + 0.7f;
		Vector(fLight, fLight - 0.5f, fLight - 0.5f, Light);

		Vector(5.f, -22.f, -10.f, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_LIGHT + 1, Position, 0.75f, Light, pObject);

		Vector(-5.f, -22.f, -10.f, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_LIGHT + 1, Position, 0.75f, Light, pObject);
	}
	break;
	case MODEL_SWORD + 23:
	{
		float fRendomPos = (float)(rand() % 60) / 20.0f - 1.5f;
		float fRendomScale = (float)(rand() % 30) / 20.0f + 1.5f;
		float fLight = (float)sinf((WorldTime) * 0.7f) * 0.2f + 0.5f;

		float fRotation = (WorldTime * 0.0006f) * 360.0f;
		float fRotation2 = (WorldTime * 0.0006f) * 360.0f;

		Vector(0.2f, 0.2f, fLight, Light);

		Vector(0.f, fRendomPos, fRendomPos, p);
		pModel->TransformPosition(BoneTransform[4], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale, Light, pObject, fRotation);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale - 0.4f, Light, pObject, 90.f + fRotation2);
		Vector(0.0f, 0.0f, 0.0f, p);
		CreateSprite(BITMAP_LIGHT, Position, 2.3f, Light, pObject);

		Vector(30.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[4], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[6], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		pModel->TransformPosition(BoneTransform[7], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		pModel->TransformPosition(BoneTransform[8], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.0f, Light, pObject);
	}
	break;
	case MODEL_STAFF + 12:
	{
		Vector(0.4f, 0.4f, 0.4f, Light);
		float fRendomPos = (float)(rand() % 60) / 20.0f - 1.5f;
		float fRendomScale = (float)(rand() % 15) / 20.0f + 1.8f;
		Vector(0.f, -170.f + fRendomPos, 0.f + fRendomPos, p);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		CreateSprite(BITMAP_SPARK + 1, Position, fRendomScale, Light, pObject);

		VectorCopy(Position, pObject->EyeLeft);
		CreateJointSync(BITMAP_JOINT_ENERGY, Position, Position, pObject->Angle, 17, pObject, 30.f);

		fRendomPos = (float)(rand() % 60) / 20.0f - 1.5f;
		fRendomScale = (float)(rand() % 15) / 20.0f + 1.0f;
		Vector(0.f, -170.f + fRendomPos, 0.f + fRendomPos, p);
		Vector(1.0f, 0.4f, 1.0f, Light);
		CreateSprite(BITMAP_LIGHT, Position, fRendomScale, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, fRendomScale - 0.3f, Light, pObject, 90.0f);
		CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 11, 2.0f);

		float fLight = (float)sinf((WorldTime) * 0.7f) * 0.2f + 0.5f;
		float fRotation = (WorldTime * 0.0006f) * 360.0f;

		Vector(fLight - 0.1f, 0.1f, fLight - 0.1f, Light);
		Vector(0.f, 10.0f, 0.0f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.5f, Light, pObject, fRotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.2f, Light, pObject, 90.0f + fRotation);

		Vector(-40.f, -10.0f, 0.0f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.0f, Light, pObject, fRotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.7f, Light, pObject, 90.0f + fRotation);

		Vector(-160.f, -5.0f, 0.0f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.2f, Light, pObject, fRotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.0f, Light, pObject, 90.0f + fRotation);
	}
	break;
	case MODEL_SWORD + 24:
	{
		Vector(0.6f, 0.6f, 0.6f, Light);

		Vector(0.f, 0.f, 3.f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT + 1, Position, 0.45f, Light, pObject);

		Vector(0.f, 0.f, -3.f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT + 1, Position, 0.45f, Light, pObject);

		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.7f, Light, pObject);

		float fScale = 0.5f + (float)(rand() % 100) / 180;
		float fRotation = (WorldTime * 0.0006f) * 360.0f;
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, fScale, Light, pObject, fRotation);

		// Flare01.jpg
		float fLight = (float)sinf((WorldTime) * 0.4f) * 0.25f + 0.6f;
		Vector(fLight, fLight, fLight, Light);
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 3.f, Light, pObject, 0.f);
	}
	break;
	case MODEL_SWORD + 25:
	{
		float fLight, fScale, fRotation;
		static float fPosition = 0.0f;
		static int iRandom;

		Vector(1.0f, 0.1f, 0.0f, Light);
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.0f, Light, pObject);	// flare01.jpg

		fScale = (float)(rand() % 30) / 60.0f + 0.2f;
		fLight = (float)sinf((WorldTime) * 0.7f) * 0.2f + 0.3f;
		fRotation = (WorldTime * 0.0006f) * 360.0f;
		Vector(0.1f + fLight, 0.2f, 0.0f, Light);
		CreateSprite(BITMAP_LIGHT + 3, Position, fScale, Light, pObject, fRotation);	// impact01.jpg

		Vector(1.0f - fLight, 0.0f, 0.0f, Light);
		if (fPosition >= 20.0f)
		{
			iRandom = rand() % 5 + 2;	// 2 ~ 6
			fPosition = 0.0f;
			//Vector(1.0f, 0.0f, 0.0f, Light);
		}
		else
			fPosition += 1.5f;
		Vector(0.f, fPosition, 0.f, p);
		pModel->TransformPosition(BoneTransform[iRandom], p, Position, true);
		CreateSprite(BITMAP_WATERFALL_4, Position, 0.7f, Light, pObject);
		Vector(0.1f, 0.1f, 0.1f, Light);
		CreateSprite(BITMAP_WATERFALL_2, Position, 0.3f, Light, pObject);

		// Flare01
		fLight = (float)sinf((WorldTime) * 0.4f) * 0.25f + 0.2f;
		Vector(0.8f + fLight, 0.1f, 0.f, Light);
		Vector(0.f, 0.f, 0.f, p);
		for (int i = 0; i < 5; ++i)
		{
			pModel->TransformPosition(BoneTransform[2 + i], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 1.1f, Light, pObject);
		}

		if (pObject->CurrentAction == PLAYER_RUN_TWO_HAND_SWORD_TWO && World != WD_10HEAVEN && rand() % 2 == 0)
		{
			if (!g_Direction.m_CKanturu.IsMayaScene())
			{
				Vector(1.f, 1.f, 1.f, Light);
				Vector(0.f, 0.f, 0.f, p);
				pModel->TransformPosition(BoneTransform[6], p, Position, true);
				Position[0] += rand() % 30 - 15.f;
				Position[1] += rand() % 30 - 15.f;
				Position[2] += 20.f;

				vec3_t	Angle;
				for (int i = 0; i < 4; i++)
				{
					Vector((float)(rand() % 60 + 60 + 90), 0.f, pObject->Angle[2], Angle);//(float)(rand()%30),Angle);
					CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, Angle);
					CreateParticleSync(BITMAP_SPARK, Position, Angle, Light);
				}
			}
		}
	}
	break;
	case MODEL_MACE + 15:
	{
		float fScale;
		Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.3f + 0.1f;
		fScale = (float)(rand() % 10) / 30.0f + 1.7f;

		Vector(0.f, 0.f, 0.f, p);

		Vector(0.8f + Luminosity, 0.5f + Luminosity, 0.1f + Luminosity, Light);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, fScale, Light, pObject);

		Vector(0.7f, 0.5f, 0.3f, Light);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 3.f, Light, pObject);

		Vector(0.8f, 0.6f, 0.4f, Light);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);

		Vector(0.8f, 0.6f, 0.4f, Light);
		pModel->TransformPosition(BoneTransform[3], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.2f, Light, pObject);
	}
	break;
	case MODEL_BOW + 22:
	{
		float fLight;
		fLight = (float)sinf((WorldTime) * 0.4f) * 0.25f;
		Vector(0.0f, 0.7f, 0.0f, Light);
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[10], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.2f, Light, pObject);

		pModel->TransformPosition(BoneTransform[28], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.2f, Light, pObject);

		pModel->TransformPosition(BoneTransform[34], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.2f, Light, pObject);

		pModel->TransformPosition(BoneTransform[16], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.2f, Light, pObject);

		Vector(0.f, 0.f, 0.f, p);
		Vector(0.3f, 0.9f, 0.2f, Light);
		if (rand() % 2 == 1)
		{
			pModel->TransformPosition(BoneTransform[10], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 14, 0.05f);
			pModel->TransformPosition(BoneTransform[28], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 14, 0.05f);
			pModel->TransformPosition(BoneTransform[34], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 14, 0.05f);
			pModel->TransformPosition(BoneTransform[16], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, pObject->Angle, Light, 14, 0.05f);
		}
	}
	break;
	case MODEL_STAFF + 13:
	{
		float fLight, fScale, fRotation;
		fLight = (float)sinf((WorldTime) * 0.7f) * 0.2f + 0.3f;
		Vector(0.f, 0.f, 0.f, p);

		// flare01
		Vector(0.7f, 0.7f, 0.7f, Light);
		fScale = (float)(rand() % 10) / 500.0f;
		pModel->TransformPosition(BoneTransform[9], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f + fScale, Light, pObject);
		//			Vector(0.1f, 0.5f, 0.1f, Light);
		//			CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);

		Vector(0.2f, 0.6f + fLight, 0.2f, Light);
		fScale = (float)(rand() % 10) / 500.0f;
		pModel->TransformPosition(BoneTransform[9], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 4.0f + fScale, Light, pObject);

		// shiny02
		Vector(0.4f, 0.5f + fLight, 0.4f, Light);
		fScale = (float)(rand() % 30) / 60.0f;
		fRotation = (WorldTime * 0.0004f) * 360.0f;
		pModel->TransformPosition(BoneTransform[9], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.2f + fScale, Light, pObject, -fRotation);

		// magic_ground1
		Vector(0.6f, 1.f, 0.6f, Light);
		fScale = (float)(rand() % 10) / 500.0f;
		fRotation = (WorldTime * 0.0006f) * 360.0f;
		pModel->TransformPosition(BoneTransform[9], p, Position, true);
		CreateSprite(BITMAP_MAGIC, Position, 0.25f + fScale, Light, pObject, fRotation);

		// 5, 6, 7, 8 flare01 , 10, 11, 12, 13
		Vector(0.f, 0.f, 0.f, p);

		for (int i = 0; i < 4; ++i)
		{
			Vector(0.1f, 0.8f, 0.1f, Light);
			pModel->TransformPosition(BoneTransform[5 + i], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 0.2f, Light, pObject);
			Vector(0.5f, 0.5f, 0.5f, Light);
			pModel->TransformPosition(BoneTransform[10 + i], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		}

		Vector(0.1f, 0.8f, 0.1f, Light);
		pModel->TransformPosition(BoneTransform[4], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.6f + fScale, Light, pObject, fRotation);

		Vector(0.6f, 1.f, 0.6f, Light);
		fScale = (float)(rand() % 10) / 500.0f;
		fRotation = (WorldTime * 0.0006f) * 360.0f;
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_MAGIC, Position, 0.15f + fScale, Light, pObject, fRotation);
		Vector(0.8f, 0.8f, 0.8f, Light);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 0.7f + fScale, Light, pObject, -fRotation);
		Vector(0.1f, 1.0f, 0.1f, Light);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f + fScale, Light, pObject, fRotation);

		Vector(0.2f, 0.6f + fLight, 0.2f, Light);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f + fScale, Light, pObject);
	}
	break;
	case MODEL_STAFF + 11:
	{
		Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.3f + 0.7f;

		Vector(0.f, 0.f, 0.f, p);
		Vector(Luminosity * 0.f, Luminosity * 0.5f, Luminosity * 1.f, Light);

		float Rotation = (float)(rand() % 360);
		pModel->TransformPosition(BoneTransform[5], p, Position, true);
		Vector(Luminosity * 0.f, Luminosity * 0.5f, Luminosity * 1.f, Light);
		CreateSprite(BITMAP_LIGHT, Position, 1.f, Light, pObject);
		Vector(0.5f, 0.5f, 0.5f, Light);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f, Light, pObject, Rotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f, Light, pObject, 360.f - Rotation);

		pModel->TransformPosition(BoneTransform[6], p, Position, true);
		Vector(Luminosity * 0.f, Luminosity * 0.5f, Luminosity * 1.f, Light);
		CreateSprite(BITMAP_LIGHT, Position, 1.f, Light, pObject);
		Vector(0.5f, 0.5f, 0.5f, Light);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f, Light, pObject, Rotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f, Light, pObject, 360.f - Rotation);

		pModel->TransformPosition(BoneTransform[8], p, Position, true);
		Vector(Luminosity * 0.f, Luminosity * 0.5f, Luminosity * 1.f, Light);
		CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
		Vector(0.5f, 0.5f, 0.5f, Light);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.f, Light, pObject, Rotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.f, Light, pObject, 360.f - Rotation);
	}
	break;
	case MODEL_MACE + 12:
	{
		float Rotation = (float)(rand() % 360);
		Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.3f + 0.7f;

		Vector(0.f, 0.f, 0.f, p);

		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		Vector(Luminosity * 0.f, Luminosity * 0.5f, Luminosity * 1.f, Light);
		CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
		Vector(0.5f, 0.5f, 0.5f, Light);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.f, Light, pObject, Rotation);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.f, Light, pObject, 360.f - Rotation);
	}
	break;
	case MODEL_SWORD + 19:
		Vector(Luminosity * 1.f, Luminosity * 0.3f, Luminosity * 0.1f, Light);

		Vector(0.f, 0.f, 0.f, p);

		for (int i = 0; i < 7; ++i)
		{
			vec3_t Light2;
			Vector(0.4f, 0.4f, 0.4f, Light2);
			pModel->TransformPosition(BoneTransform[i + 2], p, Position, true);

			if ((rand() % 3) == 0)
			{
				CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light2, pObject, (float)(rand() % 360));
			}
			CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
		}

		if (((pObject->CurrentAction<PLAYER_WALK_MALE || pObject->CurrentAction>PLAYER_RUN_RIDE_WEAPON) &&
			(pObject->CurrentAction<PLAYER_HIT_SKILL_SWORD1 || pObject->CurrentAction>PLAYER_HIT_SKILL_SWORD5)))
		{
			vec3_t pos, delta, angle;

			Vector(0.f, 0.f, 0.f, p);
			Vector(-90.f, (float)(rand() % 360), pObject->Angle[2] - 45, angle);
			pModel->TransformPosition(BoneTransform[3], p, pos, true);
			pModel->TransformPosition(BoneTransform[2], p, Position, true);

			VectorSubtract(pos, Position, delta);
		}
		break;
	case MODEL_BOW + 18:
		Vector(Luminosity * 1.f, Luminosity * 0.3f, Luminosity * 0.1f, Light);
		break;
	case MODEL_BOW + 19:
		Vector(0.f, 0.f, 10.f, p);

		for (int i = 1; i < 6; ++i)
		{
			Vector(Luminosity * 0.5f, Luminosity * 0.5f, Luminosity * 0.8f, Light);
			pModel->TransformPosition(BoneTransform[i], p, Position, true);

			CreateSprite(BITMAP_SHINY + 1, Position, 1.f, Light, pObject);

			if (i == 5)
			{
				Vector(1.f, 1.f, 1.f, Light);
				CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
			}
			else
				CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
		}
		break;
	case MODEL_SHIELD + 15:
		Vector(Luminosity * 0.6f, Luminosity * 0.6f, Luminosity * 2.f, Light);

		Vector(15.f, -15.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_SHINY + 1, Position, 1.5f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, Luminosity + 1.5f, Light, pObject);
		break;
	case MODEL_MACE + 7:
		Vector(Luminosity * 1.f, Luminosity * 0.9f, Luminosity * 0.f, Light);

		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);

		Vector(0.5f, 0.5f, 0.5f, Light);
		CreateSprite(BITMAP_LIGHT, Position, sinf(WorldTime * 0.002f) + 0.5f, Light, pObject);
		break;
	case MODEL_MACE + 8:
	{
		float Scale = sinf(WorldTime * 0.001f) + 1.f;
		Vector(Luminosity * 0.2f, Luminosity * 0.1f, Luminosity * 3.f, Light);
		Vector(-15.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, Scale, Light, pObject);
		Vector(10.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, Scale, Light, pObject);

		Scale = sinf(WorldTime * 0.01f) * 360;
		Luminosity = sinf(WorldTime * 0.001f) * 0.5f + 0.5f;
		Vector(Luminosity, Luminosity, Luminosity, Light);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light, pObject, 360 - Scale);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light, pObject, Scale);
	}
	break;
	case MODEL_MACE + 9:
	{
		for (int i = 1; i < 5; i++)
		{
			Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.35f + 0.65f;
			Vector(Luminosity * 0.6f, Luminosity * 0.8f, Luminosity * 1.f, Light);
			Vector(-10.f, 0.f, 0.f, p);
			pModel->TransformPosition(BoneTransform[i + 1], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, /*Luminosity**/1.f, Light, pObject);

			if (i == 3)
			{
				Vector(0.5f, 0.5f, 0.5f, Light);
				CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light, pObject, rand() % 360);
			}
		}
	}
	break;
	case MODEL_MACE + 10:
	{
		Luminosity = sinf(WorldTime * 0.001f) * 0.5f + 0.7f;
		Vector(Luminosity * 1.f, Luminosity * 0.8f, Luminosity * 0.6f, Light);
		Vector(0.f, 0.f, 0.f, p);

		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, Luminosity * 0.5f, Light, pObject);

		Vector(Luminosity * -10.f, 0.f, 0.f, p);
		Vector(0.6f, 0.8f, 1.f, Light);
		Luminosity = rand() % 360;
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 0.7f, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.5f, Light, pObject, Luminosity);
		CreateSprite(BITMAP_SHINY + 1, Position, 0.4f, Light, pObject, 360 - Luminosity);
	}
	break;
	case MODEL_MACE + 11:
	{
		Vector(1.f, 0.6f, 0.3f, Light);
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[1], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, pObject);

		for (int i = 0; i < 3; ++i)
		{
			Vector(i * 15.f - 10.f, 0.f, 0.f, p);
			pModel->TransformPosition(BoneTransform[2], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, pObject);
			CreateSprite(BITMAP_SHINY + 1, Position, 0.4f, Light, pObject, rand() % 360);
		}
	}
	break;
	case MODEL_MACE + 13:
	{
		Vector(Luminosity * 1.f, Luminosity * 0.3f, Luminosity * 0.1f, Light);
		Vector(0.f, 0.f, 0.f, p);
		vec3_t Light2;
		Vector(0.4f, 0.4f, 0.4f, Light2);
		pModel->TransformPosition(BoneTransform[0], p, Position, true);
		if ((rand() % 3) == 0)
		{
			CreateSprite(BITMAP_SHINY + 1, Position, 0.6f, Light2, pObject, (float)(rand() % 360));
		}
		CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);
	}
	break;
	case MODEL_SWORD + 20:
	{
		for (int i = 0; i < 2; i++) {
			Vector(0.f, 0.f, 0.f, p);
			pModel->TransformPosition(BoneTransform[i + 1], p, Position, true);
			Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.35f + 0.65f;
			Vector(Luminosity * 0.43f, Luminosity * 0.14f, Luminosity * 0.6f, Light);
			CreateSprite(BITMAP_LIGHT, Position, Luminosity * 0.9f, Light, pObject);
			Vector(0.3f, 0.3f, 0.3f, Light);
			CreateSprite(BITMAP_LIGHT, Position, float(sinf(WorldTime * 0.002f) * 0.5f) + 0.4f, Light, pObject);
		}
	}
	break;
	case MODEL_BOW + 20:
	{
		float Scale = sinf(WorldTime * 0.001f) + 1.f;
		Vector(Luminosity * 3.f, Luminosity, Luminosity, Light);
		Vector(0.f, 0.f, 0.f, p);
		pModel->TransformPosition(BoneTransform[6], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, Scale * 0.8f, Light, pObject);
		pModel->TransformPosition(BoneTransform[2], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, Scale * 0.8f, Light, pObject);

		float Rotation = sinf(WorldTime * 0.01f) * 360;
		Luminosity = sinf(WorldTime * 0.001f) * 0.3 + 0.3f;
		Vector(Luminosity, Luminosity, Luminosity, Light);
		if (!c->SafeZone)
		{
			Vector(10.f, 0.f, 0.f, p);
			pModel->TransformPosition(BoneTransform[9], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, Scale * 0.8f, Light, pObject);
			Vector(Luminosity * 3.0f, Luminosity, Luminosity, Light);
			CreateSprite(BITMAP_SHINY + 1, Position, 0.8f, Light, pObject, 360 - Rotation);
			CreateSprite(BITMAP_SHINY + 1, Position, 0.8f, Light, pObject, Rotation);
		}
	}
	break;
	case MODEL_SWORD + 21:
	case MODEL_SWORD + 31:
		if (pObject->CurrentAction == PLAYER_RUN_TWO_HAND_SWORD_TWO && World != WD_10HEAVEN && rand() % 2 == 0)
		{
			if (!g_Direction.m_CKanturu.IsMayaScene())
			{
				Vector(1.f, 1.f, 1.f, Light);
				Vector(0.f, 0.f, 0.f, p);
				pModel->TransformPosition(BoneTransform[1], p, Position, true);
				Position[0] += rand() % 30 - 15.f;
				Position[1] += rand() % 30 - 15.f;
				Position[2] += 20.f;

				vec3_t	Angle;
				for (int i = 0; i < 4; i++)
				{
					Vector((float)(rand() % 60 + 60 + 90), 0.f, pObject->Angle[2], Angle);//(float)(rand()%30),Angle);
					CreateJointSync(BITMAP_JOINT_SPARK, Position, Position, Angle);
					CreateParticleSync(BITMAP_SPARK, Position, Angle, Light);
				}
			}
		}
		break;
	case MODEL_SPEAR + 10:
		Vector(Luminosity * 0.2f, Luminosity * 0.1f, Luminosity * 0.8f, Light);
		Vector(0.f, 0.f, 0.f, p);

		for (int i = 1; i < 9; i++)
		{
			pModel->TransformPosition(BoneTransform[i], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, pObject);
		}
		break;
	case MODEL_SWORD + 26:
	{
		Vector(0.8f, 0.6f, 0.2f, Light);
		pModel->TransformByObjectBone(Position, Object, 11);		// Gold01
		CreateSprite(BITMAP_LIGHT, Position, 0.6f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 0.6f, Light, pObject);

		Vector(0.3f, 0.8f, 0.7f, Light);
		pModel->TransformByObjectBone(Position, Object, 12);		// b01
		CreateSprite(BITMAP_LIGHT, Position, 0.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 13);		// n02
		CreateSprite(BITMAP_LIGHT, Position, 0.5f, Light, pObject);

		pModel->TransformByObjectBone(Position, Object, 12);		// n04
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 13);		// b03
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);

		Vector(0.9f, 0.1f, 0.1f, Light);
		pModel->TransformByObjectBone(Position, Object, 1);		// Zx01
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.8f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 2);		// Zx02
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.7f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 3);		// Zx03
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.7f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 4);		// Zx04
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.7f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 5);		// Zx05
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.6f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 6);		// Zx06
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 7);		// Zx07
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.5f, Light, pObject);

	}break;
	case MODEL_SWORD + 27:
	{
		Vector(0.1f, 0.9f, 0.1f, Light);

		pModel->TransformByObjectBone(Position, Object, 1);		// Zx01
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.6f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 2);		// Zx02
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 3);		// Zx03
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 4);		// Zx04
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 5);		// Zx05
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.4f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 6);		// Zx06
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.3f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 7);		// Zx07
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.3f, Light, pObject);

		for (int i = 1; i <= 7; i++)
		{
			if (rand() % 4 != 0)
			{
				continue;
			}
			Vector(0.f, 0.f, 0.f, Position);
			pModel->TransformByObjectBone(Position, Object, i);
			CreateParticleSync(BITMAP_WATERFALL_4, Position, Object->Angle, Light, 12, 0.5f, Object);
		}
	}break;
	case MODEL_SWORD + 28:
	{
		float fRendomScale = (float)((rand() % 15) / 30.0f) + 0.5f;
		Vector(0.f, 0.f, 0.f, Position);
		Vector(0.1f, 0.4f, 0.9f, Light);
		pModel->TransformPosition(BoneTransform[8], Position, p, true);		// Zx01
		CreateSprite(BITMAP_FLARE_BLUE, p, 0.4f, pObject->Light, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, fRendomScale, Light, pObject);

		// 잔상 Zx01
		vec3_t vColor;
		VectorCopy(p, pObject->EyeLeft);
		Vector(0.f, 0.f, 0.9f, vColor);
		CreateJointSync(BITMAP_JOINT_ENERGY, p, p, pObject->Angle, 17, pObject, 25.f);
		//CreateEffect(MODEL_EFFECT_TRACE, p, pObject->Angle, vColor, 0, NULL, -1, 0, 0, 0, 25.f);

		pModel->TransformPosition(BoneTransform[9], Position, p, true);		// Zx02
		CreateSprite(BITMAP_FLARE_BLUE, p, 0.4f, pObject->Light, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, fRendomScale, Light, pObject);
		pModel->TransformPosition(BoneTransform[10], Position, p, true);		// Zx03
		CreateSprite(BITMAP_FLARE_BLUE, p, 0.4f, pObject->Light, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, 0.4f, Light, pObject);
		pModel->TransformPosition(BoneTransform[11], Position, p, true);		// Zx04
		CreateSprite(BITMAP_FLARE_BLUE, p, 0.4f, pObject->Light, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, 0.4f, Light, pObject);

		// 칼주변
		Vector(0.0f, 0.3f, 0.7f, Light);
		pModel->TransformPosition(BoneTransform[2], Position, p, true);		// rx01
		CreateSprite(BITMAP_LIGHTMARKS, p, 1.0f, Light, pObject);
		pModel->TransformPosition(BoneTransform[3], Position, p, true);		// rx02
		CreateSprite(BITMAP_LIGHTMARKS, p, 0.8f, Light, pObject);
		pModel->TransformPosition(BoneTransform[4], Position, p, true);		// rx03
		CreateSprite(BITMAP_LIGHTMARKS, p, 0.6f, Light, pObject);
		pModel->TransformPosition(BoneTransform[5], Position, p, true);		// rx04
		CreateSprite(BITMAP_LIGHTMARKS, p, 0.4f, Light, pObject);
		pModel->TransformPosition(BoneTransform[6], Position, p, true);		// Zx05
		CreateSprite(BITMAP_LIGHTMARKS, p, 0.2f, Light, pObject);
		pModel->TransformPosition(BoneTransform[7], Position, p, true);		// Zx06
		CreateSprite(BITMAP_LIGHTMARKS, p, 0.1f, Light, pObject);
	}break;
	case MODEL_MACE + 16:
	{
		vec3_t vDPos;
		Vector(0.5f, 0.8f, 0.5f, Light);
		// Zx04
		pModel->TransformByObjectBone(Position, Object, 11);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, Light, pObject);
		// Zx05
		pModel->TransformByObjectBone(Position, Object, 10);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, Light, pObject);

		Vector(0.9f, 0.1f, 0.3f, Light);
		// Zx02
		pModel->TransformByObjectBone(Position, Object, 9);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, Light, pObject);
		// Zx03
		pModel->TransformByObjectBone(Position, Object, 8);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, Light, pObject);
		// Zx01
		pModel->TransformByObjectBone(Position, Object, 1);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, pObject);

		Vector(0.5f, 0.8f, 0.6f, Light);
		// Zx001
		pModel->TransformByObjectBone(Position, Object, 24);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.3f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);
		CreateSprite(BITMAP_PIN_LIGHT, Position, 0.5f, Light, pObject, ((int)(WorldTime * 0.04f) % 360));
		CreateSprite(BITMAP_PIN_LIGHT, Position, 0.7f, Light, pObject, -((int)(WorldTime * 0.03f) % 360));
		CreateSprite(BITMAP_PIN_LIGHT, Position, 0.9f, Light, pObject, ((int)(WorldTime * 0.02f) % 360));

		if (rand() % 3 != 0)
		{
			float fTemp = Position[2];
			Position[2] -= 15.f;
			Vector(0.5f, 0.8f, 0.8f, Light);
			CreateParticleSync(BITMAP_CLUD64, Position, pObject->Angle, Light, 9, 0.4f);
			Position[2] = fTemp;
		}


		Vector(0.5f, 0.8f, 0.6f, Light);
		vDPos[0] = Position[0] + ((float)(rand() % 20 - 10) * 3.f);
		vDPos[1] = Position[1] + ((float)(rand() % 20 - 10) * 3.f);
		vDPos[2] = Position[2] + ((float)(rand() % 20 - 10) * 3.f);

		if (rand() % 10 == 0)
		{
			CreateEffectSync(MODEL_STAR_SHINE, vDPos, pObject->Angle, Light, 0, Object, -1, 0, 0, 0, 0.22f);
		}

		// Zx06
		pModel->TransformByObjectBone(Position, Object, 12);
		CreateSprite(BITMAP_LIGHT, Position, 0.2f, Light, pObject);
		// Zx07
		pModel->TransformByObjectBone(Position, Object, 13);
		CreateSprite(BITMAP_LIGHT, Position, 0.2f, Light, pObject);
		// Zx08
		pModel->TransformByObjectBone(Position, Object, 14);
		CreateSprite(BITMAP_LIGHT, Position, 0.2f, Light, pObject);
		// Zx09
		pModel->TransformByObjectBone(Position, Object, 15);
		CreateSprite(BITMAP_LIGHT, Position, 0.2f, Light, pObject);
		// Zx10
		pModel->TransformByObjectBone(Position, Object, 16);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		// Zx11
		pModel->TransformByObjectBone(Position, Object, 17);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		// Zx12
		pModel->TransformByObjectBone(Position, Object, 18);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		// Zx13
		pModel->TransformByObjectBone(Position, Object, 19);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		// Zx14
		pModel->TransformByObjectBone(Position, Object, 20);
		CreateSprite(BITMAP_LIGHT, Position, 0.3f, Light, pObject);
		// Zx15
		pModel->TransformByObjectBone(Position, Object, 21);
		CreateSprite(BITMAP_LIGHT, Position, 0.4f, Light, pObject);
		// Zx16
		pModel->TransformByObjectBone(Position, Object, 22);
		CreateSprite(BITMAP_LIGHT, Position, 0.4f, Light, pObject);
		// Zx17
		pModel->TransformByObjectBone(Position, Object, 23);
		CreateSprite(BITMAP_LIGHT, Position, 0.4f, Light, pObject);
	}break;
	case MODEL_MACE + 17:
	{
		float fRandomScale;
		vec3_t vPosZx01, vPosZx02, vLight1, vLight2, vDLight;

		float fLumi = absf((sinf(WorldTime * 0.0008f))) * 0.8 + 0.2f;
		Vector(fLumi * 0.6f, fLumi * 0.5f, fLumi * 0.8f, vDLight);

		Vector(0.6f, 0.5f, 0.8f, vLight1);
		Vector(0.8f, 0.8f, 0.8f, vLight2);
		pModel->TransformByObjectBone(vPosZx01, Object, 3);		// Zx01
		pModel->TransformByObjectBone(vPosZx02, Object, 4);		// Zx02

		if (((int)WorldTime / 100) % 10 == 0)
		{
			Object->m_iAnimation = rand() % 100;
			Object->EyeRight[0] = (rand() % 10 - 5);
			Object->EyeRight[1] = (rand() % 10 - 5);
			Object->EyeRight[2] = (rand() % 10 - 5);
			Object->EyeRight2[0] = (rand() % 10 - 5) * 1.2f;
			Object->EyeRight2[1] = (rand() % 10 - 5) * 1.2f;
			Object->EyeRight2[2] = (rand() % 10 - 5) * 1.2f;
		}
		// Object->m_iAnimation Random Texture
		int iRandomTexure1, iRandomTexure2;
		iRandomTexure1 = (Object->m_iAnimation / 10) % 3;	// 3개
		iRandomTexure2 = (Object->m_iAnimation) % 3;		// 3개

		// Zx01
		fRandomScale = (float)(rand() % 10) / 10.0f + 1.0f;		//(1.0~2.0)
		CreateSprite(BITMAP_LIGHT, vPosZx01, fRandomScale, vLight1, pObject);
		CreateSprite(BITMAP_SHINY + 1, vPosZx01, 0.5f, vDLight, pObject);
		VectorAdd(vPosZx01, Object->EyeRight, vPosZx01);
		CreateSprite(BITMAP_LIGHTNING_MEGA1 + iRandomTexure1, vPosZx01, (((rand() % 11) - 20) / 100.f) + 0.5f, vLight2, pObject, rand() % 380);

		// Zx02
		fRandomScale = (float)((rand() % 10) / 5.0f) + 1.5f;		//(2.0~3.25)
		CreateSprite(BITMAP_LIGHT, vPosZx02, fRandomScale, vLight1, pObject);
		CreateSprite(BITMAP_SHINY + 1, vPosZx02, 1.0f, vDLight, pObject);
		VectorAdd(vPosZx02, Object->EyeRight2, vPosZx02);
		CreateSprite(BITMAP_LIGHTNING_MEGA1 + iRandomTexure1, vPosZx02, (((rand() % 11) - 20) / 50.f) + 0.8f, vLight2, pObject, rand() % 380);
	}break;
	case MODEL_BOW + 23:
	{
		vec3_t vZX03, vZx04;
		int iNumCreateFeather = rand() % 3;

		Vector(0.2f, 0.25f, 0.3f, Light);

		for (int i = 0; i <= 43; i++)
		{
			if (i == 1)
			{
				continue;
			}
			pModel->TransformByObjectBone(Position, Object, i);	// 
			CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, Object);
		}

		if (pObject->AnimationFrame >= 4.5f && pObject->AnimationFrame <= 5.0f)
		{
			for (int i = 0; i < iNumCreateFeather; i++)
			{
				CreateEffect(MODEL_FEATHER, vZX03, pObject->Angle, Light, 0, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_FEATHER, vZX03, pObject->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_FEATHER, vZx04, pObject->Angle, Light, 0, NULL, -1, 0, 0, 0, 0.6f);
				CreateEffect(MODEL_FEATHER, vZx04, pObject->Angle, Light, 1, NULL, -1, 0, 0, 0, 0.6f);
			}
		}


	}break;
	case MODEL_STAFF + 30:
	{
		Vector(0.f, 0.f, 0.f, Position);
		Vector(0.8f, 0.3f, 0.1f, Light);
		pModel->TransformPosition(BoneTransform[4], Position, p, true);		// Rx01
		CreateSprite(BITMAP_LIGHT, p, 0.8f, Light, pObject);
		pModel->TransformPosition(BoneTransform[5], Position, p, true);		// Rx02
		CreateSprite(BITMAP_LIGHT, p, 0.4f, Light, pObject);
		pModel->TransformPosition(BoneTransform[6], Position, p, true);		// Zx02
		CreateSprite(BITMAP_LIGHT, p, 0.8f, Light, pObject);
		pModel->TransformPosition(BoneTransform[7], Position, p, true);		// Zx03
		CreateSprite(BITMAP_LIGHT, p, 0.8f, Light, pObject);
		pModel->TransformPosition(BoneTransform[8], Position, p, true);		// Zx04
		CreateSprite(BITMAP_LIGHT, p, 2.0f, Light, pObject);
		pModel->TransformPosition(BoneTransform[9], Position, p, true);		// Zx05
		CreateSprite(BITMAP_LIGHT, p, 0.8f, Light, pObject);

		float fLumi = absf((sinf(WorldTime * 0.001f))) * 0.8f + 0.2f;
		vec3_t vDLight;
		Vector(fLumi * 0.8f, fLumi * 0.1f, fLumi * 0.3f, vDLight);
		float fRendomScale = (float)(rand() % 10) / 20.0f + 0.8f;		// (0.5~1.25)
		Vector(0.8f, 0.2f, 0.4f, Light);
		pModel->TransformPosition(BoneTransform[2], Position, p, true);		// Red01
		CreateSprite(BITMAP_LIGHT, p, 2.0f, vDLight, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, fRendomScale, Light, pObject);
		pModel->TransformPosition(BoneTransform[3], Position, p, true);		// Red02
		CreateSprite(BITMAP_LIGHT, p, 2.0f, vDLight, pObject);
		CreateSprite(BITMAP_SHINY + 6, p, fRendomScale, Light, pObject);

		vec3_t vColor;
		VectorCopy(p, pObject->EyeRight);
		Vector(0.9f, 0.f, 0.f, vColor);
		CreateJoint(BITMAP_JOINT_ENERGY, p, p, pObject->Angle, 47, pObject, 25.f);

	}break;
	case MODEL_STAFF + 31:
	{
		Vector(0.f, 0.f, 0.f, Position);
		Vector(0.3f, 0.3f, 0.9f, Light);
		pModel->TransformPosition(BoneTransform[2], Position, p, true);		// Zx01
		CreateSprite(BITMAP_LIGHT, p, 2.f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, p, 2.f, Light, pObject);
		pModel->TransformPosition(BoneTransform[3], Position, p, true);		// Zx02
		CreateSprite(BITMAP_LIGHT, p, 2.5f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, p, 2.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[4], Position, p, true);		// Zx03
		CreateSprite(BITMAP_LIGHT, p, 3.f, Light, pObject);
		Vector(0.7f, 0.1f, 0.2f, Light);
		pModel->TransformPosition(BoneTransform[5], Position, p, true);		// Zx04
		CreateSprite(BITMAP_LIGHT, p, 2.f, Light, pObject);
		Vector(0.9f, 0.3f, 0.5f, Light);
		CreateSprite(BITMAP_SHINY + 6, p, 0.8f, Light, pObject);

		float fRendomScale = (float)(rand() % 15) / 20.0f + 1.0f;
		CreateSprite(BITMAP_SHINY + 1, p, fRendomScale, Light, pObject);
		CreateSprite(BITMAP_SHINY + 1, p, fRendomScale - 0.3f, Light, pObject, 90.0f);
		CreateParticleSync(BITMAP_SPARK + 1, p, pObject->Angle, Light, 11, 2.0f);

		// 잔상
		vec3_t vColor;
		VectorCopy(p, pObject->EyeLeft);
		Vector(0.f, 0.f, 0.9f, vColor);
		CreateJointSync(BITMAP_JOINT_ENERGY, p, p, pObject->Angle, 17, pObject, 30.f);
		//CreateEffect(MODEL_EFFECT_TRACE, p, pObject->Angle, vColor, 0, NULL, -1, 0, 0, 0, 30.f);

		Vector(0.7f, 0.7f, 0.7f, Light);
		CreateSprite(BITMAP_SHINY + 2, p, 2.f, Light, pObject);
	}break;
	case MODEL_STAFF + 32:
	{
		float fRandomScale;
		vec3_t vLight1, vLight2;
		Vector(0.9f, 0.7f, 0.4f, vLight1);
		Vector(0.9f, 0.1f, 0.3f, vLight2);

		pModel->TransformByObjectBone(Position, Object, 1);		// Zx01
		Vector(1.0f, 0.1f, 0.2f, Light);
		CreateSprite(BITMAP_SHINY + 6, Position, 0.7f, Light, pObject);
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.6f, Light, pObject);
		CreateSprite(BITMAP_LIGHT, Position, 2.f, Light, pObject);

		Vector(0.7f, 0.1f, 0.6f, Light);
		CreateSprite(BITMAP_SHOCK_WAVE, Position, 0.65f, Light, pObject, -((int)(WorldTime * 0.05f) % 360));
		// Object->Timer 
		// Object->EyeRight 
		Object->Timer += timefac(0.01f);
		if (Object->Timer <= 0.1f || Object->Timer > 0.9f)
		{
			Object->Timer = 0.15f;
			Vector(0.7f, 0.1f, 0.6f, Object->EyeRight);
		}
		if (Object->Timer > 0.5f)
		{
			Object->EyeRight[0] *= timepow(0.95f);
			Object->EyeRight[1] *= timepow(0.95f);
			Object->EyeRight[2] *= timepow(0.95f);
		}

		CreateSprite(BITMAP_SHOCK_WAVE, Position, Object->Timer, Object->EyeRight, pObject);

		Vector(0.9f, 0.5f, 0.2f, Light);
		fRandomScale = (float)(rand() % 5) / 25.0f + 0.3f;		// (0.3~0.4)
		CreateSprite(BITMAP_LIGHTMARKS, Position, fRandomScale, Light, pObject);

		pModel->TransformByObjectBone(Position, Object, 2);		// Zx02
		CreateSprite(BITMAP_SHINY + 6, Position, 0.3f, vLight1, pObject);
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.4f, vLight2, pObject);

		pModel->TransformByObjectBone(Position, Object, 3);		// Zx03
		CreateSprite(BITMAP_SHINY + 6, Position, 0.2f, vLight1, pObject);
		CreateSprite(BITMAP_LIGHTMARKS, Position, 0.3f, vLight2, pObject);
	}break;
	case MODEL_SHIELD + 17:
	{
		vec3_t vDLight;
		Vector(0.8f, 0.6f, 0.2f, Light);
		pModel->TransformByObjectBone(Position, Object, 4);		// Zx03
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 5);		// Zx04
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 6);		// Zx05
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 7);		// Zx06
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 8);		// Zx07
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 9);		// Zx08
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);
		pModel->TransformByObjectBone(Position, Object, 10);		// Zx09
		CreateSprite(BITMAP_LIGHT, Position, 1.5f, Light, pObject);

		float fLumi = absf((sinf(WorldTime * 0.0005f)));
		Vector(fLumi * 1.f, fLumi * 1.f, fLumi * 1.f, vDLight);
		pModel->TransformByObjectBone(Position, Object, 1);		// Zx01
		CreateSprite(BITMAP_FLARE_RED, Position, 0.5f, vDLight, pObject);
		pModel->TransformByObjectBone(Position, Object, 2);		// Zx02
		CreateSprite(BITMAP_FLARE_RED, Position, 0.3f, vDLight, pObject);
		pModel->TransformByObjectBone(Position, Object, 3);		// Zx002
		CreateSprite(BITMAP_FLARE_RED, Position, 0.3f, vDLight, pObject);


	}break;
	case MODEL_SHIELD + 18:
	{
		Vector(0.9f, 0.f, 0.2f, Light);
		pModel->TransformByObjectBone(Position, Object, 1);		// Zx01
		CreateSprite(BITMAP_LIGHT, Position, 2.0f, Light, pObject);

		Vector(1.f, 1.f, 1.f, Light);
		switch (rand() % 3)
		{
		case 0:
			CreateParticleSync(BITMAP_FIRE_HIK1, Position, Object->Angle, Light, 0, 0.7f);
			break;
		case 1:
			CreateParticleSync(BITMAP_FIRE_CURSEDLICH, Position, Object->Angle, Light, 4, 0.7f);
			break;
		case 2:
			CreateParticleSync(BITMAP_FIRE_HIK3, Position, Object->Angle, Light, 0, 0.7);
			break;
		}
	}break;
	case MODEL_SHIELD + 20:
	{
		Vector(0.f, 0.f, 0.f, Position);
		float fLumi = fabs(sinf(WorldTime * 0.001f)) + 0.1f;
		Vector(0.8f * fLumi, 0.3f * fLumi, 0.8f * fLumi, Light);
		pModel->TransformPosition(BoneTransform[1], Position, p, true);		// b01
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[2], Position, p, true);		// Zx01
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[3], Position, p, true);		// Zx05
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[4], Position, p, true);		// Zx04
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[5], Position, p, true);		// Object04
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[6], Position, p, true);		// Object05
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[7], Position, p, true);		// Zx02
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[8], Position, p, true);		// Object01
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
		pModel->TransformPosition(BoneTransform[9], Position, p, true);		// Object03
		CreateSprite(BITMAP_LIGHT, p, 1.5f, Light, pObject);
	}break;
	case MODEL_SHIELD + 21:
	{
		vec3_t vPos, vLight;
		Vector(0.f, 0.f, 0.f, vPos);
		float fLumi = fabs(sinf(WorldTime * 0.001f)) + 0.1f;
		Vector(0.2f * fLumi, 0.2f * fLumi, 0.8f * fLumi, vLight);
		pModel->TransformByObjectBone(vPos, Object, 4);
		CreateSprite(BITMAP_LIGHT, vPos, 1.2f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 5);
		CreateSprite(BITMAP_LIGHT, vPos, 1.2f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 6);
		CreateSprite(BITMAP_LIGHT, vPos, 1.2f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 7);
		CreateSprite(BITMAP_LIGHT, vPos, 1.2f, vLight, Object);

		Vector(0.2f * fLumi, 0.6f * fLumi, 0.6f * fLumi, vLight);
		pModel->TransformByObjectBone(vPos, Object, 8);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 9);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 10);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 11);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 12);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 13);
		CreateSprite(BITMAP_LIGHT, vPos, 1.4f, vLight, Object);
	}
	break;
	case MODEL_STAFF + 33:
	{
		vec3_t vPos, vLight;

		pModel->TransformByObjectBone(vPos, Object, 8);
		Vector(0.9f, 0.1f, 0.4f, vLight);
		CreateSprite(BITMAP_LIGHT, vPos, 3.5f, vLight, Object);
		vLight[0] = 0.1f + 0.8f * absf(sinf(WorldTime * 0.0008f));
		vLight[1] = 0.1f * absf(sinf(WorldTime * 0.0008f));
		vLight[2] = 0.1f + 0.4f * absf(sinf(WorldTime * 0.0008f));
		CreateSprite(BITMAP_MAGIC, vPos, 0.3f, vLight, Object);
		CreateEffectSync(MODEL_MOONHARVEST_MOON, vPos, pObject->Angle, vLight, 2, NULL, -1, 0, 0, 0, 0.12f);
		Vector(0.8f, 0.8f, 0.2f, vLight);
		CreateSprite(BITMAP_SHINY + 1, vPos, 1.0f, vLight, Object);

		//작은 구슬
		for (int i = 1; i < 8; i++)
		{
			pModel->TransformByObjectBone(vPos, Object, i);
			Vector(0.8f, 0.1f, 0.4f, vLight);
			CreateSprite(BITMAP_LIGHT, vPos, 0.6f, vLight, Object);
			CreateSprite(BITMAP_MAGIC, vPos, 0.12f, vLight, Object);
			Vector(0.8f, 0.8f, 0.2f, vLight);
			CreateSprite(BITMAP_SHINY + 1, vPos, 0.3f, vLight, Object);
		}
	}
	break;
	case MODEL_STAFF + 34:
	{
		vec3_t vPos, vLight;

		//zx01
		pModel->TransformByObjectBone(vPos, Object, 1);
		Vector(0.4f, 0.2f, 1.0f, vLight);
		CreateSprite(BITMAP_SHINY + 6, vPos, 1.4f, vLight, Object);
		Vector(0.7f, 0.1f, 0.9f, vLight);
		CreateSprite(BITMAP_LIGHT, vPos, 1.5f, vLight, Object);
		Vector(0.0f, 0.0f, 1.0f, vLight);
		CreateSprite(BITMAP_SHINY + 2, vPos, 1.5f, vLight, Object);
		Vector(0.4f, 0.4f, 1.0f, vLight);
		CreateSprite(BITMAP_PIN_LIGHT, vPos, 0.7f, vLight, Object, -((int)(WorldTime * 0.04f) % 360));
		CreateSprite(BITMAP_PIN_LIGHT, vPos, 0.8f, vLight, Object, -((int)(WorldTime * 0.03f) % 360));

		float quarterAngle, theta, fSize;
		quarterAngle = Q_PI / 180.0f * (int(WorldTime * 0.02f) % 90);
		theta = absf(sinf(quarterAngle + Q_PI / 2));
		fSize = absf(sinf(quarterAngle)) * 0.5f;
		Vector(0.7f * theta, 0.1f * theta, 0.9f * theta, vLight);
		CreateSprite(BITMAP_MAGIC, vPos, fSize, vLight, Object);
		quarterAngle = Q_PI / 180.0f * (int(WorldTime * 0.05f) % 60 + 30);
		theta = absf(sinf(quarterAngle + Q_PI / 2));
		fSize = absf(sinf(quarterAngle)) * 0.5f;
		Vector(0.1f + 0.7f * theta, 0.1f * theta, 0.1f + 0.3f * theta, vLight);
		CreateSprite(BITMAP_MAGIC, vPos, fSize, vLight, Object);

		//zx02
		Vector(0.9f, 0.0f, 0.1f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 2);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.8f, vLight, Object);

		//zx03
		Vector(0.9f, 0.4f, 0.7f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 3);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.6f, vLight, Object);
	}
	break;
	case MODEL_SPEAR + 11:
	{
		vec3_t vPos, vLight;

		//zx01, zx02
		Vector(0.9f, 0.2f, 0.7f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 3);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.8f, vLight, Object);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.3f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 4);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.8f, vLight, Object);
		CreateSprite(BITMAP_SHINY + 6, vPos, 0.3f, vLight, Object);

		//bone02
		Vector(0.2f, 0.9f, 0.1f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 5);
		CreateSprite(BITMAP_LIGHT, vPos, 1.5f, vLight, Object);
	}
	break;
	case MODEL_MACE + 18:
	{
		vec3_t vPos, vLight;
		float fSize = 0.0f;

		//rx01
		pModel->TransformByObjectBone(vPos, Object, 5);
		Vector(0.5f, 0.8f, 0.9f, vLight);
		CreateSprite(BITMAP_SHINY + 1, vPos, 0.5f, vLight, Object);
		Vector(0.4f, 0.7f, 0.9f, vLight);
		CreateSprite(BITMAP_LIGHTMARKS, vPos, 1.0f, vLight, Object);

		Vector(0.4f, 0.6f, 0.9f, vLight);
		if (int(WorldTime) % 14 == 0)
			CreateParticle(BITMAP_SHINY + 6, vPos, pObject->Angle, vLight, 1);

		//rx02
		Vector(0.5f, 0.6f, 0.9f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 6);
		CreateSprite(BITMAP_SHINY + 1, vPos, 1.1f, vLight, Object);

		//zx 01, 02
		Vector(0.8f, 0.5f, 0.2f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 2);
		CreateSprite(BITMAP_SHINY + 1, vPos, 0.5f, vLight, Object);
		pModel->TransformByObjectBone(vPos, Object, 3);
		CreateSprite(BITMAP_SHINY + 1, vPos, 0.5f, vLight, Object);

		//zx03
		Vector(1.0f, 1.0f, 1.0f, vLight);
		pModel->TransformByObjectBone(vPos, Object, 4);
		fSize = 0.5f * absf((sinf(WorldTime * 0.0015f)));
		CreateSprite(BITMAP_FLARE, vPos, fSize, vLight, Object);
	}
	break;
	case MODEL_BOW + 24:
	{
		vec3_t vPos, vLight;
		float quarterAngle, theta, fSize;

		//zx02, 03
		Vector(0.2f, 0.8f, 0.5f, vLight);
		int iBoneNum[2] = { 7,11 };

		for (int i = 0; i < 2; i++)
		{
			pModel->TransformByObjectBone(vPos, Object, iBoneNum[i]);

			CreateSprite(BITMAP_LIGHT, vPos, 0.8f, vLight, Object);
			CreateSprite(BITMAP_LIGHT, vPos, 0.8f, vLight, Object);

			quarterAngle = Q_PI / 180.0f * (int(WorldTime * 0.02f) % 90);
			theta = absf(sinf(quarterAngle + Q_PI / 2));
			fSize = absf(sinf(quarterAngle)) * 0.3f;
			Vector(0.2f * theta, 0.8f * theta, 0.5f * theta, vLight);
			CreateSprite(BITMAP_SHOCK_WAVE, vPos, fSize, vLight, Object);

			quarterAngle = Q_PI / 180.0f * (int(WorldTime * 0.05f) % 60 + 30);
			theta = absf(sinf(quarterAngle + Q_PI / 2));
			fSize = absf(sinf(quarterAngle)) * 0.3f;
			Vector(0.2f * theta, 0.8f * theta, 0.5f * theta, vLight);
			CreateSprite(BITMAP_SHOCK_WAVE, vPos, fSize, vLight, Object);
		}

		//model_bow action, frame
		if (pObject->CurrentAction >= PLAYER_HIT_FIST && pObject->CurrentAction <= PLAYER_RIDE_SKILL)
		{
			Vector(0.2f, 0.8f, 0.5f, vLight);
			for (int i = 0; i < 8; i++)
			{
				pModel->TransformByObjectBone(vPos, Object, 22 + i);
				CreateSprite(BITMAP_FLARE, vPos, 0.4f, vLight, Object);
			}
		}
	}
	break;
	default:
	{
		GMItemEffect->RenderLinkEffect((Type - MODEL_ITEM), pModel, Object);
	}
	break;
	}
}

void RenderLight(OBJECT* o, int Texture, float Scale, int Bone, float x, float y, float z)
{
	BMD* b = gmClientModels->GetModel(o->Type);
	vec3_t p, Position;
	Vector(x, y, z, p);
	b->TransformPosition(o->BoneTransform[Bone], p, Position, true);
	float Luminosity = sinf(WorldTime * 0.002f) * 0.3f + 0.7f;
	vec3_t Light;
	Vector(Luminosity * 1.f, Luminosity * 0.6f, Luminosity * 0.4f, Light);
	CreateSprite(Texture, Position, Scale, Light, o);
}

void RenderEye(OBJECT* o, int Left, int Right, float fSize = 1.0f)
{
	BMD* b = gmClientModels->GetModel(o->Type);
	if (b != NULL)
	{
		vec3_t Light, vPos, Position;
		float Luminosity = sinf(WorldTime * 0.002f) * 0.3f + 0.8f;

		Vector(Luminosity, Luminosity, Luminosity, Light);
		Vector(5.f, 0.f, 0.f, vPos);
		b->TransformPosition(o->BoneTransform[Left], vPos, Position, true);

		CreateSprite(BITMAP_SHINY + 3, Position, fSize, Light, NULL);
		Vector(-5.f, 0.f, 0.f, vPos);
		b->TransformPosition(o->BoneTransform[Right], vPos, Position, true);

		CreateSprite(BITMAP_SHINY + 3, Position, fSize, Light, NULL);
		VectorCopy(Position, o->EyeLeft);
	}
}

void RenderCharacter(CHARACTER* c, OBJECT* o, int Select)
{
	if (g_isCharacterBuff(o, eBuff_CrywolfNPCHide))
	{
		return;
	}

	BMD* b = gmClientModels->GetModel(o->Type);

	if (!b || b->NumActions == 0)
	{
		return;
	}

	bool Translate = true;

	vec3_t p, Position, Light;

	Vector(0.f, 0.f, 0.f, p);
	Vector(1.f, 1.f, 1.f, Light);

	BYTE byRender = CHARACTER_NONE;

	switch (c->MonsterIndex)
	{
	case 89:
	case 95:
	case 112:
	case 118:
	case 124:
	case 130:
	case 143:
	{
		BOOL bRender = Calc_RenderObject(o, Translate, Select, 0);

		if (!c->Object.m_pCloth)
		{
			CPhysicsClothMesh* pCloth = new CPhysicsClothMesh[1];
			pCloth[0].Create(&(c->Object), 2, 18, PCT_HEAVY);
			pCloth[0].AddCollisionSphere(0.0f, 0.0f, 0.0f, 50.0f, 18);
			pCloth[0].AddCollisionSphere(0.0f, -20.0f, 0.0f, 30.0f, 18);
			c->Object.m_pCloth = (void*)pCloth;
			c->Object.m_byNumCloth = 1;
		}
		CPhysicsCloth* pCloth = (CPhysicsCloth*)c->Object.m_pCloth;

		if (pCloth)
		{
			if (!pCloth[0].Move2(0.005f, 5))
			{
				DeleteCloth(c, o);
			}
			else
			{
				pCloth[0].Render();
			}
		}
		if (bRender)
		{
			Draw_RenderObject(o, Translate, Select, 0);
		}
	}
	break;

	default:
		if (o->Type == MODEL_PLAYER)
		{
			byRender = CHARACTER_ANIMATION;
		}
		else
			byRender = CHARACTER_RENDER_OBJ;
		break;
	}

	if (o->Type == MODEL_PLAYER && (o->SubType == MODEL_XMAS_EVENT_CHA_SSANTA || o->SubType == MODEL_XMAS_EVENT_CHA_SNOWMAN || o->SubType == MODEL_XMAS_EVENT_CHA_DEER))
	{
		OBJECT* pOwner = o->Owner;

		if (pOwner->Live == false)
		{
			o->Live = false;
		}

		VectorCopy(pOwner->Position, o->Position);
		VectorCopy(pOwner->Angle, o->Angle);
		o->PriorAction = pOwner->PriorAction;
		o->PriorAnimationFrame = pOwner->PriorAnimationFrame;
		o->CurrentAction = pOwner->CurrentAction;
		o->AnimationFrame = pOwner->AnimationFrame;

		if (timeGetTime() - o->m_dwTime >= XMAS_EVENT_TIME)
		{
			DeleteCharacter(c, o);
		}
	}
	else if (o->Type == MODEL_PLAYER && o->SubType == MODEL_XMAS_EVENT_CHANGE_GIRL
		|| o->Type == MODEL_PLAYER && o->SubType == MODEL_PANDA
		|| o->Type == MODEL_PLAYER && o->SubType == MODEL_SKELETON_CHANGED
		)
	{
		if (o->m_iAnimation >= 1)
		{
			o->m_iAnimation = 0;
			SetPlayerStop(c);
		}
	}

	if (byRender == CHARACTER_ANIMATION)
		Calc_ObjectAnimation(o, Translate, Select);

	if (g_pOption->GetRenderEffect() == true)
	{
		if (o->Alpha >= 0.5f && c->HideShadow == false)
		{
			if (World != WD_10HEAVEN && (o->Type == MODEL_PLAYER) && (!(MODEL_HELPER + 2 <= c->Helper.Type && c->Helper.Type <= MODEL_HELPER + 3) || c->SafeZone)
				&& gMapManager->InHellas() == false)
			{
				if (gMapManager->InBloodCastle() && o->m_bActionStart && c->Dead > 0)
				{
					float height = RequestTerrainHeight(o->Position[0], o->Position[1]);
					if (height < o->Position[2])
					{
						o->Position[2] = height;
					}
				}
				o->EnableShadow = true;
				RenderPartObject(&c->Object, MODEL_SHADOW_BODY, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate);
				o->EnableShadow = false;
			}
		}
	}

	if (byRender == CHARACTER_RENDER_OBJ)
	{
		if (67 == c->MonsterIndex || 74 == c->MonsterIndex || 75 == c->MonsterIndex
			|| 135 == c->MonsterIndex || 136 == c->MonsterIndex || 137 == c->MonsterIndex
			|| 300 == c->MonsterIndex || 301 == c->MonsterIndex || 302 == c->MonsterIndex || 303 == c->MonsterIndex
			|| 314 == c->MonsterIndex || 315 == c->MonsterIndex || 316 == c->MonsterIndex || 317 == c->MonsterIndex || 318 == c->MonsterIndex || 319 == c->MonsterIndex
			)
		{
			RenderObject(o, Translate, Select, c->MonsterIndex);
		}
		else
		{
			if ((c->MonsterIndex == 360 && o->CurrentAction == MONSTER01_ATTACK2))
			{
				RenderObject_AfterImage(o, Translate, Select, 0);
			}
			else
			{
				RenderObject(o, Translate, Select, 0);
			}
		}
	}

	if ((o->Type != MODEL_PLAYER && o->Kind != KIND_TRAP &&
		c->MonsterIndex != 25 && c->MonsterIndex != 22 && c->MonsterIndex != 42 && c->MonsterIndex != 242 && c->MonsterIndex != 59 && c->MonsterIndex != 63
		&& c->MonsterIndex != 152
		) && World != WD_10HEAVEN
		)
	{
		if (o->Alpha >= 0.3f)
		{
			if (gMapManager->InBloodCastle() == true && o->m_bActionStart && c->Dead > 0)
			{
				float height = RequestTerrainHeight(o->Position[0], o->Position[1]);
				if (height < o->Position[2])
				{
					o->Position[2] = height;
				}
			}

			if (c->MonsterIndex == 232)
				o->HiddenMesh = 2;
			else if (c->MonsterIndex == 233)
				o->HiddenMesh = 2;

			if (o->Type != MODEL_MONSTER01 + 60 && o->Type != MODEL_MONSTER01 + 61
				&& !(o->Type >= MODEL_FACE && o->Type <= MODEL_FACE + 6))
			{
				o->EnableShadow = true;
				RenderPartObject(&c->Object, o->Type, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate);
				o->EnableShadow = false;
			}
			if (c->MonsterIndex == 232 || c->MonsterIndex == 233)
			{
				EnableAlphaBlend();

				vec3_t Position, Light;

				VectorCopy(o->Position, Position);
				Position[2] += 20.f;

				float Luminosity = sinf(WorldTime * 0.0015f) * 0.3f + 0.8f;

				Vector(Luminosity * 0.5f, Luminosity * 0.5f, Luminosity, Light);
				RenderTerrainAlphaBitmap(BITMAP_MAGIC + 1, o->Position[0], o->Position[1], 2.7f, 2.7f, Light, -o->Angle[2]);

				o->HiddenMesh = -1;
			}
		}
	}

	if (c->MonsterIndex == 38 || c->MonsterIndex == 43 || c->MonsterIndex == 52 || c->MonsterIndex == 59 || c->MonsterIndex == 67 || (78 <= c->MonsterIndex && c->MonsterIndex <= 83) || (c->MonsterIndex >= 493 && c->MonsterIndex <= 502))
	{
		vec3_t vBackupBodyLight;

		float Bright = 1.f;
		if (c->MonsterIndex == 59)
			Bright = 0.5f;
		if (c->MonsterIndex == 43 || (78 <= c->MonsterIndex && c->MonsterIndex <= 83) || (c->MonsterIndex >= 493 && c->MonsterIndex <= 502))
		{
			if (c->MonsterIndex >= 493)
			{
				VectorCopy(b->BodyLight, vBackupBodyLight);
				Vector(1.f, 0.6f, 0.3f, b->BodyLight);
			}

			if (c->MonsterIndex == 501)
			{
				VectorCopy(b->BodyLight, vBackupBodyLight);
				Vector(1.f, 0.0f, 0.0f, b->BodyLight);

				float	fEffectScale = o->Scale * 1.6f;
				vec3_t	v3EffectLightColor, v3EffectPosition;

				Vector(1.0f, 0.6f, 0.1f, v3EffectLightColor);

				b->TransformPosition(o->BoneTransform[0], p, v3EffectPosition, true);
				CreateSprite(BITMAP_LIGHTMARKS, v3EffectPosition, 2.5f, v3EffectLightColor, o);

				b->TransformPosition(o->BoneTransform[4], p, v3EffectPosition, true);
				CreateSprite(BITMAP_LIGHTMARKS, v3EffectPosition, 2.5f, v3EffectLightColor, o);

				b->TransformPosition(o->BoneTransform[57], p, v3EffectPosition, true);
				CreateSprite(BITMAP_LIGHTMARKS, v3EffectPosition, 2.5f, v3EffectLightColor, o);

				b->TransformPosition(o->BoneTransform[60], p, v3EffectPosition, true);
				CreateSprite(BITMAP_LIGHTMARKS, v3EffectPosition, 2.5f, v3EffectLightColor, o);

				b->TransformPosition(o->BoneTransform[87], p, v3EffectPosition, true);
				CreateSprite(BITMAP_LIGHTMARKS, v3EffectPosition, 2.5f, v3EffectLightColor, o);

				Vector(1.0f, 0.8f, 0.1f, v3EffectLightColor);

				b->TransformPosition(o->BoneTransform[57], p, v3EffectPosition, true);
				CreateEffectSync(MODEL_EFFECT_FIRE_HIK3_MONO, v3EffectPosition, o->Angle, v3EffectLightColor, 1, NULL, -1, 0, 0, 0, fEffectScale);

				b->TransformPosition(o->BoneTransform[60], p, v3EffectPosition, true);
				CreateEffectSync(MODEL_EFFECT_FIRE_HIK3_MONO, v3EffectPosition, o->Angle, v3EffectLightColor, 1, NULL, -1, 0, 0, 0, fEffectScale);

				b->TransformPosition(o->BoneTransform[66], p, v3EffectPosition, true);
				CreateEffectSync(MODEL_EFFECT_FIRE_HIK3_MONO, v3EffectPosition, o->Angle, v3EffectLightColor, 1, NULL, -1, 0, 0, 0, fEffectScale);

				b->TransformPosition(o->BoneTransform[78], p, v3EffectPosition, true);
				CreateEffectSync(MODEL_EFFECT_FIRE_HIK3_MONO, v3EffectPosition, o->Angle, v3EffectLightColor, 1, NULL, -1, 0, 0, 0, fEffectScale);

				b->TransformPosition(o->BoneTransform[91], p, v3EffectPosition, true);
				CreateEffectSync(MODEL_EFFECT_FIRE_HIK3_MONO, v3EffectPosition, o->Angle, v3EffectLightColor, 1, NULL, -1, 0, 0, 0, fEffectScale);
			}
			RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_METAL | RENDER_BRIGHT, Bright);
		}

		if (c->MonsterIndex == 67)
		{
			RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_CHROME | RENDER_BRIGHT | RENDER_EXTRA, Bright);
		}
		else
		{
			RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_CHROME | RENDER_BRIGHT, Bright);
		}

		if (c->MonsterIndex >= 493 && c->MonsterIndex <= 502)
		{
			VectorCopy(vBackupBodyLight, b->BodyLight);
		}
	}
	else if (c->MonsterIndex == 69)
	{
		float Luminosity = (float)(rand() % 30 + 70) * 0.01f;
		Vector(Luminosity * 0.8f, Luminosity * 0.9f, Luminosity * 1.f, Light);

		for (int i = 0; i < 9; ++i)
		{
			b->TransformPosition(o->BoneTransform[g_chStar[i]], p, Position, true);
			CreateSprite(BITMAP_LIGHT, Position, 0.6f, Light, o);
		}

		Vector(Luminosity * 0.6f, Luminosity * 0.7f, Luminosity * 0.8f, Light);

		for (int i = 0; i < 3; i++)
		{
			Vector((float)(rand() % 20 - 10), (float)(rand() % 20 - 10), (float)(rand() % 20 - 10), p);
			b->TransformPosition(o->BoneTransform[rand() % b->NumBones], p, Position, true);
			CreateParticleSync(BITMAP_SPARK + 1, Position, o->Angle, Light, 3);
		}
	}
	else if (c->MonsterIndex == 70)
	{
		b->TransformPosition(o->BoneTransform[20], p, Position, true);
		CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, o);
	}
	else if (c->MonsterIndex == 71 || c->MonsterIndex == 74 || c->MonsterIndex == 301)
	{
		if (!c->Object.m_pCloth)
		{
			int iTex = (c->MonsterIndex == 71) ? BITMAP_ROBE + 3 : BITMAP_ROBE + 5;
			CPhysicsCloth* pCloth = new CPhysicsCloth[1];
			pCloth[0].Create(&(c->Object), 19, 0.0f, 10.0f, 0.0f, 5, 15, 30.0f, 300.0f, iTex, iTex, PCT_RUBBER | PCT_MASK_ALPHA);
			c->Object.m_pCloth = (void*)pCloth;
			c->Object.m_byNumCloth = 1;
		}
		CPhysicsCloth* pCloth = (CPhysicsCloth*)c->Object.m_pCloth;
		if (!pCloth[0].Move2(0.005f, 5))
		{
			DeleteCloth(c, o);
		}
		else
		{
			pCloth[0].Render();
		}
	}
	else if (c->MonsterIndex == 73 || c->MonsterIndex == 75)
	{
		vec3_t pos1, pos2;
		switch (c->MonsterIndex)
		{
		case 73:
			Vector(0.1f, 0.1f, 1.f, Light);

			for (int i = 13; i < 27; ++i)
			{
				b->TransformPosition(o->BoneTransform[i], p, Position, true);
				CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, o);

				VectorCopy(Position, pos2);
				if (i >= 14 && i <= 16 || i == 23)
				{
					CreateJoint(BITMAP_JOINT_THUNDER, pos1, pos2, o->Angle, 7, NULL, 20.f);
				}
				VectorCopy(Position, pos1);
			}

			for (int i = 52; i < 59; ++i)
			{
				b->TransformPosition(o->BoneTransform[i], p, Position, true);
				CreateSprite(BITMAP_LIGHT, Position, 0.8f, Light, o);
			}
			break;
		case 75:
			Vector(1.f, 1.f, 1.0f, Light);

			for (int i = 18; i < 19; ++i)
			{
				Vector(0.f, 0.f, 0.f, p);
				b->TransformPosition(o->BoneTransform[i], p, Position, true);
				CreateParticleSync(BITMAP_FIRE, Position, o->Angle, Light, 0, 0.3f);
			}
			break;
		}

		int RenderType = (c->MonsterIndex == 73) ? 0 : RENDER_EXTRA;
		RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_CHROME | RENDER_BRIGHT | RenderType, 1.f);

		if (c->MonsterIndex == 73)
		{
			RenderPartObjectBodyColor2(b, o, o->Type, o->Alpha, RENDER_CHROME2 | RENDER_LIGHTMAP | RENDER_BRIGHT, 1.f);
		}
	}
	else if (c->MonsterIndex == 77)
	{
		float fSin = 0.5f * (1.0f + sinf((float)((int)WorldTime % 10000) * 0.001f));
		RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_CHROME | RENDER_BRIGHT, 0.3f + fSin * 0.7f);
		fSin = 0.3f * (1.0f - fSin) + 0.3f;
		o->BlendMeshLight = fSin;
		o->BlendMesh = 0;
		RenderObject(o, Translate, 2, 0);
		RenderObject(o, Translate, 3, 0);
		o->BlendMesh = -1;
		memcpy(g_fBoneSave[2], o->BoneTransform[24], 3 * 4 * sizeof(float));
		o->Type++;
		RenderObject(o, Translate, Select, 0);
		memcpy(g_fBoneSave[0], o->BoneTransform[23], 3 * 4 * sizeof(float));
		memcpy(g_fBoneSave[1], o->BoneTransform[14], 3 * 4 * sizeof(float));

		if (!c->Object.m_pCloth)
		{
			CPhysicsCloth* pCloth = new CPhysicsCloth[1];
			pCloth[0].Create(o, 10, 0.0f, -10.0f, 0.0f, 5, 12, 15.0f, 240.0f, BITMAP_PHO_R_HAIR, BITMAP_PHO_R_HAIR, PCT_RUBBER | PCT_MASK_ALPHA);
			pCloth[0].AddCollisionSphere(0.0f, 0.0f, 40.0f, 30.0f, 10);
			o->m_pCloth = (void*)pCloth;
			o->m_byNumCloth = 1;
		}
		CPhysicsCloth* pCloth = (CPhysicsCloth*)c->Object.m_pCloth;
		if (!pCloth[0].Move2(0.005f, 5))
		{
			DeleteCloth(c, o);
		}
		else
		{
			pCloth[0].Render();
		}
		o->Type--;
	}

	if (c->MonsterIndex == 53 || c->MonsterIndex == 54)
	{
		RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_METAL | RENDER_BRIGHT, 1.f, BITMAP_SHINY + 1);
	}

	if (c->MonsterIndex == 42)
	{
		PART_t* w = &c->Wing;
		w->Type = MODEL_BOSS_HEAD;
		w->LinkBone = 9;
		w->CurrentAction = 1;
		w->PriorAction = 1;
		w->PlaySpeed = 0.2f;
		RenderLinkObject(0.f, 0.f, -40.f, c, w, w->Type, 0, 0, false, Translate);

		w->Type = MODEL_PRINCESS;
		w->LinkBone = 61;
		float TempScale = o->Scale;
		vec3_t TempLight;
		VectorCopy(c->Light, TempLight);
		Vector(1.f, 1.f, 1.f, c->Light);
		o->Scale = 0.9f;
		RenderLinkObject(0.f, -40.f, 45.f, c, w, w->Type, 0, 0, false, Translate);
		VectorCopy(TempLight, c->Light);
		o->Scale = TempScale;
	}
	else if (c->MonsterIndex >= 132 && c->MonsterIndex <= 134)
	{
		PART_t* w = &c->Wing;
		w->LinkBone = 1;
		w->CurrentAction = 1;
		w->PriorAction = 1;
		w->PlaySpeed = 0.2f;
		float TempScale = o->Scale;
		o->Scale = 0.7f;

		if (c->MonsterIndex == 132)
			w->Type = MODEL_STAFF + 10;
		if (c->MonsterIndex == 133)
			w->Type = MODEL_SWORD + 19;
		if (c->MonsterIndex == 134)
		{
			w->Type = MODEL_BOW + 18;
			o->Scale = 0.9f;
		}

		RenderLinkObject(0.f, 0.f, 0.f, c, w, w->Type, 0, 0, true, true);
		o->Scale = TempScale;
	}
	else if (c->MonsterIndex == 135)
	{
		RenderPartObjectBodyColor(b, o, o->Type, o->Alpha, RENDER_BRIGHT | RENDER_EXTRA, 1.0f);
	}

	if (o->Type == MODEL_MONSTER01 + 127)
	{
		vec3_t vLight;
		vec3_t vPos, vRelatedPos;
		Vector(0.3f, 0.3f, 0.0f, vLight);
		Vector(0.f, 0.f, 0.f, vRelatedPos);

		if (o->CurrentAction == MONSTER01_WALK)
		{
			o->m_iAnimation++;
			if (o->m_iAnimation % 20 == 0)
				PlayBuffer(SOUND_MOONRABBIT_WALK);
		}

		if (o->CurrentAction == MONSTER01_SHOCK)
		{
			if (o->AnimationFrame > 2.f && o->AnimationFrame <= 3.f)
				PlayBuffer(SOUND_MOONRABBIT_DAMAGE);
		}

		if (o->CurrentAction == MONSTER01_DIE)
		{
			if (o->AnimationFrame > 1.f && o->AnimationFrame <= 2.f)
				PlayBuffer(SOUND_MOONRABBIT_DEAD);

			if (o->AnimationFrame > 9.f)
			{
				if (o->SubType != -1)
				{
					Vector(0.5f, 0.5f, 0.0f, vLight);
					BoneManager::GetBonePosition(o, "Rabbit_1", vPos);

					Vector(0.7f, 1.0f, 0.6f, vLight);
					vec3_t vMoonPos;
					VectorCopy(vPos, vMoonPos);
					vMoonPos[2] += 28.f;
					CreateEffectSync(MODEL_MOONHARVEST_MOON, vMoonPos, o->Angle, vLight, 0, NULL, -1, 0, 0, 0, 0.5f);

					Vector(0.4f, 0.4f, 0.8f, vLight);
					CreateParticleSync(BITMAP_EXPLOTION_MONO, vPos, o->Angle, vLight, 10, 1.0f);

					Vector(1.0f, 1.0f, 1.0f, vLight);
					for (int i = 0; i < 200; i++)
					{
						CreateParticleSync(BITMAP_SPARK + 1, vPos, o->Angle, vLight, 21);
					}

					for (int i = 0; i < 150; i++)
					{
						CreateParticleSync(BITMAP_SPARK + 1, vPos, o->Angle, vLight, 22);
					}
					Vector(1.0f, 1.0f, 1.0f, vLight);
					switch (o->SubType)
					{
					case 0:
					{
						for (int i = 0; i < 10; i++)
						{
							CreateEffectSync(MODEL_MOONHARVEST_GAM, vPos, o->Angle, vLight);
						}
					}break;
					case 1:
					{
						for (int i = 0; i < 5; i++)
						{
							CreateEffectSync(MODEL_MOONHARVEST_SONGPUEN1, vPos, o->Angle, vLight);
							CreateEffectSync(MODEL_MOONHARVEST_SONGPUEN2, vPos, o->Angle, vLight);
						}
					}
					break;
					case 2:
					{
						for (int i = 0; i < 10; i++)
						{
							CreateEffectSync(MODEL_NEWYEARSDAY_EVENT_BEKSULKI, vPos, o->Angle, vLight);
						}
					}
					break;
					}

					o->SubType = -1;

					if (o->AnimationFrame <= 10.f)
					{
						// flare
						Vector(1.0f, 0.0f, 0.0f, vLight);
						CreateSprite(BITMAP_LIGHT, vPos, 8.0f, vLight, o);
					}
				}
			}
		}
		else
		{
			BoneManager::GetBonePosition(o, "Rabbit_1", vPos);		// Bip01 Spine 
			Vector(0.4f, 0.4f, 0.9f, vLight);
			CreateSprite(BITMAP_LIGHT, vPos, 3.0f, vLight, o);	// flare01.jpg 

			BoneManager::GetBonePosition(o, "Rabbit_2", vPos);		// Bip01 Head
			Vector(0.4f, 0.4f, 0.9f, vLight);
			CreateSprite(BITMAP_LIGHT, vPos, 2.0f, vLight, o);	// flare01.jpg 

			BoneManager::GetBonePosition(o, "Rabbit_4", vPos);		// Bip01 Pelvis
			Vector(0.4f, 0.4f, 0.9f, vLight);
			CreateSprite(BITMAP_LIGHT, vPos, 1.0f, vLight, o);	// flare01.jpg 
		}
	}

	if (o->Type == MODEL_NPC_CHERRYBLOSSOM)
	{
		vec3_t vRelativePos, vtaWorldPos, vLight, vLight1, vLight2;
		Vector(0.f, 0.f, 0.f, vRelativePos);
		Vector(1.f, 0.6f, 0.8f, vLight);
		Vector(0.3f, 0.3f, 0.3f, vLight1);
		Vector(1.f, 1.f, 1.f, vLight2);

		float Scale = absf(sinf(WorldTime * 0.002f)) * 0.2f;
		float Luminosity = absf(sinf(WorldTime * 0.002f)) * 0.4f;
		Vector(0.5f + Luminosity, 0.0f + Luminosity, 0.0f + Luminosity, Light);

		if (rand() % 2 == 0)
		{
			VectorCopy(o->Position, b->BodyOrigin);
			b->Animation(BoneTransform, o->AnimationFrame, o->PriorAnimationFrame, o->PriorAction, o->Angle, o->HeadAngle);
			b->TransformPosition(BoneTransform[43], vRelativePos, vtaWorldPos, false);
			vtaWorldPos[2] += 20.f;

			CreateParticleSync(BITMAP_CHERRYBLOSSOM_EVENT_PETAL, vtaWorldPos, o->Angle, rand() % 3 == 0 ? vLight : vLight1, 1, 0.3f);
		}

		VectorCopy(o->Position, b->BodyOrigin);
		b->Animation(BoneTransform, o->AnimationFrame, o->PriorAnimationFrame, o->PriorAction, o->Angle, o->HeadAngle);
		b->TransformPosition(BoneTransform[43], vRelativePos, vtaWorldPos, false);
		vtaWorldPos[2] += 20.f;

		CreateParticleSync(BITMAP_SPARK + 1, vtaWorldPos, o->Angle, rand() % 3 == 0 ? vLight2 : vLight1, 25, Scale + 0.2f);
		CreateParticleSync(BITMAP_SPARK + 1, vtaWorldPos, o->Angle, rand() % 2 == 0 ? vLight2 : vLight1, 25, Scale + 0.3f);

		Vector(0.7f, 0.5f, 0.2f, vLight);
		CreateSprite(BITMAP_LIGHT, vtaWorldPos, 2.f, vLight, o, 0.f);

		Vector(0.7f, 0.2f, 0.6f, vLight);
		VectorCopy(o->Position, b->BodyOrigin);
		b->Animation(BoneTransform, o->AnimationFrame, o->PriorAnimationFrame, o->PriorAction, o->Angle, o->HeadAngle);
		b->TransformPosition(BoneTransform[3], vRelativePos, vtaWorldPos, false);
		CreateSprite(BITMAP_LIGHT, vtaWorldPos, 5.f, vLight, o, 0.f);

		int iRedFlarePos[] = { 53, 56, 59, 62 };
		for (int i = 0; i < 4; ++i) {
			Vector(0.9f, 0.4f, 0.8f, vLight);
			VectorCopy(o->Position, b->BodyOrigin);
			b->Animation(BoneTransform, o->AnimationFrame, o->PriorAnimationFrame, o->PriorAction, o->Angle, o->HeadAngle);
			b->TransformPosition(BoneTransform[iRedFlarePos[i]], vRelativePos, vtaWorldPos, false);

			CreateSprite(BITMAP_LIGHT, vtaWorldPos, 1.5f, vLight, o, 0.f);

			if (rand() % 3 == 0) {

				float randpos = (float)(rand() % 30 + 5);

				if (rand() % 2 == 0) {
					vtaWorldPos[0] += randpos;
				}
				else {
					vtaWorldPos[0] -= randpos;
				}

				Vector(1.f, 0.8f, 0.4f, vLight);
				CreateParticleSync(BITMAP_SPARK + 1, vtaWorldPos, o->Angle, vLight, 15, Scale + 0.4f);
			}
		}
	}

	bool fullset = false;
	if (o->Type == MODEL_PLAYER)
	{
		if (c->HideShadow == false)
		{
			fullset = CheckFullSet(c);
		}
	}

	bool bCloak = false;

	if (/*(gCharacterManager.GetCharacterClass(c->Class) == Magic_Gladiator || gCharacterManager.GetCharacterClass(c->Class) == Soul_Master || c->GetBaseClass() == Dark_Lord || c->GetBaseClass() == Rage_Fighter) &&*/ o->Type == MODEL_PLAYER)
	{
		if (c->Change == false || (c->Change == true && c->Object.Type == MODEL_PLAYER))
		{
			bCloak = true;
		}
	}

	if (c->MonsterIndex == 55 || c->MonsterIndex == 361)
	{
		bCloak = true;
	}

	if (c->MonsterIndex >= 529 && c->MonsterIndex <= 539)
	{
		bCloak = false;
	}

	if (gMapManager->InChaosCastle() == true)
	{
		bCloak = false;
	}

	vec3_t CloakLight;
	Vector(1.f, 1.f, 1.f, CloakLight);
	if (c->GuildMarkIndex != -1)
	{
		if (EnableSoccer)
		{
			if (strcmp(GuildMark[Hero->GuildMarkIndex].GuildName, GuildMark[c->GuildMarkIndex].GuildName) == NULL)
			{
				bCloak = true;
				if (HeroSoccerTeam == 0)
				{
					Vector(1.f, 0.2f, 0.f, CloakLight);
				}
				else
				{
					Vector(0.f, 0.2f, 1.f, CloakLight);
				}
			}
			if (strcmp(GuildWarName, GuildMark[c->GuildMarkIndex].GuildName) == NULL)
			{
				bCloak = true;
				if (HeroSoccerTeam == 0)
				{
					Vector(0.f, 0.2f, 1.f, CloakLight);
				}
				else
				{
					Vector(1.f, 0.2f, 0.f, CloakLight);
				}
			}
		}
		if (SoccerObserver)
		{
			if (strcmp(SoccerTeamName[0], GuildMark[c->GuildMarkIndex].GuildName) == NULL)
			{
				bCloak = true;
				Vector(1.f, 0.2f, 0.f, CloakLight);
			}
			if (strcmp(SoccerTeamName[1], GuildMark[c->GuildMarkIndex].GuildName) == NULL)
			{
				bCloak = true;
				Vector(0.f, 0.2f, 1.f, CloakLight);
			}
		}
	}

	if (g_DuelMgr.IsDuelEnabled())
	{
		if (g_DuelMgr.IsDuelPlayer(c, DUEL_ENEMY, FALSE))
		{
			bCloak = true;
			Vector(1.f, 0.2f, 0.f, CloakLight);
		}
		else if (g_DuelMgr.IsDuelPlayer(c, DUEL_HERO, FALSE))
		{
			bCloak = true;
			Vector(0.f, 0.2f, 1.f, CloakLight);
		}
	}


	if (SceneFlag == CHARACTER_SCENE)
	{
		Vector(0.4f, 0.4f, 0.4f, Light);
	}
	else
	{
		RequestTerrainLight(o->Position[0], o->Position[1], Light);
	}

	VectorAdd(Light, o->Light, c->Light);
	if (o->Type == MODEL_MONSTER01 + 55)
	{
		Vector(.6f, .6f, .6f, c->Light);
	}

	else if (c->MonsterIndex >= 529 && c->MonsterIndex <= 539)
	{
		c->HideShadow = true;

		if (World == WD_65DOPPLEGANGER1)
		{
			Vector(0.5f, 0.7f, 1.0f, c->Light);
			c->Object.Alpha = 0.7f;
		}
		else
		{
			Vector(1.0f, 0.3f, 0.1f, c->Light);
		}

		if (World != WD_65DOPPLEGANGER1)
		{
			if (o->CurrentAction != PLAYER_DIE1)
			{
				c->Object.Alpha = 1.0f;
			}
			else
			{
				c->Object.Alpha -= 0.07f;
				float fAlpha = c->Object.Alpha;

				if (fAlpha < 0)
					fAlpha = 0;

				Vector(1.0f * fAlpha, 0.3f * fAlpha, 0.1f * fAlpha, c->Light);

				vec3_t vPos;
				int iNumBones = b->NumBones;

				b->TransformByObjectBone(vPos, o, rand() % iNumBones);
				CreateParticleSync(BITMAP_TWINTAIL_WATER, vPos, o->Angle, c->Light, 2, 0.5f);
			}
		}
		c->Object.BlendMesh = -1;

		if (g_isCharacterBuff((&c->Object), eBuff_Doppelganger_Ascension))
		{
			o->Position[2] += (2.0f);
		}
	}

	if (c == Hero)
	{
		vec3_t AbilityLight = { 1.f, 1.f, 1.f };
		if (CharacterAttribute->Ability & ABILITY_FAST_ATTACK_SPEED)
		{
			AbilityLight[0] *= 0.9f; AbilityLight[1] *= 0.5f; AbilityLight[2] *= 0.5f;
		}
		if (CharacterAttribute->Ability & ABILITY_PLUS_DAMAGE)
		{
			AbilityLight[0] *= 0.5f; AbilityLight[1] *= 0.9f; AbilityLight[2] *= 0.5f;
		}
		if (CharacterAttribute->Ability & ABILITY_FAST_ATTACK_RING)
		{
			AbilityLight[0] *= 0.9f; AbilityLight[1] *= 0.5f; AbilityLight[2] *= 0.5f;
		}
		if (CharacterAttribute->Ability & ABILITY_FAST_ATTACK_SPEED2)
		{
			AbilityLight[0] *= 0.9f; AbilityLight[1] *= 0.5f; AbilityLight[2] *= 0.5f;
		}
		if (SceneFlag == CHARACTER_SCENE)
		{
			Vector(0.5f, 0.5f, 0.5f, Light);
			VectorAdd(Light, o->Light, c->Light);
		}
		else
			VectorCopy(AbilityLight, c->Light);
	}

	if (o->Kind == KIND_NPC && World == WD_0LORENCIA && o->Type == MODEL_PLAYER && (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3))
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, c->Level << 3, 0, 0, false, false, Translate, Select);
	}
#ifdef PBG_MOD_RAGEFIGHTERSOUND
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && c->GetBaseClass() == CLASS_RAGEFIGHTER
		&& (o->SubType == MODEL_SKELETON_PCBANG || o->SubType == MODEL_HALLOWEEN
			|| o->SubType == MODEL_PANDA || o->SubType == MODEL_SKELETON_CHANGED
			|| o->SubType == MODEL_XMAS_EVENT_CHANGE_GIRL || o->SubType == MODEL_SKELETON1))
	{
		PART_t* p = &c->BodyPart[o->SubType];
		if (o->CurrentAction == PLAYER_SKILL_DRAGONKICK)
		{
			p->Type = o->SubType;
			RenderCharacter_AfterImage(c, p, Translate, Select, 2.5f, 1.0f);
		}
		else if (o->CurrentAction == PLAYER_SKILL_DARKSIDE_READY || o->CurrentAction == PLAYER_SKILL_DARKSIDE_ATTACK)
		{
			OBJECT* pObj = &c->Object;
			if (pObj->m_sTargetIndex < 0 || c->JumpTime > 0)
			{
				RenderPartObject(&c->Object, o->SubType, p, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
			}
			else
			{
				p->Type = o->SubType;
				g_CMonkSystem.DarksideRendering(c, p, Translate, Select);
			}
		}
		else
		{
			RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
		}

		if (o->SubType == MODEL_XMAS_EVENT_CHANGE_GIRL)
		{
			if (c->m_pTempParts == NULL)
			{
				c->m_pTempParts = (CSIPartsMDL*)new CSParts(MODEL_XMAS_EVENT_EARRING, 20);
				CSIPartsMDL* pParts = (CSIPartsMDL*)c->m_pTempParts;
				OBJECT* pObj = pParts->GetObject();
				pObj->Velocity = 0.25f;
				pObj->Owner = &c->Object;
			}
			else
			{
				RenderParts(c);
			}
		}
	}
#endif //PBG_MOD_RAGEFIGHTERSOUND
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_SKELETON_PCBANG)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_HALLOWEEN)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_PANDA)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_SKELETON_CHANGED)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_XMAS_EVENT_CHANGE_GIRL)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);

		if (c->m_pTempParts == NULL)
		{
			c->m_pTempParts = (CSIPartsMDL*)new CSParts(MODEL_XMAS_EVENT_EARRING, 20);
			CSIPartsMDL* pParts = (CSIPartsMDL*)c->m_pTempParts;
			OBJECT* pObj = pParts->GetObject();
			pObj->Velocity = 0.25f;
			pObj->Owner = &c->Object;
		}
		else
		{

			CSIPartsMDL* pParts = (CSIPartsMDL*)c->m_pTempParts;
			OBJECT* pObj = pParts->GetObject();

			RenderParts(c);
		}
	}
	else if (o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && o->SubType == MODEL_GM_CHARACTER)
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);

		if (!g_isCharacterBuff(o, eBuff_Cloaking))
		{
			vec3_t vLight, vPos;
			Vector(0.4f, 0.6f, 0.8f, vLight);
			VectorCopy(o->Position, vPos);
			vPos[2] += 100.f;
			CreateSprite(BITMAP_LIGHT, vPos, 6.0f, vLight, o, 0.5f);

			float fLumi;
			fLumi = sinf(WorldTime * 0.05f) * 0.4f + 0.9f;
			Vector(fLumi * 0.3f, fLumi * 0.5f, fLumi * 0.8f, vLight);
			CreateSprite(BITMAP_LIGHT, vPos, 2.0f, vLight, o);

			Vector(0.3f, 0.2f, 1.f, vLight);
			RenderAurora(BITMAP_MAGIC + 1, RENDER_BRIGHT, o->Position[0], o->Position[1], 2.5f, 2.5f, vLight);

			Vector(1.0f, 1.0f, 1.f, vLight);
			fLumi = sinf(WorldTime * 0.0015f) * 0.3f + 0.5f;
			EnableAlphaBlend();
			Vector(fLumi * vLight[0], fLumi * vLight[1], fLumi * vLight[2], vLight);
			RenderTerrainAlphaBitmap(BITMAP_GM_AURORA, o->Position[0], o->Position[1], 1.5f, 1.5f, vLight, WorldTime * 0.01f);
			RenderTerrainAlphaBitmap(BITMAP_GM_AURORA, o->Position[0], o->Position[1], 1.f, 1.f, vLight, -WorldTime * 0.01f);
			//RenderAurora ( BITMAP_GM_AURORA, RENDER_BRIGHT, o->Position[0], o->Position[1], 1.5f, 1.5f, vLight );

			if (c->Object.m_pCloth == NULL)
			{
				int iNumCloth = 2;

				CPhysicsCloth* pCloth = new CPhysicsCloth[iNumCloth];

				pCloth[0].Create(o, 20, 0.0f, 5.0f, 10.0f, 6, 5, 30.0f, 70.0f, BITMAP_GM_HAIR_1, BITMAP_GM_HAIR_1, PCT_COTTON | PCT_SHORT_SHOULDER | PCT_MASK_ALPHA);
				pCloth[0].SetWindMinMax(10, 20);
				pCloth[0].AddCollisionSphere(-10.f, 20.0f, 20.0f, 27.0f, 17);
				pCloth[0].AddCollisionSphere(10.f, 20.0f, 20.0f, 27.0f, 17);

				pCloth[1].Create(o, 20, 0.0f, 5.0f, 25.0f, 4, 4, 30.0f, 40.0f, BITMAP_GM_HAIR_3, BITMAP_GM_HAIR_3, PCT_COTTON | PCT_SHORT_SHOULDER | PCT_MASK_ALPHA);
				pCloth[1].SetWindMinMax(8, 15);
				pCloth[1].AddCollisionSphere(-10.f, 20.0f, 20.0f, 27.0f, 17);
				pCloth[1].AddCollisionSphere(10.f, 20.0f, 20.0f, 27.0f, 17);

				o->m_pCloth = (void*)pCloth;
				o->m_byNumCloth = iNumCloth;
			}

			CPhysicsCloth* pCloth = (CPhysicsCloth*)c->Object.m_pCloth;

			if (g_isCharacterBuff(o, eBuff_Cloaking))
			{
				for (int i = 0; i < o->m_byNumCloth; ++i)
				{
					if (pCloth[i].Move2(0.005f, 5) == false)
					{
						DeleteCloth(c, o);
					}
					else
					{
						vec3_t vLight;
						Vector(1.f, 1.f, 1.f, vLight);
						pCloth[i].Render(&vLight);
					}
				}
			}
			DisableAlphaBlend();
		}
	}
	else if (o->Type == MODEL_PLAYER
		&& (o->SubType == MODEL_XMAS_EVENT_CHA_SSANTA
			|| o->SubType == MODEL_XMAS_EVENT_CHA_SNOWMAN
			|| o->SubType == MODEL_XMAS_EVENT_CHA_DEER))
	{
		vec3_t vOriginPos;
		VectorCopy(o->Position, vOriginPos);
		o->Position[0] += 50.f;
		o->Position[1] += 50.f;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);

		VectorCopy(vOriginPos, o->Position);
		o->Position[0] -= 50.f;
		o->Position[1] -= 50.f;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);

		VectorCopy(vOriginPos, o->Position);
		o->Position[0] += 50.f;
		o->Position[1] -= 50.f;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (o->Type == MODEL_PLAYER && (o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3))
	{
		RenderPartObject(&c->Object, o->SubType, NULL, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
	}
	else if (!c->Change)
	{
#ifndef GUILD_WAR_EVENT
		if ((o->Kind == KIND_PLAYER && gMapManager->InChaosCastle() == true))
		{
			int RenderType = RENDER_TEXTURE;
			PART_t* p = &c->BodyPart[BODYPART_HEAD];
			if (c == Hero)
			{
				RenderType |= RENDER_CHROME;
			}
#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
			if ((o->CurrentAction == PLAYER_SKILL_DRAGONKICK) && (c->GetBaseClass() == CLASS_RAGEFIGHTER))
			{
				p->Type = MODEL_ANGEL;
				RenderCharacter_AfterImage(c, p, Translate, Select, 2.5f, 1.0f);
			}
			else if ((c->GetBaseClass() == CLASS_RAGEFIGHTER))
			{
				OBJECT* pObj = &c->Object;
				if (pObj->m_sTargetIndex < 0 || c->JumpTime > 0)
				{
					RenderPartObject(&c->Object, MODEL_ANGEL, p, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select, RenderType);
				}
				else
				{
					p->Type = MODEL_ANGEL;
					g_CMonkSystem.DarksideRendering(c, p, Translate, Select);
				}
			}
			else
#endif //PBG_ADD_NEWCHAR_MONK_SKILL
				RenderPartObject(&c->Object, MODEL_ANGEL, p, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select, RenderType);
		}
		else
#endif// GUILD_WAR_EVENT
		{
			for (int i = MAX_BODYPART - 1; i >= 0; i--)
			{
				PART_t* p = &c->BodyPart[i];
				if (p->Type != -1)
				{
					int Type = p->Type;

					if (c->GetBaseClass() == Summoner)
					{
						//int nItemType = (Type - MODEL_ITEM) / MAX_ITEM_INDEX;
						//int nItemSubType = (Type - MODEL_ITEM) % MAX_ITEM_INDEX;
						//
						//if (nItemType >= 7 && nItemType <= 11 && (nItemSubType == 10 || nItemSubType == 11))
						//{
						//	Type = c->GetSkinModelIndex() + MODEL_BODY_HELM;//MODEL_HELM2 + (nItemType - 7) * MODEL_ITEM_COMMON_NUM + nItemSubType - 10;
						//}
					}
					else if (c->GetBaseClass() == Rage_Fighter)
					{
						Type = g_CMonkSystem.ModifyTypeCommonItemMonk(Type);
					}

					BMD* BodyModel = gmClientModels->GetModel(Type);

					if (BodyModel == NULL)
						return;

					BodyModel->Skin = c->GetSkinModelIndex();

					if (c->GetBaseClass() == Dark_Lord && i == BODYPART_HELM)
					{
						o->BlendMeshLight = sinf(WorldTime * 0.001f) * 0.1f + 0.7f;

						if (i == BODYPART_HELM)
						{
							int index = Type - MODEL_HELM;
							if (index == 0 || index == 5 || index == 6 || index == 8 || index == 9)
							{
								Type = MODEL_MASK_HELM + index;
								BodyModel = gmClientModels->GetModel(Type);

								if (BodyModel)
								{
									BodyModel->Skin = c->GetSkinModelIndex();
								}
							}
						}
					}

					if (c->MonsterIndex >= 529 && c->MonsterIndex <= 539)
					{
						if (World == WD_65DOPPLEGANGER1)
							RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, p->Level << 3, p->Option1, p->ExtOption, false, false, Translate, Select, RENDER_DOPPELGANGER | RENDER_TEXTURE);
						else
							RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, p->Level << 3, p->Option1, p->ExtOption, false, false, Translate, Select, RENDER_DOPPELGANGER | RENDER_BRIGHT | RENDER_TEXTURE);
					}
					else
					{
						if ((o->CurrentAction == PLAYER_SKILL_DRAGONKICK) && (c->GetBaseClass() == CLASS_RAGEFIGHTER))
						{
							RenderCharacter_AfterImage(c, p, Translate, Select, 2.5f, 1.0f);
						}
						else if ((o->CurrentAction == PLAYER_SKILL_DARKSIDE_READY) && (c->GetBaseClass() == CLASS_RAGEFIGHTER))
						{
#ifdef PBG_MOD_RAGEFIGHTERSOUND
							if (o->m_sTargetIndex < 0 || c->JumpTime>0)
							{
								RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, p->Level << 3, p->Option1, p->ExtOption, false, false, Translate, Select);
							}
							else
#endif //PBG_MOD_RAGEFIGHTERSOUND
								g_CMonkSystem.DarksideRendering(c, p, Translate, Select);
						}
						else
						{
							if (i > BODYPART_HEAD && o->Kind == KIND_PLAYER && o->Type == MODEL_PLAYER && g_pOption->GetRenderEquipment() == false)
							{
								int nobody = ((i - 1) * MODEL_BODY_NUM) + MODEL_BODY_HELM;

								Type = gCharacterManager.GetSkinModelIndex(c->Class) + nobody;

								BodyModel = gmClientModels->GetModel(Type);

								if (BodyModel)
								{
									BodyModel->Skin = c->GetSkinModelIndex();
								}

								RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, 0, 0, 0, false, false, Translate, Select);
							}
							else
							{
								if (GMItemMng->IsLuckyItem((Type - MODEL_ITEM)))
									RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, p->Level << 3, p->Option1, MAX_MODELS, false, false, Translate, Select);
								else
									RenderPartObject(&c->Object, Type, p, c->Light, o->Alpha, p->Level << 3, p->Option1, p->ExtOption, false, false, Translate, Select);
							}
						}
					}
				}
			}
		}

		if (gMapManager->InChaosCastle() == false)
		{
			if (c->GuildMarkIndex >= 0 && o->Type == MODEL_PLAYER && o->Alpha != 0.0f
				&& (c->GuildMarkIndex == MARK_EDIT || g_GuildCache.IsExistGuildMark(GuildMark[c->GuildMarkIndex].Key) == TRUE)
				&& (!g_isCharacterBuff(o, eBuff_Cloaking)))
			{
				CreateGuildMark(c->GuildMarkIndex);

				if (c->GetBaseClass() == CLASS_RAGEFIGHTER)
				{
					vec3_t vPos;
					if (c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 59
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 60
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 61)
					{
						Vector(5.0f, 0.0f, -35.0f, vPos);
					}
					else
					{
						Vector(5.0f, 0.0f, -21.0f, vPos);
					}
					RenderGuild(o, c->BodyPart[BODYPART_ARMOR].Type, vPos);
				}
				else
				{
					RenderGuild(o, c->BodyPart[BODYPART_ARMOR].Type);
				}
			}
		}
	}

	if (bCloak || c->Wing.Type != -1)
	{
		runtime_physical_cloth(c, o, CloakLight);
	}

	float Luminosity = (float)(rand() % 30 + 70) * 0.01f;

	if (c->PK >= PVP_MURDERER2)
	{
		Vector(1.f, 0.1f, 0.1f, c->Light);
	}
	else
	{
		VectorAdd(Light, o->Light, c->Light);

		int nCastle = BLOODCASTLE_NUM + (World - WD_11BLOODCASTLE_END);
		if (nCastle > 0 && nCastle <= BLOODCASTLE_NUM)
		{
			if ((c->MonsterIndex >= 86 && c->MonsterIndex <= 89) ||
				(c->MonsterIndex >= 92 && c->MonsterIndex <= 95) ||
				(c->MonsterIndex == 99 || c->MonsterIndex == 111 || c->MonsterIndex == 112) ||
				(c->MonsterIndex >= 115 && c->MonsterIndex <= 118) ||
				(c->MonsterIndex >= 121 && c->MonsterIndex <= 124) ||
				(c->MonsterIndex >= 127 && c->MonsterIndex <= 130) ||
				(c->MonsterIndex >= 138 && c->MonsterIndex <= 143))
			{
				int level = nCastle / 3;
				if (level)
				{
					Vector(level * 0.5f, 0.1f, 0.1f, c->Light);
				}
			}
		}
	}

	if (!gMapManager->InChaosCastle() && !(gMapManager->IsCursedTemple() && !c->SafeZone))
	{
		if (g_pOption->GetRenderEquipment())
		{
			if (gmProtect->max_item_level_on >= 15)
			{
				NextGradeObjectRender(c);
			}
		}
	}

	for (int i3 = 0; i3 < 2; ++i3)
	{
		int ItemWeapon = c->Weapon[i3].Type;
		if (g_CMonkSystem.EqualItemModelType(ItemWeapon) == (MODEL_SWORD + 35))
		{
			ModelLinkObject(c, i3);
		}
	}

	bool Bind = false;
	Bind = RenderCharacterBackItem(c, o, Translate);

	if (!Bind)
	{
		if (!g_isCharacterBuff(o, eBuff_Cloaking)
			&& (g_isCharacterBuff(o, eBuff_AddCriticalDamage)
				|| g_isCharacterBuff(o, EFFECT_GREATER_CRITICAL_DAMAGE_EXTENDED))
			&& o->Kind == KIND_PLAYER
			&& o->Type == MODEL_PLAYER
			&& c->critical_work.now())
		{
			bool  renderSkillWave = (rand() % 20) ? true : false;
			short weaponType = -1;

			Vector(0.f, 0.f, 0.f, p);
			Vector(1.f, 0.6f, 0.3f, Light);

			if (c->Weapon[1].Type != MODEL_BOW + 15)
			{
				vec4_t* bone001 = o->GetBoneTransform(c->Weapon[0].LinkBone);

				if (!bone001)
					return;

				b->TransformPosition(bone001, p, Position, true);

				if (c->Weapon[0].Type >= MODEL_BOW && c->Weapon[0].Type < MODEL_STAFF)
				{
					weaponType = 1;
				}
				CreateEffect(BITMAP_FLARE_FORCE, Position, o->Angle, o->Light, 1, o, weaponType, c->Weapon[0].LinkBone);

				if (renderSkillWave == false)
				{
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 0);
				}
			}
			if (c->Weapon[1].Type != MODEL_BOW + 7 && (c->Weapon[1].Type < MODEL_SHIELD || c->Weapon[1].Type >= MODEL_SHIELD + MAX_ITEM_INDEX))
			{
				vec4_t* bone001 = o->GetBoneTransform(c->Weapon[1].LinkBone);

				if (!bone001)
					return;

				b->TransformPosition(bone001, p, Position, true);

				if (c->Weapon[1].Type >= MODEL_BOW && c->Weapon[1].Type < MODEL_STAFF)
				{
					weaponType = 1;
				}

				CreateEffect(BITMAP_FLARE_FORCE, Position, o->Angle, o->Light, 1, o, weaponType, c->Weapon[1].LinkBone);

				if (renderSkillWave == false)
				{
					CreateEffect(MODEL_DARKLORD_SKILL, Position, o->Angle, Light, 1);
				}
			}
			PlayBuffer(SOUND_CRITICAL, o);
		}

		c->PostMoveProcess_Process();

		for (int i = 0; i < 2; i++)
		{
			if (i == 0)
			{
				if (o->CurrentAction == PLAYER_HIT_SKILL_FURY_STRIKE && o->AnimationFrame <= 4.f)
					continue;

				if (true == c->PostMoveProcess_IsProcessing())
				{
					c->Skill = 0;
					continue;
				}
			}

			PART_t* w = &c->Weapon[i];

			int weapon02Kind2 = GMItemMng->GetKind2(w->Type - MODEL_ITEM);

			if (w->Type != -1 && weapon02Kind2 != ItemKind2::ARROW && weapon02Kind2 != ItemKind2::BOLT && weapon02Kind2 != ItemKind2::QUIVER && w->Type != MODEL_HELPER + 5)
			{
				Action_t* PlayerAction;
				if (o->CurrentAction != PLAYER_HIT_BOW
					&& o->CurrentAction != PLAYER_HIT_CROSSBOW
					&& o->CurrentAction != PLAYER_HIT_FLY_BOW
					&& o->CurrentAction != PLAYER_HIT_FLY_CROSSBOW)
				{
					switch (w->Type)
					{
					case MODEL_MACE + 2:
						if (o->CurrentAction < PLAYER_HIT_SWORD_RIGHT1 || o->CurrentAction > PLAYER_HIT_SWORD_RIGHT2)
						{
							w->CurrentAction = 1;
							PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
							w->PlaySpeed = PlayerAction->PlaySpeed;
						}
						else
						{
							w->CurrentAction = 2;
							PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_HIT_SWORD_RIGHT1);
							w->PlaySpeed = PlayerAction->PlaySpeed;
						}
						break;
					case MODEL_MACE + 5:
						w->CurrentAction = 0;
						PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
						w->PlaySpeed = (PlayerAction->PlaySpeed * 2.0);
						break;
					case MODEL_STAFF + 6:
						w->CurrentAction = 0;
						PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
						w->PlaySpeed = (PlayerAction->PlaySpeed * 15.0);
						break;
					case MODEL_SWORD + 35:
						if (w->AnimationFrame < 2.0)
						{
							w->CurrentAction = 0;
							PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_RAGEFIGHTER);
							w->PlaySpeed = (PlayerAction->PlaySpeed * 1.5);
							PlayBuffer(sound_Defender_attack2, 0, 0);
						}
						else
						{
							w->AnimationFrame = 2.0;
						}
						break;
					default:
						if (w->Type < MODEL_SWORD || w->Type >= MODEL_AXE)
						{
							if (w->Type == MODEL_BOW + 23)
							{
								w->CurrentAction = 0;
								PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
								w->PlaySpeed = PlayerAction->PlaySpeed;
							}
							else if (w->Type != MODEL_MACE + 16 && w->Type != MODEL_MACE + 17)
							{
								if (w->Type == MODEL_STAFF + 34)
								{
									w->CurrentAction = 0;
									PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
									w->PlaySpeed = PlayerAction->PlaySpeed;
								}
								else if (w->Type == MODEL_STAFF + 36)
								{
									w->CurrentAction = 0;
									PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_SUMMONER);
									w->PlaySpeed = PlayerAction->PlaySpeed;
								}
								else
								{
									BMD* weaponmodel = gmClientModels->GetModel(w->Type);

									if (!(weaponmodel && weaponmodel->runtime_linkaction(w, o)))
									{
										w->CurrentAction = 0;
										w->PlaySpeed = 0.0;
										w->AnimationFrame = 0.0;
										w->PriorAnimationFrame = 0.0;
									}
								}
							}
							else
							{
								w->CurrentAction = 0;
								PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
								w->PlaySpeed = PlayerAction->PlaySpeed;
							}
						}
						else
						{
							w->CurrentAction = 0;
							PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_STOP_MALE);
							w->PlaySpeed = PlayerAction->PlaySpeed;
						}
						break;
					}
				}
				else
				{
					if (w->Type == (MODEL_BOW + 23))
						w->CurrentAction = 1;
					else
						w->CurrentAction = 0;
					PlayerAction = gmClientModels->GetPlayerModelAction(PLAYER_HIT_BOW);
					w->PlaySpeed = PlayerAction->PlaySpeed;
				}

				if (g_CMonkSystem.IsSwordformGloves(w->Type))
					g_CMonkSystem.RenderSwordformGloves(c, w->Type, i, o->Alpha, Translate, Select);
				else
					RenderLinkObject(0.f, 0.f, 0.f, c, w, w->Type, w->Level, w->Option1, false, Translate);

				if (w->Level >= 7)
				{
					Vector(Luminosity * 0.5f, Luminosity * 0.4f, Luminosity * 0.3f, Light);
				}
				else if (w->Level >= 5)
				{
					Vector(Luminosity * 0.3f, Luminosity * 0.3f, Luminosity * 0.5f, Light);
				}
				else if (w->Level >= 3)
				{
					Vector(Luminosity * 0.5f, Luminosity * 0.3f, Luminosity * 0.3f, Light);
				}
				else
				{
					Vector(Luminosity * 0.3f, Luminosity * 0.3f, Luminosity * 0.3f, Light);
				}
				float Scale;
				if (c->PK < PVP_MURDERER2 && c->Level != 4)
				{
					bool Success = true;

					switch (w->Type)
					{
					case MODEL_SWORD + 4:
					case MODEL_SWORD + 5:
					case MODEL_SWORD + 13:
						Vector(0.f, -110.f, 5.f, Position);
						break;
					case MODEL_SWORD + 8:
					case MODEL_SWORD + 9:
						Vector(0.f, -110.f, -5.f, Position);
						break;
					case MODEL_SWORD + 7:
					case MODEL_SWORD + 10:
						Vector(0.f, -110.f, 0.f, Position);
						break;
					case MODEL_SWORD + 14:
					case MODEL_SWORD + 11:
						Vector(0.f, -150.f, 0.f, Position);
						break;
					case MODEL_SWORD + 12:
						Vector(Luminosity, Luminosity, Luminosity, Light);
						Vector(0.f, -160.f, 0.f, Position);
						break;
					case MODEL_SWORD + 17:
						Success = false;
						Scale = sinf(WorldTime * 0.004f) * 10.f + 20.f;
						{
							vec3_t  pos1, pos2;

							Vector(0.f, -20.f, -40.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, pos1, true);
							Vector(0.f, -160.f, -10.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, pos2, true);
							CreateJointSync(BITMAP_FLARE + 1, pos1, pos2, o->Angle, 4, o, Scale);

							Vector(0.f, -10.f, 28.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, pos1, true);
							Vector(0.f, -145.f, 18.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, pos2, true);
							CreateJointSync(BITMAP_FLARE + 1, pos1, pos2, o->Angle, 4, o, Scale);
						}
						break;
					case MODEL_SWORD + 18:
					{
						Success = false;

						Vector(0.f, -20.f, 15.f, Position);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);

						Scale = sinf(WorldTime * 0.004f) * 0.3f + 0.3f;
						Vector(Scale * 0.2f, Scale * 0.2f, Scale * 1.f, Light);
						CreateSprite(BITMAP_SHINY + 1, p, Scale + 1.f, Light, o, 0);

						Vector(0.f, -133.f, 7.f, Position);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, Position, true);

						Scale = (Scale * 20.f) + 20.f;
						CreateJointSync(BITMAP_JOINT_THUNDER, p, Position, o->Angle, 10, NULL, Scale);
						CreateJointSync(BITMAP_JOINT_THUNDER, p, Position, o->Angle, 10, NULL, Scale);
						CreateJointSync(BITMAP_JOINT_THUNDER, p, Position, o->Angle, 10, NULL, Scale);
					}
					break;
					case MODEL_STAFF + 9:
						Success = false;
						Vector(0.f, -120.f, 5.f, Position);
						Vector(Luminosity * 0.6f, Luminosity * 0.6f, Luminosity * 2.f, Light);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 1.5f, Light, o);
						CreateSprite(BITMAP_LIGHT, p, Luminosity + 1.f, Light, o);

						Vector(0.f, 100.f, 10.f, Position);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_LIGHT, p, Luminosity + 1.f, Light, o);
						break;
					case MODEL_STAFF + 4:
						Success = false;
						Vector(0.f, -90.f, 0.f, Position);
						Vector(Luminosity * 0.4f, Luminosity * 0.8f, Luminosity * 0.6f, Light);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 2.f, Light, o);
						break;
					case MODEL_SHIELD + 14:
						Success = false;
						Vector(20.f, 0.f, 0.f, Position);
						Vector(Luminosity * 0.4f, Luminosity * 0.6f, Luminosity * 1.5f, Light);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 1.5f, Light, o);
						break;
					case MODEL_STAFF + 5:
						Success = false;
						Vector(0.f, -145.f, 0.f, Position);
						Vector(Luminosity * 0.4f, Luminosity * 0.6f, Luminosity * 1.f, Light);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 1.5f, Light, o);
						CreateSprite(BITMAP_LIGHTNING + 1, p, 0.3f, Light, o);
						break;
					case MODEL_STAFF + 7:
						Success = false;
						Vector(Luminosity * 0.4f, Luminosity * 0.6f, Luminosity * 1.f, Light);
						RenderBrightEffect(b, BITMAP_SHINY + 1, 27, 2.f, Light, o);

						for (int j = 28; j <= 37; j++)
						{
							RenderBrightEffect(b, BITMAP_LIGHT, j, 1.5f, Light, o);
						}
						break;
					case MODEL_STAFF + 6:
						Success = false;
						Vector(0.f, -145.f, 0.f, Position);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						Vector(Luminosity * 1.f, Luminosity * 0.6f, Luminosity * 0.4f, Light);
						CreateSprite(BITMAP_SPARK, p, 3.f, Light, o);
						CreateSprite(BITMAP_SHINY + 2, p, 1.5f, Light, o);

						for (int j = 0; j < 4; j++)
						{
							Vector((float)(rand() % 20 - 10), (float)(rand() % 20 - 10 - 90.f), (float)(rand() % 20 - 10), Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
							CreateParticleSync(BITMAP_SPARK, p, o->Angle, Light, 1);
						}
						Vector(Luminosity * 1.f, Luminosity * 0.2f, Luminosity * 0.1f, Light);

						for (int j = 0; j < 10; j++)
						{
							if (rand() % 4 < 3)
							{
								Vector(0.f, -j * 20 + 60.f, 0.f, Position);
								b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
								CreateSprite(BITMAP_LIGHT, p, 1.f, Light, o);
							}
						}
						break;
					case MODEL_STAFF + 15:
						Success = false;
						Vector(Luminosity * 0.2f, Luminosity * 0.3f, Luminosity * 1.4f, Light);
						Vector(0.f, 0.f, 0.f, Position);
						b->TransformPosition(BoneTransform[1], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 1.f, Light, o, -(int)WorldTime * 0.1f);
						b->TransformPosition(BoneTransform[2], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 0.5f, Light, o, (int)WorldTime * 0.1f);
						break;
					case MODEL_STAFF + 16:
						Success = false;
						Vector(Luminosity * 1.0f, Luminosity * 0.3f, Luminosity * 0.4f, Light);
						Vector(0.f, 0.f, 0.f, Position);
						b->TransformPosition(BoneTransform[1], Position, p, true);
						CreateSprite(BITMAP_SHINY + 1, p, 1.f, Light, o, -(int)WorldTime * 0.1f);
						CreateSprite(BITMAP_SHINY + 1, p, 1.f, Light, o, -(int)WorldTime * 0.13f);
						break;
					case MODEL_STAFF + 17:
						Success = false;
						Scale = absf(sinf(WorldTime * 0.002f)) * 0.2f;
						Luminosity = absf(sinf(WorldTime * 0.002f)) * 0.4f;
						Vector(0.5f + Luminosity, 0.2f + Luminosity, 0.9f + Luminosity, Light);

						for (int j = 1; j <= 4; ++j)
							RenderBrightEffect(b, BITMAP_LIGHT, j, Scale + 1.0f, Light, o);
						break;
					case MODEL_STAFF + 18:
						Success = false;
						Scale = absf(sinf(WorldTime * 0.002f)) * 0.2f;
						Luminosity = absf(sinf(WorldTime * 0.002f)) * 0.4f;
						Vector(0.5f + Luminosity, 0.2f + Luminosity, 0.9f + Luminosity, Light);

						for (int j = 1; j <= 2; ++j)
						{
							RenderBrightEffect(b, BITMAP_SHINY + 1, j, Scale + 1.0f, Light, o);
							RenderBrightEffect(b, BITMAP_LIGHT, j, Scale + 1.0f, Light, o);
						}
						Vector(0.8f + Luminosity, 0.6f + Luminosity, 0.3f + Luminosity, Light);
						RenderBrightEffect(b, BITMAP_LIGHT, 3, Scale + 1.0f, Light, o);
						break;
					case MODEL_STAFF + 19:
						Success = false;
						Scale = absf(sinf(WorldTime * 0.002f)) * 0.2f;
						Luminosity = absf(sinf(WorldTime * 0.002f)) * 0.4f;
						Vector(0.5f + Luminosity, 0.2f + Luminosity, 0.9f + Luminosity, Light);

						for (int j = 2; j <= 3; ++j)
							RenderBrightEffect(b, BITMAP_LIGHT, j, Scale + 1.0f, Light, o);

						Vector(0.8f + Luminosity, 0.6f + Luminosity, 0.3f + Luminosity, Light);
						RenderBrightEffect(b, BITMAP_SHINY + 2, 2, Scale + 1.0f, Light, o);
						break;
					case MODEL_STAFF + 20:
						Success = false;
						Vector(1.0f, 0.2f, 0.1f, Light);
						Vector(0.f, 0.f, 0.f, Position);
						b->TransformPosition(BoneTransform[2], Position, p, true);
						CreateSprite(BITMAP_EVENT_CLOUD, p, 0.25f, Light, o, -(int)WorldTime * 0.1f);
						CreateSprite(BITMAP_EVENT_CLOUD, p, 0.25f, Light, o, -(int)WorldTime * 0.2f);
						Vector(1.0f, 0.4f, 0.3f, Light);
						RenderBrightEffect(b, BITMAP_SHINY + 1, 2, 1.0f, Light, o);
						Vector(1.0f, 0.2f, 0.0f, Light);
#ifndef	ASG_ADD_ETERNALWING_STICK_EFFECT
						CreateParticleSync(BITMAP_SPARK + 1, p, o->Angle, Light, 16, 1.0f);
#endif	// ASG_ADD_ETERNALWING_STICK_EFFECT

						CreateParticleSync(BITMAP_SPARK + 1, p, o->Angle, Light, 23, 1.0f);
#ifdef ASG_ADD_ETERNALWING_STICK_EFFECT
						if (rand() % 20 == 0)
							CreateParticleSync(BITMAP_SPARK + 1, p, o->Angle, Light, 20, 1.0f);
						Vector(1.0f, 0.0f, 0.0f, Light);
						RenderBrightEffect(b, BITMAP_LIGHT, 2, 3.0f, Light, o);
#endif	// ASG_ADD_ETERNALWING_STICK_EFFECT
						break;
					case MODEL_BOW + 16:
						Success = false;
						Vector(Luminosity * 0.4f, Luminosity * 0.6f, Luminosity * 1.f, Light);

						for (int j = 0; j < 6; j++)
						{
							Vector(0.f, -10.f, -j * 20.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
							CreateSprite(BITMAP_LIGHT, p, 2.f, Light, o);
						}
						break;
					case MODEL_MACE + 5:
						Success = false;
						Vector(Luminosity * 1.f, Luminosity * 0.6f, Luminosity * 0.4f, Light);

						for (int j = 0; j < 8; j++)
						{
							if (rand() % 4 < 3)
							{
								Vector(0.f, -j * 20 - 30.f, 0.f, Position);
								b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
								CreateSprite(BITMAP_LIGHT, p, 1.f, Light, o);
							}
						}
						break;
					case MODEL_MACE + 6:
						Success = false;
						Vector(0.f, -84.f, 0.f, Position);
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						Scale = sinf(WorldTime * 0.004f) * 0.3f + 0.7f;
						Vector(Scale * 1.f, Scale * 0.2f, Scale * 0.1f, Light);
						CreateSprite(BITMAP_SHINY + 1, p, Scale + 1.5f, Light, o);

						for (int j = 0; j < 5; j++)
						{
							Vector(0.f, -j * 20.f - 10.f, 0.f, Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
							CreateSprite(BITMAP_SHINY + 1, p, 1.f, Light, o);
						}
						Vector(Scale * 0.5f, Scale * 0.1f, Scale * 0.05f, Light);
						RenderBrightEffect(b, BITMAP_SHINY + 1, 2, 1.f, Light, o);
						RenderBrightEffect(b, BITMAP_SHINY + 1, 6, 1.f, Light, o);
						break;
					case MODEL_BOW + 13:
					case MODEL_BOW + 14:
						Success = false;
						if (w->Type == MODEL_BOW + 13)
						{
							Vector(Luminosity * 0.2f, Luminosity * 0.4f, Luminosity * 0.6f, Light);
						}
						else
						{
							Vector(Luminosity * 0.6f, Luminosity * 0.4f, Luminosity * 0.2f, Light);
						}

						for (int j = 0; j < 6; j++)
						{
							Vector(0.f, -20.f, (float)(-j * 20), Position);
							b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
							CreateSprite(BITMAP_LIGHT, p, 2.f, Light, o);
						}
						break;
					default:
						Success = false;
					}
					if (Success)
					{
						b->TransformPosition(o->BoneTransform[w->LinkBone], Position, p, true);
						CreateSprite(BITMAP_LIGHT, p, 1.4f, Light, o);
					}
				}
			}
		}
	}
	switch (o->Type)
	{
	case MODEL_PLAYER:
		Vector(0.f, 0.f, 0.f, p);

		if (gCharacterManager.GetCharacterClass(c->Class) == CLASS_SOULMASTER)
		{
			if (!g_isCharacterBuff(o, eBuff_Cloaking))
			{
				Vector(-4.f, 11.f, 0.f, p);
				Vector(1.f, 1.f, 1.f, Light);
				b->TransformPosition(o->BoneTransform[19], p, Position, true);
				CreateSprite(BITMAP_SPARK + 1, Position, 0.6f, Light, NULL);

				float scale = sinf(WorldTime * 0.001f) * 0.4f;
				CreateSprite(BITMAP_SHINY + 1, Position, scale, Light, NULL);
			}
		}
		if (o->CurrentAction == PLAYER_SKILL_FLASH || o->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH || o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_FLASH)
		{
			if (c->GetBaseClass() == CLASS_DARK_LORD || o->CurrentAction == PLAYER_HIT_RIDE_ATTACK_FLASH || o->CurrentAction == PLAYER_FENRIR_ATTACK_DARKLORD_FLASH)
			{
				if (o->AnimationFrame >= 1.2f && o->AnimationFrame < 1.6f)
				{
					vec3_t Angle;
					Vector(1.f, 0.f, 0.f, Angle);
					CreateEffect(BITMAP_GATHERING, o->Position, o->Angle, o->Light, 2, o);

					PlayBuffer(SOUND_ELEC_STRIKE_READY);
				}

				if (o->AnimationFrame < 2.f)
				{
					if (PartyNumber > 0 /*&& rand()%2==0*/)
					{
						if (g_pPartyManager->IsPartyMemberChar(c) == false)
							break;

						for (int i = 0; i < PartyNumber; ++i)
						{
							PARTY_t* p = &Party[i];
							if (p->index < 0) continue;

							CHARACTER* tc = gmCharacters->GetCharacter(p->index);
							if (tc != c)
							{
								VectorCopy(tc->Object.Position, Position);
								Position[2] += 150.f;
								CreateJointSync(BITMAP_JOINT_ENERGY, Position, Position, o->Angle, 12, o, 20.f);
								CreateJointSync(BITMAP_JOINT_ENERGY, Position, Position, o->Angle, 13, o, 20.f);
							}
						}
					}
				}

				if (o->AnimationFrame >= 7 && o->AnimationFrame < 8)
				{
					vec3_t Angle, Light;

					b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);

					Vector(0.8f, 0.5f, 1.f, Light);
					Vector(180.f, 45.f, 0.f, Angle);
					CreateEffectSync(MODEL_DARKLORD_SKILL, Position, Angle, Light, 2);

					Vector(0.f, 0.f, o->Angle[2], Angle);
					CreateEffectSync(MODEL_DARKLORD_SKILL, Position, Angle, Light, 2);
				}
			}
			else
			{
				if (g_isCharacterBuff(o, eBuff_Cloaking))
				{
					b->TransformPosition(o->BoneTransform[c->Weapon[0].LinkBone], p, Position, true);
					Vector(0.1f, 0.1f, 1.f, Light);
					float Scale = o->AnimationFrame * 0.1f;
					CreateSprite(BITMAP_LIGHTNING + 1, Position, Scale * 0.3f, Light, o);
					CreateSprite(BITMAP_LIGHTNING + 1, Position, Scale * 1.f, o->Light, o, -(int)WorldTime * 0.1f);
					CreateSprite(BITMAP_LIGHTNING + 1, Position, Scale * 2.5f, o->Light, o, (int)WorldTime * 0.1f);
				}
			}
		}
		else if (o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB
			|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_UNI
			|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_DINO
			|| o->CurrentAction == PLAYER_SKILL_LIGHTNING_ORB_FENRIR
			)
		{
			vec3_t vLight, vRelativePos, vWorldPos;
			Vector(1.0f, 1.0f, 1.0f, vLight);
			Vector(0.f, 0.f, 0.f, vRelativePos);
			Vector(0.f, 0.f, 0.f, vWorldPos);
			// 27 "Bip01 R Forearm"	
			b->TransformPosition(o->BoneTransform[27], vRelativePos, vWorldPos, true);

			Vector(0.2f, 0.2f, 1.0f, vLight);
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
		}
		else if (o->CurrentAction >= PLAYER_SKILL_SLEEP && o->CurrentAction <= PLAYER_SKILL_SLEEP_FENRIR)
		{
			int iSkillType = CharacterAttribute->Skill[Hero->CurrentSkill];

			vec3_t vLight, vRelativePos, vWorldPos;
			Vector(1.0f, 1.0f, 1.0f, vLight);
			Vector(0.f, 0.f, 0.f, vRelativePos);
			Vector(0.f, 0.f, 0.f, vWorldPos);
			b->TransformPosition(o->BoneTransform[37], vRelativePos, vWorldPos, true);	// "Bip01 L Hand"

			float fRot = (WorldTime * 0.0006f) * 360.0f;

			// shiny
			if (iSkillType == AT_SKILL_ALICE_SLEEP)	// nukun
			{
				Vector(0.5f, 0.2f, 0.8f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				Vector(1.0f, 1.0f, 1.0f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_THORNS)
			{
				Vector(0.8f, 0.5f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BERSERKER)
			{
				Vector(1.0f, 0.1f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_WEAKNESS)
			{
				Vector(0.8f, 0.1f, 0.1f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_ENERVATION)
			{
				Vector(0.25f, 1.0f, 0.7f, Light);
			}

			if ((iSkillType == AT_SKILL_ALICE_SLEEP || iSkillType == AT_SKILL_ALICE_THORNS
				|| iSkillType == AT_SKILL_ALICE_BERSERKER
				|| iSkillType == AT_SKILL_ALICE_WEAKNESS || iSkillType == AT_SKILL_ALICE_ENERVATION
				))
			{
				CreateSprite(BITMAP_SHINY + 5, vWorldPos, 1.0f, vLight, o, fRot);
				CreateSprite(BITMAP_SHINY + 5, vWorldPos, 0.7f, vLight, o, -fRot);
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				CreateSprite(BITMAP_SHINY + 5, vWorldPos, 1.0f, vLight, o, fRot, 1);
				CreateSprite(BITMAP_SHINY + 5, vWorldPos, 0.7f, vLight, o, -fRot, 1);
			}

			// pin_light
			if (iSkillType == AT_SKILL_ALICE_SLEEP)
			{
				Vector(0.7f, 0.0f, 0.8f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				Vector(1.0f, 1.0f, 1.0f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_THORNS)
			{
				Vector(0.8f, 0.5f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BERSERKER)
			{
				Vector(1.0f, 0.1f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_THORNS)
			{
				Vector(0.8f, 0.1f, 0.1f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_ENERVATION)
			{
				Vector(0.25f, 1.f, 0.7f, vLight);
			}

			if (iSkillType == AT_SKILL_ALICE_SLEEP || iSkillType == AT_SKILL_ALICE_THORNS
				|| iSkillType == AT_SKILL_ALICE_BERSERKER
				|| iSkillType == AT_SKILL_ALICE_WEAKNESS || iSkillType == AT_SKILL_ALICE_ENERVATION
				)
			{
				CreateSprite(BITMAP_PIN_LIGHT, vWorldPos, 1.7f, vLight, o, (float)(rand() % 360));
				CreateSprite(BITMAP_PIN_LIGHT, vWorldPos, 1.5f, vLight, o, (float)(rand() % 360));
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				CreateSprite(BITMAP_PIN_LIGHT, vWorldPos, 1.7f, vLight, o, (float)(rand() % 360), 1);
				CreateSprite(BITMAP_PIN_LIGHT, vWorldPos, 1.5f, vLight, o, (float)(rand() % 360), 1);
			}

			// cra04, clud64
			if (iSkillType == AT_SKILL_ALICE_SLEEP)
			{
				Vector(0.6f, 0.1f, 0.8f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				Vector(1.0f, 1.0f, 1.0f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_THORNS)
			{
				Vector(0.8f, 0.5f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_BERSERKER)
			{
				Vector(1.0f, 0.1f, 0.2f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_THORNS)
			{
				Vector(0.8f, 0.1f, 0.1f, vLight);
			}
			else if (iSkillType == AT_SKILL_ALICE_ENERVATION)
			{
				Vector(0.25f, 1.f, 0.7f, vLight);
			}

			if ((iSkillType == AT_SKILL_ALICE_SLEEP || iSkillType == AT_SKILL_ALICE_THORNS
				|| iSkillType == AT_SKILL_ALICE_BERSERKER
				|| iSkillType == AT_SKILL_ALICE_WEAKNESS || iSkillType == AT_SKILL_ALICE_ENERVATION
				))
			{
				CreateParticleSync(BITMAP_LIGHT + 2, vWorldPos, o->Angle, vLight, 0, 1.0f);
				CreateParticleSync(BITMAP_CLUD64, vWorldPos, o->Angle, vLight, 3, 0.5f);
			}
			else if (iSkillType == AT_SKILL_ALICE_BLIND)
			{
				CreateParticleSync(BITMAP_LIGHT + 2, vWorldPos, o->Angle, vLight, 4, 1.0f);
				CreateParticleSync(BITMAP_CLUD64, vWorldPos, o->Angle, vLight, 5, 0.5f);
			}
		}
		// ChainLighting
		else if (o->CurrentAction == PLAYER_SKILL_CHAIN_LIGHTNING)
		{
			vec3_t vRelativePos, vWorldPos, vLight;
			Vector(0.f, 0.f, 0.f, vRelativePos);
			Vector(0.4f, 0.4f, 0.8f, vLight);

			b->TransformPosition(o->BoneTransform[37], vRelativePos, vWorldPos, true);	// "Bip01 L Hand"
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
			CreateSprite(BITMAP_LIGHT, vWorldPos, 1.5f, vLight, o, 0.f);

			b->TransformPosition(o->BoneTransform[28], vRelativePos, vWorldPos, true);	// "Bip01 R Hand"
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
			CreateEffectSync(MODEL_FENRIR_THUNDER, vWorldPos, o->Angle, vLight, 2, o);
			CreateSprite(BITMAP_LIGHT, vWorldPos, 1.5f, vLight, o, 0.f);
		}

		if (g_isCharacterBuff(o, eBuff_Attack) || g_isCharacterBuff(o, eBuff_HelpNpc))
		{
			if (!g_isCharacterBuff(o, eBuff_Cloaking))
			{
				for (int i = 0; i < 2; i++)
				{
					Luminosity = (float)(rand() % 30 + 70) * 0.01f;
					Vector(Luminosity * 1.f, Luminosity * 0.3f, Luminosity * 0.2f, Light);
					b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone], p, Position, true);
					CreateSprite(BITMAP_SHINY + 1, Position, 1.5f, Light, o);
					b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone - 6], p, Position, true);
					CreateSprite(BITMAP_SHINY + 1, Position, 1.5f, Light, o);
					b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone - 7], p, Position, true);
					CreateSprite(BITMAP_SHINY + 1, Position, 1.5f, Light, o);
				}
				if (!SearchJoint(MODEL_SPEARSKILL, o, 4)
					&& !SearchJoint(MODEL_SPEARSKILL, o, 9))
				{
					for (int i = 0; i < 5; i++)
					{
						CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 4, o, 20.0f, -1, 0, 0, c->TargetCharacter);
					}
				}
			}
		}
		else if (g_isCharacterBuff(o, eBuff_Defense))
		{
			if (!g_isCharacterBuff(o, eBuff_Cloaking))
			{
				if (SearchJoint(MODEL_SPEARSKILL, o, 4) == false && SearchJoint(MODEL_SPEARSKILL, o, 9) == false)
				{
					for (int i = 0; i < 5; ++i)
					{
						CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 4, o, 20.0f, -1, 0, 0, c->TargetCharacter);
					}
				}
			}
		}

		if (g_isCharacterBuff((&c->Object), eBuff_PcRoomSeal1)
			|| g_isCharacterBuff((&c->Object), eBuff_PcRoomSeal2)
			|| g_isCharacterBuff((&c->Object), eBuff_PcRoomSeal3)
			|| g_isCharacterBuff((&c->Object), eBuff_Seal1)
			|| g_isCharacterBuff((&c->Object), eBuff_Seal2)
			|| g_isCharacterBuff((&c->Object), eBuff_Seal3)
			|| g_isCharacterBuff((&c->Object), eBuff_Seal4))
		{
			if (SearchJoint(MODEL_SPEARSKILL, o, 10) == false
				&& SearchJoint(MODEL_SPEARSKILL, o, 11) == false)
			{
				for (int i = 0; i < 3; ++i)
				{
					CreateJoint(MODEL_SPEARSKILL, o->Position, o->Position, o->Angle, 10, o, 12.0f, -1, 0, 0, c->Key);
				}
			}
		}
		else
		{
			DeleteJoint(MODEL_SPEARSKILL, o, 10);
			DeleteJoint(MODEL_SPEARSKILL, o, 11);
		}
		if (g_isCharacterBuff((&c->Object), eBuff_Thorns))
		{
			if (SearchJoint(BITMAP_FLARE, o, 43) == false)
			{
				vec3_t vLight;
				Vector(0.9f, 0.6f, 0.1f, vLight);
				CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 43, o, 50.f, 0, 0, 0, 0, vLight);
			}
		}
		else
		{
			DeleteJoint(BITMAP_FLARE, o, 43);
		}

		if (g_isCharacterBuff((&c->Object), eBuff_Berserker))
		{
			if (!SearchEffect(BITMAP_SPARK + 2, o, 2))
			{
				vec3_t vLight[2];
				Vector(0.9f, 0.0f, 0.1f, vLight[0]);
				Vector(1.0f, 1.0f, 1.0f, vLight[1]);
				for (int i = 0; i < 4; ++i)
				{
					CreateEffect(BITMAP_ORORA, o->Position, o->Angle, vLight[0], i, o);
					if (i == 2 || i == 3)
						CreateEffect(BITMAP_SPARK + 2, o->Position, o->Angle, vLight[1], i, o);
				}
				CreateEffect(BITMAP_LIGHT_MARKS, o->Position, o->Angle, vLight[0], 0, o);
			}
		}
		else
		{
			for (int i = 0; i < 4; ++i)
			{
				DeleteEffect(BITMAP_ORORA, o, i);
				if (i == 2 || i == 3)
					DeleteEffect(BITMAP_SPARK + 2, o, i);
			}
			DeleteEffect(BITMAP_LIGHT_MARKS, o);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_Blind))
		{
			if (SearchEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 3) == false)
			{
				vec3_t vLight;
				Vector(1.0f, 1.f, 1.f, vLight);
				CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o->Position, o->Angle, vLight, 3, o);
			}
		}
		else
		{
			DeleteEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 3);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_Sleep))
		{
			if (SearchEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 4) == false)
			{
				vec3_t vLight;
				Vector(0.8f, 0.3f, 0.9f, vLight);
				CreateEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o->Position, o->Angle, vLight, 4, o);
			}
		}
		else
		{
			DeleteEffect(MODEL_ALICE_BUFFSKILL_EFFECT, o, 4);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_AttackDown))
		{
			if (!SearchEffect(BITMAP_SHINY + 6, o, 1))
			{
				vec3_t vLight;
				Vector(1.4f, 0.2f, 0.2f, vLight);
				CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, vLight, 1, o, -1, 0, 0, 0, 0.5f);
				CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, vLight, 1, o, -1, 0, 0, 0, 1.f);
			}
		}
		else
		{
			DeleteEffect(BITMAP_SHINY + 6, o, 1);
			DeleteEffect(BITMAP_PIN_LIGHT, o, 1);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_DefenseDown))
		{
			if (!SearchEffect(BITMAP_SHINY + 6, o, 2))
			{
				vec3_t vLight;
				Vector(0.25f, 1.0f, 0.7f, Light);
				CreateEffect(BITMAP_SHINY + 6, o->Position, o->Angle, vLight, 2, o, -1, 0, 0, 0, 0.5f);
				CreateEffect(BITMAP_PIN_LIGHT, o->Position, o->Angle, vLight, 2, o, -1, 0, 0, 0, 1.f);
			}
		}
		else
		{
			DeleteEffect(BITMAP_SHINY + 6, o, 2);
			DeleteEffect(BITMAP_PIN_LIGHT, o, 2);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_SahamuttDOT))
		{
			g_SummonSystem.CreateDamageOfTimeEffect(AT_SKILL_SUMMON_EXPLOSION, &c->Object);
		}
		else
		{
			g_SummonSystem.RemoveDamageOfTimeEffect(AT_SKILL_SUMMON_EXPLOSION, &c->Object);
		}

		if (g_isCharacterBuff((&c->Object), eDeBuff_NeilDOT))
		{
			g_SummonSystem.CreateDamageOfTimeEffect(AT_SKILL_SUMMON_REQUIEM, &c->Object);
		}
		else
		{
			g_SummonSystem.RemoveDamageOfTimeEffect(AT_SKILL_SUMMON_REQUIEM, &c->Object);
		}

		if (g_isCharacterBuff((&c->Object), eBuff_SwellOfMagicPower))
		{
			if (!g_isCharacterBuff((&c->Object), eBuff_Cloaking))
			{
				if (!SearchEffect(MODEL_SWELL_OF_MAGICPOWER_BUFF_EFF, o, 0))
				{
					vec3_t vLight;
					Vector(0.7f, 0.2f, 0.9f, vLight);
					CreateEffect(MODEL_SWELL_OF_MAGICPOWER_BUFF_EFF, o->Position, o->Angle, vLight, 0, o);
				}
			}
		}
		else
		{
			DeleteEffect(MODEL_SWELL_OF_MAGICPOWER_BUFF_EFF, o, 0);
		}

		if (gMapManager->InChaosCastle() == false)
		{
			if (c->ExtendState)
			{
				if ((o->CurrentAction < PLAYER_WALK_MALE ||
					o->CurrentAction == PLAYER_DARKLORD_STAND ||
					o->CurrentAction == PLAYER_STOP_RIDE_HORSE ||
					o->CurrentAction == PLAYER_STOP_TWO_HAND_SWORD_TWO
					) && c->ExtendStateTime.hasElapsed())
				{
					Vector(0.2f, 0.7f, 0.9f, Light);
					CreateEffect(BITMAP_LIGHT, o->Position, o->Angle, Light, 3, o);
				}
				//c->ExtendStateTime += timefac(1.f);
			}

			if (fullset && g_pOption->GetRenderEquipment())
			{
				PartObjectColor(c->BodyPart[5].Type, o->Alpha, 0.5f, Light);

				if (!g_isCharacterBuff(o, eBuff_Cloaking))
				{
					for (int i = 0; i < 2; i++)
					{
						b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone], p, Position, true);
						CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, o);
						b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone - 6], p, Position, true);
						CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, o);
						b->TransformPosition(o->BoneTransform[c->Weapon[i].LinkBone - 7], p, Position, true);
						CreateSprite(BITMAP_LIGHT, Position, 1.3f, Light, o);
					}
				}

				int nSubType = (c->BodyPart[BODYPART_BOOTS].Type - MODEL_ITEM) % MAX_ITEM_INDEX;

				CUSTOM_SMOKE_STACK* gSmokeItem = GMProtect->GetItemSmoke(nSubType);

				if (c->BodyPart[BODYPART_BOOTS].Type != -1 && ((c->BodyPart[BODYPART_BOOTS].Type >= MODEL_BOOTS + 29 && c->BodyPart[BODYPART_BOOTS].Type <= MODEL_BOOTS + 33)
					|| c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 43 || gSmokeItem != NULL))
				{
					if (EquipmentLevelSet > 9 && gmProtect->max_item_level_on >= 10)
					{
						VectorCopy(o->Light, Light);

						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 29)
							Vector(0.65f, 0.3f, 0.1f, o->Light);
						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 30)
							Vector(0.1f, 0.1f, 0.9f, o->Light);
						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 31)
							Vector(0.0f, 0.32f, 0.24f, o->Light);
						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 32)
							Vector(0.5f, 0.24f, 0.8f, o->Light);
						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 33)
							Vector(0.6f, 0.4f, 0.0f, o->Light);
						if (c->BodyPart[BODYPART_BOOTS].Type == MODEL_BOOTS + 43)
							Vector(0.6f, 0.3f, 0.4f, o->Light);

						if (gSmokeItem != NULL)
						{
							gSmokeItem->NormalizeColor(o->Light);
						}

						if (EquipmentLevelSet == 10 && gmProtect->max_item_level_on >= 10)
						{
							if ((rand() % 4) == 0)
							{
								Vector(0.0f, -18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 0.0f, 70.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							}
						}
						else if (EquipmentLevelSet == 11 && gmProtect->max_item_level_on >= 11)
						{
							if ((rand() % 3) == 0)
							{
								Vector(0.0f, -18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 0.0f, 70.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							}
						}
						else if (EquipmentLevelSet == 12 && gmProtect->max_item_level_on >= 12)
						{
							if ((rand() % 2) == 0)
							{
								Vector(0.0f, -18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 0.0f, 70.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
								Vector(0.0f, 18.0f, 50.0f, p);
								b->TransformPosition(o->BoneTransform[0], p, Position, true);
								CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							}
						}
						else if (EquipmentLevelSet == 13 && gmProtect->max_item_level_on >= 13)
						{
							Vector(0.0f, -20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 0.0f, 70.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
						}
						else if (EquipmentLevelSet == 14 && gmProtect->max_item_level_on >= 14)
						{
							Vector(0.0f, -20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 0.0f, 70.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
						}
						else if (EquipmentLevelSet == 15 && gmProtect->max_item_level_on >= 15)
						{
							Vector(0.0f, -20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 0.0f, 70.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
							Vector(0.0f, 20.0f, 50.0f, p);
							b->TransformPosition(o->BoneTransform[0], p, Position, true);
							CreateParticleSync(BITMAP_WATERFALL_2, Position, o->Angle, o->Light, 3);
						}
						VectorCopy(Light, o->Light);
					}
				}
				if (EquipmentLevelSet > 9)
				{
					if ((rand() % 20) == 0)//(o->CurrentAction<PLAYER_WALK_MALE || o->CurrentAction>PLAYER_RUN_RIDE_WEAPON) && (rand()%6)==0)
					{
						VectorCopy(o->Light, Light);
						Vector(1.f, 1.f, 1.f, o->Light);

						if (EquipmentLevelSet == 10 && gmProtect->max_item_level_on >= 10)
						{
							CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
						}
						else if (EquipmentLevelSet == 11 && gmProtect->max_item_level_on >= 11)
						{
							if ((rand() % 8) == 0)
							{
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 0, o);
							}
							else
							{
								CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
							}
						}
						else if (EquipmentLevelSet == 12 && gmProtect->max_item_level_on >= 12)
						{
							if ((MoveSceneFrame % 6) == 0)
							{
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 0);
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 1);
							}
							else if ((MoveSceneFrame % 3) == 0 && gmProtect->max_item_level_on >= 13)
							{
								CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
							}
						}
						else if (EquipmentLevelSet == 13 && gmProtect->max_item_level_on >= 13)
						{
							if ((MoveSceneFrame % 6) == 0)
							{

								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 0);
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 1);
							}

							if ((MoveSceneFrame % 4) == 0)
							{
								CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
								CreateJointSync(BITMAP_FLARE + 1, o->Position, o->Position, o->Angle, 7, o, 20, 40, 1);
							}
						}
						else if (EquipmentLevelSet == 14 && gmProtect->max_item_level_on >= 14)
						{
							if ((MoveSceneFrame % 6) == 0)
							{
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 0);
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 1);
							}

							if ((MoveSceneFrame % 4) == 0)
							{
								CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
								CreateJointSync(BITMAP_FLARE + 1, o->Position, o->Position, o->Angle, 7, o, 20, 40, 1);
							}
						}
						else if (EquipmentLevelSet == 15 && gmProtect->max_item_level_on >= 15)
						{
							if ((MoveSceneFrame % 6) == 0)
							{

								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 0);
								CreateJointSync(BITMAP_FLARE, o->Position, o->Position, o->Angle, 18, o, 20, -1, 1);
							}

							if ((MoveSceneFrame % 4) == 0)
							{
								CreateParticleSync(BITMAP_FLARE, o->Position, o->Angle, o->Light, 0, 0.19f, o);
								CreateJointSync(BITMAP_FLARE + 1, o->Position, o->Position, o->Angle, 7, o, 20, 40, 1);
							}
						}
						VectorCopy(Light, o->Light);
					}

					if (EquipmentLevelSet == 15 && gmProtect->max_item_level_on >= 15)
					{
						//left
						vec3_t vColor;
						Vector(13.0f, 10.0f, 3.0f, p);
						b->TransformPosition(o->BoneTransform[20], p, Position, true);
						VectorCopy(Position, o->EyeLeft);

						Vector(0.09f, 0.09f, 0.8f, vColor);
						CreateJointSync(BITMAP_JOINT_ENERGY, Position, o->Position, o->Angle, 55, o, 6.0f, -1, 0, 0, -1, vColor);
						float fRad = (float)sinf((WorldTime) * 0.002f);
						Vector(0.2f, 0.4f, 0.8f, vColor);
						CreateSprite(BITMAP_SHINY + 6, Position, 0.5f * fRad, vColor, o);

						Vector(0.1f, 0.15f, 1.0f, vColor);
						CreateSprite(BITMAP_PIN_LIGHT, Position, 1.3f * fRad + 0.5f, vColor, o, 100.0f);

						// right
						Vector(13.0f, 10.0f, -3.0f, p);
						b->TransformPosition(o->BoneTransform[20], p, Position, true);
						VectorCopy(Position, o->EyeRight);

						Vector(0.09f, 0.09f, 0.8f, vColor);
						CreateJointSync(BITMAP_JOINT_ENERGY, Position, o->Position, o->Angle, 56, o, 6.0f, -1, 0, 0, -1, vColor);

						Vector(0.2f, 0.4f, 0.8f, vColor);
						CreateSprite(BITMAP_SHINY + 6, Position, 0.5f * fRad, vColor, o);

						Vector(0.1f, 0.15f, 1.0f, vColor);
						CreateSprite(BITMAP_PIN_LIGHT, Position, 1.3f * fRad + 0.5f, vColor, o, 80.0f);
					}
				}
			}
		}
		break;
	case MODEL_MONSTER01:
	case MODEL_MONSTER01 + 30:
		if ((o->Type == MODEL_MONSTER01 && c->Level == 1) || (o->Type == MODEL_MONSTER01 + 30))
			RenderEye(o, 22, 23);
		break;
	case MODEL_MONSTER01 + 52:
		RenderEye(o, 26, 27, 2.0f);
		break;
	case MODEL_MONSTER01 + 37:
		RenderLight(o, BITMAP_LIGHTNING + 1, 1.f, 63, 0.f, 0.f, 20.f);
		RenderLight(o, BITMAP_SHINY + 2, 4.f, 63, 0.f, 0.f, 20.f);
		break;
	case MODEL_MONSTER01 + 34:
		RenderLight(o, BITMAP_LIGHTNING + 1, 0.5f, 30, 0.f, 0.f, -5.f);
		RenderLight(o, BITMAP_LIGHTNING + 1, 0.5f, 39, 0.f, 0.f, -5.f);
		RenderLight(o, BITMAP_SPARK, 4.f, 30, 0.f, 0.f, -5.f);
		RenderLight(o, BITMAP_SPARK, 4.f, 39, 0.f, 0.f, -5.f);
		RenderLight(o, BITMAP_SHINY + 2, 2.f, 30, 0.f, 0.f, -5.f);
		RenderLight(o, BITMAP_SHINY + 2, 2.f, 39, 0.f, 0.f, -5.f);
		break;
	case MODEL_MONSTER01 + 36:
		RenderEye(o, 42, 43);
		RenderLight(o, BITMAP_SPARK, 2.f, 26, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SPARK, 2.f, 31, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SPARK, 2.f, 36, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SPARK, 2.f, 41, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SHINY + 2, 1.f, 26, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SHINY + 2, 1.f, 31, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SHINY + 2, 1.f, 36, 0.f, 0.f, 0.f);
		RenderLight(o, BITMAP_SHINY + 2, 1.f, 41, 0.f, 0.f, 0.f);
		break;
	case MODEL_MONSTER01 + 33:
		RenderLight(o, BITMAP_SPARK, 4.f, 9, 0.f, 0.f, 5.f);
		RenderLight(o, BITMAP_SHINY + 2, 3.f, 9, 0.f, 0.f, 5.f);
		break;
	case MODEL_MIX_NPC:
		RenderLight(o, BITMAP_LIGHT, 1.5f, 32, 0.f, 0.f, 0.f);
		break;
	case MODEL_NPC_SEVINA:
		RenderLight(o, BITMAP_LIGHT, 2.5f, 6, 0.f, 0.f, 0.f);
		break;
	case MODEL_NPC_DEVILSQUARE:
	{
		Luminosity = (float)sinf((WorldTime) * 0.002f) * 0.35f + 0.65f;

		float Scale = 0.3f;
		Vector(Luminosity, Luminosity, Luminosity, Light);

		Vector(3.5f, -12.f, 10.f, p);
		b->TransformPosition(o->BoneTransform[20], p, Position, 1);
		CreateSprite(BITMAP_LIGHTNING + 1, Position, Scale, Light, o, (WorldTime / 50.0f));
		CreateSprite(BITMAP_LIGHTNING + 1, Position, Scale, Light, o, ((-WorldTime) / 50.0f));

		if (rand() % 30 == 0)
		{
			p[0] = Position[0] + rand() % 100 - 50;
			p[1] = Position[1] + rand() % 100 - 50;
			p[2] = Position[2] + rand() % 100 - 50;

			CreateJointSync(BITMAP_JOINT_ENERGY, p, Position, o->Angle, 6, NULL, 20.f);
		}
	}
	break;
	case MODEL_NPC_CASTEL_GATE:
	{
		vec3_t vPos, vRelative;
		float fLumi, fScale;

		fLumi = (sinf(WorldTime * 0.002f) + 2.0f) * 0.5f;
		Vector(fLumi * 1.0f, fLumi * 0.5f, fLumi * 0.3f, Light);
		fScale = fLumi / 2.0f;

		Vector(4.0f, 3.0f, -4.0f, vRelative);
		b->TransformPosition(o->BoneTransform[2], vRelative, vPos, true);

		CreateSprite(BITMAP_LIGHT, vPos, fScale, Light, o);
		CreateSprite(BITMAP_KANTURU_2ND_EFFECT1, vPos, fScale, Light, o);

		Vector(5.0f, 3.0f, 2.0f, vRelative);
		b->TransformPosition(o->BoneTransform[4], vRelative, vPos, true);

		CreateSprite(BITMAP_LIGHT, vPos, fScale, Light, o);
		CreateSprite(BITMAP_KANTURU_2ND_EFFECT1, vPos, fScale, Light, o);

		if (rand() % 4 == 0)
		{
			Vector(-20.0f, 10.0f, 0.0f, vRelative);
			b->TransformPosition(
				o->BoneTransform[3], vRelative, vPos, true);

			vec3_t Angle;
			VectorCopy(o->Angle, Angle);
			Angle[0] += 120.0f;
			Angle[2] -= 60.0f;

			Vector(1.0f, 1.0f, 1.0f, Light);

			CreateParticleSync(BITMAP_FLAME, vPos, Angle, Light, 10, o->Scale / 2);
		}

	}
	break;
	case MODEL_MONSTER01 + 28:
		Vector(0.f, 0.f, 0.f, p);
		Luminosity = 1.f;
		if (c->Level == 0)
		{
			Vector(Luminosity * 1.f, Luminosity * 1.f, Luminosity * 1.f, Light);
		}
		else
		{
			Vector(Luminosity * 1.f, Luminosity * 0.4f, Luminosity * 0.f, Light);
			Vector(Luminosity * 0.2f, Luminosity * 0.7f, Luminosity * 0.1f, Light);
		}

		for (int i = 0; i < b->NumBones; i++)
		{
			if (!b->Bones[i].Dummy)
			{
				if ((i >= 15 && i <= 20) || (i >= 27 && i <= 32))
				{
				}
				else
				{
					b->TransformPosition(o->BoneTransform[i], p, Position, true);
					if (c->Level == 0)
						CreateSprite(BITMAP_SHINY + 1, Position, 2.5f, Light, o, 0.f, 1);
					else
						CreateSprite(BITMAP_MAGIC + 1, Position, 0.8f, Light, o, 0.f);
					if (rand() % 4 == 0 && o->CurrentAction >= MONSTER01_ATTACK1 && o->CurrentAction <= MONSTER01_ATTACK2)
					{
						CreateParticleSync(BITMAP_ENERGY, Position, o->Angle, Light);
					}
					//CreateParticle(BITMAP_SMOKE,Position,o->Angle,Light,1);
				}
			}
		}
		break;
	case MODEL_SEED_MASTER:
	{
		float fLumi, fScale;
		fScale = 2.0f;
		b->TransformByObjectBone(Position, o, 39);

		fLumi = (sinf(WorldTime * 0.001f) + 1.0f) * 0.3f + 0.4f;
		Vector(fLumi * 0.5f, fLumi * 0.5f, fLumi * 0.5f, Light);
		CreateSprite(BITMAP_FLARE, Position, fScale, Light, o);
	}
	break;
	case MODEL_SEED_INVESTIGATOR:
	{
		float fLumi, fScale;
		fScale = 1.0f;
		for (int i = 69; i <= 70; ++i)
		{
			b->TransformByObjectBone(Position, o, i);

			fLumi = (sinf(WorldTime * 0.001f) + 1.0f) * 0.3f + 0.4f;
			Vector(fLumi * 0.5f, fLumi * 0.5f, fLumi * 0.5f, Light);
			CreateSprite(BITMAP_FLARE_BLUE, Position, fScale, Light, o);
		}
	}
	break;
	default:
	{
		if (RenderHellasMonsterVisual(c, o, b))
			break;
		if (battleCastle::RenderBattleCastleMonsterVisual(c, o, b))
			break;
		if (M31HuntingGround::RenderHuntingGroundMonsterVisual(c, o, b))
			break;
		if (M34CryingWolf2nd::RenderCryingWolf2ndMonsterVisual(c, o, b))
			break;
		if (M34CryWolf1st::RenderCryWolf1stMonsterVisual(c, o, b))
			break;
		if (M33Aida::RenderAidaMonsterVisual(c, o, b))
			break;
		if (M37Kanturu1st::RenderKanturu1stMonsterVisual(c, o, b))
			break;
		if (M38Kanturu2nd::Render_Kanturu2nd_MonsterVisual(c, o, b))
			break;
		if (M39Kanturu3rd::RenderKanturu3rdMonsterVisual(c, o, b))
			break;
		if (SEASON3A::CGM3rdChangeUp::Instance().RenderBalgasBarrackMonsterVisual(c, o, b))
			break;
		if (g_CursedTemple->RenderMonsterVisual(c, o, b))
			break;
		if (SEASON3B::GMNewTown::RenderMonsterVisual(c, o, b))
			break;
		if (SEASON3C::GMSwampOfQuiet::RenderMonsterVisual(c, o, b))
			break;
		if (TheMapProcess().RenderMonsterVisual(c, o, b) == true)
			break;
	}
	}

	if (SceneFlag == MAIN_SCENE)
	{
		if ((o->Kind == KIND_PLAYER
			&& gMapManager->InBloodCastle() == false
			&& gMapManager->InChaosCastle() == false
			&& gMapManager->InBattleCastle()) == false)
		{
			RenderProtectGuildMark(c);
		}
	}

	giPetManager::RenderPet(c);

	if (c->GetBaseClass() == CLASS_SUMMONER)
	{
		PART_t* w = &c->Weapon[1];
		g_SummonSystem.MoveEquipEffect(c, w->Type, w->Level, w->Option1);
	}
}

void RenderCharactersClient()
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		OBJECT* o = &c->Object;

		if (c != Hero && battleCastle::IsBattleCastleStart() == true && g_isCharacterBuff(o, eBuff_Cloaking))
		{

			if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK))
			{
				if (!(c->EtcPart == PARTS_ATTACK_KING_TEAM_MARK || c->EtcPart == PARTS_ATTACK_TEAM_MARK))
				{
					if (Hero->TargetCharacter == c->Key)
						Hero->TargetCharacter = -1;
					continue;
				}
			}
			else if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2 || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK2))
			{
				if (!(c->EtcPart == PARTS_ATTACK_KING_TEAM_MARK2 || c->EtcPart == PARTS_ATTACK_TEAM_MARK2))
				{
					if (Hero->TargetCharacter == c->Key)
						Hero->TargetCharacter = -1;
					continue;
				}
			}
			else if ((Hero->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3 || Hero->EtcPart == PARTS_ATTACK_TEAM_MARK3))
			{
				if (!(c->EtcPart == PARTS_ATTACK_KING_TEAM_MARK3 || c->EtcPart == PARTS_ATTACK_TEAM_MARK3))
				{
					if (Hero->TargetCharacter == c->Key)
						Hero->TargetCharacter = -1;
					continue;
				}
			}
			else if (Hero->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || Hero->EtcPart == PARTS_DEFENSE_TEAM_MARK)
			{
				if (!(c->EtcPart == PARTS_DEFENSE_KING_TEAM_MARK || c->EtcPart == PARTS_DEFENSE_TEAM_MARK))
				{
					if (Hero->TargetCharacter == c->Key)
						Hero->TargetCharacter = -1;
					continue;
				}
			}
		}
		if (c == Hero && (0x04 & Hero->CtlCode) && SceneFlag == MAIN_SCENE)
		{
			o->OBB.StartPos[0] = 1000.0000;
			o->OBB.XAxis[0] = 1.00000;
			o->OBB.XAxis[1] = 1.00000;
			o->OBB.XAxis[2] = 1.00000;
			continue;
		}
		if (o->Live)
		{
			if (o->Visible)
			{
				RenderCharacter(c, o, (i == SelectedCharacter || i == SelectedNpc));

				if (o->Type == MODEL_PLAYER)
					battleCastle::CreateBattleCastleCharacter_Visual(c, o);

			}
		}
	}

	if (gMapManager->InBattleCastle() || World == WD_31HUNTING_GROUND)
	{
		battleCastle::InitEtcSetting();
	}
}

void RenderProtectGuildMark(CHARACTER* c)
{
	if (c->GuildType == GT_ANGEL)
	{
		if (c->ProtectGuildMarkWorldTime == 0 || WorldTime - c->ProtectGuildMarkWorldTime > 5000.0f)
		{
			OBJECT* o = &c->Object;
			CreateEffect(MODEL_PROTECTGUILD, o->Position, o->Angle, o->Light, 0, o);
			c->ProtectGuildMarkWorldTime = WorldTime;
		}
	}
}

void ClearCharacters(int Key)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		OBJECT* o = &c->Object;

		if (o->Live && c->Key != Key)
		{
			o->Live = false;
			BoneManager::UnregisterBone(c);
			gGoboidManager->DeleteBug(o);
		}
		DeletePet(c);
		DeleteCloth(c, o);
		DeleteParts(c);
	}
}

void DeleteCharacter(int Key)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		OBJECT* o = &c->Object;

		if (o->Live && c->Key == Key)
		{
			o->Live = false;
			BoneManager::UnregisterBone(c);
			gGoboidManager->DeleteBug(o);
			DeletePet(c);
			DeleteCloth(c, o);
			DeleteParts(c);
			return;
		}
	}
}

void DeleteCharacter(CHARACTER* c, OBJECT* o)
{
	o->Live = false;

	BoneManager::UnregisterBone(c);
	gGoboidManager->DeleteBug(o);
	DeletePet(c);
	DeleteCloth(c, o);
	DeleteParts(c);
}

int FindCharacterIndex(int Key)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c && c->Object.Live && c->Key == Key)
		{
			return i;
		}
	}
	return MAX_CHARACTERS_CLIENT;
}

int FindCharacterIndexByMonsterIndex(int Type)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c && c->Object.Live && c->MonsterIndex == Type)
		{
			return i;
		}
	}
	return MAX_CHARACTERS_CLIENT;
}

int HangerBloodCastleQuestItem(int Key)
{
	int index = MAX_CHARACTERS_CLIENT;
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c->Object.Live && c->Key == Key)
		{
			index = i;
		}
		c->EtcPart = 0;
	}
	return index;
}

void SetAllAction(int Action)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c && c->Object.Live && c->Object.Type == MODEL_PLAYER)
		{
			Vector(0.f, 0.f, 180.f, c->Object.Angle);
			SetAction(&c->Object, Action);
		}
	}
}

void ReleaseCharacters(void)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		OBJECT* o = &c->Object;

		if (o->BoneTransform != NULL)
		{
			delete[] o->BoneTransform;
			o->BoneTransform = NULL;
		}
		DeletePet(c);
		DeleteCloth(c, o);
		DeleteParts(c);
	}

	OBJECT* o = &CharacterView.Object;

	if (o->BoneTransform != NULL)
	{
		delete[] o->BoneTransform;
		o->BoneTransform = NULL;
	}
	DeletePet(&CharacterView);
	DeleteCloth(&CharacterView, o);
	DeleteParts(&CharacterView);

	BoneManager::UnregisterAll();
}

void CreateCharacterPointer(CHARACTER* c, int Type, unsigned char PositionX, unsigned char PositionY, float Rotation)
{
	OBJECT* o = &c->Object;
	c->PositionX = PositionX;
	c->PositionY = PositionY;
	c->TargetX = PositionX;
	c->TargetY = PositionY;

	if (c != Hero)
	{
		c->byExtensionSkill = 0;
	}
	c->m_pParts = NULL;

	giPetManager::DeletePet(c);

	int Index = TERRAIN_INDEX_REPEAT((c->PositionX), (c->PositionY));
	if ((TerrainWall[Index] & TW_SAFEZONE) == TW_SAFEZONE)
		c->SafeZone = true;
	else
		c->SafeZone = false;

	c->Path.PathNum = 0;
	c->Path.CurrentPath = 0;
	c->Movement = false;
	o->Live = true;
	o->Visible = false;
	o->AlphaEnable = true;
	o->LightEnable = true;
	o->ContrastEnable = false;
	o->EnableBoneMatrix = true;
	o->EnableShadow = false;
	c->Dead = 0;
	c->Blood = false;
	c->GuildTeam = 0;
	c->Run = 0;
	c->GuildMarkIndex = -1;
	c->PK = PVP_NEUTRAL;
	o->Type = Type;
	o->Scale = 0.9f;
	o->Timer = 0.f;
	o->Alpha = 1.f;
	o->AlphaTarget = 1.f;
	o->Velocity = 0.f;
	o->ShadowScale = 0.f;
	o->m_byHurtByOneToOne = 0;
	o->AI = 0;
	o->m_byBuildTime = 10;
	c->m_iDeleteTime = -128;
	o->m_bRenderShadow = true;
	o->m_fEdgeScale = 1.2f;
	c->m_bIsSelected = true;
	c->ExtendState = 0;
	c->ExtendStateTime = (float)(100) / REFERENCE_FPS;
	c->m_byGensInfluence = 0;
	c->GuildStatus = -1;
	c->GuildType = 0;
	c->ProtectGuildMarkWorldTime = 0.0f;
	c->GuildRelationShip = 0;
	c->GuildSkill = AT_SKILL_STUN;
	c->BackupCurrentSkill = 255;
	c->GuildMasterKillCount = 0;
	c->m_byDieType = 0;
	o->m_bActionStart = false;
	o->m_bySkillCount = 0;
	c->m_bFixForm = false;
	c->CtlCode = 0;
	c->m_CursedTempleCurSkill = AT_SKILL_CURSED_TEMPLE_PRODECTION;
	c->m_CursedTempleCurSkillPacket = false;

	o->movementType = Movement::None;

	if (Type<MODEL_FACE || Type>MODEL_FACE + 6)
	{
		c->Class = 0;
	}

	if (Type == MODEL_PLAYER)
	{
		o->PriorAction = 1;
		o->CurrentAction = 1;
	}
	else
	{
		o->PriorAction = 0;
		o->CurrentAction = 0;
	}
	o->AnimationFrame = 0.f;
	o->PriorAnimationFrame = 0;
	c->JumpTime = 0;
	o->HiddenMesh = -1;
	c->MoveSpeed = 10;

	g_CharacterClearBuff(o);

	o->Teleport = TELEPORT_NONE;
	o->Kind = KIND_PLAYER;
	c->Change = false;
	o->SubType = 0;
	c->MonsterIndex = -1;
	o->BlendMeshTexCoordU = 0.f;
	o->BlendMeshTexCoordV = 0.f;
	c->Skill = 0;
	c->AttackTime = (0);
	c->TargetCharacter = -1;
	c->AttackFlag = ATTACK_FAIL;
	c->Weapon[0].Type = -1;
	c->Weapon[0].Level = 0;
	c->Weapon[1].Type = -1;
	c->Weapon[1].Level = 0;
	c->Wing.Type = -1;
	c->Wing.ClothType = 0;
	c->Helper.Type = -1;

	c->MuunHelper[0].Type = -1;
	c->MuunHelper[1].Type = -1;

	o->Position[0] = (float)((c->PositionX) * TERRAIN_SCALE) + 0.5f * TERRAIN_SCALE;
	o->Position[1] = (float)((c->PositionY) * TERRAIN_SCALE) + 0.5f * TERRAIN_SCALE;

	o->InitialSceneFrame = MoveSceneFrame;

	if (World == -1 || c->Helper.Type != MODEL_HELPER + 3 || c->SafeZone)
	{
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]);
	}
	else if (World == WD_8TARKAN || World == WD_10HEAVEN)
	{
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 90.f;
	}
	else
	{
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 30.f;
	}

	Vector(0.f, 0.f, Rotation, o->Angle);
	Vector(0.f, 0.f, 0.f, c->Light);
	Vector(0.f, 0.f, 0.f, o->Light);
	Vector(-60.f, -60.f, 0.f, o->BoundingBoxMin);
	switch (Type)
	{
	case MODEL_PLAYER:
		Vector(40.f, 40.f, 120.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 70:
	case MODEL_MONSTER01 + 71:
	case MODEL_MONSTER01 + 72:
		Vector(40.f, 40.f, 120.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 2:
	case MODEL_MONSTER01 + 6:
	case MODEL_MONSTER01 + 9:
	case MODEL_MONSTER01 + 20:
	case MODEL_MONSTER01 + 19:
	case MODEL_MONSTER01 + 17:
		Vector(50.f, 50.f, 80.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 11:
	case MODEL_MONSTER01 + 31:
	case MODEL_MONSTER01 + 39:
	case MODEL_MONSTER01 + 42:
	case MODEL_MONSTER01 + 44:
		Vector(70.f, 70.f, 250.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 37:
		Vector(100.f, 100.f, 150.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 61:
		Vector(-120.f, -120.f, 0.f, o->BoundingBoxMin);
		Vector(100.f, 100.f, 300.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 60:
		Vector(-90.f, -50.f, 0.f, o->BoundingBoxMin);
		Vector(90.f, 50.f, 200.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 150:
		Vector(-150.f, -150.f, 0.f, o->BoundingBoxMin);
		Vector(150.f, 150.f, 400.f, o->BoundingBoxMax);
		break;
	case MODEL_MONSTER01 + 151:
	case MODEL_MONSTER01 + 152:
	case MODEL_MONSTER01 + 153:
		Vector(-100.f, -100.f, 0.f, o->BoundingBoxMin);
		Vector(100.f, 100.f, 200.f, o->BoundingBoxMax);
		break;
	case MODEL_LITTLESANTA:
	case MODEL_LITTLESANTA + 1:
	case MODEL_LITTLESANTA + 2:
	case MODEL_LITTLESANTA + 3:
	case MODEL_LITTLESANTA + 4:
	case MODEL_LITTLESANTA + 5:
	case MODEL_LITTLESANTA + 6:
	case MODEL_LITTLESANTA + 7:
	{
		Vector(-160.f, -60.f, -20.f, o->BoundingBoxMin);
		Vector(10.f, 80.f, 50.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 157:
	{
		Vector(-100.f, -70.f, 0.f, o->BoundingBoxMin);
		Vector(100.f, 70.f, 150.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 158:
	{
		Vector(-100.f, -100.f, 50.f, o->BoundingBoxMin);
		Vector(100.f, 100.f, 150.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 159:
	{
		Vector(-100.f, -100.f, 0.f, o->BoundingBoxMin);
		Vector(100.f, 100.f, 180.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 160:
	{
		Vector(-80.f, -80.f, 0.f, o->BoundingBoxMin);
		Vector(80.f, 80.f, 130.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 161:
	{
		Vector(-80.f, -80.f, 0.f, o->BoundingBoxMin);
		Vector(80.f, 80.f, 130.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 162:
	{
		Vector(-100.f, -80.f, 50.f, o->BoundingBoxMin);
		Vector(100.f, 70.f, 280.f, o->BoundingBoxMax);
	}
	break;
	case MODEL_MONSTER01 + 163:
	{
		Vector(-100.f, -80.f, 50.f, o->BoundingBoxMin);
		Vector(100.f, 70.f, 280.f, o->BoundingBoxMax);
	}
	break;
	default:
		Vector(50.f, 50.f, 150.f, o->BoundingBoxMax);
		break;
	}

	BMD* pModel = gmClientModels->GetModel(Type);

	if (pModel)
	{
		o->CreateBoneTransform(pModel->NumBones);
	}

	for (int i = 0; i < 2; i++)
	{
		c->Weapon[i].Type = -1;
		c->Weapon[i].Level = 0;
		c->Weapon[i].Option1 = 0;
	}

	for (int i = 0; i < MAX_BODYPART; i++)
	{
		c->BodyPart[i].Type = -1;
		c->BodyPart[i].Level = 0;
		c->BodyPart[i].Option1 = 0;
		c->BodyPart[i].ExtOption = 0;
	}
	c->Wing.Type = -1;
	c->Wing.ClothType = 0;
	c->Helper.Type = -1;
	c->Flag.Type = -1;
#ifdef SHUTDOWN_CORE_WING4
	c->Core.Type = -1;
#endif // SHUTDOWN_CORE_WING4
	c->MuunHelper[0].Type = -1;
	c->MuunHelper[1].Type = -1;

	c->LongRangeAttack = -1;
	c->CollisionTime = 0;
	o->CollisionRange = 200.f;
	c->Rot = 0.f;
	c->Level = 0;
	c->Item = -1;

	for (int i = 0; i < 32; ++i) c->OwnerID[i] = 0;

	o->BlendMesh = -1;
	o->BlendMeshLight = 1.f;
	switch (Type)
	{
	case MODEL_MONSTER01 + 70:
	case MODEL_MONSTER01 + 71:
	case MODEL_MONSTER01 + 72:
		c->Weapon[0].LinkBone = 33;
		c->Weapon[1].LinkBone = 42;
		break;
	case MODEL_MONSTER01 + 57:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 59:
		c->Weapon[0].LinkBone = 33;
		c->Weapon[1].LinkBone = 20;
		break;
	case MODEL_MONSTER01 + 60:
		c->Weapon[0].LinkBone = 1;
		c->Weapon[1].LinkBone = 1;
		break;
	case MODEL_MONSTER01 + 55:
		c->Weapon[0].LinkBone = 27;
		c->Weapon[1].LinkBone = 18;
		break;
	case MODEL_MONSTER01 + 52:
		c->Weapon[0].LinkBone = 36;
		c->Weapon[1].LinkBone = 45;
		break;
	case MODEL_MONSTER01 + 53:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 46:
		c->Weapon[0].LinkBone = 39;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 47:
		c->Weapon[0].LinkBone = 27;
		c->Weapon[1].LinkBone = 38;
		break;
	case MODEL_MONSTER01 + 48:
		c->Weapon[0].LinkBone = 32;
		c->Weapon[1].LinkBone = 43;
		break;
	case MODEL_MONSTER01 + 44:
		c->Weapon[0].LinkBone = 55;
		c->Weapon[1].LinkBone = 70;
		break;
	case MODEL_MONSTER01 + 42:
		c->Weapon[0].LinkBone = 43;
		break;
	case MODEL_MONSTER01 + 41:
		c->Weapon[0].LinkBone = 23;
		break;
	case MODEL_MONSTER01 + 36:
		c->Weapon[0].LinkBone = 52;
		c->Weapon[1].LinkBone = 65;
		break;
	case MODEL_MONSTER01 + 35:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 34:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 26:
		c->Weapon[0].LinkBone = 16;
		c->Weapon[1].LinkBone = 25;
		break;
	case MODEL_MONSTER01 + 29:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 27:
		c->Weapon[0].LinkBone = 17;
		c->Weapon[1].LinkBone = 28;
		break;
	case MODEL_MONSTER01 + 24:
		c->Weapon[0].LinkBone = 39;
		c->Weapon[1].LinkBone = 30;
		break;
	case MODEL_MONSTER01 + 22:
		c->Weapon[0].LinkBone = 25;
		c->Weapon[1].LinkBone = 16;
		break;
	case MODEL_MONSTER01 + 21:
		c->Weapon[0].LinkBone = 24;
		c->Weapon[1].LinkBone = 19;
		break;
	case MODEL_MONSTER01 + 19:
		c->Weapon[0].LinkBone = 31;
		c->Weapon[1].LinkBone = 22;
		break;
	case MODEL_MONSTER01 + 18:
		c->Weapon[0].LinkBone = 26;
		c->Weapon[1].LinkBone = 35;
		break;
	case MODEL_MONSTER01 + 16:
	case MODEL_MONSTER01 + 11:
		c->Weapon[0].LinkBone = 30;
		c->Weapon[1].LinkBone = 39;
		break;
	case MODEL_MONSTER01 + 3:
		c->Weapon[0].LinkBone = 26;
		c->Weapon[1].LinkBone = 36;
		break;
	case MODEL_MONSTER01:
	case MODEL_MONSTER01 + 30:
		c->Weapon[0].LinkBone = 42;
		c->Weapon[1].LinkBone = 33;
		break;
	case MODEL_MONSTER01 + 10:
	case MODEL_MONSTER01 + 4:
	case MODEL_MONSTER01 + 5:
		c->Weapon[0].LinkBone = 41;
		c->Weapon[1].LinkBone = 32;
		break;
	case MODEL_MONSTER01 + 1:
		c->Weapon[0].LinkBone = 19;
		c->Weapon[1].LinkBone = 14;
		break;
	case MODEL_MONSTER01 + 8:
		c->Weapon[0].LinkBone = 29;
		c->Weapon[1].LinkBone = 38;
		break;
	case MODEL_MONSTER01 + 40:
		c->Weapon[0].LinkBone = 20;
		c->Weapon[1].LinkBone = 33;
		break;
	default:
		if (SettingHellasMonsterLinkBone(c, Type))
			return;
		if (battleCastle::SettingBattleCastleMonsterLinkBone(c, Type))
			return;
		c->Weapon[0].LinkBone = 33;
		c->Weapon[1].LinkBone = 42;
		break;
	}
}

CHARACTER* CreateCharacter(int Key, int Type, unsigned char PositionX, unsigned char PositionY, float Rotation)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (c->Object.Live && c->Key == Key)
		{
			BoneManager::UnregisterBone(c);
			DeleteCloth(c, (&c->Object));
			DeleteParts(c);
			CreateCharacterPointer(c, Type, PositionX, PositionY, Rotation);

			UnRegisterBuff(eBuff_PhysDefense, &c->Object);
			g_CharacterClearBuff((&c->Object));
			return c;
		}
	}

	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);

		if (!c->Object.Live)
		{
			CreateCharacterPointer(c, Type, PositionX, PositionY, Rotation);
			UnRegisterBuff(eBuff_PhysDefense, &c->Object);
			g_CharacterClearBuff((&c->Object));
			c->Key = Key;
			return c;
		}
	}

	return gmCharacters->GetDummyCharacter();
}

void SetCharacterScale(CHARACTER* c)
{
	if (c->Change)
		return;

	if (c->BodyPart[BODYPART_HELM].Type == MODEL_HELM ||
		c->BodyPart[BODYPART_HELM].Type == MODEL_HELM + 2 ||
		c->BodyPart[BODYPART_HELM].Type == MODEL_HELM + 63 ||
		c->BodyPart[BODYPART_HELM].Type == MODEL_HELM + 68 ||
		c->BodyPart[BODYPART_HELM].Type == MODEL_HELM + 65 ||
		c->BodyPart[BODYPART_HELM].Type == MODEL_HELM + 70 ||
		(c->BodyPart[BODYPART_HELM].Type >= MODEL_HELM + 10 && c->BodyPart[BODYPART_HELM].Type <= MODEL_HELM + 13))
	{
		c->BodyPart[BODYPART_HEAD].Type = MODEL_BODY_HELM + gCharacterManager.GetSkinModelIndex(c->Class);
	}
	else
	{
		c->BodyPart[BODYPART_HEAD].Type = -1;
	}

	if (SceneFlag == CHARACTER_SCENE && (gmProtect->SceneCharacter == 0 || gmProtect->SceneCharacter == 4))
	{
		switch (c->GetBaseClass())
		{
		case CLASS_WIZARD:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_KNIGHT:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_ELF:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_DARK:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_DARK_LORD:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_SUMMONER:
			c->Object.Scale = 1.2f;
			break;
		case CLASS_RAGEFIGHTER:
			c->Object.Scale = 1.35f;
			break;
		}

		if (gmProtect->SceneCharacter == 4)
		{
			c->Object.Scale *= 1.3;
		}
	}
	else
	{
		if (c->Skin == 0)
		{
			switch (c->GetBaseClass())
			{
			case CLASS_WIZARD:
				c->Object.Scale = 0.9f;
				break;
			case CLASS_KNIGHT:
				c->Object.Scale = 0.9f;
				break;
			case CLASS_ELF:
				c->Object.Scale = 0.88f;
				break;
			case CLASS_DARK:
				c->Object.Scale = 0.95f;
				break;
			case CLASS_DARK_LORD:
				c->Object.Scale = 0.92f;
				break;
			case CLASS_SUMMONER:
				c->Object.Scale = 0.90f;
				break;
			case CLASS_RAGEFIGHTER:
				c->Object.Scale = 1.03f;
				break;
			}
		}
		else
		{
			switch (c->GetBaseClass())
			{
			case CLASS_WIZARD:
				c->Object.Scale = 0.93f;
				break;
			case CLASS_KNIGHT:
				c->Object.Scale = 0.93f;
				break;
			case CLASS_ELF:
				c->Object.Scale = 0.86f;
				break;
			case CLASS_DARK:
				c->Object.Scale = 0.95f;
				break;
			case CLASS_DARK_LORD:
				c->Object.Scale = 0.92f;
				break;
			case CLASS_SUMMONER:
				c->Object.Scale = 0.90f;
				break;
			case CLASS_RAGEFIGHTER:
				c->Object.Scale = 1.03f;
				break;
			}
		}
	}
}

void SetCharacterClass(CHARACTER* c)
{
	if (c->Object.Type != MODEL_PLAYER)
	{
		return;
	}

	ITEM* p = CharacterMachine->Equipment;

	if (p[EQUIPMENT_WEAPON_RIGHT].Type == -1)
		c->Weapon[0].Type = -1;
	else
		c->Weapon[0].Type = p[EQUIPMENT_WEAPON_RIGHT].Type + MODEL_ITEM;

	if (p[EQUIPMENT_WEAPON_LEFT].Type == -1)
		c->Weapon[1].Type = -1;
	else
		c->Weapon[1].Type = p[EQUIPMENT_WEAPON_LEFT].Type + MODEL_ITEM;

	if (p[EQUIPMENT_WING].Type == -1)
	{
		c->Wing.Type = -1;
		c->Wing.ClothType = 0;
	}
	else
	{
		c->Wing.Type = p[EQUIPMENT_WING].Type + MODEL_ITEM;
		c->Wing.ClothType = 0;
	}

	if (p[EQUIPMENT_HELPER].Type == -1)
		c->Helper.Type = -1;
	else
		c->Helper.Type = p[EQUIPMENT_HELPER].Type + MODEL_ITEM;

	if (p[EQUIPMENT_MUUN].Type == -1)
		c->MuunHelper[0].Type = -1;
	else
		c->MuunHelper[0].Type = p[EQUIPMENT_MUUN].Type + MODEL_ITEM;

	if (p[EQUIPMENT_EAGLE].Type == -1)
		c->MuunHelper[1].Type = -1;
	else
		c->MuunHelper[1].Type = p[EQUIPMENT_EAGLE].Type + MODEL_ITEM;

	c->Weapon[0].Level = (p[EQUIPMENT_WEAPON_RIGHT].Level >> 3) & 15;
	c->Weapon[0].Option1 = p[EQUIPMENT_WEAPON_RIGHT].Option1;
	c->Weapon[0].ExtOption = p[EQUIPMENT_WEAPON_RIGHT].ExtOption;

	c->Weapon[1].Level = (p[EQUIPMENT_WEAPON_LEFT].Level >> 3) & 15;
	c->Weapon[1].Option1 = p[EQUIPMENT_WEAPON_LEFT].Option1;
	c->Weapon[1].ExtOption = p[EQUIPMENT_WEAPON_LEFT].ExtOption;

	c->Wing.Level = (p[EQUIPMENT_WING].Level >> 3) & 15;
	c->Helper.Level = (p[EQUIPMENT_HELPER].Level >> 3) & 15;

	bool Success = true;

	if (gMapManager->InChaosCastle() == true)
		Success = false;

	if (c->Object.CurrentAction >= PLAYER_SIT1 && c->Object.CurrentAction <= PLAYER_POSE_FEMALE1)
	{
		Success = false;
	}
	if (c->Object.CurrentAction >= PLAYER_HIT_FIST && c->Object.CurrentAction <= PLAYER_RIDE_SKILL)
	{
		Success = false;
	}

	if (Success)
	{
		SetPlayerStop(c);
	}

	BYTE SkinModelIndex = gCharacterManager.GetSkinModelIndex(c->Class);

	if (p[EQUIPMENT_HELM].Type == -1)
		c->BodyPart[BODYPART_HELM].Type = MODEL_BODY_HELM + SkinModelIndex;
	else
		c->BodyPart[BODYPART_HELM].Type = p[EQUIPMENT_HELM].Type + MODEL_ITEM;


	if (p[EQUIPMENT_ARMOR].Type == -1)
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_BODY_ARMOR + SkinModelIndex;
	else
		c->BodyPart[BODYPART_ARMOR].Type = p[EQUIPMENT_ARMOR].Type + MODEL_ITEM;


	if (p[EQUIPMENT_PANTS].Type == -1)
		c->BodyPart[BODYPART_PANTS].Type = MODEL_BODY_PANTS + SkinModelIndex;
	else
		c->BodyPart[BODYPART_PANTS].Type = p[EQUIPMENT_PANTS].Type + MODEL_ITEM;


	if (p[EQUIPMENT_GLOVES].Type == -1)
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_BODY_GLOVES + SkinModelIndex;
	else
		c->BodyPart[BODYPART_GLOVES].Type = p[EQUIPMENT_GLOVES].Type + MODEL_ITEM;


	if (p[EQUIPMENT_BOOTS].Type == -1)
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BODY_BOOTS + SkinModelIndex;
	else
		c->BodyPart[BODYPART_BOOTS].Type = p[EQUIPMENT_BOOTS].Type + MODEL_ITEM;

#ifdef SHUTDOWN_CORE_WING4
	if (p[EQUIPMENT_WING_CORE].Type == -1)
		c->Core.Type = -1;
	else
		c->Core.Type = p[EQUIPMENT_WING_CORE].Type + MODEL_ITEM;
#endif // SHUTDOWN_CORE_WING4

#ifdef SYSTEM_FLAG_NAT
	if (p[EQUIPMENT_FLAG_NAT].Type == -1)
		c->Flag.Type = -1;
	else
		c->Flag.Type = p[EQUIPMENT_FLAG_NAT].Type + MODEL_ITEM;
#endif // SYSTEM_FLAG_NAT


	c->BodyPart[BODYPART_HELM].Level = (p[EQUIPMENT_HELM].Level >> 3) & 15;
	c->BodyPart[BODYPART_ARMOR].Level = (p[EQUIPMENT_ARMOR].Level >> 3) & 15;
	c->BodyPart[BODYPART_PANTS].Level = (p[EQUIPMENT_PANTS].Level >> 3) & 15;
	c->BodyPart[BODYPART_GLOVES].Level = (p[EQUIPMENT_GLOVES].Level >> 3) & 15;
	c->BodyPart[BODYPART_BOOTS].Level = (p[EQUIPMENT_BOOTS].Level >> 3) & 15;
	c->BodyPart[BODYPART_HELM].Option1 = p[EQUIPMENT_HELM].Option1;
	c->BodyPart[BODYPART_ARMOR].Option1 = p[EQUIPMENT_ARMOR].Option1;
	c->BodyPart[BODYPART_PANTS].Option1 = p[EQUIPMENT_PANTS].Option1;
	c->BodyPart[BODYPART_GLOVES].Option1 = p[EQUIPMENT_GLOVES].Option1;
	c->BodyPart[BODYPART_BOOTS].Option1 = p[EQUIPMENT_BOOTS].Option1;
	c->BodyPart[BODYPART_HELM].ExtOption = p[EQUIPMENT_HELM].ExtOption;
	c->BodyPart[BODYPART_ARMOR].ExtOption = p[EQUIPMENT_ARMOR].ExtOption;
	c->BodyPart[BODYPART_PANTS].ExtOption = p[EQUIPMENT_PANTS].ExtOption;
	c->BodyPart[BODYPART_GLOVES].ExtOption = p[EQUIPMENT_GLOVES].ExtOption;
	c->BodyPart[BODYPART_BOOTS].ExtOption = p[EQUIPMENT_BOOTS].ExtOption;

	ChangeChaosCastleUnit(c);

	SetCharacterScale(c);

	if (c == Hero)
	{
		CheckFullSet(Hero);
	}

	CharacterMachine->CalculateAll();
}

void SetChangeClass(CHARACTER* c)
{
	if (c->Object.Type != MODEL_PLAYER)
		return;

	bool Success = true;

	if (c->Object.CurrentAction >= PLAYER_SIT1 && c->Object.CurrentAction <= PLAYER_POSE_FEMALE1)
		Success = false;
	if (c->Object.CurrentAction >= PLAYER_HIT_FIST && c->Object.CurrentAction <= PLAYER_RIDE_SKILL)
		Success = false;

	if (Success)
	{
		SetPlayerStop(c);
	}

	int iSkinModelIndex = gCharacterManager.GetSkinModelIndex(c->Class);
	if (c->BodyPart[BODYPART_HELM].Type >= MODEL_BODY_HELM)
	{
		c->BodyPart[BODYPART_HELM].Type = MODEL_BODY_HELM + iSkinModelIndex;
		c->BodyPart[BODYPART_HELM].Level = 0;
		c->BodyPart[BODYPART_HELM].Option1 = 0;
		c->BodyPart[BODYPART_HELM].ExtOption = 0;
	}
	if (c->BodyPart[BODYPART_ARMOR].Type >= MODEL_BODY_ARMOR)
	{
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_BODY_ARMOR + iSkinModelIndex;
		c->BodyPart[BODYPART_ARMOR].Level = 0;
		c->BodyPart[BODYPART_ARMOR].Option1 = 0;
		c->BodyPart[BODYPART_ARMOR].ExtOption = 0;
	}
	if (c->BodyPart[BODYPART_PANTS].Type >= MODEL_BODY_PANTS)
	{
		c->BodyPart[BODYPART_PANTS].Type = MODEL_BODY_PANTS + iSkinModelIndex;
		c->BodyPart[BODYPART_PANTS].Level = 0;
		c->BodyPart[BODYPART_PANTS].Option1 = 0;
		c->BodyPart[BODYPART_PANTS].ExtOption = 0;
	}
	if (c->BodyPart[BODYPART_GLOVES].Type >= MODEL_BODY_GLOVES)
	{
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_BODY_GLOVES + iSkinModelIndex;
		c->BodyPart[BODYPART_GLOVES].Level = 0;
		c->BodyPart[BODYPART_GLOVES].Option1 = 0;
		c->BodyPart[BODYPART_GLOVES].ExtOption = 0;
	}
	if (c->BodyPart[BODYPART_BOOTS].Type >= MODEL_BODY_BOOTS)
	{
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BODY_BOOTS + iSkinModelIndex;
		c->BodyPart[BODYPART_BOOTS].Level = 0;
		c->BodyPart[BODYPART_BOOTS].Option1 = 0;
		c->BodyPart[BODYPART_BOOTS].ExtOption = 0;
	}

	SetCharacterScale(c);
}

DWORD GetGuildRelationShipTextColor(BYTE GuildRelationShip)
{
	DWORD dwColor = 0;

	if (GuildRelationShip == GR_NONE)
		dwColor = (255 << 24) + (255 << 16) + (230 << 8) + (230);
	else if (GuildRelationShip == GR_RIVAL || GuildRelationShip == GR_RIVALUNION)
		dwColor = (255 << 24) + (0 << 16) + (30 << 8) + (255);
	else
		dwColor = (255 << 24) + (0 << 16) + (255 << 8) + (200);

	return dwColor;
}

DWORD GetGuildRelationShipBGColor(BYTE GuildRelationShip)
{
	DWORD dwColor = 0;

	if (GuildRelationShip == GR_NONE)
		dwColor = (150 << 24) + (50 << 16) + (30 << 8) + (10);
	else if (GuildRelationShip == GR_RIVAL || GuildRelationShip == GR_RIVALUNION)
		dwColor = (150 << 24) + (0 << 16) + (0 << 8) + (0);
	else
		dwColor = (150 << 24) + (80 << 16) + (50 << 8) + (20);

	return dwColor;
}

CHARACTER* FindCharacterByID(char* szName)
{
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; i++)
	{
		CHARACTER* c = gmCharacters->GetCharacter(i);
		if (c->Object.Live && !strcmp(szName, c->ID))
		{
			return c;
		}
	}
	return NULL;
}

int LevelConvert(BYTE Level)
{
	switch (Level)
	{
	case 0:
		return 0;
	case 1:
		return 3;
	case 2:
		return 5;
	case 3:
		return 7;
	case 4:
		return 9;
	case 5:
		return 11;
	case 6:
		return 13;
	case 7:
		return 15;
	}
	return 0;
}

void MakeElfHelper(CHARACTER* c)
{
	OBJECT* o = &c->Object;
	c->Wing.Type = MODEL_WING + 3;
	c->Wing.ClothType = 0;
	c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 24;
	c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 24;
	c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 24;
	c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 24;
	c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 24;
	c->BodyPart[BODYPART_HELM].Level = 13;
	c->BodyPart[BODYPART_ARMOR].Level = 13;
	c->BodyPart[BODYPART_PANTS].Level = 13;
	c->BodyPart[BODYPART_GLOVES].Level = 13;
	c->BodyPart[BODYPART_BOOTS].Level = 13;

	o->Scale = 1.f;
	o->CurrentAction = PLAYER_STOP_FLY;
	o->BoundingBoxMax[2] += 70.f;
}

void GetBytePart(int partIndex, PART_t* f, DWORD packedData, BYTE SkinModelIndex)
{
	WORD nType = (packedData >> 19) & 0x1FFF;        // 13 bits para Type
	f->Level = (packedData >> 15) & 0x0F;         // 4 bits para Level
	f->Option1 = (packedData >> 9) & 0x3F;        // 6 bits para Option1
	f->ExtOption = (packedData >> 7) & 0x03;      // 2 bits para ExtOption

	if (nType != 0x1FFF)
	{
		f->Type = nType + MODEL_ITEM;
	}
	else
	{
		if (partIndex >= EQUIPMENT_HELM && partIndex <= EQUIPMENT_BOOTS)
		{
			f->Type = SkinModelIndex + (MODEL_BODY_NUM * (partIndex - 2)) + MODEL_BODY_HELM;
		}
		else
		{
			f->Type = -1;
		}
	}
}

void ChangeCharacterExt(int Key, DWORD* Equipment, CHARACTER* pCharacter, OBJECT* pHelper)
{
	CHARACTER* c;
	if (pCharacter == NULL)
		c = gmCharacters->GetCharacter(Key);
	else
		c = pCharacter;

	OBJECT* o = &c->Object;
	if (o->Type != MODEL_PLAYER)
		return;

	BYTE SkinModelIndex = c->GetSkinModelIndex();


	//EQUIPMENT_MUUN = 237,
	//EQUIPMENT_EAGLE = 238,
	//EQUIPMENT_ERING_R = 239,
	//EQUIPMENT_ERING_L = 240,
	//EQUIPMENT_COSTUME = 241,
	//EQUIPMENT_FLAG_NAT = 242,
	//int _inv[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 237, 238, 241, 242, -1 };

	GetBytePart(0, &c->Weapon[0], Equipment[0], SkinModelIndex); //-- weapon1

	GetBytePart(1, &c->Weapon[1], Equipment[1], SkinModelIndex); //-- weapon2

	GetBytePart(2, &c->BodyPart[BODYPART_HELM], Equipment[2], SkinModelIndex); //-- helm

	GetBytePart(3, &c->BodyPart[BODYPART_ARMOR], Equipment[3], SkinModelIndex); //-- armor

	GetBytePart(4, &c->BodyPart[BODYPART_PANTS], Equipment[4], SkinModelIndex); //-- pants

	GetBytePart(5, &c->BodyPart[BODYPART_GLOVES], Equipment[5], SkinModelIndex); //-- gloves

	GetBytePart(6, &c->BodyPart[BODYPART_BOOTS], Equipment[6], SkinModelIndex); //-- boots

	GetBytePart(7, &c->Wing, Equipment[7], SkinModelIndex); //-- wings

	GetBytePart(8, &c->Helper, Equipment[8], SkinModelIndex); //-- helper

	GetBytePart(9, &c->MuunHelper[0], Equipment[9], SkinModelIndex); //-- helper

	GetBytePart(10, &c->MuunHelper[1], Equipment[10], SkinModelIndex); //-- helper

#ifdef SHUTDOWN_CORE_WING4
	GetBytePart(11, &c->Core, Equipment[11], SkinModelIndex);
#endif // SHUTDOWN_CORE_WING4

#ifdef SYSTEM_FLAG_NAT
	GetBytePart(12, &c->Flag, Equipment[12], 0);
#endif // SYSTEM_FLAG_NAT


	if (c->Weapon[1].Type == (MODEL_HELPER + 5))
	{
		giPetManager::CreatePetDarkSpirit(c);

		if (gMapManager->InChaosCastle() == false)
		{
			PET_INFO* pPetInfo = c->GetEquipedPetInfo(PET_TYPE_DARK_SPIRIT);
			((CSPetSystem*)c->m_pPet)->SetPetInfo(pPetInfo);
		}
	}

	if (pHelper == NULL)
	{
		gGoboidManager->DeleteBug(o);
		ThePetProcess().DeletePet(c, -1, true);
	}
	else
	{
		pHelper->Live = false;
	}

	if (c->Helper.Type != -1)
	{
		PART_t* f = &c->Helper;

		if (f->Type == MODEL_HELPER + 37)
		{
			switch (f->Option1)
			{
			case 0x01:
			{
				if (pHelper == NULL)
					gGoboidManager->CreateBug(MODEL_FENRIR_BLACK, o->Position, o);
				else
					CreateBugSub(MODEL_FENRIR_BLACK, o->Position, o, pHelper);
			}
			break;
			case 0x02:
			{
				if (pHelper == NULL)
					gGoboidManager->CreateBug(MODEL_FENRIR_BLUE, o->Position, o);
				else
					CreateBugSub(MODEL_FENRIR_BLUE, o->Position, o, pHelper);
			}
			break;
			case 0x04:
			{
				if (pHelper == NULL)
					gGoboidManager->CreateBug(MODEL_FENRIR_GOLD, o->Position, o);
				else
					CreateBugSub(MODEL_FENRIR_GOLD, o->Position, o, pHelper);
			}
			break;
			default:
			{
				if (pHelper == NULL)
					gGoboidManager->CreateBug(MODEL_FENRIR_RED, o->Position, o);
				else
					CreateBugSub(MODEL_FENRIR_RED, o->Position, o, pHelper);
			}
			break;
			}
		}
		else if (f->Type == MODEL_HELPER)
		{
			if (pHelper == NULL)
				gGoboidManager->CreateBug(MODEL_HELPER, o->Position, o);
			else
				CreateBugSub(MODEL_HELPER, o->Position, o, pHelper);
		}
		else if (f->Type == MODEL_HELPER + 2)
		{
			if (pHelper == NULL)
				gGoboidManager->CreateBug(MODEL_UNICON, o->Position, o);
			else
				CreateBugSub(MODEL_UNICON, o->Position, o, pHelper);
		}
		else if (f->Type == MODEL_HELPER + 3)
		{
			if (pHelper == NULL)
				gGoboidManager->CreateBug(MODEL_PEGASUS, o->Position, o);
			else
				CreateBugSub(MODEL_PEGASUS, o->Position, o, pHelper);
		}
		else if (f->Type == MODEL_HELPER + 4)
		{
			if (pHelper == NULL)
				gGoboidManager->CreateBug(MODEL_DARK_HORSE, o->Position, o);
			else
				CreateBugSub(MODEL_DARK_HORSE, o->Position, o, pHelper);
		}
		else
		{
			int nType = f->Type - MODEL_ITEM;

			if (ThePetProcess().IsPet(nType))
				ThePetProcess().CreatePet(nType, c->Helper.Type, o->Position, c);
			else
			{
				if (pHelper == NULL)
					CreateGoboId(nType, o->Position, o);
				else
					CreateSubGoboId(nType, o->Position, o, pHelper);
			}
		}
	}

	if (pHelper == NULL)
	{
		if (c->MuunHelper[0].Type != -1)
		{
			PART_t* f = &c->MuunHelper[0];
			int nType = f->Type - MODEL_ITEM;
			CreateGoboId(nType, o->Position, o, 0, 1);
		}
		if (c->MuunHelper[1].Type != -1)
		{
			PART_t* f = &c->MuunHelper[1];
			int nType = f->Type - MODEL_ITEM;
			CreateGoboId(nType, o->Position, o, 0, 2);
		}
	}

	c->Wing.Level = 0;
	c->Wing.Option1 = 0;
	c->Wing.ExtOption = 0;

	c->Helper.Level = 0;
	c->Helper.Option1 = 0;
	c->Helper.ExtOption = 0;

	c->MuunHelper[0].Level = 0;
	c->MuunHelper[0].Option1 = 0;
	c->MuunHelper[0].ExtOption = 0;

	c->MuunHelper[1].Level = 0;
	c->MuunHelper[1].Option1 = 0;
	c->MuunHelper[1].ExtOption = 0;


	c->ExtendState = (Equipment[EQUIPMENT_HELPER] >> 7) & 0x03;
	c->ExtendStateTime = (float)(100) / REFERENCE_FPS;

	ChangeChaosCastleUnit(c);

	SetCharacterScale(c);

	if (c == Hero)
	{
		CharacterMachine->CalculateAll();
	}
}

void Setting_Monster(CHARACTER* c, int Type, int PositionX, int PositionY)
{
	OBJECT* o;

	int nCastle = BLOODCASTLE_NUM + (World - WD_11BLOODCASTLE_END);
	if (nCastle > 0 && nCastle <= BLOODCASTLE_NUM)
	{
		if (Type >= 84 && Type <= 143)
		{
			c->Level = 0;
			c->Object.Scale += int(nCastle / 3) * 0.05f;
		}
	}

	if (c != NULL)
	{
		o = &c->Object;
		for (int i = 0; i < MAX_MONSTER; i++)
		{
			if (Type == MonsterScript[i].Type)
			{
				strcpy(c->ID, MonsterScript[i].Name);

				break;
			}
		}

		c->MonsterIndex = Type;
		c->Object.ExtState = 0;
		c->TargetCharacter = HeroIndex;
		if (Type == 200)
			o->Kind = KIND_MONSTER;
		else if (Type >= 260)
			o->Kind = KIND_MONSTER;
		else if (Type > 200)
			o->Kind = KIND_NPC;
		else if (Type >= 150)
			o->Kind = KIND_MONSTER;
		else if (Type > 110)
			o->Kind = KIND_MONSTER;
		else if (Type >= 100)
			o->Kind = KIND_TRAP;
		else
			o->Kind = KIND_MONSTER;
		//c->Object.Kind = KIND_EDIT;
		//sprintf(c->ID,"%x",Key);
		if (Type == 368 || Type == 369 || Type == 370)
			o->Kind = KIND_NPC;
		if (Type == 367
			|| Type == 371
			|| Type == 375
			|| Type == 376 || Type == 377
			|| Type == 379
			|| Type == 380 || Type == 381 || Type == 382
			|| Type == 383 || Type == 384 || Type == 385
			|| Type == 406
			|| Type == 407
			|| Type == 408
			|| Type == 414
			|| Type == 415 || Type == 416 || Type == 417
			|| Type == 450
			|| Type == 452 || Type == 453
			|| Type == 464
			|| Type == 465
			|| Type == 467
			|| Type == 468 || Type == 469 || Type == 470	//NPC 1~8
			|| Type == 471 || Type == 472 || Type == 473
			|| Type == 474 || Type == 475
			|| Type == 478
			|| Type == 479
			|| Type == 492
			|| Type == 540
			|| Type == 541
			|| Type == 542
			|| Type == 522
			|| Type == 543 || Type == 544
			|| Type == 545
			|| Type == 546
			|| Type == 547
			|| Type == 577 || Type == 578
			|| Type == 579
			)
		{
			o->Kind = KIND_NPC;
		}
		if (Type >= 480 && Type <= 491)
		{
			o->Kind = KIND_MONSTER;
		}
		if (Type == 451)
		{
			o->Kind = KIND_TMP;
		}

		if (o->Kind == KIND_NPC)
		{
			GMProtect->GetNpcName(Type, c->ID, PositionX, PositionY);
		}
	}
}

CHARACTER* CreateMonster(int Type, int PositionX, int PositionY, int Key)
{
	CHARACTER* c = NULL;
	OBJECT* o;
	int Level;

	c = g_CursedTemple->CreateCharacters(Type, PositionX, PositionY, Key);

	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}
	c = CreateHellasMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = battleCastle::CreateBattleCastleMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M31HuntingGround::CreateHuntingGroundMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M34CryingWolf2nd::CreateCryingWolf2ndMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M34CryWolf1st::CreateCryWolf1stMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M33Aida::CreateAidaMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M37Kanturu1st::CreateKanturu1stMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M38Kanturu2nd::Create_Kanturu2nd_Monster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = M39Kanturu3rd::CreateKanturu3rdMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = SEASON3A::CGM3rdChangeUp::Instance().CreateBalgasBarrackMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = g_NewYearsDayEvent->CreateMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = SEASON3B::GMNewTown::CreateNewTownMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = SEASON3C::GMSwampOfQuiet::CreateSwampOfQuietMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = g_09SummerEvent->CreateMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = TheMapProcess().CreateMonster(Type, PositionX, PositionY, Key);
	if (c != NULL)
	{
		Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}

	c = GMMonsterMng->CreateMonster(Type, PositionX, PositionY, Key);

	if (c != NULL)
	{
		//Setting_Monster(c, Type, PositionX, PositionY);
		return c;
	}


	switch (Type)
	{
	case 224:
		OpenNpc(MODEL_NPC_CLERK);        //  
		c = CreateCharacter(Key, MODEL_NPC_CLERK, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 1.f;
		c->Object.SubType = rand() % 2 + 10;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		strcpy(c->ID, "켥위병");
		break;
#ifdef ADD_ELF_SUMMON
	case 276:
		OpenMonsterModel(53);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 53, PositionX, PositionY);
		c->Object.Scale = 1.45f;
		c->Weapon[0].Type = MODEL_SWORD + 17;//MODEL_SWORD+15;
		c->Weapon[0].Level = 5;
		break;
#endif // ADD_ELF_SUMMON
	case 152:
	case 153:
	case 154:
	case 155:
	case 156:
	case 157:
	case 158:
		c = CreateCharacter(Key, MODEL_WARCRAFT, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Weapon[0].Type = -1;
		c->Weapon[0].Level = 0;
		c->Object.Scale = 1.f;
		c->HideShadow = false;
		o = &c->Object;
		o->PriorAnimationFrame = 10.f;
		o->AnimationFrame = 10;
		o->BlendMesh = -1;
		strcpy(c->ID, "");
		break;
	case 162:
	case 164:
	case 166:
	case 168:
	case 170:
	case 172:
	case 426:
	{
		OpenMonsterModel(70);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 70, PositionX, PositionY);
		c->Object.Scale = 0.9f;
		o = &c->Object;

		c->Weapon[0].Type = MODEL_SWORD + 16;
		c->Weapon[0].Level = 0;
		c->Weapon[1].Type = MODEL_SWORD + 16;
		c->Weapon[1].Level = 0;
	}
	break;

	case 163:
	case 165:
	case 167:
	case 169:
	case 171:
	case 173:
	case 427:
	{
		int randType = 0;

		randType = rand() % 2;

		OpenMonsterModel(71 + randType);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 71 + randType, PositionX, PositionY);
		c->Object.Scale = 0.9f;
		o = &c->Object;

		c->Weapon[0].Type = -1;
		c->Weapon[0].Level = 0;
		c->Weapon[1].Type = -1;
		c->Weapon[1].Level = 0;

		if (randType == 0)
		{
			c->Weapon[0].Type = MODEL_BOW + 19;
			c->Weapon[0].Level = 0;
		}
		else
		{
			c->Weapon[0].Type = MODEL_STAFF + 5;
			c->Weapon[0].Level = 0;
		}
	}
	break;
	case 89:
	case 95:
	case 112:
	case 118:
	case 124:
	case 130:
	case 143:
	case 433:
		OpenMonsterModel(62);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 62, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_STAFF;
		c->Weapon[0].Level = 11;
		c->Object.Scale = 1.2f;
		strcpy(c->ID, "마법해골");
		break;
	case 131:
		OpenMonsterModel(61);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 61, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 0.8f;
		c->Object.EnableShadow = false;
		strcpy(c->ID, "성문");
		break;
	case 132:
		OpenMonsterModel(60);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 60, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 0.8f;
		c->Object.EnableShadow = false;
		strcpy(c->ID, "성자의석관");
		break;
	case 133:
		OpenMonsterModel(60);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 60, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 0.8f;
		c->Object.EnableShadow = false;
		strcpy(c->ID, "성자의석관");
		break;
	case 134:
		OpenMonsterModel(60);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 60, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 0.8f;
		c->Object.EnableShadow = false;
		strcpy(c->ID, "성자의석관");
		break;
	case 84:
	case 90:
	case 96:
	case 113:
	case 119:
	case 125:
	case 138:
	case 428:
		OpenMonsterModel(47);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 47, PositionX, PositionY);
		c->Object.Scale = 1.1f;
		o = &c->Object;
		break;
	case 85:
	case 91:
	case 97:
	case 114:
	case 120:
	case 126:
	case 139:
	case 429:
		OpenMonsterModel(46);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 46, PositionX, PositionY);
		c->Object.Scale = 1.1f;
		c->Weapon[1].Type = MODEL_BOW + 3;
		c->Weapon[1].Level = 1;
		o = &c->Object;
		break;
	case 86:
	case 92:
	case 98:
	case 115:
	case 121:
	case 127:
	case 140:
	case 430:
		OpenMonsterModel(59);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 59, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_AXE + 8;
		c->Weapon[0].Level = 0;
		c->Weapon[1].Type = MODEL_AXE + 8;
		c->Weapon[1].Level = 0;
		c->Object.Scale = 1.0f;
		strcpy(c->ID, "흑해골전사");
		break;
	case 87:
	case 93:
	case 99:
	case 116:
	case 122:
	case 128:
	case 141:
	case 431:
		OpenMonsterModel(58);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 58, PositionX, PositionY);
		c->Object.Scale = 0.8f;
		strcpy(c->ID, "자이언트오우거");
		break;
	case 88:
	case 94:
	case 111:
	case 117:
	case 123:
	case 129:
	case 142:
	case 432:
		OpenMonsterModel(57);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 57, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_MACE + 6;

		if (!int((7 + (World - WD_11BLOODCASTLE_END)) / 3))
			c->Weapon[0].Level = 8;
		else
			c->Weapon[0].Level = 0;

		c->Object.Scale = 1.19f;
		strcpy(c->ID, "붉은해골기사");
		break;
	case 78:
		OpenMonsterModel(19);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 19, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_AXE;
		c->Weapon[0].Level = 9;
		c->Object.Scale = 0.8f;
		strcpy(c->ID, "고블린");
		break;
	case 79:
		OpenMonsterModel(31);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 31, PositionX, PositionY);
		strcpy(c->ID, "드래곤");
		c->Object.Scale = 0.9f;
		break;
	case 80:
		OpenMonsterModel(36);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 36, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		c->Weapon[0].Type = MODEL_STAFF + 7;
		c->Weapon[0].Option1 = 63;
		break;
	case 81:
		OpenMonsterModel(34);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 34, PositionX, PositionY);
		c->Object.Scale = 1.f;
		break;
	case 82:
		OpenMonsterModel(42);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 42, PositionX, PositionY);
		c->Object.BlendMesh = 2;
		c->Object.BlendMeshLight = 1.f;
		o = &c->Object;
		c->Object.Scale = 1.8f;
		c->Weapon[0].Type = MODEL_SWORD + 16;
		c->Weapon[0].Option1 = 63;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 83:
		OpenMonsterModel(41);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 41, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		c->Weapon[0].Type = MODEL_BOW + 14;
		c->Weapon[0].Option1 = 63;
		//c->Weapon[0].Type = MODEL_BOW+16;
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 68:
		OpenMonsterModel(49);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 49, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		break;

	case 69:
		OpenMonsterModel(50);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 50, PositionX, PositionY);
		c->Object.Scale = 1.f;
		c->Object.BlendMesh = 0;
		break;
	case 70:
		OpenMonsterModel(51);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 51, PositionX, PositionY);
		c->Object.Scale = 1.3f;
		c->Object.BlendMesh = -2;
		c->Object.BlendMeshLight = 1.f;
		c->Object.m_bRenderShadow = false;
		break;
	case 301:
	case 71:
	case 74:
		OpenMonsterModel(52);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 52, PositionX, PositionY);
		if (71 == Type)
		{
			c->Object.Scale = 1.1f;
			c->Weapon[0].Type = MODEL_SWORD + 18;
			c->Weapon[0].Level = 5;
			c->Weapon[1].Type = MODEL_SHIELD + 14;
			c->Weapon[1].Level = 0;
		}
		else
		{
			c->Object.Scale = 1.3f;
			c->Weapon[0].Type = MODEL_SWORD + 18;
			c->Weapon[0].Level = 9;
			c->Weapon[1].Type = MODEL_SHIELD + 14;
			c->Weapon[1].Level = 9;
		}
		c->Object.BlendMesh = 1;
		c->Object.BlendMeshLight = 1.f;
		//gmClientModels->GetModel(MODEL_MONSTER01+52)->StreamMesh = 1;
		break;

	case 72:
		OpenMonsterModel(53);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 53, PositionX, PositionY);
		c->Object.Scale = 1.45f;
		c->Weapon[0].Type = MODEL_SWORD + 17;//MODEL_SWORD+15;
		c->Weapon[0].Level = 5;
		break;

	case 73:
	case 75:
		OpenMonsterModel(54);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 54, PositionX, PositionY);
		c->m_bFixForm = true;
		if (Type == 75)
		{
			c->Object.Scale = 1.0f;
		}
		else
		{
			c->Object.Scale = 0.8f;
		}
		gmClientModels->GetModelMesh(c->Object.Type, 0)->NoneBlendMesh = true;
		gmClientModels->GetModelMesh(c->Object.Type, 1)->NoneBlendMesh = false;
		gmClientModels->GetModelMesh(c->Object.Type, 2)->NoneBlendMesh = false;
		gmClientModels->GetModelMesh(c->Object.Type, 3)->NoneBlendMesh = true;
		gmClientModels->GetModelMesh(c->Object.Type, 4)->NoneBlendMesh = true;
		break;
	case 77:
	{
		OpenMonsterModel(55);
		OpenMonsterModel(56);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 55, PositionX, PositionY);
		c->m_bFixForm = true;
		c->Object.Scale = 1.0f;
		gmClientModels->GetModel(MODEL_MONSTER01 + 55)->StreamMesh = 0;
	}
	break;
	case 64:
		OpenMonsterModel(46);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 46, PositionX, PositionY);
		c->Object.Scale = 1.2f;
		c->Weapon[1].Type = MODEL_BOW + 3;
		c->Weapon[1].Level = 3;
		o = &c->Object;
		o->HiddenMesh = 1;
		break;
	case 137:
		OpenMonsterModel(46);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 46, PositionX, PositionY);
		c->Object.Scale = 1.2f;
		c->Weapon[1].Type = MODEL_BOW + 3;
		c->Weapon[1].Level = 5;
		o = &c->Object;
		o->HiddenMesh = 1;
		break;
	case 65:
		OpenMonsterModel(47);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 47, PositionX, PositionY);
		c->Object.Scale = 1.3f;
		o = &c->Object;
		o->HiddenMesh = 2;
		break;
	case 136:
		OpenMonsterModel(47);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 47, PositionX, PositionY);
		c->Object.Scale = 1.3f;
		o = &c->Object;
		o->HiddenMesh = 2;
		break;
	case 66:
	case 135:
		OpenMonsterModel(48);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 48, PositionX, PositionY);
		c->Object.Scale = 1.7f;
		o = &c->Object;
		break;
	case 466:
		OpenMonsterModel(156);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 156, PositionX, PositionY);
		c->Object.Scale = 0.9f;
		strcpy(c->ID, "저주받은 고블린");
		o = &c->Object;
		break;
	case 476:
		OpenMonsterModel(155);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 155, PositionX, PositionY);
		c->Object.Scale = 1.7f;
		strcpy(c->ID, "저주받은 산타");
		o = &c->Object;
		break;
	case 300:
	case 62:
		OpenMonsterModel(45);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 45, PositionX, PositionY);
		c->Object.Scale = 1.5f;
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 63:
	case 61:
		OpenMonsterModel(44);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 44, PositionX, PositionY);
		if (Type == 63)
		{
			c->Object.Scale = 1.9f;
			c->Object.BlendMesh = -2;
			c->Object.BlendMeshLight = 1.f;
		}
		else
		{
			c->Object.Scale = 1.5f;
		}
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 60:
		OpenMonsterModel(43);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 43, PositionX, PositionY);
		c->Object.Scale = 2.2f;
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 58:
	case 59:
		OpenMonsterModel(42);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 42, PositionX, PositionY);
		c->Object.BlendMesh = 2;
		c->Object.BlendMeshLight = 1.f;
		o = &c->Object;
		if (Type == 58)
		{
			c->Object.Scale = 1.8f;
			c->Weapon[0].Type = MODEL_SWORD + 16;
		}
		else
		{
			c->Object.Scale = 2.1f;
			o->SubType = 1;
			c->Weapon[0].Type = MODEL_STAFF + 8;
		}
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 57:
		OpenMonsterModel(41);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 41, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		c->Weapon[0].Type = MODEL_BOW + 14;
		//c->Weapon[0].Type = MODEL_BOW+16;
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 52:
		OpenMonsterModel(35);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 35, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		c->Weapon[0].Type = MODEL_BOW + 13;
		break;
	case 51:
		OpenMonsterModel(33);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 33, PositionX, PositionY);
		c->Object.Scale = 1.f;
		c->Level = 1;
		break;
	case 50:
		OpenMonsterModel(38);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 38, PositionX, PositionY);
		c->Object.Scale = 1.8f;
		break;
	case 49:
		OpenMonsterModel(37);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 37, PositionX, PositionY);
		c->Object.Scale = 1.f;
		c->Object.BlendMesh = 5;
		c->Object.BlendMeshLight = 0.f;
		break;
	case 48:
		OpenMonsterModel(36);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 36, PositionX, PositionY);
		c->Object.Scale = 1.4f;
		c->Weapon[0].Type = MODEL_STAFF + 6;
		break;
	case 47:
		OpenMonsterModel(35);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 35, PositionX, PositionY);
		c->Object.Scale = 1.1f;
		c->Weapon[0].Type = MODEL_BOW + 13;
		c->Object.BlendMesh = 0;
		c->Object.BlendMeshLight = 1.f;
		break;
	case 46:
		OpenMonsterModel(34);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 34, PositionX, PositionY);
		c->Object.Scale = 1.f;
		break;
	case 45:
		OpenMonsterModel(33);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 33, PositionX, PositionY);
		c->Object.Scale = 0.6f;
		break;
	case 150:
		OpenMonsterModel(32);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 32, PositionX, PositionY);
		strcpy(c->ID, "발리");
		c->Object.Scale = 0.12f;
		break;
	case 44:
		OpenMonsterModel(31);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 31, PositionX, PositionY);
		strcpy(c->ID, "드래곤");
		c->Object.Scale = 0.9f;
		break;
	case 43:
		OpenMonsterModel(2);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 2, PositionX, PositionY);
		strcpy(c->ID, "황금버지드래곤");
		c->Object.Scale = 0.7f;
		break;
	case 42:
		OpenMonsterModel(31);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 31, PositionX, PositionY);
		strcpy(c->ID, "쿤둔");
		c->Object.Scale = 1.3f;
		Vector(200.f, 150.f, 280.f, c->Object.BoundingBoxMax);
		break;
	case 41:
		OpenMonsterModel(30);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 30, PositionX, PositionY);
		strcpy(c->ID, "데쓰 카우");
		c->Weapon[0].Type = MODEL_MACE + 3;
		//c->Weapon[0].Type = MODEL_SWORD+14;
		c->Object.Scale = 1.1f;
		//c->Level = 1;
		break;
	case 40:
		OpenMonsterModel(29);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 29, PositionX, PositionY);
		strcpy(c->ID, "데쓰 나이트");
		c->Weapon[0].Type = MODEL_SWORD + 15;
		c->Weapon[0].Type = MODEL_SWORD + 14;
		//c->Weapon[1].Type = MODEL_SHIELD+8;
		c->Object.Scale = 1.3f;
		//c->Level = 1;
		break;
	case 39:
		OpenMonsterModel(28);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 28, PositionX, PositionY);
		strcpy(c->ID, "포이즌 쉐도우");
		c->Object.Scale = 1.2f;
		c->Level = 1;
		break;
	case 38:
	case 67:	//발록2
		OpenMonsterModel(27);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 27, PositionX, PositionY);
		strcpy(c->ID, "발록");
		c->Weapon[0].Type = MODEL_SPEAR + 9;
		c->Weapon[0].Level = 9;
		c->Object.Scale = 1.6f;
		break;
	case 37:
		OpenMonsterModel(26);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 26, PositionX, PositionY);
		strcpy(c->ID, "데빌");
		c->Object.Scale = 1.1f;
		break;
	case 36:
		OpenMonsterModel(28);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 28, PositionX, PositionY);
		strcpy(c->ID, "쉐도우");
		c->Object.Scale = 1.2f;
		break;
		/*OpenMonsterModel(7);
		c = CreateCharacter(Key,MODEL_MONSTER01+7,PositionX,PositionY);
		strcpy(c->ID,"블러드 고스트");
		c->Object.AlphaTarget = 0.4f;
		c->MoveSpeed = 15;
		c->Blood = true;
		c->Object.Scale = 1.1f;
		c->Level = 2;*/
	case 35:
		OpenMonsterModel(11);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 11, PositionX, PositionY);
		strcpy(c->ID, "데쓰 고르곤");
		c->Object.Scale = 1.3f;
		c->Weapon[0].Type = MODEL_AXE + 8;
		c->Weapon[1].Type = MODEL_AXE + 8;
		c->Object.BlendMesh = 1;
		c->Object.BlendMeshLight = 1.f;
		c->Level = 2;
		break;
	case 34:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "저주받은 법사");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 3;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 3;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 3;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 3;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 3;
		c->Weapon[0].Type = MODEL_STAFF + 5;
		c->Weapon[1].Type = MODEL_SHIELD + 14;
		Level = 9;
		c->BodyPart[BODYPART_HELM].Level = Level;
		c->BodyPart[BODYPART_ARMOR].Level = Level;
		c->BodyPart[BODYPART_PANTS].Level = Level;
		c->BodyPart[BODYPART_GLOVES].Level = Level;
		c->BodyPart[BODYPART_BOOTS].Level = Level;
		//c->Weapon[0].Level = Level;
		//c->Weapon[1].Level = Level;
		c->PK = PVP_MURDERER2;
		SetCharacterScale(c);
		if (gMapManager->InDevilSquare() == true)
		{
			c->Object.Scale = 1.0f;
		}
		break;
		/*OpenMonsterModel(5);
		c = CreateCharacter(Key,MODEL_MONSTER01+5,PositionX,PositionY);
		strcpy(c->ID,"자이언트");
		c->Weapon[0].Type = MODEL_AXE+2;
		c->Weapon[1].Type = MODEL_AXE+2;
		c->Object.Scale = 0.7f;
		Vector(50.f,50.f,80.f,c->Object.BoundingBoxMax);
		break;*/
	case 33:
		OpenMonsterModel(19);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 19, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_MACE + 1;
		c->Weapon[1].Type = MODEL_SHIELD + 1;
		c->Object.Scale = 1.2f;
		c->Level = 1;
		strcpy(c->ID, "고블린 대장");
		break;
	case 32:
		OpenMonsterModel(25);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 25, PositionX, PositionY);
		strcpy(c->ID, "돌괴물");
		break;
	case 31:
		OpenMonsterModel(24);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 24, PositionX, PositionY);
		strcpy(c->ID, "아곤");
		c->Object.Scale = 1.3f;
		c->Weapon[0].Type = MODEL_SWORD + 8;
		c->Weapon[1].Type = MODEL_SWORD + 8;
		break;
	case 30:
		OpenMonsterModel(23);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 23, PositionX, PositionY);
		strcpy(c->ID, "숲의괴물");
		c->Object.Scale = 0.75f;
		break;
	case 29:
		OpenMonsterModel(22);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 22, PositionX, PositionY);
		strcpy(c->ID, "헌터");
		c->Weapon[0].Type = MODEL_BOW + 10;
		c->Object.Scale = 0.95f;
		break;
	case 28:
		OpenMonsterModel(21);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 21, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_SPEAR + 1;
		c->Object.Scale = 0.8f;
		strcpy(c->ID, "풍뎅이괴물");
		c->Object.BlendMesh = 1;
		break;
	case 27:
		OpenMonsterModel(20);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 20, PositionX, PositionY);
		c->Object.Scale = 1.1f;
		strcpy(c->ID, "고리전갈");
		break;
	case 26:
		OpenMonsterModel(19);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 19, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_AXE;
		c->Object.Scale = 0.8f;
		strcpy(c->ID, "고블린");
		break;
	case 25:
		OpenMonsterModel(18);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 18, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_STAFF + 1;
		c->Object.BlendMesh = 2;
		c->Object.BlendMeshLight = 1.f;
		c->Object.Scale = 1.1f;
		c->Object.LightEnable = false;
		c->Level = 3;
		strcpy(c->ID, "아이스퀸");
		break;
	case 24:
		OpenMonsterModel(17);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 17, PositionX, PositionY);
		strcpy(c->ID, "웜");
		break;
	case 23:
		OpenMonsterModel(16);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 16, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_AXE + 7;
		c->Weapon[1].Type = MODEL_SHIELD + 10;
		c->Object.Scale = 1.15f;
		strcpy(c->ID, "호머드");
		break;
	case 22:
		OpenMonsterModel(15);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 15, PositionX, PositionY);
		c->Object.BlendMesh = 0;
		c->Object.BlendMeshLight = 1.f;
		strcpy(c->ID, "얼음괴물");
		break;
	case 21:
		OpenMonsterModel(14);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 14, PositionX, PositionY);
		c->Object.Scale = 0.95f;
		strcpy(c->ID, "암살자");
		break;
	case 20:
		OpenMonsterModel(13);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 13, PositionX, PositionY);
		strcpy(c->ID, "설인 대장");
		c->Object.Scale = 1.4f;
		break;
	case 19:
		OpenMonsterModel(12);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 12, PositionX, PositionY);
		strcpy(c->ID, "설인");
		c->Object.Scale = 1.1f;
		break;
	case 18:
		OpenMonsterModel(11);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 11, PositionX, PositionY);
		strcpy(c->ID, "고르곤");
		c->Object.Scale = 1.5f;
		c->Weapon[0].Type = MODEL_STAFF + 4;
		c->Object.BlendMesh = 1;
		c->Object.BlendMeshLight = 1.f;
		break;
	case 3:
		OpenMonsterModel(9);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 9, PositionX, PositionY);
		strcpy(c->ID, "거미");
		c->Object.Scale = 0.4f;
		break;
	case 17:
		OpenMonsterModel(10);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 10, PositionX, PositionY);
		strcpy(c->ID, "싸이크롭스");
		c->Weapon[0].Type = MODEL_AXE + 8;
		//c->Weapon[1].Type = MODEL_MACE+2;
		//c->Object.HiddenMesh = 2;
		break;
	case 0:
	case 4:
	case 8:
	default:
		OpenMonsterModel(0);
		c = CreateCharacter(Key, MODEL_MONSTER01, PositionX, PositionY);
		if (Type == 0)
		{
			c->Object.HiddenMesh = 0;
			strcpy(c->ID, "소뿔전사");
			c->Object.Scale = 0.8f;
			c->Weapon[0].Type = MODEL_AXE + 6;
		}
		else if (Type == 4)
		{
			c->Weapon[0].Type = MODEL_SPEAR + 7;
			strcpy(c->ID, "소뿔전사 대장");
			c->Object.Scale = 1.15f;
			c->Level = 1;
		}
		else if (Type == 8)
		{
			c->Weapon[0].Type = MODEL_SPEAR + 8;
			strcpy(c->ID, "포이즌 소뿔전사");
			c->Object.Scale = 1.f;
			c->Level = 2;

			g_CharacterRegisterBuff((&c->Object), eDeBuff_Poison);
		}
		break;
	case 11:
		OpenMonsterModel(7);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 7, PositionX, PositionY);
		strcpy(c->ID, "고스트");
		c->Object.AlphaTarget = 0.4f;
		c->MoveSpeed = 15;
		c->Blood = true;
		break;
	case 12:
		OpenMonsterModel(6);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 6, PositionX, PositionY);
		strcpy(c->ID, "유충");
		c->Object.Scale = 0.6f;
		break;
	case 13:
		OpenMonsterModel(8);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 8, PositionX, PositionY);
		strcpy(c->ID, "헬스파이더");
		c->Weapon[0].Type = MODEL_STAFF + 2;
		c->Object.Scale = 1.1f;
		break;
	case 1:
	case 5:
		OpenMonsterModel(1);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 1, PositionX, PositionY);
		if (Type == 1)
		{
			c->Object.HiddenMesh = 0;
			strcpy(c->ID, "하운드");
			c->Object.Scale = 0.85f;
			c->Weapon[0].Type = MODEL_SWORD + 4;
		}
		if (Type == 5)
		{
			c->Object.HiddenMesh = 1;
			c->Weapon[0].Type = MODEL_SWORD + 7;
			c->Weapon[1].Type = MODEL_SHIELD + 9;
			strcpy(c->ID, "헬하운드");
			c->Object.Scale = 1.1f;
			c->Level = 1;
		}
		break;

	case 2:
		OpenMonsterModel(2);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 2, PositionX, PositionY);
		strcpy(c->ID, "Unknown2");
		c->Object.Scale = 0.5f;
		break;

	case 10:
		OpenMonsterModel(3);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 3, PositionX, PositionY);
		strcpy(c->ID, "Unknown10");
		c->Object.Scale = 0.8f;
		c->Level = 1;
		c->Weapon[0].Type = MODEL_SWORD + 13;
		break;
	case 6:
	case 9:
		OpenMonsterModel(4);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 4, PositionX, PositionY);
		if (Type == 6)
		{
			strcpy(c->ID, "리치");
			c->Weapon[0].Type = MODEL_STAFF + 2;
			c->Object.Scale = 0.85f;
		}
		else
		{
			strcpy(c->ID, "썬더 리치");
			c->Weapon[0].Type = MODEL_STAFF + 3;
			c->Level = 1;
			c->Object.Scale = 1.1f;
		}
		break;
	case 7:
		OpenMonsterModel(5);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 5, PositionX, PositionY);
		strcpy(c->ID, "자이언트");
		c->Weapon[0].Type = MODEL_AXE + 2;
		c->Weapon[1].Type = MODEL_AXE + 2;
		c->Object.Scale = 1.6f;
		break;

	case 14:
	case 55:
	case 56:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "해골전사");
		c->Object.SubType = MODEL_SKELETON1;
		c->Blood = true;
		if (Type == 14)
		{
			c->Object.Scale = 0.95f;
			c->Weapon[0].Type = MODEL_SWORD + 6;
			c->Weapon[1].Type = MODEL_SHIELD + 4;
		}
		else if (Type == 56)
		{
			c->Object.Scale = 0.8f;
			c->Weapon[0].Type = MODEL_SPEAR + 8;
		}
		else
		{
			c->Level = 1;
			c->Object.Scale = 1.4f;
			c->Weapon[0].Type = MODEL_SPEAR + 9;
		}
		break;
	case 15:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "해골궁수");
		c->Object.Scale = 1.1f;
		c->Weapon[1].Type = MODEL_BOW + 2;
		c->Object.SubType = MODEL_SKELETON2;
		c->Level = 1;
		c->Blood = true;
		break;
	case 16:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "해골전사 대장");
		c->Object.Scale = 1.2f;
		c->Weapon[0].Type = MODEL_AXE + 3;
		c->Weapon[1].Type = MODEL_SHIELD + 6;
		c->Object.SubType = MODEL_SKELETON3;
		c->Level = 1;
		c->Blood = true;
		break;
	case 372:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "엘리트 해골전사");
		c->Object.Scale = 0.95f;
		c->Object.SubType = MODEL_SKELETON_PCBANG;
		break;
	case 373:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "잭 오랜턴");
		c->Object.Scale = 0.95f;
		c->Object.SubType = MODEL_HALLOWEEN;
		break;
	case 374:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "크리스마스 걸");
		c->Object.Scale = 0.85f;
		c->Object.SubType = MODEL_XMAS_EVENT_CHANGE_GIRL;
		break;
	case 378:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "GameMaster");
		c->Object.Scale = 1.0f;
		c->Object.SubType = MODEL_GM_CHARACTER;
		break;
	case 53:
		OpenMonsterModel(39);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 39, PositionX, PositionY);
		strcpy(c->ID, "타이탄");
		c->Object.Scale = 1.8f;
		c->Object.BlendMesh = 2;
		c->Object.BlendMeshLight = 1.f;
		o = &c->Object;
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 2, o, 30.f);
		CreateJoint(BITMAP_JOINT_ENERGY, o->Position, o->Position, o->Angle, 3, o, 30.f);
		break;
	case 54:
	case 151:
		OpenMonsterModel(40);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 40, PositionX, PositionY);
		strcpy(c->ID, "솔져");
		c->Weapon[1].Type = MODEL_BOW + 14;
		if (Type == 54)
			c->Object.Scale = 1.1f;
		else
			c->Object.Scale = 1.3f;
		break;
	case 100:
		c = CreateCharacter(Key, 39, PositionX, PositionY);
		break;
	case 101:
		c = CreateCharacter(Key, 40, PositionX, PositionY);
		break;
	case 102:
		c = CreateCharacter(Key, 51, PositionX, PositionY);
		break;
	case 103:
		c = CreateCharacter(Key, 25, PositionX, PositionY);
		break;
	case 106:
		c = CreateCharacter(Key, 51, PositionX, PositionY);
		break;
	case 200:
		c = CreateCharacter(Key, MODEL_BALL, PositionX, PositionY);
		o = &c->Object;
		o->BlendMesh = 2;
		o->Scale = 1.8f;
		c->Level = 1;
		break;
	case 226:
		OpenNpc(MODEL_NPC_BREEDER);
		c = CreateCharacter(Key, MODEL_NPC_BREEDER, PositionX, PositionY);
		strcpy(c->ID, "조련사 NPC");
		break;

#ifdef _PVP_MURDERER_HERO_ITEM
	case 227:
		OpenNpc(MODEL_MASTER);
		c = CreateCharacter(Key, MODEL_MASTER, PositionX, PositionY);
		strcpy(c->ID, "살인마상점");
		break;

	case 228:
		OpenNpc(MODEL_HERO_SHOP);
		c = CreateCharacter(Key, MODEL_HERO_SHOP, PositionX, PositionY);
		strcpy(c->ID, "영웅상점");
		break;
#endif	// _PVP_MURDERER_HERO_ITEM

	case 229:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "말론");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 9;
		c->BodyPart[BODYPART_HELM].Level = 7;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 9;
		c->BodyPart[BODYPART_ARMOR].Level = 7;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 9;
		c->BodyPart[BODYPART_PANTS].Level = 7;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 9;
		c->BodyPart[BODYPART_GLOVES].Level = 7;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 9;
		c->BodyPart[BODYPART_BOOTS].Level = 7;
		c->Weapon[0].Type = MODEL_SPEAR + 7;
		c->Weapon[0].Level = 8;
		c->Weapon[1].Type = -1;
		SetCharacterScale(c);
		break;
	case 230:
		OpenNpc(MODEL_MERCHANT_MAN);
		c = CreateCharacter(Key, MODEL_MERCHANT_MAN, PositionX, PositionY);
		strcpy(c->ID, "로랜추가상인");
		c->BodyPart[BODYPART_HELM].Type = MODEL_MERCHANT_MAN_HEAD;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_MERCHANT_MAN_UPPER + 1;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_MERCHANT_MAN_GLOVES + 1;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_MERCHANT_MAN_BOOTS;
		break;
	case 231:
		OpenNpc(MODEL_DEVIAS_TRADER);
		c = CreateCharacter(Key, MODEL_DEVIAS_TRADER, PositionX, PositionY);
		strcpy(c->ID, "데비추가상인");
		break;

	case 232:
		OpenNpc(MODEL_NPC_ARCHANGEL);
		c = CreateCharacter(Key, MODEL_NPC_ARCHANGEL, PositionX, PositionY);
		o = &c->Object;
		o->Scale = 1.f;
		o->Kind = KIND_NPC;
		break;
	case 233:
		OpenNpc(MODEL_NPC_ARCHANGEL_MESSENGER);
		c = CreateCharacter(Key, MODEL_NPC_ARCHANGEL_MESSENGER, PositionX, PositionY);
		o = &c->Object;
		o->Scale = 1.f;
		o->Kind = KIND_NPC;
		break;

	case 234:
		OpenMonsterModel(19);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 19, PositionX, PositionY);
		c->Weapon[0].Type = MODEL_STAFF;
		c->Weapon[0].Level = 4;
		c->Object.Scale = 1.5f;
		c->Object.Kind = KIND_NPC;
		SetAction(&c->Object, 0);
		break;

	case 235:
		OpenNpc(MODEL_NPC_SEVINA);
		c = CreateCharacter(Key, MODEL_NPC_SEVINA, PositionX, PositionY);
		o = &c->Object;
		o->Scale = 1.f;
		o->Kind = KIND_NPC;
		break;

	case 236:
		OpenNpc(MODEL_PLAYER);
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		o = &c->Object;
		o->SubType = MODEL_SKELETON2;
		o->Scale = 1.0f;
		o->Kind = KIND_NPC;
		c->Level = 8;
		break;
	case 237:
		OpenNpc(MODEL_NPC_DEVILSQUARE);
		c = CreateCharacter(Key, MODEL_NPC_DEVILSQUARE, PositionX, PositionY);
		break;
	case 369:
		OpenNpc(MODEL_REFINERY_NPC);
		c = CreateCharacter(Key, MODEL_REFINERY_NPC, PositionX, PositionY);
		o = &c->Object;
		break;
	case 370://환원
		OpenNpc(MODEL_RECOVERY_NPC);
		c = CreateCharacter(Key, MODEL_RECOVERY_NPC, PositionX, PositionY);
		o = &c->Object;
		break;
	case 238:
		OpenNpc(MODEL_MIX_NPC);
		c = CreateCharacter(Key, MODEL_MIX_NPC, PositionX, PositionY);
		o = &c->Object;
		o->BlendMesh = 1;
		break;
	case 239:
		OpenNpc(MODEL_TOURNAMENT);
		c = CreateCharacter(Key, MODEL_TOURNAMENT, PositionX, PositionY);
		break;
	case 240:
		OpenNpc(MODEL_STORAGE);
		c = CreateCharacter(Key, MODEL_STORAGE, PositionX, PositionY);
		break;
	case 241:
		OpenNpc(MODEL_MASTER);
		c = CreateCharacter(Key, MODEL_MASTER, PositionX, PositionY);
		strcpy(c->ID, "마스터");
		break;
	case 256:
		OpenNpc(MODEL_NPC_SERBIS);
		c = CreateCharacter(Key, MODEL_NPC_SERBIS, PositionX, PositionY);
		strcpy(c->ID, "세르비스");
		break;
	case 257:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		MakeElfHelper(c);
		strcpy(c->ID, "페이아");
		o = &c->Object;
		CreateJoint(BITMAP_FLARE, o->Position, o->Position, o->Angle, 42, o, 15.f);
		break;
	case 242:
		OpenNpc(MODEL_ELF_WIZARD);
		c = CreateCharacter(Key, MODEL_ELF_WIZARD, PositionX, PositionY);
		strcpy(c->ID, "라라 요정");
		o = &c->Object;
		o->BlendMesh = 1;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 140.f;
		break;
	case 243:
		OpenNpc(MODEL_ELF_MERCHANT);
		c = CreateCharacter(Key, MODEL_ELF_MERCHANT, PositionX, PositionY);
		strcpy(c->ID, "장인");
		break;
	case 244:
		OpenNpc(MODEL_SNOW_MERCHANT);
		c = CreateCharacter(Key, MODEL_SNOW_MERCHANT, PositionX, PositionY);
		strcpy(c->ID, "술집마담");
		break;
	case 245:
		OpenNpc(MODEL_SNOW_WIZARD);
		c = CreateCharacter(Key, MODEL_SNOW_WIZARD, PositionX, PositionY);
		strcpy(c->ID, "마법사");
		break;
	case 246:
		OpenNpc(MODEL_SNOW_SMITH);
		c = CreateCharacter(Key, MODEL_SNOW_SMITH, PositionX, PositionY);
		strcpy(c->ID, "무기상인");
		break;
	case 247:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "경비병");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 9;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 9;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 9;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 9;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 9;
		c->Weapon[0].Type = MODEL_BOW + 11;
		c->Weapon[1].Type = MODEL_BOW + 7;
		SetCharacterScale(c);
		break;
	case 248:
		OpenNpc(MODEL_MERCHANT_MAN);
		c = CreateCharacter(Key, MODEL_MERCHANT_MAN, PositionX, PositionY);
		strcpy(c->ID, "떠돌이 상인");
		c->BodyPart[BODYPART_HELM].Type = MODEL_MERCHANT_MAN_HEAD + 1;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_MERCHANT_MAN_UPPER + 1;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_MERCHANT_MAN_GLOVES + 1;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_MERCHANT_MAN_BOOTS + 1;
		break;
	case 249:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "경비병");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 9;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 9;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 9;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 9;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 9;
		c->Weapon[0].Type = MODEL_SPEAR + 7;
		SetCharacterScale(c);
		break;
	case 250:
		OpenNpc(MODEL_MERCHANT_MAN);
		c = CreateCharacter(Key, MODEL_MERCHANT_MAN, PositionX, PositionY);
		strcpy(c->ID, "떠돌이 상인");
		c->BodyPart[BODYPART_HELM].Type = MODEL_MERCHANT_MAN_HEAD;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_MERCHANT_MAN_UPPER;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_MERCHANT_MAN_GLOVES;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_MERCHANT_MAN_BOOTS;
		break;
	case 251:
		OpenNpc(MODEL_SMITH);
		c = CreateCharacter(Key, MODEL_SMITH, PositionX, PositionY);
		strcpy(c->ID, "대장장이 한스");
		c->Object.Scale = 0.95f;
		break;
	case 253:
		OpenNpc(MODEL_MERCHANT_GIRL);
		c = CreateCharacter(Key, MODEL_MERCHANT_GIRL, PositionX, PositionY);
		strcpy(c->ID, "물약파는 소녀");
		c->BodyPart[BODYPART_HELM].Type = MODEL_MERCHANT_GIRL_HEAD;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_MERCHANT_GIRL_UPPER;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_MERCHANT_GIRL_LOWER;
		break;
	case 254:
		OpenNpc(MODEL_SCIENTIST);
		c = CreateCharacter(Key, MODEL_SCIENTIST, PositionX, PositionY);
		strcpy(c->ID, "마법사 파시");
		break;
	case 255:
		OpenNpc(MODEL_MERCHANT_FEMALE);
		c = CreateCharacter(Key, MODEL_MERCHANT_FEMALE, PositionX, PositionY);
		strcpy(c->ID, "술집마담 리아먼");
		c->BodyPart[BODYPART_HELM].Type = MODEL_MERCHANT_FEMALE_HEAD + 1;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_MERCHANT_FEMALE_UPPER + 1;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_MERCHANT_FEMALE_LOWER + 1;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_MERCHANT_FEMALE_BOOTS + 1;
		break;
	case 204:
		OpenNpc(MODEL_CRYWOLF_STATUE);
		c = CreateCharacter(Key, MODEL_CRYWOLF_STATUE, PositionX, PositionY);
		strcpy(c->ID, "석상");
		c->Object.Live = false;
		break;
	case 205:
		OpenNpc(MODEL_CRYWOLF_ALTAR1);
		c = CreateCharacter(Key, MODEL_CRYWOLF_ALTAR1, PositionX, PositionY);
		strcpy(c->ID, "제단1");
		c->Object.Position[2] -= 10.0f;
		c->Object.HiddenMesh = -2;
		c->Object.Visible = false;
		c->Object.EnableShadow = false;
		break;
	case 206:
		OpenNpc(MODEL_CRYWOLF_ALTAR2);
		c = CreateCharacter(Key, MODEL_CRYWOLF_ALTAR2, PositionX, PositionY);
		strcpy(c->ID, "제단2");
		c->Object.HiddenMesh = -2;
		c->Object.Position[2] -= 10.0f;
		c->Object.Visible = false;
		c->Object.EnableShadow = false;
		break;
	case 207:
		OpenNpc(MODEL_CRYWOLF_ALTAR3);
		c = CreateCharacter(Key, MODEL_CRYWOLF_ALTAR3, PositionX, PositionY);
		strcpy(c->ID, "제단3");
		c->Object.HiddenMesh = -2;
		c->Object.Position[2] -= 10.0f;
		c->Object.Visible = false;
		c->Object.EnableShadow = false;
		break;
	case 208:
		OpenNpc(MODEL_CRYWOLF_ALTAR4);
		c = CreateCharacter(Key, MODEL_CRYWOLF_ALTAR4, PositionX, PositionY);
		strcpy(c->ID, "제단4");
		c->Object.HiddenMesh = -2;
		c->Object.Position[2] -= 10.0f;
		c->Object.Visible = false;
		c->Object.EnableShadow = false;
		break;
	case 209:
		OpenNpc(MODEL_CRYWOLF_ALTAR5);
		c = CreateCharacter(Key, MODEL_CRYWOLF_ALTAR5, PositionX, PositionY);
		strcpy(c->ID, "제단5");
		c->Object.HiddenMesh = -2;
		c->Object.Position[2] -= 10.0f;
		c->Object.Visible = false;
		c->Object.EnableShadow = false;
		break;
	case 368:
		OpenNpc(MODEL_SMELTING_NPC);
		c = CreateCharacter(Key, MODEL_SMELTING_NPC, PositionX + 1, PositionY - 1);
		strcpy(c->ID, "제련의탑NPC");
		c->Object.Scale = 2.5f;
		c->Object.EnableShadow = false;
		c->Object.m_bRenderShadow = false;
		break;
	case 379:
		OpenNpc(MODEL_WEDDING_NPC);
		c = CreateCharacter(Key, MODEL_WEDDING_NPC, PositionX, PositionY);
		strcpy(c->ID, "WeddingNPC");
		c->Object.Scale = 1.1f;
		c->Object.EnableShadow = false;
		c->Object.m_bRenderShadow = false;
		break;
	case 258:
	case 371:
	case 414:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "HelperName");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 9;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 9;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 9;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 9;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 9;
		c->Weapon[0].Type = -1;
		SetCharacterScale(c);
		c->Object.m_bpcroom = true;
		break;
	case 259:
		OpenNpc(MODEL_KALIMA_SHOP);
		c = CreateCharacter(Key, MODEL_KALIMA_SHOP, PositionX, PositionY);
		c->Object.Position[2] += 140.0f;
		strcpy(c->ID, "KalimaShop");
		break;
	case 375:
	{
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		strcpy(c->ID, "ChaosCard");
		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 30;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 30;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 30;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 30;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 30;
		c->Wing.Type = MODEL_WING + 1;
		c->Wing.ClothType = 0;
		int iLevel = 9;
		c->BodyPart[BODYPART_HELM].Level = iLevel;
		c->BodyPart[BODYPART_ARMOR].Level = iLevel;
		c->BodyPart[BODYPART_PANTS].Level = iLevel;
		c->BodyPart[BODYPART_GLOVES].Level = iLevel;
		c->BodyPart[BODYPART_BOOTS].Level = iLevel;
		c->Weapon[0].Type = -1;
		SetCharacterScale(c);
		c->Object.SubType = Type;
	}
	break;
	case 376:
	{
		OpenNpc(MODEL_BC_NPC1);
		c = CreateCharacter(Key, MODEL_BC_NPC1, PositionX, PositionY);
		strcpy(c->ID, "공성 NPC");
		c->Object.Scale = 1.0f;
		c->Object.Angle[2] = 0.f;
		CreateObject(MODEL_BC_BOX, c->Object.Position, c->Object.Angle);
	}
	break;
	case 377:
	{
		OpenNpc(MODEL_BC_NPC2);
		c = CreateCharacter(Key, MODEL_BC_NPC2, PositionX, PositionY);
		strcpy(c->ID, "공성 NPC");
		c->Object.Scale = 1.0f;
		c->Object.Angle[2] = 90.f;
		CreateObject(MODEL_BC_BOX, c->Object.Position, c->Object.Angle);
	}
	break;
	case 406:
		OpenNpc(MODEL_NPC_DEVIN);
		c = CreateCharacter(Key, MODEL_NPC_DEVIN, PositionX, PositionY);
		strcpy(c->ID, "사제데빈");
		break;
	case 407:
		OpenNpc(MODEL_NPC_QUARREL);
		c = CreateCharacter(Key, MODEL_NPC_QUARREL, PositionX, PositionY);
		strcpy(c->ID, "웨어울프쿼렐");
		c->Object.Scale = 1.9f;
		break;
	case 408:
		OpenNpc(MODEL_NPC_CASTEL_GATE);
		c = CreateCharacter(Key, MODEL_NPC_CASTEL_GATE, PositionX, PositionY, 90.f);
		strcpy(c->ID, "성문");
		o = &c->Object;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 240.f;
		c->Object.Scale = 1.2f;
		c->Object.m_fEdgeScale = 1.1f;
		c->Object.EnableShadow = false;
		c->Object.m_bRenderShadow = false;
		break;
	case 413:
	{
		OpenMonsterModel(127);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 127, PositionX, PositionY);
		strcpy(c->ID, "달토끼");
		c->Object.Scale = 0.8f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		c->Object.SubType = rand() % 3;
		c->Object.m_iAnimation = 0;

		BoneManager::RegisterBone(c, "Rabbit_1", 3);		// Bip01 Spine 	
		BoneManager::RegisterBone(c, "Rabbit_2", 16);		// Bip01 Head
		BoneManager::RegisterBone(c, "Rabbit_3", 15);		// Bip01 Neck1 
		BoneManager::RegisterBone(c, "Rabbit_4", 2);		// Bip01 Pelvis
	}

	break;
	case 450:
	{
		OpenNpc(MODEL_NPC_CHERRYBLOSSOM);
		c = CreateCharacter(Key, MODEL_NPC_CHERRYBLOSSOM, PositionX, PositionY);
		c->Object.Scale = 0.65f;
		c->Object.m_fEdgeScale = 1.08f;
		o = &c->Object;
		o->Position[2] = RequestTerrainHeight(o->Position[0], o->Position[1]) + 170.f;
		strcpy(c->ID, "벚꽃의정령");
	}
	break;
	case 451:
	{
		OpenNpc(MODEL_NPC_CHERRYBLOSSOMTREE);
		c = CreateCharacter(Key, MODEL_NPC_CHERRYBLOSSOMTREE, PositionX, PositionY);
		c->Object.Scale = 1.0f;
		c->Object.m_fEdgeScale = 0.0f;
		c->Object.m_bRenderShadow = false;
		strcpy(c->ID, "벚꽃나무");
	}
	break;
	case 579:
		OpenNpc(MODEL_LUCKYITEM_NPC);
		c = CreateCharacter(Key, MODEL_LUCKYITEM_NPC, PositionX, PositionY);
		strcpy(c->ID, "큑비드");
		c->Object.Scale = 0.95f;
		c->Object.m_fEdgeScale = 1.2f;
		gmClientModels->GetModel(MODEL_LUCKYITEM_NPC)->Actions[0].PlaySpeed = 0.45f;
		gmClientModels->GetModel(MODEL_LUCKYITEM_NPC)->Actions[1].PlaySpeed = 0.5f;
		//	gmClientModels->GetModel(MODEL_LUCKYITEM_NPC)->Actions[0].PlaySpeed = 50.0f;
		//	gmClientModels->GetModel(MODEL_LUCKYITEM_NPC)->Actions[1].PlaySpeed = 50.0f;
		break;
	case 452:
		OpenNpc(MODEL_SEED_MASTER);
		c = CreateCharacter(Key, MODEL_SEED_MASTER, PositionX, PositionY);
		strcpy(c->ID, "시드마스터");
		c->Object.Scale = 1.1f;
		c->Object.m_fEdgeScale = 1.2f;
		break;
	case 453:
		OpenNpc(MODEL_SEED_INVESTIGATOR);
		c = CreateCharacter(Key, MODEL_SEED_INVESTIGATOR, PositionX, PositionY);
		strcpy(c->ID, "시드연구가");
		c->Object.Scale = 0.9f;
		c->Object.m_fEdgeScale = 1.15f;
		//gmClientModels->GetModel(MODEL_SEED_INVESTIGATOR)->Actions[0].PlaySpeed = 0.2f;
		//gmClientModels->GetModel(MODEL_SEED_INVESTIGATOR)->Actions[1].PlaySpeed = 0.1f;
		break;
	case 464:
	{
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		//c->Class = 2;
		strcpy(c->ID, "초기화 도우미");

		c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 9;
		c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 9;
		c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 9;
		c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 9;
		c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 9;

		c->Object.m_fEdgeScale = 1.15f;
		c->Weapon[0].Type = MODEL_BOW + 11;
		c->Weapon[1].Type = MODEL_BOW + 7;
		SetCharacterScale(c);
	}
	break;
	case 477:
		OpenNpc(MODEL_XMAS2008_SNOWMAN);
		c = CreateCharacter(Key, MODEL_XMAS2008_SNOWMAN, PositionX, PositionY);
		::strcpy(c->ID, "Unknown");
		c->Object.LifeTime = 100;
		c->Object.Scale = 1.3f;
		break;
#ifdef PJH_ADD_PANDA_CHANGERING
	case 503:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "Unknown");
		c->Object.SubType = MODEL_PANDA;
		break;
#endif //PJH_ADD_PANDA_CHANGERING
	case 548:
		c = CreateCharacter(Key, MODEL_PLAYER, PositionX, PositionY);
		::strcpy(c->ID, "Unknown");
		c->Object.SubType = MODEL_SKELETON_CHANGED;
		break;
	case 468:
	case 469:
	case 470:
	case 471:
	case 472:
	case 473:
	case 474:
	case 475:
	{
		int _Model_NpcIndex = MODEL_LITTLESANTA + (Type - 468);

		OpenNpc(_Model_NpcIndex);
		c = CreateCharacter(Key, _Model_NpcIndex, PositionX, PositionY);

		c->Object.Scale = 0.43f;

		for (int i = 0; i < 5; i++)
		{
			if (i < 2 || i == 4)
			{
				//xmassanta_stand_1~2 || xmassanta_idle3
				gmClientModels->GetModel(_Model_NpcIndex)->Actions[i].PlaySpeed = 0.4f;
			}
			else// if(i >= 2 && i < 4)
			{
				//xmassanta_idle1~2
				gmClientModels->GetModel(_Model_NpcIndex)->Actions[i].PlaySpeed = 0.5f;
			}
		}
		strcpy(c->ID, "little santa");
	}
	break;
	case 478:
		//델가도
		OpenNpc(MODEL_NPC_SERBIS);
		c = CreateCharacter(Key, MODEL_NPC_SERBIS, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		break;
	case 479:
		// 결투장 문지기 NPC 타이투스
		OpenNpc(MODEL_DUEL_NPC_TITUS);
		c = CreateCharacter(Key, MODEL_DUEL_NPC_TITUS, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.1f;
		c->Object.m_fEdgeScale = 1.2f;
		break;
	case 492:
	{
		OpenNpc(MODEL_GAMBLE_NPC_MOSS);
		c = CreateCharacter(Key, MODEL_GAMBLE_NPC_MOSS, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.LifeTime = 100;
		c->Object.Scale = 0.8f;
		c->Object.m_fEdgeScale = 1.1f;

		for (int i = 0; i < 6; i++)
		{
			gmClientModels->GetModel(MODEL_GAMBLE_NPC_MOSS)->Actions[i].PlaySpeed = 0.33f;
		}
	}
	break;
	case 502:
		OpenMonsterModel(128);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 128, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.0f * 0.95f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		break;
	case 493:
		OpenMonsterModel(3);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 3, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 0.8f;
		c->Level = 1;
		c->Weapon[0].Type = MODEL_SWORD + 13;
		break;
		break;
	case 494:
		OpenMonsterModel(26);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 26, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.1f;
		break;
	case 495:
		OpenMonsterModel(101);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 101, PositionX, PositionY);
		c->Object.Scale = 1.35f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		BoneManager::RegisterBone(c, "Monster101_L_Arm", 12);
		BoneManager::RegisterBone(c, "Monster101_R_Arm", 20);
		BoneManager::RegisterBone(c, "Monster101_Head", 6);
		break;
	case 496:
		OpenMonsterModel(52);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 52, PositionX, PositionY);
		c->Object.Scale = 1.1f;
		c->Weapon[0].Type = MODEL_SWORD + 18;
		c->Weapon[0].Level = 5;
		c->Weapon[1].Type = MODEL_SHIELD + 14;
		c->Weapon[1].Level = 0;
		c->Object.BlendMesh = 1;
		c->Object.BlendMeshLight = 1.f;
		break;
	case 497:
		OpenMonsterModel(109);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 109, PositionX, PositionY);
		c->Object.Scale = 1.3f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		strcpy(c->ID, "Unknown");
		break;
	case 498:
		OpenMonsterModel(115);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 115, PositionX, PositionY);
		c->Object.Scale = 1.3f;
		c->Object.Angle[0] = 0.0f;
		c->Object.Gravity = 0.0f;
		c->Object.Distance = (float)(rand() % 20) / 10.0f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		BoneManager::RegisterBone(c, "Twintail_Hair24", 16);
		BoneManager::RegisterBone(c, "Twintail_Hair32", 24);
		break;
	case 499:
		OpenMonsterModel(149);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 149, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.5f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		break;
	case 500:
		OpenMonsterModel(142);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 142, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 0.95f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		break;
	case 501:
		OpenMonsterModel(31);
		c = CreateCharacter(Key, MODEL_MONSTER01 + 31, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 0.88f;
		c->Weapon[0].Type = -1;
		c->Weapon[1].Type = -1;
		break;
	case 540:
		OpenNpc(MODEL_DOPPELGANGER_NPC_LUGARD);
		c = CreateCharacter(Key, MODEL_DOPPELGANGER_NPC_LUGARD, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.1f;
		c->Object.m_fEdgeScale = 1.2f;
		break;
	case 541:
		OpenNpc(MODEL_DOPPELGANGER_NPC_BOX);
		c = CreateCharacter(Key, MODEL_DOPPELGANGER_NPC_BOX, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 2.3f;
		c->Object.m_fEdgeScale = 1.1f;
		break;
	case 542:
		OpenNpc(MODEL_DOPPELGANGER_NPC_GOLDENBOX);
		c = CreateCharacter(Key, MODEL_DOPPELGANGER_NPC_GOLDENBOX, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 3.3f;
		c->Object.m_fEdgeScale = 1.1f;
		break;
	case 543:
		OpenNpc(MODAL_GENS_NPC_DUPRIAN);
		c = CreateCharacter(Key, MODAL_GENS_NPC_DUPRIAN, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.0f;
		break;
	case 544:
		OpenNpc(MODAL_GENS_NPC_BARNERT);
		c = CreateCharacter(Key, MODAL_GENS_NPC_BARNERT, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.0f;
		break;
	case 545:
		OpenNpc(MODEL_UNITEDMARKETPLACE_CHRISTIN);
		c = CreateCharacter(Key, MODEL_UNITEDMARKETPLACE_CHRISTIN, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.1f;
		c->Object.m_fEdgeScale = 1.2f;
		break;
	case 546:
		OpenNpc(MODEL_UNITEDMARKETPLACE_RAUL);
		c = CreateCharacter(Key, MODEL_UNITEDMARKETPLACE_RAUL, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.0f;
		c->Object.m_fEdgeScale = 1.15f;
		break;
	case 547:
		OpenNpc(MODEL_UNITEDMARKETPLACE_JULIA);
		c = CreateCharacter(Key, MODEL_UNITEDMARKETPLACE_JULIA, PositionX, PositionY);
		strcpy(c->ID, "Unknown");
		c->Object.Scale = 1.0f;
		c->Object.m_fEdgeScale = 1.1f;
		break;
	case 566:
		OpenNpc(MODEL_TIME_LIMIT_QUEST_NPC_TERSIA);
		c = CreateCharacter(Key, MODEL_TIME_LIMIT_QUEST_NPC_TERSIA, PositionX, PositionY);
		strcpy(c->ID, "길드관리인 테르시아");
		c->Object.Scale = 0.93f;
		break;
	case 567:
		OpenNpc(MODEL_TIME_LIMIT_QUEST_NPC_BENA);
		c = CreateCharacter(Key, MODEL_TIME_LIMIT_QUEST_NPC_BENA, PositionX, PositionY);
		strcpy(c->ID, "신녀 베이나");
		c->Object.Position[2] += 145.0f;
		break;
	case 568:
	{
		OpenNpc(MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO);
		c = CreateCharacter(Key, MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO, PositionX, PositionY);
		strcpy(c->ID, "떠돌이상인 자이로");
		c->Object.LifeTime = 100;
		c->Object.Scale = 0.8f;
		c->Object.m_fEdgeScale = 1.1f;
		int i;
		for (i = 0; i < 6; ++i)
			gmClientModels->GetModel(MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO)->Actions[i].PlaySpeed = 0.33f;
	}
	break;
	case 577:
		OpenNpc(MODEL_KARUTAN_NPC_REINA);
		c = CreateCharacter(Key, MODEL_KARUTAN_NPC_REINA, PositionX, PositionY);
		strcpy(c->ID, "잡화상인 레이나");
		c->Object.Scale = 1.1f;
		c->Object.m_fEdgeScale = 1.2f;
		break;
	case 578:
		OpenNpc(MODEL_KARUTAN_NPC_VOLVO);
		c = CreateCharacter(Key, MODEL_KARUTAN_NPC_VOLVO, PositionX, PositionY);
		strcpy(c->ID, "무기상인 볼로");
		c->Object.Scale = 0.9f;
		break;
	}

	Setting_Monster(c, Type, PositionX, PositionY);

	return c;
}

CHARACTER* CreateHero(int Index, int Class, int Skin, float x, float y, float Rotate)
{
	CHARACTER* c = gmCharacters->GetCharacter(Index);

	if (c)
	{
		OBJECT* o = &c->Object;
		CreateCharacterPointer(c, MODEL_PLAYER, 0, 0, Rotate);
		Vector(0.3f, 0.3f, 0.3f, o->Light);
		c->Key = Index;
		o->Position[0] = x;
		o->Position[1] = y;
		c->Class = Class;
		c->Skin = Skin;
		g_CharacterClearBuff(o);

		if (SceneFlag == LOG_IN_SCENE && gmProtect->SceneLogin != 1)
		{
			c->BodyPart[BODYPART_HELM].Type = MODEL_HELM + 28;
			c->BodyPart[BODYPART_HELM].Level = 7;
			c->BodyPart[BODYPART_ARMOR].Type = MODEL_ARMOR + 28;
			c->BodyPart[BODYPART_ARMOR].Level = 7;
			c->BodyPart[BODYPART_PANTS].Type = MODEL_PANTS + 28;
			c->BodyPart[BODYPART_PANTS].Level = 7;
			c->BodyPart[BODYPART_GLOVES].Type = MODEL_GLOVES + 28;
			c->BodyPart[BODYPART_GLOVES].Level = 7;
			c->BodyPart[BODYPART_BOOTS].Type = MODEL_BOOTS + 28;
			c->BodyPart[BODYPART_BOOTS].Level = 7;
			c->Weapon[0].Type = MODEL_MACE + 14;
			c->Weapon[1].Type = MODEL_HELPER + 5;
			c->Weapon[0].Level = 13;
			c->Wing.Type = MODEL_HELPER + 30;
			c->Wing.ClothType = 0;
			c->Helper.Type = MODEL_HELPER + 4;
			//c->Helper.Level = 13;
		}
		else
		{
			c->BodyPart[BODYPART_HELM].Type = MODEL_BODY_HELM + gCharacterManager.GetBaseClass(Class);
			c->BodyPart[BODYPART_ARMOR].Type = MODEL_BODY_ARMOR + gCharacterManager.GetBaseClass(Class);
			c->BodyPart[BODYPART_PANTS].Type = MODEL_BODY_PANTS + gCharacterManager.GetBaseClass(Class);
			c->BodyPart[BODYPART_GLOVES].Type = MODEL_BODY_GLOVES + gCharacterManager.GetBaseClass(Class);
			c->BodyPart[BODYPART_BOOTS].Type = MODEL_BODY_BOOTS + gCharacterManager.GetBaseClass(Class);
			c->Weapon[0].Type = -1;
			c->Weapon[1].Type = -1;
			c->Wing.Type = -1;
			c->Wing.ClothType = 0;
			c->Helper.Type = -1;
		}
		c->MuunHelper[0].Type = -1;
		c->MuunHelper[1].Type = -1;

		UnRegisterBuff(eBuff_GMEffect, o);

		c->CtlCode = 0;
		SetCharacterScale(c);
		SetPlayerStop(c);
	}
	return c;
}

CHARACTER* CreateHellGate(BYTE* ID, int Key, int Index, int x, int y, int CreateFlag)
{
	CHARACTER* portal = CreateMonster(Index, x, y, Key);
	portal->Level = Index - 152 + 1;
	char Text[100];
	wsprintf(Text, portal->ID, ID);

	if (portal->Level == 7)
		portal->Object.SubType = 1;

	memcpy(portal->ID, Text, sizeof(char) * 32);

	if (CreateFlag)
	{
		CreateJoint(BITMAP_JOINT_THUNDER + 1, portal->Object.Position, portal->Object.Position, portal->Object.Angle, 1, NULL, 60.f + rand() % 10);
		CreateJoint(BITMAP_JOINT_THUNDER + 1, portal->Object.Position, portal->Object.Position, portal->Object.Angle, 1, NULL, 50.f + rand() % 10);
		CreateJoint(BITMAP_JOINT_THUNDER + 1, portal->Object.Position, portal->Object.Position, portal->Object.Angle, 1, NULL, 50.f + rand() % 10);
		CreateJoint(BITMAP_JOINT_THUNDER + 1, portal->Object.Position, portal->Object.Position, portal->Object.Angle, 1, NULL, 60.f + rand() % 10);
	}
	return portal;
}

BOOL PlayMonsterSoundGlobal(OBJECT* pObject)
{
	float fDis_x, fDis_y;
	fDis_x = pObject->Position[0] - Hero->Object.Position[0];
	fDis_y = pObject->Position[1] - Hero->Object.Position[1];
	float fDistance = sqrtf(fDis_x * fDis_x + fDis_y * fDis_y);

	if (fDistance > 500.0f)
		return true;

	switch (pObject->Type)
	{
	case MODEL_MONSTER01 + 155:
		if (pObject->CurrentAction == MONSTER01_STOP1)
		{
			// 			if (rand() % 10 == 0)
			{
				if (rand() % 2 == 0)
					PlayBuffer(SOUND_XMAS_SANTA_IDLE_1);
				else
					PlayBuffer(SOUND_XMAS_SANTA_IDLE_2);
			}
		}
		else if (pObject->CurrentAction == MONSTER01_WALK)
		{
			//if (rand() % 10 == 0)
			{
				if (rand() % 2 == 0)
					PlayBuffer(SOUND_XMAS_SANTA_WALK_1);
				else
					PlayBuffer(SOUND_XMAS_SANTA_WALK_2);
			}
		}
		else if (pObject->CurrentAction == MONSTER01_ATTACK1 || pObject->CurrentAction == MONSTER01_ATTACK2)
		{
			PlayBuffer(SOUND_XMAS_SANTA_ATTACK_1);
		}
		else if (pObject->CurrentAction == MONSTER01_SHOCK)
		{
			if (rand() % 2 == 0)
				PlayBuffer(SOUND_XMAS_SANTA_DAMAGE_1);
			else
				PlayBuffer(SOUND_XMAS_SANTA_DAMAGE_2);
		}
		else if (pObject->CurrentAction == MONSTER01_DIE)
		{
			PlayBuffer(SOUND_XMAS_SANTA_DEATH_1);
		}
		return TRUE;

	case MODEL_XMAS2008_SNOWMAN:
		if (pObject->CurrentAction == MONSTER01_WALK)
		{
			PlayBuffer(SOUND_XMAS_SNOWMAN_WALK_1);
		}
		else if (pObject->CurrentAction == MONSTER01_ATTACK1)
		{
			PlayBuffer(SOUND_XMAS_SNOWMAN_ATTACK_1);
		}
		else if (pObject->CurrentAction == MONSTER01_ATTACK2)
		{
			PlayBuffer(SOUND_XMAS_SNOWMAN_ATTACK_2);
		}
		else if (pObject->CurrentAction == MONSTER01_SHOCK)
		{
			PlayBuffer(SOUND_XMAS_SNOWMAN_DAMAGE_1);
		}
		else if (pObject->CurrentAction == MONSTER01_DIE)
		{
			if (pObject->LifeTime == 100)
			{
				PlayBuffer(SOUND_XMAS_SNOWMAN_DEATH_1);
			}
		}
		return TRUE;
	case MODEL_DUEL_NPC_TITUS:
		if (pObject->CurrentAction == MONSTER01_STOP1)
		{
			PlayBuffer(SOUND_DUEL_NPC_IDLE_1);
		}
		return TRUE;
	case MODEL_DOPPELGANGER_NPC_LUGARD:
		if (pObject->CurrentAction == MONSTER01_STOP1)
		{
			if (rand() % 2 == 0)
				PlayBuffer(SOUND_DOPPELGANGER_LUGARD_BREATH);
		}
		return TRUE;
	case MODEL_DOPPELGANGER_NPC_BOX:
		if (pObject->CurrentAction == MONSTER01_DIE)
		{
			PlayBuffer(SOUND_DOPPELGANGER_JEWELBOX_OPEN);
		}
		return TRUE;
	case MODEL_DOPPELGANGER_NPC_GOLDENBOX:
		if (pObject->CurrentAction == MONSTER01_DIE)
		{
			PlayBuffer(SOUND_DOPPELGANGER_JEWELBOX_OPEN);
		}
		return TRUE;
	}

	return FALSE;
}

bool IsBackItem(CHARACTER* c, int iType)
{
	if (gCharacterManager.GetEquipedBowType(c))
	{
		return true;
	}
	else
	{
		return (iType >= MODEL_SWORD && iType < MODEL_HELM && (iType < (MODEL_STAFF + 21) || iType >(MODEL_STAFF + 29)) && iType != (MODEL_STAFF + 40));
	}
}

bool RenderCharacterBackItem(CHARACTER* c, OBJECT* o, bool bTranslate)
{
	bool bBindBack = false;

	if (c->SafeZone)
		bBindBack = true;

	if (o->CurrentAction >= PLAYER_GREETING1 && o->CurrentAction <= PLAYER_SALUTE1)
		bBindBack = true;

	if ((World == WD_7ATLANSE || gMapManager->InHellas() || World == WD_67DOPPLEGANGER3) && (o->CurrentAction == PLAYER_WALK_SWIM || o->CurrentAction == PLAYER_RUN_SWIM))
		bBindBack = true;

	if (gMapManager->InBloodCastle() == true)
	{
		bBindBack = false;
		//if (IsGMCharacter() == true)
			//return bBindBack;
	}

	if (o->Type == MODEL_PLAYER)
	{
		bool bBack = false;
		int iBackupType = -1;

		for (int i = 0; i < 2; ++i)
		{
			int iType = c->Weapon[i].Type;
			int iLevel = c->Weapon[i].Level;
			int iOption1 = c->Weapon[i].Option1;

			if (iType < 0 || iType == MODEL_HELPER + 5)
				continue;

			if (o->Kind == KIND_NPC
				&& !World
				&& o->Type == MODEL_PLAYER
				&& o->SubType >= MODEL_SKELETON1 && o->SubType <= MODEL_SKELETON3
				&& !i)
			{
				bBack = true;
				iType = (MODEL_BOW + 9);
				iLevel = 8;
			}

			int weapon02Kind2 = GMItemMng->GetKind2(iType - MODEL_ITEM);

			if (weapon02Kind2 != ItemKind2::BOOK && IsBackItem(c, iType) == true)
				bBack = true;

			if (weapon02Kind2 != ItemKind2::ARROW && weapon02Kind2 != ItemKind2::BOLT && weapon02Kind2 != ItemKind2::QUIVER)
			{
				if (!bBindBack)
					bBack = false;
			}
			else
			{
				bBack = true;
			}

			if (bBack && iType != -1)
			{
				PART_t* w = &c->Wing;
				float fAnimationFrameBackUp = w->AnimationFrame;

				w->LinkBone = 47;
				if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
					w->PlaySpeed = 0.25f;
				else
					w->PlaySpeed = 1.f;

				PART_t t_Part;

				if (iType >= MODEL_SWORD && iType < MODEL_HELM)
				{
					::memcpy(&t_Part, w, sizeof(PART_t));
					t_Part.CurrentAction = 0;
					t_Part.AnimationFrame = 0.f;
					t_Part.PlaySpeed = 0.f;
					t_Part.PriorAction = 0;
					t_Part.PriorAnimationFrame = 0.f;
				}

				if (iType == (MODEL_BOW + 23))
				{
					PART_t* pWeapon = &c->Weapon[0];
					BYTE byTempLinkBone = pWeapon->LinkBone;
					pWeapon->CurrentAction = 2;
					pWeapon->PlaySpeed = 0.25f;
					pWeapon->LinkBone = 47;
					RenderLinkObject(0.f, 0.f, 15.f, c, pWeapon, iType, iLevel, iOption1, true, bTranslate, 0, true);
					pWeapon->LinkBone = byTempLinkBone;
				}
				else if (g_CMonkSystem.IsSwordformGloves(iType))
				{
					g_CMonkSystem.RenderSwordformGloves(c, iType, i, o->Alpha, bTranslate, false);
				}
				else
				{
					bool bRightHandItem = false;
					if (i == 0)
						bRightHandItem = true;

					int weaponkind = GMItemMng->GetKind2(iType - MODEL_ITEM);

					if (weaponkind == ItemKind2::ARROW)
					{
						bRightHandItem = true;
					}
					else if (weaponkind == ItemKind2::BOW)
					{
						bRightHandItem = false;
					}
					RenderLinkObject(0.f, 0.f, 15.f, c, &t_Part, iType, iLevel, iOption1, true, bTranslate, 0, bRightHandItem);
				}
				w->AnimationFrame = fAnimationFrameBackUp;
			}
			iBackupType = iType;
		}

		if (gMapManager->InBloodCastle() && c->EtcPart)
		{
			PART_t* w = &c->Wing;

			int iType = -1;
			w->LinkBone = 47;

			if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
				w->PlaySpeed = 0.25f;
			else
				w->PlaySpeed = 1.0f;

			switch (c->EtcPart)
			{
			case 1:
				iType = (MODEL_STAFF + 10);
				break;
			case 2:
				iType = (MODEL_SWORD + 19);
				break;
			case 3:
				iType = (MODEL_BOW + 18);
				break;
			}
			if (iType != -1)
			{
				RenderLinkObject(0.f, 0.f, 15.f, c, w, iType, 0, 0, true, bTranslate, 0, true);
			}
		}

		CreatePartsFactory(c);
		RenderParts(c);

		if (gMapManager->InChaosCastle() == false)
		{
			int iType = c->Wing.Type;

			if (iType != -1)
			{
				PART_t* w = &c->Wing;

				w->LinkBone = 47;

				if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
					w->PlaySpeed = 0.25;
				else if (w->Type == (MODEL_WING + 36))
					w->PlaySpeed = 0.5;
				else
					w->PlaySpeed = 1.0;

				CUSTOM_WING_INFO* fw = GMItemWings->Find(w->Type - MODEL_ITEM);

				if (w->Type == (MODEL_WING + 40) || w->Type == (MODEL_WING + 50) || (fw && fw->ModelLinkType != 0))
				{
					w->LinkBone = 19;
					RenderLinkObject(0.0, 0.0, 15.0, c, w, w->Type, w->Level, w->Option1, true, bTranslate, 0, true);
				}
				else
				{
					RenderLinkObject(0.0, 0.0, 15.0, c, w, w->Type, w->Level, w->Option1, false, bTranslate, 0, true);
				}

#ifdef SHUTDOWN_CORE_WING4
				iType = c->Core.Type;
				if (iType != -1)
				{
					w = &c->Core;

					w->LinkBone = 47;

					if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
						w->PlaySpeed = 0.25;
					else if (iType == (MODEL_WING + 36))
						w->PlaySpeed = 0.5;
					else
						w->PlaySpeed = 1.0;

					fw = GMItemWings->Find(w->Type - MODEL_ITEM);

					if (iType == (MODEL_WING + 40) || iType == (MODEL_WING + 50) || (fw && fw->ModelLinkType != 0))
					{
						w->LinkBone = 19;
						RenderLinkObject(0.0, 0.0, 15.0, c, w, iType, w->Level, w->Option1, true, bTranslate, 0, true);
					}
					else
					{
						RenderLinkObject(0.0, 0.0, 15.0, c, w, iType, w->Level, w->Option1, false, bTranslate, 0, true);
					}
				}
#endif // SHUTDOWN_CORE_WING4
			}

#ifdef SYSTEM_FLAG_NAT
			iType = c->Flag.Type;

			if (iType != -1)
			{
				PART_t* flag = &c->Flag;
			
				flag->LinkBone = 47;

				if (o->CurrentAction != PLAYER_FLY && o->CurrentAction != PLAYER_FLY_CROSSBOW)
					flag->PlaySpeed = 0.25;
				else
					flag->PlaySpeed = 1.0;

				RenderLinkObject(0.0, 10.0, -15.0, c, flag, flag->Type, flag->Level, flag->Option1, false, bTranslate, 0, true);
			}
#endif // SYSTEM_FLAG_NAT

			iType = c->Helper.Type;

			if (iType == MODEL_HELPER + 1)
			{
				PART_t* w = &c->Helper;
				w->LinkBone = 34;
				w->PlaySpeed = 0.5f;

				int iLevel = c->Helper.Level;
				int iOption1 = w->Option1;

				if (c->GetBaseClass() != CLASS_RAGEFIGHTER
					|| (c->BodyPart[BODYPART_ARMOR].Type != MODEL_ARMOR + 59
						&& c->BodyPart[BODYPART_ARMOR].Type != MODEL_ARMOR + 60
						&& c->BodyPart[BODYPART_ARMOR].Type != MODEL_ARMOR + 61
						&& c->BodyPart[BODYPART_ARMOR].Type != MODEL_ARMOR + 73
						&& c->BodyPart[BODYPART_ARMOR].Type != MODEL_ARMOR + 83))
				{
					RenderLinkObject(20.f, 0.f, 0.f, c, w, iType, iLevel, iOption1, false, bTranslate);
				}
				else
				{
					RenderLinkObject(20.f, -5.f, 20.f, c, w, iType, iLevel, iOption1, false, bTranslate);
				}
				vec3_t vRelativePos, vPos, vLight;
				Vector(20.f, 0.f, 15.f, vRelativePos);

				if (c->GetBaseClass() == CLASS_RAGEFIGHTER
					&& (c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 59
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 60
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 61
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 73
						|| c->BodyPart[BODYPART_ARMOR].Type == MODEL_ARMOR + 83))
				{
					Vector(20.f, -5.f, 35.f, vRelativePos);
				}

				BMD* pMode = gmClientModels->GetModel(o->Type);

				if (!pMode)
					return false;

				vec4_t* BoneTransform = o->GetBoneTransform(w->LinkBone);

				if (!BoneTransform)
					return false;

				pMode->TransformPosition(BoneTransform, vRelativePos, vPos, true);

				float fLuminosity = (float)(rand() % 30 + 70) * 0.01f;
				Vector(fLuminosity * 0.5f, fLuminosity * 0.f, fLuminosity * 0.f, vLight);
				CreateSprite(BITMAP_LIGHT, vPos, 1.5f, vLight, o);
			}
		}
	}

	return bBindBack;
}


void runtime_physical_cloth(CHARACTER* pCharacter, OBJECT* o, vec3_t CloakLight)
{
	int dark3chima1 = BITMAP_UNKNOWN859;
	int dark3chima2 = BITMAP_UNKNOWN860;
	int dark3chima3 = BITMAP_UNKNOWN861;
	int dark3chima4 = BITMAP_UNKNOWN862;
	int RuneWizchima1 = BITMAP_UNKNOWN863;
	int RuneWizchima2 = BITMAP_UNKNOWN864;
	int RuneWizchima4 = BITMAP_UNKNOWN865;

	if (pCharacter->Object.m_pCloth == NULL)
	{
		int numCloth = 0;
		bool create_physics = false;
		CPhysicsCloth* pCloth = NULL;

		if ((pCloth = GMPhysicsManager->physics_work_out(pCharacter, o, numCloth), pCloth))
		{
			create_physics = true;
		}

		if (pCharacter->GetBaseClass() == Dark_Wizard)
		{
			if (!pCloth)
			{
				numCloth = 1;
				pCloth = new CPhysicsCloth[numCloth];

				if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(1))
				{
					pCloth[0].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, dark3chima1, dark3chima1, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED | PCT_SHORT_SHOULDER);
					pCloth[0].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
					create_physics = true;
				}
			}
		}
		else if (pCharacter->GetBaseClass() == CLASS_DARK_LORD)
		{
			if (!pCloth)
			{
				numCloth = 4;
				if (pCharacter->Wing.Type == MODEL_WING + 40)
					numCloth = 6;
				else
					numCloth = 4;

				pCloth = new CPhysicsCloth[numCloth];

				if (pCharacter->Wing.Type == MODEL_WING + 40) //-- wings
				{
					pCloth[2].Create(o, 19, 0.0f, 8.0f, 10.0f, 10, 10, 180.0f, 180.0f, BITMAP_ROBE + 9, BITMAP_ROBE + 9, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);

					pCloth[4].Create(o, 19, 30.0f, 15.0f, 10.0f, 2, 5, 12.0f, 200.0f, BITMAP_ROBE + 10, BITMAP_ROBE + 10, PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_MASK_ALPHA);
					pCloth[4].AddCollisionSphere(0.0f, -15.0f, -20.0f, 30.0f, 2);
					pCloth[4].AddCollisionSphere(0.f, 0.0f, 0.0f, 35.0f, 17);

					pCloth[5].Create(o, 19, -30.0f, 20.0f, 10.0f, 2, 5, 12.0f, 200.0f, BITMAP_ROBE + 10, BITMAP_ROBE + 10, PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_MASK_ALPHA);
					pCloth[5].AddCollisionSphere(0.0f, -15.0f, -20.0f, 30.0f, 2);
					pCloth[5].AddCollisionSphere(0.f, 0.0f, 0.0f, 35.0f, 17);
					create_physics = true;
				}
				else if (pCharacter->Wing.Type == MODEL_WING + 130) //-- wings
				{
					pCloth[2].Create(o, 19, 0.0f, 8.0f, 10.0f, 10, 10, 100.0f, 100.0f, BITMAP_ROBE + 7, BITMAP_ROBE + 7, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_MASK_ALPHA);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);
					create_physics = true;
				}
				else if (pCharacter->Wing.Type == MODEL_HELPER + 30) //-- wings
				{
					pCloth[2].Create(o, 19, 0.0f, 8.0f, 10.0f, 10, 10, 180.0f, 180.0f, BITMAP_ROBE + 7, BITMAP_ROBE + 7, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_MASK_ALPHA);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
					pCloth[2].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
					pCloth[2].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);
					create_physics = true;
				}
			}

			//-- hair
			pCloth[0].Create(o, 20, 0.0f, 0.0f, 20.0f, 6, 5, 30.0f, 70.0f, BITMAP_ROBE + 6, BITMAP_ROBE + 6, PCT_CURVED | PCT_RUBBER2 | PCT_MASK_LIGHT | PLS_STRICTDISTANCE | PCT_SHORT_SHOULDER | PCT_NORMAL_THICKNESS | PCT_OPT_HAIR);
			pCloth[0].SetWindMinMax(10, 50);
			pCloth[0].AddCollisionSphere(-10.f, 20.0f, 20.0f, 27.0f, 17);
			pCloth[0].AddCollisionSphere(10.f, 20.0f, 20.0f, 27.0f, 17);
			//-- hair
			pCloth[1].Create(o, 20, 0.0f, 5.0f, 18.0f, 5, 5, 30.0f, 70.0f, BITMAP_ROBE + 6, BITMAP_ROBE + 6, PCT_CURVED | PCT_RUBBER2 | PCT_MASK_BLEND | PLS_STRICTDISTANCE | PCT_SHORT_SHOULDER | PCT_NORMAL_THICKNESS | PCT_OPT_HAIR);
			pCloth[1].SetWindMinMax(8, 40);
			pCloth[1].AddCollisionSphere(-10.f, 20.0f, 20.0f, 27.0f, 17);
			pCloth[1].AddCollisionSphere(10.f, 20.0f, 20.0f, 27.0f, 17);
			create_physics = true;

			if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(0))
			{
				pCloth[3].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, dark3chima1, dark3chima1, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED | PCT_SHORT_SHOULDER);
				pCloth[3].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
				create_physics = true;
			}
			else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(2))
			{
				pCloth[3].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, dark3chima3, dark3chima3, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED | PCT_SHORT_SHOULDER);
				pCloth[3].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
				create_physics = true;
			}
			else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(3))
			{
				pCloth[3].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, dark3chima4, dark3chima4, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED | PCT_SHORT_SHOULDER);
				pCloth[3].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
				create_physics = true;
			}
		}
		else if (pCharacter->GetBaseClass() == CLASS_RAGEFIGHTER)
		{
			if (!pCloth)
			{
				numCloth = (pCharacter->Wing.Type == MODEL_WING + 50) ? 3 : 1;
				pCloth = new CPhysicsCloth[numCloth];

				if (pCharacter->Wing.Type == MODEL_WING + 50)
				{
					pCloth[0].Create(o, 19, 0.0f, 15.0f, 5.0f, 10, 10, 180.0f, 170.0f, BITMAP_MANTOE, BITMAP_MANTOE, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, 20.0f, 37.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, 20.0f, 37.0f, 17);

					pCloth[1].Create(o, 19, 25.0f, 15.0f, 2.0f, 2, 5, 12.0f, 180.0f, BITMAP_MANTO01, BITMAP_MANTO01, PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_ELASTIC_RAGE_L | PCT_MASK_ALPHA);
					pCloth[1].AddCollisionSphere(0.0f, -15.0f, -20.0f, 35.0f, 2);
					pCloth[1].AddCollisionSphere(0.0f, 0.0f, 0.0f, 45.0f, 17);

					pCloth[2].Create(o, 19, -25.0f, 15.0f, 2.0f, 2, 5, 12.0f, 180.0f, BITMAP_MANTO01, BITMAP_MANTO01, PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_ELASTIC_RAGE_R | PCT_MASK_ALPHA);
					pCloth[2].AddCollisionSphere(0.0f, -15.0f, -20.0f, 35.0f, 2);
					pCloth[2].AddCollisionSphere(0.0f, 0.0f, 0.0f, 50.0f, 17);
					create_physics = true;
				}
				else if (pCharacter->Wing.Type == MODEL_WING + 135)
				{
					pCloth[0].Create(o, 19, 0.0f, 15.0f, 5.0f, 10, 10, 150.0f, 130.0f, BITMAP_NCCAPE, BITMAP_NCCAPE, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, 20.0f, 37.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, 20.0f, 37.0f, 17);
					create_physics = true;
				}
				else if (pCharacter->Wing.Type == MODEL_WING + 49)
				{
					pCloth[0].Create(o, 19, 0.0f, 15.0f, 5.0f, 10, 10, 180.0f, 170.0f, BITMAP_NCCAPE, BITMAP_NCCAPE, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, 20.0f, 37.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, 20.0f, 37.0f, 17);
					create_physics = true;
				}
			}
		}
		else
		{
			if (!pCloth)
			{
				numCloth = 1;
				pCloth = new CPhysicsCloth[numCloth];

				if (pCharacter->MonsterIndex == 55)
				{
					pCloth[0].Create(o, 19, 0.0f, 10.0f, 0.0f, 10, 10, 55.0f, 140.0f, BITMAP_ROBE + 2, BITMAP_ROBE + 2, PCT_CURVED | PCT_MASK_ALPHA);
					pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 35.0f, 17);
					pCloth[0].AddCollisionSphere(0.f, -10.0f, -40.0f, 50.0f, 19);
					create_physics = true;
				}
				else if (pCharacter->MonsterIndex == 361)
				{
					pCloth[0].Create(o, 2, 0.0f, 0.0f, 0.0f, 6, 6, 180.0f, 100.0f, BITMAP_NIGHTMARE_ROBE, BITMAP_NIGHTMARE_ROBE, PCT_CYLINDER | PCT_MASK_ALPHA);
					create_physics = true;
				}
				else
				{
					if (pCharacter->Wing.Type == MODEL_WING + 39)
					{
						pCloth[0].Create(o, 19, 0.0f, 15.0f, 0.0f, 10, 10, 120.0f, 120.0f, BITMAP_ROBE + 8, BITMAP_ROBE + 8, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
						pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
						pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
						pCloth[0].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
						pCloth[0].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);
						create_physics = true;
					}
					else if (pCharacter->GetCharacterClass() == Magic_Gladiator)
					{
						pCloth[0].Create(o, 19, 0.0f, 10.0f, 0.0f, 10, 10, 75.0f, 120.0f, BITMAP_ROBE, BITMAP_ROBE + 1, PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY);
						pCloth[0].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
						pCloth[0].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
						pCloth[0].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
						pCloth[0].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);
						create_physics = true;
					}
				}
			}
		}

		if (create_physics)
			o->m_byNumCloth = numCloth;
		else
			o->m_byNumCloth = 0;
		o->m_pCloth = (void*)pCloth;
	}
	else
	{
		CPhysicsCloth* pCloth = (CPhysicsCloth*)pCharacter->Object.m_pCloth;

		if (pCharacter->GetBaseClass() == CLASS_DARK_LORD)
		{
			if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(0)
				|| pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(2)
				|| pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(3)
				|| o->m_byNumCloth != 4)
			{
				if (o->m_byNumCloth == 3 || o->m_byNumCloth == 6)
				{
					bool Sucess = false;
					int m_iTexFront = 0;

					if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(0) && (!pCloth[3].CompareTexture(dark3chima1) || pCloth[3].GetOwner() == NULL))
					{
						Sucess = true;
						m_iTexFront = dark3chima1;
					}
					else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(2) && (!pCloth[3].CompareTexture(dark3chima3) || pCloth[3].GetOwner() == NULL))
					{
						Sucess = true;
						m_iTexFront = dark3chima3;
					}
					else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(3) && (!pCloth[3].CompareTexture(dark3chima4) || pCloth[3].GetOwner() == NULL))
					{
						Sucess = true;
						m_iTexFront = dark3chima4;
					}
					if (Sucess)
					{
						pCloth[3].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, m_iTexFront, m_iTexFront, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED | PCT_SHORT_SHOULDER);
						pCloth[3].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);

						if (o->m_byNumCloth == 3)
						{
							o->m_byNumCloth = 4;
						}
					}
				}
			}
			else if (o && o->m_pCloth)
			{
				pCloth[3].Destroy();
				o->m_byNumCloth = 3;
			}
		}
		else if (pCharacter->GetBaseClass() == Dark_Wizard)
		{
			if (o->m_byNumCloth >= 1)
			{
				if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(1))
				{
					if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(1) && !pCloth[0].CompareTexture(dark3chima2)) // dark3chima2
					{
						pCloth[0].Create(o, 17, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, dark3chima2, dark3chima2, 5126);
						pCloth[0].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
					}
				}
				else if (o && o->m_pCloth || pCloth[0].GetOwner() == NULL)
				{
					pCloth[0].Destroy();
					if (o->m_byNumCloth == 1)
						o->m_byNumCloth = 0;
				}
			}
		}
		else if (pCharacter->GetBaseClass() == Runer_Wizzard)
		{
			if (o->m_byNumCloth >= 1)
			{
				if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(0)
					|| pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(1)
					|| pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(2)
					|| pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(3))
				{
					bool Sucess = false;
					int m_iTexFront = 0;

					if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(0) && !pCloth[0].CompareTexture(RuneWizchima1))
					{
						Sucess = true;
						m_iTexFront = RuneWizchima1;
					}
					else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(1) && !pCloth[0].CompareTexture(RuneWizchima1))
					{
						Sucess = true;
						m_iTexFront = RuneWizchima1;
					}
					else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(2) && !pCloth[0].CompareTexture(RuneWizchima2))
					{
						Sucess = true;
						m_iTexFront = RuneWizchima2;
					}
					else if (pCharacter->BodyPart[BODYPART_ARMOR].Type == pCharacter->GetSkinBodyIndex(3) && !pCloth[0].CompareTexture(RuneWizchima4))
					{
						Sucess = true;
						m_iTexFront = RuneWizchima4;
					}

					if (Sucess && o->m_byNumCloth >= 1)
					{
						pCloth[0].Create(o, 2, 0.0, 12.5, 2.0, 5, 5, 65.0, 70.0, m_iTexFront, m_iTexFront, 5122);
						pCloth[0].AddCollisionSphere(0.0, 10.0, 0.0, 15.0, 2);
					}
				}
				else if (o && o->m_pCloth || pCloth[0].GetOwner() == NULL)
				{
					pCloth[0].Destroy();
					if (o->m_byNumCloth == 1)
						o->m_byNumCloth = 0;
				}
			}
		}
	}

	CPhysicsCloth* pCloth = (CPhysicsCloth*)pCharacter->Object.m_pCloth;

	if (!g_isCharacterBuff(o, eBuff_Cloaking) && pCloth != NULL)
	{
		bool physics_work = GMPhysicsManager->physics_work_out(pCharacter->Wing.Type);

		for (int i = 0; i < o->m_byNumCloth; i++)
		{
			if (pCharacter->GetBaseClass() == CLASS_DARK_LORD)
			{
				if (i == 2 && (!physics_work
					&& (pCharacter->Wing.Type != MODEL_HELPER + 30 
						&& pCharacter->Wing.Type != MODEL_WING + 40 
						&& pCharacter->Wing.Type != MODEL_WING + 130)
					&& (CloakLight[0] == 1.f && CloakLight[1] == 1.f && CloakLight[2] == 1.f)))
				{
					continue;
				}

				if ((i >= 0 && i <= 1) && g_ChangeRingMgr->CheckDarkLordHair(pCharacter->Object.SubType) == true)
				{
					continue;
				}
				if (i == 3 && g_ChangeRingMgr->CheckDarkLordHair(pCharacter->Object.SubType) == true)
				{
					continue;
				}
			}
			if (pCharacter->GetBaseClass() != CLASS_DARK_LORD)
			{
				if (pCharacter->GetBaseClass() == CLASS_RAGEFIGHTER || physics_work)
				{
					if (i == 0 && (!physics_work
						&& (pCharacter->Wing.Type != MODEL_WING + 49
							&& pCharacter->Wing.Type != MODEL_WING + 50
							&& pCharacter->Wing.Type != MODEL_WING + 135)
						&& (CloakLight[0] == 1.f && CloakLight[1] == 1.f && CloakLight[2] == 1.f)))
					{
						continue;
					}
				}
			}
			if (i == 0 && g_ChangeRingMgr->CheckDarkCloak(pCharacter->Class, pCharacter->Object.SubType) == true)
			{
				continue;
			}

			if (pCharacter->Object.SubType == MODEL_CURSEDTEMPLE_ALLIED_PLAYER || pCharacter->Object.SubType == MODEL_CURSEDTEMPLE_ILLUSION_PLAYER)
			{
				continue;
			}

			float Flag = 0.005f;
			if (g_isCharacterBuff(o, eDeBuff_Stun) || g_isCharacterBuff(o, eDeBuff_Sleep))
			{
				Flag = 0.0f;
			}

			if (pCloth[i].GetOwner() == NULL)
			{
				continue;
			}
			else if (!pCloth[i].Move2(Flag, 5))
			{
				DeleteCloth(pCharacter, o);
			}
			else
			{
				pCloth[i].Render((vec3_t*)CloakLight);
			}
		}
	}
}