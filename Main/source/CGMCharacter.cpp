#include "stdafx.h"
#include "CGMCharacter.h"

CGMCharacter* GetGMCharacter()
{
	static CGMCharacter s_character;
	return &s_character;
}

CGMCharacter::CGMCharacter()
{
	CharactersClient = NULL;
	Init();
}

CGMCharacter::~CGMCharacter()
{
	Release();
}

void CGMCharacter::Init()
{
	int Size = (MAX_CHARACTERS_CLIENT + 1 + 128);

	CharacterMemoryDump.resize(Size);

	CharactersClient = (CharacterMemoryDump.data()) + rand() % 128;
}

void CGMCharacter::Release()
{
	CharacterMemoryDump.clear();
	CharactersClient = NULL;
}

CHARACTER* CGMCharacter::GetCharacter(int index)
{
	if (index >= 0 && index < MAX_CHARACTERS_CLIENT)
	{
		return &CharactersClient[index];
	}
	return NULL;
}

CHARACTER* CGMCharacter::GetDummyCharacter()
{
	return &CharacterMemoryDump[MAX_CHARACTERS_CLIENT];
}

int CGMCharacter::GetCharacterIndex(CHARACTER* pCha)
{
	return static_cast<int>(pCha - &CharactersClient[0]);
}
