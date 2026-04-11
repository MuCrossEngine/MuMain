#include "stdafx.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "NewUISystem.h"
#include "ZzzInventory.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "NewUIMixExpansion.h"

SEASON3B::CNewUIMixExpansion::CNewUIMixExpansion()
	: m_pdwItemCheckBox(MIX_INVENTORY_SIZE, 0), m_vecItem(MIX_INVENTORY_SIZE)
{
	this->m_pNewUIMng = NULL;
	this->m_Pos.x = 0;
	this->m_Pos.y = 0;
	this->m_iNumItemCnt = 0;
	this->m_iNumMixIndex = -1;

	this->m_iRenderItem = -1;
	this->m_iTargetItem = -1;
	this->m_iRenderTooltipX = -1;
	this->m_iRenderTooltipY = -1;

	this->m_bItemAutoMove = 0;
	this->m_BackupTargetItem = -1;

	this->m_iSuccessRate = 100;
	this->m_iTalismanOfLuck = 0x1fff;
	this->m_iTalismanOfChaos = 0x1fff;
	this->m_iRequiredType = 0;
	this->m_iRequiredZen = 100000000;

	this->m_mixOptionList = NULL;
	this->m_iMixState = 0;
	this->m_StateNormal = 0;
	this->m_iTalismanLuckRate = 0;
	this->m_iSuccessAddRate = 0;
}

SEASON3B::CNewUIMixExpansion::~CNewUIMixExpansion()
{
	this->Release();

	mixRecipient.clear();

	m_InfoBtnCnt.clear();

	this->DeleteAllItems();
}

bool SEASON3B::CNewUIMixExpansion::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (pNewUIMng)
	{
		this->m_pNewUIMng = pNewUIMng;

		this->m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MIX_EXPANSION, this);

		this->SetButtonInfo();

		this->SetPos(x, y);

		this->Show(false);
	}

	return pNewUIMng != NULL;
}

void SEASON3B::CNewUIMixExpansion::Release()
{
	if (this->m_pNewUIMng)
	{
		this->m_pNewUIMng->RemoveUIObj(this);
	}

	SAFE_DELETE(this->m_mixOptionList);

	(&this->m_pdwItemCheckBox)->clear();
}

void SEASON3B::CNewUIMixExpansion::SetPos(int x, int y)
{
	this->m_Pos.x = x;
	this->m_Pos.y = y;
	this->m_mixOptionList->SetPosition(m_Pos.x + 41, m_Pos.y + 216);
	this->m_BtnMix.SetPos(m_Pos.x + (WIN_WINDOW_SIZEX - 64) / 2.f, m_Pos.y + 382.f);

	m_pScroll.SetPos(m_Pos.x + 155, m_Pos.y + 216);
}

bool SEASON3B::CNewUIMixExpansion::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MIX_EXPANSION);

			if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INVENTORY))
			{
				g_pNewUISystem->Hide(SEASON3B::INTERFACE_INVENTORY);
			}
			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUIMixExpansion::UpdateMouseEvent()
{
	m_StateNormal = 0;

	if (SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY))
	{
		if (m_iNumMixIndex == -1)
		{
			if (m_BtnMix.UpdateMouseEvent())
			{
				if (m_iNumMixIndex != m_mixOptionList->GetCurrent())
				{
					m_iNumMixIndex = m_mixOptionList->GetCurrent();
					this->SendRequestInfo(m_iNumMixIndex, 0);
				}
			
				PlayBuffer(25, 0, 0);
				return false;
			}
			m_mixOptionList->UpdateMouseEvent();
		}
		else
		{
			if (this->InventoryProcess())
			{
				return false;
			}
			this->ButtonProcess();
		}
	}

	return !(SEASON3B::CheckMouseIn(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY));
}

bool SEASON3B::CNewUIMixExpansion::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.0, 1.0, 1.0, 1.0);

	this->RenderFrame();

	this->RenderText();

	this->RenderButton();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUIMixExpansion::Update()
{
	if (IsVisible())
	{
		//if (m_iNumMixIndex != -1)
		//{
		//	m_mixOptionList->UpdateMouseEvent();
		//}
	}
	return true;
}

float SEASON3B::CNewUIMixExpansion::GetLayerDepth()
{
	return 4.3f;
}

void SEASON3B::CNewUIMixExpansion::OpeningProcess()
{
}

bool SEASON3B::CNewUIMixExpansion::ClosingProcess()
{
	m_iNumMixIndex = -1;
	return true;
}

int SEASON3B::CNewUIMixExpansion::FindEmptySlot()
{
	for (int i = 0; i < MIX_INVENTORY_SIZE; i++)
	{
		if (m_pdwItemCheckBox[i] == 0)
		{
			return i;
		}
	}
	return -1;
}

