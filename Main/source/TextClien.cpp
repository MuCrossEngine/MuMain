#include "StdAfx.h"
#include "TextClien.h"
#include "Util.h"
#include <stdlib.h>
#include "windows.h"
//#include "atlstr.h"
#include "Interface.h"
#include "stdio.h"
#include "./Utilities/Log/muConsoleDebug.h"

#include "zzzinventory.h"
#include "dsplaysound.h"
#include "CSItemOption.h"

cTextClien gTextClien;

void cTextClien::Load()
{
	char strFileName[255];
	memset(strFileName, 0, sizeof(strFileName));
	sprintf(strFileName, "./Data/Custom/TextClien/TextClien_%s.ini", g_strSelectedML.c_str());

	char GetFotText[35] = { 0 };
	for (int st = 0; st < 50; st++)
	{
		wsprintf(GetFotText, "Mess%d", st);
		GetPrivateProfileStringA("ThongTin", GetFotText, "Null", TextClien_ThongTin[st], sizeof(TextClien_ThongTin[st]), strFileName);
		GetPrivateProfileStringA("Khac",   GetFotText, "Null", TextClien_Khac[st], sizeof(TextClien_Khac[st]), strFileName);

		g_ConsoleDebug->Write(5, "[Load text clien: %d: %s]", st, TextClien_ThongTin[st]);
	}

	

}