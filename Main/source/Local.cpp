// Local.cpp: implementation of the Local
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Local.h"
#include "MultiLanguage.h"
#include "Utilities/Log/muConsoleDebug.h"

#define g_pMultiLanguage (CMultiLanguage::GetSingletonPtr())

bool CheckSpecialText(char* pszText)
{
	for (unsigned char* lpszCheck = (unsigned char*)pszText; *lpszCheck; ++lpszCheck)
	{
		if (1 == _mbclen(lpszCheck))
		{
			if (!(48 <= *lpszCheck && *lpszCheck < 58) && !(65 <= *lpszCheck && *lpszCheck < 91) && !(97 <= *lpszCheck && *lpszCheck < 123))
			{
				return (true);
			}
		}
		else
		{
			return (true);
		}
	}

	return false;
}

bool CheckSpecialText(wchar_t* pszText)
{
	std::wstring lpszCheck = L"";

	lpszCheck.append(pszText);

	for (wchar_t ch : lpszCheck)
	{
		if (ch == 0) break; // Fin de la cadena

		if (iswalnum(ch) || (ch >= 0x4E00 && ch <= 0x9FFF) ) // Chino básico
		{
			continue;
		}
		return true; // Contiene caracteres especiales, como símbolos
	}

	return false;
}


/**/