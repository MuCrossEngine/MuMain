#include "stdafx.h"
#include "ZzzTexture.h"
#include "NewUISystem.h"
#include "CGMEffectHandle.h"
#include "MonkSystem.h"

#ifndef __ANDROID__
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl2.h"
#endif

#ifdef EFFECT_MNG_HANDLE



SEASON3B::CGFxEffectHandle::CGFxEffectHandle()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
	m_bEditEnchant = 0;
	selectedItem = -1;
	m_bEditEnchant = FALSE;

	memset(&m_bSettings, 0, sizeof(m_bSettings));
}

SEASON3B::CGFxEffectHandle::~CGFxEffectHandle()
{
	this->Release();
}

bool SEASON3B::CGFxEffectHandle::Create(CNewUIManager* pNewUIMng, int x, int y)
{
	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_EFFECT_MANAGER, this);

		this->LoadImages();

		this->runtime_colector_Enchant("Data\\Local\\RenderMesh.bmd");

		this->SetButtonInfo();

		this->SetPos(x, y);

		Show(false);

		return true;
	}
	return false;

}

void SEASON3B::CGFxEffectHandle::Release()
{
	this->UnloadImages();

	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);
		m_pNewUIMng = NULL;
	}
}

void SEASON3B::CGFxEffectHandle::SetPos(int x, int y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

bool SEASON3B::CGFxEffectHandle::UpdateMouseEvent()
{
	return !SEASON3B::CheckMouseIn(0, 0, GetWindowsX, GetWindowsY - 51);
}

bool SEASON3B::CGFxEffectHandle::UpdateKeyEvent()
{
	if (IsVisible())
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_EFFECT_MANAGER);
			return false;
		}
	}
	return true;
}

bool SEASON3B::CGFxEffectHandle::Update()
{
	if (IsVisible())
	{
	}
	return true;
}

bool SEASON3B::CGFxEffectHandle::Render()
{
	EnableAlphaTest();

	glColor4f(1.f, 1.f, 1.f, 1.f);

	//draw_list.clear();

	this->RenderFrame();

	//render_draw_list();

	DisableAlphaBlend();

	return true;
}

void SEASON3B::CGFxEffectHandle::LoadImages()
{
	LoadBitmap("Interface\\HUD\\EDIT\\effect_back_mng.tga", IMAGE_EFFECT_BACK_MNG, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_edit.tga", IMAGE_EFFECT_BTN_EDIT, GL_LINEAR);
	LoadBitmap("Interface\\HUD\\EDIT\\effect_btn_save.tga", IMAGE_EFFECT_BTN_SAVE, GL_LINEAR);
}

void SEASON3B::CGFxEffectHandle::UnloadImages()
{
	DeleteBitmap(IMAGE_EFFECT_BACK_MNG);
	DeleteBitmap(IMAGE_EFFECT_BTN_EDIT);
	DeleteBitmap(IMAGE_EFFECT_BTN_SAVE);
}

void SEASON3B::CGFxEffectHandle::SetButtonInfo()
{

}

void SEASON3B::CGFxEffectHandle::RenderFrame()
{
	// Iniciar frame de ImGui
#ifndef __ANDROID__
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
#endif
	// Iniciar frame de ImGui
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(m_Pos.x * g_fScreenRate_x, m_Pos.y * g_fScreenRate_y));
	ImGui::SetNextWindowSize(ImVec2(460.f * g_fScreenRate_x, 264.f * g_fScreenRate_y));

	static bool GraphicImage = false;

	ImGui::Begin("RenderMesh Tools", &GraphicImage, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse);

	// Sección 1: Arriba izquierda
	ImGui::BeginChild("Child1", ImVec2((160 * g_fScreenRate_x), 0), ImGuiChildFlags_Border);

	ImGui::BeginChild("Sub-Child1", ImVec2(0, (110.f * g_fScreenRate_y)), ImGuiChildFlags_Border);
	ImGui::EndChild();

	ImGui::BeginChild("Sub-Child2", ImVec2(0, 0), ImGuiChildFlags_Border);

	//EditEnchantList.Render("##GroupDataMesh");

	ImGui::EndChild();

	// Contenido de la primera sección
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("Child2", ImVec2(0, 0), ImGuiChildFlags_Border);

	this->RenderContents();

	ImGui::EndChild();

	ImGui::End();

	ImGui::Render();

#ifndef __ANDROID__
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#endif

	//------------------
	if (selectedItem != -1)
	{
		Script_Item* item_info = GMItemMng->find(selectedItem);

		if (item_info->Name[0] != '\0')
		{
			int wmodel = item_info->Width * 20;
			int hmodel = item_info->Height * 20;

			int posx = m_Pos.x + 80 - (wmodel / 2);
			int posy = m_Pos.y + 55 - (hmodel / 2);

			RenderLocalItem3D(posx, posy, wmodel, hmodel, selectedItem);
		}
	}
}

