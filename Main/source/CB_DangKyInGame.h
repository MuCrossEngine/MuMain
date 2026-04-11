#pragma once
#include "Protocol.h"
class CUITextInputBox;

class CB_DangKyInGame
{
	enum TYPE_INPUT_DKTK
	{
		Account,
		Pass,
		Snonumber,
		Phone,
		eMaxINPUT,
	};

	struct PMSG_REGISTER_MAIN_SEND
	{
		PSBMSG_HEAD header; // C1:D2:02
		BYTE TypeSend;
		char account[11];
		char password[21];
		char numcode[8];
		char sodienthoai[14];
	};
	enum eRecvKetQua
	{
		eDangKyThanhCong = 1,
		eTaiKhoanDaTonTai = 2,
		eDuLieuNhapKhongDung = 3,
		eDatLaiMatKhauThanhCong = 11,
		eThongTinBaoMatKhongChinhXac = 12,
	};
public:
	CB_DangKyInGame();
	~CB_DangKyInGame();
	bool RenderWindow(int X, int Y);
	void OpenOnOff();
	bool RequsetDKTK();
	void RecvKQRegInGame(XULY_CGPACKET* lpMsg);
	void Clear();
	static CB_DangKyInGame* Instance();
protected:
	CUITextInputBox* CInputData[TYPE_INPUT_DKTK::eMaxINPUT];
	CUITextInputBox* CInputCaptCha;
	DWORD TimeSendRegTK;
	bool OpenDKTK;
};

#define gCB_DangKyInGame			(CB_DangKyInGame::Instance())