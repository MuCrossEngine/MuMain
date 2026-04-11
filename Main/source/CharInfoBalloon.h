//*****************************************************************************
// File: CharInfoBalloon.h
//*****************************************************************************

#if !defined(AFX_CHARINFOBALLOON_H__DC2BBC6F_834B_4738_AB09_361BF8484977__INCLUDED_)
#define AFX_CHARINFOBALLOON_H__DC2BBC6F_834B_4738_AB09_361BF8484977__INCLUDED_

#pragma once

#include "Sprite.h"

#include "ZzzInfomation.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"

class CCharInfoBalloon : public CSprite  
{
protected:
	CHARACTER*	m_pCharInfo;
	DWORD		m_dwNameColor;
	char		m_szName[64];
	char		m_szGuild[64];
	char		m_szClass[64];
	char		m_szRese[64];
	char		m_szMaster[64];

	char		m_szOnClass[64];
	char		m_szOnLevel[64];
public:
	CCharInfoBalloon();
	virtual ~CCharInfoBalloon();

	void Create(CHARACTER* pCharInfo);
	void Render();

	void SetInfo();

	char* GetName() {
		return m_szName;
	};

	char* GetGuild() {
		return m_szGuild;
	};

	char* GetClassname() {
		return m_szOnClass;
	};

	char* GetOnLevel() {
		return m_szOnLevel;
	};
};

#endif // !defined(AFX_CHARINFOBALLOON_H__DC2BBC6F_834B_4738_AB09_361BF8484977__INCLUDED_)
