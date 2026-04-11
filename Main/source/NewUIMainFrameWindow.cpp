//////////////////////////////////////////////////////////////////////
// NewUIMainFrameWindow.cpp: implementation of the CNewUIMainFrameWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CGMProtect.h"
#include "NewUIMainFrameWindow.h"	// self
#include "NewUIOptionWindow.h"
#include "NewUISystem.h"
#include "UIBaseDef.h"
#include "DSPlaySound.h"
#include "ZzzInfomation.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "wsclientinline.h"
#include "CSItemOption.h"
#include "CSChaosCastle.h"
#include "MapManager.h"
#include "CharacterManager.h"
#include "SkillManager.h"
#include "GMDoppelGanger1.h"
#include "GMDoppelGanger2.h"
#include "GMDoppelGanger3.h"
#include "GMDoppelGanger4.h"
#include "./Time/CTimCheck.h"
#include "MonkSystem.h"
#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
#include "GameShop/InGameShopSystem.h"
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME
#include "CAIController.h"
#include "CGMInvasionManager.h"
#include "CGMResetManager.h"
#include "CGMRenderGroupMesh.h"
#include "CGMScriptLua.h"
#include "CBDrawInterface.h"

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern int  MouseUpdateTime;
extern int  MouseUpdateTimeMax;
extern int SelectedCharacter;
extern int Attacking;

#ifdef test_main_dev
CGMLuaMain* GMMainScript = NULL;
#endif // test_main_dev

SEASON3B::CNewUIMainFrameWindow::CNewUIMainFrameWindow()
{
	m_pNewUIMng = NULL;
	m_pNewUI3DRenderMng = NULL;

	m_bExpEffect = false;
	m_dwExpEffectTime = 0;
	m_dwPreExp = 0;
	m_dwGetExp = 0;
	m_loPreExp = 0;
	m_loGetExp = 0;
	m_bButtonBlink = false;
	PosX = 0.0;
	PosY = 0.0;

	this->logo_tick = std::chrono::steady_clock::now();
}

SEASON3B::CNewUIMainFrameWindow::~CNewUIMainFrameWindow()
{
	Release();
}

