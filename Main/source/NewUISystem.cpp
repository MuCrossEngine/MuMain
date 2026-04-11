
#include "stdafx.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "NewUIMessageBox.h"
#include "wsclientinline.h"
#include "PersonalShopTitleImp.h"
#include "w_CursedTemple.h"
#include "MapManager.h"
#include "CGMResetManager.h"
#include "./Utilities/Log/muConsoleDebug.h"

using namespace SEASON3B;

SEASON3B::CNewUISystem::CNewUISystem()
{
	m_pNewUIMng = NULL;
	m_pNewUI3DRenderMng = NULL;
	m_pNewUIHotKey = NULL;

	m_pNewChatInputBox = NULL;
	m_pNewChatLogWindow = NULL;
	m_pNewSlideWindow = NULL;
	m_pNewFriendWindow = NULL;
	m_pNewMainFrameWindow = NULL;
	m_pNewSkillList = NULL;
	m_pNewItemMng = NULL;
	m_pNewMyInventory = NULL;
	m_pNewNPCShop = NULL;
	m_pNewPetInfoWindow = NULL;
	m_pNewMixInventory = NULL;
	m_pNewCastleWindow = NULL;
	m_pNewGuardWindow = NULL;
	m_pNewGatemanWindow = NULL;
	m_pNewGateSwitchWindow = NULL;
	m_pNewStorageInventory = NULL;
	m_pNewGuildMakeWindow = NULL;
	m_pNewGuildInfoWindow = NULL;
	m_pNewMyShopInventory = NULL;
	m_pNewPurchaseShopInventory = NULL;
	m_pNewCharacterInfoWindow = NULL;
	m_pNewMyQuestInfoWindow = NULL;
	m_pNewPartyInfoWindow = NULL;
	m_pNewPartyListWindow = NULL;
	m_pNewNPCQuest = NULL;
	m_pNewEnterBloodCastle = NULL;
	m_pNewEnterDevilSquare = NULL;
	m_pNewBloodCastle = NULL;
	m_pNewTrade = NULL;
	m_pNewKanturu2ndEnterNpc = NULL;
	m_pNewKanturuInfoWindow = NULL;
	m_pNewCatapultWindow = NULL;
	m_pNewChaosCastleTime = NULL;
	m_pNewBattleSoccerScore = NULL;
	m_pNewCommandWindow = NULL;
	m_pNewHeroPositionInfo = NULL;
	m_pNewWindowMenu = NULL;
	m_pNewOptionWindow = NULL;
	m_pNewHelpWindow = NULL;
	m_pNewItemExplanationWindow = NULL;
	m_pNewSetItemExplanation = NULL;
	m_pNewQuickCommandWindow = NULL;
	m_pNewMoveCommandWindow = NULL;
	m_pNewDuelWindow = NULL;
	m_pNewNameWindow = NULL;
	m_pNewSiegeWarfare = NULL;
	m_pNewItemEnduranceInfo = NULL;
	m_pNewBuffWindow = NULL;
	m_pNewCursedTempleEnterWindow = NULL;
	m_pNewCursedTempleWindow = NULL;
	m_pNewCursedTempleResultWindow = NULL;
	m_pNewCryWolfInterface = NULL;
#if MAIN_UPDATE > 303
	m_pNewUIMasterSkillTree = NULL;
#endif // MAIN_UPDATE != 303
	m_pNewGoldBowman = NULL;
	m_pNewGoldBowmanLena = NULL;
	m_pNewLuckyCoinRegistration = NULL;
	m_pNewExchangeLuckyCoinWindow = NULL;
	m_pNewDuelWatchWindow = NULL;
	m_pNewDuelWatchMainFrameWindow = NULL;
	m_pNewDuelWatchUserListWindow = NULL;
#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
	m_pNewInGameShop = NULL;
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME
	m_pNewDoppelGangerWindow = NULL;
	m_pNewDoppelGangerFrame = NULL;
	m_pNewNPCDialogue = NULL;
	m_pNewQuestProgress = NULL;
	m_pNewQuestProgressByEtc = NULL;
	m_pNewEmpireGuardianNPC = NULL;
	m_pNewEmpireGuardianTimer = NULL;
	m_pNewMiniMap = NULL;
	m_pNewGensRanking = NULL;
	m_pNewUnitedMarketPlaceWindow = NULL;
	m_pNewUILuckyItemWnd = NULL;
	m_pNewUIInvenExpansion = NULL;
	m_pNewUIStorageExpansion = NULL;

	m_pNewUIMacroMain = NULL;
	m_pNewUIMacroSub = NULL;
	m_pNewUIMacroGaugeBar = NULL;
	m_pNewUIInventoryJewel = NULL;
#ifdef EFFECT_MNG_HANDLE
	m_gfxEffectHandle = NULL;
#endif // EFFECT_MNG_HANDLE

#ifdef PACK_FILE_DECRYPT_H
	m_pNewUIItemTooltip = NULL;
#endif // PACK_FILE_DECRYPT_H

	m_pScriptItem = NULL;
	m_iCamWebzen = NULL;

	m_NewUIRankTop = NULL;
	m_NewUIMenuUser = NULL;
	m_NewUIMemberShip = NULL;
	m_NewUIReconnect = NULL;
	m_NewUICommandUser = NULL;
	m_NewUIEventTimeList = NULL;

	m_pNewUIInventoryCore = NULL;

	m_pNewUIMixExpansion = NULL;
}

SEASON3B::CNewUISystem::~CNewUISystem()
{
	Release();
}

bool SEASON3B::CNewUISystem::Create()
{
	m_pNewUIMng = new CNewUIManager;

	m_pNewUI3DRenderMng = new CNewUI3DRenderMng;
	if (false == m_pNewUI3DRenderMng->Create(m_pNewUIMng))
		return false;

	m_pNewChatLogWindow = new CNewUIChatLogWindow;

#if MAIN_UPDATE > 603
	if (false == m_pNewChatLogWindow->Create(m_pNewUIMng, pos_left(), (int)pos_botton() + 373, 6))
		return false;
#elif MAIN_UPDATE > 303
	//
	if (gmProtect->checkold_school())
	{
		if (false == m_pNewChatLogWindow->Create(m_pNewUIMng, pos_left(), (int)pos_botton() + 410, 6))
			return false;
	}
	else
	{
		if (false == m_pNewChatLogWindow->Create(m_pNewUIMng, pos_left(), (int)pos_botton() + 383, 6))
			return false;
	}
#else
	if (false == m_pNewChatLogWindow->Create(m_pNewUIMng, pos_left(), (int)GetWindowsY - 50, 6))
		return false;
#endif

	m_pNewOptionWindow = new CNewUIOptionWindow;
	if (m_pNewOptionWindow->Create(m_pNewUIMng, AddMiddleX(190), 70) == false)
	{
		return false;
	}

	m_pNewSlideWindow = new CNewUISlideWindow;
	if (m_pNewSlideWindow->Create(m_pNewUIMng) == false)
	{
		return false;
	}

	if (g_MessageBox->Create(m_pNewUIMng) == false)
		return false;

	return true;
}

void SEASON3B::CNewUISystem::Release()
{
	UnloadMainSceneInterface();

	SAFE_DELETE(m_pNewSlideWindow);
	SAFE_DELETE(m_pNewOptionWindow);
	SAFE_DELETE(m_pNewChatLogWindow);
	SAFE_DELETE(m_pNewUI3DRenderMng);

	m_pNewUIMng->RemoveAllUIObjs();

	SAFE_DELETE(m_pNewUIMng);
}

