#include "stdafx.h"
#include "CGMProtect.h"
#include "GOBoid.h"
#include "MapManager.h"
#include "CGMItemWings.h"
#include "w_PetProcess.h"
#include "CGMMonsterMng.h"
#include "CGMItemEffect.h"
#include "CGMJewelOfAction.h"
#include "CGMPhysicsManager.h"
#include "CGMRenderGroupMesh.h"
#include "..\\..\\..\\Util\\CCRC32.H"

#include <tlhelp32.h>
#include <intrin.h>



extern BYTE Version[SIZE_PROTOCOLVERSION];
extern BYTE Serial[SIZE_PROTOCOLSERIAL + 1];

CGMProtect::CGMProtect()
{
	m_hMutex = NULL;
	memset(&m_Kernel, 0, sizeof(m_Kernel));
	memset(&m_MainInfo, 0, sizeof(m_MainInfo));
}

CGMProtect::~CGMProtect()
{
	this->Release();
}

CGMProtect* CGMProtect::Instance()
{
	static CGMProtect sInstance;
	return &sInstance;
}

WORD CGMProtect::sin_port()
{
	return m_Kernel.sin_port;
}

WORD CGMProtect::GetMaxOfInstance()
{
	return m_MainInfo.m_MaxInstance;
}

char* CGMProtect::GetWindowName()
{
	return m_Kernel.WindowName;
}

char* CGMProtect::GetScreenPath()
{
	return m_Kernel.ScreenShotPath;
}

bool CGMProtect::CreateKeyEnv()
{
	WORD Key = 0;

	for (int n = 0; n < sizeof(m_Kernel.CustomerName); n++)
	{
		Key += (BYTE)(m_Kernel.CustomerName[n] ^ m_Kernel.ClientSerial[(n % sizeof(m_Kernel.ClientSerial))]);
	}

	this->EncDecKey[0] = (BYTE)0xE2;
	this->EncDecKey[1] = (BYTE)0x02;

	this->EncDecKey[0] += LOBYTE(Key);
	this->EncDecKey[1] += HIBYTE(Key);

	return true;
}

void CGMProtect::HookComplete()
{
	szServerIpAddress = new char[32];
	strncpy_s(szServerIpAddress, (sizeof(char) * 32), m_Kernel.IpAddress, (sizeof(char) * 32));

	g_ServerPort = m_Kernel.sin_port;

	Version[0] = (m_Kernel.ClientVersion[0] + 1);
	Version[1] = (m_Kernel.ClientVersion[2] + 2);
	Version[2] = (m_Kernel.ClientVersion[3] + 3);
	Version[3] = (m_Kernel.ClientVersion[5] + 4);
	Version[4] = (m_Kernel.ClientVersion[6] + 5);

	memcpy(Serial, m_Kernel.ClientSerial, sizeof(Serial));
}

