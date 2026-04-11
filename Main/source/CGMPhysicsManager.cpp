#include "stdafx.h"
#include "PhysicsManager.h"
#include "CGMPhysicsManager.h"

CGMPhysicsManager::CGMPhysicsManager()
{
	TextureIndex = BITMAP_PHYSICS_TEXTURES_BEGIN;
}

CGMPhysicsManager::~CGMPhysicsManager()
{
	this->physics_data.clear();
}

void CGMPhysicsManager::ReadData(FILE* fp)
{
	readVector(fp, this->physics_data);
}

size_t CGMPhysicsManager::Size()
{
	return this->physics_data.size();
}

int CGMPhysicsManager::GenerateTextureIndex()
{
	return TextureIndex++;
}

WINGS_PHYSICS_INFO* CGMPhysicsManager::Get(size_t i)
{
	return &this->physics_data[i];
}

const WINGS_PHYSICS_INFO* CGMPhysicsManager::find(int nType)
{
	auto it = std::find_if(this->physics_data.begin(), this->physics_data.end(), [&nType](const WINGS_PHYSICS_INFO& value) {
		return value.ItemIndex == nType;
		});

	return (it != this->physics_data.end()) ? &(*it) : NULL;
}

void CGMPhysicsManager::LoadImagenData(WORD& textureIndex, const std::string& filename)
{
	textureIndex = this->GenerateTextureIndex();

	std::string textureName = "Item\\Texture\\" + filename;
	LoadBitmap(textureName.c_str(), textureIndex, GL_LINEAR, GL_REPEAT, true, false);
}

CGMPhysicsManager* CGMPhysicsManager::Instance()
{
	static CGMPhysicsManager s_Instance;
	return &s_Instance;
}

void CGMPhysicsManager::LoadDataTexture()
{
	for (size_t i = 0; i < this->Size(); i++)
	{
		WINGS_PHYSICS_INFO* physicsData = this->Get(i);

		if (physicsData->RenderType == 0)
		{
			if (physicsData->textureLayer[0] != '\0')
			{
				this->LoadImagenData(physicsData->textureIndex[0], physicsData->textureLayer);

				if (physicsData->textureLayer_R[0] != '\0')
					this->LoadImagenData(physicsData->textureIndex[1], physicsData->textureLayer_R);
				else
					physicsData->textureIndex[1] = 0xffff;
			}

			if (physicsData->iBoneStrip != 0)
			{
				if (physicsData->textureStrip[0] == '\0')
				{
					physicsData->iBoneStrip = 0;
				}

				if (physicsData->textureStrip[0] != '\0')
				{
					this->LoadImagenData(physicsData->textureIndex[2], physicsData->textureStrip);

					if (physicsData->textureStrip_R[0] != '\0')
						this->LoadImagenData(physicsData->textureIndex[3], physicsData->textureStrip_R);
					else
						physicsData->textureIndex[3] = 0xffff;
				}
			}
		}
	}
}

bool CGMPhysicsManager::physics_work_out(int nType, bool modelType)
{
	if (modelType)
		nType -= MODEL_ITEM;

	return (this->find(nType) != NULL);
}

#include "Utilities/Log/muConsoleDebug.h"

