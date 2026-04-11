// w_BaseMap.h: interface for the BaseMap class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ZzzBMD.h"
#include "MapManager.h"

#define RANGE_OBJET_MAP(index_obj)		(index_obj>=MODEL_WORLD_OBJECT && index_obj<MAX_WORLD_OBJECTS)

class BaseMap
{
public:
	BaseMap() {
	}

	virtual ~BaseMap() {
		clear();
	}

	virtual bool LoadMapData() {
		return false;
	}

	//-- Object
	virtual bool CreateObject(OBJECT* o) {
		return false;
	}

	virtual bool MoveObject(OBJECT* o) {
		return false;
	}

	virtual bool RenderObjectVisual(OBJECT* o, BMD* b) {
		return false;
	}

	virtual bool RenderObjectMesh(OBJECT* o, BMD* b, bool ExtraMon = 0) {
		return false;
	}

	virtual void RenderAfterObjectMesh(OBJECT* o, BMD* b, bool ExtraMon = 0) {
	}

	virtual void RenderFrontSideVisual() {
	}

	//-- Character
	virtual CHARACTER* CreateMonster(int iType, int PosX, int PosY, int Key) {
		return NULL;
	}

	virtual bool MoveMonsterVisual(OBJECT* o, BMD* b) {
		return false;
	}

	virtual void MoveBlurEffect(CHARACTER* c, OBJECT* o, BMD* b) {
	}

	virtual bool RenderMonsterVisual(CHARACTER* c, OBJECT* o, BMD* b) {
		return false;
	}

	virtual bool AttackEffectMonster(CHARACTER* c, OBJECT* o, BMD* b) {
		return false;
	}

	virtual bool SetCurrentActionMonster(CHARACTER* c, OBJECT* o) {
		return false;
	}

	virtual bool CreateWeather(PARTICLE* o) {
		return false;
	}

	virtual bool PlayMonsterSound(OBJECT* o) {
		return false;
	}

	virtual void PlayObjectSound(OBJECT* o) {
	}

	virtual bool ReceiveMapMessage(BYTE code, BYTE subcode, BYTE* ReceiveBuffer) {
		return false;
	}

	bool isMapIndex(int type) {
		for (int i = 0; i < (int)m_MapTypes.size(); ++i)
		{
			if (type == m_MapTypes[i])
			{
				return true;
			}
		}
		return false;
	}

	int FindMapIndex(int index = 0) {
		if (m_MapTypes.size() == 0 || index >= (int)m_MapTypes.size())
			return NUM_WD;
		else
			return m_MapTypes[index];
	}

	const bool IsCurrentMap(int type) {
		return isMapIndex(type);
	}

	void AddMapIndex(int type) {
		if (!isMapIndex(type))
			m_MapTypes.push_back(type);
	}

	void clear() {
		m_MapTypes.clear();
	}

	typedef std::vector<int> WorldVector;
private:
	WorldVector		m_MapTypes;
};
