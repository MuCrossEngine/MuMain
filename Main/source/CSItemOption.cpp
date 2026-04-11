/*+++++++++++++++++++++++++++++++++++++
	INCLUDE.
+++++++++++++++++++++++++++++++++++++*/
#include "stdafx.h"
#include "ZzzOpenglUtil.h"
#include "zzzInfomation.h"
#include "zzzBmd.h"
#include "zzzObject.h"
#include "zzztexture.h"
#include "zzzCharacter.h"
#include "zzzscene.h"
#include "zzzInterface.h"
#include "zzzinventory.h"
#include "dsplaysound.h"
#include "CSItemOption.h"
#include "CharacterManager.h"
#include "UIControls.h"
#include "NewUISystem.h"
#include "SkillManager.h"

extern float g_fScreenRate_x;
extern float g_fScreenRate_y;

static CSItemOption csItemOption;


CSItemOption::CSItemOption()
{
	init();
	m_ByLineIni = 0;
	m_ByLineEnd = 0;
}

CSItemOption::~CSItemOption()
{
	mapesettype.clear();
}

void CSItemOption::init()
{
	memset(m_strSetName, 0, sizeof(m_strSetName));

	m_bViewOptionList = false;
	m_byRenderOptionList = 0;
	m_bySelectedItemOption = 0;
	m_bySameSetItem = 0;
	mapesettype.resize(MAX_SET_OPTION);
}

void CSItemOption::ClearListOnOff()
{
	memset(m_bySetOptionListOnOff, 0, sizeof(BYTE) * 16);
}

bool CSItemOption::OpenItemSetScript(bool bTestServer)
{
	char strFileName[255];
	memset(strFileName, 0, sizeof(strFileName));

#if MAIN_UPDATE > 303
#ifdef CHINESE_LANGUAGE
	sprintf(strFileName, "Data\\Local\\ItemSetOption.bmd");
#else
	sprintf(strFileName, "Data\\Local\\%s\\ItemSetOption.bmd", g_strSelectedML.c_str());
#endif // CHINESE_LANGUAGE 
#else
	sprintf(strFileName, "Data\\Local\\ItemSetOption.bmd");
#endif

	if (!OpenItemSetOption(strFileName))
		return false;

	sprintf(strFileName, "Data\\Local\\ItemSetType.bmd");

	if (!OpenItemSetType(strFileName))
		return false;

	return true;
}

int CSItemOption::Search_From_EquippedSetItemNameMapTable(char* szSetItemname)
{
	int iSizeFindName = strlen(szSetItemname);

	for (m_iterESIN = m_mapEquippedSetItemName.begin(); m_iterESIN != m_mapEquippedSetItemName.end(); ++m_iterESIN)
	{
		std::string	strCur;

		strCur = m_iterESIN->second;
		int iSizeCurName = strCur.size();

		if (!strncmp(szSetItemname, strCur.c_str(), iSizeFindName) && !strncmp(szSetItemname, strCur.c_str(), iSizeCurName))
		{
			return m_iterESIN->first;
		}
	}
	return -1;
}

bool CSItemOption::Find_From_EquippedSetItemNameMapTable(char* szSetItemname)
{
	int iResult = Search_From_EquippedSetItemNameMapTable(szSetItemname);

	if (-1 == iResult)
	{
		return false;
	}
	else
	{
		return true;
	}
}

BYTE CSItemOption::Search_From_EquippedSetItemNameSequence(char* szSetItemname)
{
	int byCur = Search_From_EquippedSetItemNameMapTable(szSetItemname);

	if (-1 != byCur)
	{
		for (m_iterESIS = m_mapEquippedSetItemSequence.begin(); m_iterESIS != m_mapEquippedSetItemSequence.end(); ++m_iterESIS)
		{
			if ((int)(m_iterESIS->second) == byCur)
			{
				return (BYTE)m_iterESIS->first;
			}
		}
	}
	return (BYTE)BYTE_MAX;
}

bool CSItemOption::OpenItemSetType(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		int Size = sizeof(ITEM_SET_TYPE);
		BYTE* Buffer = new BYTE[Size * MAX_ITEM];
		fread(Buffer, Size * MAX_ITEM, 1, fp);

		DWORD dwCheckSum;
		fread(&dwCheckSum, sizeof(DWORD), 1, fp);
		fclose(fp);

		if (dwCheckSum != GenerateCheckSum2(Buffer, Size * MAX_ITEM, 0xE5F1))
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", filename);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
		else
		{
			BYTE* pSeek = Buffer;
			for (int i = 0; i < MAX_ITEM; i++)
			{
				BuxConvert(pSeek, Size);
				memcpy(&m_ItemSetType[i], pSeek, Size);

				if (m_ItemSetType[i].byOption[0] != 0xFF)
				{
					int byOption = m_ItemSetType[i].byOption[0];
					mapesettype[byOption].EquippedSetItem.push_back(i);
				}
				if (m_ItemSetType[i].byOption[1] != 0xFF)
				{
					int byOption = m_ItemSetType[i].byOption[1];
					mapesettype[byOption].EquippedSetItem.push_back(i);
				}
				pSeek += Size;
			}
		}
		delete[] Buffer;
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", filename);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}

	return true;
}

bool CSItemOption::OpenItemSetOption(const char* filename)
{
	FILE* fp = fopen(filename, "rb");
	if (fp != NULL)
	{
		int Size = sizeof(ITEM_SET_OPTION);
		BYTE* Buffer = new BYTE[Size * MAX_SET_OPTION];
		fread(Buffer, Size * MAX_SET_OPTION, 1, fp);

		DWORD dwCheckSum;
		fread(&dwCheckSum, sizeof(DWORD), 1, fp);
		fclose(fp);

		if (dwCheckSum != GenerateCheckSum2(Buffer, Size * MAX_SET_OPTION, 0xA2F1))
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", filename);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
		else
		{
			BYTE* pSeek = Buffer;
			for (int i = 0; i < MAX_SET_OPTION; i++)
			{
				BuxConvert(pSeek, Size);
				memcpy(&m_ItemSetOption[i], pSeek, Size);

				pSeek += Size;
			}
		}
		delete[] Buffer;
	}
	else
	{
		char Text[256];
		sprintf(Text, "%s - File not exist.", filename);
		g_ErrorReport.Write(Text);
		MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}

	return true;
}

