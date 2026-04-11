#include "stdafx.h"
#include "ZzzEffect.h"
#include "GMLogInScene.h"

CGMLogInScenePtr CGMLogInScene::Make()
{
	CGMLogInScenePtr field(new CGMLogInScene);
	return field;
}

CGMLogInScene::CGMLogInScene()
{
}

CGMLogInScene::~CGMLogInScene()
{
}

void CGMLogInScene::OpenWorld()
{
}

bool CGMLogInScene::MoveObject(OBJECT* pObject)
{
	int sIndex = (pObject->Type);

	if (sIndex == 1)
	{
		pObject->BlendMeshTexCoordV -= 0.005000000;
		return true;
	}
	else if (sIndex == 13)
	{
		//pObject->BlendMeshTexCoordU = sub_DF5FDA(0.2000000, 0.6000000, 0.001000000);
	}
	return false;
}

bool CGMLogInScene::CreateObject(OBJECT* pObject)
{
	int index = (pObject->Type);

	if (index == 5)
	{
		BMD* pModel = gmClientModels->GetModel(pObject->Type);
		if (pModel)
		{
			pModel->Actions[0].PlaySpeed = 0.23;
		}
		return true;
	}
	else if (index == 38 || index == 54)
	{
		pObject->HiddenMesh = -2;
		return true;
	}
	else
	{
		pObject->CollisionRange = -428.5714;
	}
	return true;
}

bool CGMLogInScene::RenderObjectVisual(OBJECT* pObject, BMD* pModel)
{
	int sIndex = (pObject->Type);

	float Scale;
	vec3_t Light, vRelative, Position;

	switch (sIndex)
	{
	case 5:
	{
		Vector(1.000000, 0.3000000, 0.0, Light);

		float v31 = (float)((float)(rand() % 60) / 20.00000) - 1.500000;
		float v32 = (float)((float)(rand() % 10) / 20.00000) + 1.400000;

		Vector(0.0, 0.0, v31, vRelative);
		pModel->TransformPosition(BoneTransform[37], vRelative, Position, false);
		CreateSprite(BITMAP_SHINY + 1, Position, v32, Light, pObject, 0.0, 0);
		Scale = v32 - 0.3000000;
		CreateSprite(BITMAP_SHINY + 1, Position, Scale, Light, pObject, 20.00000, 0);

		v32 = (float)((float)(rand() % 10) / 20.00000) + 1.000000;
		Scale = v32 + 0.3000000;
		CreateSprite(BITMAP_LIGHT, Position, Scale, Light, pObject, 0.0, 0);

		v31 = (float)((float)(rand() % 40) / 20.00000) - 1.000000;
		v32 = (float)((float)(rand() % 8) / 20.00000) + 0.4000000;

		Vector(0.0, 0.0, v31, vRelative);
		pModel->TransformPosition(BoneTransform[37], vRelative, Position, false);
		CreateSprite(BITMAP_SHINY + 1, Position, v32, Light, pObject, 0.0, 4);
		Scale = v32 - 0.2000000;
		CreateSprite(BITMAP_SHINY + 1, Position, Scale, Light, pObject, 90.00000, 0);

		v31 = (float)((float)(rand() % 60) / 20.00000) + 2.800000;
		v32 = (float)((float)(rand() % 10) / 20.00000) + 2.000000;
		Vector(0.0, 0.0, v31, vRelative);
		pModel->TransformPosition(BoneTransform[98], vRelative, Position, false);
		CreateSprite(BITMAP_SHINY + 1, Position, v32, Light, pObject, 0.0, 0);
		Scale = v32 - 0.3000000;
		CreateSprite(BITMAP_SHINY + 1, Position, Scale, Light, pObject, 0.0, 0);

		v31 = (float)((float)(rand() % 60) / 20.00000) - 1.500000;
		v32 = (float)((float)(rand() % 10) / 20.00000) + 1.000000;

		Scale = v32 + 0.3000000;
		Vector(0.0, 0.0, v31, vRelative);
		pModel->TransformPosition(BoneTransform[98], vRelative, Position, false);
		CreateSprite(BITMAP_LIGHT, Position, Scale, Light, pObject, 0.0, 0);
	}
	break;
	case 38:
		Vector(0.02000000, 0.02000000, 0.05000000, Light);
		if (!(rand() % 6))
		{
			CreateParticleSync(BITMAP_CLOUD, pObject->Position, pObject->Angle, Light, 20, pObject->Scale, NULL);
		}
		break;
	case 54:
		Vector(0.2200000, 0.1500000, 0.1500000, Light);
		if (!(rand() % 2))
		{
			CreateParticleSync(BITMAP_WATERFALL_1, pObject->Position, pObject->Angle, Light, 2, pObject->Scale, NULL);
		}
		break;
	default:
		return false;
	}
	return true;
}

bool CGMLogInScene::RenderObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon)
{
	int sIndex = (pObject->Type);

	if (sIndex > 13)
	{
		if (sIndex >= 18 && (sIndex <= 20 || sIndex == 29 || sIndex > 54 && sIndex <= 56))
			goto LABEL_17;
		return false;
	}
	if (sIndex == 13)
	{
	LABEL_18:
		pModel->RenderBody(RENDER_TEXTURE, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, pObject->HiddenMesh, -1);
		pObject->m_bRenderAfterCharacter = 1;
		return true;
	}
	if (sIndex <= 0)
		return 0;
	if (sIndex > 3)
	{
		if (sIndex != 5)
		{
			if (sIndex > 6 && (sIndex <= 8 || sIndex > 9))
				goto LABEL_17;
			return false;
		}
		goto LABEL_18;
	}
LABEL_17:
	pModel->RenderBody(RENDER_TEXTURE, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, pObject->HiddenMesh, -1);
	pObject->m_bRenderAfterCharacter = false;
	return true;
}

void CGMLogInScene::RenderAfterObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon)
{
	int sIndex = (pObject->Type);

	if (sIndex == 5)
	{
		pModel->BeginRender(pObject->Alpha);
		pModel->StreamMesh = 0;
		Vector(0.8000000, 0.7000000, 0.3000000, pModel->BodyLight);

		pModel->RenderMesh(0, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(23, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(23, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(24, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(24, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(25, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(25, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(26, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(26, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(27, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(27, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->RenderMesh(28, 66, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
		pModel->RenderMesh(28, 68, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, BITMAP_CHROME);
		pModel->StreamMesh = -1;
		pModel->EndRender();
	}
	if (sIndex == 13)
	{
		pModel->RenderMesh(22, RENDER_TEXTURE, pObject->Alpha, -2, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);

	}
}
