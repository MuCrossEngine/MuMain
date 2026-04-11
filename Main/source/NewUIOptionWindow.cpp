// NewUIOptionWindow.cpp: implementation of the CNewUIOptionWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NewUIOptionWindow.h"
#include "NewUISystem.h"
#include "ZzzTexture.h"
#include "DSPlaySound.h"
#include "Input.h"
#include "ZzzOpenData.h"
#include "ZzzInterface.h"
#include "pugixml.hpp"
#include "./ExternalObject/leaf/regkey.h"
#include "TextClien.h"
using namespace SEASON3B;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SEASON3B::CNewUIOptionWindow::CNewUIOptionWindow()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;

	m_bAutoAttack = true;
	m_bWhisperSound = false;
	m_bSlideHelp = true;
	m_iVolumeLevel = 0;
	m_iRenderLevel = 4;

	m_RenderEffect = true;
	m_RenderEquipment = true;
	m_RenderTerrain = true;
	m_RenderObjects = true;
}

SEASON3B::CNewUIOptionWindow::~CNewUIOptionWindow()
{
	Release();
}

bool SEASON3B::CNewUIOptionWindow::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_OPTION, this);

	SetPos(x, y);

	LoadImages();

	SetButtonInfo();

	Show(false);

	return true;
}

void SEASON3B::CNewUIOptionWindow::SetButtonInfo()
{
	//m_BtnClose.ChangeTextBackColor(RGBA(255, 255, 255, 0));
	//m_BtnClose.ChangeButtonImgState(true, IMAGE_OPTION_BTN_CLOSE, true);
	//m_BtnClose.ChangeButtonInfo(m_Pos.x + 68, m_Pos.y + 209, 54, 30);
	//m_BtnClose.ChangeImgColor(BUTTON_STATE_UP, RGBA(255, 255, 255, 255));
	//m_BtnClose.ChangeImgColor(BUTTON_STATE_DOWN, RGBA(255, 255, 255, 255));

	resolutionList.Create(m_Pos.x + 20.f, m_Pos.y + 50.f, 4);

	fonttextList.Create(m_Pos.x + 20.f, m_Pos.y + 175.f, 3);

	LoadResolution("Data\\Resolutions.xml");
}

void SEASON3B::CNewUIOptionWindow::Release()
{
	UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIOptionWindow::SetPos(int x, int y)
{
	float RenderFrameX = pos_center() + 190;
	float RenderFrameY = PositionY_In_The_Mid(0) + y;

	m_Pos.x = RenderFrameX;
	m_Pos.y = RenderFrameY;

	resolutionList.SetPosition(m_Pos.x + 20.f, m_Pos.y + 50.f);

	fonttextList.SetPosition(m_Pos.x + 20.f, m_Pos.y + 175.f);
}

bool SEASON3B::CNewUIOptionWindow::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	//if (m_BtnClose.UpdateMouseEvent() == true)
	//{
	//	g_pNewUISystem->Hide(SEASON3B::INTERFACE_OPTION);
	//	return false;
	//}

	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 40.0, 15, 15))
	{
		m_bAutoAttack = !m_bAutoAttack;
	}
	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 58.0, 15, 15))
	{
		m_bWhisperSound = !m_bWhisperSound;
	}
	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 76.0, 15, 15))
	{
		m_bSlideHelp = !m_bSlideHelp;
	}

	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 94.0, 15, 15))
	{
		m_RenderEffect = !m_RenderEffect;
	}
	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 112.0, 15, 15))
	{
		m_RenderEquipment = !m_RenderEquipment;
	}
	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 130.0, 15, 15))
	{
		m_RenderTerrain = !m_RenderTerrain;
	}
	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(RenderFrameX + 210.0, RenderFrameY + 148.0, 15, 15))
	{
		m_RenderObjects = !m_RenderObjects;
	}

	if (CheckMouseIn(RenderFrameX + 108 - 8, RenderFrameY + 185, 124 + 8, 16))
	{
		int iOldValue = m_iVolumeLevel;
		if (MouseWheel > 0)
		{
			MouseWheel = 0;
			m_iVolumeLevel++;
			if (m_iVolumeLevel > 10)
			{
				m_iVolumeLevel = 10;
			}
		}
		else if (MouseWheel < 0)
		{
			MouseWheel = 0;
			m_iVolumeLevel--;
			if (m_iVolumeLevel < 0)
			{
				m_iVolumeLevel = 0;
			}
		}
		if (SEASON3B::IsRepeat(VK_LBUTTON))
		{
			int x = MouseX - (RenderFrameX + 108);
			if (x < 0)
			{
				m_iVolumeLevel = 0;
			}
			else
			{
				float fValue = (10.f * x) / 124.f;
				m_iVolumeLevel = (int)fValue + 1;
			}
		}

		if (iOldValue != m_iVolumeLevel)
		{
			SetEffectVolumeLevel(m_iVolumeLevel);
		}
	}
	if (CheckMouseIn(RenderFrameX + 100, RenderFrameY + 220, 141, 29))
	{
		if (SEASON3B::IsRepeat(VK_LBUTTON))
		{
			int x = MouseX - (RenderFrameX + 85);
			float fValue = (5.f * x) / 141.f;
			m_iRenderLevel = (int)fValue;
		}
	}

	if (resolutionList.UpdateMouseEvent())
	{
		change_resolution();
	}

	if (fonttextList.UpdateMouseEvent())
	{
		change_fontsize();
	}

	return !CheckMouseIn(RenderFrameX, RenderFrameY, 260.0, 270.0);
}

