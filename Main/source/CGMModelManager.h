#pragma once
#include "ZzzBMD.h"
#include "Singleton.h"

class CGMModelManager : public Singleton<CGMModelManager>
{
public:
	CGMModelManager();
	virtual~CGMModelManager();

	void Init();
	void Release();
	BMD* GetModel(int index);
	BMD* GetPlayerModel();
	Action_t* GetPlayerModelAction(int i);
	Action_t* GetModelAction(int index, int i);
	Mesh_t* GetModelMesh(int index, int i);
	void AccessModel(int Type, char* Path, char* FileName, int i = -1);
	void OpenTexture(int modelType, char* SubFolder, int Wrap = GL_REPEAT, int Type = GL_NEAREST, bool Check = true);

	void OpenPack(int Type, char* Path, char* FileName, int i = -1);

	void Save(int Type);
	void SaveImg(int Type);
	void copy_file_run(std::string FileOrigin, std::string FileLocation);
private:
	BMD* ClientModels;
	std::vector<BMD> modelMemoryDump;
};

#define gmClientModels			(CGMModelManager::GetSingletonPtr())