bool SEASON3B::CNewUISystem::LoadMainSceneInterface()
{
	g_MessageBox->Show(true);
	m_pNewChatLogWindow->Show(true);
	m_pNewSlideWindow->Show(true);

	float WinOpen0 = pos_right() + 640.f;
	float WinOpen1 = (WinOpen0 - WIN_WINDOW_SIZEX);
	float WinOpen2 = (WinOpen1 - WIN_WINDOW_SIZEX);
	float WinOpen3 = (WinOpen2 - WIN_WINDOW_SIZEX);

	m_pNewItemMng = new CNewUIItemMng;

	m_pNewChatInputBox = new CNewUIChatInputBox;
#if MAIN_UPDATE > 603
	if (false == m_pNewChatInputBox->Create(m_pNewUIMng, m_pNewChatLogWindow, pos_left(), (int)pos_botton() + 372))
		return false;
#else
	if (false == m_pNewChatInputBox->Create(m_pNewUIMng, m_pNewChatLogWindow, pos_left(), (int)pos_botton() + 382))
		return false;
#endif
	SetFocus(gwinhandle->GethWnd());

	m_pNewUIHotKey = new CNewUIHotKey;
	if (false == m_pNewUIHotKey->Create(m_pNewUIMng))
		return false;

	m_pNewMainFrameWindow = new CNewUIMainFrameWindow;
	if (m_pNewMainFrameWindow->Create(m_pNewUIMng, m_pNewUI3DRenderMng) == false)
		return false;

	m_pNewSkillList = new CNewUISkillList;
	if (m_pNewSkillList->Create(m_pNewUIMng, m_pNewUI3DRenderMng) == false)
		return false;

	m_pNewFriendWindow = new CNewUIFriendWindow;
	if (m_pNewFriendWindow->Create(m_pNewUIMng) == false)
		return false;

	m_pNewMyInventory = new CNewUIMyInventory;
	if (false == m_pNewMyInventory->Create(m_pNewUIMng, m_pNewUI3DRenderMng, WinOpen1, 0))
		return false;

	m_pNewNPCShop = new CNewUINPCShop;
	if (false == m_pNewNPCShop->Create(m_pNewUIMng, WinOpen2, 0))
		return false;

	m_pNewPetInfoWindow = new CNewUIPetInfoWindow;
	if (false == m_pNewPetInfoWindow->Create(m_pNewUIMng, WinOpen2, 0))
		return false;

	m_pNewMixInventory = new CNewUIMixInventory;
	if (m_pNewMixInventory->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewCastleWindow = new CNewUICastleWindow;
	if (m_pNewCastleWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewGuardWindow = new CNewUIGuardWindow;
	if (m_pNewGuardWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewGatemanWindow = new CNewUIGatemanWindow;
	if (m_pNewGatemanWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewGateSwitchWindow = new CNewUIGateSwitchWindow;
	if (m_pNewGateSwitchWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewStorageInventory = new CNewUIStorageInventory;
	if (m_pNewStorageInventory->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewGuildInfoWindow = new CNewUIGuildInfoWindow;
	if (m_pNewGuildInfoWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewGuildMakeWindow = new CNewUIGuildMakeWindow;
	if (m_pNewGuildMakeWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	CreatePersonalItemTable();

	m_pNewMyShopInventory = new CNewUIMyShopInventory;
	if (m_pNewMyShopInventory->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewPurchaseShopInventory = new CNewUIPurchaseShopInventory;
	if (m_pNewPurchaseShopInventory->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewCharacterInfoWindow = new CNewUICharacterInfoWindow;
	if (m_pNewCharacterInfoWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewMyQuestInfoWindow = new CNewUIMyQuestInfoWindow;
	if (m_pNewMyQuestInfoWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewPartyInfoWindow = new CNewUIPartyInfoWindow;
	if (m_pNewPartyInfoWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewPartyListWindow = new CNewUIPartyListWindow;

#if MAIN_UPDATE > 303
	if (!gmProtect->checkold_school())
	{
		if (m_pNewPartyListWindow->Create(m_pNewUIMng, WinOpen0 - (79 + 128), 18) == false)
			return false;
	}
	else
	{
		if (m_pNewPartyListWindow->Create(m_pNewUIMng, WinOpen0 - (56 + 128), 18) == false)
			return false;
	}
#else
	if (m_pNewPartyListWindow->Create(m_pNewUIMng, WinOpen0 - (54 + 128), 5) == false)
		return false;
#endif

	m_pNewNPCQuest = new CNewUINPCQuest;
	if (m_pNewNPCQuest->Create(m_pNewUIMng, m_pNewUI3DRenderMng, WinOpen1, 0) == false)
		return false;

	m_pNewEnterBloodCastle = new CNewUIEnterBloodCastle;
	if (m_pNewEnterBloodCastle->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewEnterDevilSquare = new CNewUIDevilSquareEnter;
	if (m_pNewEnterDevilSquare->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewBloodCastle = new CNewUIBloodCastle;
	if (m_pNewBloodCastle->Create(m_pNewUIMng, WinOpen0 - 127, (int)GetWindowsY - 132) == false)
		return false;

	m_pNewTrade = new CNewUITrade;
	if (m_pNewTrade->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewKanturu2ndEnterNpc = new CNewUIKanturu2ndEnterNpc;
	if (m_pNewKanturu2ndEnterNpc->Create(m_pNewUIMng, AddMiddleX(230), (AddMiddleY() + 20) - 51) == false)
	{
		return false;
	}

	m_pNewKanturuInfoWindow = new CNewUIKanturuInfoWindow;
	if (m_pNewKanturuInfoWindow->Create(m_pNewUIMng, WinOpen0 - 99, (int)GetWindowsY - 129) == false)
	{
		return false;
	}

	m_pNewChaosCastleTime = new CNewUIChaosCastleTime;
	if (m_pNewChaosCastleTime->Create(m_pNewUIMng, WinOpen0 - 127, (int)GetWindowsY - 132) == false)
		return false;

	m_pNewBattleSoccerScore = new CNewUIBattleSoccerScore;
	if (m_pNewBattleSoccerScore->Create(m_pNewUIMng, WinOpen0 - 131, (int)GetWindowsY - 121) == false)
		return false;

	m_pNewCommandWindow = new CNewUICommandWindow;
	if (m_pNewCommandWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewCatapultWindow = new CNewUICatapultWindow;
	if (m_pNewCatapultWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
	{
		return false;
	}

	m_pNewWindowMenu = new CNewUIWindowMenu;
	if (m_pNewWindowMenu->Create(m_pNewUIMng, WinOpen0 - 112, (int)GetWindowsY - 51) == false)
	{
		return false;
	}

	m_pNewHeroPositionInfo = new CNewUIHeroPositionInfo;
	if (m_pNewHeroPositionInfo->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewHelpWindow = new CNewUIHelpWindow;
	if (m_pNewHelpWindow->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewItemExplanationWindow = new CNewUIItemExplanationWindow;
	if (m_pNewItemExplanationWindow->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewSetItemExplanation = new CNewUISetItemExplanation;
	if (m_pNewSetItemExplanation->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewQuickCommandWindow = new CNewUIQuickCommandWindow;
	if (m_pNewQuickCommandWindow->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewMoveCommandWindow = new CNewUIMoveCommandWindow;

	if (m_pNewMoveCommandWindow->Create(m_pNewUIMng, 1, 1) == false)
		return false;

	m_pNewDuelWindow = new CNewUIDuelWindow;
	if (m_pNewDuelWindow->Create(m_pNewUIMng, WinOpen0 - 131, (int)GetWindowsY - 121) == false)
	{
		return false;
	}

	m_pNewNameWindow = new CNewUINameWindow;
	if (m_pNewNameWindow->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewSiegeWarfare = new CNewUISiegeWarfare;
	if (m_pNewSiegeWarfare->Create(m_pNewUIMng, WinOpen0 - 154, (int)GetWindowsY - 246) == false)
		return false;

#if MAIN_UPDATE > 303
	m_pNewItemEnduranceInfo = new CNewUIItemEnduranceInfo;
	if (m_pNewItemEnduranceInfo->Create(m_pNewUIMng, 2, 26) == false)
	{
		return false;
	}
#else
	m_pNewItemEnduranceInfo = new CNewUIItemEnduranceInfo;
	if (m_pNewItemEnduranceInfo->Create(m_pNewUIMng, 2, 68) == false)
	{
		return false;
	}
#endif

	m_pNewBuffWindow = new CNewUIBuffWindow;
	if (m_pNewBuffWindow->Create(m_pNewUIMng, 220, 25) == false)
	{
		return false;
	}

	m_pNewCursedTempleEnterWindow = new CNewUICursedTempleEnter;
	if (m_pNewCursedTempleEnterWindow->Create(m_pNewUIMng, AddMiddleX(230), (AddMiddleY() + 80) - 51) == false)
	{
		return false;
	}

	m_pNewCursedTempleWindow = new CNewUICursedTempleSystem;
	if (m_pNewCursedTempleWindow->Create(m_pNewUIMng, 0, 0) == false)
	{
		return false;
	}

	m_pNewCursedTempleResultWindow = new CNewUICursedTempleResult;
	if (m_pNewCursedTempleResultWindow->Create(m_pNewUIMng, AddMiddleX(230), (AddMiddleY() + 120) - 51) == false)
	{
		return false;
	}

	m_pNewCryWolfInterface = new CNewUICryWolf;
	if (m_pNewCryWolfInterface->Create(m_pNewUIMng, 0, 0) == false)
		return false;

#if MAIN_UPDATE > 303
	m_pNewUIMasterSkillTree = new CNewUIMasterSkillTree;
	if (m_pNewUIMasterSkillTree->Create() == false)
		return false;
#endif // MAIN_UPDATE != 303

	m_pNewMiniMap = new CNewUIMiniMap;
	if (m_pNewMiniMap->Create(m_pNewUIMng, 0, 0) == false)
		return false;

	m_pNewGoldBowman = new CNewUIGoldBowmanWindow;
	if (m_pNewGoldBowman->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewGoldBowmanLena = new CNewUIGoldBowmanLena;
	if (m_pNewGoldBowmanLena->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewLuckyCoinRegistration = new CNewUIRegistrationLuckyCoin;
	if (m_pNewLuckyCoinRegistration->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewExchangeLuckyCoinWindow = new CNewUIExchangeLuckyCoin;
	if (m_pNewExchangeLuckyCoinWindow->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewDuelWatchWindow = new CNewUIDuelWatchWindow;
	if (m_pNewDuelWatchWindow->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewDuelWatchMainFrameWindow = new CNewUIDuelWatchMainFrameWindow;
	if (m_pNewDuelWatchMainFrameWindow->Create(m_pNewUIMng, m_pNewUI3DRenderMng) == false)
		return false;

	m_pNewDuelWatchUserListWindow = new CNewUIDuelWatchUserListWindow;
	if (m_pNewDuelWatchUserListWindow->Create(m_pNewUIMng, WinOpen0 - 57, (int)GetWindowsY - 51) == false)
		return false;

#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
	m_pNewInGameShop = new CNewUIInGameShop;
	if (m_pNewInGameShop->Create(m_pNewUIMng, PositionX_The_Mid(640), PositionY_In_The_Mid(0)) == false)
		return false;
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME

	m_pNewDoppelGangerWindow = new CNewUIDoppelGangerWindow;
	if (m_pNewDoppelGangerWindow->Create(m_pNewUIMng, m_pNewUI3DRenderMng, WinOpen1, 0) == false)
		return false;

	m_pNewDoppelGangerFrame = new CNewUIDoppelGangerFrame;
	if (m_pNewDoppelGangerFrame->Create(m_pNewUIMng, WinOpen0 - 227, (int)GetWindowsY - 138) == false)
		return false;

	m_pNewNPCDialogue = new CNewUINPCDialogue;
	if (m_pNewNPCDialogue->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewQuestProgress = new CNewUIQuestProgress;
	if (m_pNewQuestProgress->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewQuestProgressByEtc = new CNewUIQuestProgressByEtc;
	if (m_pNewQuestProgressByEtc->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewEmpireGuardianNPC = new CNewUIEmpireGuardianNPC;
	if (m_pNewEmpireGuardianNPC->Create(m_pNewUIMng, m_pNewUI3DRenderMng, WinOpen1, 0) == false)
		return false;

	m_pNewEmpireGuardianTimer = new CNewUIEmpireGuardianTimer;
	if (m_pNewEmpireGuardianTimer->Create(m_pNewUIMng, WinOpen0 - 133, (int)GetWindowsY - 138) == false)
		return false;

#ifdef PBG_MOD_STAMINA_UI
	m_pNewUIStamina = new CNewUIStamina;
	if (m_pNewUIStamina->Create(m_pNewUIMng, WinOpen0, GetWindowsY) == false)
		return false;
#endif //PBG_MOD_STAMINA_UI

	m_pNewGensRanking = new CNewUIGensRanking;
	if (m_pNewGensRanking->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewUnitedMarketPlaceWindow = new CNewUIUnitedMarketPlaceWindow;
	if (m_pNewUnitedMarketPlaceWindow->Create(m_pNewUIMng, m_pNewUI3DRenderMng, WinOpen1, 0) == false)
		return false;

	m_pNewUILuckyItemWnd = new CNewUILuckyItemWnd;
	if (m_pNewUILuckyItemWnd->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewUIInvenExpansion = new CNewUIInvenExpansion;
	if (m_pNewUIInvenExpansion->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewUIStorageExpansion = new CNewUIStorageExpansion;
	if (m_pNewUIStorageExpansion->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

#if MAIN_UPDATE > 303
	m_pNewUIMacroGaugeBar = new CNewUIMacroGaugeBar;
	if (!gmProtect->checkold_school())
	{
		if (m_pNewUIMacroGaugeBar->Create(m_pNewUIMng, 0, 14) == false)
			return false;
	}
	else
	{
		if (m_pNewUIMacroGaugeBar->Create(m_pNewUIMng, 0, 2) == false)
			return false;
	}
#else
	m_pNewUIMacroGaugeBar = new CNewUIMacroGaugeBar;
	if (m_pNewUIMacroGaugeBar->Create(m_pNewUIMng, 0, 2) == false)
		return false;
#endif

	m_pNewUIMacroMain = new CNewUIMacroMain;
	if (m_pNewUIMacroMain->Create(m_pNewUIMng, WinOpen1, 0) == false)
		return false;

	m_pNewUIMacroSub = new CNewUIMacroSub;
	if (m_pNewUIMacroSub->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewUIInventoryJewel = new CNewUIInventoryJewel;

	if (m_pNewUIInventoryJewel->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewUIInventoryCore = new CNewUIInventoryCore;

	if (m_pNewUIInventoryCore->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

	m_pNewUIMixExpansion = new CNewUIMixExpansion;

	if (m_pNewUIMixExpansion->Create(m_pNewUIMng, WinOpen2, 0) == false)
		return false;

#ifdef EFFECT_MNG_HANDLE
	m_gfxEffectHandle = new CGFxEffectHandle;

	if (m_gfxEffectHandle->Create(m_pNewUIMng, PositionX_The_Mid(459), PositionY_The_Mid(264)) == false)
		return false;
#endif // EFFECT_MNG_HANDLE

#ifdef PACK_FILE_DECRYPT_H
	m_pNewUIItemTooltip = new CNewUIItemTooltip;
	if (m_pNewUIItemTooltip == NULL)
		return false;
#endif // PACK_FILE_DECRYPT_H

	m_pScriptItem = new CScriptItem;
	if (m_pScriptItem == NULL)
		return false;

	m_iCamWebzen = new CGMCameraWorld;
	if (m_iCamWebzen == NULL)
		return false;

	m_NewUIReconnect = new CNewUIReconnect;

	if (m_NewUIReconnect->Create(m_pNewUIMng, PositionX_The_Mid(352), PositionY_The_Mid(113)) == false)
	{
		return false;
	}

	m_NewUIRankTop = new CNewUIRankingTop;

	if (m_NewUIRankTop->Create(m_pNewUIMng, PositionX_The_Mid(366), 100) == false)
	{
		return false;
	}

	m_NewUICommandUser = new CNewUICommandFilter;

	if (m_NewUICommandUser->Create(m_pNewUIMng, PositionX_The_Mid(214), 100) == false)
	{
		return false;
	}

	m_NewUIEventTimeList = new CNewUIEventTimeView;

	if (m_NewUIEventTimeList->Create(m_pNewUIMng, PositionX_The_Mid(214), 100) == false)
	{
		return false;
	}

	m_NewUIMenuUser = new CNewUIMenuUser;

	if (m_NewUIMenuUser->Create(m_pNewUIMng, PositionX_The_Mid(214), 100) == false)
	{
		return false;
	}

	m_NewUIMemberShip = new CNewUIPurcharseVip;

	if (m_NewUIMemberShip->Create(m_pNewUIMng, PositionX_The_Mid(214), 100) == false)
	{
		return false;
	}

	m_iCamWebzen->Init();

	return true;
}

void SEASON3B::CNewUISystem::UnloadMainSceneInterface()
{
	if (g_pNewUIMng)
	{
		g_pNewUIMng->ShowAllInterfaces(false);
	}
	SAFE_DELETE(m_pNewMiniMap);

	SAFE_DELETE(m_pNewChatInputBox);
	SAFE_DELETE(m_pNewFriendWindow);
	SAFE_DELETE(m_pNewMainFrameWindow);
	SAFE_DELETE(m_pNewSkillList);
	SAFE_DELETE(m_pNewMyInventory);
	SAFE_DELETE(m_pNewNPCShop);
	SAFE_DELETE(m_pNewPetInfoWindow);
	SAFE_DELETE(m_pNewMixInventory);
	SAFE_DELETE(m_pNewCastleWindow);
	SAFE_DELETE(m_pNewGuardWindow);
	SAFE_DELETE(m_pNewGatemanWindow);
	SAFE_DELETE(m_pNewGateSwitchWindow);
	SAFE_DELETE(m_pNewStorageInventory);
	SAFE_DELETE(m_pNewGuildMakeWindow);
	SAFE_DELETE(m_pNewGuildInfoWindow);
	SAFE_DELETE(m_pNewMyShopInventory);
	SAFE_DELETE(m_pNewPurchaseShopInventory);
	SAFE_DELETE(m_pNewCharacterInfoWindow);
	SAFE_DELETE(m_pNewMyQuestInfoWindow);
	SAFE_DELETE(m_pNewPartyInfoWindow);
	SAFE_DELETE(m_pNewPartyListWindow);
	SAFE_DELETE(m_pNewNPCQuest);
	SAFE_DELETE(m_pNewEnterBloodCastle);
	SAFE_DELETE(m_pNewEnterDevilSquare);
	SAFE_DELETE(m_pNewBloodCastle);
	SAFE_DELETE(m_pNewTrade);
	SAFE_DELETE(m_pNewKanturu2ndEnterNpc);
	SAFE_DELETE(m_pNewKanturuInfoWindow);
	SAFE_DELETE(m_pNewCatapultWindow);
	SAFE_DELETE(m_pNewChaosCastleTime);
	SAFE_DELETE(m_pNewBattleSoccerScore);
	SAFE_DELETE(m_pNewCommandWindow);
	SAFE_DELETE(m_pNewHeroPositionInfo);
	SAFE_DELETE(m_pNewWindowMenu);
	SAFE_DELETE(m_pNewHelpWindow);
	SAFE_DELETE(m_pNewItemExplanationWindow);
	SAFE_DELETE(m_pNewSetItemExplanation);
	SAFE_DELETE(m_pNewQuickCommandWindow);
	SAFE_DELETE(m_pNewMoveCommandWindow);
	SAFE_DELETE(m_pNewDuelWindow);
	SAFE_DELETE(m_pNewNameWindow);
	SAFE_DELETE(m_pNewSiegeWarfare);
	SAFE_DELETE(m_pNewItemEnduranceInfo);
	SAFE_DELETE(m_pNewBuffWindow);
	SAFE_DELETE(m_pNewCursedTempleEnterWindow);
	SAFE_DELETE(m_pNewCursedTempleWindow);
	SAFE_DELETE(m_pNewCursedTempleResultWindow);
	SAFE_DELETE(m_pNewCryWolfInterface);
#if MAIN_UPDATE > 303
	SAFE_DELETE(m_pNewUIMasterSkillTree);
#endif // MAIN_UPDATE != 303
	SAFE_DELETE(m_pNewGoldBowman);
	SAFE_DELETE(m_pNewGoldBowmanLena);
	SAFE_DELETE(m_pNewLuckyCoinRegistration);
	SAFE_DELETE(m_pNewExchangeLuckyCoinWindow);
	SAFE_DELETE(m_pNewDuelWatchWindow);
	SAFE_DELETE(m_pNewDuelWatchMainFrameWindow);
	SAFE_DELETE(m_pNewDuelWatchUserListWindow);
#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
	SAFE_DELETE(m_pNewInGameShop);
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME
	SAFE_DELETE(m_pNewDoppelGangerWindow);
	SAFE_DELETE(m_pNewDoppelGangerFrame);
	SAFE_DELETE(m_pNewNPCDialogue);
	SAFE_DELETE(m_pNewQuestProgress);
	SAFE_DELETE(m_pNewQuestProgressByEtc);
	SAFE_DELETE(m_pNewEmpireGuardianNPC);
	SAFE_DELETE(m_pNewEmpireGuardianTimer);
	SAFE_DELETE(m_pNewGensRanking);
	SAFE_DELETE(m_pNewUnitedMarketPlaceWindow);
	SAFE_DELETE(m_pNewUILuckyItemWnd);

	SAFE_DELETE(m_pNewUIInvenExpansion);
	SAFE_DELETE(m_pNewUIStorageExpansion);

	SAFE_DELETE(m_pNewUIMacroMain);
	SAFE_DELETE(m_pNewUIMacroSub);
	SAFE_DELETE(m_pNewUIMacroGaugeBar);
	SAFE_DELETE(m_pNewUIInventoryJewel);
#ifdef EFFECT_MNG_HANDLE
	SAFE_DELETE(m_gfxEffectHandle);
#endif // EFFECT_MNG_HANDLE

#ifdef PACK_FILE_DECRYPT_H
	SAFE_DELETE(m_pNewUIItemTooltip);
#endif // PACK_FILE_DECRYPT_H
	SAFE_DELETE(m_pScriptItem);
	SAFE_DELETE(m_iCamWebzen);
	SAFE_DELETE(m_NewUIReconnect);
	SAFE_DELETE(m_NewUIRankTop);

	SAFE_DELETE(m_NewUICommandUser);
	SAFE_DELETE(m_NewUIEventTimeList);

	SAFE_DELETE(m_NewUIMenuUser);
	SAFE_DELETE(m_NewUIMemberShip);

	SAFE_DELETE(m_pNewUIInventoryCore);

	SAFE_DELETE(m_pNewUIMixExpansion);

	SAFE_DELETE(m_pNewItemMng);

	ReleasePersonalItemTable();
}

bool SEASON3B::CNewUISystem::IsVisible(DWORD dwKey)
{
	if (m_pNewUIMng)
	{
		return m_pNewUIMng->IsInterfaceVisible(dwKey);
	}

	return false;
}

void SEASON3B::CNewUISystem::Show(DWORD dwKey)
{
#ifdef PBG_ADD_INGAMESHOP_UI_ITEMSHOP
	if (g_pInGameShop->IsInGameShop())
		return;
#endif //PBG_ADD_INGAMESHOP_UI_ITEMSHOP

	float WinOpen0 = pos_right(640.f);
	float WinOpen1 = (WinOpen0 - WIN_WINDOW_SIZEX);
	float WinOpen2 = (WinOpen1 - WIN_WINDOW_SIZEX);
	float WinOpen3 = (WinOpen2 - WIN_WINDOW_SIZEX);

	if (m_pNewUIMng)
	{
		switch (dwKey)
		{
		case SEASON3B::INTERFACE_FRIEND:
			g_pMainFrame->SetBtnState(SEASON3B::MAINFRAME_BTN_FRIEND, true);
			m_pNewFriendWindow->OpenMainWnd(WinOpen0 - 250, Position_In_The_Down(221));
			break;
		case SEASON3B::INTERFACE_PET:
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
				Hide(SEASON3B::INTERFACE_INVENTORY);
			if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
				Hide(INTERFACE_MYQUEST);
			HideGroupBeforeOpenInterface();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_CHARACTER);
			g_pMainFrame->SetBtnState(SEASON3B::MAINFRAME_BTN_CHAINFO, true);
			m_pNewPetInfoWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_MYQUEST:
			HideGroupBeforeOpenInterface();

			if (IsVisible(SEASON3B::INTERFACE_CHARACTER))
			{
				g_pMyQuestInfoWindow->SetPos(WinOpen2, 0);
			}
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
			{
				Hide(SEASON3B::INTERFACE_INVENTORY);
			}
			if (IsVisible(SEASON3B::INTERFACE_PET))
			{
				Hide(SEASON3B::INTERFACE_PET);
			}
			g_pMyQuestInfoWindow->OpenningProcess();
			if (GMProtect->LookAndFeelType() == 3 || GMProtect->LookAndFeelType() == 4)
			{
				g_pMainFrame->SetBtnState(MAINFRAME_BTN_GUILD, true);
			}
			break;
		case SEASON3B::INTERFACE_INVENTORY:
			HideGroupBeforeOpenInterface();

			g_pMainFrame->SetBtnState(SEASON3B::MAINFRAME_BTN_MYINVEN, true);

			if (IsVisible(SEASON3B::INTERFACE_CHARACTER))
				g_pMyInventory->SetPos(WinOpen2, 0);

			if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
				Hide(SEASON3B::INTERFACE_MYQUEST);

			g_pMyInventory->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_CHARACTER:
			HideGroupBeforeOpenInterface();

			g_pMainFrame->SetBtnState(MAINFRAME_BTN_CHAINFO, true);

			if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
			{
				g_pMyInventory->SetPos(WinOpen2, 0);

				if (IsVisible(SEASON3B::INTERFACE_INVENTORY_JEWEL))
				{
					m_pNewUIInventoryJewel->ClosingProcess();
					m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_JEWEL, false);
				}

				if (IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE))
				{
					m_pNewUIInventoryCore->ClosingProcess();
					m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_CORE, false);
				}
			}
			else if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
			{
				g_pMyQuestInfoWindow->SetPos(WinOpen2, 0);
			}
			g_pCharacterInfoWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_MIXINVENTORY:
			HideAllGroupA();
			g_pMixInventory->OpeningProcess();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_MIX_EXPANSION:
			HideAllGroupA();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_NPCSHOP:
			HideAllGroupA();
			g_pNPCShop->OpenningProcess();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_STORAGE:
			HideAllGroupA();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_MYSHOP_INVENTORY:
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				Hide(SEASON3B::INTERFACE_CHARACTER);
				m_pNewUIInvenExpansion->SetThirdPosition(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
			}
			else
			{
				HideAllGroupA();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			}
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);

			if (gMapManager->IsCursedTemple() == true)
			{
				g_pMyShopInventory->OpenButtonLock();
			}
			else
			{
				g_pMyShopInventory->OpenButtonUnLock();
			}
			break;
		case SEASON3B::INTERFACE_INVENTORY_JEWEL:
			m_pNewUIInventoryJewel->OpenningProcess();
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				Hide(SEASON3B::INTERFACE_CHARACTER);
				m_pNewUIInvenExpansion->SetThirdPosition(SEASON3B::INTERFACE_INVENTORY_JEWEL);
			}
			else
			{
				HideAllGroupA();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			}
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);

			if (gMapManager->IsCursedTemple() == true)
			{
				g_pMyShopInventory->OpenButtonLock();
			}
			else
			{
				g_pMyShopInventory->OpenButtonUnLock();
			}
			break;
		case SEASON3B::INTERFACE_INVENTORY_CORE:
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				Hide(SEASON3B::INTERFACE_CHARACTER);
				m_pNewUIInvenExpansion->SetThirdPosition(SEASON3B::INTERFACE_INVENTORY_CORE);
			}
			else
			{
				HideAllGroupA();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
				m_pNewUIInventoryCore->OpeningProcess();
			}
			break;
		case SEASON3B::INTERFACE_GUILDINFO:
			HideAllGroupA();
			g_pGuildInfoWindow->OpenningProcess();
			if (GMProtect->LookAndFeelType() == 1 || GMProtect->LookAndFeelType() == 2)
			{
				g_pMainFrame->SetBtnState(MAINFRAME_BTN_GUILD, true);
			}
			break;
		case SEASON3B::INTERFACE_TRADE:
			HideAllGroupA();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_INVENTORY_EXTENSION:
			if (!m_pNewUIInvenExpansion->SetThirdPosition(SEASON3B::INTERFACE_BEGIN))
			{
				HideAllGroupA();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
				m_pNewUIInvenExpansion->SetPos(WinOpen2, 0);
			}
			if ( IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION) )
			{
				g_pStorageExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_STORAGE_EXTENSION, false);
			}
			m_pNewUIInvenExpansion->OpeningProcess();
			break;
		case SEASON3B::INTERFACE_STORAGE_EXTENSION:
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				m_pNewUIInvenExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
			}
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, true);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_STORAGE, true);
			g_pStorageExpansion->SetPos(WinOpen3, 0);
			g_pStorageExpansion->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_BLOODCASTLE:
			HideAllGroupA();
			g_pEnterBloodCastle->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY:
			HideAllGroupA();
			break;
		case SEASON3B::INTERFACE_PARTY:
			HideAllGroupA();
			m_pNewPartyInfoWindow->OpenningProcess();
			if (GMProtect->LookAndFeelType() == 1 || GMProtect->LookAndFeelType() == 2)
			{
				g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTY, true);
			}
			break;
		case SEASON3B::INTERFACE_NPCQUEST:
			HideAllGroupA();
			g_pNPCQuest->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_DEVILSQUARE:
			HideAllGroupA();
			g_pEnterDevilSquare->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_CATAPULT:
			HideAllGroupA();
			g_pCatapultWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_COMMAND:
			HideAllGroupA();
			m_pNewCommandWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_WINDOW_MENU:
			g_pWindowMenu->OpenningProcess();
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_WINDOW, true);
			break;
		case SEASON3B::INTERFACE_SENATUS:
			HideAllGroupA();
			g_pCastleWindow->OpeningProcess();
			break;
		case SEASON3B::INTERFACE_GUARDSMAN:
			HideAllGroupA();
			g_pGuardWindow->OpeningProcess();
			break;
		case SEASON3B::INTERFACE_GATEKEEPER:
			HideAllGroupA();
			g_pGatemanWindow->OpeningProcess();
			break;
		case SEASON3B::INTERFACE_NPCGUILDMASTER:
			HideAllGroupA();
			break;
		case SEASON3B::INTERFACE_MASTER_LEVEL:
			HideAllGroupA();
			break;
		case SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC:
			HideAllGroupB();
			break;
		case SEASON3B::INTERFACE_HERO_POSITION_INFO:
			m_pNewHeroPositionInfo->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_CHAOSCASTLE_TIME:
			if (IsVisible(SEASON3B::INTERFACE_CHATINPUTBOX))
			{
				Hide(SEASON3B::INTERFACE_CHATINPUTBOX);
			}
			m_pNewChaosCastleTime->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_BLOODCASTLE_TIME:
			g_pBloodCastle->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_OPTION:
			g_pOption->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_HELP:
			Hide(SEASON3B::INTERFACE_MOVEMAP);
			Hide(SEASON3B::INTERFACE_ITEM_EXPLANATION);
			Hide(SEASON3B::INTERFACE_SETITEM_EXPLANATION);
			g_pHelp->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_ITEM_EXPLANATION:
			Hide(SEASON3B::INTERFACE_MOVEMAP);
			Hide(SEASON3B::INTERFACE_HELP);
			Hide(SEASON3B::INTERFACE_SETITEM_EXPLANATION);
			g_pItemExplanation->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_SETITEM_EXPLANATION:
			Hide(SEASON3B::INTERFACE_MOVEMAP);
			Hide(SEASON3B::INTERFACE_HELP);
			Hide(SEASON3B::INTERFACE_ITEM_EXPLANATION);
			g_pSetItemExplanation->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_QUICK_COMMAND:
			g_pQuickCommand->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_MOVEMAP:
			Hide(SEASON3B::INTERFACE_HELP);
			Hide(SEASON3B::INTERFACE_ITEM_EXPLANATION);
			Hide(SEASON3B::INTERFACE_SETITEM_EXPLANATION);
			m_pNewMoveCommandWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_CHATINPUTBOX:
			m_pNewChatInputBox->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_SIEGEWARFARE:
			m_pNewSiegeWarfare->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_ITEM_ENDURANCE_INFO:
			m_pNewItemEnduranceInfo->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_BUFF_WINDOW:
			m_pNewBuffWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_CRYWOLF:
			m_pNewCryWolfInterface->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_GOLD_BOWMAN:
			m_pNewGoldBowman->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_GOLD_BOWMAN_LENA:
			m_pNewGoldBowmanLena->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION:
			HideAllGroupA();
			g_pLuckyCoinRegistration->OpenningProcess();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, true);
			break;
		case SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN:
			HideAllGroupA();
			g_pExchangeLuckyCoinWindow->OpenningProcess();
			break;

		case SEASON3B::INTERFACE_DUELWATCH:
			m_pNewDuelWatchWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_DUELWATCH_MAINFRAME:
			m_pNewDuelWatchMainFrameWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_DUELWATCH_USERLIST:
			m_pNewDuelWatchUserListWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_DOPPELGANGER_NPC:
			m_pNewDoppelGangerWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_DOPPELGANGER_FRAME:
			m_pNewDoppelGangerFrame->OpenningProcess();
			break;

		case SEASON3B::INTERFACE_NPC_DIALOGUE:
			HideAllGroupA();
			g_pNPCDialogue->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_QUEST_PROGRESS:
			HideAllGroupA();
			g_pQuestProgress->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_QUEST_PROGRESS_ETC:
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
				Hide(SEASON3B::INTERFACE_INVENTORY);
			if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
			{
				Hide(SEASON3B::INTERFACE_MYQUEST);
				g_pQuestProgressByEtc->SetPos(WinOpen1, 0);
			}
			if (IsVisible(SEASON3B::INTERFACE_CHARACTER))
				g_pQuestProgressByEtc->SetPos(WinOpen2, 0);
			g_pQuestProgressByEtc->ProcessOpening();
			break;
		case SEASON3B::INTERFACE_INGAMESHOP:
#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
			g_ConsoleDebug->Write(MCD_NORMAL, "InGameShopStatue.Txt CallStack - CNewUISystem.Show()\r\n");
			HideAll();
			g_pInGameShop->OpeningProcess();
#ifndef KJH_MOD_SHOP_SCRIPT_DOWNLOAD
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTCHARGE, true);
#endif // KJH_MOD_SHOP_SCRIPT_DOWNLOAD
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME
			break;
		case SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC:
			m_pNewEmpireGuardianNPC->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER:
			m_pNewEmpireGuardianTimer->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_MINI_MAP:
			m_pNewMiniMap->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_GENSRANKING:
			HideAllGroupA();
			g_pNewUIGensRanking->OpenningProcess();
			g_pNewUIGensRanking->SetPos(WinOpen1, 0);
			break;
		case SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA:
			m_pNewUnitedMarketPlaceWindow->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_LUCKYITEMWND:
			HideAllGroupA();
			m_pNewUILuckyItemWnd->OpeningProcess();
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY);
			break;
		case SEASON3B::INTERFACE_MACRO_OFICIAL:
			if (g_pMixInventory->ClosingProcess() == false)
				return;
			this->HideAllGroupA();
			m_pNewUIMacroMain->OpenningProcess();
			break;
		case SEASON3B::INTERFACE_RANKING_TOP:
			m_NewUIRankTop->OpenningProcess();
			HideGroupOpenInterface(INTERFACE_RANKING_TOP);
			break;
		case SEASON3B::INTERFACE_COMMAND_LIST:
			m_NewUICommandUser->OpenningProcess();
			HideGroupOpenInterface(INTERFACE_COMMAND_LIST);
			break;
		case SEASON3B::INTERFACE_EVENT_TIME:
			m_NewUIEventTimeList->OpenningProcess();
			HideGroupOpenInterface(INTERFACE_EVENT_TIME);
			break;
		case SEASON3B::INTERFACE_CUSTOM_MENU:
			m_NewUIMenuUser->OpenningProcess();
			HideGroupOpenInterface(INTERFACE_CUSTOM_MENU);
			break;
		case SEASON3B::INTERFACE_SHOW_VIP:
			m_NewUIMemberShip->OpenningProcess();
			HideGroupOpenInterface(INTERFACE_SHOW_VIP);
			break;
		}

		m_pNewUIMng->ShowInterface(dwKey);

		int iScreenWidth = GetScreenWidth();
		m_pNewItemEnduranceInfo->SetPos(iScreenWidth);
		m_pNewBuffWindow->SetPos(iScreenWidth);

#if MAIN_UPDATE > 303
		if (m_pNewPartyListWindow->IsEnableMove())
		{
			if (!m_pNewItemEnduranceInfo->IsHideInterfaceWorld() && iScreenWidth == ((int)GetWindowsX))
				m_pNewPartyListWindow->SetPos(iScreenWidth - 128);
			else
				m_pNewPartyListWindow->SetPos(iScreenWidth);
		}
#else
		if (m_pNewPartyListWindow->IsEnableMove())
		{
			m_pNewPartyListWindow->SetPos(iScreenWidth - 2.f);
		}
#endif // MAIN_UPDATE > 303

		

		if (m_pNewUIInvenExpansion->GetThirdInterface() || IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
		{
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_HERO_POSITION_INFO, false);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_PARTY_INFO_WINDOW, false);
		}
	}
}

void SEASON3B::CNewUISystem::Hide(DWORD dwKey)
{
	if (m_pNewUIMng)
	{
		float WinOpen0 = pos_right(640.f);
		float WinOpen1 = (WinOpen0 - WIN_WINDOW_SIZEX);
		float WinOpen2 = (WinOpen1 - WIN_WINDOW_SIZEX);
		float WinOpen3 = (WinOpen2 - WIN_WINDOW_SIZEX);

		switch (dwKey)
		{
		case SEASON3B::INTERFACE_FRIEND:
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_FRIEND, false);
			m_pNewFriendWindow->HideAllWindow(TRUE, TRUE);
			break;
		case SEASON3B::INTERFACE_CHARACTER:
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_CHAINFO, false);
			if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
			{
				g_pMyQuestInfoWindow->SetPos(WinOpen1, 0);
			}
			if (IsVisible((SEASON3B::INTERFACE_INVENTORY)))
			{
				g_pMyInventory->SetPos(WinOpen1, 0);
			}
			if (IsVisible((SEASON3B::INTERFACE_PET)))
			{
				Hide(SEASON3B::INTERFACE_PET);
			}
			if (IsVisible((SEASON3B::INTERFACE_QUEST_PROGRESS_ETC)))
			{
				g_pQuestProgressByEtc->SetPos(WinOpen1, 0);
			}
			break;
		case SEASON3B::INTERFACE_INVENTORY:
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);

			if (IsVisible(SEASON3B::INTERFACE_MIXINVENTORY))
			{
				if (m_pNewMixInventory->ClosingProcess() == false)
					return;
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_MIXINVENTORY, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_MIX_EXPANSION))
			{
				if (m_pNewUIMixExpansion->ClosingProcess() == false)
					return;
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_MIX_EXPANSION, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_STORAGE))
			{
				if (!g_pStorageInventory->ClosingProcess())
					return;
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_STORAGE, false);
				if (IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
				{
					g_pStorageExpansion->ClosingProcess();
					m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_STORAGE_EXTENSION, false);
				}
			}
			if (IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND))
			{
				if (g_pLuckyItemWnd->ClosingProcess() == false)
					return;
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_LUCKYITEMWND, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_NPCSHOP))
			{
				g_pNPCShop->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_NPCSHOP, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_MYSHOP_INVENTORY))
			{
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_MYSHOP_INVENTORY, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY))
			{
				g_pPurchaseShopInventory->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_TRADE))
			{
				g_pTrade->ProcessCloseBtn();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_TRADE, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION))
			{
				m_pNewLuckyCoinRegistration->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN))
			{
				m_pNewExchangeLuckyCoinWindow->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND))
			{
				m_pNewUILuckyItemWnd->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_LUCKYITEMWND, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_JEWEL))
			{
				m_pNewUIInventoryJewel->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_JEWEL, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE))
			{
				m_pNewUIInventoryCore->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_CORE, false);
			}
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				m_pNewUIInvenExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
			}
			g_pMyInventory->SetPos(WinOpen1, 0);
			g_pMyInventory->ClosingProcess();
			break;
		case SEASON3B::INTERFACE_MIXINVENTORY:
			if (m_pNewMixInventory->ClosingProcess() == false)
				return;
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				m_pNewUIInvenExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
			}
			break;
		case SEASON3B::INTERFACE_MIX_EXPANSION:
			if (m_pNewMixInventory->ClosingProcess() == false)
				return;

			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				m_pNewUIInvenExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
			}
			break;
		case SEASON3B::INTERFACE_NPCSHOP:
			g_pNPCShop->ClosingProcess();
			g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
			if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
			{
				m_pNewUIInvenExpansion->ClosingProcess();
				m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
			}
			break;
		default:
			if (dwKey != SEASON3B::INTERFACE_MYSHOP_INVENTORY && dwKey != SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY)
			{
				switch (dwKey)
				{
				case SEASON3B::INTERFACE_STORAGE:
					if (!g_pStorageInventory->ClosingProcess())
						return;
					g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);
					m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);

					if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
					{
						m_pNewUIInvenExpansion->ClosingProcess();
						m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
					}
					if (IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
					{
						g_pStorageExpansion->ClosingProcess();
						m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_STORAGE_EXTENSION, false);
					}
					break;
				case SEASON3B::INTERFACE_TRADE:
					g_pTrade->ClosingProcess();
					g_pMainFrame->SetBtnState(MAINFRAME_BTN_MYINVEN, false);
					m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);

					if (IsVisible(SEASON3B::INTERFACE_INVENTORY_EXTENSION))
					{
						m_pNewUIInvenExpansion->ClosingProcess();
						m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY_EXTENSION, false);
					}
					break;
				case SEASON3B::INTERFACE_BLOODCASTLE:
					g_pEnterBloodCastle->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_PET:
					m_pNewPetInfoWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_PARTY:
					m_pNewPartyInfoWindow->ClosingProcess();
					if (GMProtect->LookAndFeelType() == 1 || GMProtect->LookAndFeelType() == 2)
					{
						g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTY, false);
					}
					break;
				case SEASON3B::INTERFACE_MYQUEST:
					m_pNewMyQuestInfoWindow->ClosingProcess();
					m_pNewMyQuestInfoWindow->SetPos(WinOpen1, 0);
					if (GMProtect->LookAndFeelType() == 3 || GMProtect->LookAndFeelType() == 4)
					{
						g_pMainFrame->SetBtnState(MAINFRAME_BTN_GUILD, false);
					}
					break;
				case SEASON3B::INTERFACE_SENATUS:
					m_pNewCastleWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_GUARDSMAN:
					m_pNewGuardWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_GATEKEEPER:
					m_pNewGatemanWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_GATESWITCH:
					m_pNewGateSwitchWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_NPCQUEST:
					m_pNewNPCQuest->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_DEVILSQUARE:
					g_pEnterDevilSquare->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_BLOODCASTLE_TIME:
					g_pBloodCastle->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_CATAPULT:
					g_pCatapultWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_CHAOSCASTLE_TIME:
					m_pNewChaosCastleTime->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_COMMAND:
					m_pNewCommandWindow->ClosingProcess();
					break;
				case SEASON3B::INTERFACE_WINDOW_MENU:
					g_pMainFrame->SetBtnState(MAINFRAME_BTN_WINDOW, false);
					break;
				default:
					if (dwKey != SEASON3B::INTERFACE_OPTION)
					{
						switch (dwKey)
						{
						case SEASON3B::INTERFACE_HERO_POSITION_INFO:
							m_pNewHeroPositionInfo->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_HELP:
							g_pHelp->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_ITEM_EXPLANATION:
							g_pItemExplanation->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_SETITEM_EXPLANATION:
							g_pSetItemExplanation->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_QUICK_COMMAND:
							g_pQuickCommand->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_MOVEMAP:
							m_pNewCommandWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_CHATINPUTBOX:
							m_pNewChatInputBox->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_GUILDINFO:
							m_pNewGuildInfoWindow->ClosingProcess();
							if (GMProtect->LookAndFeelType() == 1 || GMProtect->LookAndFeelType() == 2)
							{
								g_pMainFrame->SetBtnState(MAINFRAME_BTN_GUILD, false);
							}
							break;
						case SEASON3B::INTERFACE_NPCGUILDMASTER:
							m_pNewGuildMakeWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_SIEGEWARFARE:
							m_pNewSiegeWarfare->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_ITEM_ENDURANCE_INFO:
							m_pNewItemEnduranceInfo->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_BUFF_WINDOW:
							m_pNewBuffWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_CURSEDTEMPLE_RESULT:
							m_pNewCursedTempleResultWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_CRYWOLF:
							m_pNewCryWolfInterface->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_GOLD_BOWMAN:
							m_pNewGoldBowman->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_GOLD_BOWMAN_LENA:
							m_pNewGoldBowmanLena->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION:
							m_pNewLuckyCoinRegistration->ClosingProcess();

							if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
							{
								m_pNewMyInventory->ClosingProcess();
								m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
							}
							break;
						case SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN:
							m_pNewExchangeLuckyCoinWindow->ClosingProcess();

							if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
							{
								m_pNewMyInventory->ClosingProcess();
								m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
							}
							break;
						case SEASON3B::INTERFACE_DUELWATCH:
							m_pNewDuelWatchWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_DUELWATCH_MAINFRAME:
							m_pNewDuelWatchMainFrameWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_DUELWATCH_USERLIST:
							m_pNewDuelWatchUserListWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_INGAMESHOP:
							g_pInGameShop->ClosingProcess();
							g_pMainFrame->SetBtnState(MAINFRAME_BTN_PARTCHARGE, false);
							break;
						case SEASON3B::INTERFACE_DOPPELGANGER_NPC:
							m_pNewDoppelGangerWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_DOPPELGANGER_FRAME:
							m_pNewDoppelGangerFrame->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_NPC_DIALOGUE:
							m_pNewNPCDialogue->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_QUEST_PROGRESS:
							m_pNewQuestProgress->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_QUEST_PROGRESS_ETC:
							m_pNewQuestProgressByEtc->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC:
							m_pNewEmpireGuardianNPC->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER:
							m_pNewEmpireGuardianTimer->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_MINI_MAP:
							m_pNewMiniMap->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_GENSRANKING:
							g_pNewUIGensRanking->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA:
							m_pNewUnitedMarketPlaceWindow->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_LUCKYITEMWND:
							if (g_pLuckyItemWnd->ClosingProcess() == false)
								return;
							if (IsVisible(SEASON3B::INTERFACE_INVENTORY))
							{
								m_pNewMyInventory->ClosingProcess();
								m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_INVENTORY, false);
							}
							break;
						case SEASON3B::INTERFACE_INVENTORY_EXTENSION:
							m_pNewUIInvenExpansion->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_STORAGE_EXTENSION:
							g_pStorageExpansion->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_MACRO_OFICIAL:
							m_pNewUIMacroMain->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_RANKING_TOP:
							m_NewUIRankTop->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_COMMAND_LIST:
							m_NewUICommandUser->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_EVENT_TIME:
							m_NewUIEventTimeList->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_CUSTOM_MENU:
							m_NewUIMenuUser->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_SHOW_VIP:
							m_NewUIMemberShip->ClosingProcess();
							break;
						case SEASON3B::INTERFACE_INVENTORY_CORE:
							m_pNewUIInventoryCore->ClosingProcess();
							break;
						}
					}
					else
					{
						g_pOption->ClosingProcess();
					}
					break;
				}
			}
			else
			{
				if (dwKey == SEASON3B::INTERFACE_MYSHOP_INVENTORY)
				{
					g_pMyShopInventory->ClosingProcess();
				}
				else if (dwKey == SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY)
				{
					g_pPurchaseShopInventory->ClosingProcess();
				}
				g_pMyInventory->SetPos(WinOpen1, 0);
			}
			break;
		}

		m_pNewUIMng->ShowInterface(dwKey, false);

		int iScreenWidth = GetScreenWidth();
		m_pNewItemEnduranceInfo->SetPos(iScreenWidth);
		m_pNewBuffWindow->SetPos(iScreenWidth);

