#include "stdafx.h"
#include "CGMScriptLua.h"
#include "ZzzInfomation.h"
#include "NewUISystem.h"
#include "./Utilities/Log/muConsoleDebug.h"

CGMLuaBase::CGMLuaBase()
{
	lua.open_libraries(sol::lib::base);
}

CGMLuaBase::~CGMLuaBase()
{
}

bool CGMLuaBase::runtime_link(std::vector<BYTE> DataBytes)
{
	try {
		std::string scriptContent = std::string(DataBytes.begin(), DataBytes.end());
		this->runtime_link_model();
		this->runtime_link_object();
		this->runtime_link_character();
		this->runtime_link_funtion();

		lua.script(scriptContent);
		return true;
	}
	catch (const sol::error& e) {
		//std::cerr << "Error loading script from buffer: " << e.what() << std::endl;
	}
	return false;
}

bool CGMLuaBase::RenderLinkAction(PART_t* f, OBJECT* pObj)
{
	this->pass_to_lua("pObj", pObj);

	sol::function runtime_function_unique = lua["RenderLinkAction"];

	bool Success = false;
	if (runtime_function_unique.valid())
	{
		runtime_function_unique(f);
		Success = true;
	}

	return Success;
}

bool CGMLuaBase::RenderPartObjectBody(BMD* pModel, OBJECT* pObj, float Alpha, int RenderType)
{
	this->pass_to_lua("pObj", pObj);
	this->pass_to_lua("pModel", pModel);
	this->pass_to_lua_value("Texture", 0);
	this->pass_to_lua_value("Alpha", Alpha);
	this->pass_to_lua_value("RenderType", RenderType);
	this->pass_to_lua_table("pBodyLight", pModel->BodyLight, 3);
	//this->pass_to_lua_value("RenderType", RenderType);

	sol::function runtime_function_unique = lua["RenderPartObjectBody"];

	bool Success = false;
	if (runtime_function_unique.valid())
	{
		runtime_function_unique();
		Success = true;
	}

	return Success;
}

bool CGMLuaBase::RenderEffectObjectBody(BMD* pModel, OBJECT* pObj, float Alpha, int RenderType, int Texture)
{
	this->pass_to_lua("pObj", pObj);
	this->pass_to_lua("pModel", pModel);
	this->pass_to_lua_value("Alpha", Alpha);
	this->pass_to_lua_value("Texture", Texture);
	this->pass_to_lua_value("RenderType", RenderType);
	this->pass_to_lua_table("pBodyLight", pModel->BodyLight, 3);

	sol::function runtime_function_unique = lua["RenderEffectObjectBody"];

	bool Success = false;
	if (runtime_function_unique.valid())
	{
		pModel->BeginRender(Alpha);

		if (!pModel->LightEnable)
		{
			if (pObj->Alpha >= 0.99f)
				glColor3fv(pModel->BodyLight);
			else
				glColor4f(pModel->BodyLight[0], pModel->BodyLight[1], pModel->BodyLight[2], pObj->Alpha);
		}
		runtime_function_unique();
		pModel->EndRender();
		Success = true;
	}

	return Success;
}

void CGMLuaBase::runtime_link_model()
{
	lua.new_usertype<BMD>("BMD",
		sol::constructors<BMD()>(),
		"NumMeshs", &BMD::NumMeshs,
		"BodyLight", &BMD::BodyLight,
		"StreamMesh", &BMD::StreamMesh,
		"HideSkin", &BMD::HideSkin,
		"EndRender", &BMD::EndRender,
		"BeginRender", &BMD::BeginRender,
		"GetBodyLight", &BMD::GetBodyLight,
		"RenderMesh", &BMD::RenderMesh,
		"RenderBody", &BMD::RenderBody
		);
}