bool SEASON3B::CNewUIOptionWindow::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_OPTION) == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_OPTION);
			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}

	return true;
}

bool SEASON3B::CNewUIOptionWindow::Update()
{
	return true;
}

bool SEASON3B::CNewUIOptionWindow::Render()
{
	EnableAlphaTest();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	RenderFrame();

	RenderContents();

	RenderButtons();

	DisableAlphaBlend();

	return true;
}

float SEASON3B::CNewUIOptionWindow::GetLayerDepth()	//. 10.5f
{
	return 10.5f;
}

float SEASON3B::CNewUIOptionWindow::GetKeyEventOrder()	// 10.f;
{
	return 10.0f;
}

void SEASON3B::CNewUIOptionWindow::OpenningProcess()
{

}

void SEASON3B::CNewUIOptionWindow::ClosingProcess()
{
	SaveOptions();
}

void SEASON3B::CNewUIOptionWindow::SetGameOptions(BYTE GameOption)
{
	if ((GameOption & AUTOATTACK_ON) == AUTOATTACK_ON)
	{
		this->SetAutoAttack(true);
	}
	else
	{
		this->SetAutoAttack(false);
	}

	if ((GameOption & WHISPER_SOUND_ON) == WHISPER_SOUND_ON)
	{
		this->SetWhisperSound(true);
	}
	else
	{
		this->SetWhisperSound(false);
	}

	if ((GameOption & SLIDE_HELP_ON) == SLIDE_HELP_ON)
	{
		this->SetSlideHelp(true);
	}
	else
	{
		this->SetSlideHelp(false);
	}

	if ((GameOption & RENDER_EFFECT_ON) == RENDER_EFFECT_ON)
	{
		this->SetRenderEffect(true);
	}
	else
	{
		this->SetRenderEffect(false);
	}

	if ((GameOption & RENDER_EQUIPMENT_ON) == RENDER_EQUIPMENT_ON)
	{
		this->SetRenderEquipment(true);
	}
	else
	{
		this->SetRenderEquipment(false);
	}

	if ((GameOption & RENDER_TERRAIN_ON) == RENDER_TERRAIN_ON)
	{
		this->SetRenderTerrain(true);
	}
	else
	{
		this->SetRenderTerrain(false);
	}

	if ((GameOption & RENDER_OBJECTS_ON) == RENDER_OBJECTS_ON)
	{
		this->SetRenderObjects(true);
	}
	else
	{
		this->SetRenderObjects(false);
	}
}

