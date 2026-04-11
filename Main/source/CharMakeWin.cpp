//*****************************************************************************
// File: CharMakeWin.cpp
//*****************************************************************************

#include "stdafx.h"
#include "CharMakeWin.h"
#include "Input.h"
#include "UIMng.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzOpenglUtil.h"
#include "DSPlaySound.h"
#include "ZzzAI.h"
#include "ZzzScene.h"
#include "UIControls.h"
#include "wsclientinline.h"
#include "Local.h"
#include "CharacterManager.h"
#include "CGMProtect.h"

#define	CMW_OK		0
#define	CMW_CANCEL	1

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern int g_iChatInputType;
extern CUITextInputBox* g_pSingleTextInputBox;

void MoveCharacterCamera(vec3_t Origin, vec3_t Position, vec3_t Angle);

CCharMakeWin::CCharMakeWin()
{
	m_nSelJob = 0;
	m_nDescLine = 0;
	memset(m_aszJobDesc, 0, sizeof(m_aszJobDesc));
}

CCharMakeWin::~CCharMakeWin()
{
}

void CCharMakeWin::UpdateDisplay(bool bSuccess)
{
	m_nSuccess = bSuccess;

	if (m_nSuccess == 1)
	{
		CHARACTER* pCharacter = gmCharacters->GetCharacter(SelectedHero);

		if (pCharacter)
		{
			m_nSelJob = pCharacter->GetBaseClass();

			switch (gCharacterManager.GetBaseClass(m_nSelJob))
			{
			case 7:
				m_nSelJob = 0;
				break;
			case 8:
				m_nSelJob = 1;
				break;
			case 9:
				m_nSelJob = 2;
				break;
			case 10:
				m_nSelJob = 5;
				break;
			case 11:
				m_nSelJob = 0;
				break;
			case 12:
				m_nSelJob = 1;
				break;
			case 13:
				m_nSelJob = 2;
				break;
			case 14:
				m_nSelJob = 3;
				break;
			case 15:
				m_nSelJob = 4;
				break;
			case 16:
				m_nSelJob = 5;
				break;
			case 17:
				m_nSelJob = 6;
				break;
			default:
				break;
			}
		}

		UpdateDisplay();

		for (int j = 0; j < MAX_CLASS; ++j)
			m_abtnJob[j].SetCheck(false);
		m_abtnJob[m_nSelJob].SetCheck(true);
	}
	else
	{
		UpdateDisplay();
	}
}

void CCharMakeWin::Create()
{
	CInput rInput = CInput::Instance();
	CWin::Create(rInput.GetScreenWidth(), rInput.GetScreenHeight());

	m_winBack.Create(454, 406, -2);

	m_asprBack[CMW_SPR_INPUT].Create(346, 38, BITMAP_LOG_IN);

	m_asprBack[CMW_SPR_STAT].Create(108, 80);

	m_asprBack[CMW_SPR_DESC].Create(454, 51);

	int i;
	for (i = CMW_SPR_STAT; i < CMW_SPR_MAX; ++i)
	{
		m_asprBack[i].SetAlpha(143);
		m_asprBack[i].SetColor(0, 0, 0);
	}

	DWORD adwJobBtnClr[8] =
	{
		CLRDW_BR_GRAY, CLRDW_BR_GRAY, CLRDW_WHITE, CLRDW_GRAY,
		CLRDW_BR_GRAY, CLRDW_BR_GRAY, CLRDW_WHITE, CLRDW_GRAY
	};

	int nText;
	int _btn_classname[MAX_CLASS] = { 20, 21, 22, 23, 24, 1687, 3150 };

	int max_button_create = MAX_CLASS;

	if (gmProtect->RemoveClass == 1)
	{
		max_button_create = Dark_Lord;
	}
	else if (gmProtect->RemoveClass == 2)
	{
		max_button_create = Summoner;
	}
	else if (gmProtect->RemoveClass == 3)
	{
		max_button_create = Rage_Fighter;
	}

	for (i = 0; i < max_button_create; ++i)
	{
		m_abtnJob[i].Create(108, 26, BITMAP_LOG_IN + 1, 4, 2, 1, 0, 3, 3, 3, 0);

		nText = _btn_classname[i];
		m_abtnJob[i].SetText(GlobalText[nText], adwJobBtnClr);
		CWin::RegisterButton(&m_abtnJob[i]);
	}

	for (i = 0; i < 2; ++i)
	{
		m_aBtn[i].Create(54, 30, BITMAP_BUTTON + i, 3, 2, 1);
		CWin::RegisterButton(&m_aBtn[i]);
	}

	::memset(m_aszJobDesc, 0, sizeof(char) * CMW_DESC_LINE_MAX * CMW_DESC_ROW_MAX);
	m_nDescLine = 0;

	m_nSelJob = CLASS_KNIGHT;
	m_abtnJob[m_nSelJob].SetCheck(true);

	UpdateDisplay();
}

