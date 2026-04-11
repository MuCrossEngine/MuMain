#include "stdafx.h"
#include "jpexs.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "ZzzInterface.h"
#include "NewUISystem.h"
#include "NewUICommon.h"
#include "CBDrawInterface.h"
#include "TextClien.h"
#include "wsclientinline.h"
#include "CustomEventTime.h"
using namespace SEASON3B;
CBDrawInterface* CBDrawInterface::Instance()
{
	static CBDrawInterface s_Instance;
	return &s_Instance;
}

CBDrawInterface::CBDrawInterface()
{

}

CBDrawInterface::~CBDrawInterface()
{
}
DWORD  SleepTimeHP = 0;
void CBDrawInterface::RenderFrame()
{
	g_CustomEventTime->DrawEventTimePanelWindow();

	gInterface->DrawMessageBox();
}


