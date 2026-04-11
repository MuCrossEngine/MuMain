// NewUIGuildInfoWindow.cpp: implementation of the CNewUIGuildInfoWindow class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "NewUIMiniMap.h"
#include "NewUISystem.h"
#include "NewUICommonMessageBox.h"
#include "NewUICustomMessageBox.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "NewUIGuildInfoWindow.h"
#include "NewUIButton.h"
#include "NewUIMyInventory.h"
#include "CSitemOption.h"
#include "MapManager.h"
#include "ZzzAI.h"
#include "ZzzLodTerrain.h"
#include "ScaleForm.h"
#include "TextClien.h"

using namespace SEASON3B;

extern int TargetX;
extern int TargetY;
extern BYTE m_OccupationState;
extern WORD TerrainWall[TERRAIN_SIZE * TERRAIN_SIZE];

SEASON3B::CNewUIMiniMap::CNewUIMiniMap()
{
	m_pNewUIMng = NULL;

	m_Lenth[0].x = 800;
	m_Lenth[1].x = 1000;
	m_Lenth[2].x = 1200;
	m_Lenth[3].x = 1400;
	m_Lenth[4].x = 1600;
	m_Lenth[5].x = 1800;
	m_Lenth[0].y = 800;
	m_Lenth[1].y = 1000;
	m_Lenth[2].y = 1200;
	m_Lenth[3].y = 1400;
	m_Lenth[4].y = 1600;
	m_Lenth[5].y = 1800;
	m_MiniPos = 0;
	m_bSuccess = false;


	m_TooltipText = "";
	m_TooltipTextColor = -1;

	m_Move = false;
	m_Map = -1;
	m_MoveX = -1;
	m_MoveY = -1;

	m_State = true;

	this->runtime_move();
}

SEASON3B::CNewUIMiniMap::~CNewUIMiniMap()
{
	Release();
}

bool SEASON3B::CNewUIMiniMap::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (NULL == pNewUIMng)
		return false;

	m_pNewUIMng = pNewUIMng;
	m_pNewUIMng->AddUIObj(SEASON3B::INTERFACE_MINI_MAP, this);

	this->LoadImages();

#if MAIN_UPDATE > 303
	m_BtnExit.ChangeButtonImgState(true, IMAGE_MINIMAP_INTERFACE + 6, false);
	m_BtnExit.ChangeButtonInfo(x + 610, y + 3, 85, 85);
	m_BtnExit.ChangeToolTipText(GlobalText[1002], false);

	this->SetPos(x, y);
#endif

	this->Zoom(3.f);

	Show(false);

	return true;
}

void SEASON3B::CNewUIMiniMap::Release()
{
	UnloadImages();

	(&m_WayPoint)->clear();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CNewUIMiniMap::LoadImages()
{
	LoadBitmap("Interface\\mini_map_ui_corner.tga", IMAGE_MINIMAP_INTERFACE + 1, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_line.jpg", IMAGE_MINIMAP_INTERFACE + 2, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_cha.tga", IMAGE_MINIMAP_INTERFACE + 3, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_portal.tga", IMAGE_MINIMAP_INTERFACE + 4, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_npc.tga", IMAGE_MINIMAP_INTERFACE + 5, GL_LINEAR);
	LoadBitmap("Interface\\mini_map_ui_cancel.tga", IMAGE_MINIMAP_INTERFACE + 6, GL_LINEAR);

	LoadBitmap("Interface\\HUD\\minimap_frame.tga", IMAGE_MINIMAP_INTERFACE + 1, GL_LINEAR);
}

void SEASON3B::CNewUIMiniMap::UnloadImages()
{
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 1);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 2);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 3);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 4);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 5);
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE + 6);
}

float SEASON3B::CNewUIMiniMap::GetLayerDepth()
{
	return 8.1f;
}

void SEASON3B::CNewUIMiniMap::SetPos(int x, int y)
{
#if MAIN_UPDATE > 303
	m_BtnExit.ChangeButtonInfo(GetWindowsX - 27, 3, 30, 25);
#endif
}

void SEASON3B::CNewUIMiniMap::SetBtnPos(int Num, float x, float y, float nx, float ny)
{
	m_Btn_Loc[Num][0] = x;
	m_Btn_Loc[Num][1] = y;
	m_Btn_Loc[Num][2] = nx;
	m_Btn_Loc[Num][3] = ny;
}

bool SEASON3B::CNewUIMiniMap::UpdateKeyEvent()
{
	if (g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP))
	{
		if (IsPress(VK_ESCAPE) == true || IsPress(VK_TAB) == true)
		{
			g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
			PlayBuffer(SOUND_CLICK01);
			return false;
		}
	}

	return true;
}

