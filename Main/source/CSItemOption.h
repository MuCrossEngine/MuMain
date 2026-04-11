//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifndef __CSITEM_OPTION_H__
#define __CSITEM_OPTION_H__

#include "Singleton.h"
#include "zzzinfomation.h"
#include <map>

#define	BYTE_MAX				255

#define MAX_SET_OPTION			256
#define EXT_A_SET_OPTION		1
#define EXT_B_SET_OPTION		2
const BYTE MASTERY_OPTION = 24;

class CSItemOption : public Singleton<CSItemOption>
{
	typedef struct
	{
		std::vector<int> EquippedSetItem;
	}ITEM_SET_TYPE_EXT;

	typedef std::map<int, std::string>		MAP_EQUIPPEDSETITEMNAME;
	typedef std::map<BYTE, int>				MAP_EQUIPPEDSETITEM_SEQUENCE;
	typedef std::vector<ITEM_SET_TYPE_EXT>	MAP_ITEM_SET_TYPE;
	typedef std::vector<int>				MAP_ITEM_SET_OPTION;
private:
/*+00004*/	ITEM_SET_TYPE   m_ItemSetType[MAX_ITEM]; //-- size: 32768
/*+32772*/	ITEM_SET_OPTION m_ItemSetOption[MAX_SET_OPTION]; //-- size: 7040
/*+39812*/	bool m_bViewOptionList;
/*+39813*/	BYTE m_byRenderOptionList;
/*+39814*/	BYTE m_bySelectedItemOption;
/*+39815*/	BYTE m_bySameSetItem;
/*+39816*/	char m_strSetName[2][32];
/*+39880*/	MAP_EQUIPPEDSETITEMNAME					m_mapEquippedSetItemName;
/*+39912*/	MAP_EQUIPPEDSETITEMNAME::iterator		m_iterESIN;
/*+39920*/	MAP_EQUIPPEDSETITEM_SEQUENCE			m_mapEquippedSetItemSequence;
/*+39952*/	MAP_EQUIPPEDSETITEM_SEQUENCE::iterator	m_iterESIS;
/*+39960*/	BYTE m_bySetOptionANum;
/*+39961*/	BYTE m_bySetOptionBNum;
public:
/*+39962*/	BYTE m_bySetOptionIndex[2];
/*+39964*/	BYTE m_bySetOptionList[16][3];
/*+40012*/	BYTE m_bySetOptionListOnOff[16];
/*+40028*/	int  m_iSetOptionListValue[16][2];
/*+40156*/	int  m_arLimitSetItemOptionCount[MAX_EQUIPMENT];
			int  m_ByLineIni;
			int  m_ByLineEnd;
			MAP_ITEM_SET_TYPE mapesettype;
private:
	int Search_From_EquippedSetItemNameMapTable(char* szSetItemname);
public:
	CSItemOption();
	virtual~CSItemOption();

	bool Find_From_EquippedSetItemNameMapTable(char* szSetItemname);
	BYTE Search_From_EquippedSetItemNameSequence(char* szSetItemname);
	bool OpenItemSetType(const char* filename);
	bool OpenItemSetOption(const char* filename);
	void checkItemType(BYTE* optioList, const int iType, const int setType);
	void calcSetOptionList(BYTE* optioList);
	void getExplainText(char* text, const BYTE option, const BYTE value, const BYTE SetA);
	void getAllAddState(DWORD* Strength, DWORD* Dexterity, DWORD* Energy, DWORD* Vitality, DWORD* Charisma);
	int GetSetItmeCount(const ITEM* pselecteditem);
	bool isFullseteffect(const ITEM* ip);
	void getAllAddOptionStatesbyCompare(DWORD* Strength, DWORD* Dexterity, DWORD* Energy, DWORD* Vitality, DWORD* Charisma, DWORD iCompareStrength, DWORD iCompareDexterity, DWORD iCompareEnergy, DWORD iCompareVitality, DWORD iC);
	void getAllAddStateOnlyAddValue(DWORD* AddStrength, DWORD* AddDexterity, DWORD* AddEnergy, DWORD* AddVitality, DWORD* AddCharisma);

	void init();
	void ClearListOnOff();
	bool OpenItemSetScript(bool bTestServer);
	bool IsDisableSkill(int Type, int Energy, int Charisma = 0);
	BYTE IsChangeSetItem(const int Type, const int SubType);
	WORD GetMixItemLevel(const int Type);
	bool GetSetItemName(char* strName, const int iType, const int setType);
	void PlusSpecial(DWORD* Value, const int Special);
	void PlusSpecialPercent(DWORD* Value, const int Special);
	void PlusSpecialLevel(DWORD* Value, const DWORD SrcValue, const int Special);
	void PlusMastery(int* Value, const BYTE MasteryType);
	void MinusSpecialPercent(int* Value, const int Special);
	void GetSpecial(DWORD* Value, int Special);
	void GetSpecialPercent(DWORD* Value, int Special);
	void GetSpecialLevel(DWORD* Value, const DWORD SrcValue, int Special);
	int GetDefaultOptionValue(ITEM* ip, WORD* Value);
	bool GetDefaultOptionText(const ITEM* ip, char* Text);
	int RenderDefaultOptionText(const ITEM* ip, int TextNum);
	bool Special_Option_Check(int Kind = 0);
	void CheckItemSetOptions();
	void MoveSetOptionList(const int StartX, const int StartY);
	void RenderSetOptionButton(const int StartX, const int StartY);
	void RenderSetOptionList(const int StartX, const int StartY);
	void UpdateCount_SetOptionPerEquippedSetItem(const BYTE* byOptionList, int* arLimitSetItemOptionCount, ITEM* ItemsEquipment);
	int GetCurrentTypeSetitemCount(const ITEM& CurItem_, const BYTE* byOptionList);

	int GetSetItmeCount(ITEM* pItem, int num);
	int RenderSetOptionListInItem(ITEM* pItem, int TextNum, bool bIsEquippedItem = false);
	void ClearOptionHelper();
	void CheckRenderOptionHelper(const char* FilterName);
	void RenderOptionHelper();
	BYTE GetSetOptionANum();
	BYTE GetSetOptionBNum();
	void SetViewOptionList(bool bView);
	bool IsViewOptionList();
};

#define g_csItemOption CSItemOption::GetSingleton()


extern std::string g_strSelectedML;

#endif
