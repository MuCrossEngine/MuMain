#pragma once
#include "_struct.h"

struct nInformation
{
	int Index;
	int ItemIndexMin;
	int ItemIndexMax;
	int LevelMin;
	int LevelMax;
	int Exc;
	int Anc;
	//--
	char OptionName[32];
	int OptionColor;
	//--
	int TextIndex[15];
	int TextColor[15];
	int LevelItem[15];
};

struct nText
{
	int Index;
	char Text[254];
};

class nCInfo
{
public:
	nCInfo();
	virtual ~nCInfo();
	void loadnInformation(nInformation* info);
	void loadnInformationSet(nInformation* info);
	void loadnText(nText* info);
	int nInformationOP(int TextNum, ITEM* ItemSearch);
	void IniInformation();
public:
	std::vector<nInformation> m_CustomInfo;
	std::vector<nInformation> m_CustomInfoSet;
	std::map<int, nText> m_CustomDescripcionInfo;
};

extern nCInfo GInfo;