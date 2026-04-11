#pragma once
#include "Protocol.h"
#if(CB_GETMIXRATE)
class CB_GetMixRate
{
public:
	struct PMSG_CHAOX_MIX_GET_RATE_SEND
	{
		PBMSG_HEAD header;
		int MixID;
		int MixInfo;
	};

	struct PMSG_CHAOX_MIX_GET_RATE_RECV
	{
		PBMSG_HEAD header; // C1:86
		int Rate;
		int Zen;
	};


	CB_GetMixRate();
	~CB_GetMixRate();
	void SetInfoMixID(int MixID);
	void CGSendMixInfo();
	void Clear();
	int RealMix;

	void GCRecvMixInfo(BYTE* lpMsg, int size);
	bool ShowRateInfo;
	static CB_GetMixRate* Instance();
};

#define gCB_GetMixRate			(CB_GetMixRate::Instance())

#endif