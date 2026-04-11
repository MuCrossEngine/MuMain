#include "stdafx.h"
#include "CGMProtect.h"
#include "ZzzCharacter.h"
#include "supportingfeature.h"
#include "CGMMonsterMng.h"

CGMMonsterMng::CGMMonsterMng()
{
}

CGMMonsterMng::~CGMMonsterMng()
{
	UniqueMonsterData.clear();
}

CGMMonsterMng* CGMMonsterMng::Instance()
{
	static CGMMonsterMng sInstance;
	return &sInstance;
}

void CGMMonsterMng::LoadDataMonster(CUSTOM_MONSTER_INFO* info, int Size)
{
	for (int i = 0; i < Size; i++)
	{
		if (info[i].isUsable() == true)
		{
			UniqueMonsterData.push_back(info[i]);
		}
	}
}

void CGMMonsterMng::LoadModelMonster()
{
	for (size_t i = 0; i < UniqueMonsterData.size(); i++)
	{
		UniqueMonsterData[i].OpenLoad();
	}
}

bool CGMMonsterMng::IsMonsterByIndex(int monsterIndex)
{
	for (size_t i = 0; i < UniqueMonsterData.size(); i++)
	{
		if (UniqueMonsterData[i].monsterIndex == monsterIndex && UniqueMonsterData[i].Kind == KIND_MONSTER)
		{
			return true;
		}
	}
	return false;
}

CUSTOM_MONSTER_INFO* CGMMonsterMng::FindMonsterByIndex(int monsterIndex)
{
	for (size_t i = 0; i < UniqueMonsterData.size(); i++)
	{
		if (UniqueMonsterData[i].monsterIndex == monsterIndex)
		{
			return &UniqueMonsterData[i];
		}
	}
	return NULL;
}

extern int HeroIndex;


CHARACTER* CGMMonsterMng::CreateMonster(int Type, int PositionX, int PositionY, int Key)
{
	CHARACTER* pCharacter = NULL;

	CUSTOM_MONSTER_INFO*  DataMonster = FindMonsterByIndex(Type);

	if (DataMonster != NULL)
	{
		pCharacter = CreateCharacter(Key, DataMonster->RenderIndex, PositionX, PositionY, 0.f);

		if (pCharacter != NULL)
		{
			OBJECT* o = &pCharacter->Object;
			o->Live = true;
			o->ExtState = 0;
			o->Kind = DataMonster->Kind;
			o->Scale = DataMonster->fSize;

			pCharacter->MonsterIndex = Type;
			pCharacter->TargetCharacter = HeroIndex;

			if (DataMonster->Name[0] != '\0')
			{
				strncpy(pCharacter->ID, DataMonster->Name, sizeof(DataMonster->Name));
			}

			if (o->Kind == KIND_NPC)
			{
				GMProtect->GetNpcName(Type, pCharacter->ID, PositionX, PositionY);
			}
		}
	}

	return pCharacter;
}
