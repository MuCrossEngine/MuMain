#include "StdAfx.h"
#include "CB_DangKyInGame.h"
#include "NewUISystem.h"
#include "CBInterface.h"
#include "CharacterManager.h"
#include "Util.h"
#include "Protocol.h"
#include "NewUIBase.h"
#include "ZzzInterface.h"
#include "UIMng.h"
using namespace SEASON3B;

CB_DangKyInGame* CB_DangKyInGame::Instance()
{
	static CB_DangKyInGame s_Instance;
	return &s_Instance;
}
CB_DangKyInGame::CB_DangKyInGame()
{

	this->Clear();
}


CB_DangKyInGame::~CB_DangKyInGame()
{
	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		SAFE_DELETE(CInputData[i]);
	}

	SAFE_DELETE(CInputCaptCha);
}

void CB_DangKyInGame::Clear()
{
	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		CInputData[i] = NULL;
	}
	CInputCaptCha = NULL;
	this->TimeSendRegTK = GetTickCount();
	this->OpenDKTK = false;
}

bool CheckChuoiKyTuDacBiet(const std::string& str)
{
	for (char ch : str)
	{
		if (!isdigit(ch) && !(ch >= 'a' && ch <= 'z') && !(ch >= 'A' && ch <= 'Z'))
		{
			return false;
		}
	}
	return true;
}

void CB_DangKyInGame::OpenOnOff()
{
	if (GetTickCount() - gInterface->Data[eWindow_DangKyInGame].EventTick > 300)
	{
		gInterface->Data[eWindow_DangKyInGame].EventTick = GetTickCount();
		if (gInterface->Data[eWindow_DangKyInGame].OnShow)
		{
			gInterface->Data[eWindow_DangKyInGame].OnShow = 0;
			return;
		}
		//==Show hoac send packet open
		gInterface->Data[eWindow_DangKyInGame].OnShow = 1;


	}
}

bool CB_DangKyInGame::RenderWindow(int X, int Y)
{
	if (!gInterface->Data[eWindow_DangKyInGame].OnShow || !gCB_DangKyInGame)
	{
		if (this->OpenDKTK)
		{
			this->Clear();
		}
		return 0;
	}

	this->OpenDKTK = true;
	float WindowW = 262;
	float WindowH = 250;

	static char* mTextStatus[] = { "®️ TÀI KHOẢN :", "®️ MẬT KHẨU :", "®️ 7 Số Bí Mật :", "®️ Số Điện Thoại :" , "Captcha :" };
	static int mInputType[] = { UIOPTION_NOLOCALIZEDCHARACTERS,
								UIOPTION_NOLOCALIZEDCHARACTERS,
								UIOPTION_NUMBERONLY,
								UIOPTION_NUMBERONLY };
	static int mMaxInput[] = { 10,
							   10,
							   7,
							   11 };


	float StartX = (MAX_WIN_WIDTH / 2) - (WindowW / 2);;
	float StartY = 150;
	gInterface->Data[eWindow_DangKyInGame].AllowMove = false;

	gInterface->gDrawWindowCustom(&StartX, &StartY, WindowW, WindowH, eWindow_DangKyInGame, "Đăng Ký Tài Khoản");

	TextDraw(g_hFontBold, StartX, StartY + 50, 0xFFA200B8, 0x0, WindowW, 0, 3, "Tài khoản chỉ được sử dụng #các ký tự 0~9, a~z"); //Notice

	float InputW = 110;
	float InputKhoangCach = 20;
	StartY += 30;

	for (int i = 0; i < TYPE_INPUT_DKTK::eMaxINPUT; i++)
	{
		TextDraw(g_hFontBold, StartX + 30, StartY + 50, 0xFFFFFFFF, 0x0, 100, 0, 1, mTextStatus[i]);
		gInterface->DrawBarForm((StartX + 120) - 3, (StartY + 50) - 3, InputW, 16, 0.0, 0.0, 0.0, 1.0);
		if (gInterface->RenderInputBox(StartX + 120, StartY + 50, InputW, 14, "", this->CInputData[i], (UIOPTIONS)mInputType[i], mMaxInput[i], (i == 1 ? TRUE : FALSE)))
		{
			this->CInputData[i]->SetTextColor(255, 255, 255, 255);
			this->CInputData[i]->SetBackColor(255, 0, 0, 0);

			if (this->CInputData[i + 1]) this->CInputData[i]->SetTabTarget(this->CInputData[i + 1]);
		}
		StartY += InputKhoangCach;
	}

	//===Captcha
	StartY += 20;
	float NTW = 70;
	TextDraw((HFONT)g_hFontBold, StartX + 40, StartY + 50, 0x62FF00FF, 0x0, 100, 0, 1, "Mã Captcha");
	float CaptChaX = StartX + 105;
	float CaptChaY = StartY + 45;
	if (!CInputCaptCha)
	{
		CInputCaptCha = new CUITextInputBox;
		CInputCaptCha->Init(gwinhandle->GethWnd(), CaptChaX, CaptChaY, 4);
		CInputCaptCha->SetBackColor(0, 0, 0, 0);
		CInputCaptCha->SetTextColor(255, 255, 157, 0);
		CInputCaptCha->SetFont((HFONT)g_hFont);
		CInputCaptCha->SetState(UISTATE_NORMAL);
		CInputCaptCha->SetOption(UIOPTION_NUMBERONLY);
		CInputCaptCha->SetPosition(CaptChaX, CaptChaY);
	}
	else
	{
		gInterface->RenderCaptchaNumber(CaptChaX, CaptChaY, CInputCaptCha, gInterface->vCaptcha.c_str());
	}
	StartY += 30;
	if (gInterface->DrawButton(StartX + 100, StartY + 45, 100, 12, "Xác Nhận", 80, true))
	{
		char CGetTextCaptCha[5] = { 0, };
		this->CInputCaptCha->GetText(CGetTextCaptCha, MAX_ID_SIZE + 1);
		std::string CaptchaInput(CGetTextCaptCha);
		if (!gInterface->check_Captcha(gInterface->vCaptcha, CaptchaInput))
		{
			gInterface->OpenMessageBox("Error", "Sai Mã Captcha");

		}
		else
		{
			this->RequsetDKTK();
		}

	}
	gInterface->DrawMessageBox();
	return 1;
}

