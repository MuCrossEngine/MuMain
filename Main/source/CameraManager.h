#pragma once

class CCameraManager
{
public:
	static CCameraManager* Instance();

	CCameraManager();
	virtual~CCameraManager();
	void OpenCameraSetting(char* WorldName);

	float GetFov();
	float GetDistance();
	float GetDistanceTarget();
	void GetAngle(vec3_t Angle);
	void GetPosition(vec3_t Position);
	double CalcViewFar(float a2);
	double CalcWidthCam();

	BOOL Enable;
private:
	float Ratio;
	float CameraFOV;
	float CameraWidthFOV[9];
	float CameraDistance;
	float CameraDistanceTarget;

	vec3_t CameraAngle;
	vec3_t CameraPosition;
};


#define gCameraManager		(CCameraManager::Instance())


extern int Open_Camera_Angle_Position(char* WorldName);
