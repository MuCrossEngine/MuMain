//*****************************************************************************
// File: CharSelMainWin.h
//*****************************************************************************

#pragma once

#include "Win.h"
#include "Button.h"

#define	CSMW_SPR_DECO			0
#define	CSMW_SPR_INFO			1
#define	CSMW_SPR_MAX			2

#define	CSMW_BTN_CREATE			0
#define	CSMW_BTN_MENU			1
#define	CSMW_BTN_CONNECT		2
#define	CSMW_BTN_DELETE			3
#define	CSMW_BTN_MAX			4

class CCharSelMainWin : public CWin  
{
protected:
	CSprite	m_asprBack[CSMW_SPR_MAX];
	CButton	m_aBtn[CSMW_BTN_MAX];
	bool	m_bAccountBlockItem;

public:
	CCharSelMainWin();
	virtual ~CCharSelMainWin();
	
	void Create();
	void SetPosition(int nXCoord, int nYCoord);
	void Show(bool bShow);
	bool CursorInWin(int nArea);
	void UpdateDisplay();

	bool runtime_check_button();
protected:
	void PreRelease();
	void UpdateWhileActive(double dDeltaTick);
	void RenderControls();
	void DeleteCharacter();

	bool update_button_info(int i, int _RenderFrameX, int _RenderFrameY);
	void render_button_info(int i, int _RenderFrameX, int _RenderFrameY);
};