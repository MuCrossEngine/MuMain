#include "stdafx.h"
#include "jpexs.h"
#include "CGMProtect.h"
#include "NewUISystem.h"
#include "wsclientinline.h"
#include "NewUICommandFilter.h"

extern void HookColorFont(int TextColor);

SEASON3B::CNewUICommandFilter::CNewUICommandFilter()
{
	m_pNewUIMng = NULL;
	m_Pos.x = 0;
	m_Pos.y = 0;
	max_view = 12;

	text_input = "";
	command_auto.clear();
}

SEASON3B::CNewUICommandFilter::~CNewUICommandFilter()
{
	Release();
}

bool SEASON3B::CNewUICommandFilter::Create(CNewUIManager* pNewUIMng, float x, float y)
{
	bool Success = false;

	if (pNewUIMng)
	{
		m_pNewUIMng = pNewUIMng;

		m_pNewUIMng->AddUIObj(INTERFACE_COMMAND_LIST, this);

		this->LoadImages();

		this->SetInfo();

		this->SetPos(x, y);

		this->Show(false);

		Success = true;
	}
	return Success;
}

void SEASON3B::CNewUICommandFilter::Release()
{
	if (m_pNewUIMng)
	{
		m_pNewUIMng->RemoveUIObj(this);

		this->UnloadImages();
	}
}

void SEASON3B::CNewUICommandFilter::SetInfo()
{
	for (size_t i = 0; i < GMProtect->CustomCommand.size(); i++)
	{
		if (GMProtect->CustomCommand[i].m_Index != -1
			&& GMProtect->CustomCommand[i].Command[0] != '\0')
		{
			command_server.push_back(GMProtect->CustomCommand[i]);
		}
	}
	m_pScrollBar.Create(m_Pos.x + 198, m_Pos.y + 51, 169);
	m_pScrollBar.SetPercent(0.0);
}

void SEASON3B::CNewUICommandFilter::SetPos(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
	m_pScrollBar.SetPos(m_Pos.x + 198, m_Pos.y + 51);
}

void SEASON3B::CNewUICommandFilter::LoadImages()
{
	LoadBitmap("Interface\\HUD\\top_back_3.tga", IMAGE_TOP_BACK3, GL_LINEAR);
}

void SEASON3B::CNewUICommandFilter::UnloadImages()
{
	DeleteBitmap(IMAGE_TOP_BACK3);
}

bool SEASON3B::CNewUICommandFilter::UpdateKeyEvent()
{
	if (IsVisible() == true)
	{
		if (SEASON3B::IsPress(VK_ESCAPE))
		{
			g_pNewUISystem->Hide(INTERFACE_COMMAND_LIST);
			return false;
		}
	}
	return true;
}

bool SEASON3B::CNewUICommandFilter::UpdateMouseEvent()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 214.f, 230.f))
	{
		if (SEASON3B::CheckMouseIn(RenderFrameX + 192.f, RenderFrameY + 3.5f, 19.f, 19.f) && SEASON3B::IsRelease(VK_LBUTTON))
		{
			g_pNewUISystem->Hide(INTERFACE_COMMAND_LIST);
			return false;
		}

		size_t good_count = command_filter.size();

		if (good_count > (size_t)max_view)
		{
			if (SEASON3B::CheckMouseIn(m_Pos.x + 10, m_Pos.y + 50, 195, 176))
			{
				double prev = m_pScrollBar.GetPercent();
				if (MouseWheel <= 0)
				{
					if (MouseWheel < 0)
					{
						MouseWheel = 0;
						prev += 0.1;
						if (prev > 1.0)
							prev = 1.0;
						m_pScrollBar.SetPercent(prev);
					}
				}
				else
				{
					MouseWheel = 0;
					prev -= 0.1;
					if (prev < 0.0)
						prev = 0.0;
					m_pScrollBar.SetPercent(prev);
				}
			}

			m_pScrollBar.UpdateMouseEvent();
		}
	}
	return !SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY, 214.f, 230.f);
}

bool SEASON3B::CNewUICommandFilter::Render()
{
	EnableAlphaTest(true);

	glColor4f(1.f, 1.f, 1.f, 1.f);

	RenderFrame();

	RenderTexte();

	DisableAlphaBlend();

	return true;
}

bool SEASON3B::CNewUICommandFilter::Update()
{
	if (IsVisible())
	{
		m_pScrollBar.Update();
	}
	return true;
}