void CCharMakeWin::PreRelease()
{
	for (int i = 0; i < CMW_SPR_MAX; ++i)
		m_asprBack[i].Release();
	m_winBack.Release();
}

void CCharMakeWin::SetPosition(int nXCoord, int nYCoord)
{
	int nBaseY = nYCoord + 131;
	int nBaseX = nXCoord + 346;

	int nBtnHeight = m_abtnJob[0].GetHeight();

	m_winBack.SetPosition(nXCoord, nYCoord);

	if (gmProtect->RemoveClass == 1)
	{
		nBaseY = nYCoord + (246 - nBtnHeight);
		m_asprBack[CMW_SPR_STAT].SetPosition(nBaseX, nBaseY - 107);

		for (int i = 0; i < 4; ++i)
		{
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + i * nBtnHeight);
		}
	}
	else if (gmProtect->RemoveClass == 2)
	{
		nBaseY += (nBtnHeight + nBtnHeight);
		m_asprBack[CMW_SPR_STAT].SetPosition(nBaseX, nBaseY - 107);

		for (int i = 0; i < 3; ++i)
		{
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + i * nBtnHeight);
		}

		nBaseY = nYCoord + 246;

		for (int i = 3; i <= CLASS_DARK_LORD; ++i)
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + (i - 2) * nBtnHeight);
	}
	else if (gmProtect->RemoveClass == 3)
	{
		nBaseY += nBtnHeight;
		m_asprBack[CMW_SPR_STAT].SetPosition(nBaseX, nBaseY - 107);

		for (int i = 0; i < 3; ++i)
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + i * nBtnHeight);

		nBaseY = nYCoord + 246;
		m_abtnJob[CLASS_SUMMONER].SetPosition(nBaseX, nBaseY);

		for (int i = 3; i <= CLASS_DARK_LORD; ++i)
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + (i - 2) * nBtnHeight);
	}
	else
	{
		m_asprBack[CMW_SPR_STAT].SetPosition(nBaseX, nYCoord + 24);

		for (int i = 0; i < 3; ++i)
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + i * nBtnHeight);

		m_abtnJob[CLASS_SUMMONER].SetPosition(nBaseX, nBaseY + 3 * nBtnHeight);

		nBaseY = nYCoord + 246;
		m_abtnJob[CLASS_RAGEFIGHTER].SetPosition(nBaseX, nBaseY);

		for (int i = 3; i <= CLASS_DARK_LORD; ++i)
			m_abtnJob[i].SetPosition(nBaseX, nBaseY + (i - 2) * nBtnHeight);
	}

	nBaseY = nYCoord + 325;
	m_aBtn[CMW_OK].SetPosition(nBaseX, nBaseY);
	m_aBtn[CMW_CANCEL].SetPosition(nXCoord + 400, nBaseY);

	m_asprBack[CMW_SPR_INPUT].SetPosition(nXCoord, nYCoord + 317);

	if (g_pSingleTextInputBox)
		g_pSingleTextInputBox->SetPosition(
			int((m_asprBack[CMW_SPR_INPUT].GetXPos() + 78) / g_fScreenRate_x),
			int((m_asprBack[CMW_SPR_INPUT].GetYPos() + 21) / g_fScreenRate_y));

	m_asprBack[CMW_SPR_DESC].SetPosition(nXCoord, nYCoord + 355);
}

