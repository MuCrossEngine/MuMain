#pragma once
class ResolutionConfig
{
public:
	int index;
	std::string text_name;  // "800x600"
	int width;
	int height;
	int fontsize;
	float view_far;
	float view_far_increase;
	float width_far;
	float width_far_increase;
	float width_near;
	float width_near_increase;
	float interface_scale_x;
	float interface_scale_y;
	float tvt_scale;
	float mu_legend_scale;

	ResolutionConfig();
	ResolutionConfig(int idx, const std::string& resText, int fSize, float vFar, float vFarInc,
		float wFar, float wFarInc, float wNear, float wNearInc,
		float iScaleX, float iScaleY, float tScale, float muScale);
	void ParseResolution();
};

class CSettings
{
public:
	CSettings();
	~CSettings();
	void Load(const char* filename);
	const char* Getfontfamily();
	ResolutionConfig* FindInfoByIndex(int idx);
	int FindInfoByIndex(const std::string text_name);
	void FindResolutionByIndex(int idx, unsigned int* width, unsigned int* height, int* fontsize);
private:
	std::string fontname;
	ResolutionConfig data_default;
	std::vector<ResolutionConfig> settings;
};