// NewUIBuffWindow.cpp: implementation of the CNewUIBuffWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIBuffWindow.h"
#include "ZzzBMD.h"
#include "ZzzCharacter.h"
#include "ZzzTexture.h"
#include "ZzzInventory.h"
#include "UIControls.h"
#include "NewUICommonMessageBox.h"

using namespace SEASON3B;

namespace
{
	const float BUFF_IMG_WIDTH = 16.0f;
	const float BUFF_IMG_HEIGHT = 20.0f;
	const int BUFF_MAX_LINE_COUNT = 8;
	const int BUFF_IMG_SPACE = 5;
};

SEASON3B::CNewUIBuffWindow::CNewUIBuffWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
}

SEASON3B::CNewUIBuffWindow::~CNewUIBuffWindow()
{
	Release();
}

bool SEASON3B::CNewUIBuffWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_BUFF_WINDOW, this);

	SetPos(x, y);

	LoadImages();

	Show(true);

	return true;
}

void SEASON3B::CNewUIBuffWindow::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIBuffWindow::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

void SEASON3B::CNewUIBuffWindow::SetPos(int iScreenWidth)
{
#if MAIN_UPDATE > 303
	if (iScreenWidth == gwinhandle->GetScreenX())
	{
		SetPos(PositionX_In_The_Mid(220), 25);
	}
	else if (iScreenWidth == (gwinhandle->GetScreenX() - 190))
	{
		SetPos(PositionX_In_The_Mid(125), 25);
	}
	else if (iScreenWidth == (gwinhandle->GetScreenX() - 380))
	{
		SetPos(PositionX_In_The_Mid(86), 25);
	}
	else if (iScreenWidth == (gwinhandle->GetScreenX() - 570))
	{
		SetPos(PositionX_In_The_Mid(30), 25);
	}
	else
	{
		SetPos(PositionX_In_The_Mid(220), 25);
	}
#endif
}

void SEASON3B::CNewUIBuffWindow::BuffSort(OBJECT* pHeroObject, std::list<eBuffState>& buffstate)
{
	int iBuffSize = g_CharacterBuffSize(pHeroObject);

	for (int i = 0; i < iBuffSize; ++i)
	{
		eBuffState eBuffType = g_CharacterBuff(pHeroObject, i);

		if (SetDisableRenderBuff(eBuffType))
			continue;

		if (eBuffType != eBuffNone)
		{
			eBuffClass eBuffClassType = g_IsBuffClass(eBuffType);

			if (eBuffClassType == eBuffClass_Buff)
			{
				buffstate.push_front(eBuffType);
			}
			else if (eBuffClassType == eBuffClass_DeBuff)
			{
				buffstate.push_back(eBuffType);
			}
			else {
				assert(!"SetDisableRenderBuff");
			}
		}
	}
}

bool SEASON3B::CNewUIBuffWindow::SetDisableRenderBuff(const eBuffState& _BuffState)
{
	switch (_BuffState)
	{
#ifdef PBG_ADD_PKSYSTEM_INGAMESHOP
	case eDeBuff_MoveCommandWin:
#endif //PBG_ADD_PKSYSTEM_INGAMESHOP
	case eDeBuff_FlameStrikeDamage:
	case eDeBuff_GiganticStormDamage:
	case eDeBuff_LightningShockDamage:
	case eDeBuff_Discharge_Stamina:
		return true;
	default:
		return false;
}
	return false;
}

