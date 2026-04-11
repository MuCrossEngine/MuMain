//////////////////////////////////////////////////////////////////////
// NewUICommon.h: interface for the CNewUICommon class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWUICOMMON_H__0668BCBC_7537_454B_82FD_9D6BBBBDBA84__INCLUDED_)
#define AFX_NEWUICOMMON_H__0668BCBC_7537_454B_82FD_9D6BBBBDBA84__INCLUDED_

#pragma once
#include "NewUIScrollBar.h"
#include "NewUIMessageBox.h"

namespace SEASON3B
{
	class CNewUIButton;

	bool CreateOkMessageBox(const unicode::t_string& strMsg, DWORD dwColor = 0xffffffff, float fPriority = 3.f);
	bool CreateOkCancelMessageBox(const unicode::t_string& strMsg, DWORD dwColor = 0xffffffff, float fPriority = 3.f, EVENT_CALLBACK pFunc = NULL);

	int IsPurchaseShop(); 
	#define g_IsPurchaseShop SEASON3B::IsPurchaseShop()
	int TextDraw(HFONT font, int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...);

	bool CheckMouseIn(float x, float y, float width, float height, bool bFixed = true);
	void RenderImageF(GLuint Image, float x, float y, float width, float height);
	void RenderImageF(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, float uw, float vh);
	void RenderImageF(GLuint uiImageType, float x, float y, float width, float height, DWORD color, float su, float sv, float uw, float vh);

	void RenderImage2F(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, float uw, float vh);

	void RenderImage(GLuint uiImageType, float x, float y);
	void RenderImages(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, float uw, float vh);
	void RenderFrameAnimation(GLuint Image, float x, float y, float width, float height, float u, float v, float uw, float vh, float Time, int Numcol, int nFrame, float uFrame = 0.0, float vFrame = 0.0);
	void RenderFrameAnimation2(GLuint Image, float x, float y, float width, float height, float Rotate, float uw, float vh, float Time, int Numcol, int nFrame, bool fScale = true);
	
	void RenderImage(GLuint uiImageType, float x, float y, float width, float height);
	void RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv);
	void RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv, DWORD color);
	void RenderImage(GLuint uiImageType, float x, float y, float width, float height, float su, float sv,float uw, float vh, DWORD color = RGBA(255,255,255,255));
	
	float RenderNumber(float x, float y, int iNum, float fScale = 1.0f);

	void begin3D();
	void endrender3D();
	void RenderLocalItem3D(float sx, float sy, float Width, float Height, int Type, int Level = 0, int Option1 = 0, int ExtOption = 0, bool PickUp = false);
	void Render2Item3D(float sx, float sy, float Width, float Height, int Type, int Level = 0, int Option1 = 0, int ExtOption = 0, bool PickUp = false);
	const char* FontHTML(const char* text, BYTE Style, bool bold, bool addLineBreak);
	const char* FontHTML(const char* text, DWORD color, BYTE Size, bool bold, bool addLineBreak, int aling_on = 0);

	void RenderlookFetch(int Image, float x, float y, bool Enable, bool bRight = false);
	bool CheckMouseFetch(float x, float y, bool Enable, bool bRight = false);

	void RenderNavigationControls(float RenderFrameX, float RenderFrameY, float RenderSizeX, int aling_on = 0);
	template <typename T>
	bool UpdateNavigationControls(float RenderFrameX, float RenderFrameY, float RenderSizeX, T& data, T data_min = 0, T data_max = 0, T data_steep = 0);

	bool IsNone(int iVirtKey);
	bool IsRelease(int iVirtKey);
	bool IsPress(int iVirtKey);
	bool IsRepeat(int iVirtKey);
	
	class CNewKeyInput
	{
		struct INPUTSTATEINFO 
		{
			BYTE byKeyState;
		} m_pInputInfo[256];

#ifndef ASG_FIX_ACTIVATE_APP_INPUT
		void Init();
#endif

	public:
		enum KEY_STATE 
		{ 
			KEY_NONE=0,
			KEY_RELEASE,
			KEY_PRESS,
			KEY_REPEAT
		};
		~CNewKeyInput();
		
		static CNewKeyInput* GetInstance();
#ifdef ASG_FIX_ACTIVATE_APP_INPUT
		void Init();
#endif	
		void ScanAsyncKeyState();

		bool IsNone(int iVirtKey);
		bool IsRelease(int iVirtKey);
		bool IsPress(int iVirtKey); 
		bool IsRepeat(int iVirtKey);
		void SetKeyState(int iVirtKey, KEY_STATE KeyState);
	protected:
		CNewKeyInput();
	};



	class stSkillList
	{
	public:
		stSkillList() {
			iIndex = -1;
			iX = 0; iY = 0;
			Type = 0;
		};
		virtual~stSkillList() {};
	public:
		int iIndex;
		int iX;
		int iY;
		int Type;
	};

	class CCheckBox
	{
	public:
		CCheckBox() {
			Initialize();
		}
		virtual~CCheckBox() {
			Destroy();
		}
	private:
		HFONT m_hTextFont;
		int m_ImgIndex;
		float su;
		float sv;
		POINT m_Pos;
		SIZE m_Size;
		unicode::t_string m_Name;
		bool m_bEnable;
	public:
		void Initialize();
		void Destroy();
		void RegisterImgIndex(int imgindex);
		void ChangeCheckBoxInfo(int x, int y, int sx, int sy, float su = 0.0, float sv = 0.0);
		void ChangeCheckBoxText(unicode::t_string btname);;
		bool Create(int imgindex, int x, int y, int sx, int sy, unicode::t_string btname);
		bool Render();
		bool UpdateMouseEvent();
		const POINT& GetPos();
		void SetPos(int x, int y);
		void SetSize(int width, int height);
		bool IsSelected() {
			return m_bEnable;
		};
		void SetSelected(bool enable) {
			m_bEnable = enable != false;
		}
	};

	class stMacroUIImage
	{
	public:
		stMacroUIImage();
		virtual~stMacroUIImage();
	public:
		POINT m_Pos;
		float output_width;
		float output_height;
		int m_ImgIndex;
	public:
		const POINT& GetPos();
		void SetPos(int x, int y);
		void Register(int imgindex, int x, int y, int width, int height);
		void Render() {
			RenderImage(m_ImgIndex, m_Pos.x, m_Pos.y, output_width, output_height);
		};
	};

	class stMacroUIText
	{
	public:
		stMacroUIText();
		virtual~stMacroUIText();
	public:
		POINT m_Pos;
		unicode::t_string m_Name;
	public:
		const POINT& GetPos();
		void SetPos(int x, int y);
		void Register(int x, int y, const char* pzText);
		void Render();
	};

	class CSlideBar
	{
	public:
		CSlideBar();
		virtual~CSlideBar();
	private:
		POINT m_Pos;
		__int16 m_iValue;
		__int16 m_MaxLength;
		__int16 m_MinLength;
		int m_Imgindex;
		int m_ImgBack;
		__int16 m_Width;
		__int16 m_Height;
	public:
		const POINT& GetPos();
		void SetPos(int x, int y);
		void Create(int imgback, int imgindex, int x, int y, __int16 sx, __int16 sy, __int16 iMaxLength, __int16 start);
		bool MouseUpdate();
		void Render();
		int GetSlideLevel();
		void SetSlideLevel(__int16 Value);
	};

	class COptionButtonGroup
	{
	public:
		COptionButtonGroup();
		virtual~COptionButtonGroup();
	private:
		std::vector<CCheckBox*> m_box;
		int m_imgindex;
		int m_index;
		POINT m_Pos;
		int m_w;
		int m_h;
	public:
		const POINT& GetPos();
		void SetPos(int x, int y);
		void Create(int imgindex, int x, int y, int w, int h, unsigned char count, float su = 0.0, float sv = 0.0);
		void SetOptionText(unsigned char index, int offset_x, int offset_y, const char* text);
		bool UpdateMouseEvent();
		bool Render();
		void SetIndex(int index);
		int GetIndex();
	};

	struct ItemComboBox {
		int id;
		std::string text;

		// Constructor de conveniencia
		ItemComboBox(int id, const std::string& text) : id(id), text(text) {}
	};

	