#if MAIN_UPDATE > 303
		if (m_pNewPartyListWindow->IsEnableMove())
		{
			if (!m_pNewItemEnduranceInfo->IsHideInterfaceWorld() && iScreenWidth == ((int)GetWindowsX))
				m_pNewPartyListWindow->SetPos(iScreenWidth - 128);
			else
				m_pNewPartyListWindow->SetPos(iScreenWidth);
		}
#else
		if (m_pNewPartyListWindow->IsEnableMove())
		{
			m_pNewPartyListWindow->SetPos(iScreenWidth - 2.f);
		}
#endif // MAIN_UPDATE > 303
		

		if (m_pNewUIInvenExpansion->GetThirdInterface() != true && !IsVisible(SEASON3B::INTERFACE_STORAGE_EXTENSION))
		{
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_HERO_POSITION_INFO, true);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_PARTY_INFO_WINDOW, true);
			m_pNewUIMng->ShowInterface(SEASON3B::INTERFACE_MACRO_PROGRESS, true);
		}
	}
}

void SEASON3B::CNewUISystem::Toggle(DWORD dwKey)
{
	IsVisible(dwKey) ? Hide(dwKey) : Show(dwKey);
}

void SEASON3B::CNewUISystem::HideAll()
{
	if (m_pNewUIMng)
	{
		for (int i = SEASON3B::INTERFACE_BEGIN + 1; i < INTERFACE_END; i++)
		{
			if (IsImpossibleHideInterface(i) == false)
			{
				if (IsVisible(i) == true)
				{
					Hide(i);
				}
			}
		}
		GMResetManager->ClosingProcess();
	}
}

