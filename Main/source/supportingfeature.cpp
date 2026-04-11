#include "stdafx.h"
#include "GlobalBitmap.h"
#include "supportingfeature.h"

void CUSTOM_ITEM_INFO::release()
{
	Itemindex = -1;
	filename[0] = '\0';
	RenderColor = -1;
};

void CUSTOM_MONSTER_INFO::release()
{
	monsterIndex = -1;
	filename[0] = '\0';
	directory[0] = '\0';
}

void CUSTOM_NPC_NAME::release()
{
	IndexNpc = -1;
	Name[0] = '\0';
	PositionX = -1;
	PositionY = -1;
	WorldIndex = -1;
}

void CUSTOM_MAPE_INFO::release()
{
	mapIndex = -1;
	checkIndex = -1;
	ImgName[0] = '\0';
	lpszMp3[0] = '\0';
}

void CUSTOM_IMAGE_INFO::release()
{
	ImgIndex = -1;
	ImgName[0] = '\0';
}

void CUSTOM_WING_INFO::release()
{
	ItemIndex = -1;
	filename[0] = '\0';
}

void CUSTOM_ITEM_STACK::release()
{
	ItemIndex = -1;
	MaxStack = 0;
}

void CUSTOM_SMOKE_STACK::release()
{
	ItemIndex = -1;
	EffectColor = -1;
}

void CUSTOM_PET_STACK::release()
{
	ItemIndex = -1;
	RenderType = -1;
	MovementType = 0;
}

void CUSTOM_EFFECT_STACK::release()
{
	ItemIndex = -1;
	EffectColor = -1;
}

bool CUSTOM_ITEM_INFO::isUsable()
{
	return (Itemindex != -1 && strlen(filename) != 0);
}

bool CUSTOM_MONSTER_INFO::isUsable()
{
	return (monsterIndex != -1 && strlen(filename) != 0);
}

bool CUSTOM_NPC_NAME::isUsable()
{
	return (IndexNpc != -1 && Name[0] != '\0');
}

bool CUSTOM_MAPE_INFO::isUsable()
{
	return false;
}

bool CUSTOM_IMAGE_INFO::isUsable()
{
	return (ImgIndex != -1 && strlen(ImgName) != 0);
}

bool CUSTOM_WING_INFO::isUsable()
{
	return (ItemIndex != -1 && strlen(filename) != 0);
}

bool CUSTOM_ITEM_STACK::isUsable()
{
	return (ItemIndex != -1) && MaxStack > 0;
}

bool CUSTOM_SMOKE_STACK::isUsable()
{
	return (ItemIndex != -1);
}

bool CUSTOM_PET_STACK::isUsable()
{
	return (ItemIndex != -1 && strlen(filename) != 0);
}

bool CUSTOM_EFFECT_STACK::isUsable()
{
	return (ItemIndex != -1);
}

void CUSTOM_ITEM_INFO::OpenLoad()
{
	if (isUsable())
	{
		int modelId = Itemindex + MODEL_ITEM;

		if (Itemindex >= ITEM_HELM && Itemindex < ITEM_WING)
		{
			gmClientModels->AccessModel(modelId, "Data\\Player\\", filename);
		}
		else
		{
			gmClientModels->AccessModel(modelId, "Data\\Item\\", filename);
		}
		gmClientModels->OpenTexture(modelId, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}
}

void CUSTOM_WING_INFO::OpenLoad()
{
	if (isUsable())
	{
		int modelId = ItemIndex + MODEL_ITEM;

		gmClientModels->AccessModel(modelId, "Data\\Item\\", filename);

		gmClientModels->OpenTexture(modelId, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}
}

void CUSTOM_PET_STACK::OpenLoad()
{
	if (isUsable())
	{
		int modelId = ItemIndex + MODEL_ITEM;

		gmClientModels->AccessModel(modelId, "Data\\Item\\", filename);

		gmClientModels->OpenTexture(modelId, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}
}

void CUSTOM_ITEM_INFO::OpenBattle(int modelId, bool Left)
{
	if (isUsable())
	{
		char strText[MAX_PATH];

		if(Left == true)
			unicode::_sprintf(strText, "%s_L", filename);
		else
			unicode::_sprintf(strText, "%s_R", filename);

		gmClientModels->AccessModel(modelId, "Data\\Item\\", strText);

		gmClientModels->OpenTexture(modelId, "Item\\Texture\\", GL_REPEAT, GL_NEAREST);
	}
}

void CUSTOM_MONSTER_INFO::OpenLoad()
{
	if (isUsable())
	{
		char Pathdirectory[MAX_PATH];
		sprintf_s(Pathdirectory, "Data\\%s", directory);

		int modelIndex = (monsterIndex - 579);

		if (modelIndex >= 0)
		{
			RenderIndex = modelIndex + MODEL_MONSTER_DUMY;

			gmClientModels->AccessModel(RenderIndex, Pathdirectory, filename);

			gmClientModels->OpenTexture(RenderIndex, directory, GL_REPEAT, GL_NEAREST);

			BMD* pModel = gmClientModels->GetModel(RenderIndex);

			if (pModel && pModel->Actions)
			{
				if (Kind == KIND_MONSTER)
				{
					pModel->Actions[MONSTER01_STOP1].PlaySpeed = 0.25f;
					pModel->Actions[MONSTER01_STOP2].PlaySpeed = 0.2f;
					pModel->Actions[MONSTER01_WALK].PlaySpeed = 0.34f;
					pModel->Actions[MONSTER01_ATTACK1].PlaySpeed = 0.33f;
					pModel->Actions[MONSTER01_ATTACK2].PlaySpeed = 0.33f;
					pModel->Actions[MONSTER01_SHOCK].PlaySpeed = 0.5f;
					pModel->Actions[MONSTER01_DIE].PlaySpeed = 0.55f;
					pModel->Actions[MONSTER01_DIE].Loop = true;
				}
				else
				{
					for (int i = 0; i < pModel->NumActions; i++)
					{
						pModel->Actions[i].PlaySpeed = 0.25f;
					}
				}
			}
		}
	}
}

#include "ZzzTexture.h"
#include "Utilities/Log/muConsoleDebug.h"

void CUSTOM_IMAGE_INFO::OpenLoad()
{
	if (isUsable())
	{
		char Pathdirectory[MAX_PATH];
		sprintf_s(Pathdirectory, "Item\\Texture\\%s", ImgName);
		LoadBitmap(Pathdirectory, ImgIndex, Wrap, Type, true, false);
	}
}
