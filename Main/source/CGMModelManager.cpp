#include "stdafx.h"
#include "ZzzTexture.h"
#include "CGMModelManager.h"
#include "./Utilities/Log/muConsoleDebug.h"

#ifdef __ANDROID__
#include <android/log.h>
#define MU_ANDROID_MODEL_LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MUAndroid", __VA_ARGS__)
#endif

CGMModelManager::CGMModelManager()
{
	ClientModels = NULL;
	Init();
}

CGMModelManager::~CGMModelManager()
{
	Release();
}

void CGMModelManager::Init()
{
	modelMemoryDump.resize((MAX_MODELS + 1024));
	ClientModels = (modelMemoryDump.data()) + rand() % 1024;
}

void CGMModelManager::Release()
{
	ClientModels = NULL;
	modelMemoryDump.clear();
}

BMD* CGMModelManager::GetModel(int index)
{
	if (index >= 0 && index < MAX_MODELS)
	{
		return &ClientModels[index];
	}
	return nullptr;
}

BMD* CGMModelManager::GetPlayerModel()
{
	return &ClientModels[MODEL_PLAYER];
}

Action_t* CGMModelManager::GetPlayerModelAction(int i)
{
	return &ClientModels[MODEL_PLAYER].Actions[i];
}

Action_t* CGMModelManager::GetModelAction(int index, int i)
{
	BMD* pModel = GetModel(index);

	if (pModel)
	{
		return &pModel->Actions[i];
	}

	return nullptr;
}

Mesh_t* CGMModelManager::GetModelMesh(int index, int i)
{
	BMD* pModel = GetModel(index);

	if (pModel)
	{
		return pModel->GetMesh(i);
	}

	return nullptr;
}

void CGMModelManager::AccessModel(int Type, char* Path, char* FileName, int i)
{
	char Name[64];
	if (i == -1)
		sprintf(Name, "%s.bmd", FileName);
	else if (i >= 10)
		sprintf(Name, "%s%d.bmd", FileName, i);
	else
		sprintf(Name, "%s0%d.bmd", FileName, i);

	bool Success = false;

	BMD* pModel = this->GetModel(Type);

	if (pModel)
	{
		pModel->m_iBMDSeqID = Type;

	#ifdef __ANDROID__
		if (strcmp(Path, "Data\\Player\\") == 0)
		{
			MU_ANDROID_MODEL_LOGI("AccessModel begin type=%d file=%s%s", Type, Path, Name);
		}
	#endif

		Success = pModel->Open2(Path, Name);

	#ifdef __ANDROID__
		if (strcmp(Path, "Data\\Player\\") == 0)
		{
			MU_ANDROID_MODEL_LOGI("AccessModel end type=%d file=%s%s success=%d meshes=%d bones=%d actions=%d",
				Type,
				Path,
				Name,
				Success ? 1 : 0,
				pModel->NumMeshs,
				pModel->NumBones,
				pModel->NumActions);
		}
	#endif

		if (Success == false)
		{
			if ((strcmp(FileName, "Monster") == NULL || strcmp(FileName, "Player") == NULL || strcmp(FileName, "PlayerTest") == NULL || strcmp(FileName, "Angel") == NULL))
			{
				char Text[256];
				sprintf(Text, "[%s][%s] file does not exist.", Path, Name);

				MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
				SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
			}
		}
	}
}