bool SEASON3B::CNewUIBuffWindow::UpdateMouseEvent()
{
	float x = 0.0f, y = 0.0f;
	int buffwidthcount = 0, buffheightcount = 1;

	std::list<eBuffState> buffstate;
	BuffSort(&Hero->Object, buffstate);

	std::list<eBuffState>::iterator iter;

	//if (gmProtect->checkold_school())
	//{
		buffheightcount = 0;
	//}

	for (iter = buffstate.begin(); iter != buffstate.end(); )
	{
		std::list<eBuffState>::iterator tempiter = iter;
		++iter;
		eBuffState buff = (*tempiter);

		x = m_Pos.x + (buffwidthcount * (BUFF_IMG_WIDTH + BUFF_IMG_SPACE));

		//if (gmProtect->checkold_school())
		//{
			y = m_Pos.y + (buffheightcount * (BUFF_IMG_HEIGHT + BUFF_IMG_SPACE));
		/*}
		else
		{
			y = m_Pos.y - (buffheightcount * (BUFF_IMG_HEIGHT + BUFF_IMG_SPACE));
		}*/


		if (SEASON3B::CheckMouseIn(x, y, BUFF_IMG_WIDTH, BUFF_IMG_HEIGHT))
		{
			if (buff == eBuff_InfinityArrow)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CInfinityArrowCancelMsgBoxLayout));
				}
			}
			else if (buff == eBuff_SwellOfMagicPower)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBuffSwellOfMPCancelMsgBoxLayOut));
				}
			}
			else if (buff == EFFECT_MAGIC_CIRCLE_IMPROVED)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBuffSwellOfMPUpCancelMsgBoxLayOut));
				}
			}
			else if (buff == EFFECT_MAGIC_CIRCLE_ENHANCED)
			{
				if (SEASON3B::IsRelease(VK_RBUTTON))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CBuffSwellOfMPMasteryCancelMsgBoxLayOut));
				}
			}
			return false;
		}

		if (++buffwidthcount >= BUFF_MAX_LINE_COUNT)
		{
			buffwidthcount = 0;
			++buffheightcount;
		}
	}
	return true;
}

bool SEASON3B::CNewUIBuffWindow::UpdateKeyEvent()
{
	return true;
}

bool SEASON3B::CNewUIBuffWindow::Update()
{
	return true;
}

bool SEASON3B::CNewUIBuffWindow::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderBuffStatus(BUFF_RENDER_ICON);

	RenderBuffStatus(BUFF_RENDER_TOOLTIP);

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CNewUIBuffWindow::RenderBuffStatus(BUFF_RENDER renderstate)
{
	float x = 0.0f, y = 0.0f;
	int buffwidthcount = 0, buffheightcount = 1;

	std::list<eBuffState> buffstate;

	BuffSort(&Hero->Object, buffstate);

	std::list<eBuffState>::iterator iter;

	//if (gmProtect->checkold_school())
	//{
		buffheightcount = 0;
	//}

	for (iter = buffstate.begin(); iter != buffstate.end(); )
	{
		std::list<eBuffState>::iterator tempiter = iter;
		++iter;
		eBuffState buff = (*tempiter);

		x = m_Pos.x + (buffwidthcount * (BUFF_IMG_WIDTH + BUFF_IMG_SPACE));
		//if (gmProtect->checkold_school())
		//{
			y = m_Pos.y + (buffheightcount * (BUFF_IMG_HEIGHT + BUFF_IMG_SPACE));
		/*}
		else
		{
			y = m_Pos.y - (buffheightcount * (BUFF_IMG_HEIGHT + BUFF_IMG_SPACE));
		}*/

		if (renderstate == BUFF_RENDER_ICON)
		{
			RenderBuffIcon(buff, x, y, BUFF_IMG_WIDTH, BUFF_IMG_HEIGHT);
		}
		else if (renderstate == BUFF_RENDER_TOOLTIP)
		{
			if (SEASON3B::CheckMouseIn(x, y, BUFF_IMG_WIDTH, BUFF_IMG_HEIGHT))
			{
				float fTooltip_x = x + (BUFF_IMG_WIDTH / 2);
				float fTooltip_y = y;

				//if (gmProtect->checkold_school())
				//{
					y += (BUFF_IMG_HEIGHT * 2);
				/*}
				else
				{
					y -= (BUFF_IMG_HEIGHT * 2);
				}*/
				eBuffClass buffclass = g_IsBuffClass(buff);
				RenderBuffTooltip(buffclass, buff, fTooltip_x, fTooltip_y);
			}
		}

		if (++buffwidthcount >= BUFF_MAX_LINE_COUNT)
		{
			buffwidthcount = 0;
			++buffheightcount;
		}
	}
}

void SEASON3B::CNewUIBuffWindow::RenderBuffIcon(eBuffState& eBuffType, float x, float y, float width, float height)
{
	float u, v;

	if (eBuffType > eBuffNone && eBuffType < eBuff_Count) // eBuff_Berserker
	{
		int skillindex = ((int)eBuffType - 1) % 80;

		u = (double)(skillindex % 10) * 20.f / 256.0;
		v = (double)(skillindex / 10) * 28.f / 256.0;
		int imgindex = ((int)eBuffType - 1) / 80 + IMAGE_BUFF_STATUS;
		RenderBitmap(imgindex, x, y, width, height, u, v, 20.f / 256.f, 28.f / 256.f);
	}
}

