#pragma once
class CPhysicsCloth;

class CGMPhysicsManager
{
public:
	CGMPhysicsManager();
	virtual~CGMPhysicsManager();
	void ReadData(FILE* fp);
	void LoadDataTexture();
	bool physics_work_out(int nType, bool modelType = true);
	CPhysicsCloth* physics_work_out(CHARACTER* pCharacter, OBJECT* pObject, int &numCloth);

	bool physics_work_out(int nType, BMD* pModel, OBJECT* o);
protected:
	size_t Size();
	int GenerateTextureIndex();
	WINGS_PHYSICS_INFO* Get(size_t i);
	const WINGS_PHYSICS_INFO* find(int nType);
	void LoadImagenData(WORD &textureIndex, const std::string& filename);
public:
	static CGMPhysicsManager* Instance();
private:
	int TextureIndex;
	std::vector<WINGS_PHYSICS_INFO> physics_data;
};

#define GMPhysicsManager			(CGMPhysicsManager::Instance())