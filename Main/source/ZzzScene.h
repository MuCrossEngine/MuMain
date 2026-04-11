#pragma once
#include "zzzinfomation.h"

extern int MenuStateCurrent;
extern int MenuStateNext;
extern int  SceneFlag;
extern int  MoveSceneFrame;
extern int  frame_scene_desplace;
//extern bool EnableEdit;
extern int  ErrorMessage;
extern bool InitServerList;
extern bool InitLogIn;
extern bool InitLoading;
extern bool InitCharacterScene;
extern bool InitMainScene;
extern bool EnableMainRender;
extern char *szServerIpAddress;
extern unsigned short g_ServerPort;
extern int g_iLengthAuthorityCode;

extern void LoadingScene(HDC hDC);
extern void Scene(HDC Hdc);
extern bool CheckName();
extern void StartGame();

extern BOOL ShowCheckBox( int num, int index, int message=MESSAGE_TRADE_CHECK );

extern int	SeparateTextIntoLines( const char* lpszText, char *lpszSeparated, int iMaxLine, int iLineSize);

bool	GetTimeCheck(int DelayTime);
void	SetEffectVolumeLevel(int level);
void    SetViewPortLevel ( int level );

bool IsEnterPressed();
void SetEnterPressed( bool enterpressed );


extern bool update_camera_in_real_time();