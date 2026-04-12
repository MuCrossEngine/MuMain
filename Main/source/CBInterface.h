#pragma once
#include "Singleton.h"
#include "DSPlaySound.h"
//#include "Protocol.h"
#include "NewUIBase.h"
#include "NewUIManager.h"
#include "NewUIButton.h"
#include "UIControls.h"
#include "Defined_Global.h"

#define Color4f(r, b, g, o)			((o << 24) + (g << 16) + (b << 8) + r)
typedef void(*PUSHEVENT_CALLBACK_LPVOID)(LPVOID);
enum ColorMacro
{
	eTextShop = Color4f(0xFFu, 0xDCu, 0x96u, 0xFFu),
	eTextGuild = Color4f(0xFFu, 0xBDu, 0x19u, 0xFFu),
	eShinyGreen = Color4f(172, 255, 56, 255),
	eShinyGreen100 = Color4f(172, 255, 56, 100),
	eGold = Color4f(255, 189, 25, 255),
	eGold150 = Color4f(255, 189, 25, 150),
	eGold180 = Color4f(255, 189, 25, 180),
	eGold220 = Color4f(255, 189, 25, 220),
	eWhite = Color4f(255, 255, 255, 255),
	eWhite150 = Color4f(255, 255, 255, 150),
	eWhite160 = Color4f(255, 255, 255, 160),
	eWhite170 = Color4f(255, 255, 255, 170),
	eWhite180 = Color4f(255, 255, 255, 180),
	eWhite190 = Color4f(255, 255, 255, 200),
	eOrange = Color4f(255, 105, 25, 255),
	eGray100 = Color4f(211, 211, 211, 100),
	eGray150 = Color4f(50, 50, 50, 150),
	eGray190 = Color4f(50, 50, 50, 190),
	eBlowPink = Color4f(220, 20, 60, 255),
	eBlack = Color4f(0, 0, 0, 255),
	eRed = Color4f(225, 0, 0, 255),
	eRed2 = Color4f(225, 95, 45, 255),
	eExcellent = Color4f(0, 225, 139, 255),
	eAncient = Color4f(1, 223, 119, 255),
	eSocket = Color4f(153, 102, 204, 255),
	eBlue = Color4f(36, 242, 252, 255),
	eBlue250 = Color4f(36, 242, 252, 250),
	eYellow = Color4f(255, 255, 0, 255),
	eYellow170 = Color4f(255, 255, 0, 170),
	eBrown = Color4f(69, 39, 18, 255),
	eColorAdvance = Color4f(109, 25, 192, 255),
	eColorHealth = Color4f(191, 191, 146, 255),
};

enum ObjectID
{
	eFlag01 = 1,
	eFlag02,
	eTIME,
	eFlag04,
	eFlag05,
	eOpenWindow,
	//==Img


	//=WindowMenu
	eBeginWindowCustom, //bat dau cac object window
	eWindowMessageBox,
	eWindowEventTime,
#if(CB_DANGKYINGAME)
	eWindow_DangKyInGame,
#endif

	eEndWindowCustom, //ket thuc


	EndObjectCustom,
};

class CB_Interface
{
public:
	struct InterfaceObject
	{
		DWORD	ModelID;
		float	Width;
		float	Height;
		float	X;
		float	Y;
		float	MaxX;
		float	MaxY;
		DWORD	EventTick;
		bool	OnClick;
		bool	 OnShow;
		BYTE	Attribute;
		bool	ByClose;
		long	OpenedValue;
		BYTE	Speed;
		char	StatValue[20];
		int		Type;
		//
		bool	FirstLoad;
		float  curX;
		float	curY;
		bool	Hover;
		bool  AllowMove;
		void OpenClose(int Type = -1)
		{
			this->OnShow ^= 1;
			this->ByClose ^= 1;
			this->Hover ^= 1;
			if (Type != -1) this->Type = Type;
			if (!this->OnShow) this->Type = 0;

		};
		void Close()
		{
			this->OnShow = false;
			this->ByClose = false;
			this->Hover = false;
			this->Type = 0;
			this->AllowMove = true;
		};

		void Open()
		{
			this->OnShow = true;
			this->ByClose = false;
		};

		void Open(int Value, int Speed)
		{
			this->OnShow = true;
			this->OpenedValue = Value;
			this->Speed = Speed;
			this->ByClose = false;
			this->FirstLoad = true;
		};

		void Close(int Value, int Speed)
		{
			this->OnShow = false;
			this->OpenedValue = Value;
			this->Speed = Speed;
			this->ByClose = false;
			this->Type = 0;
		}

		void CloseAnimated(int Speed)
		{
			this->Speed = Speed;
			this->ByClose = true;
			this->Type = 0;
		}


	};

