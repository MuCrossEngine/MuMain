#ifndef _MIX_MGR_H_
#define _MIX_MGR_H_
#pragma once
#define MAX_MIX_SOURCES						8
#define MAX_MIX_NAMES						3
#define MAX_MIX_DESCRIPTIONS				3
#define MAX_MIX_RATE_TOKEN					32

namespace SEASON3A
{
	enum eChaosMixNumber
	{
		CHAOS_MIX_CHAOS_ITEM = 1,
		CHAOS_MIX_DEVIL_SQUARE = 2,
		CHAOS_MIX_PLUS_ITEM_LEVEL1 = 3,
		CHAOS_MIX_PLUS_ITEM_LEVEL2 = 4,
		CHAOS_MIX_DINORANT = 5,
		CHAOS_MIX_FRUIT = 6,
		CHAOS_MIX_WING1 = 7,
		CHAOS_MIX_BLOOD_CASTLE = 8,
		CHAOS_MIX_WING2 = 11,
		CHAOS_MIX_PET1 = 13,
		CHAOS_MIX_PET2 = 14,
		CHAOS_MIX_SIEGE_POTION1 = 15,
		CHAOS_MIX_SIEGE_POTION2 = 16,
		CHAOS_MIX_LIFE_STONE = 17,
		CHAOS_MIX_SENIOR = 18,
		CHAOS_MIX_PLUS_ITEM_LEVEL3 = 22,
		CHAOS_MIX_PLUS_ITEM_LEVEL4 = 23,
		CHAOS_MIX_WING3 = 24,
		CHAOS_MIX_PIECE_OF_HORN = 25,
		CHAOS_MIX_BROKEN_HORN = 26,
		CHAOS_MIX_HORN_OF_FENRIR = 27,
		CHAOS_MIX_HORN_OF_FENRIR_UPGRADE = 28,
		CHAOS_MIX_SHIELD_POTION1 = 30,
		CHAOS_MIX_SHIELD_POTION2 = 31,
		CHAOS_MIX_SHIELD_POTION3 = 32,
		CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_PURITY = 33,
		CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_SMELT = 34,
		CHAOS_MIX_JEWEL_OF_HARMONY_ITEM_RESTORE = 35,
		CHAOS_MIX_ITEM_380 = 36,
		CHAOS_MIX_ILLUSION_TEMPLE = 37,
		CHAOS_MIX_FEATHER_OF_CONDOR = 38,
		CHAOS_MIX_WING4 = 39,
		CHAOS_MIX_CHAOS_CARD = 40,
		CHAOS_MIX_CHERRY_BLOSSOM = 41,
		CHAOS_MIX_SOCKET_ITEM_CREATE_SEED = 42,
		CHAOS_MIX_SOCKET_ITEM_CREATE_SEED_SPHERE = 43,
		CHAOS_MIX_SOCKET_ITEM_MOUNT_SEED_SPHERE = 44,
		CHAOS_MIX_SOCKET_ITEM_UN_MOUNT_SEED_SPHERE = 45,
		CHAOS_MIX_IMPERIAL_GUARDIAN = 46,
		CHAOS_MIX_CHEST = 47,
		CHAOS_MIX_SUMMON_SCROLL = 48,
		CHAOS_MIX_PLUS_ITEM_LEVEL5 = 49,
		CHAOS_MIX_PLUS_ITEM_LEVEL6 = 50,
		CHAOS_MIX_LUCKY_ITEM_CREATE = 51,
		CHAOS_MIX_LUCKY_ITEM_REFINE = 52,
		CHAOS_MIX_MONSTER_WING = 56,
		CHAOS_MIX_SOCKET_WEAPON = 57,
	};

	class CMixItem
	{
	public:
		CMixItem() { Reset(); }
		virtual ~CMixItem() { }
		CMixItem(ITEM * pItem, int iMixValue) { SetItem(pItem, iMixValue); }
		
		void Reset();
		void SetItem(ITEM * pItem, DWORD dwMixValue);

		bool IsSameItem(const CMixItem & rhs) const
		{
			return (m_sType == rhs.m_sType && m_iLevel == rhs.m_iLevel &&
				(m_bCanStack || m_iDurability == rhs.m_iDurability) && m_iOption == rhs.m_iOption &&
				m_dwSpecialItem == rhs.m_dwSpecialItem);
		}
		
		bool operator==(ITEM * rhs)
		{
			return IsSameItem(CMixItem(rhs, 0));
		}

