#include "stdafx.h"
#include "ScriptItem.h"

CScriptItem::CScriptItem()
{
	this->OpenData();
}

CScriptItem::~CScriptItem()
{
	mapitemPosition.clear();
}

void CScriptItem::OpenData()
{
	char filename[MAX_PATH];

	sprintf_s(filename, "Data\\Local\\ItemTRSData.bmd");

	int Size = PackFileDecrypt<ITEM_POSITION>(filename, mapitemPosition, 0, sizeof(ITEM_POSITION), 0xE2F1);

	if (Size == 0)
	{
		SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
	}
}

ITEM_POSITION* CScriptItem::FindItemRelative(int itemindex)
{
	type_map_position::iterator it = mapitemPosition.find(itemindex);

	if (it != mapitemPosition.end())
	{
		return &it->second;
	}
	else
	{
		static ITEM_POSITION info;
		info.index = itemindex;
		info.X = 0.5;
		info.Y = 0.8;
		info.Z = 0.0;
		info.scale = 0.0039;

		if (itemindex >= ITEM_SWORD && itemindex < ITEM_BOW)
		{
			Vector(180.0, 270, 15.0, info.angle);
		}
		else if (itemindex >= ITEM_BOW && itemindex < ITEM_STAFF)
		{
			Vector(180.0, -90, 15.0, info.angle);
		}
		else
		{
			Vector(270, -10.0, 0, info.angle);
		}
		return &info;
	}
}