float SEASON3B::CNewUICommandFilter::GetLayerDepth()
{
	return 10.0f;
}

void SEASON3B::CNewUICommandFilter::OpenningProcess()
{
	this->ReOrganizer();
	m_pScrollBar.SetPercent(0);
}

void SEASON3B::CNewUICommandFilter::ClosingProcess()
{
}

void SEASON3B::CNewUICommandFilter::RenderFrame()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;

	RenderImageF(IMAGE_TOP_BACK3, RenderFrameX, RenderFrameY, 214.f, 230.f, 0.0, 0.0, 854.0, 917.0); //-- background
}

void SEASON3B::CNewUICommandFilter::RenderTexte()
{
	float RenderFrameX = m_Pos.x;
	float RenderFrameY = m_Pos.y;


	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFontBold);
	g_pRenderText->SetTextColor(CLRDW_WHITE3);

	g_pRenderText->RenderFont(RenderFrameX + 12, RenderFrameY + 10, TextGlobal[28], 0, 0, RT3_SORT_LEFT);
	RenderFrameY += 32;

	g_pRenderText->SetBgColor(52, 57, 67, 255);
	g_pRenderText->RenderFont(RenderFrameX + 10, RenderFrameY, "Command", 70, 18, RT3_SORT_CENTER);
	g_pRenderText->RenderFont(RenderFrameX + 80, RenderFrameY, "Information", 124, 18, RT3_SORT_CENTER);

	RenderFrameY += 18;

	size_t secure = 0;
	size_t current = 0;
	size_t good_count = command_filter.size();

	if (good_count > (size_t)max_view)
	{
		double prev = m_pScrollBar.GetPercent();

		current = (int)((double)(unsigned int)(good_count - max_view) * prev);
	}

	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetFont(g_hFont);

	for (size_t i = current; i < good_count && secure < (size_t)max_view; i++, secure++)
	{
		HookColorFont(command_filter[i].m_bColor);

		g_pRenderText->RenderFont(RenderFrameX + 20, RenderFrameY, command_filter[i].Command, 0, 14, RT3_SORT_LEFT);
		
		g_pRenderText->RenderFont(RenderFrameX + 80, RenderFrameY, command_filter[i].Description, 110, 14, RT3_SORT_LEFT);

		RenderFrameY += 14;
	}

	if (good_count > (size_t)max_view)
	{
		m_pScrollBar.Render();
	}
}

int SEASON3B::CNewUICommandFilter::countMatches(const std::string& input, const std::string& command)
{
	int matchCount = 0;
	for (size_t i = 0; i < input.size() && i < command.size(); ++i)
	{
		if (input[i] == command[i])
		{
			++matchCount;
		}
		else
		{
			break;
		}
	}
	return matchCount;
}

std::vector<CUSTOM_COMMAND_INFO> SEASON3B::CNewUICommandFilter::autocomplete(const std::string& input)
{
	std::vector<std::pair<CUSTOM_COMMAND_INFO, int>> matchedCommands;

	// Buscar las coincidencias en los comandos
	for (const auto& cmd : command_filter)
	{
		if (cmd.m_Index != -1)
		{  // Verificar si el comando es válido
			std::string commandStr = cmd.Command; // Convertir char[] a std::string
			int matchCount = countMatches(input, commandStr);

			if (matchCount >= (int)input.size())
			{ // Solo añadir si hay alguna coincidencia
				matchedCommands.push_back({ cmd, matchCount });
			}
		}
	}

	std::vector<CUSTOM_COMMAND_INFO> result;

	if (matchedCommands.size() > 0)
	{
		// Ordenar las coincidencias por la cantidad de caracteres coincidentes (mayor a menor)
		std::sort(matchedCommands.begin(), matchedCommands.end(),
			[](const std::pair<CUSTOM_COMMAND_INFO, int>& a, const std::pair<CUSTOM_COMMAND_INFO, int>& b)
			{
				return a.second > b.second;
			});
		// Crear un vector temporal para devolver las coincidencias ordenadas
		for (const auto& pair : matchedCommands)
		{
			result.push_back(pair.first);
		}
	}
	return result;
}

std::vector<CUSTOM_COMMAND_INFO> SEASON3B::CNewUICommandFilter::filter_commands(int accountLevel, bool isAdmin)
{
	std::vector<CUSTOM_COMMAND_INFO> filteredCommands;

	for (const auto& cmd : command_server)
	{
		// Si el comando es solo para administradores, verificar si el usuario es admin
		if ((cmd.m_bGameMaster == 1 || cmd.m_bGameMaster == 2) && !isAdmin)
		{
			continue; // Saltar este comando si no es admin
		}

		// Verificar si el nivel de cuenta tiene acceso al comando
		if (cmd.m_AccountLevel[accountLevel] == 1)
		{
			filteredCommands.push_back(cmd);
		}
	}

	return filteredCommands;
}