		short m_sType;
		int m_iLevel;
		int m_iOption;
		int m_iDurability;
		DWORD m_dwSpecialItem;
		int m_iCount;
		int m_iTestCount;
		BOOL m_bMixLuck;
		BOOL m_bIsEquipment;
		BOOL m_bCanStack;
		BOOL m_bIsWing;
		BOOL m_bIsUpgradedWing;
		BOOL m_bIs3rdUpgradedWing;
		DWORD m_dwMixValue;
		BOOL m_bIsCharmItem;
		BOOL m_bIsChaosCharmItem;
		BOOL m_bIsJewelItem;
		BOOL m_b380AddedItem;
		BOOL m_bFenrirAddedItem;
		WORD m_wHarmonyOption;
		WORD m_wHarmonyOptionLevel;
		BYTE m_bySocketCount;
		BYTE m_bySocketSeedID[MAX_SOCKETS];
		BYTE m_bySocketSphereLv[MAX_SOCKETS];
		BYTE m_bySeedSphereID;
	};

	class CMixItemInventory
	{
	public:
		CMixItemInventory() { Reset(); }
		virtual ~CMixItemInventory() { Reset(); }
		void Reset() { m_iNumMixItems = 0; }
		int AddItem(ITEM * pItem);
		int GetNumMixItems() { return m_iNumMixItems; }
		CMixItem * GetMixItems() { return m_MixItems; }
	protected:
		DWORD EvaluateMixItemValue(ITEM * pItem);
	protected:
		CMixItem m_MixItems[32];
		int m_iNumMixItems;
	};

	enum _SPECIAL_ITEM_RECIPE_
	{
		RCP_SP_EXCELLENT = 1,
		RCP_SP_ADD380ITEM = 2,
		RCP_SP_SETITEM = 4,
		RCP_SP_HARMONY = 8,
		RCP_SP_SOCKETITEM = 16,
	};

	typedef struct _MIX_RECIPE_ITEM
	{
		short m_sTypeMin;
		short m_sTypeMax;
		int m_iLevelMin;
		int m_iLevelMax;
		int m_iOptionMin;
		int m_iOptionMax;
		int m_iDurabilityMin;
		int m_iDurabilityMax;
		int m_iCountMin;
		int m_iCountMax;
		DWORD m_dwSpecialItem;
	} MIX_RECIPE_ITEM;

	enum _MIX_TYPES_
	{
		MIXTYPE_GOBLIN_NORMAL,
		MIXTYPE_GOBLIN_CHAOSITEM,
		MIXTYPE_GOBLIN_ADD380,
		MIXTYPE_CASTLE_SENIOR,
		MIXTYPE_TRAINER,
		MIXTYPE_OSBOURNE,
		MIXTYPE_JERRIDON,
		MIXTYPE_ELPIS,
		MIXTYPE_CHAOS_CARD,
		MIXTYPE_CHERRYBLOSSOM,
		MIXTYPE_EXTRACT_SEED,
		MIXTYPE_SEED_SPHERE,
		MIXTYPE_ATTACH_SOCKET,
		MIXTYPE_DETACH_SOCKET,
		MAX_MIX_TYPES
	};

	enum _MIXRATE_OPS
	{
		MRCP_NUMBER = 0,
		MRCP_ADD,	// +
		MRCP_SUB,	// -
		MRCP_MUL,	// *
		MRCP_DIV,	// /
		MRCP_LP,	// (
		MRCP_RP,	// )
		MRCP_INT,	// INT()
		MRCP_MAXRATE = 32,
		MRCP_ITEM,
		MRCP_WING,
		MRCP_EXCELLENT,
		MRCP_EQUIP,
		MRCP_SET,
		MRCP_LEVEL1,
		MRCP_NONJEWELITEM,
		MRCP_LUCKOPT = 64
	};

	typedef struct _MIXRATE_TOKEN
	{
		enum _MIXRATE_OPS op;
		float value;
	} MIXRATE_TOKEN;

	typedef struct _MIX_RECIPE
	{
		int m_iMixIndex;
		int m_iMixID;
		int m_iMixName[MAX_MIX_NAMES];
		int m_iMixDesc[MAX_MIX_DESCRIPTIONS];
		int m_iMixAdvice[MAX_MIX_DESCRIPTIONS];
		int m_iWidth;
		int m_iHeight;
		int m_iRequiredLevel;
		BYTE m_bRequiredZenType;
		DWORD m_dwRequiredZen;
		int m_iNumRateData;
		MIXRATE_TOKEN m_RateToken[MAX_MIX_RATE_TOKEN];
		int m_iSuccessRate;
		BYTE m_bMixOption;
		BYTE m_bCharmOption;
		BYTE m_bChaosCharmOption;
		MIX_RECIPE_ITEM m_MixSources[MAX_MIX_SOURCES];
		int m_iNumMixSources;
	} MIX_RECIPE;

