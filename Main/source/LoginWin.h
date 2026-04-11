//*****************************************************************************
// File: LoginWin.h
//*****************************************************************************
#pragma once

#include "Win.h"

#include "Button.h"
#include "NewUICursedTempleSystem.h"

class CUITextInputBox;

#define	LIW_ACCOUNT		0
#define	LIW_PASSWORD	1
#define LIW_OK			0
#define LIW_CANCEL		1
#define LIW_BTN			2
#define LIW_SAVE		3


typedef struct
{
	std::string NameId;
	std::string PassID;
}LOGIN_ACCOUNT_REG;

class CLoginWin : public CWin
{
	enum IMAGE_LIST
	{
		newui_ctskillup = SEASON3B::CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_SKILLUPBT,
		newui_ctskilldown = SEASON3B::CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_SKILLDOWNBT,
		newui_party_x = BITMAP_PARTY_INFO_BEGIN + 3,
		newui_battleskill = BITMAP_INTERFACE_EX + 34,
	};
protected:
	CSprite m_asprInputBox[2];
#ifndef SAVE_ACCOUNT_SYSTEM
	CButton m_aBtn[2];
#else
	CButton m_aBtn[4];
#if MAIN_UPDATE > 303
	CButton m_BtnRemove[MAX_ACCOUNT_REG];
	CButton m_BtnaLogin[MAX_ACCOUNT_REG];
#endif
#endif // SAVE_ACCOUNT_SYSTEM
	CUITextInputBox* m_pIDInputBox;
	CUITextInputBox* m_pPassInputBox;
#if(CB_DANGKYINGAME)
	CButton		m_DangKy;
#endif
public:
	CLoginWin();
	virtual ~CLoginWin();
	void Create();
	void SetPosition(int nXCoord, int nYCoord);
	void Show(bool bShow);
	bool CursorInWin(int nArea);

	void ConnectConnectionServer();

	CUITextInputBox* GetIDInputBox() const;
	CUITextInputBox* GetPassInputBox() const;
private:
	BOOL FirstLoad;
#ifdef SAVE_ACCOUNT_SYSTEM
#if MAIN_UPDATE > 303
	BOOL IsLoginTick;
#endif
	int _RenderFrameX;
	int _RenderFrameY;
	std::vector<LOGIN_ACCOUNT_REG> stAccountMacro;
#endif // SAVE_ACCOUNT_SYSTEM
protected:
	void PreRelease();
	void UpdateWhileActive(double dDeltaTick);
	void UpdateWhileShow(double dDeltaTick);
	void RenderControls();
	void RequestLogin(bool Save = true);
	void CancelLogin();
#ifdef SAVE_ACCOUNT_SYSTEM
	void OpenAccountTable(bool bVisible);
	void ReloadAccount(bool reload, bool update, bool save);
	void SaveAccount(std::string NameId, std::string PassID);
#endif // SAVE_ACCOUNT_SYSTEM
};

