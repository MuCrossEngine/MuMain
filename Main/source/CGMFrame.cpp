#include "stdafx.h"
#include "CGMFrame.h"

char TextList[90][100];
int  TextListColor[90];
int  TextBold[90];


float AddMiddleX(float x)
{
	return ((gwinhandle->GetScreenX() - x) / 2.f);
}

float AddMiddleY(float x)
{
	return ((gwinhandle->GetScreenY() - x) / 2.f);
}

float AddPositionX(float x)
{
	return ((gwinhandle->GetScreenX() - 640.0) + x);
}

float AddPositionY(float x)
{
	return ((gwinhandle->GetScreenY() - 480.0) + x);
}