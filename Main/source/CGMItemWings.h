#pragma once
class CUSTOM_WING_INFO;

class CGMItemWings
{
public:
	CGMItemWings();
	virtual~CGMItemWings();
	static CGMItemWings* Instance();
	void ReadData(FILE* fp);
	void LoadItemModel();
	bool IsWingsByIndex(int nType);
	CUSTOM_WING_INFO* Find(int nType);
private:
	std::vector<CUSTOM_WING_INFO> data_wings;
};


#define GMItemWings			(CGMItemWings::Instance())