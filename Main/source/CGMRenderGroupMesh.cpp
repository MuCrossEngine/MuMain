#include "stdafx.h"
#include "MonkSystem.h"
#include "UIControls.h"
#include "DSPlaySound.h"
#include "ZzzInterface.h"
#include "NewUICommonMessageBox.h"
#include "UIControlRender.h"
#ifndef __ANDROID__
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#endif
#include "CGMRenderGroupMesh.h"
#include "NewUISystem.h"
#include "Utilities/Log/muConsoleDebug.h"

#ifdef EFFECT_KERNEL_VERTEX

CGMRenderGroupMesh::CGMRenderGroupMesh()
{
	this->runtime_make_edit(0);
	// Lista de valores base
	renderaction = TRUE;
	std::vector<int> baseValues = {
		RENDER_TEXTURE, RENDER_CHROME, RENDER_METAL,
		RENDER_BRIGHT, RENDER_EXTRA, RENDER_CHROME2,
		RENDER_CHROME3, RENDER_CHROME4, RENDER_CHROME5,
		RENDER_CHROME6, RENDER_CHROME7, RENDER_CHROME8
	};

	render_flag = runtime_make_renderflag(baseValues);
}

CGMRenderGroupMesh::~CGMRenderGroupMesh()
{
	render_flag.clear();
	m_RenderItem.clear();
	m_RenderMonster.clear();
	m_DefaultRenderType.clear();
}

CGMRenderGroupMesh* CGMRenderGroupMesh::Instance()
{
	static CGMRenderGroupMesh s_instance;
	return &s_instance;
}

void CGMRenderGroupMesh::LoadData(int Type, RENDER_MESH_EFFECT* Info, size_t Size)
{
	for (size_t i = 0; i < Size; i++)
	{
		this->push_back(Info[i].Index, Info[i], Type);
	}
}

void CGMRenderGroupMesh::LoadData(RENDER_MESH_EFFECT_DEFAULT* Info, size_t Size)
{
	for (size_t i = 0; i < Size; i++)
	{
		m_DefaultRenderType[Info[i].ItemIndex] = Info[i];
	}
}

void CGMRenderGroupMesh::push_back(int Index, RENDER_MESH_EFFECT info, int Type)
{
	if (Type == RENDER_MESH_OBJ_ITEM)
	{
		this->m_RenderItem[Index].Group.push_back(info);
	}
	if (Type == RENDER_MESH_OBJ_MONSTER)
	{
		this->m_RenderMonster[Index].Group.push_back(info);
	}
}

RENDER_GROUP_MESH_EFFECT* CGMRenderGroupMesh::FindItemRender(int ItemIndex)
{
	std::map<int, RENDER_GROUP_MESH_EFFECT>::iterator li = this->m_RenderItem.find(ItemIndex);

	if (li != this->m_RenderItem.end())
	{
		return &li->second;
	}

	return NULL;
}

int CGMRenderGroupMesh::FindDefaultRender(int ItemIndex)
{
	std::map<int, RENDER_MESH_EFFECT_DEFAULT>::iterator li = this->m_DefaultRenderType.find(ItemIndex);

	if (li != this->m_DefaultRenderType.end())
	{
		return li->second.EffectIndex;
	}

	return -1;
}

bool CGMRenderGroupMesh::runtime_render_obj(int Index, BMD* pModel, OBJECT* pObject, int Type)
{
	bool success = false;

	RENDER_GROUP_MESH_EFFECT* data = NULL;

	if (Type == RENDER_MESH_OBJ_ITEM)
	{
		data = this->FindItemRender(Index);

		if (data)
		{
			success = true;

			vec3_t BodyLight;

			VectorCopy(pModel->BodyLight, BodyLight);

			for (size_t i = 0; i < data->size(); i++)
			{
				this->runtime_make_render(&data->Group[i], pModel, pObject);
				VectorCopy(BodyLight, pModel->BodyLight);
			}
		}
	}

	return success;
}

