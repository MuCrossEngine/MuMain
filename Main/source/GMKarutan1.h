//*****************************************************************************
// File: GMKarutan1.h
//
// Desc: ≈≥∑Á≈∫1 ∏ , ∏ÛΩ∫≈Õ.
//
// producer: Ahn Sang-Kyu (10.08.03)
//*****************************************************************************

#if !defined(AFX_GMKARUTAN1_H__A2F56C80_26D8_4474_AECE_63DA2DA511A9__INCLUDED_)
#define AFX_GMKARUTAN1_H__A2F56C80_26D8_4474_AECE_63DA2DA511A9__INCLUDED_

#pragma once

#include "w_BaseMap.h"

BoostSmartPointer( CGMKarutan1 );

class CGMKarutan1 : public BaseMap
{
protected:
	CGMKarutan1();

public:
	virtual ~CGMKarutan1();

	static CGMKarutan1Ptr Make();

	virtual bool CreateObject(OBJECT* o);
	virtual bool MoveObject(OBJECT* o);
	virtual bool RenderObjectVisual(OBJECT* o, BMD* b);
	virtual bool RenderObjectMesh(OBJECT* o, BMD* b, bool ExtraMon = 0);
	virtual void RenderAfterObjectMesh(OBJECT* o, BMD* b, bool ExtraMon = 0);
	virtual CHARACTER* CreateMonster(int iType, int PosX, int PosY, int Key);
	virtual bool MoveMonsterVisual(OBJECT* o, BMD* b);
	virtual void MoveBlurEffect(CHARACTER* c, OBJECT* o, BMD* b);
	virtual bool RenderMonsterVisual(CHARACTER* c, OBJECT* o, BMD* b);
	virtual bool AttackEffectMonster(CHARACTER* c, OBJECT* o, BMD* b);
	virtual bool SetCurrentActionMonster(CHARACTER* c, OBJECT* o);

	virtual bool PlayMonsterSound(OBJECT* o);
	virtual void PlayObjectSound(OBJECT* o);
	void PlayBGM();
};

extern bool IsKarutanMap();

#endif // !defined(AFX_GMKARUTAN1_H__A2F56C80_26D8_4474_AECE_63DA2DA511A9__INCLUDED_)
