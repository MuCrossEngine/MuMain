
#ifndef _NEWCHATLONGWINDOW_H_
#define _NEWCHATLONGWINDOW_H_

#pragma once

#include "NewUIBase.h"
#include "ZzzTexture.h"

#pragma warning(disable : 4786)
#include <string>
#include <vector>

namespace SEASON3B 
{
	class CNewUIManager;

	enum MESSAGE_TYPE
	{
		TYPE_ALL_MESSAGE = 0,
		TYPE_CHAT_MESSAGE,
		TYPE_WHISPER_MESSAGE,
		TYPE_SYSTEM_MESSAGE,
		TYPE_ERROR_MESSAGE,
		TYPE_PARTY_MESSAGE,
		TYPE_GUILD_MESSAGE,
		TYPE_UNION_MESSAGE,
		TYPE_GM_MESSAGE,
		TYPE_GENS_MESSAGE,
		NUMBER_OF_TYPES,
		TYPE_UNKNOWN = 0xFFFFFFFF
	};

	extern void RemoveItem(ITEM* pItem);

	template <class T>
	class TMessageText
	{
		typedef std::basic_string<T, std::char_traits<T>, std::allocator<T> > type_string;
		type_string m_strID, m_strText;
		MESSAGE_TYPE m_MsgType;
		DWORD m_dwIndentSize;
#ifdef ITEM_CHAT_SELL_SYSTEM
		void* m_dwIndentItem;
#endif // ITEM_CHAT_SELL_SYSTEM

	public:
#ifdef ITEM_CHAT_SELL_SYSTEM
		TMessageText() : m_MsgType(TYPE_UNKNOWN), m_dwIndentSize(0), m_dwIndentItem(0) {}
		~TMessageText() { Release(); }

		bool Create(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, void* MemoryData = NULL)
		{
			if (MsgType >= NUMBER_OF_TYPES)
				return false;

			m_strID = strID;
			m_strText = strText;
			m_MsgType = MsgType;
			m_dwIndentItem = MemoryData;
			return true;
		}

		void Release()
		{
			m_strID.resize(0);
			m_strText.resize(0);
			m_MsgType = TYPE_UNKNOWN;
			RemoveItem(static_cast<ITEM*>(m_dwIndentItem));
		}

		ITEM* GetItemIndent()
		{
			return static_cast<ITEM*>(m_dwIndentItem);
		}
#else
		TMessageText() : m_MsgType(TYPE_UNKNOWN), m_dwIndentSize(0) {}
		~TMessageText() { Release(); }

		bool Create(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, void* MemoryData)
		{
			if (MsgType >= NUMBER_OF_TYPES)
				return false;

			m_strID = strID;
			m_strText = strText;
			m_MsgType = MsgType;
			return true;
		}

		void Release()
		{
			m_strID.resize(0);
			m_strText.resize(0);
			m_MsgType = TYPE_UNKNOWN;
		}
#endif // ITEM_CHAT_SELL_SYSTEM

		const type_string& GetID() const { return m_strID; }
		const type_string& GetText() const { return m_strText; }
		MESSAGE_TYPE GetType() const { return m_MsgType; }
		DWORD GetSerial() const { return m_dwIndentSize; }
	};

	typedef TMessageText<unicode::t_char> CMessageText;

	class CNewUIChatLogWindow: public CNewUIObj
	{
	public:
		enum IMAGE_LIST
		{
			IMAGE_SCROLL_TOP = BITMAP_INTERFACE_NEW_CHATLOGWND_BEGIN,	//. newui_scrollbar_up.tga
			IMAGE_SCROLL_MIDDLE,	//. newui_scrollbar_m.tga
			IMAGE_SCROLL_BOTTOM,	//. newui_scrollbar_down.tga
			IMAGE_SCROLLBAR_ON,		//. newui_scroll_on.tga
			IMAGE_SCROLLBAR_OFF,	//. newui_scroll_off.tga
			IMAGE_DRAG_BTN,			//. newui_scrollbar_stretch.tga
		};

	private:
		enum 
		{
			MAX_CHAT_BUFFER_SIZE = 60,
			MAX_NUMBER_OF_LINES = 200,
#if MAIN_UPDATE > 303
			WND_WIDTH = 281,
#else
			WND_WIDTH = 268,
#endif // MAIN_UPDATE == 303

			FONT_LEADING = 4,
			WND_TOP_BOTTOM_EDGE = 2,
			WND_LEFT_RIGHT_EDGE = 4,
			RESIZING_BTN_WIDTH = WND_WIDTH,
			RESIZING_BTN_HEIGHT = 10,
			SCROLL_BAR_WIDTH = 7,
			SCROLL_TOP_BOTTOM_PART_HEIGHT = 3,
			SCROLL_MIDDLE_PART_HEIGHT = 15,
			SCROLL_BTN_WIDTH = 15,
			SCROLL_BTN_HEIGHT = 30,
			CLIENT_WIDTH = WND_WIDTH - SCROLL_BAR_WIDTH - (WND_LEFT_RIGHT_EDGE*2),
		};
		enum EVENT_STATE
		{
			EVENT_NONE = 0,
			EVENT_CLIENT_WND_HOVER,
			EVENT_SCROLL_BTN_DOWN,
			EVENT_RESIZING_BTN_HOVER,
			EVENT_RESIZING_BTN_DOWN,
			EVENT_RESIZING_BTN_UP,
		};