bool SEASON3B::CNewUIMiniMap::Render()
{
	EnableAlphaTest();
	glColor4f(1.f, 1.f, 1.f, 1.f);

	if (m_bSuccess)
	{
		runtime_render_map(true);
	}

	DisableAlphaBlend();

	return m_bSuccess;
}

bool SEASON3B::CNewUIMiniMap::Update()
{
	return true;
}

void SEASON3B::CNewUIMiniMap::LoadImages(const char* Filename)
{
	char Fname[300];
	sprintf(Fname, "Data\\Interface\\HUD\\NaviMap\\Navimap%02d.ozt", World + 1);
	FILE* pFile = fopen(Fname, "rb");

#if MAIN_UPDATE > 303
	g_pItemEnduranceInfo->HideInterfaceWorld(false);
#endif
	DeleteBitmap(IMAGE_MINIMAP_INTERFACE);

	if (pFile == NULL)
	{
		m_bSuccess = false;
		return;
	}
	else
	{
		fclose(pFile);
		sprintf(Fname, "Interface\\HUD\\NaviMap\\Navimap%02d.tga", World + 1);
		LoadBitmap(Fname, IMAGE_MINIMAP_INTERFACE, GL_LINEAR);
		m_bSuccess = true;
	}

#if MAIN_UPDATE > 303
#ifdef CHINESE_LANGUAGE
	sprintf(Fname, "Data\\Local\\Minimap\\Minimap_%s.bmd", Filename);
#else
	sprintf(Fname, "Data\\Local\\%s\\Minimap\\Minimap_%s_%s.bmd", g_strSelectedML.c_str(), Filename, g_strSelectedML.c_str());
#endif // CHINESE_LANGUAGE
#else
	sprintf(Fname, "Data\\Local\\Minimap\\Minimap_%s.bmd", Filename);
#endif

	m_MinimapData.clear();

	FILE* fp = fopen(Fname, "rb");

	if (fp != NULL)
	{
		int Size = sizeof(MINI_MAP);
		BYTE* Buffer = new BYTE[Size * MAX_MINI_MAP_DATA + 45];
		fread(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 1, fp);

		DWORD dwCheckSum;
		fread(&dwCheckSum, sizeof(DWORD), 1, fp);
		fclose(fp);

		if (dwCheckSum != GenerateCheckSum2(Buffer, (Size * MAX_MINI_MAP_DATA) + 45, 0x2BC1))
		{
			char Text[256];
			sprintf(Text, "%s - File corrupted.", Fname);
			g_ErrorReport.Write(Text);
			MessageBox(gwinhandle->GethWnd(), Text, NULL, MB_OK);
			SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
		}
		else
		{
			BYTE* pSeek = Buffer;

			for (int i = 0; i < MAX_MINI_MAP_DATA; i++)
			{
				MINI_MAP info;
				BuxConvert(pSeek, Size);
				memcpy(&info, pSeek, Size);

				if (info.Name[0] != '\0' && info.Kind > 0 && info.Kind < 7)
				{
					m_MinimapData.push_back(info);
				}
				pSeek += Size;
			}
		}

		delete[] Buffer;
	}
}

bool SEASON3B::CNewUIMiniMap::IsSuccess()
{
	return m_bSuccess;
}

bool SEASON3B::CNewUIMiniMap::UpdateMouseEvent()
{
	if (this->runtime_update_target())
	{
		return false;
	}

	return true;



#if MAIN_UPDATE > 303
	bool ret = true;

	if (m_BtnExit.UpdateMouseEvent() == true)
	{
		g_pNewUISystem->Hide(SEASON3B::INTERFACE_MINI_MAP);
		return true;
	}

	if (IsPress(VK_LBUTTON))
	{
		ret = Check_Mouse(MouseX, MouseY);
		if (ret == false)
		{
			PlayBuffer(SOUND_CLICK01);
		}

	}

	if (CheckMouseIn(0, 0, GetWindowsX, GetWindowsY))
	{
		return false;
	}
	return ret;
#endif
}

bool SEASON3B::CNewUIMiniMap::Check_Mouse(int mx, int my)
{
	return true;
}

