#pragma once
#define scalejpexs(p)				(0.05 * p)
#define scale_jpexs(x, p)			(x + (0.05 * p))

namespace jpexs
{
	void RenderHudMapjpexs_1dds(int id, float iPos_x, float iPos_y, char* Name, bool assert = 0);
	void Renderjpexs_1dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float x2, float y1, float y2, bool assert = 0);
	//--
	void RenderBoxjpexs_dds(int id, float fRenderPosX, float fRenderPosY, float fRenderWidth, float fRenderHeight);
	void RenderTipjpexs_dds(int id, float fRenderPosX, float fRenderPosY, float fRenderWidth, float fRenderHeight);
	void RenderStorejpexs_dds(int id, float fRenderPosX, float fRenderPosY, float fRenderWidth, float fRenderHeight);
	void RenderCaracterjpexs_dds(int id, float fRenderPosX, float fRenderPosY, float fRenderWidth, float fRenderHeight);

	void Renderjpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2, bool assert = 0);
	void RenderRepetLjpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2, bool assert = 0);
	void RenderRepetR1jpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2);
	void RenderRepetR2jpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2);
	void RenderRepetR3jpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2);

	//void fOpenInterfaceSwf(std::string filename, std::map<int, DecJpexs>* m_Vec);
	//int fOpenScript(std::string filename, BYTE** pbyBuffer, int allocatorSize, DWORD Key);
};