void SEASON3B::CGFxEffectHandle::RenderContents()
{
	reload_collection_item(0);

	ImGui::Text("Selected Item");

	comboBoxItems.Render("##comboItem", "No available");

	int currentindex = comboBoxItems.getSelectedIndex();

	if (selectedItem != currentindex)
	{
		selectedItem = currentindex;

		reload_collection_item(1);
	}

	ImGui::Text("Selected GroupId");

	comboBoxGroup.Render("##comboGroup", "Error");

	m_bSettings.GroupId = comboBoxGroup.getSelectedIndex();

	ImGui::Text("color mesh");

	ImGui::ColorEdit3("RGB", m_bSettings.Color);


	ImGui::Text("RenderFlag");

	ImGui::InputInt("##RenderFlag", &m_bSettings.RenderFlag);


	ImGui::Text("RenderType");

	ImGui::InputInt("##RenderType", &m_bSettings.RenderType, 1);


	ImGui::Text("RenderTime");

	ImGui::InputFloat("##RenderTime", &m_bSettings.TimeEffectType, 0.1);


	ImGui::Text("TextureId");

	ImGui::InputInt("##TextureId", &m_bSettings.TextureID, 1);


	ImGui::Text("TextureTime");

	ImGui::InputFloat("##TextureTime", &m_bSettings.TimeTextureID, 0.1);


	ImGui::NewLine();

	if (ImGui::Button("Edit"))
	{
		m_bEditEnchant = !m_bEditEnchant;
	}
	ImGui::SameLine();
	if (ImGui::Button("Save"))
	{
		runtime_export_settings("RenderEffect.txt");
	}
}

void SEASON3B::CGFxEffectHandle::RenderButtons()
{
}

void SEASON3B::CGFxEffectHandle::reload_collection_item(int currentIndex)
{
	if (comboBoxItems.empty())
	{
		for (int i = 0; i < MAX_ITEM_LINE; i++)
		{
			Script_Item* item_info = GMItemMng->find(i);

			if (item_info->Name[0] != '\0')
			{
				comboBoxItems.push_back(i, item_info->Name);
			}
		}
	}

	if (currentIndex == 1 && selectedItem != -1)
	{
		comboBoxGroup.clear();

		BMD* pModel = gmClientModels->GetModel(selectedItem + MODEL_ITEM);
		if (pModel)
		{
			char nameId[50];
			for (int i = 0; i < pModel->NumMeshs; i++)
			{
				sprintf_s(nameId, "GroupId %d", i + 1);
				comboBoxGroup.push_back(i, nameId);
			}

			VectorCopy(pModel->BodyLight, m_bSettings.Color);
		}
	}
}

float SEASON3B::CGFxEffectHandle::GetLayerDepth()
{
	return 10.5f;
}

float SEASON3B::CGFxEffectHandle::GetKeyEventOrder()
{
	return 10.f;
}

void SEASON3B::CGFxEffectHandle::OpenningProcess()
{
}

void SEASON3B::CGFxEffectHandle::ClosingProcess()
{
}

void SEASON3B::CGFxEffectHandle::AddEnchantEffect(int itemindex, const EnchantEffect& effect)
{
	mapeEnchant[itemindex].buffer.push_back(effect);
}


