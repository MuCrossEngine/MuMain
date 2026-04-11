#include "stdafx.h"
#include "MapManager.h"
#include "ZzzEffect.h"
#include "GMCharacterScene.h"

CGMCharacterScenePtr CGMCharacterScene::Make()
{
	CGMCharacterScenePtr pkfield(new CGMCharacterScene);

	return pkfield;
}

CGMCharacterScene::CGMCharacterScene()
{
}

CGMCharacterScene::~CGMCharacterScene()
{
}

void CGMCharacterScene::OpenWorld()
{
}

bool CGMCharacterScene::MoveObject(OBJECT* pObject)
{
	int Index = (pObject->Type);

	switch (Index)
	{
	case 26:
	case 25:
		pObject->BlendMeshTexCoordV -= timefac(0.03000000);
		return true;
	case 23:
	case 24:
		pObject->BlendMeshTexCoordV -= timefac(0.02000000);
		return true;
	case 22:
		pObject->BlendMeshTexCoordV -= timefac(0.01000000);
		return true;
	case 78:
		pObject->BlendMeshTexCoordU += timefac(0.01000000);
		return true;
	}

	return false;
}

bool CGMCharacterScene::CreateObject(OBJECT* pObject)
{
	int Index = (pObject->Type);

	switch (Index)
	{
		//case 38:
	case 14:
	case 15:
	case 54:
	case 55:
	case 56:
	case 57:
	case 69:
	case 80:
		pObject->HiddenMesh = -2;
		return true;
	}

	return false;
}

bool CGMCharacterScene::RenderObjectVisual(OBJECT* pObject, BMD* pModel)
{
	int Index = (pObject->Type);

	float Scale;
	vec3_t Light, Position;

	switch (Index)
	{
	case 31:
		vec3_t vPos;
		Vector(0.0, 0.0, 0.0, vPos);
		Vector(0.0, 0.0, 0.0, Position);

		Scale = 0.20000000;
		Vector(0.8000000, 0.9400000, 0.9400000, Light);
		pModel->TransformPosition(BoneTransform[72], vPos, Position, false);
		CreateSprite(BITMAP_SPARK + 1, Position, Scale, Light, pObject, 0, 0);

		Scale = 1.2000000;
		Vector(0.7800000, 0.4400000, 0.7500000, Light);
		pModel->TransformPosition(BoneTransform[84], vPos, Position, false);
		CreateSprite(BITMAP_SPARK + 1, Position, Scale, Light, pObject, 0, 0);

		Scale = 0.8000000;
		Vector(0.5800000, 0.0700000, 0.5600000, Light);
		pModel->TransformPosition(BoneTransform[96], vPos, Position, false);
		CreateSprite(BITMAP_SPARK + 1, Position, Scale, Light, pObject, 0, 0);
		return true;
	case 38:
		if (!(rand() % 6))
		{
			Vector(0.02000000, 0.02000000, 0.05000000, Light);
			CreateParticleSync(BITMAP_CLOUD, pObject->Position, pObject->Angle, Light, 20, pObject->Scale, pObject);
		}
		return true;
	case 54:
		if (!(rand() % 2))
		{
			Scale = pObject->Scale;
			Vector(1.0, 1.0, 1.0, Light);
			CreateParticleSync(BITMAP_WATERFALL_1, pObject->Position, pObject->Angle, Light, 2, Scale, NULL);
		}
		return true;
	case 55:
		Scale = pObject->Scale * 2.000000;
		Vector(1.0, 1.0, 1.0, Light);
		CreateParticleSync(BITMAP_WATERFALL_3, pObject->Position, pObject->Angle, Light, 14, Scale, NULL);
		return true;
	case 56:
		if (!(rand() % 3))
		{
			Scale = pObject->Scale;
			Vector(1.0, 1.0, 1.0, Light);
			CreateParticleSync(BITMAP_WATERFALL_2, pObject->Position, pObject->Angle, Light, 2, Scale, NULL);
		}
		return true;
	case 69:
		if (!(rand() % 3))
		{
			Scale = 0.2f;
			Vector(0.4, 0.6, 0.7, Light);
			CreateParticleSync(BITMAP_TWINTAIL_WATER, pObject->Position, pObject->Angle, Light, 1, Scale, NULL);
		}
		return true;
	case 80:
		if (!(rand() % 8))
		{
			Scale = pObject->Scale;
			Vector(1.0, 1.0, 1.0, Light);
			CreateParticleSync(BITMAP_SMOKE, pObject->Position, pObject->Angle, Light, 69, Scale, NULL);
		}
		return true;
	}
	return false;
}

bool CGMCharacterScene::RenderObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon)
{
	int Index = (pObject->Type);

	if (Index <= 33)
	{
		if (Index >= 14 && Index < 17)
			return true;

		if (Index != 33 && Index != 4 && (Index <= 6 || Index > 8 && Index != 12 && (Index <= 23 || Index > 26 && Index != 31)))
			return false;
		goto LABEL_19;
	}

	if (Index == 36)
	{
	LABEL_19:
		pObject->m_bRenderAfterCharacter = true;
		return 0;
	}
	if (Index != 60)
	{
		if (Index != 61 && Index != 71)
			return false;
		goto LABEL_19;
	}
	pModel->RenderMesh(1, RENDER_TEXTURE, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
	pObject->m_bRenderAfterCharacter = true;
	return true;
}

void CGMCharacterScene::RenderAfterObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon)
{
	int Index = (pObject->Type);

	if (Index <= 33)
	{
		if (Index == 17)
		{
			return;
		}
		if (Index != 33 && Index != 4 && (Index <= 6 || Index > 8 && Index != 12 && (Index <= 23 || Index > 26 && Index != 31)))
		{
			return;
		}
	LABEL_18:
		pModel->RenderBody(RENDER_TEXTURE, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, pObject->HiddenMesh, -1);
		return;
	}
	switch (Index)
	{
	case 36:
		goto LABEL_18;
	case 60:
		pModel->RenderMesh(0, RENDER_TEXTURE, pObject->Alpha, -2, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		break;
	case 61:
	case 71:
		goto LABEL_18;
	}
}