bool CGMRenderGroupMesh::runtime_make_item(int modelType, BMD* pModel, OBJECT* pObject)
{
	bool is_rendered = false;

	int itemindex = g_CMonkSystem.EqualItemModelType(modelType) - MODEL_ITEM;

	vec3_t BodyLight;

	if (renderaction == TRUE && (&edit_mesh)->Index == itemindex)
	{
		is_rendered = true;

		VectorCopy(pModel->BodyLight, BodyLight);

		if (edit_item.size() != 0)
		{
			for (size_t i = 0; i < edit_item.size(); i++)
			{
				if (edit_mesh == edit_item[i])
				{
					continue;
				}
				this->runtime_make_render(&edit_item[i], pModel, pObject);
			}

			this->runtime_make_render(&edit_mesh, pModel, pObject);
		}
		else
		{
			for (int i = 0; i < pModel->NumMeshs; i++)
			{
				glColor3fv(pModel->BodyLight);
				if ((&edit_mesh)->GroupId != i)
					pModel->RenderMesh(i, RENDER_TEXTURE, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
				else
					this->runtime_make_render(&edit_mesh, pModel, pObject);
			}
		}
		VectorCopy(BodyLight, pModel->BodyLight);
	}
	else
	{
		RENDER_GROUP_MESH_EFFECT* data = this->FindItemRender(itemindex);

		if (data)
		{
			is_rendered = true;
			VectorCopy(pModel->BodyLight, BodyLight);
			for (size_t i = 0; i < data->size(); i++)
			{
				RENDER_MESH_EFFECT* b = &data->Group[i];

				if (!(b->RenderType == 99 && (BodyLight[0] != 1.0 || BodyLight[1] != 1.0 || BodyLight[2] != 1.0)))
				{
					this->runtime_make_render(&data->Group[i], pModel, pObject);
				}
			}
			VectorCopy(BodyLight, pModel->BodyLight);
		}
		else
		{
			int Effectindex = FindDefaultRender(itemindex);

			if (Effectindex != -1)
			{
				is_rendered = true;
				VectorCopy(pModel->BodyLight, BodyLight);
				pModel->runtime_make_render(Effectindex, pObject);
				VectorCopy(BodyLight, pModel->BodyLight);
			}
		}
	}

	return is_rendered;
}

bool CGMRenderGroupMesh::runtime_make_effect(int modelType, BMD* pModel, OBJECT* pObj, float Alpha, int RenderType, int Texture)
{
	bool is_rendered = false;

	int itemindex = g_CMonkSystem.EqualItemModelType(modelType) - MODEL_ITEM;

	//-- render tools edit
	if (renderaction == TRUE && (&edit_mesh)->Index == itemindex)
	{
		is_rendered = true;

		pModel->BeginRender(Alpha);
		if (!pModel->LightEnable)
		{
			if (pObj->Alpha >= 0.99f)
				glColor3fv(pModel->BodyLight);
			else
				glColor4f(pModel->BodyLight[0], pModel->BodyLight[1], pModel->BodyLight[2], pObj->Alpha);
		}

		RENDER_MESH_EFFECT* b;

		if (edit_item.size() != 0)
		{
			for (size_t i = 0; i < edit_item.size(); i++)
			{
				b = &edit_item[i];

				if (edit_mesh == edit_item[i] || !b->RenderLevel)
					continue;

				pModel->RenderMesh(b->GroupId, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
			}

			b = &edit_mesh;
			if (b->RenderLevel)
			{
				pModel->RenderMesh(b->GroupId, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
			}
		}
		else
		{
			b = &edit_mesh;
			for (int i = 0; i < pModel->NumMeshs; i++)
			{
				if (b->GroupId != i)
				{
					pModel->RenderMesh(i, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
				}
				else
				{
					if (!b->RenderLevel)
						continue;

					pModel->RenderMesh(b->GroupId, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
				}
			}
		}
		pModel->EndRender();
	}
	else
	{
		//-- render maininfo list
		RENDER_GROUP_MESH_EFFECT* data = this->FindItemRender(itemindex);
		if (data)
		{
			is_rendered = true;
			pModel->BeginRender(Alpha);

			if (!pModel->LightEnable)
			{
				if (pObj->Alpha >= 0.99f)
					glColor3fv(pModel->BodyLight);
				else
					glColor4f(pModel->BodyLight[0], pModel->BodyLight[1], pModel->BodyLight[2], pObj->Alpha);
			}

			vec3_t _BodyLight;
			VectorCopy(pModel->BodyLight, _BodyLight);

			for (size_t i = 0; i < data->size(); i++)
			{
				RENDER_MESH_EFFECT* b = &data->Group[i];

				if (b->RenderLevel == TRUE)
				{
					if (!(b->RenderType == 99 && (_BodyLight[0] != 1.0 || _BodyLight[1] != 1.0 || _BodyLight[2] != 1.0)))
					{
						pModel->RenderMesh(b->GroupId, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
					}
				}
			}
			pModel->EndRender();
		}
		else
		{
			int Effectindex = FindDefaultRender(itemindex);

			if (Effectindex != -1)
			{
				is_rendered = true;

				pModel->BeginRender(Alpha);
				if (!pModel->LightEnable)
				{
					if (pObj->Alpha >= 0.99f)
						glColor3fv(pModel->BodyLight);
					else
						glColor4f(pModel->BodyLight[0], pModel->BodyLight[1], pModel->BodyLight[2], pObj->Alpha);
				}

				pModel->runtime_make_effect(Effectindex, pObj, Alpha, RenderType, Texture);
				pModel->EndRender();
			}
		}
	}


	return is_rendered;
}

void CGMRenderGroupMesh::runtime_make_render(RENDER_MESH_EFFECT* b, BMD* pModel, OBJECT* pObject)
{
	int Texture = 0;
	int blend_mesh = 0;
	int RenderFlag = 0;
	float blend_mesh_light = 0;
	float blend_mesh_tex_coord_u = 0;
	float blend_mesh_tex_coord_v = 0;

	Texture = b->Texture;
	RenderFlag = b->RenderFlag;

	if (RenderFlag <= 0)
	{
		blend_mesh = pObject->BlendMesh;
		blend_mesh_light = pObject->BlendMeshLight;
		blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
		blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;

		pModel->RenderBody(-(RenderFlag), pObject->Alpha, blend_mesh, blend_mesh_light, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, pObject->HiddenMesh, Texture);
	}
	else
	{
		if (RenderFlag > RENDER_TEXTURE)
		{
			blend_mesh = b->GroupId;
			b->NormalizeColor(pModel->BodyLight);
			glColor3fv(pModel->BodyLight);
		}
		else
		{
			blend_mesh = pObject->BlendMesh;

			if (b->BodyLight == 0)
				glColor3fv(pModel->BodyLight);
			else
				glColor3f(b->red(), b->green(), b->blue());
		}

		if (b->TimeTexture > 0.0)
		{
			float fLuminosity = Render22(-(b->TimeTexture), 0.0);

			fLuminosity = ((fLuminosity * 0.7f) + 0.3f);

			if (b->RenderType < 4)
				blend_mesh_light = (float)(fLuminosity * b->BlendLight);
			else
				blend_mesh_light = fLuminosity;
		}
		else
		{
			blend_mesh_light = pObject->BlendMeshLight;
		}

		DWORD blessedcb01 = 1000;

		if (b->TimeEffect > 0.0)
		{
			blessedcb01 /= b->TimeEffect;
		}

		float blessedcb02 = 0.f;
		float blessedcb03 = 0.f;
		
		if (b->RenderType != 99)
		{
			if (b->RenderType >= 0 && b->RenderType < 4)
			{
				blessedcb02 = (float)((blessedcb01 - timeGetTime()) % (blessedcb01 + 1)) / static_cast<float>(blessedcb01);
			}
			else if (b->RenderType == 4)
			{
				blessedcb01 = static_cast<DWORD>((1.f / b->BlendLight)); // nFrame
				blessedcb01 *= blessedcb01;

				float factor = blessedcb01 / (b->TimeEffect * CLOCKS_PER_SEC);
				blessedcb02 = ((int)(timeGetTime() * factor) % blessedcb01) * b->BlendLight;
				blessedcb03 = ((int)(timeGetTime() * factor) / blessedcb01) * b->BlendLight;
			}
			else if (b->RenderType >= 5)
			{
				blessedcb01 = static_cast<DWORD>((1.f / b->BlendLight)); // nFrame
				float factor = blessedcb01 / (b->TimeEffect * CLOCKS_PER_SEC);
				blessedcb02 = ((int)(timeGetTime() * factor) % blessedcb01) * b->BlendLight;
			}
		}

		switch (b->RenderType)
		{
		case 0: //Movimiento en X
			blend_mesh_tex_coord_u = blessedcb02;
			blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
			break;
		case 1: //Movimiento en Y
			blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
			blend_mesh_tex_coord_v = blessedcb02;
			break;
		case 2: //Movimiento inverso en X
			blend_mesh_tex_coord_u = -blessedcb02;
			blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
			break;
		case 3: //Movimiento inverso en Y
			blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
			blend_mesh_tex_coord_v = -blessedcb02;
			break;
		case 4://Frame 4x4
		{
			blend_mesh_tex_coord_u = blessedcb02;
			blend_mesh_tex_coord_v = blessedcb03;
		}
		break;
		case 5: //Frame 1x4
			blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
			blend_mesh_tex_coord_v = blessedcb02;
			break;
		case 6: //Frame 1x5
			blend_mesh_tex_coord_u = blessedcb02;
			blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
			break;
		default: //Sin Efecto
			blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
			blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
			break;
		}

		pModel->RenderMesh(b->GroupId, RenderFlag, pObject->Alpha, blend_mesh, blend_mesh_light, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, Texture);
	}
}

std::vector<int> CGMRenderGroupMesh::runtime_make_renderflag(const std::vector<int>& values)
{
	std::vector<int> combinations;
	int totalCombinations = 1 << values.size(); // 2^n combinaciones

	for (int i = 1; i < totalCombinations; ++i)
	{ // Inicia en 1 para omitir 0
		int combination = 0;
		for (size_t j = 0; j < values.size(); ++j)
		{
			if (i & (1 << j))
			{
				combination |= values[j];
			}
		}
		combinations.push_back(combination);
	}

	std::sort(combinations.begin(), combinations.end());

	return combinations;
}

void CGMRenderGroupMesh::runtime_make_edit(int body)
{
	if (body == 0)
	{
		(&edit_mesh)->Index = 0;
		item_level = 0;
		edit_item.clear();
	}

	(&edit_mesh)->GroupId = 0; //-- meshId
	(&edit_mesh)->RenderFlag = RENDER_TEXTURE;
	(&edit_mesh)->Texture = -1;
	(&edit_mesh)->BodyLight = -1;
	(&edit_mesh)->RenderType = -1;
	(&edit_mesh)->BlendLight = 1.f;
	(&edit_mesh)->TimeEffect = -1;
	(&edit_mesh)->TimeTexture = -1;
	(&edit_mesh)->RenderLevel = TRUE;
	renderflag = 0;
}

bool CGMRenderGroupMesh::check_full_windows()
{
	float iRenderFrameX = GetWindowsX - 100.f;
	//float iRenderFrameY = GetWindowsY - 52.f;

	if (((int)iRenderFrameX > GetScreenWidth()
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MINI_MAP)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_INGAMESHOP)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_MASTER_LEVEL)
		|| g_pNewUISystem->IsVisible(SEASON3B::INTERFACE_WINDOW_MENU))
		)
	{
		return true;
	}

	return false;
}

void CGMRenderGroupMesh::render()
{
	if (this->check_full_windows() == false && g_MessageBox->IsEmpty())
	{
		if (FindText(Hero->ID, "webzen") || g_isCharacterBuff((&Hero->Object), eBuff_GMEffect) || ((Hero->CtlCode == CTLCODE_20OPERATOR) || (Hero->CtlCode == CTLCODE_08OPERATOR)))
		{
			int frameY = 388;

			if (gmProtect->WindowsResetInfo)
			{
				frameY -= 30;
			}

			SEASON3B::RenderImageF(IMAGE_EFFECT_BTN_SETTING, pos_center(560), pos_botton(frameY), 76.f, 40.f, 0.0, 0.0, 648.0, 340.f);
		}
	}
}

bool CGMRenderGroupMesh::update()
{
	if (this->check_full_windows() == false && g_MessageBox->IsEmpty())
	{
		int frameY = 388;

		if (gmProtect->WindowsResetInfo)
		{
			frameY -= 30;
		}

		if (SEASON3B::CheckMouseIn(pos_center(560), pos_botton(frameY), 76.f, 40.f))
		{
			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				if (FindText(Hero->ID, "webzen") || g_isCharacterBuff((&Hero->Object), eBuff_GMEffect) || ((Hero->CtlCode == CTLCODE_20OPERATOR) || (Hero->CtlCode == CTLCODE_08OPERATOR)))
				{
					SEASON3B::CreateMessageBox(MSGBOX_LAYOUT_CLASS(SEASON3B::CNewUIMakeRenderMessageBoxLayout));
					PlayBuffer(SOUND_CLICK01);
				}
			}
			return true;
		}
	}
	return false;
}

bool CGMRenderGroupMesh::runtime_update_interface()
{
	float RenderFrameX = 0;
	float RenderFrameY = 0;
	float RenderSizeX = 459.f;
	float RenderSizeY = 264.f;

	RenderFrameX = (GetWindowsX - RenderSizeX) * 0.5f;
	RenderFrameY = (((GetWindowsY - 51.f) - RenderSizeY) * 0.5f);

	bool success = false;

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY))
	{
		int type = ((&edit_mesh)->Index / MAX_ITEM_INDEX);

		int subtype = ((&edit_mesh)->Index % MAX_ITEM_INDEX);

		int maxgroup = 0;

		BMD* pModel = gmClientModels->GetModel((&edit_mesh)->Index + MODEL_ITEM);

		if (pModel && pModel->NumMeshs)
		{
			maxgroup = (pModel->NumMeshs - 1);
		}

		this->runtime_make_button_steep(RenderFrameX + 120.f, RenderFrameY + 61.f, 1, 0, MAX_ITEM_TYPE-1, &type, false, false, true);

		this->runtime_make_button_steep(RenderFrameX + 120.f, RenderFrameY + 79.f, 1, 0, MAX_ITEM_INDEX-1, &subtype, false, false, true);

		this->runtime_make_button(IMAGE_EFFECT_BTN_EDIT, RenderFrameX + 150.f, RenderFrameY + 61.f, 39.f, 22.f, 0, true);

		this->runtime_make_button(IMAGE_EFFECT_BTN_SAVE, RenderFrameX + 192.f, RenderFrameY + 61.f, 39.f, 22.f, 1, true);

		this->runtime_make_button(IMAGE_EFFECT_BTN_CLEAR, RenderFrameX + 234.f, RenderFrameY + 61.f, 39.f, 22.f, 2, true);

		//-- right
		this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 101.f, 1, 0, 15, &item_level);

		this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 159.f, 0.01f, 0.f, 1.f, &(&edit_mesh)->BlendLight);

		this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 187.f, 1, 0, maxgroup, &(&edit_mesh)->GroupId);

		this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 215.f, 1, 0, (int)render_flag.size(), &renderflag);

		//-- left
		this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 101.f, 1, 0, 6, &(&edit_mesh)->RenderType, true, true);

		this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 130.f, 0.1f, 0.0f, 10.f, &(&edit_mesh)->TimeEffect, true, true);

		this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 159.f, 1, 33001, 33500, &(&edit_mesh)->Texture, true, true);

		this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 187.f, 0.1f, 0.0f, 10.f, &(&edit_mesh)->TimeTexture, true, true);

		this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 215.f, 1, 0, 1, &(&edit_mesh)->RenderLevel, false, false, true);

		if ((size_t)renderflag < render_flag.size())
		{
			(&edit_mesh)->RenderFlag = render_flag[renderflag];
		}

		if ((type * MAX_ITEM_INDEX) + subtype != (&edit_mesh)->Index)
		{
			(&edit_mesh)->Index = (type * MAX_ITEM_INDEX) + subtype;
		}

		success = true;
	}

	return success;
}

