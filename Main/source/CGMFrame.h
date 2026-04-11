#pragma once
#define GetWindowsX								(gwinhandle->GetScreenX())
#define GetWindowsY								(gwinhandle->GetScreenY())
#define PositionX_Add(x)						((GetWindowsX - 640.f + x ))
#define PositionY_Add(x)						((GetWindowsY - 480.f + x ))
#define PositionX_The_Mid(x)					(((GetWindowsX - x) / 2.f))
#define PositionY_The_Mid(x)					((((GetWindowsY-51) - x) / 2.f))

#define to_pos_top(x)							(((float)x))
#define to_pos_left(x)							(((float)x))
#define to_pos_center(x)						((((GetWindowsX - 640.f) * 0.5f) + (float)(x)))
#define to_pos_center2(x)						((((GetWindowsX - 480.f) * 0.5f) + (float)(x)))
#define to_pos_right(x)							((GetWindowsX - (640.f - (float)(x))))
#define to_pos_botton(x)						((GetWindowsY - (480.f - (float)(x))))

#define PositionX_In_The_Mid(x)					(((GetWindowsX - 640.f) / 2.f + (x)))
#define PositionY_In_The_Mid(x)					(((GetWindowsY - 480.f) / 2.f + (x)))
#define Position_In_The_Left(x)					(((GetWindowsX) - x ))
#define Position_In_The_Down(x)					(((GetWindowsY) - x ))
#define ScaleMA(MA)								((MA * GetWindowsY / 480.0))

#define GetNoWideX(num)							((num / g_fScreenRate_x) * ((float)WindowWidth / 640.0))
#define GetNoWideY(num)							((num / g_fScreenRate_y) * ((float)WindowHeight / 480.0))


extern float g_fScreenRate_x;
extern float g_fScreenRate_y;
extern unsigned int WindowWidth;
extern unsigned int WindowHeight;

extern int  TextBold[90];
extern char TextList[90][100];
extern int  TextListColor[90];

extern float AddMiddleX(float x = 640.0);
extern float AddMiddleY(float x = 480.0);
extern float AddPositionX(float x = 0.0);
extern float AddPositionY(float x = 0.0);

inline float pos_top(float x = 0)
{
	return to_pos_top(x);
}

inline float pos_centervertical(float x = 0)
{
	return to_pos_center2(x);
}

inline float pos_left(float x = 0)
{
	return to_pos_left(x);
}


inline float pos_center(float x = 0)
{
	return to_pos_center(x);
}


inline float pos_right(float x = 0)
{
	return to_pos_right(x);
}


inline float pos_botton(float x = 0)
{
	return to_pos_botton(x);
}

