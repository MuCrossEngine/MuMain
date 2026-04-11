#pragma once
typedef int                 SInt;
typedef unsigned int        UInt;

#ifdef SHUTDOWN_SCALEFORM_INFO
typedef void (*FUNC_INTERNAL)(void*, const char*, const char*);
typedef void (*FUNC_EXTERNAL)(void*, const char*, const void*, UInt);
typedef void (*PLACEMENT_NEW)(void*, UInt);

class SCALEFORMOBJECT //-- size: 24
{
private:
	BYTE inheritance[24];
public:
	SCALEFORMOBJECT();
	SCALEFORMOBJECT(UInt Type);
	bool AddMember(const char* name, bool to_member);
	bool AddMember(const char* name, int to_member);
	bool AddMember(const char* name, double to_member);
	bool AddMember(const char* name, const char* to_member);
	bool AddElement(UInt idx, void* to_element);
	bool SetArraySize(UInt Size);
};

class SCALEFORMHANDLE //-- size: 80
{
private:
	BYTE inheritance[80];
public:
	SCALEFORMHANDLE();
	bool AddMovie(FUNC_INTERNAL pCommand, FUNC_EXTERNAL pExternalCommand, const char* pfilename, UInt AlignType);
	bool Update();
	bool Render();
	void CreateObject(SCALEFORMOBJECT* gfxValue);
	void CreateArray(SCALEFORMOBJECT* gfxValue, UInt sz);
	void Invoke(const char* methodName, SCALEFORMOBJECT* gfxValue);
	void Invoke(const char* methodName, const char* pargFmt, va_list args);
	void OnResetDevice(SInt bufw, SInt bufh);
	bool OnCreateDevice(SInt bufw, SInt bufh);
	void runtime_event(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class SCALEFORM
{
	class CONTAINER_HANDLE
	{
	public:
		BOOL Live;
		UInt index;
		SCALEFORMHANDLE shared;
	};
public:
	SCALEFORM();
	virtual~SCALEFORM();
	void Release();
	static SCALEFORM* Instance();
	void runtime_connection();
	void runtime_disconnect();
	void LoadMovie();
	void LoadMovie(UInt index, const char* pfilename, FUNC_INTERNAL pCommand, FUNC_EXTERNAL pExternal, UInt align);
	void CreateObject(UInt index, SCALEFORMOBJECT* gfxValue);
	void CreateArray(UInt index, SCALEFORMOBJECT* gfxValue, UInt Size);

	void Render(UInt index);
	void Update(UInt index);

	void Invoke(UInt index, const char* pmethodName, const char* pargFmt, ...);
	void Invoke(UInt index, const char* pmethodName, SCALEFORMOBJECT* pargFmt);
	void Invoke(UInt index, const char* pmethodName, SCALEFORMOBJECT* pargFmt, int Size);
	bool CallBack(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	SCALEFORMHANDLE* GetSharedHandleByIndex(int searchIndex);
private:
	HMODULE hModule;
	std::vector<std::shared_ptr<CONTAINER_HANDLE>> hldList;
};

namespace gfxNotice
{
	void addNoticeText(char* Text, DWORD Style);
	void addEventMapText(char* Text);
	void addLongMovementText(const char* Text, int Size, int YPos, int Duration);
}

#define gfxinit					(SCALEFORM::Instance())

#endif // SHUTDOWN_SCALEFORM_INFO
