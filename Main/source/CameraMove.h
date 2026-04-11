// CameraMove.h: interface for the CCameraMove class.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAMOVE_H__B3723780_481C_43B7_B72C_5D1782D8324C__INCLUDED_)
#define AFX_CAMERAMOVE_H__B3723780_481C_43B7_B72C_5D1782D8324C__INCLUDED_

#pragma once

#include <vector>
#include <string>

class CCameraMove  
{
#pragma pack(push, 1)
	struct WAYPOINT {
		/*+00*/int iIndex;
		/*+04*/float fCameraX;
		/*+08*/float fCameraY;
		/*+12*/float fCameraZ;
		/*+16*/int iDelay;
		/*+20*/float fCameraMoveAccel;
		/*+24*/float fCameraDistanceLevel;
	};
#pragma pack(pop)

	typedef std::vector<WAYPOINT*>	t_WayPointList;
	t_WayPointList m_listWayPoint;
	/*+024*/float m_CameraStartPos[3];
	/*+036*/float m_fCameraStartDistanceLevel;
	/*+040*/float m_iDelayCount;
	/*+044*/DWORD m_dwCameraWalkState;
	/*+048*/float m_CurrentCameraPos[3];
	/*+060*/float m_fCurrentDistanceLevel;
	/*+064*/DWORD	m_dwCurrentIndex;
	/*+068*/int		m_iSelectedTile;
	int work_movel;
	void Init();
public:
	enum {
		CAMERAWALK_STATE_READY = 0,
		CAMERAWALK_STATE_MOVE,
		CAMERAWALK_STATE_DONE,
	};
	~CCameraMove();

	bool LoadCameraWalkScript(const std::string& filename);
	void UnLoadCameraWalkScript();
	bool SaveCameraWalkScript(const std::string& filename);

	void AddWayPoint(int iGridX, int iGridY, float fCameraMoveAccel, float fCameraDistanceLevel, int iDelay);
	void RemoveWayPoint(int iGridX, int iGridY);

	void SetCameraMoveAccel(int iTileIndex, float fCameraMoveAccel);
	void SetCameraDistanceLevel(int iTileIndex, float fCameraDistanceLevel);
	void SetDelay(int iTileIndex, int iDelay);
	float GetCameraMoveAccel(int iTileIndex);
	float GetCameraDistanceLevel(int iTileIndex);
	int GetDelay(int iTileIndex);

	bool IsCameraMove() const;
	void UpdateWayPoint();
	void GetCurrentCameraPos(float CameraPos[3]);
	float GetCurrentCameraDistanceLevel() const;

	void PlayCameraWalk(float StartPos[3], float fStartDistanceLevel);
	void StopCameraWalk(bool bDone);
	void UpdateCameraStartPos(float StartPos[3]);

	void SetCameraWalkState(DWORD dwCameraWalkState);
	DWORD GetCameraWalkState() const;

	void RenderWayPoint();
	void SetSelectedTile(int iTileIndex);
	DWORD GetSelectedTile() const;

	static CCameraMove* GetInstancePtr();
public:
	CCameraMove();		//. ban create instance

// tourmode
private:
	/*+072*/BOOL m_bTourMode;
	/*+076*/BOOL m_bTourPause;
	/*+080*/float m_fForceSpeed;
	/*+084*/float m_fTourCameraAngle;
	/*+088*/float m_fTargetTourCameraAngle;
	/*+092*/float m_vTourCameraPos[3];
	/*+104*/float m_fCameraAngle;
	/*+108*/float m_fFrustumAngle;
public:
	BOOL SetTourMode(BOOL bFlag, BOOL bRandomStart = FALSE, int _index = 0);
	BOOL IsTourMode() { return m_bTourMode; }
	BOOL IsTourPaused() { return m_bTourPause; }
	void PauseTour(BOOL bFlag);
	void ForwardTour(float fSpeed);
	void BackwardTour(float fSpeed);
	void UpdateTourWayPoint();
	void SetAngleFrustum(float _Value);
	const float GetAngleFrustum() const { return m_fCameraAngle; };
	void SetFrustumAngle(float _Value);
	float GetFrustumAngle();	
	float GetCameraAngle() { return m_fTourCameraAngle; }
};

#endif // !defined(AFX_CAMERAMOVE_H__B3723780_481C_43B7_B72C_5D1782D8324C__INCLUDED_)