bool SEASON3B::CGFxEffectHandle::runtime_colector_Enchant(std::string filename)
{
	EnchantEffect* item_info;
	int MaxLine = PackFileDecrypt(filename, item_info, 0, sizeof(EnchantEffect), 0xE2F1);

	if (MaxLine != 0 && item_info != NULL)
	{
		for (int i = 0; i < MaxLine; i++)
		{
			this->AddEnchantEffect(item_info[i].itemindex, item_info[i]);
		}
	}
	SAFE_DELETE_ARRAY(item_info);

	return false;
}

void SEASON3B::CGFxEffectHandle::runtime_export_settings(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");

	std::string buffer;
	buffer += std::to_string((int)(selectedItem >> 9));
	buffer += "    " + std::to_string((int)(selectedItem % 512));
	buffer += "    " + std::to_string(m_bSettings.Color[0]);
	buffer += "    " + std::to_string(m_bSettings.Color[1]);
	buffer += "    " + std::to_string(m_bSettings.Color[2]);
	buffer += "    " + std::to_string(m_bSettings.GroupId);

	buffer += "    " + std::to_string(m_bSettings.RenderFlag);
	buffer += "    " + std::to_string(m_bSettings.RenderType);
	buffer += "    " + std::to_string(m_bSettings.TimeEffectType);
	buffer += "    " + std::to_string(m_bSettings.TextureID);
	buffer += "    " + std::to_string(m_bSettings.TimeTextureID);
	buffer += "    1";
	if (buffer.size() > 0)
	{
		buffer += "\n";
		fwrite(buffer.data(), 1, buffer.size(), fp);
		buffer.clear();
	}
	fclose(fp);
}

bool SEASON3B::CGFxEffectHandle::runtime_render_Enchant(int modelType, BMD* pModel, OBJECT* pObject)
{
	bool is_rendered = false;
	int itemindex = g_CMonkSystem.EqualItemModelType(modelType);

	if (m_bEditEnchant && itemindex == selectedItem)
	{
		is_rendered = true;
		const EnchantEffect* Enchant = &m_bSettings;

		for (int i = 0; i < pModel->NumMeshs; i++)
		{
			if (i != Enchant->GroupId)
			{
				pModel->RenderMesh(i, 2, pObject->Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV, -1);
			}
			else
			{
				runtime_interpret_setting(Enchant, pModel, pObject);
			}
		}
	}
	else
	{
		const std::vector<EnchantEffect>& effects = GetEnchantEffects(itemindex);

		if (!effects.empty())
		{
			for (size_t i = 0; i < effects.size(); i++)
			{
				const EnchantEffect* Enchant = &effects[i];

				runtime_interpret_setting(Enchant, pModel, pObject);
			}
			is_rendered = true;
		}
	}

	return is_rendered;
}

bool SEASON3B::CGFxEffectHandle::runtime_render_NoGlow(int modelType, BMD* pModel, OBJECT* pObject, int RenderType, float Alpha)
{
	bool is_rendered = false;
	int itemindex = g_CMonkSystem.EqualItemModelType(modelType);

	/*if (m_bEditEnchant && itemindex == selectedItem)
	{
		is_rendered = true;
	}
	else
	{*/
	const std::vector<EnchantEffect>& effects = GetEnchantEffects(itemindex);

	if (!effects.empty())
	{
		for (size_t i = 0; i < effects.size(); i++)
		{
			const EnchantEffect* Enchant = &effects[i];

			if (Enchant->NoGlow)
			{
				pModel->RenderMesh(Enchant->GroupId, RenderType, Alpha, pObject->BlendMesh, pObject->BlendMeshLight, pObject->BlendMeshTexCoordU, pObject->BlendMeshTexCoordV);
				is_rendered = true;
			}
		}
	}
	//}
	return is_rendered;
}


