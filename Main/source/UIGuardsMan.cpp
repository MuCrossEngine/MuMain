//////////////////////////////////////////////////////////////////////////
//  UIGuardsMan.cpp
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "wsclientinline.h"
#include "UIGuildInfo.h"
#include "UIGuardsMan.h"
#include "NewUIMyInventory.h"

CUIGuardsMan g_GuardsMan;

CUIGuardsMan::CUIGuardsMan()
{
	m_dwRegMark		= 0;
	m_eRegStatus	= REG_STATUS_NONE;
}

CUIGuardsMan::~CUIGuardsMan()
{
}

bool CUIGuardsMan::IsSufficentDeclareLevel()
{
	if( Hero->GuildStatus != G_MASTER )
		return false;

	if( CharacterAttribute->Level >= BC_REQ_LEVEL )
	{
		return true;
	}
	else
	{
		return false;
	}
}

DWORD CUIGuardsMan::GetMyMarkCount()
{
	DWORD dwResult = 0;
	ITEM* pItem = NULL;

	for (int i = 0; i < (int)g_pMyInventory->GetNumberOfItems(); ++i)
	{
		pItem = g_pMyInventory->GetItem(i);

		if (!pItem) continue;

		int nItemLevel = (pItem->Level >> 3) & 15;
		if (pItem->Type == ITEM_POTION + 21 && nItemLevel == 3)
		{
			dwResult += pItem->Durability;
		}
	}

	return dwResult;
}

int CUIGuardsMan::GetMyMarkSlotIndex()
{
	ITEM* pItem = NULL;

	SEASON3B::CNewUIInventoryCtrl* pNewInventoryCtrl = g_pMyInventory->GetInventoryCtrl(0);

	for (int i = 0; g_pMyInventory->GetInvenEnableCnt(); ++i)
	{
		pNewInventoryCtrl = g_pMyInventory->GetInventoryCtrl(i);

		for (int j = 0; j < (int)pNewInventoryCtrl->GetNumberOfItems(); ++j)
		{
			pItem = pNewInventoryCtrl->GetItem(j);
			int nItemLevel = (pItem->Level >> 3) & 15;
			if (pItem->Type == ITEM_POTION + 21 && nItemLevel == 3)
			{
				return g_pMyInventory->GetInvenSlotIndex(pItem->x, pItem->y, i);
			}
		}
	}

	return -1;
}

