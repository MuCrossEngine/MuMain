#include "stdafx.h"
#include "ItemTooltip.h"
#include "Util.h"
#include "CharacterManager.h"
#include "ZzzCharacter.h"
#define ITEM(x, y)					((x * 512) + y)
nCInfo GInfo;


nCInfo::nCInfo()
{
	this->m_CustomInfo.clear();
	this->m_CustomInfoSet.clear();
	this->m_CustomDescripcionInfo.clear();
}

nCInfo::~nCInfo()
{
}

void nCInfo::loadnInformation(nInformation* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].ItemIndexMin == -1 || info[n].ItemIndexMax == -1)
		{
			return;
		}
		//this->m_CustomInfo.insert(std::pair<int, nInformation>(info[n].ItemIndex, info[n]));
		this->m_CustomInfo.push_back(info[n]);
	}
}
void nCInfo::loadnInformationSet(nInformation* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].ItemIndexMin == -1 || info[n].ItemIndexMax == -1)
		{
			return;
		}
		//this->m_CustomInfo.insert(std::pair<int, nInformation>(info[n].ItemIndex, info[n]));
		this->m_CustomInfoSet.push_back(info[n]);
	}
}
void nCInfo::loadnText(nText* info) // OK
{
	for (int n = 0; n < MaxLineItemToolTip; n++)
	{
		if (info[n].Index < 0 || info[n].Index > 7680)
		{
			return;
		}
		this->m_CustomDescripcionInfo.insert(std::pair<int, nText>(info[n].Index, info[n]));
	}
}

