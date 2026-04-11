// NewUIButton.h: interface for the CNewUIButton class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWUIBUTTON_H__7DC4490D_D859_4159_9EE5_FBC4ECDE209A__INCLUDED_)
#define AFX_NEWUIBUTTON_H__7DC4490D_D859_4159_9EE5_FBC4ECDE209A__INCLUDED_

#pragma once

namespace SEASON3B
{
	enum BUTTON_STATE
	{
		BUTTON_STATE_NONE = -1,
		BUTTON_STATE_UP = 0,
		BUTTON_STATE_DOWN,
		BUTTON_STATE_OVER,
	};

	enum RADIOGROUPEVENT
	{
		RADIOGROUPEVENT_NONE = -1,
	};

	struct FPOINT
	{
		float x;
		float y;
		FPOINT() :x(0), y(0) {};
		FPOINT(float ix, float iy) :x(ix), y(iy) {};

		operator POINT() const {
			POINT p;
			p.x = static_cast<int>(x);
			p.y = static_cast<int>(y);
			return p;
		}
	};
	struct ButtonInfo
	{
		int s_ImgIndex;
		int s_BTstate;
		unsigned int s_imgColor;
		float su;
		float sv;
		float uw;
		float vh;

		ButtonInfo() : s_ImgIndex(0), s_BTstate(0), s_imgColor(0xffffffff),
			su(0.f), sv(0.f), uw(0.f), vh(0.f)
		{
		}
	};

	typedef std::map<int, ButtonInfo>  ButtonStateMap;

	class CNewUIBaseButton
	{
	public:
		CNewUIBaseButton();
		virtual ~CNewUIBaseButton();

	public:
		void SetPos(const FPOINT& pos);
		void SetSize(const FPOINT& size);
		void SetPos(float x, float y);
		void SetSize(float sx, float sy);

	public:
		const FPOINT& GetPos();
		const FPOINT& GetSize();
		const BUTTON_STATE GetBTState();

	public:
		void Lock();
		void UnLock();
		bool IsLock();

	public:
		bool RadioProcess();
		bool Process();

	protected:
		FPOINT					m_Pos;
		FPOINT					m_Size;
		BUTTON_STATE			m_EventState;
		bool					m_Lock;
	};

	inline void CNewUIBaseButton::SetPos(const FPOINT& pos)
	{
		m_Pos = pos;
	}

	inline void CNewUIBaseButton::SetSize(const FPOINT& size)
	{
		m_Size = size;
	}

	inline const FPOINT& CNewUIBaseButton::GetPos()
	{
		return m_Pos;
	}

	inline const FPOINT& CNewUIBaseButton::GetSize()
	{
		return m_Size;
	}

	inline const BUTTON_STATE CNewUIBaseButton::GetBTState()
	{
		return m_EventState;
	}

#ifndef KJH_MOD_RADIOBTN_MOUSE_OVER_IMAGE			// #ifndef
	inline void CNewUIBaseButton::Lock()
	{
		m_Lock = true;
	}

	inline void CNewUIBaseButton::UnLock()
	{
		m_Lock = false;
	}
#endif // KJH_MOD_RADIOBTN_MOUSE_OVER_IMAGE

	inline bool CNewUIBaseButton::IsLock()
	{
		return m_Lock;
	}

	class CNewUIButton : public CNewUIBaseButton
	{
	public:
		CNewUIButton();
		virtual ~CNewUIButton();
		void ChangeButtonImgState(bool imgregister, int imgindex, bool overflg = false, bool isimgwidth = false, bool bClickEffect = false, float su = 0.f, float sv = 0.f, float uw = 0.f, float vh = 0.f);
		void ChangeButtonInfo(float x, float y, float sx, float sy);
	private:
		void Initialize();
		void Destroy();
	public:
		void RegisterButtonState(BUTTON_STATE eventstate, int imgindex, int btstate, float su = 0.f, float sv = 0.f, float uw = 0.f, float vh = 0.f);
		void UnRegisterButtonState();
		void ChangeImgColor(BUTTON_STATE eventstate, unsigned int color);
		void ChangeImgDrawCoord(BUTTON_STATE eventstate, float su, float sv, float uw, float vh);
		void ChangeText(unicode::t_string btname);
		void SetFont(HFONT hFont);
		void ChangeButtonState(BUTTON_STATE eventstate, int iButtonState);
		void MoveTextPos(int iX, int iY);
		void MoveTextTipPos(int iX, int iY);
		void ChangeTextBackColor(const DWORD bcolor);
		void ChangeTextColor(const DWORD color);
		void ChangeToolTipText(unicode::t_string tooltiptext, bool istoppos = false);
		void ChangeToolTipTextColor(const DWORD color);
		void SetToolTipFont(HFONT hFont);
		void ChangeImgWidth(bool isimgwidth);
		void ChangeImgIndex(int imgindex, int curimgstate = 0, float su = 0.0, float sv = 0.0, float uw = 0.0, float vh = 0.0);
		void ChangeAlpha(unsigned char fAlpha, bool isfontalph = true);
		void ChangeAlpha(float fAlpha, bool isfontalph = true);
		bool UpdateMouseEvent();
		void updatefont(int hfont);
		bool Render(bool RendOption = false);
		void CurSize(WORD uw, WORD uv);
		void ChangeFrame(BUTTON_STATE btn_state = BUTTON_STATE_NONE);
	private:
		ButtonStateMap		m_ButtonInfo;
		unicode::t_string	m_Name;
		unicode::t_string	m_TooltipText;
		int 				m_hTextFont;
		int 				m_hToolTipFont;
		DWORD				m_NameColor;
		DWORD				m_NameBackColor;
		DWORD				m_TooltipTextColor;
		int					m_CurImgIndex;
		int					m_CurImgState;
		WORD				m_ImgWidth;
		WORD				m_ImgHeight;
		unsigned int		m_CurImgColor;
		bool				m_IsTopPos;
		bool				m_IsImgWidth;
		unsigned char		m_fAlpha;
		bool				m_bClickEffect;
		int					m_iMoveTextPosX;
		int					m_iMoveTextPosY;
		int					m_iMoveTextTipPosX;
		int					m_iMoveTextTipPosY;
		ButtonInfo*			m_BtnInfo;
	};