bool SEASON3B::CNewUIMixExpansion::InsertItem(int iLinealPos, BYTE* pbyItemPacket)
{
	if (iLinealPos < 0 || iLinealPos >= MIX_INVENTORY_SIZE)
		return false;

	ITEM* pNewItem = g_pNewItemMng->CreateItem(pbyItemPacket);

	if (pNewItem)
	{
		if (m_pdwItemCheckBox[iLinealPos] == 0)
		{
			m_iNumItemCnt++;
			m_vecItem[iLinealPos].pItemObj = (pNewItem);
			m_pdwItemCheckBox[iLinealPos] = pNewItem->Key;

			char Name[254];
			int iLevel = (pNewItem->Level >> 3) & 0xF;
			GetItemName(pNewItem->Type, iLevel, Name);
			m_vecItem[iLinealPos].m_ItemName = Name;


			if (this->m_iTalismanOfLuck != 0x1FFF && this->m_iTalismanOfLuck == pNewItem->Type)
				m_iSuccessAddRate += m_iTalismanLuckRate;

			this->RecipientCheck();
			return true;
		}
		else
		{
			g_pNewItemMng->DeleteItem(pNewItem);
		}
	}
	return false;
}

void SEASON3B::CNewUIMixExpansion::DeleteItem(int iLinealPos, bool Success)
{
	if (m_pdwItemCheckBox[iLinealPos] != 0)
	{
		m_iNumItemCnt--;
		m_pdwItemCheckBox[iLinealPos] = 0;


		if (this->m_iTalismanOfLuck != 0x1FFF && this->m_iTalismanOfLuck == m_vecItem[iLinealPos].pItemObj->Type)
		{
			if ((short)(m_iSuccessAddRate - m_iTalismanLuckRate) >= 0)
			{
				m_iSuccessAddRate -= m_iTalismanLuckRate;
			}
		}

		g_pNewItemMng->DeleteItem(m_vecItem[iLinealPos].pItemObj);

		if (Success == true)
		{
			this->RecipientCheck();
		}

		m_iRenderItem = -1;
		m_iTargetItem = -1;
	}
}

void SEASON3B::CNewUIMixExpansion::DeleteAllItems()
{
	for (int i = 0; i < MIX_INVENTORY_SIZE; i++)
	{
		this->DeleteItem(i, false);
	}
	m_iNumItemCnt = 0;
	m_iRenderItem = -1;
	m_iTargetItem = -1;
	m_pdwItemCheckBox.resize(MIX_INVENTORY_SIZE, 0);
}

bool SEASON3B::CNewUIMixExpansion::IsReadyToMix()
{
	int complete = 0;
	for (size_t i = 0; i < mixRecipient.size(); i++)
	{
		if (mixRecipient[i].SpecialItem == MIX_SOURCE_YES)
		{
			complete++;
		}
	}

	return complete == mixRecipient.size();
}

void SEASON3B::CNewUIMixExpansion::SetButtonInfo()
{
	this->m_mixOptionList = new CNewUIListGroupButton;
	this->m_mixOptionList->Create(m_Pos.x + 155, m_Pos.y + 216, 5, BITMAP_btn_empty, 108, 29, 8);
	this->m_mixOptionList->SetCurrent(this->m_iNumMixIndex);

	this->m_BtnMix.ChangeButtonImgState(true, CNewUIMessageBoxMng::IMAGE_MSGBOX_BTN_EMPTY_SMALL, true, false, false);
	this->m_BtnMix.ChangeButtonInfo(m_Pos.x + 63, m_Pos.y + 385, 64, 29);
	this->m_BtnMix.ChangeText(GlobalText[228]);

	this->m_pScroll.Create(m_Pos.x + 145, m_Pos.y + 216, 150);
	this->m_pScroll.SetPercent(0);
}

bool SEASON3B::CNewUIMixExpansion::ButtonProcess()
{
	if (this->m_iNumMixIndex != -1)
	{
		if (SEASON3B::CheckMouseIn(m_Pos.x + 25, m_Pos.y + 215, 160, 160) == true)
		{
			if (m_iNumItemCnt > MAX_ITEM_VIEW)
			{
				double prev = m_pScroll.GetPercent();
				if (MouseWheel <= 0)
				{
					if (MouseWheel < 0)
					{
						MouseWheel = 0;
						prev += 0.1;
						if (prev > 1.0)
							prev = 1.0;
						m_pScroll.SetPercent(prev);
					}
				}
				else
				{
					MouseWheel = 0;
					prev -= 0.1;
					if (prev < 0.0)
						prev = 0.0;
					m_pScroll.SetPercent(prev);
				}
			}
		}
		if (m_BtnMix.UpdateMouseEvent())
		{
			if (this->IsReadyToMix())
			{
				//g_ConsoleDebug->Write(5, "estamos enviando el mix: %d", m_iNumMixIndex);
				this->SendRequestInfo(m_iNumMixIndex, 1);
			}
			return true;
		}
		m_pScroll.UpdateMouseEvent();
	}

	return false;
}