bool CSItemOption::IsDisableSkill(int Type, int Energy, int Charisma)
{
	bool success = false;
	int SkillEnergy = 0;

	if (Type < Skill_Explosion_Sahamutt || Type > Skill_Pollution)
		SkillEnergy = (int)((double)(SkillAttribute[Type].Energy * SkillAttribute[Type].Level) * 0.04f + 20.f);
	else
		SkillEnergy = (int)((double)(SkillAttribute[Type].Energy * SkillAttribute[Type].Level) * 0.03f + 20.f);

	if (gCharacterManager.GetBaseClass(Hero->Class) == CLASS_KNIGHT)
		SkillEnergy = 4 * SkillAttribute[Type].Energy * SkillAttribute[Type].Level / 100 + 10;

	if (Type > Skill_Anger_Blow_Mastery)
	{
		switch (Type)
		{
		case Skill_Force_Wave_Strengthener:
		LABEL_19:
			SkillEnergy = 15;
			break;
		case Skill_Strike_of_Destruction_Strengthe:
		case Skill_Triple_Shot_Strengthener:
		case Skill_Penetration_Strengthener:
		case Skill_Triple_Shot_Mastery:
		case Skill_Ice_Arrow_Strengthener:
		case Skill_Infinity_Arrow_Strengthener:
		case Skill_Dark_Horse_Strengthener:
		case Skill_Earthshake_Strengthener:
		case Skill_Earthshake_Mastery:
		case Skill_Fire_Scream_Strengthener:
		case Skill_Chaotic_Diseier_Strengthener:
			goto LABEL_20;
		default:
			break;
		}
	}
	else if (Type >= Skill_Cyclone_Strengthener)
	{
	LABEL_20:
		SkillEnergy = 0;
	}
	else
	{
		switch (Type)
		{
		case Skill_Energy_Ball:
			SkillEnergy = 0;
			break;
		case Skill_Summon_Goblin:
			SkillEnergy = 30;
			break;
		case Skill_Summon_Stone_Golem:
			SkillEnergy = 60;
			break;
		case Skill_Summon_Assassin:
			SkillEnergy = 90;
			break;
		case Skill_Summon_Elite_Yeti:
			SkillEnergy = 130;
			break;
		case Skill_Summon_Dark_Knight:
			SkillEnergy = 170;
			break;
		case Skill_Summon_Bali:
			SkillEnergy = 210;
			break;
		case Skill_Summon_Soldier:
			SkillEnergy = 300;
			break;
		case Skill_Force:
			goto LABEL_19;
		case Skill_Defense:
		case Skill_Falling_Slash:
		case Skill_Lunge:
		case Skill_Uppercut:
		case Skill_Cyclone:
		case Skill_Slash:
		case Skill_Triple_Shot:
		case Skill_Twisting_Slash:
		case Skill_Rageful_Blow:
		case Skill_Death_Stab:
		case Skill_Crescent_Moon_Slash:
		case Skill_Lance:
		case Skill_Starfall:
		case Skill_Impale:
		case Skill_Swell_Life:
		case Skill_Fire_Breath:
		case Skill_Ice_Arrow:
		case Skill_Penetration:
		case Skill_Fire_Slash:
		case Skill_Power_Slash:
		case Skill_Spiral_Slash:
		case Skill_Earthshake:
		case Skill_Stun:
		case Skill_Cancel_Stun:
		case Skill_Swell_Mana:
		case Skill_Invisibility:
		case Skill_Cancel_Invisibility:
		case Skill_Abolish_Magic:
		case Skill_Mana_Rays:
		case Skill_Brand:
		case Skill_Plasma_Storm:
		case Skill_Infinity_Arrow:
		case Skill_Fire_Scream:
		case Skill_Explosion:
		case Skill_PetRaven_Default:
		case Skill_PetRaven_Rand:
		case Skill_PetRaven_Owner:
		case Skill_PetRaven_Target:
		case Skill_Strike_of_Destruction:
		case Skill_Recovery:
		case Skill_Multi_Shot:
		case Skill_Chaotic_Diseier:
			goto LABEL_20;
		default:
			break;
		}
	}

	if (Charisma <= 0 || Charisma >= SkillAttribute[Type].Charisma)
	{
		success = Energy < SkillEnergy;
	}
	else
	{
		success = true;
	}

	return success;
}

BYTE CSItemOption::IsChangeSetItem(const int Type, const int SubType = -1)
{
	ITEM_SET_TYPE& itemSType = m_ItemSetType[Type];

	if (SubType == -1)
	{
		if (itemSType.byOption[0] == 0xFF && itemSType.byOption[1] == 0xFF)
			return 0;
		return 0xFF;
	}
	else
	{
		if (itemSType.byOption[SubType] == 0xFF)
			return 0;

		return SubType + 1;
	}
}

WORD CSItemOption::GetMixItemLevel(const int Type)
{
	if (Type < 0) return 0;

	WORD MixLevel = 0;
	ITEM_SET_TYPE& itemSType = m_ItemSetType[Type];

	MixLevel = MAKEWORD(itemSType.byMixItemLevel[0], itemSType.byMixItemLevel[1]);

	return MixLevel;
}

bool CSItemOption::GetSetItemName(char* strName, const int iType, const int setType)
{
	int setItemType = ((setType % 0x04) - 1);

	if (setItemType == 0 || setItemType == 1)
	{
		ITEM_SET_TYPE& itemSType = m_ItemSetType[iType];
		if (itemSType.byOption[setItemType] != 0xFF && itemSType.byOption[setItemType] != 0)
		{
			ITEM_SET_OPTION& itemOption = m_ItemSetOption[itemSType.byOption[setItemType]];
			memcpy(strName, itemOption.strSetName, sizeof(char) * 32);
			int length = strlen(strName);
			strName[length] = ' ';
			strName[length + 1] = 0;
			return true;
		}
	}
	return false;
}

void CSItemOption::checkItemType(BYTE* optionList, const int iType, const int setType)
{
	int setItemType = ((setType % 0x04) - 1);

	if (setItemType == 0 || setItemType == 1)
	{
		ITEM_SET_TYPE& itemSType = m_ItemSetType[iType];

		if (itemSType.byOption[setItemType] != 0xFF && itemSType.byOption[setItemType] != 0)
		{
			for (int i = 0; i < 30; i += 3)
			{
				if (optionList[i] == 0)
				{
					optionList[i] = itemSType.byOption[setItemType];
					optionList[i + 1]++;
					optionList[i + 2] = setItemType - 1;
					break;
				}
				else if (optionList[i] == itemSType.byOption[setItemType])
				{
					optionList[i + 1]++;
					optionList[i + 2] = setItemType;
					break;
				}
			}
		}
	}
}

