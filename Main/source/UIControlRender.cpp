#include "stdafx.h"
#include "UIControlRender.h"

CUIComboBox::CUIComboBox()
{
	selectedIndex = -1;
	label = LabelGenerator("combobox");
}

CUIComboBox::~CUIComboBox()
{
	items.clear();
}

void CUIComboBox::Render(float RenderFrameX, float RenderFrameY, float RenderSizeX)
{
	ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));

	if (items.empty())
	{
		ImGui::Text("No options available");
		return;
	}

	ImGui::PushItemWidth(RenderSizeX);
	if (ImGui::BeginCombo(label.c_str(), items[selectedIndex].first.c_str()))
	{
		for (size_t i = 0; i < items.size(); ++i)
		{
			bool isSelected = (selectedIndex == i);

			if (ImGui::Selectable(items[i].first.c_str(), isSelected))
				selectedIndex = i;

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();
}

void CUIComboBox::SetOptions(const std::vector<std::pair<std::string, int>>& opts)
{
	items = opts;

	if (selectedIndex < 0 && !items.empty())
	{
		selectedIndex = 0;
	}
}

int CUIComboBox::GetSelectedIndex() const
{
	return selectedIndex;
}

int CUIComboBox::GetSelectedValue() const
{
	if (items.empty())
		return 0;
	else
		return items[selectedIndex].second;
}

std::string CUIComboBox::GetSelectedLabel() const
{
	return items[selectedIndex].first;
}

std::string LabelGenerator(std::string baseLabel)
{
	static int counter_label = 0;

	std::ostringstream oss;
	oss << "##" << baseLabel << counter_label++;

	return oss.str();
}

CUICheckBox::CUICheckBox(bool initialValue) : value(initialValue)
{
	label = LabelGenerator("CheckBox");
}

void CUICheckBox::Render(float RenderFrameX, float RenderFrameY)
{
	ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));
	ImGui::Checkbox(label.c_str(), &value);
}

bool CUICheckBox::IsChecked() const
{
	return value;
}

void CUICheckBox::SetChecked(bool checked)
{
	value = checked;
}

bool render_widget_color(float RenderFrameX, float RenderFrameY, float RenderSizeX, DWORD* col)
{
	ImGui::SetCursorPos(ImVec2(RenderFrameX * g_fScreenRate_x, RenderFrameY * g_fScreenRate_y));
	if(RenderSizeX != 0)
		ImGui::PushItemWidth(RenderSizeX * g_fScreenRate_x);
	return ImGui::ColorEdit3("##", col, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerMask_);
}

void RenderLabel(float RenderFrameX, float RenderFrameY, bool bullet, ImU32 col_black, const char* fmt, ...)
{
	if (bullet)
	{
		ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));
		ImGui::Bullet();

		RenderFrameX += 20.f;
		//RenderFrameY += 5.f;
	}

	ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));

	if (col_black != 0)
	{
		char buffer[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buffer, sizeof(buffer), fmt, args);
		va_end(args);

		// Obtener la posición y calcular el tamaño del texto
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 textSize = ImGui::CalcTextSize(buffer);

		ImGui::GetWindowDrawList()->AddRectFilled(
			ImVec2(pos.x - 2, pos.y - 2),
			ImVec2(pos.x + textSize.x + 2, pos.y + textSize.y + 2),
			col_black);

		ImGui::Text(buffer);
	}
	else
	{
		va_list args;
		va_start(args, fmt);
		ImGui::TextV(fmt, args);  // Usa TextV para manejo de argumentos variables
		va_end(args);
	}
}

bool RenderButton(float RenderFrameX, float RenderFrameY, const char* label, const ImVec2& size)
{
	ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));
	return ImGui::Button(label, size);  // Devuelve true si el botón fue presionado
}

extern void RenderInputNumber(float RenderFrameX, float RenderFrameY, const char* label, DWORD* value, int width = 0, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);
