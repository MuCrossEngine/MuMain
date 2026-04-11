#include "stdafx.h"
#include "CGMProtect.h"

#include "UIControls.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzOpenData.h"
#include "ZzzAI.h"
#include "ZzzInventory.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "SMD.h" 
#include "./Utilities/Log/ErrorReport.h"
#include "PhysicsManager.h"
#include "CSQuest.h"
#include "GMCrywolf1st.h"
#include "CMVP1stDirection.h"
#include "CSItemOption.h"
#include "CSChaosCastle.h"
#include "GMHellas.h"
#include "GMBattleCastle.h"
#include "CComGem.h"
#include "GM_Kanturu_3rd.h"
#include "CDirection.h"
#include "PersonalShopTitleImp.h"
#include "MoveCommandData.h"
#include "UIMng.h"

#include "MapManager.h"
#include "Event.h"
#include "ChangeRingManager.h"
#include "NewUISystem.h"
#include "w_CursedTemple.h"
#include "CameraMove.h"
#include "w_MapHeaders.h"
#include "QuestMng.h"
#include "ServerListManager.h"
#include "MonkSystem.h"
#include "CharacterManager.h"
#include "ReadScript.h"
#include "CGMMonsterMng.h"
#include "MixMgr.h"
#include "ScaleForm.h"
#include "CGMFontLayer.h"

///////////////////////////////////////////
extern BOOL g_bUseChatListBox;
///////////////////////////////////////////
extern SEASON3A::CMixRecipeMgr g_MixRecipeMgr;

void OpenPlayers()
{
	gmClientModels->AccessModel(MODEL_PLAYER, "Data\\Player\\", "Player");

	if (gmClientModels->GetPlayerModel()->NumMeshs > 0)
	{
		g_ErrorReport.Write("Player.bmd file error.\r\n");
		gwinhandle->SendWindowMessage("Player.bmd file error!!!", TRUE);
	}

	for (int i = 0; i < MAX_CLASS_PLAYER; ++i)
	{
		gmClientModels->AccessModel(MODEL_BODY_HELM + i, "Data\\Player\\", "HelmClass", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_ARMOR + i, "Data\\Player\\", "ArmorClass", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_PANTS + i, "Data\\Player\\", "PantClass", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_GLOVES + i, "Data\\Player\\", "GloveClass", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_BOOTS + i, "Data\\Player\\", "BootClass", i + 1);

		if (gCharacterManager.CharacterIsEvo3(i) == false)
		{
			gmClientModels->AccessModel(MODEL_BODY_HELM + MAX_CLASS_PLAYER + i, "Data\\Player\\", "HelmClass2", i + 1);
			gmClientModels->AccessModel(MODEL_BODY_ARMOR + MAX_CLASS_PLAYER + i, "Data\\Player\\", "ArmorClass2", i + 1);
			gmClientModels->AccessModel(MODEL_BODY_PANTS + MAX_CLASS_PLAYER + i, "Data\\Player\\", "PantClass2", i + 1);
			gmClientModels->AccessModel(MODEL_BODY_GLOVES + MAX_CLASS_PLAYER + i, "Data\\Player\\", "GloveClass2", i + 1);
			gmClientModels->AccessModel(MODEL_BODY_BOOTS + MAX_CLASS_PLAYER + i, "Data\\Player\\", "BootClass2", i + 1);
		}

		gmClientModels->AccessModel(MODEL_BODY_HELM + (MAX_CLASS_PLAYER * 2) + i, "Data\\Player\\", "HelmClass3", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_ARMOR + (MAX_CLASS_PLAYER * 2) + i, "Data\\Player\\", "ArmorClass3", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_PANTS + (MAX_CLASS_PLAYER * 2) + i, "Data\\Player\\", "PantClass3", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_GLOVES + (MAX_CLASS_PLAYER * 2) + i, "Data\\Player\\", "GloveClass3", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_BOOTS + (MAX_CLASS_PLAYER * 2) + i, "Data\\Player\\", "BootClass3", i + 1);

		gmClientModels->AccessModel(MODEL_BODY_HELM + (MAX_CLASS_PLAYER * 3) + i, "Data\\Player\\", "HelmClass4", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_ARMOR + (MAX_CLASS_PLAYER * 3) + i, "Data\\Player\\", "ArmorClass4", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_PANTS + (MAX_CLASS_PLAYER * 3) + i, "Data\\Player\\", "PantClass4", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_GLOVES + (MAX_CLASS_PLAYER * 3) + i, "Data\\Player\\", "GloveClass4", i + 1);
		gmClientModels->AccessModel(MODEL_BODY_BOOTS + (MAX_CLASS_PLAYER * 3) + i, "Data\\Player\\", "BootClass4", i + 1);
	}

	for (int i = 0; i < 10; i++)
	{
		gmClientModels->AccessModel(MODEL_HELM + i, "Data\\Player\\", "HelmMale", i + 1);
		gmClientModels->AccessModel(MODEL_ARMOR + i, "Data\\Player\\", "ArmorMale", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + i, "Data\\Player\\", "PantMale", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES + i, "Data\\Player\\", "GloveMale", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + i, "Data\\Player\\", "BootMale", i + 1);
	}

	for (int i = 0; i < 5; i++)
	{
		gmClientModels->AccessModel(MODEL_HELM + i + 10, "Data\\Player\\", "HelmElf", i + 1);
		gmClientModels->AccessModel(MODEL_ARMOR + i + 10, "Data\\Player\\", "ArmorElf", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + i + 10, "Data\\Player\\", "PantElf", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES + i + 10, "Data\\Player\\", "GloveElf", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + i + 10, "Data\\Player\\", "BootElf", i + 1);

	}
	gmClientModels->AccessModel(MODEL_ARMOR + 15, "Data\\Player\\", "ArmorMale", 16);
	gmClientModels->AccessModel(MODEL_PANTS + 15, "Data\\Player\\", "PantMale", 16);
	gmClientModels->AccessModel(MODEL_GLOVES + 15, "Data\\Player\\", "GloveMale", 16);
	gmClientModels->AccessModel(MODEL_BOOTS + 15, "Data\\Player\\", "BootMale", 16);

	gmClientModels->AccessModel(MODEL_HELM + 16, "Data\\Player\\", "HelmMale", 17);
	gmClientModels->AccessModel(MODEL_ARMOR + 16, "Data\\Player\\", "ArmorMale", 17);
	gmClientModels->AccessModel(MODEL_PANTS + 16, "Data\\Player\\", "PantMale", 17);
	gmClientModels->AccessModel(MODEL_GLOVES + 16, "Data\\Player\\", "GloveMale", 17);
	gmClientModels->AccessModel(MODEL_BOOTS + 16, "Data\\Player\\", "BootMale", 17);

	gmClientModels->AccessModel(MODEL_MASK_HELM + 0, "Data\\Player\\", "MaskHelmMale", 1);
	gmClientModels->AccessModel(MODEL_MASK_HELM + 5, "Data\\Player\\", "MaskHelmMale", 6);
	gmClientModels->AccessModel(MODEL_MASK_HELM + 6, "Data\\Player\\", "MaskHelmMale", 7);
	gmClientModels->AccessModel(MODEL_MASK_HELM + 8, "Data\\Player\\", "MaskHelmMale", 9);
	gmClientModels->AccessModel(MODEL_MASK_HELM + 9, "Data\\Player\\", "MaskHelmMale", 10);

	for (int i = 0; i < 4; i++)
	{
		if (i == 2)
		{
			gmClientModels->AccessModel(MODEL_HELM + 17 + i, "Data\\Player\\", "HelmMaleTest", 18 + i);
			gmClientModels->AccessModel(MODEL_ARMOR + 17 + i, "Data\\Player\\", "ArmorMaleTest", 18 + i);
			gmClientModels->AccessModel(MODEL_PANTS + 17 + i, "Data\\Player\\", "PantMaleTest", 18 + i);
			gmClientModels->AccessModel(MODEL_GLOVES + 17 + i, "Data\\Player\\", "GloveMaleTest", 18 + i);
			gmClientModels->AccessModel(MODEL_BOOTS + 17 + i, "Data\\Player\\", "BootMaleTest", 18 + i);
		}
		else
		{
			if (i < 3)
				gmClientModels->AccessModel(MODEL_HELM + 17 + i, "Data\\Player\\", "HelmMale", 18 + i);
			gmClientModels->AccessModel(MODEL_ARMOR + 17 + i, "Data\\Player\\", "ArmorMale", 18 + i);

			if (i == 1)
				gmClientModels->AccessModel(MODEL_PANTS + 17 + i, "Data\\Player\\", "t_PantMale", 18 + i);
			else
				gmClientModels->AccessModel(MODEL_PANTS + 17 + i, "Data\\Player\\", "PantMale", 18 + i);

			gmClientModels->AccessModel(MODEL_GLOVES + 17 + i, "Data\\Player\\", "GloveMale", 18 + i);
			gmClientModels->AccessModel(MODEL_BOOTS + 17 + i, "Data\\Player\\", "BootMale", 18 + i);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (i != 2)
			gmClientModels->AccessModel(MODEL_HELM + 21 + i, "Data\\Player\\", "HelmMale", 22 + i);

		gmClientModels->AccessModel(MODEL_ARMOR + 21 + i, "Data\\Player\\", "ArmorMale", 22 + i);
		gmClientModels->AccessModel(MODEL_PANTS + 21 + i, "Data\\Player\\", "PantMale", 22 + i);
		gmClientModels->AccessModel(MODEL_GLOVES + 21 + i, "Data\\Player\\", "GloveMale", 22 + i);
		gmClientModels->AccessModel(MODEL_BOOTS + 21 + i, "Data\\Player\\", "BootMale", 22 + i);
	}

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->AccessModel(MODEL_HELM + 25 + i, "Data\\Player\\", "HelmMale", 26 + i);
		gmClientModels->AccessModel(MODEL_ARMOR + 25 + i, "Data\\Player\\", "ArmorMale", 26 + i);
		gmClientModels->AccessModel(MODEL_PANTS + 25 + i, "Data\\Player\\", "PantMale", 26 + i);
		gmClientModels->AccessModel(MODEL_GLOVES + 25 + i, "Data\\Player\\", "GloveMale", 26 + i);
		gmClientModels->AccessModel(MODEL_BOOTS + 25 + i, "Data\\Player\\", "BootMale", 26 + i);
	}

	for (int i = 0; i < 5; ++i)
	{
		gmClientModels->AccessModel(MODEL_ARMOR + 29 + i, "Data\\Player\\", "HDK_ArmorMale", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + 29 + i, "Data\\Player\\", "HDK_PantMale", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES + 29 + i, "Data\\Player\\", "HDK_GloveMale", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + 29 + i, "Data\\Player\\", "HDK_BootMale", i + 1);
	}

	gmClientModels->AccessModel(MODEL_HELM + 29, "Data\\Player\\", "HDK_HelmMale", 1);
	gmClientModels->AccessModel(MODEL_HELM + 30, "Data\\Player\\", "HDK_HelmMale", 2);
	gmClientModels->AccessModel(MODEL_HELM + 31, "Data\\Player\\", "HDK_HelmMale", 3);
	gmClientModels->AccessModel(MODEL_HELM + 33, "Data\\Player\\", "HDK_HelmMale", 5);

	for (int i = 0; i < 5; ++i)
	{
		gmClientModels->AccessModel(MODEL_ARMOR + 34 + i, "Data\\Player\\", "CW_ArmorMale", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + 34 + i, "Data\\Player\\", "CW_PantMale", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES + 34 + i, "Data\\Player\\", "CW_GloveMale", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + 34 + i, "Data\\Player\\", "CW_BootMale", i + 1);
	}

	gmClientModels->AccessModel(MODEL_HELM + 34, "Data\\Player\\", "CW_HelmMale", 1);
	gmClientModels->AccessModel(MODEL_HELM + 35, "Data\\Player\\", "CW_HelmMale", 2);
	gmClientModels->AccessModel(MODEL_HELM + 36, "Data\\Player\\", "CW_HelmMale", 3);
	gmClientModels->AccessModel(MODEL_HELM + 38, "Data\\Player\\", "CW_HelmMale", 5);

	for (int i = 0; i < 6; ++i)
	{
		gmClientModels->AccessModel(MODEL_HELM + 39 + i, "Data\\Player\\", "HelmMale", 40 + i);
		gmClientModels->AccessModel(MODEL_ARMOR + 39 + i, "Data\\Player\\", "ArmorMale", 40 + i);
		gmClientModels->AccessModel(MODEL_PANTS + 39 + i, "Data\\Player\\", "PantMale", 40 + i);
		gmClientModels->AccessModel(MODEL_GLOVES + 39 + i, "Data\\Player\\", "GloveMale", 40 + i);
		gmClientModels->AccessModel(MODEL_BOOTS + 39 + i, "Data\\Player\\", "BootMale", 40 + i);
	}

	for (int i = 0; i < MODEL_ITEM_COMMON_NUM; ++i)
	{
		gmClientModels->AccessModel(MODEL_HELM2 + i, "Data\\Player\\", "HelmElfC", i + 1);
		gmClientModels->AccessModel(MODEL_ARMOR2 + i, "Data\\Player\\", "ArmorElfC", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS2 + i, "Data\\Player\\", "PantElfC", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES2 + i, "Data\\Player\\", "GloveElfC", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS2 + i, "Data\\Player\\", "BootElfC", i + 1);
	}

	for (int i = 45; i <= 53; i++)
	{
		if (i != 47 && i != 48)
			gmClientModels->AccessModel(MODEL_HELM + i, "Data\\Player\\", "HelmMale", i + 1);
	}

	for (int i = 45; i <= 53; i++)
	{
		gmClientModels->AccessModel(MODEL_ARMOR + i, "Data\\Player\\", "ArmorMale", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + i, "Data\\Player\\", "PantMale", i + 1);
		gmClientModels->AccessModel(MODEL_GLOVES + i, "Data\\Player\\", "GloveMale", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + i, "Data\\Player\\", "BootMale", i + 1);
	}

	for (int i = 0; i < MODEL_ITEM_COMMONCNT_RAGEFIGHTER; ++i)
	{
		gmClientModels->AccessModel(MODEL_HELM_MONK + i, "Data\\Player\\", "HelmMonk", i + 1);
		gmClientModels->AccessModel(MODEL_ARMOR_MONK + i, "Data\\Player\\", "ArmorMonk", i + 1);
		gmClientModels->AccessModel(MODEL_PANTS_MONK + i, "Data\\Player\\", "PantMonk", i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS_MONK + i, "Data\\Player\\", "BootMonk", i + 1);
	}

	for (int i = 0; i < 3; ++i)
	{
		gmClientModels->AccessModel(MODEL_HELM + 59 + i, "Data\\Player\\", "HelmMale", 60 + i);
		gmClientModels->AccessModel(MODEL_ARMOR + 59 + i, "Data\\Player\\", "ArmorMale", 60 + i);
		gmClientModels->AccessModel(MODEL_PANTS + 59 + i, "Data\\Player\\", "PantMale", 60 + i);
		gmClientModels->AccessModel(MODEL_BOOTS + 59 + i, "Data\\Player\\", "BootMale", 60 + i);
	}

	gmClientModels->AccessModel(MODEL_HELM + 73, "Data\\Player\\", "HelmMale74", -1);
	gmClientModels->AccessModel(MODEL_ARMOR + 73, "Data\\Player\\", "ArmorMale74", -1);
	gmClientModels->AccessModel(MODEL_PANTS + 73, "Data\\Player\\", "PantMale74", -1);
	gmClientModels->AccessModel(MODEL_BOOTS + 73, "Data\\Player\\", "BootMale74", -1);

	gmClientModels->AccessModel(MODEL_HELM + 78, "Data\\Player\\", "HelmMale", 2);
	gmClientModels->AccessModel(MODEL_ARMOR + 78, "Data\\Player\\", "ArmorMale", 2);
	gmClientModels->AccessModel(MODEL_PANTS + 78, "Data\\Player\\", "PantMale", 2);
	gmClientModels->AccessModel(MODEL_GLOVES + 78, "Data\\Player\\", "GloveMale", 2);
	gmClientModels->AccessModel(MODEL_BOOTS + 78, "Data\\Player\\", "BootMale", 2);

	gmClientModels->AccessModel(MODEL_HELM + 79, "Data\\Player\\", "HelmMale", 4);
	gmClientModels->AccessModel(MODEL_ARMOR + 79, "Data\\Player\\", "ArmorMale", 4);
	gmClientModels->AccessModel(MODEL_PANTS + 79, "Data\\Player\\", "PantMale", 4);
	gmClientModels->AccessModel(MODEL_GLOVES + 79, "Data\\Player\\", "GloveMale", 4);
	gmClientModels->AccessModel(MODEL_BOOTS + 79, "Data\\Player\\", "BootMale", 4);

	gmClientModels->AccessModel(MODEL_HELM + 80, "Data\\Player\\", "HelmElf", 5);
	gmClientModels->AccessModel(MODEL_ARMOR + 80, "Data\\Player\\", "ArmorElf", 5);
	gmClientModels->AccessModel(MODEL_PANTS + 80, "Data\\Player\\", "PantElf", 5);
	gmClientModels->AccessModel(MODEL_GLOVES + 80, "Data\\Player\\", "GloveElf", 5);
	gmClientModels->AccessModel(MODEL_BOOTS + 80, "Data\\Player\\", "BootElf", 5);

	gmClientModels->AccessModel(MODEL_HELM + 81, "Data\\Player\\", "HelmMale", 26);
	gmClientModels->AccessModel(MODEL_ARMOR + 81, "Data\\Player\\", "ArmorMale", 26);
	gmClientModels->AccessModel(MODEL_PANTS + 81, "Data\\Player\\", "PantMale", 26);
	gmClientModels->AccessModel(MODEL_GLOVES + 81, "Data\\Player\\", "GloveMale", 26);
	gmClientModels->AccessModel(MODEL_BOOTS + 81, "Data\\Player\\", "BootMale", 26);

	gmClientModels->AccessModel(MODEL_HELM + 82, "Data\\Player\\", "HelmMale", 41);
	gmClientModels->AccessModel(MODEL_ARMOR + 82, "Data\\Player\\", "ArmorMale", 41);
	gmClientModels->AccessModel(MODEL_PANTS + 82, "Data\\Player\\", "PantMale", 41);
	gmClientModels->AccessModel(MODEL_GLOVES + 82, "Data\\Player\\", "GloveMale", 41);
	gmClientModels->AccessModel(MODEL_BOOTS + 82, "Data\\Player\\", "BootMale", 41);

	gmClientModels->AccessModel(MODEL_HELM + 83, "Data\\Player\\", "HelmMale", 60);
	gmClientModels->AccessModel(MODEL_ARMOR + 83, "Data\\Player\\", "ArmorMale", 60);
	gmClientModels->AccessModel(MODEL_PANTS + 83, "Data\\Player\\", "PantMale", 60);
	gmClientModels->AccessModel(MODEL_BOOTS + 83, "Data\\Player\\", "BootMale", 60);

	CPhysicsClothMesh* pCloth = new CPhysicsClothMesh[1];
	pCloth[0].Create(&Hero->Object, 2, 17, 0.0f, 0.0f, 0.0f, 5, 8, 45.0f, 85.0f, BITMAP_PANTS_G_SOUL, BITMAP_PANTS_G_SOUL, PCT_MASK_ALPHA | PCT_HEAVY | PCT_STICKED, MODEL_PANTS + 18);
	delete[] pCloth;

	for (int i = 0; i < 1; i++)
		gmClientModels->AccessModel(MODEL_SHADOW_BODY + i, "Data\\Player\\", "Shadow", i + 1);

	gmClientModels->GetPlayerModel()->BoneHead = 20;
	gmClientModels->GetPlayerModel()->BoneFoot[0] = 6;
	gmClientModels->GetPlayerModel()->BoneFoot[1] = 13;

	for (int i = PLAYER_STOP_MALE; i <= PLAYER_STOP_RIDE_WEAPON; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.28f;

	gmClientModels->GetPlayerModelAction(PLAYER_STOP_SWORD)->PlaySpeed = 0.26f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_TWO_HAND_SWORD)->PlaySpeed = 0.24f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_SPEAR)->PlaySpeed = 0.24f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_BOW)->PlaySpeed = 0.22f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_CROSSBOW)->PlaySpeed = 0.22f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_SUMMONER)->PlaySpeed = 0.24f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_WAND)->PlaySpeed = 0.30f;

	for (int i = PLAYER_WALK_MALE; i <= PLAYER_RUN_RIDE_WEAPON; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.3f;

	gmClientModels->GetPlayerModelAction(PLAYER_WALK_WAND)->PlaySpeed = 0.44f;
	gmClientModels->GetPlayerModelAction(PLAYER_RUN_WAND)->PlaySpeed = 0.76f;
	gmClientModels->GetPlayerModelAction(PLAYER_WALK_SWIM)->PlaySpeed = 0.35f;
	gmClientModels->GetPlayerModelAction(PLAYER_RUN_SWIM)->PlaySpeed = 0.35f;

	for (int i = PLAYER_DEFENSE1; i <= PLAYER_SHOCK; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.32f;

	for (int i = PLAYER_DIE1; i <= PLAYER_DIE2; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.45f;

	for (int i = PLAYER_SIT1; i < MAX_PLAYER_ACTION; i++)
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.4f;

	gmClientModels->GetPlayerModelAction(PLAYER_SHOCK)->PlaySpeed = 0.4f;
	gmClientModels->GetPlayerModelAction(PLAYER_SEE1)->PlaySpeed = 0.28f;
	gmClientModels->GetPlayerModelAction(PLAYER_SEE_FEMALE1)->PlaySpeed = 0.28f;
	gmClientModels->GetPlayerModelAction(PLAYER_HEALING1)->PlaySpeed = 0.2f;
	gmClientModels->GetPlayerModelAction(PLAYER_HEALING_FEMALE1)->PlaySpeed = 0.2f;

	gmClientModels->GetPlayerModelAction(PLAYER_JACK_1)->PlaySpeed = 0.38f;
	gmClientModels->GetPlayerModelAction(PLAYER_JACK_2)->PlaySpeed = 0.38f;

	gmClientModels->GetPlayerModelAction(PLAYER_SANTA_1)->PlaySpeed = 0.34f;
	gmClientModels->GetPlayerModelAction(PLAYER_SANTA_2)->PlaySpeed = 0.30f;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_RIDER)->PlaySpeed = 0.2f;
	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_RIDER_FLY)->PlaySpeed = 0.2f;

	gmClientModels->GetPlayerModelAction(PLAYER_STOP_TWO_HAND_SWORD_TWO)->PlaySpeed = 0.24f;
	gmClientModels->GetPlayerModelAction(PLAYER_WALK_TWO_HAND_SWORD_TWO)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_RUN_TWO_HAND_SWORD_TWO)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_TWO_HAND_SWORD_TWO)->PlaySpeed = 0.24f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_ONETOONE)->PlaySpeed = 0.45f;

	gmClientModels->GetPlayerModelAction(PLAYER_DIE1)->Loop = true;
	gmClientModels->GetPlayerModelAction(PLAYER_DIE2)->Loop = true;
	gmClientModels->GetPlayerModelAction(PLAYER_COME_UP)->Loop = true;

	gmClientModels->GetPlayerModelAction(PLAYER_SKILL_HELL_BEGIN)->Loop = true;

	gmClientModels->GetPlayerModelAction(PLAYER_DARKLORD_STAND)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_DARKLORD_WALK)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_STOP_RIDE_HORSE)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_RUN_RIDE_HORSE)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_STRIKE)->PlaySpeed = 0.2f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_TELEPORT)->PlaySpeed = 0.28f;

	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_STRIKE)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_TELEPORT)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_HORSE_SWORD)->PlaySpeed = 0.28f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_ATTACK_FLASH)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_RIDE_ATTACK_MAGIC)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_HIT_DARKHORSE)->PlaySpeed = 0.2f;

	gmClientModels->GetPlayerModelAction(PLAYER_IDLE1_DARKHORSE)->PlaySpeed = 1.0f;
	gmClientModels->GetPlayerModelAction(PLAYER_IDLE2_DARKHORSE)->PlaySpeed = 1.0f;

	for (int i = PLAYER_FENRIR_ATTACK; i <= PLAYER_FENRIR_WALK_WEAPON_LEFT; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.45f;
	}

	for (int i = PLAYER_FENRIR_RUN; i <= PLAYER_FENRIR_RUN_ONE_LEFT_ELF; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.71f;
	}

	for (int i = PLAYER_FENRIR_STAND; i <= PLAYER_FENRIR_STAND_WEAPON_LEFT; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.4f;
	}

	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_MAGIC)->PlaySpeed = 0.30f;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_TELEPORT)->PlaySpeed = 0.3f;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_SWORD)->PlaySpeed = 0.28f;
	gmClientModels->GetPlayerModelAction(PLAYER_FENRIR_ATTACK_DARKLORD_FLASH)->PlaySpeed = 0.3f;

	gmClientModels->GetPlayerModelAction(PLAYER_HIGH_SHOCK)->PlaySpeed = 0.3f;

	for (int i = PLAYER_RAGE_FENRIR; i <= PLAYER_RAGE_FENRIR_ATTACK_RIGHT; i++)
	{
		if (i >= PLAYER_RAGE_FENRIR_TWO_SWORD && i <= PLAYER_RAGE_FENRIR_ONE_LEFT)
			gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.225f;
		else
			gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.45f;
	}
	for (int i = PLAYER_RAGE_FENRIR_STAND_TWO_SWORD; i <= PLAYER_RAGE_FENRIR_STAND_ONE_LEFT; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.2f;
	}
	gmClientModels->GetPlayerModelAction(PLAYER_RAGE_FENRIR_STAND)->PlaySpeed = 0.21f;

	for (int i = PLAYER_RAGE_FENRIR_RUN; i <= PLAYER_RAGE_FENRIR_RUN_ONE_LEFT; i++)
	{
		gmClientModels->GetPlayerModelAction(i)->PlaySpeed = 0.355f;
	}
	gmClientModels->GetPlayerModelAction(PLAYER_RAGE_UNI_RUN)->PlaySpeed = 0.3f;

	gmClientModels->GetPlayerModelAction(PLAYER_RAGE_UNI_STOP_ONE_RIGHT)->PlaySpeed = 0.18f;

	gmClientModels->GetPlayerModelAction(PLAYER_STOP_RAGEFIGHTER)->PlaySpeed = 0.16f;

	gmClientModels->GetPlayerModelAction(PLAYER_STOP_FLY_BOW)->PlaySpeed = 0.28f;

	SetAttackSpeed();

	gmClientModels->AccessModel(MODEL_GM_CHARACTER, "Data\\Skill\\", "youngza");
}

void OpenPlayerTextures()
{
	//-- Skin basic evo1
	LoadBitmap("Player\\skin_barbarian_01.jpg", BITMAP_SKIN, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_wizard_01.jpg", BITMAP_SKIN + 1, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_archer_01.jpg", BITMAP_SKIN + 2, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_special_01.jpg", BITMAP_SKIN + 3, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\level_man02.jpg", BITMAP_SKIN + 4, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class603.jpg", BITMAP_SKIN + 5, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class107.jpg", BITMAP_SKIN + 6, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class108.jpg", BITMAP_SKIN + 7, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- Skin basic evo2
	LoadBitmap("Player\\level_man022.jpg", BITMAP_SKIN + 15, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\level_man01.jpg", BITMAP_SKIN + 16, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\level_man033.jpg", BITMAP_SKIN + 17, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\LevelClass207_1.jpg", BITMAP_SKIN + 18, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\level_man02.jpg", BITMAP_SKIN + 19, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class603.jpg", BITMAP_SKIN + 20, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class107.jpg", BITMAP_SKIN + 21, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class108.jpg", BITMAP_SKIN + 22, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- skin basic evo3
	LoadBitmap("Player\\skinClass301.jpg", BITMAP_SKIN + 30, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass302.jpg", BITMAP_SKIN + 31, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass303.jpg", BITMAP_SKIN + 32, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass304.jpg", BITMAP_SKIN + 33, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class305.jpg", BITMAP_SKIN + 34, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class603.jpg", BITMAP_SKIN + 35, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class207.jpg", BITMAP_SKIN + 36, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class308.jpg", BITMAP_SKIN + 37, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- skin basic evo4
	LoadBitmap("Player\\SkinClass301.jpg", BITMAP_SKIN + 45, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass302.jpg", BITMAP_SKIN + 46, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass303_4thColor.jpg", BITMAP_SKIN + 47, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass304.jpg", BITMAP_SKIN + 48, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class305.jpg", BITMAP_SKIN + 49, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class603.jpg", BITMAP_SKIN + 50, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class207.jpg", BITMAP_SKIN + 51, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass408.jpg", BITMAP_SKIN + 52, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass409.jpg", BITMAP_SKIN + 53, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass414.jpg", BITMAP_SKIN + 58, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\hair_r.jpg", BITMAP_HAIR + 4, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_r.jpg", BITMAP_HAIR + 19, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_r.jpg", BITMAP_HAIR + 34, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair_R.jpg", BITMAP_HAIR + 49, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\hair_Class603.tga", BITMAP_HAIR + 5, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class603.tga", BITMAP_HAIR + 20, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class603.tga", BITMAP_HAIR + 35, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\SkinClass406_hair.tga", BITMAP_HAIR + 50, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\hair_Class207.tga", BITMAP_HAIR + 6, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class207.tga", BITMAP_HAIR + 21, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class207.tga", BITMAP_HAIR + 36, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\TKnight_Hair.tga", BITMAP_HAIR + 51, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\hair_Class108.tga", BITMAP_HAIR + 7, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class108.tga", BITMAP_HAIR + 37, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\hair_Class108.tga", BITMAP_HAIR + 52, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- evolucion1
	LoadBitmap("Player\\HQSkinClass101.jpg", BITMAP_HQSKIN, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass102.jpg", BITMAP_HQSKIN + 1, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass103.jpg", BITMAP_HQSKIN + 2, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkin_special_01.jpg", BITMAP_HQSKIN + 3, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQlevel_man02.jpg", BITMAP_HQSKIN + 4, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass406.jpg", BITMAP_HQSKIN + 5, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass407.jpg", BITMAP_HQSKIN + 6, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass408.jpg", BITMAP_HQSKIN + 8, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\HQSkinClass109.jpg", BITMAP_HQSKIN + 8, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass110.jpg", BITMAP_HQSKIN + 9, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass111.jpg", BITMAP_HQSKIN + 10, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass112.jpg", BITMAP_HQSKIN + 11, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass113.jpg", BITMAP_HQSKIN + 12, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass114.jpg", BITMAP_HQSKIN + 13, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass115.jpg", BITMAP_HQSKIN + 14, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQlevel_man022.jpg", BITMAP_HQSKIN + 15, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass202.jpg", BITMAP_HQSKIN + 16, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQlevel_man033.jpg", BITMAP_HQSKIN + 17, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- evolucion2
	//-- summoner
	LoadBitmap("Player\\HQSkinClass406.jpg", BITMAP_HQSKIN + 20, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass406.jpg", BITMAP_HQSKIN + 35, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\HQSkinClass109.jpg", BITMAP_HQSKIN + 22, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass110.jpg", BITMAP_HQSKIN + 23, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass111.jpg", BITMAP_HQSKIN + 24, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass112.jpg", BITMAP_HQSKIN + 25, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass113.jpg", BITMAP_HQSKIN + 26, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass114.jpg", BITMAP_HQSKIN + 27, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass115.jpg", BITMAP_HQSKIN + 29, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class301.jpg", BITMAP_HQSKIN + 30, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class302.jpg", BITMAP_HQSKIN + 31, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class303.jpg", BITMAP_HQSKIN + 32, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class304.jpg", BITMAP_HQSKIN + 33, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class305.jpg", BITMAP_HQSKIN + 34, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass407.jpg", BITMAP_HQSKIN + 36, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass408.jpg", BITMAP_HQSKIN + 37, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass309.jpg", BITMAP_HQSKIN + 38, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass310.jpg", BITMAP_HQSKIN + 39, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass311.jpg", BITMAP_HQSKIN + 40, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass312.jpg", BITMAP_HQSKIN + 41, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass313.jpg", BITMAP_HQSKIN + 42, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass314.jpg", BITMAP_HQSKIN + 43, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass315.jpg", BITMAP_HQSKIN + 44, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	//-- evolucion4

	LoadBitmap("Player\\HQskinClass401.jpg", BITMAP_HQSKIN + 45, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass402.jpg", BITMAP_HQSKIN + 46, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass403.jpg", BITMAP_HQSKIN + 47, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass404.jpg", BITMAP_HQSKIN + 48, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass405.jpg", BITMAP_HQSKIN + 49, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass406.jpg", BITMAP_HQSKIN + 50, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass407.jpg", BITMAP_HQSKIN + 51, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass408.jpg", BITMAP_HQSKIN + 52, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass409.jpg", BITMAP_HQSKIN + 53, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass410.jpg", BITMAP_HQSKIN + 54, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass411.jpg", BITMAP_HQSKIN + 55, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass412.jpg", BITMAP_HQSKIN + 56, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass413.jpg", BITMAP_HQSKIN + 57, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass414.jpg", BITMAP_HQSKIN + 58, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass415.jpg", BITMAP_HQSKIN + 59, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\HQSkin2Class114.jpg", BITMAP_HQSKIN2 + 13, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkin2Class114.jpg", BITMAP_HQSKIN2 + 27, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkin2Class314.jpg", BITMAP_HQSKIN2 + 43, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkin2Class414.jpg", BITMAP_HQSKIN2 + 58, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);



	LoadBitmap("Player\\HQhair_R.jpg", BITMAP_HQHAIR + 4, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	//-- summoner
	LoadBitmap("Player\\HQhair406.tga", BITMAP_HQHAIR + 5, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair406.tga", BITMAP_HQHAIR + 20, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair406.tga", BITMAP_HQHAIR + 35, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\HQhair_R.jpg", BITMAP_HQHAIR + 32, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair_R.jpg", BITMAP_HQHAIR + 49, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair406.tga", BITMAP_HQHAIR + 50, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair407.tga", BITMAP_HQHAIR + 51, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQhair108.tga", BITMAP_HQHAIR + 52, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	LoadBitmap("Player\\HQlevel_man021.tga", BITMAP_UNKNOWN859, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQlevel_man022_2.tga", BITMAP_UNKNOWN860, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskin_Class305_2.tga", BITMAP_UNKNOWN861, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\skin_Class405_2.TGA", BITMAP_UNKNOWN862, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQSkinClass109_1.tga", BITMAP_UNKNOWN863, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass309_1.tga", BITMAP_UNKNOWN864, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Player\\HQskinClass409_1.tga", BITMAP_UNKNOWN865, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	

	for (int i = 0; i < MAX_CLASS_PLAYER; ++i)
	{
		gmClientModels->OpenTexture(MODEL_BODY_HELM + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_ARMOR + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_PANTS + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_GLOVES + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_BOOTS + i, "Player\\");

		if (gCharacterManager.CharacterIsEvo3(i) == false)
		{
			gmClientModels->OpenTexture(MODEL_BODY_HELM + MAX_CLASS_PLAYER + i, "Player\\");
			gmClientModels->OpenTexture(MODEL_BODY_ARMOR + MAX_CLASS_PLAYER + i, "Player\\");
			gmClientModels->OpenTexture(MODEL_BODY_PANTS + MAX_CLASS_PLAYER + i, "Player\\");
			gmClientModels->OpenTexture(MODEL_BODY_GLOVES + MAX_CLASS_PLAYER + i, "Player\\");
			gmClientModels->OpenTexture(MODEL_BODY_BOOTS + MAX_CLASS_PLAYER + i, "Player\\");
		}

		gmClientModels->OpenTexture(MODEL_BODY_HELM + (MAX_CLASS_PLAYER * 2) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_ARMOR + (MAX_CLASS_PLAYER * 2) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_PANTS + (MAX_CLASS_PLAYER * 2) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_GLOVES + (MAX_CLASS_PLAYER * 2) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_BOOTS + (MAX_CLASS_PLAYER * 2) + i, "Player\\");

		gmClientModels->OpenTexture(MODEL_BODY_HELM + (MAX_CLASS_PLAYER * 3) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_ARMOR + (MAX_CLASS_PLAYER * 3) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_PANTS + (MAX_CLASS_PLAYER * 3) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_GLOVES + (MAX_CLASS_PLAYER * 3) + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BODY_BOOTS + (MAX_CLASS_PLAYER * 3) + i, "Player\\");
	}

	//int nIndex;
	//for (int i = 0; i < 3; ++i)
	//{
	//	for (int j = 0; j < MAX_CLASS; ++j)
	//	{
	//		nIndex = j + MAX_CLASS * i;
	//		if (i != 1 || j != CLASS_DARK && j != CLASS_DARK_LORD && j != CLASS_RAGEFIGHTER)
	//		{
	//			gmClientModels->OpenTexture(MODEL_BODY_HELM + nIndex, "Player\\");
	//			gmClientModels->OpenTexture(MODEL_BODY_ARMOR + nIndex, "Player\\");
	//			gmClientModels->OpenTexture(MODEL_BODY_PANTS + nIndex, "Player\\");
	//			gmClientModels->OpenTexture(MODEL_BODY_GLOVES + nIndex, "Player\\");
	//			gmClientModels->OpenTexture(MODEL_BODY_BOOTS + nIndex, "Player\\");
	//		}
	//	}
	//}

	for (int i = 0; i < 17; i++)
	{
		gmClientModels->OpenTexture(MODEL_HELM + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + i, "Player\\");
	}

	for (int i = 0; i < 4; i++)
	{
		if (i != 2)
			gmClientModels->OpenTexture(MODEL_HELM + 21 + i, "Player\\");

		gmClientModels->OpenTexture(MODEL_ARMOR + 21 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 21 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 21 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 21 + i, "Player\\");
	}

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->OpenTexture(MODEL_HELM + 25 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR + 25 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 25 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 25 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 25 + i, "Player\\");
	}

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->OpenTexture(MODEL_HELM + 17 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR + 17 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 17 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 17 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 17 + i, "Player\\");
	}

	gmClientModels->OpenTexture(MODEL_MASK_HELM + 0, "Player\\");
	gmClientModels->OpenTexture(MODEL_MASK_HELM + 5, "Player\\");
	gmClientModels->OpenTexture(MODEL_MASK_HELM + 6, "Player\\");
	gmClientModels->OpenTexture(MODEL_MASK_HELM + 8, "Player\\");
	gmClientModels->OpenTexture(MODEL_MASK_HELM + 9, "Player\\");

	gmClientModels->OpenTexture(MODEL_SHADOW_BODY, "Player\\");

	LoadBitmap("Player\\Robe01.jpg", BITMAP_ROBE);
	LoadBitmap("Player\\Robe02.jpg", BITMAP_ROBE + 1);
	LoadBitmap("Player\\Robe03.tga", BITMAP_ROBE + 2);
	LoadBitmap("Player\\DarklordRobe.tga", BITMAP_ROBE + 7);
	LoadBitmap("Item\\Texture\\msword03.tga", BITMAP_ROBE + 8);
	LoadBitmap("Item\\Texture\\dl_redwings02.tga", BITMAP_ROBE + 9);
	LoadBitmap("Item\\Texture\\dl_redwings03.tga", BITMAP_ROBE + 10);

	for (int i = 0; i < 5; ++i)
	{
		gmClientModels->OpenTexture(MODEL_ARMOR + 29 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 29 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 29 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 29 + i, "Player\\");
	}
	gmClientModels->OpenTexture(MODEL_HELM + 29, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 30, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 31, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 33, "Player\\");

	for (int i = 0; i < 5; ++i)
	{
		gmClientModels->OpenTexture(MODEL_ARMOR + 34 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 34 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 34 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 34 + i, "Player\\");
	}
	gmClientModels->OpenTexture(MODEL_HELM + 34, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 35, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 36, "Player\\");
	gmClientModels->OpenTexture(MODEL_HELM + 38, "Player\\");

	char szFileName[64];

	for (int i = 0; i < 6; ++i)
	{
		gmClientModels->OpenTexture(MODEL_HELM + 39 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR + 39 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 39 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + 39 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 39 + i, "Player\\");

		::sprintf(szFileName, "Player\\InvenArmorMale%d.tga", 40 + i);
		::LoadBitmap(szFileName, BITMAP_INVEN_ARMOR + i);
		::sprintf(szFileName, "Player\\InvenPantsMale%d.tga", 40 + i);
		::LoadBitmap(szFileName, BITMAP_INVEN_PANTS + i);
	}

	LoadBitmap("Item\\Texture\\succubus_armor.tga", BITMAP_SKIN_ARMOR_SUCCUBUS, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);
	LoadBitmap("Item\\Texture\\succubus_pants2.tga", BITMAP_SKIN_PANTS_SUCCUBUS, GL_NEAREST, GL_CLAMP_TO_EDGE, true, false);

	//::sprintf(szFileName, "Player\\Item312_Armoritem.tga");
	//::LoadBitmap(szFileName, BITMAP_SKIN_ARMOR_DEVINE);
	//::sprintf(szFileName, "Player\\Item312_Pantitem.tga");
	//::LoadBitmap(szFileName, BITMAP_SKIN_PANTS_DEVINE);
	//::sprintf(szFileName, "Player\\SkinClass706_upperitem.tga");
	//::LoadBitmap(szFileName, BITMAP_SKIN_ARMOR_SUCCUBUS);
	//::sprintf(szFileName, "Player\\SkinClass706_loweritem.tga");
	//::LoadBitmap(szFileName, BITMAP_SKIN_PANTS_SUCCUBUS);

	for (int i = 0; i < MODEL_ITEM_COMMON_NUM; ++i)
	{
		gmClientModels->OpenTexture(MODEL_HELM2 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR2 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS2 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES2 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS2 + i, "Player\\");
	}

	for (int i = 45; i <= 53; i++)
	{
		if (i != 47 && i != 48)
			gmClientModels->OpenTexture(MODEL_HELM + i, "Player\\");
	}

	for (int i = 45; i <= 53; i++)
	{
		gmClientModels->OpenTexture(MODEL_ARMOR + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_GLOVES + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + i, "Player\\");
	}

	gmClientModels->OpenTexture(MODEL_GM_CHARACTER, "Skill\\");

	for (int i = 0; i < MODEL_ITEM_COMMONCNT_RAGEFIGHTER; ++i)
	{
		gmClientModels->OpenTexture(MODEL_HELM_MONK + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR_MONK + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS_MONK + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS_MONK + i, "Player\\");
	}

	for (int i = 0; i < 3; ++i)
	{
		gmClientModels->OpenTexture(MODEL_HELM + 59 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_ARMOR + 59 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_PANTS + 59 + i, "Player\\");
		gmClientModels->OpenTexture(MODEL_BOOTS + 59 + i, "Player\\");
	}

	gmClientModels->OpenTexture(MODEL_HELM + 73, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 73, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 73, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 73, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 78, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 78, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 78, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_GLOVES + 78, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 78, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 79, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 79, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 79, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_GLOVES + 79, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 79, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 80, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 80, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 80, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_GLOVES + 80, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 80, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 81, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 81, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 81, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_GLOVES + 81, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 81, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 82, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 82, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 82, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_GLOVES + 82, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 82, "Player\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELM + 83, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ARMOR + 83, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_PANTS + 83, "Player\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOOTS + 83, "Player\\", GL_REPEAT, GL_NEAREST);
}

void OpenItems()
{
	//////////////////////////////////////////////////////////////////////////
	//  MODEL_SWORD
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 17; i++)
		gmClientModels->AccessModel(MODEL_SWORD + i, "Data\\Item\\", "Sword", i + 1);

	gmClientModels->AccessModel(MODEL_SWORD + 17, "Data\\Item\\", "Sword", 18);
	gmClientModels->AccessModel(MODEL_SWORD + 18, "Data\\Item\\", "Sword", 19);
	gmClientModels->AccessModel(MODEL_SWORD + 19, "Data\\Item\\", "Sword", 20);

	gmClientModels->AccessModel(MODEL_SWORD + 20, "Data\\Item\\", "Sword", 21);
	gmClientModels->AccessModel(MODEL_SWORD + 21, "Data\\Item\\", "Sword", 22);
	gmClientModels->AccessModel(MODEL_SWORD + 31, "Data\\Item\\", "Sword", 32);

	//////////////////////////////////////////////////////////////////////////
	//	MODEL_AXE
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 9; i++)
		gmClientModels->AccessModel(MODEL_AXE + i, "Data\\Item\\", "Axe", i + 1);

	//////////////////////////////////////////////////////////////////////////
	//	MODEL_MACE
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 7; i++)
		gmClientModels->AccessModel(MODEL_MACE + i, "Data\\Item\\", "Mace", i + 1);

	gmClientModels->AccessModel(MODEL_MACE + 7, "Data\\Item\\", "Mace", 8);

	// MODEL_MACE+8,9,10,11,12
	for (int i = 0; i < 5; i++)
		gmClientModels->AccessModel(MODEL_MACE + 8 + i, "Data\\Item\\", "Mace", 9 + i);

	gmClientModels->AccessModel(MODEL_MACE + 13, "Data\\Item\\", "Saint");
	gmClientModels->AccessModel(MODEL_MACE + 21, "Data\\Item\\", "Mace", 10);

	//////////////////////////////////////////////////////////////////////////
	//	MODEL_SPEAR
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 10; i++)
		gmClientModels->AccessModel(MODEL_SPEAR + i, "Data\\Item\\", "Spear", i + 1);

	gmClientModels->AccessModel(MODEL_SPEAR + 10, "Data\\Item\\", "Spear", 11);

	//////////////////////////////////////////////////////////////////////////
	//	MODEL_SHIELD
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 15; i++)
		gmClientModels->AccessModel(MODEL_SHIELD + i, "Data\\Item\\", "Shield", i + 1);

	gmClientModels->AccessModel(MODEL_SHIELD + 15, "Data\\Item\\", "Shield", 16);
	gmClientModels->AccessModel(MODEL_SHIELD + 16, "Data\\Item\\", "Shield", 17);

	gmClientModels->AccessModel(MODEL_SHIELD + 27, "Data\\Item\\", "Shield", 15);
	gmClientModels->AccessModel(MODEL_SHIELD + 28, "Data\\Item\\", "Shield", 12);

	//////////////////////////////////////////////////////////////////////////
	//  MODEL_STAFF
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 9; i++)
		gmClientModels->AccessModel(MODEL_STAFF + i, "Data\\Item\\", "Staff", i + 1);

	gmClientModels->AccessModel(MODEL_STAFF + 9, "Data\\Item\\", "Staff", 10);
	gmClientModels->AccessModel(MODEL_STAFF + 10, "Data\\Item\\", "Staff", 11);
	gmClientModels->AccessModel(MODEL_STAFF + 11, "Data\\Item\\", "Staff", 12);

	for (int i = 14; i <= 20; ++i)
		gmClientModels->AccessModel(MODEL_STAFF + i, "Data\\Item\\", "Staff", i + 1);

	gmClientModels->AccessModel(MODEL_STAFF + 38, "Data\\Item\\", "Staff", 6);
	gmClientModels->AccessModel(MODEL_STAFF + 39, "Data\\Item\\", "Staff", 7);
	gmClientModels->AccessModel(MODEL_STAFF + 40, "Data\\Item\\", "Book_of_Neil", -1);

	//////////////////////////////////////////////////////////////////////////
	//  MODEL_BOW
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 7; i++)
		gmClientModels->AccessModel(MODEL_BOW + i, "Data\\Item\\", "Bow", i + 1);

	for (int i = 0; i < 7; i++)
		gmClientModels->AccessModel(MODEL_BOW + i + 8, "Data\\Item\\", "CrossBow", i + 1);

	gmClientModels->AccessModel(MODEL_BOW + 7, "Data\\Item\\", "Arrows", 1);
	gmClientModels->AccessModel(MODEL_BOW + 15, "Data\\Item\\", "Arrows", 2);
	gmClientModels->AccessModel(MODEL_BOW + 16, "Data\\Item\\", "CrossBow", 17);
	gmClientModels->AccessModel(MODEL_BOW + 17, "Data\\Item\\", "Bow", 18);
	gmClientModels->AccessModel(MODEL_BOW + 18, "Data\\Item\\", "Bow", 19);
	gmClientModels->AccessModel(MODEL_BOW + 19, "Data\\Item\\", "CrossBow", 20);
	gmClientModels->AccessModel(MODEL_BOW + 20, "Data\\Item\\", "Bow", 20);

	//////////////////////////////////////////////////////////////////////////
	//  MODEL_HELPER
	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < 3; i++)
		gmClientModels->AccessModel(MODEL_HELPER + i, "Data\\Player\\", "Helper", i + 1);

	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_HELPER + i + 8, "Data\\Item\\", "Ring", i + 1);

	g_ChangeRingMgr->LoadItemModel();


	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_HELPER + i + 12, "Data\\Item\\", "Necklace", i + 1);

	gmClientModels->AccessModel(MODEL_HELPER + 3, "Data\\Player\\", "Helper", 4);
	gmClientModels->AccessModel(MODEL_HELPER + 4, "Data\\Item\\", "DarkHorseHorn");
	gmClientModels->AccessModel(MODEL_HELPER + 31, "Data\\Item\\", "DarkHorseSoul");
	gmClientModels->AccessModel(MODEL_HELPER + 5, "Data\\Item\\", "SpiritBill");

	gmClientModels->AccessModel(MODEL_HELPER + 21, "Data\\Item\\", "FireRing");
	gmClientModels->AccessModel(MODEL_HELPER + 22, "Data\\Item\\", "GroundRing");
	gmClientModels->AccessModel(MODEL_HELPER + 23, "Data\\Item\\", "WindRing");
	gmClientModels->AccessModel(MODEL_HELPER + 24, "Data\\Item\\", "ManaRing");
	gmClientModels->AccessModel(MODEL_HELPER + 25, "Data\\Item\\", "IceNecklace");
	gmClientModels->AccessModel(MODEL_HELPER + 26, "Data\\Item\\", "WindNecklace");
	gmClientModels->AccessModel(MODEL_HELPER + 27, "Data\\Item\\", "WaterNecklace");
	gmClientModels->AccessModel(MODEL_HELPER + 28, "Data\\Item\\", "AgNecklace");
	gmClientModels->AccessModel(MODEL_HELPER + 29, "Data\\Item\\", "EventChaosCastle");
	gmClientModels->AccessModel(MODEL_HELPER + 7, "Data\\Item\\", "Covenant");
	gmClientModels->AccessModel(MODEL_HELPER + 11, "Data\\Item\\", "SummonBook");
	gmClientModels->AccessModel(MODEL_EVENT + 18, "Data\\Item\\", "LifeStoneItem");

	gmClientModels->AccessModel(MODEL_HELPER + 32, "Data\\Item\\", "FR_1");
	gmClientModels->AccessModel(MODEL_HELPER + 33, "Data\\Item\\", "FR_2");
	gmClientModels->AccessModel(MODEL_HELPER + 34, "Data\\Item\\", "FR_3");
	gmClientModels->AccessModel(MODEL_HELPER + 35, "Data\\Item\\", "FR_4");
	gmClientModels->AccessModel(MODEL_HELPER + 36, "Data\\Item\\", "FR_5");
	gmClientModels->AccessModel(MODEL_HELPER + 37, "Data\\Item\\", "FR_6");

	gmClientModels->AccessModel(MODEL_HELPER + 46, "Data\\Item\\", "entrancegray");
	gmClientModels->AccessModel(MODEL_HELPER + 47, "Data\\Item\\", "entrancered");
	gmClientModels->AccessModel(MODEL_HELPER + 48, "Data\\Item\\", "entrancebleu");
	gmClientModels->AccessModel(MODEL_POTION + 54, "Data\\Item\\", "juju");
	gmClientModels->AccessModel(MODEL_HELPER + 43, "Data\\Item\\", "monmark01");
	gmClientModels->AccessModel(MODEL_HELPER + 44, "Data\\Item\\", "monmark02");
	gmClientModels->AccessModel(MODEL_HELPER + 45, "Data\\Item\\", "monmark03");
	gmClientModels->AccessModel(MODEL_POTION + 53, "Data\\Item\\", "bujuck01");

	gmClientModels->AccessModel(MODEL_POTION + 58, "Data\\Item\\", "expensiveitem01");
	gmClientModels->AccessModel(MODEL_POTION + 59, "Data\\Item\\", "expensiveitem02a");
	gmClientModels->AccessModel(MODEL_POTION + 60, "Data\\Item\\", "expensiveitem02b");
	gmClientModels->AccessModel(MODEL_POTION + 61, "Data\\Item\\", "expensiveitem03a");
	gmClientModels->AccessModel(MODEL_POTION + 62, "Data\\Item\\", "expensiveitem03b");
	gmClientModels->AccessModel(MODEL_POTION + 70, "Data\\Item\\", "EPotionR");
	gmClientModels->AccessModel(MODEL_POTION + 71, "Data\\Item\\", "EPotionB");
	gmClientModels->AccessModel(MODEL_POTION + 72, "Data\\Item\\", "elitescroll_quick");
	gmClientModels->AccessModel(MODEL_POTION + 73, "Data\\Item\\", "elitescroll_depence");
	gmClientModels->AccessModel(MODEL_POTION + 74, "Data\\Item\\", "elitescroll_anger");
	gmClientModels->AccessModel(MODEL_POTION + 75, "Data\\Item\\", "elitescroll_magic");
	gmClientModels->AccessModel(MODEL_POTION + 76, "Data\\Item\\", "elitescroll_strenth");
	gmClientModels->AccessModel(MODEL_POTION + 77, "Data\\Item\\", "elitescroll_mana");

	gmClientModels->AccessModel(MODEL_HELPER + 83, "Data\\Item\\", "amulet_satan");
	gmClientModels->AccessModel(MODEL_HELPER + 84, "Data\\Item\\", "amulet_sky");
	gmClientModels->AccessModel(MODEL_HELPER + 85, "Data\\Item\\", "amulet_elf");
	gmClientModels->AccessModel(MODEL_HELPER + 86, "Data\\Item\\", "amulet_disaster");
	gmClientModels->AccessModel(MODEL_HELPER + 87, "Data\\Item\\", "amulet_cloak");
	gmClientModels->AccessModel(MODEL_HELPER + 88, "Data\\Item\\", "amulet_dragon");
	gmClientModels->AccessModel(MODEL_HELPER + 89, "Data\\Item\\", "amulet_soul");
	gmClientModels->AccessModel(MODEL_HELPER + 90, "Data\\Item\\", "amulet_spirit");
	gmClientModels->AccessModel(MODEL_HELPER + 91, "Data\\Item\\", "amulet_despair");
	gmClientModels->AccessModel(MODEL_HELPER + 92, "Data\\Item\\", "amulet_dark");

	gmClientModels->AccessModel(MODEL_HELPER + 59, "Data\\Item\\", "sealmove");
	gmClientModels->AccessModel(MODEL_HELPER + 54, "Data\\Item\\", "resetfruit_power");
	gmClientModels->AccessModel(MODEL_HELPER + 55, "Data\\Item\\", "resetfruit_quick");
	gmClientModels->AccessModel(MODEL_HELPER + 56, "Data\\Item\\", "resetfruit_strenth");
	gmClientModels->AccessModel(MODEL_HELPER + 57, "Data\\Item\\", "resetfruit_energe");
	gmClientModels->AccessModel(MODEL_HELPER + 58, "Data\\Item\\", "resetfruit_command");
	gmClientModels->AccessModel(MODEL_POTION + 78, "Data\\Item\\", "secret_power");
	gmClientModels->AccessModel(MODEL_POTION + 79, "Data\\Item\\", "secret_quick");
	gmClientModels->AccessModel(MODEL_POTION + 80, "Data\\Item\\", "secret_strenth");
	gmClientModels->AccessModel(MODEL_POTION + 81, "Data\\Item\\", "secret_energe");
	gmClientModels->AccessModel(MODEL_POTION + 82, "Data\\Item\\", "secret_command");
	gmClientModels->AccessModel(MODEL_HELPER + 60, "Data\\Item\\", "indulgence");
	gmClientModels->AccessModel(MODEL_HELPER + 61, "Data\\Item\\", "entrancepurple");
	gmClientModels->AccessModel(MODEL_POTION + 83, "Data\\Item\\", "expensiveitem04b");

	gmClientModels->AccessModel(MODEL_POTION + 145, "Data\\Item\\", "rareitem_ticket7");
	gmClientModels->AccessModel(MODEL_POTION + 146, "Data\\Item\\", "rareitem_ticket8");
	gmClientModels->AccessModel(MODEL_POTION + 147, "Data\\Item\\", "rareitem_ticket9");
	gmClientModels->AccessModel(MODEL_POTION + 148, "Data\\Item\\", "rareitem_ticket10");
	gmClientModels->AccessModel(MODEL_POTION + 149, "Data\\Item\\", "rareitem_ticket11");
	gmClientModels->AccessModel(MODEL_POTION + 150, "Data\\Item\\", "rareitem_ticket12");
	gmClientModels->AccessModel(MODEL_HELPER + 125, "Data\\Item\\", "DoppelCard");
	gmClientModels->AccessModel(MODEL_HELPER + 126, "Data\\Item\\", "BarcaCard");
	gmClientModels->AccessModel(MODEL_HELPER + 127, "Data\\Item\\", "Barca7Card");
	gmClientModels->AccessModel(MODEL_HELPER + 128, "Data\\Item\\", "HawkStatue");
	gmClientModels->AccessModel(MODEL_HELPER + 129, "Data\\Item\\", "SheepStatue");
	gmClientModels->AccessModel(MODEL_HELPER + 134, "Data\\Item\\", "horseshoe");
	gmClientModels->AccessModel(MODEL_HELPER + 130, "Data\\Item\\", "ork_cham");
	gmClientModels->AccessModel(MODEL_HELPER + 132, "Data\\Item\\", "goldenork_cham");


	gmClientModels->AccessModel(MODEL_POTION + 91, "Data\\Item\\", "alicecard");
	gmClientModels->AccessModel(MODEL_POTION + 92, "Data\\Item\\", "juju");
	gmClientModels->AccessModel(MODEL_POTION + 93, "Data\\Item\\", "juju");
	gmClientModels->AccessModel(MODEL_POTION + 95, "Data\\Item\\", "juju");
	gmClientModels->AccessModel(MODEL_POTION + 94, "Data\\Item\\", "EPotionR");

	gmClientModels->AccessModel(MODEL_POTION + 84, "Data\\Item\\", "cherrybox");
	gmClientModels->AccessModel(MODEL_POTION + 85, "Data\\Item\\", "chwine");
	gmClientModels->AccessModel(MODEL_POTION + 86, "Data\\Item\\", "chgateaux");
	gmClientModels->AccessModel(MODEL_POTION + 87, "Data\\Item\\", "chpetal");
	gmClientModels->AccessModel(MODEL_POTION + 88, "Data\\Item\\", "chbranche");
	gmClientModels->AccessModel(MODEL_POTION + 89, "Data\\Item\\", "chbranche_red");
	gmClientModels->AccessModel(MODEL_POTION + 90, "Data\\Item\\", "chbranche_yellow");

	gmClientModels->AccessModel(MODEL_HELPER + 62, "Data\\Item\\", "Curemark");
	gmClientModels->AccessModel(MODEL_HELPER + 63, "Data\\Item\\", "Holinessmark");
	gmClientModels->AccessModel(MODEL_POTION + 97, "Data\\Item\\", "battlescroll");
	gmClientModels->AccessModel(MODEL_POTION + 98, "Data\\Item\\", "strengscroll");
	gmClientModels->AccessModel(MODEL_POTION + 96, "Data\\Item\\", "strengamulet");
	gmClientModels->AccessModel(MODEL_HELPER + 64, "Data\\Item\\", "demon");
	gmClientModels->AccessModel(MODEL_HELPER + 65, "Data\\Item\\", "maria");

	gmClientModels->AccessModel(MODEL_HELPER + 80, "Data\\Item\\", "PandaPet");
	gmClientModels->AccessModel(MODEL_HELPER + 67, "Data\\Item\\", "xmas_deer");
	gmClientModels->AccessModel(MODEL_HELPER + 106, "Data\\Item\\", "pet_unicorn");
	gmClientModels->AccessModel(MODEL_HELPER + 123, "Data\\Item\\", "skeletonpet");

	gmClientModels->AccessModel(MODEL_HELPER + 69, "Data\\Item\\", "ressurection");
	gmClientModels->AccessModel(MODEL_HELPER + 70, "Data\\Item\\", "potalcharm");
	gmClientModels->AccessModel(MODEL_HELPER + 81, "Data\\Item\\", "suhocham01");
	gmClientModels->AccessModel(MODEL_HELPER + 82, "Data\\Item\\", "imteam_protect");
	gmClientModels->AccessModel(MODEL_HELPER + 93, "Data\\Item\\", "MasterSealA");
	gmClientModels->AccessModel(MODEL_HELPER + 94, "Data\\Item\\", "MasterSealB");
	gmClientModels->AccessModel(MODEL_POTION + 140, "Data\\Item\\", "strengscroll");
	gmClientModels->AccessModel(MODEL_HELPER + 49, "Data\\Item\\", "scrollpaper");
	gmClientModels->AccessModel(MODEL_HELPER + 50, "Data\\Item\\", "oath");
	gmClientModels->AccessModel(MODEL_HELPER + 51, "Data\\Item\\", "songbl");
	gmClientModels->AccessModel(MODEL_HELPER + 52, "Data\\Item\\", "condolstone");
	gmClientModels->AccessModel(MODEL_HELPER + 53, "Data\\Item\\", "condolwing");
	gmClientModels->AccessModel(MODEL_POTION + 64, "Data\\Item\\", "songss");
	gmClientModels->AccessModel(MODEL_POTION + 65, "Data\\Item\\", "deathbeamstone");
	gmClientModels->AccessModel(MODEL_POTION + 66, "Data\\Item\\", "hellhorn");
	gmClientModels->AccessModel(MODEL_POTION + 67, "Data\\Item\\", "phoenixfeather");
	gmClientModels->AccessModel(MODEL_POTION + 68, "Data\\Item\\", "Deye");

	for (int i = 0; i < 6; ++i)
		gmClientModels->AccessModel(MODEL_WING + 60 + i, "Data\\Item\\", "s30_seed");
	for (int i = 0; i < 5; ++i)
		gmClientModels->AccessModel(MODEL_WING + 70 + i, "Data\\Item\\", "s30_sphere_body", i + 1);
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 6; ++j)
			gmClientModels->AccessModel(MODEL_WING + 100 + i * 6 + j, "Data\\Item\\", "s30_sphere", i + 1);
	}

	gmClientModels->AccessModel(MODEL_HELPER + 107, "Data\\Item\\", "FatalRing");
	gmClientModels->AccessModel(MODEL_HELPER + 104, "Data\\Item\\", "injang_AG");
	gmClientModels->AccessModel(MODEL_HELPER + 105, "Data\\Item\\", "injang_SD");
	gmClientModels->AccessModel(MODEL_HELPER + 103, "Data\\Item\\", "EXPscroll");
	gmClientModels->AccessModel(MODEL_POTION + 133, "Data\\Item\\", "ESDPotion");

	gmClientModels->AccessModel(MODEL_HELPER + 109, "Data\\Item\\InGameShop\\", "PeriodRingBlue");
	gmClientModels->AccessModel(MODEL_HELPER + 110, "Data\\Item\\InGameShop\\", "PeriodRingRed");
	gmClientModels->AccessModel(MODEL_HELPER + 111, "Data\\Item\\InGameShop\\", "PeriodRingYellow");
	gmClientModels->AccessModel(MODEL_HELPER + 112, "Data\\Item\\InGameShop\\", "PeriodRingViolet");
	gmClientModels->AccessModel(MODEL_HELPER + 113, "Data\\Item\\InGameShop\\", "necklace_red");
	gmClientModels->AccessModel(MODEL_HELPER + 114, "Data\\Item\\InGameShop\\", "necklace_blue");
	gmClientModels->AccessModel(MODEL_HELPER + 115, "Data\\Item\\InGameShop\\", "necklace_green");

	gmClientModels->AccessModel(MODEL_HELPER + 116, "Data\\Item\\", "monmark02");
	gmClientModels->AccessModel(MODEL_WING + 130, "Data\\Item\\", "DarkLordRobe");

	gmClientModels->AccessModel(MODEL_WING + 131, "Data\\Item\\Ingameshop\\", "alice1wing");
	gmClientModels->AccessModel(MODEL_WING + 132, "Data\\Item\\Ingameshop\\", "elf_wing");
	gmClientModels->AccessModel(MODEL_WING + 133, "Data\\Item\\Ingameshop\\", "angel_wing");
	gmClientModels->AccessModel(MODEL_WING + 134, "Data\\Item\\Ingameshop\\", "devil_wing");
	gmClientModels->AccessModel(MODEL_HELPER + 124, "Data\\Item\\", "ChannelCard");


	for (int i = 0; i < 7; i++)
		gmClientModels->AccessModel(MODEL_POTION + i, "Data\\Item\\", "Potion", i + 1);

	gmClientModels->AccessModel(MODEL_POTION + 8, "Data\\Item\\", "Antidote", 1);
	gmClientModels->AccessModel(MODEL_POTION + 9, "Data\\Item\\", "Beer", 1);
	gmClientModels->AccessModel(MODEL_POTION + 10, "Data\\Item\\", "Scroll", 1);
	gmClientModels->AccessModel(MODEL_POTION + 11, "Data\\Item\\", "MagicBox", 1);
	gmClientModels->AccessModel(MODEL_POTION + 12, "Data\\Item\\", "Event", 1);

	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_POTION + i + 13, "Data\\Item\\", "Jewel", i + 1);

	gmClientModels->AccessModel(MODEL_POTION + 15, "Data\\Item\\", "Gold", 1);
	gmClientModels->AccessModel(MODEL_POTION + 16, "Data\\Item\\", "Jewel", 3);

	for (int i = 0; i < 3; i++)
		gmClientModels->AccessModel(MODEL_POTION + 17 + i, "Data\\Item\\", "Devil", i);

	gmClientModels->AccessModel(MODEL_POTION + 20, "Data\\Item\\", "Drink", 0);
	gmClientModels->AccessModel(MODEL_POTION + 21, "Data\\Item\\", "ConChip", 0);
	gmClientModels->AccessModel(MODEL_POTION + 31, "Data\\Item\\", "suho", -1);
	gmClientModels->AccessModel(MODEL_HELPER + 38, "Data\\Item\\", "kanneck2");
	gmClientModels->AccessModel(MODEL_POTION + 41, "Data\\Item\\", "rs");
	gmClientModels->AccessModel(MODEL_POTION + 42, "Data\\Item\\", "jos");
	gmClientModels->AccessModel(MODEL_POTION + 43, "Data\\Item\\", "LowRefineStone");
	gmClientModels->AccessModel(MODEL_POTION + 44, "Data\\Item\\", "HighRefineStone");
	gmClientModels->AccessModel(MODEL_POTION + 7, "Data\\Item\\", "SpecialPotion");

	for (int i = 0; i < 4; ++i)
	{
		gmClientModels->AccessModel(MODEL_POTION + 23 + i, "Data\\Item\\", "Quest", i);
	}
	gmClientModels->AccessModel(MODEL_POTION + 27, "Data\\Item\\", "godesteel");

	for (int i = 0; i < 2; i++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 28 + i, "Data\\Item\\", "HELLASITEM", i);
	}

	for (int i = 0; i < 3; ++i)
		gmClientModels->AccessModel(MODEL_POTION + 35 + i, "Data\\Item\\", "sdwater", i + 1);

	for (int i = 0; i < 3; ++i)
		gmClientModels->AccessModel(MODEL_POTION + 38 + i, "Data\\Item\\", "megawater", i + 1);

	gmClientModels->AccessModel(MODEL_POTION + 120, "Data\\Item\\InGameShop\\", "gold_coin");
	gmClientModels->AccessModel(MODEL_POTION + 121, "Data\\Item\\InGameShop\\", "itembox_gold");
	gmClientModels->AccessModel(MODEL_POTION + 122, "Data\\Item\\InGameShop\\", "itembox_silver");
	gmClientModels->AccessModel(MODEL_POTION + 123, "Data\\Item\\InGameShop\\", "itembox_gold");
	gmClientModels->AccessModel(MODEL_POTION + 124, "Data\\Item\\InGameShop\\", "itembox_silver");
	for (int k = 0; k < 6; k++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 134 + k, "Data\\Item\\InGameShop\\", "package_money_item");
	}

	gmClientModels->AccessModel(MODEL_WING + 30, "Data\\Item\\", "Jewel", 1);
	gmClientModels->AccessModel(MODEL_WING + 31, "Data\\Item\\", "Jewel", 2);
	gmClientModels->AccessModel(MODEL_WING + 136, "Data\\Item\\", "Jewel", 3);
	gmClientModels->AccessModel(MODEL_WING + 137, "Data\\Item\\", "jewel", 22);
	gmClientModels->AccessModel(MODEL_WING + 138, "Data\\Item\\", "suho", -1);
	gmClientModels->AccessModel(MODEL_WING + 139, "Data\\Item\\", "rs");
	gmClientModels->AccessModel(MODEL_WING + 140, "Data\\Item\\", "jos");
	gmClientModels->AccessModel(MODEL_WING + 141, "Data\\Item\\", "Jewel", 15);
	gmClientModels->AccessModel(MODEL_WING + 142, "Data\\Item\\", "LowRefineStone");
	gmClientModels->AccessModel(MODEL_WING + 143, "Data\\Item\\", "HighRefineStone");

	gmClientModels->AccessModel(MODEL_EVENT + 4, "Data\\Item\\", "MagicBox", 2);
	gmClientModels->AccessModel(MODEL_EVENT + 6, "Data\\Item\\", "MagicBox", 5);
	gmClientModels->AccessModel(MODEL_EVENT + 7, "Data\\Item\\", "Beer", 2);
	gmClientModels->AccessModel(MODEL_EVENT + 8, "Data\\Item\\", "MagicBox", 6);
	gmClientModels->AccessModel(MODEL_EVENT + 9, "Data\\Item\\", "MagicBox", 7);

	gmClientModels->AccessModel(MODEL_HELPER + 66, "Data\\Item\\", "santa_village", -1);

	gmClientModels->AccessModel(MODEL_POTION + 100, "Data\\Item\\", "coin7", -1);

	g_XmasEvent->LoadXmasEventItem();

	gmClientModels->AccessModel(MODEL_EVENT + 10, "Data\\Item\\", "MagicBox", 8);

	gmClientModels->AccessModel(MODEL_EVENT + 5, "Data\\Item\\", "MagicBox", 3);


	gmClientModels->AccessModel(MODEL_POTION + 63, "Data\\Item\\", "GM", 1);
	gmClientModels->AccessModel(MODEL_POTION + 52, "Data\\Item\\", "GM", 2);

	gmClientModels->AccessModel(MODEL_EVENT, "Data\\Item\\", "Event", 2);
	gmClientModels->AccessModel(MODEL_EVENT + 1, "Data\\Item\\", "Event", 3);

	gmClientModels->AccessModel(MODEL_POTION + 99, "Data\\Item\\", "xmasfire", -1);

	for (int i = 0; i < 4; i++)
	{
		if (i < 3)
		{
			gmClientModels->AccessModel(MODEL_HELPER + 16 + i, "Data\\Item\\", "EventBloodCastle", i);
		}
		else
		{
			gmClientModels->AccessModel(MODEL_EVENT + 11 + (i - 3), "Data\\Item\\", "EventBloodCastle", i);
		}
	}

	gmClientModels->AccessModel(MODEL_EVENT + 12, "Data\\Item\\", "QuestItem3RD", 0);
	gmClientModels->AccessModel(MODEL_EVENT + 13, "Data\\Item\\", "QuestItem3RD", 1);
	gmClientModels->AccessModel(MODEL_EVENT + 14, "Data\\Item\\", "RingOfLordEvent", 0);
	gmClientModels->AccessModel(MODEL_EVENT + 15, "Data\\Item\\", "MagicRing", 0);

	gmClientModels->AccessModel(MODEL_POTION + 22, "Data\\Item\\", "jewel", 22);

	for (int i = 0; i < 2; ++i)
	{
		gmClientModels->AccessModel(MODEL_HELPER + 14 + i, "Data\\Item\\", "Quest", 4 + i);
	}

	gmClientModels->AccessModel(MODEL_EVENT + 16, "Data\\Item\\", "DarkLordSleeve");
	gmClientModels->AccessModel(MODEL_HELPER + 30, "Data\\Item\\", "DarkLordRobe");

	for (int i = 0; i < 3; i++)
		gmClientModels->AccessModel(MODEL_WING + i, "Data\\Item\\", "Wing", i + 1);

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->AccessModel(MODEL_WING + 3 + i, "Data\\Item\\", "Wing", 4 + i);
	}

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->AccessModel(MODEL_WING + 36 + i, "Data\\Item\\", "Wing", 8 + i);
	}
	gmClientModels->AccessModel(MODEL_WING + 40, "Data\\Item\\", "DarkLordRobe02");

	for (int i = 41; i <= 43; ++i)
		gmClientModels->AccessModel(MODEL_WING + i, "Data\\Item\\", "Wing", i + 1);

	gmClientModels->AccessModel(MODEL_STAFF + 21, "Data\\Item\\", "Book_of_Sahamutt");
	gmClientModels->AccessModel(MODEL_STAFF + 22, "Data\\Item\\", "Book_of_Neil");
	gmClientModels->AccessModel(MODEL_STAFF + 23, "Data\\Item\\", "Book_of_Rargle");

	for (int i = 0; i < 9; ++i)
	{
		gmClientModels->AccessModel(MODEL_ETC + 19 + i, "Data\\Item\\", "rollofpaper");
	}

	for (int i = 0; i < 13; i++)
	{
		if (i + 7 != 15)
			gmClientModels->AccessModel(MODEL_WING + i + 7, "Data\\Item\\", "Gem", i + 1);
	}

	gmClientModels->AccessModel(MODEL_WING + 15, "Data\\Item\\", "Jewel", 15);
	gmClientModels->AccessModel(MODEL_WING + 20, "Data\\Item\\", "Gem", 14);

	gmClientModels->AccessModel(MODEL_WING + 44, "Data\\Item\\", "Gem", 6);

	gmClientModels->AccessModel(MODEL_WING + 45, "Data\\Item\\", "Gem", 6);

	gmClientModels->AccessModel(MODEL_WING + 46, "Data\\Item\\", "Gem", 6);

	gmClientModels->AccessModel(MODEL_WING + 47, "Data\\Item\\", "Gem", 6);

	for (int i = 0; i < 4; i++)
	{
		gmClientModels->AccessModel(MODEL_WING + 21 + i, "Data\\Item\\", "SkillScroll");
	}
	gmClientModels->AccessModel(MODEL_WING + 35, "Data\\Item\\", "SkillScroll");

	gmClientModels->AccessModel(MODEL_WING + 48, "Data\\Item\\", "SkillScroll");

	gmClientModels->AccessModel(MODEL_POTION + 45, "Data\\Item\\", "hobakhead");
	gmClientModels->AccessModel(MODEL_POTION + 46, "Data\\Item\\", "hellowinscroll");
	gmClientModels->AccessModel(MODEL_POTION + 47, "Data\\Item\\", "hellowinscroll");
	gmClientModels->AccessModel(MODEL_POTION + 48, "Data\\Item\\", "hellowinscroll");
	gmClientModels->AccessModel(MODEL_POTION + 49, "Data\\Item\\", "Gogi");
	gmClientModels->AccessModel(MODEL_POTION + 50, "Data\\Item\\", "pumpkincup");

	gmClientModels->AccessModel(MODEL_POTION + 32, "Data\\Item\\", "giftbox_bp");
	gmClientModels->AccessModel(MODEL_POTION + 33, "Data\\Item\\", "giftbox_br");
	gmClientModels->AccessModel(MODEL_POTION + 34, "Data\\Item\\", "giftbox_bb");

	gmClientModels->AccessModel(MODEL_EVENT + 21, "Data\\Item\\", "p03box");
	gmClientModels->AccessModel(MODEL_EVENT + 22, "Data\\Item\\", "obox02");
	gmClientModels->AccessModel(MODEL_EVENT + 23, "Data\\Item\\", "blue01");

	gmClientModels->AccessModel(MODEL_WING + 32, "Data\\Item\\", "giftbox_r");
	gmClientModels->AccessModel(MODEL_WING + 33, "Data\\Item\\", "giftbox_g");
	gmClientModels->AccessModel(MODEL_WING + 34, "Data\\Item\\", "giftbox_b");


	for (int i = 0; i < 19; i++)
		gmClientModels->AccessModel(MODEL_ETC + i, "Data\\Item\\", "Book", i + 1);

	gmClientModels->AccessModel(MODEL_ETC + 29, "Data\\Item\\", "Book", 18);

	gmClientModels->AccessModel(MODEL_ETC + 28, "Data\\Item\\", "Book", 18);


	gmClientModels->AccessModel(MODEL_SWORD + 22, "Data\\Item\\", "HDK_Sword");
	gmClientModels->AccessModel(MODEL_SWORD + 23, "Data\\Item\\", "HDK_Sword2");
	gmClientModels->AccessModel(MODEL_MACE + 14, "Data\\Item\\", "HDK_Mace");
	gmClientModels->AccessModel(MODEL_BOW + 21, "Data\\Item\\", "HDK_Bow");
	gmClientModels->AccessModel(MODEL_STAFF + 12, "Data\\Item\\", "HDK_Staff");

	gmClientModels->AccessModel(MODEL_SWORD + 24, "Data\\Item\\", "CW_Sword");
	gmClientModels->AccessModel(MODEL_SWORD + 25, "Data\\Item\\", "CW_Sword2");
	gmClientModels->AccessModel(MODEL_MACE + 15, "Data\\Item\\", "CW_Mace");
	gmClientModels->AccessModel(MODEL_BOW + 22, "Data\\Item\\", "CW_Bow");
	gmClientModels->AccessModel(MODEL_STAFF + 13, "Data\\Item\\", "CW_Staff");

	gmClientModels->AccessModel(MODEL_SWORD + 26, "Data\\Item\\", "Sword_27");
	gmClientModels->AccessModel(MODEL_SWORD + 27, "Data\\Item\\", "Sword_28");
	gmClientModels->AccessModel(MODEL_SWORD + 28, "Data\\Item\\", "Sword_29");
	gmClientModels->AccessModel(MODEL_MACE + 16, "Data\\Item\\", "Mace_17");
	gmClientModels->AccessModel(MODEL_MACE + 17, "Data\\Item\\", "Mace_18");
	gmClientModels->AccessModel(MODEL_BOW + 23, "Data\\Item\\", "Bow_24");
	gmClientModels->AccessModel(MODEL_STAFF + 30, "Data\\Item\\", "Staff_31");
	gmClientModels->AccessModel(MODEL_STAFF + 31, "Data\\Item\\", "Staff_32");
	gmClientModels->AccessModel(MODEL_STAFF + 32, "Data\\Item\\", "Staff_33");
	gmClientModels->AccessModel(MODEL_SHIELD + 17, "Data\\Item\\", "Shield_18");
	gmClientModels->AccessModel(MODEL_SHIELD + 18, "Data\\Item\\", "Shield_19");
	gmClientModels->AccessModel(MODEL_SHIELD + 19, "Data\\Item\\", "Shield_20");
	gmClientModels->AccessModel(MODEL_SHIELD + 20, "Data\\Item\\", "Shield_21");

	gmClientModels->AccessModel(MODEL_SHIELD + 21, "Data\\Item\\", "crosssheild");

	gmClientModels->AccessModel(MODEL_BOW + 24, "Data\\Item\\", "gamblebow");
	gmClientModels->AccessModel(MODEL_STAFF + 33, "Data\\Item\\", "gamble_wand");
	gmClientModels->AccessModel(MODEL_STAFF + 34, "Data\\Item\\", "gamble_stick");
	gmClientModels->AccessModel(MODEL_SPEAR + 11, "Data\\Item\\", "gamble_scyder01");
	gmClientModels->AccessModel(MODEL_MACE + 18, "Data\\Item\\", "gamble_safter01");

	gmClientModels->AccessModel(MODEL_HELPER + 71, "Data\\Item\\", "gamble_scyderx01");
	gmClientModels->AccessModel(MODEL_HELPER + 72, "Data\\Item\\", "gamble_wand01");
	gmClientModels->AccessModel(MODEL_HELPER + 73, "Data\\Item\\", "gamble_bowx01");
	gmClientModels->AccessModel(MODEL_HELPER + 74, "Data\\Item\\", "gamble_safterx01");
	gmClientModels->AccessModel(MODEL_HELPER + 75, "Data\\Item\\", "gamble_stickx01");

	gmClientModels->AccessModel(MODEL_HELPER + 97, "Data\\Item\\Ingameshop\\", "charactercard");
	gmClientModels->AccessModel(MODEL_HELPER + 98, "Data\\Item\\Ingameshop\\", "charactercard");
	gmClientModels->AccessModel(MODEL_POTION + 91, "Data\\Item\\", "alicecard");

#ifdef PBG_ADD_CHARACTERSLOT
	gmClientModels->AccessModel(MODEL_HELPER + 99, "Data\\Item\\Ingameshop\\", "key");
	gmClientModels->AccessModel(MODEL_SLOT_LOCK, "Data\\Item\\Ingameshop\\", "lock");
#endif //PBG_ADD_CHARACTERSLOT
#ifdef PBG_ADD_SECRETITEM
#ifdef PBG_MOD_SECRETITEM
	gmClientModels->AccessModel(MODEL_HELPER + 117, "Data\\Item\\Ingameshop\\", "FRpotionD");
#else //PBG_MOD_SECRETITEM
	gmClientModels->AccessModel(MODEL_HELPER + 117, "Data\\Item\\Ingameshop\\", "FRpotionA");
#endif //PBG_MOD_SECRETITEM
	gmClientModels->AccessModel(MODEL_HELPER + 118, "Data\\Item\\Ingameshop\\", "FRpotionA");
	gmClientModels->AccessModel(MODEL_HELPER + 119, "Data\\Item\\Ingameshop\\", "FRpotionB");
	gmClientModels->AccessModel(MODEL_HELPER + 120, "Data\\Item\\Ingameshop\\", "FRpotionC");
#endif //PBG_ADD_SECRETITEM

	gmClientModels->AccessModel(MODEL_POTION + 110, "Data\\Item\\", "indication");
	gmClientModels->AccessModel(MODEL_POTION + 111, "Data\\Item\\", "speculum");

	gmClientModels->AccessModel(MODEL_POTION + 101, "Data\\Item\\", "doubt_paper");
	gmClientModels->AccessModel(MODEL_POTION + 102, "Data\\Item\\", "warrant");
	gmClientModels->AccessModel(MODEL_POTION + 109, "Data\\Item\\", "secromicon");
	for (int c = 0; c < 6; c++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 103 + c, "Data\\Item\\", "secromicon_piece");
	}

	gmClientModels->AccessModel(MODEL_POTION + 112, "Data\\Item\\Ingameshop\\", "ItemBoxKey_silver");

	gmClientModels->AccessModel(MODEL_POTION + 113, "Data\\Item\\Ingameshop\\", "ItemBoxKey_gold");

	for (int c = 0; c < 6; c++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 114 + c, "Data\\Item\\Ingameshop\\", "primium_membership_item");
	}

	for (int c = 0; c < 4; c++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 126 + c, "Data\\Item\\Ingameshop\\", "primium_membership_item");
	}

	for (int c = 0; c < 3; c++)
	{
		gmClientModels->AccessModel(MODEL_POTION + 130 + c, "Data\\Item\\Ingameshop\\", "primium_membership_item");
	}

	gmClientModels->AccessModel(MODEL_HELPER + 121, "Data\\Item\\Ingameshop\\", "entrancegreen");

	gmClientModels->AccessModel(MODEL_POTION + 141, "Data\\Item\\", "requitalbox_red");
	gmClientModels->AccessModel(MODEL_POTION + 142, "Data\\Item\\", "requitalbox_violet");
	gmClientModels->AccessModel(MODEL_POTION + 143, "Data\\Item\\", "requitalbox_blue");
	gmClientModels->AccessModel(MODEL_POTION + 144, "Data\\Item\\", "requitalbox_wood");


	gmClientModels->AccessModel(MODEL_POTION + 151, "Data\\Item\\", "rollofpaper");
	gmClientModels->AccessModel(MODEL_POTION + 152, "Data\\Item\\", "rollofpaper");
	gmClientModels->AccessModel(MODEL_POTION + 153, "Data\\Item\\", "songss", -1);
	gmClientModels->AccessModel(MODEL_POTION + 154, "Data\\Item\\", "hydra_jewel", -1);
	gmClientModels->AccessModel(MODEL_POTION + 155, "Data\\Item\\", "tan", -1);
	gmClientModels->AccessModel(MODEL_POTION + 156, "Data\\Item\\", "Mace01", -1);
	gmClientModels->AccessModel(MODEL_POTION + 157, "Data\\Item\\", "roboxgreen", -1);
	gmClientModels->AccessModel(MODEL_POTION + 158, "Data\\Item\\", "roboxred", -1);
	gmClientModels->AccessModel(MODEL_POTION + 159, "Data\\Item\\", "roboxpurple", -1);


	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_ARMLEFT, "Data\\Item\\", "class15_armleft");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_ARMRIGHT, "Data\\Item\\", "class15_armright");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_BODYLEFT, "Data\\Item\\", "class15_bodyleft");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_BODYRIGHT, "Data\\Item\\", "class15_bodyright");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_BOOTLEFT, "Data\\Item\\", "class15_bootleft");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_BOOTRIGHT, "Data\\Item\\", "class15_bootright");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_HEAD, "Data\\Item\\", "class15_head");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_PANTLEFT, "Data\\Item\\", "class15_pantleft");
	gmClientModels->AccessModel(MODEL_15GRADE_ARMOR_OBJ_PANTRIGHT, "Data\\Item\\", "class15_pantright");

	gmClientModels->AccessModel(MODEL_WING + 49, "Data\\Item\\", "Wing", 50);
	gmClientModels->AccessModel(MODEL_WING + 50, "Data\\Item\\", "Wing", 51);
	gmClientModels->AccessModel(MODEL_WING + 135, "Data\\Item\\", "Wing", 50);
	LoadBitmap("Item\\Texture\\NCcape.tga", BITMAP_NCCAPE, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\Texture\\monk_manto01.TGA", BITMAP_MANTO01, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\Texture\\monke_manto.TGA", BITMAP_MANTOE, GL_LINEAR, GL_REPEAT);

	g_CMonkSystem.LoadModelItem();

	for (int _nRollIndex = 0; _nRollIndex < 7; ++_nRollIndex)
		gmClientModels->AccessModel(MODEL_ETC + 30 + _nRollIndex, "Data\\Item\\", "rollofpaper");

	gmClientModels->AccessModel(MODEL_HELPER + 135, "Data\\Item\\LuckyItem\\", "LuckyCardgreen");
	gmClientModels->AccessModel(MODEL_HELPER + 136, "Data\\Item\\LuckyItem\\", "LuckyCardgreen");
	gmClientModels->AccessModel(MODEL_HELPER + 137, "Data\\Item\\LuckyItem\\", "LuckyCardgreen");
	gmClientModels->AccessModel(MODEL_HELPER + 138, "Data\\Item\\LuckyItem\\", "LuckyCardgreen");
	gmClientModels->AccessModel(MODEL_HELPER + 139, "Data\\Item\\LuckyItem\\", "LuckyCardgreen");
	gmClientModels->AccessModel(MODEL_HELPER + 140, "Data\\Item\\LuckyItem\\", "LuckyCardred");
	gmClientModels->AccessModel(MODEL_HELPER + 141, "Data\\Item\\LuckyItem\\", "LuckyCardred");
	gmClientModels->AccessModel(MODEL_HELPER + 142, "Data\\Item\\LuckyItem\\", "LuckyCardred");
	gmClientModels->AccessModel(MODEL_HELPER + 143, "Data\\Item\\LuckyItem\\", "LuckyCardred");
	gmClientModels->AccessModel(MODEL_HELPER + 144, "Data\\Item\\LuckyItem\\", "LuckyCardred");

	gmClientModels->AccessModel(MODEL_POTION + 160, "Data\\Item\\LuckyItem\\", "lucky_items01");
	gmClientModels->AccessModel(MODEL_POTION + 161, "Data\\Item\\LuckyItem\\", "lucky_items02");

	char	szLuckySetFileName[][50] = { "new_Helm", "new_Armor", "new_Pant", "new_Glove", "new_Boot" };
	char* szLuckySetPath = { "Data\\Player\\LuckyItem\\" };
	char	szLuckySetPathName[50] = { "" };
	int		nIndex = 62;

	for (int i = 0; i < 11; i++)
	{
		sprintf(szLuckySetPathName, "%s%d\\", szLuckySetPath, nIndex);
		if (nIndex != 71)
			gmClientModels->AccessModel(MODEL_HELM + nIndex, szLuckySetPathName, szLuckySetFileName[0], i + 1);
		gmClientModels->AccessModel(MODEL_ARMOR + nIndex, szLuckySetPathName, szLuckySetFileName[1], i + 1);
		gmClientModels->AccessModel(MODEL_PANTS + nIndex, szLuckySetPathName, szLuckySetFileName[2], i + 1);
		if (nIndex != 72)
			gmClientModels->AccessModel(MODEL_GLOVES + nIndex, szLuckySetPathName, szLuckySetFileName[3], i + 1);
		gmClientModels->AccessModel(MODEL_BOOTS + nIndex, szLuckySetPathName, szLuckySetFileName[4], i + 1);
		nIndex++;
	}

	gmClientModels->AccessModel(MODEL_POTION + 162, "Data\\Item\\", "Mbag", -1);
	gmClientModels->AccessModel(MODEL_POTION + 163, "Data\\Item\\", "EXbond", -1);
	gmClientModels->AccessModel(MODEL_POTION + 164, "Data\\Item\\", "AncientStatue", -1);
	gmClientModels->AccessModel(MODEL_POTION + 165, "Data\\Item\\", "MagicCloth", -1);
	gmClientModels->AccessModel(MODEL_POTION + 166, "Data\\Item\\", "SpaceCloth", -1);
	gmClientModels->AccessModel(MODEL_POTION + 167, "Data\\Item\\", "PhaseCloth", -1);
	gmClientModels->AccessModel(MODEL_POTION + 168, "Data\\Item\\", "DimensionCloth", -1);
	gmClientModels->AccessModel(MODEL_STAFF + 36, "Data\\Item\\", "Archangelus", -1);

	gmClientModels->GetModelMesh(MODEL_SPEAR, 1)->NoneBlendMesh;
	gmClientModels->GetModelMesh(MODEL_SPEAR, 1)->NoneBlendMesh = true;
	gmClientModels->GetModelMesh(MODEL_SWORD + 10, 1)->NoneBlendMesh = true;
	gmClientModels->GetModelMesh(MODEL_STAFF + 6, 2)->NoneBlendMesh = true;
	gmClientModels->GetModelMesh(MODEL_MACE + 6, 1)->NoneBlendMesh = true;
	gmClientModels->GetModelMesh(MODEL_EVENT + 9, 1)->NoneBlendMesh = true;
}

void OpenItemTextures()
{
	for (int i = 0; i < 4; ++i)
	{
		if (i >= 3)
			gmClientModels->OpenTexture(i + MODEL_EVENT + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		else
			gmClientModels->OpenTexture(i + MODEL_HELPER + 16, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}

	gmClientModels->OpenTexture(MODEL_EVENT + 12, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 13, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 14, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 15, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int j = 0; j < 4; ++j)
		gmClientModels->OpenTexture(j + MODEL_WING + 3, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int k = 0; k < 4; ++k)
		gmClientModels->OpenTexture(k + MODEL_WING + 36, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_WING + 40, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	LoadBitmap("Item\\Texture\\msword01_r.jpg", BITMAP_3RDWING_LAYER, GL_LINEAR, GL_REPEAT);

	for (int l = 41; l <= 43; ++l)
		gmClientModels->OpenTexture(l + MODEL_WING, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int m = 21; m <= 23; ++m)
		gmClientModels->OpenTexture(m + MODEL_STAFF, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int n = 0; n < 9; ++n)
		gmClientModels->OpenTexture(n + MODEL_ETC + 19, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\Texture\\rollofpaper_R.jpg", BITMAP_ROOLOFPAPER_EFFECT_R, GL_LINEAR);

	gmClientModels->OpenTexture(MODEL_HELPER + 4, "Skill\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 4, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 31, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 5, "Skill\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 5, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 22, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 31, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 7, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 7, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 11, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_EVENT + 18, "Monster\\", GL_REPEAT, GL_NEAREST);

	for (int ii = 0; ii < 2; ++ii)
		gmClientModels->OpenTexture(ii + MODEL_HELPER + 14, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_SWORD + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 18, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 9, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 3, "Skill\\", GL_REPEAT, GL_NEAREST);
	for (int jj = 0; jj < 4; ++jj)
		gmClientModels->OpenTexture(jj + MODEL_POTION + 23, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_POTION + 27, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int kk = 0; kk < 2; ++kk)
		gmClientModels->OpenTexture(kk + MODEL_POTION + 28, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 29, "Npc\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 19, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 10, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 18, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 19, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 31, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 15, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 16, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SPEAR + 10, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 7, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int ll = 0; ll < 17; ++ll)
	{
		gmClientModels->OpenTexture(ll + MODEL_SWORD, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_AXE, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_MACE, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_SPEAR, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_STAFF, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_SHIELD, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_BOW, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_HELPER, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_WING, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_POTION, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(ll + MODEL_ETC, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}

	for (int mm = 14; mm <= 20; ++mm)
		gmClientModels->OpenTexture(mm + MODEL_STAFF, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int nn = 21; nn <= 28; ++nn)
		gmClientModels->OpenTexture(nn + MODEL_HELPER, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i1 = 0; i1 < 5; ++i1)
		gmClientModels->OpenTexture(i1 + MODEL_MACE + 8, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i2 = 0; i2 < 2; ++i2)
		gmClientModels->OpenTexture(i2 + MODEL_SWORD + 20, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_BOW + 20, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i3 = 17; i3 < 21; ++i3)
		gmClientModels->OpenTexture(i3 + MODEL_WING, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i4 = 0; i4 < 4; ++i4)
		gmClientModels->OpenTexture(i4 + MODEL_WING + 21, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_WING + 48, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 35, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 44, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 45, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 46, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 47, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ETC + 29, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_ETC + 28, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i5 = 17; i5 < 19; ++i5)
		gmClientModels->OpenTexture(i5 + MODEL_ETC, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_ARROW, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 4, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 5, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 6, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 7, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 8, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 9, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 10, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 16, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 30, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i6 = 0; i6 < 3; ++i6)
		gmClientModels->OpenTexture(i6 + MODEL_POTION + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i7 = 0; i7 < 2; ++i7)
		gmClientModels->OpenTexture(i7 + MODEL_POTION + 20, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i8 = 0; i8 < 6; ++i8)
		gmClientModels->OpenTexture(i8 + MODEL_POTION + 35, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i9 = 0; i9 < 2; ++i9)
		gmClientModels->OpenTexture(i9 + MODEL_EVENT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_WING + 30, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 31, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 136, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 137, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 138, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 139, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 140, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 141, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 142, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 143, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 22, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 23, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 14, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 21, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 12, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 45, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 46, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 47, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 48, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 49, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 50, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 32, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 33, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 34, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_EVENT + 21, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 22, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_EVENT + 23, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 38, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 41, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 42, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 43, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 44, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 32, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 33, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 34, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 24, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 25, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 15, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 22, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 13, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 32, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 33, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 34, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 35, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 36, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 37, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 46, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 47, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 48, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 54, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 43, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 44, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 45, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 53, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i10 = 0; i10 < 5; ++i10)
		gmClientModels->OpenTexture(i10 + MODEL_POTION + 58, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_POTION + 70, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 71, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 72, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 73, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 74, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 75, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 76, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 77, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 83, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 84, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 85, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 86, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 87, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 88, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 89, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 90, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 91, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 92, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 59, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 54, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 55, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 56, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 57, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 58, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 78, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 79, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 80, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 81, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 82, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 60, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 61, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 92, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 93, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 95, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 94, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 62, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 63, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 97, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 98, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 96, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 64, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 65, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 67, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 80, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 106, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 123, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 66, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 100, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 69, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 70, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 81, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 82, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 93, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 94, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 140, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 84, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 85, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 86, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 87, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 88, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 89, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 90, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 49, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 50, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 51, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 52, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 53, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 55, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 56, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 57, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 64, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 65, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 66, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 67, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 68, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 63, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 52, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	g_ChangeRingMgr->LoadItemTexture();

	gmClientModels->OpenTexture(MODEL_POTION + 99, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 145, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 146, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 147, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 148, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 149, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 150, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i11 = 0; i11 < 9; ++i11)
		gmClientModels->OpenTexture(i11 + MODEL_POTION + 151, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 125, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 126, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 127, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 128, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 129, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 134, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 130, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 132, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 26, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 27, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 28, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 16, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 23, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 30, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 31, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 32, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 17, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 18, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 19, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 20, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i12 = 0; i12 < 6; ++i12)
		gmClientModels->OpenTexture(i12 + MODEL_WING + 60, "Effect\\", GL_REPEAT, GL_NEAREST);

	for (int i13 = 0; i13 < 5; ++i13)
		gmClientModels->OpenTexture(i13 + MODEL_WING + 70, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i14 = 0; i14 < 30; ++i14)
	{
		gmClientModels->OpenTexture(i14 + MODEL_WING + 100, "Effect\\", GL_REPEAT, GL_NEAREST);
		gmClientModels->OpenTexture(i14 + MODEL_WING + 100, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}

	gmClientModels->OpenTexture(MODEL_SHIELD + 21, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 24, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 33, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 34, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SWORD + 29, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 18, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 71, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 72, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 73, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 74, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 75, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 97, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 98, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 91, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 169, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 110, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 111, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 101, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 102, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 109, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i15 = 0; i15 < 6; ++i15)
		gmClientModels->OpenTexture(i15 + MODEL_POTION + 103, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 107, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 104, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 105, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 103, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 133, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 109, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 110, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 111, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 112, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 113, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 114, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 115, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 120, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 121, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 122, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 123, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 124, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	for (int i16 = 0; i16 < 6; ++i16)
		gmClientModels->OpenTexture(i16 + MODEL_POTION + 134, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\InGameShop\\membership_item_blue.jpg", BITMAP_PACKAGEBOX_BLUE, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\InGameShop\\membership_item_gold.jpg", BITMAP_PACKAGEBOX_GOLD, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\InGameShop\\membership_item_green.jpg", BITMAP_PACKAGEBOX_GREEN, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\InGameShop\\membership_item_pouple.jpg", BITMAP_PACKAGEBOX_PUPLE, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\InGameShop\\membership_item_red.jpg", BITMAP_PACKAGEBOX_RED, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\InGameShop\\membership_item_sky.jpg", BITMAP_PACKAGEBOX_SKY, GL_LINEAR, GL_REPEAT);

	gmClientModels->OpenTexture(MODEL_HELPER + 116, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 130, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	for (int i17 = 0; i17 < 4; ++i17)
		gmClientModels->OpenTexture(i17 + MODEL_WING + 131, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 124, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 112, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 113, "Item\\Ingameshop\\", GL_REPEAT, GL_NEAREST);

	for (int i18 = 0; i18 < 6; ++i18)
		gmClientModels->OpenTexture(i18 + MODEL_POTION + 114, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\InGameShop\\mebership_3items_green.jpg", BITMAP_INGAMESHOP_PRIMIUM6, GL_LINEAR, GL_REPEAT);

	for (int i19 = 0; i19 < 4; ++i19)
		gmClientModels->OpenTexture(i19 + MODEL_POTION + 126, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\InGameShop\\mebership_3items_red.jpg", BITMAP_INGAMESHOP_COMMUTERTICKET4, GL_LINEAR, GL_REPEAT);

	for (int i20 = 0; i20 < 3; ++i20)
		gmClientModels->OpenTexture(i20 + MODEL_POTION + 130, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\InGameShop\\mebership_3items_yellow.jpg", BITMAP_INGAMESHOP_SIZECOMMUTERTICKET3, GL_LINEAR, GL_REPEAT);
	gmClientModels->OpenTexture(MODEL_HELPER + 121, "Item\\InGameShop\\", GL_REPEAT, GL_NEAREST);

	for (int i21 = 0; i21 < 4; ++i21)
		gmClientModels->OpenTexture(i21 + MODEL_POTION + 141, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_ARMLEFT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_ARMRIGHT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_BODYLEFT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_BODYRIGHT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_BOOTLEFT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_BOOTRIGHT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_HEAD, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_PANTLEFT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_15GRADE_ARMOR_OBJ_PANTRIGHT, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	LoadBitmap("Item\\Texture\\rgb_mix.jpg", BITMAP_RGB_MIX, GL_LINEAR, GL_REPEAT);

	gmClientModels->OpenTexture(MODEL_WING + 49, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 50, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_WING + 135, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	g_CMonkSystem.LoadModelItemTexture();

	for (int i22 = 0; i22 < 7; ++i22)
		gmClientModels->OpenTexture(i22 + MODEL_ETC + 30, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	gmClientModels->OpenTexture(MODEL_HELPER + 137, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 136, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 137, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 138, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 139, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 140, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 141, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 142, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 143, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_HELPER + 144, "Item\\LuckyItem\\", GL_REPEAT, GL_NEAREST);


	char* szLuckySetPath = { "Player\\LuckyItem\\" };
	char  szLuckySetPathName[50] = { "" };
	int   nIndex = 62;

	for (int i = 0; i < 11; i++)
	{
		sprintf(szLuckySetPathName, "%s%d\\", szLuckySetPath, nIndex);
		if (nIndex != 71)
			gmClientModels->OpenTexture(MODEL_HELM + nIndex, szLuckySetPathName);

		gmClientModels->OpenTexture(MODEL_ARMOR + nIndex, szLuckySetPathName);
		gmClientModels->OpenTexture(MODEL_PANTS + nIndex, szLuckySetPathName);

		if (nIndex != 72)
			gmClientModels->OpenTexture(MODEL_GLOVES + nIndex, szLuckySetPathName);

		gmClientModels->OpenTexture(MODEL_BOOTS + nIndex, szLuckySetPathName);
		nIndex++;
	}

	gmClientModels->OpenTexture(MODEL_POTION + 160, "Item\\LuckyItem\\");
	gmClientModels->OpenTexture(MODEL_POTION + 161, "Item\\LuckyItem\\");

	sprintf(szLuckySetPathName, "Player\\LuckyItem\\65\\InvenArmorMale40_luck.tga");
	LoadBitmap(szLuckySetPathName, BITMAP_INVEN_ARMOR + 6);
	sprintf(szLuckySetPathName, "Player\\LuckyItem\\65\\InvenPantsMale40_luck.tga");
	LoadBitmap(szLuckySetPathName, BITMAP_INVEN_PANTS + 6);
	sprintf(szLuckySetPathName, "Player\\LuckyItem\\70\\InvenArmorMale41_luck.tga");
	LoadBitmap(szLuckySetPathName, BITMAP_INVEN_ARMOR + 7);
	sprintf(szLuckySetPathName, "Player\\LuckyItem\\70\\InvenPantsMale41_luck.tga");
	LoadBitmap(szLuckySetPathName, BITMAP_INVEN_PANTS + 7);

	gmClientModels->OpenTexture(MODEL_POTION + 162, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 163, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 164, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 165, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 166, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 167, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_POTION + 168, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 36, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);

	LoadBitmap("Item\\Texture\\ArchangelusWeapon_render.jpg", BITMAP_ARCHANGELUSWEAPON_RENDER, GL_LINEAR, GL_REPEAT, true, false);
	LoadBitmap("Effect\\pin_star.jpg", BITMAP_PIN_STAR, GL_LINEAR, GL_REPEAT, true, false);
	LoadBitmap("Item\\Texture\\PhoenixSoul_render.JPG", BITMAP_PHOENIXSOUL_RENDER, GL_LINEAR, GL_REPEAT, true, false);

	gmClientModels->OpenTexture(MODEL_SWORD + 39, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_MACE + 21, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_BOW + 27, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 38, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 39, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_STAFF + 40, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 27, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	gmClientModels->OpenTexture(MODEL_SHIELD + 28, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
}

void DeleteNpcs()
{
	for (int i = MODEL_NPC_BEGIN; i < MODEL_NPC_END; i++)
		gmClientModels->GetModel(i)->Release();

	for (int i = SOUND_NPC; i < SOUND_NPC_END; i++)
		ReleaseBuffer(i);
}

void OpenNpc(int Type)
{
	BMD* b = gmClientModels->GetModel(Type);

	if (b->NumActions > 0)
		return;

	switch (Type)
	{
	case MODEL_MERCHANT_FEMALE:
		gmClientModels->AccessModel(MODEL_MERCHANT_FEMALE, "Data\\Npc\\", "Female", 1);

		for (int i = 0; i < 2; i++)
		{
			gmClientModels->AccessModel(MODEL_MERCHANT_FEMALE_HEAD + i, "Data\\Npc\\", "FemaleHead", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_FEMALE_UPPER + i, "Data\\Npc\\", "FemaleUpper", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_FEMALE_LOWER + i, "Data\\Npc\\", "FemaleLower", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_FEMALE_BOOTS + i, "Data\\Npc\\", "FemaleBoots", i + 1);
			gmClientModels->OpenTexture(MODEL_MERCHANT_FEMALE_HEAD + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_FEMALE_UPPER + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_FEMALE_LOWER + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_FEMALE_BOOTS + i, "Npc\\");
		}
		break;
	case MODEL_MERCHANT_MAN:
		gmClientModels->AccessModel(MODEL_MERCHANT_MAN, "Data\\Npc\\", "Man", 1);

		for (int i = 0; i < 2; i++)
		{
			gmClientModels->AccessModel(MODEL_MERCHANT_MAN_HEAD + i, "Data\\Npc\\", "ManHead", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_MAN_UPPER + i, "Data\\Npc\\", "ManUpper", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_MAN_GLOVES + i, "Data\\Npc\\", "ManGloves", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_MAN_BOOTS + i, "Data\\Npc\\", "ManBoots", i + 1);
			gmClientModels->OpenTexture(MODEL_MERCHANT_MAN_HEAD + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_MAN_UPPER + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_MAN_GLOVES + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_MAN_BOOTS + i, "Npc\\");
		}
		break;
	case MODEL_MERCHANT_GIRL:
		gmClientModels->AccessModel(MODEL_MERCHANT_GIRL, "Data\\Npc\\", "Girl", 1);

		for (int i = 0; i < 2; i++)
		{
			gmClientModels->AccessModel(MODEL_MERCHANT_GIRL_HEAD + i, "Data\\Npc\\", "GirlHead", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_GIRL_UPPER + i, "Data\\Npc\\", "GirlUpper", i + 1);
			gmClientModels->AccessModel(MODEL_MERCHANT_GIRL_LOWER + i, "Data\\Npc\\", "GirlLower", i + 1);
			gmClientModels->OpenTexture(MODEL_MERCHANT_GIRL_HEAD + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_GIRL_UPPER + i, "Npc\\");
			gmClientModels->OpenTexture(MODEL_MERCHANT_GIRL_LOWER + i, "Npc\\");
		}
		break;
	case MODEL_SMITH:
		gmClientModels->AccessModel(MODEL_SMITH, "Data\\Npc\\", "Smith", 1);
		LoadWaveFile(SOUND_NPC, "Data\\Sound\\nBlackSmith.wav", 1);
		break;
	case MODEL_SCIENTIST:
		gmClientModels->AccessModel(MODEL_SCIENTIST, "Data\\Npc\\", "Wizard", 1);
		break;
	case MODEL_SNOW_MERCHANT:
		gmClientModels->AccessModel(MODEL_SNOW_MERCHANT, "Data\\Npc\\", "SnowMerchant", 1);
		break;
	case MODEL_SNOW_SMITH:
		gmClientModels->AccessModel(MODEL_SNOW_SMITH, "Data\\Npc\\", "SnowSmith", 1);
		break;
	case MODEL_SNOW_WIZARD:
		gmClientModels->AccessModel(MODEL_SNOW_WIZARD, "Data\\Npc\\", "SnowWizard", 1);
		break;
	case MODEL_ELF_WIZARD:
		gmClientModels->AccessModel(MODEL_ELF_WIZARD, "Data\\Npc\\", "ElfWizard", 1);
		LoadWaveFile(SOUND_NPC, "Data\\Sound\\nHarp.wav", 1);
		break;
	case MODEL_ELF_MERCHANT:
		gmClientModels->AccessModel(MODEL_ELF_MERCHANT, "Data\\Npc\\", "ElfMerchant", 1);
		break;
	case MODEL_MASTER:
		gmClientModels->AccessModel(MODEL_MASTER, "Data\\Npc\\", "Master", 1);
		break;
	case MODEL_STORAGE:
		gmClientModels->AccessModel(MODEL_STORAGE, "Data\\Npc\\", "Storage", 1);
		break;
	case MODEL_TOURNAMENT:
		gmClientModels->AccessModel(MODEL_TOURNAMENT, "Data\\Npc\\", "Tournament", 1);
		break;
	case MODEL_MIX_NPC:
		gmClientModels->AccessModel(MODEL_MIX_NPC, "Data\\Npc\\", "MixNpc", 1);
		LoadWaveFile(SOUND_NPC + 1, "Data\\Sound\\nMix.wav", 1);
		break;
	case MODEL_REFINERY_NPC:
		gmClientModels->AccessModel(MODEL_REFINERY_NPC, "Data\\Npc\\", "os");
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;
	case MODEL_RECOVERY_NPC:
		gmClientModels->AccessModel(MODEL_RECOVERY_NPC, "Data\\Npc\\", "je");
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;
	case MODEL_NPC_DEVILSQUARE:
		gmClientModels->AccessModel(MODEL_NPC_DEVILSQUARE, "Data\\Npc\\", "DevilNpc", 1);
		break;

	case MODEL_NPC_SEVINA:
		gmClientModels->AccessModel(MODEL_NPC_SEVINA, "Data\\Npc\\", "Sevina", 1);
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;
	case MODEL_NPC_ARCHANGEL:
		gmClientModels->AccessModel(MODEL_NPC_ARCHANGEL, "Data\\Npc\\", "BloodCastle", 1);
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;
	case MODEL_NPC_ARCHANGEL_MESSENGER:
		gmClientModels->AccessModel(MODEL_NPC_ARCHANGEL_MESSENGER, "Data\\Npc\\", "BloodCastle", 2);
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;

		//  데비아스, 로랜시아 추가 상점 NPC
	case MODEL_DEVIAS_TRADER:
		gmClientModels->AccessModel(MODEL_DEVIAS_TRADER, "Data\\Npc\\", "DeviasTrader", 1);
		gmClientModels->OpenTexture(Type, "Npc\\");
		break;

#ifdef _PVP_ATTACK_GUARD
	case MODEL_ANGEL:
		gmClientModels->AccessModel(MODEL_ANGEL, "Data\\Player\\", "Angel");
		gmClientModels->OpenTexture(MODEL_ANGEL, "Npc\\");
		break;
#endif	// _PVP_ATTACK_GUARD
	case MODEL_NPC_BREEDER:    //  조련사 NPC.
		gmClientModels->AccessModel(MODEL_NPC_BREEDER, "Data\\Npc\\", "Breeder");
		gmClientModels->OpenTexture(MODEL_NPC_BREEDER, "Npc\\");
		break;
#ifdef _PVP_MURDERER_HERO_ITEM
	case MODEL_HERO_SHOP:	// 영웅 상점
		gmClientModels->AccessModel(MODEL_HERO_SHOP, "Data\\Npc\\", "HeroNpc");
		gmClientModels->OpenTexture(MODEL_HERO_SHOP, "Npc\\");
		break;
#endif	// _PVP_MURDERER_HERO_ITEM

	case MODEL_NPC_CAPATULT_ATT:
		gmClientModels->AccessModel(MODEL_NPC_CAPATULT_ATT, "Data\\Npc\\", "Model_Npc_Catapult_Att");
		gmClientModels->OpenTexture(MODEL_NPC_CAPATULT_ATT, "Npc\\");
		break;

	case MODEL_NPC_CAPATULT_DEF:
		gmClientModels->AccessModel(MODEL_NPC_CAPATULT_DEF, "Data\\Npc\\", "Model_Npc_Catapult_Def");
		gmClientModels->OpenTexture(MODEL_NPC_CAPATULT_DEF, "Npc\\");
		break;

	case MODEL_NPC_SENATUS:
		gmClientModels->AccessModel(MODEL_NPC_SENATUS, "Data\\Npc\\", "NpcSenatus");
		gmClientModels->OpenTexture(MODEL_NPC_SENATUS, "Npc\\");
		break;

	case MODEL_NPC_GATE_SWITCH:
		gmClientModels->AccessModel(MODEL_NPC_GATE_SWITCH, "Data\\Npc\\", "NpcGateSwitch");
		gmClientModels->OpenTexture(MODEL_NPC_GATE_SWITCH, "Npc\\");
		break;

	case MODEL_NPC_CROWN:
		gmClientModels->AccessModel(MODEL_NPC_CROWN, "Data\\Npc\\", "NpcCrown");
		gmClientModels->OpenTexture(MODEL_NPC_CROWN, "Npc\\");
		break;

	case MODEL_NPC_CHECK_FLOOR:
		gmClientModels->AccessModel(MODEL_NPC_CHECK_FLOOR, "Data\\Npc\\", "NpcCheckFloor");
		gmClientModels->OpenTexture(MODEL_NPC_CHECK_FLOOR, "Npc\\");
		break;

	case MODEL_NPC_CLERK:
		gmClientModels->AccessModel(MODEL_NPC_CLERK, "Data\\Npc\\", "NpcClerk");
		gmClientModels->OpenTexture(MODEL_NPC_CLERK, "Npc\\");
		break;

	case MODEL_NPC_BARRIER:
		gmClientModels->AccessModel(MODEL_NPC_BARRIER, "Data\\Npc\\", "NpcBarrier");
		gmClientModels->OpenTexture(MODEL_NPC_BARRIER, "Npc\\");
		break;
	case MODEL_NPC_SERBIS:
		gmClientModels->AccessModel(MODEL_NPC_SERBIS, "Data\\Npc\\", "npc_mulyak");
		gmClientModels->OpenTexture(MODEL_NPC_SERBIS, "Npc\\");
		break;
	case MODEL_KALIMA_SHOP:
		gmClientModels->AccessModel(MODEL_KALIMA_SHOP, "Data\\Npc\\", "kalnpc");
		gmClientModels->OpenTexture(MODEL_KALIMA_SHOP, "Npc\\");
		break;
	case MODEL_BC_NPC1:
		gmClientModels->AccessModel(MODEL_BC_NPC1, "Data\\Npc\\", "npcpharmercy1");
		gmClientModels->OpenTexture(MODEL_BC_NPC1, "Npc\\");
		gmClientModels->AccessModel(MODEL_BC_BOX, "Data\\Npc\\", "box");
		gmClientModels->OpenTexture(MODEL_BC_BOX, "Npc\\");
		break;
	case MODEL_BC_NPC2:
		gmClientModels->AccessModel(MODEL_BC_NPC2, "Data\\Npc\\", "npcpharmercy2");
		gmClientModels->OpenTexture(MODEL_BC_NPC2, "Npc\\");
		gmClientModels->AccessModel(MODEL_BC_BOX, "Data\\Npc\\", "box");
		gmClientModels->OpenTexture(MODEL_BC_BOX, "Npc\\");
		break;
	case MODEL_CRYWOLF_STATUE:
		gmClientModels->AccessModel(MODEL_CRYWOLF_STATUE, "Data\\Object35\\", "Object82");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_STATUE, "Object35\\");
		break;
	case MODEL_CRYWOLF_ALTAR1:
		gmClientModels->AccessModel(MODEL_CRYWOLF_ALTAR1, "Data\\Object35\\", "Object57");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_ALTAR1, "Object35\\");
		break;
	case MODEL_CRYWOLF_ALTAR2:
		gmClientModels->AccessModel(MODEL_CRYWOLF_ALTAR2, "Data\\Object35\\", "Object57");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_ALTAR2, "Object35\\");
		break;
	case MODEL_CRYWOLF_ALTAR3:
		gmClientModels->AccessModel(MODEL_CRYWOLF_ALTAR3, "Data\\Object35\\", "Object57");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_ALTAR3, "Object35\\");
		break;
	case MODEL_CRYWOLF_ALTAR4:
		gmClientModels->AccessModel(MODEL_CRYWOLF_ALTAR4, "Data\\Object35\\", "Object57");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_ALTAR4, "Object35\\");
		break;
	case MODEL_CRYWOLF_ALTAR5:
		gmClientModels->AccessModel(MODEL_CRYWOLF_ALTAR5, "Data\\Object35\\", "Object57");
		gmClientModels->OpenTexture(MODEL_CRYWOLF_ALTAR5, "Object35\\");
		break;
	case MODEL_KANTURU2ND_ENTER_NPC:
	{
		gmClientModels->AccessModel(MODEL_KANTURU2ND_ENTER_NPC, "Data\\Npc\\", "to3gate");
		gmClientModels->OpenTexture(MODEL_KANTURU2ND_ENTER_NPC, "Npc\\");
	}
	break;
	case MODEL_SMELTING_NPC:
		gmClientModels->AccessModel(MODEL_SMELTING_NPC, "Data\\Npc\\", "Elpis");
		gmClientModels->OpenTexture(MODEL_SMELTING_NPC, "Npc\\");
		break;
	case MODEL_NPC_DEVIN:
		gmClientModels->AccessModel(MODEL_NPC_DEVIN, "Data\\Npc\\", "devin");
		gmClientModels->OpenTexture(MODEL_NPC_DEVIN, "Npc\\");
		break;
	case MODEL_NPC_QUARREL:
		gmClientModels->AccessModel(MODEL_NPC_QUARREL, "Data\\Npc\\", "WereQuarrel");
		gmClientModels->OpenTexture(MODEL_NPC_QUARREL, "Monster\\");
		break;
	case MODEL_NPC_CASTEL_GATE:
		gmClientModels->AccessModel(MODEL_NPC_CASTEL_GATE, "Data\\Npc\\", "cry2doorhead");
		gmClientModels->OpenTexture(MODEL_NPC_CASTEL_GATE, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_ENTER_NPC:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ENTER_NPC, "Data\\Npc\\", "mirazu");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_ENTER_NPC, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_ALLIED_NPC:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ALLIED_NPC, "Data\\Npc\\", "allied");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_ALLIED_NPC, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_ILLUSION_NPC:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ILLUSION_NPC, "Data\\Npc\\", "illusion");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_ILLUSION_NPC, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_STATUE:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_STATUE, "Data\\Npc\\", "songsom");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_STATUE, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_ALLIED_BASKET:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ALLIED_BASKET, "Data\\Npc\\", "songko");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_ALLIED_BASKET, "Npc\\");
		break;
	case MODEL_CURSEDTEMPLE_ILLUSION__BASKET:
		gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ILLUSION__BASKET, "Data\\Npc\\", "songk2");
		gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_ILLUSION__BASKET, "Npc\\");
		break;
	case MODEL_WEDDING_NPC:
		gmClientModels->AccessModel(MODEL_WEDDING_NPC, "Data\\Npc\\", "Wedding");
		gmClientModels->OpenTexture(MODEL_WEDDING_NPC, "Npc\\");
		break;
	case MODEL_ELBELAND_SILVIA:
		gmClientModels->AccessModel(MODEL_ELBELAND_SILVIA, "Data\\Npc\\", "silvia");
		gmClientModels->OpenTexture(MODEL_ELBELAND_SILVIA, "Npc\\");
		break;
	case MODEL_ELBELAND_RHEA:
		gmClientModels->AccessModel(MODEL_ELBELAND_RHEA, "Data\\Npc\\", "rhea");
		gmClientModels->OpenTexture(MODEL_ELBELAND_RHEA, "Npc\\");
		break;
	case MODEL_ELBELAND_MARCE:
		gmClientModels->AccessModel(MODEL_ELBELAND_MARCE, "Data\\Npc\\", "marce");
		gmClientModels->OpenTexture(MODEL_ELBELAND_MARCE, "Npc\\");
		break;
	case MODEL_NPC_CHERRYBLOSSOM:
		gmClientModels->AccessModel(MODEL_NPC_CHERRYBLOSSOM, "Data\\Npc\\cherryblossom\\", "cherry_blossom");
		gmClientModels->OpenTexture(MODEL_NPC_CHERRYBLOSSOM, "Npc\\cherryblossom\\");
		break;
	case MODEL_NPC_CHERRYBLOSSOMTREE:
		gmClientModels->AccessModel(MODEL_NPC_CHERRYBLOSSOMTREE, "Data\\Npc\\cherryblossom\\", "sakuratree");
		gmClientModels->OpenTexture(MODEL_NPC_CHERRYBLOSSOMTREE, "Npc\\cherryblossom\\");
		break;
	case MODEL_SEED_MASTER:
		gmClientModels->AccessModel(MODEL_SEED_MASTER, "Data\\Npc\\", "goblinmaster");
		gmClientModels->OpenTexture(MODEL_SEED_MASTER, "Npc\\");
		break;
	case MODEL_SEED_INVESTIGATOR:
		gmClientModels->AccessModel(MODEL_SEED_INVESTIGATOR, "Data\\Npc\\", "seedgoblin");
		gmClientModels->OpenTexture(MODEL_SEED_INVESTIGATOR, "Npc\\");
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
		gmClientModels->AccessModel(MODEL_LITTLESANTA + (Type - MODEL_LITTLESANTA), "Data\\Npc\\", "xmassanta");

		int _index = 9;
		int _index_end = 14;

		b->Textures->FileName[_index_end] = NULL;
		for (int i = _index_end - 1; i > _index; i--)
		{
			b->Textures->FileName[i] = b->Textures->FileName[i - 1];
		}
		int _temp = (Type - MODEL_LITTLESANTA) + 1;
		char _temp2[10] = { 0, };
		itoa(_temp, _temp2, 10);
		b->Textures->FileName[_index] = _temp2[0];

		gmClientModels->OpenTexture(MODEL_LITTLESANTA + (Type - MODEL_LITTLESANTA), "Npc\\");
	}
	break;
	case MODEL_XMAS2008_SNOWMAN:
	{
		gmClientModels->AccessModel(MODEL_XMAS2008_SNOWMAN, "Data\\Item\\", "snowman");
		gmClientModels->AccessModel(MODEL_XMAS2008_SNOWMAN_HEAD, "Data\\Item\\", "snowman_die_head_model");
		gmClientModels->AccessModel(MODEL_XMAS2008_SNOWMAN_BODY, "Data\\Item\\", "snowman_die_body");
		gmClientModels->OpenTexture(MODEL_XMAS2008_SNOWMAN, "Item\\Texture\\");
		gmClientModels->OpenTexture(MODEL_XMAS2008_SNOWMAN_HEAD, "Item\\Texture\\");
		gmClientModels->OpenTexture(MODEL_XMAS2008_SNOWMAN_BODY, "Item\\Texture\\");

		LoadWaveFile(SOUND_XMAS_SNOWMAN_WALK_1, "Data\\Sound\\xmas\\SnowMan_Walk01.wav");
		LoadWaveFile(SOUND_XMAS_SNOWMAN_ATTACK_1, "Data\\Sound\\xmas\\SnowMan_Attack01.wav");
		LoadWaveFile(SOUND_XMAS_SNOWMAN_ATTACK_2, "Data\\Sound\\xmas\\SnowMan_Attack02.wav");
		LoadWaveFile(SOUND_XMAS_SNOWMAN_DAMAGE_1, "Data\\Sound\\xmas\\SnowMan_Damage01.wav");
		LoadWaveFile(SOUND_XMAS_SNOWMAN_DEATH_1, "Data\\Sound\\xmas\\SnowMan_Death01.wav");
	}
	break;
	case MODEL_XMAS2008_SNOWMAN_NPC:
		gmClientModels->AccessModel(MODEL_XMAS2008_SNOWMAN_NPC, "Data\\Npc\\", "snowman");
		gmClientModels->OpenTexture(MODEL_XMAS2008_SNOWMAN_NPC, "Npc\\");
		break;
	case MODEL_XMAS2008_SANTA_NPC:
		gmClientModels->AccessModel(MODEL_XMAS2008_SANTA_NPC, "Data\\Npc\\", "npcsanta");
		gmClientModels->OpenTexture(MODEL_XMAS2008_SANTA_NPC, "Npc\\");
		break;
	case MODEL_DUEL_NPC_TITUS:
		gmClientModels->AccessModel(MODEL_DUEL_NPC_TITUS, "Data\\Npc\\", "titus");
		gmClientModels->OpenTexture(MODEL_DUEL_NPC_TITUS, "Npc\\");

		LoadWaveFile(SOUND_DUEL_NPC_IDLE_1, "Data\\Sound\\w64\\GatekeeperTitus.wav");
		break;
	case MODEL_GAMBLE_NPC_MOSS:
		gmClientModels->AccessModel(MODEL_GAMBLE_NPC_MOSS, "Data\\Npc\\", "gambler_moss");
		gmClientModels->OpenTexture(MODEL_GAMBLE_NPC_MOSS, "Npc\\");
		break;
	case MODEL_DOPPELGANGER_NPC_LUGARD:
		gmClientModels->AccessModel(MODEL_DOPPELGANGER_NPC_LUGARD, "Data\\Npc\\", "Lugard");
		gmClientModels->OpenTexture(MODEL_DOPPELGANGER_NPC_LUGARD, "Npc\\");
		LoadWaveFile(SOUND_DOPPELGANGER_LUGARD_BREATH, "Data\\Sound\\Doppelganger\\Lugard.wav");
		break;
	case MODEL_DOPPELGANGER_NPC_BOX:
		gmClientModels->AccessModel(MODEL_DOPPELGANGER_NPC_BOX, "Data\\Npc\\", "DoppelgangerBox");
		gmClientModels->OpenTexture(MODEL_DOPPELGANGER_NPC_BOX, "Npc\\");
		LoadWaveFile(SOUND_DOPPELGANGER_JEWELBOX_OPEN, "Data\\Sound\\Doppelganger\\treasurebox_open.wav");
		break;
	case MODEL_DOPPELGANGER_NPC_GOLDENBOX:
		gmClientModels->AccessModel(MODEL_DOPPELGANGER_NPC_GOLDENBOX, "Data\\Npc\\", "DoppelgangerBox");
		gmClientModels->OpenTexture(MODEL_DOPPELGANGER_NPC_GOLDENBOX, "Npc\\");
		LoadWaveFile(SOUND_DOPPELGANGER_JEWELBOX_OPEN, "Data\\Sound\\Doppelganger\\treasurebox_open.wav");
		break;
	case MODAL_GENS_NPC_DUPRIAN:
		gmClientModels->AccessModel(MODAL_GENS_NPC_DUPRIAN, "Data\\Npc\\", "duprian");
		gmClientModels->OpenTexture(MODAL_GENS_NPC_DUPRIAN, "Npc\\");
		break;
	case MODAL_GENS_NPC_BARNERT:
		gmClientModels->AccessModel(MODAL_GENS_NPC_BARNERT, "Data\\Npc\\", "barnert");
		gmClientModels->OpenTexture(MODAL_GENS_NPC_BARNERT, "Npc\\");
		break;
	case MODEL_UNITEDMARKETPLACE_CHRISTIN:
		gmClientModels->AccessModel(MODEL_UNITEDMARKETPLACE_CHRISTIN, "Data\\Npc\\", "UnitedMarketPlace_christine");
		gmClientModels->OpenTexture(MODEL_UNITEDMARKETPLACE_CHRISTIN, "Npc\\");
		break;
	case MODEL_UNITEDMARKETPLACE_RAUL:
		gmClientModels->AccessModel(MODEL_UNITEDMARKETPLACE_RAUL, "Data\\Npc\\", "UnitedMarkedPlace_raul");
		gmClientModels->OpenTexture(MODEL_UNITEDMARKETPLACE_RAUL, "Npc\\");
		break;
	case MODEL_UNITEDMARKETPLACE_JULIA:
		gmClientModels->AccessModel(MODEL_UNITEDMARKETPLACE_JULIA, "Data\\Npc\\", "UnitedMarkedPlace_julia");
		gmClientModels->OpenTexture(MODEL_UNITEDMARKETPLACE_JULIA, "Npc\\");
		break;
	case MODEL_TIME_LIMIT_QUEST_NPC_TERSIA:
		gmClientModels->AccessModel(MODEL_TIME_LIMIT_QUEST_NPC_TERSIA, "Data\\Npc\\", "tersia");
		gmClientModels->OpenTexture(MODEL_TIME_LIMIT_QUEST_NPC_TERSIA, "Npc\\");
		break;
	case MODEL_TIME_LIMIT_QUEST_NPC_BENA:
		gmClientModels->AccessModel(MODEL_TIME_LIMIT_QUEST_NPC_BENA, "Data\\Npc\\", "bena");
		gmClientModels->OpenTexture(MODEL_TIME_LIMIT_QUEST_NPC_BENA, "Npc\\");
		break;
	case MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO:
		gmClientModels->AccessModel(MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO, "Data\\Npc\\", "gambler_moss");
		gmClientModels->OpenTexture(MODEL_TIME_LIMIT_QUEST_NPC_ZAIRO, "Npc\\");
		break;
	case MODEL_KARUTAN_NPC_REINA:
		gmClientModels->AccessModel(MODEL_KARUTAN_NPC_REINA, "Data\\Npc\\", "UnitedMarketPlace_christine");
		gmClientModels->OpenTexture(MODEL_KARUTAN_NPC_REINA, "Npc\\");
		break;
	case MODEL_KARUTAN_NPC_VOLVO:
		gmClientModels->AccessModel(MODEL_KARUTAN_NPC_VOLVO, "Data\\Npc\\", "volvo");
		gmClientModels->OpenTexture(MODEL_KARUTAN_NPC_VOLVO, "Npc\\");
		break;
	case MODEL_LUCKYITEM_NPC:
		gmClientModels->AccessModel(MODEL_LUCKYITEM_NPC, "Data\\Npc\\LuckyItem\\", "npc_burial");
		gmClientModels->OpenTexture(MODEL_LUCKYITEM_NPC, "Npc\\LuckyItem\\");
		break;
	}

	for (int i = 0; i < b->NumActions; i++)
		b->Actions[i].PlaySpeed = 0.25f;

	if (b->NumMeshs > 0)
		gmClientModels->OpenTexture(Type, "Npc\\");

	if (b->Actions)
	{
		switch (Type)
		{
		case MODEL_XMAS2008_SNOWMAN:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.9f;
			break;
		case MODEL_PANDA:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.9f;
			break;
		case MODEL_DOPPELGANGER_NPC_BOX:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.1f;
			break;
		case MODEL_DOPPELGANGER_NPC_GOLDENBOX:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.1f;
			break;
		case MODAL_GENS_NPC_DUPRIAN:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.6f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.6f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 1.4f;
			break;
		case MODAL_GENS_NPC_BARNERT:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 1.2f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
		case MODEL_UNITEDMARKETPLACE_RAUL:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f;
			break;
		case MODEL_UNITEDMARKETPLACE_JULIA:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f;
			break;
		case MODEL_UNITEDMARKETPLACE_CHRISTIN:
		case MODEL_KARUTAN_NPC_REINA:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.6f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f;
			break;
		case MODEL_TIME_LIMIT_QUEST_NPC_TERSIA:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.3f;
			break;
		case MODEL_KARUTAN_NPC_VOLVO:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.2f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			break;
		}
	}
}

void SetMonsterSound(int Type, int s1, int s2, int s3, int s4, int s5, int s6 = -1, int s7 = -1, int s8 = -1, int s9 = -1, int s10 = -1)
{
	BMD* pModel = gmClientModels->GetModel(Type);
	if (pModel)
	{
		pModel->Sounds[0] = s1;
		pModel->Sounds[1] = s2;
		pModel->Sounds[2] = s3;
		pModel->Sounds[3] = s4;
		pModel->Sounds[4] = s5;
		pModel->Sounds[5] = s6;
		pModel->Sounds[6] = s7;
		pModel->Sounds[7] = s8;
		pModel->Sounds[8] = s9;
		pModel->Sounds[9] = s10;
	}
}

void DeleteMonsters()
{
	for (int i = MODEL_MONSTER01; i < MODEL_MONSTER_END; i++)
		gmClientModels->GetModel(i)->Release();

	//if (release)
	//{
	//	for (int i = MODEL_MONSTER_DUMY_BENGI; i < MODEL_MONSTER_DUMY_END; i++)
	//		gmClientModels->GetModel(i)->Release();
	//}

	for (int i = SOUND_MONSTER; i < SOUND_MONSTER_END; i++)
		ReleaseBuffer(i);

	for (int i = SOUND_ELBELAND_RABBITSTRANGE_ATTACK01; i <= SOUND_ELBELAND_ENTERATLANCE01; i++)
		ReleaseBuffer(i);
}

void OpenMonsterModel(int Type)
{
	g_ErrorReport.Write("OpenMonsterModel(%d)\r\n", Type);

	int Index = MODEL_MONSTER01 + Type;

	BMD* b = gmClientModels->GetModel(Index);

	if (b->NumActions > 0 || b->NumMeshs > 0)
		return;

	gmClientModels->AccessModel(Index, "Data\\Monster\\", "Monster", Type + 1);

	if (b->NumMeshs == 0)
		return;

	if (gMapManager->InChaosCastle() == true && Type >= 70 && Type <= 72)
	{
		gmClientModels->OpenTexture(Index, "Npc\\");
	}
	else if (gMapManager->InBattleCastle() == true && Type == 74)
	{
		gmClientModels->OpenTexture(Index, "Object31\\");
	}
	else
	{
		gmClientModels->OpenTexture(Index, "Monster\\");
	}

	switch (Type)
	{
	case 147:
	{
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART1, "Data\\Monster\\", "icegiantpart_1");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART1, "Monster\\");
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART2, "Data\\Monster\\", "icegiantpart_2");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART2, "Monster\\");
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART3, "Data\\Monster\\", "icegiantpart_3");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART3, "Monster\\");
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART4, "Data\\Monster\\", "icegiantpart_4");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART4, "Monster\\");
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART5, "Data\\Monster\\", "icegiantpart_5");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART5, "Monster\\");
		gmClientModels->AccessModel(MODEL_ICE_GIANT_PART6, "Data\\Monster\\", "icegiantpart_6");
		gmClientModels->OpenTexture(MODEL_ICE_GIANT_PART6, "Monster\\");
	}
	break;
	}

	if (b->Actions)
	{
		b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
		b->Actions[MONSTER01_STOP2].PlaySpeed = 0.2f;
		b->Actions[MONSTER01_WALK].PlaySpeed = 0.34f;
		b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
		b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
		b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.5f;
		b->Actions[MONSTER01_DIE].PlaySpeed = 0.55f;
		b->Actions[MONSTER01_DIE].Loop = true;
	}

	for (int j = MONSTER01_STOP1; j < MONSTER01_DIE; j++)
	{
		if (b->Actions && b->NumActions > j)
		{
			if (Type == 3)
				b->Actions[j].PlaySpeed *= 1.2f;
			if (Type == 5 || Type == 25)
				b->Actions[j].PlaySpeed *= 0.7f;
			if (Type == 37 || Type == 42)
				b->Actions[j].PlaySpeed *= 0.4f;
		}
	}

	if (b->Actions)
	{
		switch (Type)
		{
		case 2:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.7f; break;
		case 6:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.6f; break;
		case 8:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.7f; break;
		case 9:
			b->Actions[MONSTER01_WALK].PlaySpeed = 1.2f; break;
		case 10:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.28f; break;
		case 12:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f; break;
		case 13:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.28f; break;
		case 17:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f; break;
		case 19:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.6f; break;
		case 20:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.4f; break;
		case 21:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f; break;
		case 28:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f; break;
		case 39:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.22f; break;
		case 41:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.18f; break;
		case 42:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.35f;
			break;
		case 44:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 63:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.1f;
			break;
		case 64:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 66:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.1f;
			break;
		case 67:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.2f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.3f;
			break;
		case 69:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.1f;
			break;
		case 70:	// 
		case 71:	// 
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			break;
		case 72:
			//    case 73:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			break;
		case 89:
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.33f;
			break;
		case 94:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.20f;
			break;
		case 92:
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.33f;
			break;
		case 99:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.37f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.37f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.15f;
			break;
		case 100:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
			break;
		case 101:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
		case 102:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.15f;
			break;
		case 104:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
			break;
		case 105:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.15f;
			break;
		case 106:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.19f;
			break;
		case 107:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 108:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 109:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 110:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
			break;
		case 111:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 112:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.26f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.26f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.21f;
			break;
		case 113:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 114:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.34f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.23f;
			break;
		case 115:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.34f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.23f;
			break;
		case 116:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.34f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 121:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.22f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.22f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.12f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.22f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.25f;
			break;
		case 118:
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.12f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.12f;
		case 119:
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.12f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.12f;
		case 120:
			break;
		case 122:
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
			break;
		case 127:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.50f;
			break;
		case 154:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.50f;
			break;
		case 157:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.17f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
		}
		break;
		case 158:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.2f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.18f;
		}
		break;
		case 159:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.2f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.17f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
		}
		break;
		case 160:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.17f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.4f;
		}
		break;
		case 161:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.17f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.4f;
		}
		break;
		case 162:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
		}
		break;
		case 163:
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
		}
		break;
		case 128:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 129:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.23f;
			break;
		case 130:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 131:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.45f;
			break;
		case 132:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.35;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 133:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.35;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 134:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
			break;
		case 135:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.28f;
			break;
		case 136:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 137:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 138:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 139:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 140:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 141:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 142:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 143:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 144:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 145:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.6f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.35f;
			break;
		case 146:
			break;
		case 147:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.13f;
			break;
		case 148:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.46f;
			break;
		case 149:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.21f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.23f;
			break;
		case 150:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.18f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.25f;
			break;
		case 151:
		case 152:
		case 153:
			break;
		case 155:
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.29f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.4f;
			break;
		case 164:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.38f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.38f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.38f;
			break;
		case 165:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.86f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.86f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.76f;
			break;
		case 166:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.55f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.75f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.80f;
			break;
		case 167:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.71f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.40f;
			break;
		case 168:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.38f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.45f;
			break;
		case 169:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.45f;
			break;
		case 170:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.40f;
			break;
		case 171:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.45f;
			break;
		case 172:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.66f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.55f;
			break;
		case 173:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.36f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.40f;
			break;
		case 174:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.65f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.86f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.86f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.86f;
			break;
		case 175:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.60f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.96f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.96f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 1.00f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 176:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.38f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.38f;
			break;
		case 177:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.37f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 178:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.45f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.40f;
			break;
#ifdef LDS_ADD_EG_2_MONSTER_GUARDIANPRIEST
		case 179:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.40f;
			break;
#endif // LDS_ADD_EG_2_MONSTER_GUARDIANPRIEST
		case 180:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.20f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.55f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.66f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.66f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.55f;
			break;
		case 181:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.35f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.50f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30f;
			break;
		case 189:
		case 190:
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 191:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f * 2.0f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.2f * 2.0f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.34f * 2.0f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.25f * 2.0f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.25f * 2.0f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.5f * 2.0f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.55f * 2.0f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.33f * 3.0f;
			break;
		case 192:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.30f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.30;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.30;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.30;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.30;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.30;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.30;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.30;
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.30;
			break;
		case 205:
			break;
		case 206:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.13f;
			break;
		case 207:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.46f;
			break;
		case 208:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.21f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.23f;
			break;
		case 193:
			break;
		case 194:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.15f;
			break;
		case 195:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
			break;
		case 196:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
			break;
		case 201:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 202:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 203:
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 197:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.23f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.28f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.19f;
			break;
		case 198:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.27f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 199:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.26f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.26f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.21f;
			break;
		case 200:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.3f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 209:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.95f;
			break;
		case 210:
			b->Actions[MONSTER01_WALK].PlaySpeed = 1.00f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.40f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.20f;
			break;
		case 211:
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.7f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.7f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.6f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.8f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.8f;
			b->Actions[MONSTER01_SHOCK].PlaySpeed = 0.25f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 212:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.3f;
			break;
		case 213:
		case 214:
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.9f;
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.37f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.37f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.25f;
			break;
		case 215:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.80f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.80f;
			break;
		case 216:
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.75f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.75f;
			break;
		}
		switch (Type)
		{
		case 14:
		case 31:
		case 39:
		case 40:
			b->Actions[MONSTER01_STOP2].Loop = true;
			break;
		}
	}

	int Channel = 2;
	bool Enable = true;
	switch (Type)
	{
	case 70:
	case 71:
		LoadWaveFile(SOUND_MONSTER + 161, "Data\\Sound\\mOrcCapAttack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 161, 161, -1);
		b->BoneHead = 20;
		break;

	case 72:
		LoadWaveFile(SOUND_MONSTER + 162, "Data\\Sound\\mOrcArcherAttack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 162, 162, -1);
		b->BoneHead = 20;
		break;
	case 0:
	case 30:
		LoadWaveFile(SOUND_MONSTER, "Data\\Sound\\mBull1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 1, "Data\\Sound\\mBull2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 2, "Data\\Sound\\mBullAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 3, "Data\\Sound\\mBullAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 4, "Data\\Sound\\mBullDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 0, 1, 2, 3, 4);
		b->BoneHead = 20;
		break;
	case 1:
		LoadWaveFile(SOUND_MONSTER + 5, "Data\\Sound\\mHound1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 6, "Data\\Sound\\mHound2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 7, "Data\\Sound\\mHoundAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 8, "Data\\Sound\\mHoundAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 9, "Data\\Sound\\mHoundDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 5, 6, 7, 8, 9);
		b->BoneHead = 5;
		break;
	case 2:
		LoadWaveFile(SOUND_MONSTER + 10, "Data\\Sound\\mBudge1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 11, "Data\\Sound\\mBudgeAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 12, "Data\\Sound\\mBudgeDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 10, 11, 11, 11, 12);
		b->BoneHead = 7;
		break;
	case 9:
		LoadWaveFile(SOUND_MONSTER + 13, "Data\\Sound\\mSpider1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 13, 13, 13, 13, 13);
		break;
	case 3:
	case 29:
	case 39:
		LoadWaveFile(SOUND_MONSTER + 15, "Data\\Sound\\mDarkKnight1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 16, "Data\\Sound\\mDarkKnight2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 17, "Data\\Sound\\mDarkKnightAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 18, "Data\\Sound\\mDarkKnightAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 19, "Data\\Sound\\mDarkKnightDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 15, 16, 17, 18, 19);
		if (Type == 3)
			b->BoneHead = 16;
		else if (Type == 29)
			b->BoneHead = 20;
		else
			b->BoneHead = 19;
		break;
	case 4:
		LoadWaveFile(SOUND_MONSTER + 20, "Data\\Sound\\mWizard1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 21, "Data\\Sound\\mWizard2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 22, "Data\\Sound\\mWizardAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 23, "Data\\Sound\\mWizardAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 24, "Data\\Sound\\mWizardDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 20, 21, 22, 23, 24);
		b->BoneHead = 20;
		break;
	case 5:
		LoadWaveFile(SOUND_MONSTER + 25, "Data\\Sound\\mGiant1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 26, "Data\\Sound\\mGiant2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 27, "Data\\Sound\\mGiantAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 28, "Data\\Sound\\mGiantAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 29, "Data\\Sound\\mGiantDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 25, 26, 27, 28, 29);
		b->BoneHead = 20;
		break;
	case 6:
		LoadWaveFile(SOUND_MONSTER + 30, "Data\\Sound\\mLarva1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 31, "Data\\Sound\\mLarva2.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 30, 31, 30, 31, 31);
		break;
	case 8:
		LoadWaveFile(SOUND_MONSTER + 32, "Data\\Sound\\mHellSpider1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 33, "Data\\Sound\\mHellSpiderAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 34, "Data\\Sound\\mHellSpiderDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 32, 33, 33, 33, 34);
		b->BoneHead = 16;
		break;
	case 7:
		LoadWaveFile(SOUND_MONSTER + 35, "Data\\Sound\\mGhost1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 36, "Data\\Sound\\mGhost2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 37, "Data\\Sound\\mGhostAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 38, "Data\\Sound\\mGhostAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 39, "Data\\Sound\\mGhostDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 35, 36, 37, 38, 39);
		b->BoneHead = 20;
		break;
	case 10:
		LoadWaveFile(SOUND_MONSTER + 40, "Data\\Sound\\mOgre1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 41, "Data\\Sound\\mOgre2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 42, "Data\\Sound\\mOgreAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 43, "Data\\Sound\\mOgreAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 44, "Data\\Sound\\mOgreDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 40, 41, 42, 43, 44);
		b->BoneHead = 20;
		break;
	case 11:
		LoadWaveFile(SOUND_MONSTER + 45, "Data\\Sound\\mGorgon1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 46, "Data\\Sound\\mGorgon2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 47, "Data\\Sound\\mGorgonAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 48, "Data\\Sound\\mGorgonAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 49, "Data\\Sound\\mGorgonDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 45, 46, 47, 48, 49);
		b->BoneHead = 20;
		break;
	case 15:
		LoadWaveFile(SOUND_MONSTER + 50, "Data\\Sound\\mIceMonster1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 51, "Data\\Sound\\mIceMonster2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 52, "Data\\Sound\\mIceMonsterDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 50, 51, 50, 50, 52);
		b->BoneHead = 19;
		break;
	case 17:
		LoadWaveFile(SOUND_MONSTER + 53, "Data\\Sound\\mWorm1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 54, "Data\\Sound\\mWorm2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 55, "Data\\Sound\\mWormDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 53, 53, 55, 55, 55);
		break;
	case 16:
		LoadWaveFile(SOUND_MONSTER + 56, "Data\\Sound\\mHomord1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 57, "Data\\Sound\\mHomord2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 58, "Data\\Sound\\mHomordAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 59, "Data\\Sound\\mHomordDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 56, 57, 58, 58, 59);
		break;
	case 18:
		LoadWaveFile(SOUND_MONSTER + 60, "Data\\Sound\\mIceQueen1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 61, "Data\\Sound\\mIceQueen2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 62, "Data\\Sound\\mIceQueenAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 63, "Data\\Sound\\mIceQueenAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 64, "Data\\Sound\\mIceQueenDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 60, 61, 62, 63, 64);
		b->BoneHead = 16;
		break;
	case 14:
		LoadWaveFile(SOUND_MONSTER + 65, "Data\\Sound\\mAssassinAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 66, "Data\\Sound\\mAssassinAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 67, "Data\\Sound\\mAssassinDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 65, 66, 67);
		b->BoneHead = 20;
		if (b->Actions)
		{
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.35f;
		}
		break;
	case 12:
	case 13:
		LoadWaveFile(SOUND_MONSTER + 68, "Data\\Sound\\mYeti1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 69, "Data\\Sound\\mYeti2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 70, "Data\\Sound\\mYetiAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 71, "Data\\Sound\\mYetiDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + 12, 68, 68, 70, 70, 71);
		SetMonsterSound(MODEL_MONSTER01 + 13, 68, 69, 70, 70, 71);
		gmClientModels->GetModel(MODEL_MONSTER01 + 12)->BoneHead = 20;
		gmClientModels->GetModel(MODEL_MONSTER01 + 13)->BoneHead = 20;
		break;
	case 19:
		LoadWaveFile(SOUND_MONSTER + 72, "Data\\Sound\\mGoblin1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 73, "Data\\Sound\\mGoblin2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 74, "Data\\Sound\\mGoblinAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 75, "Data\\Sound\\mGoblinAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 76, "Data\\Sound\\mGoblinDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 72, 73, 74, 75, 76);
		b->BoneHead = 6;
		break;
	case 20:
		LoadWaveFile(SOUND_MONSTER + 77, "Data\\Sound\\mScorpion1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 78, "Data\\Sound\\mScorpion2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 79, "Data\\Sound\\mScorpionAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 80, "Data\\Sound\\mScorpionAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 81, "Data\\Sound\\mScorpionDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 77, 78, 79, 80, 81);
		break;
	case 21:
		LoadWaveFile(SOUND_MONSTER + 82, "Data\\Sound\\mBeetle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 83, "Data\\Sound\\mBeetleAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 84, "Data\\Sound\\mBeetleDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 82, 82, 83, 83, 84);
		b->BoneHead = 5;
		break;
	case 22:
		LoadWaveFile(SOUND_MONSTER + 85, "Data\\Sound\\mHunter1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 86, "Data\\Sound\\mHunter2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 87, "Data\\Sound\\mHunterAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 88, "Data\\Sound\\mHunterAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 89, "Data\\Sound\\mHunterDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 85, 86, 87, 88, 89);
		b->BoneHead = 6;
		break;
	case 23:
		LoadWaveFile(SOUND_MONSTER + 90, "Data\\Sound\\mWoodMon1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 91, "Data\\Sound\\mWoodMon2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 92, "Data\\Sound\\mWoodMonAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 93, "Data\\Sound\\mWoodMonAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 94, "Data\\Sound\\mWoodMonDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 90, 91, 92, 93, 94);
		b->BoneHead = 6;
		break;
	case 24:
		LoadWaveFile(SOUND_MONSTER + 95, "Data\\Sound\\mArgon1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 96, "Data\\Sound\\mArgon2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 97, "Data\\Sound\\mArgonAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 98, "Data\\Sound\\mArgonAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 99, "Data\\Sound\\mArgonDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 95, 96, 97, 98, 99);
		b->BoneHead = 16;
		break;
	case 25:
		LoadWaveFile(SOUND_MONSTER + 100, "Data\\Sound\\mGolem1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 101, "Data\\Sound\\mGolem2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 102, "Data\\Sound\\mGolemAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 103, "Data\\Sound\\mGolemAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 104, "Data\\Sound\\mGolemDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 100, 101, 102, 103, 104);
		b->BoneHead = 5;
		break;
	case 26:
		LoadWaveFile(SOUND_MONSTER + 105, "Data\\Sound\\mYeti1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 106, "Data\\Sound\\mSatanAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 107, "Data\\Sound\\mYetiDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 105, 105, 106, 106, 107);
		b->BoneHead = 6;
		break;
	case 27:
		LoadWaveFile(SOUND_MONSTER + 108, "Data\\Sound\\mBalrog1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 109, "Data\\Sound\\mBalrog2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 110, "Data\\Sound\\mWizardAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 111, "Data\\Sound\\mGorgonAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 112, "Data\\Sound\\mBalrogDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 108, 109, 110, 111, 112);
		b->BoneHead = 6;
		b->StreamMesh = 1;
		break;
	case 28:
		LoadWaveFile(SOUND_MONSTER + 113, "Data\\Sound\\mShadow1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 114, "Data\\Sound\\mShadow2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 115, "Data\\Sound\\mShadowAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 116, "Data\\Sound\\mShadowAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 117, "Data\\Sound\\mShadowDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 113, 114, 115, 116, 117);
		b->BoneHead = 5;
		break;
	case 31:
		LoadWaveFile(SOUND_MONSTER + 123, "Data\\Sound\\mYeti1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 124, "Data\\Sound\\mBullAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 125, "Data\\Sound\\mYetiDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 123, 123, 124, 124, 125);
		if (b->Actions)
		{
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.7f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.8f;
			b->Actions[MONSTER01_DIE + 1].PlaySpeed = 0.8f;
		}
		break;
	case 32:
		LoadWaveFile(SOUND_MONSTER + 126, "Data\\Sound\\mBali1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 127, "Data\\Sound\\mBali2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 128, "Data\\Sound\\mBaliAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 129, "Data\\Sound\\mBaliAttack2.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 126, 127, 128, 129, 127);
		if (b->Actions)
		{
			b->Actions[MONSTER01_ATTACK3].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_ATTACK4].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_APEAR].PlaySpeed = 0.4f;
			b->Actions[MONSTER01_RUN].PlaySpeed = 0.4f;
		}
		b->BoneHead = 6;
		break;

	case 33:
		LoadWaveFile(SOUND_MONSTER + 130, "Data\\Sound\\mBahamut1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 131, "Data\\Sound\\mYeti1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 130, 130, 131, 131, 130);
		break;
	case 34:
		LoadWaveFile(SOUND_MONSTER + 132, "Data\\Sound\\mBepar1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 133, "Data\\Sound\\mBepar2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 134, "Data\\Sound\\mBalrog1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 132, 133, 104, 104, 133);
		if (b->Actions)
		{
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.5f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.5f;
		}
		b->BoneHead = 20;//인어
		break;
	case 35:
		LoadWaveFile(SOUND_MONSTER + 135, "Data\\Sound\\mValkyrie1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 136, "Data\\Sound\\mBaliAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 137, "Data\\Sound\\mValkyrieDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 135, 135, 136, 136, 137);
		b->BoneHead = 19;
		break;
	case 36:
	case 40:
		LoadWaveFile(SOUND_MONSTER + 138, "Data\\Sound\\mLizardKing1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 139, "Data\\Sound\\mLizardKing2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 140, "Data\\Sound\\mGorgonDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 138, 139, 138, 139, 140);
		if (Type == 36)
			b->BoneHead = 19;
		else
			b->BoneHead = 6;
		break;
	case 37:
		LoadWaveFile(SOUND_MONSTER + 141, "Data\\Sound\\mHydra1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 142, "Data\\Sound\\mHydraAttack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 141, 141, 142, 142, 141);
		if (b->Actions)
		{
			b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.15f;
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.2f;
		}
		break;
	case 41:
		LoadWaveFile(SOUND_MONSTER + 143, "Data\\Sound\\iron1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 144, "Data\\Sound\\iron_attack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 143, 143, 144, 144, 144);
		b->BoneHead = 3;
		break;
	case 42:
		LoadWaveFile(SOUND_MONSTER + 145, "Data\\Sound\\jaikan1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 146, "Data\\Sound\\jaikan2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 147, "Data\\Sound\\jaikan_attack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 148, "Data\\Sound\\jaikan_attack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 149, "Data\\Sound\\jaikan_die.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 145, 146, 147, 148, 149);
		LoadBitmap("Monster\\bv01_2.jpg", BITMAP_MONSTER_SKIN, GL_LINEAR, GL_REPEAT);
		LoadBitmap("Monster\\bv02_2.jpg", BITMAP_MONSTER_SKIN + 1, GL_LINEAR, GL_REPEAT);
		b->BoneHead = 20;
		break;
	case 43:
		LoadWaveFile(SOUND_MONSTER + 150, "Data\\Sound\\blood1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 151, "Data\\Sound\\blood_attack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 152, "Data\\Sound\\blood_attack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 153, "Data\\Sound\\blood_die.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 150, 150, 151, 152, 153);
		b->BoneHead = 7;
		break;
	case 44:
		LoadWaveFile(SOUND_MONSTER + 154, "Data\\Sound\\death1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 155, "Data\\Sound\\death_attack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 156, "Data\\Sound\\death_die.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 154, 154, 155, 155, 156);
		b->BoneHead = 6;
		break;
	case 45:
		LoadWaveFile(SOUND_MONSTER + 157, "Data\\Sound\\mutant1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 158, "Data\\Sound\\mutant2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 159, "Data\\Sound\\mutant_attack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 157, 158, 159, 159, 159);
		b->BoneHead = 6;
		break;
	case 46:
		LoadWaveFile(SOUND_MONSTER + 162, "Data\\Sound\\mOrcArcherAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 4, "Data\\Sound\\mBullDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 162, 162, 4);
		b->BoneHead = 7;
		break;
	case 47:
		LoadWaveFile(SOUND_MONSTER + 86, "Data\\Sound\\mHunter2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 4, "Data\\Sound\\mBullDie.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 161, "Data\\Sound\\mOrcCapAttack1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 86, 86, 161, 161, 4);
		b->BoneHead = 20;
		break;
	case 48:
		LoadWaveFile(SOUND_MONSTER + 160, "Data\\Sound\\mCursedKing1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 164, "Data\\Sound\\mCursedKing2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 163, "Data\\Sound\\mCursedKingDie1.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 160, 164, -1, -1, 163);
		b->BoneHead = 20;
		break;
	case 52:
		LoadBitmap("Monster\\iui02.tga", BITMAP_ROBE + 3);
		LoadBitmap("Monster\\iui03.tga", BITMAP_ROBE + 5);
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		LoadWaveFile(SOUND_MONSTER + 180, "Data\\Sound\\mMegaCrust1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 181, "Data\\Sound\\mMegaCrustAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 182, "Data\\Sound\\mMegaCrustDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 180, 180, 181, 181, 182);
		break;
	case 49:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		LoadWaveFile(SOUND_MONSTER + 177, "Data\\Sound\\mMolt1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 178, "Data\\Sound\\mMoltAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 179, "Data\\Sound\\mMoltDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 177, 177, 178, 178, 179);
		break;
	case 50:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		//LoadWaveFile(SOUND_MONSTER+174,"Data\\Sound\\mAlquamos1.wav"    ,Channel,Enable);
		LoadWaveFile(SOUND_MONSTER + 175, "Data\\Sound\\mAlquamosAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 176, "Data\\Sound\\mAlquamosDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 175, 175, 175, 175, 176);
		break;
	case 51:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		LoadWaveFile(SOUND_MONSTER + 171, "Data\\Sound\\mRainner1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 172, "Data\\Sound\\mRainnerAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 173, "Data\\Sound\\mRainnerDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 171, -1, 172, 172, 173);
		break;
	case 53:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		LoadWaveFile(SOUND_MONSTER + 168, "Data\\Sound\\mPhantom1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 169, "Data\\Sound\\mPhantomAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 170, "Data\\Sound\\mPhantomDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 168, 168, 169, 169, 170);
		break;
	case 54:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		LoadWaveFile(SOUND_MONSTER + 165, "Data\\Sound\\mDrakan1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 166, "Data\\Sound\\mDrakanAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 167, "Data\\Sound\\mDrakanDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 165, 165, 166, 166, 167);
		break;
	case 55:
		LoadWaveFile(SOUND_MONSTER + 183, "Data\\Sound\\mPhoenix1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 184, "Data\\Sound\\mPhoenix1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 185, "Data\\Sound\\mPhoenixAttack1.wav", Channel, Enable);
		//LoadWaveFile(SOUND_MONSTER+186,"Data\\Sound\\mDarkPhoenixDie.wav"    ,Channel,Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 183, 184, 185, 185, -1);
	case 56:
		if (b->Actions)
		{
			b->Actions[MONSTER01_DIE].PlaySpeed = 0.22f;
		}
		//b->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.01f;
		//b->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.01f;
		break;
	case 62:
		LoadWaveFile(SOUND_MONSTER + 186, "Data\\Sound\\mMagicSkull.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 187, "Data\\Sound\\mMagicSkull.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 186, -1, -1, -1, 187);
		break;
	case 61:
		break;
	case 60:
		break;
	case 59:
		LoadWaveFile(SOUND_MONSTER + 4, "Data\\Sound\\mBullDie.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 188, "Data\\Sound\\mHunter2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 189, "Data\\Sound\\mBlackSkullDie.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 190, "Data\\Sound\\mBlackSkullAttack.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 188, -1, 190, -1, 189);
		break;
	case 58:
		LoadWaveFile(SOUND_MONSTER + 188, "Data\\Sound\\mHunter2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 191, "Data\\Sound\\mGhaintOrgerDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 188, -1, 190, -1, 191);
		break;
	case 57:
		LoadWaveFile(SOUND_MONSTER + 192, "Data\\Sound\\mRedSkull.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 193, "Data\\Sound\\mRedSkullDie.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 194, "Data\\Sound\\mRedSkullAttack.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 192, -1, 194, -1, 193);
		break;
	case 63:
		LoadWaveFile(SOUND_MONSTER + 195, "Data\\Sound\\mDAngelIdle.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 196, "Data\\Sound\\mDAngelAttack.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 197, "Data\\Sound\\mDAngelDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 195, 195, 196, 196, 197);
		break;
	case 64:
		LoadWaveFile(SOUND_MONSTER + 232, "Data\\Sound\\mKundunIdle.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 233, "Data\\Sound\\mKundunAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 234, "Data\\Sound\\mKundunAttack2.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 232, 232, 233, 234, -1);
		break;
	case 65:
		LoadWaveFile(SOUND_MONSTER + 198, "Data\\Sound\\mBSoldierIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 199, "Data\\Sound\\mBSoldierIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 200, "Data\\Sound\\mBSoldierAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 201, "Data\\Sound\\mBSoldierAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 202, "Data\\Sound\\mBSoldierDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 198, 199, 200, 201, 202);
		break;
	case 66:
		LoadWaveFile(SOUND_MONSTER + 203, "Data\\Sound\\mEsisIdle.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 204, "Data\\Sound\\mEsisAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 205, "Data\\Sound\\mEsisAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 206, "Data\\Sound\\mEsisDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 203, 203, 204, 205, 206);
		break;
	case 67:
		LoadWaveFile(SOUND_MONSTER + 207, "Data\\Sound\\mDsIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 208, "Data\\Sound\\mDsIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 209, "Data\\Sound\\mDsAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 210, "Data\\Sound\\mDsAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 211, "Data\\Sound\\mDsDeath.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 212, "Data\\Sound\\mLsIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 213, "Data\\Sound\\mLsIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 214, "Data\\Sound\\mLsAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 215, "Data\\Sound\\mLsAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 216, "Data\\Sound\\mLsDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216);
		break;
	case 68:
		LoadWaveFile(SOUND_MONSTER + 217, "Data\\Sound\\mNecronIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 218, "Data\\Sound\\mNecronIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 219, "Data\\Sound\\mNecronAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 220, "Data\\Sound\\mNecronAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 221, "Data\\Sound\\mNecronDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 217, 218, 219, 220, 221);
		break;
	case 69:
		LoadWaveFile(SOUND_MONSTER + 222, "Data\\Sound\\mSvIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 223, "Data\\Sound\\mSvIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 224, "Data\\Sound\\mSvAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 225, "Data\\Sound\\mSvAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 226, "Data\\Sound\\mSvDeath.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 227, "Data\\Sound\\mLvIdle1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 228, "Data\\Sound\\mLvIdle2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 229, "Data\\Sound\\mLvAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 230, "Data\\Sound\\mLvAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 231, "Data\\Sound\\mLvDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231);
		break;
	case 73:
		LoadWaveFile(SOUND_MONSTER + 232, "Data\\Sound\\BattleCastle\\oCDoorDis.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, -1, -1, 232);
		break;
	case 86:
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, -1, -1, -1);
		if (b->Actions)
		{
			b->Actions[MONSTER01_STOP1].PlaySpeed = 0.05f;
			b->Actions[MONSTER01_STOP2].PlaySpeed = 0.05f;
			b->Actions[MONSTER01_WALK].PlaySpeed = 0.1f;
		}
		break;
	case 76:
		LoadWaveFile(SOUND_MONSTER + 233, "Data\\Sound\\BattleCastle\\m_BowMercAttack.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 234, "Data\\Sound\\BattleCastle\\m_BowMercDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 233, 233, 234);
		break;
	case 77:
		LoadWaveFile(SOUND_MONSTER + 235, "Data\\Sound\\BattleCastle\\m_SpearMercAttack.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 236, "Data\\Sound\\BattleCastle\\m_SpearMercDeath.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, -1, -1, 235, 235, 236);
		break;
	case 79:
		break;
	case 128:
		LoadWaveFile(SOUND_ELBELAND_RABBITSTRANGE_ATTACK01, "Data\\Sound\\w52\\SE_Mon_rabbitstrange_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_RABBITSTRANGE_DEATH01, "Data\\Sound\\w52\\SE_Mon_rabbitstrange_death01.wav", 1);
		break;
	case 129:
		LoadWaveFile(SOUND_ELBELAND_RABBITUGLY_BREATH01, "Data\\Sound\\w52\\SE_Mon_rabbitugly_breath01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_RABBITUGLY_ATTACK01, "Data\\Sound\\w52\\SE_Mon_rabbitugly_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_RABBITUGLY_DEATH01, "Data\\Sound\\w52\\SE_Mon_rabbitugly_death01.wav", 1);
		break;
	case 130:
		LoadWaveFile(SOUND_ELBELAND_WOLFHUMAN_MOVE02, "Data\\Sound\\w52\\SE_Mon_wolfhuman_move02.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_WOLFHUMAN_ATTACK01, "Data\\Sound\\w52\\SE_Mon_wolfhuman_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_WOLFHUMAN_DEATH01, "Data\\Sound\\w52\\SE_Mon_wolfhuman_death01.wav", 1);
		break;
	case 131:
		LoadWaveFile(SOUND_ELBELAND_BUTTERFLYPOLLUTION_MOVE01, "Data\\Sound\\w52\\SE_Mon_butterflypollution_move01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_BUTTERFLYPOLLUTION_DEATH01, "Data\\Sound\\w52\\SE_Mon_butterflypollution_death01.wav", 1);
		break;
	case 132:
		LoadWaveFile(SOUND_ELBELAND_CURSERICH_MOVE01, "Data\\Sound\\w52\\SE_Mon_curserich_move01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_CURSERICH_ATTACK01, "Data\\Sound\\w52\\SE_Mon_curserich_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_CURSERICH_DEATH01, "Data\\Sound\\w52\\SE_Mon_curserich_death01.wav", 1);
		break;
	case 133:
		LoadWaveFile(SOUND_ELBELAND_TOTEMGOLEM_MOVE01, "Data\\Sound\\w52\\SE_Mon_totemgolem_move01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_TOTEMGOLEM_MOVE02, "Data\\Sound\\w52\\SE_Mon_totemgolem_move02.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_TOTEMGOLEM_ATTACK01, "Data\\Sound\\w52\\SE_Mon_totemgolem_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_TOTEMGOLEM_ATTACK02, "Data\\Sound\\w52\\SE_Mon_totemgolem_attack02.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_TOTEMGOLEM_DEATH01, "Data\\Sound\\w52\\SE_Mon_totemgolem_death01.wav", 1);
		break;
	case 134:
		LoadWaveFile(SOUND_ELBELAND_BEASTWOO_MOVE01, "Data\\Sound\\w52\\SE_Mon_beastwoo_move01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_BEASTWOO_ATTACK01, "Data\\Sound\\w52\\SE_Mon_beastwoo_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_BEASTWOO_DEATH01, "Data\\Sound\\w52\\SE_Mon_beastwoo_death01.wav", 1);
		break;
	case 135:
		LoadWaveFile(SOUND_ELBELAND_BEASTWOOLEADER_MOVE01, "Data\\Sound\\w52\\SE_Mon_beastwooleader_move01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_BEASTWOOLEADER_ATTACK01, "Data\\Sound\\w52\\SE_Mon_beastwooleader_attack01.wav", 1);
		LoadWaveFile(SOUND_ELBELAND_BEASTWOO_DEATH01, "Data\\Sound\\w52\\SE_Mon_beastwoo_death01.wav", 1);
		break;
	case 136:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_UNUS_ATTACK01, "Data\\Sound\\w57\\Sapi-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_DEATH01, "Data\\Sound\\w57\\Sapi-Death.wav", 1);
		break;
	case 137:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_UNUS_ATTACK01, "Data\\Sound\\w57\\Sapi-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_DEATH01, "Data\\Sound\\w57\\Sapi-Death.wav", 1);
		break;
	case 138:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_TRES_ATTACK01, "Data\\Sound\\w57\\Sapi-Attack1.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SAPI_DEATH01, "Data\\Sound\\w57\\Sapi-Death.wav", 1);
		break;
	case 139:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_PAWN_ATTACK01, "Data\\Sound\\w57\\ShadowPawn-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_DEATH01, "Data\\Sound\\w57\\Shadow-Death.wav", 1);
		break;
	case 140:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_KNIGHT_ATTACK01, "Data\\Sound\\w57\\ShadowKnight-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_DEATH01, "Data\\Sound\\w57\\Shadow-Death.wav", 1);
		break;
	case 141:
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_ROOK_ATTACK01, "Data\\Sound\\w57\\ShadowRook-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_SHADOW_DEATH01, "Data\\Sound\\w57\\Shadow-Death.wav", 1);
		break;
	case 142:
		LoadWaveFile(SOUND_SWAMPOFQUIET_THUNDER_NAIPIN_BREATH01, "Data\\Sound\\w57\\Naipin-Thunder.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_ATTACK01, "Data\\Sound\\w57\\Naipin-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_DEATH01, "Data\\Sound\\w57\\Naipin-Death.wav", 1);
		break;
	case 143:
		LoadWaveFile(SOUND_SWAMPOFQUIET_GHOST_NAIPIN_BREATH01, "Data\\Sound\\w57\\Naipin-Ghost.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_ATTACK01, "Data\\Sound\\w57\\Naipin-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_DEATH01, "Data\\Sound\\w57\\Naipin-Death.wav", 1);
		break;
	case 144:
		LoadWaveFile(SOUND_SWAMPOFQUIET_BLAZE_NAIPIN_BREATH01, "Data\\Sound\\w57\\Naipin-Blaze.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_ATTACK01, "Data\\Sound\\w57\\Naipin-Attack.wav", 1);
		LoadWaveFile(SOUND_SWAMPOFQUIET_NAIPIN_DEATH01, "Data\\Sound\\w57\\Naipin-Death.wav", 1);
		break;
	case 145:
		LoadWaveFile(SOUND_ELBELAND_WOLFHUMAN_DEATH01, "Data\\Sound\\w52\\SE_Mon_wolfhuman_death01.wav", 1);
		LoadWaveFile(SOUND_RAKLION_ICEWALKER_ATTACK, "Data\\Sound\\w58w59\\IceWalker_attack.wav", 1);
		LoadWaveFile(SOUND_RAKLION_ICEWALKER_MOVE, "Data\\Sound\\w58w59\\IceWalker_move.wav", 1);
		break;
		LoadWaveFile(SOUND_RAKLION_GIANT_MAMUD_MOVE, "Data\\Sound\\w57\\GiantMammoth_move.wav", 1);
		LoadWaveFile(SOUND_RAKLION_GIANT_MAMUD_ATTACK, "Data\\Sound\\w57\\GiantMammoth_attack.wav", 1);
		LoadWaveFile(SOUND_RAKLION_GIANT_MAMUD_DEATH, "Data\\Sound\\w57\\GiantMammoth_death.wav", 1);
	case 147:
		LoadWaveFile(SOUND_RAKLION_ICEGIANT_MOVE, "Data\\Sound\\w58w59\\IceGiant_move.wav", 1);
		LoadWaveFile(SOUND_RAKLION_ICEGIANT_DEATH, "Data\\Sound\\w58w59\\IceGiant_death.wav", 1);
		break;
	case 148:
		LoadWaveFile(SOUND_MONSTER + 34, "Data\\Sound\\m헬스파이더죽기.wav", 1);
		LoadWaveFile(SOUND_RAKLION_COOLERTIN_ATTACK, "Data\\Sound\\w58w59\\Coolertin_attack.wav", 1);
		LoadWaveFile(SOUND_RAKLION_COOLERTIN_MOVE, "Data\\Sound\\w58w59\\Coolertin_move.wav", 1);
		break;
	case 149:
		LoadWaveFile(SOUND_RAKLION_IRON_KNIGHT_MOVE, "Data\\Sound\\w58w59\\IronKnight_move.wav", 1);
		LoadWaveFile(SOUND_RAKLION_IRON_KNIGHT_ATTACK, "Data\\Sound\\w58w59\\IronKnight_attack.wav", 1);
		LoadWaveFile(SOUND_MONSTER + 154, "Data\\Sound\\death1.wav", 1);
		break;
	case 150:
		LoadWaveFile(SOUND_RAKLION_SERUFAN_ATTACK1, "Data\\Sound\\w58w59\\Selupan_attack1.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_ATTACK2, "Data\\Sound\\w58w59\\Selupan_attack2.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_CURE, "Data\\Sound\\w58w59\\Selupan_cure.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_RAGE, "Data\\Sound\\w58w59\\Selupan_rage.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_WORD1, "Data\\Sound\\w58w59\\Selupan_word1.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_WORD2, "Data\\Sound\\w58w59\\Selupan_word2.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_WORD3, "Data\\Sound\\w58w59\\Selupan_word3.wav", 1);
		LoadWaveFile(SOUND_RAKLION_SERUFAN_WORD4, "Data\\Sound\\w58w59\\Selupan_word4.wav", 1);
		break;
	case 155:
		LoadWaveFile(SOUND_XMAS_SANTA_IDLE_1, "Data\\Sound\\xmas\\DarkSanta_Idle01.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_IDLE_2, "Data\\Sound\\xmas\\DarkSanta_Idle02.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_WALK_1, "Data\\Sound\\xmas\\DarkSanta_Walk01.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_WALK_2, "Data\\Sound\\xmas\\DarkSanta_Walk02.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_ATTACK_1, "Data\\Sound\\xmas\\DarkSanta_Attack01.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_DAMAGE_1, "Data\\Sound\\xmas\\DarkSanta_Damage01.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_DAMAGE_2, "Data\\Sound\\xmas\\DarkSanta_Damage02.wav");
		LoadWaveFile(SOUND_XMAS_SANTA_DEATH_1, "Data\\Sound\\xmas\\DarkSanta_Death01.wav");
		break;
	case 156:
		LoadWaveFile(SOUND_MONSTER + 72, "Data\\Sound\\mGoblin1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 73, "Data\\Sound\\mGoblin2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 74, "Data\\Sound\\mGoblinAttack1.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 75, "Data\\Sound\\mGoblinAttack2.wav", Channel, Enable);
		LoadWaveFile(SOUND_MONSTER + 76, "Data\\Sound\\mGoblinDie.wav", Channel, Enable);
		SetMonsterSound(MODEL_MONSTER01 + Type, 72, 73, 74, 75, 76);
		break;
	case 157:
	{
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_ATTACK, "Data\\Sound\\w64\\ZombieWarrior_attack.wav");
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_DAMAGE01, "Data\\Sound\\w64\\ZombieWarrior_damage01.wav");
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_DAMAGE02, "Data\\Sound\\w64\\ZombieWarrior_damage02.wav");
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_DEATH, "Data\\Sound\\w64\\ZombieWarrior_death.wav");
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_MOVE01, "Data\\Sound\\w64\\ZombieWarrior_move01.wav");
		LoadWaveFile(SOUND_PKFIELD_ZOMBIEWARRIOR_MOVE02, "Data\\Sound\\w64\\ZombieWarrior_move02.wav");
	}
	break;
	case 158:
	{
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_ATTACK, "Data\\Sound\\w64\\RaisedGladiator_attack.wav");
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_DAMAGE01, "Data\\Sound\\w64\\RaisedGladiator_damage01.wav");
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_DAMAGE02, "Data\\Sound\\w64\\RaisedGladiator_damage02.wav");
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_DEATH, "Data\\Sound\\w64\\RaisedGladiator_death.wav");
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_MOVE01, "Data\\Sound\\w64\\RaisedGladiator_move01.wav");
		LoadWaveFile(SOUND_PKFIELD_RAISEDGLADIATOR_MOVE02, "Data\\Sound\\w64\\RaisedGladiator_move02.wav");
	}
	break;
	case 159:
	{
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_ATTACK, "Data\\Sound\\w64\\AshesButcher_attack.wav");
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_DAMAGE01, "Data\\Sound\\w64\\AshesButcher_damage01.wav");
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_DAMAGE02, "Data\\Sound\\w64\\AshesButcher_damage02.wav");
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_DEATH, "Data\\Sound\\w64\\AshesButcher_death.wav");
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_MOVE01, "Data\\Sound\\w64\\AshesButcher_move01.wav");
		LoadWaveFile(SOUND_PKFIELD_ASHESBUTCHER_MOVE02, "Data\\Sound\\w64\\AshesButcher_move02.wav");
	}
	break;
	case 160:
	{
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_ATTACK, "Data\\Sound\\w64\\BloodAssassin_attack.wav");
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_DAMAGE01, "Data\\Sound\\w64\\BloodAssassin_damage01.wav");
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_DAMAGE02, "Data\\Sound\\w64\\BloodAssassin_damage02.wav");
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_DEDTH, "Data\\Sound\\w64\\BloodAssassin_death.wav");
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_MOVE01, "Data\\Sound\\w64\\BloodAssassin_move01.wav");
		LoadWaveFile(SOUND_PKFIELD_BLOODASSASSIN_MOVE02, "Data\\Sound\\w64\\BloodAssassin_move02.wav");
	}
	break;
	case 162:
	{
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_ATTACK01, "Data\\Sound\\w64\\BurningLavaGolem_attack01.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_ATTACK02, "Data\\Sound\\w64\\BurningLavaGolem_attack02.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_DAMAGE01, "Data\\Sound\\w64\\BurningLavaGolem_damage01.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_DAMAGE02, "Data\\Sound\\w64\\BurningLavaGolem_damage02.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_DEATH, "Data\\Sound\\w64\\BurningLavaGolem_death.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_MOVE01, "Data\\Sound\\w64\\BurningLavaGolem_move01.wav");
		LoadWaveFile(SOUND_PKFIELD_BURNINGLAVAGOLEM_MOVE02, "Data\\Sound\\w64\\BurningLavaGolem_move02.wav");
	}
	break;
	case 190:
	{
		LoadWaveFile(SOUND_DOPPELGANGER_RED_BUGBEAR_ATTACK, "Data\\Sound\\Doppelganger\\Angerbutcher_attack.wav");
		LoadWaveFile(SOUND_DOPPELGANGER_RED_BUGBEAR_DEATH, "Data\\Sound\\Doppelganger\\Angerbutcher_death.wav");
	}
	break;
	case 189:
	{
		LoadWaveFile(SOUND_DOPPELGANGER_BUGBEAR_ATTACK, "Data\\Sound\\Doppelganger\\Butcher_attack.wav");
		LoadWaveFile(SOUND_DOPPELGANGER_BUGBEAR_DEATH, "Data\\Sound\\Doppelganger\\Butcher_death.wav");
	}
	break;
	case 191:
	{
		LoadWaveFile(SOUND_DOPPELGANGER_SLIME_ATTACK, "Data\\Sound\\Doppelganger\\Doppelganger_attack.wav");
		LoadWaveFile(SOUND_DOPPELGANGER_SLIME_DEATH, "Data\\Sound\\Doppelganger\\Doppelganger_death.wav");
	}
	break;
	case 209:
		LoadWaveFile(SOUND_KARUTAN_TCSCORPION_ATTACK, "Data\\Sound\\Karutan\\ToxyChainScorpion_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_TCSCORPION_DEATH, "Data\\Sound\\Karutan\\ToxyChainScorpion_death.wav");
		LoadWaveFile(SOUND_KARUTAN_TCSCORPION_HIT, "Data\\Sound\\Karutan\\ToxyChainScorpion_hit.wav");
		break;
	case 210:
		LoadWaveFile(SOUND_KARUTAN_BONESCORPION_ATTACK, "Data\\Sound\\Karutan\\BoneScorpion_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_BONESCORPION_DEATH, "Data\\Sound\\Karutan\\BoneScorpion_death.wav");
		LoadWaveFile(SOUND_KARUTAN_BONESCORPION_HIT, "Data\\Sound\\Karutan\\BoneScorpion_hit.wav");
		break;
	case 211:
		LoadWaveFile(SOUND_KARUTAN_ORCUS_MOVE1, "Data\\Sound\\Karutan\\Orcus_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_ORCUS_MOVE2, "Data\\Sound\\Karutan\\Orcus_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_ORCUS_ATTACK1, "Data\\Sound\\Karutan\\Orcus_attack_1.wav");
		LoadWaveFile(SOUND_KARUTAN_ORCUS_ATTACK2, "Data\\Sound\\Karutan\\Orcus_attack_2.wav");
		LoadWaveFile(SOUND_KARUTAN_ORCUS_DEATH, "Data\\Sound\\Karutan\\Orcus_death.wav");
		break;
	case 212:
		LoadWaveFile(SOUND_KARUTAN_GOLOCH_MOVE1, "Data\\Sound\\Karutan\\Goloch_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_GOLOCH_MOVE2, "Data\\Sound\\Karutan\\Goloch_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_GOLOCH_ATTACK, "Data\\Sound\\Karutan\\Goloch_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_GOLOCH_DEATH, "Data\\Sound\\Karutan\\Goloch_death.wav");
		break;
	case 213:
		LoadWaveFile(SOUND_KARUTAN_CRYPTA_MOVE1, "Data\\Sound\\Karutan\\Crypta_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPTA_MOVE2, "Data\\Sound\\Karutan\\Crypta_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPTA_ATTACK, "Data\\Sound\\Karutan\\Crypta_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPTA_DEATH, "Data\\Sound\\Karutan\\Crypta_death.wav");
		break;
	case 214:
		LoadWaveFile(SOUND_KARUTAN_CRYPOS_MOVE1, "Data\\Sound\\Karutan\\Crypos_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPOS_MOVE2, "Data\\Sound\\Karutan\\Crypos_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPOS_ATTACK1, "Data\\Sound\\Karutan\\Crypos_attack_1.wav");
		LoadWaveFile(SOUND_KARUTAN_CRYPOS_ATTACK2, "Data\\Sound\\Karutan\\Crypos_attack_2.wav");
		break;
	case 215:
		LoadWaveFile(SOUND_KARUTAN_CONDRA_MOVE1, "Data\\Sound\\Karutan\\Condra_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_CONDRA_MOVE2, "Data\\Sound\\Karutan\\Condra_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_CONDRA_ATTACK, "Data\\Sound\\Karutan\\Condra_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_CONDRA_DEATH, "Data\\Sound\\Karutan\\Condra_death.wav");
		break;
	case 216:
		LoadWaveFile(SOUND_KARUTAN_CONDRA_MOVE1, "Data\\Sound\\Karutan\\Condra_move1.wav");
		LoadWaveFile(SOUND_KARUTAN_CONDRA_MOVE2, "Data\\Sound\\Karutan\\Condra_move2.wav");
		LoadWaveFile(SOUND_KARUTAN_NARCONDRA_ATTACK, "Data\\Sound\\Karutan\\NarCondra_attack.wav");
		LoadWaveFile(SOUND_KARUTAN_CONDRA_DEATH, "Data\\Sound\\Karutan\\Condra_death.wav");
		break;
	}
}

void OpenMonsterModels()
{

}

void OpenSkills()
{
	gmClientModels->AccessModel(MODEL_ICE, "Data\\Skill\\", "Ice", 1);
	gmClientModels->AccessModel(MODEL_ICE_SMALL, "Data\\Skill\\", "Ice", 2);
	gmClientModels->AccessModel(MODEL_FIRE, "Data\\Skill\\", "Fire", 1);
	gmClientModels->AccessModel(MODEL_POISON, "Data\\Skill\\", "Poison", 1);
	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_STONE1 + i, "Data\\Skill\\", "Stone", i + 1);
	gmClientModels->AccessModel(MODEL_CIRCLE, "Data\\Skill\\", "Circle", 1);
	gmClientModels->AccessModel(MODEL_CIRCLE_LIGHT, "Data\\Skill\\", "Circle", 2);
	gmClientModels->AccessModel(MODEL_MAGIC1, "Data\\Skill\\", "Magic", 1);
	gmClientModels->AccessModel(MODEL_MAGIC2, "Data\\Skill\\", "Magic", 2);
	gmClientModels->AccessModel(MODEL_STORM, "Data\\Skill\\", "Storm", 1);
	gmClientModels->AccessModel(MODEL_LASER, "Data\\Skill\\", "Laser", 1);

	for (int i = 0; i < 3; i++)
		gmClientModels->AccessModel(MODEL_SKELETON1 + i, "Data\\Skill\\", "Skeleton", i + 1);

	gmClientModels->AccessModel(MODEL_SKELETON_PCBANG, "Data\\Skill\\", "Skeleton", 3);
	gmClientModels->AccessModel(MODEL_HALLOWEEN, "Data\\Skill\\", "Jack");

	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_BLUE, "Data\\Skill\\", "hcandyblue");
	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_ORANGE, "Data\\Skill\\", "hcandyorange");
	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_RED, "Data\\Skill\\", "hcandyred");
	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_YELLOW, "Data\\Skill\\", "hcandyyellow");
	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_HOBAK, "Data\\Skill\\", "hhobak");
	gmClientModels->AccessModel(MODEL_HALLOWEEN_CANDY_STAR, "Data\\Skill\\", "hstar");
	LoadBitmap("Skill\\jack04.jpg", BITMAP_JACK_1);
	LoadBitmap("Skill\\jack05.jpg", BITMAP_JACK_2);
	LoadBitmap("Monster\\iui02.tga", BITMAP_ROBE + 3);
	gmClientModels->AccessModel(MODEL_POTION + 45, "Data\\Item\\", "hobakhead");
	gmClientModels->OpenTexture(MODEL_POTION + 45, "Item\\Texture\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ALLIED_PLAYER, "Data\\Skill\\", "unitedsoldier");
	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_ILLUSION_PLAYER, "Data\\Skill\\", "illusionist");

	gmClientModels->AccessModel(MODEL_WOOSISTONE, "Data\\Skill\\", "woositone");
	gmClientModels->OpenTexture(MODEL_WOOSISTONE, "Skill\\");

	g_NewYearsDayEvent->LoadModel();

	gmClientModels->AccessModel(MODEL_SAW, "Data\\Skill\\", "Saw", 1);

	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_BONE1 + i, "Data\\Skill\\", "Bone", i + 1);

	for (int i = 0; i < 3; i++)
		gmClientModels->AccessModel(MODEL_SNOW1 + i, "Data\\Skill\\", "Snow", i + 1);

	gmClientModels->AccessModel(MODEL_UNICON, "Data\\Skill\\", "Rider", 1);
	gmClientModels->AccessModel(MODEL_PEGASUS, "Data\\Skill\\", "Rider", 2);
	gmClientModels->AccessModel(MODEL_DARK_HORSE, "Data\\Skill\\", "DarkHorse");

	gmClientModels->AccessModel(MODEL_FENRIR_BLACK, "Data\\Skill\\", "fenril_black");
	gmClientModels->OpenTexture(MODEL_FENRIR_BLACK, "Skill\\");

	gmClientModels->AccessModel(MODEL_FENRIR_RED, "Data\\Skill\\", "fenril_red");
	gmClientModels->OpenTexture(MODEL_FENRIR_RED, "Skill\\");

	gmClientModels->AccessModel(MODEL_FENRIR_BLUE, "Data\\Skill\\", "fenril_blue");
	gmClientModels->OpenTexture(MODEL_FENRIR_BLUE, "Skill\\");

	gmClientModels->AccessModel(MODEL_FENRIR_GOLD, "Data\\Skill\\", "fenril_gold");
	gmClientModels->OpenTexture(MODEL_FENRIR_BLUE, "Skill\\");

	gmClientModels->AccessModel(MODEL_PANDA, "Data\\Item\\", "panda");
	gmClientModels->OpenTexture(MODEL_PANDA, "Item\\Texture\\");

	gmClientModels->AccessModel(MODEL_SKELETON_CHANGED, "Data\\Item\\", "trans_skeleton");
	gmClientModels->OpenTexture(MODEL_SKELETON_CHANGED, "Item\\Texture\\");


	gmClientModels->AccessModel(MODEL_DARK_SPIRIT, "Data\\Skill\\", "DarkSpirit");
	LoadBitmap("Skill\\dkthreebody_r.jpg", BITMAP_MONSTER_SKIN + 2, GL_LINEAR, GL_REPEAT);

	gmClientModels->AccessModel(MODEL_WARCRAFT, "Data\\Skill\\", "HellGate");
	gmClientModels->GetModel(MODEL_WARCRAFT)->Actions[0].LockPositions = false;
	SAFE_DELETE_ARRAY(gmClientModels->GetModel(MODEL_WARCRAFT)->Actions[0].Positions);
	gmClientModels->GetModel(MODEL_WARCRAFT)->Actions[0].PlaySpeed = 0.15f;

	gmClientModels->AccessModel(MODEL_ARROW, "Data\\Skill\\", "Arrow", 1);
	gmClientModels->AccessModel(MODEL_ARROW_STEEL, "Data\\Skill\\", "ArrowSteel", 1);
	gmClientModels->AccessModel(MODEL_ARROW_THUNDER, "Data\\Skill\\", "ArrowThunder", 1);
	gmClientModels->AccessModel(MODEL_ARROW_LASER, "Data\\Skill\\", "ArrowLaser", 1);
	gmClientModels->AccessModel(MODEL_ARROW_V, "Data\\Skill\\", "ArrowV", 1);
	gmClientModels->AccessModel(MODEL_ARROW_SAW, "Data\\Skill\\", "ArrowSaw", 1);
	gmClientModels->AccessModel(MODEL_ARROW_NATURE, "Data\\Skill\\", "ArrowNature", 1);

	gmClientModels->OpenTexture(MODEL_MAGIC_CAPSULE2, "Skill\\");
	gmClientModels->AccessModel(MODEL_MAGIC_CAPSULE2, "Data\\Skill\\", "Protect", 2);

	gmClientModels->AccessModel(MODEL_ARROW_SPARK, "Data\\Skill\\", "Arrow_Spark");
	gmClientModels->OpenTexture(MODEL_ARROW_SPARK, "Skill\\");

	gmClientModels->AccessModel(MODEL_DARK_SCREAM, "Data\\Skill\\", "darkfirescrem02");
	gmClientModels->OpenTexture(MODEL_DARK_SCREAM, "Skill\\");
	gmClientModels->AccessModel(MODEL_DARK_SCREAM_FIRE, "Data\\Skill\\", "darkfirescrem01");
	gmClientModels->OpenTexture(MODEL_DARK_SCREAM_FIRE, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMON, "Data\\SKill\\", "nightmaresum");
	gmClientModels->OpenTexture(MODEL_SUMMON, "SKill\\");
	gmClientModels->AccessModel(MODEL_MULTI_SHOT1, "Data\\Effect\\", "multishot01");
	gmClientModels->OpenTexture(MODEL_MULTI_SHOT1, "Effect\\");
	gmClientModels->AccessModel(MODEL_MULTI_SHOT2, "Data\\Effect\\", "multishot02");
	gmClientModels->OpenTexture(MODEL_MULTI_SHOT2, "Effect\\");
	gmClientModels->AccessModel(MODEL_MULTI_SHOT3, "Data\\Effect\\", "multishot03");
	gmClientModels->OpenTexture(MODEL_MULTI_SHOT3, "Effect\\");
	gmClientModels->AccessModel(MODEL_DESAIR, "Data\\SKill\\", "desair");
	gmClientModels->OpenTexture(MODEL_DESAIR, "SKill\\");
	gmClientModels->AccessModel(MODEL_ARROW_RING, "Data\\Skill\\", "CW_Bow_Skill");
	gmClientModels->OpenTexture(MODEL_ARROW_RING, "Skill\\");

	gmClientModels->AccessModel(MODEL_ARROW_DARKSTINGER, "Data\\Skill\\", "sketbows_arrows");
	gmClientModels->OpenTexture(MODEL_ARROW_DARKSTINGER, "Skill\\");
	gmClientModels->AccessModel(MODEL_FEATHER, "Data\\Skill\\", "darkwing_hetachi");
	gmClientModels->OpenTexture(MODEL_FEATHER, "Skill\\");
	gmClientModels->AccessModel(MODEL_FEATHER_FOREIGN, "Data\\Skill\\", "darkwing_hetachi");
	gmClientModels->OpenTexture(MODEL_FEATHER_FOREIGN, "Skill\\");
	LoadBitmap("Effect\\Bugbear_R.jpg", BITMAP_BUGBEAR_R, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\pk_mon02_fire.jpg", BITMAP_PKMON01, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\pk_mon03_red.jpg", BITMAP_PKMON02, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\pk_mon02_green.jpg", BITMAP_PKMON03, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\pk_mon03_green.jpg", BITMAP_PKMON04, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\lavagiantAa_e.jpg", BITMAP_PKMON05, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\lavagiantBa_e.jpg", BITMAP_PKMON06, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\eff_magma_red.jpg", BITMAP_LAVAGIANT_FOOTPRINT_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\eff_magma_violet.jpg", BITMAP_LAVAGIANT_FOOTPRINT_V, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\raymond_sword_R.jpg", BITMAP_RAYMOND_SWORD, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\mist01.jpg", BITMAP_AG_ADDITION_EFFECT, GL_LINEAR, GL_REPEAT);
	gmClientModels->AccessModel(MODEL_ARROW_GAMBLE, "Data\\Skill\\", "gamble_arrows01");
	gmClientModels->OpenTexture(MODEL_ARROW_GAMBLE, "Skill\\");

	gmClientModels->OpenTexture(MODEL_SPEARSKILL, "Skill\\");
	gmClientModels->AccessModel(MODEL_SPEARSKILL, "Data\\Skill\\", "RidingSpear", 1);
	gmClientModels->AccessModel(MODEL_PROTECT, "Data\\Skill\\", "Protect", 1);

	for (int i = 0; i < 2; i++)
		gmClientModels->AccessModel(MODEL_BIG_STONE1 + i, "Data\\Skill\\", "BigStone", i + 1);

	gmClientModels->AccessModel(MODEL_MAGIC_CIRCLE1, "Data\\Skill\\", "MagicCircle", 1);
	gmClientModels->AccessModel(MODEL_ARROW_WING, "Data\\Skill\\", "ArrowWing", 1);
	gmClientModels->AccessModel(MODEL_ARROW_BOMB, "Data\\Skill\\", "ArrowBomb", 1);
	gmClientModels->AccessModel(MODEL_BALL, "Data\\Skill\\", "Ball", 1);//공

	gmClientModels->GetModel(MODEL_BALL)->Actions[0].PlaySpeed = 0.5f;

	gmClientModels->AccessModel(MODEL_SKILL_BLAST, "Data\\Skill\\", "Blast", 1);
	gmClientModels->AccessModel(MODEL_SKILL_INFERNO, "Data\\Skill\\", "Inferno", 1);
	gmClientModels->AccessModel(MODEL_ARROW_DOUBLE, "Data\\Skill\\", "ArrowDouble", 1);

	gmClientModels->AccessModel(MODEL_ARROW_BEST_CROSSBOW, "Data\\Skill\\", "KCross");
	gmClientModels->AccessModel(MODEL_ARROW_DRILL, "Data\\Skill\\", "Carow");
	gmClientModels->AccessModel(MODEL_COMBO, "Data\\Skill\\", "combo");

	gmClientModels->AccessModel(MODEL_GATE + 0, "Data\\Object12\\", "Gate", 1);
	gmClientModels->AccessModel(MODEL_GATE + 1, "Data\\Object12\\", "Gate", 2);
	gmClientModels->AccessModel(MODEL_STONE_COFFIN + 0, "Data\\Object12\\", "StoneCoffin", 1);
	gmClientModels->AccessModel(MODEL_STONE_COFFIN + 1, "Data\\Object12\\", "StoneCoffin", 2);

	for (int i = 0; i < 2; ++i)
	{
		gmClientModels->OpenTexture(MODEL_GATE + 1, "Monster\\");
		gmClientModels->OpenTexture(MODEL_STONE_COFFIN + i, "Monster\\");
	}

	gmClientModels->AccessModel(MODEL_AIR_FORCE, "Data\\Skill\\", "AirForce");
	gmClientModels->AccessModel(MODEL_WAVES, "Data\\Skill\\", "m_Waves");
	gmClientModels->AccessModel(MODEL_PIERCING2, "Data\\Skill\\", "m_Piercing");
	gmClientModels->AccessModel(MODEL_PIER_PART, "Data\\Skill\\", "PierPart");
	gmClientModels->AccessModel(MODEL_DARKLORD_SKILL, "Data\\Skill\\", "DarkLordSkill");
	gmClientModels->AccessModel(MODEL_GROUND_STONE, "Data\\Skill\\", "groundStone");
	gmClientModels->GetModel(MODEL_GROUND_STONE)->Actions[0].Loop = false;
	gmClientModels->AccessModel(MODEL_GROUND_STONE2, "Data\\Skill\\", "groundStone2");
	gmClientModels->GetModel(MODEL_GROUND_STONE2)->Actions[0].Loop = false;
	gmClientModels->AccessModel(MODEL_WATER_WAVE, "Data\\Skill\\", "seamanFX");
	gmClientModels->AccessModel(MODEL_SKULL, "Data\\Skill\\", "Skull");
	gmClientModels->AccessModel(MODEL_LACEARROW, "Data\\Skill\\", "LaceArrow");
	gmClientModels->OpenTexture(MODEL_LACEARROW, "Item\\Texture\\", GL_CLAMP_TO_EDGE);

	gmClientModels->AccessModel(MODEL_MANY_FLAG, "Data\\Skill\\", "ManyFlag");
	gmClientModels->AccessModel(MODEL_WEBZEN_MARK, "Data\\Skill\\", "MuSign");
	gmClientModels->AccessModel(MODEL_STUN_STONE, "Data\\Skill\\", "GroundCrystal");
	gmClientModels->AccessModel(MODEL_SKIN_SHELL, "Data\\Skill\\", "skinshell");
	gmClientModels->AccessModel(MODEL_MANA_RUNE, "Data\\Skill\\", "ManaRune");
	gmClientModels->AccessModel(MODEL_SKILL_JAVELIN, "Data\\Skill\\", "Javelin");
	gmClientModels->AccessModel(MODEL_ARROW_IMPACT, "Data\\Skill\\", "ArrowImpact");
	gmClientModels->AccessModel(MODEL_SWORD_FORCE, "Data\\Skill\\", "SwordForce");

	gmClientModels->AccessModel(MODEL_FLY_BIG_STONE1, "Data\\Skill\\", "FlyBigStone1");
	gmClientModels->AccessModel(MODEL_FLY_BIG_STONE2, "Data\\Skill\\", "FlyBigStone2");
	gmClientModels->AccessModel(MODEL_BIG_STONE_PART1, "Data\\Skill\\", "FlySmallStone1");
	gmClientModels->AccessModel(MODEL_BIG_STONE_PART2, "Data\\Skill\\", "FlySmallStone2");
	gmClientModels->AccessModel(MODEL_WALL_PART1, "Data\\Skill\\", "WallStone1");
	gmClientModels->AccessModel(MODEL_WALL_PART2, "Data\\Skill\\", "WallStone2");
	gmClientModels->AccessModel(MODEL_GATE_PART1, "Data\\Skill\\", "GatePart1");
	gmClientModels->AccessModel(MODEL_GATE_PART2, "Data\\Skill\\", "GatePart2");
	gmClientModels->AccessModel(MODEL_GATE_PART3, "Data\\Skill\\", "GatePart3");
	gmClientModels->AccessModel(MODEL_AURORA, "Data\\Skill\\", "Aurora");
	gmClientModels->AccessModel(MODEL_TOWER_GATE_PLANE, "Data\\Skill\\", "TowerGatePlane");
	gmClientModels->AccessModel(MODEL_GOLEM_STONE, "Data\\Skill\\", "golem_stone");
	gmClientModels->AccessModel(MODEL_FISSURE, "Data\\Skill\\", "bossrock");
	gmClientModels->AccessModel(MODEL_FISSURE_LIGHT, "Data\\Skill\\", "bossrocklight");
	gmClientModels->AccessModel(MODEL_PROTECTGUILD, "Data\\Skill\\", "ProtectGuild");
	gmClientModels->AccessModel(MODEL_DARK_ELF_SKILL, "Data\\Skill\\", "elf_skill");
	gmClientModels->AccessModel(MODEL_BALGAS_SKILL, "Data\\Skill\\", "WaveForce");
	gmClientModels->AccessModel(MODEL_DEATH_SPI_SKILL, "Data\\Skill\\", "deathsp_eff");

	if (gmClientModels->GetModel(MODEL_BALGAS_SKILL)->Actions)
	{
		gmClientModels->GetModel(MODEL_BALGAS_SKILL)->Actions[0].Loop = false;
	}

	gmClientModels->OpenTexture(MODEL_DARK_ELF_SKILL, "Skill\\");
	gmClientModels->OpenTexture(MODEL_BALGAS_SKILL, "Skill\\");
	gmClientModels->OpenTexture(MODEL_DEATH_SPI_SKILL, "Skill\\");

	gmClientModels->OpenTexture(MODEL_DARK_HORSE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_DARK_SPIRIT, "Skill\\");

	gmClientModels->OpenTexture(MODEL_WARCRAFT, "Skill\\");
	gmClientModels->OpenTexture(MODEL_PEGASUS, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 1, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 2, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 3, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 5, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 7, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKILL_FURY_STRIKE + 8, "Skill\\");
	gmClientModels->OpenTexture(MODEL_WAVE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_TAIL, "Skill\\");

	gmClientModels->OpenTexture(MODEL_WAVE_FORCE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_BLIZZARD, "Skill\\");

	g_XmasEvent->LoadXmasEvent();

	gmClientModels->OpenTexture(MODEL_ARROW_BEST_CROSSBOW, "Skill\\");
	gmClientModels->OpenTexture(MODEL_ARROW_DRILL, "Skill\\");
	gmClientModels->OpenTexture(MODEL_COMBO, "Skill\\");

	gmClientModels->OpenTexture(MODEL_AIR_FORCE, "Skill\\", GL_CLAMP_TO_EDGE);
	gmClientModels->OpenTexture(MODEL_WAVES, "Skill\\");
	gmClientModels->OpenTexture(MODEL_PIERCING2, "Skill\\");
	gmClientModels->OpenTexture(MODEL_PIER_PART, "Skill\\");
	gmClientModels->OpenTexture(MODEL_GROUND_STONE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_GROUND_STONE2, "Skill\\");
	gmClientModels->OpenTexture(MODEL_WATER_WAVE, "Skill\\", GL_CLAMP_TO_EDGE);
	gmClientModels->OpenTexture(MODEL_SKULL, "Skill\\", GL_CLAMP_TO_EDGE);
	gmClientModels->OpenTexture(MODEL_MANY_FLAG, "Skill\\");
	gmClientModels->OpenTexture(MODEL_WEBZEN_MARK, "Skill\\");

	gmClientModels->OpenTexture(MODEL_FLY_BIG_STONE1, "Npc\\");
	gmClientModels->OpenTexture(MODEL_FLY_BIG_STONE2, "Skill\\");
	gmClientModels->OpenTexture(MODEL_BIG_STONE_PART1, "Skill\\");
	gmClientModels->OpenTexture(MODEL_BIG_STONE_PART2, "Skill\\");
	gmClientModels->OpenTexture(MODEL_WALL_PART1, "Object31\\");
	gmClientModels->OpenTexture(MODEL_WALL_PART2, "Object31\\");
	gmClientModels->OpenTexture(MODEL_GATE_PART1, "Monster\\");
	gmClientModels->OpenTexture(MODEL_GATE_PART2, "Monster\\");
	gmClientModels->OpenTexture(MODEL_GATE_PART3, "Monster\\");
	gmClientModels->OpenTexture(MODEL_AURORA, "Monster\\");
	gmClientModels->OpenTexture(MODEL_TOWER_GATE_PLANE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_GOLEM_STONE, "Monster\\");
	gmClientModels->OpenTexture(MODEL_FISSURE, "Skill\\");
	gmClientModels->OpenTexture(MODEL_FISSURE_LIGHT, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SKIN_SHELL, "Effect\\");
	gmClientModels->OpenTexture(MODEL_PROTECTGUILD, "Item\\Texture\\");

	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 1, "Data\\Skill\\", "EarthQuake", 1);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 2, "Data\\Skill\\", "EarthQuake", 2);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 3, "Data\\Skill\\", "EarthQuake", 3);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 5, "Data\\Skill\\", "EarthQuake", 5);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 7, "Data\\Skill\\", "EarthQuake", 7);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 8, "Data\\Skill\\", "EarthQuake", 8);
	gmClientModels->AccessModel(MODEL_WAVE, "Data\\Skill\\", "flashing");
	gmClientModels->AccessModel(MODEL_TAIL, "Data\\Skill\\", "tail");

	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 4, "Data\\Skill\\", "EarthQuake", 4);
	gmClientModels->AccessModel(MODEL_SKILL_FURY_STRIKE + 6, "Data\\Skill\\", "EarthQuake", 6);
	gmClientModels->AccessModel(MODEL_PIERCING, "Data\\Skill\\", "Piercing");

	gmClientModels->AccessModel(MODEL_WAVE_FORCE, "Data\\Skill\\", "WaveForce");
	gmClientModels->AccessModel(MODEL_BLIZZARD, "Data\\Skill\\", "Blizzard");

	gmClientModels->AccessModel(MODEL_ARROW_AUTOLOAD, "Data\\Skill\\", "arrowsrefill");
	gmClientModels->OpenTexture(MODEL_ARROW_AUTOLOAD, "Effect\\");

	gmClientModels->AccessModel(MODEL_INFINITY_ARROW, "Data\\Skill\\", "arrowsre", 1);
	gmClientModels->OpenTexture(MODEL_INFINITY_ARROW, "Skill\\");
	gmClientModels->AccessModel(MODEL_INFINITY_ARROW1, "Data\\Skill\\", "arrowsre", 2);
	gmClientModels->OpenTexture(MODEL_INFINITY_ARROW1, "Skill\\");
	gmClientModels->AccessModel(MODEL_INFINITY_ARROW2, "Data\\Skill\\", "arrowsre", 3);
	gmClientModels->OpenTexture(MODEL_INFINITY_ARROW2, "Skill\\");
	gmClientModels->AccessModel(MODEL_INFINITY_ARROW3, "Data\\Skill\\", "arrowsre", 4);
	gmClientModels->OpenTexture(MODEL_INFINITY_ARROW3, "Effect\\");
	gmClientModels->AccessModel(MODEL_INFINITY_ARROW4, "Data\\Skill\\", "arrowsre", 5);
	gmClientModels->OpenTexture(MODEL_INFINITY_ARROW4, "Skill\\");

	gmClientModels->AccessModel(MODEL_SHIELD_CRASH, "Data\\Effect\\", "atshild");
	gmClientModels->OpenTexture(MODEL_SHIELD_CRASH, "Effect\\");

	gmClientModels->AccessModel(MODEL_SHIELD_CRASH2, "Data\\Effect\\", "atshild2");
	gmClientModels->OpenTexture(MODEL_SHIELD_CRASH2, "Effect\\");

	gmClientModels->AccessModel(MODEL_IRON_RIDER_ARROW, "Data\\Effect\\", "ironobj");
	gmClientModels->OpenTexture(MODEL_IRON_RIDER_ARROW, "Effect\\");

	gmClientModels->AccessModel(MODEL_KENTAUROS_ARROW, "Data\\Effect\\", "cantasarrow");
	gmClientModels->OpenTexture(MODEL_KENTAUROS_ARROW, "Effect\\");

	gmClientModels->AccessModel(MODEL_BLADE_SKILL, "Data\\Effect\\", "bladetonedo");
	gmClientModels->OpenTexture(MODEL_BLADE_SKILL, "Effect\\");

	gmClientModels->AccessModel(MODEL_CHANGE_UP_EFF, "Data\\Effect\\", "Change_Up_Eff");
	gmClientModels->OpenTexture(MODEL_CHANGE_UP_EFF, "Effect\\");
	gmClientModels->GetModel(MODEL_CHANGE_UP_EFF)->Actions[0].PlaySpeed = 0.005f;
	gmClientModels->AccessModel(MODEL_CHANGE_UP_NASA, "Data\\Effect\\", "changup_nasa");

	gmClientModels->OpenTexture(MODEL_CHANGE_UP_NASA, "Effect\\");
	gmClientModels->AccessModel(MODEL_CHANGE_UP_CYLINDER, "Data\\Effect\\", "clinderlight");
	gmClientModels->OpenTexture(MODEL_CHANGE_UP_CYLINDER, "Effect\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_HOLYITEM, "Data\\Skill\\", "eventsungmul");
	gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_HOLYITEM, "Skill\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_PRODECTION_SKILL, "Data\\Skill\\", "eventshild");
	gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_PRODECTION_SKILL, "Skill\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_RESTRAINT_SKILL, "Data\\Skill\\", "eventroofe");
	gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_RESTRAINT_SKILL, "Skill\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_STATUE_PART1, "Data\\Npc\\", "songck1");
	gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_STATUE_PART1, "Npc\\");

	gmClientModels->AccessModel(MODEL_CURSEDTEMPLE_STATUE_PART2, "Data\\Npc\\", "songck2");
	gmClientModels->OpenTexture(MODEL_CURSEDTEMPLE_STATUE_PART2, "Npc\\");

	gmClientModels->AccessModel(MODEL_FENRIR_THUNDER, "Data\\Effect\\", "lightning_type01");
	gmClientModels->OpenTexture(MODEL_FENRIR_THUNDER, "Effect\\");

	for (int i = MODEL_SKILL_BEGIN; i < MODEL_SKILL_END; i++)
	{
		if (i == MODEL_PIERCING)
		{
			gmClientModels->OpenTexture(i, "Skill\\");
		}
		else
			gmClientModels->OpenTexture(i, "Skill\\");
	}

	LoadBitmap("Skill\\flower1.tga", BITMAP_FLOWER01);
	LoadBitmap("Skill\\flower2.tga", BITMAP_FLOWER01 + 1);
	LoadBitmap("Skill\\flower3.tga", BITMAP_FLOWER01 + 2);

	gmClientModels->AccessModel(MODEL_MOONHARVEST_GAM, "Data\\Effect\\", "chusukgam");
	gmClientModels->OpenTexture(MODEL_MOONHARVEST_GAM, "Effect\\");
	gmClientModels->AccessModel(MODEL_MOONHARVEST_SONGPUEN1, "Data\\Effect\\", "chusukseung1");
	gmClientModels->OpenTexture(MODEL_MOONHARVEST_SONGPUEN1, "Effect\\");
	gmClientModels->AccessModel(MODEL_MOONHARVEST_SONGPUEN2, "Data\\Effect\\", "chusukseung2");
	gmClientModels->OpenTexture(MODEL_MOONHARVEST_SONGPUEN2, "Effect\\");
	gmClientModels->AccessModel(MODEL_MOONHARVEST_MOON, "Data\\Effect\\", "chysukmoon");
	gmClientModels->OpenTexture(MODEL_MOONHARVEST_MOON, "Effect\\");

	gmClientModels->AccessModel(MODEL_ALICE_BUFFSKILL_EFFECT, "Data\\Effect\\", "elshildring");
	gmClientModels->OpenTexture(MODEL_ALICE_BUFFSKILL_EFFECT, "Effect\\");
	gmClientModels->AccessModel(MODEL_ALICE_BUFFSKILL_EFFECT2, "Data\\Effect\\", "elshildring2");
	gmClientModels->OpenTexture(MODEL_ALICE_BUFFSKILL_EFFECT2, "Effect\\");

	gmClientModels->AccessModel(MODEL_SUMMONER_WRISTRING_EFFECT, "Data\\Effect\\", "ringtyperout");
	gmClientModels->OpenTexture(MODEL_SUMMONER_WRISTRING_EFFECT, "Effect\\");

	gmClientModels->AccessModel(MODEL_SUMMONER_EQUIP_HEAD_SAHAMUTT, "Data\\Skill\\", "sahatail");
	gmClientModels->OpenTexture(MODEL_SUMMONER_EQUIP_HEAD_SAHAMUTT, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_EQUIP_HEAD_NEIL, "Data\\Skill\\", "nillsohwanz");
	gmClientModels->OpenTexture(MODEL_SUMMONER_EQUIP_HEAD_NEIL, "Skill\\");

	gmClientModels->AccessModel(MODEL_SUMMONER_EQUIP_HEAD_LAGUL, "Data\\Skill\\", "lagul_head");
	gmClientModels->OpenTexture(MODEL_SUMMONER_EQUIP_HEAD_LAGUL, "Skill\\");

	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT1, "Data\\Effect\\", "Suhwanzin1");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT1, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT11, "Data\\Effect\\", "Suhwanzin11");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT11, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT111, "Data\\Effect\\", "Suhwanzin111");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT111, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT2, "Data\\Effect\\", "Suhwanzin2");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT2, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT22, "Data\\Effect\\", "Suhwanzin22");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT22, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT222, "Data\\Effect\\", "Suhwanzin222");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT222, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_CASTING_EFFECT4, "Data\\Effect\\", "Suhwanzin4");
	gmClientModels->OpenTexture(MODEL_SUMMONER_CASTING_EFFECT4, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_SAHAMUTT, "Data\\Skill\\", "summon_sahamutt");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_SAHAMUTT, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL, "Data\\Skill\\", "summon_neil");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL, "Skill\\");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL, "Effect\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_LAGUL, "Data\\Skill\\", "summon_lagul");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_LAGUL, "Skill\\");

	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_NIFE1, "Data\\Skill\\", "nelleff_nife01");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_NIFE1, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_NIFE2, "Data\\Skill\\", "nelleff_nife02");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_NIFE2, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_NIFE3, "Data\\Skill\\", "nelleff_nife03");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_NIFE3, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_GROUND1, "Data\\Skill\\", "nell_nifegrund01");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_GROUND1, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_GROUND2, "Data\\Skill\\", "nell_nifegrund02");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_GROUND2, "Skill\\");
	gmClientModels->AccessModel(MODEL_SUMMONER_SUMMON_NEIL_GROUND3, "Data\\Skill\\", "nell_nifegrund03");
	gmClientModels->OpenTexture(MODEL_SUMMONER_SUMMON_NEIL_GROUND3, "Skill\\");
	gmClientModels->AccessModel(MODEL_MOVE_TARGETPOSITION_EFFECT, "Data\\Effect\\", "MoveTargetPosEffect");
	gmClientModels->OpenTexture(MODEL_MOVE_TARGETPOSITION_EFFECT, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_SAPITRES_ATTACK_1, "Data\\Effect\\", "Sapiatttres");
	gmClientModels->OpenTexture(MODEL_EFFECT_SAPITRES_ATTACK_1, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_SAPITRES_ATTACK_2, "Data\\Effect\\", "Sapiatttres2");
	gmClientModels->OpenTexture(MODEL_EFFECT_SAPITRES_ATTACK_2, "Effect\\");

	gmClientModels->AccessModel(MODEL_RAKLION_BOSS_CRACKEFFECT, "Data\\Effect\\", "knight_plancrack_grand");
	gmClientModels->OpenTexture(MODEL_RAKLION_BOSS_CRACKEFFECT, "Effect\\");
	gmClientModels->AccessModel(MODEL_RAKLION_BOSS_MAGIC, "Data\\Effect\\", "serufan_magic");
	gmClientModels->OpenTexture(MODEL_RAKLION_BOSS_MAGIC, "Effect\\");
	//gmClientModels->GetModel(MODEL_RAKLION_BOSS_MAGIC)->Actions[0].PlaySpeed = 0.005f;

	gmClientModels->AccessModel(MODEL_EFFECT_SKURA_ITEM, "Data\\Effect\\cherryblossom\\", "Skura_iteam_event");

	gmClientModels->AccessModel(MODEL_EFFECT_BROKEN_ICE0, "Data\\Effect\\", "ice_stone00");
	gmClientModels->OpenTexture(MODEL_EFFECT_BROKEN_ICE0, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_BROKEN_ICE1, "Data\\Effect\\", "ice_stone01");
	gmClientModels->OpenTexture(MODEL_EFFECT_BROKEN_ICE1, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_BROKEN_ICE2, "Data\\Effect\\", "ice_stone02");
	gmClientModels->OpenTexture(MODEL_EFFECT_BROKEN_ICE2, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_BROKEN_ICE3, "Data\\Effect\\", "ice_stone03");
	gmClientModels->OpenTexture(MODEL_EFFECT_BROKEN_ICE3, "Effect\\");
	gmClientModels->AccessModel(MODEL_NIGHTWATER_01, "Data\\Effect\\", "nightwater01");
	gmClientModels->OpenTexture(MODEL_NIGHTWATER_01, "Effect\\");
	gmClientModels->AccessModel(MODEL_KNIGHT_PLANCRACK_A, "Data\\Effect\\", "knight_plancrack_a");
	gmClientModels->OpenTexture(MODEL_KNIGHT_PLANCRACK_A, "Effect\\");
	gmClientModels->GetModel(MODEL_KNIGHT_PLANCRACK_A)->Actions[0].PlaySpeed = 0.3f;
	gmClientModels->AccessModel(MODEL_KNIGHT_PLANCRACK_B, "Data\\Effect\\", "knight_plancrack_b");
	gmClientModels->OpenTexture(MODEL_KNIGHT_PLANCRACK_B, "Effect\\");
	gmClientModels->GetModel(MODEL_KNIGHT_PLANCRACK_B)->Actions[0].PlaySpeed = 0.3f;
	gmClientModels->AccessModel(MODEL_EFFECT_FLAME_STRIKE, "Data\\Effect\\", "FlameStrike");
	gmClientModels->OpenTexture(MODEL_EFFECT_FLAME_STRIKE, "Effect\\");
	gmClientModels->AccessModel(MODEL_SWELL_OF_MAGICPOWER, "Data\\Effect\\", "magic_powerup");
	gmClientModels->OpenTexture(MODEL_SWELL_OF_MAGICPOWER, "Effect\\");
	gmClientModels->AccessModel(MODEL_ARROWSRE06, "Data\\Effect\\", "arrowsre06");
	gmClientModels->OpenTexture(MODEL_ARROWSRE06, "Effect\\");
	gmClientModels->AccessModel(MODEL_DOPPELGANGER_SLIME_CHIP, "Data\\Effect\\", "slime_chip");
	gmClientModels->OpenTexture(MODEL_DOPPELGANGER_SLIME_CHIP, "Effect\\");
	gmClientModels->AccessModel(MODEL_EFFECT_UMBRELLA_GOLD, "Data\\Effect\\", "japan_gold01");
	gmClientModels->OpenTexture(MODEL_EFFECT_UMBRELLA_GOLD, "Effect\\");
	gmClientModels->AccessModel(MODEL_EMPIREGUARDIANBOSS_FRAMESTRIKE, "Data\\Effect\\", "Karanebos_sword_framestrike");
	gmClientModels->OpenTexture(MODEL_EMPIREGUARDIANBOSS_FRAMESTRIKE, "Effect\\");
	//gmClientModels->GetModel(MODEL_EMPIREGUARDIANBOSS_FRAMESTRIKE)->Actions[MONSTER01_STOP1].PlaySpeed = 3.0f;
	gmClientModels->AccessModel(MODEL_DEASULER, "Data\\Monster\\", "deasther_boomerang");
	gmClientModels->OpenTexture(MODEL_DEASULER, "Monster\\");
	gmClientModels->AccessModel(MODEL_EFFECT_SD_AURA, "Data\\Effect\\", "shield_up");
	gmClientModels->OpenTexture(MODEL_EFFECT_SD_AURA, "Effect\\");

#ifdef PBG_ADD_NEWCHAR_MONK_SKILL
	gmClientModels->AccessModel(MODEL_WOLF_HEAD_EFFECT, "Data\\Effect\\", "wolf_head_effect");
	gmClientModels->OpenTexture(MODEL_WOLF_HEAD_EFFECT, "Effect\\");
	LoadBitmap("Effect\\sbumb.jpg", BITMAP_SBUMB, GL_LINEAR, GL_REPEAT);
	gmClientModels->AccessModel(MODEL_DOWN_ATTACK_DUMMY_L, "Data\\Effect\\", "down_right_punch");
	gmClientModels->OpenTexture(MODEL_DOWN_ATTACK_DUMMY_L, "Effect\\");
	gmClientModels->AccessModel(MODEL_DOWN_ATTACK_DUMMY_R, "Data\\Effect\\", "down_left_punch");
	gmClientModels->OpenTexture(MODEL_DOWN_ATTACK_DUMMY_R, "Effect\\");
	gmClientModels->AccessModel(MODEL_SHOCKWAVE01, "Data\\Effect\\", "shockwave01");
	gmClientModels->OpenTexture(MODEL_SHOCKWAVE01, "Effect\\");
	gmClientModels->AccessModel(MODEL_SHOCKWAVE02, "Data\\Effect\\", "shockwave02");
	gmClientModels->OpenTexture(MODEL_SHOCKWAVE02, "Effect\\");
	gmClientModels->AccessModel(MODEL_SHOCKWAVE_SPIN01, "Data\\Effect\\", "shockwave_spin01");
	gmClientModels->OpenTexture(MODEL_SHOCKWAVE_SPIN01, "Effect\\");
	gmClientModels->AccessModel(MODEL_WINDFOCE, "Data\\Effect\\", "wind_foce");
	gmClientModels->OpenTexture(MODEL_WINDFOCE, "Effect\\");
	gmClientModels->AccessModel(MODEL_WINDFOCE_MIRROR, "Data\\Effect\\", "wind_foce_mirror");
	gmClientModels->OpenTexture(MODEL_WINDFOCE_MIRROR, "Effect\\");
	gmClientModels->AccessModel(MODEL_WOLF_HEAD_EFFECT2, "Data\\Effect\\", "wolf_head_effect2");
	gmClientModels->OpenTexture(MODEL_WOLF_HEAD_EFFECT2, "skill\\");
	gmClientModels->AccessModel(MODEL_SHOCKWAVE_GROUND01, "Data\\Effect\\", "shockwave_ground01");
	gmClientModels->OpenTexture(MODEL_SHOCKWAVE_GROUND01, "Effect\\");
	gmClientModels->AccessModel(MODEL_DRAGON_KICK_DUMMY, "Data\\Effect\\", "dragon_kick_dummy");
	gmClientModels->OpenTexture(MODEL_DRAGON_KICK_DUMMY, "Effect\\");
	gmClientModels->AccessModel(MODEL_DRAGON_LOWER_DUMMY, "Data\\Effect\\", "knight_plancrack_dragon");
	gmClientModels->OpenTexture(MODEL_DRAGON_LOWER_DUMMY, "Effect\\");
	gmClientModels->AccessModel(MODEL_VOLCANO_OF_MONK, "Data\\Effect\\", "volcano_of_monk");
	gmClientModels->OpenTexture(MODEL_VOLCANO_OF_MONK, "Effect\\");
	gmClientModels->AccessModel(MODEL_VOLCANO_STONE, "Data\\Effect\\", "volcano_stone");
	gmClientModels->OpenTexture(MODEL_VOLCANO_STONE, "Effect\\");
	LoadBitmap("Effect\\force_Pillar.jpg", BITMAP_FORCEPILLAR, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\!SwordEff.jpg", BITMAP_SWORDEFF, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\Damage1.jpg", BITMAP_DAMAGE1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\ground_wind.jpg", BITMAP_GROUND_WIND, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Kwave2.jpg", BITMAP_KWAVE2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Damage2.jpg", BITMAP_DAMAGE2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\volcano_core.jpg", BITMAP_VOLCANO_CORE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	gmClientModels->AccessModel(MODEL_SHOCKWAVE03, "Data\\Effect\\", "shockwave03");
	gmClientModels->OpenTexture(MODEL_SHOCKWAVE03, "Effect\\");
	LoadBitmap("Effect\\ground_smoke.tga", BITMAP_GROUND_SMOKE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\knightSt_blue.jpg", BITMAP_KNIGHTST_BLUE, GL_LINEAR, GL_CLAMP_TO_EDGE);

	gmClientModels->AccessModel(MODEL_PHOENIX_SHOT_EFFECT, "Data\\Effect\\", "phoenix_shot_effect");
	gmClientModels->OpenTexture(MODEL_PHOENIX_SHOT_EFFECT, "Effect\\");

	gmClientModels->AccessModel(MODEL_WIND_SPIN01, "Data\\Effect\\", "wind_spin01");
	gmClientModels->OpenTexture(MODEL_WIND_SPIN01, "Effect\\");

	gmClientModels->AccessModel(MODEL_WIND_SPIN02, "Data\\Effect\\", "wind_spin02");
	gmClientModels->OpenTexture(MODEL_WIND_SPIN02, "Effect\\");

	gmClientModels->AccessModel(MODEL_WIND_SPIN03, "Data\\Effect\\", "wind_spin03");
	gmClientModels->OpenTexture(MODEL_WIND_SPIN03, "Effect\\");

	char strText[100];

	for (int jj = 0; jj < 6; ++jj)
	{
		sprintf(strText, "Effect\\xsuper000%d.jpg", jj + 1);
		LoadBitmap(strText, jj + BITMAP_SUPER0001, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	}

	LoadBitmap("Effect\\SwordEffor2_blur.jpg", BITMAP_SWORDEFFOR2_BLUR, GL_LINEAR, GL_CLAMP_TO_EDGE);

	gmClientModels->AccessModel(MODEL_SWORD_35_WING, "Data\\Item\\", "sword36wing");
	gmClientModels->OpenTexture(MODEL_SWORD_35_WING, "Item\\Texture\\");
#endif //PBG_ADD_NEWCHAR_MONK_SKILL

	gmClientModels->AccessModel(MODEL_CONDRA_STONE, "Data\\Monster\\", "condra_7_stone");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE, "Monster\\");
	gmClientModels->AccessModel(MODEL_CONDRA_STONE1, "Data\\Monster\\", "condra_7_stone_2");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE1, "Monster\\");
	gmClientModels->AccessModel(MODEL_CONDRA_STONE2, "Data\\Monster\\", "condra_7_stone_3");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE2, "Monster\\");
	gmClientModels->AccessModel(MODEL_CONDRA_STONE3, "Data\\Monster\\", "condra_7_stone_4");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE3, "Monster\\");
	gmClientModels->AccessModel(MODEL_CONDRA_STONE4, "Data\\Monster\\", "condra_7_stone_5");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE4, "Monster\\");
	gmClientModels->AccessModel(MODEL_CONDRA_STONE5, "Data\\Monster\\", "condra_7_stone_6");
	gmClientModels->OpenTexture(MODEL_CONDRA_STONE5, "Monster\\");

	gmClientModels->AccessModel(MODEL_NARCONDRA_STONE, "Data\\Monster\\", "nar_condra_7_stone_1");
	gmClientModels->OpenTexture(MODEL_NARCONDRA_STONE, "Monster\\");
	gmClientModels->AccessModel(MODEL_NARCONDRA_STONE1, "Data\\Monster\\", "nar_condra_7_stone_2");
	gmClientModels->OpenTexture(MODEL_NARCONDRA_STONE1, "Monster\\");
	gmClientModels->AccessModel(MODEL_NARCONDRA_STONE2, "Data\\Monster\\", "nar_condra_7_stone_3");
	gmClientModels->OpenTexture(MODEL_NARCONDRA_STONE2, "Monster\\");
	gmClientModels->AccessModel(MODEL_NARCONDRA_STONE3, "Data\\Monster\\", "nar_condra_7_stone_4");
	gmClientModels->OpenTexture(MODEL_NARCONDRA_STONE3, "Monster\\");

	LoadBitmap("Monster\\bossmap1_R.jpg", BITMAP_HGBOSS_PATTERN, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\bosswing.tga", BITMAP_HGBOSS_WING, GL_NEAREST, GL_REPEAT);

	LoadBitmap("Skill\\bossrock1_R.JPG", BITMAP_FISSURE_FIRE, GL_LINEAR, GL_CLAMP_TO_EDGE);

	GMMonsterMng->LoadModelMonster();
}

void SaveWorld(int iWorld)
{
	char WorldName[32];
	char FileName[64];
	sprintf(WorldName, "World%d", iWorld);

	sprintf(FileName, "Data2\\%s\\TerrainLight.jpg", WorldName);
	SaveTerrainLight(FileName);
	sprintf(FileName, "Data2\\%s\\TerrainHeight.bmp", WorldName);
	SaveTerrainHeight(FileName);
	sprintf(FileName, "Data\\%s\\Terrain.map", WorldName);
	SaveTerrainMapping(FileName, iWorld);
	sprintf(FileName, "Data\\%s\\Terrain.att", WorldName);
	SaveTerrainAttribute(FileName, iWorld);
	sprintf(FileName, "Data\\%s\\Terrain.obj", WorldName);
	SaveObjects(FileName, iWorld);
}

void OpenImages()
{
	LoadBitmap("Interface\\command.jpg", BITMAP_SKILL_INTERFACE + 2);

	LoadBitmap("Interface\\Item_Back01.jpg", BITMAP_INVENTORY);
	LoadBitmap("Interface\\Item_Back02.jpg", BITMAP_INVENTORY + 1);
	LoadBitmap("Interface\\Item_Money.jpg", BITMAP_INVENTORY + 11);

#if MAIN_UPDATE > 303
	if (gmProtect->LookAndFeel == 1)
	{
		LoadBitmap("Interface\\HUD\\Look-1\\Item_cap.jpg", BITMAP_INVENTORY + 3);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_upper.jpg", BITMAP_INVENTORY + 4);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_lower.jpg", BITMAP_INVENTORY + 5);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_weapon01.jpg", BITMAP_INVENTORY + 6);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_gloves.jpg", BITMAP_INVENTORY + 7);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_boots.jpg", BITMAP_INVENTORY + 8);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_necklace.jpg", BITMAP_INVENTORY + 9);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_ring.jpg", BITMAP_INVENTORY + 10);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_wing.jpg", BITMAP_INVENTORY + 14);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_fairy.jpg", BITMAP_INVENTORY + 15);
		LoadBitmap("Interface\\HUD\\Look-1\\Item_weapon02.jpg", BITMAP_INVENTORY + 16);
		LoadBitmap("Interface\\HUD\\Look-1\\btn_pshop.jpg", BITMAP_INVENTORY_BUTTON + 2);
		LoadBitmap("Interface\\HUD\\Look-1\\btn_pshop2.jpg", BITMAP_INVENTORY_BUTTON + 3);
		LoadBitmap("Interface\\HUD\\Look-1\\repair_01.jpg", BITMAP_INVENTORY_BUTTON + 4);
		LoadBitmap("Interface\\HUD\\Look-1\\repair_02.jpg", BITMAP_INVENTORY_BUTTON + 5);
		LoadBitmap("Interface\\HUD\\Look-1\\suho_button1.jpg", BITMAP_INVENTORY_BUTTON + 6);
		LoadBitmap("Interface\\HUD\\Look-1\\suho_button2.jpg", BITMAP_INVENTORY_BUTTON + 7);
		LoadBitmap("Interface\\HUD\\Look-1\\quest1.jpg", BITMAP_INVENTORY_BUTTON + 8);
		LoadBitmap("Interface\\HUD\\Look-1\\quest2.jpg", BITMAP_INVENTORY_BUTTON + 9);
		LoadBitmap("Interface\\HUD\\Look-1\\Pet1.jpg", BITMAP_INVENTORY_BUTTON + 18);
		LoadBitmap("Interface\\HUD\\Look-1\\Pet2.jpg", BITMAP_INVENTORY_BUTTON + 19);
		LoadBitmap("Interface\\HUD\\Look-1\\Money_01.jpg", BITMAP_SKILL_INTERFACE);
		LoadBitmap("Interface\\HUD\\Look-1\\Money_02.jpg", BITMAP_SKILL_INTERFACE + 1);
		LoadBitmap("Interface\\HUD\\Look-1\\expansion_button1.jpg", BITMAP_INTERFACE);
		LoadBitmap("Interface\\HUD\\Look-1\\expansion_button2.jpg", BITMAP_INTERFACE + 1);
		LoadBitmap("Interface\\HUD\\Look-1\\level_01.jpg", BITMAP_INTERFACE + 2);
		LoadBitmap("Interface\\HUD\\Look-1\\level_02.jpg", BITMAP_INTERFACE + 3);
		LoadBitmap("Interface\\HUD\\Look-1\\Level_box.jpg", BITMAP_INTERFACE + 4);
		LoadBitmap("Interface\\HUD\\Look-1\\buy_01.jpg", BITMAP_INTERFACE + 5);
		LoadBitmap("Interface\\HUD\\Look-1\\buy_02.jpg", BITMAP_INTERFACE + 6);
		LoadBitmap("Interface\\HUD\\Look-1\\sell_01.jpg", BITMAP_INTERFACE + 7);
		LoadBitmap("Interface\\HUD\\Look-1\\sell_02.jpg", BITMAP_INTERFACE + 8);
		LoadBitmap("Interface\\HUD\\Look-1\\Message_box.jpg", Bitmap_Message_box);
		LoadBitmap("Interface\\HUD\\Look-1\\Message_box1.jpg", Bitmap_Message_box1);
		LoadBitmap("Interface\\HUD\\Look-1\\Message_box2.jpg", Bitmap_Message_box2);
		LoadBitmap("Interface\\HUD\\Look-1\\Message_box3.jpg", Bitmap_Message_box3);
		LoadBitmap("Interface\\HUD\\Look-1\\win_button2.tga", Bitmap_win_button2);

		LoadBitmap("Interface\\HUD\\Look-1\\Message_box1.jpg", BITMAP_INTERFACE + 21);
		LoadBitmap("Interface\\HUD\\Look-1\\Message_box2.jpg", BITMAP_INTERFACE + 22);
		

		LoadBitmap("Interface\\HUD\\Look-1\\nis_back.jpg", Bitmap_Nis_back);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_bar.jpg", Bitmap_Nis_bar);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnalpha.jpg", Bitmap_Nis_btnalpha);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnarrow.jpg", Bitmap_Nis_btnarrow);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnchat.jpg", Bitmap_Nis_btnchat);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnend.jpg", Bitmap_Nis_btnend);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnmax.jpg", Bitmap_Nis_btnmax);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnmin.jpg", Bitmap_Nis_btnmin);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_btnsize.jpg", Bitmap_Nis_btnsize);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_hframe.jpg", Bitmap_Nis_hframe);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_rsframe.jpg", Bitmap_Nis_rsframe);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_rsframe.tga", Bitmap_Nis_rsframe_t);
		LoadBitmap("Interface\\HUD\\Look-1\\nis_vframe.jpg", Bitmap_vframe);
		LoadBitmap("Interface\\HUD\\Look-1\\text_button1.jpg", Bitmap_text_button1);
		LoadBitmap("Interface\\HUD\\Look-1\\text_button2.jpg", Bitmap_text_button2);
		LoadBitmap("Interface\\HUD\\Look-1\\dialogue1.jpg", Bitmap_dialogue1);
		LoadBitmap("Interface\\HUD\\Look-1\\dialogue2.jpg", Bitmap_dialogue2);
		LoadBitmap("Interface\\HUD\\Look-1\\dialogue2_1.jpg", Bitmap_dialogue2_1);
	}
	else if (gmProtect->LookAndFeel == 2)
	{
		LoadBitmap("Interface\\HUD\\Look-2\\Item_cap.jpg", BITMAP_INVENTORY + 3);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_upper.jpg", BITMAP_INVENTORY + 4);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_lower.jpg", BITMAP_INVENTORY + 5);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_weapon01.jpg", BITMAP_INVENTORY + 6);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_gloves.jpg", BITMAP_INVENTORY + 7);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_boots.jpg", BITMAP_INVENTORY + 8);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_necklace.jpg", BITMAP_INVENTORY + 9);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_ring.jpg", BITMAP_INVENTORY + 10);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_wing.jpg", BITMAP_INVENTORY + 14);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_fairy.jpg", BITMAP_INVENTORY + 15);
		LoadBitmap("Interface\\HUD\\Look-2\\Item_weapon02.jpg", BITMAP_INVENTORY + 16);
		LoadBitmap("Interface\\HUD\\Look-2\\btn_pshop.jpg", BITMAP_INVENTORY_BUTTON + 2);
		LoadBitmap("Interface\\HUD\\Look-2\\btn_pshop2.jpg", BITMAP_INVENTORY_BUTTON + 3);
		LoadBitmap("Interface\\HUD\\Look-2\\repair_01.jpg", BITMAP_INVENTORY_BUTTON + 4);
		LoadBitmap("Interface\\HUD\\Look-2\\repair_02.jpg", BITMAP_INVENTORY_BUTTON + 5);
		LoadBitmap("Interface\\HUD\\Look-2\\suho_button1.jpg", BITMAP_INVENTORY_BUTTON + 6);
		LoadBitmap("Interface\\HUD\\Look-2\\suho_button2.jpg", BITMAP_INVENTORY_BUTTON + 7);
		LoadBitmap("Interface\\HUD\\Look-2\\quest1.jpg", BITMAP_INVENTORY_BUTTON + 8);
		LoadBitmap("Interface\\HUD\\Look-2\\quest2.jpg", BITMAP_INVENTORY_BUTTON + 9);
		LoadBitmap("Interface\\HUD\\Look-2\\Pet1.jpg", BITMAP_INVENTORY_BUTTON + 18);
		LoadBitmap("Interface\\HUD\\Look-2\\Pet2.jpg", BITMAP_INVENTORY_BUTTON + 19);
		LoadBitmap("Interface\\HUD\\Look-2\\Money_01.jpg", BITMAP_SKILL_INTERFACE);
		LoadBitmap("Interface\\HUD\\Look-2\\Money_02.jpg", BITMAP_SKILL_INTERFACE + 1);
		LoadBitmap("Interface\\HUD\\Look-2\\expansion_button1.jpg", BITMAP_INTERFACE);
		LoadBitmap("Interface\\HUD\\Look-2\\expansion_button2.jpg", BITMAP_INTERFACE + 1);
		LoadBitmap("Interface\\HUD\\Look-2\\level_01.jpg", BITMAP_INTERFACE + 2);
		LoadBitmap("Interface\\HUD\\Look-2\\level_02.jpg", BITMAP_INTERFACE + 3);
		LoadBitmap("Interface\\HUD\\Look-2\\Level_box.jpg", BITMAP_INTERFACE + 4);
		LoadBitmap("Interface\\HUD\\Look-2\\buy_01.jpg", BITMAP_INTERFACE + 5);
		LoadBitmap("Interface\\HUD\\Look-2\\buy_02.jpg", BITMAP_INTERFACE + 6);
		LoadBitmap("Interface\\HUD\\Look-2\\sell_01.jpg", BITMAP_INTERFACE + 7);
		LoadBitmap("Interface\\HUD\\Look-2\\sell_02.jpg", BITMAP_INTERFACE + 8);
		LoadBitmap("Interface\\HUD\\Look-2\\Message_box.jpg", Bitmap_Message_box);
		LoadBitmap("Interface\\HUD\\Look-2\\Message_box1.jpg", Bitmap_Message_box1);
		LoadBitmap("Interface\\HUD\\Look-2\\Message_box2.jpg", Bitmap_Message_box2);
		LoadBitmap("Interface\\HUD\\Look-2\\Message_box3.jpg", Bitmap_Message_box3);
		LoadBitmap("Interface\\HUD\\Look-2\\win_button2.tga", Bitmap_win_button2);

		LoadBitmap("Interface\\HUD\\Look-2\\Message_box1.jpg", BITMAP_INTERFACE + 21);
		LoadBitmap("Interface\\HUD\\Look-2\\Message_box2.jpg", BITMAP_INTERFACE + 22);

		LoadBitmap("Interface\\HUD\\Look-2\\nis_back.jpg", Bitmap_Nis_back);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_bar.jpg", Bitmap_Nis_bar);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnalpha.jpg", Bitmap_Nis_btnalpha);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnarrow.jpg", Bitmap_Nis_btnarrow);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnchat.jpg", Bitmap_Nis_btnchat);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnend.jpg", Bitmap_Nis_btnend);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnmax.jpg", Bitmap_Nis_btnmax);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnmin.jpg", Bitmap_Nis_btnmin);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_btnsize.jpg", Bitmap_Nis_btnsize);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_hframe.jpg", Bitmap_Nis_hframe);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_rsframe.jpg", Bitmap_Nis_rsframe);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_rsframe.tga", Bitmap_Nis_rsframe_t);
		LoadBitmap("Interface\\HUD\\Look-2\\nis_vframe.jpg", Bitmap_vframe);
		LoadBitmap("Interface\\HUD\\Look-2\\text_button1.jpg", Bitmap_text_button1);
		LoadBitmap("Interface\\HUD\\Look-2\\text_button2.jpg", Bitmap_text_button2);
		LoadBitmap("Interface\\HUD\\Look-2\\dialogue1.jpg", Bitmap_dialogue1);
		LoadBitmap("Interface\\HUD\\Look-2\\dialogue2.jpg", Bitmap_dialogue2);
		LoadBitmap("Interface\\HUD\\Look-2\\dialogue2_1.jpg", Bitmap_dialogue2_1);
	}
#else
	LoadBitmap("Interface\\HUD\\Look-2\\Item_cap.jpg", BITMAP_INVENTORY + 3);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_upper.jpg", BITMAP_INVENTORY + 4);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_lower.jpg", BITMAP_INVENTORY + 5);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_weapon01.jpg", BITMAP_INVENTORY + 6);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_gloves.jpg", BITMAP_INVENTORY + 7);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_boots.jpg", BITMAP_INVENTORY + 8);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_necklace.jpg", BITMAP_INVENTORY + 9);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_ring.jpg", BITMAP_INVENTORY + 10);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_wing.jpg", BITMAP_INVENTORY + 14);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_fairy.jpg", BITMAP_INVENTORY + 15);
	LoadBitmap("Interface\\HUD\\Look-2\\Item_weapon02.jpg", BITMAP_INVENTORY + 16);
	LoadBitmap("Interface\\HUD\\Look-2\\btn_pshop.jpg", BITMAP_INVENTORY_BUTTON + 2);
	LoadBitmap("Interface\\HUD\\Look-2\\btn_pshop2.jpg", BITMAP_INVENTORY_BUTTON + 3);
	LoadBitmap("Interface\\HUD\\Look-2\\repair_01.jpg", BITMAP_INVENTORY_BUTTON + 4);
	LoadBitmap("Interface\\HUD\\Look-2\\repair_02.jpg", BITMAP_INVENTORY_BUTTON + 5);
	LoadBitmap("Interface\\HUD\\Look-2\\suho_button1.jpg", BITMAP_INVENTORY_BUTTON + 6);
	LoadBitmap("Interface\\HUD\\Look-2\\suho_button2.jpg", BITMAP_INVENTORY_BUTTON + 7);
	LoadBitmap("Interface\\HUD\\Look-2\\quest1.jpg", BITMAP_INVENTORY_BUTTON + 8);
	LoadBitmap("Interface\\HUD\\Look-2\\quest2.jpg", BITMAP_INVENTORY_BUTTON + 9);
	LoadBitmap("Interface\\HUD\\Look-2\\Pet1.jpg", BITMAP_INVENTORY_BUTTON + 18);
	LoadBitmap("Interface\\HUD\\Look-2\\Pet2.jpg", BITMAP_INVENTORY_BUTTON + 19);
	LoadBitmap("Interface\\HUD\\Look-2\\Money_01.jpg", BITMAP_SKILL_INTERFACE);
	LoadBitmap("Interface\\HUD\\Look-2\\Money_02.jpg", BITMAP_SKILL_INTERFACE + 1);
	LoadBitmap("Interface\\HUD\\Look-2\\expansion_button1.jpg", BITMAP_INTERFACE);
	LoadBitmap("Interface\\HUD\\Look-2\\expansion_button2.jpg", BITMAP_INTERFACE + 1);
	LoadBitmap("Interface\\HUD\\Look-2\\level_01.jpg", BITMAP_INTERFACE + 2);
	LoadBitmap("Interface\\HUD\\Look-2\\level_02.jpg", BITMAP_INTERFACE + 3);
	LoadBitmap("Interface\\HUD\\Look-2\\Level_box.jpg", BITMAP_INTERFACE + 4);
	LoadBitmap("Interface\\HUD\\Look-2\\buy_01.jpg", BITMAP_INTERFACE + 5);
	LoadBitmap("Interface\\HUD\\Look-2\\buy_02.jpg", BITMAP_INTERFACE + 6);
	LoadBitmap("Interface\\HUD\\Look-2\\sell_01.jpg", BITMAP_INTERFACE + 7);
	LoadBitmap("Interface\\HUD\\Look-2\\sell_02.jpg", BITMAP_INTERFACE + 8);
	LoadBitmap("Interface\\HUD\\Look-2\\Message_box.jpg", Bitmap_Message_box);
	LoadBitmap("Interface\\HUD\\Look-2\\Message_box1.jpg", Bitmap_Message_box1);
	LoadBitmap("Interface\\HUD\\Look-2\\Message_box2.jpg", Bitmap_Message_box2);
	LoadBitmap("Interface\\HUD\\Look-2\\Message_box3.jpg", Bitmap_Message_box3);
	LoadBitmap("Interface\\HUD\\Look-2\\win_button2.tga", Bitmap_win_button2);
	
	LoadBitmap("Interface\\HUD\\Look-2\\nis_back.jpg", Bitmap_Nis_back);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_bar.jpg", Bitmap_Nis_bar);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnalpha.jpg", Bitmap_Nis_btnalpha);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnarrow.jpg", Bitmap_Nis_btnarrow);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnchat.jpg", Bitmap_Nis_btnchat);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnend.jpg", Bitmap_Nis_btnend);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnmax.jpg", Bitmap_Nis_btnmax);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnmin.jpg", Bitmap_Nis_btnmin);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_btnsize.jpg", Bitmap_Nis_btnsize);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_hframe.jpg", Bitmap_Nis_hframe);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_rsframe.jpg", Bitmap_Nis_rsframe);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_rsframe.tga", Bitmap_Nis_rsframe_t);
	LoadBitmap("Interface\\HUD\\Look-2\\nis_vframe.jpg", Bitmap_vframe);
	LoadBitmap("Interface\\HUD\\Look-2\\text_button1.jpg", Bitmap_text_button1);
	LoadBitmap("Interface\\HUD\\Look-2\\text_button2.jpg", Bitmap_text_button2);
	LoadBitmap("Interface\\HUD\\Look-2\\dialogue1.jpg", Bitmap_dialogue1);
	LoadBitmap("Interface\\HUD\\Look-2\\dialogue2.jpg", Bitmap_dialogue2);
	LoadBitmap("Interface\\HUD\\Look-2\\dialogue2_1.jpg", Bitmap_dialogue2_1);
#endif

	LoadBitmap("Interface\\Item_box.jpg", BITMAP_INVENTORY + 17);
	LoadBitmap("Interface\\InventoryBox2.jpg", BITMAP_INVENTORY + 18);

	LoadBitmap("Interface\\Trading_line.jpg", BITMAP_INVENTORY + 19);

	LoadBitmap("Interface\\exit_01.jpg", BITMAP_INVENTORY_BUTTON);
	LoadBitmap("Interface\\exit_02.jpg", BITMAP_INVENTORY_BUTTON + 1);

	LoadBitmap("Interface\\accept_box01.jpg", BITMAP_INVENTORY_BUTTON + 10);
	LoadBitmap("Interface\\accept_box02.jpg", BITMAP_INVENTORY_BUTTON + 11);

	LoadBitmap("Interface\\mix_button1.jpg", BITMAP_INVENTORY_BUTTON + 12);
	LoadBitmap("Interface\\mix_button2.jpg", BITMAP_INVENTORY_BUTTON + 13);

	LoadBitmap("Interface\\lock_01.jpg", BITMAP_INVENTORY_BUTTON + 14);
	LoadBitmap("Interface\\lock_02.jpg", BITMAP_INVENTORY_BUTTON + 15);
	LoadBitmap("Interface\\lock_03.jpg", BITMAP_INVENTORY_BUTTON + 16);
	LoadBitmap("Interface\\lock_04.jpg", BITMAP_INVENTORY_BUTTON + 17);
	LoadBitmap("Interface\\guild.tga", BITMAP_GUILD);

	//
}

void OpenSounds()
{
	bool Enable3DSound = true;

	LoadWaveFile(SOUND_WIND01, "Data\\Sound\\aWind.wav", 1);
	LoadWaveFile(SOUND_RAIN01, "Data\\Sound\\aRain.wav", 1);
	LoadWaveFile(SOUND_DUNGEON01, "Data\\Sound\\aDungeon.wav", 1);
	LoadWaveFile(SOUND_FOREST01, "Data\\Sound\\aForest.wav", 1);
	LoadWaveFile(SOUND_TOWER01, "Data\\Sound\\aTower.wav", 1);
	LoadWaveFile(SOUND_WATER01, "Data\\Sound\\aWater.wav", 1);
	LoadWaveFile(SOUND_DESERT01, "Data\\Sound\\desert.wav", 1);
	//LoadWaveFile(SOUND_BOSS01		    ,"Data\\Sound\\a쿤둔.wav",1);
	LoadWaveFile(SOUND_HUMAN_WALK_GROUND, "Data\\Sound\\pWalk(Soil).wav", 2);
	LoadWaveFile(SOUND_HUMAN_WALK_GRASS, "Data\\Sound\\pWalk(Grass).wav", 2);
	LoadWaveFile(SOUND_HUMAN_WALK_SNOW, "Data\\Sound\\pWalk(Snow).wav", 2);
	LoadWaveFile(SOUND_HUMAN_WALK_SWIM, "Data\\Sound\\pSwim.wav", 2);

	LoadWaveFile(SOUND_BIRD01, "Data\\Sound\\aBird1.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_BIRD02, "Data\\Sound\\aBird2.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_BAT01, "Data\\Sound\\aBat.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_RAT01, "Data\\Sound\\aMouse.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_TRAP01, "Data\\Sound\\aGrate.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_DOOR01, "Data\\Sound\\aDoor.wav", 1);
	LoadWaveFile(SOUND_DOOR02, "Data\\Sound\\aCastleDoor.wav", 1);

	LoadWaveFile(SOUND_HEAVEN01, "Data\\Sound\\aHeaven.wav", 1);
	LoadWaveFile(SOUND_THUNDERS01, "Data\\Sound\\aThunder01.wav", 1);
	LoadWaveFile(SOUND_THUNDERS02, "Data\\Sound\\aThunder02.wav", 1);
	LoadWaveFile(SOUND_THUNDERS03, "Data\\Sound\\aThunder03.wav", 1);

	//attack
	LoadWaveFile(SOUND_BRANDISH_SWORD01, "Data\\Sound\\eSwingWeapon1.wav", 2);
	LoadWaveFile(SOUND_BRANDISH_SWORD02, "Data\\Sound\\eSwingWeapon2.wav", 2);
	LoadWaveFile(SOUND_BRANDISH_SWORD03, "Data\\Sound\\eSwingLightSword.wav", 2);
	LoadWaveFile(SOUND_BOW01, "Data\\Sound\\eBow.wav", 2);
	LoadWaveFile(SOUND_CROSSBOW01, "Data\\Sound\\eCrossbow.wav", 2);
	LoadWaveFile(SOUND_MIX01, "Data\\Sound\\eMix.wav", 2);

	//player
	LoadWaveFile(SOUND_DRINK01, "Data\\Sound\\pDrink.wav", 1);
	LoadWaveFile(SOUND_EAT_APPLE01, "Data\\Sound\\pEatApple.wav", 1);
	LoadWaveFile(SOUND_HEART, "Data\\Sound\\pHeartBeat.wav", 1);
	LoadWaveFile(SOUND_GET_ENERGY, "Data\\Sound\\pEnergy.wav", 1);
	LoadWaveFile(SOUND_HUMAN_SCREAM01, "Data\\Sound\\pMaleScream1.wav", 2);
	LoadWaveFile(SOUND_HUMAN_SCREAM02, "Data\\Sound\\pMaleScream2.wav", 2);
	LoadWaveFile(SOUND_HUMAN_SCREAM03, "Data\\Sound\\pMaleScream3.wav", 2);
	LoadWaveFile(SOUND_HUMAN_SCREAM04, "Data\\Sound\\pMaleDie.wav", 2);
	LoadWaveFile(SOUND_FEMALE_SCREAM01, "Data\\Sound\\pFemaleScream1.wav", 2);
	LoadWaveFile(SOUND_FEMALE_SCREAM02, "Data\\Sound\\pFemaleScream2.wav", 2);

	LoadWaveFile(SOUND_DROP_ITEM01, "Data\\Sound\\pDropItem.wav", 1);
	LoadWaveFile(SOUND_DROP_GOLD01, "Data\\Sound\\pDropMoney.wav", 1);
	LoadWaveFile(SOUND_JEWEL01, "Data\\Sound\\eGem.wav", 1);
	LoadWaveFile(SOUND_GET_ITEM01, "Data\\Sound\\pGetItem.wav", 1);
	//LoadWaveFile(SOUND_SHOUT01    		,"Data\\Sound\\p기합.wav",1); 

	//skill
	LoadWaveFile(SOUND_SKILL_DEFENSE, "Data\\Sound\\sKnightDefense.wav", 1);
	LoadWaveFile(SOUND_SKILL_SWORD1, "Data\\Sound\\sKnightSkill1.wav", 1);
	LoadWaveFile(SOUND_SKILL_SWORD2, "Data\\Sound\\sKnightSkill2.wav", 1);
	LoadWaveFile(SOUND_SKILL_SWORD3, "Data\\Sound\\sKnightSkill3.wav", 1);
	LoadWaveFile(SOUND_SKILL_SWORD4, "Data\\Sound\\sKnightSkill4.wav", 1);
	LoadWaveFile(SOUND_MONSTER + 116, "Data\\Sound\\mShadowAttack1.wav", 1);

	LoadWaveFile(SOUND_STORM, "Data\\Sound\\sTornado.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_EVIL, "Data\\Sound\\sEvil.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_MAGIC, "Data\\Sound\\sMagic.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_HELLFIRE, "Data\\Sound\\sHellFire.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_ICE, "Data\\Sound\\sIce.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_FLAME, "Data\\Sound\\sFlame.wav", 2, Enable3DSound);
	//LoadWaveFile(SOUND_FLASH            ,"Data\\Sound\\m히드라공격1.wav",2,Enable3DSound);
	LoadWaveFile(SOUND_FLASH, "Data\\Sound\\sAquaFlash.wav", 2, Enable3DSound);

	LoadWaveFile(SOUND_BREAK01, "Data\\Sound\\eBreak.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_EXPLOTION01, "Data\\Sound\\eExplosion.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_METEORITE01, "Data\\Sound\\eMeteorite.wav", 2, Enable3DSound);
	//LoadWaveFile(SOUND_METEORITE02	    ,"Data\\Sound\\e유성.wav",2,Enable3DSound);
	LoadWaveFile(SOUND_THUNDER01, "Data\\Sound\\eThunder.wav", 1, Enable3DSound);

	LoadWaveFile(SOUND_BONE1, "Data\\Sound\\mBone1.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_BONE2, "Data\\Sound\\mBone2.wav", 2, Enable3DSound);
	LoadWaveFile(SOUND_ASSASSIN, "Data\\Sound\\mAssassin1.wav", 1, Enable3DSound);


	LoadWaveFile(SOUND_ATTACK01, "Data\\Sound\\eMeleeHit1.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 1, "Data\\Sound\\eMeleeHit2.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 2, "Data\\Sound\\eMeleeHit3.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 3, "Data\\Sound\\eMeleeHit4.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 4, "Data\\Sound\\eMeleeHit5.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 5, "Data\\Sound\\eMissileHit1.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 6, "Data\\Sound\\eMissileHit2.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 7, "Data\\Sound\\eMissileHit3.wav", 2);
	LoadWaveFile(SOUND_ATTACK01 + 8, "Data\\Sound\\eMissileHit4.wav", 2);

	LoadWaveFile(SOUND_FIRECRACKER1, "Data\\Sound\\eFirecracker1.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_FIRECRACKER2, "Data\\Sound\\eFirecracker2.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_MEDAL, "Data\\Sound\\eMedal.wav", 1, Enable3DSound);
	LoadWaveFile(SOUND_PHOENIXEXP, "Data\\Sound\\ePhoenixExp.wav", 1, Enable3DSound);
	//	LoadWaveFile(SOUND_PHOENIXFIRE		,"Data\\Sound\\ePhoenixFire.wav",1,Enable3DSound);

	LoadWaveFile(SOUND_RIDINGSPEAR, "Data\\Sound\\eRidingSpear.wav", 1);
	LoadWaveFile(SOUND_RAIDSHOOT, "Data\\Sound\\eRaidShoot.wav", 1);
	LoadWaveFile(SOUND_SWELLLIFE, "Data\\Sound\\eSwellLife.wav", 1);
	LoadWaveFile(SOUND_PIERCING, "Data\\Sound\\ePiercing.wav", 1);
	LoadWaveFile(SOUND_ICEARROW, "Data\\Sound\\eIceArrow.wav", 1);
	LoadWaveFile(SOUND_TELEKINESIS, "Data\\Sound\\eTelekinesis.wav", 1);
	LoadWaveFile(SOUND_SOULBARRIER, "Data\\Sound\\eSoulBarrier.wav", 1);
	LoadWaveFile(SOUND_BLOODATTACK, "Data\\Sound\\eBloodAttack.wav", 1);

	LoadWaveFile(SOUND_HIT_GATE, "Data\\Sound\\eHitGate.wav", 1);
	LoadWaveFile(SOUND_HIT_GATE2, "Data\\Sound\\eHitGate2.wav", 1);
	LoadWaveFile(SOUND_HIT_CRISTAL, "Data\\Sound\\eHitCristal.wav", 1);
	LoadWaveFile(SOUND_DOWN_GATE, "Data\\Sound\\eDownGate.wav", 1);
	LoadWaveFile(SOUND_CROW, "Data\\Sound\\eCrow.wav", 1);

	LoadWaveFile(SOUND_DEATH_POISON1, "Data\\Sound\\eBlastPoison_1.wav", 2);
	LoadWaveFile(SOUND_DEATH_POISON2, "Data\\Sound\\eBlastPoison_2.wav", 2);
	LoadWaveFile(SOUND_SUDDEN_ICE1, "Data\\Sound\\eSuddenIce_1.wav", 2);
	LoadWaveFile(SOUND_SUDDEN_ICE2, "Data\\Sound\\eSuddenIce_2.wav", 2);
	LoadWaveFile(SOUND_NUKE1, "Data\\Sound\\eHellFire2_1.wav", 1);
	LoadWaveFile(SOUND_NUKE2, "Data\\Sound\\eHellFire2_2.wav", 1);
	LoadWaveFile(SOUND_COMBO, "Data\\Sound\\eCombo.wav", 1);
	LoadWaveFile(SOUND_FURY_STRIKE1, "Data\\Sound\\eRageBlow_1.wav", 1);
	LoadWaveFile(SOUND_FURY_STRIKE2, "Data\\Sound\\eRageBlow_2.wav", 1);
	LoadWaveFile(SOUND_FURY_STRIKE3, "Data\\Sound\\eRageBlow_3.wav", 1);
	LoadWaveFile(SOUND_LEVEL_UP, "Data\\Sound\\pLevelUp.wav", 1);
	LoadWaveFile(SOUND_CHANGE_UP, "Data\\Sound\\nMalonSkillMaster.wav", 1);

	LoadWaveFile(SOUND_CHAOS_ENVIR, "Data\\Sound\\aChaos.wav", 1);
	LoadWaveFile(SOUND_CHAOS_END, "Data\\Sound\\aChaosEnd.wav", 1);
	LoadWaveFile(SOUND_CHAOS_FALLING, "Data\\Sound\\pMaleScream.wav", 1);
	LoadWaveFile(SOUND_CHAOS_FALLING_STONE, "Data\\Sound\\eWallFall.wav", 1);
	LoadWaveFile(SOUND_CHAOS_MOB_BOOM01, "Data\\Sound\\eMonsterBoom1.wav", 2);
	LoadWaveFile(SOUND_CHAOS_MOB_BOOM02, "Data\\Sound\\eMonsterBoom2.wav", 2);
	LoadWaveFile(SOUND_CHAOS_THUNDER01, "Data\\Sound\\eElec1.wav", 1);
	LoadWaveFile(SOUND_CHAOS_THUNDER02, "Data\\Sound\\eElec2.wav", 1);

	LoadWaveFile(SOUND_RUN_DARK_HORSE_1, "Data\\Sound\\pHorseStep1.wav", 1);
	LoadWaveFile(SOUND_RUN_DARK_HORSE_2, "Data\\Sound\\pHorseStep2.wav", 1);
	LoadWaveFile(SOUND_RUN_DARK_HORSE_3, "Data\\Sound\\pHorseStep3.wav", 1);
	LoadWaveFile(SOUND_DARKLORD_PAIN, "Data\\Sound\\pDarkPain.wav", 1);
	LoadWaveFile(SOUND_DARKLORD_DEAD, "Data\\Sound\\pDarkDeath.wav", 1);
	LoadWaveFile(SOUND_ATTACK_SPEAR, "Data\\Sound\\sDarkSpear.wav", 1);
	LoadWaveFile(SOUND_ATTACK_FIRE_BUST, "Data\\Sound\\eFirebust.wav", 1);
	LoadWaveFile(SOUND_ATTACK_FIRE_BUST_EXP, "Data\\Sound\\eFirebustBoom.wav", 1);
	LoadWaveFile(SOUND_PART_TELEPORT, "Data\\Sound\\eSummon.wav", 1);
	LoadWaveFile(SOUND_ELEC_STRIKE, "Data\\Sound\\sDarkElecSpike.wav", 1);
	LoadWaveFile(SOUND_ELEC_STRIKE_READY, "Data\\Sound\\sDarkElecSpikeReady.wav", 1);
	LoadWaveFile(SOUND_EARTH_QUAKE, "Data\\Sound\\sDarkEarthQuake.wav", 1);
	LoadWaveFile(SOUND_CRITICAL, "Data\\Sound\\sDarkCritical.wav", 1);
	LoadWaveFile(SOUND_DSPIRIT_MISSILE, "Data\\Sound\\DSpirit_Missile.wav", 4);
	LoadWaveFile(SOUND_DSPIRIT_SHOUT, "Data\\Sound\\DSpirit_Shout.wav", 1);
	LoadWaveFile(SOUND_DSPIRIT_RUSH, "Data\\Sound\\DSpirit_Rush.wav", 3);
	LoadWaveFile(SOUND_FENRIR_RUN_1, "Data\\Sound\\pW_run-01.wav", 1);
	LoadWaveFile(SOUND_FENRIR_RUN_2, "Data\\Sound\\pW_run-02.wav", 1);
	LoadWaveFile(SOUND_FENRIR_RUN_3, "Data\\Sound\\pW_run-03.wav", 1);
	LoadWaveFile(SOUND_FENRIR_WALK_1, "Data\\Sound\\pW_step-01.wav", 1);
	LoadWaveFile(SOUND_FENRIR_WALK_2, "Data\\Sound\\pW_step-02.wav", 1);
	LoadWaveFile(SOUND_FENRIR_DEATH, "Data\\Sound\\pWdeath.wav", 1);
	LoadWaveFile(SOUND_FENRIR_IDLE_1, "Data\\Sound\\pWidle1.wav", 1);
	LoadWaveFile(SOUND_FENRIR_IDLE_2, "Data\\Sound\\pWidle2.wav", 1);
	LoadWaveFile(SOUND_FENRIR_DAMAGE_1, "Data\\Sound\\pWpain1.wav", 1);
	LoadWaveFile(SOUND_FENRIR_DAMAGE_1, "Data\\Sound\\pWpain2.wav", 1);
	LoadWaveFile(SOUND_FENRIR_SKILL, "Data\\Sound\\pWskill.wav", 1);
	LoadWaveFile(SOUND_JEWEL02, "Data\\Sound\\Jewel_Sound.wav", 1);
	LoadWaveFile(sound_handclap_01, "Data\\Sound\\handclap-01.wav", 1, false);
	LoadWaveFile(sound_handclap_02, "Data\\Sound\\handclap-02.wav", 1, false);
	LoadWaveFile(sound_handclap_03, "Data\\Sound\\handclap-03.wav", 1, false);
	LoadWaveFile(sound_handclap_04, "Data\\Sound\\handclap-04.wav", 1, false);
	LoadWaveFile(sound_handclap_05, "Data\\Sound\\handclap-05.wav", 1, false);
	LoadWaveFile(sound_handclap_06, "Data\\Sound\\handclap-06.wav", 1, false);
	LoadWaveFile(SOUND_KUNDUN_ITEM_SOUND, "Data\\Sound\\kundunitem.wav", 1);

	g_XmasEvent->LoadXmasEventSound();
	g_NewYearsDayEvent->LoadSound();

	LoadWaveFile(SOUND_SHIELDCLASH, "Data\\Sound\\shieldclash.wav", 1);
	LoadWaveFile(SOUND_INFINITYARROW, "Data\\Sound\\infinityArrow.wav", 1);
	LoadWaveFile(SOUND_FIRE_SCREAM, "Data\\Sound\\Darklord_firescream.wav", 1);
	LoadWaveFile(SOUND_MOONRABBIT_WALK, "Data\\Sound\\SE_Ev_rabbit_walk.wav", 1);
	LoadWaveFile(SOUND_MOONRABBIT_DAMAGE, "Data\\Sound\\SE_Ev_rabbit_damage.wav", 1);
	LoadWaveFile(SOUND_MOONRABBIT_DEAD, "Data\\Sound\\SE_Ev_rabbit_death.wav", 1);
	LoadWaveFile(SOUND_MOONRABBIT_EXPLOSION, "Data\\Sound\\SE_Ev_rabbit_Explosion.wav", 1);
	LoadWaveFile(SOUND_SUMMON_SAHAMUTT, "Data\\Sound\\SE_Ch_summoner_skill05_explosion01.wav", 1);
	LoadWaveFile(SOUND_SUMMON_EXPLOSION, "Data\\Sound\\SE_Ch_summoner_skill05_explosion03.wav", 1);
	LoadWaveFile(SOUND_SUMMON_NEIL, "Data\\Sound\\SE_Ch_summoner_skill06_requiem01.wav", 1);
	LoadWaveFile(SOUND_SUMMON_REQUIEM, "Data\\Sound\\SE_Ch_summoner_skill06_requiem02.wav", 1);
	LoadWaveFile(SOUND_SUMMOM_RARGLE, "Data\\Sound\\Rargle.wav", 1);
	LoadWaveFile(SOUND_SUMMON_SKILL_LIGHTORB, "Data\\Sound\\SE_Ch_summoner_skill01_lightningof.wav", 1);
	LoadWaveFile(SOUND_SUMMON_SKILL_SLEEP, "Data\\Sound\\SE_Ch_summoner_skill03_sleep.wav", 1);
	LoadWaveFile(SOUND_SUMMON_SKILL_BLIND, "Data\\Sound\\SE_Ch_summoner_skill04_blind.wav", 1);
	LoadWaveFile(SOUND_SUMMON_SKILL_THORNS, "Data\\Sound\\SE_Ch_summoner_skill02_ssonze.wav", 1);
	LoadWaveFile(SOUND_SKILL_CHAIN_LIGHTNING, "Data\\Sound\\SE_Ch_summoner_skill08_chainlightning.wav", 1);
	LoadWaveFile(SOUND_SKILL_DRAIN_LIFE, "Data\\Sound\\SE_Ch_summoner_skill07_lifedrain.wav", 1);
	LoadWaveFile(SOUND_SKILL_WEAKNESS, "Data\\Sound\\SE_Ch_summoner_weakness.wav", 1);
	LoadWaveFile(SOUND_SKILL_ENERVATION, "Data\\Sound\\SE_Ch_summoner_innovation.wav", 1);
	LoadWaveFile(SOUND_SKILL_BERSERKER, "Data\\Sound\\Berserker.wav", 1);
	LoadWaveFile(SOUND_CHERRYBLOSSOM_EFFECT0, "Data\\Sound\\cherryblossom\\Eve_CherryBlossoms01.wav");
	LoadWaveFile(SOUND_CHERRYBLOSSOM_EFFECT1, "Data\\Sound\\cherryblossom\\Eve_CherryBlossoms02.wav");
	LoadWaveFile(SOUND_SKILL_BLOWOFDESTRUCTION, "Data\\Sound\\BLOW_OF_DESTRUCTION.wav");
	LoadWaveFile(SOUND_SKILL_FLAME_STRIKE, "Data\\Sound\\flame_strike.wav");
	LoadWaveFile(SOUND_SKILL_GIGANTIC_STORM, "Data\\Sound\\gigantic_storm.wav");
	LoadWaveFile(SOUND_SKILL_LIGHTNING_SHOCK, "Data\\Sound\\lightning_shock.wav");
	LoadWaveFile(SOUND_SKILL_SWELL_OF_MAGICPOWER, "Data\\Sound\\SwellofMagicPower.wav");
	LoadWaveFile(SOUND_SKILL_MULTI_SHOT, "Data\\Sound\\multi_shot.wav");
	LoadWaveFile(SOUND_SKILL_RECOVER, "Data\\Sound\\recover.wav");
	LoadWaveFile(SOUND_SKILL_CAOTIC, "Data\\Sound\\caotic.wav");

	LoadWaveFile(SOUND_XMAS_FIRECRACKER, "Data\\Sound\\xmas\\Christmas_Fireworks01.wav");

	g_09SummerEvent->LoadSound();

#ifdef PBG_ADD_NEWCHAR_MONK
	LoadWaveFile(SOUND_RAGESKILL_THRUST, "Data\\Sound\\Ragefighter\\Rage_Thrust.wav");
	LoadWaveFile(SOUND_RAGESKILL_THRUST_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Thrust_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_STAMP, "Data\\Sound\\Ragefighter\\Rage_Stamp.wav");
	LoadWaveFile(SOUND_RAGESKILL_STAMP_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Stamp_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_GIANTSWING, "Data\\Sound\\Ragefighter\\Rage_Giantswing.wav");
	LoadWaveFile(SOUND_RAGESKILL_GIANTSWING_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Giantswing_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_DARKSIDE, "Data\\Sound\\Ragefighter\\Rage_Darkside.wav");
	LoadWaveFile(SOUND_RAGESKILL_DARKSIDE_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Darkside_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_DRAGONLOWER, "Data\\Sound\\Ragefighter\\Rage_Dragonlower.wav");
	LoadWaveFile(SOUND_RAGESKILL_DRAGONLOWER_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Dragonlower_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_DRAGONKICK, "Data\\Sound\\Ragefighter\\Rage_Dragonkick.wav");
	LoadWaveFile(SOUND_RAGESKILL_DRAGONKICK_ATTACK, "Data\\Sound\\Ragefighter\\Rage_Dragonkick_Att.wav");
	LoadWaveFile(SOUND_RAGESKILL_BUFF_1, "Data\\Sound\\Ragefighter\\Rage_Buff_1.wav");
	LoadWaveFile(SOUND_RAGESKILL_BUFF_2, "Data\\Sound\\Ragefighter\\Rage_Buff_2.wav");
#endif //PBG_ADD_NEWCHAR_MONK
	LoadWaveFile(sound_CombatMaster_attack2, "Data\\Sound\\w69w70w71w72\\CombatMaster_attack2.wav", 4, 0);
	LoadWaveFile(sound_CombatMaster_attack3, "Data\\Sound\\w69w70w71w72\\CombatMaster_attack3.wav", 4, 0);
	LoadWaveFile(sound_Defender_attack2, "Data\\Sound\\w69w70w71w72\\Defender_attack2.wav", 4, 0);
}

extern int	g_iRenderTextType;


void OpenFont()
{
	InitPath();
	LoadBitmap("Interface\\FontInput.tga", BITMAP_FONT, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\FontTest.tga", BITMAP_FONT + 1);
	LoadBitmap("Interface\\Hit.tga", BITMAP_FONT_HIT, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\HUD\\FontNumPower.tga", BITMAP_FONT_POWER, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\HUD\\FontTextPower.tga", BITMAP_TEXT_POWER, GL_NEAREST, GL_CLAMP_TO_EDGE);

	g_pRenderText->Create(0, g_hDC);

	GMFontLayer->runtime_font_property(g_hFont, FontHeight - 1);
}

void SaveMacro(char* FileName)
{
	FILE* fp = fopen(FileName, "wt");
	for (int i = 0; i < 10; i++)
	{
		fprintf(fp, "%s\n", MacroText[i]);
	}
	fclose(fp);
}

void OpenMacro(char* FileName)
{
	FILE* fp = fopen(FileName, "rt");
	if (fp != NULL)
	{
		for (int i = 0; i < 10; i++)
		{
			fscanf(fp, "%s", MacroText[i]);
		}
		fclose(fp);
	}
}

void SaveOptions()
{
	// 0 ~ 19 skill hotkey
	BYTE options[30] = { 0x00, };

	int iSkillType = -1;
	for (int i = 0; i < 10; ++i)
	{
		iSkillType = g_pMainFrame->GetSkillHotKey(i);

		int iIndex = i * 2;
		if (iSkillType != -1)
		{

			options[iIndex] = HIBYTE(CharacterAttribute->Skill[iSkillType]);
			options[iIndex + 1] = LOBYTE(CharacterAttribute->Skill[iSkillType]);
		}
		else
		{
			options[iIndex] = 0xff;
			options[iIndex + 1] = 0xff;
		}
	}

	if (g_pOption->IsAutoAttack())
	{
		options[20] |= AUTOATTACK_ON;
	}

	if (g_pOption->IsWhisperSound())
	{
		options[20] |= WHISPER_SOUND_ON;
	}

	if (g_pOption->IsSlideHelp())
	{
		options[20] |= SLIDE_HELP_ON;
	}

	if (g_pOption->GetRenderEffect())
	{
		options[20] |= RENDER_EFFECT_ON;
	}

	if (g_pOption->GetRenderEquipment())
	{
		options[20] |= RENDER_EQUIPMENT_ON;
	}

	if (g_pOption->GetRenderTerrain())
	{
		options[20] |= RENDER_TERRAIN_ON;
	}

	if (g_pOption->GetRenderObjects())
	{
		options[20] |= RENDER_OBJECTS_ON;
	}

	options[21] = static_cast<BYTE>((g_pMainFrame->GetItemHotKey(SEASON3B::HOTKEY_Q) - ITEM_POTION) & 0xFF);
	options[22] = static_cast<BYTE>((g_pMainFrame->GetItemHotKey(SEASON3B::HOTKEY_W) - ITEM_POTION) & 0xFF);
	options[23] = static_cast<BYTE>((g_pMainFrame->GetItemHotKey(SEASON3B::HOTKEY_E) - ITEM_POTION) & 0xFF);

	BYTE wChatListBoxSize = g_pChatListBox->GetNumberOfLines(g_pChatListBox->GetCurrentMsgType()) / 3;

	if (g_bUseChatListBox == FALSE)
		wChatListBoxSize = 0;

	BYTE wChatListBoxBackAlpha = g_pChatListBox->GetBackAlpha() * 10;
	options[24] = wChatListBoxSize << 4 | wChatListBoxBackAlpha;
	options[25] = static_cast<BYTE>((g_pMainFrame->GetItemHotKey(SEASON3B::HOTKEY_R) - ITEM_POTION) & 0xFF);


	options[26] = g_pMainFrame->GetItemHotKeyLevel(SEASON3B::HOTKEY_Q);
	options[27] = g_pMainFrame->GetItemHotKeyLevel(SEASON3B::HOTKEY_W);
	options[28] = g_pMainFrame->GetItemHotKeyLevel(SEASON3B::HOTKEY_E);
	options[29] = g_pMainFrame->GetItemHotKeyLevel(SEASON3B::HOTKEY_R);

	SendRequestHotKey(options);
}

void OpenLogoSceneData()
{
	if (gmProtect->SceneLogin == 1)
	{
		LoadBitmap("Logo\\Sky5.jpg", BITMAP_LOG_IN + 9);
		LoadBitmap("Logo\\Sky0.jpg", BITMAP_LOG_IN + 10);

		gmClientModels->AccessModel(MODEL_SHIP, "Data\\Object1\\", "Ship01");
		gmClientModels->OpenTexture(MODEL_SHIP, "Object1\\", GL_REPEAT, GL_LINEAR);

		for (int i = 0; i < 4; i++)
		{
			gmClientModels->AccessModel(MODEL_LOGO + i, "Data\\Logo\\", "Logo", i + 1);
			gmClientModels->OpenTexture(MODEL_LOGO + i, "Logo\\", GL_REPEAT, GL_LINEAR);
		}
	}
	else if (gmProtect->SceneLogin == 2)
	{
		LoadBitmap("Logo\\Login02_Back01.jpg", BITMAP_LOG_IN + 9);
		LoadBitmap("Logo\\Login02_Back02.jpg", BITMAP_LOG_IN + 10);
	}

	//image
	::LoadBitmap("Interface\\cha_bt.tga", BITMAP_LOG_IN);
	::LoadBitmap("Interface\\server_b2_all.tga", BITMAP_LOG_IN + 1);
	::LoadBitmap("Interface\\server_b2_loding.jpg", BITMAP_LOG_IN + 2);
	::LoadBitmap("Interface\\server_deco_all.tga", BITMAP_LOG_IN + 3);
	::LoadBitmap("Interface\\server_menu_b_all.tga", BITMAP_LOG_IN + 4);
	::LoadBitmap("Interface\\server_credit_b_all.tga", BITMAP_LOG_IN + 5);
	::LoadBitmap("Interface\\deco.tga", BITMAP_LOG_IN + 6);
	::LoadBitmap("Interface\\login_back.tga", BITMAP_LOG_IN + 7);
	::LoadBitmap("Interface\\login_me.tga", BITMAP_LOG_IN + 8);
	::LoadBitmap("Interface\\server_ex03.tga", BITMAP_LOG_IN + 11, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\server_ex01.tga", BITMAP_LOG_IN + 12);
	::LoadBitmap("Interface\\server_ex02.jpg", BITMAP_LOG_IN + 13, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\cr_mu_lo.tga", BITMAP_LOG_IN + 14, GL_LINEAR);
	::LoadBitmap("Interface\\HUD\\ConnectList.tga", BITMAP_LOG_IN + 15, GL_LINEAR);
	
#ifdef MOVIE_DIRECTSHOW
	::LoadBitmap("Interface\\movie_b_all.tga", BITMAP_LOG_IN + 15);
#endif	// MOVIE_DIRECTSHOW
}

void ReleaseLogoSceneData()
{
	for (int i = BITMAP_LOG_IN; i <= BITMAP_LOG_IN_END; ++i)
		::DeleteBitmap(i);
	for (int i = BITMAP_TEMP; i < BITMAP_TEMP + 30; i++)
		DeleteBitmap(i);

	gMapManager->DeleteObjects();
	ClearCharacters();
}

void OpenCharacterSceneData()
{
	if (gmProtect->SceneCharacter == 1)
	{
		LoadBitmap("Logo\\Interface01.tga", BITMAP_LOG_IN + 8, GL_NEAREST, GL_REPEAT);
		LoadBitmap("Logo\\Interface02.tga", BITMAP_LOG_IN + 9, GL_NEAREST, GL_REPEAT);
		LoadBitmap("Logo\\Interface03.tga", BITMAP_LOG_IN + 10, GL_NEAREST, GL_REPEAT);
		LoadBitmap("Logo\\Interface04.tga", BITMAP_LOG_IN + 14, GL_NEAREST, GL_REPEAT);
	}
	else if (gmProtect->SceneCharacter == 2)
	{
		LoadBitmap("Logo\\character02_back01.jpg", BITMAP_LOG_IN + 9);
		LoadBitmap("Logo\\character02_back02.jpg", BITMAP_LOG_IN + 10);
	}

	LoadBitmap("Interface\\cha_id.tga", BITMAP_LOG_IN);
	LoadBitmap("Interface\\cha_bt.tga", BITMAP_LOG_IN + 1);
	LoadBitmap("Interface\\deco.tga", BITMAP_LOG_IN + 2);
	LoadBitmap("Interface\\b_create.tga", BITMAP_LOG_IN + 3);
	LoadBitmap("Interface\\server_menu_b_all.tga", BITMAP_LOG_IN + 4);
	LoadBitmap("Interface\\b_connect.tga", BITMAP_LOG_IN + 5);
	LoadBitmap("Interface\\b_delete.tga", BITMAP_LOG_IN + 6);

	if (GMProtect->CharInfoBalloonType() == 0)
		LoadBitmap("Interface\\character_ex.tga", BITMAP_LOG_IN + 7);
	if (GMProtect->CharInfoBalloonType() == 1)
		LoadBitmap("Interface\\HUD\\character_ex(1).tga", BITMAP_LOG_IN + 7);
	if (GMProtect->CharInfoBalloonType() == 2)
		LoadBitmap("Interface\\HUD\\character_ex(2).tga", BITMAP_LOG_IN + 7);

	LoadBitmap("Interface\\server_ex03.tga", BITMAP_LOG_IN + 11, GL_NEAREST, GL_REPEAT);
	LoadBitmap("Interface\\server_ex01.tga", BITMAP_LOG_IN + 12);
	LoadBitmap("Interface\\server_ex02.jpg", BITMAP_LOG_IN + 13, GL_NEAREST, GL_REPEAT);

	LoadBitmap("Interface\\HUD\\CharacterSelect_I1.tga", BITMAP_LOG_IN + 15);


	LoadBitmap("Effect\\Impack03.jpg", BITMAP_EXT_LOG_IN + 2);

	LoadBitmap("Logo\\chasellight.jpg", BITMAP_EFFECT);

	gmClientModels->AccessModel(MODEL_CARD, "Data\\Logo\\", "Logo", 5);
	gmClientModels->OpenTexture(MODEL_CARD, "Logo\\", GL_REPEAT, GL_LINEAR);

	int Class = MAX_CLASS;

	for (int i = 0; i < Class; i++)
	{
		gmClientModels->AccessModel(MODEL_FACE + i, "Data\\Logo\\", "NewFace", i + 1);
		gmClientModels->OpenTexture(MODEL_FACE + i, "Logo\\");
	}

	for (int i = 0; i < Class; i++)
	{
		gmClientModels->GetModel(MODEL_FACE + i)->Actions[0].PlaySpeed = 0.3f;
		gmClientModels->GetModel(MODEL_FACE + i)->Actions[1].PlaySpeed = 0.3f;
	}
	gmClientModels->GetModel(MODEL_FACE + CLASS_SUMMONER)->Actions[0].PlaySpeed = 0.25f;
}

void ReleaseCharacterSceneData()
{
	for (int i = BITMAP_LOG_IN; i <= BITMAP_LOG_IN_END; ++i)
		::DeleteBitmap(i);

	for (int i = BITMAP_TEMP; i < BITMAP_TEMP + 30; i++)
		DeleteBitmap(i);

	gMapManager->DeleteObjects();

	for (int i = 0; i < MAX_CLASS; i++)
		gmClientModels->GetModel(MODEL_FACE + i)->Release();

	ClearCharacters();
}

void OpenBasicData(HDC hDC)
{
	CUIMng& rUIMng = CUIMng::Instance();

	rUIMng.RenderTitleSceneUI(hDC, 0, 12);

	LoadBitmap("Interface\\Cursor.tga", BITMAP_CURSOR, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorPush.tga", BITMAP_CURSOR + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorAttack.tga", BITMAP_CURSOR + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorGet.tga", BITMAP_CURSOR + 3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorTalk.tga", BITMAP_CURSOR + 4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorRepair.tga", BITMAP_CURSOR + 5, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorLeanAgainst.tga", BITMAP_CURSOR + 6, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorSitDown.tga", BITMAP_CURSOR + 7, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorDontMove.tga", BITMAP_CURSOR + 8, GL_LINEAR, GL_CLAMP_TO_EDGE);
	//interface
	LoadBitmap("Interface\\ok.jpg", BITMAP_INTERFACE + 11);
	LoadBitmap("Interface\\ok2.jpg", BITMAP_INTERFACE + 12);
	LoadBitmap("Interface\\cancel.jpg", BITMAP_INTERFACE + 13);
	LoadBitmap("Interface\\cancel2.jpg", BITMAP_INTERFACE + 14);
	LoadBitmap("Interface\\win_titlebar.jpg", BITMAP_INTERFACE_EX + 8);
	LoadBitmap("Interface\\win_button.tga", BITMAP_INTERFACE_EX + 9);
	LoadBitmap("Interface\\win_size.jpg", BITMAP_INTERFACE_EX + 10);
	LoadBitmap("Interface\\win_resize.tga", BITMAP_INTERFACE_EX + 11);
	LoadBitmap("Interface\\win_scrollbar.jpg", BITMAP_INTERFACE_EX + 12);
	LoadBitmap("Interface\\win_check.tga", BITMAP_INTERFACE_EX + 13);
	LoadBitmap("Interface\\win_mail.tga", BITMAP_INTERFACE_EX + 14);
	LoadBitmap("Interface\\win_mark.tga", BITMAP_INTERFACE_EX + 15);
	LoadBitmap("Interface\\win_letter.jpg", BITMAP_INTERFACE_EX + 16);
	LoadBitmap("Interface\\win_man.jpg", BITMAP_INTERFACE_EX + 17);
	LoadBitmap("Interface\\win_push.jpg", BITMAP_INTERFACE_EX + 18);
	LoadBitmap("Interface\\win_question.tga", BITMAP_INTERFACE_EX + 20);
	LoadBitmap("Logo\\Webzenlogo.jpg", BITMAP_INTERFACE_EX + 22);

#ifdef DUEL_SYSTEM
	LoadWaveFile(SOUND_OPEN_DUELWINDOW, "Data\\Sound\\iDuelWindow.wav", 1);
	LoadWaveFile(SOUND_START_DUEL, "Data\\Sound\\iDuelStart.wav", 1);
#endif // DUEL_SYSTEM

	LoadBitmap("Interface\\CursorID.tga", BITMAP_INTERFACE_EX + 29);
	LoadBitmap("Interface\\bar.jpg", BITMAP_INTERFACE + 23);
	LoadBitmap("Interface\\back1.jpg", BITMAP_INTERFACE + 24);
	LoadBitmap("Interface\\back2.jpg", BITMAP_INTERFACE + 25);
	LoadBitmap("Interface\\back3.jpg", BITMAP_INTERFACE + 26);

	LoadBitmap("Effect\\Fire01.jpg", BITMAP_FIRE, GL_LINEAR, GL_CLAMP_TO_EDGE);			// GM3rdChangeUp, GMCrywolf1st,GMHellas,Kanturu 3rd
	LoadBitmap("Effect\\Fire02.jpg", BITMAP_FIRE + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);			// GM3rdChangeUp, GMCrywolf1st,GMHellas,Kanturu 3rd
	LoadBitmap("Effect\\Fire03.jpg", BITMAP_FIRE + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);			// GM3rdChangeUp, GMCrywolf1st,GMHellas,Kanturu 3rd
	LoadBitmap("Effect\\PoundingBall.jpg", BITMAP_POUNDING_BALL, GL_LINEAR, GL_CLAMP_TO_EDGE); // Kanturu 2nd
	LoadBitmap("Effect\\fi01.jpg", BITMAP_ADV_SMOKE, GL_LINEAR, GL_CLAMP_TO_EDGE);		// GM 3rd ChangeUp, CryingWolf2nd
	LoadBitmap("Effect\\fi02.tga", BITMAP_ADV_SMOKE + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);		// GM 3rd ChangeUp, CryingWolf2nd
	LoadBitmap("Effect\\fantaF.jpg", BITMAP_TRUE_FIRE, GL_LINEAR, GL_CLAMP_TO_EDGE);	// GM Aida, GMBattleCastle, ....
	LoadBitmap("Effect\\fantaB.jpg", BITMAP_TRUE_BLUE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\JointSpirit02.jpg", BITMAP_JOINT_SPIRIT2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Piercing.jpg", BITMAP_PIERCING, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Monster\\iui06.jpg", BITMAP_ROBE + 6);
	LoadBitmap("Effect\\Magic_b.jpg", BITMAP_MAGIC_EMBLEM);
	LoadBitmap("Player\\dark3chima3.tga", BITMAP_DARKLOAD_SKIRT_3RD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Player\\kaa.tga", BITMAP_DARK_LOAD_SKIRT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\ShockWave.jpg", BITMAP_SHOCK_WAVE, GL_LINEAR);
	LoadBitmap("Effect\\Flame01.jpg", BITMAP_FLAME, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\flare01.jpg", BITMAP_LIGHT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Magic_Ground1.jpg", BITMAP_MAGIC, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Magic_Ground2.jpg", BITMAP_MAGIC + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Magic_Circle1.jpg", BITMAP_MAGIC + 2, GL_LINEAR);
#ifdef ASG_ADD_INFLUENCE_GROUND_EFFECT
	LoadBitmap("Effect\\guild_ring01.jpg", BITMAP_OUR_INFLUENCE_GROUND, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\enemy_ring02.jpg", BITMAP_ENEMY_INFLUENCE_GROUND, GL_LINEAR, GL_CLAMP_TO_EDGE);
#endif	// ASG_ADD_INFLUENCE_GROUND_EFFECT
	LoadBitmap("Effect\\Spark02.jpg", BITMAP_SPARK, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Spark03.jpg", BITMAP_SPARK + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\smoke01.jpg", BITMAP_SMOKE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\smoke02.tga", BITMAP_SMOKE + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\smoke05.tga", BITMAP_SMOKE + 4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\blood01.tga", BITMAP_BLOOD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\blood.tga", BITMAP_BLOOD + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Explotion01.jpg", BITMAP_EXPLOTION, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Skill\\twlighthik01.jpg", BITMAP_TWLIGHT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Skill\\2line_gost.jpg", BITMAP_2LINE_GHOST, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\damage01mono.jpg", BITMAP_DAMAGE_01_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\SwordEff_mono.jpg", BITMAP_SWORD_EFFECT_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\flamestani.jpg", BITMAP_FLAMESTANI, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Effect\\Spark.jpg", BITMAP_SPARK + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firehik02.jpg", BITMAP_FIRE_CURSEDLICH, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\totemgolem_leaf.tga", BITMAP_LEAF_TOTEMGOLEM, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\empact01.jpg", BITMAP_SUMMON_IMPACT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\loungexflow.jpg", BITMAP_SUMMON_SAHAMUTT_EXPLOSION, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\gostmark01.jpg", BITMAP_DRAIN_LIFE_GHOST, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\mzine_typer2.jpg", BITMAP_MAGIC_ZIN, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\shiny05.jpg", BITMAP_SHINY + 6, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\hikorora.jpg", BITMAP_ORORA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\lightmarks.jpg", BITMAP_LIGHT_MARKS, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\cursorpin01.jpg", BITMAP_TARGET_POSITION_EFFECT1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\cursorpin02.jpg", BITMAP_TARGET_POSITION_EFFECT2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\smokelines01.jpg", BITMAP_SMOKELINE1, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\smokelines02.jpg", BITMAP_SMOKELINE2, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\smokelines03.jpg", BITMAP_SMOKELINE3, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\lighting_mega01.jpg", BITMAP_LIGHTNING_MEGA1, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\lighting_mega02.jpg", BITMAP_LIGHTNING_MEGA2, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\lighting_mega03.jpg", BITMAP_LIGHTNING_MEGA3, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\firehik01.jpg", BITMAP_FIRE_HIK1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firehik03.jpg", BITMAP_FIRE_HIK3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firehik_mono01.jpg", BITMAP_FIRE_HIK1_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firehik_mono02.jpg", BITMAP_FIRE_HIK2_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firehik_mono03.jpg", BITMAP_FIRE_HIK3_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\clouds3.jpg", BITMAP_RAKLION_CLOUDS, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\icenightlight.jpg", BITMAP_IRONKNIGHT_BODY_BRIGHT, GL_LINEAR, GL_CLAMP_TO_EDGE);

	g_XmasEvent->LoadXmasEventEffect();

	LoadBitmap("Skill\\younghtest1.tga", BITMAP_GM_HAIR_1);
	LoadBitmap("Skill\\younghtest3.tga", BITMAP_GM_HAIR_3);
	LoadBitmap("Skill\\gmmzine.jpg", BITMAP_GM_AURORA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\explotion01mono.jpg", BITMAP_EXPLOTION_MONO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\Success_kantru.tga", BITMAP_KANTURU_SUCCESS, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\Failure_kantru.tga", BITMAP_KANTURU_FAILED, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\MonsterCount.tga", BITMAP_KANTURU_COUNTER, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Clud64.jpg", BITMAP_CLUD64, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\clouds.jpg", BITMAP_CLOUD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Skill\\SwordEff.jpg", BITMAP_BLUE_BLUR, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Impack03.jpg", BITMAP_IMPACT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\ScolTail.jpg", BITMAP_SCOLPION_TAIL, GL_LINEAR);
	LoadBitmap("Effect\\FireSnuff.jpg", BITMAP_FIRE_SNUFF, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\coll.jpg", BITMAP_SPOT_WATER, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\BowE.jpg", BITMAP_DS_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shockwave.jpg", BITMAP_DS_SHOCK, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\DinoE.jpg", BITMAP_EXPLOTION + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shiny01.jpg", BITMAP_SHINY, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shiny02.jpg", BITMAP_SHINY + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shiny03.jpg", BITMAP_SHINY + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\eye01.jpg", BITMAP_SHINY + 3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\ring.jpg", BITMAP_SHINY + 4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shiny04.jpg", BITMAP_SHINY + 5, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Chrome01.jpg", BITMAP_CHROME, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\blur01.jpg", BITMAP_BLUR, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\bab2.jpg", BITMAP_CHROME + 1, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\motion_blur.jpg", BITMAP_BLUR + 1, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\motion_blur_r.jpg", BITMAP_BLUR + 2, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\motion_mono.jpg", BITMAP_BLUR + 3, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\motion_blur_r3.jpg", BITMAP_BLUR + 6, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\gra.jpg", BITMAP_BLUR + 7, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\spinmark01.jpg", BITMAP_BLUR + 8, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\flamestani.jpg", BITMAP_BLUR + 9, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\sword_blur.jpg", BITMAP_BLUR + 10, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\joint_sword_red.jpg", BITMAP_BLUR + 11, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\motion_blur_r2.jpg", BITMAP_BLUR + 12, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\motion_blur_r3.jpg", BITMAP_BLUR + 13, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\blur02.jpg", BITMAP_BLUR2, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\lightning2.jpg", BITMAP_LIGHTNING + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Thunder01.jpg", BITMAP_ENERGY, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\Spark01.jpg", BITMAP_JOINT_SPARK, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\JointThunder01.jpg", BITMAP_JOINT_THUNDER, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\JointSpirit01.jpg", BITMAP_JOINT_SPIRIT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\JointLaser01.jpg", BITMAP_JOINT_ENERGY, GL_LINEAR);
	LoadBitmap("Effect\\JointEnergy01.jpg", BITMAP_JOINT_HEALING, GL_LINEAR);
	LoadBitmap("Effect\\JointLaser02.jpg", BITMAP_JOINT_LASER + 1, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Monster\\iui03.jpg", BITMAP_JANUSEXT, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Monster\\magic_H.tga", BITMAP_PHO_R_HAIR, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Item\\Texture\\t_lower_14m.tga", BITMAP_PANTS_G_SOUL, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Skill\\Skull.jpg", BITMAP_SKULL, GL_NEAREST);
	LoadBitmap("Effect\\motion_blur_r2.jpg", BITMAP_JOINT_FORCE, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Effect\\Fire04.jpg", BITMAP_FIRECRACKER, GL_LINEAR);
	LoadBitmap("Effect\\Flare.jpg", BITMAP_FLARE, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Effect\\Chrome02.jpg", BITMAP_CHROME2, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Chrome03.jpg", BITMAP_CHROME3, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Chrome06.jpg", BITMAP_CHROME6, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Chrome07.jpg", BITMAP_CHROME7, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\energy01.jpg", BITMAP_CHROME_ENERGY, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\energy02.jpg", BITMAP_CHROME_ENERGY2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\2.jpg", BITMAP_2LINE, GL_LINEAR, GL_REPEAT);

	LoadBitmap("Effect\\firecracker0001.jpg", BITMAP_FIRECRACKER0001, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0002.jpg", BITMAP_FIRECRACKER0002, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0003.jpg", BITMAP_FIRECRACKER0003, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0004.jpg", BITMAP_FIRECRACKER0004, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0005.jpg", BITMAP_FIRECRACKER0005, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0006.jpg", BITMAP_FIRECRACKER0006, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\firecracker0007.jpg", BITMAP_FIRECRACKER0007, GL_NEAREST, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Shiny05.jpg", BITMAP_SHINY + 5, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Effect\\partCharge1\\bujuckline.jpg", BITMAP_LUCKY_SEAL_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Item\\Texture\\nfm03.jpg", BITMAP_BLUECHROME, GL_NEAREST, GL_REPEAT);
	LoadBitmap("Effect\\flareBlue.jpg", BITMAP_FLARE_BLUE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\NSkill.jpg", BITMAP_FLARE_FORCE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Flare02.jpg", BITMAP_FLARE + 1, GL_LINEAR, GL_REPEAT);
	LoadBitmap("Monster\\King11.jpg", BITMAP_WHITE_WIZARD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\Kni000.jpg", BITMAP_DEST_ORC_WAR0, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\Kni011.jpg", BITMAP_DEST_ORC_WAR1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\Kni022.jpg", BITMAP_DEST_ORC_WAR2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Skill\\pinkWave.jpg", BITMAP_PINK_WAVE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\flareRed.jpg", BITMAP_FLARE_RED, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Fire05.jpg", BITMAP_FIRE + 3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Hole.jpg", BITMAP_HOLE, GL_LINEAR, GL_REPEAT);
	//	LoadBitmap( "Monster\\mop011.jpg"        , BITMAP_OTHER_SKIN,    GL_LINEAR, GL_REPEAT );
	LoadBitmap("Effect\\WATERFALL1.jpg", BITMAP_WATERFALL_1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\WATERFALL2.jpg", BITMAP_WATERFALL_2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\WATERFALL3.jpg", BITMAP_WATERFALL_3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\WATERFALL4.jpg", BITMAP_WATERFALL_4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\WATERFALL5.jpg", BITMAP_WATERFALL_5, GL_LINEAR, GL_CLAMP_TO_EDGE);
	/*
	LoadBitmap("Interface\\in_bar.tga"		, BITMAP_MVP_INTERFACE, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_bar2.jpg"		, BITMAP_MVP_INTERFACE + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_deco.tga"		, BITMAP_MVP_INTERFACE + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main.tga"		, BITMAP_MVP_INTERFACE + 3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_icon_bal1.tga"		, BITMAP_MVP_INTERFACE + 4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_icon_dl1.tga"		, BITMAP_MVP_INTERFACE + 5, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_icon_dl2.tga"		, BITMAP_MVP_INTERFACE + 6, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_number1.tga"		, BITMAP_MVP_INTERFACE + 7, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_number2.tga"		, BITMAP_MVP_INTERFACE + 8, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main2.tga"		, BITMAP_MVP_INTERFACE + 9, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_failure.tga"		, BITMAP_MVP_INTERFACE + 10, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_success.tga"		, BITMAP_MVP_INTERFACE + 11, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\t_main.tga"		, BITMAP_MVP_INTERFACE + 12, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_no1.tga"		, BITMAP_MVP_INTERFACE + 13, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_no2.tga"		, BITMAP_MVP_INTERFACE + 14, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_no3.tga"		, BITMAP_MVP_INTERFACE + 15, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_ok1.tga"		, BITMAP_MVP_INTERFACE + 16, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_ok2.tga"		, BITMAP_MVP_INTERFACE + 17, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_ok3.tga"		, BITMAP_MVP_INTERFACE + 18, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_yes1.tga"		, BITMAP_MVP_INTERFACE + 19, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_yes2.tga"		, BITMAP_MVP_INTERFACE + 20, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_b_yes3.tga"		, BITMAP_MVP_INTERFACE + 21, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_main.tga"		, BITMAP_MVP_INTERFACE + 22, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_number1_1.tga"		, BITMAP_MVP_INTERFACE + 23, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_number2_1.tga"		, BITMAP_MVP_INTERFACE + 24, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\in_main_number0_2.tga"		, BITMAP_MVP_INTERFACE + 25, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_table.tga"		, BITMAP_MVP_INTERFACE + 26, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_rank.tga"		, BITMAP_MVP_INTERFACE + 27, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_D.tga"		, BITMAP_MVP_INTERFACE + 28, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_C.tga"		, BITMAP_MVP_INTERFACE + 29, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_B.tga"		, BITMAP_MVP_INTERFACE + 30, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_A.tga"		, BITMAP_MVP_INTERFACE + 31, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_S.tga"		, BITMAP_MVP_INTERFACE + 32, GL_LINEAR, GL_CLAMP_TO_EDGE);
	//icon_Rank_0
	LoadBitmap("Interface\\icon_Rank_0.tga"		, BITMAP_MVP_INTERFACE + 33, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_1.tga"		, BITMAP_MVP_INTERFACE + 34, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_2.tga"		, BITMAP_MVP_INTERFACE + 35, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_3.tga"		, BITMAP_MVP_INTERFACE + 36, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_4.tga"		, BITMAP_MVP_INTERFACE + 37, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_5.tga"		, BITMAP_MVP_INTERFACE + 38, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_6.tga"		, BITMAP_MVP_INTERFACE + 39, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_7.tga"		, BITMAP_MVP_INTERFACE + 40, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_8.tga"		, BITMAP_MVP_INTERFACE + 41, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_9.tga"		, BITMAP_MVP_INTERFACE + 42, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\icon_Rank_exp.tga"		, BITMAP_MVP_INTERFACE + 43, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\m_main_rank.tga"		, BITMAP_MVP_INTERFACE + 44, GL_LINEAR, GL_CLAMP_TO_EDGE);
	*/
	//m_main_rank
	//icon_Rank_exp
	LoadBitmap("Interface\\BattleSkill.tga", BITMAP_INTERFACE_EX + 34);
	LoadBitmap("Effect\\Flashing.jpg", BITMAP_FLASH, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\senatusmap.jpg", BITMAP_INTERFACE_EX + 35);
	LoadBitmap("Interface\\gate_button2.jpg", BITMAP_INTERFACE_EX + 36);
	LoadBitmap("Interface\\gate_button1.jpg", BITMAP_INTERFACE_EX + 37);
	LoadBitmap("Interface\\suho_button2.jpg", BITMAP_INTERFACE_EX + 38);
	LoadBitmap("Interface\\suho_button1.jpg", BITMAP_INTERFACE_EX + 39);
	LoadBitmap("Interface\\DoorCL.jpg", BITMAP_INTERFACE_EX + 40);
	LoadBitmap("Interface\\DoorOP.jpg", BITMAP_INTERFACE_EX + 41);
	//OpenJpeg( "Effect\\FireSnuff.jpg"       , BITMAP_FIRE_SNUFF,    GL_LINEAR, GL_CLAMP_TO_EDGE );
	LoadBitmap("Object31\\Flag.tga", BITMAP_INTERFACE_MAP + 0, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\CursorAttack2.tga", BITMAP_CURSOR2);
	LoadBitmap("Effect\\Cratered.tga", BITMAP_CRATER);
	LoadBitmap("Effect\\FormationMark.tga", BITMAP_FORMATION_MARK);
	LoadBitmap("Effect\\Plus.tga", BITMAP_PLUS);
	LoadBitmap("Effect\\eff_lighting.jpg", BITMAP_FENRIR_THUNDER);
	LoadBitmap("Effect\\eff_lightinga01.jpg", BITMAP_FENRIR_FOOT_THUNDER1);
	LoadBitmap("Effect\\eff_lightinga02.jpg", BITMAP_FENRIR_FOOT_THUNDER2);
	LoadBitmap("Effect\\eff_lightinga03.jpg", BITMAP_FENRIR_FOOT_THUNDER3);
	LoadBitmap("Effect\\eff_lightinga04.jpg", BITMAP_FENRIR_FOOT_THUNDER4);
	LoadBitmap("Effect\\eff_lightinga05.jpg", BITMAP_FENRIR_FOOT_THUNDER5);
	LoadBitmap("Interface\\Progress_Back.jpg", BITMAP_INTERFACE_EX + 42);
	LoadBitmap("Interface\\Progress.jpg", BITMAP_INTERFACE_EX + 43);
	LoadBitmap("Effect\\Inferno.jpg", BITMAP_INFERNO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Lava.jpg", BITMAP_LAVA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\frame.tga", BITMAP_INTERFACE_MAP + 3);
	LoadBitmap("Interface\\i_attack.tga", BITMAP_INTERFACE_MAP + 4);
	LoadBitmap("Interface\\i_defense.tga", BITMAP_INTERFACE_MAP + 5);
	LoadBitmap("Interface\\i_wait.tga", BITMAP_INTERFACE_MAP + 6);
	LoadBitmap("Interface\\b_command01.jpg", BITMAP_INTERFACE_MAP + 8);
	LoadBitmap("Interface\\b_command02.jpg", BITMAP_INTERFACE_MAP + 9);
	LoadBitmap("Interface\\b_group02.jpg", BITMAP_INTERFACE_MAP + 10);
	LoadBitmap("Interface\\b_zoomout01.jpg", BITMAP_INTERFACE_MAP + 11);
	LoadBitmap("Interface\\hourglass.tga", BITMAP_INTERFACE_MAP + 7);
	LoadBitmap("Interface\\dot.tga", BITMAP_INTERFACE_EX + 44);
	LoadBitmap("Object9\\Impack03.jpg", BITMAP_LIGHT + 1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\buserbody_r.jpg", BITMAP_BERSERK_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\busersword_r.jpg", BITMAP_BERSERK_WP_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	//	LoadBitmap("Monster\\gigantiscorn_R.jpg", BITMAP_GIGANTIS_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\bladeeff2_r.jpg", BITMAP_BLADEHUNTER_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\illumi_R.jpg", BITMAP_TWINTAIL_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\prsona_R.jpg", BITMAP_PRSONA_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\prsonass_R.jpg", BITMAP_PRSONA_EFFECT2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("effect\\water.jpg", BITMAP_TWINTAIL_WATER, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\cra_04.jpg", BITMAP_LIGHT + 2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\Impack01.jpg", BITMAP_LIGHT + 3, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Interface\\message_ok_b_all.tga", BITMAP_BUTTON);
	::LoadBitmap("Interface\\loding_cancel_b_all.tga", BITMAP_BUTTON + 1);
	::LoadBitmap("Interface\\message_close_b_all.tga", BITMAP_BUTTON + 2);
	::LoadBitmap("Interface\\message_back.tga", BITMAP_MESSAGE_WIN);
	::LoadBitmap("Interface\\delete_secret_number.tga", BITMAP_MSG_WIN_INPUT);
	::LoadBitmap("Interface\\op1_stone.jpg", BITMAP_SYS_WIN, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\op1_back1.tga", BITMAP_SYS_WIN + 1);
	::LoadBitmap("Interface\\op1_back2.tga", BITMAP_SYS_WIN + 2);
	::LoadBitmap("Interface\\op1_back3.jpg", BITMAP_SYS_WIN + 3, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\op1_back4.jpg", BITMAP_SYS_WIN + 4, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\op1_b_all.tga", BITMAP_TEXT_BTN);
	::LoadBitmap("Interface\\op2_back1.tga", BITMAP_OPTION_WIN);
	::LoadBitmap("Interface\\op2_ch.tga", BITMAP_CHECK_BTN);
	::LoadBitmap("Interface\\op2_volume3.tga", BITMAP_SLIDER);
	::LoadBitmap("Interface\\op2_volume2.jpg", BITMAP_SLIDER + 1, GL_NEAREST, GL_REPEAT);
	::LoadBitmap("Interface\\op2_volume1.tga", BITMAP_SLIDER + 2);
	::LoadBitmap("Effect\\clouds2.jpg", BITMAP_EVENT_CLOUD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\pin_lights.jpg", BITMAP_PIN_LIGHT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\deasuler_cloth.tga", BITMAP_DEASULER_CLOTH, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Item\\Texture\\soketmagic_stape02.jpg", BITMAP_SOCKETSTAFF, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\lightmarks.jpg", BITMAP_LIGHTMARKS, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Effect\\lightmarks.jpg", BITMAP_LIGHTMARKS_FOREIGN, GL_LINEAR, GL_CLAMP_TO_EDGE);

	rUIMng.RenderTitleSceneUI(hDC, 1, 12);

	::LoadBitmap("Item\\Texture\\entrance_R.jpg", BITMAP_FREETICKET_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\juju_R.jpg", BITMAP_CHAOSCARD_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Item\\Texture\\monmark01a.jpg", BITMAP_LUCKY_SEAL_EFFECT43, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\monmark02a.jpg", BITMAP_LUCKY_SEAL_EFFECT44, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\monmark03a.jpg", BITMAP_LUCKY_SEAL_EFFECT45, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Item\\Texture\\bujuck01alpa.jpg", BITMAP_LUCKY_CHARM_EFFECT53, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\expensiveitem01_R.jpg", BITMAP_RAREITEM1_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\expensiveitem02a_R.jpg", BITMAP_RAREITEM2_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\expensiveitem02b_R.jpg", BITMAP_RAREITEM3_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\expensiveitem03a_R.jpg", BITMAP_RAREITEM4_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\expensiveitem03b_R.jpg", BITMAP_RAREITEM5_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Item\\Texture\\alicecard_R.tga", BITMAP_CHARACTERCARD_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Item\\Ingameshop\\kacama_R.jpg", BITMAP_CHARACTERCARD_R_MA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Ingameshop\\kacada_R.jpg", BITMAP_CHARACTERCARD_R_DA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Ingameshop\\kacama_R.jpg", BITMAP_CHARACTERCARD_R_LA, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Item\\Texture\\jujug_R.jpg", BITMAP_NEWCHAOSCARD_GOLD_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\jujul_R.jpg", BITMAP_NEWCHAOSCARD_RARE_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\jujum_R.jpg", BITMAP_NEWCHAOSCARD_MINI_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Effect\\cherryblossom\\sakuras01.jpg", BITMAP_CHERRYBLOSSOM_EVENT_PETAL, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Effect\\cherryblossom\\sakuras02.jpg", BITMAP_CHERRYBLOSSOM_EVENT_FLOWER, GL_LINEAR, GL_CLAMP_TO_EDGE);

	::LoadBitmap("Object39\\k_effect_01.JPG", BITMAP_KANTURU_2ND_EFFECT1, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\deathbeamstone_R.jpg", BITMAP_ITEM_EFFECT_DBSTONE_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\hellhorn_R.jpg", BITMAP_ITEM_EFFECT_HELLHORN_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\phoenixfeather_R.tga", BITMAP_ITEM_EFFECT_PFEATHER_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\Deye_R.jpg", BITMAP_ITEM_EFFECT_DEYE_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Item\\Texture\\wing3chaking2.jpg", BITMAP_ITEM_NIGHT_3RDWING_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("NPC\\lumi.jpg", BITMAP_CURSEDTEMPLE_NPC_MESH_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("item\\Texture\\songko2_R.jpg", BITMAP_CURSEDTEMPLE_HOLYITEM_MESH_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("skill\\unitedsoldier_wing.tga", BITMAP_CURSEDTEMPLE_ALLIED_PHYSICSCLOTH);
	LoadBitmap("skill\\illusionistcloth.tga", BITMAP_CURSEDTEMPLE_ILLUSION_PHYSICSCLOTH);
	LoadBitmap("effect\\masker.jpg", BITMAP_CURSEDTEMPLE_EFFECT_MASKER, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("effect\\wind01.jpg", BITMAP_CURSEDTEMPLE_EFFECT_WIND, GL_LINEAR, GL_CLAMP_TO_EDGE);
	::LoadBitmap("Interface\\menu01_new2_SD.jpg", BITMAP_INTERFACE_EX + 46);

#if MAIN_UPDATE > 303
#ifdef ASG_ADD_GENS_SYSTEM
#ifdef CHINESE_LANGUAGE
	std::string strFileName = "Local\\ImgsMapName\\MapNameAddStrife.tga";
#else
	std::string strFileName = "Local\\" + g_strSelectedML + "\\ImgsMapName\\MapNameAddStrife.tga";
#endif // CHINESE_LANGUAGE

	::LoadBitmap(strFileName.c_str(), BITMAP_INTERFACE_EX + 47);
#endif	// ASG_ADD_GENS_SYSTEM
#endif // MAIN_UPDATE > 303


#ifdef ASG_ADD_GENS_MARK
	::LoadBitmap("Interface\\Gens_mark_D_new.tga", BITMAP_GENS_MARK_DUPRIAN);
	::LoadBitmap("Interface\\Gens_mark_V_new.tga", BITMAP_GENS_MARK_BARNERT);
#endif	// ASG_ADD_GENS_MARK

	LoadBitmap("Monster\\serufanarm_R.jpg", BITMAP_SERUFAN_ARM_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\serufanwand_R.jpg", BITMAP_SERUFAN_WAND_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("npc\\santa.jpg", BITMAP_GOOD_SANTA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("npc\\santa_baggage.jpg", BITMAP_GOOD_SANTA_BAGGAGE, GL_LINEAR, GL_CLAMP_TO_EDGE);

#ifdef PJH_ADD_PANDA_CHANGERING	
	LoadBitmap("Item\\Texture\\pandabody_R.jpg", BITMAP_PANDABODY_R, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR
#endif //PJH_ADD_PANDA_CHANGERING

	LoadBitmap("Monster\\DGicewalker_body.jpg", BITMAP_DOPPELGANGER_ICEWALKER0, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR
	LoadBitmap("Monster\\DGicewalker_R.jpg", BITMAP_DOPPELGANGER_ICEWALKER1, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR
	LoadBitmap("Monster\\Snake1.jpg", BITMAP_DOPPELGANGER_SNAKE01, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR

	LoadBitmap("NPC\\goldboit.jpg", BITMAP_DOPPELGANGER_GOLDENBOX1, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR
	LoadBitmap("NPC\\goldline.jpg", BITMAP_DOPPELGANGER_GOLDENBOX2, GL_LINEAR, GL_LINEAR);	// OPENGL_ERROR

//BITMAP_LIGHT_RED
	LoadBitmap("effect\\flare01_red.jpg", BITMAP_LIGHT_RED, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("effect\\gra.jpg", BITMAP_GRA, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("effect\\ring_of_gradation.jpg", BITMAP_RING_OF_GRADATION, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Interface\\InGameShop\\ingame_pack_check.tga", BITMAP_IGS_CHECK_BUTTON, GL_LINEAR);
	LoadBitmap("Monster\\AssassinLeader_body_R.jpg", BITMAP_ASSASSIN_EFFECT1, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Item\\Texture\\rareitem_ticket_7_body.jpg", BITMAP_RAREITEM7, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\rareitem_ticket_8_body.jpg", BITMAP_RAREITEM8, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\rareitem_ticket_9_body.jpg", BITMAP_RAREITEM9, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\rareitem_ticket_10_body.jpg", BITMAP_RAREITEM10, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\rareitem_ticket_11_body.jpg", BITMAP_RAREITEM11, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\rareitem_ticket_12_body.jpg", BITMAP_RAREITEM12, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Item\\Texture\\DoppelCard.jpg", BITMAP_DOPPLEGANGGER_FREETICKET, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\BarcaCard.jpg", BITMAP_BARCA_FREETICKET, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\Barca7Card.jpg", BITMAP_BARCA7TH_FREETICKET, GL_LINEAR, GL_CLAMP_TO_EDGE);


	LoadBitmap("Item\\Texture\\ork_cham_R.jpg", BITMAP_ORK_CHAM_LAYER_R, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Item\\Texture\\goldenork_cham_R.jpg", BITMAP_GOLDEN_ORK_CHAM_LAYER_R, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Monster\\BoneSE.jpg", BITMAP_BONE_SCORPION_SKIN_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\KryptaBall2.jpg", BITMAP_KRYPTA_BALL_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\bora_golem_effect.jpg", BITMAP_CONDRA_SKIN_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\bora_golem2_effect.jpg", BITMAP_CONDRA_SKIN_EFFECT2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\king_golem01_effect.jpg", BITMAP_NARCONDRA_SKIN_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\king_golem02_effect.jpg", BITMAP_NARCONDRA_SKIN_EFFECT2, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Monster\\king_golem03_effect.jpg", BITMAP_NARCONDRA_SKIN_EFFECT3, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("NPC\\voloE.jpg", BITMAP_VOLO_SKIN_EFFECT, GL_LINEAR, GL_CLAMP_TO_EDGE);



	LoadBitmap("Item\\Texture\\black_gold_crom05.jpg", BITMAP_black_gold_crom05, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\absolute02_swordrender.jpg", BITMAP_absolute02_swordrender, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\black_gold_crom03.jpg", BITMAP_black_gold_crom03, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\flame_chrome.jpg", BITMAP_flame_chrome, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\ground_wind2.jpg", BITMAP_ground_wind2, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\alicewing4db_render.jpg", BITMAP_alicewing4db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\elfwing4da_render01.jpg", BITMAP_elfwing4da_render01, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\elfwing4da_render02.jpg", BITMAP_elfwing4da_render02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\magicianwing4da_render.jpg", BITMAP_magicianwing4da_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\dknightwing4db_render.jpg", BITMAP_dknightwing4db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\line_of_big_R.jpg", BITMAP_line_of_big_R, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\soulknightswordB_render.jpg", BITMAP_soulknightswordB_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\soulknightswordA_render.jpg", BITMAP_soulknightswordA_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\soulmagicswordA_render.jpg", BITMAP_soulmagicswordA_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\hs_soularcherbowA01_render.jpg", BITMAP_hs_soularcherbowA01_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_SoulWizardStaff01_render.jpg", BITMAP_sd_SoulWizardStaff01_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_soulsummonerstickA02_render.jpg", BITMAP_sd_soulsummonerstickA02_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\soulfighterclawA_render.jpg", BITMAP_soulfighterclawA_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\hs_blueeyeknightA03r.jpg", BITMAP_hs_blueeyeknightA03r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\black_gold_crom04_mono.jpg", BITMAP_black_gold_crom04_mono, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\hs_blueeyeknightA02_render.jpg", BITMAP_hs_blueeyeknightA02_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\fire_15frame_mono.jpg", BITMAP_fire_15frame_mono, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\alpha_line2.jpg", BITMAP_alpha_line2, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\blueeyemagicsword02r.jpg", BITMAP_blueeyemagicsword02r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\blueeyemagicsword03r.jpg", BITMAP_blueeyemagicsword03r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\black_gold_crom04.jpg", BITMAP_black_gold_crom04, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\blueeyesceptre_render.jpg", BITMAP_blueeyesceptre_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\Blue_eyebow02r.jpg", BITMAP_Blue_eyebow02r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\blue_eye_ws02_render.jpg", BITMAP_blue_eye_ws02_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\bsummoners02_render.jpg", BITMAP_bsummoners02_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\Blueeyeclaw_render.jpg", BITMAP_Blueeyeclaw_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\mace_mastr02s_render.jpg", BITMAP_mace_mastr02s_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\Darkangel_StaffA_render.jpg", BITMAP_Darkangel_StaffA_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_soullance01_render.jpg", BITMAP_sd_soullance01_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\runewing3db_render.jpg", BITMAP_runewing3db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\runewing4db_render.jpg", BITMAP_runewing4db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\runewing4dc_star01.jpg", BITMAP_runewing4dc_star01, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\runewing4dc_star02.jpg", BITMAP_runewing4dc_star02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\hs_blueeyelanceA02r.jpg", BITMAP_hs_blueeyelanceA02r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\MMsSs2_r.jpg", BITMAP_MMsSs2_r, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\mace5master02_render.jpg", BITMAP_mace5master02_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\wings_van_render.jpg", BITMAP_wings_van_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\benzia_wing4_render.jpg", BITMAP_benzia_wing4_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\benzia_wing4a_render.jpg", BITMAP_benzia_wing4a_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\benzia_wing4b_render.jpg", BITMAP_benzia_wing4b_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\season_wing_period_render.jpg", BITMAP_season_wing_period_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\firek_sq_mono.jpg", BITMAP_firek_sq_mono, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\0120.jpg", BITMAP_0120, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\0143.jpg", BITMAP_0143, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\ChaWing_renderbright.jpg", BITMAP_ChaWing_renderbright, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\KCWWingP_Render.jpg", BITMAP_KCWWingP_Render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\KCWWingR_Render.jpg", BITMAP_KCWWingR_Render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\BoodAnelsword_render.jpg", BITMAP_BoodAnelsword_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\BAGsword_render.jpg", BITMAP_BAGsword_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\bloodclaw2_render.jpg", BITMAP_bloodclaw2_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\booldangel_sapterr.jpg", BITMAP_booldangel_sapterr, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\blood_bow_render.jpg", BITMAP_blood_bow_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\bloodangelstaff01_render.jpg", BITMAP_bloodangelstaff01_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\bloodangelstaff02.jpg", BITMAP_bloodangelstaff02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\bloodangelstick01_render.jpg", BITMAP_bloodangelstick01_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\black_gold_crom02.jpg", BITMAP_black_gold_crom02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Effect\\black_gold_crom02_mono.jpg", BITMAP_black_gold_crom02_mono, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\AngelicBow_overlay.jpg", BITMAP_AngelicBow_overlay, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_wing3C_render.jpg", BITMAP_sd_wing3C_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_wing3B_render.jpg", BITMAP_sd_wing3B_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\K_StylizedMoon_E02.jpg", BITMAP_K_StylizedMoon_E02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\K_StylizedMoon_SwordB02_Render01.jpg", BITMAP_K_StylizedMoon_SwordB02_Render01, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\K_StylizedMoon_SwordB02_Render02.jpg", BITMAP_K_StylizedMoon_SwordB02_Render02, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_wing4C_Red_render.jpg", BITMAP_sd_wing4C_Red_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\sd_wing4D_render.jpg", BITMAP_sd_wing4D_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\growlancerwing4db_render.jpg", BITMAP_growlancerwing4db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\gra.jpg", BITMAP_gra, GL_LINEAR, GL_CLAMP_TO_EDGE, false, false);
	LoadBitmap("Item\\Texture\\gra2.jpg", BITMAP_gra2, GL_LINEAR, GL_CLAMP_TO_EDGE, false, false);
	LoadBitmap("Item\\Texture\\gra3.jpg", BITMAP_gra3, GL_LINEAR, GL_CLAMP_TO_EDGE, false, false);

	LoadBitmap("Item\\Texture\\admangto.tga", BITMAP_admangto, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\deathha.tga", BITMAP_deathha, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\deathjung.tga", BITMAP_deathjung, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\darklordwing4de.tga", BITMAP_darklordwing4de, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\darklordwing4de_render.jpg", BITMAP_darklordwing4de_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\darklordwing4dd.tga", BITMAP_darklordwing4dd, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\darklordwing4dd_render.JPG", BITMAP_darklordwing4dd_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\ragefighterwing4db.TGA", BITMAP_ragefighterwing4db, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\ragefighterwing4db_render.JPG", BITMAP_ragefighterwing4db_render, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\season_wing_periodc.tga", BITMAP_season_wing_periodc, GL_LINEAR, GL_REPEAT, false, false);
	LoadBitmap("Item\\Texture\\season_wing_periodc2.tga", BITMAP_season_wing_periodc2, GL_LINEAR, GL_REPEAT, false, false);




	g_ErrorReport.Write("> First Load Files OK.\r\n");

	OpenPlayers();
	rUIMng.RenderTitleSceneUI(hDC, 2, 12);

	OpenPlayerTextures();
	rUIMng.RenderTitleSceneUI(hDC, 3, 12);

	OpenItems();
	rUIMng.RenderTitleSceneUI(hDC, 4, 12);

	OpenItemTextures();
	rUIMng.RenderTitleSceneUI(hDC, 5, 12);

	GMProtect->LoadingProgressive();
	rUIMng.RenderTitleSceneUI(hDC, 6, 12);

	OpenSkills();
	rUIMng.RenderTitleSceneUI(hDC, 7, 12);

	OpenImages();
	rUIMng.RenderTitleSceneUI(hDC, 8, 12);

	OpenSounds();
	rUIMng.RenderTitleSceneUI(hDC, 9, 12);

	g_ServerListManager->LoadServerListScript();

#if MAIN_UPDATE > 303
#ifdef CHINESE_LANGUAGE
	g_MixRecipeMgr.OpenRecipeFile("Data\\Local\\Mix.bmd");

	OpenDialogFile("Data\\Local\\Dialog.bmd");

	GMItemMng->OpenFile("Data\\Local\\Item.bmd");

	SEASON3B::CMoveCommandData::OpenMoveReqScript("Data\\Local\\movereq.bmd");

	OpenMonsterScript("Data\\Local\\NpcName.txt");

	g_csQuest.OpenQuestScript("Data\\Local\\Quest.bmd");

	OpenSkillScript("Data\\Local\\Skill.bmd");

	g_SocketItemMgr.OpenSocketItemScript("Data\\Local\\SocketItem.bmd");

	OpenTextData();

	g_csItemOption.OpenItemSetScript(false);

	g_QuestMng.LoadQuestScript();

	OpenGateScript("Data\\Gate.bmd");

	OpenFilterFile("Data\\Local\\Filter.bmd");

	OpenNameFilterFile("Data\\Local\\FilterName.bmd");

	OpenMonsterSkillScript("Data\\Local\\MonsterSkill.bmd");

	g_pMasterSkillTree->OpenMasterSkillTreeData("Data\\Local\\MasterSkillTreeData.bmd");

	g_pMasterSkillTree->OpenMasterSkillTooltip("Data\\Local\\MasterSkillTooltip.bmd");
#else
	char Text[100];

	g_MixRecipeMgr.OpenRecipeFile("Data\\Local\\Mix.bmd");

	sprintf(Text, "Data\\Local\\%s\\Dialog.bmd", g_strSelectedML.c_str());
	OpenDialogFile(Text);

	sprintf(Text, "Data\\Local\\%s\\Item.bmd", g_strSelectedML.c_str());
	GMItemMng->OpenFile(Text);

	sprintf(Text, "Data\\Local\\%s\\movereq.bmd", g_strSelectedML.c_str());
	SEASON3B::CMoveCommandData::OpenMoveReqScript(Text);

	sprintf(Text, "Data\\Local\\%s\\NpcName.txt", g_strSelectedML.c_str());
	OpenMonsterScript(Text);

	sprintf(Text, "Data\\Local\\%s\\Quest.bmd", g_strSelectedML.c_str());
	g_csQuest.OpenQuestScript(Text);

	sprintf(Text, "Data\\Local\\%s\\Skill.bmd", g_strSelectedML.c_str());
	OpenSkillScript(Text);

	sprintf(Text, "Data\\Local\\%s\\SocketItem.bmd", g_strSelectedML.c_str());
	g_SocketItemMgr.OpenSocketItemScript(Text);

	OpenTextData();

	g_csItemOption.OpenItemSetScript(false);

	g_QuestMng.LoadQuestScript();

	OpenGateScript("Data\\Gate.bmd");

	OpenFilterFile("Data\\Local\\Filter.bmd");

	OpenNameFilterFile("Data\\Local\\FilterName.bmd");

	OpenMonsterSkillScript("Data\\Local\\MonsterSkill.bmd");

	g_pMasterSkillTree->OpenMasterSkillTreeData("Data\\Local\\MasterSkillTreeData.bmd");

	sprintf(Text, "Data\\Local\\%s\\MasterSkillTooltip.bmd", g_strSelectedML.c_str());

	g_pMasterSkillTree->OpenMasterSkillTooltip(Text);
#endif // CHINESE_LANGUAGE
#else
	g_MixRecipeMgr.OpenRecipeFile("Data\\Local\\Mix.bmd");

	OpenDialogFile("Data\\Local\\Dialog.bmd");

	GMItemMng->OpenFile("Data\\Local\\Item.bmd");

	SEASON3B::CMoveCommandData::OpenMoveReqScript("Data\\Local\\movereq.bmd");

	OpenMonsterScript("Data\\Local\\NpcName.txt");

	g_csQuest.OpenQuestScript("Data\\Local\\Quest.bmd");

	OpenSkillScript("Data\\Local\\Skill.bmd");

	OpenTextData();

	g_csItemOption.OpenItemSetScript(false);

	g_QuestMng.LoadQuestScript();

	OpenGateScript("Data\\Gate.bmd");

	OpenFilterFile("Data\\Local\\Filter.bmd");

	OpenNameFilterFile("Data\\Local\\FilterName.bmd");

	OpenMonsterSkillScript("Data\\Local\\MonsterSkill.bmd");
#endif // MAIN_UPDATE > 303

#ifdef SHUTDOWN_SCALEFORM_INFO
	gfxinit->LoadMovie();
#endif // SHUTDOWN_SCALEFORM_INFO

	rUIMng.RenderTitleSceneUI(hDC, 10, 12);

	LoadWaveFile(SOUND_TITLE01, "Data\\Sound\\iTitle.wav", 1);
	LoadWaveFile(SOUND_MENU01, "Data\\Sound\\iButtonMove.wav", 2);
	LoadWaveFile(SOUND_CLICK01, "Data\\Sound\\iButtonClick.wav", 1);
	LoadWaveFile(SOUND_ERROR01, "Data\\Sound\\iButtonError.wav", 1);
	LoadWaveFile(SOUND_INTERFACE01, "Data\\Sound\\iCreateWindow.wav", 1);

	LoadWaveFile(SOUND_REPAIR, "Data\\Sound\\iRepair.wav", 1);
	LoadWaveFile(SOUND_WHISPER, "Data\\Sound\\iWhisper.wav", 1);

	LoadWaveFile(SOUND_FRIEND_CHAT_ALERT, "Data\\Sound\\iFMSGAlert.wav", 1);
	LoadWaveFile(SOUND_FRIEND_MAIL_ALERT, "Data\\Sound\\iFMailAlert.wav", 1);
	LoadWaveFile(SOUND_FRIEND_LOGIN_ALERT, "Data\\Sound\\iFLogInAlert.wav", 1);

	LoadWaveFile(SOUND_RING_EVENT_READY, "Data\\Sound\\iEvent3min.wav", 1);
	LoadWaveFile(SOUND_RING_EVENT_START, "Data\\Sound\\iEventStart.wav", 1);
	LoadWaveFile(SOUND_RING_EVENT_END, "Data\\Sound\\iEventEnd.wav", 1);

	rUIMng.RenderTitleSceneUI(hDC, 11, 12);
}

#if MAIN_UPDATE > 303
void OpenTextData()
{
	char Text[100];

#ifdef CHINESE_LANGUAGE
	sprintf(Text, "Data\\Local\\Text.bmd");
#else
	sprintf(Text, "Data\\Local\\%s\\Text.bmd", g_strSelectedML.c_str());
#endif // CHINESE_LANGUAGE

	GlobalText.Load(Text, CGlobalText::LD_USA_CANADA_TEXTS | CGlobalText::LD_FOREIGN_TEXTS);

#ifdef CHINESE_LANGUAGE
	sprintf(Text, "Data\\Local\\GlobalText.bmd");
#else
	sprintf(Text, "Data\\Local\\%s\\GlobalText.bmd", g_strSelectedML.c_str());
#endif // CHINESE_LANGUAGE
	TextGlobal.Load(Text, CGlobalText::LD_USA_CANADA_TEXTS | CGlobalText::LD_FOREIGN_TEXTS);

	OpenMacro("Data\\Macro.txt");
}
#else
void OpenTextData()
{
	GlobalText.Load("Data\\Local\\Text.bmd", CGlobalText::LD_USA_CANADA_TEXTS | CGlobalText::LD_FOREIGN_TEXTS);
	TextGlobal.Load("Data\\Local\\GlobalText.bmd", CGlobalText::LD_USA_CANADA_TEXTS | CGlobalText::LD_FOREIGN_TEXTS);
	OpenMacro("Data\\Macro.txt");
}
#endif // MAIN_UPDATE > 303

void ReleaseMainData()
{
	gMapManager->DeleteObjects();
	DeleteNpcs();
	DeleteMonsters();
	ClearItems();
	ClearCharacters();
}