void SEASON3B::CNewUISystem::HideAllGroupA()
{
	Hide(SEASON3B::INTERFACE_INVENTORY);
	Hide(SEASON3B::INTERFACE_CHARACTER);

	DWORD dwGroupA[] =
	{
		SEASON3B::INTERFACE_MIXINVENTORY,
		SEASON3B::INTERFACE_STORAGE,
		SEASON3B::INTERFACE_NPCSHOP,
		SEASON3B::INTERFACE_MYSHOP_INVENTORY,
		SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY,
		SEASON3B::INTERFACE_PET,
		SEASON3B::INTERFACE_MYQUEST,
		SEASON3B::INTERFACE_NPCQUEST,
		SEASON3B::INTERFACE_PARTY,
		SEASON3B::INTERFACE_SENATUS,
		SEASON3B::INTERFACE_GUARDSMAN,
		SEASON3B::INTERFACE_COMMAND,
		SEASON3B::INTERFACE_GUILDINFO,
		SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC,
		SEASON3B::INTERFACE_DUELWATCH,
		SEASON3B::INTERFACE_DOPPELGANGER_NPC,
		SEASON3B::INTERFACE_GOLD_BOWMAN,
		SEASON3B::INTERFACE_GOLD_BOWMAN_LENA,
		SEASON3B::INTERFACE_NPC_DIALOGUE,
		SEASON3B::INTERFACE_QUEST_PROGRESS,
		SEASON3B::INTERFACE_QUEST_PROGRESS_ETC,
		SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC,
#ifdef PBG_ADD_GENSRANKING
		SEASON3B::INTERFACE_GENSRANKING,
#endif //PBG_ADD_GENSRANKING
		SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA,
		SEASON3B::INTERFACE_LUCKYITEMWND,
		SEASON3B::INTERFACE_INVENTORY_EXTENSION,
		SEASON3B::INTERFACE_STORAGE_EXTENSION,
		SEASON3B::INTERFACE_MACRO_OFICIAL,
		SEASON3B::INTERFACE_INVENTORY_JEWEL,
		SEASON3B::INTERFACE_INVENTORY_CORE,
		SEASON3B::INTERFACE_MIX_EXPANSION,
		0,
	};

	if (m_pNewUIMng)
	{
		for (int i = 0; dwGroupA[i] != 0; i++)
		{
			m_pNewUIMng->ShowInterface(dwGroupA[i], false);
		}
		GMResetManager->ClosingProcess();
	}
}

