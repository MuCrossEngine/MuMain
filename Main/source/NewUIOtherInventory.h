#pragma once
#include "NewUIBase.h"
#include "NewUIInventoryCtrl.h"
#include "NewUIMessageBox.h"
#include "NewUI3DRenderMng.h"
#include "NewUIButton.h"

namespace SEASON3B
{
	class CNewUIOtherInventory : public CNewUIObj
	{
	private:
		CNewUIManager* m_pNewUIMng;
		POINT m_Pos;
		EQUIPMENT_ITEM m_EquipmentSlots[MAX_NEW_EQUIPMENT];
		int	m_iPointedSlot;

	public:
		CNewUIOtherInventory();
	};

}