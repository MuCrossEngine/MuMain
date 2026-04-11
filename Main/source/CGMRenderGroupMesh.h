#pragma once
#ifdef EFFECT_KERNEL_VERTEX


#include "NewUIQuestProgress.h"

#define RENDER_MESH_OBJ_ITEM				0
#define RENDER_MESH_OBJ_MONSTER				1


typedef struct
{
	std::vector<RENDER_MESH_EFFECT> Group;

	size_t size() {
		return this->Group.size();
	}
}RENDER_GROUP_MESH_EFFECT;

class CGMRenderGroupMesh
{
	enum IMAGE_LIST
	{
		IMAGE_ND_BTN_L = SEASON3B::CNewUIQuestProgress::IMAGE_QP_BTN_L,
		IMAGE_ND_BTN_R = SEASON3B::CNewUIQuestProgress::IMAGE_QP_BTN_R,
	};
private:
	std::map<int, RENDER_GROUP_MESH_EFFECT> m_RenderItem;
	std::map<int, RENDER_GROUP_MESH_EFFECT> m_RenderMonster;
	std::map<int, RENDER_MESH_EFFECT_DEFAULT> m_DefaultRenderType;
public:
	CGMRenderGroupMesh();
	virtual~CGMRenderGroupMesh();
	static CGMRenderGroupMesh* Instance();
	void LoadData(int Type, RENDER_MESH_EFFECT* Info, size_t Size);
	void LoadData(RENDER_MESH_EFFECT_DEFAULT* Info, size_t Size);
	void push_back(int Index, RENDER_MESH_EFFECT info, int Type = RENDER_MESH_OBJ_ITEM);
	bool runtime_render_obj(int modelType, BMD* pModel, OBJECT* pObject, int Type);
	bool runtime_make_item(int modelType, BMD* pModel, OBJECT* pObject);
	bool runtime_make_effect(int modelType, BMD* pModel, OBJECT* pObject, float Alpha, int RenderType, int Texture);

	void runtime_make_render(RENDER_MESH_EFFECT* b, BMD* pModel, OBJECT* pObject);
	std::vector<int> runtime_make_renderflag(const std::vector<int>& values);
	void runtime_make_edit(int body);
	bool check_full_windows();
	void render();
	bool update();
	bool runtime_update_interface();
	void runtime_make_interface();
	void runtime_make_text(float RenderFrameX, float RenderFrameY);
	bool runtime_make_button(int Image, float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY, BYTE Type, bool checkaction = false);

	void runtime_make_file();
	template <typename T>
	void runtime_make_button_steep(float RenderFrameX, float RenderFrameY, T steep, T steepmin, T steepmax, T* value, bool neg = false, bool stb = false, bool go_back = false);
	//void runtime_make_button_steep(float RenderFrameX, float RenderFrameY, int steep, int steepmin, int steepmax, float* value);
private:
	int item_level;
	int renderflag;
	BOOL renderaction;
	std::vector<int> render_flag;
	RENDER_MESH_EFFECT edit_mesh;
	std::vector<RENDER_MESH_EFFECT> edit_item;



	RENDER_GROUP_MESH_EFFECT* FindItemRender(int ItemIndex);
	int FindDefaultRender(int ItemIndex);
};

#define GMRenderGroupMesh			(CGMRenderGroupMesh::Instance())

template<typename T>
inline void CGMRenderGroupMesh::runtime_make_button_steep(float RenderFrameX, float RenderFrameY, T steep, T steepmin, T steepmax, T* value, bool neg, bool make, bool go_back)
{
	if (value == NULL)
	{
		SEASON3B::RenderlookFetch(IMAGE_ND_BTN_R, RenderFrameX, RenderFrameY, true);

		SEASON3B::RenderlookFetch(IMAGE_ND_BTN_L, RenderFrameX, RenderFrameY, true, true);
	}
	else
	{
		static CheckerTime button_steep;
		if (SEASON3B::CheckMouseFetch(RenderFrameX, RenderFrameY, true, false) || SEASON3B::CheckMouseFetch(RenderFrameX, RenderFrameY, true, true))
		{
			if (SEASON3B::IsPress(VK_LBUTTON))
			{
				button_steep.intervalSet(0.15f);
			}

			if (SEASON3B::IsRelease(VK_LBUTTON))
			{
				button_steep.intervalSet(0.0);
			}
		}

		if (SEASON3B::CheckMouseFetch(RenderFrameX, RenderFrameY, true, false))
		{
			if (SEASON3B::IsRelease(VK_LBUTTON) || button_steep.hasElapsed())
			{
				if (*value < steepmax)
				{
					if (make && *value < steepmin)
						*value = steepmin;
					else
						*value += steep;
				}
				else if (go_back)
				{
					*value = steepmin;
				}
			}

		}
		if (SEASON3B::CheckMouseFetch(RenderFrameX, RenderFrameY, true, true))
		{
			if (SEASON3B::IsRelease(VK_LBUTTON) || button_steep.hasElapsed())
			{
				if (*value > steepmin)
					*value -= steep;
				else if (neg)
					*value = static_cast<T>(-1);
				else if (go_back)
					*value = steepmax;
			}
		}
	}
}
#endif // EFFECT_KERNEL_VERTEX