CPhysicsCloth* CGMPhysicsManager::physics_work_out(CHARACTER* pCharacter, OBJECT* pObject, int& numCloth)
{
	CPhysicsCloth* pCloth = NULL;
	const WINGS_PHYSICS_INFO* physicsData;

	physicsData = this->find((pCharacter->Wing.Type - MODEL_ITEM));

	if (physicsData)
	{
		BYTE textureLayer = 0;
		BYTE textureStrip = 1;
		BYTE RenderType01 = physicsData->RenderType;
		BYTE byFirstClass = pCharacter->GetBaseClass();

		switch (byFirstClass)
		{
		case Dark_Lord:
			textureLayer = 2;
			textureStrip = 4;
			break;
		case Rage_Fighter:
			textureLayer = 0;
			textureStrip = 1;
			break;
		default:
			textureLayer = 1;
			textureStrip = 2;
			break;
		}

		if (RenderType01 == 0)
		{
			if (physicsData->iBoneStrip)
				numCloth = 4;
			else
				numCloth = 2;
		}
		else if (RenderType01 == 1 || RenderType01 == 4)
		{
			numCloth = 2;
		}
		else if (RenderType01 == 2 || RenderType01 == 3 || RenderType01 == 6)
		{
			numCloth = 4;
		}

		if (numCloth != 0)
		{
			if (byFirstClass == Dark_Lord)
			{
				numCloth += 2;
			}

			pCloth = new CPhysicsCloth[numCloth];

			if (textureLayer < numCloth)
			{
				if (RenderType01 == 1) //-- (admangto.tga)
				{
					pCloth[textureLayer].Create(pObject, 19, 0, 20.f, -10.f, 10, 10, 100.f, 150.f, BITMAP_admangto, BITMAP_admangto, 5125);
					pCloth[textureLayer].AddCollisionSphere(-10.f, 5.f, -25.f, 25.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, 5.f, -25.f, 25.f, 17);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -5.f, -35.f, 28.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -5.f, -35.f, 28.f, 17);
				}
				else if (RenderType01 == 2) //-- (deathha.tga, deathjung.tga)
				{
					pCloth[textureLayer].Create(pObject, 19, 0, 15.f, 5.f, 10, 10, 180.f, 170.f, BITMAP_deathha, BITMAP_deathha, 5125);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -10.f, -10.f, 38.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -10.f, -10.f, 35.f, 17);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -10.f, 20.f, 37.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -10.f, 20.f, 37.f, 17);

					if (textureStrip < (numCloth - 1))
					{
						pCloth[textureStrip].Create(pObject, 19, 30.f, 15.f, 10.f, 2, 5, 12.0f, 200.f, BITMAP_deathjung, BITMAP_deathjung, 4096);
						pCloth[textureStrip].AddCollisionSphere(0.0f, -15.f, -20.f, 30.f, 2);
						pCloth[textureStrip].AddCollisionSphere(0.0f, 0.0f, 0.0f, 35.f, 17);
						textureStrip++;
						pCloth[textureStrip].Create(pObject, 19, -30.f, 20.f, 10.f, 2, 5, 12.0f, 200.f, BITMAP_deathjung, BITMAP_deathjung, 4096);
						pCloth[textureStrip].AddCollisionSphere(0.0f, -15.f, -20.f, 30.f, 2);
						pCloth[textureStrip].AddCollisionSphere(0.0f, 0.0f, 0.0f, 35.f, 17);
					}
				}
				else if (RenderType01 == 3) //-- (darklordwing4de.tga, darklordwing4de_render.jpg, darklordwing4dd.tga, darklordwing4dd_render.JPG)
				{
					pCloth[textureLayer].Create(pObject, 19, 0, 25.f, -10.f, 10, 10, 130.f, 160.f, BITMAP_darklordwing4de, BITMAP_darklordwing4de, 4101);
					pCloth[textureLayer].SetTextureLuminosity(BITMAP_darklordwing4de_render, 3.0, 0.5, 1.0);
					pCloth[textureLayer].AddCollisionSphere(-10.f, 5.f, -25.f, 25.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, 5.f, -25.f, 25.f, 17);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -5.f, -35.f, 28.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -5.f, -35.f, 28.f, 17);

					if (textureStrip < (numCloth - 1))
					{
						pCloth[textureStrip].Create(pObject, 19, 45.f, 34.f, -10.f, 2, 5, 12.0f, 140.f, BITMAP_darklordwing4dd, BITMAP_darklordwing4dd, 4096);
						pCloth[textureStrip].SetTextureLuminosity(BITMAP_darklordwing4dd_render, 1.0, 0.1, 1.0);
						pCloth[textureStrip].AddCollisionSphere(0.0f, -15.f, 10.f, 35.f, 3);
						pCloth[textureStrip].AddCollisionSphere(20.f, 5.f, 10.f, 35.f, 17);
						textureStrip++;
						pCloth[textureStrip].Create(pObject, 19, -45.f, 30.f, -10.f, 2, 5, 12.0f, 140.f, BITMAP_darklordwing4dd, BITMAP_darklordwing4dd, 4096);
						pCloth[textureStrip].SetTextureLuminosity(BITMAP_darklordwing4dd_render, 1.0, 0.1, 1.0);
						pCloth[textureStrip].AddCollisionSphere(0.0f, -15.f, 10.f, 35.f, 3);
						pCloth[textureStrip].AddCollisionSphere(-20.f, 5.f, 10.f, 35.f, 17);
					}
				}
				else if (RenderType01 == 4) //-- (ragefighterwing4db.TGA, ragefighterwing4db_render.JPG)
				{
					pCloth[textureLayer].Create(pObject, 19, 0.f, 28.f, 0.f, 10, 10, 180.f, 150.f, BITMAP_ragefighterwing4db, BITMAP_ragefighterwing4db, 5125);
					pCloth[textureLayer].SetTextureLuminosity(BITMAP_ragefighterwing4db_render, 3.0, 0.4, 1.0);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -10.f, -10.f, 35.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -10.f, -10.f, 35.f, 17);
					pCloth[textureLayer].AddCollisionSphere(-10.f, -10.f, 20.f, 37.f, 17);
					pCloth[textureLayer].AddCollisionSphere(10.f, -10.f, 20.f, 37.f, 17);
				}
				else if (RenderType01 == 5)
				{
				}
				else if (RenderType01 == 6) //-- (season_wing_periodc.tga, season_wing_periodc2.tga)
				{
					pCloth[textureLayer].Create(pObject, 19, 32.f, 24.f, 0.f, 10, 10, 65.f, 125.f, BITMAP_season_wing_periodc, BITMAP_season_wing_periodc, 5125);
					pCloth[textureLayer].AddCollisionSphere(0.f, -15.f, -40.f, 37.f, 17);
					pCloth[textureLayer].AddCollisionSphere(0.f, 0.f, 0.f, 35.f, 2);

					if (textureStrip < numCloth)
					{
						pCloth[textureStrip].Create(pObject, 19, -32.f, 24.f, 0.f, 10, 10, 65.f, 125.f, BITMAP_season_wing_periodc2, BITMAP_season_wing_periodc2, 5125);
						pCloth[textureStrip].AddCollisionSphere(0.f, -10.f, -40.f, 37.f, 17);
						pCloth[textureStrip].AddCollisionSphere(0.f, 0.f, 0.f, 35.f, 2);
					}
				}
				else if (RenderType01 == 7)
				{
				}
				else
				{
					if (byFirstClass == Rage_Fighter)
					{
						if (RenderType01 == 0)
						{
							pCloth[textureLayer].Create(pObject, 19, 0.0f, 15.0f, 5.0f, 10, 10, 180.0f, 170.0f, physicsData->textureIndex[0], physicsData->textureIndex[0], PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
							pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[1], 3.0, 0.4, 1.0);
							pCloth[textureLayer].AddCollisionSphere(-10.f, -10.0f, -10.0f, 35.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(10.f, -10.0f, -10.0f, 35.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(-10.f, -10.0f, 20.0f, 37.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(10.f, -10.0f, 20.0f, 37.0f, 17);

							if (physicsData->iBoneStrip && textureStrip < (numCloth - 1))
							{
								pCloth[textureStrip].Create(pObject, 19, 25.0f, 15.0f, 2.0f, 2, 5, 12.0f, 180.0f, physicsData->textureIndex[2], physicsData->textureIndex[2], PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_ELASTIC_RAGE_L | PCT_MASK_ALPHA);
								pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[3], 3.0, 0.4, 1.0);
								pCloth[textureStrip].AddCollisionSphere(0.0f, -15.0f, -20.0f, 35.0f, 2);
								pCloth[textureStrip].AddCollisionSphere(0.0f, 0.0f, 0.0f, 45.0f, 17);
								textureStrip++;
								pCloth[textureStrip].Create(pObject, 19, -25.0f, 15.0f, 2.0f, 2, 5, 12.0f, 180.0f, physicsData->textureIndex[2], physicsData->textureIndex[2], PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_ELASTIC_RAGE_R | PCT_MASK_ALPHA);
								pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[3], 3.0, 0.4, 1.0);
								pCloth[textureStrip].AddCollisionSphere(0.0f, -15.0f, -20.0f, 35.0f, 2);
								pCloth[textureStrip].AddCollisionSphere(0.0f, 0.0f, 0.0f, 50.0f, 17);
							}
						}
					}
					else
					{
						if (RenderType01 == 0)
						{
							pCloth[textureLayer].Create(pObject, 19, 0.0f, 8.0f, 10.0f, 10, 10, 180.0f, 180.0f, physicsData->textureIndex[0], physicsData->textureIndex[0], PCT_CURVED | PCT_SHORT_SHOULDER | PCT_HEAVY | PCT_MASK_ALPHA);
							pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[1], 3.0, 0.4, 1.0);
							pCloth[textureLayer].AddCollisionSphere(-10.f, -10.0f, -10.0f, 25.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(10.f, -10.0f, -10.0f, 25.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(-10.f, -10.0f, 20.0f, 27.0f, 17);
							pCloth[textureLayer].AddCollisionSphere(10.f, -10.0f, 20.0f, 27.0f, 17);

							if (physicsData->iBoneStrip && textureStrip < (numCloth - 1))
							{
								pCloth[textureStrip].Create(pObject, 19, 30.0f, 15.0f, 10.0f, 2, 5, 12.0f, 200.0f, physicsData->textureIndex[2], physicsData->textureIndex[2], PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_MASK_ALPHA);
								pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[3], 3.0, 0.4, 1.0);
								pCloth[textureStrip].AddCollisionSphere(0.0f, -15.0f, -20.0f, 30.0f, 2);
								pCloth[textureStrip].AddCollisionSphere(0.f, 0.0f, 0.0f, 35.0f, 17);
								textureStrip++;
								pCloth[textureStrip].Create(pObject, 19, -30.0f, 20.0f, 10.0f, 2, 5, 12.0f, 200.0f, physicsData->textureIndex[2], physicsData->textureIndex[2], PCT_FLAT | PCT_SHAPE_NORMAL | PCT_COTTON | PCT_MASK_ALPHA);
								pCloth[textureLayer].SetTextureLuminosity(physicsData->textureIndex[3], 3.0, 0.4, 1.0);
								pCloth[textureStrip].AddCollisionSphere(0.0f, -15.0f, -20.0f, 30.0f, 2);
								pCloth[textureStrip].AddCollisionSphere(0.f, 0.0f, 0.0f, 35.0f, 17);
							}
						}
					}
				}
			}
		}
	}

	return pCloth;
}

