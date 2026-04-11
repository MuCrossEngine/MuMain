#pragma once
#include "NewUIQuestProgress.h"

#define MAX_INVASION		50

typedef struct
{
	int MonsterIndex;
	int Monster_Kill;
	int MonsterCount;
} INVASION_MONSTER_INFO;

typedef struct
{
	int index;
	std::string Name;
	DWORD TimeRemaing;
	std::vector<INVASION_MONSTER_INFO> total_monster;
}INVASION_GLOBAL_INFO;

typedef std::map<int, INVASION_GLOBAL_INFO> type_map_invasion;

namespace SEASON3B
{
	class CGMInvasionManager
	{
		enum IMAGE_LIST
		{
			IMAGE_ND_BTN_L = CNewUIQuestProgress::IMAGE_QP_BTN_L,
			IMAGE_ND_BTN_R = CNewUIQuestProgress::IMAGE_QP_BTN_R,
		};
	public:
		CGMInvasionManager();
		virtual~CGMInvasionManager();

		void SetInvasion(int Index, DWORD TimeRemaing, char* Name);
		void SetMonsterKill(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount);
		void SetMonsterCount(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount);
		void SetPos(float RenderFrameX, float RenderFrameY);

		void Update();
		bool UpdateMouseEvent();
		void RenderFrame(float RenderFrameX, float RenderFrameY);

		void NextInvasion();
		void PreviousInvasion();

		INVASION_GLOBAL_INFO* FindInvasion(int Index);
	private:
		int m_CountActive;
		bool is_Opentable;
		POINT m_Pos;
		float m_RenderFrameX;
		float m_RenderFrameY;
		int currentInvasion;
		std::chrono::steady_clock::time_point last_time;
		type_map_invasion InvasionInfo;
		//INVASION_GLOBAL_INFO [MAX_INVASION];
	public:
		static CGMInvasionManager* Instance() {
			static CGMInvasionManager sInstance;
			return &sInstance;
		};
	};
}

#define GMInvasionManager			(SEASON3B::CGMInvasionManager::Instance())