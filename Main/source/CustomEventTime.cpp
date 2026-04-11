#include "stdafx.h"
#include "CustomEventTime.h"
#include "CBInterface.h"
#include "Util.h"
#include "TextClien.h"
#include "NewUISystem.h"
#include "UIControls.h"

#include "wsclientinline.h"

using namespace SEASON3B;


CCustomEventTime* CCustomEventTime::Instance()
{
	static CCustomEventTime s_Instance;
	return &s_Instance;
}


CCustomEventTime::CCustomEventTime()
{
}

void CCustomEventTime::Init()
{
	this->Click = false;
	this->mNewDataEventTime.clear();
	this->MaxListData = 0;
}


void CCustomEventTime::Load(CUSTOM_EVENT_INFO* info) // OK
{
	for (int n = 0; n < MAX_EVENTTIME; n++)
	{
		this->SetInfo(info[n]);
	}
}

void CCustomEventTime::SetInfo(CUSTOM_EVENT_INFO info) // OK
{
	if (info.Index < 0 || info.Index >= MAX_EVENTTIME)
	{
		return;
	}

	this->m_CustomEventInfo[info.Index] = info;
}

void CCustomEventTime::ClearCustomEventTime() // OK
{
	for (int n = 0; n < MAX_EVENTTIME; n++)
	{
		gCustomEventTime[n].time = -1;
	}
	this->count = 0;
	this->EventTimeEnable = 0;
}
void CCustomEventTime::OpenTestWindow(int Page)
{
	gInterface->Data[eWindowEventTime].OnShow = true;

	PMSG_CUSTOM_EVENTTIME_SEND pMsg;

	pMsg.header.set(0xF3, 0xE8, sizeof(pMsg));

	pMsg.Page = Page;

	DataSend((BYTE*)&pMsg, pMsg.header.size);

};
void CCustomEventTime::OnOffWindow() // OK
{
	gInterface->Data[eWindowEventTime].OpenClose();
	if (gInterface->Data[eWindowEventTime].OnShow)
	{
		g_CustomEventTime->ClearCustomEventTime();
		g_CustomEventTime->OpenTestWindow();
	}
}
void CCustomEventTime::GCReqEventTime(PMSG_CUSTOM_EVENTTIME_RECV* lpMsg) // OK
{
	this->count = lpMsg->count;
	this->mNewDataEventTime.clear();
	this->MaxListData = lpMsg->MaxList;
	for (int n = 0; n < lpMsg->count; n++)
	{
		CUSTOM_EVENTTIME_DATA* lpInfo = (CUSTOM_EVENTTIME_DATA*)(((BYTE*)lpMsg) + sizeof(PMSG_CUSTOM_EVENTTIME_RECV) + (sizeof(CUSTOM_EVENTTIME_DATA) * n));

		//this->gCustomEventTime[n].index = lpInfo->index;
		//this->gCustomEventTime[n].time = lpInfo->time;
		//g_Console.AddMessage(1, "Debug %d (%d) %s Max %d", lpInfo->index, lpInfo->time, lpInfo->NameEvent, this->MaxListData);
		this->mNewDataEventTime.push_back(*lpInfo);
		//if (this->gCustomEventTime[n].index >= 28 && this->gCustomEventTime[n].time != -1) this->Arena = 1;
	}

	this->EventTimeEnable = 1;
}

