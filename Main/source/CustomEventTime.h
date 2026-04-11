#pragma once

#include "Protocol.h"
#include "CBInterface.h"

#define pSetCursorFocus			0

#define MAX_EVENTTIME 42

struct PMSG_CUSTOM_EVENTTIME_RECV
{
	PSWMSG_HEAD h;
	int MaxList;
	int count;
};

struct PMSG_CUSTOM_EVENTTIME_SEND
{
	PSBMSG_HEAD header; // C1:BF:51
	BYTE Page;
};

struct CUSTOM_EVENTTIME_DATA
{
	int index;
	char NameEvent[30];
	char DesString[90];
	int NumberGate;
	int time;
};

struct CUSTOM_EVENT_INFO
{
	int Index;
	char Name[40];
};


class CCustomEventTime
{
public:
	CCustomEventTime();
	void Init();
	void Load(CUSTOM_EVENT_INFO* info);
	void SetInfo(CUSTOM_EVENT_INFO info);
	void ClearCustomEventTime();
	void GCReqEventTime(PMSG_CUSTOM_EVENTTIME_RECV* lpMsg);
	void DrawEventTimePanelWindow();

	bool Click;
	DWORD OpenTestDelay;
	void OpenTestWindow(int Page = 0);

	CUSTOM_EVENT_INFO m_CustomEventInfo[MAX_EVENTTIME];
	int count;
	CUSTOM_EVENTTIME_DATA gCustomEventTime[MAX_EVENTTIME];
	int EventTimeLoad;
	int EventTimeEnable;
	int EventTimeTickCount;
	int Page;
	int Arena;
	std::vector<CUSTOM_EVENTTIME_DATA> mNewDataEventTime;
	int MaxListData;
	void OnOffWindow();
	static CCustomEventTime* Instance();

};
#define g_CustomEventTime			(CCustomEventTime::Instance())



