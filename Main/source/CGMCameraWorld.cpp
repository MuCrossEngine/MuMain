#include "stdafx.h"
#include "NewUISystem.h"
#include "ZzzInterface.h"
#include "CGMCameraWorld.h"
#include "WINHANDLE.h"
#include "TextClien.h"

CGMCameraWorld::CGMCameraWorld()
{
	m_Enable = 0;
	m_IsMove = 0;
	m_IsZoom = 0;
	cursor.X = 0;
	cursor.Y = 0;
	zoomPercent[0] = 40.0;
	zoomPercent[1] = 180.0;
	zoomPrecision = 2.0;

	m_Default = new CAMERA_INFO;
	m_Current = new CAMERA_INFO;
	m_nBackup = new CAMERA_INFO;

	m_Default->IsLoad = 0;

	m_Current->zoom = 35.0;
	m_Current->Rot.Y = -48.5;
	m_Current->Rot.Z = 150.0;
	m_Current->camAngle = 45.0;

	m_Default->zoom = 35.0;
	m_Default->Rot.Y = -48.5;
	m_Default->Rot.Z = 150.0;
	m_Default->camAngle = 45.0;
}

CGMCameraWorld::~CGMCameraWorld()
{
	delete m_Default;
	delete m_Current;
	delete m_nBackup;
}

void CGMCameraWorld::Init()
{
	ResolutionConfig* conf = gwinhandle->LoadCurrentConfig();

	m_Current->camWidthFar = 2000.0 * g_fScreenRate_x;
	m_Default->camWidthFar = 2000.0 * g_fScreenRate_x;
	//--
	m_Current->WidthFar[0] = 3000.0 * g_fScreenRate_x;
	m_Current->WidthFar[1] = 2250.0 * g_fScreenRate_x;
	m_Current->WidthFar[2] = 1300.0 * g_fScreenRate_x;
	m_Current->WidthFar[3] = 1250.0 * g_fScreenRate_x;
	m_Current->WidthFar[5] = 1900.0 * g_fScreenRate_x;
	//--
	m_Current->WidthNear[0] = 540.0 * g_fScreenRate_y;
	m_Current->WidthNear[1] = 540.0 * g_fScreenRate_y;
	m_Current->WidthNear[2] = 580.0 * g_fScreenRate_y;
	m_Current->WidthNear[3] = 540.0 * g_fScreenRate_y;
	m_Current->WidthNear[5] = 600.0 * g_fScreenRate_y;
	//--
	m_Current->camviewFar[0] = 8500.0 * g_fScreenRate_x; //-- WD_6STADIUM
	m_Current->camviewFar[1] = 5100.0 * g_fScreenRate_x; //-- WD_30BATTLECASTLE_2
	m_Current->camviewFar[2] = 3300.0 * g_fScreenRate_x; //-- WD_30BATTLECASTLE
	m_Current->camviewFar[3] = 2850.0 * g_fScreenRate_x; //-- WD_62SANTA_TOWN
	m_Current->camviewFar[5] = 3300.0 * g_fScreenRate_x;

	if (conf)
	{
		m_Current->WidthFar[4] = conf->width_far;
		m_Current->WidthNear[4] = conf->width_near;
		m_Current->camviewFar[4] = conf->view_far; //-- General
	}
	else
	{
		m_Current->WidthFar[4] = 1190.0 * g_fScreenRate_x;
		m_Current->WidthNear[4] = 540.0 * g_fScreenRate_y;
		m_Current->camviewFar[4] = 2800.0 * g_fScreenRate_x; //-- General
	}

	memcpy(m_nBackup, m_Current, sizeof(CAMERA_INFO));
	memcpy(m_Default->WidthFar, m_Current->WidthFar, sizeof(m_Default->WidthFar));
	memcpy(m_Default->WidthNear, m_Current->WidthNear, sizeof(m_Default->WidthNear));
	memcpy(m_Default->camviewFar, m_Current->camviewFar, sizeof(m_Default->camviewFar));
}