void SEASON3B::CNewUIMainFrameWindow::LoadImages()
{
#if MAIN_UPDATE > 603
	LoadBitmap("Interface\\HUD_TEST\\newui_main_left.dds", IMAGE_MENU_1, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_right1.dds", IMAGE_MENU_2, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_right2.dds", IMAGE_MENU_2_1, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_center1.dds", IMAGE_MENU_3, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_center2.dds", IMAGE_MENU_4, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_red.dds", IMAGE_GAUGE_RED, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_blue.dds", IMAGE_GAUGE_BLUE, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_main_hero_att.dds", IMAGE_GAUGE_AG, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\group_effra_light.jpg", BITMAP_group_effra_light, GL_LINEAR); //-- effra_light
	LoadBitmap("Interface\\HUD_TEST\\normal_effra_light.tga", BITMAP_normal_effra_light, GL_LINEAR); //-- effra_light
	LoadBitmap("Interface\\HUD_TEST\\newui_mask_left.dds", BITMAP_mask_left, GL_LINEAR);
	LoadBitmap("Interface\\HUD_TEST\\newui_mask_right.dds", BITMAP_mask_right, GL_LINEAR);
	LoadBitmap("Interface\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
	LoadBitmap("Interface\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
#else
#if MAIN_UPDATE > 303
	if (gmProtect->LookAndFeel == 1)
	{
		LoadBitmap("Interface\\HUD\\Look-1\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu01_new.jpg", IMAGE_MENU_1, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu02.jpg", IMAGE_MENU_2, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\helper_buttons.tga", IMAGE_MENU_2_1, GL_LINEAR);

		if (gmProtect->MuHelperOficial)
			LoadBitmap("Interface\\HUD\\Look-1\\Menu03_new_helper.jpg", IMAGE_MENU_3, GL_LINEAR);
		else
			LoadBitmap("Interface\\HUD\\Look-1\\Menu03_new.jpg", IMAGE_MENU_3, GL_LINEAR);

		LoadBitmap("Interface\\HUD\\Look-1\\Menu_blue.jpg", IMAGE_GAUGE_BLUE, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu_green.jpg", IMAGE_GAUGE_GREEN, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu_red.jpg", IMAGE_GAUGE_RED, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu03_new_ag.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu_character.jpg", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu_inventory.jpg", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-1\\Guild.jpg", IMAGE_MENU_BTN_GUILD, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-1\\Menu_party.jpg", IMAGE_MENU_BTN_PARTY, GL_LINEAR, GL_CLAMP_TO_EDGE);

		if (WindowWidth > 800 && gmProtect->ScreenType != 0)
			LoadBitmap("Interface\\HUD\\Look-1\\newui_menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		else
			LoadBitmap("Interface\\HUD\\Look-1\\Menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	}
	else if (gmProtect->LookAndFeel == 2)
	{
		LoadBitmap("Interface\\HUD\\Look-2\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu01_new.jpg", IMAGE_MENU_1, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu02.jpg", IMAGE_MENU_2, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\helper_buttons.tga", IMAGE_MENU_2_1, GL_LINEAR);

		if (gmProtect->MuHelperOficial)
			LoadBitmap("Interface\\HUD\\Look-1\\Menu03_new_helper.jpg", IMAGE_MENU_3, GL_LINEAR);
		else
			LoadBitmap("Interface\\HUD\\Look-1\\Menu03_new.jpg", IMAGE_MENU_3, GL_LINEAR);

		LoadBitmap("Interface\\HUD\\Look-2\\Menu_blue.jpg", IMAGE_GAUGE_BLUE, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu_green.jpg", IMAGE_GAUGE_GREEN, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu_red.jpg", IMAGE_GAUGE_RED, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu03_new_ag.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu_character.jpg", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu_inventory.jpg", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-2\\Guild.jpg", IMAGE_MENU_BTN_GUILD, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-2\\Menu_party.jpg", IMAGE_MENU_BTN_PARTY, GL_LINEAR, GL_CLAMP_TO_EDGE);

		LoadBitmap("Interface\\HUD\\Look-2\\menu01_new2.jpg", IMAGE_MENU_BTN_WINDOW, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-2\\menu01_new2_SD.jpg", IMAGE_GAUGE_SD, GL_LINEAR);

		if (WindowWidth > 800 && gmProtect->ScreenType != 0)
			LoadBitmap("Interface\\HUD\\Look-2\\newui_menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		else
			LoadBitmap("Interface\\HUD\\Look-2\\Menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	}
	else if (gmProtect->LookAndFeel == 3)
	{
		LoadBitmap("Interface\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
		LoadBitmap("Interface\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu01.tga", IMAGE_MENU_1, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu02.tga", IMAGE_MENU_2, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu03.tga", IMAGE_MENU_3, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_blue.tga", IMAGE_GAUGE_BLUE, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_green.tga", IMAGE_GAUGE_GREEN, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_red.tga", IMAGE_GAUGE_RED, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_ag.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_sd.jpg", IMAGE_GAUGE_SD, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt01.tga", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt02.tga", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt03.tga", IMAGE_MENU_BTN_FRIEND, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt04.tga", IMAGE_MENU_BTN_WINDOW, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt05.tga", IMAGE_MENU_BTN_CSHOP, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_menu_Bt06.tga", IMAGE_MENU_BTN_GUILD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	}
	else if (gmProtect->LookAndFeel == 4)
	{
		LoadBitmap("Interface\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
		LoadBitmap("Interface\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu01.tga", IMAGE_MENU_1, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu02.tga", IMAGE_MENU_2, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu03.tga", IMAGE_MENU_3, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_blue.tga", IMAGE_GAUGE_BLUE, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_green.tga", IMAGE_GAUGE_GREEN, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_red.tga", IMAGE_GAUGE_RED, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_ag.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_sd.jpg", IMAGE_GAUGE_SD, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt01.tga", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt02.tga", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt03.tga", IMAGE_MENU_BTN_FRIEND, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt04.tga", IMAGE_MENU_BTN_WINDOW, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt05.tga", IMAGE_MENU_BTN_CSHOP, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_menu_Bt06.tga", IMAGE_MENU_BTN_GUILD, GL_LINEAR, GL_CLAMP_TO_EDGE);
	}
	else if (gmProtect->LookAndFeel == 5)
	{
		LoadBitmap("Interface\\HUD\\Look-5\\Main_IE.tga", IMAGE_MENU_1, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\MainBar.tga", IMAGE_MENU_2, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\MainBack.tga", IMAGE_MENU_3, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\MainDecor.tga", IMAGE_MENU_2_1, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\Menu-Blue.tga", IMAGE_GAUGE_BLUE, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\Menu-Green.tga", IMAGE_GAUGE_GREEN, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\Menu-Red.tga", IMAGE_GAUGE_RED, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\Menu-AG.tga", IMAGE_GAUGE_AG, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\Menu-SD.tga", IMAGE_GAUGE_SD, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\none.tga", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\none.tga", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\none.tga", IMAGE_MENU_BTN_FRIEND, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\none.tga", IMAGE_MENU_BTN_WINDOW, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\HUD\\Look-5\\none.tga", IMAGE_MENU_BTN_CSHOP, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		//LoadBitmap("Interface\\HUD\\Look-5\\Menu-Button.tga", Bitmap_CashShopBtn, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		//LoadBitmap("Interface\\HUD\\Look-5\\Menu-Friend.tga", Bitmap_FriendBtn, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		//LoadBitmap("Interface\\HUD\\Look-5\\Menu-Character.tga", Bitmap_CharacterBtn, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	}
	else
	{
		LoadBitmap("Interface\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
		LoadBitmap("Interface\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu01.jpg", IMAGE_MENU_1, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu02.jpg", IMAGE_MENU_2, GL_LINEAR);
		LoadBitmap("Interface\\partCharge1\\newui_menu03.jpg", IMAGE_MENU_3, GL_LINEAR);
		LoadBitmap("Interface\\partCharge1\\newui_menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		LoadBitmap("Interface\\newui_menu02-03.jpg", IMAGE_MENU_2_1, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu_blue.jpg", IMAGE_GAUGE_BLUE, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu_green.jpg", IMAGE_GAUGE_GREEN, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu_red.jpg", IMAGE_GAUGE_RED, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu_ag.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
		LoadBitmap("Interface\\newui_menu_sd.jpg", IMAGE_GAUGE_SD, GL_LINEAR);
		LoadBitmap("Interface\\partCharge1\\newui_menu_Bt01.jpg", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\partCharge1\\newui_menu_Bt02.jpg", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\partCharge1\\newui_menu_Bt03.jpg", IMAGE_MENU_BTN_FRIEND, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\partCharge1\\newui_menu_Bt04.jpg", IMAGE_MENU_BTN_WINDOW, GL_LINEAR, GL_CLAMP_TO_EDGE);
		LoadBitmap("Interface\\partCharge1\\newui_menu_Bt05.jpg", IMAGE_MENU_BTN_CSHOP, GL_LINEAR, GL_CLAMP_TO_EDGE);
	}
#else
	LoadBitmap("Interface\\New_ui_interface.tga", IMAGE_MENU_1, GL_LINEAR);
	LoadBitmap("Interface\\t_main.TGA", IMAGE_MENU_3, GL_LINEAR);

	LoadBitmap("Interface\\Menu_Blue.tga", IMAGE_GAUGE_BLUE, GL_LINEAR);
	LoadBitmap("Interface\\Menu_Green.tga", IMAGE_GAUGE_GREEN, GL_LINEAR);
	LoadBitmap("Interface\\Menu_Red.tga", IMAGE_GAUGE_RED, GL_LINEAR);
	LoadBitmap("Interface\\Menu03_new_AG.jpg", IMAGE_GAUGE_AG, GL_LINEAR);
	LoadBitmap("Interface\\menu01_new2_SD.jpg", IMAGE_GAUGE_SD, GL_LINEAR);
	LoadBitmap("Interface\\Mu_helper_buttons.tga", IMAGE_MENU_2, GL_LINEAR);

	LoadBitmap("Interface\\Menu_party.jpg", IMAGE_MENU_BTN_PARTY, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\Menu_character.jpg", IMAGE_MENU_BTN_CHAINFO, GL_LINEAR, GL_CLAMP_TO_EDGE);
	LoadBitmap("Interface\\Menu_inventory.jpg", IMAGE_MENU_BTN_MYINVEN, GL_LINEAR, GL_CLAMP_TO_EDGE);

	LoadBitmap("Interface\\HUD\\Look-1\\newui_exbar.jpg", IMAGE_GAUGE_EXBAR, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\Look-1\\Exbar_Master.jpg", IMAGE_MASTER_GAUGE_BAR, GL_LINEAR);
#endif // MAIN_UPDATE == 303
#endif // test_main_dev

	if (gmProtect->m_RenderMuLogo != 0)
	{
		LoadBitmap("Interface\\HUD\\MU-logo.tga", BITMAP_IMAGE_FRAME_EMU, GL_LINEAR, GL_CLAMP_TO_EDGE);
	}
	LoadBitmap("Custom\\newui_button_register.tga", Bitmap_button_register, GL_LINEAR, GL_CLAMP_TO_EDGE);
	GMResetManager->LoadImages();
}

void SEASON3B::CNewUIMainFrameWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_MENU_1);
	DeleteBitmap(IMAGE_MENU_2);
	DeleteBitmap(IMAGE_MENU_3);
	DeleteBitmap(IMAGE_MENU_4);
	DeleteBitmap(IMAGE_MENU_2_1);
	DeleteBitmap(IMAGE_GAUGE_BLUE);
	DeleteBitmap(IMAGE_GAUGE_GREEN);
	DeleteBitmap(IMAGE_GAUGE_RED);
	DeleteBitmap(IMAGE_GAUGE_AG);
	DeleteBitmap(IMAGE_GAUGE_SD);
	DeleteBitmap(IMAGE_GAUGE_EXBAR);
	DeleteBitmap(IMAGE_MENU_BTN_CHAINFO);
	DeleteBitmap(IMAGE_MENU_BTN_MYINVEN);
	DeleteBitmap(IMAGE_MENU_BTN_FRIEND);
	DeleteBitmap(IMAGE_MENU_BTN_WINDOW);

	if (gmProtect->LookAndFeel >= 1 && gmProtect->LookAndFeel <= 4)
	{
		DeleteBitmap(IMAGE_MENU_BTN_GUILD);
	}
	if (gmProtect->m_RenderMuLogo != 0)
	{
		DeleteBitmap(BITMAP_IMAGE_FRAME_EMU);
	}

	GMResetManager->UnloadImages();

}

bool SEASON3B::CNewUIMainFrameWindow::Create(CNewUIManager* pNewUIMng, CNewUI3DRenderMng* pNewUI3DRenderMng)
{
	if (NULL == pNewUIMng || NULL == pNewUI3DRenderMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MAINFRAME, this);

	m_pNewUI3DRenderMng = pNewUI3DRenderMng;
	m_pNewUI3DRenderMng->Add3DRenderObj(this, ITEMHOTKEYNUMBER_CAMERA_Z_ORDER);

	PosX = pos_center(0.0);

	PosY = GetWindowsY;

#ifdef test_main_dev
	SAFE_DELETE(GMMainScript);
	GMMainScript = new CGMLuaMain;
	GMMainScript->Open();
#endif // test_main_dev

	LoadImages();

	SetButtonInfo();

	m_ItemHotKey.Init();

#ifdef test_main_dev
	if (GMMainScript)
	{
		GMMainScript->SetPos();
	}
#endif // test_main_dev

	Show(true);

	return true;
}

void SEASON3B::CNewUIMainFrameWindow::SetButtonInfo()
{
#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		float x_Next = PosX;
		float y_Next = PosY - 48;

		m_BtnWinParty.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnWinParty.ChangeButtonImgState(true, IMAGE_MENU_BTN_PARTY, true);
		m_BtnWinParty.ChangeButtonInfo(x_Next + 346.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnWinParty.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnWinParty.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnWinParty.ChangeToolTipText(GlobalText[361], true);
		//--
		m_BtnChaInfo.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnChaInfo.ChangeButtonImgState(true, IMAGE_MENU_BTN_CHAINFO, true);
		m_BtnChaInfo.ChangeButtonInfo(x_Next + 378.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeToolTipText(GlobalText[362], true);
		//--
		m_BtnMyInven.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnMyInven.ChangeButtonImgState(true, IMAGE_MENU_BTN_MYINVEN, true);
		m_BtnMyInven.ChangeButtonInfo(x_Next + 408.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeToolTipText(GlobalText[363], true);
		//--
		m_BtnWinGuild.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnWinGuild.ChangeButtonImgState(true, IMAGE_MENU_BTN_GUILD, true);
		m_BtnWinGuild.ChangeButtonInfo(x_Next + 579.0, y_Next + 26.5, 56.0, 24.0);
		m_BtnWinGuild.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnWinGuild.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnWinGuild.ChangeToolTipText(GlobalText[364], true);

		if (gmProtect->LookAndFeel == 2)
		{
			//m_BtnFriend.ChangeTextBackColor(RGBA(255, 255, 255, 0));
			//m_BtnFriend.ChangeButtonImgState(true, IMAGE_MENU_BTN_FRIEND, true);
			//m_BtnFriend.ChangeButtonInfo(x_Next + 579.0, y_Next + 1.5, 56.0, 24.0);
			//m_BtnFriend.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
			//m_BtnFriend.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
			//m_BtnFriend.ChangeToolTipText(GlobalText[1043], true);

			m_BtnWindow.ChangeTextBackColor(RGBA(255, 255, 255, 0));
			m_BtnWindow.ChangeButtonImgState(true, IMAGE_MENU_BTN_WINDOW, true);
			m_BtnWindow.ChangeButtonInfo(x_Next + 5.0, y_Next + 1.5, 56.0, 24.0);
			m_BtnWindow.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
			m_BtnWindow.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
			m_BtnWindow.ChangeToolTipText(GlobalText[1744], true);

			g_pFriendMenu->Init();
		}
	}
	else if (gmProtect->checknew_school())
	{
		float x_Next = PosX + 41;
		float y_Next = (PosY - 72) + 39;
		float x_Add = 24;
		float y_Add = 26;

		m_BtnCShop.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnCShop.ChangeButtonImgState(true, IMAGE_MENU_BTN_CSHOP, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnCShop.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnCShop.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnCShop.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnCShop.ChangeToolTipText(GlobalText[3421], true);
		x_Next += x_Add;
		m_BtnChaInfo.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnChaInfo.ChangeButtonImgState(true, IMAGE_MENU_BTN_CHAINFO, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnChaInfo.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeToolTipText(GlobalText[3423], true);
		x_Next += x_Add;
		//--
		m_BtnMyInven.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnMyInven.ChangeButtonImgState(true, IMAGE_MENU_BTN_MYINVEN, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnMyInven.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeToolTipText(GlobalText[3425], true);
		//--
		x_Next = PosX + 525;
		m_BtnWinGuild.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnWinGuild.ChangeButtonImgState(true, IMAGE_MENU_BTN_GUILD, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnWinGuild.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnWinGuild.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnWinGuild.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnWinGuild.ChangeToolTipText(GlobalText[3427], true);
		x_Next += x_Add;

		m_BtnFriend.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnFriend.ChangeButtonImgState(true, IMAGE_MENU_BTN_FRIEND, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnFriend.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnFriend.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnFriend.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnFriend.ChangeToolTipText(GlobalText[1043], true);
		x_Next += x_Add;

		m_BtnWindow.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnWindow.ChangeButtonImgState(true, IMAGE_MENU_BTN_WINDOW, true, false, false, 0.0, 0.0, 35.f, 38.f);
		m_BtnWindow.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnWindow.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnWindow.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnWindow.ChangeToolTipText(GlobalText[1744], true);
	}
	else
	{
		float x_Next = PosX + 489;
		float y_Next = PosY - 51;
		float x_Add = 30;
		float y_Add = 41;

		m_BtnCShop.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnCShop.ChangeButtonImgState(true, IMAGE_MENU_BTN_CSHOP, true);
		m_BtnCShop.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnCShop.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnCShop.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnCShop.ChangeToolTipText(GlobalText[2277], true);
		x_Next += x_Add;

		m_BtnChaInfo.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnChaInfo.ChangeButtonImgState(true, IMAGE_MENU_BTN_CHAINFO, true);
		m_BtnChaInfo.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnChaInfo.ChangeToolTipText(GlobalText[362], true);
		x_Next += x_Add;

		m_BtnMyInven.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnMyInven.ChangeButtonImgState(true, IMAGE_MENU_BTN_MYINVEN, true);
		m_BtnMyInven.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnMyInven.ChangeToolTipText(GlobalText[363], true);
		x_Next += x_Add;

		m_BtnFriend.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnFriend.ChangeButtonImgState(true, IMAGE_MENU_BTN_FRIEND, true);
		m_BtnFriend.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnFriend.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnFriend.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnFriend.ChangeToolTipText(GlobalText[1043], true);
		x_Next += x_Add;

		m_BtnWindow.ChangeTextBackColor(RGBA(255, 255, 255, 0));
		m_BtnWindow.ChangeButtonImgState(true, IMAGE_MENU_BTN_WINDOW, true);
		m_BtnWindow.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		m_BtnWindow.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
		m_BtnWindow.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));
		m_BtnWindow.ChangeToolTipText(GlobalText[1744], true);
	}
#else
	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	m_BtnWinParty.ChangeButtonInfo(RenderFrameX + 318.0, RenderFrameY + 450.5, 23.0, 22.0);
	m_BtnWinParty.ChangeToolTipText(GlobalText[361], true);

	m_BtnChaInfo.ChangeButtonInfo(RenderFrameX + 348.0, RenderFrameY + 450.5, 23.0, 22.0);
	m_BtnChaInfo.ChangeToolTipText(GlobalText[362], true);

	m_BtnMyInven.ChangeButtonInfo(RenderFrameX + 378.0, RenderFrameY + 450.5, 23.0, 22.0);
	m_BtnMyInven.ChangeToolTipText(GlobalText[363], true);
#endif // MAIN_UPDATE == 303

	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->SetPos((GetWindowsX / 2.f), 2);
	}
}

void SEASON3B::CNewUIMainFrameWindow::Release()
{
	UnloadImages();

	if (m_pNewUI3DRenderMng)
	{
		m_pNewUI3DRenderMng->Remove3DRenderObj(this);
		m_pNewUI3DRenderMng = NULL;
	}

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}

#ifdef test_main_dev
	SAFE_DELETE(GMMainScript);
#endif // test_main_dev
}

bool SEASON3B::CNewUIMainFrameWindow::Render()
{
#ifdef test_main_dev
	EnableAlphaTest(true);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (GMMainScript)
	{
		GMMainScript->Render();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	m_pNewUI3DRenderMng->RenderUI2DEffect(ITEMHOTKEYNUMBER_CAMERA_Z_ORDER, UI2DEffectCallback, this, 0, 0);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	g_pSkillList->RenderCurrentSkillAndHotSkillList();

	DisableAlphaBlend();

	return true;
#else

	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->RenderFrame((GetWindowsX / 2.f), 2);
	}

#ifdef EFFECT_KERNEL_VERTEX
	GMRenderGroupMesh->render();
#endif // EFFECT_KERNEL_VERTEX

	GMResetManager->Render();

	gCBDrawInterface->RenderFrame();

#if MAIN_UPDATE > 303
	if (gmProtect->LookAndFeel == 5)
		Render64Frame();
	else
		RenderFrame();

	m_pNewUI3DRenderMng->RenderUI2DEffect(ITEMHOTKEYNUMBER_CAMERA_Z_ORDER, UI2DEffectCallback, this, 0, 0);

	g_pSkillList->RenderCurrentSkillAndHotSkillList();

	EnableAlphaTest();

	if (gmProtect->LookAndFeel == 5)
	{
		Render64Energy();

		Render64Shield();
	}
	else
	{
		RenderLifeMana();

		RenderGuageSD();

		RenderGuageAG();

		RenderButtons();

		if (gmProtect->checkold_school())
		{
			RenderExperience(pos_center() + 221.0, pos_botton() + 439.0, 198.0, 4.f);
		}
		else if (gmProtect->checknew_school())
		{
			RenderExperience(pos_center() + 61.0, pos_botton() + 474.f, 516.f, 4.f);
		}
		else
		{
			RenderExperience(pos_center() + 2.f, pos_botton() + 473.f, 629.f, 5.f);
		}
	}
#else
	RenderFrame();

	RenderLifeMana();

	RenderGuageSD();

	RenderGuageAG();

	RenderButtons();

	RenderExperiencedown();

	m_pNewUI3DRenderMng->RenderUI2DEffect(ITEMHOTKEYNUMBER_CAMERA_Z_ORDER, UI2DEffectCallback, this, 0, 0);

	g_pSkillList->RenderCurrentSkillAndHotSkillList();
#endif // MAIN_UPDATE == 303

	if (gmProtect->m_RenderMuLogo != 0 && GrabEnable)
	{
		this->logo_tick = std::chrono::steady_clock::now();
	}
	RenderLogo();

	DisableAlphaBlend();

	return true;
#endif // test_main_dev
}

void SEASON3B::CNewUIMainFrameWindow::Render3D()
{
	m_ItemHotKey.RenderItems();

	GMResetManager->Render3d();
}

void SEASON3B::CNewUIMainFrameWindow::EventOrderWindows(double WindowsX, double WindowsY)
{
#if MAIN_UPDATE > 603
	if (GMMainScript)
	{
		GMMainScript->SetPos();
	}
#elif MAIN_UPDATE > 303
	PosX = pos_center();
	PosY = GetWindowsY;

	m_ItemHotKey.Init();

	if (gmProtect->checkold_school())
	{
		float x_Next = PosX;
		float y_Next = PosY - 48;

		m_BtnWinParty.ChangeButtonInfo(x_Next + 346.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnChaInfo.ChangeButtonInfo(x_Next + 378.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnMyInven.ChangeButtonInfo(x_Next + 408.0, y_Next + 19.0, 28.0, 30.0);
		m_BtnWinGuild.ChangeButtonInfo(x_Next + 579.0, y_Next + 26.5, 56.0, 24.0);

		if (gmProtect->LookAndFeel == 2)
		{
			m_BtnFriend.ChangeButtonInfo(x_Next + 579.0, y_Next + 1.5, 56.0, 24.0);
			m_BtnWindow.ChangeButtonInfo(x_Next + 5.0, y_Next + 1.5, 56.0, 24.0);
			g_pFriendMenu->Init();
		}

		if (WindowWidth > 800 && gmProtect->ScreenType != 0)
			LoadBitmap("Interface\\HUD\\Look-1\\newui_menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
		else
			LoadBitmap("Interface\\HUD\\Look-1\\Menu04.tga", IMAGE_MENU_4, GL_LINEAR, GL_CLAMP_TO_EDGE, true, false);
	}
	else if (gmProtect->checknew_school())
	{
		float x_Next = PosX + 41;
		float y_Next = (PosY - 72) + 39;
		float x_Add = 24;
		float y_Add = 26;

		m_BtnCShop.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnChaInfo.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnMyInven.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		//--
		x_Next = PosX + 525;
		m_BtnWinGuild.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnFriend.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnWindow.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
	}
	else
	{
		float x_Next = PosX + 489;
		float y_Next = PosY - 51;
		float x_Add = 30;
		float y_Add = 41;

		m_BtnCShop.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnChaInfo.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnMyInven.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnFriend.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
		x_Next += x_Add;
		m_BtnWindow.ChangeButtonInfo(x_Next, y_Next, x_Add, y_Add);
	}
#else
	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	m_ItemHotKey.Init();

	m_BtnWinParty.SetPos(RenderFrameX + 318.0, RenderFrameY + 450.5);

	m_BtnChaInfo.SetPos(RenderFrameX + 348.0, RenderFrameY + 450.5);

	m_BtnMyInven.SetPos(RenderFrameX + 378.0, RenderFrameY + 450.5);
#endif


	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->SetPos((GetWindowsX / 2.f), 2);
	}
}

void SEASON3B::CNewUIMainFrameWindow::UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB)
{
	g_pMainFrame->RenderHotKeyItemCount();

	GMResetManager->RenderItemNumber();

#ifdef ITEM_CHAT_SELL_SYSTEM
	g_pMainFrame->RenderItem3DPost();
#endif // ITEM_CHAT_SELL_SYSTEM
}

bool SEASON3B::CNewUIMainFrameWindow::IsVisible() const
{
	return CNewUIObj::IsVisible();
}

#if MAIN_UPDATE > 303
void RenderBouding(GLuint uiImageType, float posx, float y, float width, float height, float su, float sv, float uw, float vh, float percent, float NumberX, float NumberY, int current, int maxStat, int index)
{
	float fH = height - (percent * height);

	float RenderFrameX = posx;
	float RenderFrameY = (percent * height) + y;

	SEASON3B::RenderImage(uiImageType, RenderFrameX, RenderFrameY, width, fH, su, sv + (percent * height / vh), width / uw, (1.0f - percent) * height / vh);

	if (gmProtect->checkold_school())
	{
		glColor3f(1.0, 0.94999999, 0.75);
		SEASON3B::RenderNumber(RenderFrameX + NumberX, y + NumberY, current, 0.90);
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	else
	{
		SEASON3B::RenderNumber(RenderFrameX + NumberX, y + NumberY, current, 0.90);
	}

	if (SEASON3B::CheckMouseIn(RenderFrameX, y, width, height))
	{
		char strTipText[256];

		sprintf(strTipText, GlobalText[index], current, maxStat);

		RenderTipText(RenderFrameX, (int)(y - 16), strTipText);
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderFrame()
{
	if (gmProtect->checkold_school())
	{
		float RenderFrameX = PosX;
		float RenderFrameY = PosY - 48.0;

		RenderBitmap(IMAGE_MENU_1, RenderFrameX, RenderFrameY, 256.0, 48.0, 0, 0, 1.0, 0.75);
		RenderFrameX += 256.0;
		RenderBitmap(IMAGE_MENU_2, RenderFrameX, RenderFrameY, 128.0, 48.0, 0, 0, 1.0, 0.75);
		RenderFrameX += 128.0;
		RenderBitmap(IMAGE_MENU_3, RenderFrameX, RenderFrameY, 256.0, 48.0, 0, 0, 1.0, 0.75);

		if (GMProtect->CheckWideScreen())
		{
			RenderFrameX = PosX;
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) || g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL))
			{
				SEASON3B::RenderImageF(IMAGE_MENU_4, RenderFrameX - 52.0, RenderFrameY - 3.f, 128.f, 51.f, 0.0, 13.f, 128.f, 51.f);
				RenderFrameX += 564.0;
				SEASON3B::RenderImageF(IMAGE_MENU_4, RenderFrameX, RenderFrameY - 3.f, 128.f, 51.f, 128.f, 13.f, -128.f, 51.f);
			}
			else
			{
				SEASON3B::RenderImageF(IMAGE_MENU_4, RenderFrameX - 52.0, RenderFrameY - 16.f, 128.f, 64.f, 0.0, 0.0, 128.f, 64.f);
				RenderFrameX += 564.0;

				if (GetScreenWidth() != (int)GetWindowsX)
					SEASON3B::RenderImageF(IMAGE_MENU_4, RenderFrameX, RenderFrameY, 128.f, 48.f, 128.f, 16.f, -128.f, 48.f);
				else
					SEASON3B::RenderImageF(IMAGE_MENU_4, RenderFrameX, RenderFrameY - 16.f, 128.f, 64.f, 128.f, 0.0, -128.f, 64.f);
			}
		}
		else
		{
			if (!g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) && !g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL))
			{
				RenderFrameX = PosX;
				RenderFrameY -= 45.;
				RenderBitmap(IMAGE_MENU_4, RenderFrameX, RenderFrameY, 108.0, 45.0, 0, 0, 0.84375, 0.703125);

				if (GetScreenWidth() == (int)GetWindowsX)
				{
					RenderFrameX += 532.0;
					RenderBitmap(IMAGE_MENU_4, RenderFrameX, RenderFrameY, 108.0, 45.0, 0.84375, 0, -0.84375, 0.703125);
				}
			}
		}

		this->wope_render_btn();

		this->RenderPosition();
	}
	else
	{
		float frameX = PosX;
		float frameY = PosY - 51.0;
		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			frameY = PosY - 72.0;

			Render64LifeMana();

			SEASON3B::RenderImageF(IMAGE_MENU_1, frameX, frameY, 640.0, 72.0, 0.0, 22.0, 934.0, 106.0);

			if (g_pSkillList->IsSkillListUp() == true)
			{
				SEASON3B::RenderImageF(IMAGE_MENU_2, frameX + 444, frameY + 42.0, 6.0, 12.0, 83.0, 19.0, -12.0, 22.0);
			}
			else
			{
				SEASON3B::RenderImageF(IMAGE_MENU_2, frameX + 445, frameY + 42.0, 6.0, 12.0, 71.0, 19.0, 12.0, 22.0);
			}
		}
		else
		{
			if (WindowWidth > 800)
			{
				SEASON3B::RenderImageF(IMAGE_MENU_4, frameX - 70.28, frameY, 70.28, 51.0, 0.0, 0.0, 113.0, 82.0);
			}

			SEASON3B::RenderImage(IMAGE_MENU_1, frameX, frameY, 256.0, 51.0);
			frameX += 256.0;
			SEASON3B::RenderImage(IMAGE_MENU_2, frameX, frameY, 128.0, 51.0);
			frameX += 128.0;
			SEASON3B::RenderImage(IMAGE_MENU_3, frameX, frameY, 256.0, 51.0);
			frameX += 256.0;

			if (WindowWidth > 800)
			{
				SEASON3B::RenderImageF(IMAGE_MENU_4, frameX, frameY, 70.28, 51.0, 113.0, 0.0, -113.0, 82.0);
			}
			if (g_pSkillList->IsSkillListUp() == true)
			{
				SEASON3B::RenderImage(IMAGE_MENU_2_1, frameX - 418.0, frameY, 160.0, 40.0);
			}
		}
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderPosition()
{
	float RenderFrameX = PosX;
	float RenderFrameY = PosY - 18.0;

	glColor3f(0.60000002, 0.60000002, 0.60000002);
	SEASON3B::RenderNumber(RenderFrameX + 24.0, RenderFrameY, Hero->PositionX, 0.90);

	SEASON3B::RenderNumber(RenderFrameX + 48.0, RenderFrameY, Hero->PositionY, 0.90);
}

void SEASON3B::CNewUIMainFrameWindow::Render64LifeMana()
{
	float fLife = 0.f;
	float fMana = 0.f;

	DWORD wLifeMax = CharacterAttribute->LifeMax;
	DWORD wLife = min(max(0, CharacterAttribute->Life), wLifeMax);
	DWORD wManaMax = CharacterAttribute->ManaMax;
	DWORD wMana = min(max(0, CharacterAttribute->Mana), wManaMax);

	if (wLifeMax > 0 && (wLife > 0 && (wLife / (float)wLifeMax) < 0.2f))
	{
		PlayBuffer(SOUND_HEART);
	}

	if (wLifeMax > 0)
	{
		fLife = (wLifeMax - wLife) / (float)wLifeMax;
	}
	if (wManaMax > 0)
	{
		fMana = (wManaMax - wMana) / (float)wManaMax;
	}

	float frameX = PosX;
	float frameY = PosY - 72.0;

	double width = 60.0;
	double height = 60.0;
	double porcento = (1.0 - fLife);

	float iPosX = frameX + 108;
	float iPosY = frameY + (double)(fLife * height);
	float Height = (double)(porcento * height);

	if (gmProtect->LookAndFeel == 3)
	{
		SEASON3B::RenderImageF(IMAGE_MENU_3, iPosX, frameY, 68, 68, 0.0, 0.0, 96.0, 96.0);

		if (g_isCharacterBuff((&Hero->Object), eDeBuff_Poison))
			SEASON3B::RenderFrameAnimation(IMAGE_GAUGE_GREEN, iPosX, iPosY, width, Height, 0.0, (fLife * 128.0), 128.0, (porcento * 128.0), 2.44, 8, 64, 0.0, 128.0);
		else
			SEASON3B::RenderFrameAnimation(IMAGE_GAUGE_RED, iPosX, iPosY, width, Height, 0.0, (fLife * 128.0), 128.0, (porcento * 128.0), 2.44, 8, 64, 0.0, 128.0);

		porcento = (1.0 - fMana);

		iPosX = frameX + 465;
		iPosY = frameY + (double)(fMana * height);
		Height = (double)(porcento * height);

		SEASON3B::RenderImageF(IMAGE_MENU_3, iPosX, frameY, 68, 68, 0.0, 0.0, 96.0, 96.0);

		SEASON3B::RenderFrameAnimation(IMAGE_GAUGE_BLUE, iPosX, iPosY, width, Height, 0.0, (fMana * 128.0), 128.0, (porcento * 128.0), 2.44, 8, 64, 0.0, 128.0);

	}
	if (gmProtect->LookAndFeel == 4)
	{
		SEASON3B::RenderImageF(IMAGE_MENU_3, iPosX, frameY, width, height, 2.0, 2.0, 158.0, 158.0);

		if (g_isCharacterBuff((&Hero->Object), eDeBuff_Poison))
			RenderFrameAnimation(IMAGE_GAUGE_GREEN, iPosX, iPosY, width, Height, 0.0, (fLife * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);
		else
			RenderFrameAnimation(IMAGE_GAUGE_RED, iPosX, iPosY, width, Height, 0.0, (fLife * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);

		porcento = (1.0 - fMana);

		iPosX = frameX + 465;
		iPosY = frameY + (double)(fMana * height);
		Height = (double)(porcento * height);

		SEASON3B::RenderImageF(IMAGE_MENU_3, iPosX, frameY, width, height, 160, 2.0, -158.0, 158.0);

		SEASON3B::RenderFrameAnimation(IMAGE_GAUGE_BLUE, iPosX, iPosY, width, Height, 0.0, (fMana * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderLifeMana()
{
	float fLife = 0.f;
	float fMana = 0.f;
	int indeximg = 0;

	DWORD wLifeMax = CharacterAttribute->LifeMax;
	DWORD wLife = CharacterAttribute->Life;
	DWORD wManaMax = CharacterAttribute->ManaMax;
	DWORD wMana = CharacterAttribute->Mana;

	if (wLifeMax > 0 && (wLife > 0 && (wLife / (float)wLifeMax) < 0.2f))
	{
		PlayBuffer(SOUND_HEART);
	}

	if (wLifeMax > 0)
		fLife = (wLifeMax - wLife) / (float)wLifeMax;

	if (wManaMax > 0)
		fMana = (wManaMax - wMana) / (float)wManaMax;

	if (g_isCharacterBuff((&Hero->Object), eDeBuff_Poison))
		indeximg = IMAGE_GAUGE_GREEN;
	else
		indeximg = IMAGE_GAUGE_RED;

	if (gmProtect->LookAndFeel != 3 && gmProtect->LookAndFeel != 4)
	{
		if (gmProtect->checkold_school())
		{
			RenderBouding(indeximg, PosX + 97.0, PosY - 48.0, 53.0, 48.0, 0.0, 0.0, 64.0, 64.0, fLife, 25.0, 30, wLife, wLifeMax, 358);

			RenderBouding(IMAGE_GAUGE_BLUE, PosX + 489.0, PosY - 48.0, 53.0, 48.0, 0.0, 0.0, 64.0, 64.0, fMana, 25.0, 30, wMana, wManaMax, 359);
		}
		else
		{
			RenderBouding(indeximg, PosX + 158.0, PosY - 48.0, 45.0, 39.0, 0.0, 0.0, 64.0, 64.0, fLife, 25.0, 15, wLife, wLifeMax, 358);

			RenderBouding(IMAGE_GAUGE_BLUE, PosX + 437.0, PosY - 48.0, 45.0, 39.0, 0.0, 0.0, 64.0, 64.0, fMana, 25.0, 15, wMana, wManaMax, 359);
		}
	}
	else
	{
		float frameX = PosX + 108.0;
		float frameY = PosY - 72.0;

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		SEASON3B::RenderNumber(frameX + 30.0, frameY + 30.0, wLife, 0.8);

		if (SEASON3B::CheckMouseIn(frameX, frameY, 60.0, 60.0))
		{
			char strTipText[256];
			sprintf(strTipText, GlobalText[358], wLife, wLifeMax);
			RenderTipText(frameX, (int)(frameY - 16), strTipText);
		}

		frameX += 357.0;

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		SEASON3B::RenderNumber(frameX + 35.0, frameY + 30.0, wMana, 0.8);

		if (SEASON3B::CheckMouseIn(frameX, frameY, 60.0, 60.0))
		{
			char strTipText[256];
			sprintf(strTipText, GlobalText[359], wMana, wManaMax);
			RenderTipText(frameX, (int)(frameY - 16), strTipText);
		}
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderGuageAG()
{
	float fSkillMana = 0.0f;
	DWORD dwSkillMana = CharacterAttribute->SkillMana;
	DWORD dwMaxSkillMana = CharacterAttribute->SkillManaMax;

	if (dwMaxSkillMana > 0)
	{
		fSkillMana = (dwMaxSkillMana - dwSkillMana) / (float)dwMaxSkillMana;
	}

	if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		float frameX = PosX + 396.0;
		float frameY = (PosY - 72.0) + 19.0;

		double width = 68.0;
		double height = 9.0;
		double porcento = (1.0 - fSkillMana);

		float iPosX = frameX + (double)(fSkillMana * width);
		float Width = (double)(porcento * width);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		SEASON3B::RenderImageF(IMAGE_GAUGE_AG, iPosX, frameY, Width, height, (fSkillMana * 122.0), 0.0, (porcento * 122.0), 16.0);

		SEASON3B::RenderNumber(frameX + 35.0, frameY + 1.0, dwSkillMana, 0.8);

		if (SEASON3B::CheckMouseIn(frameX, frameY, width, height))
		{
			char strTipText[256];
			sprintf(strTipText, GlobalText[214], dwSkillMana, dwMaxSkillMana);
			RenderTipText(frameX, (int)(frameY - 16), strTipText);
		}
	}
	else
	{
		if (gmProtect->checkold_school())
		{
			RenderBouding(IMAGE_GAUGE_AG, PosX + 551.0, PosY - 42.0, 15.0, 36.0, 0.0, 0.0, 16.0, 64.f, fSkillMana, 10, 28, dwSkillMana, dwMaxSkillMana, 214);
		}
		else
		{
			RenderBouding(IMAGE_GAUGE_AG, PosX + 420.0, PosY - 49.f, 16.f, 39.f, 0.0, 0.0, 16.0, 64.f, fSkillMana, 10, 30, dwSkillMana, dwMaxSkillMana, 214);
		}
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderGuageSD()
{
	if (gmProtect->LookAndFeel != 1)
	{

		float fShield = 0.0f;
		DWORD wShield = CharacterAttribute->Shield;
		DWORD wMaxShield = CharacterAttribute->ShieldMax;

		if (wMaxShield > 0)
		{
			fShield = (wMaxShield - wShield) / (float)wMaxShield;
		}

		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			float frameX = PosX + 178.0;
			float frameY = (PosY - 72.0) + 19.0;

			double width = 68.0;
			double height = 9.0;
			double porcento = (1.0 - fShield);
			float Width = (double)(porcento * width);

			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			SEASON3B::RenderImageF(IMAGE_GAUGE_SD, frameX, frameY, Width, height, 0.0, 0.0, (porcento * 122.0), 16.0);

			SEASON3B::RenderNumber(frameX + 35.0, frameY + 1.0, wShield, 0.8);

			if (SEASON3B::CheckMouseIn(frameX, frameY, width, height))
			{
				char strTipText[256];
				sprintf(strTipText, GlobalText[2037], wShield, wMaxShield);
				RenderTipText(frameX, (int)(frameY - 16), strTipText);
			}
		}
		else if (gmProtect->LookAndFeel == 2)
		{
			float RenderFrameX = PositionX_The_Mid(640.0);
			float RenderFrameY = PositionY_Add(0);

			RenderBouding(IMAGE_GAUGE_SD, RenderFrameX + 73.0, RenderFrameY + 438.0, 15.0, 36.0, 0.0, 0.0, 16.0, 64.f, fShield, 10, 28, wShield, wMaxShield, 2037);
		}
		else
		{
			RenderBouding(IMAGE_GAUGE_SD, PosX + 204.0, PosY - 49.f, 16.f, 39.f, 0.0, 0.0, 16.0, 64.f, fShield, 10, 30, wShield, wMaxShield, 2037);
		}
	}
}

int SEASON3B::CNewUIMainFrameWindow::RenderExperience(float x, float y, float width, float height)
{
	int iExp = 0;
	__int64 dwExperience;
	__int64 dwNexExperience;
	__int64 baseExperience = 0;

	int textureExp;

	float fExpBarNum = 0.f;
	double fNeedExp = 0.0;
	double fExp = 0.0;

	bool effect_load = false;

	glColor3f(1.f, 1.f, 1.f);

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		textureExp = IMAGE_MASTER_GAUGE_BAR;
		//__int64 wLevel = (__int64)Master_Level_Data.nMLevel;
		dwExperience = (__int64)Master_Level_Data.lMasterLevel_Experince;
		dwNexExperience = (__int64)Master_Level_Data.lNext_MasterLevel_Experince;
		baseExperience = CharacterAttribute->character_experience(LEVEL_DATA_MASTER, Master_Level_Data.nMLevel);

		fNeedExp = dwNexExperience - baseExperience;
		fExp = dwExperience - baseExperience;

		if (dwExperience < baseExperience)
		{
			fExp = 0.f;
		}

		if (fExp > 0.f && fNeedExp > 0)
		{
			fExpBarNum = (fExp / fNeedExp) * 10.0;
		}

		if (m_bExpEffect == true)
		{
			fExp = m_loPreExp - baseExperience;
			effect_load = (m_loPreExp >= baseExperience);
		}
	}
	else
	{
		textureExp = IMAGE_GAUGE_EXBAR;
		dwExperience = (__int64)CharacterAttribute->Experience;
		dwNexExperience = (__int64)CharacterAttribute->NextExperince;
		baseExperience = CharacterAttribute->character_experience(LEVEL_DATA_NORMAL, CharacterAttribute->Level - 1);

		fNeedExp = dwNexExperience - baseExperience;
		fExp = dwExperience - baseExperience;

		if (dwExperience < baseExperience)
		{
			fExp = 0.f;
		}

		if (fExp > 0.f && fNeedExp > 0)
		{
			fExpBarNum = (fExp / fNeedExp) * 10.f;
		}

		if (m_bExpEffect == true)
		{
			fExp = m_dwPreExp - baseExperience;
			effect_load = (m_dwPreExp >= baseExperience);
		}
	}

	float x_Next = x;
	float Exp_width = width;

	float fProgress = fExpBarNum - (int)fExpBarNum;

	if (m_bExpEffect == true)
	{
		float fPreProgress = 0.f;

		if (m_dwPreExp >= baseExperience)
		{
			int iPreExpBarNum = 0;
			int iExpBarNum = 0;

			if (fExp > 0.f && fNeedExp > 0.f)
			{
				fPreProgress = (fExp / fNeedExp) * 10.f;
				iPreExpBarNum = (int)fPreProgress;
				fPreProgress = fPreProgress - (int)fPreProgress;
			}

			iExpBarNum = (int)fExpBarNum;

			if (iExpBarNum <= iPreExpBarNum)
			{
				float fGapProgress = 0.f;

				fGapProgress = fProgress - fPreProgress;

				Exp_width = (fPreProgress * width);
				RenderBitmap(textureExp, x, y, Exp_width, height, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

				x_Next = x;
				Exp_width += fGapProgress * width;
				RenderBitmap(textureExp, x_Next, y, Exp_width, height, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

				glColor4f(1.0, 1.0, 1.0, 0.40000001);
				RenderColor(x_Next, y, Exp_width, height, 0.0, 0);
				EndRenderColor();
			}
			else
			{
				Exp_width = fProgress * width;
				RenderBitmap(textureExp, x, y, Exp_width, height, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
				glColor4f(1.0, 1.0, 1.0, 0.40000001);
				RenderColor(x, y, Exp_width, height, 0.0, 0);
				EndRenderColor();
			}
		}
		else
		{
			Exp_width = fProgress * width;
			RenderBitmap(textureExp, x, y, Exp_width, height, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
			glColor4f(1.0, 1.0, 1.0, 0.40000001);
			RenderColor(x, y, Exp_width, height, 0.0, 0);
			EndRenderColor();
		}
	}
	else
	{
		Exp_width = fProgress * width;
		RenderBitmap(textureExp, x, y, Exp_width, height, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
	}
	iExp = (int)fExpBarNum;

	RenderBitmap(textureExp, x, y, width, height, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);

	EnableAlphaTest(true);
#if MAIN_UPDATE > 603
	SEASON3B::RenderNumber(x + width + 7, y - 1, iExp, 0.7);
#else
	if (gmProtect->checkold_school())
	{
		glColor3f(0.91000003, 0.81, 0.60000002);
		SEASON3B::RenderNumber(pos_center() + 425.0, pos_botton() + 434.0, iExp, 1.f);
		glColor3f(1.0, 1.0, 1.0);
	}
	else
	{
		SEASON3B::RenderNumber(pos_center() + 635.0, pos_botton() + 469.0, iExp, 1.f);
	}
#endif

	if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
	{
		char strTipText[256];
		x_Next = x + (width / 2) - 40;
		sprintf(strTipText, GlobalText[1748], dwExperience, dwNexExperience);
		RenderTipText(x_Next, y - 58, strTipText);
	}

	return iExp;
}
#else
void Renderfragment(int imgindex, float RenderFrameX, float RenderFrameY, float RenderWidth, float RenderHight, double Percent, int Rendermode)
{
	BITMAP_t* pText = &Bitmaps[imgindex];

	if (Rendermode == 1)
	{
		float RenderFramex = RenderFrameX + (RenderWidth * Percent);
		float Renderwidth = RenderWidth - (RenderWidth * Percent);

		float output_width = (pText->output_height * Percent);

		RenderBitmap(imgindex, RenderFramex, RenderFrameY, Renderwidth, RenderHight, output_width / pText->Width, 0.0, (pText->output_width - output_width) / pText->Width, pText->output_height / pText->Height, true, true, 0.0);
	}
	else if (Rendermode == 2)
	{
		float RenderFramex = RenderFrameX + (RenderWidth * Percent);
		float Renderwidth = RenderWidth - (RenderWidth * Percent);

		float output_width = (pText->output_height * Percent);

		RenderBitmap(imgindex, RenderFramex, RenderFrameY, Renderwidth, RenderHight, 0.0, 0.0, (pText->output_width - output_width) / pText->Width, pText->output_height / pText->Height, true, true, 0.0);
	}
	else
	{
		float RenderFramey = RenderFrameY + (RenderHight * Percent);
		float Renderheight = RenderHight - (RenderHight * Percent);

		float output_height = (pText->output_height * Percent);

		RenderBitmap(imgindex, RenderFrameX, RenderFramey, RenderWidth, Renderheight, 0.0, output_height / pText->Height, pText->output_width / pText->Width, (pText->output_height - output_height) / pText->Height, true, true, 0.0);
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderFrame()
{
	float RenderFrameX = pos_center(0);
	float RenderFrameY = PositionY_Add(0);

	if (Hero->SafeZone)
	{
		g_pRenderText->SetBgColor(0x0);
		g_pRenderText->SetTextColor(-1);
		g_pRenderText->SetFont(g_hFontBold);

		if (CheckMouseIn(RenderFrameX + 108.0, RenderFrameY + 448.0, 50.f, 20.0) && MouseLButton)
			SEASON3B::RenderImage(Bitmap_text_button2, RenderFrameX + 108.0, RenderFrameY + 448.0, 50.f, 20.0, 0 / 64.0, 0 / 32.f, 56.0 / 64.f, 24.0 / 32.f); //-- 
		else
			SEASON3B::RenderImage(Bitmap_text_button1, RenderFrameX + 108.0, RenderFrameY + 448.0, 50.f, 20.0, 0 / 64.0, 0 / 32.f, 56.0 / 64.f, 24.0 / 32.f); //-- 
		g_pRenderText->RenderText(RenderFrameX + 108.0, RenderFrameY + 448.0, GlobalText[2279], 50.f, 20.0, RT3_SORT_CENTER);

		if (CheckMouseIn(RenderFrameX + 482.0, RenderFrameY + 448.0, 50.f, 20.0) && MouseLButton)
			SEASON3B::RenderImage(Bitmap_text_button2, RenderFrameX + 482.0, RenderFrameY + 448.0, 50.f, 20.0, 0 / 64.0, 0 / 32.f, 56.0 / 64.f, 24.0 / 32.f); //-- 
		else
			SEASON3B::RenderImage(Bitmap_text_button1, RenderFrameX + 482.0, RenderFrameY + 448.0, 50.f, 20.0, 0 / 64.0, 0 / 32.f, 56.0 / 64.f, 24.0 / 32.f); //-- 
		g_pRenderText->RenderText(RenderFrameX + 482.0, RenderFrameY + 448.0, GlobalText[2890], 50.f, 20.0, RT3_SORT_CENTER);
	}

	SEASON3B::RenderImageF(IMAGE_MENU_3, RenderFrameX + 98.00, RenderFrameY + 461.0, 62.0, 20.0, 0.0, 0.0, 113.0, 36.0);
	SEASON3B::RenderImageF(IMAGE_MENU_3, RenderFrameX + 480.0, RenderFrameY + 461.0, 62.0, 20.0, 113.0, 0.0, -113.0, 36.0);
	//-- scale: 1.8125
	SEASON3B::RenderImageF(IMAGE_MENU_1, RenderFrameX + 152.0, RenderFrameY + 432.0, 336.0, 48.0, 0.0, 0.0, 550.0, 87.0);


	//-- Mu Helper
	if (gmAIController->IsRunning())
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0) && MouseLButton)
			SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0, 32.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- play
		else
			SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0, 32.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- play
	}
	else
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0) && MouseLButton)
			SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0, 1.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- play
		else
			SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0, 1.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- play
	}

	if (SEASON3B::CheckMouseIn(RenderFrameX + 407.0, RenderFrameY + 462.0, 18.0, 11.0) && MouseLButton)
		SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 462.0, 18.0, 11.0, 1.0 / 120.0, 26.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- config
	else
		SEASON3B::RenderImage(IMAGE_MENU_2, RenderFrameX + 407.0, RenderFrameY + 462.0, 18.0, 11.0, 1.0 / 120.0, 26.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- config

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_PARTY))
	{
		SEASON3B::RenderImage(IMAGE_MENU_BTN_PARTY, RenderFrameX + 318.0, RenderFrameY + 450.5, 23.0, 22.0, 0.0, 0.0, 24.0 / 32.0, 24.0 / 32.0); //-- Party
	}

	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER))
	{
		SEASON3B::RenderImage(IMAGE_MENU_BTN_CHAINFO, RenderFrameX + 348.0, RenderFrameY + 450.5, 23.0, 22.0, 0.0, 0.0, 24.0 / 32.0, 24.0 / 32.0); //-- Character
	}
	//
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY))
	{
		SEASON3B::RenderImage(IMAGE_MENU_BTN_MYINVEN, RenderFrameX + 378.0, RenderFrameY + 450.5, 23.0, 22.0, 0.0, 0.0, 24.0 / 32.0, 24.0 / 32.0); //-- Inventory
	}

	RenderPosition();
}

void SEASON3B::CNewUIMainFrameWindow::RenderPosition()
{
	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0) + 472.0;

	glColor3f(0.60000002, 0.60000002, 0.60000002);
	SEASON3B::RenderNumber(RenderFrameX + 132.0, RenderFrameY, Hero->PositionX, 0.75);

	SEASON3B::RenderNumber(RenderFrameX + 148.0, RenderFrameY, Hero->PositionY, 0.75);

	glColor3f(1.0, 1.0, 1.0);

	unicode::t_char szDebugText[128];
	unicode::_sprintf(szDebugText, "FPS : %.1f", FPS);
	//unicode::t_char szMousePos[128];
	//unicode::_sprintf(szMousePos, "MousePos : %d %d %d", (int)MouseX, (int)MouseY, MouseLButtonPush);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(255, 255, 255, 200);
	g_pRenderText->RenderFont(RenderFrameX + 488.0, RenderFrameY, szDebugText, 62, 0, 1);
	//g_pRenderText->RenderText(10, 36, szMousePos);
	g_pRenderText->SetFont(g_hFont);
}

void SEASON3B::CNewUIMainFrameWindow::RenderLifeMana()
{
	float fLife = 0.f;
	float fMana = 0.f;
	int indeximg = 0;

	DWORD wLifeMax = CharacterAttribute->LifeMax;
	DWORD wLife = CharacterAttribute->Life;
	DWORD wManaMax = CharacterAttribute->ManaMax;
	DWORD wMana = CharacterAttribute->Mana;

	if (wLifeMax > 0 && (wLife > 0 && (wLife / (float)wLifeMax) < 0.2f))
	{
		PlayBuffer(SOUND_HEART);
	}

	if (wLifeMax > 0)
		fLife = (wLifeMax - wLife) / (float)wLifeMax;

	if (wManaMax > 0)
		fMana = (wManaMax - wMana) / (float)wManaMax;

	if (g_isCharacterBuff((&Hero->Object), eDeBuff_Poison))
		indeximg = IMAGE_GAUGE_GREEN;
	else
		indeximg = IMAGE_GAUGE_RED;

	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	Renderfragment(indeximg, RenderFrameX + 152.0, RenderFrameY + 432.0, 53.0, 41.0, fLife, 0);

	indeximg = IMAGE_GAUGE_BLUE;

	Renderfragment(indeximg, RenderFrameX + 435.0, RenderFrameY + 432.0, 53.0, 41.0, fMana, 0);


	char strTipText[256];

	if (SEASON3B::CheckMouseIn(RenderFrameX + 152.0, RenderFrameY + 432.0, 53.0, 41.0))
	{
		sprintf(strTipText, GlobalText[358], wLife, wLifeMax);

		RenderTipText(RenderFrameX + 152.0, RenderFrameY + 412.0, strTipText);
	}

	if (SEASON3B::CheckMouseIn(RenderFrameX + 435.0, RenderFrameY + 432.0, 53.0, 41.0))
	{
		sprintf(strTipText, GlobalText[359], wMana, wManaMax);

		RenderTipText(RenderFrameX + 435.0, RenderFrameY + 412.0, strTipText);
	}

	glColor3f(1.0, 0.94999999, 0.75);

	SEASON3B::RenderNumber(RenderFrameX + 176.0, RenderFrameY + 465.0, wLife, 0.90);

	SEASON3B::RenderNumber(RenderFrameX + 461.0, RenderFrameY + 465.0, wMana, 0.90);
}

void SEASON3B::CNewUIMainFrameWindow::RenderGuageAG()
{
	float fSkillMana = 0.0f;
	DWORD dwSkillMana = CharacterAttribute->SkillMana;
	DWORD dwMaxSkillMana = CharacterAttribute->SkillManaMax;

	if (dwMaxSkillMana > 0)
	{
		fSkillMana = (dwMaxSkillMana - dwSkillMana) / (float)dwMaxSkillMana;
	}

	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	glColor3f(1.0, 1.0, 1.0);

	Renderfragment(IMAGE_GAUGE_AG, RenderFrameX + 322.0, RenderFrameY + 434.5, 75.0, 8.0, fSkillMana, 1);

	if (SEASON3B::CheckMouseIn(RenderFrameX + 322.0, RenderFrameY + 434.5, 75.0, 8.0))
	{
		char strTipText[256];
		sprintf(strTipText, GlobalText[214], dwSkillMana, dwMaxSkillMana);
		RenderTipText(RenderFrameX + 322.0, RenderFrameY + 420, strTipText);
	}

	glColor3f(1.0, 0.94999999, 0.75);

	SEASON3B::RenderNumber(RenderFrameX + 357.0, RenderFrameY + 434.5, dwSkillMana, 0.8);
}

void SEASON3B::CNewUIMainFrameWindow::RenderGuageSD()
{
	float fShield = 0.0f;
	DWORD wShield = CharacterAttribute->Shield;
	DWORD wMaxShield = CharacterAttribute->ShieldMax;

	if (wMaxShield > 0)
	{
		fShield = (wMaxShield - wShield) / (float)wMaxShield;
	}


	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	glColor3f(1.0, 1.0, 1.0);

	Renderfragment(IMAGE_GAUGE_SD, RenderFrameX + 243.0, RenderFrameY + 434.5, 75.0, 8.0, fShield, 2);

	if (SEASON3B::CheckMouseIn(RenderFrameX + 243.0, RenderFrameY + 434.5, 75.0, 8.0))
	{
		char strTipText[256];
		sprintf(strTipText, GlobalText[2037], wShield, wMaxShield);
		RenderTipText(RenderFrameX + 243.0, RenderFrameY + 420, strTipText);
	}

	glColor3f(1.0, 0.94999999, 0.75);

	SEASON3B::RenderNumber(RenderFrameX + 275.0, RenderFrameY + 434.5, wShield, 0.8);
}

void SEASON3B::CNewUIMainFrameWindow::RenderExperiencedown()
{
	__int64 wLevel;
	__int64 dwNexExperience;
	__int64 dwExperience;

	double x_Next = 0.0;
	double Expwidth = 0.0;

	double RenderFrameX = PositionX_The_Mid(640.0) + 201.0;
	double RenderFrameY = PositionY_Add(0) + 476.5;
	double Expheight = 3.0;
	double ExpBar = 212.0;

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= CharacterAttribute->MaxCharacterLevel)
	{
		wLevel = (__int64)Master_Level_Data.nMLevel;
		dwNexExperience = (__int64)Master_Level_Data.lNext_MasterLevel_Experince;
		dwExperience = (__int64)Master_Level_Data.lMasterLevel_Experince;
	}
	else
	{
		wLevel = CharacterAttribute->Level;
		dwNexExperience = CharacterAttribute->NextExperince;
		dwExperience = CharacterAttribute->Experience;
	}

	int iExp = 0;

	if (gCharacterManager.IsMasterLevel(CharacterAttribute->Class) == true && CharacterAttribute->Level >= 400)
	{
		__int64 iTotalLevel = wLevel + 400;
		__int64 iTOverLevel = iTotalLevel - 255;
		__int64 iBaseExperience = 0;

		__int64 iData_Master =
			(((__int64)9 + (__int64)iTotalLevel) * (__int64)iTotalLevel * (__int64)iTotalLevel * (__int64)10)
			+
			(((__int64)9 + (__int64)iTOverLevel) * (__int64)iTOverLevel * (__int64)iTOverLevel * (__int64)1000);

		iBaseExperience = (iData_Master - (__int64)3892250000) / (__int64)2;

		double fNeedExp = (double)dwNexExperience - (double)iBaseExperience;
		double fExp = (double)dwExperience - (double)iBaseExperience;

		if (dwExperience < iBaseExperience)
		{
			fExp = 0.f;
		}

		double fExpBarNum = 0.f;

		if (fExp > 0.f && fNeedExp > 0)
		{
			fExpBarNum = ((double)fExp / (double)fNeedExp) * (double)10.f;
		}

		double fProgress = fExpBarNum - __int64(fExpBarNum);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		if (m_bExpEffect == true)
		{
			double fPreProgress = 0.f;
			double fPreExp = (double)m_loPreExp - (double)iBaseExperience;

			if (m_loPreExp >= iBaseExperience)
			{
				int iPreExpBarNum = 0;
				int iExpBarNum = 0;
				if (fPreExp > 0.f && fNeedExp > 0.f)
				{
					fPreProgress = ((double)fPreExp / (double)fNeedExp) * (double)10.f;
					iPreExpBarNum = (int)fPreProgress;
					fPreProgress = (double)fPreProgress - __int64(fPreProgress);
				}
				iExpBarNum = (int)fExpBarNum;

				if (iExpBarNum <= iPreExpBarNum)
				{
					double fGapProgress = (fProgress - fPreProgress);

					Expwidth = (fPreProgress * ExpBar);
					RenderBitmap(IMAGE_MASTER_GAUGE_BAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

					//x_Next = Expwidth + RenderFrameX;
					Expwidth += fGapProgress * ExpBar;
					RenderBitmap(IMAGE_MASTER_GAUGE_BAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

					glColor4f(1.0, 1.0, 1.0, 0.60000002);
					RenderColor(RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0);
					EndRenderColor();
				}
				else
				{
					Expwidth = fProgress * ExpBar;
					RenderBitmap(IMAGE_MASTER_GAUGE_BAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);
					glColor4f(1.0, 1.0, 1.0, 0.60000002);
					RenderColor(RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0);
					EndRenderColor();
				}
			}
			else
			{
				Expwidth = fProgress * ExpBar;
				RenderBitmap(IMAGE_MASTER_GAUGE_BAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);
				glColor4f(1.0, 1.0, 1.0, 0.60000002);
				RenderColor(RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0);
				EndRenderColor();
			}
		}
		else
		{
			Expwidth = fProgress * ExpBar;
			RenderBitmap(IMAGE_MASTER_GAUGE_BAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);
		}

		iExp = (int)fExpBarNum;
	}
	else
	{
		WORD wPriorLevel = wLevel - 1;
		DWORD dwPriorExperience = 0;

		if (CharacterAttribute->MaxCharacterLevel > 400)
		{
			if (wPriorLevel > 0)
			{
				float normalized_level = static_cast<float>(wPriorLevel) / CharacterAttribute->MaxCharacterLevel * 400.0f;

				float baseExperience = (9.0f + normalized_level) * normalized_level * normalized_level * 10.0f;

				if (normalized_level > 255.0f)
				{
					float LevelOver_N = normalized_level - 255.0f;
					baseExperience += (9.0f + LevelOver_N) * LevelOver_N * LevelOver_N * 1000.0f;
				}

				dwPriorExperience = static_cast<DWORD>(baseExperience);
			}
		}
		else
		{
			if (wPriorLevel > 0)
			{
				dwPriorExperience = (9 + wPriorLevel) * wPriorLevel * wPriorLevel * 10;

				if (wPriorLevel > 255)
				{
					int iLevelOverN = wPriorLevel - 255;
					dwPriorExperience += (9 + iLevelOverN) * iLevelOverN * iLevelOverN * 1000;
				}
			}
		}

		float fNeedExp = dwNexExperience - dwPriorExperience;
		float fExp = dwExperience - dwPriorExperience;

		if (dwExperience < dwPriorExperience)
		{
			fExp = 0.f;
		}

		float fExpBarNum = 0.f;
		if (fExp > 0.f && fNeedExp > 0)
		{
			fExpBarNum = (fExp / fNeedExp) * 10.f;
		}

		float fProgress = fExpBarNum;
		fProgress = fProgress - (int)fProgress;

		if (m_bExpEffect == true)
		{
			float fPreProgress = 0.f;
			fExp = m_dwPreExp - dwPriorExperience;


			if (m_dwPreExp >= dwPriorExperience)
			{
				int iPreExpBarNum = 0;
				int iExpBarNum = 0;
				if (fExp > 0.f && fNeedExp > 0.f)
				{
					fPreProgress = (fExp / fNeedExp) * 10.f;
					iPreExpBarNum = (int)fPreProgress;
					fPreProgress = fPreProgress - (int)fPreProgress;
				}

				iExpBarNum = (int)fExpBarNum;

				if (iExpBarNum <= iPreExpBarNum)
				{
					float fGapProgress = 0.f;

					fGapProgress = fProgress - fPreProgress;

					Expwidth = (fPreProgress * ExpBar);
					RenderBitmap(IMAGE_GAUGE_EXBAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

					x_Next = RenderFrameX;
					Expwidth += fGapProgress * ExpBar;
					RenderBitmap(IMAGE_GAUGE_EXBAR, x_Next, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, 1, 1, 0.0);

					glColor4f(1.0, 1.0, 1.0, 0.40000001);
					RenderColor(x_Next, RenderFrameY, Expwidth, Expheight, 0.0, 0);
					EndRenderColor();
				}
				else
				{
					Expwidth = fProgress * ExpBar;
					RenderBitmap(IMAGE_GAUGE_EXBAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
					glColor4f(1.0, 1.0, 1.0, 0.40000001);
					RenderColor(RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0);
					EndRenderColor();
				}
			}
			else
			{
				Expwidth = fProgress * ExpBar;
				RenderBitmap(IMAGE_GAUGE_EXBAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
				glColor4f(1.0, 1.0, 1.0, 0.40000001);
				RenderColor(RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0);
				EndRenderColor();
			}
		}
		else
		{
			Expwidth = fProgress * ExpBar;
			RenderBitmap(IMAGE_GAUGE_EXBAR, RenderFrameX, RenderFrameY, Expwidth, Expheight, 0.0, 0.0, 0.75, 1.0, true, true, 0.0);
		}
		iExp = (int)fExpBarNum;
	}


	EnableAlphaTest(true);
	glColor3f(0.91000003, 0.81, 0.60000002);
	SEASON3B::RenderNumber(RenderFrameX + 239, RenderFrameY - 4, iExp, 0.8);
	DisableAlphaBlend();
	glColor3f(1.0, 1.0, 1.0);

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, ExpBar, Expheight) == true)
	{
		char strTipText[256];
		x_Next = RenderFrameX + (ExpBar / 2) - 40;
		sprintf(strTipText, GlobalText[1748], dwExperience, dwNexExperience);
		RenderTipText(x_Next, RenderFrameY - 58, strTipText);
	}
}
#endif

void SEASON3B::CNewUIMainFrameWindow::RenderHotKeyItemCount()
{
	m_ItemHotKey.RenderItemCount();
}

#if MAIN_UPDATE > 303
void SEASON3B::CNewUIMainFrameWindow::RenderButtons()
{
	if (gmProtect->checkold_school())
	{
		float RenderFrameX = pos_center();
		float RenderFrameY = PositionY_Add(0);

		//-- party
		if (SEASON3B::CheckMouseIn(RenderFrameX + 348.0, RenderFrameY + 452.0, 24.0, 24.0))
		{
			RenderTipText(RenderFrameX + 348, RenderFrameY + 417, GlobalText[361]);
		}
		if (g_pNewUISystem->IsVisible(INTERFACE_PARTY))
		{
			RenderBitmap(IMAGE_MENU_BTN_PARTY, RenderFrameX + 348.0, RenderFrameY + 452.0, 24.0, 24.0, 0, 0, 0.75, 0.75, true, true, 0.0);
		}
		//-- character
		if (SEASON3B::CheckMouseIn(RenderFrameX + 379.0, RenderFrameY + 452.0, 24.0, 24.0))
		{
			RenderTipText(RenderFrameX + 379, RenderFrameY + 417, GlobalText[362]);
		}
		if (g_pNewUISystem->IsVisible(INTERFACE_CHARACTER))
		{
			RenderBitmap(IMAGE_MENU_BTN_CHAINFO, RenderFrameX + 379.0, RenderFrameY + 452.0, 24.0, 24.0, 0, 0, 0.75, 0.75, true, true, 0.0);
		}
		//-- inventory
		if (SEASON3B::CheckMouseIn(RenderFrameX + 410.0, RenderFrameY + 452.0, 24.0, 24.0))
		{
			RenderTipText(RenderFrameX + 410, RenderFrameY + 417, GlobalText[363]); // Inventory (I,V)
		}
		if (g_pNewUISystem->IsVisible(INTERFACE_INVENTORY))
		{
			RenderBitmap(IMAGE_MENU_BTN_MYINVEN, RenderFrameX + 410.0, RenderFrameY + 452.0, 24.0, 24.0, 0, 0, 0.75, 0.75, true, true, 0.0);
		}
		//-- guild
		if (SEASON3B::CheckMouseIn(RenderFrameX + 582.0, RenderFrameY + 459.0, 52.0, 18.0))
		{
			RenderTipText(RenderFrameX + 582, RenderFrameY + 444, GlobalText[364]); // Guild (G)
		}
		if (g_pNewUISystem->IsVisible(INTERFACE_GUILDINFO))
		{
			RenderBitmap(IMAGE_MENU_BTN_GUILD, RenderFrameX + 582.0, RenderFrameY + 459.0, 52.0, 18.0, 0, 0, 0.8125, 0.5625, true, true, 0.0);
		}

		if (gmProtect->LookAndFeel == 2)
		{
			//-- command
			if (SEASON3B::CheckMouseIn(RenderFrameX + 6.0, RenderFrameY + 435.0, 53.0, 19.0))
			{
				RenderTipText(RenderFrameX + 6.0, RenderFrameY + 417, GlobalText[939]); // Command (D)
			}
			if (g_pNewUISystem->IsVisible(INTERFACE_COMMAND))
			{
				RenderBitmap(IMAGE_MENU_BTN_WINDOW, RenderFrameX + 6.0, RenderFrameY + 435.0, 53.0, 19.0, 0, 0, 0.828125, 0.59375, true, true, 0.0);
			}

			g_pFriendMenu->RenderFriendButton();
		}
	}
	else if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		RenderCharInfoButton();

		//m_BtnCShop.Render(35.0, 38.0);
		//
		//m_BtnMyInven.Render(35.0, 38.0);
		//
		//m_BtnWinGuild.Render(35.0, 38.0); //-- quest

		m_BtnCShop.Render();

		m_BtnMyInven.Render();

		m_BtnWinGuild.Render(); //-- quest

		RenderFriendButton();

		//m_BtnWindow.Render(35.0, 38.0);
		m_BtnWindow.Render();
	}
	else
	{
		m_BtnCShop.Render();

		RenderCharInfoButton();

		m_BtnMyInven.Render();

		RenderFriendButton();

		m_BtnWindow.Render();
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderCharInfoButton()
{
	if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		//m_BtnChaInfo.Render(35.0, 38.0);
		m_BtnChaInfo.Render();
	}
	else
	{
		m_BtnChaInfo.Render();

		if (g_QuestMng.IsQuestIndexByEtcListEmpty())
			return;

		if (g_Time.GetTimeCheck(5, 500))
			m_bButtonBlink = !m_bButtonBlink;

		if (m_bButtonBlink)
		{
			if (!(g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)
				|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER)))
			{
				if (gmProtect->checkold_school())
				{
					RenderImage(IMAGE_MENU_BTN_CHAINFO, PosX + 378.0, (PosY - 51.0) + 19.0, 28.0, 30.0, 0.0f, 30.0);
				}
				else
				{
					RenderImage(IMAGE_MENU_BTN_CHAINFO, PosX + 489 + 30.0, (PosY - 51.0), 30, 41, 0.0f, 41.f);
				}
			}
		}
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderFriendButton()
{
	if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		//m_BtnFriend.Render(35.0, 38.0);
		m_BtnFriend.Render();
	}
	else
	{
		m_BtnFriend.Render();

		int iBlinkTemp = g_pFriendMenu->GetBlinkTemp();
		BOOL bIsAlertTime = (iBlinkTemp % 24 < 12);

		if (g_pFriendMenu->IsNewChatAlert() && bIsAlertTime)
		{
			RenderFriendButtonState();
		}
		if (g_pFriendMenu->IsNewMailAlert())
		{
			if (bIsAlertTime)
			{
				RenderFriendButtonState();

				if (iBlinkTemp % 24 == 11)
				{
					g_pFriendMenu->IncreaseLetterBlink();
				}
			}
		}
		else if (g_pLetterList->CheckNoReadLetter())
		{
			RenderFriendButtonState();
		}

		g_pFriendMenu->IncreaseBlinkTemp();
	}
}

void SEASON3B::CNewUIMainFrameWindow::RenderFriendButtonState()
{
	if (gmProtect->checkold_school())
	{
		if (gmProtect->LookAndFeel == 2)
		{
			float x_Next = PosX + 579.0;
			float y_Next = (PosY - 51) + 1.5;

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_FRIEND) == true)
			{
				RenderImage(IMAGE_MENU_BTN_FRIEND, x_Next, y_Next, 56.0, 24.0, 0.0f, 72.0);
			}
			else
			{
				RenderImage(IMAGE_MENU_BTN_FRIEND, x_Next, y_Next, 56.0, 24.0, 0.0f, 24.0);
			}
		}
	}
	else
	{
		if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_FRIEND) == true)
		{
			RenderImage(IMAGE_MENU_BTN_FRIEND, PosX + 489 + (30 * 3), PosY - 51, 30, 41, 0.0f, 123.f);
		}
		else
		{
			RenderImage(IMAGE_MENU_BTN_FRIEND, PosX + 489 + (30 * 3), PosY - 51, 30, 41, 0.0f, 41.f);
		}
	}
}
#else
void SEASON3B::CNewUIMainFrameWindow::RenderButtons()
{
	m_BtnMyInven.Render();

	m_BtnChaInfo.Render();

	m_BtnWinParty.Render();
}
#endif

void SEASON3B::CNewUIMainFrameWindow::RenderLogo()
{
	if (gmProtect->m_RenderMuLogo != 0)
	{
		auto runtimeThread = std::chrono::steady_clock::now();
		double difTime = std::chrono::duration<double>(runtimeThread - this->logo_tick).count();

		if (difTime < 1.5)
		{
			EnableAlphaTest(true);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			if (gmProtect->m_RenderMuLogo == 1)
			{
				SEASON3B::RenderImage(BITMAP_IMAGE_FRAME_EMU, 10, 10, 150, 114);
			}
			if (gmProtect->m_RenderMuLogo == 2)
			{
				SEASON3B::RenderImage(BITMAP_IMAGE_FRAME_EMU, GetWindowsX - 160, 10, 150, 114);
			}
			if (gmProtect->m_RenderMuLogo == 3)
			{
				SEASON3B::RenderImage(BITMAP_IMAGE_FRAME_EMU, 10, GetWindowsY - 144, 150, 114);
			}
			if (gmProtect->m_RenderMuLogo == 4)
			{
				SEASON3B::RenderImage(BITMAP_IMAGE_FRAME_EMU, GetWindowsX - 160, GetWindowsY - 144, 150, 114);
			}
			if (gmProtect->m_RenderMuLogo == 5)
			{
				SEASON3B::RenderImage(BITMAP_IMAGE_FRAME_EMU, PositionX_The_Mid(150), PositionY_The_Mid(114), 150, 114);
			}
		}
	}
}

#ifdef ITEM_CHAT_SELL_SYSTEM
void SEASON3B::CNewUIMainFrameWindow::RenderItem3DPost()
{
	ITEM* dwIndentItem = g_pChatListBox->GetMsgsIndent();

	if (dwIndentItem)
	{
		g_pNewItemTooltip->RenderItemTooltip(300, 100, dwIndentItem, false, 0, false, true);
	}
}
#endif // ITEM_CHAT_SELL_SYSTEM

void SEASON3B::CNewUIMainFrameWindow::Render64Buttons()
{
}

void SEASON3B::CNewUIMainFrameWindow::Render64Frame()
{
	float RenderFrameX = PositionX_In_The_Mid(0.0);
	float RenderFrameY = Position_In_The_Down(78.f);

	this->RenderLife64();

	RenderImageF(IMAGE_MENU_1, RenderFrameX + 95.f, RenderFrameY - 8.0, 450.f, 80.f, 0.0, 4.0, 988.f, 176.f);
	//--
	RenderImageF(IMAGE_MENU_2, RenderFrameX, RenderFrameY + 69, 640.f, 9.f, 0.0, 0.0, 1024, 24.0);
	//--
	RenderImageF(IMAGE_MENU_2_1, RenderFrameX + 2.000, RenderFrameY + 51.0, 60.0, 20.0, 120.0, 0.0, -120.0, 39.0);

	RenderImageF(IMAGE_MENU_2_1, RenderFrameX + 578.0, RenderFrameY + 51.0, 60.0, 20.0, 0.000, 0.0, +120.0, 39.0);

	this->Render64LifeHub();
}

void SEASON3B::CNewUIMainFrameWindow::RenderLife64()
{
	float fLife = 0.f, fMana = 0.f;

	DWORD wLifeMax, wLife, wManaMax, wMana;

	wLifeMax = CharacterAttribute->LifeMax; wLife = CharacterAttribute->Life;
	wManaMax = CharacterAttribute->ManaMax; wMana = CharacterAttribute->Mana;

	float RenderFrameX = (float)PositionX_In_The_Mid(0);
	float RenderFrameY = (float)Position_In_The_Down(80.f);

	if (wLifeMax > 0)
	{
		if (wLife > 0 && (wLife / (float)wLifeMax) < 0.2f)
			PlayBuffer(SOUND_HEART, 0, 0);
	}

	if (wLifeMax > 0)
		fLife = (wLifeMax - wLife) / (float)wLifeMax;

	if (wManaMax > 0)
		fMana = (wManaMax - wMana) / (float)wManaMax;

	double width = 68.0;
	double height = 65.0;
	double porcento = (1.0 - fLife);

	float PosX = RenderFrameX + 131;
	float PosY = RenderFrameY + (double)(fLife * height);
	float Height = (double)(porcento * height);

	RenderImageF(IMAGE_MENU_3, PosX, RenderFrameY, width, height, 2.0, 2.0, 158.0, 158.0);

	if (!g_isCharacterBuff((&Hero->Object), eDeBuff_Poison))
		RenderFrameAnimation(IMAGE_GAUGE_RED, PosX, PosY, width, Height, 0.0, (fLife * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);
	else
		RenderFrameAnimation(IMAGE_GAUGE_GREEN, PosX, PosY, width, Height, 0.0, (fLife * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);

	porcento = (1.0 - fMana);

	PosX = RenderFrameX + 442;
	PosY = RenderFrameY + (double)(fMana * height);
	Height = (double)(porcento * height);

	RenderImageF(IMAGE_MENU_3, PosX, RenderFrameY, width, height, 160, 2.0, -158.0, 158.0);

	RenderFrameAnimation(IMAGE_GAUGE_BLUE, PosX, PosY, width, Height, 0.0, (fMana * 152.0), 152.0, (porcento * 152.0), 1.44, 6, 36, 0.0, 152.0);
}

void SEASON3B::CNewUIMainFrameWindow::Render64LifeHub()
{
	DWORD LifeCur = CharacterAttribute->Life;
	DWORD ManaCur = CharacterAttribute->Mana;
	DWORD LifeMax = CharacterAttribute->LifeMax;
	DWORD ManaMax = CharacterAttribute->ManaMax;
	float RenderFrameX = PositionX_In_The_Mid(0.0);
	float RenderFrameY = Position_In_The_Down(78.f);

	glColor4f(0.8, 0.8, 0.8, 1.f);

	SEASON3B::RenderNumber(RenderFrameX + 165.0, RenderFrameY + 63.0, LifeCur, 0.85);

	SEASON3B::RenderNumber(RenderFrameX + 480.0, RenderFrameY + 63.0, ManaCur, 0.85);

	char strText[256];
	memset(strText, 0, sizeof(strText));

	if (SEASON3B::CheckMouseIn(RenderFrameX + 131, RenderFrameY - 2, 68, 65))
	{
		sprintf_s(strText, GlobalText[358], LifeCur, LifeMax);
		RenderTipText((int)RenderFrameX + 131, (int)RenderFrameY - 20, strText);
		glColor4f(1.0, 1.0, 1.0, 1.0);
	}

	if (SEASON3B::CheckMouseIn(RenderFrameX + 442, RenderFrameY - 2, 68, 65))
	{
		sprintf_s(strText, GlobalText[359], ManaCur, ManaMax);
		RenderTipText((int)RenderFrameX + 442, (int)RenderFrameY - 20, strText);
		glColor4f(1.0, 1.0, 1.0, 1.0);
	}
}

void SEASON3B::CNewUIMainFrameWindow::Render64Energy()
{
	float fEnergy = 0.0;
	DWORD SkillManaMax = CharacterAttribute->SkillManaMax;
	DWORD SkillManaCur = CharacterAttribute->SkillMana;

	if (SkillManaMax > 0)
	{
		fEnergy = (SkillManaMax - SkillManaCur) / (float)SkillManaMax;
	}

	float RenderFrameX = (float)PositionX_In_The_Mid(0);
	float RenderFrameY = (float)Position_In_The_Down(51.f) + 5.0;

	double porcento = (1.0 - fEnergy);
	double width = (double)(porcento * 84.0);

	float height = 8.0;
	float PosX = RenderFrameX + (double)(84.0 - width) + 351.0;

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderImageF(IMAGE_GAUGE_AG, PosX, RenderFrameY, width, height, (fEnergy * 169.0), 0.0, (porcento * 169.0), 17.0);

	char strText[256];
	memset(strText, 0, sizeof(strText));

	if (SEASON3B::CheckMouseIn(RenderFrameX + 351, RenderFrameY, 84, 8))
	{
		sprintf_s(strText, GlobalText[214], SkillManaCur, SkillManaMax);
		RenderTipText((int)RenderFrameX + 351, (int)RenderFrameY - 20, strText);
	}

	glColor4f(0.8, 0.8, 0.8, 1.f);
	SEASON3B::RenderNumber(RenderFrameX + 393, RenderFrameY, SkillManaCur, 0.85);
}

void SEASON3B::CNewUIMainFrameWindow::Render64Shield()
{
	//
	float fShield = 0.0;
	DWORD ShieldMax = CharacterAttribute->ShieldMax;
	DWORD ShieldCur = CharacterAttribute->Shield;

	if (ShieldMax > 0)
	{
		fShield = (ShieldMax - ShieldCur) / (float)ShieldMax;
	}

	float RenderFrameX = (float)PositionX_In_The_Mid(0);
	float RenderFrameY = (float)Position_In_The_Down(51.f) + 5.0;

	double porcento = (1.0 - fShield);
	double width = (double)(porcento * 84.0);

	float PosX = RenderFrameX + 205.0;
	float height = 8.0;

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderImageF(IMAGE_GAUGE_SD, PosX, RenderFrameY, width, height, 0.0, 0.0, (porcento * 169.0), 17.0);

	char strText[256];
	memset(strText, 0, sizeof(strText));

	if (SEASON3B::CheckMouseIn(RenderFrameX + 205, RenderFrameY, 84, 8))
	{
		sprintf_s(strText, GlobalText[2037], ShieldCur, ShieldMax);
		RenderTipText((int)RenderFrameX + 205, (int)RenderFrameY - 20, strText);
	}
	glColor4f(0.8, 0.8, 0.8, 1.f);
	SEASON3B::RenderNumber(RenderFrameX + 247, RenderFrameY, ShieldCur, 0.85);
}

bool SEASON3B::CNewUIMainFrameWindow::UpdateMouseEvent()
{
	if (g_pNewUIHotKey->IsStateGameOver() == true)
	{
		return true;
	}

	if (BtnProcess() == true)
	{
		return false;
	}

#ifdef EFFECT_KERNEL_VERTEX
	if (GMRenderGroupMesh->update())
	{
		return false;
	}
#endif // EFFECT_KERNEL_VERTEX

	if (GMResetManager->UpdateMouseEvent() == true)
	{
		return false;
	}

	if (gmProtect->InvasionInfoTime)
	{
		if (GMInvasionManager->UpdateMouseEvent() == true)
		{
			return false;
		}
	}
	return true;
}

#if MAIN_UPDATE > 303
bool SEASON3B::CNewUIMainFrameWindow::BtnProcess()
{
	if (g_pNewUIHotKey->CanUpdateKeyEventRelatedMyInventory() == true)
	{
		if (m_BtnMyInven.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_INVENTORY);
			PlayBuffer(SOUND_CLICK01);
			return true;
		}
	}
	else if (g_pNewUIHotKey->CanUpdateKeyEvent() == true)
	{
		if (m_BtnMyInven.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_INVENTORY);
			PlayBuffer(SOUND_CLICK01);
			return true;
		}
		else if (m_BtnChaInfo.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_CHARACTER);

			PlayBuffer(SOUND_CLICK01);

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER))
				g_QuestMng.SendQuestIndexByEtcSelection();

			return true;
		}
		else if (m_BtnFriend.UpdateMouseEvent() == true && gmProtect->LookAndFeel != 1)
		{
			if (gMapManager->InChaosCastle() == true)
			{
				PlayBuffer(SOUND_CLICK01);
				return true;
			}

			int iLevel = CharacterAttribute->Level;

			if (iLevel < 6)
			{
				if (g_pChatListBox->CheckChatRedundancy(GlobalText[1067]) == FALSE)
				{
					g_pChatListBox->AddText("", GlobalText[1067], SEASON3B::TYPE_SYSTEM_MESSAGE);
				}
			}
			else
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_FRIEND);
			}
			PlayBuffer(SOUND_CLICK01);
			return true;
		}
		else if (m_BtnWindow.UpdateMouseEvent() == true)
		{
			if (gmProtect->LookAndFeel != 1 && gmProtect->LookAndFeel == 2)
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_COMMAND);
				PlayBuffer(SOUND_CLICK01);
			}
			else
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_WINDOW_MENU);
				PlayBuffer(SOUND_CLICK01);
			}
			return true;
		}
		else if (m_BtnCShop.UpdateMouseEvent() == true && (gmProtect->LookAndFeel != 1 && gmProtect->LookAndFeel != 2))
		{
			if (GMProtect->IsInGameShop() == false)
				return false;

			if (g_pInGameShop->IsInGameShopOpen() == false)
				return false;

			if (g_InGameShopSystem->IsScriptDownload() == true)
			{
				if (g_InGameShopSystem->ScriptDownload() == false)
					return false;
			}

			if (g_InGameShopSystem->IsBannerDownload() == true)
			{
				g_InGameShopSystem->BannerDownload();
			}
			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) == false)
			{
				if (g_InGameShopSystem->GetIsRequestShopOpenning() == false)
				{
					SendRequestIGS_CashShopOpen(0);
					g_InGameShopSystem->SetIsRequestShopOpenning(true);
					g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTCHARGE, true);
				}
			}
			else
			{
				SendRequestIGS_CashShopOpen(1);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_INGAMESHOP);
			}
			return true;
		}

		if (gmProtect->checkold_school())
		{
			if (this->wope_update_btn())
			{
				return true;
			}

			if (m_BtnWinGuild.UpdateMouseEvent() == true)
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_GUILDINFO);
				PlayBuffer(SOUND_CLICK01);
				return true;
			}
			if (m_BtnWinParty.UpdateMouseEvent() == true)
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_PARTY);
				PlayBuffer(SOUND_CLICK01);
				return true;
			}
		}
		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			if (m_BtnWinGuild.UpdateMouseEvent() == true)
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MYQUEST);
				PlayBuffer(SOUND_CLICK01);
				return true;
			}
		}
	}
	return false;
}
#else
bool SEASON3B::CNewUIMainFrameWindow::BtnProcess()
{
	if (g_pNewUIHotKey->CanUpdateKeyEventRelatedMyInventory() == true)
	{
		if (m_BtnMyInven.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_INVENTORY);
			PlayBuffer(SOUND_CLICK01);
			return true;
		}
	}
	else if (g_pNewUIHotKey->CanUpdateKeyEvent() == true)
	{
		float RenderFrameX = PositionX_The_Mid(640.0);
		float RenderFrameY = PositionY_Add(0);

		if (Hero->SafeZone)
		{
			if (CheckMouseIn(RenderFrameX + 108.0, RenderFrameY + 448.0, 50.f, 20.0) && IsRelease(VK_LBUTTON))
			{
				if (GMProtect->IsInGameShop() == false)
					return false;

				if (g_pInGameShop->IsInGameShopOpen() == false)
					return false;

				if (g_InGameShopSystem->IsScriptDownload() == true)
				{
					if (g_InGameShopSystem->ScriptDownload() == false)
						return false;
				}

				if (g_InGameShopSystem->IsBannerDownload() == true)
				{
					g_InGameShopSystem->BannerDownload();
				}
				if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP) == false)
				{
					if (g_InGameShopSystem->GetIsRequestShopOpenning() == false)
					{
						SendRequestIGS_CashShopOpen(0);
						g_InGameShopSystem->SetIsRequestShopOpenning(true);
						g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTCHARGE, true);
					}
				}
				else
				{
					SendRequestIGS_CashShopOpen(1);
					g_pNewUISystem->Hide(SEASON3B::INTERFACE_INGAMESHOP);
				}
				return true;
			}
			if (CheckMouseIn(RenderFrameX + 482.0, RenderFrameY + 448.0, 50.f, 20.0) && IsRelease(VK_LBUTTON))
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_SHOW_VIP);
				return true;
			}
		}

		if (m_BtnMyInven.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_INVENTORY);
			PlayBuffer(SOUND_CLICK01);
			return true;
		}

		if (m_BtnChaInfo.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_CHARACTER);

			PlayBuffer(SOUND_CLICK01);

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_CHARACTER))
			{
				g_QuestMng.SendQuestIndexByEtcSelection();
			}
			return true;
		}

		if (m_BtnWinParty.UpdateMouseEvent() == true)
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_PARTY);
			PlayBuffer(SOUND_CLICK01);
			return true;
		}

		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX + 407.0, RenderFrameY + 450, 18.0, 11.0))
			{
				if (gmAIController->IsRunning() != true)
				{
					if (gmAIController->CanUseAIController())
					{
						SendRequestStartHelper(false);
					}
					return true;
				}
				else
				{
					SendRequestStartHelper(true);
					return true;
				}
			}

			if (SEASON3B::CheckMouseIn(RenderFrameX + 407.0, RenderFrameY + 462.0, 18.0, 11.0))
			{
				g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MACRO_OFICIAL);
				PlayBuffer(SOUND_CLICK01);

				if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL))
				{
					g_pNewUISystem->Hide(SEASON3B::INTERFACE_MACRO_OFICIAL_SUB);
				}
				return true;
			}
		}
	}

	return false;
}
#endif // MAIN_UPDATE == 303