void CGMLuaBase::runtime_link_object()
{
	lua.new_usertype<OBJECT>("OBJECT",
		sol::constructors<OBJECT()>(),
		"Alpha", &OBJECT::Alpha,
		"BlendMesh", &OBJECT::BlendMesh,
		"CurrentAction", &OBJECT::CurrentAction,
		"BlendMeshLight", &OBJECT::BlendMeshLight,
		"BlendMeshTexCoordU", &OBJECT::BlendMeshTexCoordU,
		"BlendMeshTexCoordV", &OBJECT::BlendMeshTexCoordV,
		"AnimationFrame", &OBJECT::AnimationFrame
		);
}

void CGMLuaBase::runtime_link_character()
{
	lua.new_usertype<PART_t>("PART_t",
		sol::constructors<PART_t()>(),
		"Type", &PART_t::Type,
		"CurrentAction", &PART_t::CurrentAction,
		"PriorAction", &PART_t::PriorAction,
		"AnimationFrame", &PART_t::AnimationFrame,
		"PriorAnimationFrame", &PART_t::PriorAnimationFrame,
		"PlaySpeed", &PART_t::PlaySpeed,
		"m_byNumCloth", &PART_t::m_byNumCloth
		);
}

void CGMLuaBase::runtime_link_funtion()
{
	lua.set_function("RenderAlphaG", &Render22);
	lua.set_function("Vector", &to_funtion__Vector);
	lua.set_function("cast_int", &to_funtion__cast_int);
	lua.set_function("glColor4f", &to_funtion__glColor4f);
	lua.set_function("glColor3fv", &to_funtion__glColor3fv);
	lua.set_function("VectorCopy", &to_funtion__VectorCopy);
	lua.set_function("timeGetTime", &to_funtion__timeGetTime);
}

int to_funtion__cast_int(float number)
{
	return static_cast<int>(number);
}

int to_funtion__timeGetTime()
{
	return timeGetTime();
}

void to_funtion__glColor3fv(float* v)
{
	glColor3fv(v);
}

void to_funtion__glColor4f(float r, float g, float b, float a)
{
	glColor4f(r, g, b, a);
}

void to_funtion__Vector(float a, float b, float c, float* v)
{
	if (v)
	{
		v[0] = a;
		v[1] = b;
		v[2] = c;
	}
}

void to_funtion__VectorCopy(sol::table lua_table, float* b)
{
	if (b)
	{
		b[0] = lua_table.get<float>(1);
		b[1] = lua_table.get<float>(2);
		b[2] = lua_table.get<float>(3);
	}
}

void to_funtion__RenderLevel(int GroupMesh, float Red, float Blue, float Green)
{
	//BMD* b = lua[uniqueId];
}

#ifdef test_main_dev
int findTextureName(std::string name)
{
	return Bitmaps.findByFileName(name);
}

void RenderImageF1(std::string Name, float x, float y, float width, float height)
{
	GLuint uiImageType = findTextureName(Name);
	SEASON3B::RenderImageF(uiImageType, x, y, width, height);
}
void RenderImageF2(std::string Name, float x, float y, float width, float height, float su, float sv, float uw, float vh)
{
	GLuint uiImageType = findTextureName(Name);
	SEASON3B::RenderImageF(uiImageType, x, y, width, height, su, sv, uw, vh);
}

void RenderImageF3(std::string Name, float x, float y, float width, float height, DWORD color, float su, float sv, float uw, float vh)
{
	GLuint uiImageType = findTextureName(Name);
	SEASON3B::RenderImageF(uiImageType, x, y, width, height, color, su, sv, uw, vh);
}

bool GetSkillListUp()
{
	return g_pSkillList->IsSkillListUp();
}

CGMLuaMain::CGMLuaMain()
{
	lua.open_libraries(sol::lib::base);
}

CGMLuaMain::~CGMLuaMain()
{
}

