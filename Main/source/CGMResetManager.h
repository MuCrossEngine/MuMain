#pragma once
#define MAX_ITEM_JEWEL_KOF		10

namespace SEASON3B
{
	typedef struct
	{
		bool m_Enable;
		int box_count;
		int m_Count;
		int m_MaxCount;
		int m_MinLevel;
		DWORD m_ReqMoney;
		int m_RewardItem;
		int m_RewardPoint;
		int m_ItemIndex[MAX_ITEM_JEWEL_KOF];
		int m_ItemCount[MAX_ITEM_JEWEL_KOF];
	}RESET_MANAGER_INFO;

	typedef struct
	{
		bool m_Enable;
		int box_count;
		int m_Count;
		int m_MaxCount;
		int m_MinLevel;
		int m_MinReset;
		DWORD m_ReqMoney;
		int m_RewardItem;
		int m_RewardPoint;
		int m_ItemIndex[MAX_ITEM_JEWEL_KOF];
		int m_ItemCount[MAX_ITEM_JEWEL_KOF];
	}MASTER_RESET_MANAGER_INFO;

	class CGMResetManager
	{
	public:
		CGMResetManager();
		virtual~CGMResetManager();
		void LoadImages();
		void UnloadImages();
		void SetResetInfo(int _CurReset, int _MaxReset, int _MinLevel, int _RewardPoint, DWORD _ReqMoney, int* _ItemIndex, int* _ItemCount);
		void SetMasterResetInfo(int _MasterReset, int _MaxMasterReset, int _MinLevel, int _MinReset, int _RewardPoint, DWORD _ReqMoney, int* _ItemIndex, int* _ItemCount);

		void Render();
		bool UpdateKeyEvent();
		bool UpdateMouseEvent();
		bool ButtonProcess();
		void Render3d();
		void RenderItemNumber();
		void RenderFrame();
		void RenderButtons();
		void ClosingProcess();

		void RenderResetInfo();
		void RenderMasterResetInfo();
	private:
		BYTE _TypeUI;
		bool _Visible;
		float RenderSizeX;
		float RenderSizeY;
		float RenderFrameX;
		float RenderFrameY;
		RESET_MANAGER_INFO reset;
		MASTER_RESET_MANAGER_INFO master;
	public:
		static CGMResetManager* Instance() {
			static CGMResetManager sInstance;
			return &sInstance;
		};
	};
}

#define GMResetManager			(SEASON3B::CGMResetManager::Instance())