bool SEASON3B::CNewUIMixExpansion::InventoryProcess()
{
	if (SEASON3B::CheckMouseIn(m_Pos.x + 25, m_Pos.y + 215, 160, 160) == true)
	{
		if (g_pMyInventory->GetRepairMode() != 1 && EquipmentItem == false)
		{
			CNewUIPickedItem* pPickedItem = CNewUIInventoryCtrl::GetPickedItem();

			if (pPickedItem)
			{
				ITEM* pItemObj = pPickedItem->GetItem();

				if (pItemObj && this->IsMixItem(pItemObj))
				{
					if (SEASON3B::IsPress(VK_LBUTTON))
					{
						int m_iDstIndex = this->FindEmptySlot();

						if (pPickedItem->GetOwnerInventory() == NULL && pItemObj->ex_src_type == ITEM_EX_SRC_EQUIPMENT)
						{
							int iSourceIndex = pPickedItem->GetSourceLinealPos();
							SendRequestEquipmentItem(0, iSourceIndex, pItemObj, 23, m_iDstIndex);
						}
						else
						{
							int InvenIndex = g_pMyInventory->IsInvenctrlSame(pPickedItem->GetOwnerInventory());

							if (InvenIndex != -1 && m_iDstIndex != -1)
							{
								int iSourceIndex = pPickedItem->GetSourceLinealPos();
								iSourceIndex = g_pMyInventory->SlotCntSendSrcInvenIndex(iSourceIndex, InvenIndex);
								SendRequestEquipmentItem(0, iSourceIndex + 12, pItemObj, 23, m_iDstIndex);
							}
						}
					}
				}
				else
				{
					m_StateNormal = ARGB(0xff, 0xff, 0, 0);
				}
			}
			else
			{
				if (SEASON3B::IsPress(VK_RBUTTON) && m_iTargetItem != -1 && m_iTargetItem == m_iRenderItem && !this->IsItemAutoMove())
				{
					if (m_pdwItemCheckBox[m_iTargetItem] != 0)
					{
						ITEM* pItemObj = m_vecItem[m_iTargetItem].pItemObj;
				
						int m_iDstIndex = g_pMyInventory->FindEmptySlot(pItemObj, false);
				
						if (m_iDstIndex != -1)
						{
							m_bItemAutoMove = TRUE;
							m_BackupTargetItem = m_iTargetItem;
							SendRequestEquipmentItem(23, m_iTargetItem, pItemObj, 0, m_iDstIndex + 12);
				
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

void SEASON3B::CNewUIMixExpansion::RecipientCheck()
{
	for (size_t i = 0; i < mixRecipient.size(); i++)
	{
		mixRecipient[i].iNumMixItem = 0;
		mixRecipient[i].SpecialItem = MIX_SOURCE_ERROR;
	
		for (size_t j = 0; j < MIX_INVENTORY_SIZE; j++)
		{
			if (m_pdwItemCheckBox[j] != 0)
			{
				mixRecipient[i].CheckRecipe(m_vecItem[j].pItemObj);
			}
		}
	}
}

void SEASON3B::CNewUIMixExpansion::RenderFrame()
{
	SEASON3B::window_nomal(m_Pos.x, m_Pos.y, WIN_WINDOW_SIZEX, WIN_WINDOW_SIZEY);
}

void SEASON3B::CNewUIMixExpansion::RenderText()
{
	int RenderFrameX = m_Pos.x;
	int RenderFrameY = m_Pos.y;

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(-1);
	g_pRenderText->SetFont(g_hFontBold);

	RenderFrameY += 15;
	g_pRenderText->RenderFont(RenderFrameX + WIN_WINDOW_SIZEX / 2, RenderFrameY, "Mix Expansion", 0, 0, RT3_WRITE_CENTER);

	RenderFrameY += 10;

	if (m_iNumMixIndex == -1)
	{
		g_pRenderText->SetTextColor(255, 128, 0, 255);
		g_pRenderText->RenderFont(RenderFrameX + WIN_WINDOW_SIZEX / 2, RenderFrameY, GlobalText[734], 0, 0, RT3_WRITE_CENTER); RenderFrameY += 30;
		RenderFrameX += 20;
		g_pRenderText->SetTextColor(CLRDW_GREEN);

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, "- Always remember to get inventory space", 0, 0, 1); RenderFrameY += 10;
		g_pRenderText->RenderFont(RenderFrameX + 5, RenderFrameY, "to get the resulting item", 0, 0, 1); RenderFrameY += 15;

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, "- The recommended space is 4x4", 0, 0, 1); RenderFrameY += 15;

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, "- Remember that failing the combination ", 0, 0, 1); RenderFrameY += 10;
		g_pRenderText->RenderFont(RenderFrameX + 5, RenderFrameY, "can lose the items.", 0, 0, 1); RenderFrameY += 15;

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, "- The probability of success can increase ", 0, 0, 1); RenderFrameY += 10;
		g_pRenderText->RenderFont(RenderFrameX + 5, RenderFrameY, "if you use the appropriate items", 0, 0, 1);
	}
	else
	{
		char pszText[255];
		memset(pszText, 0, sizeof(pszText));

		if ((size_t)m_iNumMixIndex < m_InfoBtnCnt.size())
		{
			g_pRenderText->SetTextColor(255, 128, 0, 255);
			g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, m_InfoBtnCnt[m_iNumMixIndex].iName, 190, 0, 3); RenderFrameY += 25;
		}

		RenderFrameX += 20;
		g_pRenderText->SetTextColor(CLRDW_RED);
		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, GlobalText[601], 0, 0, 1); RenderFrameY += 10;

		g_pRenderText->SetTextColor(CLRDW_YELLOW2);

		if ((m_iSuccessRate + m_iSuccessAddRate) > 100)
		{
			sprintf_s(pszText, GlobalText[584], GlobalText[591], 100);
		}
		else
		{
			sprintf_s(pszText, GlobalText[584], GlobalText[591], (m_iSuccessRate + m_iSuccessAddRate));
		}

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, pszText, 0, 0, 1); RenderFrameY += 10;

		if (m_iRequiredType == 1)
			sprintf_s(pszText, "%s Coin1: (%d)", GlobalText[1372], m_iRequiredZen);
		else if (m_iRequiredType == 2)
			sprintf_s(pszText, "%s Coin2: (%d)", GlobalText[1372], m_iRequiredZen);
		else if (m_iRequiredType == 3)
			sprintf_s(pszText, "%s Coin3: (%d)", GlobalText[1372], m_iRequiredZen);
		else
			sprintf_s(pszText, "%s %s: (%d)", GlobalText[1372], GlobalText[224], m_iRequiredZen);

		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, pszText, 0, 0, 1); RenderFrameY += 20;

		g_pRenderText->SetTextColor(CLRDW_GREEN);
		g_pRenderText->RenderFont(RenderFrameX, RenderFrameY, GlobalText[2809], 0, 0, 1); RenderFrameY += 10;

		for (size_t i = 0; i < mixRecipient.size(); i++)
		{
			if (mixRecipient[i].SpecialItem == MIX_SOURCE_ERROR)
				g_pRenderText->SetTextColor(CLRDW_RED);
			else if (mixRecipient[i].SpecialItem == MIX_SOURCE_PARTIALLY)
				g_pRenderText->SetTextColor(CLRDW_WHITE);
			else if (mixRecipient[i].SpecialItem == MIX_SOURCE_YES)
				g_pRenderText->SetTextColor(CLRDW_CYAN);
			else
				g_pRenderText->SetTextColor(CLRDW_RED);
		
			sprintf_s(pszText, mixRecipient[i].pszNameMix.c_str());
			g_pRenderText->RenderFont(RenderFrameX + 10, RenderFrameY, pszText, 0, 0, 1); RenderFrameY += 10;
		}

		this->RenderItemList();
		
		if (m_StateNormal != 0)
		{
			this->RenderTargetBox(m_Pos.x + 15.f, m_Pos.y + 210.f, 158, 160, m_StateNormal);
		}
	}
}

