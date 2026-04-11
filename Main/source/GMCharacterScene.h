#pragma once
#ifndef _GMCHARACTER_FIELD_H_
#define _GMCHARACTER_FIELD_H_

#include "w_BaseMap.h"

BoostSmartPointer(CGMCharacterScene);

class CGMCharacterScene : public BaseMap
{
public:
	static CGMCharacterScenePtr Make();

	CGMCharacterScene();
	virtual~CGMCharacterScene();
	virtual void OpenWorld();
	virtual bool MoveObject(OBJECT* pObject);
	virtual bool CreateObject(OBJECT* pObject);
	virtual bool RenderObjectVisual(OBJECT* pObject, BMD* pModel);
	virtual bool RenderObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon = 0);
	virtual void RenderAfterObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon = 0);
};
#endif