bool CB_DangKyInGame::RequsetDKTK()
{
	char szID[MAX_ID_SIZE + 1] = { 0, };
	char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
	char szSno[7 + 1] = { 0, };
	char szSDT[11 + 1] = { 0, };

	this->CInputData[Account]->GetText(szID, MAX_ID_SIZE + 1);
	this->CInputData[Pass]->GetText(szPass, MAX_PASSWORD_SIZE + 1);
	this->CInputData[Snonumber]->GetText(szSno, 7 + 1);
	this->CInputData[Phone]->GetText(szSDT, 11 + 1);

	if (this->TimeSendRegTK > GetTickCount())
	{
		gInterface->OpenMessageBox("Error", "Thao tác chậm lại !!");
		return 0;
	}
	if (strlen(szID) < 1)
	{
		gInterface->OpenMessageBox("Error", "Vui lòng nhập Tài Khoản");
		return 0;
	}
	if (strlen(szPass) < 1)
	{
		gInterface->OpenMessageBox("Error", "Vui lòng nhập Mật Khẩu");
		return 0;
	}
	if (strlen(szSno) < 7)
	{
		gInterface->OpenMessageBox("Error", "Vui lòng nhập 7 số bảo mật");
		return 0;
	}
	if (strlen(szSDT) < 10)
	{
		gInterface->OpenMessageBox("Error", "Vui lòng nhập Số điện thoại");
		return 0;
	}

	if (!CheckChuoiKyTuDacBiet(szID) || !CheckChuoiKyTuDacBiet(szPass))
	{
		gInterface->OpenMessageBox("Error", "Tài khoản không được chứa ký tự đặc biệt");
		return 0;
	}
	//==Send GS
	PMSG_REGISTER_MAIN_SEND pMsg;
	pMsg.header.set(0xD3, 0x05, sizeof(pMsg));
	pMsg.TypeSend = 0x01;

	memcpy(&pMsg.account, szID, sizeof(pMsg.account));
	memcpy(&pMsg.password, szPass, sizeof(pMsg.password));
	memcpy(&pMsg.numcode, szSno, sizeof(pMsg.numcode));
	memcpy(&pMsg.sodienthoai, szSDT, sizeof(pMsg.sodienthoai));

	DataSend((LPBYTE)&pMsg, pMsg.header.size);

	gInterface->vCaptcha = gInterface->generateCaptcha(4); //tao lai Captcha
	this->TimeSendRegTK = GetTickCount() + 5000;

	return 1;
}

void CB_DangKyInGame::RecvKQRegInGame(XULY_CGPACKET* lpMsg)
{
	int TypeKQNotice = lpMsg->ThaoTac;
	//gInterface->OpenMessageBox("KQ", "KQ %d", lpMsg->ThaoTac);

	char szID[MAX_ID_SIZE + 1] = { 0, };
	this->CInputData[Account]->GetText(szID, MAX_ID_SIZE + 1);

	char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
	this->CInputData[Pass]->GetText(szPass, MAX_PASSWORD_SIZE + 1);


	switch (TypeKQNotice)
	{
	case CB_DangKyInGame::eDangKyThanhCong:
	{
		gInterface->OpenMessageBox("Kết Quả", "Chúc mừng bạn đã đăng ký thành công ! \n ID : %s", szID);
		CUIMng& rUIMng = CUIMng::Instance();
		rUIMng.m_LoginWin.GetIDInputBox()->SetText(szID);
		rUIMng.m_LoginWin.GetPassInputBox()->SetText(szPass);
	}
	break;
	case CB_DangKyInGame::eTaiKhoanDaTonTai:
		gInterface->OpenMessageBox("Kết Quả", "ID : %s \n Đã tồn tại vui lòng đăng ký TK khác!", szID);
		break;
	case CB_DangKyInGame::eDuLieuNhapKhongDung:
		gInterface->OpenMessageBox("Kết Quả", "Thông tin nhập không phù hợp, kiểm tra lại!");
		break;
	default:
		break;
	}
}