void CGMLuaMain::Open()
{
	FILE* fp = fopen(".\\Data\\Interface\\HUD_TEST\\main.GLIB", "rb");

	if (fp == NULL)
	{
		return;
	}

	fseek(fp, 0, SEEK_END);
	int DataBytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	std::vector<BYTE> Data(DataBytes, 0);
	fread(Data.data(), 1, DataBytes, fp);
	fclose(fp);

	this->runtime_link(Data);
}

bool CGMLuaMain::runtime_link(std::vector<BYTE> buff)
{
	try {
		std::string scriptContent = std::string(buff.begin(), buff.end());

		this->funtions_hook();

		lua.script(scriptContent);
		return true;
	}
	catch (const sol::error& e) {
		//std::cerr << "Error loading script from buffer: " << e.what() << std::endl;
	}
	return false;
}

bool CGMLuaMain::funtions_hook()
{
	lua.set_function("pos_top", &pos_top);
	lua.set_function("pos_left", &pos_left);
	lua.set_function("pos_center", &pos_center);
	lua.set_function("pos_right", &pos_right);
	lua.set_function("pos_botton", &pos_botton);
	lua.set_function("RenderImage1", &RenderImageF1);
	lua.set_function("RenderImage2", &RenderImageF2);
	lua.set_function("RenderImage3", &RenderImageF3);
	lua.set_function("GetSkillListUp", &GetSkillListUp);
	lua.set_function("findTextureName", &findTextureName);
	lua.set_function("render_mask", &SEASON3B::runtime_render_mask);
	lua.set_function("number_mask", &SEASON3B::runtime_number_mask);
	lua.set_function("RenderNumber", &SEASON3B::RenderNumber);

	lua.new_usertype<CHARACTER_ATTRIBUTE>("CHARACTER_ATTRIBUTE",
		sol::constructors<CHARACTER_ATTRIBUTE()>(),
		"Life", &CHARACTER_ATTRIBUTE::Life,
		"Mana", &CHARACTER_ATTRIBUTE::Mana,
		"Shield", &CHARACTER_ATTRIBUTE::Shield,
		"SkillMana", &CHARACTER_ATTRIBUTE::SkillMana
		);

	lua.new_usertype<CHARACTER_MACHINE>("CHARACTER_MACHINE",
		sol::constructors<CHARACTER_MACHINE()>(),
		"Character", &CHARACTER_MACHINE::Character,
		"GetRealLife", &CHARACTER_MACHINE::GetRealLife,
		"GetRealMana", &CHARACTER_MACHINE::GetRealMana,
		"GetRealShield", &CHARACTER_MACHINE::GetRealShield,
		"GetRealSkillMP", &CHARACTER_MACHINE::GetRealSkillMP
		);

	lua.new_usertype<SEASON3B::CNewUIMainFrameWindow>("CNewUIMainFrameWindow",
		sol::constructors<SEASON3B::CNewUIMainFrameWindow()>(),
		"SetBtnPosition", &SEASON3B::CNewUIMainFrameWindow::SetBtnPosition,
		"SetHotPosition", &SEASON3B::CNewUIMainFrameWindow::SetHotPosition,
		"SetSkillPosition", &SEASON3B::CNewUIMainFrameWindow::SetSkillPosition,
		"RenderExperience", &SEASON3B::CNewUIMainFrameWindow::RenderExperience
		);

	this->pass_to_lua("NewMainFrameWindow", g_pMainFrame);

	return false;
}

void CGMLuaMain::Render()
{
	this->pass_to_lua("CharacterMachine", CharacterMachine);
	this->pass_to_lua("CharacterAttribute", CharacterAttribute);

	sol::function runtime_function_unique = lua["Render"];

	if (runtime_function_unique.valid())
	{
		runtime_function_unique();
	}
}

void CGMLuaMain::SetPos()
{
	//this->pass_to_lua("NewMainFrameWindow", g_pMainFrame);

	sol::function runtime_function_unique = lua["Position"];

	if (runtime_function_unique.valid())
	{
		runtime_function_unique();
	}
}
#endif // test_main_dev