void CCharMakeWin::Show(bool bShow)
{
	CWin::Show(bShow);

	int i;
	for (i = 0; i < CMW_SPR_MAX; ++i)
		m_asprBack[i].Show(bShow);

	for (i = 0; i < MAX_CLASS; ++i)
		m_abtnJob[i].Show(bShow);

	for (i = 0; i < 2; ++i)
		m_aBtn[i].Show(bShow);

	if (g_pSingleTextInputBox)
	{
		if (bShow)
		{
			g_pSingleTextInputBox->SetState(UISTATE_NORMAL);
			g_pSingleTextInputBox->SetOption(UIOPTION_NULL);
			g_pSingleTextInputBox->SetBackColor(0, 0, 0, 0);
			g_pSingleTextInputBox->SetTextLimit(10);
			g_pSingleTextInputBox->SetByteLimite(0x0A);
			g_pSingleTextInputBox->GiveFocus();
		}
		else
		{
			g_pSingleTextInputBox->SetText(NULL);
			g_pSingleTextInputBox->SetState(UISTATE_HIDE);
			if (g_pSingleTextInputBox->HaveFocus())
			{
				SetFocus(gwinhandle->GethWnd());
			}
		}
	}
}

bool CCharMakeWin::CursorInWin(int nArea)
{
	if (!CWin::m_bShow)
		return false;

	switch (nArea)
	{
	case WA_MOVE:
		return false;
	}

	return CWin::CursorInWin(nArea);
}

void CCharMakeWin::UpdateDisplay()
{
	const int _SecondClassCnt[] = { CLASS_DARK_LORD, CLASS_DARK, CLASS_RAGEFIGHTER };

	for (int i = 0; i < MAX_CLASS; ++i)
	{
		m_abtnJob[i].SetEnable(true);
	}

	for (int i = 0; i < 3 - GetCreateMaxClass(); ++i)
	{
		m_abtnJob[_SecondClassCnt[i]].SetEnable(false);
	}

	for (int i = 0; i < CLASS_CHARACTERCARD_TOTALCNT; ++i)
	{
		if (!g_CharCardEnable.bCharacterEnable[i])
		{
			m_abtnJob[i + CLASS_DARK].SetEnable(false);
		}
	}

	if (m_nSelJob == CLASS_DARK_LORD)
		m_asprBack[CMW_SPR_STAT].SetSize(0, 96, Y);
	else
		m_asprBack[CMW_SPR_STAT].SetSize(0, 80, Y);

	int nText = m_nSelJob == CLASS_SUMMONER ? 1690 : 1705 + m_nSelJob;

	if (m_nSelJob == CLASS_RAGEFIGHTER)
		nText = 3152;
	m_nDescLine = ::SeparateTextIntoLines(GlobalText[nText], m_aszJobDesc[0], CMW_DESC_LINE_MAX, CMW_DESC_ROW_MAX);

	SelectCreateCharacter();
}