bool SEASON3B::CNewUIMainFrameWindow::UpdateKeyEvent()
{
	if (m_ItemHotKey.UpdateKeyEvent() == false)
	{
		return false;
	}

	if (GMResetManager->UpdateKeyEvent())
	{
		return false;
	}

	return true;
}

bool SEASON3B::CNewUIMainFrameWindow::Update()
{
	if (m_bExpEffect == true)
	{
		if (timeGetTime() - m_dwExpEffectTime > 2000)
		{
			m_bExpEffect = false;
			m_dwExpEffectTime = 0;
			m_dwGetExp = 0;
		}
	}
	if (gmProtect->InvasionInfoTime)
	{
		GMInvasionManager->Update();
	}

	return true;
}

float SEASON3B::CNewUIMainFrameWindow::GetLayerDepth()
{
	return 10.6f;
}

float SEASON3B::CNewUIMainFrameWindow::GetKeyEventOrder()
{
	return 2.9f;
}


void SEASON3B::CNewUIMainFrameWindow::SetItemHotKey(int iHotKey, int iItemType, int iItemLevel)
{
	m_ItemHotKey.SetHotKey(iHotKey, iItemType, iItemLevel);
}

int SEASON3B::CNewUIMainFrameWindow::GetItemHotKey(int iHotKey)
{
	return m_ItemHotKey.GetHotKey(iHotKey);
}

