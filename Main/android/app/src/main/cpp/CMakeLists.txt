cmake_minimum_required(VERSION 3.22.1)
project("mucrossengine" CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

# ─────────────────────────────────────────────────────────────────────────────
# Paths
# ─────────────────────────────────────────────────────────────────────────────
set(SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../source")
set(DEP_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../dependencies")
set(UTIL_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../Util")
set(PLATFORM_DIR "${SOURCE_DIR}/Platform")
set(NATIVE_APP_GLUE_DIR "${ANDROID_NDK}/sources/android/native_app_glue")

# ─────────────────────────────────────────────────────────────────────────────
# Compiler definitions (mirror Release config from Main.vcxproj)
# ─────────────────────────────────────────────────────────────────────────────
add_compile_definitions(
    __ANDROID__
    NDEBUG
    _FOREIGN_NDEBUG
    _LANGUAGE_FOREIGN
    _LANGUAGE_ENG
    LDS_PATCH_GLOBAL_100520
    PROTO_EXTRA
    MAIN_UPDATE=603
    FT2_BUILD_LIBRARY
    ASIO_STANDALONE
    ASIO_NO_DEPRECATED
    # Disable Windows-specific features
    NO_DIRECTSHOW
    NO_NPROTECT
    NO_THEMIDA
    __NDK_FPABI__=
)

# ─────────────────────────────────────────────────────────────────────────────
# Include paths
# ─────────────────────────────────────────────────────────────────────────────
include_directories(
    ${SOURCE_DIR}
    ${PLATFORM_DIR}
    ${NATIVE_APP_GLUE_DIR}
    ${DEP_DIR}/include
    ${DEP_DIR}/include/glm
    ${DEP_DIR}/include/Sol
    ${DEP_DIR}/include/Lua
    ${DEP_DIR}/include/asio
    ${DEP_DIR}/include/cryptopp820
    ${DEP_DIR}/ImGui
    ${UTIL_DIR}/include
)

# ─────────────────────────────────────────────────────────────────────────────
# Third-party source compiled inline
# ─────────────────────────────────────────────────────────────────────────────

# pugixml (already in source/)
# freetype2 — TODO: add source to Main/dependencies/freetype-src/
#   find_package(Freetype) or add_subdirectory(...)
# lua 5.4  — TODO: add source to Main/dependencies/lua-5.4-src/
# crypto++ — TODO: add source to Main/dependencies/cryptopp-src/
# turbojpeg— TODO: add source to Main/dependencies/libjpeg-turbo-src/

# ─────────────────────────────────────────────────────────────────────────────
# Android NDK libraries
# ─────────────────────────────────────────────────────────────────────────────
find_library(log-lib     log)
find_library(android-lib android)
find_library(egl-lib     EGL)
find_library(gles3-lib   GLESv3)
find_library(opensl-lib  OpenSLES)
find_library(dl-lib      dl)
find_library(z-lib       z)

# ─────────────────────────────────────────────────────────────────────────────
# Platform Abstraction Layer sources
# ─────────────────────────────────────────────────────────────────────────────
set(PLATFORM_SOURCES
    ${PLATFORM_DIR}/AndroidEglWindow.cpp
    ${PLATFORM_DIR}/AndroidNetworkPollCompat.cpp
    ${PLATFORM_DIR}/AndroidTextRenderer.cpp
    ${PLATFORM_DIR}/AudioOpenSLES.cpp
    ${PLATFORM_DIR}/GameAssetPath.cpp
    ${PLATFORM_DIR}/GameConfigBootstrap.cpp
    ${PLATFORM_DIR}/GameConnectServerBootstrap.cpp
    ${PLATFORM_DIR}/GameDownloader.cpp
    ${PLATFORM_DIR}/GameMouseInput.cpp
    ${PLATFORM_DIR}/GamePacketCryptoBootstrap.cpp
    ${PLATFORM_DIR}/LegacyAndroidClientStubs.cpp
    ${PLATFORM_DIR}/LegacyClientRuntime.cpp
    ${PLATFORM_DIR}/LauncherHelperAndroid.cpp
    ${PLATFORM_DIR}/OpenGLESRenderBackend.cpp
    ${PLATFORM_DIR}/RenderBackend.cpp
    ${PLATFORM_DIR}/SimpleModulusStub.cpp
    ${PLATFORM_DIR}/MuCryptoStub.cpp
    ${PLATFORM_DIR}/LegacyGlobalsAndroid.cpp
    ${PLATFORM_DIR}/ImGuiBackendStub.cpp
    ${PLATFORM_DIR}/TurboJpegStub.cpp
    ${PLATFORM_DIR}/LegacyBootstrapStubs.cpp
    ${PLATFORM_DIR}/CGMFontLayerStub.cpp
    ${PLATFORM_DIR}/CCRC32Adapter.cpp
    ${DEP_DIR}/ImGui/imgui.cpp
    ${DEP_DIR}/ImGui/imgui_draw.cpp
    ${DEP_DIR}/ImGui/imgui_tables.cpp
    ${DEP_DIR}/ImGui/imgui_widgets.cpp
)

# ─────────────────────────────────────────────────────────────────────────────
# Main game source files
# (Winmain.cpp and WINHANDLE.cpp are excluded — replaced by android_main.cpp)
# ─────────────────────────────────────────────────────────────────────────────
set(GAME_SOURCES
    # ── Entry point ──────────────────────────────────────────────────────────
    ${CMAKE_CURRENT_SOURCE_DIR}/android_main.cpp
    ${NATIVE_APP_GLUE_DIR}/android_native_app_glue.c

    # ── Precompiled header impl ──────────────────────────────────────────────
    ${SOURCE_DIR}/StdAfx.cpp

    # ── Core rendering / 3D ─────────────────────────────────────────────────
    ${SOURCE_DIR}/ZzzOpenglUtil.cpp
    ${SOURCE_DIR}/ZzzBMD.cpp
    ${SOURCE_DIR}/ZzzCharacter.cpp
    ${SOURCE_DIR}/ZzzEffect.cpp
    ${SOURCE_DIR}/ZzzEffectBlurSpark.cpp
    ${SOURCE_DIR}/ZzzEffectFireLeave.cpp
    ${SOURCE_DIR}/ZzzEffectJoint.cpp
    ${SOURCE_DIR}/ZzzEffectMagicSkill.cpp
    ${SOURCE_DIR}/ZzzEffectNoUse.cpp
    ${SOURCE_DIR}/ZzzEffectParticle.cpp
    ${SOURCE_DIR}/ZzzEffectPoint.cpp
    ${SOURCE_DIR}/ZzzEffectPointer.cpp
    ${SOURCE_DIR}/ZzzLodTerrain.cpp
    ${SOURCE_DIR}/ZzzObject.cpp
    ${SOURCE_DIR}/ZzzTexture.cpp
    ${SOURCE_DIR}/ZzzInfomation.cpp
    ${SOURCE_DIR}/ZzzInterface.cpp
    ${SOURCE_DIR}/ZzzInventory.cpp
    ${SOURCE_DIR}/ZzzAI.cpp
    ${SOURCE_DIR}/zzzeffectsprite.cpp
    ${SOURCE_DIR}/zzzpath.cpp
    ${SOURCE_DIR}/ZzzOpenData.cpp
    ${SOURCE_DIR}/ShadowVolume.cpp
    ${SOURCE_DIR}/Sprite.cpp
    ${SOURCE_DIR}/BoneManager.cpp
    ${SOURCE_DIR}/CGMModelManager.cpp
    ${SOURCE_DIR}/CGMRenderGroupMesh.cpp
    ${SOURCE_DIR}/CShaderGL.cpp
    ${SOURCE_DIR}/Math/ZzzMathLib.cpp
    ${SOURCE_DIR}/SMD.cpp
    ${SOURCE_DIR}/SMD2BMD.cpp

    # ── Game loop / scenes ───────────────────────────────────────────────────
    ${SOURCE_DIR}/ZzzScene.cpp
    ${SOURCE_DIR}/GMLogInScene.cpp
    ${SOURCE_DIR}/GMCharacterScene.cpp
    ${SOURCE_DIR}/LoadingScene.cpp
    ${SOURCE_DIR}/MovieScene.cpp

    # ── Characters / objects ─────────────────────────────────────────────────
    ${SOURCE_DIR}/CGMCharacter.cpp
    ${SOURCE_DIR}/CharacterManager.cpp
    ${SOURCE_DIR}/CGMEffectHandle.cpp
    ${SOURCE_DIR}/CGMItemEffect.cpp
    ${SOURCE_DIR}/CGMItemMng.cpp
    ${SOURCE_DIR}/CGMItemWings.cpp
    ${SOURCE_DIR}/CGMJewelOfAction.cpp
    ${SOURCE_DIR}/CGMMonsterMng.cpp
    ${SOURCE_DIR}/CGMPhysicsManager.cpp
    ${SOURCE_DIR}/PhysicsManager.cpp
    ${SOURCE_DIR}/CGMInvasionManager.cpp
    ${SOURCE_DIR}/CGMResetManager.cpp
    ${SOURCE_DIR}/CGMCameraWorld.cpp
    ${SOURCE_DIR}/CameraManager.cpp
    ${SOURCE_DIR}/CameraMove.cpp
    ${SOURCE_DIR}/CAIController.cpp

    # ── Maps / zones ─────────────────────────────────────────────────────────
    ${SOURCE_DIR}/GMAcheron.cpp
    ${SOURCE_DIR}/GMAida.cpp
    ${SOURCE_DIR}/GMBattleCastle.cpp
    ${SOURCE_DIR}/GMCryingWolf2nd.cpp
    ${SOURCE_DIR}/GMCrywolf1st.cpp
    ${SOURCE_DIR}/GMDeventer.cpp
    ${SOURCE_DIR}/GMDoppelGanger1.cpp
    ${SOURCE_DIR}/GMDoppelGanger2.cpp
    ${SOURCE_DIR}/GMDoppelGanger3.cpp
    ${SOURCE_DIR}/GMDoppelGanger4.cpp
    ${SOURCE_DIR}/GMDuelArena.cpp
    ${SOURCE_DIR}/GMEmpireGuardian1.cpp
    ${SOURCE_DIR}/GMEmpireGuardian2.cpp
    ${SOURCE_DIR}/GMEmpireGuardian3.cpp
    ${SOURCE_DIR}/GMEmpireGuardian4.cpp
    ${SOURCE_DIR}/GMFerea.cpp
    ${SOURCE_DIR}/GMHellas.cpp
    ${SOURCE_DIR}/GMHuntingGround.cpp
    ${SOURCE_DIR}/GMKarutan1.cpp
    ${SOURCE_DIR}/GM_kanturu_1st.cpp
    ${SOURCE_DIR}/GM_Kanturu_2nd.cpp
    ${SOURCE_DIR}/GM_Kanturu_3rd.cpp
    ${SOURCE_DIR}/GM3rdChangeUp.cpp
    ${SOURCE_DIR}/GMNewTown.cpp
    ${SOURCE_DIR}/GMNixiesLake.cpp
    ${SOURCE_DIR}/GMSantaTown.cpp
    ${SOURCE_DIR}/GMSwampOfQuiet.cpp
    ${SOURCE_DIR}/GMUnitedMarketPlace.cpp
    ${SOURCE_DIR}/GMUrukMountain.cpp
    ${SOURCE_DIR}/GMZoneNars.cpp
    ${SOURCE_DIR}/GM_PK_Field.cpp
    ${SOURCE_DIR}/GM_Raklion.cpp
    ${SOURCE_DIR}/GOBoid.cpp
    ${SOURCE_DIR}/CKANTURUDirection.cpp
    ${SOURCE_DIR}/CMVP1stDirection.cpp
    ${SOURCE_DIR}/CDirection.cpp
    ${SOURCE_DIR}/w_MapProcess.cpp
    ${SOURCE_DIR}/CSMapServer.cpp
    ${SOURCE_DIR}/CSWaterTerrain.cpp
    ${SOURCE_DIR}/MapManager.cpp
    ${SOURCE_DIR}/TestEventMap.cpp

    # ── Networking / protocol ────────────────────────────────────────────────
    ${SOURCE_DIR}/Protocol.cpp
    ${SOURCE_DIR}/ProtocolSend.cpp
    ${SOURCE_DIR}/SocketSystem.cpp
    ${SOURCE_DIR}/WSclient.cpp
    ${SOURCE_DIR}/wsctlc.cpp
    ${SOURCE_DIR}/ConnectVersionHex.cpp
    ${SOURCE_DIR}/ServerGroup.cpp
    ${SOURCE_DIR}/ServerInfo.cpp
    ${SOURCE_DIR}/ServerListManager.cpp
    ${SOURCE_DIR}/KeyGenerater.cpp
    ${SOURCE_DIR}/ProtectSysKey.cpp
    ${SOURCE_DIR}/steady_clock.cpp

    # ── Items ────────────────────────────────────────────────────────────────
    ${SOURCE_DIR}/ItemInfo.cpp
    ${SOURCE_DIR}/ItemManager.cpp
    ${SOURCE_DIR}/ItemTooltip.cpp
    ${SOURCE_DIR}/ItemAddOptioninfo.cpp
    ${SOURCE_DIR}/CSItemOption.cpp
    ${SOURCE_DIR}/CSParts.cpp
    ${SOURCE_DIR}/ChangeRingManager.cpp
    ${SOURCE_DIR}/CComGem.cpp
    ${SOURCE_DIR}/GambleSystem.cpp
    ${SOURCE_DIR}/MixMgr.cpp
    ${SOURCE_DIR}/zzzMixInvetory.cpp
    ${SOURCE_DIR}/ScriptItem.cpp
    ${SOURCE_DIR}/TextureScript.cpp
    ${SOURCE_DIR}/CGMItemDropName.cpp

    # ── UI — legacy ──────────────────────────────────────────────────────────
    ${SOURCE_DIR}/UIMng.cpp
    ${SOURCE_DIR}/Input.cpp
    ${SOURCE_DIR}/UIManager.cpp
    ${SOURCE_DIR}/UIWindows.cpp
    ${SOURCE_DIR}/UIControls.cpp
    ${SOURCE_DIR}/UIControlRender.cpp
    ${SOURCE_DIR}/UIDefaultBase.cpp
    ${SOURCE_DIR}/UIMapName.cpp
    ${SOURCE_DIR}/UIPopup.cpp
    ${SOURCE_DIR}/Win.cpp
    ${SOURCE_DIR}/WinEx.cpp
    ${SOURCE_DIR}/BlackWin.cpp
    ${SOURCE_DIR}/Button.cpp
    ${SOURCE_DIR}/Slider.cpp
    ${SOURCE_DIR}/CUIListBox.cpp
    ${SOURCE_DIR}/GaugeBar.cpp
    ${SOURCE_DIR}/CBDrawInterface.cpp
    ${SOURCE_DIR}/CBInterface.cpp

    # ── UI — new system ──────────────────────────────────────────────────────
    ${SOURCE_DIR}/NewUIManager.cpp
    ${SOURCE_DIR}/NewUISystem.cpp
    ${SOURCE_DIR}/NewUI3DRenderMng.cpp
    ${SOURCE_DIR}/NewUICommon.cpp
    ${SOURCE_DIR}/NewUIButton.cpp
    ${SOURCE_DIR}/NewUIScrollBar.cpp
    ${SOURCE_DIR}/NewUITextBox.cpp
    ${SOURCE_DIR}/NewUIMessageBox.cpp
    ${SOURCE_DIR}/NewUICommonMessageBox.cpp
    ${SOURCE_DIR}/NewUICustomMessageBox.cpp
    ${SOURCE_DIR}/NewUIMainFrameWindow.cpp
    ${SOURCE_DIR}/NewUICharacterInfoWindow.cpp
    ${SOURCE_DIR}/NewUIMyInventory.cpp
    ${SOURCE_DIR}/NewUIInventoryCore.cpp
    ${SOURCE_DIR}/NewUIInventoryCtrl.cpp
    ${SOURCE_DIR}/NewUIInventoryJewel.cpp
    ${SOURCE_DIR}/NewUIInvenExpansion.cpp
    ${SOURCE_DIR}/NewUIOtherInventory.cpp
    ${SOURCE_DIR}/NewUIStorageInventory.cpp
    ${SOURCE_DIR}/NewUIStorageExpansion.cpp
    ${SOURCE_DIR}/NewUIMixInventory.cpp
    ${SOURCE_DIR}/NewUIMixExpansion.cpp
    ${SOURCE_DIR}/NewUIMyShopInventory.cpp
    ${SOURCE_DIR}/NewUIPurchaseShopInventory.cpp
    ${SOURCE_DIR}/NewUINPCShop.cpp
    ${SOURCE_DIR}/NewUITrade.cpp
    ${SOURCE_DIR}/NewUIChatLogWindow.cpp
    ${SOURCE_DIR}/NewUIChatInputBox.cpp
    ${SOURCE_DIR}/NewUIFriendWindow.cpp
    ${SOURCE_DIR}/NewUIPartyInfoWindow.cpp
    ${SOURCE_DIR}/NewUIPartyListWindow.cpp
    ${SOURCE_DIR}/NewUIGuildInfoWindow.cpp
    ${SOURCE_DIR}/NewUIGuildMakeWindow.cpp
    ${SOURCE_DIR}/NewUIHelpWindow.cpp
    ${SOURCE_DIR}/NewUIHotKey.cpp
    ${SOURCE_DIR}/NewUIOptionWindow.cpp
    ${SOURCE_DIR}/NewUIMiniMap.cpp
    ${SOURCE_DIR}/NewUIBuffWindow.cpp
    ${SOURCE_DIR}/NewUIHeroPositionInfo.cpp
    ${SOURCE_DIR}/NewUIRenderNumber.cpp
    ${SOURCE_DIR}/NewUIGroup.cpp
    ${SOURCE_DIR}/NewUIWindowMenu.cpp
    ${SOURCE_DIR}/NewUIMenuUser.cpp
    ${SOURCE_DIR}/NewUISlideWindow.cpp
    ${SOURCE_DIR}/NewUIListGroupButton.cpp
    ${SOURCE_DIR}/NewUIItemMng.cpp
    ${SOURCE_DIR}/NewUIItemTooltip.cpp
    ${SOURCE_DIR}/NewUIItemExplanationWindow.cpp
    ${SOURCE_DIR}/NewUIItemEnduranceInfo.cpp
    ${SOURCE_DIR}/NewUISetItemExplanation.cpp
    ${SOURCE_DIR}/NewUIMasterSkillTree.cpp
    ${SOURCE_DIR}/NewUICommandWindow.cpp
    ${SOURCE_DIR}/NewUICommandFilter.cpp
    ${SOURCE_DIR}/NewUIQuickCommandWindow.cpp
    ${SOURCE_DIR}/NewUIMoveCommandWindow.cpp
    ${SOURCE_DIR}/NewUIQuestProgress.cpp
    ${SOURCE_DIR}/NewUIQuestProgressByEtc.cpp
    ${SOURCE_DIR}/NewUIMyQuestInfoWindow.cpp
    ${SOURCE_DIR}/NewUINPCQuest.cpp
    ${SOURCE_DIR}/NewUINPCDialogue.cpp
    ${SOURCE_DIR}/NewUIPetInfoWindow.cpp
    ${SOURCE_DIR}/NewUINameWindow.cpp
    ${SOURCE_DIR}/NewUIEventTimeView.cpp
    ${SOURCE_DIR}/NewUIGatemanWindow.cpp
    ${SOURCE_DIR}/NewUIGateSwitchWindow.cpp
    ${SOURCE_DIR}/NewUIGuardWindow.cpp
    ${SOURCE_DIR}/NewUIGensRanking.cpp
    ${SOURCE_DIR}/NewUIRankingTop.cpp
    ${SOURCE_DIR}/NewUIReconnect.cpp
    ${SOURCE_DIR}/NewUIDuelWindow.cpp
    ${SOURCE_DIR}/NewUIDuelWatchWindow.cpp
    ${SOURCE_DIR}/NewUIDuelWatchMainFrameWindow.cpp
    ${SOURCE_DIR}/NewUIDuelWatchUserListWindow.cpp
    ${SOURCE_DIR}/NewUICryWolf.cpp
    ${SOURCE_DIR}/NewUIKanturuEvent.cpp
    ${SOURCE_DIR}/NewUIBloodCastleEnter.cpp
    ${SOURCE_DIR}/NewUIBloodCastleTime.cpp
    ${SOURCE_DIR}/NewUIChaosCastleTime.cpp
    ${SOURCE_DIR}/NewUIDevilSquareEnter.cpp
    ${SOURCE_DIR}/NewUICursedTempleEnter.cpp
    ${SOURCE_DIR}/NewUICursedTempleResult.cpp
    ${SOURCE_DIR}/NewUICursedTempleSystem.cpp
    ${SOURCE_DIR}/NewUIDoppelGangerFrame.cpp
    ${SOURCE_DIR}/NewUIDoppelGangerWindow.cpp
    ${SOURCE_DIR}/NewUIBattleSoccerScore.cpp
    ${SOURCE_DIR}/NewUIEmpireGuardianNPC.cpp
    ${SOURCE_DIR}/NewUIEmpireGuardianTimer.cpp
    ${SOURCE_DIR}/NewUICastleWindow.cpp
    ${SOURCE_DIR}/NewUICatapultWindow.cpp
    ${SOURCE_DIR}/NewUISeigeWarfare.cpp
    ${SOURCE_DIR}/NewUISiegeWarBase.cpp
    ${SOURCE_DIR}/NewUISiegeWarCommander.cpp
    ${SOURCE_DIR}/NewUISiegeWarObserver.cpp
    ${SOURCE_DIR}/NewUISiegeWarSoldier.cpp
    ${SOURCE_DIR}/NewUIGoldBowmanWindow.cpp
    ${SOURCE_DIR}/NewUIGoldBowmanLena.cpp
    ${SOURCE_DIR}/NewUIUnitedMarketPlaceWindow.cpp
    ${SOURCE_DIR}/NewUIExchangeLuckyCoin.cpp
    ${SOURCE_DIR}/NewUIRegistrationLuckyCoin.cpp
    ${SOURCE_DIR}/NewUILuckyItemWnd.cpp
    ${SOURCE_DIR}/NewUIMacroMain.cpp
    ${SOURCE_DIR}/NewUIMacroSub.cpp
    ${SOURCE_DIR}/NewUIMacroGaugeBar.cpp
    ${SOURCE_DIR}/NewUIPurcharseVip.cpp

    # ── Login / char select / server select ──────────────────────────────────
    ${SOURCE_DIR}/LoginWin.cpp
    ${SOURCE_DIR}/LoginMainWin.cpp
    ${SOURCE_DIR}/ServerSelWin.cpp
    ${SOURCE_DIR}/ServerMsgWin.cpp
    ${SOURCE_DIR}/CharSelMainWin.cpp
    ${SOURCE_DIR}/CharMakeWin.cpp
    ${SOURCE_DIR}/CharInfoBalloon.cpp
    ${SOURCE_DIR}/CharInfoBalloonMng.cpp
    ${SOURCE_DIR}/CreditWin.cpp
    ${SOURCE_DIR}/CreditInfo.cpp

    # ── Guild / Party / Social ───────────────────────────────────────────────
    ${SOURCE_DIR}/GuildCache.cpp
    ${SOURCE_DIR}/GuildManager.cpp
    ${SOURCE_DIR}/UIGuildInfo.cpp
    ${SOURCE_DIR}/UIGuildMaster.cpp
    ${SOURCE_DIR}/PartyManager.cpp
    ${SOURCE_DIR}/DuelMgr.cpp

    # ── Quest / Events ───────────────────────────────────────────────────────
    ${SOURCE_DIR}/QuestInfo.cpp
    ${SOURCE_DIR}/QuestMng.cpp
    ${SOURCE_DIR}/CSQuest.cpp
    ${SOURCE_DIR}/Event.cpp
    ${SOURCE_DIR}/MatchEvent.cpp
    ${SOURCE_DIR}/CustomEventTime.cpp
    ${SOURCE_DIR}/CSChaosCastle.cpp
    ${SOURCE_DIR}/CSEventMatch.cpp
    ${SOURCE_DIR}/NewBloodCastleSystem.cpp
    ${SOURCE_DIR}/NewChaosCastleSystem.cpp
    ${SOURCE_DIR}/GlobalPortalSystem.cpp

    # ── NPC / Shops ──────────────────────────────────────────────────────────
    ${SOURCE_DIR}/npcBreeder.cpp
    ${SOURCE_DIR}/npcCatapult.cpp
    ${SOURCE_DIR}/npcGateSwitch.cpp
    ${SOURCE_DIR}/NewGatemanWindow.cpp
    ${SOURCE_DIR}/UIGateKeeper.cpp
    ${SOURCE_DIR}/UIGuardsMan.cpp
    ${SOURCE_DIR}/UISenatus.cpp
    ${SOURCE_DIR}/UIJewelHarmony.cpp
    ${SOURCE_DIR}/DialogInfo.cpp
    ${SOURCE_DIR}/PersonalShopTitleImp.cpp
    ${SOURCE_DIR}/GCCertification.cpp
    ${SOURCE_DIR}/SysMenuWin.cpp
    ${SOURCE_DIR}/OptionWin.cpp

    # ── Pet system ───────────────────────────────────────────────────────────
    ${SOURCE_DIR}/CSPetSystem.cpp
    ${SOURCE_DIR}/GIPetManager.cpp
    ${SOURCE_DIR}/w_BasePet.cpp
    ${SOURCE_DIR}/w_PetProcess.cpp
    ${SOURCE_DIR}/w_PetActionCollecter.cpp
    ${SOURCE_DIR}/w_PetActionCollecter_Add.cpp
    ${SOURCE_DIR}/w_PetActionDemon.cpp
    ${SOURCE_DIR}/w_PetActionMuun.cpp
    ${SOURCE_DIR}/w_PetActionRound.cpp
    ${SOURCE_DIR}/w_PetActionStand.cpp
    ${SOURCE_DIR}/w_PetActionUnicorn.cpp

    # ── Buffs / Status ───────────────────────────────────────────────────────
    ${SOURCE_DIR}/w_Buff.cpp
    ${SOURCE_DIR}/w_BuffScriptLoader.cpp
    ${SOURCE_DIR}/w_BuffStateSystem.cpp
    ${SOURCE_DIR}/w_BuffStateValueControl.cpp
    ${SOURCE_DIR}/w_BuffTimeControl.cpp
    ${SOURCE_DIR}/_GlobalFunctions.cpp

    # ── Character info / world objects ───────────────────────────────────────
    ${SOURCE_DIR}/w_CharacterInfo.cpp
    ${SOURCE_DIR}/w_ObjectInfo.cpp
    ${SOURCE_DIR}/w_CursedTemple.cpp

    # ── Skills ───────────────────────────────────────────────────────────────
    ${SOURCE_DIR}/SkillEffectMgr.cpp
    ${SOURCE_DIR}/SkillInfo.cpp
    ${SOURCE_DIR}/SkillManager.cpp
    ${SOURCE_DIR}/SummonSystem.cpp
    ${SOURCE_DIR}/MonkSystem.cpp

    # ── Sound ────────────────────────────────────────────────────────────────
    #${SOURCE_DIR}/DSplaysound.cpp  # Windows DirectSound - replaced by AudioOpenSLES.cpp
    #${SOURCE_DIR}/DSwaveIO.cpp   # Windows Wave API - no longer needed

    # ── UI misc ──────────────────────────────────────────────────────────────
    ${SOURCE_DIR}/MsgWin.cpp
    ${SOURCE_DIR}/PList.cpp
    ${SOURCE_DIR}/SlideInfo.cpp
    ${SOURCE_DIR}/ExplanationInfo.cpp
    ${SOURCE_DIR}/FilterInfo.cpp
    ${SOURCE_DIR}/FilterNameInfo.cpp
    ${SOURCE_DIR}/SideHair.cpp
    ${SOURCE_DIR}/Builder.cpp
    ${SOURCE_DIR}/ScaleForm.cpp
    ${SOURCE_DIR}/Resolutions.cpp
    ${SOURCE_DIR}/GameCensorship.cpp
    ${SOURCE_DIR}/CB_DangKyInGame.cpp
    ${SOURCE_DIR}/CB_GetMixRate.cpp
    ${SOURCE_DIR}/MultiLanguage.cpp
    ${SOURCE_DIR}/TextClien.cpp
    ${SOURCE_DIR}/Local.cpp

    # ── Font / Text ──────────────────────────────────────────────────────────
    ${SOURCE_DIR}/CGMFrame.cpp
    ${SOURCE_DIR}/CGMHeadChat.cpp

    # ── Misc game systems ────────────────────────────────────────────────────
    ${SOURCE_DIR}/PathFinding.cpp
    ${SOURCE_DIR}/PortalMgr.cpp
    ${SOURCE_DIR}/Observer.cpp
    ${SOURCE_DIR}/MoveCommandData.cpp
    ${SOURCE_DIR}/MovereqInfo.cpp
    ${SOURCE_DIR}/CSResourceManager.cpp
    ${SOURCE_DIR}/jpexs.cpp
    ${SOURCE_DIR}/supportingfeature.cpp
    ${SOURCE_DIR}/GlobalBitmap.cpp
    ${SOURCE_DIR}/pugixml.cpp

    # ── Protection / Config ──────────────────────────────────────────────────
    ${SOURCE_DIR}/CGMProtect.cpp

    # ── Utilities ────────────────────────────────────────────────────────────
    ${SOURCE_DIR}/Util.cpp
    ${SOURCE_DIR}/UsefulDef.cpp
    ${SOURCE_DIR}/Time/Timer.cpp
    ${SOURCE_DIR}/Time/CTimCheck.cpp
    ${SOURCE_DIR}/Utilities/Log/ErrorReport.cpp
    ${SOURCE_DIR}/Utilities/Log/DebugAngel.cpp
    ${SOURCE_DIR}/Utilities/Log/muConsoleDebug.cpp
    ${SOURCE_DIR}/Utilities/Log/WindowsConsole.cpp
    ${SOURCE_DIR}/Utilities/Memory/MemoryLock.cpp

    # ── Leaf (exception/crash Windows-only modules excluded on Android) ───────
    ${SOURCE_DIR}/ExternalObject/Leaf/xortrans.cpp
    ${SOURCE_DIR}/ExternalObject/Leaf/xstreambuf.cpp

    # ── GameShop ────────────────────────────────────────────────────────────
    ${SOURCE_DIR}/GameShop/InGameShopSystem.cpp
    ${SOURCE_DIR}/GameShop/NewUIInGameShop.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSBuyConfirm.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSBuyPackageItem.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSBuySelectItem.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSCommon.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSDeleteItemConfirm.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSGiftStorageItemInfo.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSSendGift.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSSendGiftConfirm.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSStorageItemInfo.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSUseBuffConfirm.cpp
    ${SOURCE_DIR}/GameShop/MsgBoxIGSUseItemConfirm.cpp
    ${SOURCE_DIR}/GameShop/FileDownloader/DownloadInfo.cpp
    ${SOURCE_DIR}/GameShop/FileDownloader/FTPConnecter.cpp
    ${SOURCE_DIR}/GameShop/FileDownloader/FileDownloader.cpp
    ${SOURCE_DIR}/GameShop/FileDownloader/HTTPConnecter.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/BannerInfo.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/BannerInfoList.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/BannerListManager.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/FTPFileDownLoader.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ListManager.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopCategory.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopCategoryList.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopList.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopListManager.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopPackage.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopPackageList.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopProduct.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/ShopProductList.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/StringMethod.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/StringToken.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/interface/PathMethod/Path.cpp
    ${SOURCE_DIR}/GameShop/ShopListManager/interface/WZResult/WZResult.cpp
)

# ─────────────────────────────────────────────────────────────────────────────
# Shared library target
# ─────────────────────────────────────────────────────────────────────────────
add_library(mucrossengine SHARED
    ${GAME_SOURCES}
    ${PLATFORM_SOURCES}
)

target_compile_options(mucrossengine PRIVATE
    -Wno-error=format-security
    -Wno-c++11-narrowing
    -w
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-tautological-compare
    -Wno-reorder
)

# ─────────────────────────────────────────────────────────────────────────────
# Link
# ─────────────────────────────────────────────────────────────────────────────
target_link_libraries(mucrossengine
    ${log-lib}
    ${android-lib}
    ${egl-lib}
    ${gles3-lib}
    ${opensl-lib}
    ${dl-lib}
    ${z-lib}
    # TODO: add freetype, lua54, cryptopp when source is added
)