	class CNewUIRadioButton : public CNewUIBaseButton
	{
	public:
		CNewUIRadioButton();
		virtual ~CNewUIRadioButton();

	public:
		void ChangeRadioButtonImgState(int imgindex, bool isDown = false, bool bClickEffect = false);
		void ChangeRadioButtonInfo(int x, int y, int sx, int sy);
		void ChangeFrame(BUTTON_STATE eventstate);
	public:
		void ChangeImgColor(BUTTON_STATE eventstate, unsigned int color);
		void ChangeText(unicode::t_string btname);
		void ChangeTextBackColor(const DWORD bcolor);
		void ChangeTextColor(const DWORD color);
		void ChangeButtonState(BUTTON_STATE eventstate, int iButtonState);
		void ChangeButtonState(int iImgIndex, BUTTON_STATE eventstate, int iButtonState);
		void SetFont(HFONT hFont);
	public:
		void RegisterButtonState(BUTTON_STATE eventstate, int imgindex, int btstate);
		void UnRegisterButtonState();
	public:
		bool UpdateMouseEvent(bool isGroupevent = false);
	public:
		void updatefont(int hfont);
		bool Render();
	private:
		void ChangeImgIndex(int imgindex, int curimgstate = 0);
		void ChangeFrame();
		void Initialize();
		void Destroy();
	private:
		ButtonStateMap			m_RadioButtonInfo;
		unicode::t_string		m_Name;
		DWORD					m_NameColor;
		DWORD					m_NameBackColor;
		DWORD					m_CurImgIndex;
		DWORD					m_CurImgState;
		DWORD					m_ImgWidth;
		DWORD					m_ImgHeight;
		DWORD					m_CurImgColor;
		int						m_hTextFont;
		bool					m_bClickEffect;
#ifdef KJH_MOD_RADIOBTN_MOUSE_OVER_IMAGE
		bool					m_bLockImage;
#endif // KJH_MOD_RADIOBTN_MOUSE_OVER_IMAGE
	};

	inline void CNewUIRadioButton::ChangeText(unicode::t_string btname)
	{
		m_Name = btname;
	}

	inline void CNewUIRadioButton::ChangeTextBackColor(const DWORD bcolor)
	{
		m_NameBackColor = bcolor;
	}

	inline void CNewUIRadioButton::ChangeTextColor(const DWORD color)
	{
		m_NameColor = color;
	}

	inline void CNewUIRadioButton::SetFont(HFONT hFont)
	{
		if (hFont == g_hFont)
		{
			m_hTextFont = 1;
		}
		else if (hFont == g_hFontBold)
		{
			m_hTextFont = 2;
		}
		else if (hFont == g_hFontBold)
		{
			m_hTextFont = 3;
		}
		else
		{
			m_hTextFont = 0;
		}
	}

	class CNewUIRadioGroupButton
	{
	public:
		CNewUIRadioGroupButton();
		virtual ~CNewUIRadioGroupButton();

	public:
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		void CreateRadioGroup(int radiocount, int imgindex, bool bClickEffect = false);
		void ChangeRadioButtonInfo(bool iswidth, int x, int y, int sx, int sy, int iDistance = 1);
		void MoveTextPos(int iX, int iY);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
		void CreateRadioGroup(int radiocount, int imgindex);
		void ChangeRadioButtonInfo(bool iswidth, int x, int y, int sx, int sy);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
		void ChangeRadioText(std::list<unicode::t_string>& textlist);
		void ChangeFrame(int buttonIndex);
		void LockButtonindex(int buttonIndex);
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		void SetFont(HFONT hFont, int iButtonIndex);
		void SetFont(HFONT hFont);
		void ChangeButtonState(BUTTON_STATE eventstate, int iButtonState);
		void ChangeButtonState(int iBtnIndex, int iImgIndex,
			BUTTON_STATE eventstate, int iButtonState);
		POINT GetPos(int iButtonIndex);
#else // KJH_ADD_INGAMESHOP_UI_SYSTEM
		void SetFontIndex(int buttonIndex, HFONT hFont);
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM

	public:
		void RegisterRadioButton(CNewUIRadioButton* button);
		void UnRegisterRadioButton();

	public:
		const int GetCurButtonIndex();

	public:
		int UpdateMouseEvent();

	public:
		bool Render();

	private:
		void SetCurButtonIndex(int index);
		void Initialize();
		void Destroy();

	private:
		typedef std::list<CNewUIRadioButton*>      RadioButtonList;

	private:
		RadioButtonList				m_RadioList;
		DWORD						m_CurButtonIndex;
#ifdef KJH_ADD_INGAMESHOP_UI_SYSTEM
		int							m_iButtonDistance;			// 버튼과 버튼사이의 간격
#endif // KJH_ADD_INGAMESHOP_UI_SYSTEM
	};

	inline void CNewUIRadioGroupButton::SetCurButtonIndex(int index)
	{
		m_CurButtonIndex = index;
	}

	inline const int CNewUIRadioGroupButton::GetCurButtonIndex()
	{
		return m_CurButtonIndex;
	}
};

#endif // !defined(AFX_NEWUIBUTTON_H__7DC4490D_D859_4159_9EE5_FBC4ECDE209A__INCLUDED_)





