void CGMCameraWorld::Toggle()
{
	m_Enable = !m_Enable;

	if (m_Enable != 0)
		CreateNotice(gTextClien.TextClien_Khac[0], 0); //"Cam ONLINE"
	else
		CreateNotice(gTextClien.TextClien_Khac[1], 0); //"Cam OFFLINE"

	if (((m_Default->IsLoad == 0) ? (m_Default->IsLoad++) : m_Default->IsLoad) == 0)
	{
		m_Default->Rot.X = CameraAngle[2];
	}
}

void CGMCameraWorld::Backup()
{
	if (this->m_Enable != 0 && SceneFlag == MAIN_SCENE)
	{
		this->DefaultValues();
	}
}

bool CGMCameraWorld::IsEnable()
{
	return this->m_Enable!=0;
}

void CGMCameraWorld::DefaultValues()
{
	CameraAngle[2] = m_Default->Rot.X;
	m_Current->zoom = m_Default->zoom;
	m_Current->Rot.X = m_Default->Rot.X;
	m_Current->Rot.Y = m_Default->Rot.Y;
	m_Current->Rot.Z = m_Default->Rot.Z;
	m_Current->camAngle = m_Default->camAngle;
	m_Current->camWidthFar = m_Default->camWidthFar;
	memcpy(m_Current->WidthFar, m_Default->WidthFar, sizeof(m_Current->WidthFar));
	memcpy(m_Current->WidthNear, m_Default->WidthNear, sizeof(m_Current->WidthFar));
	memcpy(m_Current->camviewFar, m_Default->camviewFar, sizeof(m_Current->camviewFar));
}

void CGMCameraWorld::recover_backup_cam()
{
	memcpy(m_Current, m_nBackup, sizeof(CAMERA_INFO));
}

void CGMCameraWorld::CalcNearFar()
{
	float percent = (float)(m_Current->zoom / (m_Default->zoom / 100.f));

	m_Current->camWidthFar = (float)CalcFactor(m_Default->camWidthFar, percent) + (abs((m_Current->Rot.Z - m_Default->Rot.Z)) * 3.0f);

	for (int i = 0; i < 6; i++)
	{
		m_Current->camviewFar[i] = (float)CalcFactor(m_Default->camviewFar[i], percent) + (abs((m_Current->Rot.Z - m_Default->Rot.Z)) * 2.0f);

		m_Current->WidthFar[i] = (double)CalcFactor(m_Default->WidthFar[i], percent) + (abs((m_Current->Rot.Z - m_Default->Rot.Z)) * 2.0f);

		m_Current->WidthNear[i] = (double)CalcFactor(m_Default->WidthNear[i], percent) + (abs((m_Current->Rot.Z - m_Default->Rot.Z)) * 2.0f);
	}
	memcpy(m_nBackup, m_Current, sizeof(CAMERA_INFO));
}

void CGMCameraWorld::PreparedZoom()
{
	zoomfade = 0;

	m_IsZoom = 0;

	zoomFactor = 0.0;

	desiredZoomFactor = 0;

	current_scroll_value = 0;

	smoothed_zoom_factor = 0;

	zoomLimit[0] = (m_Default->zoom / 100.f) * zoomPercent[0];

	zoomLimit[1] = (m_Default->zoom / 100.f) * zoomPercent[1];
}