bool CGMPhysicsManager::physics_work_out(int nType, BMD* pModel, OBJECT* o)
{
	const WINGS_PHYSICS_INFO* physicsData;

	physicsData = this->find((nType - MODEL_ITEM));

	bool work_out = false;

	if (physicsData)
	{
		BYTE RenderType01 = physicsData->RenderType;

		work_out = true;

		if (pModel->BodyLight[0] != 1.0 || pModel->BodyLight[1] != 1.0 || pModel->BodyLight[2] != 1.0)
		{
			pModel->BodyLight[0] = 1.0;
			pModel->BodyLight[1] = 1.0;
			pModel->BodyLight[2] = 1.0;
			glColor3fv(pModel->BodyLight);

			if (RenderType01 == 1)
			{
				pModel->RenderMesh(1, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(2, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(3, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(4, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
			}
			else if (RenderType01 == 2)
			{
				pModel->RenderMesh(1, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
			}
			else if (RenderType01 == 3)
			{
				pModel->RenderMesh(0, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(3, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(4, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(5, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
			}
			else if (RenderType01 == 4)
			{
				pModel->RenderMesh(1, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(2, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(3, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
			}
			else if (RenderType01 == 6)
			{
				pModel->RenderMesh(0, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(1, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(2, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(3, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
				pModel->RenderMesh(4, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
			}
			else if (RenderType01 == 0)
			{
				for (int i = 0; i < pModel->NumMeshs; i++)
				{
					if (physicsData->iBoneLayer != i && physicsData->iBoneStrip != i)
					{
						pModel->RenderMesh(i, 2, o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, -1);
					}
				}
			}
			else
			{
				work_out = false;
			}
		}
		else
		{
			pModel->BodyLight[0] = 1.0;
			pModel->BodyLight[1] = 1.0;
			pModel->BodyLight[2] = 1.0;
			glColor3fv(pModel->BodyLight);
			pModel->RenderBody(RENDER_TEXTURE,o->Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, o->HiddenMesh, -1);
		}

	}


	return work_out;
}



