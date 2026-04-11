#include "stdafx.h"
#include "Protocol.h"
#include "WSclient.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "SkillManager.h"
#include "CharacterManager.h"
#if (CB_GETMIXRATE)
#include "CB_GetMixRate.h"
#endif
#include "CustomEventTime.h"
#include "CB_DangKyInGame.h"

BOOL ProtocolCoreEx(BYTE head, BYTE* lpMsg, int size, int key) // OK
{
	switch (head)
	{
#if(CB_GETMIXRATE)
	case 0x88:
		if (gCB_GetMixRate) gCB_GetMixRate->GCRecvMixInfo(lpMsg, size);
		break;
#endif
	case 0xF3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
		case 0xE8:
			g_CustomEventTime->GCReqEventTime((PMSG_CUSTOM_EVENTTIME_RECV*)lpMsg);
			return 1;
		}
		break;
	case 0xD3:
		switch (((lpMsg[0] == 0xC1) ? lpMsg[3] : lpMsg[4]))
		{
#if(CB_DANGKYINGAME)
		case 0x05:
			gCB_DangKyInGame->RecvKQRegInGame((XULY_CGPACKET*)lpMsg);
			break;
#endif
		}
		break;

	}
	return false;
}