bool SEASON3B::CNewUIMiniMap::Check_Btn(int mx, int my)
{
#if MAIN_UPDATE > 303
	for (int i = 0; i < MAX_MINI_MAP_DATA && m_MinimapData[i].Kind > 0; i++)
	{
		if (mx > m_Btn_Loc[i][0] && mx < (m_Btn_Loc[i][0] + m_Btn_Loc[i][2]) && my > m_Btn_Loc[i][1] && my < (m_Btn_Loc[i][1] + m_Btn_Loc[i][3]))
		{
			SIZE Fontsize;
			m_TooltipText = (unicode::t_string)m_MinimapData[i].Name;
			g_pRenderText->SetFont(g_hFont);
			g_pMultiLanguage->_GetTextExtentPoint32(g_pRenderText->GetFontDC(), m_TooltipText.c_str(), m_TooltipText.size(), &Fontsize);

			Fontsize.cx = Fontsize.cx / g_fScreenRate_x;
			Fontsize.cy = Fontsize.cy / g_fScreenRate_y;

			int x = m_Btn_Loc[i][0] + ((m_Btn_Loc[i][2] / 2) - (Fontsize.cx / 2));
			int y = m_Btn_Loc[i][1] + m_Btn_Loc[i][3] + 2;

			y = m_Btn_Loc[i][1] - (Fontsize.cy + 2);

			DWORD backuptextcolor = g_pRenderText->GetTextColor();
			DWORD backuptextbackcolor = g_pRenderText->GetBgColor();

			g_pRenderText->SetTextColor(RGBA(255, 255, 255, 255));
			g_pRenderText->SetBgColor(RGBA(0, 0, 0, 180));
			g_pRenderText->RenderText(x, y, m_TooltipText.c_str(), Fontsize.cx + 6, 0, RT3_SORT_CENTER);

			g_pRenderText->SetTextColor(backuptextcolor);
			g_pRenderText->SetBgColor(backuptextbackcolor);

			return true;
		}
	}
#endif
	return false;
}

bool SEASON3B::CNewUIMiniMap::movement_automatic()
{
	if (this->IsMoving())
	{
		if (this->m_Map != World || Hero->Dead != 0)
		{
			this->StopMove();
			return false;
		}

		auto pos = (&m_WayPoint)->front();

		float distance = sqrt(pow((float)(Hero->PositionX - pos.first), 2) + pow((float)(Hero->PositionY - pos.second), 2));

		if (distance < 2)
		{
			(&m_WayPoint)->pop_front();

			if (!(&m_WayPoint)->empty())
			{
				pos = (&m_WayPoint)->front();

				if (Hero->Movement != 0 && PathFinding2(Hero->PositionX, Hero->PositionY, pos.first, pos.second, &Hero->Path, 0.f))
				{
					SendMove(Hero, &Hero->Object);
				}

				gfxNotice::addLongMovementText(gTextClien.TextClien_Khac[2], 20, 150, 2000); //"Auto-Moving"
			}
		}
		else if (distance >= 12.f)
		{
			this->StopMove();
		}
		else if (Hero->Movement == 0 && PathFinding2(Hero->PositionX, Hero->PositionY, pos.first, pos.second, &Hero->Path, 0.f))
		{
			if (checkNormalizer)
			{
				Hero->Movement = true;
				SendMove(Hero, &Hero->Object);
				gfxNotice::addLongMovementText(gTextClien.TextClien_Khac[2], 20, 150, 2000); //"Auto-Moving"
			}
		}

		return false;
	}

	return true;
}

void SEASON3B::CNewUIMiniMap::OpenningProcess()
{
}

void SEASON3B::CNewUIMiniMap::ClosingProcess()
{
}

bool SEASON3B::CNewUIMiniMap::IsMoving()
{
	return !(&m_WayPoint)->empty();
}

void SEASON3B::CNewUIMiniMap::StopMove()
{
	if (this->IsMoving())
	{
		(&m_WayPoint)->clear();

		if (Hero->Movement)
		{
			SetPlayerStop(Hero);
		}
	}
}

float SEASON3B::CNewUIMiniMap::GetZoom()
{
	return this->m_RenderZoom;
}

void SEASON3B::CNewUIMiniMap::Zoom(float value)
{
	this->m_RenderZoom = max(1.f, min(this->m_RenderZoom + value, 6.f));
}

void SEASON3B::CNewUIMiniMap::runtime_move()
{
	if (this->IsVisible())
	{
		this->m_RenderFrameX = (70.f * g_fScreenRate_x);
		this->m_RenderFrameY = (70.f * g_fScreenRate_y);
		this->m_RenderSizeX = (GetWindowsX - 140.f) * g_fScreenRate_x;
		this->m_RenderSizeY = (GetWindowsY - 166.f) * g_fScreenRate_y;
	}
	else
	{
		this->m_RenderFrameX = (GetWindowsX - 128.f) * g_fScreenRate_x;
		this->m_RenderFrameY = (15.f * g_fScreenRate_y);
		this->m_RenderSizeX = (128.f * g_fScreenRate_x);
		this->m_RenderSizeY = (128.f * g_fScreenRate_y);
	}
}

