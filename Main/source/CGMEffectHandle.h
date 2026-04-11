#pragma once
#if !defined(AFX_NEWUIEFFECTHANDLE_H__1469FA1D_7C15_4AFE_AD6E_59C303E72BC0__INCLUDED_)
#define AFX_NEWUIEFFECTHANDLE_H__1469FA1D_7C15_4AFE_AD6E_59C303E72BC0__INCLUDED_
#pragma once

#include "NewUIManager.h"
#include "NewUIMyInventory.h"

#ifdef EFFECT_MNG_HANDLE


namespace SEASON3B
{
	typedef struct
	{
		int itemindex;
		float Color[3];
		int GroupId; //-- meshId
		int RenderFlag;
		int RenderType; //-- EffectType
		float TimeEffectType;
		int TextureID;
		float TimeTextureID;
		int NoGlow;
	}EnchantEffect;

	typedef struct
	{
		std::vector<EnchantEffect> buffer;
	}ConEnchant;

	typedef std::map<int, ConEnchant> type_map_Enchant;


	class ComboBoxGird
	{
	public:
		ComboBoxGird() {
			selectedId = 0;
			selectedIndex = 0;
		};

		void Render(const char* nameId, const char* error);
		void push_back(int id, std::string name);
		void clear() {
			selectedId = 0;
			selectedIndex = 0;
			comboBoxItems.clear();
		}
		bool empty() {
			return comboBoxItems.empty();
		}
		int getSelectedIndex() {
			return selectedIndex;
		}
	private:
		int selectedId;
		int selectedIndex;
		std::vector<std::pair<int, std::string>> comboBoxItems;
	};

	class GirdListBox
	{
	public:
		GirdListBox() {
			clear();
		}
		void clear() {
			selectedIndex = -1;
			listItems.clear();
		}
		bool empty() {
			return listItems.empty();
		}
		int getSelectedIndex() {
			return selectedIndex;
		}
		void Render(const char* nameId);
		void push_back(std::string name);
	private:
		int selectedIndex;
		std::vector<std::string> listItems;

	};

	class CGFxEffectHandle : public CNewUIObj
	{
	private:
		CNewUIManager*	m_pNewUIMng;
		POINT			m_Pos;
		int				selectedItem;
		int				selectedGroup;
		BOOL			m_bEditEnchant;
		ComboBoxGird comboBoxItems;
		ComboBoxGird comboBoxGroup;
		//GirdListBox EditEnchantList;
		EnchantEffect	m_bSettings;
		type_map_Enchant mapeEnchant;
		//std::vector<EnchantEffect> EditEnchant;
	public:
		CGFxEffectHandle();
		virtual ~CGFxEffectHandle();

		bool Create(CNewUIManager* pNewUIMng, int x, int y);
		void Release();

		void SetPos(int x, int y);

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();

		float GetLayerDepth();	//. 10.5f
		float GetKeyEventOrder();	// 10.f;

		void OpenningProcess();
		void ClosingProcess();

		const std::vector<EnchantEffect>& GetEnchantEffects(int itemindex) const;

		void AddEnchantEffect(int itemindex, const EnchantEffect& effect);
		bool runtime_colector_Enchant(std::string filename);
		bool runtime_render_Enchant(int itemindex, BMD* pModel, OBJECT* pObject);
		bool runtime_render_NoGlow(int itemindex, BMD* pModel, OBJECT* pObject, int RenderType, float Alpha);
	private:
		void LoadImages();
		void UnloadImages();

		void SetButtonInfo();
		void RenderFrame();
		void RenderContents();
		void RenderButtons();
		void runtime_export_settings(std::string filename);
		void reload_collection_item(int currentIndex);
		void runtime_interpret_setting(const EnchantEffect* Enchant, BMD* pModel, OBJECT* pObject);
	};

	inline const std::vector<EnchantEffect>& CGFxEffectHandle::GetEnchantEffects(int itemindex) const
	{
		static const std::vector<EnchantEffect> emptyVector;

		type_map_Enchant::const_iterator it = mapeEnchant.find(itemindex);
		if (it != mapeEnchant.end())
		{
			return it->second.buffer;
		}
		return emptyVector;
	}
}
#endif // EFFECT_MNG_HANDLE

#endif // !defined(AFX_NEWUIEFFECTHANDLE_H__1469FA1D_7C15_4AFE_AD6E_59C303E72BC0__INCLUDED_)