void CSItemOption::calcSetOptionList(BYTE* optionList)
{
	int Class = gCharacterManager.GetBaseClass(Hero->Class);
	int ExClass = gCharacterManager.IsSecondClass(Hero->Class);

	BYTE bySetOptionListTmp[2][16];
	BYTE bySetOptionListTmp2[2][16];
	int iSetOptionListValue[2][16];

	unsigned int arruiSetItemTypeSequence[2][16];
	memset(arruiSetItemTypeSequence, 0, sizeof(unsigned int) * 32);

	BYTE optionCount[2] = { 0, 0 };  //  
	BYTE setType = 0;

	m_bySameSetItem = 0;

	ZeroMemory(m_strSetName, sizeof(char) * 2 * 32);

	m_bySetOptionIndex[0] = 0;
	m_bySetOptionIndex[1] = 0;
	m_bySetOptionANum = 0;
	m_bySetOptionBNum = 0;
	Hero->ExtendState = 0;

	unsigned int iSetItemTypeSequence = 0, iCurrentSetItemTypeSequence = 0;

	m_mapEquippedSetItemName.clear();
	m_mapEquippedSetItemSequence.clear();

	for (int i = 0; i < 30; i += 3)
	{
		if (optionList[i + 1] >= 2)
		{
			int count = optionList[i + 1] - 1;
			int standardCount = min(count, 6);
			ITEM_SET_OPTION& itemOption = m_ItemSetOption[optionList[i]];

			BYTE RequireClass = 0;
			if (itemOption.byRequireClass[0] == 1 && Class == CLASS_WIZARD)
				RequireClass = 1;
			if (itemOption.byRequireClass[0] == 2 && Class == CLASS_WIZARD && ExClass)
				RequireClass = 1;
			if (itemOption.byRequireClass[1] == 1 && Class == CLASS_KNIGHT)
				RequireClass = 1;
			if (itemOption.byRequireClass[1] == 2 && Class == CLASS_KNIGHT && ExClass)
				RequireClass = 1;
			if (itemOption.byRequireClass[2] == 1 && Class == CLASS_ELF)
				RequireClass = 1;
			if (itemOption.byRequireClass[2] == 2 && Class == CLASS_ELF && ExClass)
				RequireClass = 1;
			if (itemOption.byRequireClass[3] == 1 && Class == CLASS_DARK)
				RequireClass = 1;
			if (itemOption.byRequireClass[3] == 1 && Class == CLASS_DARK && ExClass)
				RequireClass = 1;
			if (itemOption.byRequireClass[4] == 1 && Class == CLASS_DARK_LORD)
				RequireClass = 1;
			if (itemOption.byRequireClass[4] == 1 && Class == CLASS_DARK_LORD && ExClass)
				RequireClass = 1;
			if (itemOption.byRequireClass[5] == 1 && Class == CLASS_SUMMONER)
				RequireClass = 1;
			if (itemOption.byRequireClass[5] == 1 && Class == CLASS_SUMMONER && ExClass)
				RequireClass = 1;
#ifdef PBG_ADD_NEWCHAR_MONK
			if (itemOption.byRequireClass[6] == 1 && Class == CLASS_RAGEFIGHTER)
				RequireClass = 1;
			if (itemOption.byRequireClass[6] == 1 && Class == CLASS_RAGEFIGHTER && ExClass)
				RequireClass = 1;
#endif //PBG_ADD_NEWCHAR_MONK
			setType = optionList[i + 2];
			m_bySetOptionIndex[setType] = optionList[i];
			if (m_strSetName[setType][0] != 0 && strcmp(m_strSetName[setType], itemOption.strSetName) != NULL)
			{
				if (m_strSetName[0][0] == 0)
					strcpy(m_strSetName[0], itemOption.strSetName);
				else
					strcpy(m_strSetName[1], itemOption.strSetName);
				m_bySameSetItem = count;
			}
			else
			{
				strcpy(m_strSetName[setType], itemOption.strSetName);
			}

			bool bFind = false;
			for (m_iterESIN = m_mapEquippedSetItemName.begin(); m_iterESIN != m_mapEquippedSetItemName.end(); ++m_iterESIN)
			{
				std::string strCur = m_iterESIN->second;

				if (strcmp(itemOption.strSetName, strCur.c_str()) == 0)
				{
					bFind = true;
					break;
				}
			}

			if (false == bFind)
			{
				iCurrentSetItemTypeSequence = iSetItemTypeSequence++;
				m_mapEquippedSetItemName.insert(std::pair<int, std::string>(iCurrentSetItemTypeSequence, itemOption.strSetName));
			}

			BYTE option[2];
			int  value[2];
			for (int j = 0; j < count; ++j)
			{
				option[0] = 0xFF; option[1] = 0xFF;
				if (j < standardCount)
				{
					option[0] = itemOption.byStandardOption[j][0];
					value[0] = itemOption.byStandardOptionValue[j][0];
					option[1] = itemOption.byStandardOption[j][1];
					value[1] = itemOption.byStandardOptionValue[j][1];
				}
				else if (j < (count - standardCount))
				{
					option[0] = itemOption.byExtOption[j];
					value[0] = itemOption.byExtOptionValue[j];
				}

				if (option[0] != 0xFF)
				{
					if (option[0] < MASTERY_OPTION)
					{
						arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
						bySetOptionListTmp[setType][optionCount[setType]] = option[0];
						bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
						iSetOptionListValue[setType][optionCount[setType]] = value[0];
						optionCount[setType]++;

					}
					else
					{
						if (itemOption.byRequireClass[Class] && ExClass >= itemOption.byRequireClass[Class] - 1)
						{
							arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
							bySetOptionListTmp[setType][optionCount[setType]] = option[0];
							bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
							iSetOptionListValue[setType][optionCount[setType]] = value[0];
							optionCount[setType]++;
						}
					}
				}
				if (option[1] != 0xFF)
				{
					if (option[1] < MASTERY_OPTION)
					{
						arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
						bySetOptionListTmp[setType][optionCount[setType]] = option[1];
						bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
						iSetOptionListValue[setType][optionCount[setType]] = value[1];
						optionCount[setType]++;
					}
					else
					{
						if (itemOption.byRequireClass[Class] && ExClass >= itemOption.byRequireClass[Class] - 1)
						{
							arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
							bySetOptionListTmp[setType][optionCount[setType]] = option[1];
							bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
							iSetOptionListValue[setType][optionCount[setType]] = value[1];
							optionCount[setType]++;
						}
					}
				}
			}

			if (count >= itemOption.byOptionCount - 2)
			{
				for (int j = 0; j < 5; ++j)
				{
					option[0] = itemOption.byFullOption[j];
					value[0] = itemOption.byFullOptionValue[j];
					if (option[0] != 0xFF)
					{
						if (option[0] < MASTERY_OPTION)
						{
							arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
							bySetOptionListTmp[setType][optionCount[setType]] = option[0];
							bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
							iSetOptionListValue[setType][optionCount[setType]] = value[0];
							optionCount[setType]++;

							if (m_bySameSetItem != 0) m_bySameSetItem++;
						}
						else
						{
							if (itemOption.byRequireClass[Class] && ExClass >= itemOption.byRequireClass[Class] - 1)
							{
								arruiSetItemTypeSequence[setType][optionCount[setType]] = iCurrentSetItemTypeSequence;
								bySetOptionListTmp[setType][optionCount[setType]] = option[0];
								bySetOptionListTmp2[setType][optionCount[setType]] = RequireClass;
								iSetOptionListValue[setType][optionCount[setType]] = value[0];
								optionCount[setType]++;

								if (m_bySameSetItem != 0) m_bySameSetItem++;
							}
						}
					}
				}
				Hero->ExtendState = 1;
			}
		}
	}

	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < optionCount[i]; ++j)
		{
			m_mapEquippedSetItemSequence.insert(std::pair<BYTE, int>((i * optionCount[0]) + j, arruiSetItemTypeSequence[i][j]));
			m_bySetOptionList[(i * optionCount[0]) + j][0] = bySetOptionListTmp[i][j];
			m_bySetOptionList[(i * optionCount[0]) + j][1] = bySetOptionListTmp2[i][j];
			m_iSetOptionListValue[(i * optionCount[0]) + j][0] = iSetOptionListValue[i][j];
			m_iSetOptionListValue[(i * optionCount[0]) + j][1] = iSetOptionListValue[i][j];
		}
	}
	m_bySetOptionANum = optionCount[0];
	m_bySetOptionBNum = optionCount[1];
}