bool SEASON3B::CNewUIMiniMap::runtime_update_target()
{
	if (SEASON3B::CheckMouseIn(this->m_RenderFrameX, this->m_RenderFrameY, this->m_RenderSizeX, this->m_RenderSizeY, false))
	{
		if (MouseWheel != 0)
		{
			this->Zoom(0.5f * ((MouseWheel < 0) ? -1 : 1));
			MouseWheel = 0;
		}

#if MAIN_UPDATE > 303
		if (SEASON3B::IsRepeat(VK_CONTROL) || this->IsVisible() == false)
		{
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				if (this->IsMoving())
				{
					this->StopMove();
				}
				else
				{
					int Tx = 0, Ty = 0;

					const float mouseTx = MouseRenderX - (this->m_RenderFrameX + (this->m_RenderSizeX * 0.5f));
					const float mouseTy = MouseRenderY - (this->m_RenderFrameY + (this->m_RenderSizeY * 0.5f));

					if (this->runtime_calc_target(mouseTx, mouseTy, &Tx, &Ty))
					{
						this->m_MoveX = Tx;
						this->m_MoveY = Ty;
						this->m_Map = World;
						this->FindPath(Hero->PositionX, Hero->PositionY, this->m_MoveX, this->m_MoveY);
					}
				}
			}
			return true;
		}
#else
		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (this->IsMoving())
			{
				this->StopMove();
			}
			else
			{
				int Tx = 0, Ty = 0;

				const float mouseTx = MouseRenderX - (this->m_RenderFrameX + (this->m_RenderSizeX * 0.5f));
				const float mouseTy = MouseRenderY - (this->m_RenderFrameY + (this->m_RenderSizeY * 0.5f));

				if (this->runtime_calc_target(mouseTx, mouseTy, &Tx, &Ty))
				{
					this->m_MoveX = Tx;
					this->m_MoveY = Ty;
					this->m_Map = World;

					gfxNotice::addLongMovementText("Auto-Moving", 20, 150, 2000);
					this->FindPath(Hero->PositionX, Hero->PositionY, this->m_MoveX, this->m_MoveY);
				}
			}
		}
		return true;
#endif
	}
	return false;
}

void SEASON3B::CNewUIMiniMap::runtime_render_map(bool rendername)
{
	float Matrix[3][4];
	vec3_t rot, p1, p2, angle;

	this->runtime_move();

	glEnable(GL_SCISSOR_TEST);
	glScissor((GLsizei)this->m_RenderFrameX, (GLsizei)(WindowHeight - (this->m_RenderFrameY + this->m_RenderSizeY)), (GLsizei)this->m_RenderSizeX, (GLsizei)this->m_RenderSizeY);

	glColor4f(0.f, 0.f, 0.f, 0.25f);
	RenderColor(this->m_RenderFrameX, this->m_RenderFrameY, this->m_RenderSizeX, this->m_RenderSizeY, 0.0, false, false);
	EndRenderColor();

	float _ratio = (MAX_MAP_SIZE / (float)(TERRAIN_SIZE * TERRAIN_SCALE));

	p1[0] = ((-Hero->GetPositionY()) * _ratio) * this->m_RenderZoom;
	p1[1] = ((-Hero->GetPositionX()) * _ratio) * this->m_RenderZoom;
	p1[2] = 0.f;

	Vector(40.f, 45.f, -30.f, rot);

	AngleMatrix(rot, Matrix);
	VectorRotate(p1, Matrix, p2);

	Vector(40.f, -45.f, 30.f, angle);

	const float Tx = this->m_RenderFrameX + (this->m_RenderSizeX * 0.5f);
	const float Ty = this->m_RenderFrameY + (this->m_RenderSizeY * 0.5f);

	RenderBitmapLocalProjection(IMAGE_MINIMAP_INTERFACE, Tx + p2[0], Ty + p2[1], (512 * this->m_RenderZoom), (512 * this->m_RenderZoom), angle, 0.f, 0.f, 1.f, 1.f, false);

	for (size_t i = 0; i < m_MinimapData.size(); i++)
	{
		float Rot_Loc = (double)m_MinimapData[i].Rotation;
		runtime_render_objet(i, m_MinimapData[i].Location[0], m_MinimapData[i].Location[1], 1, m_MinimapData[i].Kind, 0.0, m_MinimapData[i].Name, rot);
	}

	if (this->IsMoving())
	{
		for (std::deque<PAIR>::iterator it = (&m_WayPoint)->begin(); it != (&m_WayPoint)->end(); it++)
		{
			runtime_render_objet(-1, it->first, it->second, 1, 6, 0.f, NULL, rot);
		}
	}

	if (rendername)
	{
		g_pRenderText->SetTextColor(CLRDW_WHITE);
		g_pRenderText->SetBgColor(0, 0, 0, 125);

		for (size_t i = 0; i < m_MinimapData.size(); i++)
		{
			runtime_render_name(i, m_MinimapData[i].Name);
		}
	}

	glDisable(GL_SCISSOR_TEST);

	float Rot = (((BYTE)((Hero->Object.Angle[2] + 22.5f) / 360.f * 8.f + 1.f) % 8) + 2) * 45.0;

	RenderFrameAnimation2(BITMAP_ITEM_ENDURANCE_INFO_BEGIN + 13, Tx, Ty, 40.f, 40.f, Rot, 32.0 / 512.0, 32.0 / 128.0, 1.25, 10, 30, false);
	//RenderBitmapRotate(IMAGE_MINIMAP_INTERFACE + 1, Tx, Ty, 40.f, 40.f, Hero->Object.Angle[2] + 135.f, 0.f, 0.f, 32.f / 64.f, 32.f / 256.f, false);
}