	enum _MIX_SOURCE_STATUS { MIX_SOURCE_ERROR, MIX_SOURCE_NO, MIX_SOURCE_PARTIALLY, MIX_SOURCE_YES };

	class CMixRecipes
	{
	public:
		CMixRecipes() { Reset(); }
		virtual ~CMixRecipes() { Reset(); }

		void Reset();
		void AddRecipe(MIX_RECIPE * pMixRecipe);
		void ClearCheckRecipeResult();
		int CheckRecipe(int iNumMixItems, CMixItem * pMixItems);
		int CheckRecipeSimilarity(int iNumMixItems, CMixItem * pMixItems);

		BOOL IsMixSource(ITEM * pItem);
		MIX_RECIPE * GetCurRecipe();
		int GetCurMixID();

		BOOL GetCurRecipeName(unicode::t_char * pszNameOut, int iNameLine);
		BOOL GetCurRecipeDesc(unicode::t_char * pszDescOut, int iDescLine);
		MIX_RECIPE * GetMostSimilarRecipe();
		BOOL GetMostSimilarRecipeName(unicode::t_char * pszNameOut, int iNameLine);
		BOOL GetRecipeAdvice(unicode::t_char * pszAdviceOut, int iAdivceLine);
		int GetSourceName(int iItemNum, unicode::t_char * pszNameOut, int iNumMixItems, CMixItem * pMixItems);
		BOOL IsReadyToMix() { return (m_iCurMixIndex > 0); }
		int GetSuccessRate() { return m_iSuccessRate; }
		DWORD GetReqiredZen() { return m_dwRequiredZen; }
		int GetFirstItemSocketCount() { return m_byFirstItemSocketCount; }
		int GetFirstItemSocketSeedID(int iIndex) { if (iIndex >= m_byFirstItemSocketCount) return SOCKET_EMPTY; else return m_byFirstItemSocketSeedID[iIndex]; }
		int GetFirstItemSocketShpereLv(int iIndex) { if (iIndex >= m_byFirstItemSocketCount) return 0; else return m_byFirstItemSocketSphereLv[iIndex]; }
		void CalcCharmBonusRate(int iNumMixItems, CMixItem * pMixItems);
		void CalcChaosCharmCount(int iNumMixItems, CMixItem * pMixItems);

#ifdef LJH_MOD_CANNOT_USE_CHARMITEM_AND_CHAOSCHARMITEM_SIMULTANEOUSLY
		WORD GetTotalChaosCharmCount()	{ return m_wTotalChaosCharmCount; }
		WORD GetTotalCharmCount()		{ return m_wTotalCharmBonus; }
#endif //LJH_MOD_CANNOT_USE_CHARMITEM_AND_CHAOSCHARMITEM_SIMULTANEOUSLY
#if(CB_GETMIXRATE)
		void SetRealMixRate(int Rate) { this->m_iSuccessRate = Rate; };
		void SetRealMixZen(int Zen) { this->m_dwRequiredZen = Zen; };
#endif
	protected:
		bool IsOptionItem(MIX_RECIPE_ITEM & rItem) { return (rItem.m_iCountMin == 0); }	// 옵션(안넣어도 되는) 아이템인가
		BOOL CheckRecipeSub(std::vector<MIX_RECIPE *>::iterator iter, int iNumMixItems, CMixItem * pMixItems);
		int CheckRecipeSimilaritySub(std::vector<MIX_RECIPE *>::iterator iter, int iNumMixItems, CMixItem * pMixItems);	// 유사도 비교
		bool CheckItem(MIX_RECIPE_ITEM & rItem, CMixItem & rSource);	// 같은 아이템인지 비교
		void EvaluateMixItems(int iNumMixItems, CMixItem * pMixItems);
		void CalcMixRate(int iNumMixItems, CMixItem * pMixItems);
		void CalcMixReqZen(int iNumMixItems, CMixItem * pMixItems);
		BOOL GetRecipeName(MIX_RECIPE * pRecipe, unicode::t_char * pszNameOut, int iNameLine, BOOL bSimilarRecipe);	// 주어진 조합법의 이름 얻기
		BOOL IsChaosItem(CMixItem & rSource);
		BOOL IsChaosJewel(CMixItem & rSource);
		BOOL Is380AddedItem(CMixItem & rSource);
		BOOL IsFenrirAddedItem(CMixItem & rSource);
		BOOL IsUpgradableItem(CMixItem & rSource);
		BOOL IsSourceOfRefiningStone(CMixItem & rSource);
		BOOL IsCharmItem(CMixItem & rSource);
		BOOL IsChaosCharmItem(CMixItem & rSource);
		BOOL IsJewelItem(CMixItem & rSource);
		BOOL IsSourceOfAttachSeedSphereToWeapon(CMixItem & rSource);
		BOOL IsSourceOfAttachSeedSphereToArmor(CMixItem & rSource);