void SEASON3B::CNewUICommandFilter::ReOrganizer()
{
	bool is_Admin = (Hero->CtlCode == CTLCODE_20OPERATOR || Hero->CtlCode == CTLCODE_08OPERATOR);

	command_filter.clear();
	command_filter = filter_commands(CharacterMachine->AccountLevel, is_Admin);

	//g_ConsoleDebug->Write(MCD_RECEIVE, "0x03 [ReceiveJoinMapServer]");
}

void SEASON3B::CNewUICommandFilter::filterClear()
{
	text_input = "";
	filter_percent = 0.0;
	command_auto.clear();
}

void SEASON3B::CNewUICommandFilter::RenderAutoComplete(float RenderFrameX, float RenderFrameY, std::string input)
{
	size_t secure = 0, current = 0;

	if (command_filter.size() == 0)
	{
		this->ReOrganizer();
	}

	if (text_input != input)
	{
		text_input = input;
		filter_percent = 0.0;
		command_auto.clear();
		command_auto = autocomplete(input);
	}

	size_t good_count = command_auto.size();

	if (good_count > 0)
	{
		float _ScreenX = RenderFrameX * g_fScreenRate_x;
		float _ScreenY = (RenderFrameY - 100) * g_fScreenRate_y;
		float RenderSizeX = (g_pChatInputBox->GetRenderWidh() * g_fScreenRate_x);
		float RenderSizeY = (100 * g_fScreenRate_y);

		if (good_count < 7)
		{
			_ScreenY = (RenderFrameY - ((good_count * 14) + 2)) * g_fScreenRate_y;
			RenderSizeY = ((good_count * 14) + 2) * g_fScreenRate_y;
		}

		if (good_count > 7)
		{
			if (SEASON3B::CheckMouseIn(RenderFrameX, RenderFrameY - 100, g_pChatInputBox->GetRenderWidh(), 100))
			{
				double prev = filter_percent;

				if (MouseWheel < 0)
				{
					if (MouseWheel < 0)
					{
						MouseWheel = 0;
						prev += 0.1;
						if (prev > 1.0)
						{
							prev = 1.0;
						}
						filter_percent = (prev);
					}
				}
				else if (MouseWheel > 0)
				{
					MouseWheel = 0;
					prev -= 0.1;
					if (prev < 0.0)
					{
						prev = 0.0;
					}
					filter_percent = (prev);
				}
			}

			current = (int)((double)(unsigned int)(good_count - 7) * filter_percent);
		}

		jpexs::RenderBoxjpexs_dds(IMAGE_HUD_PLAYER, _ScreenX, _ScreenY, RenderSizeX, RenderSizeY);

		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetTextColor(CLRDW_WHITE3);

		int text_size = (g_pChatInputBox->GetRenderWidh() / 2);

		for (size_t i = current; i < good_count && secure < 7; i++, secure++)
		{
			RenderFrameY -= 14;
			g_pRenderText->RenderFont(RenderFrameX + 5, RenderFrameY, command_auto[i].Description, text_size - 8, 14, RT3_SORT_LEFT);

			g_pRenderText->RenderFont(RenderFrameX + text_size + 5, RenderFrameY, command_auto[i].Information, text_size - 4, 14, RT3_SORT_LEFT);
		}
	}
}


void SEASON3B::CNewUICommandFilter::SendCommand(int index, const char* lpszFormat, ...)
{
	if (index >= 0 && index < MAX_CUSTOM_COMMAND)
	{
		char lpszBuffer[256] = { 0, };
		va_list va;
		va_start(va, lpszFormat);
		vsnprintf(lpszBuffer, sizeof(lpszBuffer), lpszFormat, va);
		va_end(va);


		char lpszCommand[256];
		memset(lpszCommand, sizeof(lpszCommand), 0);

		sprintf_s(lpszCommand, "%s ", command_server[index].Command);

		strncat_s(lpszCommand, sizeof(lpszCommand) - strlen(lpszCommand) - 1, lpszBuffer, _TRUNCATE);

		if (strlen(lpszCommand))
		{
			SendChat(lpszCommand);
		}
	}
}