bool CGMProtect::ReadMainFile()
{
	char text[MAX_PATH];

	if (this->IsRunningInVM())
	{
		MessageBox(0, "virtual machine detected", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (ReadMainConnect(GetExecutablePath("Data\\Local\\Connect.msil"), &m_Kernel, sizeof(MAIN_INFO_ENV)) == false)
	{
		sprintf_s(text, "%s - not found or invalid!", GetExecutablePath("Data\\Local\\Connect.msil").c_str());
		MessageBox(0, text, "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (LoadMainFileInfo(m_MainInfo, GetExecutablePath("Data\\av-code45.pak")) == false)
	{
		return false;
	}

	if (ReadTextFile(".\\Data\\Local\\CBTextInfo.bin") == 0)
	{
		MessageBox(0, "Config corrupt! ReadTextFile", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return 0;
	}
	GInfo.loadnInformation(this->m_TextInfo.m_TRSTooltipData);
	GInfo.loadnInformationSet(this->m_TextInfo.m_TRSTooltipSetData);
	GInfo.loadnText(this->m_TextInfo.m_TRSTooltipText);

#ifdef MAX_INSTANCE_GAME
	if (this->runtime_create_mutex() == false)
	{
		MessageBox(0, "has exceeded the limit of allowed instances", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
#endif // MAX_INSTANCE_GAME

	this->CreateKeyEnv();

	runtime_open_module_crc32(GetExecutablePath(m_MainInfo.AntiHackName), m_MainInfo.AntiHackCRC32);

	return true;
}

bool CGMProtect::ReadTextFile(char* name) // OK
{
	HANDLE file = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (file == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	if (GetFileSize(file, 0) != sizeof(TEXT_FILE_INFO))
	{
		CloseHandle(file);
		MessageBox(0, "ReadTextFile 1", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	DWORD OutSize = 0;

	if (ReadFile(file, &this->m_TextInfo, sizeof(TEXT_FILE_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(file);
		MessageBox(0, "ReadTextFile 2", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	for (int n = 0; n < sizeof(TEXT_FILE_INFO); n++)
	{
		((BYTE*)&this->m_TextInfo)[n] -= (BYTE)(0x95 ^ HIBYTE(n));
		((BYTE*)&this->m_TextInfo)[n] ^= (BYTE)(0xCA ^ LOBYTE(n));
	}

	CloseHandle(file);


	return 1;
}
bool CGMProtect::ReadMainConnect(std::string filename, void* pEnvStruct, int Size)
{
	BYTE* Buffer = NULL;
	bool success = false;

	int MaxLine = PackFileDecrypt(filename, &Buffer, 1, Size, 0x5A18);

	if (MaxLine != 0 && Buffer)
	{
		success = true;
		memcpy(pEnvStruct, Buffer, Size);
	}

	SAFE_DELETE_ARRAY(Buffer);

	return success;
}

bool CGMProtect::LoadMainFileInfo(MAIN_FILE_INFO& mainInfo, std::string& filePath)
{
	bool Success = true;

	FILE* fp = fopen(filePath.c_str(), "rb");

	if (fp != nullptr)
	{
		DWORD dwCheckSum = 0;
		fread(&dwCheckSum, sizeof(DWORD), 1u, fp);
		fread(&mainInfo, sizeof(MAIN_FILE_INFO), 1, fp);

		if (dwCheckSum != GenerateCheckSum2((BYTE*)&mainInfo, sizeof(MAIN_FILE_INFO), 0x5A18))
		{
			fclose(fp);
			Success = false;
			MessageBox(NULL, "Data\\av-code45.pak - File corrupted.", "TxtConvertor", MB_OK | MB_ICONWARNING);
		}
		else
		{
			BuxConvert((BYTE*)&mainInfo, sizeof(MAIN_FILE_INFO));
			// Serializar cada vector
			/*+001*/ gMapManager->ReadData(fp);
			/*+002*/ readVector(fp, CustomItem);
			/*+003*/ GMJewelOfAction->ReadData(fp);
			/*+004*/ readVector(fp, CustomImagen);
			/*+005*/ GMItemWings->ReadData(fp);
			/*+006*/ GMPhysicsManager->ReadData(fp);
			/*+007*/ readVector(fp, CustomPetItem);
			/*+008*/ readVector(fp, CustomStacks);
			/*+009*/ readVector(fp, CustomSmokeItem);
			/*+010*/ readVector(fp, CustomMonster);
			/*+011*/ readVector(fp, CustomNpcName);
			/*+012*/ readVector(fp, CustomEffectDinamic);
			/*+013*/ readVector(fp, CustomEffectStatics);
			/*+014*/ readVector(fp, CustomPetEffectDinamic);
			/*+015*/ readVector(fp, CustomPetEffectStatics);
			/*+016*/ readVector(fp, CustomItemRenderMesh);
			/*+017*/ readVector(fp, CustomCharRenderMesh);
			/*+018*/ readVector(fp, CustomBuyVip);
			/*+019*/ readVector(fp, CustomCommand);
			/*+020*/ readVector(fp, CustomCharNameWave);
			/*+021*/ readVector(fp, CustomDefaultRenderMesh);
			fclose(fp);
		}
	}
	else
	{
		Success = false;
		MessageBox(0, "Data\\av-code45.pak - File not found", "TxtConvertor", MB_OK | MB_ICONERROR);
	}

	return Success;
}

#ifdef KERNEL_KEY
_ModulusDecrypt ModulusDecrypt;
_ModulusDecrypt2 ModulusDecrypt2;
#endif // KERNEL_KEY


void CGMProtect::CheckPluginFile()
{
	runtime_open_module_crc32(GetExecutablePath(m_MainInfo.ReShaderName), m_MainInfo.ReShaderCRC32);


#ifdef KERNEL_KEY
	HMODULE module = LoadLibrary("Kernel24.dll");

	if (module != nullptr)
	{
		ModulusDecrypt = (_ModulusDecrypt)GetProcAddress(module, "ModulusDecrypt");;
		ModulusDecrypt2 = (_ModulusDecrypt2)GetProcAddress(module, "ModulusDecrypt2");
	}
#endif // KERNEL_KEY

	//runtime_open_module_crc32(GetExecutablePath(m_MainInfo.ReShaderName), m_MainInfo.ReShaderCRC32);
}

bool CGMProtect::IsInvExt()
{
	return m_MainInfo.InventoryExtension != 0;
}

bool CGMProtect::IsBaulExt()
{
	return m_MainInfo.WareHouseExtension != 0;
}

bool CGMProtect::shutdown_oficial_helper()
{
	return m_MainInfo.MuHelperOficial != 0;
}

bool CGMProtect::IsInGameShop()
{
	return m_MainInfo.CashShopInGame != 0;
}

bool CGMProtect::IsMasterSkill()
{
	return m_MainInfo.shutdown_master_skill != 0;
}

bool CGMProtect::CheckWideScreen()
{
	return (WindowWidth > 800 && m_MainInfo.ScreenType != 0);
}

bool CGMProtect::ChatEmoticon()
{
	return m_MainInfo.ChatEmoticon;
}

bool CGMProtect::PickItemTextType()
{
	return m_MainInfo.PickItemTextType;
}

bool CGMProtect::CastleSkillEnable()
{
	return m_MainInfo.CastleSkillEnable;
}

int CGMProtect::CharInfoBalloonType()
{
	return m_MainInfo.CharInfoBalloonType;
}

int CGMProtect::LookAndFeelType()
{
	return m_MainInfo.LookAndFeel;
}

int CGMProtect::GetAttackTimeForClass(BYTE Class)
{
	int AttackSpeedForClass = 15;

	if (m_MainInfo.MaxAttackSpeed[Class] > 0xFFFF)
	{
		switch (Class)
		{
		case Dark_Wizard:
			AttackSpeedForClass = 0x02;
			break;
		case Dark_Knight:
			AttackSpeedForClass = 0x0F;
			break;
		case Fairy_Elf:
			AttackSpeedForClass = 0x02;
			break;
		case Magic_Gladiator:
			AttackSpeedForClass = 0x02;
			break;
		case Dark_Lord:
			AttackSpeedForClass = 0x02;
			break;
		case Summoner:
			AttackSpeedForClass = 0x02;
			break;
		case Rage_Fighter:
			AttackSpeedForClass = 0x0F;
			break;
		}
	}

	return AttackSpeedForClass;
}

DWORD CGMProtect::GetAttackSpeedForClass(BYTE Class)
{
	return m_MainInfo.MaxAttackSpeed[Class];
}

void CGMProtect::LoadItemModel()
{
	for (size_t i = 0; i < CustomItem.size(); i++)
	{
		CUSTOM_ITEM_INFO* f = &CustomItem[i];

		if (f->isUsable() == false)
			return;

		f->OpenLoad();
	}
}

void CGMProtect::LoadPetsModel()
{
	for (size_t i = 0; i < CustomPetItem.size(); i++)
	{
		CUSTOM_PET_STACK* f = &CustomPetItem[i];

		if (f->isUsable() == false)
			return;

		f->OpenLoad();

		if (f->MovementType == 0)
		{
			int action[] = { PC4_SATAN, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 1)
		{
			int action[] = { XMAS_RUDOLPH, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 2)
		{
			int action[] = { PANDA, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 3)
		{
			int action[] = { UNICORN, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 4)
		{
			int action[] = { SKELETON, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 8)
		{
			int action[] = { ON_PETMUUN, 0 };
			float speed[] = { 0.5, 0 };
			ThePetProcess().REGCustomPet(f->ItemIndex, f->RenderScale, 1, action, speed);
		}
		else if (f->MovementType == 5 || f->MovementType == 6 || f->MovementType == 7 || f->MovementType == 11
			|| f->MovementType == 12
			|| f->MovementType == 13)
		{
			GoBoidREG(f->ItemIndex, f->ItemIndex + MODEL_ITEM, f->MovementType, f->RenderScale);
		}
	}
}

void CGMProtect::LoadClawModel(int itemindex, int model_L, int model_R)
{
	for (size_t i = 0; i < CustomItem.size(); i++)
	{
		CUSTOM_ITEM_INFO* f = &CustomItem[i];

		if (f->isUsable() == false)
			return;

		if (f->Itemindex == itemindex)
		{
			f->OpenBattle(model_L, true);

			f->OpenBattle(model_R, false);
			return;
		}
	}
}

void CGMProtect::LoadImageTexture()
{
	for (size_t i = 0; i < CustomImagen.size(); i++)
	{
		CUSTOM_IMAGE_INFO* f = &CustomImagen[i];

		if (f->isUsable() == false)
			return;

		f->OpenLoad();
	}
}

bool CGMProtect::GetItemColor(int itemindex, float* color)
{
	for (size_t i = 0; i < CustomItem.size(); i++)
	{
		CUSTOM_ITEM_INFO* f = &CustomItem[i];

		if (f->isUsable() == false)
			return false;

		if (f->Itemindex == itemindex)
		{
			f->NormalizeColor(color);
			return true;
		}
	}
	return false;
}

bool CGMProtect::IsPetBug(int itemindex)
{
	if (itemindex >= 0 && itemindex < MAX_ITEM)
	{
		for (size_t i = 0; i < CustomPetItem.size(); i++)
		{
			CUSTOM_PET_STACK* f = &CustomPetItem[i];

			if (f->isUsable() == false)
				break;

			if (f->ItemIndex == itemindex
				&& (f->MovementType == 5
					|| f->MovementType == 6
					|| f->MovementType == 7
					|| f->MovementType == 11))
			{
				return true;
			}
		}
	}
	return false;
}

int CGMProtect::GetPetMovement(int itemindex)
{
	if (itemindex >= 0 && itemindex < MAX_ITEM)
	{
		for (size_t i = 0; i < CustomPetItem.size(); i++)
		{
			CUSTOM_PET_STACK* f = &CustomPetItem[i];

			if (f->isUsable() == false)
				break;

			if (f->ItemIndex == itemindex)
			{
				return f->MovementType;
			}
		}
	}
	return 0;
}

bool CGMProtect::CheckItemStack(int _index, int _level)
{
	if ((_index == ITEM_POTION + 7)
		|| (_index >= ITEM_POTION && _index <= ITEM_POTION + 8 && _index != ITEM_POTION + 7)
		|| (_index >= ITEM_POTION + 38 && _index <= ITEM_POTION + 40)
		|| (_index == ITEM_BOW + 7)
		|| (_index == ITEM_BOW + 15)
		|| (_index == ITEM_POTION + 29)
		|| (_index >= ITEM_HELPER + 32 && _index <= ITEM_HELPER + 34)
		|| (_index >= ITEM_POTION + 46 && _index <= ITEM_POTION + 50)
		|| (_index == ITEM_POTION + 70)
		|| (_index == ITEM_POTION + 71)
		|| (_index == ITEM_POTION + 78)
		|| (_index == ITEM_POTION + 79)
		|| (_index == ITEM_POTION + 80)
		|| (_index == ITEM_POTION + 81)
		|| (_index == ITEM_POTION + 82)
		|| (_index == ITEM_POTION + 94)
		|| (_index == ITEM_POTION + 85)
		|| (_index == ITEM_POTION + 86)
		|| (_index == ITEM_POTION + 87)
		|| (_index == ITEM_POTION + 88)
		|| (_index == ITEM_POTION + 89)
		|| (_index == ITEM_POTION + 90)
		|| (_index == ITEM_POTION + 100)
		|| (_index == ITEM_POTION + 110)
		|| (_index == ITEM_POTION + 101)
		|| (_index == ITEM_POTION + 133))
	{
		return false;
	}

	auto it = std::find_if(CustomStacks.begin(), CustomStacks.end(),
		[_index, _level](const CUSTOM_ITEM_STACK& info) {
			return _index == info.ItemIndex && (info.Level == -1 || info.Level == _level);
		});

	return (it != CustomStacks.end());
}

CUSTOM_PET_STACK* CGMProtect::GetPetAction(int itemindex)
{
	if (itemindex >= 0 && itemindex < MAX_ITEM)
	{
		for (size_t i = 0; i < CustomPetItem.size(); i++)
		{
			CUSTOM_PET_STACK* f = &CustomPetItem[i];

			if (f->isUsable() == false)
				break;

			if (f->ItemIndex == itemindex)
			{
				return f;
			}
		}
	}
	return NULL;
}

CUSTOM_ITEM_STACK* CGMProtect::GetItemStack(int _index, int _level)
{
	auto it = std::find_if(CustomStacks.begin(), CustomStacks.end(),
		[_index, _level](const CUSTOM_ITEM_STACK& info) {
			return _index == info.ItemIndex && (info.Level == -1 || info.Level == _level);
		});

	return (it != CustomStacks.end()) ? &(*it) : NULL;
}

CUSTOM_SMOKE_STACK* CGMProtect::GetItemSmoke(int _index)
{
	if (_index >= 0 && _index < MAX_ITEM_INDEX)
	{
		auto it = std::find_if(CustomSmokeItem.begin(), CustomSmokeItem.end(),
			[_index](const CUSTOM_SMOKE_STACK& info) {
				return _index == info.ItemIndex;
			});

		return (it != CustomSmokeItem.end()) ? &(*it) : NULL;
	}

	return NULL;
}

bool CGMProtect::GetNpcName(int IndexNpc, char* Name, int PositionX, int PositionY)
{
	for (size_t i = 0; i < CustomNpcName.size(); i++)
	{
		CUSTOM_NPC_NAME* f = &CustomNpcName[i];

		if (f->isUsable() == false)
			break;

		if (f->IndexNpc != IndexNpc)
		{
			continue;
		}

		if (f->PositionX != -1 && f->PositionX != PositionX)
		{
			continue;
		}

		if (f->PositionY != -1 && f->PositionY != PositionY)
		{
			continue;
		}

		strcpy(Name, f->Name);
		return true;

	}
	return false;
}

bool areStringsEqual(const char* str1, const char* str2)
{
	return strcmp(str1, str2) == 0;
}

bool CGMProtect::IsRenderWave(const char* Name)
{
	auto it = std::find_if(CustomCharNameWave.begin(), CustomCharNameWave.end(),
		[Name](const CHARACTER_NAME_EFFECT& info) {
			return areStringsEqual(info.Name, Name);// Name == info.ItemIndex;
		});

	return (it != CustomCharNameWave.end());
}

void CGMProtect::LoadingProgressive()
{
	GMMonsterMng->LoadDataMonster(CustomMonster.data(), CustomMonster.size());

	GMItemEffect->LoadDataEffect(0, CustomEffectStatics.data(), CustomEffectStatics.size());

	GMItemEffect->LoadDataEffect(1, CustomEffectDinamic.data(), CustomEffectDinamic.size());

	GMItemEffect->LoadDataEffect(0, CustomPetEffectStatics.data(), CustomPetEffectStatics.size());

	GMItemEffect->LoadDataEffect(1, CustomPetEffectDinamic.data(), CustomPetEffectDinamic.size());

#ifdef EFFECT_KERNEL_VERTEX
	GMRenderGroupMesh->LoadData(RENDER_MESH_OBJ_ITEM, CustomItemRenderMesh.data(), CustomItemRenderMesh.size());

	GMRenderGroupMesh->LoadData(CustomDefaultRenderMesh.data(), CustomDefaultRenderMesh.size());
#endif // EFFECT_KERNEL_VERTEX

	GMJewelOfAction->LoadItemModel();

	GMPhysicsManager->LoadDataTexture();

	GMItemWings->LoadItemModel();

	this->LoadItemModel();

	this->LoadPetsModel();

	this->LoadImageTexture();

	CustomItemRenderMesh.clear();

	CustomEffectStatics.clear();

	CustomEffectDinamic.clear();

	CustomPetEffectStatics.clear();

	CustomPetEffectDinamic.clear();

	CustomDefaultRenderMesh.clear();
}

void CGMProtect::runtime_open_module_crc32(std::string FileName, DWORD _crc32)
{
	if (_crc32 == 0)
		return;

	CCRC32 CRC32;

	DWORD PluginCRC32;

	if (CRC32.FileCRC(FileName.c_str(), &PluginCRC32, 1024) == 0)
	{
		std::string text = "Plugin CRC error!: " + FileName;
		MessageBox(0, text.c_str(), "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return;
	}

	if (PluginCRC32 != _crc32)
	{
		MessageBox(0, "Plugin CRC error!", "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return;
	}

	HMODULE hModule = LoadLibrary(FileName.c_str());

	if (hModule == 0)
	{
		std::string text = "[LOADLIBRARY] Plugin No Fount error!: " + FileName;
		MessageBox(0, text.c_str(), "Error", MB_OK | MB_ICONERROR);
		ExitProcess(0);
		return;
	}

	if (this->IsWindows7OrLower() == false)
	{
		char filename[MAX_PATH] = { 0, };

		GetModulePath(hModule, filename);

		if (CRC32.FileCRC(filename, &PluginCRC32, 1024) == 0)
		{
			std::string text = "[SET_CRC] Plugin No Fount error!: ";
			text += filename;

			MessageBox(0, text.c_str(), "Error", MB_OK | MB_ICONERROR);
			ExitProcess(0);
			return;
		}

		if (PluginCRC32 != _crc32)
		{
			MessageBox(0, "[CMP_CRC] Plugin CRC error!", "Error", MB_OK | MB_ICONERROR);
			ExitProcess(0);
			return;
		}
	}

	void (*EntryProc)() = (void(*)())GetProcAddress(hModule, "EntryProc");

	if (EntryProc != 0)
	{
		EntryProc();
	}

	MODULE_RAM_LIBRARYE info;

	info.hModule = hModule;

	info._crc32 = _crc32;

	m_hLibrary.push_back(info);
}

void CGMProtect::runtime_checked_crc32()
{
	CCRC32 CRC32;

	DWORD PluginCRC32;

	for (MODULE_RAM_LIBRARYE Library : m_hLibrary)
	{
		if (Library.hModule != 0)
		{
			char filename[MAX_PATH];

			GetModulePath(Library.hModule, filename);

			if (CRC32.FileCRC(filename, &PluginCRC32, 1024) == 0)
			{
				SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
				return;
			}

			if (Library._crc32 != PluginCRC32)
			{
				SendMessage(gwinhandle->GethWnd(), WM_DESTROY, 0, 0);
				return;
			}
		}
	}
}

#ifdef MAX_INSTANCE_GAME
bool CGMProtect::runtime_create_mutex()
{
	if (this->GetMaxOfInstance() == 0)
	{
		return true;
	}

	std::string run_app = this->GetWindowName();

	for (int i = 1; i <= this->GetMaxOfInstance(); i++)
	{
		std::string run_mutex = run_app + (std::to_string(i));

		DWORD hExistingMutex = this->GetProcessIDFromMutex(run_mutex.c_str());

		if (!hExistingMutex)
		{
			m_hMutex = CreateMutex(NULL, TRUE, run_mutex.c_str());

			if (m_hMutex && GetLastError() != ERROR_ALREADY_EXISTS)
			{
				return true; // Permitimos ejecutar la aplicación
			}
		}
	}

	return false;
}

void CGMProtect::runtime_delete_mutex()
{
	if (this->GetMaxOfInstance() != 0)
	{
		CloseHandle(m_hMutex);
	}
}
DWORD CGMProtect::GetProcessIDFromMutex(const std::string& mutexName)
{
	// Intentar abrir el mutex existente
	HANDLE hExistingMutex = OpenMutex(SYNCHRONIZE, FALSE, mutexName.c_str());

	if (!hExistingMutex)
	{
		//std::cerr << "No se pudo abrir el mutex. Error: " << GetLastError() << std::endl;
		return 0;
	}

	// Obtener la lista de procesos en el sistema
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hExistingMutex);
		//std::cerr << "Error al obtener snapshot de procesos." << std::endl;
		return 0;
	}

	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	DWORD ownerPID = 0;

	if (Process32First(hSnapshot, &pe)) {
		do {
			HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, pe.th32ProcessID);
			if (hProcess) {
				HANDLE hDupMutex = NULL;
				// Intentar duplicar el Mutex en el proceso
				if (DuplicateHandle(hProcess, hExistingMutex, GetCurrentProcess(), &hDupMutex, 0, FALSE, DUPLICATE_SAME_ACCESS))
				{
					if (hDupMutex)
					{
						ownerPID = pe.th32ProcessID; // Se encontró el proceso que posee el mutex
						CloseHandle(hDupMutex);
						CloseHandle(hProcess);
						break;
					}
				}
				CloseHandle(hProcess);
			}
		} while (Process32Next(hSnapshot, &pe));
	}

	if (ownerPID == 0)
	{
		ReleaseMutex(hExistingMutex);
	}
	CloseHandle(hSnapshot);
	CloseHandle(hExistingMutex);

	return ownerPID;
}
#endif // MAX_INSTANCE_GAME

void CGMProtect::Release()
{
	CustomItem.clear();
	CustomImagen.clear();
	CustomPetItem.clear();
	CustomStacks.clear();
	CustomSmokeItem.clear();
	CustomMonster.clear();
	CustomNpcName.clear();
	CustomEffectDinamic.clear();
	CustomEffectStatics.clear();
	CustomPetEffectDinamic.clear();
	CustomPetEffectStatics.clear();
	CustomItemRenderMesh.clear();
	CustomCharRenderMesh.clear();
	CustomBuyVip.clear();
	CustomCommand.clear();
	CustomCharNameWave.clear();
}

bool CGMProtect::IsWindows7OrLower()
{
	RTL_OSVERSIONINFOW rovi = { 0 };
	rovi.dwOSVersionInfoSize = sizeof(rovi);

	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");

	if (hMod)
	{
		RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

		if (pRtlGetVersion && pRtlGetVersion(&rovi) == 0)
		{
			return rovi.dwMajorVersion < 8; // Windows 7 o menor
		}
	}
	return false; // No se pudo obtener la versión, asumimos Windows 8 o superior
}

bool CGMProtect::IsWindows11()
{
	RTL_OSVERSIONINFOW rovi = { 0 };
	rovi.dwOSVersionInfoSize = sizeof(rovi);

	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");

	if (hMod)
	{
		RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hMod, "RtlGetVersion");

		if (pRtlGetVersion && pRtlGetVersion(&rovi) == 0)
		{
			return (rovi.dwMajorVersion >= 10 && rovi.dwBuildNumber >= 22000); // Windows 11
		}
	}
	return false; // No es Windows 11
}

bool CGMProtect::IsRunningInVM()
{
	/*int cpuInfo[4] = { 0 };

	// Verificar si Hypervisor está presente
	__cpuid(cpuInfo, 1);
	if (!(cpuInfo[2] & (1 << 31))) {
		return false; // No hay hipervisor, no es una VM
	}

	// Obtener el nombre del hipervisor
	char hyperVendor[13] = { 0 };
	__cpuid(cpuInfo, 0x40000000);
	memcpy(hyperVendor, &cpuInfo[1], 4);
	memcpy(hyperVendor + 4, &cpuInfo[2], 4);
	memcpy(hyperVendor + 8, &cpuInfo[3], 4);

	std::string hvName(hyperVendor);

	if (hvName == "Microsoft Hv")
	{
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);

		int logicalProcessors = sysInfo.dwNumberOfProcessors;
		int physicalCores = cpuInfo[1] >> 16 & 0xFF; // Cantidad de núcleos físicos según CPUID

		if (logicalProcessors > physicalCores * 2)
		{
			return true; // Probablemente una VM
		}

		return false; // Windows 11 con Hyper-V pero no una VM
	}

	// Otros hipervisores
	if (hvName == "VMwareVMware" || hvName == "VBoxVBoxVBox" || hvName == "XenVMMXenVMM")
	{
		return true;
	}*/

	return false;
}