void SEASON3B::CNewUISystem::HideAllGroupB()
{
	Hide(SEASON3B::INTERFACE_FRIEND);
	Hide(SEASON3B::INTERFACE_INVENTORY);
	Hide(SEASON3B::INTERFACE_CHARACTER);

	DWORD dwGroupB[] =
	{
		SEASON3B::INTERFACE_MIXINVENTORY,
		SEASON3B::INTERFACE_STORAGE,
		SEASON3B::INTERFACE_NPCSHOP,
		SEASON3B::INTERFACE_MYSHOP_INVENTORY,
		SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY,
		SEASON3B::INTERFACE_PET,
		SEASON3B::INTERFACE_MYQUEST,
		SEASON3B::INTERFACE_NPCQUEST,
		SEASON3B::INTERFACE_PARTY,
		SEASON3B::INTERFACE_SENATUS,
		SEASON3B::INTERFACE_GUARDSMAN,
		SEASON3B::INTERFACE_COMMAND,
		SEASON3B::INTERFACE_GUILDINFO,
		SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC,
		SEASON3B::INTERFACE_CURSEDTEMPLE_NPC,
		SEASON3B::INTERFACE_DUELWATCH,
		SEASON3B::INTERFACE_DOPPELGANGER_NPC,
		SEASON3B::INTERFACE_GOLD_BOWMAN,
		SEASON3B::INTERFACE_GOLD_BOWMAN_LENA,
		SEASON3B::INTERFACE_NPC_DIALOGUE,
		SEASON3B::INTERFACE_QUEST_PROGRESS,
		SEASON3B::INTERFACE_QUEST_PROGRESS_ETC,
		SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC,
#ifdef PBG_ADD_GENSRANKING
		SEASON3B::INTERFACE_GENSRANKING,
#endif //PBG_ADD_GENSRANKING
		SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA,
		SEASON3B::INTERFACE_LUCKYITEMWND,
		SEASON3B::INTERFACE_INVENTORY_EXTENSION,
		SEASON3B::INTERFACE_INVENTORY_JEWEL,
		SEASON3B::INTERFACE_INVENTORY_CORE,
		SEASON3B::INTERFACE_MIX_EXPANSION,
		0,
	};

	if (m_pNewUIMng)
	{
		for (int i = 0; dwGroupB[i] != 0; i++)
		{
			m_pNewUIMng->ShowInterface(dwGroupB[i], false);
		}
		GMResetManager->ClosingProcess();
	}
}
void SEASON3B::CNewUISystem::HideGroupBeforeOpenInterface()
{
	DWORD dwGroupC[] =
	{
		SEASON3B::INTERFACE_PARTY,
		SEASON3B::INTERFACE_COMMAND,
		SEASON3B::INTERFACE_GUILDINFO,
		SEASON3B::INTERFACE_GOLD_BOWMAN,
		SEASON3B::INTERFACE_GOLD_BOWMAN_LENA,
		SEASON3B::INTERFACE_GENSRANKING,
		SEASON3B::INTERFACE_MACRO_OFICIAL,
		0,
	};

	if (m_pNewUIMng)
	{
		for (int i = 0; dwGroupC[i] != 0; i++)
		{
			m_pNewUIMng->ShowInterface(dwGroupC[i], false);
		}
		GMResetManager->ClosingProcess();
	}
}

