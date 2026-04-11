#include "stdafx.h"
#include "jpexs.h"
#include "UIBaseDef.h"
#include "UIControls.h"
#include "CGMInvasionManager.h"
#include "NewUISystem.h"

extern int FontHeight;
extern char* getMonsterName(int type);

SEASON3B::CGMInvasionManager::CGMInvasionManager()
{
	m_CountActive = 0;
	is_Opentable = false;
	currentInvasion = -1;
	last_time = std::chrono::steady_clock::now();

	m_Pos.x = 0;
	m_Pos.y = 0;
	m_RenderFrameX = 0;
	m_RenderFrameY = 0;

	InvasionInfo.clear();
}

SEASON3B::CGMInvasionManager::~CGMInvasionManager()
{
	for (type_map_invasion::iterator it = InvasionInfo.begin(); it != InvasionInfo.end(); ++it)
	{
		it->second.total_monster.clear();
	}

	InvasionInfo.clear();
}

void SEASON3B::CGMInvasionManager::SetInvasion(int Index, DWORD TimeRemaing, char* Name)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		currentInvasion = Index;
		INVASION_GLOBAL_INFO& invasion = InvasionInfo[Index];

		// Asignar la nueva información
		invasion.index = Index;
		invasion.Name = Name; // Si 'Name' es un char*, usa std::string para asignar directamente
		invasion.TimeRemaing = TimeRemaing;
		// Limpiar los monstruos previos si los hubiera
		invasion.total_monster.clear();
	}
}

