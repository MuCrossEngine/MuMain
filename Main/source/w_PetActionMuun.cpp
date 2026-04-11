#include "stdafx.h"
#include "ZzzAI.h"
#include "DSPlaySound.h"
#include "CGMItemEffect.h"
#include "wsclientinline.h"
#include "w_PetActionMuun.h"

PetActionMuunPtr PetActionMuun::Make()
{
	PetActionMuunPtr temp(new PetActionMuun);
	return temp;
}

PetActionMuun::PetActionMuun()
{
	m_isRooting = false;
	m_dwSendDelayTime = 0;
	m_dwRootingTime = 0;
	m_dwRoundCountDelay = 0;
	m_state = eAction_Stand;
	m_fRadWidthStand = 0.0f;
	m_fRadWidthGet = 0.0f;
	m_bIsMoving = FALSE;
}

PetActionMuun::~PetActionMuun()
{
}

bool PetActionMuun::Release(OBJECT* obj, CHARACTER* Owner)
{
	return false;
}

bool PetActionMuun::Model(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender)
{
	if (NULL == obj || NULL == Owner)
		return false;

	return true;
}

bool PetActionMuun::Move(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender)
{
	if (NULL == obj || NULL == Owner) return FALSE;

	//FindZen(obj);

	if (m_state == eAction_Stand && m_isRooting)
	{
		m_state = eAction_Move;
	}

	float MoveRange = 12.0f;
	vec3_t targetPos, Range, Direction;
	bool _isMove = false;

	m_fRadWidthStand = ((2 * Q_PI) / 4000.0f) * fmodf(tick , 4000);
	m_fRadWidthGet = ((2 * Q_PI) / 2000.0f) * fmodf(tick , 2000);

	obj->Position[2] = obj->Owner->Position[2];

	VectorSubtract(obj->Owner->Position, obj->Position, Range);

	float Distance = sqrtf(Range[0] * Range[0] + Range[1] * Range[1]);

	if (Distance > SEARCH_LENGTH * 3)
	{
		obj->Position[0] = obj->Owner->Position[0] + (sinf(m_fRadWidthStand) * CIRCLE_STAND_RADIAN);
		obj->Position[1] = obj->Owner->Position[1] + (cosf(m_fRadWidthStand) * CIRCLE_STAND_RADIAN);

		VectorCopy(obj->Owner->Angle, obj->Angle);

		m_state = eAction_Stand;
		m_isRooting = false;
		obj->CurrentAction = 0;
	}

	switch (m_state)
	{
	case eAction_Stand:
	{
		VectorCopy(obj->Owner->Position, targetPos);
		VectorSubtract(targetPos, obj->Position, Range);

		Distance = sqrtf(Range[0] * Range[0] + Range[1] * Range[1]);

		bool fastspeedter = true;

		if (80.0f >= MoveRange)
		{
			float Angle = CreateAngle(obj->Position[0], obj->Position[1], targetPos[0], targetPos[1]); //test
			obj->Angle[2] = TurnAngle2(obj->Angle[2], Angle, (8.0f));
		}

		if (Distance > 150.0)
		{
			obj->CurrentAction = 2;
			fastspeedter = false;
		}
		else
		{
			obj->CurrentAction = 0;
		}

		AngleMatrix(obj->Angle, obj->Matrix);
		VectorRotate(obj->Direction, obj->Matrix, Direction);
		interpolatesum(obj->Position, Direction);

		float Speed = (MoveRange * MoveRange >= Distance) ? 0 : (float)log(Distance) * 2.3f;

		obj->Direction[0] = 0.0f;
		if (fastspeedter)
			obj->Direction[1] = -timefac(Speed);
		else
			obj->Direction[1] = -(Speed);
		obj->Direction[2] = 0.0f;
	}
	break;
	case eAction_Move:
	{
		if (!m_isRooting)
		{
			m_isRooting = false;
			m_state = eAction_Return;
			break;
		}

		targetPos[0] = m_RootItem.position[0] + (sinf(m_fRadWidthGet) * CIRCLE_STAND_RADIAN);
		targetPos[1] = m_RootItem.position[1] + (cosf(m_fRadWidthGet) * CIRCLE_STAND_RADIAN);
		targetPos[2] = m_RootItem.position[2];

		VectorSubtract(targetPos, obj->Position, Range);

		Distance = sqrtf(Range[0] * Range[0] + Range[1] * Range[1]);
		if (Distance >= MoveRange)
		{
			float Angle = CreateAngle(obj->Position[0], obj->Position[1], targetPos[0], targetPos[1]);
			obj->Angle[2] = TurnAngle2(obj->Angle[2], Angle, (20.0f));
		}

		AngleMatrix(obj->Angle, obj->Matrix);
		VectorRotate(obj->Direction, obj->Matrix, Direction);
		interpolatesum(obj->Position, Direction);

		float Speed = (20.0f >= Distance) ? 0 : (float)log(Distance) * 2.5f;

		obj->Direction[0] = 0.0f;
		obj->Direction[1] = -(Speed);
		obj->Direction[2] = 0.0f;

		if (0 == Speed || CompTimeControl(100000, m_dwRootingTime))
		{
			m_dwSendDelayTime = GetTickCount();
			m_dwRootingTime = GetTickCount();
			m_state = eAction_Get;
		}
	}
	break;
	case eAction_Get:
	{
		if (!m_isRooting || SEARCH_LENGTH < Distance || CompTimeControl(3000, m_dwRootingTime))
		{
			m_isRooting = false;
			m_dwRootingTime = GetTickCount();
			m_state = eAction_Return;
			break;
		}

		VectorCopy(m_RootItem.position, targetPos);

		float Angle = CreateAngle(obj->Position[0], obj->Position[1], targetPos[0], targetPos[1]);
		obj->Angle[2] = TurnAngle2(obj->Angle[2], Angle, timefac(10.0f));

		if (CompTimeControl(1000, m_dwSendDelayTime))
		{
			if (&Hero->Object == obj->Owner)
				SendRequestGetItem(m_RootItem.itemIndex);
		}
	}
	break;
	case eAction_Return:
	{
		targetPos[0] = obj->Owner->Position[0] + (sinf(m_fRadWidthStand) * CIRCLE_STAND_RADIAN);
		targetPos[1] = obj->Owner->Position[1] + (cosf(m_fRadWidthStand) * CIRCLE_STAND_RADIAN);
		targetPos[2] = obj->Owner->Position[2];

		VectorSubtract(targetPos, obj->Position, Range);

		Distance = sqrtf(Range[0] * Range[0] + Range[1] * Range[1]);
		if (Distance >= MoveRange)
		{
			float Angle = CreateAngle(obj->Position[0], obj->Position[1], targetPos[0], targetPos[1]);
			obj->Angle[2] = TurnAngle2(obj->Angle[2], Angle, 20.0f);
		}

		AngleMatrix(obj->Angle, obj->Matrix);
		VectorRotate(obj->Direction, obj->Matrix, Direction);

		obj->Position[0] += timefac(Direction[0]);
		obj->Position[1] += timefac(Direction[1]);
		obj->Position[2] += timefac(Direction[2]);

		float Speed = (MoveRange >= Distance) ? 0 : (float)log(Distance) * 2.5f;

		obj->Direction[0] = 0.0f;
		obj->Direction[1] = -(Speed);
		obj->Direction[2] = 0.0f;

		if (0 == Speed || CompTimeControl(3000, m_dwRootingTime))
		{
			m_state = eAction_Stand;
		}
	}
	break;
	}

	return TRUE;
}

bool PetActionMuun::Effect(OBJECT* obj, CHARACTER* Owner, int targetKey, double tick, bool bForceRender)
{
	BMD* pModel = gmClientModels->GetModel(obj->Type);

	if (pModel)
	{
		GMItemEffect->RenderPetEffect((obj->Type - MODEL_ITEM), pModel, obj);


		if (obj->Owner->ExtState == 1)
		{
			vec3_t Angle = { 0.f, 0.f, 0.f };
			vec3_t Position = { obj->Position[0], obj->Position[1], obj->Position[2] };

			for (int i = 0; i < 5; ++i)
			{
				CreateJoint(BITMAP_FLARE, Position, Position, Angle, 0, obj, 40, 2);
			}
			obj->Owner->ExtState = 0;
		}
	}
	return false;
}