void CCharMakeWin::UpdateWhileActive(double dDeltaTick)
{
	if (dDeltaTick >= 0.0)
	{
		if (!m_nSuccess)
		{
			for (int i = 0; i < MAX_CLASS; ++i)
			{
				if (m_abtnJob[i].IsClick())
				{
					for (int j = 0; j < MAX_CLASS; ++j)
						m_abtnJob[j].SetCheck(false);
					m_abtnJob[i].SetCheck(true);

					if (m_nSelJob != i)
					{
						m_nSelJob = i;
						UpdateDisplay();
					}
					break;
				}
			}
		}

		//if (g_pSingleTextInputBox)
		//	g_pSingleTextInputBox->DoAction();

		if (m_aBtn[CMW_OK].IsClick())
		{
			RequestCreateCharacter();
		}
		else if (m_aBtn[CMW_CANCEL].IsClick())
		{
			CUIMng::Instance().HideWin(this);
		}
		else
		{
			if (CInput::Instance().IsKeyDown(VK_RETURN))
			{
				::PlayBuffer(SOUND_CLICK01);
				RequestCreateCharacter();
			}
			else if (CInput::Instance().IsKeyDown(VK_ESCAPE))
			{
				::PlayBuffer(SOUND_CLICK01);
				CUIMng::Instance().HideWin(this);
				CUIMng::Instance().SetSysMenuWinShow(false);
			}
		}
		UpdateCreateCharacter();
	}
}

void CCharMakeWin::RequestCreateCharacter()
{
	char lpszCheck[MAX_PATH] = { 0, };
	wchar_t szReceivedChat[MAX_PATH] = {0,};
	if (g_pSingleTextInputBox)
	{
		g_pSingleTextInputBox->GetText(szReceivedChat);
		g_pSingleTextInputBox->GetText(lpszCheck);
	}

	CUIMng& rUIMng = CUIMng::Instance();

	if (::strlen(lpszCheck) < 4 || ::strlen(lpszCheck) >= MAX_ID_SIZE)
	{
		rUIMng.PopUpMsgWin(MESSAGE_MIN_LENGTH);
	}
	else if (::CheckName())
	{
		rUIMng.PopUpMsgWin(MESSAGE_ID_SPACE_ERROR);
	}
	else if (CheckSpecialText(szReceivedChat))
	{
		rUIMng.PopUpMsgWin(MESSAGE_SPECIAL_NAME);
	}
	else
	{
		SendRequestCreateCharacter(lpszCheck, CharacterView.Class, CharacterView.Skin);
		rUIMng.HideWin(this);
		rUIMng.PopUpMsgWin(MESSAGE_WAIT);
	}
}

void CCharMakeWin::RenderControls()
{
	RenderCreateCharacter();
	::EnableAlphaTest();

	int i;
	for (i = 0; i < CMW_SPR_MAX; ++i)
	{
		m_asprBack[i].Render();
	}
	CWin::RenderButtons();
	g_pRenderText->SetFont(g_hFixFont);
	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->SetBgColor(0);

	char* apszStat[MAX_CLASS][4] =
	{
		"18", "18", "15", "30",
		"28", "20", "25", "10",
		"22", "25", "20", "15",
		"26", "26", "26", "26",
		"26", "20", "20", "15",
		"21", "21", "18", "23",
#ifdef PBG_ADD_NEWCHAR_MONK
		"32", "27", "25", "20",
#endif //PBG_ADD_NEWCHAR_MONK
	};
	int nStatBaseX = m_asprBack[CMW_SPR_STAT].GetXPos() + 22;
	int nStatY;
	for (i = 0; i < 4; ++i)
	{
		nStatY = int((m_asprBack[CMW_SPR_STAT].GetYPos() + 10 + i * 17)
			/ g_fScreenRate_y);

		g_pRenderText->SetTextColor(CLRDW_ORANGE);
		g_pRenderText->RenderText(int((nStatBaseX + 54) / g_fScreenRate_x), nStatY, apszStat[m_nSelJob][i]);
		g_pRenderText->SetTextColor(CLRDW_WHITE);
		g_pRenderText->RenderText(int(nStatBaseX / g_fScreenRate_x), nStatY, GlobalText[1701 + i]);
	}

	if (m_nSelJob == CLASS_DARK_LORD)
	{
		nStatY = int((m_asprBack[CMW_SPR_STAT].GetYPos() + 10 + 4 * 17) / g_fScreenRate_y);

		g_pRenderText->SetTextColor(CLRDW_ORANGE);
		g_pRenderText->RenderText(int((nStatBaseX + 54) / g_fScreenRate_x), nStatY, "25");
		g_pRenderText->SetTextColor(CLRDW_WHITE);
		g_pRenderText->RenderText(int(nStatBaseX / g_fScreenRate_x), nStatY, GlobalText[1738]);
	}
	{
		for (i = 0; i < m_nDescLine; ++i)
		{
			g_pRenderText->RenderText(int((m_asprBack[CMW_SPR_DESC].GetXPos() + 10) / g_fScreenRate_x),
				int((m_asprBack[CMW_SPR_DESC].GetYPos() + 12 + i * 19) / g_fScreenRate_y), m_aszJobDesc[i]);
		}
	}

	g_pRenderText->SetFont(g_hFont);

	if (g_pSingleTextInputBox)
		g_pSingleTextInputBox->Render();
}

