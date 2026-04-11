#include "stdafx.h"
#include "NewUISystem.h"
#include "NewUIInventoryCore.h"

SEASON3B::CNewUIInventoryCore::CNewUIInventoryCore()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
}

SEASON3B::CNewUIInventoryCore::~CNewUIInventoryCore()
{
}

bool SEASON3B::CNewUIInventoryCore::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (pNewUIMng && g_pNewItemMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_INVENTORY_CORE, this);

		SetButtonInfo();

		SetPos(x, y);

		Show(false);

		return true;
	}
	return false;
}

void SEASON3B::CNewUIInventoryCore::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIInventoryCore::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;

#if MAIN_UPDATE > 603
	m_BtnExit.SetPos(m_Pos.x + 22, m_Pos.y + 384);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		SetButtonPosition(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
	}
	else
	{
		m_BtnExit.SetPos(m_Pos.x + 13, m_Pos.y + 391);
	}
#else
	SetButtonPosition(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
#endif

	g_pMyInventory->SetEquipementPosition(EQUIPMENT_WING_CORE, m_Pos.x + ((WIN_WINDOW_SIZEX - 61.f) * 0.5) + 2, m_Pos.y + 153);
}

const POINT& SEASON3B::CNewUIInventoryCore::GetPos() const
{
	return m_Pos;
}

bool SEASON3B::CNewUIInventoryCore::UpdateMouseEvent()
{
	if (m_BtnExit.UpdateMouseEvent())
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY_CORE);
		return false;
	}

	if (SEASON3B::IsPress(VK_LBUTTON)
		|| SEASON3B::IsRelease(VK_LBUTTON)
		|| SEASON3B::IsRepeat(VK_LBUTTON))
	{
		return (!CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY));
	}

	return true;
}

bool SEASON3B::CNewUIInventoryCore::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIInventoryCore::Update()
{
	return true;
}

bool SEASON3B::CNewUIInventoryCore::Render()
{
	EnableAlphaTest();

	glColor4f(1.0, 1.0, 1.0, 1.0);

	RenderFrame();

	RenderInven();

	RenderTexts();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

float SEASON3B::CNewUIInventoryCore::GetLayerDepth()
{
	return 4.15f;
}

void SEASON3B::CNewUIInventoryCore::OpeningProcess()
{
	g_pMyInventory->SetEquipementRender(EQUIPMENT_WING_CORE, true);
	g_pMyInventory->SetEquipementPosition(EQUIPMENT_WING_CORE, m_Pos.x + ((WIN_WINDOW_SIZEX - 61.f) * 0.5) + 2, m_Pos.y + 153);
}

void SEASON3B::CNewUIInventoryCore::ClosingProcess()
{
	g_pMyInventory->SetEquipementRender(EQUIPMENT_WING_CORE, false);
}

bool SEASON3B::CNewUIInventoryCore::CheckExpansionInventory()
{
	if (IsVisible())
		return CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY) != false;
	else
		return false;
}

void SEASON3B::CNewUIInventoryCore::SetButtonInfo()
{
#if MAIN_UPDATE > 603
	m_BtnExit.ChangeButtonImgState(true, BITMAP_btn_exit_00, false);
	m_BtnExit.ChangeButtonInfo(m_Pos.x + 22, m_Pos.y + 384, 20, 20);
	m_BtnExit.CurSize(69.f, 72.f);
#elif MAIN_UPDATE > 303
	if (gmProtect->checkold_school())
	{
		SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
	}
	else
	{
		m_BtnExit.ChangeButtonImgState(true, CNewUIMyInventory::IMAGE_INVENTORY_EXIT_BTN, false);
		m_BtnExit.ChangeButtonInfo(m_Pos.x + 13, m_Pos.y + 391, 36, 29);
	}
#else
	SetButtonInterface(&m_BtnExit, m_Pos.x, m_Pos.y, 1);
#endif
}

void SEASON3B::CNewUIInventoryCore::RenderFrame()
{
	SEASON3B::window_nomal(m_Pos.x, m_Pos.y, 4);

#if MAIN_UPDATE <= 603
	SEASON3B::RenderImageF(IMAGE_ITEM_DECOR_CORE, m_Pos.x + ((WIN_WINDOW_SIZEX - 180.f) * 0.5f), m_Pos.y + 27.f, 180.f, 177.f, 0.0, 6.0, 375.f, 369.f);
#endif

	RenderImage(CNewUIMyQuestInfoWindow::IMAGE_MYQUEST_LINE, (float)m_Pos.x + 1, (float)m_Pos.y + 190.f, 188.f, 21.f);

	int iPoint = CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_MINIMAPICON_HOLYITEM_PC;

	g_pNameWindow->RenderTooltip(((float)m_Pos.x + 20.f), ((float)m_Pos.y + 215.f), ((float)WIN_WINDOW_SIZEX - 40.f), 170.f, true);

	RenderBitmap(iPoint, ((float)m_Pos.x + 28.f), (float)m_Pos.y + 224.f, 8.f, 8.f, 0.f, 0.f, 14.f / 16.f, 14.f / 16.f);
}

void SEASON3B::CNewUIInventoryCore::RenderTexts()
{
	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->SetTextColor(CLRDW_GOLD);

	g_pRenderText->SetBgColor(0);

	g_pRenderText->RenderText(m_Pos.x + 40, m_Pos.y + 222, GlobalText[1840], 0, 12); //-- +Effect limitation

	/*(2 Opciones Adicionales Aleatorias)*/
	/*int RenderFrameY = m_Pos.y + 245;

	int next_line = 10;
	g_pRenderText->SetTextColor(CLRDW_GREEN);
	g_pRenderText->SetFont(g_hFont);

	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Attack Power 170", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Defense Elemental 100", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Attack Success Rate 125", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Defense Success Rate 125", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Attack Power (II) 240", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Defense Elemental (II) 210", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Attack Success Rate (II) 200", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Elemental Defense Success Rate (II) 200", 0, 12);
	RenderFrameY += next_line;

	g_pRenderText->SetTextColor(0x06, 0xce, 0xea, 0xff);
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Extra Nuke Damage 80", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Extra Bolt Damage 80", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Extra AoE Damage 80", 0, 12);
	RenderFrameY += next_line;
	g_pRenderText->RenderText(m_Pos.x + 35, RenderFrameY, "Increase Extra Damage Reduction 50", 0, 12);
	*/

}

void SEASON3B::CNewUIInventoryCore::RenderButtons()
{
	m_BtnExit.Render();
}

void SEASON3B::CNewUIInventoryCore::RenderInven()
{
	g_pNameWindow->RenderTooltip(((float)m_Pos.x + 30.f), ((float)m_Pos.y + 240.f), ((float)WIN_WINDOW_SIZEX - 60.f), 130.f, true);
}