void SEASON3B::CGFxEffectHandle::runtime_interpret_setting(const EnchantEffect* Enchant, BMD* pModel, OBJECT* pObject)
{
	int blend_mesh = 0;
	float blend_mesh_light = 0;
	float blend_mesh_tex_coord_u = 0;
	float blend_mesh_tex_coord_v = 0;

	int TextureID = Enchant->TextureID;

	vec3_t BodyLight = { 0.0, 0.0, 0.0 };
	VectorCopy(pModel->BodyLight, BodyLight);

	if (Enchant->RenderFlag == RENDER_TEXTURE)
	{
		if (Enchant->Color[0] == 1.f && Enchant->Color[1] == 1.f && Enchant->Color[2] == 1.f)//PK Fixed
			glColor3fv(pModel->BodyLight);
		else
			glColor3f(Enchant->Color[0], Enchant->Color[1], Enchant->Color[2]);

		blend_mesh = pObject->BlendMesh;
	}
	else
	{
		VectorCopy(Enchant->Color, pModel->BodyLight);
		blend_mesh = Enchant->GroupId;
	}

	float timeeeee = Render22(-(Enchant->TimeTextureID), 0.0) * 0.7 + 0.2;

	if (Enchant->TimeTextureID == -1.f)
	{
		blend_mesh_light = pObject->BlendMeshLight;
	}
	else
	{
		blend_mesh_light = timeeeee;
	}

	int Effect01 = 1000;
	float Effect02 = ((int)(Effect01 - timeGetTime() * Enchant->TimeEffectType) % (Effect01 + 1)) / (double)Effect01;

	int Frame = (((int)((timeGetTime()) * Enchant->TimeEffectType)) % 600 / 40);
	double FrameX = (double)(Frame % 4) * 0.25;
	double FrameY = (double)(Frame / 4) * 0.25;
	float Frame2 = ((int)(timeGetTime() * Enchant->TimeEffectType) % 16 / 4) * 0.25;

	switch (Enchant->RenderType)
	{
	case 0://Movimiento en X
		blend_mesh_tex_coord_u = Effect02;
		blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
		break;
	case 1://Movimiento en Y
		blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
		blend_mesh_tex_coord_v = Effect02;
		break;
	case 2://Movimiento inverso en X
		blend_mesh_tex_coord_u = -Effect02;
		blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
		break;
	case 3://Movimiento inverso en Y
		blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
		blend_mesh_tex_coord_v = -Effect02;
		break;
	case 4://Frame 4x4
		blend_mesh_tex_coord_u = FrameX;
		blend_mesh_tex_coord_v = FrameY;
		break;
	case 5://Frame 1x4
		blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
		blend_mesh_tex_coord_v = Frame2;
		break;
	default://Sin Efecto
		blend_mesh_tex_coord_u = pObject->BlendMeshTexCoordU;
		blend_mesh_tex_coord_v = pObject->BlendMeshTexCoordV;
		break;
	}

	pModel->RenderMesh(Enchant->GroupId, Enchant->RenderFlag, pObject->Alpha, blend_mesh, blend_mesh_light, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, TextureID);
	VectorCopy(BodyLight, pModel->BodyLight);
}

void SEASON3B::ComboBoxGird::Render(const char* nameId, const char* error)
{
	const char* selectedLabel1 = ((unsigned int)selectedId >= comboBoxItems.size()) ? error : comboBoxItems[selectedId].second.c_str();

	int selected = selectedIndex;
	if (ImGui::BeginCombo(nameId, selectedLabel1, 0))
	{
		for (size_t i = 0; i < comboBoxItems.size(); ++i)
		{
			bool isSelected = (selectedIndex == comboBoxItems[i].first);
			if (ImGui::Selectable(comboBoxItems[i].second.c_str(), isSelected))
			{
				selectedId = i;
				selectedIndex = comboBoxItems[i].first;
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus(); // Foco en la opción seleccionada
			}
		}
		ImGui::EndCombo();
	}
}

void SEASON3B::ComboBoxGird::push_back(int id, std::string name)
{
	comboBoxItems.push_back({ id, name });
}

void SEASON3B::GirdListBox::Render(const char* nameId)
{
	if (listItems.size() > 0)
	{
		std::vector<const char*> items_cstr;
		items_cstr.reserve(listItems.size());

		for (const std::string& item : listItems)
		{
			items_cstr.push_back(item.c_str());
		}

		if (ImGui::ListBox(nameId, &selectedIndex, items_cstr.data(), (int)items_cstr.size(), -1))
		{
		}
	}
}

void SEASON3B::GirdListBox::push_back(std::string name)
{
	listItems.push_back(name);
}
#endif // EFFECT_MNG_HANDLE