int SEASON3B::CNewUIMainFrameWindow::GetItemHotKeyLevel(int iHotKey)
{
	return m_ItemHotKey.GetHotKeyLevel(iHotKey);
}

void SEASON3B::CNewUIMainFrameWindow::UseHotKeyItemRButton()
{
	m_ItemHotKey.UseItemRButton();
}

void SEASON3B::CNewUIMainFrameWindow::UpdateItemHotKey()
{
	m_ItemHotKey.UpdateKeyEvent();
}

void SEASON3B::CNewUIMainFrameWindow::ResetSkillHotKey()
{
	g_pSkillList->Reset();
}

void SEASON3B::CNewUIMainFrameWindow::SetSkillHotKey(int iHotKey, int iSkillType)
{
	g_pSkillList->SetHotKey(iHotKey, iSkillType);
}

int SEASON3B::CNewUIMainFrameWindow::GetSkillHotKey(int iHotKey)
{
	return g_pSkillList->GetHotKey(iHotKey);
}

int SEASON3B::CNewUIMainFrameWindow::GetSkillHotKeyIndex(int iSkillType)
{
	return g_pSkillList->GetSkillIndex(iSkillType);
}

SEASON3B::CNewUIItemHotKey::CNewUIItemHotKey()
{
	for (int i = 0; i < HOTKEY_COUNT; ++i)
	{
		m_iHotKey[i].Type = -1;
		m_iHotKey[i].Level = 0;
	}

	rendercount = HOTKEY_COUNT;
}