void CSItemOption::getExplainText(char* text, const BYTE option, const BYTE value, const BYTE SetIndex)
{
	switch (option + AT_SET_OPTION_IMPROVE_STRENGTH)
	{
	case AT_SET_OPTION_IMPROVE_MAGIC_POWER:
		sprintf(text, GlobalText[632], value);
		break;
	case AT_SET_OPTION_IMPROVE_STRENGTH:
	case AT_SET_OPTION_IMPROVE_DEXTERITY:
	case AT_SET_OPTION_IMPROVE_ENERGY:
	case AT_SET_OPTION_IMPROVE_VITALITY:
	case AT_SET_OPTION_IMPROVE_CHARISMA:
	case AT_SET_OPTION_IMPROVE_ATTACK_MIN:
	case AT_SET_OPTION_IMPROVE_ATTACK_MAX:
		sprintf(text, GlobalText[950 + option], value);
		break;
	case AT_SET_OPTION_IMPROVE_DAMAGE:
	case AT_SET_OPTION_IMPROVE_ATTACKING_PERCENT:
	case AT_SET_OPTION_IMPROVE_DEFENCE:
	case AT_SET_OPTION_IMPROVE_MAX_LIFE:
	case AT_SET_OPTION_IMPROVE_MAX_MANA:
	case AT_SET_OPTION_IMPROVE_MAX_AG:
	case AT_SET_OPTION_IMPROVE_ADD_AG:
	case AT_SET_OPTION_IMPROVE_CRITICAL_DAMAGE_PERCENT:
	case AT_SET_OPTION_IMPROVE_CRITICAL_DAMAGE:
	case AT_SET_OPTION_IMPROVE_EXCELLENT_DAMAGE_PERCENT:
	case AT_SET_OPTION_IMPROVE_EXCELLENT_DAMAGE:
	case AT_SET_OPTION_IMPROVE_SKILL_ATTACK:
	case AT_SET_OPTION_DOUBLE_DAMAGE:
		sprintf(text, GlobalText[949 + option], value);
		break;
	case AT_SET_OPTION_DISABLE_DEFENCE:
		sprintf(text, GlobalText[970], value);
		break;
	case AT_SET_OPTION_TWO_HAND_SWORD_IMPROVE_DAMAGE:
		sprintf(text, GlobalText[983], value);
		break;
	case AT_SET_OPTION_IMPROVE_SHIELD_DEFENCE:
		sprintf(text, GlobalText[984], value);
		break;
	case AT_SET_OPTION_IMPROVE_ATTACK_1:
	case AT_SET_OPTION_IMPROVE_ATTACK_2:
	case AT_SET_OPTION_IMPROVE_MAGIC:
		//	case AT_SET_OPTION_IMPROVE_DEFENCE_1:
		//	case AT_SET_OPTION_IMPROVE_DEFENCE_2:
	case AT_SET_OPTION_IMPROVE_DEFENCE_3:
	case AT_SET_OPTION_IMPROVE_DEFENCE_4:
	case AT_SET_OPTION_FIRE_MASTERY:
	case AT_SET_OPTION_ICE_MASTERY:
	case AT_SET_OPTION_THUNDER_MASTERY:
	case AT_SET_OPTION_POSION_MASTERY:
	case AT_SET_OPTION_WATER_MASTERY:
	case AT_SET_OPTION_WIND_MASTERY:
	case AT_SET_OPTION_EARTH_MASTERY:
		sprintf(text, GlobalText[971 + (option + AT_SET_OPTION_IMPROVE_STRENGTH - AT_SET_OPTION_IMPROVE_ATTACK_2)], value);
		break;
	}
}

void CSItemOption::PlusSpecial(DWORD* Value, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value += optionValue;
	}
}

void CSItemOption::PlusSpecialPercent(DWORD* Value, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value += ((*Value) * optionValue) / 100;
	}
}


void CSItemOption::PlusSpecialLevel(DWORD* Value, const DWORD SrcValue, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	int count = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionList[i][1] != 0 && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue = m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
			count++;
		}
	}

	if (optionValue)
	{
		optionValue = SrcValue * optionValue / 100;
		*Value += (optionValue * count);
	}
}

void CSItemOption::PlusMastery(int* Value, const BYTE MasteryType)
{
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] >= MASTERY_OPTION && (m_bySetOptionList[i][0] - MASTERY_OPTION - 5) == MasteryType && m_bySetOptionList[i][1] != 0 && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value += optionValue;
	}
}

void CSItemOption::MinusSpecialPercent(int* Value, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionList[i][1] != 0 && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value -= *Value * optionValue / 100;
	}
}

void CSItemOption::GetSpecial(DWORD* Value, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value += optionValue;
	}
}

void CSItemOption::GetSpecialPercent(DWORD* Value, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue += m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
		}
	}

	if (optionValue)
	{
		*Value += *Value * optionValue / 100;
	}
}

void CSItemOption::GetSpecialLevel(DWORD* Value, const DWORD SrcValue, int Special)
{
	Special -= AT_SET_OPTION_IMPROVE_STRENGTH;
	int optionValue = 0;
	int count = 0;
	for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
	{
		if (m_bySetOptionList[i][0] == Special && m_bySetOptionList[i][1] != 0 && m_bySetOptionListOnOff[i] == 0)
		{
			optionValue = m_iSetOptionListValue[i][0];
			m_bySetOptionListOnOff[i] = 1;
			count++;
		}
	}

	if (optionValue)
	{
		optionValue = SrcValue * optionValue / 100;
		*Value += (optionValue * count);
	}
}

int CSItemOption::GetDefaultOptionValue(ITEM* ip, WORD* Value)
{
	*Value = ((ip->ExtOption >> 2) % 0x04);

	Script_Item* p = GMItemMng->find(ip->Type);

	return p->AttType;
}

bool CSItemOption::GetDefaultOptionText(const ITEM* ip, char* Text)
{
	if (((ip->ExtOption >> 2) % 0x04) <= 0)
		return false;

	switch (GMItemMng->find(ip->Type)->AttType)
	{
	case SET_OPTION_STRENGTH:
		sprintf(Text, GlobalText[950], ((ip->ExtOption >> 2) % 0x04) * 5);
		break;
	case SET_OPTION_DEXTERITY:
		sprintf(Text, GlobalText[951], ((ip->ExtOption >> 2) % 0x04) * 5);
		break;
	case SET_OPTION_ENERGY:
		sprintf(Text, GlobalText[952], ((ip->ExtOption >> 2) % 0x04) * 5);
		break;
	case SET_OPTION_VITALITY:
		sprintf(Text, GlobalText[953], ((ip->ExtOption >> 2) % 0x04) * 5);
		break;
	default:
		return false;
	}
	return true;
}

bool CSItemOption::Special_Option_Check(int Kind)
{
	bool success = false;

	if (&SkillAttribute[Kind])
	{
		if (SkillAttribute[Kind].ItemSkill == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				ITEM* item = &CharacterMachine->Equipment[EQUIPMENT_WEAPON_RIGHT + i];
				if (item && item->Type > -1)
				{
					Script_Item* item_attr = GMItemMng->find(item->Type);
					if (item_attr)
					{
						WORD wSkillIndex = item_attr->m_wSkillIndex;
						int temp = Kind;
#if MAIN_UPDATE > 303
						for (int j = g_pMasterSkillTree->GetBeforeSkillID(Kind); j; j = g_pMasterSkillTree->GetBeforeSkillID(j))
						{
							temp = j;
						}
#endif // MAIN_UPDATE != 303

						if (wSkillIndex == temp)
						{
							for (int j = 0; j < item->SpecialNum; j++)
							{
								if (item->Special[j] == temp)
									return true;
							}
						}
					}
				}
			}
		}
		else
		{
			success = true;
		}
	}
	else
	{
		success = false;
	}

	return success;
}

