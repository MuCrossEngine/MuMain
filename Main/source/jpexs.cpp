#include "stdafx.h"
#include "jpexs.h"

void jpexs::RenderBoxjpexs_dds(int id, float fRenderPosX, float fRenderPosY, float fRenderWidth, float fRenderHeight)
{
	int iNumx = fRenderWidth / 8;
	int iNumy = fRenderHeight / 8;

	iNumx = ((iNumx * 8) < fRenderWidth) ? (iNumx + 1) : iNumx;
	iNumy = ((iNumy * 8) < fRenderHeight) ? (iNumy + 1) : iNumy;

	fRenderPosX += ((fRenderWidth - (iNumx * 8)) / 2.f);
	fRenderPosY += ((fRenderHeight - (iNumy * 8)) / 2.f);

	fRenderWidth = (iNumx - 2) * 8.f;
	fRenderHeight = (iNumy - 2) * 8.f;

	Renderjpexs_dds(id, fRenderPosX, fRenderPosY, 8.f, 8.f, 880.0, 71.0, 888.0, 79.f); //-- _trackChatView_Top
	Renderjpexs_dds(id, fRenderPosX, fRenderPosY + 8.f + fRenderHeight, 8.f, 8.f, 910.0, 71.0, 918.0, 79.f); //-- _trackChatView_Top

	//-- resize right to left
	Renderjpexs_dds(id, fRenderPosX, fRenderPosY + 8.f, 8.f, fRenderHeight, 940.0, 71.0, 948.0, 79.f); //-- _trackChatView_Top
	Renderjpexs_dds(id, fRenderPosX + 8.f + fRenderWidth, fRenderPosY + 8.f, 8.f, fRenderHeight, 920.0, 71.0, 928.0, 79.f); //-- _trackChatView_Top

	Renderjpexs_dds(id, fRenderPosX + 8.f + fRenderWidth, fRenderPosY, 8.f, 8.f, 860.0, 71.0, 868.0, 79.f); //-- _trackChatView_Top
	Renderjpexs_dds(id, fRenderPosX + 8.f + fRenderWidth, fRenderPosY + 8.f + fRenderHeight, 8.f, 8.f, 900.0, 71.0, 908.0, 79.f); //-- _trackChatView_Top

	//-- resize top to down
	Renderjpexs_dds(id, fRenderPosX + 8.f, fRenderPosY, fRenderWidth, 8.f, 870.0, 71.0, 878.0, 79.f); //-- _trackChatView_Top
	Renderjpexs_dds(id, fRenderPosX + 8.f, fRenderPosY + 8.f + fRenderHeight, fRenderWidth, 8.f, 890.0, 71.0, 898.0, 79.f); //-- _trackChatView_Top


	//-- background
	Renderjpexs_dds(id, fRenderPosX + 8.f, fRenderPosY + 8.f, fRenderWidth, fRenderHeight, 930.0, 71.0, 939.0, 79.f); //-- _trackChatView_Top
}

void jpexs::Renderjpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2, bool assert)
{
	float vh = y2 - y1;
	float uw = x2 - x1;

	if (assert == false)
		SEASON3B::RenderImage2F(id, iPos_x, iPos_y, width, height, x1, y1, uw, vh);
	else
		SEASON3B::RenderImageF(id, iPos_x, iPos_y, width, height, x1, y1, uw, vh);
}

void jpexs::RenderRepetR1jpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2)
{
	float vh = y2 - y1;
	float uw = x2 - x1;

	float nScaleBits = width / uw;

	if (height > vh)
	{
		int nwidth = height / vh;

		for (int i = 0; i < nwidth; i++)
		{
			SEASON3B::RenderImage2F(id, iPos_x, iPos_y + (vh * i), width, vh, x1, y1, uw, vh * nScaleBits);
		}

		if ((nwidth * vh) < height)
		{
			nScaleBits = (nwidth * vh) / height;
			SEASON3B::RenderImage2F(id, iPos_x, iPos_y + (vh * nwidth), width, (height - (nwidth * vh)), x1, y1, uw, vh * nScaleBits);
		}
	}
	else
	{
		SEASON3B::RenderImage2F(id, iPos_x, iPos_y, width, height, x1, y1, uw, vh);
	}
}

void jpexs::RenderRepetR2jpexs_dds(int id, float iPos_x, float iPos_y, float width, float height, float x1, float y1, float x2, float y2)
{
	float vh = y2 - y1;
	float uw = x2 - x1;

	float nScaleBits = height / vh;

	if (width > uw)
	{
		int nwidth = width / uw;

		for (int i = 0; i < nwidth; i++)
		{
			SEASON3B::RenderImage2F(id, iPos_x + (uw * i), iPos_y, uw, height, x1, y1, uw * nScaleBits, vh);
		}

		if ((nwidth * uw) < width)
		{
			nScaleBits = (nwidth * uw) / width;
			SEASON3B::RenderImage2F(id, iPos_x + (uw * nwidth), iPos_y, (width - (nwidth * uw)), height, x1, y1, uw * nScaleBits, vh);
		}
	}
	else
	{
		SEASON3B::RenderImage2F(id, iPos_x, iPos_y, width, height, x1, y1, uw, vh);
	}
}