SEASON3B::CNewUIItemHotKey::~CNewUIItemHotKey()
{
}

void SEASON3B::CNewUIItemHotKey::Init()
{
	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

#if MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		rendercount = HOTKEY_R;

		SetPosition(HOTKEY_Q, RenderFrameX + 210, RenderFrameY + 453.0, RenderFrameX + 226.0, RenderFrameY + 447.0);

		SetPosition(HOTKEY_W, RenderFrameX + 240, RenderFrameY + 453.0, RenderFrameX + 257.0, RenderFrameY + 447.0);

		SetPosition(HOTKEY_E, RenderFrameX + 270, RenderFrameY + 453.0, RenderFrameX + 288.0, RenderFrameY + 447.0);

		//SetPosition(HOTKEY_R, RenderFrameX + 300, RenderFrameY + 453.0, RenderFrameX + 334.0, RenderFrameY + 447.0);
	}
	else if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		SetPosition(HOTKEY_Q, RenderFrameX + 188, RenderFrameY + 445.0, RenderFrameX + 203, RenderFrameY + 457.0);

		SetPosition(HOTKEY_W, RenderFrameX + 217, RenderFrameY + 445.0, RenderFrameX + 232, RenderFrameY + 457.0);

		SetPosition(HOTKEY_E, RenderFrameX + 246, RenderFrameY + 445.0, RenderFrameX + 261, RenderFrameY + 457.0);

		SetPosition(HOTKEY_R, RenderFrameX + 275, RenderFrameY + 445.0, RenderFrameX + 290, RenderFrameY + 457.0);
	}
	else
	{
		SetPosition(HOTKEY_Q, RenderFrameX + 10.00, RenderFrameY + 443.0, RenderFrameX + 30.0, RenderFrameY + 457.0);

		SetPosition(HOTKEY_W, RenderFrameX + 48.00, RenderFrameY + 443.0, RenderFrameX + 68.0, RenderFrameY + 457.0);

		SetPosition(HOTKEY_E, RenderFrameX + 86.00, RenderFrameY + 443.0, RenderFrameX + 106.0, RenderFrameY + 457.0);

		SetPosition(HOTKEY_R, RenderFrameX + 124.0, RenderFrameY + 443.0, RenderFrameX + 144.0, RenderFrameY + 457.0);
	}
#else
	rendercount = HOTKEY_R;

	SetPosition(HOTKEY_Q, RenderFrameX + 208, RenderFrameY + 453.0, RenderFrameX + 222.0, RenderFrameY + 450.0);

	SetPosition(HOTKEY_W, RenderFrameX + 234, RenderFrameY + 453.0, RenderFrameX + 248.0, RenderFrameY + 450.0);

	SetPosition(HOTKEY_E, RenderFrameX + 257, RenderFrameY + 453.0, RenderFrameX + 271.0, RenderFrameY + 450.0);
#endif
}

void SEASON3B::CNewUIItemHotKey::SetPosition(int iHotKey, float x, float y, float x2, float y2)
{
	if (iHotKey >= 0 && iHotKey < rendercount)
	{
		m_iHotKey[iHotKey].m_ItemX = x;
		m_iHotKey[iHotKey].m_ItemY = y;
		m_iHotKey[iHotKey].m_NumbX = x2;
		m_iHotKey[iHotKey].m_NumbY = y2;
	}
}

bool SEASON3B::CNewUIItemHotKey::UpdateKeyEvent()
{
	int iIndex = -1;

	if (SEASON3B::IsPress('Q') == true)
	{
		iIndex = GetHotKeyItemIndex(HOTKEY_Q);
	}
	else if (SEASON3B::IsPress('W') == true)
	{
		iIndex = GetHotKeyItemIndex(HOTKEY_W);
	}
	else if (SEASON3B::IsPress('E') == true)
	{
		iIndex = GetHotKeyItemIndex(HOTKEY_E);
	}
	else if (SEASON3B::IsPress('R') == true && (gmProtect->LookAndFeel != 1 && gmProtect->LookAndFeel != 2))
	{
		iIndex = GetHotKeyItemIndex(HOTKEY_R);
	}

	if (iIndex != -1)
	{
		ITEM* pItem = NULL;
		pItem = g_pMyInventory->FindItem(iIndex);
		if ((pItem->Type >= ITEM_POTION + 78 && pItem->Type <= ITEM_POTION + 82))
		{
			std::list<eBuffState> secretPotionbufflist;
			secretPotionbufflist.push_back(eBuff_SecretPotion1);
			secretPotionbufflist.push_back(eBuff_SecretPotion2);
			secretPotionbufflist.push_back(eBuff_SecretPotion3);
			secretPotionbufflist.push_back(eBuff_SecretPotion4);
			secretPotionbufflist.push_back(eBuff_SecretPotion5);

			if (g_isCharacterBufflist((&Hero->Object), secretPotionbufflist) != eBuffNone)
			{
				SEASON3B::CreateOkMessageBox(GlobalText[2530], RGBA(255, 30, 0, 255));
			}
			else
			{
				SendRequestUse(iIndex, 0);
			}
		}
		else

		{
			SendRequestUse(iIndex, 0);
		}
		return false;
	}

	return true;
}

int SEASON3B::CNewUIItemHotKey::GetHotKeyItemIndex(int iType, bool bItemCount)
{
	int iStartItemType = 0, iEndItemType = 0;
	int i, j;

	switch (iType)
	{
	case HOTKEY_Q:
		if (GetHotKeyCommonItem(iType, iStartItemType, iEndItemType) == false)
		{
			if (m_iHotKey[iType].Type >= ITEM_POTION + 4 && m_iHotKey[iType].Type <= ITEM_POTION + 6)
			{
				iStartItemType = ITEM_POTION + 6; iEndItemType = ITEM_POTION + 4;
			}
			else
			{
				iStartItemType = ITEM_POTION + 3; iEndItemType = ITEM_POTION + 0;
			}
		}
		break;
	case HOTKEY_W:
		if (GetHotKeyCommonItem(iType, iStartItemType, iEndItemType) == false)
		{
			if (m_iHotKey[iType].Type >= ITEM_POTION + 0 && m_iHotKey[iType].Type <= ITEM_POTION + 3)
			{
				iStartItemType = ITEM_POTION + 3; iEndItemType = ITEM_POTION + 0;
			}
			else
			{
				iStartItemType = ITEM_POTION + 6; iEndItemType = ITEM_POTION + 4;
			}
		}
		break;
	case HOTKEY_E:
		if (GetHotKeyCommonItem(iType, iStartItemType, iEndItemType) == false)
		{
			if (m_iHotKey[iType].Type >= ITEM_POTION + 0 && m_iHotKey[iType].Type <= ITEM_POTION + 3)
			{
				iStartItemType = ITEM_POTION + 3; iEndItemType = ITEM_POTION + 0;
			}
			else if (m_iHotKey[iType].Type >= ITEM_POTION + 4 && m_iHotKey[iType].Type <= ITEM_POTION + 6)
			{
				iStartItemType = ITEM_POTION + 6; iEndItemType = ITEM_POTION + 4;
			}
			else
			{
				iStartItemType = ITEM_POTION + 8; iEndItemType = ITEM_POTION + 8;
			}
		}
		break;
	case HOTKEY_R:
		if (GetHotKeyCommonItem(iType, iStartItemType, iEndItemType) == false && (gmProtect->LookAndFeel != 1 && gmProtect->LookAndFeel != 2))
		{
			if (m_iHotKey[iType].Type >= ITEM_POTION + 0 && m_iHotKey[iType].Type <= ITEM_POTION + 3)
			{
				iStartItemType = ITEM_POTION + 3; iEndItemType = ITEM_POTION + 0;
			}
			else if (m_iHotKey[iType].Type >= ITEM_POTION + 4 && m_iHotKey[iType].Type <= ITEM_POTION + 6)
			{
				iStartItemType = ITEM_POTION + 6; iEndItemType = ITEM_POTION + 4;
			}
			else
			{
				iStartItemType = ITEM_POTION + 37; iEndItemType = ITEM_POTION + 35;
			}
		}
		break;
	}

	int iItemCount = 0;
	ITEM* pItem = NULL;

	int iNumberofItems = g_pMyInventory->GetNumberOfItems();

	for (i = iStartItemType; i >= iEndItemType; --i)
	{
		if (bItemCount)
		{
			for (j = 0; j < iNumberofItems; ++j)
			{
				pItem = g_pMyInventory->GetItem(j);

				if (pItem == NULL)
					continue;

				if ((pItem->Type == i && ((pItem->Level >> 3) & 15) == m_iHotKey[iType].Level)
					|| (pItem->Type == i && (pItem->Type >= ITEM_POTION + 0 && pItem->Type <= ITEM_POTION + 3))
					)
				{
					if (pItem->Type == ITEM_POTION + 9
						|| pItem->Type == ITEM_POTION + 10
						|| pItem->Type == ITEM_POTION + 20
						)
					{
						iItemCount++;
					}
					else
					{
						iItemCount += pItem->Durability;
					}
				}
			}
		}
		else
		{
			int iIndex = -1;
			if (i >= ITEM_POTION + 0 && i <= ITEM_POTION + 3)
			{
				iIndex = g_pMyInventory->FindItemReverseIndex(i);
			}
			else
			{
				iIndex = g_pMyInventory->FindItemReverseIndex(i, m_iHotKey[iType].Level);
			}

			if (-1 != iIndex)
			{
				pItem = g_pMyInventory->FindItem(iIndex);
				if ((pItem->Type != ITEM_POTION + 7
					&& pItem->Type != ITEM_POTION + 10
					&& pItem->Type != ITEM_POTION + 20)
					|| ((pItem->Level >> 3) & 15) == m_iHotKey[iType].Level
					)
				{
					return iIndex;
				}
			}
		}
	}

	if (bItemCount == true)
	{
		return iItemCount;
	}

	return -1;
}

bool SEASON3B::CNewUIItemHotKey::GetHotKeyCommonItem(IN int iHotKey, OUT int& iStart, OUT int& iEnd)
{
	switch (m_iHotKey[iHotKey].Type)
	{
	case ITEM_POTION + 7:
	case ITEM_POTION + 8:
	case ITEM_POTION + 9:
	case ITEM_POTION + 10:
	case ITEM_POTION + 20:
	case ITEM_POTION + 46:
	case ITEM_POTION + 47:
	case ITEM_POTION + 48:
	case ITEM_POTION + 49:
	case ITEM_POTION + 50:
	case ITEM_POTION + 70:
	case ITEM_POTION + 71:
	case ITEM_POTION + 78:
	case ITEM_POTION + 79:
	case ITEM_POTION + 80:
	case ITEM_POTION + 81:
	case ITEM_POTION + 82:
	case ITEM_POTION + 94:
	case ITEM_POTION + 85:
	case ITEM_POTION + 86:
	case ITEM_POTION + 87:
	case ITEM_POTION + 133:
		if (m_iHotKey[iHotKey].Type != ITEM_POTION + 20 || m_iHotKey[iHotKey].Level == 0)
		{
			iStart = iEnd = m_iHotKey[iHotKey].Type;
			return true;
		}
		break;
	default:
		if (m_iHotKey[iHotKey].Type >= ITEM_POTION + 35 && m_iHotKey[iHotKey].Type <= ITEM_POTION + 37)
		{
			iStart = ITEM_POTION + 37; iEnd = ITEM_POTION + 35;
			return true;
		}
		else if (m_iHotKey[iHotKey].Type >= ITEM_POTION + 38 && m_iHotKey[iHotKey].Type <= ITEM_POTION + 40)
		{
			iStart = ITEM_POTION + 40; iEnd = ITEM_POTION + 38;
			return true;
		}
		break;
	}
	return false;
}

int SEASON3B::CNewUIItemHotKey::GetHotKeyItemCount(int iType)
{
	return 0;
}

void SEASON3B::CNewUIItemHotKey::SetHotKey(int iHotKey, int iItemType, int iItemLevel)
{
	if (iHotKey >= 0 && iHotKey < rendercount)
	{
		if (CNewUIMyInventory::CanRegisterItemHotKey(iItemType) == true)
		{
			m_iHotKey[iHotKey].Type = iItemType;
			m_iHotKey[iHotKey].Level = iItemLevel;
		}
		else
		{
			m_iHotKey[iHotKey].Type = -1;
			m_iHotKey[iHotKey].Level = 0;
		}
	}
}

int SEASON3B::CNewUIItemHotKey::GetHotKey(int iHotKey)
{
	if (iHotKey >= 0 && iHotKey < rendercount)
	{
		return m_iHotKey[iHotKey].Type;
	}

	return -1;
}

int SEASON3B::CNewUIItemHotKey::GetHotKeyLevel(int iHotKey)
{
	if (iHotKey >= 0 && iHotKey < rendercount)
	{
		return m_iHotKey[iHotKey].Level;
	}

	return 0;
}

void SEASON3B::CNewUIItemHotKey::RenderItems()
{
	for (int i = 0; i < rendercount; ++i)
	{
		int iIndex = GetHotKeyItemIndex(i);
		if (iIndex != -1)
		{
			ITEM* pItem = g_pMyInventory->FindItem(iIndex);
			if (pItem)
			{
				float x = m_iHotKey[i].m_ItemX;
				float y = m_iHotKey[i].m_ItemY;
#if MAIN_UPDATE > 303
				RenderItem3D(x, y, 20.0, 20.0, pItem->Type, pItem->Level, 0, 0);
#else
				RenderItem3D(x, y - 2, 20.0, 20.0, pItem->Type, pItem->Level, 0, 0, false, 0.0028);
#endif
			}
		}
	}
}

void SEASON3B::CNewUIItemHotKey::RenderItemCount()
{
	float x, y;

	if (gmProtect->checkold_school())
		glColor3f(1.0, 0.94999999, 0.75);
	else
		glColor4f(1.f, 1.f, 1.f, 1.f);


	for (int i = 0; i < rendercount; ++i)
	{
		int iCount = GetHotKeyItemIndex(i, true);
		if (iCount > 0)
		{
			x = m_iHotKey[i].m_NumbX;
			y = m_iHotKey[i].m_NumbY;

#ifdef test_main_dev
			SEASON3B::RenderNumber(x, y, iCount, 0.85);
#else
#if MAIN_UPDATE > 303
			if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
			{
				SEASON3B::RenderNumber(x, y, iCount, 0.85);
			}
			else
			{
				SEASON3B::RenderNumber(x, y, iCount);
			}
#else
			SEASON3B::RenderNumber(x, y + 1, iCount, 0.80);
#endif
#endif // test_main_dev

		}
	}
}

void SEASON3B::CNewUIItemHotKey::UseItemRButton()
{
	int x, y;
	for (int i = 0; i < rendercount; ++i)
	{
		x = m_iHotKey[i].m_ItemX;
		y = m_iHotKey[i].m_ItemY;

		if (SEASON3B::CheckMouseIn(x, y, 20, 20) == true)
		{
			if (MouseRButtonPush)
			{
				MouseRButtonPush = false;
				int iIndex = GetHotKeyItemIndex(i);
				if (iIndex != -1)
				{
					SendRequestUse(iIndex, 0);
					break;
				}
			}
		}
	}
}

SEASON3B::CNewUISkillList::CNewUISkillList()
{
	m_pNewUIMng = NULL;
	Reset();
}

SEASON3B::CNewUISkillList::~CNewUISkillList()
{
	Release();
}

bool SEASON3B::CNewUISkillList::Create(CNewUIManager* pNewUIMng, CNewUI3DRenderMng* pNewUI3DRenderMng)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_SKILL_LIST, this);

	m_pNewUI3DRenderMng = pNewUI3DRenderMng;

	LoadImages();

	Show(true);

	return true;
}

void SEASON3B::CNewUISkillList::Release()
{
	if (m_pNewUI3DRenderMng)
	{
		m_pNewUI3DRenderMng->DeleteUI2DEffectObject(UI2DEffectCallback);
	}

	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUISkillList::Reset()
{
	m_bSkillList = false;
	m_bHotKeySkillListUp = false;

	m_bRenderSkillInfo = false;
	m_iRenderSkillInfoType = 0;
	m_iRenderSkillInfoPosX = 0;
	m_iRenderSkillInfoPosY = 0;

	for (int i = 0; i < SKILLHOTKEY_COUNT; ++i)
	{
		m_iHotKeySkillType[i] = -1;
	}

	m_EventState = EVENT_NONE;
}

void SEASON3B::CNewUISkillList::LoadImages()
{
#if MAIN_UPDATE > 603
	LoadBitmap("Interface\\newui_skill.jpg", IMAGE_SKILL1, GL_LINEAR); //-- 31310
	LoadBitmap("Interface\\newui_skill2.jpg", IMAGE_SKILL2, GL_LINEAR); //-- 31311
	LoadBitmap("Interface\\newui_non_skill.jpg", IMAGE_NON_SKILL1, GL_LINEAR); //-- 31316
	LoadBitmap("Interface\\newui_non_skill2.jpg", IMAGE_NON_SKILL2, GL_LINEAR); //-- 31317
	LoadBitmap("Interface\\newui_skillbox.jpg", IMAGE_SKILLBOX, GL_LINEAR); //-- 31314
	LoadBitmap("Interface\\newui_skillbox2.jpg", IMAGE_SKILLBOX_USE, GL_LINEAR); //-- 31315
	LoadBitmap("Interface\\newui_command.jpg", IMAGE_COMMAND, GL_LINEAR); //-- 31312
	LoadBitmap("Interface\\newui_non_command.jpg", IMAGE_NON_COMMAND, GL_LINEAR); //-- 31318
	LoadBitmap("Interface\\newui_skill3.jpg", IMAGE_SKILL3, GL_LINEAR); //-- 31313
	LoadBitmap("Interface\\newui_non_skill3.jpg", IMAGE_NON_SKILL3, GL_LINEAR); //-- 31319
#elif MAIN_UPDATE > 303
	if (gmProtect->LookAndFeel == 1)
	{
		LoadBitmap("Interface\\HUD\\Look-1\\Skill.jpg", IMAGE_SKILL1, GL_LINEAR); //-- 31310
		LoadBitmap("Interface\\HUD\\Look-1\\Skill2.jpg", IMAGE_SKILL2, GL_LINEAR); //-- 31311
		LoadBitmap("Interface\\HUD\\Look-1\\Skill.jpg", IMAGE_NON_SKILL1, GL_LINEAR); //-- 31316
		LoadBitmap("Interface\\HUD\\Look-1\\Skill2.jpg", IMAGE_NON_SKILL2, GL_LINEAR); //-- 31317
		LoadBitmap("Interface\\HUD\\Look-1\\skillbox.jpg", IMAGE_SKILLBOX, GL_LINEAR); //-- 31314
		LoadBitmap("Interface\\HUD\\Look-1\\skillbox2.jpg", IMAGE_SKILLBOX_USE, GL_LINEAR); //-- 31315
		LoadBitmap("Interface\\HUD\\Look-1\\command.jpg", IMAGE_COMMAND, GL_LINEAR); //-- 31312
		LoadBitmap("Interface\\HUD\\Look-1\\command.jpg", IMAGE_NON_COMMAND, GL_LINEAR); //-- 31318
	}
	else if (gmProtect->LookAndFeel == 2)
	{
		LoadBitmap("Interface\\HUD\\Look-2\\Skill.jpg", IMAGE_SKILL1, GL_LINEAR); //-- 31310
		LoadBitmap("Interface\\HUD\\Look-2\\Skill2.jpg", IMAGE_SKILL2, GL_LINEAR); //-- 31311
		LoadBitmap("Interface\\HUD\\Look-2\\Skill.jpg", IMAGE_NON_SKILL1, GL_LINEAR); //-- 31316
		LoadBitmap("Interface\\HUD\\Look-2\\Skill2.jpg", IMAGE_NON_SKILL2, GL_LINEAR); //-- 31317
		LoadBitmap("Interface\\HUD\\Look-2\\skillbox.jpg", IMAGE_SKILLBOX, GL_LINEAR); //-- 31314
		LoadBitmap("Interface\\HUD\\Look-2\\skillbox2.jpg", IMAGE_SKILLBOX_USE, GL_LINEAR); //-- 31315
		LoadBitmap("Interface\\HUD\\Look-2\\command.jpg", IMAGE_COMMAND, GL_LINEAR); //-- 31312
		LoadBitmap("Interface\\HUD\\Look-2\\command.jpg", IMAGE_NON_COMMAND, GL_LINEAR); //-- 31318
	}
	else
	{
		LoadBitmap("Interface\\newui_skill.jpg", IMAGE_SKILL1, GL_LINEAR); //-- 31310
		LoadBitmap("Interface\\newui_skill2.jpg", IMAGE_SKILL2, GL_LINEAR); //-- 31311
		LoadBitmap("Interface\\newui_non_skill.jpg", IMAGE_NON_SKILL1, GL_LINEAR); //-- 31316
		LoadBitmap("Interface\\newui_non_skill2.jpg", IMAGE_NON_SKILL2, GL_LINEAR); //-- 31317
		LoadBitmap("Interface\\newui_skillbox.jpg", IMAGE_SKILLBOX, GL_LINEAR); //-- 31314
		LoadBitmap("Interface\\newui_skillbox2.jpg", IMAGE_SKILLBOX_USE, GL_LINEAR); //-- 31315
		LoadBitmap("Interface\\newui_command.jpg", IMAGE_COMMAND, GL_LINEAR); //-- 31312
		LoadBitmap("Interface\\newui_non_command.jpg", IMAGE_NON_COMMAND, GL_LINEAR); //-- 31318
	}

	LoadBitmap("Interface\\newui_skill3.jpg", IMAGE_SKILL3, GL_LINEAR); //-- 31313
	LoadBitmap("Interface\\newui_non_skill3.jpg", IMAGE_NON_SKILL3, GL_LINEAR); //-- 31319

	if (gmProtect->LookAndFeel == 3)
	{
		LoadBitmap("Interface\\HUD\\Look-3\\newui_skillbox1.tga", IMAGE_SKILLBOX_EX7001, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-3\\newui_skillbox2.tga", IMAGE_SKILLBOX_EX7002, GL_LINEAR);
	}
	if (gmProtect->LookAndFeel == 4)
	{
		LoadBitmap("Interface\\HUD\\Look-4\\newui_skillbox1.tga", IMAGE_SKILLBOX_EX7001, GL_LINEAR);
		LoadBitmap("Interface\\HUD\\Look-4\\newui_skillbox2.tga", IMAGE_SKILLBOX_EX7002, GL_LINEAR);
	}
#else
	LoadBitmap("Interface\\HUD\\Look-2\\Skill.jpg", IMAGE_SKILL1, GL_LINEAR); //-- 31310
	LoadBitmap("Interface\\HUD\\Look-2\\Skill2.jpg", IMAGE_SKILL2, GL_LINEAR); //-- 31311
	LoadBitmap("Interface\\HUD\\Look-2\\Skill.jpg", IMAGE_NON_SKILL1, GL_LINEAR); //-- 31316
	LoadBitmap("Interface\\HUD\\Look-2\\Skill2.jpg", IMAGE_NON_SKILL2, GL_LINEAR); //-- 31317
	LoadBitmap("Interface\\HUD\\Look-2\\skillbox.jpg", IMAGE_SKILLBOX, GL_LINEAR); //-- 31314
	LoadBitmap("Interface\\HUD\\Look-2\\skillbox2.jpg", IMAGE_SKILLBOX_USE, GL_LINEAR); //-- 31315
	LoadBitmap("Interface\\HUD\\Look-2\\command.jpg", IMAGE_COMMAND, GL_LINEAR); //-- 31312
	LoadBitmap("Interface\\HUD\\Look-2\\command.jpg", IMAGE_NON_COMMAND, GL_LINEAR); //-- 31318
	LoadBitmap("Interface\\newui_skill3.jpg", IMAGE_SKILL3, GL_LINEAR); //-- 31313
	LoadBitmap("Interface\\newui_non_skill3.jpg", IMAGE_NON_SKILL3, GL_LINEAR); //-- 31319
#endif
}

void SEASON3B::CNewUISkillList::UnloadImages()
{
	DeleteBitmap(IMAGE_SKILL1);
	DeleteBitmap(IMAGE_SKILL2);
	DeleteBitmap(IMAGE_COMMAND);
	DeleteBitmap(IMAGE_SKILLBOX);
	DeleteBitmap(IMAGE_SKILLBOX_USE);
	DeleteBitmap(IMAGE_NON_SKILL1);
	DeleteBitmap(IMAGE_NON_SKILL2);
	DeleteBitmap(IMAGE_NON_COMMAND);
	DeleteBitmap(IMAGE_SKILL3);
	DeleteBitmap(IMAGE_NON_SKILL3);
}

bool SEASON3B::CNewUISkillList::UpdateMouseEvent()
{
#if MAIN_UPDATE > 603
	if (g_isCharacterBuff((&Hero->Object), eBuff_DuelWatch))
	{
		m_bSkillList = false;
		return true;
	}

	m_bRenderSkillInfo = false;
	BYTE bySkillNumber = CharacterAttribute->SkillNumber;
	BYTE bySkillMasterNumber = CharacterAttribute->SkillMasterNumber;

	float x, y, width, height;

	x = m_iRenderSkillSlot.x;
	y = m_iRenderSkillSlot.y;
	width = 28.f;
	height = 32.f;

	if (SEASON3B::CheckMouseIn(x, y, width, height))
	{
		m_EventState = EVENT_BTN_HOVER_CURRENTSKILL;

		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			m_bSkillList = !m_bSkillList;
			PlayBuffer(SOUND_CLICK01);
			m_EventState = EVENT_NONE;
			return false;
		}

		if (SEASON3B::IsRepeat(VK_LBUTTON))
		{
			m_EventState = EVENT_BTN_DOWN_CURRENTSKILL;
			return false;
		}

		m_bRenderSkillInfo = true;
		m_iRenderSkillInfoType = Hero->CurrentSkill;
		m_iRenderSkillInfoPosX = x - 5;
		m_iRenderSkillInfoPosY = y - 35;
		return true;
	}
	else
	{
		m_EventState = EVENT_NONE;
	}


	width = m_iRenderSkillHotKey[3].x - m_iRenderSkillHotKey[0].x + width;

	if (SEASON3B::CheckMouseIn(m_iRenderSkillHotKey[0].x, y, width, height))
	{
		int iStartIndex = (m_bHotKeySkillListUp == true) ? 5 : 1;

		width = 28.f;
		height = 32.f;

		for (int i = 0, iIndex = iStartIndex; i < 4; ++i, iIndex++)
		{
			x = m_iRenderSkillHotKey[i].x;
			y = m_iRenderSkillHotKey[i].y;

			if (iIndex == 10)
				iIndex = 0;

			if (SEASON3B::CheckMouseIn(x, y, width, height))
			{
				m_EventState = EVENT_BTN_HOVER_SKILLHOTKEY;

				if (m_iHotKeySkillType[iIndex] == -1)
				{
					m_bRenderSkillInfo = false;
					m_iRenderSkillInfoType = -1;
					m_EventState = EVENT_NONE;
					continue;
				}

				WORD bySkillType = CharacterAttribute->Skill[m_iHotKeySkillType[iIndex]];

				if (bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
					continue;

				BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

				if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
				{
					continue;
				}

				if (SEASON3B::IsRelease(VK_LBUTTON))
				{
					if (m_iRenderSkillInfoType == m_iHotKeySkillType[iIndex])
					{
						m_EventState = EVENT_NONE;
						m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];
						Hero->CurrentSkill = m_iHotKeySkillType[iIndex];
						PlayBuffer(SOUND_CLICK01);
						return false;
					}
					PlayBuffer(SOUND_CLICK01);
					m_EventState = EVENT_NONE;
					return false;
				}

				if (SEASON3B::IsRepeat(VK_LBUTTON))
				{
					m_EventState = EVENT_BTN_DOWN_SKILLHOTKEY;
					return false;
				}


				m_bRenderSkillInfo = true;
				m_iRenderSkillInfoType = m_iHotKeySkillType[iIndex];
				m_iRenderSkillInfoPosX = x - 5;
				m_iRenderSkillInfoPosY = y - 35;
				return true;
			}
		}
	}

	if (m_bSkillList == false)
		return true;

	mu_int32 iSkillCount = 0;
	mu_boolean bMouseOnSkillList = false;
	EVENT_STATE PrevEventState = m_EventState;

	float fOrigX = pos_center() + 385.f;
	x = pos_center() + 385.f;
	y = pos_botton() + 360.f;
	width = 32.f; height = 38.f;

	for (int i = 0; i < MAX_MAGIC; ++i)
	{
		mu_int32 bySkillType = CharacterAttribute->Skill[i];

		if (bySkillType && (bySkillType < AT_SKILL_STUN || bySkillType > AT_SKILL_REMOVAL_BUFF))
		{
			if (Hero->m_pPet == NULL && bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END)
				continue;

			mu_int8 bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

			if (bySkillUseType != SKILL_USE_TYPE_MASTERLEVEL && bySkillUseType != SKILL_USE_TYPE_MASTER)
			{
				if (iSkillCount == 18)
					y -= height;

				if (iSkillCount >= 14)
				{
					if (iSkillCount < 14 || iSkillCount >= 18)
					{
						x = fOrigX - width * 12.f + (iSkillCount - 17) * width;
					}
					else
					{
						x = fOrigX - width * 8.f - (iSkillCount - 14) * width;
					}
				}
				else
				{
					int iQuotient = iSkillCount / 2;

					if (iSkillCount % 2)
						x = fOrigX - (iQuotient + 1) * width;
					else
						x = iQuotient * width + fOrigX;
				}
				iSkillCount++;

				if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
				{
					bMouseOnSkillList = true;

					m_EventState = EVENT_BTN_HOVER_SKILLLIST;

					if (SEASON3B::IsRelease(VK_LBUTTON))
					{
						if (m_iRenderSkillInfoType == i)
						{
							m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];
							Hero->CurrentSkill = i;
							m_bSkillList = false;
							m_EventState = EVENT_NONE;
							PlayBuffer(SOUND_CLICK01);
						}
						return false;
					}

					if (SEASON3B::IsRepeat(VK_LBUTTON))
					{
						m_EventState = EVENT_BTN_DOWN_SKILLLIST;
						return false;
					}

					m_bRenderSkillInfo = true;
					m_iRenderSkillInfoType = i;
					m_iRenderSkillInfoPosX = x;
					m_iRenderSkillInfoPosY = y - 10;
					return false;
				}
			}
		}
	}

	if (bMouseOnSkillList == false && (m_EventState == EVENT_BTN_HOVER_SKILLLIST || m_EventState == EVENT_BTN_DOWN_SKILLLIST))
	{
		m_EventState = EVENT_NONE;
	}

	return true;
#else

	if (g_isCharacterBuff((&Hero->Object), eBuff_DuelWatch))
	{
		m_bSkillList = false;
		return true;
	}

	BYTE bySkillNumber = CharacterAttribute->SkillNumber;
	BYTE bySkillMasterNumber = CharacterAttribute->SkillMasterNumber;

	float x, y, width, height;

	m_bRenderSkillInfo = false;

	if (bySkillNumber <= 0)
	{
		return true;
	}

	float FrameX = PositionX_The_Mid(640.0);
	float FrameY = GetWindowsY - 51.0;

#if MAIN_UPDATE > 603
	x = m_iRenderSkillSlot.x;
	y = m_iRenderSkillSlot.y;

	width = 32.f; height = 38.f;
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		x = PositionX_In_The_Mid((310.0 - 6.0));
		y = gwinhandle->GetScreenY() - (33.0 + 6.0);
		width = 32.f; height = 38.f;
	}
	else if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		FrameY = GetWindowsY - 72.0;

		x = FrameX + 305.0;
		y = FrameY + 34.0;
		width = 22; height = 26.f;
	}
	else
	{
		x = FrameX + 385.f;
		y = FrameY + 2.0;
		width = 32.f; height = 38.f;
	}
