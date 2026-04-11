#include "stdafx.h"
#include "pugixml.hpp"
#include "Resolutions.h"

ResolutionConfig::ResolutionConfig()
{
	text_name = "800x600";
}

ResolutionConfig::ResolutionConfig(int idx, const std::string& resText, int fSize, float vFar, float vFarInc, float wFar, float wFarInc, float wNear, float wNearInc, float iScaleX, float iScaleY, float tScale, float muScale)
	: index(idx), text_name(resText), fontsize(fSize),
	view_far(vFar), view_far_increase(vFarInc),
	width_far(wFar), width_far_increase(wFarInc),
	width_near(wNear), width_near_increase(wNearInc),
	interface_scale_x(iScaleX), interface_scale_y(iScaleY),
	tvt_scale(tScale), mu_legend_scale(muScale)
{
	this->ParseResolution();
}

void ResolutionConfig::ParseResolution()
{
	size_t xPos = text_name.find('x');
	if (xPos != std::string::npos)
	{
		width = std::stoi(text_name.substr(0, xPos));       // Extrae el ancho
		height = std::stoi(text_name.substr(xPos + 1));     // Extrae la altura
	}
	else
	{
		width = 0;
		height = 0;
	}
}

CSettings::CSettings()
{
	data_default = { 0, "800x600", 13, 3200.0f, 270.0f, 1600.0f, 95.0f, 500.0f, 55.0f, 1.0f, 1.0f, 0.15f, 0.5f };
	this->Load("Data\\Resolutions.xml");
}

CSettings::~CSettings()
{
	settings.clear();
}

void CSettings::Load(const char* filename)
{
	pugi::xml_document file;
	pugi::xml_parse_result res = file.load_file(filename);

	if (res.status != pugi::status_ok)
	{
		return;
	}

	pugi::xml_node root = file.child("ResolutionsInfo");

	for (pugi::xml_node child_io = root.child("Resolution"); child_io; child_io = child_io.next_sibling())
	{
		ResolutionConfig data_info;

		data_info.index = child_io.attribute("index").as_int();
		data_info.text_name = child_io.attribute("name").as_string();
		data_info.fontsize = child_io.attribute("font_size").as_int();
		data_info.view_far = child_io.attribute("view_far").as_float();
		data_info.view_far_increase = child_io.attribute("view_far_increase").as_float();
		data_info.width_far = child_io.attribute("width_far").as_float();
		data_info.width_far_increase = child_io.attribute("width_far_increase").as_float();
		data_info.width_near = child_io.attribute("width_near").as_float();
		data_info.width_near_increase = child_io.attribute("width_near_increase").as_float();
		//data_info.interface_scale_x = child_io.attribute("index").as_float();
		//data_info.interface_scale_y = child_io.attribute("index").as_float();
		//data_info.tvt_scale = child_io.attribute("tvt_scale").as_float();
		//data_info.mu_legend_scale = child_io.attribute("mu_legend_scale").as_float();

		data_info.ParseResolution();

		settings.push_back(data_info);
	}
	fontname = root.attribute("fontname").as_string("Arial Unicode MS");
}

const char* CSettings::Getfontfamily()
{
	return fontname.c_str();
}

ResolutionConfig* CSettings::FindInfoByIndex(int idx)
{
	auto it = std::find_if(settings.begin(), settings.end(),
		[idx](const ResolutionConfig& res) {
			return res.index == idx;
		});

	if (it != settings.end())
	{
		return &(*it);
	}

	return &data_default;
}

int CSettings::FindInfoByIndex(const std::string text_name)
{
	auto it = std::find_if(settings.begin(), settings.end(),
		[text_name](const ResolutionConfig& res) {
			return res.text_name == text_name;
		});

	if (it != settings.end())
	{
		return (*it).index;
	}

	return -1;
}

void CSettings::FindResolutionByIndex(int idx, unsigned int* width, unsigned int* height, int* fontsize)
{
	ResolutionConfig* data = this->FindInfoByIndex(idx);

	if (width) *width = data->width;

	if (height) *height = data->height;

	if (fontsize) *fontsize = data->fontsize;
}