void SEASON3B::CNewUIOptionWindow::LoadImages()
{
	LoadBitmap("Interface\\newui_button_close.tga", IMAGE_OPTION_BTN_CLOSE, GL_LINEAR);
	LoadBitmap("Interface\\newui_msgbox_back.jpg", IMAGE_OPTION_FRAME_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_item_back03.tga", IMAGE_OPTION_FRAME_DOWN, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_top.tga", IMAGE_OPTION_FRAME_UP, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_back06(L).tga", IMAGE_OPTION_FRAME_LEFT, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_back06(R).tga", IMAGE_OPTION_FRAME_RIGHT, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_line.jpg", IMAGE_OPTION_LINE, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_point.tga", IMAGE_OPTION_POINT, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_check.tga", IMAGE_OPTION_BTN_CHECK, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_effect03.tga", IMAGE_OPTION_EFFECT_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_effect04.tga", IMAGE_OPTION_EFFECT_COLOR, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_volume01.tga", IMAGE_OPTION_VOLUME_BACK, GL_LINEAR);
	LoadBitmap("Interface\\newui_option_volume02.tga", IMAGE_OPTION_VOLUME_COLOR, GL_LINEAR);

	LoadBitmap("Interface\\HUD\\checkbox.tga", IMAGE_CHECK_LIVE);
	LoadBitmap("Interface\\HUD\\uncheckbox.tga", IMAGE_UNCHECK_LIVE);
}

void SEASON3B::CNewUIOptionWindow::UnloadImages()
{
	DeleteBitmap(IMAGE_OPTION_BTN_CLOSE);
	DeleteBitmap(IMAGE_OPTION_FRAME_BACK);
	DeleteBitmap(IMAGE_OPTION_FRAME_DOWN);
	DeleteBitmap(IMAGE_OPTION_FRAME_UP);
	DeleteBitmap(IMAGE_OPTION_FRAME_LEFT);
	DeleteBitmap(IMAGE_OPTION_FRAME_RIGHT);
	DeleteBitmap(IMAGE_OPTION_LINE);
	DeleteBitmap(IMAGE_OPTION_POINT);
	DeleteBitmap(IMAGE_OPTION_BTN_CHECK);
	DeleteBitmap(IMAGE_OPTION_EFFECT_BACK);
	DeleteBitmap(IMAGE_OPTION_EFFECT_COLOR);
	DeleteBitmap(IMAGE_OPTION_VOLUME_BACK);
	DeleteBitmap(IMAGE_OPTION_VOLUME_COLOR);
}

void SEASON3B::CNewUIOptionWindow::RenderFrame()
{
	window_backmsg(m_Pos.x, m_Pos.y, 260.f, 270.f);

	RenderTable(m_Pos.x + 15.f, m_Pos.y + 45.f, 75.f, 110.f);

	RenderTable(m_Pos.x + 15.f, m_Pos.y + 170.f, 75.f, 85.f);

	int Bitmap_point_draw = CNewUICursedTempleSystem::IMAGE_CURSEDTEMPLESYSTEM_MINIMAPICON_HOLYITEM_PC;

	RenderBitmap(Bitmap_point_draw, ((float)m_Pos.x + 18.f), (float)m_Pos.y + 32.f, 8.f, 8.f, 0.f, 0.f, 14.f / 16.f, 14.f / 16.f);

	RenderBitmap(Bitmap_point_draw, ((float)m_Pos.x + 18.f), (float)m_Pos.y + 157.f, 8.f, 8.f, 0.f, 0.f, 14.f / 16.f, 14.f / 16.f);

	resolutionList.Render();

	fonttextList.Render();
}

void SEASON3B::CNewUIOptionWindow::RenderContents()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	g_pRenderText->SetBgColor(0);

	g_pRenderText->SetFont(g_hFontBold);

	g_pRenderText->SetTextColor(CLRDW_GOLD);

	g_pRenderText->RenderText(RenderFrameX + 130, RenderFrameY + 8, GlobalText[3450], 0, 0, 8);

	g_pRenderText->RenderText(RenderFrameX + 30, RenderFrameY + 32, gTextClien.TextClien_Khac[4]); //--

	g_pRenderText->RenderText(RenderFrameX + 30, RenderFrameY + 157, gTextClien.TextClien_Khac[5]); //--

	g_pRenderText->SetFont(g_hFont);

	g_pRenderText->SetTextColor(255, 255, 255, 255);

	g_pRenderText->SetBgColor(0);

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 40, GlobalText[386], 0, 15); //-- Automatic Attack

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 58, GlobalText[387], 0, 15); //-- Beep sound for whispering

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 76, GlobalText[919], 0, 15); //-- Slide Help

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 94, gTextClien.TextClien_Khac[6], 0, 15); //-- Slide Help

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 112, gTextClien.TextClien_Khac[7], 0, 15); //-- Slide Help

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 130, gTextClien.TextClien_Khac[8], 0, 15); //-- Slide Help

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 148, gTextClien.TextClien_Khac[9], 0, 15); //-- Slide Help

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 175, GlobalText[389]); //-- Volume

	g_pRenderText->RenderText(RenderFrameX + 100, RenderFrameY + 210, GlobalText[1840]); //-- +Effect limitation

}