#else
	float RenderFrameX = PositionX_The_Mid(640.0);
	float RenderFrameY = PositionY_Add(0);

	x = RenderFrameX + 286.0;
	y = RenderFrameY + 451.0;
	width = 16.0;
	height = 20.0;
#endif
	if (m_EventState == EVENT_NONE && MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == true)
	{
		m_EventState = EVENT_BTN_HOVER_CURRENTSKILL;
		return true;
	}
	if (m_EventState == EVENT_BTN_HOVER_CURRENTSKILL && MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == false)
	{
		m_EventState = EVENT_NONE;
		return true;
	}
	if (m_EventState == EVENT_BTN_HOVER_CURRENTSKILL && (MouseLButtonPush == true || MouseLButtonDBClick == true) && SEASON3B::CheckMouseIn(x, y, width, height) == true)
	{
		m_EventState = EVENT_BTN_DOWN_CURRENTSKILL;
		return false;
	}
	if (m_EventState == EVENT_BTN_DOWN_CURRENTSKILL)
	{
		if (MouseLButtonPush == false && MouseLButtonDBClick == false)
		{
			if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_bSkillList = !m_bSkillList;
				PlayBuffer(SOUND_CLICK01);
				m_EventState = EVENT_NONE;
				return false;
			}
			m_EventState = EVENT_NONE;
			return true;
		}
	}

	if (m_EventState == EVENT_BTN_HOVER_CURRENTSKILL)
	{
		m_bRenderSkillInfo = true;
		m_iRenderSkillInfoType = Hero->CurrentSkill;
		m_iRenderSkillInfoPosX = x - 5;
		if (gmProtect->checkold_school())
		{
			m_iRenderSkillInfoPosY = y - 25.0;
		}
		else
		{
			m_iRenderSkillInfoPosY = y;
		}

		return false;
	}
	else if (m_EventState == EVENT_BTN_DOWN_CURRENTSKILL)
	{
		return false;
	}

#if MAIN_UPDATE > 603
	x = m_iRenderSkillHotKey[0].x;
	y = m_iRenderSkillHotKey[0].y;

	width = 32.f * 5.f; height = 38.f;

	if (m_EventState == EVENT_NONE && MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == true)
	{
		m_EventState = EVENT_BTN_HOVER_SKILLHOTKEY;
		return true;
	}
	if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY && MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == false)
	{
		m_EventState = EVENT_NONE;
		return true;
	}
	if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY && MouseLButtonPush == true && SEASON3B::CheckMouseIn(x, y, width, height) == true)
	{
		m_EventState = EVENT_BTN_DOWN_SKILLHOTKEY;
		return false;
	}

	width = 32.f; height = 38.f;

	int iStartIndex = (m_bHotKeySkillListUp == true) ? 6 : 1;

	for (int i = 0, iIndex = iStartIndex; i < 5; ++i, iIndex++)
	{
		x = m_iRenderSkillHotKey[i].x;
		y = m_iRenderSkillHotKey[i].y;

		if (iIndex == 10)
			iIndex = 0;

		if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			if (m_iHotKeySkillType[iIndex] == -1)
			{
				if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY)
				{
					m_bRenderSkillInfo = false;
					m_iRenderSkillInfoType = -1;
				}
				if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY && MouseLButtonPush == false)
				{
					m_EventState = EVENT_NONE;
				}
				continue;
			}

			WORD bySkillType = CharacterAttribute->Skill[m_iHotKeySkillType[iIndex]];

			if (bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
				continue;

			BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

			if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
			{
				continue;
			}

			if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY)
			{
				m_bRenderSkillInfo = true;
				m_iRenderSkillInfoType = m_iHotKeySkillType[iIndex];
				m_iRenderSkillInfoPosX = x - 5;
				m_iRenderSkillInfoPosY = y;
				return true;
			}
			if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY)
			{
				if (MouseLButtonPush == false)
				{
					if (m_iRenderSkillInfoType == m_iHotKeySkillType[iIndex])
					{
						m_EventState = EVENT_NONE;
						m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];
						Hero->CurrentSkill = m_iHotKeySkillType[iIndex];
						PlayBuffer(SOUND_CLICK01);
						return false;
					}
					else
					{
						m_EventState = EVENT_NONE;
					}
				}
			}
		}
	}

	x = m_iRenderSkillHotKey[0].x;
	y = m_iRenderSkillHotKey[0].y;
	width = 32.f * 5.f; height = 38.f;

	if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY)
	{
		if (MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == false)
		{
			m_EventState = EVENT_NONE;
			return true;
		}
		return false;
	}
#elif MAIN_UPDATE > 303
	//-- skill current hot and skill
	if (!gmProtect->checkold_school())
	{
		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			x = FrameX + 331.5;
			y = FrameY + 34.0;
			width = 22.0 * 5.f; height = 26.0;
		}
		else
		{
			x = FrameX + 222.f;
			y = FrameY + 2.0;
			width = 32.f * 5.f; height = 38.f;
		}

		if (m_EventState == EVENT_NONE && MouseLButtonPush == false
			&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			m_EventState = EVENT_BTN_HOVER_SKILLHOTKEY;
			return true;
		}
		if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY && MouseLButtonPush == false
			&& SEASON3B::CheckMouseIn(x, y, width, height) == false)
		{
			m_EventState = EVENT_NONE;
			return true;
		}
		if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY && MouseLButtonPush == true
			&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			m_EventState = EVENT_BTN_DOWN_SKILLHOTKEY;
			return false;
		}

		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			width = 22.0; height = 26.0;
		}
		else
		{
			width = 32.f; height = 38.f;
		}

		int iStartIndex = (m_bHotKeySkillListUp == true) ? 6 : 1;

		for (int i = 0, iIndex = iStartIndex; i < 5; ++i, iIndex++)
		{
			if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
			{
				x = FrameX + (width * i) + 331.5;
			}
			else
			{
				x = FrameX + (width * i) + 222.0;
			}
			if (iIndex == 10)
				iIndex = 0;

			if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				if (m_iHotKeySkillType[iIndex] == -1)
				{
					if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY)
					{
						m_bRenderSkillInfo = false;
						m_iRenderSkillInfoType = -1;
					}
					if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY && MouseLButtonPush == false)
					{
						m_EventState = EVENT_NONE;
					}
					continue;
				}

				WORD bySkillType = CharacterAttribute->Skill[m_iHotKeySkillType[iIndex]];

				if (bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
					continue;

				BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

				if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
				{
					continue;
				}

				if (m_EventState == EVENT_BTN_HOVER_SKILLHOTKEY)
				{
					m_bRenderSkillInfo = true;
					m_iRenderSkillInfoType = m_iHotKeySkillType[iIndex];
					m_iRenderSkillInfoPosX = x - 5;
					m_iRenderSkillInfoPosY = y;
					return true;
				}
				if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY)
				{
					if (MouseLButtonPush == false)
					{
						if (m_iRenderSkillInfoType == m_iHotKeySkillType[iIndex])
						{
							m_EventState = EVENT_NONE;
							m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];
							Hero->CurrentSkill = m_iHotKeySkillType[iIndex];
							PlayBuffer(SOUND_CLICK01);
							return false;
						}
						else
						{
							m_EventState = EVENT_NONE;
						}
					}
				}
			}
		}

		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			x = FrameX + 331.5;
			y = FrameY + 34.0;
			width = 22.0 * 5.f; height = 26.0;
		}
		else
		{
			x = FrameX + 222.f;
			y = FrameY + 2.0;
			width = 32.f * 5.f; height = 38.f;
		}

		if (m_EventState == EVENT_BTN_DOWN_SKILLHOTKEY)
		{
			if (MouseLButtonPush == false && SEASON3B::CheckMouseIn(x, y, width, height) == false)
			{
				m_EventState = EVENT_NONE;
				return true;
			}
			return false;
		}
	}
#endif
	if (m_bSkillList == false)
		return true;

	WORD bySkillType = 0;
	int iSkillCount = 0;
	int iSkillReset = 0;
	bool bMouseOnSkillList = false;
	EVENT_STATE PrevEventState = m_EventState;

#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		float fOrigX = 0;
		x = pos_center() + 385.f; width = 32; height = 38;

		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			y = pos_botton() + 365;
			fOrigX = pos_center() + 335.f;
		}
		else
		{
			y = pos_botton() + 390;
			fOrigX = pos_center() + 385.f;
		}

		if (gmProtect->checkold_school())
		{
			fOrigX = pos_center() + 320.0;
			y = pos_botton() + 370.0;
			width = 32.0; height = 36.0;
		}

		for (int i = 0; i < MAX_MAGIC; ++i)
		{
			bySkillType = CharacterAttribute->Skill[i];

			if (bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
				continue;
#if MAIN_UPDATE > 303
			if (bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END)
				continue;
#else
			if (Hero->m_pPet == NULL && bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END)
				continue;
#endif

			BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

			if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
			{
				continue;
			}


			if (iSkillCount == 18)
			{
				y -= height;
			}

			if (iSkillCount < 14)
			{
				int iRemainder = iSkillCount % 2;
				int iQuotient = iSkillCount / 2;

				if (iRemainder == 0)
				{
					x = fOrigX + iQuotient * width;
				}
				else
				{
					x = fOrigX - (iQuotient + 1) * width;
				}
			}
			else if (iSkillCount >= 14 && iSkillCount < 18)
			{
				x = fOrigX - (8 * width) - ((iSkillCount - 14) * width);
			}
			else
			{
				x = fOrigX - (12 * width) + ((iSkillCount - 17) * width);
			}

			iSkillCount++;

			if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				bMouseOnSkillList = true;
				if (m_EventState == EVENT_NONE && MouseLButtonPush == false)
				{
					m_EventState = EVENT_BTN_HOVER_SKILLLIST;
					break;
				}
			}

			if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == true
				&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_EventState = EVENT_BTN_DOWN_SKILLLIST;
				break;
			}

			if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == false
				&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_bRenderSkillInfo = true;
				m_iRenderSkillInfoType = i;
				m_iRenderSkillInfoPosX = x;
				m_iRenderSkillInfoPosY = y;
			}

			if (m_EventState == EVENT_BTN_DOWN_SKILLLIST && MouseLButtonPush == false
				&& m_iRenderSkillInfoType == i && SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_EventState = EVENT_NONE;

				m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];

				Hero->CurrentSkill = i;
				m_bSkillList = false;

				PlayBuffer(SOUND_CLICK01);
				return false;
			}
		}
	}
	else
	{
		float fOrigX = pos_center() + 320.0;
		x = pos_center() + 320.0;
		y = pos_botton() + 370.0;
		width = 32; height = 36;

		for (int i = 0; i < MAX_MAGIC; ++i)
		{
			bySkillType = CharacterAttribute->Skill[i];

			if ((bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
				|| (Hero->m_pPet == NULL && bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END))

			{
				continue;
			}

			BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

			if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
			{
				continue;
			}


			if (iSkillCount % 18 == 0 && iSkillCount > 0)
			{
				iSkillReset = 0;
				x = 0.0;
				y -= height;
			}

			if (iSkillCount < 18)
			{
				int iRemainder = iSkillCount % 2;
				int iQuotient = iSkillCount / 2;

				if (iRemainder == 0)
					x = fOrigX + iQuotient * width;
				else
					x = fOrigX - (iQuotient + 1) * width;
			}
			else
			{
				x = fOrigX - (9 * width) + (iSkillReset * width); // De izquierda a derecha
			}

			iSkillReset++;
			iSkillCount++;

			if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				bMouseOnSkillList = true;
				if (m_EventState == EVENT_NONE && MouseLButtonPush == false)
				{
					m_EventState = EVENT_BTN_HOVER_SKILLLIST;
					break;
				}
			}

			if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == true
				&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_EventState = EVENT_BTN_DOWN_SKILLLIST;
				break;
			}

			if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == false
				&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_bRenderSkillInfo = true;
				m_iRenderSkillInfoType = i;
				m_iRenderSkillInfoPosX = x;
				m_iRenderSkillInfoPosY = y;
			}

			if (m_EventState == EVENT_BTN_DOWN_SKILLLIST && MouseLButtonPush == false
				&& m_iRenderSkillInfoType == i && SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				m_EventState = EVENT_NONE;

				m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];

				Hero->CurrentSkill = i;
				m_bSkillList = false;

				PlayBuffer(SOUND_CLICK01);
				return false;
			}
		}
	}
#else

	float fOrigX = pos_center() + 320.0;
	x = pos_center() + 320.0;
	y = pos_botton() + 370.0;
	width = 32; height = 36;

	for (int i = 0; i < MAX_MAGIC; ++i)
	{
		bySkillType = CharacterAttribute->Skill[i];

		if ((bySkillType == 0 || (bySkillType >= AT_SKILL_STUN && bySkillType <= AT_SKILL_REMOVAL_BUFF))
			|| (Hero->m_pPet == NULL && bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END))

		{
			continue;
		}

		BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

		if (bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
		{
			continue;
		}


		if (iSkillCount % 18 == 0 && iSkillCount > 0)
		{
			iSkillReset = 0;
			x = 0.0;
			y -= height;
		}

		if (iSkillCount < 18)
		{
			int iRemainder = iSkillCount % 2;
			int iQuotient = iSkillCount / 2;

			if (iRemainder == 0)
				x = fOrigX + iQuotient * width;
			else
				x = fOrigX - (iQuotient + 1) * width;
		}
		else
		{
			x = fOrigX - (9 * width) + (iSkillReset * width); // De izquierda a derecha
		}

		iSkillReset++;
		iSkillCount++;

		if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			bMouseOnSkillList = true;
			if (m_EventState == EVENT_NONE && MouseLButtonPush == false)
			{
				m_EventState = EVENT_BTN_HOVER_SKILLLIST;
				break;
			}
		}

		if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == true
			&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			m_EventState = EVENT_BTN_DOWN_SKILLLIST;
			break;
		}

		if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == false
			&& SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			m_bRenderSkillInfo = true;
			m_iRenderSkillInfoType = i;
			m_iRenderSkillInfoPosX = x;
			m_iRenderSkillInfoPosY = y;
		}

		if (m_EventState == EVENT_BTN_DOWN_SKILLLIST && MouseLButtonPush == false
			&& m_iRenderSkillInfoType == i && SEASON3B::CheckMouseIn(x, y, width, height) == true)
		{
			m_EventState = EVENT_NONE;

			m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];

			Hero->CurrentSkill = i;
			m_bSkillList = false;

			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}
#endif
	if (PrevEventState != m_EventState)
	{
		if (m_EventState == EVENT_NONE || m_EventState == EVENT_BTN_HOVER_SKILLLIST)
			return true;
		return false;
	}
#if MAIN_UPDATE > 303
	if ((gmProtect->LookAndFeel != 1 && gmProtect->LookAndFeel != 2) && Hero->m_pPet != NULL)
	{
		width = 32; height = 38;

		if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
		{
			x = AddMiddleX() + 303.f;
			y = AddPositionY() + 327;
		}
		else
		{
			x = AddMiddleX() + 353.f;
			y = AddPositionY() + 352;
		}
		for (int i = AT_PET_COMMAND_DEFAULT; i < AT_PET_COMMAND_END; ++i)
		{
			if (SEASON3B::CheckMouseIn(x, y, width, height) == true)
			{
				bMouseOnSkillList = true;

				if (m_EventState == EVENT_NONE && MouseLButtonPush == false)
				{
					m_EventState = EVENT_BTN_HOVER_SKILLLIST;
					return true;
				}
				if (m_EventState == EVENT_BTN_HOVER_SKILLLIST && MouseLButtonPush == true)
				{
					m_EventState = EVENT_BTN_DOWN_SKILLLIST;
					return false;
				}

				if (m_EventState == EVENT_BTN_HOVER_SKILLLIST)
				{
					m_bRenderSkillInfo = true;
					m_iRenderSkillInfoType = i;
					m_iRenderSkillInfoPosX = x;
					m_iRenderSkillInfoPosY = y;
				}
				if (m_EventState == EVENT_BTN_DOWN_SKILLLIST && MouseLButtonPush == false
					&& m_iRenderSkillInfoType == i)
				{
					m_EventState = EVENT_NONE;

					m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];

					Hero->CurrentSkill = i;
					m_bSkillList = false;
					PlayBuffer(SOUND_CLICK01);
					return false;
				}
			}
			x += width;
		}
	}