void SEASON3B::CNewUISystem::HideGroupOpenInterface(DWORD dwKey)
{
	DWORD dwGroupC[] =
	{
		SEASON3B::INTERFACE_CUSTOM_MENU,
		SEASON3B::INTERFACE_SHOW_VIP,
		SEASON3B::INTERFACE_COMMAND_LIST,
		SEASON3B::INTERFACE_EVENT_TIME,
		SEASON3B::INTERFACE_RANKING_TOP,
		0,
	};

	if (m_pNewUIMng)
	{
		for (int i = 0; dwGroupC[i] != 0; i++)
		{
			if (dwGroupC[i] != dwKey && this->IsVisible(dwGroupC[i]))
			{
				this->Hide(dwGroupC[i]);
			}
		}
		GMResetManager->ClosingProcess();
	}
}

void SEASON3B::CNewUISystem::Enable(DWORD dwKey)
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->EnableInterface(dwKey);
	}
}

void SEASON3B::CNewUISystem::Disable(DWORD dwKey)
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->EnableInterface(dwKey, false);
	}
}

bool SEASON3B::CNewUISystem::CheckMouseUse()
{
	if (m_pNewUIMng)
	{
		if (m_pNewUIMng->GetActiveMouseUIObj() || gInterface->BtnProcess())
			return true;
	}
	return false;
}

bool SEASON3B::CNewUISystem::CheckKeyUse()
{
	if (m_pNewUIMng)
	{
		if (m_pNewUIMng->GetActiveKeyUIObj())
			return true;
	}
	return false;
}

bool SEASON3B::CNewUISystem::Update()
{
	if (m_pNewItemMng)
	{
		m_pNewItemMng->Update();
	}

	if (m_pNewUIMng)
	{
		m_pNewUIMng->UpdateMouseEvent();
		m_pNewUIMng->UpdateKeyEvent();
		return m_pNewUIMng->Update();
	}
	return false;
}

bool SEASON3B::CNewUISystem::Render()
{
	bool bResult = false;

	BeginBitmap();
	if (m_pNewUIMng)
		bResult = m_pNewUIMng->Render();
	EndBitmap();

	return bResult;
}

void SEASON3B::CNewUISystem::RenderFrameUpdate(double WindowsX, double WindowsY)
{
	DWORD dwGroupA[] =
	{
		SEASON3B::INTERFACE_PARTY,
		SEASON3B::INTERFACE_MYQUEST,
		SEASON3B::INTERFACE_NPCQUEST,
		SEASON3B::INTERFACE_GUILDINFO,
		SEASON3B::INTERFACE_TRADE,
		SEASON3B::INTERFACE_STORAGE,
		SEASON3B::INTERFACE_MIXINVENTORY,
		SEASON3B::INTERFACE_COMMAND,
		SEASON3B::INTERFACE_PET,
		SEASON3B::INTERFACE_NPCSHOP,
		SEASON3B::INTERFACE_INVENTORY,
		SEASON3B::INTERFACE_MYSHOP_INVENTORY,
		SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY,
		SEASON3B::INTERFACE_CHARACTER,
		SEASON3B::INTERFACE_NPCBREEDER,
		SEASON3B::INTERFACE_SERVERDIVISION,
		SEASON3B::INTERFACE_DEVILSQUARE,
		SEASON3B::INTERFACE_BLOODCASTLE,
		SEASON3B::INTERFACE_NPCGUILDMASTER,
		SEASON3B::INTERFACE_GUARDSMAN,
		SEASON3B::INTERFACE_SENATUS,
		SEASON3B::INTERFACE_CATAPULT,
		SEASON3B::INTERFACE_REFINERY,
		SEASON3B::INTERFACE_REFINERYINFO,
		SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC,
		SEASON3B::INTERFACE_CURSEDTEMPLE_NPC,
		SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM,
		SEASON3B::INTERFACE_CHATINPUTBOX,
		SEASON3B::INTERFACE_WINDOW_MENU,
		SEASON3B::INTERFACE_OPTION,
		SEASON3B::INTERFACE_CHATLOGWINDOW,
		SEASON3B::INTERFACE_PARTY_INFO_WINDOW,
		SEASON3B::INTERFACE_BLOODCASTLE_TIME,
		SEASON3B::INTERFACE_CHAOSCASTLE_TIME,
		SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE,
		SEASON3B::INTERFACE_MAINFRAME,
		SEASON3B::INTERFACE_MASTER_LEVEL,
		SEASON3B::INTERFACE_GOLD_BOWMAN,
		SEASON3B::INTERFACE_GOLD_BOWMAN_LENA,
		SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION,
		SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN,
		SEASON3B::INTERFACE_INGAMESHOP,
		SEASON3B::INTERFACE_DOPPELGANGER_NPC,
		SEASON3B::INTERFACE_QUEST_PROGRESS,
		SEASON3B::INTERFACE_QUEST_PROGRESS_ETC,
		SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC,
		SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER,
		SEASON3B::INTERFACE_NPC_DIALOGUE,
		SEASON3B::INTERFACE_GENSRANKING,
		SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA,
		SEASON3B::INTERFACE_LUCKYITEMWND,
		SEASON3B::INTERFACE_MACRO_OFICIAL,
		SEASON3B::INTERFACE_MACRO_OFICIAL_SUB,
		SEASON3B::INTERFACE_MIX_EXPANSION,
		0,
	};

	if (m_pNewUIMng)
	{
		for (int i = 0; dwGroupA[i] != 0; i++)
		{
			m_pNewUIMng->UpdateInterface(dwGroupA[i], WindowsX, WindowsY);
		}
	}
}

CNewUIManager* SEASON3B::CNewUISystem::GetNewUIManager() const
{
	return m_pNewUIMng;
}

CNewUI3DRenderMng* SEASON3B::CNewUISystem::GetNewUI3DRenderMng() const
{
	return m_pNewUI3DRenderMng;
}


CNewUIHotKey* SEASON3B::CNewUISystem::GetNewUIHotKey() const
{
	return m_pNewUIHotKey;
}

bool SEASON3B::CNewUISystem::IsImpossibleSendMoveInterface()
{
	if (IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
		|| IsVisible(SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC)
		|| IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND)
		)
	{
		return true;
	}

	return false;
}