void CGMCameraWorld::ZoomInNearFar()
{
	if (!g_pNewUISystem->CheckMouseUse() && MouseOnWindow == 0
		&& SEASON3B::CheckMouseIn(0, 0, GetScreenWidth(), GetWindowsY)
		&& g_dwMouseUseUIID == 0)
	{
		if (m_Enable == 0 || SceneFlag != MAIN_SCENE)
		{
			m_IsZoom = 0;
			return;
		}

		if (MouseWheel < 0 || MouseWheel > 0)
		{
			PreparedZoom();
			m_IsZoom = TRUE;
			zoomfade = (MouseWheel > 0) ? 1 : 2;
			zoomFactor = m_Current->zoom;
			MouseWheel = 0;
		}

		if (zoomfade == 1)
			desiredZoomFactor = (zoomFactor - zoomPrecision);

		if (zoomfade == 2)
			desiredZoomFactor = (zoomFactor + zoomPrecision);

		zoomfade = 0;

		if (m_IsZoom)
		{
			if (smoothed_zoom_factor == desiredZoomFactor)
			{
				PreparedZoom();
				return;
			}
			float mu = current_scroll_value++ / 60.f;

			smoothed_zoom_factor = hermite_interpolation(zoomFactor, desiredZoomFactor, 0.0, 0.0, mu);

			if (smoothed_zoom_factor >= zoomLimit[0] && smoothed_zoom_factor <= zoomLimit[1])
			{
				m_Current->zoom = smoothed_zoom_factor;

				float percent = (float)(m_Current->zoom / (m_Default->zoom / 100.f));

				if (percent < 75.0)
				{
					if (zoomfade == 1)
					{
						if (m_Current->Rot.Y > -90.0f)
						{
							SetAngleY(-2.42, -44.0);
						}
					}
					else if (zoomfade == 2)
					{
						if (m_Current->Rot.Y < -45.0f)
						{
							SetAngleY(2.42, 44.0);
						}
					}
				}

				m_Current->zoom = smoothed_zoom_factor;
				CalcNearFar();
			}
			else
			{
				PreparedZoom();
			}
		}
	}
}

void CGMCameraWorld::RotateInNearFar()
{
	if (m_Enable == 0 || m_IsZoom || SceneFlag != MAIN_SCENE || g_pNewUISystem->CheckMouseUse())
	{
		return;
	}

	m_IsMove = 0;

	if (SEASON3B::IsPress(VK_MBUTTON))
	{
		cursor.X = MouseX;
		cursor.Y = MouseY;
	}

	if (SEASON3B::IsRepeat(VK_MBUTTON))
		m_IsMove = TRUE;

	if (!m_IsMove) return;

	if (cursor.X < MouseX)
	{
		SetAngleX(5.0);
	}
	else if (cursor.X > MouseX)
	{
		SetAngleX(-5.0);
	}

	if (cursor.Y < MouseY)
	{
		if (m_Current->Rot.Y < -45.0f)
		{
			SetAngleY(2.42, 44.0);
		}
	}
	else if (cursor.Y > MouseY)
	{
		if (m_Current->Rot.Y > -90.0f)
		{
			SetAngleY(-2.42, -44.0);
		}
	}
	cursor.X = MouseX;
	cursor.Y = MouseY;
}

void CGMCameraWorld::SetAngleX(float fValue)
{
	if (m_Current->Rot.X > 309.0f || m_Current->Rot.X < -417.0f)
		m_Current->Rot.X = -45.0;
	else
		m_Current->Rot.X += fValue;

	CalcNearFar();
	CameraAngle[2] = m_Current->Rot.X;
	m_Current->camAngle = -m_Current->Rot.X;
}

void CGMCameraWorld::SetAngleY(float fValue1, float fValue2)
{
	if (m_Current->Rot.Y < -45.0f || m_Current->Rot.Y > -90.0f)
	{
		m_Current->Rot.Y += fValue1;
		m_Current->Rot.Z -= fValue2;
		CalcNearFar();
	}
}

float CGMCameraWorld::hermite_interpolation(float y0, float y1, float y2, float y3, float mu)
{
	float m0, m1, mu2, mu3;
	float a0, a1, a2, a3;

	mu = sin(mu * Q_PI / 2.0f); // Aplicar EaseOutSine

	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1 - y0) * 0.5f + (y2 - y1) * 0.5f;
	m1 = (y2 - y1) * 0.5f + (y3 - y2) * 0.5f;
	a0 = 2 * mu3 - 3 * mu2 + 1;
	a1 = mu3 - 2 * mu2 + mu;
	a2 = mu3 - mu2;
	a3 = -2 * mu3 + 3 * mu2;

	return (a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2);
}

CAMERA_INFO* CGMCameraWorld::CurrentCam()
{
	return m_Current;
}

float CGMCameraWorld::GetSmoothed_zoom_factor()
{
	return m_Current->zoom / m_Default->zoom;
}