#endif
	if (bMouseOnSkillList == false && m_EventState == EVENT_BTN_HOVER_SKILLLIST)
	{
		m_EventState = EVENT_NONE;
		return true;
	}
	if (bMouseOnSkillList == false && MouseLButtonPush == false && m_EventState == EVENT_BTN_DOWN_SKILLLIST)
	{
		m_EventState = EVENT_NONE;
		return false;
	}
	if (m_EventState == EVENT_BTN_DOWN_SKILLLIST)
	{
		if (MouseLButtonPush == false)
		{
			m_EventState = EVENT_NONE;
			return true;
		}
		return false;
	}

	return true;
#endif

}

bool SEASON3B::CNewUISkillList::UpdateKeyEvent()
{
	for (int i = 0; i < 9; ++i)
	{
		if (SEASON3B::IsPress('1' + i))
		{
			UseHotKey(i + 1);
		}
	}

	if (SEASON3B::IsPress('0'))
	{
		UseHotKey(0);
	}

	if (m_EventState == EVENT_BTN_HOVER_SKILLLIST)
	{
		if (SEASON3B::IsRepeat(VK_CONTROL))
		{
			for (int i = 0; i < 9; ++i)
			{
				if (SEASON3B::IsPress('1' + i))
				{
					SetHotKey(i + 1, m_iRenderSkillInfoType);

					return false;
				}
			}

			if (SEASON3B::IsPress('0'))
			{
				SetHotKey(0, m_iRenderSkillInfoType);

				return false;
			}
		}
	}

	if (SEASON3B::IsRepeat(VK_SHIFT))
	{
		for (int i = 0; i < 4; ++i)
		{
			if (SEASON3B::IsPress('1' + i))
			{
				Hero->CurrentSkill = AT_PET_COMMAND_DEFAULT + i;
				return false;
			}
		}
	}

	return true;
}

bool SEASON3B::CNewUISkillList::IsArrayUp(BYTE bySkill)
{
	for (int i = 0; i < SKILLHOTKEY_COUNT; ++i)
	{
		if (m_iHotKeySkillType[i] == bySkill)
		{

#if MAIN_UPDATE > 603
			if (i == 0 || i > 4)
				return true;
			else
				return false;
#else
			if (i == 0 || i > 5)
				return true;
			else
				return false;
#endif
		}
	}

	return false;
}

bool SEASON3B::CNewUISkillList::IsArrayIn(BYTE bySkill)
{
	for (int i = 0; i < SKILLHOTKEY_COUNT; ++i)
	{
		if (m_iHotKeySkillType[i] == bySkill)
		{
			return true;
		}
	}

	return false;
}

void SEASON3B::CNewUISkillList::SetHotKey(int iHotKey, int iSkillType)
{
	for (int i = 0; i < SKILLHOTKEY_COUNT; ++i)
	{
		if (m_iHotKeySkillType[i] == iSkillType)
		{
			m_iHotKeySkillType[i] = -1;
			break;
		}
	}

	m_iHotKeySkillType[iHotKey] = iSkillType;
}

int SEASON3B::CNewUISkillList::GetHotKey(int iHotKey)
{
	return m_iHotKeySkillType[iHotKey];
}

int SEASON3B::CNewUISkillList::GetSkillIndex(int iSkillType)
{
	int iReturn = -1;
	for (int i = 0; i < MAX_MAGIC; ++i)
	{
		if (CharacterAttribute->Skill[i] == iSkillType)
		{
			iReturn = i;
			break;
		}
	}

	return iReturn;
}

void SEASON3B::CNewUISkillList::UseHotKey(int iHotKey)
{
	if (m_iHotKeySkillType[iHotKey] != -1)
	{
		if (m_iHotKeySkillType[iHotKey] >= AT_PET_COMMAND_DEFAULT && m_iHotKeySkillType[iHotKey] < AT_PET_COMMAND_END)
		{
			if (Hero->m_pPet == NULL)
			{
				return;
			}
		}

		WORD wHotKeySkill = CharacterAttribute->Skill[m_iHotKeySkillType[iHotKey]];

		if (wHotKeySkill == 0)
		{
			return;
		}

		m_wHeroPriorSkill = CharacterAttribute->Skill[Hero->CurrentSkill];

		Hero->CurrentSkill = m_iHotKeySkillType[iHotKey];

		WORD bySkill = CharacterAttribute->Skill[Hero->CurrentSkill];


		if (
			g_pOption->IsAutoAttack() == true
			&& gMapManager->currentMap != WD_6STADIUM
			&& gMapManager->InChaosCastle() == false
			&& (bySkill == AT_SKILL_TELEPORT || bySkill == AT_SKILL_TELEPORT_B))
		{
			SelectedCharacter = -1;
			Attacking = -1;
		}
	}
}

bool SEASON3B::CNewUISkillList::Update()
{
	if (IsArrayIn(Hero->CurrentSkill) == true)
	{
		if (IsArrayUp(Hero->CurrentSkill) == true)
			m_bHotKeySkillListUp = true;
		else
			m_bHotKeySkillListUp = false;
	}

	if (Hero->m_pPet == NULL
		&& (Hero->CurrentSkill >= AT_PET_COMMAND_DEFAULT && Hero->CurrentSkill < AT_PET_COMMAND_END))
	{
		Hero->CurrentSkill = 0;
	}
	return true;
}

#ifdef test_main_dev
void SEASON3B::CNewUISkillList::RenderCurrentSkillAndHotSkillList()
{
#if MAIN_UPDATE > 303
	BYTE bySkillNumber = CharacterAttribute->SkillNumber;

	if (bySkillNumber > 0)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		RenderSkillIcon(Hero->CurrentSkill, m_iRenderSkillSlot.x, m_iRenderSkillSlot.y, 18.0, 25.0, false, 0.0, 0.0);

		int iStartSkillIndex = 1;

		if (m_bHotKeySkillListUp)
		{
			iStartSkillIndex = 5;
		}

		for (int i = 0; i < 4; ++i)
		{
			int iIndex = iStartSkillIndex + i;

			if (iIndex == 10)
				iIndex = 0;

			if (!(m_iHotKeySkillType[iIndex] == -1 || m_iHotKeySkillType[iIndex] >= AT_PET_COMMAND_DEFAULT && m_iHotKeySkillType[iIndex] < AT_PET_COMMAND_END && Hero->m_pPet == NULL))
			{
				RenderSkillIcon(m_iHotKeySkillType[iIndex], m_iRenderSkillHotKey[i].x, m_iRenderSkillHotKey[i].y, 18.0, 25.0, false);
			}
		}
	}
#else
	BYTE bySkillNumber = CharacterAttribute->SkillNumber;

	if (bySkillNumber > 0)
	{
		float RenderFrameX = PositionX_The_Mid(640.0);
		float RenderFrameY = PositionY_Add(0);
		RenderSkillIcon(Hero->CurrentSkill, RenderFrameX + 286.0, RenderFrameY + 451, 16.0, 20.0, false, 0.0, 0.0, true);
	}
#endif // MAIN_UPDATE == 303
}
#else
void SEASON3B::CNewUISkillList::RenderCurrentSkillAndHotSkillList()
{
#if MAIN_UPDATE > 303
	int i;
	float x, y, width, height;

	BYTE bySkillNumber = CharacterAttribute->SkillNumber;

	if (bySkillNumber > 0)
	{
		if (gmProtect->checkold_school())
		{
			float RenderFrameX = PositionX_The_Mid(640.0);
			float RenderFrameY = PositionY_Add(0);

			RenderSkillIcon(Hero->CurrentSkill, RenderFrameX + 304 + 6, RenderFrameY + 444 + 4, 20.0, 28.0, true, 20.0, 20.0, true);
		}
		else
		{
			int iStartSkillIndex = 1;
			if (m_bHotKeySkillListUp)
			{
				iStartSkillIndex = 6;
			}
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

			if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
			{
				float FrameX = PositionX_The_Mid(640.0);
				float FrameY = GetWindowsY - 72.0;

				width = 22.0;
				height = 26.0;

				x = FrameX + 331.5;
				y = FrameY + 34.0;

				for (i = 0; i < 5; ++i)
				{
					x = FrameX + (i * width) + 331.5;

					int iIndex = iStartSkillIndex + i;

					if (iIndex == 10)
						iIndex = 0;

					if (!(m_iHotKeySkillType[iIndex] == -1
						|| m_iHotKeySkillType[iIndex] >= AT_PET_COMMAND_DEFAULT && m_iHotKeySkillType[iIndex] < AT_PET_COMMAND_END && Hero->m_pPet == NULL))
					{
						RenderSkillIcon(m_iHotKeySkillType[iIndex], x + 3, y + 3, 16.0, 21.0, false);
						if (Hero->CurrentSkill == m_iHotKeySkillType[iIndex])
						{
							SEASON3B::RenderImageF(IMAGE_SKILLBOX_EX7002, x, y, 22.0, 26.0, 0.0, 0.0, 36.0, 44.0);
						}
					}
					SEASON3B::RenderNumber(x + 11, y - 5, iIndex, 0.75);
				}

				x = FrameX + 305.0;
				y = FrameY + 34.0;
				RenderSkillIcon(Hero->CurrentSkill, x + 3, y + 3, 16.0, 21.0, false);
				SEASON3B::RenderImageF(IMAGE_SKILLBOX_EX7001, x, y, 22.0, 26.0, 0.0, 0.0, 36.0, 44.0);
			}
			else
			{
				x = AddMiddleX() + 190; y = AddPositionY() + 431; width = 32; height = 38;

				for (i = 0; i < 5; ++i)
				{
					x += width;

					int iIndex = iStartSkillIndex + i;
					if (iIndex == 10)
					{
						iIndex = 0;
					}

					if (m_iHotKeySkillType[iIndex] == -1)
					{
						continue;
					}

					if (m_iHotKeySkillType[iIndex] >= AT_PET_COMMAND_DEFAULT && m_iHotKeySkillType[iIndex] < AT_PET_COMMAND_END)
					{
						if (Hero->m_pPet == NULL)
						{
							continue;
						}
					}

					if (Hero->CurrentSkill == m_iHotKeySkillType[iIndex])
					{
						SEASON3B::RenderImage(IMAGE_SKILLBOX_USE, x, y, width, height);
					}
					RenderSkillIcon(m_iHotKeySkillType[iIndex], x + 6, y + 6, 20, 28);
				}

				width = 20; x = AddMiddleX() + 392;
				height = 28; y = AddPositionY() + 437;

				RenderSkillIcon(Hero->CurrentSkill, x, y, width, height);
			}
		}
	}
#else
	if (CharacterAttribute->SkillNumber > 0)
	{
		RenderSkillIcon(Hero->CurrentSkill, pos_center() + 286.0, pos_botton() + 451, 16.0, 20.0, false, 0.0, 0.0, true);
	}
#endif // MAIN_UPDATE == 303
}
#endif // test_main_dev

