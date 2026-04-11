#pragma once
#include "supportingfeature.h"
#include "ItemTooltip.h"
#define MIN_PORT				55901
#define MAX_PORT				56000
#define SIZE_PROTOCOLVERSION	( 5)
#define SIZE_PROTOCOLSERIAL		( 16)
#define PORT_RANGE(x)			(((x)<MIN_PORT)?0:((x)>MAX_PORT)?0:1)

#ifdef KERNEL_KEY

typedef int (*_ModulusDecrypt)(BYTE*, BYTE*, unsigned int);
typedef int (*_ModulusDecrypt2)(std::vector<BYTE>&);
#endif // KERNEL_KEY

typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);


typedef struct
{
	char CustomerName[32];
	char IpAddress[32];
	WORD sin_port;
	char ClientVersion[8];
	char ClientSerial[17];
	char WindowName[32];
	char ScreenShotPath[50];
}MAIN_INFO_ENV;

typedef struct 
{
	DWORD m_MaxInstance;
	DWORD AntiHackCRC32;
	char AntiHackName[32];
	DWORD ReShaderCRC32;
	char ReShaderName[32];
	UINT HelperActiveAlert;
	UINT HelperActiveLevel;
	UINT MaxAttackSpeed[MAX_CLASS];
	UINT ReconnectTime;
	BYTE RemoveClass;
	BYTE ScreenType;
	BYTE LookAndFeel;
	BYTE ChatEmoticon;
	BYTE PickItemTextType;
	BYTE CastleSkillEnable;
	BYTE CharInfoBalloonType;
	BYTE InventoryExtension;
	BYTE MaxInventoryExtension;
	bool ServerNameDefault;
	bool WareHouseExtension;
	bool CashShopInGame;
	bool MuHelperOficial;
	bool shutdown_master_skill;
	bool InvasionInfoTime;
	bool shutdown_tooltipold;
	bool m_RenderItemNameDrop;
	bool m_RenderCharacterName;
	bool shutdown_item_earring;
	bool shutdown_item_core;
	bool shutdown_item_flag;
	bool shutdown_item_muun;
	bool shutdown_item_eagle;
	//------------------------------------
	BYTE SceneLogin;
	BYTE SceneCharacter;
	BYTE m_RenderMuLogo;
	BYTE RankUserSwitch;
	BYTE PartyHpBarType;
	bool RenderGuildLogo;
	bool RankUserShowOverHead;
	bool RankUserOnlyOnSafeZone;
	bool RankUserShowReset;
	bool RankUserShowMasterReset;
	bool RankUserNeedAltKey;
	bool MenuSwitch;
	BYTE MenuTypeButton;
	bool MenuButtonVipShop;
	bool MenuButtonRankTop;
	bool MenuButtonCommand;
	bool MenuButtonOptions;
	bool MenuButtonEventTime;
	bool WindowsVipShop;
	bool WindowsRankTop;
	bool WindowsCommand;
	bool WindowsEventTime;
	bool WindowsJewelInventory;
	bool WindowsResetInfo;
	BYTE SwitchPersonalShopType;
	bool CustomOffStoreEnable;
	bool CustomStoreEnableJoB;
	bool CustomStoreEnableJoS;
	bool CustomStoreEnableJoC;
	bool CustomStoreEnableCoin1;
	bool CustomStoreEnableCoin2;
	bool CustomStoreEnableCoin3;
	bool remember_account;
	bool trade_system_pro;
	BYTE shutdown_popup;
	BYTE ajust_fps_render;
	BYTE max_item_level_on;
#ifdef SHUTDOWN_LEVEL_WING3
	BYTE shutdown_level_wing3;
#endif // SHUTDOWN_LEVEL_WING3



	bool checkold_school() {
		return this->LookAndFeel == 1 || this->LookAndFeel == 2;
	};
	bool checknew_school() {
		return this->LookAndFeel == 3 || this->LookAndFeel == 4;
	};
} MAIN_FILE_INFO;
struct TEXT_FILE_INFO
{
	nInformation m_TRSTooltipData[MaxLineItemToolTip];
	nInformation m_TRSTooltipSetData[MaxLineItemToolTip];
	nText m_TRSTooltipText[MaxLineItemToolTip];
};
typedef struct
{
	HMODULE hModule;
	DWORD _crc32;
} MODULE_RAM_LIBRARYE;

class CGMProtect
{
public:
	CGMProtect();
	virtual~CGMProtect();
	static CGMProtect* Instance();