void SEASON3B::CNewUIMiniMap::runtime_render_objet(int i, int x, int y, int group, int type, float angle, const char* name, const vec3_t angles)
{
	vec3_t p1, p2;
	float Matrix[3][4];

	if (group == 0 && type == 0)
	{
		type = 7;
	}

	const float su = (32.f / 64.f) * ((type - 1) % 2);
	const float sv = (32.f / 256.f) + ((32.f / 256.f) * ((type - 1) / 2));
	const float ratio = (MAX_MAP_SIZE / (float)(TERRAIN_SIZE * TERRAIN_SCALE)) * m_RenderZoom;

	p1[0] = (((y * TERRAIN_SCALE) - Hero->GetPositionY()) * ratio);
	p1[1] = (((x * TERRAIN_SCALE) - Hero->GetPositionX()) * ratio);
	p1[2] = 0.f;

	AngleMatrix(angles, Matrix);
	VectorRotate(p1, Matrix, p2);

	const float Tx = this->m_RenderFrameX + (this->m_RenderSizeX * 0.5f) + p2[0];
	const float Ty = this->m_RenderFrameY + (this->m_RenderSizeY * 0.5f) + p2[1];

	//RenderBitmapLocalRotate(IMAGE_MINIMAP_INTERFACE + 5, Tx, Ty, 16.f, 16.f, angle, su, sv, 0.546875, 0.546875, false);
	RenderBitmapLocalRotate2(IMAGE_MINIMAP_INTERFACE + 1, Tx, Ty, 26.f, 26.f, angle, su, sv, 32.f / 64.f, 32.f / 256.f, false);

	if (name && name[0])
	{
		m_Btn_Loc[i][0] = (Tx);
		m_Btn_Loc[i][1] = (Ty - 25.f);
	}
}

void SEASON3B::CNewUIMiniMap::runtime_render_name(int i, const char* name)
{
	if (name && name[0])
	{
		g_pRenderText->RenderTextClipped(m_Btn_Loc[i][0], m_Btn_Loc[i][1], name, 0, 0, 8, 0);
	}
}

bool SEASON3B::CNewUIMiniMap::runtime_calc_target(float mx, float my, int* Tx, int* Ty)
{
	float matrix[3][4];
	vec3_t p1, p2, r;

	Vector(40.f, 45.f, -30.f, r);
	AngleMatrix(r, matrix);

	const float ratio = TERRAIN_SCALE * (MAX_MAP_SIZE / (float)(TERRAIN_SIZE * TERRAIN_SCALE)) * m_RenderZoom;

	for (int y = 0; y < TERRAIN_SIZE; y++)
	{
		for (int x = 0; x < TERRAIN_SIZE; x++)
		{
			p1[0] = (y - Hero->PositionY) * ratio;
			p1[1] = (x - Hero->PositionX) * ratio;
			p1[2] = 0.f;

			VectorRotate(p1, matrix, p2);

			if (abs(mx - p2[0]) <= 4.f && abs(my - p2[1]) <= 4.f)
			{
				(*Tx) = x;
				(*Ty) = y;
				return (*Tx >= 0 && *Tx < TERRAIN_SIZE&&* Ty >= 0 && *Ty < TERRAIN_SIZE);
			}
		}
	}

	return false;
}
