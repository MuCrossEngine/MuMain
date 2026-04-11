#pragma once
#include "supportingfeature.h"

class CGMJewelOfAction
{
public:
	CGMJewelOfAction();
	virtual~CGMJewelOfAction();
	void ReadData(FILE* fp);
	void LoadItemModel();
	__int8 GetOptCount(int Option);
	__int8 GetExcCount(int Option);
	bool CheckOfJewelByItem(int ItemIndex);
	bool CheckOfJewelReinforce(int sorceIndex, const ITEM* pItem);


	static CGMJewelOfAction* Instance();
private:
	const CUSTOM_JEWEL_INFO* find(int ItemIndex);
	std::vector<CUSTOM_JEWEL_INFO> pack_of_jewel;
};

#define GMJewelOfAction				(CGMJewelOfAction::Instance())