#include "stdafx.h"
#include "CameraManager.h"

extern int GetScreenWidth();

CCameraManager* CCameraManager::Instance()
{
	static CCameraManager sInstance;
	return &sInstance;
}

CCameraManager::CCameraManager()
{
	Enable = false;
	Ratio = 1.0;
	CameraFOV = 35.0;
	CameraDistance = 1000.0;
	CameraDistanceTarget = 35.0;

	CameraAngle[0] = 0.0;
	CameraAngle[1] = 0.0;
	CameraAngle[2] = 0.0;
	CameraPosition[0] = 0.0;
	CameraPosition[1] = 0.0;
	CameraPosition[2] = 0.0;

	CameraWidthFOV[0] = 0.0;
	CameraWidthFOV[1] = 0.0;
	CameraWidthFOV[2] = 0.0;
	CameraWidthFOV[3] = 0.0;
	CameraWidthFOV[4] = 0.0;
	CameraWidthFOV[5] = 0.0;
	CameraWidthFOV[6] = 0.0;
	CameraWidthFOV[7] = 0.0;
	CameraWidthFOV[8] = 0.0;
}

CCameraManager::~CCameraManager()
{
}

void CCameraManager::OpenCameraSetting(char* WorldName)
{
	Enable = false;

	int version = Open_Camera_Angle_Position(WorldName);

	if (version == 0)
		return;

	Enable = true;

	char FileName[64];
	char ReturnedString[MAX_PATH];

	sprintf(FileName, "Data\\%s\\CAP.ini", WorldName);

	GetPrivateProfileString("CAMERA ANGLE", "Angle X", "\0", ReturnedString, 0xBu, FileName);
	CameraAngle[0] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA ANGLE", "Angle Y", "\0", ReturnedString, 0xBu, FileName);
	CameraAngle[1] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA ANGLE", "Angle Z", "\0", ReturnedString, 0xBu, FileName);
	CameraAngle[2] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA POSITION", "Position X", "\0", ReturnedString, 0x1Eu, FileName);
	CameraPosition[0] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA POSITION", "Position Y", "\0", ReturnedString, 0x1Eu, FileName);
	CameraPosition[1] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA POSITION", "Position Z", "\0", ReturnedString, 0x1Eu, FileName);
	CameraPosition[2] = atof(ReturnedString);

	GetPrivateProfileString("CAMERA DISTANCE", "Distance", "\0", ReturnedString, 0xBu, FileName);
	CameraDistance = atof(ReturnedString);

	GetPrivateProfileString("CAMERA DISTANCE", "Z_Distance", "\0", ReturnedString, 0xBu, FileName);
	CameraDistanceTarget = atof(ReturnedString);

	GetPrivateProfileString("CAMERA FOV", "FOV", "35.000000", ReturnedString, 0xBu, FileName);
	CameraFOV = atof(ReturnedString);

	GetPrivateProfileString("CAMERA FOV", "FOV1", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[0] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV2", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[1] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV3", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[2] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV4", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[3] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV5", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[4] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV6", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[5] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV7", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[6] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV8", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[7] = atof(ReturnedString);
	GetPrivateProfileString("CAMERA FOV", "FOV9", "35.000000", ReturnedString, 0xBu, FileName);
	CameraWidthFOV[8] = atof(ReturnedString);

	//GetPrivateProfileString("CAMERA RATIO", "Ratio", "\0", ReturnedString, 0xBu, FileName);
	//Ratio = atof(ReturnedString);
	DeleteFile(FileName);
}

float CCameraManager::GetFov()
{
	if (m_Resolution >= 0 && m_Resolution < 9)
	{
		return CameraWidthFOV[m_Resolution];
	}
	else
	{
		return CameraFOV;
	}
}

float CCameraManager::GetDistance()
{
	return CameraDistance;
}

float CCameraManager::GetDistanceTarget()
{
	return CameraDistanceTarget;
}

void CCameraManager::GetAngle(vec3_t Angle)
{
	VectorCopy(CameraAngle, Angle);
}

void CCameraManager::GetPosition(vec3_t Position)
{
	VectorCopy(CameraPosition, Position);
}

double CCameraManager::CalcViewFar(float a2)
{
	return (float)((float)((float)(a2 * CameraDistance) * CameraDistanceTarget) * Ratio);
}

double CCameraManager::CalcWidthCam()
{
	float Width = ((float)GetScreenWidth() / GetWindowsX);
	return (float)((float)((float)(Width * CameraDistance) * CameraDistanceTarget) * Ratio);
}