void SEASON3B::CNewUIMixExpansion::RenderButton()
{
	m_BtnMix.Render();

	if (m_iNumMixIndex == -1)
	{
		if (m_mixOptionList)
		{
			g_pOption->RenderTable(m_Pos.x + (WIN_WINDOW_SIZEX - 160) / 2, m_Pos.y + 210, 160, 164);
			m_mixOptionList->Render();
		}
	}
	else
	{
		m_pScroll.Render();
	}
}

void SEASON3B::CNewUIMixExpansion::RenderItemList()
{
	int PosX = m_Pos.x + 25;
	int PosY = m_Pos.y + 215;

	int current = 0;
	if (m_iNumItemCnt > MAX_ITEM_VIEW)
	{
		double prev = m_pScroll.GetPercent();
		current = (int)((double)(unsigned int)(m_iNumItemCnt - MAX_ITEM_VIEW) * prev);
	}

	char pszText[255];
	sprintf_s(pszText, GlobalText[69], m_iNumItemCnt);
	g_pRenderText->SetTextColor(CLRDW_WHITE);
	g_pRenderText->RenderFont(PosX, (PosY - 20), pszText, 0, 0, 1);

	m_iRenderItem = -1;

	int Secure = current;
	for (int i = 0; i < MIX_INVENTORY_SIZE; i++)
	{
		if (Secure >= m_iNumItemCnt)
		{
			break;
		}
		if (Secure <= (current + MAX_ITEM_VIEW))
		{
			if (m_pdwItemCheckBox[i] != 0)
			{
				if (m_StateNormal == 0 && SEASON3B::CheckMouseIn(PosX - 6, PosY, 130, 15))
				{
					if (m_iTargetItem == i)
					{
						m_iRenderItem = i;
						m_iRenderTooltipX = PosX + 20;
						m_iRenderTooltipY = PosY + 5;
					}
					else if (MouseLButton)
					{
						m_iTargetItem = i;
					}
					g_pRenderText->SetTextColor(CLRDW_WHITE);
				}
				else
				{
					if (m_iTargetItem == i)
						g_pRenderText->SetTextColor(CLRDW_WHITE);
					else
						g_pRenderText->SetTextColor(CLRDW_VIOLET);
				}

				g_pRenderText->RenderFont(PosX, PosY, m_vecItem[i].GetName(), 0, 15, 1);

				if (m_StateNormal == 0)
				{
					if (m_iTargetItem == i || SEASON3B::CheckMouseIn(PosX - 6, PosY, 126, 15))
					{
						this->RenderTargetBox(PosX - 5.f, PosY, 126.f, 15.f);
					}
				}
				PosY += 15;
				Secure++;
			}
		}
	}
}

