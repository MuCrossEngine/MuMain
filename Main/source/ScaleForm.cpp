#include "stdafx.h"
#include "ScaleForm.h"
#include "Utilities/Log/muConsoleDebug.h"

#ifdef SHUTDOWN_SCALEFORM_INFO
PLACEMENT_NEW placementObj_new = NULL;
PLACEMENT_NEW placementMov_new = NULL;

SCALEFORM::SCALEFORM()
{
	hModule = NULL;
}

SCALEFORM::~SCALEFORM()
{
	Release();
}

void SCALEFORM::Release()
{
	if (hModule != NULL)
	{
		hldList.clear();

		void (*ExitProc)() = (void(*)())GetProcAddress(hModule, "runtime_kill");

		if (ExitProc) ExitProc();

		FreeLibrary(hModule);
	}
}

SCALEFORM* SCALEFORM::Instance()
{
	static SCALEFORM s_Instance;
	return &s_Instance;
}

void SCALEFORM::runtime_connection()
{
	hModule = LoadLibrary("ScaleForm.dll");

	if (hModule != NULL)
	{
		void (*EntryProc)() = (void(*)())GetProcAddress(hModule, "EntryProc");

		if (EntryProc != 0)
		{
			placementMov_new = ((PLACEMENT_NEW)GetProcAddress(hModule, "InitializeMovieLoader"));

			placementObj_new = ((PLACEMENT_NEW)GetProcAddress(hModule, "InitializeContainerType"));

			EntryProc();
		}

		if (placementMov_new == NULL || placementObj_new == NULL)
		{
			FreeLibrary(hModule);
		}
	}
}

void SCALEFORM::runtime_disconnect()
{
	Release();
}

void commandInternal(void* pClass, const char* pName, const char* pCommand)
{
}

void commandExternal(void* pClass, const char* pName, const void* gfxValue, UInt numArgs)
{
}

void SCALEFORM::LoadMovie()
{
	this->LoadMovie(0, "hudNotice.gfx", commandInternal, commandExternal, 5);
}

void SCALEFORM::LoadMovie(UInt index, const char* pfilename, FUNC_INTERNAL pCommand, FUNC_EXTERNAL pExternal, UInt align)
{
	if (placementMov_new)
	{
		auto gfxFile = std::make_shared<CONTAINER_HANDLE>();

		if (gfxFile)
		{
			gfxFile->Live = false;
			gfxFile->index = index;
			gfxFile->shared.AddMovie(pCommand, pExternal, pfilename, align);
			gfxFile->shared.OnCreateDevice(WindowWidth, WindowHeight);
			hldList.push_back(gfxFile);
		}
	}
}

void SCALEFORM::CreateObject(UInt index, SCALEFORMOBJECT* gfxValue)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		shared->CreateObject(gfxValue);
	}
}

void SCALEFORM::CreateArray(UInt index, SCALEFORMOBJECT* gfxValue, UInt Size)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		shared->CreateArray(gfxValue, Size);
	}
}

void SCALEFORM::Render(UInt index)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		EndBitmap();
		shared->Render();
		BeginBitmap();
	}
}

void SCALEFORM::Update(UInt index)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		shared->Update();
	}
}

void SCALEFORM::Invoke(UInt index, const char* pmethodName, const char* pargFmt, ...)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		va_list args;
		va_start(args, pargFmt);
		shared->Invoke(pmethodName, pargFmt, args);
		va_end(args);
	}
}

void SCALEFORM::Invoke(UInt index, const char* pmethodName, SCALEFORMOBJECT* pargFmt)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		shared->Invoke(pmethodName, pargFmt);
	}
}

void SCALEFORM::Invoke(UInt index, const char* pmethodName, SCALEFORMOBJECT* pargFmt, int Size)
{
	SCALEFORMHANDLE* shared = GetSharedHandleByIndex(index);

	if (shared)
	{
		shared->Invoke(pmethodName, pargFmt);
	}
}

bool SCALEFORM::CallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (size_t i = 0; i < hldList.size(); i++)
	{
		if (hldList[i]->Live)
		{
			hldList[i]->shared.runtime_event(uMsg, wParam, lParam);
		}
	}

	return false;
}

SCALEFORMHANDLE* SCALEFORM::GetSharedHandleByIndex(int searchIndex)
{
	SCALEFORMHANDLE* shared = NULL;

	auto it = std::find_if(hldList.begin(), hldList.end(), [searchIndex](const std::shared_ptr<CONTAINER_HANDLE>& handle) {
		return handle->index == searchIndex;
		});

	if (it != hldList.end())
	{
		shared = &(*it)->shared;
	}

	return shared;
}

SCALEFORMOBJECT::SCALEFORMOBJECT()
{
	memset(inheritance, 0, sizeof(inheritance));
	placementObj_new(this, 0x00);
}

SCALEFORMOBJECT::SCALEFORMOBJECT(UInt Type)
{
	memset(inheritance, 0, sizeof(inheritance));
	placementObj_new(this, Type);
}

bool SCALEFORMOBJECT::AddMember(const char* name, bool to_member)
{
	return (*(bool(__thiscall**)(void*, const char*, int))(*(DWORD*)this + 4))(this, name, to_member);
}