int CSItemOption::RenderDefaultOptionText(const ITEM* ip, int TextNum)
{
	int TNum = TextNum;

	if (((ip->ExtOption >> 2) % 0x04) <= 0)
		return TNum;

	if(GMItemMng->find(ip->Type)->AttType >= SET_OPTION_STRENGTH && GMItemMng->find(ip->Type)->AttType < SET_OPTION_END)
		sprintf(TextList[TNum++], "\n");

	if (GetDefaultOptionText(ip, TextList[TNum]))
	{
		TextListColor[TNum] = TEXT_COLOR_BLUE;
		TNum++;

		if ((ip->Type >= ITEM_HELPER + 8 && ip->Type <= ITEM_HELPER + 9) || (ip->Type >= ITEM_HELPER + 12 && ip->Type <= ITEM_HELPER + 13) || (ip->Type >= ITEM_HELPER + 21 && ip->Type <= ITEM_HELPER + 27))
		{
			sprintf(TextList[TNum], GlobalText[1165]);
			TextListColor[TNum] = TEXT_COLOR_BLUE;
			TNum++;
		}
	}

	return TNum;
}

void CSItemOption::getAllAddState(DWORD* Strength, DWORD* Dexterity, DWORD* Energy, DWORD* Vitality, DWORD* Charisma)
{
	for (int i = EQUIPMENT_WEAPON_RIGHT; i < MAX_EQUIPMENT; ++i)
	{
		ITEM* item = &CharacterMachine->Equipment[i];

		if (item->Durability <= 0)
		{
			continue;
		}

		WORD Result = 0;
		switch (GetDefaultOptionValue(item, &Result))
		{
		case SET_OPTION_STRENGTH:
			*Strength += Result * 5;
			break;

		case SET_OPTION_DEXTERITY:
			*Dexterity += Result * 5;
			break;

		case SET_OPTION_ENERGY:
			*Energy += Result * 5;
			break;

		case SET_OPTION_VITALITY:
			*Vitality += Result * 5;
			break;
		}
	}

	GetSpecial(Strength, AT_SET_OPTION_IMPROVE_STRENGTH);
	GetSpecial(Dexterity, AT_SET_OPTION_IMPROVE_DEXTERITY);
	GetSpecial(Energy, AT_SET_OPTION_IMPROVE_ENERGY);
	GetSpecial(Vitality, AT_SET_OPTION_IMPROVE_VITALITY);
	GetSpecial(Charisma, AT_SET_OPTION_IMPROVE_CHARISMA);
}

void CSItemOption::getAllAddStateOnlyAddValue(DWORD* AddStrength, DWORD* AddDexterity, DWORD* AddEnergy, DWORD* AddVitality, DWORD* AddCharisma)
{
	*AddStrength = *AddDexterity = *AddEnergy = *AddVitality = *AddCharisma = 0;
	memset(m_bySetOptionListOnOff, 0, sizeof(BYTE) * 16);

	getAllAddState(AddStrength, AddDexterity, AddEnergy, AddVitality, AddCharisma);
}

void CSItemOption::getAllAddOptionStatesbyCompare(DWORD* Strength, DWORD* Dexterity, DWORD* Energy, DWORD* Vitality, DWORD* Charisma, DWORD iCompareStrength, DWORD iCompareDexterity, DWORD iCompareEnergy, DWORD iCompareVitality, DWORD iCompareCharisma)
{
	for (int i = EQUIPMENT_WEAPON_RIGHT; i < MAX_EQUIPMENT; ++i)
	{
		ITEM* item = &CharacterMachine->Equipment[i];

		if (item->RequireStrength > iCompareStrength ||
			item->RequireDexterity > iCompareDexterity ||
			item->RequireEnergy > iCompareEnergy)
		{
			continue;
		}

		if (item->Durability <= 0)
		{
			continue;
		}

		WORD Result = 0;
		switch (GetDefaultOptionValue(item, &Result))
		{
		case SET_OPTION_STRENGTH:
			*Strength += Result * 5;
			break;

		case SET_OPTION_DEXTERITY:
			*Dexterity += Result * 5;
			break;

		case SET_OPTION_ENERGY:
			*Energy += Result * 5;
			break;

		case SET_OPTION_VITALITY:
			*Vitality += Result * 5;
			break;
		}
	}

	memset(m_bySetOptionListOnOff, 0, sizeof(BYTE) * 16);

	GetSpecial(Strength, AT_SET_OPTION_IMPROVE_STRENGTH);
	GetSpecial(Dexterity, AT_SET_OPTION_IMPROVE_DEXTERITY);
	GetSpecial(Energy, AT_SET_OPTION_IMPROVE_ENERGY);
	GetSpecial(Vitality, AT_SET_OPTION_IMPROVE_VITALITY);
	GetSpecial(Charisma, AT_SET_OPTION_IMPROVE_CHARISMA);
}


void CSItemOption::CheckItemSetOptions()
{
	BYTE byOptionList[30] = { 0, };
	ITEM* itemTmp = NULL;

	memset(m_bySetOptionList, 0, sizeof(BYTE) * 16);

	for (int i = 0; i < MAX_EQUIPMENT; ++i)
	{
		if (i == EQUIPMENT_WING || i == EQUIPMENT_HELPER)
			continue;

		ITEM* ip = &CharacterMachine->Equipment[i];

		if (ip->Durability <= 0)
		{
			continue;
		}
		if (((i == EQUIPMENT_WEAPON_LEFT || i == EQUIPMENT_RING_LEFT) && itemTmp != nullptr && itemTmp->Type == ip->Type && itemTmp->ExtOption == (ip->ExtOption % 0x04)))
		{
			continue;
		}

		if (ip->Type > -1)
		{
			checkItemType(byOptionList, ip->Type, ip->ExtOption);
		}

		if (i == EQUIPMENT_WEAPON_RIGHT || i == EQUIPMENT_RING_RIGHT)
		{
			itemTmp = ip;
			itemTmp->Type = ip->Type;
		}
	}

	calcSetOptionList(byOptionList);

	getAllAddStateOnlyAddValue(&CharacterAttribute->AddStrength, &CharacterAttribute->AddDexterity, &CharacterAttribute->AddEnergy, &CharacterAttribute->AddVitality, &CharacterAttribute->AddCharisma);

	WORD AllStrength = CharacterAttribute->Strength + CharacterAttribute->AddStrength;
	WORD AllDexterity = CharacterAttribute->Dexterity + CharacterAttribute->AddDexterity;
	WORD AllEnergy = CharacterAttribute->Energy + CharacterAttribute->AddEnergy;
	WORD AllVitality = CharacterAttribute->Vitality + CharacterAttribute->AddVitality;
	WORD AllCharisma = CharacterAttribute->Charisma + CharacterAttribute->AddCharisma;
	WORD AllLevel = CharacterAttribute->Level;

	memset(byOptionList, 0, sizeof(BYTE) * 30);
	memset(m_bySetOptionList, 0xFF, sizeof(BYTE) * 16);

	for (int i = 0; i < MAX_EQUIPMENT_INDEX; ++i)
	{
		if (i == EQUIPMENT_WING || i == EQUIPMENT_HELPER) continue;

		ITEM* ip = &CharacterMachine->Equipment[i];

		if (ip->RequireDexterity > AllDexterity || ip->RequireEnergy > AllEnergy || ip->RequireStrength > AllStrength || ip->RequireLevel > AllLevel || ip->RequireCharisma > AllCharisma || ip->Durability <= 0 || (IsRequireEquipItem(ip) == false))
		{
			continue;
		}

		if (!itemTmp || ((i == EQUIPMENT_WEAPON_LEFT || i == EQUIPMENT_RING_LEFT) && itemTmp != nullptr && itemTmp->Type == ip->Type && itemTmp->ExtOption == (ip->ExtOption % 0x04)))
		{
			continue;
		}

		if (ip->Type > -1)
		{
			checkItemType(byOptionList, ip->Type, ip->ExtOption);
		}

		if (i == EQUIPMENT_WEAPON_RIGHT || i == EQUIPMENT_RING_RIGHT)
		{
			itemTmp = ip;
			itemTmp->Type = ip->Type;
		}
	}

	UpdateCount_SetOptionPerEquippedSetItem(byOptionList, m_arLimitSetItemOptionCount, CharacterMachine->Equipment);
	calcSetOptionList(byOptionList);
}