#ifdef EFFECT_MNG_HANDLE
	class CInputComboBox
	{
		enum IMAGE_LIST
		{
			IMAGE_BTN_UP = BITMAP_CURSEDTEMPLE_BEGIN + 14,
			IMAGE_BTN_DOWN,
		};
	private:
		CNewUIScrollBar m_ScrollBar;
		bool isVisible;
		size_t m_iViewCount;
		int selectedIndex;
		int posX, posY, width, height;
		std::vector<ItemComboBox> objects;
	public:
		CInputComboBox();
		virtual~CInputComboBox();
		void setLocation(int x, int y);
		void setBounds(int x, int y, int sx, int sy);
		void addItem(const ItemComboBox& object);
		void SelectedIndex(int index);
		void RemoveAll();
		int getSelectedIndex() const;
		const char* getSelectedText();
		const char* getIndexText(int index);
		int getSelectedId();


		void Render();
		bool UpdateMouse();
		bool Update();
	private:
		void RenderFrameWork();
	};
#endif // EFFECT_MNG_HANDLE

	void runtime_number_mask(int groupid, float RenderFrameX, float RenderFrameY, int numerador, int size);
	void runtime_render_mask(int img, float porcento, float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY, float su, float sv, float uw, float vh, int type);

	void LoadPos(CNewUIButton* eBtn, int index, int RenderPosX, int RenderPosY);
	void Loadbutton(CNewUIButton* eBtn, int index, int RenderPosX, int RenderPosY, int RenderSizeX, int RenderSizeY, int imgindex, bool overflg = false, bool isimgwidth = false, bool bClickEffect = false, float su = 0.f, float sv = 0.f, float uw = 0.f, float vh = 0.f);

	void window_nomal(float RenderX, float RenderY, int type = 0, float division = 0.0);
	void window_emergent(CNewUIMessageBoxBase* pMsgBox, int MiddleCount, int type = -1);
	void window_backmsg(float RenderFrameX, float RenderFrameY, float RenderSizeX, float RenderSizeY);


}


template<typename T>
inline bool SEASON3B::UpdateNavigationControls(float RenderFrameX, float RenderFrameY, float RenderSizeX, T& data, T data_min, T data_max, T data_steep)
{
	bool updated = false;
	if (CheckMouseIn(RenderFrameX, RenderFrameY, 17, 18))
	{
		//-- add code min
		if(IsRelease(VK_LBUTTON))
			data = max(data - data_steep, data_min); // Garantiza que no baje de data_min
		updated = true;
	}

	RenderFrameX += RenderSizeX - 17.f;
	if (CheckMouseIn(RenderFrameX, RenderFrameY, 17, 18))
	{
		//-- add code max
		if (IsRelease(VK_LBUTTON))
			data = min(data + data_steep, data_max); // Garantiza que no supere data_max
		updated = true;
	}
	return updated;
}

#define g_pNewKeyInput	SEASON3B::CNewKeyInput::GetInstance()

#endif // !defined(AFX_NEWUICOMMON_H__0668BCBC_7537_454B_82FD_9D6BBBBDBA84__INCLUDED_)