void SEASON3B::CNewUIOptionWindow::RenderChecked(float RenderFrameX, float RenderFrameY, bool bEnable)
{
	if (bEnable)
		RenderImage(IMAGE_CHECK_LIVE, RenderFrameX, RenderFrameY, 15.0, 15.0, 0.0, 0.0, 0.75, 0.75);
	else
		RenderImage(IMAGE_UNCHECK_LIVE, RenderFrameX, RenderFrameY, 15.0, 15.0, 0.0, 0.0, 0.75, 0.75);
}

void SEASON3B::CNewUIOptionWindow::RenderButtons()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 40.0, m_bAutoAttack);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 58.0, m_bWhisperSound);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 76.0, m_bSlideHelp);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 94.0, m_RenderEffect);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 112.0, m_RenderEquipment);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 130.0, m_RenderTerrain);

	RenderChecked(RenderFrameX + 210.0, RenderFrameY + 148.0, m_RenderObjects);


	RenderImage(IMAGE_OPTION_VOLUME_BACK, RenderFrameX + 108, RenderFrameY + 185, 124.f, 16.f);

	if (m_iVolumeLevel > 0)
	{
		RenderImage(IMAGE_OPTION_VOLUME_COLOR, RenderFrameX + 108, RenderFrameY + 185, 124.f * 0.1f * (m_iVolumeLevel), 16.f);
	}

	RenderImage(IMAGE_OPTION_EFFECT_BACK, RenderFrameX + 100, RenderFrameY + 220, 141.f, 29.f);

	if (m_iRenderLevel >= 0)
	{
		RenderImage(IMAGE_OPTION_EFFECT_COLOR, RenderFrameX + 100, RenderFrameY + 220, 141.f * 0.2f * (m_iRenderLevel + 1), 29.f);
	}
}

void SEASON3B::CNewUIOptionWindow::RenderTable(float x, float y, float width, float height)
{
	EnableAlphaTest();

	glColor4f(0.0, 0.0, 0.0, 0.40000001);

	RenderColor((x + 3), (y + 2), (width - 7), (height - 7), 0.0, 0);

	EndRenderColor();

#if MAIN_UPDATE <= 603
	RenderImage(IMAGE_MAIN_TABLE_TOP_LEFT, x, y, 14.0, 14.0);

	RenderImage(IMAGE_MAIN_TABLE_TOP_RIGHT, (x + width - 14), y, 14.0, 14.0);

	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_LEFT, x, (y + height - 14), 14.0, 14.0);

	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_RIGHT, (x + width - 14), (y + height - 14), 14.0, 14.0);

	RenderImage(IMAGE_MAIN_TABLE_TOP_PIXEL, (x + 6), y, (width - 12), 14.0);

	RenderImage(IMAGE_MAIN_TABLE_RIGHT_PIXEL, (x + width - 14), (y + 6), 14.0, (height - 14));

	RenderImage(IMAGE_MAIN_TABLE_BOTTOM_PIXEL, (x + 6), (y + height - 14), (width - 12), 14.0);

	RenderImage(IMAGE_MAIN_TABLE_LEFT_PIXEL, x, (y + 6), 14.0, (height - 14));
#endif
}

void SEASON3B::CNewUIOptionWindow::SetAutoAttack(bool bAuto)
{
	m_bAutoAttack = bAuto;
}

bool SEASON3B::CNewUIOptionWindow::IsAutoAttack()
{
	return m_bAutoAttack;
}

void SEASON3B::CNewUIOptionWindow::SetWhisperSound(bool bSound)
{
	m_bWhisperSound = bSound;
}

bool SEASON3B::CNewUIOptionWindow::IsWhisperSound()
{
	return m_bWhisperSound;
}

