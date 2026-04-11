#pragma once

typedef struct // (size:32)
{
/*+00*/	int index;
		float X;
		float Y;
		float Z;
///*+04*/	float pos[3];
/*+16*/	float angle[3];
/*+28*/	float scale;
} ITEM_POSITION;

typedef std::map<int, ITEM_POSITION> type_map_position;

class CScriptItem
{
public:
	CScriptItem();
	virtual~CScriptItem();

	void OpenData();
	ITEM_POSITION* FindItemRelative(int itemindex);
private:
	type_map_position mapitemPosition;
};

