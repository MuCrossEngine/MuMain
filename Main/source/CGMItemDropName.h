#pragma once

class CGMItemDropName
{
public:
	CGMItemDropName();
	~CGMItemDropName();

	static CGMItemDropName* Instance();

	const char* GetIDColor(int Style);
	void AddInfoUI(ITEM_t* tItem);
	void RemoveInfoUI(int index);
	void SetUIPos(int index, float _ScreenX, float _ScreenY);
	void SetHighestDepth(int index);
	void SetVisibleInfo(int _iKey, bool _bVisible);
};

#define GMItemDropName					(CGMItemDropName::Instance())