void SEASON3B::CNewUIMixExpansion::RenderTargetBox(float iPos_x, float iPos_y, float width, float height, DWORD color)
{
	EnableAlphaTest(true);

	BYTE red = GetRed(color);
	BYTE green = GetGreen(color);
	BYTE blue = GetBlue(color);

	glColor4ub(red, green, blue, 30);
	RenderColor(iPos_x, iPos_y, width, height, 0.0, 0);

	glColor4ub(red, green, blue, 255);
	RenderColor(iPos_x, iPos_y, width, 1.f, 0.0, 0);
	RenderColor(iPos_x, iPos_y + height, width, 1.f, 0.0, 0);

	RenderColor(iPos_x, iPos_y + 1.f, 1.f, height - 1.f, 0.0, 0);
	RenderColor(iPos_x + width - 1.f, iPos_y + 1.f, 1.f, height - 1.f, 0.0, 0);

	EndRenderColor();
}

void SEASON3B::CNewUIMixExpansion::SendRequestInfo(int index, BYTE Type)
{
	if (Type == 0)
	{
		if (index < m_InfoBtnCnt.size())
		{
			int mixIndex = m_InfoBtnCnt[index].iMixIndex;
			SendRequestMixIndex(mixIndex);
		}
	}
	else
	{
		int mixIndex = 1;
		SendRequestMixState(mixIndex);
	}
}

void SEASON3B::CNewUIMixExpansion::ReceiveMixOptions(BYTE* ReceiveBuffer)
{
	m_InfoBtnCnt.clear();

	m_mixOptionList->clear();

	LPPHEADER_MIX_DEFAULT Data = (LPPHEADER_MIX_DEFAULT)ReceiveBuffer;

	int offset = sizeof(PHEADER_MIX_DEFAULT);

	for (int i = 0; i < Data->Value; i++)
	{
		MIX_BTN_TEMPLATE Data = *((MIX_BTN_TEMPLATE*)(ReceiveBuffer + offset));

		m_InfoBtnCnt.push_back(Data);

		m_mixOptionList->push_back(Data.iName);

		//g_ConsoleDebug->Write(5, "[elemento: %s]", Data.iName);

		offset += sizeof(MIX_BTN_TEMPLATE);
	}
}

void SEASON3B::CNewUIMixExpansion::ReceiveMixRecipients(BYTE* ReceiveBuffer)
{
	LPPHEADER_MIX_INFORMATION Data = (LPPHEADER_MIX_INFORMATION)ReceiveBuffer;

	mixRecipient.clear();

	int offset = sizeof(PHEADER_MIX_INFORMATION);

	this->m_iSuccessRate = Data->iSuccessRate;

	this->m_iSuccessAddRate = 0;

	this->m_iRequiredZen = Data->iRequiredZen;

	this->m_iRequiredType = Data->iRequiredType;

	this->m_iTalismanLuckRate = Data->iTalismanLuckRate;

	if (Data->iTalismanOfLuck >= 0 && Data->iTalismanOfLuck < 0x1FFF)
		this->m_iTalismanOfLuck = Data->iTalismanOfLuck;
	else
		this->m_iTalismanOfLuck = 0x1FFF;

	if (Data->iTalismanOfChaos >= 0 && Data->iTalismanOfChaos < 0x1FFF)
		this->m_iTalismanOfChaos = Data->iTalismanOfChaos;
	else
		this->m_iTalismanOfChaos = 0x1FFF;

	//g_ConsoleDebug->Write(5, "[ReceiveMixRecipients: %d]", Data->Value);

	for (int i = 0; i < Data->Value; i++)
	{
		MIX_RECIPIENT_CHECK info;

		info.iNumMixItem = 0;

		info.SpecialItem = MIX_SOURCE_ERROR;

		memcpy(&info.pMixItems, &*((MIX_RECIPE_ITEM*)(ReceiveBuffer + offset)), sizeof(MIX_RECIPE_ITEM));

		info.GetSourceName(&info.pMixItems);

		mixRecipient.push_back(info);

		offset += sizeof(MIX_RECIPE_ITEM);
	}
}