bool SCALEFORMOBJECT::AddMember(const char* name, int to_member)
{
	return (*(bool(__thiscall**)(void*, const char*, double))(*(DWORD*)this + 8))(this, name, static_cast<double>(to_member));
}

bool SCALEFORMOBJECT::AddMember(const char* name, double to_member)
{
	return (*(bool(__thiscall**)(void*, const char*, double))(*(DWORD*)this + 8))(this, name, to_member);
}

bool SCALEFORMOBJECT::AddMember(const char* name, const char* to_member)
{
	return (*(bool(__thiscall**)(void*, const char*, const char*))(*(DWORD*)this + 12))(this, name, to_member);
}

bool SCALEFORMOBJECT::AddElement(UInt idx, void* to_element)
{
	return (*(bool(__thiscall**)(void*, UInt, void*))(*(DWORD*)this + 16))(this, idx, to_element);
}

bool SCALEFORMOBJECT::SetArraySize(UInt Size)
{
	return (*(bool(__thiscall**)(void*, UInt))(*(DWORD*)this + 20))(this, Size);
}

SCALEFORMHANDLE::SCALEFORMHANDLE()
{
	memset(this, 0, sizeof(SCALEFORMHANDLE));
	placementMov_new(this, 0x01);
}

bool SCALEFORMHANDLE::AddMovie(FUNC_INTERNAL pCommand, FUNC_EXTERNAL pExternalCommand, const char* pfilename, UInt AlignType)
{
	return (*(bool(__thiscall**)(void*, FUNC_INTERNAL, FUNC_EXTERNAL, const char*, UInt))(*(DWORD*)this + 4))(this,
		pCommand, pExternalCommand, pfilename, AlignType);
}

bool SCALEFORMHANDLE::Update()
{
	return (*(bool(__thiscall**)(void*))(*(DWORD*)this + 8))(this);
}

bool SCALEFORMHANDLE::Render()
{
	return (*(bool(__thiscall**)(void*))(*(DWORD*)this + 12))(this);
}

void SCALEFORMHANDLE::CreateObject(SCALEFORMOBJECT* gfxValue)
{
	(*(bool(__thiscall**)(void*, void*))(*(DWORD*)this + 16))(this, gfxValue);
}

void SCALEFORMHANDLE::CreateArray(SCALEFORMOBJECT* gfxValue, UInt sz)
{
	(*(bool(__thiscall**)(void*, void*, UInt))(*(DWORD*)this + 20))(this, gfxValue, sz);
}

void SCALEFORMHANDLE::Invoke(const char* methodName, SCALEFORMOBJECT* gfxValue)
{
	(*(void(__thiscall**)(void*, const char*, void*))(*(DWORD*)this + 28))(this, methodName, gfxValue);
}

void SCALEFORMHANDLE::Invoke(const char* methodName, const char* pargFmt, va_list args)
{
	(*(void(__thiscall**)(void*, const char*, const char*, va_list))(*(DWORD*)this + 24))(this, methodName, pargFmt, args);
}

void SCALEFORMHANDLE::OnResetDevice(SInt bufw, SInt bufh)
{
	(*(void(__thiscall**)(void*, SInt, SInt))(*(DWORD*)this + 32))(this, bufw, bufh);
}

bool SCALEFORMHANDLE::OnCreateDevice(SInt bufw, SInt bufh)
{
	return (*(bool(__thiscall**)(void*, SInt, SInt))(*(DWORD*)this + 36))(this, bufw, bufh);
}

void SCALEFORMHANDLE::runtime_event(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	(*(void(__thiscall**)(void*, UInt, WPARAM, LPARAM))(*(DWORD*)this + 40))(this, uMsg, wParam, lParam);
}


extern int FontHeight;

void gfxNotice::addNoticeText(char* Text, DWORD Style)
{
	DWORD HexColor = RGBA(0xff, 0xff, 0xff, 0xff);

	switch (Style)
	{
	case 0:
		HexColor = RGBA(0xff, 0xc8, 0x32, 0xff);
		break;
	case 1:
		HexColor = RGBA(0xff, 0xff, 0xff, 0xff);
		break;
	case 2:
		HexColor = RGBA(0x32, 0xe2, 0x86, 0xff);
		break;
	}

	const char* HTML = SEASON3B::FontHTML(Text, HexColor, (int)(FontHeight-1), true, false, 3);

	gfxinit->Invoke(0, "addText", "%s", HTML);
}

void gfxNotice::addEventMapText(char* Text)
{
	const char* HTML = SEASON3B::FontHTML(Text, RGBA(0xff, 0xcc, 0x19u, 0xffu), 0, false, true);
	gfxinit->Invoke(0, "addEventMapText", "%s", HTML);
}

void gfxNotice::addLongMovementText(const char* Text, int Size, int YPos, int Duration)
{
	const char* textHTML = SEASON3B::FontHTML(Text, RGBA(0xff, 0xcc, 0x19u, 0xffu), Size, false, true);

	SCALEFORMOBJECT gfxvalue(6);

	gfxinit->CreateObject(0, &gfxvalue);
	gfxvalue.AddMember("Duration", (double)Duration);
	gfxvalue.AddMember("YPos", (double)(YPos / g_fScreenRate_y));
	gfxvalue.AddMember("Text", textHTML);
	gfxinit->Invoke(0, "addLongMovementText", &gfxvalue);
}
#endif // SHUTDOWN_SCALEFORM_INFO