void CGMRenderGroupMesh::runtime_make_interface()
{
	float RenderFrameX = 0;
	float RenderFrameY = 0;
	float RenderSizeX = 459.f;
	float RenderSizeY = 264.f;

	RenderFrameX = (GetWindowsX - RenderSizeX) * 0.5f;
	RenderFrameY = (((GetWindowsY - 51.f) - RenderSizeY) * 0.5f);


	SEASON3B::RenderImageF(IMAGE_EFFECT_BACK_MNG, RenderFrameX, RenderFrameY, 459.f, 264.f, 0.0, 0.0, 918.f, 528.f);

	//static float rgb[3] = { 1.0f, 1.0f, 1.0f };

	int* value = NULL;

	this->runtime_make_text(RenderFrameX, RenderFrameY);

	//-- index item
	this->runtime_make_button_steep(RenderFrameX + 120.f, RenderFrameY + 61.f, 0, 0, 0, value);

	//-- subindex item
	this->runtime_make_button_steep(RenderFrameX + 120.f, RenderFrameY + 79.f, 0, 0, 0, value);

	this->runtime_make_button(IMAGE_EFFECT_BTN_EDIT, RenderFrameX + 150.f, RenderFrameY + 61.f, 39.f, 22.f, 0, false);

	this->runtime_make_button(IMAGE_EFFECT_BTN_SAVE, RenderFrameX + 192.f, RenderFrameY + 61.f, 39.f, 22.f, 1, false);

	this->runtime_make_button(IMAGE_EFFECT_BTN_CLEAR, RenderFrameX + 234.f, RenderFrameY + 61.f, 39.f, 22.f, 2, false);

	//-- right
	this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 101.f, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 159.f, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 187.f, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 260.f, RenderFrameY + 215.f, 0, 0, 0, value);

	//-- left
	this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 101, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 130, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 159, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 187, 0, 0, 0, value);

	this->runtime_make_button_steep(RenderFrameX + 406.f, RenderFrameY + 215.f, 0, 0, 0, value);


	SEASON3B::RenderLocalItem3D(RenderFrameX + 44.f, RenderFrameY + 108.f, 84.f, 117.f, (&edit_mesh)->Index, (item_level << 3));