void SEASON3B::CGMInvasionManager::SetMonsterKill(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		INVASION_GLOBAL_INFO* invasion = FindInvasion(Index);

		if (invasion != NULL)
		{
			size_t length = invasion->total_monster.size();

			for (size_t i = 0; i < length; i++)
			{
				INVASION_MONSTER_INFO* info = &invasion->total_monster[i];

				if (info->MonsterIndex == MonsterIndex)
				{
					info->Monster_Kill = Monster_Kill;

					info->MonsterCount = MonsterCount;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::SetMonsterCount(int Index, int MonsterIndex, int Monster_Kill, int MonsterCount)
{
	if (Index != -1 && Index >= 0 && Index < MAX_INVASION)
	{
		INVASION_GLOBAL_INFO* invasion = FindInvasion(Index);

		if (invasion != NULL)
		{
			invasion->total_monster.push_back({ MonsterIndex, Monster_Kill, MonsterCount });
		}
	}
}

void SEASON3B::CGMInvasionManager::Update()
{
	auto current_time = std::chrono::steady_clock::now();
	double difTime = std::chrono::duration<double>(current_time - last_time).count();

	if (difTime >= 1.0)
	{
		m_CountActive = 0;
		last_time = current_time;

		for (type_map_invasion::iterator it = InvasionInfo.begin(); it != InvasionInfo.end(); ++it)
		{
			INVASION_GLOBAL_INFO* invasion = &it->second;

			if (invasion->TimeRemaing > 0)
			{
				m_CountActive++;
				invasion->TimeRemaing -= 1;

				if (currentInvasion == -1)
				{
					currentInvasion = it->first;
				}
			}
			else
			{
				if (currentInvasion == it->first)
				{
					currentInvasion = -1;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::SetPos(float RenderFrameX, float RenderFrameY)
{
	float RenderSizeX = 160.0;

	m_RenderFrameX = RenderFrameX;

	m_RenderFrameY = RenderFrameY;

	m_RenderFrameX -= (RenderSizeX / 2.f);
}

bool SEASON3B::CGMInvasionManager::UpdateMouseEvent()
{
	float RenderSizeX = 160.0;
	float RenderSizeY = 160.0;

	RenderSizeY = ((FontHeight + 2) * 2) / g_fScreenRate_y;

	if ((m_RenderFrameX + RenderSizeX) > GetScreenWidth()
		|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
		|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
		|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
		)
	{
		return false;
	}

	if (SEASON3B::CheckMouseIn(m_RenderFrameX, m_RenderFrameY, RenderSizeX, RenderSizeY))
	{
		if (SEASON3B::IsRelease(VK_LBUTTON))
		{
			if (m_CountActive > 0 && SEASON3B::CheckMouseIn(m_RenderFrameX + 3.0, m_RenderFrameY + (RenderSizeY - 18.f) / 2.f, 17.f, 18.f))
			{
				this->PreviousInvasion();
			}
			else if (m_CountActive > 0 && SEASON3B::CheckMouseIn(m_RenderFrameX + RenderSizeX - 20.0, m_RenderFrameY + (RenderSizeY - 18.f) / 2.f, 17.f, 18.f))
			{
				this->NextInvasion();
			}
			else
			{
				is_Opentable = !is_Opentable;
			}
		}
		return true;
	}

	return false;
}

void SEASON3B::CGMInvasionManager::RenderFrame(float RenderFrameX, float RenderFrameY)
{
	float RenderSizeX = 160.0;
	float RenderSizeY = 160.0;
	float _ScreenX = 0.f, _ScreenY = 0.f;

	if ((m_RenderFrameX + RenderSizeX) > GetScreenWidth()
		|| g_pNewUISystem->IsVisible(INTERFACE_MINI_MAP)
		|| g_pNewUISystem->IsVisible(INTERFACE_INGAMESHOP)
		|| g_pNewUISystem->IsVisible(INTERFACE_MASTER_LEVEL)
		)
	{
		return;
	}

	if (m_CountActive > 0)
	{
		INVASION_GLOBAL_INFO* Invasion = this->FindInvasion(currentInvasion);

		_ScreenX = (m_RenderFrameX * g_fScreenRate_x);
		_ScreenY = (m_RenderFrameY * g_fScreenRate_y);

		RenderSizeY = ((FontHeight + 2) * 2);

		jpexs::RenderBoxjpexs_dds(IMAGE_HUD_PLAYER, _ScreenX, _ScreenY, RenderSizeX * g_fScreenRate_x, RenderSizeY);

		g_pRenderText->SetFont(g_hFontBold);

		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetTextColor(CLRDW_YELLOW);

		if (Invasion != NULL && Invasion->Name[0] != '\0')
		{
			g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, Invasion->Name.c_str(), (RenderSizeX * g_fScreenRate_x), RenderSizeY, RT3_SORT_CENTER);
		}
		else
		{
			g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, "Invasion", (RenderSizeX * g_fScreenRate_x), RenderSizeY, RT3_SORT_CENTER);
		}

		if (m_CountActive > 0)
		{
			RenderlookFetch(IMAGE_ND_BTN_L, m_RenderFrameX + 3.0, m_RenderFrameY + (RenderSizeY / g_fScreenRate_y - 18.f) / 2.f, true);

			RenderlookFetch(IMAGE_ND_BTN_R, m_RenderFrameX + RenderSizeX - 20.0, m_RenderFrameY + (RenderSizeY / g_fScreenRate_y - 18.f) / 2.f, true);
		}

		if (is_Opentable)
		{
			if (Invasion && Invasion->TimeRemaing > 0)
			{
				char pszText[100];
				size_t length = Invasion->total_monster.size();

				int totalseconds = Invasion->TimeRemaing;
				int hours = totalseconds / 3600;
				int minutes = (totalseconds / 60) % 60;
				int seconds = totalseconds % 60;

				sprintf_s(pszText, "Time: %02d:%02d:%02d", hours, minutes, seconds);

				_ScreenY = RenderSizeY + 8 * g_fScreenRate_y;

				length += 2;
				RenderSizeY = ((FontHeight + 3) * length);
				length -= 2;

				jpexs::RenderBoxjpexs_dds(IMAGE_HUD_PLAYER, _ScreenX, _ScreenY, (RenderSizeX * g_fScreenRate_x), RenderSizeY);

				RenderSizeY = (FontHeight * 2);

				g_pRenderText->SetTextColor(CLRDW_GOLD);
				g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, pszText, (RenderSizeX * g_fScreenRate_x), RenderSizeY, RT3_SORT_CENTER);

				_ScreenY += RenderSizeY;

				g_pRenderText->SetFont(g_hFont);

				g_pRenderText->SetTextColor(CLRDW_WHITE3);

				RenderSizeY = (FontHeight + 2);

				_ScreenX += 6 * g_fScreenRate_x;

				for (size_t i = 0; i < length; i++)
				{
					INVASION_MONSTER_INFO* Data = &Invasion->total_monster[i];

					sprintf_s(pszText, "- %s (%d / %d)", getMonsterName(Data->MonsterIndex), Data->Monster_Kill, Data->MonsterCount);

					g_pRenderText->RenderTextClipped(_ScreenX, _ScreenY, pszText, 0, RenderSizeY, RT3_SORT_LEFT);
					_ScreenY += RenderSizeY;
				}
			}
		}
	}
}

void SEASON3B::CGMInvasionManager::NextInvasion()
{
	// Buscar el iterador que apunta a la clave actual
	auto it = InvasionInfo.find(currentInvasion);

LABEL_REPEAT:
	// Si no existe o ya está al final, no hay siguiente
	if (it == InvasionInfo.end() || std::next(it) == InvasionInfo.end())
	{
		return; // Retornar si no hay siguiente clave
	}

	// Avanzar al siguiente iterador
	++it;

	// Devolver la siguiente clave
	if (it->second.TimeRemaing == 0)
	{
		goto LABEL_REPEAT;
	}

	currentInvasion = it->first;
}

void SEASON3B::CGMInvasionManager::PreviousInvasion()
{
	// Buscar el iterador que apunta a la clave actual
	auto it = InvasionInfo.find(currentInvasion);

LABEL_REPEAT:
	// Si no existe o ya es el primero, no hay clave anterior
	if (it == InvasionInfo.end() || it == InvasionInfo.begin())
	{
		return; // Retornar si no hay clave anterior
	}

	// Retroceder al iterador anterior
	--it;

	if (it->second.TimeRemaing == 0)
	{
		goto LABEL_REPEAT;
	}

	currentInvasion = it->first;
}

INVASION_GLOBAL_INFO* SEASON3B::CGMInvasionManager::FindInvasion(int Index)
{
	// Buscar en el map usando find
	auto it = InvasionInfo.find(Index);

	// Verificar si se encontró el elemento
	if (it != InvasionInfo.end())
	{
		// Devolver un puntero al valor asociado (INVASION_GLOBAL_INFO)
		return &(it->second);
	}

	return NULL;
}