void SEASON3B::CNewUIMixExpansion::ReceiveInventory(BYTE* ReceiveBuffer)
{
	LPPHEADER_MIX_DEFAULT Data = (LPPHEADER_MIX_DEFAULT)ReceiveBuffer;

	int offset = sizeof(PHEADER_MIX_DEFAULT);

	this->DeleteAllItems();

	for (int i = 0; i < Data->Value; i++)
	{
		LPPRECEIVE_INVENTORY Data2 = (LPPRECEIVE_INVENTORY)(ReceiveBuffer + offset);

		this->InsertItem(Data2->Index, Data2->Item);

		offset += sizeof(LPPRECEIVE_INVENTORY);
	}
}

bool SEASON3B::CNewUIMixExpansion::IsMixItem(ITEM* pItemObj)
{
	if (this->m_iTalismanOfLuck != 0x1FFF && this->m_iTalismanOfLuck == pItemObj->Type)
		return true;

	if (this->m_iTalismanOfChaos != 0x1FFF && this->m_iTalismanOfChaos == pItemObj->Type)
		return true;

	for (int i = 0; i < mixRecipient.size(); i++)
	{
		if ((&mixRecipient[i])->IsMixItem(pItemObj))
		{
			return true;
		}
	}
	return false;
}

bool SEASON3B::CNewUIMixExpansion::IsItemAutoMove()
{
	return m_bItemAutoMove;
}

void SEASON3B::CNewUIMixExpansion::ProcessItemAutoMoveSuccess()
{
	if (this->IsItemAutoMove())
	{
		m_bItemAutoMove = 0;
		this->DeleteItem(m_BackupTargetItem);
		m_BackupTargetItem = -1;
	}
}

void SEASON3B::CNewUIMixExpansion::ProcessItemAutoMoveFailure()
{
	if (this->IsItemAutoMove())
	{
		m_bItemAutoMove = 0;
		m_BackupTargetItem = -1;
	}
}

SEASON3B::MIX_RECIPIENT_CHECK::MIX_RECIPIENT_CHECK()
{
	SpecialItem = 0;
	iNumMixItem = 0;
	pszNameMix = "";
	memset(&pMixItems, 0, sizeof(pMixItems));
}

bool SEASON3B::MIX_RECIPIENT_CHECK::IsMixItem(ITEM* pItem)
{
	int m_sType = pItem->Type;
	int m_iLevel = ((pItem->Level >> 3) & 15);
	int m_iDurability = pItem->Durability;
	int m_iOption = 0;
	int m_bMixLuck = 0;
	int m_SpecialItem = 0;

	for (int i = 0; i < pItem->SpecialNum; i++)
	{
		switch (pItem->Special[i])
		{
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
			m_iOption = pItem->SpecialValue[i];
			break;
		case 0x56:
			m_iOption = pItem->SpecialValue[i] * 4;
			break;
		case 0x55:
			m_bMixLuck = 1;
			break;
		default:
			continue;
		}
	}

	if ((pItem->Option1 & 63) > 0)
		m_SpecialItem |= RCP_SP_EXCELLENT;
	if (pItem->RequireLevel >= 380)
		m_SpecialItem |= RCP_SP_ADD380ITEM;
	if ((pItem->ExtOption % 0x04) == 1 || (pItem->ExtOption % 0x04) == 2)
		m_SpecialItem |= RCP_SP_SETITEM;

	if ((m_sType >= pMixItems.minIndex && m_sType <= pMixItems.maxIndex)
		&& m_iLevel >= pMixItems.minLevel && m_iLevel <= pMixItems.maxLevel
		&& m_iDurability >= pMixItems.minDurability && m_iDurability <= pMixItems.maxDurability
		&& m_iOption >= pMixItems.minOpc && m_iOption <= pMixItems.maxOpc
		&& (pMixItems.luck == 0 || (pMixItems.luck != 0 && m_bMixLuck))
		&& (pMixItems.exeItem == 0 || (pMixItems.exeItem != 0 && ((m_SpecialItem & RCP_SP_EXCELLENT) == RCP_SP_EXCELLENT)))
		&& (pMixItems.accItem == 0 || (pMixItems.accItem != 0 && ((m_SpecialItem & RCP_SP_SETITEM) == RCP_SP_SETITEM)))
		)
	{
		return true;
	}
	return false;
}