bool SEASON3B::CNewUISkillList::Render()
{
#if MAIN_UPDATE > 603
	if ((mu_int32)CharacterAttribute->SkillNumber > 0 && m_bSkillList)
	{
		mu_float x, y, width, height;

		x = pos_center();
		y = pos_botton();
		mu_float fOrigX = x + 385.f;

		y += 360.f;
		width = 32.f;
		height = 38.f;
		x += 385.f;

		int iSkillCount = 0;

		for (int i = 0; i < MAX_MAGIC; ++i)
		{
			mu_int32 bySkillType = CharacterAttribute->Skill[i];

			if (bySkillType && (bySkillType < AT_SKILL_STUN || bySkillType > AT_SKILL_REMOVAL_BUFF))
			{
				if (Hero->m_pPet == NULL && bySkillType >= AT_PET_COMMAND_DEFAULT && bySkillType < AT_PET_COMMAND_END)
					continue;

				BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

				if (bySkillUseType != SKILL_USE_TYPE_MASTER && bySkillUseType != SKILL_USE_TYPE_MASTERLEVEL)
				{
					if (iSkillCount == 18)
						y -= height;

					if (iSkillCount >= 14)
					{
						if (iSkillCount < 14 || iSkillCount >= 18)
						{
							x = fOrigX - width * 12.f + (iSkillCount - 17) * width;
						}
						else
						{
							x = fOrigX - width * 8.f - (iSkillCount - 14) * width;
						}
					}
					else
					{
						int iQuotient = iSkillCount / 2;

						if (iSkillCount % 2)
							x = fOrigX - (iQuotient + 1) * width;
						else
							x = iQuotient * width + fOrigX;
					}
					iSkillCount++;

					if (i == Hero->CurrentSkill)
						SEASON3B::RenderImage(IMAGE_SKILLBOX_USE, x, y, width, height);
					else
						SEASON3B::RenderImage(IMAGE_SKILLBOX, x, y, width, height);
					RenderSkillIcon(i, x + 6, y + 6, 20, 28);
				}
			}
		}
	}

#elif MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		int i;
		float x, y, width, height;

		BYTE bySkillNumber = CharacterAttribute->SkillNumber;

		if (bySkillNumber > 0)
		{
			if (m_bSkillList == true)
			{
				float fOrigX = AddMiddleX() + 385.f;
				int iSkillType = 0;
				int iSkillCount = 0;

#if MAIN_UPDATE > 603
				width = 32; height = 38;
				y = pos_botton() + 360;
				x = pos_center() + 385.f;
				fOrigX = pos_center() + 385.f;

#else
				if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
				{
					fOrigX = AddMiddleX() + 335.f;
					y = AddPositionY() + 365;
				}
				else
				{
					fOrigX = AddMiddleX() + 385.f;
					y = AddPositionY() + 390;
				}

				width = 32; height = 38;

				if (gmProtect->checkold_school())
				{
					fOrigX = PositionX_The_Mid(640.0) + 320.0;
					y = PositionY_Add(0) + 370.0;
					width = 32.0; height = 36.0;
				}
#endif

				for (i = 0; i < MAX_MAGIC; ++i)
				{
					iSkillType = CharacterAttribute->Skill[i];

#if MAIN_UPDATE > 603
					if (Hero->m_pPet == NULL && iSkillType >= AT_PET_COMMAND_DEFAULT && iSkillType < AT_PET_COMMAND_END)
						continue;
#else
					if (iSkillType >= AT_PET_COMMAND_DEFAULT && iSkillType < AT_PET_COMMAND_END)
						continue;
#endif

					if (iSkillType != 0 && (iSkillType < AT_SKILL_STUN || iSkillType > AT_SKILL_REMOVAL_BUFF))
					{
						BYTE bySkillUseType = SkillAttribute[iSkillType].SkillUseType;

						if (bySkillUseType == SKILL_USE_TYPE_MASTER || bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
						{
							continue;
						}

						if (iSkillCount == 18)
						{
							y -= height;
						}

						if (iSkillCount < 14)
						{
							int iRemainder = iSkillCount % 2;
							int iQuotient = iSkillCount / 2;

							if (iRemainder == 0)
							{
								x = fOrigX + iQuotient * width;
							}
							else
							{
								x = fOrigX - (iQuotient + 1) * width;
							}
						}
						else if (iSkillCount >= 14 && iSkillCount < 18)
						{
							x = fOrigX - (8 * width) - ((iSkillCount - 14) * width);
						}
						else
						{
							x = fOrigX - (12 * width) + ((iSkillCount - 17) * width);
						}

						iSkillCount++;


						if (gmProtect->checkold_school())
						{
							RenderSkillIcon(i, x + 6.0, y + 4.0, 20.0, 28.0, true, 20.0, 20.0, true);
						}
						else
						{
							if (i == Hero->CurrentSkill)
								SEASON3B::RenderImage(IMAGE_SKILLBOX_USE, x, y, width, height);
							else
								SEASON3B::RenderImage(IMAGE_SKILLBOX, x, y, width, height);
							RenderSkillIcon(i, x + 6, y + 6, 20, 28);
						}
					}
				}
#if MAIN_UPDATE <= 603
				RenderPetSkill();
#endif

			}
		}
	}
	else
	{
		BYTE bySkillNumber = CharacterAttribute->SkillNumber;

		if (bySkillNumber > 0 && m_bSkillList)
		{
			int iSkillType = 0;
			int iSkillCount = 0;
			int iSkillReset = 0;
			float fOrigX = pos_center() + 320.0;

			float RenderSizeX = 32.f;
			float RenderSizeY = 36.0;
			float RenderFrameX = pos_center() + 320.0;
			float RenderFrameY = pos_botton() + 370.0;

			for (int i = 0; i < MAX_MAGIC; ++i)
			{
				iSkillType = CharacterAttribute->Skill[i];

				if (Hero->m_pPet == NULL && iSkillType >= AT_PET_COMMAND_DEFAULT && iSkillType < AT_PET_COMMAND_END)
					continue;

				if (iSkillType != 0 && (iSkillType < AT_SKILL_STUN || iSkillType > AT_SKILL_REMOVAL_BUFF))
				{
					BYTE bySkillUseType = SkillAttribute[iSkillType].SkillUseType;

					if (bySkillUseType == SKILL_USE_TYPE_MASTER || bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
					{
						continue;
					}

					if (iSkillCount % 18 == 0 && iSkillCount > 0)
					{
						iSkillReset = 0;
						RenderFrameX = 0.0;
						RenderFrameY -= RenderSizeY;
					}

					if (iSkillCount < 18)
					{
						int iRemainder = iSkillCount % 2;
						int iQuotient = iSkillCount / 2;

						if (iRemainder == 0)
							RenderFrameX = fOrigX + iQuotient * RenderSizeX;
						else
							RenderFrameX = fOrigX - (iQuotient + 1) * RenderSizeX;
					}
					else
					{
						RenderFrameX = fOrigX - (9 * RenderSizeX) + (iSkillReset * RenderSizeX); // De izquierda a derecha
					}

					iSkillReset++;
					iSkillCount++;
					RenderSkillIcon(i, RenderFrameX + 6.0, RenderFrameY + 4.0, 20.0, 28.0, true, 20.0, 20.0, true);
				}
			}
		}
	}
#else
	BYTE bySkillNumber = CharacterAttribute->SkillNumber;

	if (bySkillNumber > 0 && m_bSkillList)
	{
		int iSkillType = 0;
		int iSkillCount = 0;
		int iSkillReset = 0;
		float fOrigX = pos_center() + 320.0;

		float RenderSizeX = 32.f;
		float RenderSizeY = 36.0;
		float RenderFrameX = pos_center() + 320.0;
		float RenderFrameY = pos_botton() + 370.0;

		for (int i = 0; i < MAX_MAGIC; ++i)
		{
			iSkillType = CharacterAttribute->Skill[i];

			if (Hero->m_pPet == NULL && iSkillType >= AT_PET_COMMAND_DEFAULT && iSkillType < AT_PET_COMMAND_END)
				continue;

			if (iSkillType != 0 && (iSkillType < AT_SKILL_STUN || iSkillType > AT_SKILL_REMOVAL_BUFF))
			{
				BYTE bySkillUseType = SkillAttribute[iSkillType].SkillUseType;

				if (bySkillUseType == SKILL_USE_TYPE_MASTER || bySkillUseType == SKILL_USE_TYPE_MASTERLEVEL)
				{
					continue;
				}

				if (iSkillCount % 18 == 0 && iSkillCount > 0)
				{
					iSkillReset = 0;
					RenderFrameX = 0.0;
					RenderFrameY -= RenderSizeY;
				}

				if (iSkillCount < 18)
				{
					int iRemainder = iSkillCount % 2;
					int iQuotient = iSkillCount / 2;

					if (iRemainder == 0)
						RenderFrameX = fOrigX + iQuotient * RenderSizeX;
					else
						RenderFrameX = fOrigX - (iQuotient + 1) * RenderSizeX;
				}
				else
				{
					RenderFrameX = fOrigX - (9 * RenderSizeX) + (iSkillReset * RenderSizeX); // De izquierda a derecha
				}

				iSkillReset++;
				iSkillCount++;
				RenderSkillIcon(i, RenderFrameX + 6.0, RenderFrameY + 4.0, 20.0, 28.0, true, 20.0, 20.0, true);
			}
		}
	}
#endif

	if (m_bRenderSkillInfo == true && m_pNewUI3DRenderMng)
	{
		m_pNewUI3DRenderMng->RenderUI2DEffect(INVENTORY_CAMERA_Z_ORDER, UI2DEffectCallback, this, 0, 0);
	}

	return true;
}

void SEASON3B::CNewUISkillList::RenderSkillInfo()
{
	::RenderSkillInfo(m_iRenderSkillInfoPosX + 15, m_iRenderSkillInfoPosY - 10, m_iRenderSkillInfoType);
}

float SEASON3B::CNewUISkillList::GetLayerDepth()
{
	return 5.2f;
}

WORD SEASON3B::CNewUISkillList::GetHeroPriorSkill()
{
	return m_wHeroPriorSkill;
}

void SEASON3B::CNewUISkillList::SetHeroPriorSkill(BYTE bySkill)
{
	m_wHeroPriorSkill = bySkill;
}

void SEASON3B::CNewUISkillList::RenderPetSkill()
{
	if (Hero->m_pPet == NULL)
	{
		return;
	}

	float x, y, width, height;

	width = 32; height = 38;

	if (gmProtect->LookAndFeel == 3 || gmProtect->LookAndFeel == 4)
	{
		x = AddMiddleX() + 303.f;
		y = AddPositionY() + 327;
	}
	else
	{
		x = AddMiddleX() + 353.f;
		y = AddPositionY() + 352;
	}
	for (int i = AT_PET_COMMAND_DEFAULT; i < AT_PET_COMMAND_END; ++i)
	{
		if (i == Hero->CurrentSkill)
		{
			SEASON3B::RenderImage(IMAGE_SKILLBOX_USE, x, y, width, height);
		}
		else
		{
			SEASON3B::RenderImage(IMAGE_SKILLBOX, x, y, width, height);
		}

		RenderSkillIcon(i, x + 6, y + 6, 20, 28);
		x += width;
	}
}

void SEASON3B::CNewUISkillList::RenderSkillIcon(int iIndex, float x, float y, float width, float height, bool RenderNumber, float NBX, float NBY, bool Renderback)
{
	WORD bySkillType = CharacterAttribute->Skill[iIndex];

	if (bySkillType == 0)
	{
		return;
	}

	bool bCantSkill = false;
	int Skill_Icon = SkillAttribute[bySkillType].Magic_Icon;
	BYTE bySkillUseType = SkillAttribute[bySkillType].SkillUseType;

	int sc_type_helper = Hero->Helper.Type;
	OBJECT* so = &Hero->Object;

	if (!gSkillManager.DemendConditionCheckSkill(bySkillType))
		bCantSkill = true;

	if (!IsCanBCSkill(bySkillType))
		bCantSkill = true;

	if (g_isCharacterBuff((&Hero->Object), eBuff_AddSkill) && bySkillUseType == SKILL_USE_TYPE_BRAND)
	{
		bCantSkill = true;
	}

	if (bySkillType == Skill_Impale)
	{
		if (so->movementType == Movement::Fenrir || so->movementType == Movement::Uniria || so->movementType == Movement::Dinorant)
		{
			int iTypeL = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
			int iTypeR = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;

			if ((iTypeL < ITEM_SPEAR || iTypeL >= ITEM_BOW) && (iTypeR < ITEM_SPEAR || iTypeR >= ITEM_BOW))
			{
				bCantSkill = true;
			}
		}
		else
		{
			bCantSkill = true;
		}
	}

	/*if (bySkillType >= AT_SKILL_BLOCKING
		&& bySkillType <= AT_SKILL_SWORD5
		&& (sc_type_helper == MODEL_HELPER + 2 || sc_type_helper == MODEL_HELPER + 3 || sc_type_helper == MODEL_HELPER + 37))
	{
		bCantSkill = true;
	}

	if ((bySkillType == AT_SKILL_ICE_BLADE || bySkillType == Skill_Power_Slash_Strengthener)
		&& (sc_type_helper == MODEL_HELPER + 2 || sc_type_helper == MODEL_HELPER + 3 || sc_type_helper == MODEL_HELPER + 37))
	{
		bCantSkill = true;
	}*/

	int iEnergy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;

	if (g_csItemOption.IsDisableSkill(bySkillType, iEnergy))
		bCantSkill = true;


	if (bySkillType == AT_SKILL_PARTY_TELEPORT && PartyNumber <= 0)
		bCantSkill = true;


	if (bySkillType == AT_SKILL_PARTY_TELEPORT && gMapManager->IsDoppelRenewal())
		bCantSkill = true;

	if (bySkillType == Skill_Earthshake
		|| bySkillType == Skill_Dark_Horse_Strengthener
		|| bySkillType == Skill_Earthshake_Mastery
		|| bySkillType == Skill_Earthshake_Strengthener)
	{
		BYTE byDarkHorseLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;
		if (byDarkHorseLife == 0 || so->movementType != Movement::Horse)
		{
			bCantSkill = true;
		}
	}

	if (bySkillType == Skill_Infinity_Arrow
		|| bySkillType == Skill_Infinity_Arrow_Strengthener
		|| bySkillType == Skill_Expansion_of_Wizardry
		|| bySkillType == Skill_Expansion_of_Wizardry_Power_Up
		|| bySkillType == Skill_Expansion_of_Wizardry_Mastery)
	{
		if (g_csItemOption.IsDisableSkill(bySkillType, iEnergy))
			bCantSkill = true;

		if (g_isCharacterBuff((&Hero->Object), eBuff_InfinityArrow)
			|| g_isCharacterBuff((&Hero->Object), EFFECT_INFINITY_ARROW_IMPROVED)
			|| g_isCharacterBuff((&Hero->Object), eBuff_SwellOfMagicPower)
			|| g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_IMPROVED)
			|| g_isCharacterBuff((&Hero->Object), EFFECT_MAGIC_CIRCLE_ENHANCED)
			)
		{
			bCantSkill = true;
		}
	}

	if (bySkillType == Skill_Fire_Slash || bySkillType == Skill_Fire_Slash_Strengthener)
	{
		DWORD Strength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;

		if (Strength < 596)
			bCantSkill = true;

		int iTypeL = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
		int iTypeR = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;

		if (!(iTypeR != -1 && (iTypeR < ITEM_STAFF || iTypeR >= ITEM_STAFF + MAX_ITEM_INDEX) && (iTypeL < ITEM_STAFF || iTypeL >= ITEM_STAFF + MAX_ITEM_INDEX)))
		{
			bCantSkill = true;
		}
	}

	if ((bySkillType == AT_SKILL_PARALYZE || bySkillType == Skill_Ice_Arrow_Strengthener)
		&& (CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity) < 646)
	{
		bCantSkill = true;
	}

	if (bySkillType == AT_SKILL_WHEEL || bySkillType == Skill_Twisting_Slash_Strengthener || bySkillType == Skill_Twisting_Slash_Strengthener2 || bySkillType == Skill_Twisting_Slash_Mastery)
	{
		int iTypeL = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
		int iTypeR = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;

		if (!(iTypeR != -1 && (iTypeR < ITEM_STAFF || iTypeR >= ITEM_STAFF + MAX_ITEM_INDEX) && (iTypeL < ITEM_STAFF || iTypeL >= ITEM_STAFF + MAX_ITEM_INDEX)))
		{
			bCantSkill = true;
		}
	}

	if (gMapManager->InChaosCastle() == true)
	{
		if (bySkillType == Skill_Earthshake
			|| bySkillType == Skill_Fire_Breath
			|| bySkillType == Skill_Dark_Horse_Strengthener
			|| bySkillType == Skill_Earthshake_Mastery
			|| (bySkillType >= Skill_PetRaven_Default && bySkillType <= Skill_PetRaven_Target)
			|| bySkillType == Skill_Earthshake_Strengthener)
		{
			bCantSkill = true;
		}
	}
	else if (bySkillType == Skill_Earthshake || bySkillType == Skill_Dark_Horse_Strengthener || bySkillType == Skill_Earthshake_Mastery || bySkillType == Skill_Earthshake_Strengthener)
	{
		BYTE byDarkHorseLife = CharacterMachine->Equipment[EQUIPMENT_HELPER].Durability;
		if (byDarkHorseLife == 0)
		{
			bCantSkill = true;
		}
	}

	DWORD iCharisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;

	if (g_csItemOption.IsDisableSkill(bySkillType, iEnergy, iCharisma))
		bCantSkill = true;

	if (!g_CMonkSystem.IsSwordformGlovesUseSkill(bySkillType))
		bCantSkill = true;

	//if (g_CMonkSystem.IsRideNotUseSkill(bySkillType, sc_type_helper))
	//	bCantSkill = true;

	ITEM* pLeftRing = &CharacterMachine->Equipment[EQUIPMENT_RING_LEFT];
	ITEM* pRightRing = &CharacterMachine->Equipment[EQUIPMENT_RING_RIGHT];

	if (g_CMonkSystem.IsChangeringNotUseSkill(pLeftRing->Type, pRightRing->Type, pLeftRing->Level, pRightRing->Level) && gCharacterManager.GetBaseClass(Hero->Class) == CLASS_RAGEFIGHTER)
	{
		bCantSkill = true;
	}

	float fU = 0.0;
	float fV = 0.0;
	int iKindofSkill = 0;

	if (!g_csItemOption.Special_Option_Check(bySkillType))
		bCantSkill = true;

	if (!g_csItemOption.Special_Option_Check(bySkillType))
		bCantSkill = true;

	float uWidth = 20.0;
	float vHeight = 28.0;

	if ((signed int)bySkillType < Skill_PetRaven_Default || (signed int)bySkillType > Skill_PetRaven_Target + 1)
	{
		if (bySkillType == 76)
		{
			fU = uWidth * 4.0 / 256.0;
			fV = 0.0;
			iKindofSkill = KOS_COMMAND;
		}
		else if ((signed int)bySkillType < Skill_Drain_Life || (signed int)bySkillType > Skill_Damage_Reflection)
		{
			if ((signed int)bySkillType < Skill_Sleep || (signed int)bySkillType > Skill_Sleep + 1)
			{
				if (bySkillType == Skill_Berserker)
				{
					fU = uWidth * 10.0 / 256.0;
					fV = vHeight * 3.0 / 256.0;
					iKindofSkill = KOS_SKILL2;
				}
				else if ((signed int)bySkillType < Skill_Weakness || (signed int)bySkillType > Skill_Innovation)
				{
					if ((signed int)bySkillType < Skill_Explosion_Sahamutt || (signed int)bySkillType > Skill_Requiem)
					{
						if (bySkillType == Skill_Pollution)
						{
							fU = uWidth * 11.0 / 256.0;
							fV = vHeight * 3.0 / 256.0;
							iKindofSkill = KOS_SKILL2;
						}
						else if (bySkillType == Skill_Strike_of_Destruction)
						{
							fU = uWidth * 7.0 / 256.0;
							fV = vHeight * 2.0 / 256.0;
							iKindofSkill = KOS_SKILL2;
						}
						else if (bySkillType != Skill_Chaotic_Diseier_Strengthener)
						{
							switch (bySkillType)
							{
							case Skill_Chaotic_Diseier:
								fU = uWidth * 3.0 / 256.0;
								fV = vHeight * 8.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Recovery:
								fU = uWidth * 9.0 / 256.0;
								fV = vHeight * 2.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Multi_Shot:
								if (gCharacterManager.GetEquipedBowType_Skill() == BOWTYPE_NONE)
									bCantSkill = 1;
								fU = uWidth * 0.0 / 256.0;
								fV = vHeight * 8.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Flame_Strike:
							{
								int iTypeL = CharacterMachine->Equipment[EQUIPMENT_WEAPON_LEFT].Type;
								int iTypeR = CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT].Type;
								if (iTypeR == -1 || iTypeR >= ITEM_STAFF && iTypeR < ITEM_STAFF + MAX_ITEM_INDEX || iTypeL >= iTypeR && iTypeL < ITEM_STAFF + MAX_ITEM_INDEX)
									bCantSkill = true;
								fU = uWidth * 1.0 / 256.0;
								fV = vHeight * 8.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
							}
							break;
							case Skill_Gigantic_Storm:
								fU = uWidth * 2.0 / 256.0;
								fV = vHeight * 8.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Lightning_Shock:
								fU = uWidth * 2.0 / 256.0;
								fV = vHeight * 3.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Chain_Lightning_Strengthener:
								fU = uWidth * 6.0 / 256.0;
								fV = vHeight * 8.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							case Skill_Expansion_of_Wizardry:
								fU = uWidth * 8.0 / 256.0;
								fV = vHeight * 2.0 / 256.0;
								iKindofSkill = KOS_SKILL2;
								break;
							default:
								if (bySkillUseType == Skill_Fire_Ball)
								{
									fU = uWidth / 256.0 * (double)(Skill_Icon % 12);
									fV = vHeight / 256.0 * (double)(Skill_Icon / 12 + 4);
									iKindofSkill = KOS_SKILL2;
								}
								else if ((signed int)bySkillType < Skill_Killing_Blow)
								{
									if ((signed int)bySkillType < 57)
									{
										fU = (double)((bySkillType - 1) % 8) * uWidth / 256.0;
										fV = (double)((bySkillType - 1) / 8) * vHeight / 256.0;
										iKindofSkill = KOS_SKILL1;
									}
									else
									{
										fU = (double)((bySkillType - 57) % 8) * uWidth / 256.0;
										fV = (double)((bySkillType - 57) / 8) * vHeight / 256.0;
										iKindofSkill = KOS_SKILL2;
									}
								}
								else
								{
									fU = (double)((bySkillType - 260) % 12) * uWidth / 256.0;
									fV = (double)((bySkillType - 260) / 12) * vHeight / 256.0;
									iKindofSkill = KOS_SKILL3;
								}
								break;
							}
						}
					}
					else
					{
						fU = (double)((bySkillType - 217) % 8) * uWidth / 256.0;
						fV = vHeight * 3.0 / 256.0;
						iKindofSkill = KOS_SKILL2;
					}
				}
				else
				{
					fU = (double)(bySkillType - 213) * uWidth / 256.0;
					fV = vHeight * 3.0 / 256.0;
					iKindofSkill = KOS_SKILL2;
				}
			}
			else
			{
				fU = (double)((bySkillType - 215) % 8) * uWidth / 256.0;
				fV = vHeight * 3.0 / 256.0;
				iKindofSkill = KOS_SKILL2;
			}
		}
		else
		{
			fU = (double)((bySkillType - 214) % 8) * uWidth / 256.0;
			fV = vHeight * 3.0 / 256.0;
			iKindofSkill = KOS_SKILL2;
		}
	}
	else
	{
		fU = (double)((bySkillType - 120) % 8) * uWidth / 256.0;
		fV = (double)((bySkillType - 120) / 8) * vHeight / 256.0;
		iKindofSkill = KOS_COMMAND;
	}

	if (bySkillUseType == SKILL_USE_TYPE_MASTERACTIVE)
		iKindofSkill = KOS_SKILL4;

	int iSkillIndex = 0;
	switch (iKindofSkill)
	{
	case KOS_COMMAND:
		iSkillIndex = IMAGE_COMMAND;
		if (bCantSkill == 1)
			iSkillIndex = IMAGE_NON_COMMAND;
		break;
	case KOS_SKILL1:
		iSkillIndex = IMAGE_SKILL1;
		if (bCantSkill == 1)
			iSkillIndex = IMAGE_NON_SKILL1;
		break;
	case KOS_SKILL2:
		iSkillIndex = IMAGE_SKILL2;
		if (bCantSkill == 1)
			iSkillIndex = IMAGE_NON_SKILL2;
		break;
	case KOS_SKILL3:
		iSkillIndex = IMAGE_SKILL3;
		if (bCantSkill == 1)
			iSkillIndex = IMAGE_NON_SKILL3;
		break;
	case KOS_SKILL4:
		iSkillIndex = IMAGE_MASTER_SKILL;
		if (bCantSkill == 1)
			iSkillIndex = IMAGE_NON_MASTER_SKILL;
		break;
	default:
		break;
	}
	if (iSkillIndex)
	{
		float uw = 0.0;
		float vh = 0.0;
		if (bySkillUseType == 4)
		{
			fU = uWidth / 512.0 * (double)(Skill_Icon % 25);
			fV = vHeight / 512.0 * (double)(Skill_Icon / 25);
			uw = uWidth / 512.0;
			vh = vHeight / 512.0;
			RenderBitmap(iSkillIndex, x, y, width, height, fU, fV, uw, vh, true, true, 0.0);
		}
		else
		{
			uw = uWidth / 256.0;
			vh = vHeight / 256.0;

			if (gmProtect->checkold_school())
			{
				if (bCantSkill)
					glColor3f(1.0, 0.5, 0.5);
				else
					glColor3f(1.0, 0.89999998, 0.80000001);

				if (Renderback)
				{
					float percent = height / 28.0;
					float nwidth = 32.0 * percent;
					float nhight = 36.0 * percent;

					SEASON3B::RenderImage(IMAGE_SKILLBOX, x + ((width - nwidth) / 2.f), y + ((height - nhight) / 2.f), nwidth, nhight, 0.0, 0.0, 1.0, 36 / 64.0);
				}
			}
			RenderBitmap(iSkillIndex, x, y, width, height, fU, fV, uw, vh, true, true, 0.0);
		}
	}

	int iHotKey = -1;
	for (int i = 0; i < SKILLHOTKEY_COUNT; ++i)
	{
		if (m_iHotKeySkillType[i] == iIndex)
		{
			iHotKey = i;
			break;
		}
	}

	if (iHotKey != -1 && RenderNumber == TRUE)
	{
		glColor3f(1.f, 0.9f, 0.8f);
		SEASON3B::RenderNumber(x + NBX, y + NBY, iHotKey);
	}

	if ((bySkillType != 262 && bySkillType != 265 && bySkillType != 264 && bySkillType != 558 && bySkillType != 560 || !bCantSkill)
		&& bySkillType != 77
		&& bySkillType != 233
		&& bySkillType != 380
		&& bySkillType != 383
		&& bySkillType != 441)
	{
		RenderSkillDelay(iIndex, x, y, width, height);
	}
}

void SEASON3B::CNewUISkillList::RenderSkillDelay(int iIndex, float x, float y, float width, float height)
{
	int iSkillDelay = CharacterAttribute->SkillDelay[iIndex];

	if (iSkillDelay > 0)
	{
		int iSkillType = CharacterAttribute->Skill[iIndex];

		int iSkillMaxDelay = SkillAttribute[iSkillType].Delay;

		float fPersent = (float)(iSkillDelay / (float)iSkillMaxDelay);

		EnableAlphaTest(true);
		glColor4f(1.f, 0.5f, 0.5f, 0.5f);
		float fdeltaH = height * fPersent;
		RenderColor(x, y + height - fdeltaH, width, fdeltaH);
		EndRenderColor();
	}
}

bool SEASON3B::CNewUISkillList::IsSkillListUp()
{
	return m_bHotKeySkillListUp;
}

void SEASON3B::CNewUISkillList::ResetMouseLButton()
{
	MouseLButton = false;
	MouseLButtonPop = false;
	MouseLButtonPush = false;
}

void SEASON3B::CNewUISkillList::SetPosition(int iType, int x, int y)
{
	if (iType == 0)
	{
		m_iRenderSkillSlot.x = x;
		m_iRenderSkillSlot.y = y;
	}
	else
	{
		iType -= 1;
		m_iRenderSkillHotKey[iType].x = x;
		m_iRenderSkillHotKey[iType].y = y;
	}
}

void SEASON3B::CNewUISkillList::UI2DEffectCallback(LPVOID pClass, DWORD dwParamA, DWORD dwParamB)
{
	if (pClass)
	{
		CNewUISkillList* pSkillList = (CNewUISkillList*)(pClass);
		pSkillList->RenderSkillInfo();
	}
}

void SEASON3B::CNewUIMainFrameWindow::SetPreExp_Wide(__int64 dwPreExp)
{
	m_loPreExp = dwPreExp;
}

void SEASON3B::CNewUIMainFrameWindow::SetGetExp_Wide(__int64 dwGetExp)
{
	m_loGetExp = dwGetExp;

	if (m_loGetExp > 0)
	{
		m_bExpEffect = true;
		m_dwExpEffectTime = timeGetTime();
	}
}

void SEASON3B::CNewUIMainFrameWindow::SetPreExp(DWORD dwPreExp)
{
	m_dwPreExp = dwPreExp;
}

void SEASON3B::CNewUIMainFrameWindow::SetGetExp(DWORD dwGetExp)
{
	m_dwGetExp = dwGetExp;

	if (m_dwGetExp > 0)
	{
		m_bExpEffect = true;
		m_dwExpEffectTime = timeGetTime();
	}
}

void SEASON3B::CNewUIMainFrameWindow::SetBtnState(int iBtnType, bool bStateDown)
{
#if MAIN_UPDATE > 303
	switch (iBtnType)
	{
#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
	case MAINFRAME_BTN_PARTCHARGE:
	{
		if (!gmProtect->checkold_school())
		{
			if (bStateDown)
			{
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_UP, 2);
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_OVER, 3);
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			else
			{
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_UP, 0);
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_OVER, 1);
				m_BtnCShop.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			m_BtnCShop.ChangeFrame(BUTTON_STATE_UP);
		}
	}
	break;
#endif //defined defined PBG_ADD_INGAMESHOP_UI_MAINFRAME
	case MAINFRAME_BTN_CHAINFO:
	{
		if (bStateDown)
		{
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_UP, 2);
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_OVER, 3);
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_DOWN, 2);
		}
		else
		{
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_UP, 0);
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_OVER, 1);
			m_BtnChaInfo.ChangeButtonState(BUTTON_STATE_DOWN, 2);
		}
		m_BtnChaInfo.ChangeFrame(BUTTON_STATE_UP);
	}
	break;
	case MAINFRAME_BTN_MYINVEN:
	{
		if (bStateDown)
		{
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_UP, 2);
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_OVER, 3);
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_DOWN, 2);
		}
		else
		{
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_UP, 0);
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_OVER, 1);
			m_BtnMyInven.ChangeButtonState(BUTTON_STATE_DOWN, 2);
		}
		m_BtnMyInven.ChangeFrame(BUTTON_STATE_UP);
	}
	break;
	case MAINFRAME_BTN_FRIEND:
	{
		if (gmProtect->LookAndFeel != 1)
		{
			if (bStateDown)
			{
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_UP, 2);
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_OVER, 3);
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			else
			{
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_UP, 0);
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_OVER, 1);
				m_BtnFriend.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			m_BtnFriend.ChangeFrame(BUTTON_STATE_UP);
		}
	}
	break;
	case MAINFRAME_BTN_WINDOW:
	{
		if (gmProtect->LookAndFeel != 1)
		{
			if (bStateDown)
			{
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_UP, 2);
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_OVER, 3);
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			else
			{
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_UP, 0);
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_OVER, 1);
				m_BtnWindow.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			m_BtnWindow.ChangeFrame(BUTTON_STATE_UP);
		}
	}
	break;
	case MAINFRAME_BTN_GUILD:
	{
		if (gmProtect->LookAndFeel >= 1 && gmProtect->LookAndFeel <= 4)
		{
			if (bStateDown)
			{
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_UP, 2);
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_OVER, 3);
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_DOWN, 2);
				m_BtnWinGuild.ChangeImgIndex(IMAGE_MENU_BTN_GUILD, 2);
			}
			else
			{
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_UP, 0);
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_OVER, 1);
				m_BtnWinGuild.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			m_BtnWinGuild.ChangeFrame(BUTTON_STATE_UP);
		}
	}
	break;
	case MAINFRAME_BTN_PARTY:
	{
		if (gmProtect->checkold_school())
		{
			if (bStateDown)
			{
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_UP, 2);
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_OVER, 3);
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			else
			{
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_UP, 0);
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_OVER, 1);
				m_BtnWinParty.ChangeButtonState(BUTTON_STATE_DOWN, 2);
			}
			m_BtnWinGuild.ChangeFrame(BUTTON_STATE_UP);
		}
	}
	break;
	}
#endif // MAIN_UPDATE != 303
}

#ifdef test_main_dev
void SEASON3B::CNewUIMainFrameWindow::SetHotPosition(int iHotType, float x1, float y1, float x2, float y2)
{
	m_ItemHotKey.SetPosition(iHotType, x1, y1, x2, y2);
}

void SEASON3B::CNewUIMainFrameWindow::SetBtnPosition(int iBtnType, int posX, int posY)
{
	switch (iBtnType)
	{
	case MAINFRAME_BTN_PARTCHARGE:
	{
		if (!gmProtect->checkold_school())
		{
			m_BtnCShop.SetPos(posX, posY);
		}
	}
	break;
	case MAINFRAME_BTN_CHAINFO:
	{
		m_BtnChaInfo.SetPos(posX, posY);
	}
	break;
	case MAINFRAME_BTN_MYINVEN:
	{
		m_BtnMyInven.SetPos(posX, posY);
	}
	break;
	case MAINFRAME_BTN_FRIEND:
	{
		if (gmProtect->LookAndFeel != 1)
		{
			m_BtnFriend.SetPos(posX, posY);
		}
	}
	break;
	case MAINFRAME_BTN_WINDOW:
	{
		if (gmProtect->LookAndFeel != 1)
		{
			m_BtnWindow.SetPos(posX, posY);
		}
	}
	break;
	case MAINFRAME_BTN_GUILD:
	{
		if (gmProtect->LookAndFeel >= 1 && gmProtect->LookAndFeel <= 4)
		{
			m_BtnWinGuild.SetPos(posX, posY);
		}
	}
	break;
	case MAINFRAME_BTN_PARTY:
	{
		if (gmProtect->checkold_school())
		{
			m_BtnWinParty.SetPos(posX, posY);
		}
	}
	break;
	}
}

void SEASON3B::CNewUIMainFrameWindow::SetSkillPosition(int iType, int posX, int posY)
{
	if (g_pSkillList)
	{
		g_pSkillList->SetPosition(iType, posX, posY);
	}
}
#endif // test_main_dev


void SEASON3B::CNewUIMainFrameWindow::wope_render_btn()
{
	if (GMProtect->shutdown_oficial_helper())
	{
		if (gmAIController->IsRunning())
		{
			if (SEASON3B::CheckMouseIn(pos_center() + 447.f, pos_botton() + 452.f, 18.0, 11.0) && MouseLButton)
				SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 452.f, 18.0, 12.0, 32.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- play
			else
				SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 452.f, 18.0, 12.0, 32.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- play
		}
		else
		{
			if (SEASON3B::CheckMouseIn(pos_center() + 447.f, pos_botton() + 452.f, 18.0, 11.0) && MouseLButton)
				SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 452.f, 18.0, 12.0, 1.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- play
			else
				SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 452.f, 18.0, 12.0, 1.0 / 120.0, 2.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- play
		}

		if (SEASON3B::CheckMouseIn(pos_center() + 446.f, pos_botton() + 465.f, 18.0, 11.0) && MouseLButton)
			SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 465.f, 18.0, 12.0, 1.0 / 120.0, 26.0 / 64.0, 32.0 / 120.0, 22 / 64.0, RGBA(100, 100, 100, 255)); //-- config
		else
			SEASON3B::RenderImage(IMAGE_MENU_2_1, pos_center() + 447.f, pos_botton() + 465.f, 18.0, 12.0, 1.0 / 120.0, 26.0 / 64.0, 32.0 / 120.0, 22 / 64.0); //-- config
	}
}

bool SEASON3B::CNewUIMainFrameWindow::wope_update_btn()
{
	if (GMProtect->shutdown_oficial_helper() && SEASON3B::IsRelease(VK_LBUTTON))
	{
		if (SEASON3B::CheckMouseIn(pos_center() + 447.f, pos_botton() + 452.f, 18.0, 12.0))
		{
			if (gmAIController->IsRunning())
			{
				SendRequestStartHelper(true);
			}
			else
			{
				if (gmAIController->CanUseAIController())
				{
					SendRequestStartHelper(false);
				}
			}
			PlayBuffer(SOUND_CLICK01);
			return true;
		}
		else if (SEASON3B::CheckMouseIn(pos_center() + 447.f, pos_botton() + 465.f, 18.0, 12.0))
		{
			g_pNewUISystem->Toggle(SEASON3B::INTERFACE_MACRO_OFICIAL);
			PlayBuffer(SOUND_CLICK01);

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MACRO_OFICIAL))
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_MACRO_OFICIAL_SUB);
			}
			return true;
		}
	}

	return false;
}