void CCharMakeWin::SelectCreateCharacter()
{
	CharacterView.Class = m_nSelJob;
	CreateCharacterPointer(&CharacterView, MODEL_FACE + CharacterView.Class, 0, 0);
	CharacterView.Object.Kind = 0;
	SetAction(&CharacterView.Object, 1);
}

void CCharMakeWin::UpdateCreateCharacter()
{
	if (!CharacterAnimation(&CharacterView, &CharacterView.Object))
		SetAction(&CharacterView.Object, 0);
}

void CCharMakeWin::RenderCreateCharacter()
{
	OBJECT* o = &CharacterView.Object;
	vec3_t Position, Angle;

	Vector(1.0f, 1.0f, 1.0f, o->Light);
	Vector(10, -500.f, 48.f, Position);
	Vector(-90.f, 0.f, 0.f, Angle);
	CameraFOV = 10.f;
	MoveCharacterCamera(CharacterView.Object.Position, Position, Angle);

	BeginOpengl(m_winBack.GetXPos() / g_fScreenRate_x, m_winBack.GetYPos() / g_fScreenRate_y, 410 / g_fScreenRate_x, 335 / g_fScreenRate_y, true);

	if (CharacterView.Class == CLASS_WIZARD)
	{
		Vector(0.f, 0.f, -40.0f, o->Angle);
		o->Scale = 5.9f;
	}
	else if (CharacterView.Class == CLASS_KNIGHT)
	{
		Vector(0.f, 0.f, -12.0f, o->Angle);
		o->Scale = 6.05f;
	}
	else if (CharacterView.Class == CLASS_ELF)
	{
		Vector(8.f, 0.f, 5.0f, o->Angle);
		o->Scale = 9.1f;
		CharacterView.Object.Position[0] += 4.8f;
	}
	else if (CharacterView.Class == CLASS_DARK)
	{
		Vector(8.f, 0.f, -13.0f, o->Angle);
		o->Scale = 6.0f;
		CharacterView.Object.Position[2] += 1.8f;
	}
	else if (CharacterView.Class == CLASS_DARK_LORD)
	{
		Vector(8.f, 0.f, -18.0f, o->Angle);
		o->Scale = 6.0f;
	}
	else if (CharacterView.Class == CLASS_SUMMONER)
	{
		Vector(2.f, 0.f, 2.0f, o->Angle);
		o->Scale = 9.1f;
		CharacterView.Object.Position[0] += 4.8f;
		CharacterView.Object.Position[2] += 4.0f;
	}
#ifdef PBG_ADD_NEWCHAR_MONK
	else if (CharacterView.Class == CLASS_RAGEFIGHTER)
	{
		o->Scale = 6.0f;
		CharacterView.Object.Position[0] += 9.8f;
		CharacterView.Object.Position[2] -= 7.5f;
	}
#endif //PBG_ADD_NEWCHAR_MONK

	RenderCharacter(&CharacterView, o);

	glViewport2(0, 0, WindowWidth, WindowHeight);

	EndOpengl();
}