void CSItemOption::MoveSetOptionList(const int StartX, const int StartY)
{
	int x, y, Width, Height;

	Width = 162; Height = 20; x = StartX + 14; y = StartY + 22;
	if (MouseX >= x && MouseX < x + Width && MouseY >= y && MouseY < y + Height)
	{
		m_bViewOptionList = true;

		MouseLButtonPush = false;
		MouseUpdateTime = 0;
		MouseUpdateTimeMax = 6;
	}
}

void CSItemOption::RenderSetOptionButton(const int StartX, const int StartY)
{
	float x, y, Width, Height;
	char  Text[100];

	Width = 162.f; Height = 20.f; x = (float)StartX + 14; y = (float)StartY + 22;
	RenderBitmap(BITMAP_INTERFACE_EX + 21, x, y, Width, Height, 0.f, 0.f, Width / 256.f, Height / 32.f);

	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(0, 0, 0, 0xFF);
	g_pRenderText->SetBgColor(100, 0, 0, 0);
	sprintf(Text, "[%s]", GlobalText[989]);
	g_pRenderText->RenderText(StartX + 96, (int)(y + 3), Text, 0, 0, RT3_WRITE_CENTER);

	g_pRenderText->SetTextColor(0xffffffff);
	if (m_bySetOptionANum > 0 || m_bySetOptionBNum > 0)
		g_pRenderText->SetTextColor(0xFF, 204, 25, 0xFF);
	else
		g_pRenderText->SetTextColor(128, 128, 128, 0xFF);
	g_pRenderText->RenderText(StartX + 95, (int)(y + 2), Text, 0, 0, RT3_WRITE_CENTER);
	g_pRenderText->SetTextColor(0xFF, 0xFF, 0xFF, 0xFF);
}

void CSItemOption::RenderSetOptionList(const int StartX, const int StartY)
{
	if (m_bViewOptionList && (m_bySetOptionANum > 0 || m_bySetOptionBNum > 0))
	{
		g_pRenderText->SetTextColor(0xFF, 0xFF, 0xFF, 0xFF);
		g_pRenderText->SetBgColor(100, 0, 0, 0);

		int PosX, PosY;

		PosX = StartX + 95;
		PosY = StartY + 40;

		BYTE TextNum = 0;
		BYTE SkipNum = 0;
		BYTE setIndex = 0;

		sprintf(TextList[TextNum], "\n"); TextListColor[TextNum] = 0; TextBold[TextNum] = false; TextNum++; SkipNum++;

		int		iCurSetItemTypeSequence = 0, iCurSetItemType = -1;

		for (int i = 0; i < m_bySetOptionANum + m_bySetOptionBNum; ++i)
		{
			std::string	strCurrent;

			m_iterESIS = m_mapEquippedSetItemSequence.find(i);

			if (m_mapEquippedSetItemSequence.end() != m_iterESIS)
			{
				iCurSetItemTypeSequence = m_iterESIS->second;
			}
			else
			{
				iCurSetItemTypeSequence = -1;
			}

			if (iCurSetItemType != iCurSetItemTypeSequence)
			{
				iCurSetItemType = iCurSetItemTypeSequence;

				m_iterESIN = m_mapEquippedSetItemName.find(iCurSetItemTypeSequence);

				if (m_iterESIN != m_mapEquippedSetItemName.end())
				{
					strCurrent = m_iterESIN->second;

					sprintf(TextList[TextNum], "%s %s", strCurrent.c_str(), GlobalText[1089]);
					TextListColor[TextNum] = 3;
					TextBold[TextNum++] = true;
					sprintf(TextList[TextNum++], "\n"); SkipNum++;
				}
			}
			getExplainText(TextList[TextNum], m_bySetOptionList[i][0], m_iSetOptionListValue[i][0], setIndex);
			if (m_bySetOptionList[i][0] >= AT_SET_OPTION_IMPROVE_ATTACK_1 && m_bySetOptionList[i][1] == 0)
				TextListColor[TextNum] = 2;
			else
				TextListColor[TextNum] = 1;
			TextBold[TextNum] = false;
			TextNum++;
		}

		sprintf(TextList[TextNum], "\n"); TextListColor[TextNum] = 0; TextBold[TextNum] = false; TextNum++; SkipNum++;
		sprintf(TextList[TextNum], "\n"); TextListColor[TextNum] = 0; TextBold[TextNum] = false; TextNum++; SkipNum++;

		RenderTipTextList(PosX, PosY, TextNum, 120, RT3_SORT_CENTER);
	}
}

void CSItemOption::CheckRenderOptionHelper(const char* FilterName)
{
	char Name[256];

	if (FilterName[0] != '/') return;

	int Length1 = strlen(FilterName);
	for (int i = 0; i < MAX_SET_OPTION; ++i)
	{
		ITEM_SET_OPTION& setOption = m_ItemSetOption[i];
		if (setOption.byOptionCount < 0xFF)
		{
			sprintf(Name, "/%s", setOption.strSetName);

			int Length2 = strlen(Name);

			m_byRenderOptionList = 0;
			if (strncmp(FilterName, Name, Length1) == NULL && strncmp(FilterName, Name, Length2) == NULL)
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_ITEM_EXPLANATION);
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_HELP);
				g_pNewUISystem->Show(SEASON3B::INTERFACE_SETITEM_EXPLANATION);

				m_byRenderOptionList = i + 1;
				return;
			}
		}
	}
}

