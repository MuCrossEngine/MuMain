#pragma once
#include <sol/sol.hpp>

class BMD;
class OBJECT;
class CHARACTER_MACHINE;

class CGMLuaBase
{
private:
	sol::state lua;
public:
	CGMLuaBase();
	virtual~CGMLuaBase();
	bool runtime_link(std::vector<BYTE> buff);
	bool RenderLinkAction(PART_t* f, OBJECT* pObj);
	bool RenderPartObjectBody(BMD* pModel, OBJECT* pObj, float Alpha, int RenderType);
	bool RenderEffectObjectBody(BMD* pModel, OBJECT* pObj, float Alpha, int RenderType, int Texture);
private:
	void runtime_link_model();
	void runtime_link_object();
	void runtime_link_funtion();
	void runtime_link_character();

	template<typename T>
	void pass_to_lua(const char* uniqueId, T* uniqueObjet) {
		lua[uniqueId] = uniqueObjet;
	};
	template<typename T>
	void pass_to_lua_value(const char* uniqueId, T uniqueObjet) {
		lua[uniqueId] = uniqueObjet;
	};
	template<typename T>
	void pass_to_lua_table(const char* uniqueId, T* uniqueObjet, size_t lSize) {
		sol::table luaTable = lua.create_table(lSize);
		for (size_t i = 0; i < lSize; ++i) {
			luaTable[i + 1] = uniqueObjet[i];  // Lua tables are 1-based
		}
		lua[uniqueId] = luaTable;
	};
};

#ifdef test_main_dev
class CGMLuaMain
{
private:
	sol::state lua;
public:
	CGMLuaMain();
	virtual~CGMLuaMain();
	void Open();
	bool runtime_link(std::vector<BYTE> buff);
	bool funtions_hook();


	void Render();
	void SetPos();
public:
	template<typename T>
	void pass_to_lua(const char* uniqueId, T* uniqueObjet) {
		lua[uniqueId] = uniqueObjet;
	};
	template<typename T>
	void pass_to_lua_value(const char* uniqueId, T uniqueObjet) {
		lua[uniqueId] = uniqueObjet;
	};
	template<typename T>
	void pass_to_lua_table(const char* uniqueId, T* uniqueObjet, size_t lSize) {
		sol::table luaTable = lua.create_table(lSize);
		for (size_t i = 0; i < lSize; ++i) {
			luaTable[i + 1] = uniqueObjet[i];  // Lua tables are 1-based
		}
		lua[uniqueId] = luaTable;
	};
};
#endif // test_main_dev

extern int to_funtion__cast_int(float number);
extern int to_funtion__timeGetTime();
extern void to_funtion__glColor3fv(float* v);
extern void to_funtion__glColor4f(float r, float g, float b, float a);
extern void to_funtion__Vector(float a, float b, float c, float* v);
extern void to_funtion__VectorCopy(sol::table lua_table, float* b);
extern void to_funtion__RenderLevel(int GroupMesh, float Red, float Blue, float Green);