		float MixrateAddSub();
		float MixrateMulDiv();
		float MixrateFactor();
	protected:
		std::vector<MIX_RECIPE *> m_Recipes;
		int m_iCurMixIndex;
		int m_iMostSimilarMixIndex;
		int m_iSuccessRate;
		DWORD m_dwRequiredZen;
		BOOL m_bFindMixLuckItem;
		DWORD m_dwTotalItemValue;
		DWORD m_dwExcellentItemValue;
		DWORD m_dwEquipmentItemValue;
		DWORD m_dwWingItemValue;
		DWORD m_dwSetItemValue;
		DWORD m_iFirstItemLevel;
		int m_iFirstItemType;
		DWORD m_dwTotalNonJewelItemValue;
		BYTE m_byFirstItemSocketCount;
		BYTE m_byFirstItemSocketSeedID[MAX_SOCKETS];
		BYTE m_byFirstItemSocketSphereLv[MAX_SOCKETS];
		WORD m_wTotalCharmBonus;
		WORD m_wTotalChaosCharmCount;
		int m_iMixSourceTest[MAX_MIX_SOURCES];
		int m_iMostSimilarMixSourceTest[MAX_MIX_SOURCES];
		int m_iMixRateIter;
		MIXRATE_TOKEN * m_pMixRates;
	};

	class CMixRecipeMgr
	{
	public:
		CMixRecipeMgr():m_iMixType(0),m_bIsMixInit(TRUE)
		{
			m_iMixSubType = 0;
			m_btPlusChaosRate = 0;
			//OpenRecipeFile("Data\\Local\\Mix.bmd");
		}
		virtual ~CMixRecipeMgr() {}

		void SetMixType(int iMixType) { this->m_iMixType = iMixType; }
		int GetMixInventoryType();
		int GetMixInventoryEquipmentIndex();
		void ResetMixItemInventory();
		void AddItemToMixItemInventory(ITEM * pItem);
		void CheckMixInventory();
		void SetMixSubType(int iMixSubType) { this->m_iMixSubType = iMixSubType; }
		int GetMixSubType() { return this->m_iMixSubType; }

