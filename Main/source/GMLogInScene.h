#pragma once
#ifndef _GMLOGIN_FIELD_H_
#define _GMLOGIN_FIELD_H_

#include "w_BaseMap.h"

BoostSmartPointer(CGMLogInScene);


class CGMLogInScene : public BaseMap
{
public:
	static CGMLogInScenePtr Make();

	CGMLogInScene();
	virtual~CGMLogInScene();
	virtual void OpenWorld();
	virtual bool MoveObject(OBJECT* pObject);
	virtual bool CreateObject(OBJECT* pObject);
	virtual bool RenderObjectVisual(OBJECT* pObject, BMD* pModel);
	virtual bool RenderObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon = 0);
	virtual void RenderAfterObjectMesh(OBJECT* pObject, BMD* pModel, bool ExtraMon = 0);
};


#endif