bool SEASON3B::MIX_RECIPIENT_CHECK::CheckRecipe(ITEM* pItem)
{
	int m_sType = pItem->Type;
	int m_iLevel = ((pItem->Level >> 3) & 15);
	int m_iDurability = pItem->Durability;
	int m_iOption = 0;
	int m_bMixLuck = 0;
	int m_SpecialItem = 0;

	for (int i = 0; i < pItem->SpecialNum; i++)
	{
		switch (pItem->Special[i])
		{
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
			m_iOption = pItem->SpecialValue[i];
			break;
		case 0x56:
			m_iOption = pItem->SpecialValue[i] * 4;
			break;
		case 0x55:
			m_bMixLuck = 1;
			break;
		default:
			continue;
		}
	}

	if ((pItem->Option1 & 63) > 0)
		m_SpecialItem |= RCP_SP_EXCELLENT;
	if (pItem->RequireLevel >= 380)
		m_SpecialItem |= RCP_SP_ADD380ITEM;
	if ((pItem->ExtOption % 0x04) == 1 || (pItem->ExtOption % 0x04) == 2)
		m_SpecialItem |= RCP_SP_SETITEM;

	if ((m_sType >= pMixItems.minIndex && m_sType <= pMixItems.maxIndex)
		&& m_iLevel >= pMixItems.minLevel && m_iLevel <= pMixItems.maxLevel
		&& m_iDurability >= pMixItems.minDurability && m_iDurability <= pMixItems.maxDurability
		&& m_iOption >= pMixItems.minOpc && m_iOption <= pMixItems.maxOpc
		&& (pMixItems.luck == 0 || (pMixItems.luck != 0 && m_bMixLuck))
		&& (pMixItems.exeItem == 0 || (pMixItems.exeItem != 0 && ((m_SpecialItem & RCP_SP_EXCELLENT) == RCP_SP_EXCELLENT)))
		&& (pMixItems.accItem == 0 || (pMixItems.accItem != 0 && ((m_SpecialItem & RCP_SP_SETITEM) == RCP_SP_SETITEM)))
		)
	{
		SpecialItem = MIX_SOURCE_PARTIALLY;

		iNumMixItem++;
		if (iNumMixItem >= pMixItems.minCnt && iNumMixItem <= pMixItems.maxCnt)
		{
			SpecialItem = MIX_SOURCE_YES;
		}
		return true;
	}

	return false;
}

