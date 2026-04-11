#pragma once
#define MaxLineItemToolTip						3000

#define MAX_CUSTOM_MAPE						150
#define MAX_CUSTOM_ITEM						2048
#define MAX_CUSTOM_IMAGE					512
#define MAX_CUSTOM_MONSTER					512
#define MAX_CUSTOM_SMOKE					512
#define MAX_CUSTOM_NPC_NAME					100
#define MAX_CUSTOM_WINGS					256
#define MAX_CUSTOM_STACK					256
#define MAX_CUSTOM_PET						256
#define MAX_CUSTOM_EFFECT					5000
#define MAX_CUSTOM_COMMAND					100
#define MAX_CHARACTER_NAME					100


class CUSTOM_ITEM_INFO
{
public:
	int Itemindex;
	DWORD RenderColor;
	char filename[32];
public:
	CUSTOM_ITEM_INFO(){
		release();
	};
	float red()
	{
		return ((RenderColor) & 0xff) / 255.f;
	};
	float green()
	{
		return (((RenderColor) >> 8) & 0xff) / 255.f;
	};
	float blue()
	{
		return (((RenderColor) >> 16) & 0xff) / 255.f;
	};
	void NormalizeColor(vec3_t vOut)
	{
		if (vOut)
		{
			vOut[0] = this->red();
			vOut[1] = this->green();
			vOut[2] = this->blue();
		}
	};
	void release();
	bool isUsable();
	void OpenLoad();
	void OpenBattle(int modelId, bool Left);
};

class CUSTOM_MONSTER_INFO
{
public:
	int monsterIndex;
	int RenderIndex;
	int Kind;
	float fSize;
	char Name[32];
	char directory[64];
	char filename[32];
public:
	CUSTOM_MONSTER_INFO() {
		release();
	};
	void release();
	bool isUsable();
	void OpenLoad();
};

class CUSTOM_NPC_NAME
{
public:
	int IndexNpc;
	int PositionX;
	int PositionY;
	char Name[32];
	int WorldIndex;
public:
	CUSTOM_NPC_NAME() {
		release();
	};
	void release();
	bool isUsable();
};

class CUSTOM_MAPE_INFO
{
public:
	int mapIndex;
	int checkIndex;
	int textIndex;
	int TerrainExt;
	char ImgName[32];
	char lpszMp3[32];
public:
	CUSTOM_MAPE_INFO() {
		release();
	};
	void release();
	bool isUsable();
};

class CUSTOM_IMAGE_INFO
{
public:
	int ImgIndex;
	char ImgName[32];
	int Wrap;
	int Type;
public:
	CUSTOM_IMAGE_INFO() {
		release();
	};
	void release();
	bool isUsable();
	void OpenLoad();
};

class CUSTOM_WING_INFO
{
public:
	int ItemIndex;
	int DefenseConstA;
	int IncDamageConstA;
	int IncDamageConstB;
	int DecDamageConstA;
	int DecDamageConstB;
	int OptionIndex1;
	int OptionValue1;
	int OptionIndex2;
	int OptionValue2;
	int OptionIndex3;
	int OptionValue3;
	int NewOptionIndex1;
	int NewOptionValue1;
	int NewOptionIndex2;
	int NewOptionValue2;
	int NewOptionIndex3;
	int NewOptionValue3;
	int NewOptionIndex4;
	int NewOptionValue4;
	int ModelLinkType;
	char filename[32];
public:
	CUSTOM_WING_INFO() {
		release();
	};
	void release();
	bool isUsable();
	void OpenLoad();
};

class CUSTOM_ITEM_STACK
{
public:
	int ItemIndex;
	int Level;
	int MaxStack;
	int ItemConvert;
	int break_up;
public:
	CUSTOM_ITEM_STACK() {
		release();
	};
	void release();
	bool isUsable();
};

class CUSTOM_SMOKE_STACK
{
public:
	int ItemIndex;
	DWORD EffectColor;
public:
	CUSTOM_SMOKE_STACK() {
		release();
	};
	void release();
	bool isUsable();

	float red()
	{
		return ((EffectColor) & 0xff) / 255.f;
	};
	float green()
	{
		return (((EffectColor) >> 8) & 0xff) / 255.f;
	};
	float blue()
	{
		return (((EffectColor) >> 16) & 0xff) / 255.f;
	};
	void NormalizeColor(vec3_t vOut)
	{
		if (vOut)
		{
			vOut[0] = this->red();
			vOut[1] = this->green();
			vOut[2] = this->blue();
		}
	};
};

class CUSTOM_PET_STACK
{
public:
	int ItemIndex;
	int RenderType;
	int MovementType;
	int RenderEffect;
	float RenderScale;
	char filename[32];
public:
	CUSTOM_PET_STACK() {
		release();
	};
	void release();
	bool isUsable();
	void OpenLoad();
};

