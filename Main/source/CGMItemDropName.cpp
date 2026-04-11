#include "stdafx.h"
#include "ScaleForm.h"
#include "NewUISystem.h"
#include "CGMItemDropName.h"

CGMItemDropName::CGMItemDropName()
{
}

CGMItemDropName::~CGMItemDropName()
{
}

CGMItemDropName* CGMItemDropName::Instance()
{
	static CGMItemDropName s_Intance;
	return &s_Intance;
}

const char* CGMItemDropName::GetIDColor(int Style)
{
	const char* HexColor;

	switch (Style)
	{
	case 0:
		HexColor = "#FFFFFF";
		break;
	case 1:
		HexColor = "#7FB2FF";
		break;
	case 2:
		HexColor = "#FF3319";
		break;
	case 3:
		HexColor = "#FFCC19";
		break;
	case 4:
		HexColor = "#19FF7F";
		break;
	case 5:
		HexColor = "#A00000";
		break;
	case 6:
		HexColor = "#FF19FF";
		break;
	case 7:
		HexColor = "#0000A0";
		break;
	case 8:
		HexColor = "#A06600";
		break;
	case 9:
		HexColor = "#00FF00";
		break;
	case 10:
		HexColor = "#B2B2B2";
		break;
	case 11:
		HexColor = "#CC7FCC";
		break;
	case 12:
		HexColor = "#B266FF";
		break;
	case 13:
		HexColor = "#E56B0A";
		break;
	case 14:
		HexColor = "#800080";
		break;
	case 15:
		HexColor = "#FFFF00";
		break;
	case 16:
		HexColor = "#00B0F0";
		break;
	case 17:
		HexColor = "#00B050";
		break;
	case 18:
		HexColor = "#CCFF00";
		break;
	case 19:
		HexColor = "#00FFFF";
		break;
	case 20:
		HexColor = "#BCBCBC";
		break;
	case 21:
		HexColor = "#C1C469";
		break;
	default:
		HexColor = "#FFFFFF";
		break;
	}

	return HexColor;
}

void CGMItemDropName::AddInfoUI(ITEM_t* tItem)
{

}

void CGMItemDropName::RemoveInfoUI(int index)
{
}

void CGMItemDropName::SetUIPos(int index, float _ScreenX, float _ScreenY)
{
}

void CGMItemDropName::SetHighestDepth(int index)
{
}

void CGMItemDropName::SetVisibleInfo(int _iKey, bool _bVisible)
{
}