#ifndef __ANDROID__
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
#endif

	ImGui::NewFrame();
	ImGui::SetNextWindowPos(ImVec2(RenderFrameX * g_fScreenRate_x, RenderFrameY * g_fScreenRate_y));
	ImGui::SetNextWindowSize(ImVec2(RenderSizeX * g_fScreenRate_x, RenderSizeY * g_fScreenRate_y));

	static bool GraphicImage = true;

	ImGui::Begin("RenderMesh", &GraphicImage, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground);

	render_widget_color(202.f, 136.f, 76.0, &((&edit_mesh)->BodyLight));

	ImGui::End();

	ImGui::Render();

#ifndef __ANDROID__
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#endif
}

void CGMRenderGroupMesh::runtime_make_text(float RenderFrameX, float RenderFrameY)
{
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(-1);

	char strText[255] = { 0, };

	sprintf_s(strText, "%d", (int)((&edit_mesh)->Index / 512));
	g_pRenderText->RenderFont(RenderFrameX + 76, RenderFrameY + 64, strText, 21, 15, 3);

	sprintf_s(strText, "%d", (int)((&edit_mesh)->Index % 512));
	g_pRenderText->RenderFont(RenderFrameX + 76, RenderFrameY + 81, strText, 21, 15, 3);

	//-- colores
	sprintf_s(strText, "%d", item_level);
	g_pRenderText->RenderFont(RenderFrameX + 205, RenderFrameY + 103, strText, 32, 15, 3);

	sprintf_s(strText, "%.2f", (&edit_mesh)->BlendLight);
	g_pRenderText->RenderFont(RenderFrameX + 205, RenderFrameY + 161, strText, 32, 15, 3);

	//-- group id
	sprintf_s(strText, "%d", (&edit_mesh)->GroupId);
	g_pRenderText->RenderFont(RenderFrameX + 205, RenderFrameY + 189, strText, 32, 15, 3);

	//-- renderflag
	sprintf_s(strText, "%d", (&edit_mesh)->RenderFlag);
	g_pRenderText->RenderFont(RenderFrameX + 205, RenderFrameY + 217, strText, 32, 15, 3);


	//-- render type
	if((&edit_mesh)->RenderType >= 0 && (&edit_mesh)->RenderType <= 6)
	{
		sprintf_s(strText, "%d", (&edit_mesh)->RenderType);
	}
	else
	{
		sprintf_s(strText, "Default");
	}
	g_pRenderText->RenderFont(RenderFrameX + 351, RenderFrameY + 103, strText, 32, 15, 3);

	//-- time effect
	sprintf_s(strText, "%.1f", (&edit_mesh)->TimeEffect);
	g_pRenderText->RenderFont(RenderFrameX + 351, RenderFrameY + 132, strText, 32, 15, 3);

	//-- texture id
	sprintf_s(strText, "%d", (&edit_mesh)->Texture);
	g_pRenderText->RenderFont(RenderFrameX + 351, RenderFrameY + 161, strText, 32, 15, 3);

	//-- time texture
	sprintf_s(strText, "%.1f", (&edit_mesh)->TimeTexture);
	g_pRenderText->RenderFont(RenderFrameX + 351, RenderFrameY + 189, strText, 32, 15, 3);

	//-- render level
	sprintf_s(strText, "%s", (&edit_mesh)->RenderLevel != 0 ? "TRUE" : "FALSE");
	g_pRenderText->RenderFont(RenderFrameX + 351, RenderFrameY + 217, strText, 32, 15, 3);


	BMD* pModel = gmClientModels->GetModel((&edit_mesh)->Index + MODEL_ITEM);

	BYTE GroupId = (&edit_mesh)->GroupId;

	if (pModel && (&edit_mesh)->GroupId < pModel->NumMeshs)
	{
		g_pRenderText->SetTextColor(CLRDW_GOLD);
		g_pRenderText->RenderFont(RenderFrameX + 44.f, RenderFrameY + 232, pModel->Textures[GroupId].FileName, 0, 15, 0);
	}

}