bool SEASON3B::CNewUISystem::IsImpossibleTradeInterface()
{
	if (IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
		|| IsVisible(SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC)
		|| IsVisible(SEASON3B::INTERFACE_STORAGE)
		|| IsVisible(SEASON3B::INTERFACE_INGAMESHOP)
		|| IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND)
		|| IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL)
		|| IsVisible(SEASON3B::INTERFACE_MIX_EXPANSION)
		)
	{
		return true;
	}

	return false;
}

bool SEASON3B::CNewUISystem::IsImpossibleDuelInterface()
{
	if (IsVisible(SEASON3B::INTERFACE_MIXINVENTORY)
		|| IsVisible(SEASON3B::INTERFACE_KANTURU2ND_ENTERNPC)
		|| IsVisible(SEASON3B::INTERFACE_STORAGE)
		|| IsVisible(SEASON3B::INTERFACE_INGAMESHOP)
		|| IsVisible(SEASON3B::INTERFACE_LUCKYITEMWND)
		|| IsVisible(SEASON3B::INTERFACE_MIX_EXPANSION)
		)
	{
		return true;
	}

	return false;
}

bool SEASON3B::CNewUISystem::IsImpossibleHideInterface(DWORD dwKey)
{
	if (dwKey == SEASON3B::INTERFACE_MAINFRAME
		|| dwKey == SEASON3B::INTERFACE_SKILL_LIST
		|| dwKey == SEASON3B::INTERFACE_SLIDEWINDOW
		|| dwKey == SEASON3B::INTERFACE_MESSAGEBOX
		|| dwKey == SEASON3B::INTERFACE_CHATLOGWINDOW
		|| dwKey == SEASON3B::INTERFACE_PARTY_INFO_WINDOW
		|| dwKey == SEASON3B::INTERFACE_KANTURU_INFO
		|| dwKey == SEASON3B::INTERFACE_BLOODCASTLE_TIME
		|| dwKey == SEASON3B::INTERFACE_CHAOSCASTLE_TIME
		|| dwKey == SEASON3B::INTERFACE_BATTLE_SOCCER_SCORE
		|| dwKey == SEASON3B::INTERFACE_DUEL_WINDOW
		|| dwKey == SEASON3B::INTERFACE_CRYWOLF
		|| dwKey == SEASON3B::INTERFACE_HERO_POSITION_INFO
		|| dwKey == SEASON3B::INTERFACE_NAME_WINDOW
		|| dwKey == SEASON3B::INTERFACE_SIEGEWARFARE
		|| dwKey == SEASON3B::INTERFACE_ITEM_TOOLTIP
		|| dwKey == SEASON3B::INTERFACE_HOTKEY
		|| dwKey == SEASON3B::INTERFACE_CURSEDTEMPLE_GAMESYSTEM
		|| dwKey == SEASON3B::INTERFACE_ITEM_ENDURANCE_INFO
		|| dwKey == SEASON3B::INTERFACE_BUFF_WINDOW
		|| (dwKey >= SEASON3B::INTERFACE_3DRENDERING_CAMERA_BEGIN && dwKey <= SEASON3B::INTERFACE_3DRENDERING_CAMERA_END)
		|| dwKey == SEASON3B::INTERFACE_DUELWATCH_MAINFRAME
		|| dwKey == SEASON3B::INTERFACE_DUELWATCH_USERLIST
		|| dwKey == SEASON3B::INTERFACE_DOPPELGANGER_FRAME
		|| dwKey == SEASON3B::INTERFACE_GOLD_BOWMAN
		|| dwKey == SEASON3B::INTERFACE_GOLD_BOWMAN_LENA
		|| dwKey == SEASON3B::INTERFACE_EMPIREGUARDIAN_TIMER
		)
	{
		return true;
	}

	return false;
}

void SEASON3B::CNewUISystem::UpdateSendMoveInterface()
{
	if (IsVisible(SEASON3B::INTERFACE_TRADE))
	{
		SendRequestTradeExit();
		Hide(SEASON3B::INTERFACE_TRADE);
	}
	if (IsVisible(SEASON3B::INTERFACE_STORAGE))
	{
		Hide(SEASON3B::INTERFACE_STORAGE);
	}
	if (IsVisible(SEASON3B::INTERFACE_NPCGUILDMASTER))
	{
		Hide(SEASON3B::INTERFACE_NPCGUILDMASTER);
	}
	if (IsVisible(SEASON3B::INTERFACE_MYQUEST))
	{
		Hide(SEASON3B::INTERFACE_MYQUEST);
	}
	if (IsVisible(SEASON3B::INTERFACE_NPCQUEST))
	{
		Hide(SEASON3B::INTERFACE_NPCQUEST);
	}
	if (IsVisible(SEASON3B::INTERFACE_NPCSHOP))
	{
		Hide(SEASON3B::INTERFACE_NPCSHOP);
	}
	if (IsVisible(SEASON3B::INTERFACE_GUARDSMAN))
	{
		Hide(SEASON3B::INTERFACE_GUARDSMAN);
	}
	if (IsVisible(SEASON3B::INTERFACE_DEVILSQUARE))
	{
		Hide(SEASON3B::INTERFACE_DEVILSQUARE);
	}
	if (IsVisible(SEASON3B::INTERFACE_BLOODCASTLE))
	{
		Hide(SEASON3B::INTERFACE_BLOODCASTLE);
	}
	if (IsVisible(SEASON3B::INTERFACE_CURSEDTEMPLE_NPC))
	{
		Hide(SEASON3B::INTERFACE_CURSEDTEMPLE_NPC);
	}
	if (IsVisible(SEASON3B::INTERFACE_MYSHOP_INVENTORY))
	{
		Hide(SEASON3B::INTERFACE_MYSHOP_INVENTORY);
	}
	if (IsVisible(SEASON3B::INTERFACE_INVENTORY_JEWEL))
	{
		Hide(SEASON3B::INTERFACE_INVENTORY_JEWEL);
	}
	if (IsVisible(SEASON3B::INTERFACE_INVENTORY_CORE))
	{
		Hide(SEASON3B::INTERFACE_INVENTORY_CORE);
	}
	if (IsVisible(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY))
	{
		Hide(SEASON3B::INTERFACE_PURCHASESHOP_INVENTORY);
	}
	if (IsVisible(SEASON3B::INTERFACE_DUELWATCH))
	{
		Hide(SEASON3B::INTERFACE_DUELWATCH);
	}
	if (IsVisible(SEASON3B::INTERFACE_DOPPELGANGER_NPC))
	{
		Hide(SEASON3B::INTERFACE_DOPPELGANGER_NPC);
	}
	if (IsVisible(SEASON3B::INTERFACE_NPC_DIALOGUE))
	{
		Hide(SEASON3B::INTERFACE_NPC_DIALOGUE);
	}
	if (IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS))
	{
		Hide(SEASON3B::INTERFACE_QUEST_PROGRESS);
	}
	if (IsVisible(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC))
	{
		Hide(SEASON3B::INTERFACE_QUEST_PROGRESS_ETC);
	}
	if (IsVisible(SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC))
	{
		Hide(SEASON3B::INTERFACE_EMPIREGUARDIAN_NPC);
	}
	if (IsVisible(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION))
	{
		Hide(SEASON3B::INTERFACE_LUCKYCOIN_REGISTRATION);
	}
	if (IsVisible(SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN))
	{
		Hide(SEASON3B::INTERFACE_EXCHANGE_LUCKYCOIN);
	}
	if (IsVisible(SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA))
	{
		Hide(SEASON3B::INTERFACE_UNITEDMARKETPLACE_NPC_JULIA);
	}
	if (IsVisible(SEASON3B::INTERFACE_MIX_EXPANSION))
	{
		//SendRequestTradeExit();
		Hide(SEASON3B::INTERFACE_MIX_EXPANSION);
	}
}

CNewUISystem* SEASON3B::CNewUISystem::GetInstance()
{
	static CNewUISystem s_NewUISystem;
	return &s_NewUISystem;
}

CNewUIChatLogWindow* SEASON3B::CNewUISystem::GetUI_NewChatLogWindow() const
{
	return m_pNewChatLogWindow;
}

CNewUISlideWindow* SEASON3B::CNewUISystem::GetUI_NewSlideWindow() const
{
	return m_pNewSlideWindow;
}

CNewUIFriendWindow* SEASON3B::CNewUISystem::GetUI_NewFriendWindow() const
{
	return m_pNewFriendWindow;
}

CNewUIMainFrameWindow* SEASON3B::CNewUISystem::GetUI_NewMainFrameWindow() const
{
	return m_pNewMainFrameWindow;
}

CNewUISkillList* SEASON3B::CNewUISystem::GetUI_NewSkillList() const
{
	return m_pNewSkillList;
}

CNewUIChatInputBox* SEASON3B::CNewUISystem::GetUI_NewChatInputBox() const
{
	return m_pNewChatInputBox;
}

CNewUIItemMng* SEASON3B::CNewUISystem::GetUI_NewItemMng() const
{
	return m_pNewItemMng;
}

CNewUIMyInventory* SEASON3B::CNewUISystem::GetUI_NewMyInventory() const
{
	return m_pNewMyInventory;
}

CNewUINPCShop* SEASON3B::CNewUISystem::GetUI_NewNpcShop() const
{
	return m_pNewNPCShop;
}

CNewUIPetInfoWindow* SEASON3B::CNewUISystem::GetUI_NewPetInfoWindow() const
{
	return m_pNewPetInfoWindow;
}

CNewUIMixInventory* SEASON3B::CNewUISystem::GetUI_NewMixInventory() const
{
	return m_pNewMixInventory;
}

CNewUICastleWindow* SEASON3B::CNewUISystem::GetUI_NewCastleWindow() const
{
	return m_pNewCastleWindow;
}

CNewUIGuardWindow* SEASON3B::CNewUISystem::GetUI_NewGuardWindow() const
{
	return m_pNewGuardWindow;
}

CNewUIGatemanWindow* SEASON3B::CNewUISystem::GetUI_NewGatemanWindow() const
{
	return m_pNewGatemanWindow;
}

CNewUIGateSwitchWindow* SEASON3B::CNewUISystem::GetUI_NewGateSwitchWindow() const
{
	return m_pNewGateSwitchWindow;
}

CNewUIStorageInventory* SEASON3B::CNewUISystem::GetUI_NewStorageInventory() const
{
	return m_pNewStorageInventory;
}

CNewUIGuildMakeWindow* SEASON3B::CNewUISystem::GetUI_NewGuildMakeWindow() const
{
	return m_pNewGuildMakeWindow;
}

CNewUIGuildInfoWindow* SEASON3B::CNewUISystem::GetUI_NewGuildInfoWindow() const
{
	return m_pNewGuildInfoWindow;
}

CNewUICryWolf* SEASON3B::CNewUISystem::GetUI_NewCryWolfInterface() const
{
	return m_pNewCryWolfInterface;
}

#if MAIN_UPDATE > 303
CNewUIMasterSkillTree* SEASON3B::CNewUISystem::GetUI_NewUIMasterSkillTree() const
{
	return m_pNewUIMasterSkillTree;
}
#endif // MAIN_UPDATE != 303


CNewUIMyShopInventory* SEASON3B::CNewUISystem::GetUI_NewMyShopInventory() const
{
	return m_pNewMyShopInventory;
}

CNewUIPurchaseShopInventory* SEASON3B::CNewUISystem::GetUI_NewPurchaseShopInventory() const
{
	return m_pNewPurchaseShopInventory;
}

CNewUICharacterInfoWindow* SEASON3B::CNewUISystem::GetUI_NewCharacterInfoWindow() const
{
	return m_pNewCharacterInfoWindow;
}

CNewUIMyQuestInfoWindow* SEASON3B::CNewUISystem::GetUI_NewMyQuestInfoWindow() const
{
	return m_pNewMyQuestInfoWindow;
}