void CSItemOption::RenderOptionHelper()
{
	if (m_byRenderOptionList <= 0)
		return;

	int TextNum = 0;
	int sx = 0, sy = 0;
	ZeroMemory(TextListColor, 20 * sizeof(int));
	for (int i = 0; i < 30; i++)
	{
		TextList[i][0] = NULL;
	}

	ITEM_SET_OPTION& setOption = m_ItemSetOption[m_byRenderOptionList - 1];
	if (setOption.byOptionCount >= 0xFF)
	{
		m_byRenderOptionList = 0;
		return;
	}

	BYTE    option1 = 0xFF;
	BYTE    option2 = 0xFF;
	BYTE    value1 = 0xFF;
	BYTE    value2 = 0xFF;
	sprintf(TextList[TextNum], "\n"); TextNum++;
	sprintf(TextList[TextNum], "%s %s %s", setOption.strSetName, GlobalText[1089], GlobalText[159]);
	TextListColor[TextNum] = TEXT_COLOR_YELLOW;
	TextNum++;

	sprintf(TextList[TextNum], "\n"); TextNum++;
	sprintf(TextList[TextNum], "\n"); TextNum++;

	int ItemOptionCount = 0;
	int ItemElementAdd = 2;

	for (int i = 0; i < 13; ++i)
	{
		if (i >= 6)
		{
			if (i >= 8)
			{
				option1 = setOption.byFullOption[i - 8];
				value1 = setOption.byFullOptionValue[i - 8];
				ItemOptionCount = 0;
			}
			else
			{
				option1 = setOption.byExtOption[i - 6];
				value1 = setOption.byExtOptionValue[i - 6];
			}
			ItemOptionCount = 0;
		}
		else
		{
			option1 = setOption.byStandardOption[i][0];
			option2 = setOption.byStandardOption[i][1];
			value1 = setOption.byStandardOptionValue[i][0];
			value2 = setOption.byStandardOptionValue[i][1];

			ItemOptionCount = (option1 != 0xFF || option2 != 0xFF) ? 1 : 0;
		}


		if (option1 != 0xFF)
		{
			if (ItemOptionCount == 1)
			{
				sprintf(TextList[TextNum], GlobalText[877], ItemElementAdd);
				TextListColor[TextNum] = TEXT_COLOR_GREEN;
				TextBold[TextNum++] = 0;
				ItemElementAdd++;
			}

			getExplainText(TextList[TextNum], option1, value1, 0);
			TextListColor[TextNum] = TEXT_COLOR_BLUE;
			TextBold[TextNum] = false; TextNum++;
		}
		if (option2 != 0xFF)
		{
			getExplainText(TextList[TextNum], option2, value2, 0);
			TextListColor[TextNum] = TEXT_COLOR_BLUE;
			TextBold[TextNum] = false; TextNum++;
		}
	}
	sprintf(TextList[TextNum], "\n"); TextNum++;
	sprintf(TextList[TextNum], "\n"); TextNum++;

	SIZE TextSize = { 0, 0 };
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[0], 1, &TextSize);
	RenderTipTextList(sx, sy, TextNum, 0);
}

int CSItemOption::GetSetItmeCount(const ITEM* pselecteditem)
{
	int setitemcount = 0;
	ITEM_SET_TYPE& itemsettype = m_ItemSetType[pselecteditem->Type];
	int setItemType = (pselecteditem->ExtOption % 0x04) - 1;

	if (setItemType == 0 || setItemType == 1)
	{
		BYTE subtype = itemsettype.byOption[setItemType];

		for (int j = 0; j < MAX_ITEM; j++)
		{
			ITEM_SET_TYPE& temptype = m_ItemSetType[j];
			for (int i = 0; i < 2; i++)
			{
				BYTE tempsubtype = temptype.byOption[i];

				if (subtype == tempsubtype)
				{
					setitemcount++;
				}
			}
		}
	}
	return setitemcount;
}

bool CSItemOption::isFullseteffect(const ITEM* pselecteditem)
{
	int mysetitemcount = 0;
	int setItemType = (pselecteditem->ExtOption % 0x04) - 1;

	if (setItemType == 0 || setItemType == 1)
	{
		ITEM_SET_TYPE& selectedItemType = m_ItemSetType[pselecteditem->Type];
		BYTE selectedItemOption = selectedItemType.byOption[setItemType];
		ITEM_SET_OPTION& selecteditemoption = m_ItemSetOption[selectedItemOption];
		int	Cmp_Buff[10] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };

		for (int i = 0; i < MAX_EQUIPMENT; i++)
		{
			ITEM* p = &CharacterMachine->Equipment[i];

			if (p)
			{
				bool Continue_Set = false;
				for (int ipjh = 0; ipjh < mysetitemcount; ipjh++)
				{
					if (p->Type == Cmp_Buff[ipjh])
					{
						Continue_Set = true;
						break;
					}
				}
				if (Continue_Set == true)
					continue;

				setItemType = (p->ExtOption % 0x04) - 1;

				if (setItemType == 0 || setItemType == 1)
				{
					ITEM_SET_TYPE& myitemSType = m_ItemSetType[p->Type];
					BYTE myItemOption = myitemSType.byOption[setItemType];
					ITEM_SET_OPTION& setOption = m_ItemSetOption[myItemOption];

					if (strcmp(selecteditemoption.strSetName, setOption.strSetName) == NULL)
					{
						Cmp_Buff[mysetitemcount] = p->Type;
						mysetitemcount++;
					}
				}
			}
		}
	}

	if (mysetitemcount == GetSetItmeCount(pselecteditem))
		return true;
	else
		return false;
}

void CSItemOption::UpdateCount_SetOptionPerEquippedSetItem(const BYTE* byOptionList, int* arLimitSetItemOptionCount, ITEM* ItemsEquipment)
{
	for (int iE = 0; iE < MAX_EQUIPMENT_INDEX; ++iE)
	{
		int& iCurCount = arLimitSetItemOptionCount[iE];
		ITEM& CurItem = ItemsEquipment[iE];

		iCurCount = GetCurrentTypeSetitemCount(CurItem, byOptionList);
	}
}

int CSItemOption::GetCurrentTypeSetitemCount(const ITEM& CurItem_, const BYTE* byOptionList)
{
	BYTE bySetType = CurItem_.ExtOption;

	int setItemType = (bySetType % 0x04) - 1;

	ITEM_SET_TYPE& itemSType = m_ItemSetType[CurItem_.Type];

	for (int i = 0; i < 30; i += 3)
	{
		ITEM_SET_OPTION& itemOption = m_ItemSetOption[byOptionList[i]];

		if (setItemType == 0 || setItemType == 1)
		{
			if (byOptionList[i] == itemSType.byOption[setItemType])
			{
				int iEquippedCount = byOptionList[i + 1];

				if (iEquippedCount >= itemOption.byOptionCount - 1)
				{
					return 0xFF;
				}

				return iEquippedCount;
			}
		}
	}

	return 0;
}

int CSItemOption::GetSetItmeCount(ITEM* pItem, int TNum)
{
	int setItemType = (pItem->ExtOption % 0x04) - 1;

	if (setItemType == 0 || setItemType == 1)
	{
		char strText[100];
		memset(strText, 0, sizeof(strText));

		BYTE subtype = m_ItemSetType[pItem->Type].byOption[setItemType];

		ITEM_SET_OPTION& setOption = m_ItemSetOption[subtype];

		sprintf(TextList[TNum++], "\n");

		sprintf(TextList[TNum], GlobalText[878]);
		TextListColor[TNum] = TEXT_COLOR_YELLOW;
		TextBold[TNum++] = true;

		sprintf(TextList[TNum++], "\n");

		sprintf_s(TextList[TNum], "%s", setOption.strSetName);
		TextListColor[TNum] = TEXT_COLOR_GREEN;
		TextBold[TNum++] = true;

		sprintf(TextList[TNum++], "\n");

		MAP_ITEM_SET_OPTION& SetItem = mapesettype[subtype].EquippedSetItem;

		bool success = false;

		for (size_t i = 0; i < SetItem.size(); i++)
		{
			int ItemType = SetItem[i];

			Script_Item* pItemAtt = GMItemMng->find(ItemType);

			sprintf_s(strText, "%s %s", setOption.strSetName, pItemAtt->Name);

			success = true;

			if (pItemAtt->m_byItemSlot >= EQUIPMENT_WEAPON_RIGHT && pItemAtt->m_byItemSlot < MAX_EQUIPMENT)
			{
				ITEM* pItemE = &CharacterMachine->Equipment[pItemAtt->m_byItemSlot];

				if ((pItemAtt->m_byItemSlot == EQUIPMENT_WEAPON_RIGHT || pItemAtt->m_byItemSlot == EQUIPMENT_RING_RIGHT) 
					&& pItemE->Type != ItemType)
				{
					pItemE = &CharacterMachine->Equipment[pItemAtt->m_byItemSlot + 1];
				}


				if (pItemE->Type != -1)
				{
					setItemType = (pItemE->ExtOption % 0x04) - 1;

					if (setItemType == 0 || setItemType == 1)
					{
						BYTE btype = m_ItemSetType[pItemE->Type].byOption[setItemType];

						if (btype == subtype)
						{
							sprintf(TextList[TNum], strText);
							TextListColor[TNum] = TEXT_COLOR_GREEN;
							TextBold[TNum++] = 0;
						}
						else
						{
							sprintf(TextList[TNum], strText);
							TextListColor[TNum] = TEXT_COLOR_GRAY;
							TextBold[TNum++] = 0;
						}
					}
					else
					{
						sprintf(TextList[TNum], strText);
						TextListColor[TNum] = TEXT_COLOR_GRAY;
						TextBold[TNum++] = 0;
					}
				}
				else
				{
					sprintf(TextList[TNum], strText);
					TextListColor[TNum] = TEXT_COLOR_GRAY;
					TextBold[TNum++] = 0;
				}
			}
		}

		if(success)
			sprintf(TextList[TNum++], "\n");
	}
	return TNum;
}

