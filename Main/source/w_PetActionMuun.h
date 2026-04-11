#pragma once
#include "w_PetActionCollecter_Add.h"
#include "w_PetActionCollecter.h"
#include "ZzzBMD.h"


BoostSmartPointer(PetActionMuun);
class PetActionMuun : public PetActionCollecterAdd
{
public:
	static PetActionMuunPtr Make();
	virtual ~PetActionMuun();
	virtual bool Release(OBJECT* obj, CHARACTER* Owner);

public:
	virtual bool Model(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender);
	virtual bool Move(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender);
	virtual bool Effect(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender);
private:
	PetActionMuun();
	BOOL m_bIsMoving;

};
