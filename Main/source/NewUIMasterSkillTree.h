#ifndef __NEW_UI_MASTER_SKILL_TREE_H__
#define __NEW_UI_MASTER_SKILL_TREE_H__
#pragma once

#if MAIN_UPDATE > 303
#include "NewUIBase.h"
#include "ZzzTexture.h"
#include "NewUIButton.h"

#define MAX_MASTER_SKILL_DATA 512
#define MASTER_SKILL_CATEGORY 3

typedef struct _MASTER_SKILL_DATA
{
	_MASTER_SKILL_DATA() {
		Id = 0;
		ClassCode = 0;
		Group = 0;
		RequiredPoints = 0;
		MaxLevel = 0;
		ArrowDirection = 0;
		RequireSkill[0] = 0;
		RequireSkill[1] = 0;
		Skill = 0;
		DefValue = 0.f;
	}
	/*+000*/	WORD Id;
	/*+002*/	WORD ClassCode;
	/*+004*/	BYTE Group;
	/*+005*/	BYTE RequiredPoints;
	/*+006*/	BYTE MaxLevel;
	/*+007*/	BYTE ArrowDirection;
	/*+008*/	int RequireSkill[2];
	/*+016*/	int Skill;
	/*+020*/	float DefValue;
}MASTER_SKILL_DATA;

typedef struct _MASTER_SKILL_TOOLTIP
{
	_MASTER_SKILL_TOOLTIP() {
		MasterId = 0;
		ClassCode = 0;
		memset(Rank, 0, sizeof(Rank));
		memset(Info, 0, sizeof(Info));
		memset(Required, 0, sizeof(Required));
		memset(Info1, 0, sizeof(Info1));
		memset(Info2, 0, sizeof(Info2));
		memset(Info3, 0, sizeof(Info3));
		memset(Info4, 0, sizeof(Info4));
	}
	/*+000*/	DWORD MasterId;
	/*+004*/	WORD ClassCode;
	/*+006*/	char Rank[64];
	/*+070*/	char Info[256];
	/*+326*/	char Required[32];
	/*+358*/	char Info1[64];
	/*+422*/	char Info2[64];
	/*+486*/	char Info3[64];
	/*+550*/	char Info4[64];
} MASTER_SKILL_TOOLTIP;

class CSkillTreeInfo
{
public:
	CSkillTreeInfo()
	{
		this->SkillPoint = 0;
		this->SkillValue = 0.0f;
		this->SkillNextValue = 0.0f;
	};

	~CSkillTreeInfo()
	{
	};

	void SetSkillTreeInfo(BYTE point, float value, float nextvalue)
	{
		this->SkillPoint = point;
		this->SkillValue = value;
		this->SkillNextValue = nextvalue;
	}

	BYTE GetSkillPoint() { return this->SkillPoint; }
	float GetSkillValue() { return this->SkillValue; }
	float GetSkillNextValue() { return this->SkillNextValue; }
private:
	BYTE SkillPoint;
	float SkillValue;
	float SkillNextValue;
};

namespace SEASON3B
{
	class CNewUIMasterSkillTree : public CNewUIObj
	{
		typedef std::map<DWORD, CSkillTreeInfo> type_map_master_info;
		typedef std::map<DWORD, MASTER_SKILL_DATA> type_map_master_data;
		typedef std::map<DWORD, MASTER_SKILL_TOOLTIP> type_map_master_tooltip;

		enum IMAGE_LIST
		{
			IMAGE_MASTER_INTERFACE = BITMAP_INTERFACE_MASTER_BEGIN,
		};
	public:
		CNewUIMasterSkillTree();
		~CNewUIMasterSkillTree();
	private:
		/*+012*/	BITMAP_t* pBitMap[2];
		/*+020*/	POINT m_Pos;
		/*+028*/	SIZE m_Size;
		/*+036*/	DWORD ButtonX[MASTER_SKILL_CATEGORY];
		/*+048*/	DWORD ButtonY[MASTER_SKILL_CATEGORY];
		/*+060*/	CNewUIButton m_BtnExit;
		/*+232*/	int CategoryPoint[MASTER_SKILL_CATEGORY];
		/*+235*/	BYTE skillPoint[MASTER_SKILL_CATEGORY][10];
		/*+265*/	BYTE ConsumePoint;
		/*+268*/	int CurSkillID;
		/*+272*/	DWORD classCode;
		/*+276*/	POINT categoryPos[MASTER_SKILL_CATEGORY];
		/*+300*/	DWORD categoryTextIndex;
		/*+304*/	DWORD classNameTextIndex;
		/*+308*/	type_map_master_info skilltreeinfo;
		/*+340*/	type_map_master_data masterData;
		/*+372*/	type_map_master_tooltip masterToolTip;
	public:
		bool Create();
		void Release();
		void SetPos();
		void OpenMasterSkillTreeData(const char* FileName);
		void OpenMasterSkillTooltip(const char* FileName);

		void InitMasterSkillPoint();
		void SetMasterType(unsigned __int8 byClass);
		void SetMasterSkillTreeData();
		void SetMasterSkillToolTipData();
		bool SetMasterSkillTreeInfo(BYTE Index, BYTE SkillPoint, float Value, float NextValue);
		int SetDivideString(char* text, int isItemTollTip, int TextNum, int iTextColor, int iTextBold, bool isPercent);

		bool Render();
		bool Update();
		bool UpdateMouseEvent();
		bool UpdateKeyEvent();
		float GetLayerDepth();
		void EventOrderWindows(double WindowsX, double WindowsY);

		void LoadImages();
		void UnloadImages();

		void RenderText();
		void RenderIcon();
		void RenderToolTip();
		bool CheckMouse(int posx, int posy);
		bool CheckBtn();
		bool CheckAttributeArea(int group, MASTER_SKILL_DATA* info);
		bool CheckSkillPoint(WORD mLevelUpPoint, int reqPoint, int maxPoint, WORD skillPoint);
		bool CheckParentSkill(DWORD reqSkill1, DWORD reqSkill2);
		bool CheckRankPoint(BYTE group, BYTE rank, BYTE skillPoint);
		bool CheckBeforeSkill(int skill, BYTE skillPoint);
		int GetBeforeSkillID(int index);
		int GetBaseSkillID(int index);
		void SkillUpgrade(int index, char point, float value, float nextvalue);
		bool IsLearnSkill(int index);
		float GetSkillValue(int index);
		float GetSkillCumulativeValue(int index, BYTE damage);
		BYTE GetSkillLevel(int index);
		void RegisterSkillInCharacterAttribute(int index);
		void SetConsumePoint(BYTE ConsumePoint);
		void SetCurSkillID(int index);


		BYTE GetConsumePoint();
		int GetCurSkillID();
		void ClearSkillTreeInfo();
		void ClearSkillTreeData();
		void ClearSkillTooltipData();

		template <typename T>
		bool InBounds(const T& value, const T& lowerBound, const T& upperBound)
		{
			return value >= lowerBound && value <= upperBound;
		}
	};
}
#endif // MAIN_UPDATE != 303
#endif
