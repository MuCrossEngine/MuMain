#pragma once

extern std::string LabelGenerator(std::string baseLabel);

class CUIComboBox
{
public:
	CUIComboBox();
	virtual~CUIComboBox();

	void Render(float RenderFrameX, float RenderFrameY, float RenderSizeX);
	void SetOptions(const std::vector<std::pair<std::string, int>>& opts);


	int GetSelectedIndex() const;
	int GetSelectedValue() const;
	std::string GetSelectedLabel() const;
private:
	int selectedIndex;
	std::string label;
	std::vector<std::pair<std::string, int>> items;
};

class CUICheckBox
{
private:
	bool value;
	std::string label;
public:
	CUICheckBox(bool initialValue = false);

	void Render(float RenderFrameX, float RenderFrameY);

	bool IsChecked() const;
	void SetChecked(bool checked);
};

extern bool render_widget_color(float RenderFrameX, float RenderFrameY, float RenderSizeX, DWORD* col);
extern void RenderLabel(float RenderFrameX, float RenderFrameY, bool bullet, ImU32 col_black, const char* fmt, ...);
extern bool RenderButton(float RenderFrameX, float RenderFrameY, const char* label, const ImVec2& size = ImVec2(0, 0));

template<typename T>
class CUInputBox
{
private:
	T value;
	std::string label;
public:
	CUInputBox(T initialValue);
	void Render(float RenderFrameX, float RenderFrameY, float RenderSizeX);
	T GetValue() const { return value; }
	void SetValue(T newValue) { value = newValue; }
};

template<typename T>
inline CUInputBox<T>::CUInputBox(T initialValue): value(initialValue)
{
	label = LabelGenerator("InputBox");
}

template<typename T>
inline void CUInputBox<T>::Render(float RenderFrameX, float RenderFrameY, float RenderSizeX)
{
	ImGui::SetCursorPos(ImVec2(RenderFrameX, RenderFrameY));
	if (RenderSizeX != 0)
	{
		ImGui::PushItemWidth(RenderSizeX);
	}

	if constexpr (std::is_same<T, int>::value)
	{
		ImGui::InputInt(label.c_str(), &value, 0, 0);
	}
	else if constexpr (std::is_same<T, float>::value)
	{
		ImGui::InputFloat(label.c_str(), &value, 0, 0);
	}
	else if constexpr (std::is_same<T, std::string>::value)
	{
		char buffer[256];
		std::strncpy(buffer, value.c_str(), sizeof(buffer));
		if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
		{
			value = buffer;
		}
	}

	if (RenderSizeX != 0)
	{
		ImGui::PopItemWidth();
	}
}