void CCustomEventTime::DrawEventTimePanelWindow()
{

	if (!gInterface->Data[eWindowEventTime].OnShow)
	{
		return;
	}


	float MainWidth = 310;
	float MainHeight = 315;
	float StartY = 80.0;
	float StartX = (MAX_WIN_WIDTH / 2) - (MainWidth / 2);

	gInterface->gDrawWindowCustom(&StartX, &StartY, MainWidth, MainHeight, eWindowEventTime, "Bảng Thời Gian Sự Kiện  2");
	int RowCol = (MainWidth / 4);
	float MainCenter = StartX + RowCol;
	float ButtonX = MainCenter - (float)(29.0 / 2);
	float StartBody = StartY;
	DWORD Color = 0xFFFFFFB8;

	//EventTime Next
	//if ((this->MaxListData / 14) > this->Page)
	//{
	//	if (gInterface->DrawButtonGUI(gInterface->Data[eButtonNext].ModelID, StartX + MainWidth - 38, StartY + (MainHeight / 2) - 20, 25, 40))
	//	{
	//		this->Page++;
	//		this->OpenTestWindow(this->Page);
	//	}
	//}
	////EventTime ePrev
	//if (this->Page > 0)
	//{
	//	if (gInterface->DrawButtonGUI(gInterface->Data[eButtonPrevious].ModelID, StartX + 10, StartY + (MainHeight / 2) - 20, 25, 40))
	//	{
	//		this->Page--;
	//		this->OpenTestWindow(this->Page);
	//	}
	//}
	TextDraw((HFONT)g_hFont, StartX, StartY + (MainHeight - 35), 0x7DF4FFFF, 0x0, MainWidth, 0, 3, "Trang: %d/%d", this->Page + 1, (this->MaxListData / 14) + 1);

	if (this->Page > 0)
	{
		if (gInterface->DrawButtonGUI(CNewUIInGameShop::IMAGE_IGS_PAGE_LEFT, StartX + 40, StartY + (MainHeight - 40), 20, 23, 3))
		{
			this->Page--;
			this->OpenTestWindow(this->Page);
		}
	}
	if ((this->MaxListData / 14) > this->Page)
	{
		if (gInterface->DrawButtonGUI(CNewUIInGameShop::IMAGE_IGS_PAGE_RIGHT, StartX + (MainWidth - 65), StartY + (MainHeight - 40), 20, 23, 3))
		{
			this->Page++;
			this->OpenTestWindow(this->Page);
		}
	}
	if (this->EventTimeEnable == 1)
	{
		gInterface->DrawInfoBox(StartX + 20, StartBody + 37, MainWidth - 50, 10, 0x00000096, 0); //1vs3
		//gInterface->DrawBarForm(StartX+20, StartBody + 37, MainWidth-50, 15, 0.5445, 0.55, 0.549, 0.8);
		TextDraw(g_hFontBold, StartX + 10, StartBody + 39, 0xFFFFFFA8, 0x0, RowCol, 0, 3, "Sự kiện");
		TextDraw(g_hFontBold, StartX + 10 + (RowCol * 1), StartBody + 39, 0xFFFFFFA8, 0x0, RowCol, 0, 3, "Vị Trí");
		TextDraw(g_hFontBold, StartX + 10 + (RowCol * 2), StartBody + 39, 0xFFFFFFA8, 0x0, RowCol, 0, 3, "Thời Gian");
		TextDraw(g_hFontBold, StartX + (RowCol * 3), StartBody + 39, 0xFFFFFFA8, 0x0, RowCol, 0, 3, "Move");

		if ((GetTickCount() - this->EventTimeTickCount) > 1000)
		{
			for (int i = 0; i < this->count; i++)
			{
				if (this->mNewDataEventTime[i].time > 0)
				{
					this->mNewDataEventTime[i].time = this->mNewDataEventTime[i].time - 1;
				}
			}
			this->EventTimeTickCount = GetTickCount();
		}

		char text1[20];
		char text2[30];
		int totalseconds;
		int hours;
		int minutes;
		int seconds;
		int days;

		int line = 0;

		for (int i = 0; i < this->mNewDataEventTime.size(); i++)
		{
			if (this->mNewDataEventTime[i].time <= -1)
			{
				wsprintf(text2, "Disabled");
				//continue;
			}
			else if (this->mNewDataEventTime[i].time == 0)
			{
				wsprintf(text2, "Online");
			}
			else
			{
				totalseconds = this->mNewDataEventTime[i].time;
				hours = totalseconds / 3600;
				minutes = (totalseconds / 60) % 60;
				seconds = totalseconds % 60;
				wsprintf(text2, "%02d:%02d:%02d", hours, minutes, seconds);

				if (hours > 23)
				{
					days = hours / 24;
					wsprintf(text2, "%d day(s)+", days);
				}
				else
				{
					wsprintf(text2, "%02d:%02d:%02d", hours, minutes, seconds);
				}
			}

			if (this->mNewDataEventTime[i].time <= -1)
			{
				Color = 0xFFB145B8;
			}
			else if (this->mNewDataEventTime[i].time == 0)
			{
				Color = 0x45FF7AB8;
			}
			else if (this->mNewDataEventTime[i].time < 300)
			{
				Color = 0xA8FF45B8;
			}
			else
			{
				Color = 0xFFFFFFB8;
			}
			//gInterface->DrawBarForm(StartX + 20, StartBody + 37, MainWidth - 50, 15, 0.5445, 0.55, 0.549, 0.8);
			TextDraw(g_hFontBold, StartX + 10, StartBody + 58 + (line), 0xFFFF478A8, 0x0, RowCol, 0, 3, TEXT(this->mNewDataEventTime[i].NameEvent));
			TextDraw(g_hFontBold, StartX + 10 + (RowCol * 1), StartBody + 58 + (line), 0x61FFD0A8, 0x0, RowCol, 0, 3, TEXT(this->mNewDataEventTime[i].DesString));
			TextDraw(g_hFontBold, StartX + 10 + (RowCol * 2), StartBody + 58 + (line), Color, 0x0, RowCol, 0, 3, TEXT(text2));

			if (this->mNewDataEventTime[i].NumberGate != -1)
			{
				//TextDraw(g_hFontBold, StartX + (RowCol * 3), StartBody + 58 + (line), Color, 0x0, RowCol, 0, 3, "%d", this->mNewDataEventTime[i].NumberGate);
				if (gInterface->DrawButtonGUI(BITMAP_HERO_POSITION_INFO_BEGIN + 6, StartX + 30 + (RowCol * 3), StartBody + 58 + (line), 18, 13))
				{
					XULY_CGPACKET pMsg;
					pMsg.header.set(0xD3, 0x01, sizeof(pMsg));
					pMsg.ThaoTac = this->mNewDataEventTime[i].NumberGate;
					DataSend((LPBYTE)&pMsg, pMsg.header.size);
				}

				if (SEASON3B::CheckMouseIn(StartX + 30 + (RowCol * 3), StartBody + 58 + (line), 18, 13) == 1)
				{
					RenderTipText(StartX + 30 + 30 + (RowCol * 3), StartBody + 58 + (line), "Di Chuyên Đế Event");
				}
			}

			//gCBUtil.DrawText((HFONT)CustomFont.FontSize15, StartX, StartBody + 55 + (line)+2, 0xFFFFFFA8, 0x0, MainWidth, 0, 3, "_________________________________");

			line += 15;
		}
	}
	else
	{
		if (this->EventTimeLoad == 1)
		{
			gInterface->DrawFormat(eGold, MainCenter + 20, StartBody + 100, 52, 1, "Loading ..");
			this->EventTimeLoad = 2;
		}
		else if (this->EventTimeLoad == 2)
		{
			gInterface->DrawFormat(eGold, MainCenter + 20, StartBody + 100, 52, 1, "Loading ...");
			this->EventTimeLoad = 3;
		}
		else if (this->EventTimeLoad == 3)
		{
			gInterface->DrawFormat(eGold, MainCenter + 20, StartBody + 100, 52, 1, "Loading ....");
			this->EventTimeLoad = 4;
		}
		else if (this->EventTimeLoad == 4)
		{
			gInterface->DrawFormat(eGold, MainCenter + 20, StartBody + 100, 52, 1, "Loading .....");
			this->EventTimeLoad = 0;
		}
		else
		{
			gInterface->DrawFormat(eGold, MainCenter + 20, StartBody + 100, 52, 1, "Loading .");
			this->EventTimeLoad = 1;
		}
	}

}