void SEASON3B::CNewUIBuffWindow::RenderBuffTooltip(eBuffClass& eBuffClassType, eBuffState& eBuffType, float x, float y)
{
	int TextNum = 0;
	::memset(TextList, 0, sizeof(TextList));
	::memset(TextListColor, 0, sizeof(TextListColor));
	::memset(TextBold, 0, sizeof(TextBold));

	std::list<std::string> tooltipinfo;
	g_BuffToolTipString(tooltipinfo, eBuffType);

	for (std::list<std::string>::iterator iter = tooltipinfo.begin(); iter != tooltipinfo.end(); ++iter)
	{
		std::string& temp = *iter;

		unicode::_sprintf(TextList[TextNum], temp.c_str());

		if (TextNum == 0)
		{
			TextListColor[TextNum] = TEXT_COLOR_BLUE;
			TextBold[TextNum] = true;
		}
		else
		{
			TextListColor[TextNum] = TEXT_COLOR_WHITE;
			TextBold[TextNum] = false;
		}
		TextNum += 1;
	}

	std::string bufftime;
	g_BuffStringTime(eBuffType, bufftime);

	if (bufftime.size() != 0)
	{
		unicode::_sprintf(TextList[TextNum++], "\n");

		unicode::_sprintf(TextList[TextNum], GlobalText[2533], bufftime.c_str());
		TextListColor[TextNum] = TEXT_COLOR_PURPLE;
		TextBold[TextNum] = false;
		TextNum += 1;
	}

	SIZE TextSize = { 0, 0 };
	g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), TextList[0], 1, &TextSize);
	RenderTipTextList(x, y, TextNum, 0);
}

float SEASON3B::CNewUIBuffWindow::GetLayerDepth()	//. 5.3f
{
	return 0.95f;
}

void SEASON3B::CNewUIBuffWindow::OpenningProcess()
{

}

void SEASON3B::CNewUIBuffWindow::ClosingProcess()
{

}

#if MAIN_UPDATE <= 303
void SEASON3B::CNewUIBuffWindow::RenderBuffStatus(float x, float y, OBJECT* pHeroObject)
{
	float RenderFrameX = 0.0;
	float RenderFrameY = 0.0;

	int buffwidthcount = 0, buffheightcount = 1;

	std::list<eBuffState> buffstate;

	BuffSort(pHeroObject, buffstate);

	float RenderSizeX = 16.f;
	float RenderSizeY = 20.f;

	for (std::list<eBuffState>::iterator iter = buffstate.begin(); iter != buffstate.end(); )
	{
		std::list<eBuffState>::iterator tempiter = iter;
		++iter;
		eBuffState buff = (*tempiter);

		if (buffwidthcount < 4)
		{
			RenderFrameX = x - (buffwidthcount * RenderSizeX); // Renderiza a la izquierda
		}
		else
		{
			RenderFrameX = x + ((buffwidthcount - 4) * RenderSizeX) + RenderSizeX; // Renderiza a la derecha
		}

		RenderFrameY = y - (buffheightcount * (RenderSizeY + 2));

		RenderBuffIcon(buff, RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY);

		if (++buffwidthcount >= BUFF_MAX_LINE_COUNT)
		{
			buffwidthcount = 0;
			++buffheightcount;
		}
	}
}
#endif

void SEASON3B::CNewUIBuffWindow::LoadImages()
{
	LoadBitmap("Interface\\newui_statusicon.jpg", IMAGE_BUFF_STATUS, GL_LINEAR);
	LoadBitmap("Interface\\newui_statusicon2.jpg", IMAGE_BUFF_STATUS2, GL_LINEAR);
	LoadBitmap("Interface\\newui_statusicon3.jpg", IMAGE_BUFF_STATUS3, GL_LINEAR);
}

void SEASON3B::CNewUIBuffWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_BUFF_STATUS);
	DeleteBitmap(IMAGE_BUFF_STATUS2);
	DeleteBitmap(IMAGE_BUFF_STATUS3);
}
