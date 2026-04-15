#pragma once
#include "Singleton.h"

class CGMCharacter : public Singleton<CGMCharacter>
{
public:
	CGMCharacter();
	~CGMCharacter();
	void Init();
	void Release();
	CHARACTER* GetCharacter(int index);
	CHARACTER* GetDummyCharacter();
	int GetCharacterIndex(CHARACTER* pCha);
private:
	CHARACTER* CharactersClient;
	std::vector<CHARACTER> CharacterMemoryDump;
};

CGMCharacter* GetGMCharacter();

#define gmCharacters			(GetGMCharacter())