		BOOL IsMixSource(ITEM * pItem)
		{
			m_MixRecipe[GetMixInventoryType()].CalcCharmBonusRate(m_MixItemInventory.GetNumMixItems(), m_MixItemInventory.GetMixItems());
			m_MixRecipe[GetMixInventoryType()].CalcChaosCharmCount(m_MixItemInventory.GetNumMixItems(), m_MixItemInventory.GetMixItems());
			return m_MixRecipe[GetMixInventoryType()].IsMixSource(pItem);
		}
		int CheckRecipe(int iNumMixItems, CMixItem * pMixItems)
		{
			return m_MixRecipe[GetMixInventoryType()].CheckRecipe(iNumMixItems, pMixItems);
		}
		int CheckRecipeSimilarity(int iNumMixItems, CMixItem * pMixItems)
		{
			return m_MixRecipe[GetMixInventoryType()].CheckRecipeSimilarity(iNumMixItems, pMixItems);
		}
		BOOL IsReadyToMix()
		{
			return m_MixRecipe[GetMixInventoryType()].IsReadyToMix();
		}
		MIX_RECIPE * GetCurRecipe()
		{
			return m_MixRecipe[GetMixInventoryType()].GetCurRecipe();
		}
		int GetSuccessRate()
		{
			return m_MixRecipe[GetMixInventoryType()].GetSuccessRate();
		}
		int GetReqiredZen()
		{
			return m_MixRecipe[GetMixInventoryType()].GetReqiredZen();
		}
		int GetCurMixID()
		{
			return m_MixRecipe[GetMixInventoryType()].GetCurMixID();
		}
#if(CB_GETMIXRATE)
		int RealRate;
		int RealZen;
		void SetRealMixRate(int Rate) { m_MixRecipe[GetMixInventoryType()].SetRealMixRate(Rate); };
		void SetRealMixZen(int Zen) { m_MixRecipe[GetMixInventoryType()].SetRealMixZen(Zen); };
#endif
		BOOL GetCurRecipeName(unicode::t_char * pszNameOut, int iNameLine)
		{
			return m_MixRecipe[GetMixInventoryType()].GetCurRecipeName(pszNameOut, iNameLine);
		}
		BOOL GetCurRecipeDesc(unicode::t_char * pszDescOut, int iDescLine)
		{
			return m_MixRecipe[GetMixInventoryType()].GetCurRecipeDesc(pszDescOut, iDescLine);
		}
		MIX_RECIPE * GetMostSimilarRecipe()
		{
			return m_MixRecipe[GetMixInventoryType()].GetMostSimilarRecipe();
		}
		BOOL GetMostSimilarRecipeName(unicode::t_char * pszNameOut, int iNameLine)
		{
			return m_MixRecipe[GetMixInventoryType()].GetMostSimilarRecipeName(pszNameOut, iNameLine);
		}
		BOOL GetRecipeAdvice(unicode::t_char * pszAdviceOut, int iAdivceLine)
		{
			return m_MixRecipe[GetMixInventoryType()].GetRecipeAdvice(pszAdviceOut, iAdivceLine);
		}
		int GetSourceName(int iItemNum, unicode::t_char * pszNameOut)
		{
			return m_MixRecipe[GetMixInventoryType()].GetSourceName(iItemNum, pszNameOut, m_MixItemInventory.GetNumMixItems(),
				m_MixItemInventory.GetMixItems());
		}
		void ClearCheckRecipeResult()
		{
			m_MixRecipe[GetMixInventoryType()].ClearCheckRecipeResult();
			SetMixType(0);
		}
		BOOL IsMixInit() { return m_bIsMixInit; }

		void SetPlusChaosRate(BYTE btPlusChaosRate) { m_btPlusChaosRate = btPlusChaosRate; }
		BYTE GetPlusChaosRate() { return m_btPlusChaosRate; }
		int GetFirstItemSocketCount() { return m_MixRecipe[GetMixInventoryType()].GetFirstItemSocketCount(); }
		int GetFirstItemSocketSeedID(int iIndex) { if (iIndex >= GetFirstItemSocketCount()) return SOCKET_EMPTY; else return m_MixRecipe[GetMixInventoryType()].GetFirstItemSocketSeedID(iIndex); }
		int GetFirstItemSocketShpereLv(int iIndex) { if (iIndex >= GetFirstItemSocketCount()) return 0; else return m_MixRecipe[GetMixInventoryType()].GetFirstItemSocketShpereLv(iIndex); }
		int GetSeedSphereID(int iOrder);

#ifdef LJH_MOD_CANNOT_USE_CHARMITEM_AND_CHAOSCHARMITEM_SIMULTANEOUSLY
		WORD GetTotalChaosCharmCount()
		{ 
			m_MixRecipe[GetMixInventoryType()].CalcChaosCharmCount(m_MixItemInventory.GetNumMixItems(), m_MixItemInventory.GetMixItems());
			return m_MixRecipe[GetMixInventoryType()].GetTotalChaosCharmCount(); 
		}
		WORD GetTotalCharmCount()
		{ 
			m_MixRecipe[GetMixInventoryType()].CalcCharmBonusRate(m_MixItemInventory.GetNumMixItems(), m_MixItemInventory.GetMixItems());
			return m_MixRecipe[GetMixInventoryType()].GetTotalCharmCount(); 
		}
#endif //LJH_MOD_CANNOT_USE_CHARMITEM_AND_CHAOSCHARMITEM_SIMULTANEOUSLY

		void OpenRecipeFile(const unicode::t_char* szFileName);	// mix.bmd

	protected:
		CMixRecipes m_MixRecipe[MAX_MIX_TYPES];
		CMixItemInventory m_MixItemInventory;
		int m_iMixType;
		int m_iMixSubType;
		BOOL m_bIsMixInit;
		BYTE m_btPlusChaosRate;
	};
}

extern SEASON3A::CMixRecipeMgr g_MixRecipeMgr;

#endif	// _MIX_MGR_H_
