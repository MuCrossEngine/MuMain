#pragma once

class CUSTOM_MONSTER_INFO;

typedef std::vector<CUSTOM_MONSTER_INFO> type_monster;

class CGMMonsterMng
{
public:
	CGMMonsterMng();
	virtual~CGMMonsterMng();
	static CGMMonsterMng* Instance();
	void LoadDataMonster(CUSTOM_MONSTER_INFO* info, int Size);
	void LoadModelMonster();
	bool IsMonsterByIndex(int monsterIndex);
	CUSTOM_MONSTER_INFO* FindMonsterByIndex(int monsterIndex);

	CHARACTER* CreateMonster(int monsterIndex, int PositionX, int PositionY, int Key);
private:
	type_monster UniqueMonsterData;
};

#define GMMonsterMng			(CGMMonsterMng::Instance())