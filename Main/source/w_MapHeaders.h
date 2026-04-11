// w_MapHeaders.h: interface for the BaseMap class.
//////////////////////////////////////////////////////////////////////
#pragma once
#include "w_MapProcess.h"
#include "MapManager.h"

template<typename T>
T& TheWorld( int type ) 
{
	//return static_cast<T&>(TheMapProcess().GetMap(type));
	return dynamic_cast<T&>(TheMapProcess().GetMap(type));
}

#include "GM_Raklion.h"
#include "GMSantaTown.h"
#include "GM_PK_Field.h"
#include "GMDuelArena.h"
#include "GMDoppelGanger1.h"
#include "GMDoppelGanger2.h"
#include "GMDoppelGanger3.h"
#include "GMDoppelGanger4.h"
#include "GMEmpireGuardian1.h"
#include "GMEmpireGuardian2.h"
#include "GMEmpireGuardian3.h"
#include "GMEmpireGuardian4.h"
#include "GMUnitedMarketPlace.h"
#include "GMKarutan1.h"
#include "GMCharacterScene.h"
#include "GMLogInScene.h"
#include "GMAcheron.h"
#include "GMDeventer.h"
#include "GMUrukMountain.h"
#include "GMZoneNars.h"
#include "GMFerea.h"
#include "GMNixiesLake.h"

#define g_Raklion	TheWorld<SEASON4A::CGM_Raklion>(WD_57ICECITY)
#define g_SantaTown TheWorld<CGMSantaTown>(WD_62SANTA_TOWN)
#define g_PKField TheWorld<CGM_PK_Field>(WD_63PK_FIELD)
#define g_DuelArena TheWorld<CGMDuelArena>(WD_64DUELARENA)
#define g_DoppelGanger1 TheWorld<CGMDoppelGanger1>(WD_65DOPPLEGANGER1)
#define g_DoppelGanger2 TheWorld<CGMDoppelGanger2>(WD_66DOPPLEGANGER2)
#define g_DoppelGanger3 TheWorld<CGMDoppelGanger3>(WD_67DOPPLEGANGER3)
#define g_DoppelGanger4 TheWorld<CGMDoppelGanger4>(WD_68DOPPLEGANGER4)
#define g_EmpireGuardian1 TheWorld<GMEmpireGuardian1>(WD_69EMPIREGUARDIAN1)
#define g_EmpireGuardian2 TheWorld<GMEmpireGuardian2>(WD_70EMPIREGUARDIAN2)
#define g_EmpireGuardian3 TheWorld<GMEmpireGuardian3>(WD_71EMPIREGUARDIAN3)
#define g_EmpireGuardian4 TheWorld<GMEmpireGuardian4>(WD_72EMPIREGUARDIAN4)
#define g_UnitedMarketPlace TheWorld<GMUnitedMarketPlace>(WD_79UNITEDMARKETPLACE)
#define g_Karutan1 TheWorld<CGMKarutan1>(WD_80KARUTAN1)
#define g_CharacterScene TheWorld<CGMCharacterScene>(WD_93NEW_CHARACTER_SCENE)
#define g_LogInScene TheWorld<CGMLogInScene>(WD_94NEW_LOGIN_SCENE)