	WORD sin_port();
#ifdef MAX_INSTANCE_GAME
	WORD GetMaxOfInstance();
#endif // MAX_INSTANCE_GAME

	char* GetWindowName();
	char* GetScreenPath();
	bool CreateKeyEnv();
	void HookComplete();
	bool ReadMainFile();
	bool ReadMainConnect(std::string filename, void* pEnvStruct, int Size);
	bool LoadMainFileInfo(MAIN_FILE_INFO& mainInfo, std::string& filePath);
	bool ReadTextFile(char* name);

	void CheckPluginFile();

	bool IsInvExt();
	bool IsBaulExt();
	bool shutdown_oficial_helper();
	bool IsInGameShop();
	bool IsMasterSkill();
	bool CheckWideScreen();

	bool ChatEmoticon();
	bool PickItemTextType();
	bool CastleSkillEnable();

	int CharInfoBalloonType();
	int LookAndFeelType();

	int GetAttackTimeForClass(BYTE Class);
	DWORD GetAttackSpeedForClass(BYTE Class);

	void LoadItemModel();
	void LoadPetsModel();
	void LoadClawModel(int itemindex, int model_L, int model_R);
	void LoadImageTexture();
	bool GetItemColor(int itemindex, float* color);

	bool IsPetBug(int itemindex);
	int GetPetMovement(int itemindex);

	bool CheckItemStack(int _index, int _level);
	CUSTOM_PET_STACK* GetPetAction(int _index);
	CUSTOM_ITEM_STACK* GetItemStack(int _index, int _level);
	CUSTOM_SMOKE_STACK* GetItemSmoke(int _index);
	bool GetNpcName(int IndexNpc, char* Name, int PositionX, int PositionY);
	bool IsRenderWave(const char* Name);

	void LoadingProgressive();
	void runtime_checked_crc32();
	void runtime_open_module_crc32(std::string FileName, DWORD _crc32);
#ifdef MAX_INSTANCE_GAME
	bool runtime_create_mutex();
	void runtime_delete_mutex();
	DWORD GetProcessIDFromMutex(const std::string& mutexName);
#endif // MAX_INSTANCE_GAME

	void Release();
	bool IsWindows7OrLower();
	bool IsWindows11();
	bool IsRunningInVM();
protected:
#ifdef MAX_INSTANCE_GAME
	HANDLE m_hMutex;
#endif // MAX_INSTANCE_GAME

	MAIN_INFO_ENV m_Kernel;
public:
	BYTE EncDecKey[2];
	MAIN_FILE_INFO m_MainInfo;
	TEXT_FILE_INFO m_TextInfo;
	std::vector<MODULE_RAM_LIBRARYE> m_hLibrary;
	//------------------------------------
	std::vector<CUSTOM_ITEM_INFO> CustomItem;
	std::vector<CUSTOM_IMAGE_INFO> CustomImagen;
	std::vector<CUSTOM_PET_STACK> CustomPetItem;
	std::vector<CUSTOM_ITEM_STACK> CustomStacks;
	std::vector<CUSTOM_SMOKE_STACK> CustomSmokeItem;
	std::vector<CUSTOM_MONSTER_INFO> CustomMonster;
	std::vector<CUSTOM_NPC_NAME> CustomNpcName;
	std::vector<CUSTOM_EFFECT_STACK> CustomEffectDinamic;
	std::vector<CUSTOM_EFFECT_STACK> CustomEffectStatics;
	std::vector<CUSTOM_EFFECT_STACK> CustomPetEffectDinamic;
	std::vector<CUSTOM_EFFECT_STACK> CustomPetEffectStatics;
	std::vector<RENDER_MESH_EFFECT> CustomItemRenderMesh;
	std::vector<RENDER_MESH_EFFECT> CustomCharRenderMesh;
	std::vector<TEMPLATE_MEMBERSHIP> CustomBuyVip;
	std::vector<CUSTOM_COMMAND_INFO> CustomCommand;
	std::vector<CHARACTER_NAME_EFFECT> CustomCharNameWave;
	std::vector<RENDER_MESH_EFFECT_DEFAULT> CustomDefaultRenderMesh;
	
};

#ifdef KERNEL_KEY
extern _ModulusDecrypt ModulusDecrypt;
extern _ModulusDecrypt2 ModulusDecrypt2;
#endif // KERNEL_KEY


#define GMProtect				(CGMProtect::Instance())
#define gmProtect				(&(CGMProtect::Instance()->m_MainInfo))