int CSItemOption::RenderSetOptionListInItem(ITEM* pItem, int TextNum, bool bIsEquippedItem)
{
	int TNum = TextNum;

	int setItemType = (pItem->ExtOption % 0x04) - 1;

	m_ByLineIni = 0;
	m_ByLineEnd = 0;

	if (setItemType == 0 || setItemType == 1)
	{
		ITEM_SET_TYPE& itemSType = m_ItemSetType[pItem->Type];

		m_bySelectedItemOption = itemSType.byOption[setItemType];

		if (m_bySelectedItemOption > 0 && m_bySelectedItemOption != 0xFF)
		{
			ITEM_SET_OPTION& setOption = m_ItemSetOption[m_bySelectedItemOption];

			if (setOption.byOptionCount >= 0xFF)
			{
				m_bySelectedItemOption = 0;
				return TNum;
			}

			BYTE option1 = 0xFF;
			BYTE option2 = 0xFF;
			BYTE value1 = 0xFF;
			BYTE value2 = 0xFF;
			BYTE count1 = 0;
			BYTE byLimitOptionNum;

			if (m_bySetOptionANum > 0)
				byLimitOptionNum = m_bySetOptionANum - m_bySameSetItem;
			else
				byLimitOptionNum = 0;

			if (m_bySetOptionBNum > 0)
				byLimitOptionNum += m_bySetOptionBNum - m_bySameSetItem;

			count1 = Search_From_EquippedSetItemNameSequence(setOption.strSetName);

			if (count1 == 0xFF)
				byLimitOptionNum = 0;
			else
				byLimitOptionNum = abs((m_bySetOptionBNum + m_bySetOptionANum) - m_bySameSetItem);

			bool isfulloption = isFullseteffect(pItem);

			if (isfulloption)
			{
				byLimitOptionNum = 13;
			}

			if (gmProtect->shutdown_tooltipold)
			{
				TNum = GetSetItmeCount(pItem, TNum);
				m_ByLineIni = TNum;
			}
			else
			{
				sprintf(TextList[TNum++], "\n");
			}

			sprintf(TextList[TNum], GlobalText[159]);
			TextListColor[TNum] = TEXT_COLOR_YELLOW;
			TextBold[TNum++] = true;
			sprintf(TextList[TNum++], "\n");

			BYTE byCurrentSelectedSlotIndex = pItem->bySelectedSlotIndex;

			int iLimitOptionCount = m_arLimitSetItemOptionCount[byCurrentSelectedSlotIndex] - 1;

			int ItemOptionCount = 0;
			int ItemElementAdd = 2;

			for (int i = 0; i <= MAX_SETITEM_OPTIONS; ++i)
			{
				if (i >= 6)
				{
					if (i >= 8)
					{
						if (((pItem->ExtOption % 0x04) - 1) == 0)
						{
							option1 = setOption.byFullOption[i - 8];
							value1 = setOption.byFullOptionValue[i - 8];
						}
						else
						{
							option2 = setOption.byFullOption[i - 8];
							value2 = setOption.byFullOptionValue[i - 8];
						}
						byLimitOptionNum = (isfulloption) ? 13 : 0xFF;
						ItemOptionCount = 0;
					}
					else if (((pItem->ExtOption % 0x04) - 1) == 0)
					{
						option1 = setOption.byExtOption[i - 6];
						value1 = setOption.byExtOptionValue[i - 6];
					}
					else
					{
						option2 = setOption.byExtOption[i - 6];
						value2 = setOption.byExtOptionValue[i - 6];
					}
					ItemOptionCount = 0;
				}
				else
				{
					option1 = setOption.byStandardOption[i][0];
					option2 = setOption.byStandardOption[i][1];
					value1 = setOption.byStandardOptionValue[i][0];
					value2 = setOption.byStandardOptionValue[i][1];
					ItemOptionCount = (option1 != 0xFF || option2 != 0xFF) ? 1 : 0;
				}

				if (option1 != 0xFF)
				{
					if (gmProtect->shutdown_tooltipold)
					{
						if (ItemOptionCount == 1)
						{
							sprintf(TextList[TNum], GlobalText[877], ItemElementAdd);
							TextListColor[TNum] = TEXT_COLOR_GREEN;
							TextBold[TNum++] = 0;
							ItemElementAdd++;
						}
					}
					getExplainText(TextList[TNum], option1, value1, 0);

					if (m_bySetOptionList[count1][0] != option1 || byLimitOptionNum == 0xFF || iLimitOptionCount <= i || !byLimitOptionNum || bIsEquippedItem)
					{
						TextListColor[TNum] = TEXT_COLOR_GRAY;
					}
					else
					{
						TextListColor[TNum] = TEXT_COLOR_BLUE;
						count1++;
					}
					TextBold[TNum++] = false;
				}
				if (option2 != 0xFF)
				{
					getExplainText(TextList[TNum], option2, value2, 0);

					if (m_bySetOptionList[count1][0] == option2 && byLimitOptionNum != 0xFF && iLimitOptionCount > i && byLimitOptionNum)
					{
						TextListColor[TNum] = TEXT_COLOR_BLUE;
						count1++;
					}
					else
					{
						TextListColor[TNum] = TEXT_COLOR_GRAY;
					}
					TextBold[TNum++] = false;
				}
			}
			if (gmProtect->shutdown_tooltipold)
			{
				m_ByLineEnd = TNum;
			}
			sprintf(TextList[TNum++], "\n");
		}
	}

	return TNum;
}

void CSItemOption::ClearOptionHelper()
{
	m_byRenderOptionList = 0;
}

BYTE CSItemOption::GetSetOptionANum()
{
	return m_bySetOptionANum;
}

BYTE CSItemOption::GetSetOptionBNum()
{
	return m_bySetOptionBNum;
}

void CSItemOption::SetViewOptionList(bool bView)
{
	m_bViewOptionList = bView;
}

bool CSItemOption::IsViewOptionList()
{
	return m_bViewOptionList;
}