void SEASON3B::MIX_RECIPIENT_CHECK::GetSourceName(MIX_RECIPE_ITEM* pRecipeItem)
{
	char szTempName[2540];
	memset(szTempName, 0, sizeof(szTempName));

	GetItemName(pRecipeItem->minIndex, pRecipeItem->minLevel, szTempName);

	if (pRecipeItem->minIndex == pRecipeItem->maxIndex
		&& (pRecipeItem->minLevel == pRecipeItem->maxLevel || (pRecipeItem->minLevel == 0 && pRecipeItem->maxLevel == 0xFF))
		&& (pRecipeItem->minOpc == pRecipeItem->maxOpc || (pRecipeItem->minOpc == 0 && pRecipeItem->maxOpc == 0xFF)))
	{
		if (pRecipeItem->minDurability == pRecipeItem->maxDurability)
			sprintf_s(szTempName, "%s(%d)", szTempName, pRecipeItem->minDurability);
	}
	else
	{
		if (pRecipeItem->minIndex == ITEM_SWORD && pRecipeItem->maxIndex == (ITEM_BOOTS + MAX_ITEM_INDEX - 1)
			|| pRecipeItem->minIndex == ITEM_SWORD && pRecipeItem->maxIndex == (ITEM_HELPER + MAX_ITEM_INDEX - 1))
		{
			if (pRecipeItem->minLevel != 0 && pRecipeItem->minLevel == pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d", GlobalText[2336], pRecipeItem->minLevel);
			else if (pRecipeItem->minLevel != pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d~+%d", GlobalText[2336], pRecipeItem->minLevel, pRecipeItem->maxLevel);
			else
				sprintf_s(szTempName, GlobalText[2336]);
		}
		else if (pRecipeItem->minIndex == ITEM_SWORD && pRecipeItem->maxIndex == ITEM_STAFF + MAX_ITEM_INDEX - 1)
		{
			if (pRecipeItem->minLevel != 0 && pRecipeItem->minLevel == pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d", GlobalText[2337], pRecipeItem->minLevel);
			else if (pRecipeItem->minLevel != pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d~+%d", GlobalText[2337], pRecipeItem->minLevel, pRecipeItem->maxLevel);
			else
				sprintf_s(szTempName, GlobalText[2337]);
		}
		else if (pRecipeItem->minIndex == ITEM_SHIELD && pRecipeItem->maxIndex == ITEM_BOOTS + MAX_ITEM_INDEX - 1)
		{
			if (pRecipeItem->minLevel != 0 && pRecipeItem->minLevel == pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d", GlobalText[2338], pRecipeItem->minLevel);
			else if (pRecipeItem->minLevel != pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d~+%d", GlobalText[2338], pRecipeItem->minLevel, pRecipeItem->maxLevel);
			else
				sprintf_s(szTempName, GlobalText[2338]);
		}
		else if (pRecipeItem->minIndex == ITEM_WING && pRecipeItem->maxIndex == ITEM_WING + 2
			|| pRecipeItem->minIndex == ITEM_WING + 41 && pRecipeItem->maxIndex == ITEM_WING + 41)
		{
			if (pRecipeItem->minLevel != 0 && pRecipeItem->minLevel == pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d", GlobalText[2339], pRecipeItem->minLevel);
			else if (pRecipeItem->minLevel != pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d~+%d", GlobalText[2339], pRecipeItem->minLevel, pRecipeItem->maxLevel);
			else
				sprintf_s(szTempName, GlobalText[2339]);
		}
		else if (pRecipeItem->minIndex == ITEM_WING + 3 && pRecipeItem->maxIndex == ITEM_WING + 6
			|| pRecipeItem->minIndex == ITEM_WING + 42 && pRecipeItem->maxIndex == ITEM_WING + 42)
		{
			if (pRecipeItem->minLevel != 0 && pRecipeItem->minLevel == pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d", GlobalText[2348], pRecipeItem->minLevel);
			else if (pRecipeItem->minLevel != pRecipeItem->maxLevel)
				sprintf_s(szTempName, "%s +%d~+%d", GlobalText[2348], pRecipeItem->minLevel, pRecipeItem->maxLevel);
			else
				sprintf_s(szTempName, GlobalText[2348]);
		}
		else if (pRecipeItem->minIndex == pRecipeItem->maxIndex && (pRecipeItem->minIndex == ITEM_MACE + 6 || pRecipeItem->minIndex == ITEM_BOW + 6 || pRecipeItem->minIndex == ITEM_STAFF + 7))
			sprintf_s(szTempName, GlobalText[2340]);
		else if (pRecipeItem->minIndex == ITEM_WING + 60 && pRecipeItem->maxIndex == ITEM_WING + 65)
			sprintf_s(szTempName, GlobalText[2680]);
		else if (pRecipeItem->minIndex == ITEM_WING + 70 && pRecipeItem->maxIndex == ITEM_WING + 74)
			sprintf_s(szTempName, GlobalText[2681]);
		else if (pRecipeItem->minIndex == ITEM_WING + 100 && pRecipeItem->maxIndex == ITEM_WING + 129)
			sprintf_s(szTempName, GlobalText[2682]);
		else if (pRecipeItem->minIndex == ITEM_WING + 100 && pRecipeItem->maxIndex == ITEM_WING + 128)
			sprintf_s(szTempName, "%s (%s)", GlobalText[2682], GlobalText[2684]);
		else if (pRecipeItem->minIndex == ITEM_WING + 101 && pRecipeItem->maxIndex == ITEM_WING + 129)
			sprintf_s(szTempName, "%s (%s)", GlobalText[2682], GlobalText[2685]);
		else
		{
			int iNameLen = strlen(szTempName);
			for (int j = 1; j <= 3 && iNameLen - j - 1 >= 0; ++j)
			{
				if (szTempName[iNameLen - j] == '+')
					szTempName[iNameLen - j - 1] = '\0';
			}
		}

		if (pRecipeItem->minOpc == 0 && pRecipeItem->maxOpc == 255);
		else if (pRecipeItem->minOpc == pRecipeItem->maxOpc)
			sprintf_s(szTempName, "%s +%d%s", szTempName, pRecipeItem->minOpc, GlobalText[2343]);
		else if (pRecipeItem->minOpc == 0)
			sprintf_s(szTempName, "%s +%d%s%s", szTempName, pRecipeItem->maxOpc, GlobalText[2343], GlobalText[2342]);
		else if (pRecipeItem->maxOpc == 255)
			sprintf_s(szTempName, "%s +%d%s%s", szTempName, pRecipeItem->minOpc, GlobalText[2343], GlobalText[2341]);
		else
			sprintf_s(szTempName, "%s +%d~%d%s", szTempName, pRecipeItem->minOpc, pRecipeItem->maxOpc, GlobalText[2343]);
	}

	if (pRecipeItem->minCnt == 0 && pRecipeItem->maxCnt == 255)
		sprintf_s(szTempName, "%s (%s)", szTempName, GlobalText[2344]);
	else if (pRecipeItem->minCnt == pRecipeItem->maxCnt)
		sprintf_s(szTempName, "%s %d%s", szTempName, pRecipeItem->minCnt, GlobalText[2345]);
	else if (pRecipeItem->minCnt == 0)
		sprintf_s(szTempName, "%s %d%s %s", szTempName, pRecipeItem->maxCnt, GlobalText[2345], GlobalText[2342]);
	else if (pRecipeItem->maxCnt == 255)
		sprintf_s(szTempName, "%s %d%s %s", szTempName, pRecipeItem->minCnt, GlobalText[2345], GlobalText[2341]);
	else
		sprintf_s(szTempName, "%s %d~%d%s", szTempName, pRecipeItem->minCnt, pRecipeItem->maxCnt, GlobalText[2345]);

	if (pRecipeItem->exeItem)
	{
		sprintf(szTempName, "%s %s", GlobalText[620], szTempName);
	}
	if (pRecipeItem->accItem)
	{
		sprintf(szTempName, "%s %s", GlobalText[1089], szTempName);
	}

	pszNameMix = szTempName;
}
