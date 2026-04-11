#include "stdafx.h"
#include "supportingfeature.h"
#include "CGMItemWings.h"

CGMItemWings::CGMItemWings()
{
}

CGMItemWings::~CGMItemWings()
{
	data_wings.clear();
}

CGMItemWings* CGMItemWings::Instance()
{
	static CGMItemWings sInstance;
	return &sInstance;
}

void CGMItemWings::ReadData(FILE* fp)
{
	readVector(fp, this->data_wings);
}

void CGMItemWings::LoadItemModel()
{
	for (size_t i = 0; i < data_wings.size(); i++)
	{
		data_wings[i].OpenLoad();
	}
}

bool CGMItemWings::IsWingsByIndex(int nType)
{
	return (bool)(this->Find(nType) != NULL);
}

CUSTOM_WING_INFO* CGMItemWings::Find(int nType)
{
	auto it = std::find_if(data_wings.begin(), data_wings.end(), [&nType](const CUSTOM_WING_INFO& value) {
		return value.ItemIndex == nType;
		});

	return (it != data_wings.end()) ? &(*it) : NULL;
}