bool nGetTextNum(int TextIndex)
{
	std::map<int, nText>::iterator it = GInfo.m_CustomDescripcionInfo.find(TextIndex);

	if (it != GInfo.m_CustomDescripcionInfo.end())
	{
		return 1;
	}

	return 0;
}
char* nGetTextLine(int TextIndex)
{
	std::map<int, nText>::iterator it = GInfo.m_CustomDescripcionInfo.find(TextIndex);

	if (it != GInfo.m_CustomDescripcionInfo.end())
	{
		return it->second.Text;
	}

	return "no found";
}
extern	char    TextList[90][100];
extern	int     TextListColor[90];
extern	int     TextBold[90];
bool CheckItemSlotOpt(int IndexItem, int Level, int Exl)
{
	int CountSlot = 0;
	BYTE byFirstClass = gCharacterManager.GetBaseClass(Hero->Class);

	int GetIndexSlot[6] =
	{
		Hero->BodyPart[BODYPART_HELM].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_ARMOR].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_PANTS].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_GLOVES].Type - MODEL_ITEM,
		Hero->BodyPart[BODYPART_BOOTS].Type - MODEL_ITEM,
	};
	int GetLevelSlot[6] =
	{
	   Hero->BodyPart[BODYPART_HELM].Level,
		Hero->BodyPart[BODYPART_ARMOR].Level,
		Hero->BodyPart[BODYPART_PANTS].Level,
		Hero->BodyPart[BODYPART_GLOVES].Level,
		Hero->BodyPart[BODYPART_BOOTS].Level,
	};
	int GetExlSlot[6] =
	{
	   Hero->BodyPart[BODYPART_HELM].Option1,
		Hero->BodyPart[BODYPART_ARMOR].Option1,
		Hero->BodyPart[BODYPART_PANTS].Option1,
		Hero->BodyPart[BODYPART_GLOVES].Option1,
		Hero->BodyPart[BODYPART_BOOTS].Option1,
	};
	for (int Slot = 0; Slot < 6; Slot++)
	{
		if ((int)GetIndexSlot[Slot] == (int)ITEM((7 + Slot), IndexItem)
			&& Level <= GetLevelSlot[Slot] && Exl <= GetExlSlot[Slot])
		{
			CountSlot++;
		}
	}
	if (CountSlot >= 5 || (CountSlot >= 4 && (byFirstClass == 3 || byFirstClass == 6)))
	{
		return 1;
	}
	return 0;
}
#include "CBInterface.h"
int nCInfo::nInformationOP(int TextNum, ITEM* ItemSearch)
{

	//std::map<int, nInformation>::iterator it = GInfo.m_CustomInfo.find(ItemSearch);

	//if (it != GInfo.m_CustomInfo.end())
	//{

	//	wsprintf(TextList[TextNum], it->second.OptionName);
	//	TextListColor[TextNum] = it->second.OptionColor;
	//	TextBold[TextNum++] = 1;

	//	wsprintf(TextList[TextNum++], "\n");

	//	for (int i = 0; i < 15; i++)
	//	{
	//		if (it->second.TextIndex[i] == -1) { continue; }
	//		wsprintf(TextList[TextNum], "%s", nGetTextLine(it->second.TextIndex[i]));
	//		TextListColor[TextNum] = it->second.TextColor[i];
	//		TextBold[TextNum++] = 0;
	//	}
	//}
	//wsprintf(TextList[TextNum++], "\n");
	bool SetColorGray = false;

	for (int i = 0; i < this->m_CustomInfoSet.size(); i++)
	{
		if (this->m_CustomInfoSet[i].ItemIndexMin == -1 || this->m_CustomInfoSet[i].ItemIndexMax == -1) continue;

		if (ItemSearch->Type < (MODEL_HELM - 1171) || ItemSearch->Type >= (MODEL_WING - 1171)) continue;
		int GetIndexItemSet = (ItemSearch->Type % 512);

		//gInterface.DrawMessage(1, "Get Set %d ~ %d/%d ", ItemSearch->Type, this->m_CustomInfoSet[i].ItemIndexMin, this->m_CustomInfoSet[i].ItemIndexMax);
		if (GetIndexItemSet >= this->m_CustomInfoSet[i].ItemIndexMin && GetIndexItemSet <= this->m_CustomInfoSet[i].ItemIndexMax)
		{

			wsprintf(TextList[TextNum], this->m_CustomInfoSet[i].OptionName);
			TextListColor[TextNum] = this->m_CustomInfoSet[i].OptionColor;
			TextBold[TextNum++] = 1;

			wsprintf(TextList[TextNum++], "\n");
			for (int ib = 0; ib < 15; ib++)
			{
				if (!nGetTextNum(this->m_CustomInfoSet[i].TextIndex[ib])) { continue; }

				if (CheckItemSlotOpt(GetIndexItemSet, this->m_CustomInfoSet[i].LevelItem[ib], this->m_CustomInfoSet[i].Exc))
				{
					//g_Console.AddMessage(1, "Class %d Set %d CountSlot  %d Exl %d %d ", byFirstClass, GetIndexItemSet, CountSlot, (int)ItemSearch->ExcellentOption, it->second.Exc);
					SetColorGray = false;
				}
				else
				{
					SetColorGray = true;
				}

				wsprintf(TextList[TextNum], "%s", nGetTextLine(this->m_CustomInfoSet[i].TextIndex[ib]));

				TextListColor[TextNum] = (SetColorGray) ? 10 : this->m_CustomInfoSet[i].TextColor[ib];
				TextBold[TextNum++] = 0;
			}
			wsprintf(TextList[TextNum++], "\n");
		}
	}

	for (int iT = 0; iT < this->m_CustomInfo.size(); iT++)
	{
		int Level = (ItemSearch->Level >> 3) & 15;
		if (ItemSearch->Type < this->m_CustomInfo[iT].ItemIndexMin || ItemSearch->Type > this->m_CustomInfo[iT].ItemIndexMax) continue;
		if ((Level < this->m_CustomInfo[iT].LevelMin && this->m_CustomInfo[iT].LevelMin != -1) || (Level > this->m_CustomInfo[iT].LevelMax && this->m_CustomInfo[iT].LevelMax != -1))		continue;
		if (this->m_CustomInfo[iT].Anc != -1 && (ItemSearch->ExtOption % 0x04) == 0)		continue;



		wsprintf(TextList[TextNum], this->m_CustomInfo[iT].OptionName);
		TextListColor[TextNum] = this->m_CustomInfo[iT].OptionColor;
		TextBold[TextNum++] = 1;

		wsprintf(TextList[TextNum++], "\n");

		//gInterface.DrawMessage(1, "Get %d ~ %d ", ItemSearch->Type, (int)((ItemSearch->Level >> 3) & 15));

		for (int ibx = 0; ibx < 15; ibx++)
		{
			if (!nGetTextNum(this->m_CustomInfo[iT].TextIndex[ibx])) { continue; }

			if ((this->m_CustomInfo[iT].LevelItem[ibx] > (int)((ItemSearch->Level >> 3) & 15) && this->m_CustomInfo[iT].LevelItem[ibx] != -1) || ((int)ItemSearch->Option1 < this->m_CustomInfo[iT].Exc && this->m_CustomInfo[iT].Exc != -1))
			{
				SetColorGray = true;
			}
			else
			{
				SetColorGray = false;
			}
			wsprintf(TextList[TextNum], "%s", nGetTextLine(this->m_CustomInfo[iT].TextIndex[ibx]));

			TextListColor[TextNum] = (SetColorGray) ? 10 : this->m_CustomInfo[iT].TextColor[ibx];
			TextBold[TextNum++] = 0;
		}
		wsprintf(TextList[TextNum++], "\n");


	}

	return TextNum;
}