void SEASON3B::CNewUIOptionWindow::SetSlideHelp(bool bHelp)
{
	m_bSlideHelp = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::IsSlideHelp()
{
	return m_bSlideHelp;
}

void SEASON3B::CNewUIOptionWindow::SetVolumeLevel(int iVolume)
{
	m_iVolumeLevel = iVolume;
}

int SEASON3B::CNewUIOptionWindow::GetVolumeLevel()
{
	return m_iVolumeLevel;
}

void SEASON3B::CNewUIOptionWindow::SetRenderLevel(int iRender)
{
	m_iRenderLevel = iRender;
}

int SEASON3B::CNewUIOptionWindow::GetRenderLevel()
{
	return m_iRenderLevel;
}

void SEASON3B::CNewUIOptionWindow::SetRenderEffect(bool bHelp)
{
	m_RenderEffect = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::GetRenderEffect()
{
	return m_RenderEffect;
}

void SEASON3B::CNewUIOptionWindow::SetRenderEquipment(bool bHelp)
{
	m_RenderEquipment = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::GetRenderEquipment()
{
	return m_RenderEquipment;
}

void SEASON3B::CNewUIOptionWindow::SetRenderTerrain(bool bHelp)
{
	m_RenderTerrain = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::GetRenderTerrain()
{
	return m_RenderTerrain;
}

void SEASON3B::CNewUIOptionWindow::SetRenderObjects(bool bHelp)
{
	m_RenderObjects = bHelp;
}

bool SEASON3B::CNewUIOptionWindow::GetRenderObjects()
{
	return m_RenderObjects;
}

void SEASON3B::CNewUIOptionWindow::change_resolution()
{
	int index = gwinhandle->GetDisplayIndex(resolutionList.as_string());

	if (m_Resolution != index)
	{
		leaf::CRegKey regkey;

		regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu2\\Config");

		if (regkey.WriteDword("Resolution", index))
		{
			m_Resolution = index;

			double backupWidth = gwinhandle->GetScreenX();

			double backupHight = gwinhandle->GetScreenY();

			gwinhandle->SetDisplayIndex(index, false);

			CameraFactorPtr->Init();

			OpenFont();

			ClearInput(TRUE);

			CInput& rInput = CInput::Instance();

			rInput.Create(gwinhandle->GethWnd(), WindowWidth, WindowHeight);

			g_pMoveCommandWindow->SetPos(1, 1);

			this->SetPos(0, 70);

			g_pNewUISystem->RenderFrameUpdate(backupWidth, backupHight);

			g_pNewUI3DRenderMng->Reload3DEffectObject(WindowWidth, WindowHeight);
		}
	}
}

void SEASON3B::CNewUIOptionWindow::change_fontsize()
{
	int fontsize = fonttextList.as_interget();

	if (fontsize != FontHeight)
	{
		gwinhandle->SetFontSize(fontsize);

		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file("Data\\Resolutions.xml");

		if (res.status == pugi::status_ok)
		{
			pugi::xml_node root = file.child("ResolutionsInfo");

			for (pugi::xml_node child = root.child("Resolution"); child; child = child.next_sibling())
			{
				std::string text_name = child.attribute("name").as_string();

				resolutionList.push_back(text_name);

				if (gwinhandle->GetDisplayIndex() == child.attribute("index").as_int())
				{
					child.attribute("font_size").set_value(fontsize);

					file.save_file("Data\\Resolutions.xml");
					return;
				}
			}
		}
	}
}

void SEASON3B::CNewUIOptionWindow::LoadResolution(const char* filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		return;
	}

	int fontSize = 13;
	int fontindex = 0;

	pugi::xml_node root = file.child("ResolutionsInfo");

	for (pugi::xml_node child_io = root.child("Resolution"); child_io; child_io = child_io.next_sibling())
	{
		std::string text_name = child_io.attribute("name").as_string();

		resolutionList.push_back(text_name);

		if (gwinhandle->GetDisplayIndex() == child_io.attribute("index").as_int())
		{
			fontSize = child_io.attribute("font_size").as_int();
		}
	}

	root = file.child("FontSize");

	int i = 0;

	for (pugi::xml_node child_io = root.child("size"); child_io; child_io = child_io.next_sibling(), i++)
	{
		std::string text_name = child_io.attribute("fontsize").as_string();

		fonttextList.push_back(text_name);

		if (fontSize == child_io.attribute("fontsize").as_int())
		{
			fontindex = i;
		}
	}

	fonttextList.SetCurrent(fontindex);

	resolutionList.SetCurrent(gwinhandle->GetDisplayIndex());
}