	enum ObjWindow // -> Season 6.3
	{
		FriendList = 1,
		MoveList = 2,
		Party = 3,
		Quest = 4,
		NPC_Devin = 5,
		Guild = 6,
		Trade = 7,
		Warehouse = 8,
		ChaosBox = 9,
		CommandWindow = 10,
		PetInfo = 11,
		Shop = 12,
		Inventory = 13,
		Store = 14,
		OtherStore = 15,
		Character = 16,
		DevilSquare = 19,
		BloodCastle = 20,
		CreateGuild = 21,
		GuardNPC = 22,
		SeniorNPC = 23,
		GuardNPC2 = 24,
		CastleGateSwitch = 25,
		CatapultNPC = 26,
		CrywolfGate = 29,
		IllusionTemple = 30,
		HeroList = 32,
		ChatWindow = 33,
		FastMenu = 34,
		Options = 35,
		Help = 36,
		FastDial = 39,
		SkillTree = 57,
		GoldenArcher1 = 58,
		GoldenArcher2 = 59,
		LuckyCoin1 = 60,
		LuckyCoin2 = 61,
		NPC_Duel = 62,
		NPC_Titus = 63,
		CashShop = 65,
		Lugard = 66,
		QuestList1 = 68,
		QuestList2 = 69,
		Jerint = 70,
		FullMap = 72,
		NPC_Dialog = 73,
		GensInfo = 74,
		NPC_Julia = 75,
		NPC_ChaosMix = 76,
		ExpandInventory = 77,
		ExpandWarehouse = 78,
		MuHelper = 79,
	};

	CB_Interface();
	virtual ~CB_Interface();
	void CloseAllWindowCustom();
	bool gDrawWindowCustom(float* StartX, float* StartY, float CuaSoW, float CuaSoH, int eNumWindow, LPCSTR Text, ...);
	int TextDraw(HFONT font, int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...);

	InterfaceObject Data[EndObjectCustom];
	void SetBlockCur(bool Delay);
	void	DrawBarForm(float PosX, float PosY, float Width, float Height, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
	bool BtnProcess();
	bool CheckWindow(int WindowID);
	bool DrawButton(float PosX, float PosY, float SizeButton, int FontSize, const char* ButtonText, float mSizeButtonW = 0.0f, bool IsEnter = false);
	virtual int DrawFormat(DWORD Color, int PosX, int PosY, int Width, int Align, LPCSTR Text, ...);
	virtual char* NumberFormat(int Vaule);
	bool		DrawButtonGUI(int IDGUID, float PosX, float PosY, float SizeW, float SizeH, int TypeButton = 0);
	DWORD BlockMouseWheel;
	virtual int DrawMessage(int Mode, LPCSTR Text, ...);
	void		DrawInfoBox(float PosX, float PosY, float Width, float Height, DWORD bkcolor = 0x0, int Arg6 = 0, int Arg7 = 0);
	bool RenderCheckBox(float PosX, float PosY, DWORD TextColor, bool Checkis, LPCSTR Text, ...);
	bool DrawSelectBar(int X, int Y, int W, int H, int* SelectNum, std::vector<std::string> Data);
	bool AutoHP;
	bool AutoCtrlPK;
	bool RenderInputBox(float PosX, float PosY, float W, float H, char* TextSet, CUITextInputBox*& Input, UIOPTIONS UiOption, int MaxChar = 50, bool isPass = 0);

	//==Mess Box
	std::string MsgBoxCaption;
	std::string MsgBoxText;
	int MsgBoxType;
	int MsgLine;
	PUSHEVENT_CALLBACK_LPVOID MsgBoxCallback;
	void DrawMessageBox();
	void OpenMessageBox(char* caption, char* Format, ...);
	void OpenMessageBoxOkCancel(PUSHEVENT_CALLBACK_LPVOID pCallbackFunc, char* caption, char* Format, ...);
	//====
	void		RenderCaptchaNumber(float PosX, float PosY, CUITextInputBox* a6, LPCSTR Text, ...);
	bool		check_Captcha(std::string& captcha, std::string& user_input);
	std::string generateCaptcha(int n);
	std::string vCaptcha;

	static CB_Interface* Instance();
private:
	void MoveWindows(int eNumWindow);
	bool BLockButtonMoveWindow;
	DWORD BLockMouseMoveClick;


	DWORD BButtonClickTime;
	bool BLockButtonHover;
	bool BLockMessageBox;
};
#define gInterface			(CB_Interface::Instance())
#define MAX_WIN_WIDTH			(gwinhandle->GetScreenX())
#define MAX_WIN_HEIGHT			(gwinhandle->GetScreenY())