class CUSTOM_EFFECT_STACK
{
public:
	int ItemIndex;
	int Texture;
	int iBoneNumber;
	int SubType;
	float Scale;
	float Angle;
	DWORD EffectColor;
public:
	CUSTOM_EFFECT_STACK()
	{
		release();
	};
	void release();
	bool isUsable();

	float red()
	{
		return ((EffectColor) & 0xff) / 255.f;
	};
	float green()
	{
		return (((EffectColor) >> 8) & 0xff) / 255.f;
	};
	float blue()
	{
		return (((EffectColor) >> 16) & 0xff) / 255.f;
	};
	void NormalizeColor(vec3_t vOut)
	{
		if (vOut)
		{
			vOut[0] = this->red();
			vOut[1] = this->green();
			vOut[2] = this->blue();
		}
	};
};

struct TEMPLATE_MEMBERSHIP
{
	int Index;
	int CntExp;
	int CntDrop;
	int CntDays;
	int AccountLevel;
	int Coin[3];
	char text_name[32];
};

struct CUSTOM_COMMAND_INFO
{
	int m_Index;
	BYTE m_bColor;
	BYTE m_AccountLevel[4];
	BYTE m_bGameMaster;
	char Command[32];
	char Description[100];
	char Information[100];

	void release() {
		m_Index = -1;
	}
};

struct CHARACTER_NAME_EFFECT
{
	int index;
	char Name[32];

	void release()
	{
		index = -1;
	}
};

class RENDER_MESH_EFFECT
{
public:
	RENDER_MESH_EFFECT() {
		this->release();
	};

	int Index;
	int GroupId; //-- meshId
	int RenderFlag;
	int Texture;
	DWORD BodyLight;
	int RenderType;
	float BlendLight;
	float TimeEffect;
	float TimeTexture;
	BOOL RenderLevel;

	bool operator==(const RENDER_MESH_EFFECT& other) const {
		return (Index == other.Index
			&& GroupId == other.GroupId
			&& RenderFlag == other.RenderFlag
			&& Texture == other.Texture
			&& BodyLight == other.BodyLight
			&& RenderType == other.RenderType
			&& BlendLight == other.BlendLight
			&& TimeEffect == other.TimeEffect
			&& TimeTexture == other.TimeTexture
			&& RenderLevel == other.RenderLevel);
	}
public:
	void release() {
		Index = -1;
	};

	float red() {
		return ((BodyLight) & 0xff) / 255.f;
	};

	float green() {
		return (((BodyLight) >> 8) & 0xff) / 255.f;
	};

	float blue() {
		return (((BodyLight) >> 16) & 0xff) / 255.f;
	};

	void SetCLRREFF(float* rgb)
	{
		BYTE r = static_cast<BYTE>(rgb[0] * 255.f);

		BYTE g = static_cast<BYTE>(rgb[1] * 255.f);

		BYTE b = static_cast<BYTE>(rgb[2] * 255.f);

		BodyLight = (DWORD)((255 << 24) | (r) | (g << 8) | (b << 16));
	}

	void NormalizeColor(vec3_t vOut)
	{
		if (vOut && BodyLight != 0)
		{
			vOut[0] = this->red();
			vOut[1] = this->green();
			vOut[2] = this->blue();
		}
	};
};

class RENDER_MESH_EFFECT_DEFAULT
{
public:
	int ItemIndex;
	int EffectIndex;
public:
	RENDER_MESH_EFFECT_DEFAULT() {
		release();
	}
	void release() {
		ItemIndex = -1;
	};
};


class CUSTOM_JEWEL_INFO
{
public:
	int ItemIndex;
	int MinLevel;
	int MaxLevel;
	int MaxOption1;
	int MaxOption2;
	int MinOption3;
	int MaxOption3;
	int MinNewOption;
	int MaxNewOption;
	int MaxSetOption;
	int MinSocketOption;
	int MaxSocketOption;
	int EnableSlotWeapon;
	int EnableSlotArmor;
	int EnableSlotWing;
	int SuccessRate[4];
	int SalePrice;
	char filename[32];
	//release();
};

class WINGS_PHYSICS_INFO
{
public:
	int ItemIndex;
	BYTE RenderType;
	BYTE iStrip;
	BYTE iBoneLayer;
	BYTE iBoneStrip;
	WORD textureIndex[4];
	char textureLayer[32];
	char textureStrip[32];
	char textureLayer_R[32];
	char textureStrip_R[32];
public:
	WINGS_PHYSICS_INFO() {
		release();
	};

	void release() {
		ItemIndex = -1;
		textureIndex[0] = 0;
		textureIndex[1] = 0;
		textureIndex[2] = 0;
		textureIndex[3] = 0;
		memset(textureLayer, 0, sizeof(textureLayer));
		memset(textureStrip, 0, sizeof(textureStrip));
		memset(textureLayer_R, 0, sizeof(textureLayer_R));
		memset(textureStrip_R, 0, sizeof(textureStrip_R));
	};
};