bool CGMRenderGroupMesh::runtime_make_button(int Image, float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY, BYTE Type, bool checkaction)
{
	bool success = false;

	float sv = 0.0;

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY))
	{
		if (MouseLButton) sv += 30.f;

		if (checkaction && SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (Type == 0)
			{
				edit_item.push_back(edit_mesh);
				this->runtime_make_edit(1);
			}
			if (Type == 1)
			{
				this->runtime_make_file();
			}
			if (Type == 2)
			{
				this->runtime_make_edit(0);
			}
			success = true;
		}
		sv += 30.f;
	}

	if (checkaction == false)
	{	
		SEASON3B::RenderImageF(Image, RenderFrameX, RenderFrameY, RenderSizeX, RenderSizeY, 0.0, sv, 54.f, 30.f);
	}

	return success;
}

void CGMRenderGroupMesh::runtime_make_file()
{
	FILE* fp = fopen("RenderEffect.txt", "wb");

	if (fp)
	{
		std::string buffer;

		buffer = "//----------------------------------------------------------------------------------------------------------------------------------------";
		textwrite(buffer, true, fp);

		buffer = "//#Index     #GroupId #RenderFlag #Texture #Color #BodyLightColor     #Luminosity  #RenderType #TimeEffect #TimeTexture #GlowLevel";
		textwrite(buffer, true, fp);

		buffer = "//----------------------------------------------------------------------------------------------------------------------------------------";
		textwrite(buffer, true, fp);

		if (edit_item.size() != 0)
		{
			for (size_t i = 0; i < edit_item.size(); i++)
			{
				RENDER_MESH_EFFECT* info = &edit_item[i];

				buffer += std::to_string((int)(info->Index / MAX_ITEM_INDEX));
				buffer += "    " + std::to_string((int)(info->Index % MAX_ITEM_INDEX));
				buffer += "    " + std::to_string(info->GroupId);
				buffer += "    " + std::to_string(info->RenderFlag);
				buffer += "    " + std::to_string(info->Texture);
				buffer += "    " + std::to_string((int)(info->BodyLight != 0));
				buffer += "    " + std::to_string(info->red());
				buffer += "    " + std::to_string(info->green());
				buffer += "    " + std::to_string(info->blue());
				buffer += "    " + std::to_string(info->BlendLight);

				buffer += "    " + std::to_string(info->RenderType);
				buffer += "    " + std::to_string(info->TimeEffect);
				buffer += "    " + std::to_string(info->TimeTexture);
				buffer += "    " + std::to_string((int)info->RenderLevel);

				if (buffer.size() > 0)
				{
					textwrite(buffer, true, fp);
				}
			}
		}
		else
		{
			RENDER_MESH_EFFECT* info = &edit_mesh;

			buffer += std::to_string((int)(info->Index / MAX_ITEM_INDEX));
			buffer += "    " + std::to_string((int)(info->Index % MAX_ITEM_INDEX));
			buffer += "    " + std::to_string(info->GroupId);
			buffer += "    " + std::to_string(info->RenderFlag);
			buffer += "    " + std::to_string(info->Texture);
			buffer += "    " + std::to_string((int)(info->BodyLight != 0));
			buffer += "    " + std::to_string(info->red());
			buffer += "    " + std::to_string(info->green());
			buffer += "    " + std::to_string(info->blue());
			buffer += "    " + std::to_string(info->BlendLight);

			buffer += "    " + std::to_string(info->RenderType);
			buffer += "    " + std::to_string(info->TimeEffect);
			buffer += "    " + std::to_string(info->TimeTexture);
			buffer += "    " + std::to_string((int)info->RenderLevel);

			if (buffer.size() > 0)
			{
				textwrite(buffer, true, fp);
			}
		}
		fclose(fp);
	}
}
#endif // EFFECT_KERNEL_VERTEX