CNewUIPartyInfoWindow* SEASON3B::CNewUISystem::GetUI_NewPartyInfoWindow() const
{
	return m_pNewPartyInfoWindow;
}

CNewUIPartyListWindow* SEASON3B::CNewUISystem::GetUI_NewPartyListWindow() const
{
	return m_pNewPartyListWindow;
}

CNewUINPCQuest* SEASON3B::CNewUISystem::GetUI_NewNPCQuest() const
{
	return m_pNewNPCQuest;
}

CNewUIEnterBloodCastle* SEASON3B::CNewUISystem::GetUI_NewEnterBloodCastle() const
{
	return m_pNewEnterBloodCastle;
}

CNewUIDevilSquareEnter* SEASON3B::CNewUISystem::GetUI_NewEnterDevilSquare() const
{
	return m_pNewEnterDevilSquare;
}

CNewUIBloodCastle* SEASON3B::CNewUISystem::GetUI_NewBloodCastle() const
{
	return m_pNewBloodCastle;
}

CNewUITrade* SEASON3B::CNewUISystem::GetUI_NewTrade() const
{
	return m_pNewTrade;
}

CNewUIKanturu2ndEnterNpc* SEASON3B::CNewUISystem::GetUI_NewKanturu2ndEnterNpc() const
{
	return m_pNewKanturu2ndEnterNpc;
}

CNewUIKanturuInfoWindow* SEASON3B::CNewUISystem::GetUI_NewKanturuInfoWindow() const
{
	return m_pNewKanturuInfoWindow;
}

CNewUICatapultWindow* SEASON3B::CNewUISystem::GetUI_NewCatapultWindow() const
{
	return m_pNewCatapultWindow;
}

CNewUIChaosCastleTime* SEASON3B::CNewUISystem::GetUI_NewChaosCastleTime() const
{
	return m_pNewChaosCastleTime;
}

CNewUICommandWindow* SEASON3B::CNewUISystem::GetUI_NewCommandWindow() const
{
	return m_pNewCommandWindow;
}

CNewUIWindowMenu* SEASON3B::CNewUISystem::GetUI_NewWindowMenu() const
{
	return m_pNewWindowMenu;
}

CNewUIOptionWindow* SEASON3B::CNewUISystem::GetUI_NewOptionWindow() const
{
	return m_pNewOptionWindow;
}

CNewUIHeroPositionInfo* SEASON3B::CNewUISystem::GetUI_NewHeroPositionInfo() const
{
	return m_pNewHeroPositionInfo;
}

CNewUIHelpWindow* SEASON3B::CNewUISystem::GetUI_NewHelpWindow() const
{
	return m_pNewHelpWindow;
}

CNewUIItemExplanationWindow* SEASON3B::CNewUISystem::GetUI_NewItemExplanationWindow() const
{
	return m_pNewItemExplanationWindow;
}

CNewUISetItemExplanation* SEASON3B::CNewUISystem::GetUI_NewSetItemExplanation() const
{
	return m_pNewSetItemExplanation;
}

CNewUIQuickCommandWindow* SEASON3B::CNewUISystem::GetUI_NewQuickCommandWindow() const
{
	return m_pNewQuickCommandWindow;
}

CNewUIMoveCommandWindow* SEASON3B::CNewUISystem::GetUI_NewMoveCommandWindow() const
{
	return m_pNewMoveCommandWindow;
}

CNewUIBattleSoccerScore* SEASON3B::CNewUISystem::GetUI_NewBattleSoccerScore() const
{
	return m_pNewBattleSoccerScore;
}

CNewUIDuelWindow* SEASON3B::CNewUISystem::GetUI_NewDuelWindow() const
{
	return m_pNewDuelWindow;
}

CNewUISiegeWarfare* SEASON3B::CNewUISystem::GetUI_NewSiegeWarfare() const
{
	return m_pNewSiegeWarfare;
}

CNewUIItemEnduranceInfo* SEASON3B::CNewUISystem::GetUI_NewItemEnduranceInfo() const
{
	return m_pNewItemEnduranceInfo;
}

CNewUIBuffWindow* SEASON3B::CNewUISystem::GetUI_NewBuffWindow() const
{
	return m_pNewBuffWindow;
}

CNewUICursedTempleEnter* SEASON3B::CNewUISystem::GetUI_NewCursedTempleEnterWindow() const
{
	return m_pNewCursedTempleEnterWindow;
}

CNewUICursedTempleSystem* SEASON3B::CNewUISystem::GetUI_NewCursedTempleWindow() const
{
	return m_pNewCursedTempleWindow;
}

CNewUICursedTempleResult* SEASON3B::CNewUISystem::GetUI_NewCursedTempleResultWindow() const
{
	return m_pNewCursedTempleResultWindow;
}

CNewUIGoldBowmanWindow* SEASON3B::CNewUISystem::GetUI_pNewGoldBowman() const
{
	return m_pNewGoldBowman;
}

CNewUIGoldBowmanLena* SEASON3B::CNewUISystem::GetUI_pNewGoldBowmanLena() const
{
	return m_pNewGoldBowmanLena;
}

CNewUIRegistrationLuckyCoin* SEASON3B::CNewUISystem::GetUI_pNewLuckyCoinRegistration() const
{
	return m_pNewLuckyCoinRegistration;
}

CNewUIExchangeLuckyCoin* SEASON3B::CNewUISystem::GetUI_pNewExchangeLuckyCoin() const
{
	return m_pNewExchangeLuckyCoinWindow;
}

CNewUIMiniMap* SEASON3B::CNewUISystem::GetUI_pNewUIMiniMap() const
{
	return m_pNewMiniMap;
}

CNewUIDuelWatchWindow* SEASON3B::CNewUISystem::GetUI_pNewDuelWatch() const
{
	return m_pNewDuelWatchWindow;
}

CNewUIDuelWatchMainFrameWindow* SEASON3B::CNewUISystem::GetUI_pNewDuelWatchMainFrame() const
{
	return m_pNewDuelWatchMainFrameWindow;
}

CNewUIDuelWatchUserListWindow* SEASON3B::CNewUISystem::GetUI_pNewDuelWatchUserList() const
{
	return m_pNewDuelWatchUserListWindow;
}

#ifdef PBG_ADD_INGAMESHOP_UI_MAINFRAME
CNewUIInGameShop* SEASON3B::CNewUISystem::GetUI_pNewInGameShop() const
{
	return m_pNewInGameShop;
}
#endif //PBG_ADD_INGAMESHOP_UI_MAINFRAME

CNewUIDoppelGangerWindow* SEASON3B::CNewUISystem::GetUI_pNewDoppelGangerWindow() const
{
	return m_pNewDoppelGangerWindow;
}

CNewUIDoppelGangerFrame* SEASON3B::CNewUISystem::GetUI_pNewDoppelGangerFrame() const
{
	return m_pNewDoppelGangerFrame;
}

CNewUINPCDialogue* SEASON3B::CNewUISystem::GetUI_NewNPCDialogue() const
{
	return m_pNewNPCDialogue;
}

CNewUIQuestProgress* SEASON3B::CNewUISystem::GetUI_NewQuestProgress() const
{
	return m_pNewQuestProgress;
}

CNewUIQuestProgressByEtc* SEASON3B::CNewUISystem::GetUI_NewQuestProgressByEtc() const
{
	return m_pNewQuestProgressByEtc;
}

CNewUIEmpireGuardianNPC* SEASON3B::CNewUISystem::GetUI_pNewEmpireGuardianNPC() const
{
	return m_pNewEmpireGuardianNPC;
}

CNewUIEmpireGuardianTimer* SEASON3B::CNewUISystem::GetUI_pNewEmpireGuardianTimer() const
{
	return m_pNewEmpireGuardianTimer;
}

#ifdef PBG_MOD_STAMINA_UI
CNewUIStamina* SEASON3B::CNewUISystem::GetUI_pNewUIStamina() const
{
	return m_pNewUIStamina;
}
#endif //PBG_MOD_STAMINA_UI

#ifdef PBG_ADD_GENSRANKING
CNewUIGensRanking* SEASON3B::CNewUISystem::GetUI_NewGensRanking() const
{
	return m_pNewGensRanking;
}
#endif //PBG_ADD_GENSRANKING

CNewUIUnitedMarketPlaceWindow* SEASON3B::CNewUISystem::GetUI_pNewUnitedMarketPlaceWindow() const
{
	return m_pNewUnitedMarketPlaceWindow;
}

CNewUILuckyItemWnd* SEASON3B::CNewUISystem::Get_pNewUILuckyItemWnd() const
{
	return m_pNewUILuckyItemWnd;
}

CNewUIInvenExpansion* SEASON3B::CNewUISystem::Get_pNewUIInvenExpansion() const
{
	return m_pNewUIInvenExpansion;
}

CNewUIStorageExpansion* SEASON3B::CNewUISystem::Get_pNewUIStorageExpansion() const
{
	return m_pNewUIStorageExpansion;
}

CNewUIMacroMain* SEASON3B::CNewUISystem::GetUI_pNewUIMacroWindow() const
{
	return m_pNewUIMacroMain;
}

CNewUIMacroSub* SEASON3B::CNewUISystem::GetUI_pNewUIMacroSubWindow() const
{
	return m_pNewUIMacroSub;
}

CNewUIMacroGaugeBar* SEASON3B::CNewUISystem::GetUI_pNewUIMacroGaugeBar() const
{
	return m_pNewUIMacroGaugeBar;
}

CNewUIInventoryJewel* SEASON3B::CNewUISystem::GetUI_pNewUIInventoryJewel() const
{
	return m_pNewUIInventoryJewel;
}

#ifdef EFFECT_MNG_HANDLE
CGFxEffectHandle* SEASON3B::CNewUISystem::GetUI_gfxHangle() const
{
	return m_gfxEffectHandle;
}
#endif // EFFECT_MNG_HANDLE

CNewUIReconnect* SEASON3B::CNewUISystem::GetUI_pNewUIReconnect() const
{
	return m_NewUIReconnect;
}

#ifdef PACK_FILE_DECRYPT_H
CNewUIItemTooltip* SEASON3B::CNewUISystem::GetUI_NewItemTooltip() const
{
	return m_pNewUIItemTooltip;
}
#endif // PACK_FILE_DECRYPT_H

CScriptItem* SEASON3B::CNewUISystem::GetUI_NewItemScript() const
{
	return m_pScriptItem;
}

CGMCameraWorld* SEASON3B::CNewUISystem::GetUI_NewUICamWebzen() const
{
	return m_iCamWebzen;
}

CNewUINameWindow* SEASON3B::CNewUISystem::GetUI_NewUINameWindow() const
{
	return m_pNewNameWindow;
}

CNewUIRankingTop* SEASON3B::CNewUISystem::GetUI_NewUIRankingTop() const
{
	return m_NewUIRankTop;
}

CNewUICommandFilter* SEASON3B::CNewUISystem::GetUI_NewUICommandUser() const
{
	return m_NewUICommandUser;
}

CNewUIEventTimeView* SEASON3B::CNewUISystem::GetUI_NewUIEventTimeList() const
{
	return m_NewUIEventTimeList;
}

CNewUIMenuUser* SEASON3B::CNewUISystem::GetUI_NewUIMenuUser() const
{
	return m_NewUIMenuUser;
}

CNewUIPurcharseVip* SEASON3B::CNewUISystem::GetUI_NewUIMemberShip() const
{
	return m_NewUIMemberShip;
}

CNewUIInventoryCore* SEASON3B::CNewUISystem::GetUI_NewUIInventoryCore() const
{
	return m_pNewUIInventoryCore;
}

CNewUIMixExpansion* SEASON3B::CNewUISystem::GetUI_NewUIMixExpansion() const
{
	return m_pNewUIMixExpansion;
}