		typedef std::basic_string<unicode::t_char, std::char_traits<unicode::t_char>,std::allocator<unicode::t_char> > type_string;
		typedef std::vector<CMessageText*>	type_vector_msgs;
		typedef std::vector<type_string>	type_vector_filters;

		CNewUIManager*		m_pNewUIMng;
		type_vector_msgs	m_vecAllMsgs;
		type_vector_msgs	m_VecChatMsgs;
		type_vector_msgs	m_vecWhisperMsgs;
		type_vector_msgs	m_vecPartyMsgs;
		type_vector_msgs	m_vecGuildMsgs;
		type_vector_msgs	m_vecUnionMsgs;
		type_vector_msgs	m_VecSystemMsgs;
		type_vector_msgs	m_vecErrorMsgs;
		type_vector_msgs	m_vecGMMsgs;
		type_vector_msgs	m_vecGensMsgs;
		bool				m_msgEnable[NUMBER_OF_TYPES];
		type_vector_filters	m_vecFilters;
		POINT				m_WndPos;
		POINT				m_ScrollBtnPos;
		SIZE				m_WndSize;
		int					m_nShowingLines;
		MESSAGE_TYPE		m_CurrentRenderMsgType;
		bool				m_bShowChatLog;
		int					m_iCurrentRenderEndLine;
		int					m_iGrapRelativePosY;
		float				m_fBackAlpha;
		EVENT_STATE			m_EventState;
		bool				m_bShowFrame;
		bool				m_bPointedMessage;
		int					m_iPointedMessageIndex;
		//-- exp
		POINT				m_Pos;
		SIZE				m_Size;
		WORD				m_MaxLines;
		bool				m_msgBackEnable[NUMBER_OF_TYPES];

		void Init();
		void LoadImages();
		void UnloadImages();
		bool RenderBackground();
		bool RenderMessages();
		bool RenderFrame();

		void RenderButton();
		bool UpdateButton();
		void sub_4193B1(int a1, int a2, float a3, float a4, float a5, float a6, GLfloat alpha, int a8);

		void SupportPosition();
		void RecoverBackPosition();
		void SetRelativeData(int x, int y, int numberline);
	public:
		CNewUIChatLogWindow();
		virtual ~CNewUIChatLogWindow();
		
		bool Create(CNewUIManager* pNewUIMng, int x, int y, int nShowingLines = 6);
		void Release();
		void SetPos(int x, int y);

#ifdef ITEM_CHAT_SELL_SYSTEM
		void AddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType = TYPE_ALL_MESSAGE, void* MemoryData = 0);
#else
		void AddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType = TYPE_ALL_MESSAGE);
#endif // ITEM_CHAT_SELL_SYSTEM
		void RemoveFrontLine(MESSAGE_TYPE MsgType);
		void Clear(MESSAGE_TYPE MsgType);
		void ClearAll(int type = -1);

		size_t GetNumberOfLines(MESSAGE_TYPE MsgType);
		MESSAGE_TYPE GetCurrentMsgType() const;

		void ChangeMessage(MESSAGE_TYPE MsgType);
		void SetMsgVisible(int MsgType, bool bShow);
		void SetAllMsgEnable();
		void SetAllMsgDisable();
		void ReCalc_BasicPos(int* RenderStart, float* RenderPosY);
		void ReCalc_RenderMessages(int* RenderStart, float* RenderPosY);

		int GetCurrentRenderEndLine() const;
		void Scrolling(int nRenderEndLine);
		
		void SetFilterText(const type_string& strFilterText);
		void ResetFilter();
		
		void SetSizeAuto();
		void SetNumberOfShowingLines(int nShowingLines, OUT LPSIZE lpBoxSize = NULL);
		size_t GetNumberOfShowingLines() const;
		void SetBackAlphaAuto();
		void SetBackAlpha(float fAlpha);
		float GetBackAlpha() const;
		void ShowFrame();
		void HideFrame();
		bool IsShowFrame();

		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		bool Update();
		bool Render();
		float GetLayerDepth();	//. 6.1f
		float GetKeyEventOrder();	//. 8.0f
		void EventOrderWindows(double WindowsX, double WindowsY);

		bool CheckChatRedundancy(const type_string& strText, int iSearchLine = 1);
		void UpdateWndSize();
		void UpdateScrollPos();
#ifdef ITEM_CHAT_SELL_SYSTEM
		ITEM* GetMsgsIndent();
#endif // ITEM_CHAT_SELL_SYSTEM
	protected:
		type_vector_msgs* GetMsgs(MESSAGE_TYPE MsgType);
#ifdef ITEM_CHAT_SELL_SYSTEM
		void ProcessAddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType, void* MemoryData);
#else
		void ProcessAddText(const type_string& strID, const type_string& strText, MESSAGE_TYPE MsgType, MESSAGE_TYPE ErrMsgType);
#endif // ITEM_CHAT_SELL_SYSTEM
		void SeparateText(IN const type_string& strID, IN const type_string& strText, OUT type_string& strText1, OUT type_string& strText2);
		bool CheckFilterText(const type_string& strTestText);
		void AddFilterWord(const type_string& strWord);

	};
}

#endif // _NEWCHATLONGWINDOW_H_