void CGMModelManager::OpenTexture(int modelType, char* SubFolder, int Wrap, int Type, bool Check)
{
	BMD* pModel = this->GetModel(modelType);

	if (pModel)
	{
		for (int i = 0; i < pModel->NumMeshs; i++)
		{
			Texture_t* pTexture = &pModel->Textures[i];

			pModel->IndexTexture[i] = BITMAP_UNKNOWN;
			pModel->LightTexture[i] = 0;

			char szFullPath[256] = { 0, };
			strcpy(szFullPath, "Data\\");
			strcat(szFullPath, SubFolder);
			strcat(szFullPath, pTexture->FileName);

			char __ext[_MAX_EXT] = { 0, };
			_splitpath(pTexture->FileName, NULL, NULL, NULL, __ext);

			if (Strnicmp(pTexture->FileName, "nude_"))
			{
				pModel->LightTexture[i] = 1;
			}
			else if (Strnicmp(pTexture->FileName, "cotton_"))
			{
				pModel->LightTexture[i] = 2;

			}
			else if (Strnicmp(pTexture->FileName, "cotton2_"))
			{
				pModel->LightTexture[i] = 3;
			}
			else if (Strnicmp(pTexture->FileName, "K_nude"))
			{
				pModel->LightTexture[i] = 4;
			}

			if (Strnicmp(pTexture->FileName, "skin_"))
			{
				pModel->IndexTexture[i] = BITMAP_SKIN;
			}
			else if (Strnicmp(pTexture->FileName, "level"))
			{
				pModel->IndexTexture[i] = BITMAP_SKIN;
			}
			else if (Strnicmp(pTexture->FileName, "hid"))
			{
				pModel->IndexTexture[i] = BITMAP_HIDE;
			}
			else if (Strnicmp(pTexture->FileName, "hair"))
			{
				pModel->IndexTexture[i] = BITMAP_HAIR;
			}
			else if (Strnicmp(pTexture->FileName, "HQskin3"))
			{
				pModel->IndexTexture[i] = BITMAP_HQSKIN3;
			}
			else if (Strnicmp(pTexture->FileName, "HQskin2"))
			{
				pModel->IndexTexture[i] = BITMAP_HQSKIN2;
			}
			else if (Strnicmp(pTexture->FileName, "HQskin"))
			{
				pModel->IndexTexture[i] = BITMAP_HQSKIN;
			}
			else if (Strnicmp(pTexture->FileName, "HQlevel_"))
			{
				pModel->IndexTexture[i] = BITMAP_HQSKIN;
			}
			else if (Strnicmp(pTexture->FileName, "HQhair_"))
			{
				pModel->IndexTexture[i] = BITMAP_HQHAIR;
			}
			else if (Strnicmp(pTexture->FileName, "HQhair"))
			{
				pModel->IndexTexture[i] = BITMAP_HQHAIR;
			}
			else if (tolower(__ext[1]) == 't')
			{
			#ifdef __ANDROID__
				pModel->IndexTexture[i] = Bitmaps.LoadImage(szFullPath, GL_LINEAR_MIPMAP_LINEAR, Wrap);
			#else
				pModel->IndexTexture[i] = Bitmaps.LoadImage(szFullPath, GL_NEAREST_MIPMAP_NEAREST, Wrap);
			#endif
			}
			else if (tolower(__ext[1]) == 'j')
			{
				pModel->IndexTexture[i] = Bitmaps.LoadImage(szFullPath, GL_LINEAR_MIPMAP_LINEAR, Wrap);
			}

			if (pModel->IndexTexture[i] == BITMAP_UNKNOWN)
			{
				BITMAP_t* pBitmap = Bitmaps.FindTextureByName(pTexture->FileName);
				if (pBitmap)
				{
					Bitmaps.LoadImage(pBitmap->BitmapIndex, pBitmap->FileName);
					pModel->IndexTexture[i] = pBitmap->BitmapIndex;
				}
				else
				{
					char szErrorMsg[256] = { 0, };
					sprintf(szErrorMsg, "OpenTexture Failed: %s of %s   %s", szFullPath, pModel->Name, pTexture->FileName);
					PopUpErrorCheckMsgBox(szErrorMsg, Check);
					break;
				}
			}
		}
	}
}

void CGMModelManager::OpenPack(int Type, char* Path, char* FileName, int i)
{
	char Name[64];
	if (i == -1)
		sprintf(Name, "%s.pack", FileName);
	else if (i >= 10)
		sprintf(Name, "%s%d.pack", FileName, i);
	else
		sprintf(Name, "%s0%d.pack", FileName, i);

	bool Success = false;

	BMD* pModel = this->GetModel(Type);

	if (pModel)
	{
		//pModel->m_iBMDSeqID = Type;
		//Success = pModel->OpenPack(Path, Name);
		//
		//if (Success == false)
		//{
		//	if ((strcmp(FileName, "Monster") == NULL || strcmp(FileName, "Player") == NULL || strcmp(FileName, "PlayerTest") == NULL || strcmp(FileName, "Angel") == NULL))
		//	{
		//		char Text[256];
		//		sprintf(Text, "%s file does not exist.", Name);
		//		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		//		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		//	}
		//}
	}
}

void CGMModelManager::Save(int Type)
{
	char urlFileName[128];
	CreateDirectory("Data2", 0);
	CreateDirectory("Data2\\Player", 0);

	sprintf_s(urlFileName, "Data2\\Player\\");

	BMD* pModel = this->GetModel(Type);

	if (pModel && pModel->NumMeshs > 0)
	{
		pModel->Save2(urlFileName, pModel->FileName);

		for (int i = 0; i < pModel->NumMeshs; i++)
		{
			Texture_t* pTexture = &pModel->Textures[i];

			char FileName1[128]; sprintf_s(FileName1, "Data\\Player\\%s", pTexture->FileName);
			char FileName2[128]; sprintf_s(FileName2, "Data2\\Player\\%s", pTexture->FileName);

			copy_file_run(FileName1, FileName2);
		}
	}
}

void CGMModelManager::SaveImg(int Type)
{
	CreateDirectory("Data2", 0);
	CreateDirectory("Data2\\Player", 0);

	BITMAP_t* pBitmap = &Bitmaps[Type];

	if (pBitmap->FileName[0] != '\0')
	{
		char FileName[128];
		sprintf_s(FileName, "Data2\\Player\\%s", GetFileNameWithExtension(pBitmap->FileName).c_str());

		copy_file_run(pBitmap->FileName, FileName);
	}
}

void CGMModelManager::copy_file_run(std::string FileOrigin, std::string FileLocation)
{
	changeExtension(FileOrigin);
	changeExtension(FileLocation);

	FILE* fp = fopen(FileOrigin.c_str(), "rb");

	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		int fileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		BYTE* Buffer = new BYTE[fileLen];

		fread(Buffer, fileLen, 1, fp);

		fclose(fp);

		fp = fopen(FileLocation.c_str(), "wb");

		if (fp)
		{
			fwrite(Buffer, fileLen, 1u, fp);
			fclose(fp);
		}
		delete[]Buffer;
	}
}