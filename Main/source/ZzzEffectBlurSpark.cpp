///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZzzOpenglUtil.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzTexture.h"
#include "ZzzAi.h"
#include "ZzzEffect.h"
#include "DSPlaySound.h"
#include "WSClient.h"
#include "NewUISystem.h"

#define MAX_BLURS      100
#define MAX_BLUR_TAILS 30
#define MAX_BLUR_LIFETIME 30

#define MAX_OBJECTBLURS		1000
#define MAX_OBJECTBLUR_TAILS 600
#define MAX_OBJECTBLUR_LIFETIME 30
//#define MAX_OBJECTBLUR_LIFETIME 11		// FIX

typedef struct
{
	bool      Live;
	int       Type;
	int        LifeTime;
	CHARACTER* Owner;
	int       Number;
	vec3_t    Light;
	vec3_t    p1[MAX_BLUR_TAILS];
	vec3_t    p2[MAX_BLUR_TAILS];
	int       SubType;
} BLUR;

typedef struct
{
	bool      Live;
	int       Type;
	int       LifeTime;
	OBJECT*   Owner;
	int       Number;
	vec3_t    Light;
	int		  _iLimitLifeTime;
	vec3_t    p1[MAX_OBJECTBLUR_TAILS];
	vec3_t    p2[MAX_OBJECTBLUR_TAILS];
	int       SubType;
} OBJECT_BLUR;

BLUR Blur[MAX_BLURS];
OBJECT_BLUR ObjectBlur[MAX_OBJECTBLURS];

void AddBlur(BLUR* b, vec3_t p1, vec3_t p2, vec3_t Light, int Type)
{
	b->Type = Type;
	VectorCopy(Light, b->Light);
	for (int i = b->Number - 1; i >= 0; i--)
	{
		VectorCopy(b->p1[i], b->p1[i + 1]);
		VectorCopy(b->p2[i], b->p2[i + 1]);
	}
	VectorCopy(p1, b->p1[0]);
	VectorCopy(p2, b->p2[0]);
	b->Number++;
	if (b->Number >= MAX_BLUR_TAILS - 1)
	{
		b->Number = MAX_BLUR_TAILS - 1;
	}
}

void CreateBlur(CHARACTER* Owner, vec3_t p1, vec3_t p2, vec3_t Light, int Type, bool Short, int SubType)
{
	for (int i = 0; i < MAX_BLURS; i++)
	{
		BLUR* b = &Blur[i];
		if (b->Live && b->Owner == Owner)
		{
			if (SubType > 0 && b->SubType != SubType)
				continue;
			AddBlur(b, p1, p2, Light, Type);
			return;
		}
	}

	for (int i = 0; i < MAX_BLURS; i++)
	{
		BLUR* b = &Blur[i];
		if (!b->Live)
		{
			b->Live = true;
			b->Owner = Owner;
			b->Number = 0;
			b->LifeTime = standlimit(Short ? 15 : MAX_BLUR_LIFETIME);
			b->SubType = SubType;
			AddBlur(b, p1, p2, Light, Type);
			return;
		}
	}
}

void MoveBlurs()
{
	for (int i = 0; i < MAX_BLURS; i++)
	{
		BLUR* b = &Blur[i];
		if (b->Live)
		{
			b->LifeTime--;
			if (checkNormalizer)
				b->Number--;

			for (int i = b->Number - 1; i >= 0; i--)
			{
				VectorCopy(b->p1[i], b->p1[i + 1]);
				VectorCopy(b->p2[i], b->p2[i + 1]);
			}
			if (b->LifeTime <= 0)
			{
				b->Number--;
				if (b->Number <= 0)
				{
					b->Live = false;
				}
			}
		}
	}
	MoveObjectBlurs();
}

void RenderBlurs()
{
	if (g_pOption->GetRenderEffect() == false)
	{
		return;
	}

	int Type;
	//DisableCullFace();
	for (int i = 0; i < MAX_BLURS; i++)
	{
		BLUR* b = &Blur[i];
		if (b->Live)
		{
			Type = b->Type;

			int nTexture = BITMAP_BLUR + Type;
			if (Type == 3)
			{
				nTexture = BITMAP_BLUR2;
			}
			else if (Type == 4)
			{
				nTexture = BITMAP_BLUR;
			}
			else if (Type == 5)
			{
				nTexture = BITMAP_BLUR + 3;
			}
			if (b->Owner->Level == 0 && (Type <= 3 || (Type >= 5 && Type <= 10)))
			{
				EnableAlphaBlend();
			}
			else
			{
				EnableAlphaBlendMinus();
			}

			if (Type > 2) Type = Type - 3;
			if (b->Number >= 2)
			{
				BindTexture(nTexture);
				for (int j = 0; j < b->Number - 1; j++)
				{
					glBegin(GL_TRIANGLE_FAN);
					float Light;
					float TexU;
					if (b->Owner->Level == 0)
						Light = (b->Number - j) / (float)b->Number;
					else
						Light = 1.f;
					glColor3f(b->Light[0] * Light, b->Light[1] * Light, b->Light[2] * Light);
					TexU = (j) / (float)b->Number;
					glTexCoord2f(TexU, 1.f);
					glVertex3fv(b->p1[j]);
					glTexCoord2f(TexU, 0.f);
					glVertex3fv(b->p2[j]);

					if (b->Owner->Level == 0)
						Light = (b->Number - (j + 1)) / (float)b->Number;
					else
						Light = 1.f;

					glColor3f(b->Light[0] * Light, b->Light[1] * Light, b->Light[2] * Light);
					TexU = (j + 1) / (float)b->Number;
					glTexCoord2f(TexU, 0.f);
					glVertex3fv(b->p2[j + 1]);
					glTexCoord2f(TexU, 1.f);
					glVertex3fv(b->p1[j + 1]);
					glEnd();
				}
			}
		}
	}
	RenderObjectBlurs();
}

void ClearAllObjectBlurs()
{
	for (int i = 0; i < MAX_OBJECTBLURS; ++i)
	{
		ObjectBlur[i].Live = false;
	}
}

void AddObjectBlur(OBJECT_BLUR* b, vec3_t p1, vec3_t p2, vec3_t Light, int Type)
{
	b->Type = Type;
	VectorCopy(Light, b->Light);

	for (int i = b->Number - 1; i >= 0; i--)
	{
		VectorCopy(b->p1[i], b->p1[i + 1]);
		VectorCopy(b->p2[i], b->p2[i + 1]);
	}
	VectorCopy(p1, b->p1[0]);
	VectorCopy(p2, b->p2[0]);
	b->Number++;

	if (b->Number >= MAX_OBJECTBLUR_TAILS - 1)
	{
		b->Number = MAX_OBJECTBLUR_TAILS - 1;
	}
}

void CreateObjectBlur(OBJECT* Owner, vec3_t p1, vec3_t p2, vec3_t Light, int Type, bool Short, int SubType, int iLimitLifeTime)
{
	for (int i = 0; i < MAX_OBJECTBLURS; i++)
	{
		OBJECT_BLUR* b = &ObjectBlur[i];
		if (b->Live && b->Owner == Owner)
		{
			if (SubType > 0 && b->SubType != SubType)
				continue;
			AddObjectBlur(b, p1, p2, Light, Type);
			return;
		}
	}

	for (int i = 0; i < MAX_OBJECTBLURS; i++)
	{
		OBJECT_BLUR* b = &ObjectBlur[i];
		if (!b->Live)
		{
			b->Live = true;
			b->Owner = Owner;
			b->Number = 0;
			if (iLimitLifeTime > -1)
			{
				b->LifeTime = iLimitLifeTime;
				b->_iLimitLifeTime = iLimitLifeTime;
			}
			else
			{
				b->_iLimitLifeTime = standlimit(Short ? 15 : MAX_OBJECTBLUR_LIFETIME);
				b->LifeTime = b->_iLimitLifeTime;
			}
			b->SubType = SubType;

			AddObjectBlur(b, p1, p2, Light, Type);
			return;
		}
	}
}

void MoveObjectBlurs()
{
	for (int i = 0; i < MAX_OBJECTBLURS; i++)
	{
		OBJECT_BLUR* b = &ObjectBlur[i];
		if (b->Live)
		{
			b->LifeTime--;
			if (checkNormalizer)
				b->Number--;

			if (b->LifeTime <= 0)
			{
				b->Number = 0;
				b->Live = false;
				continue;
			}

			for (int i = b->Number - 1; i >= 0; i--)
			{
				VectorCopy(b->p1[i], b->p1[i + 1]);
				VectorCopy(b->p2[i], b->p2[i + 1]);
			}
		}
	}
}

void RenderObjectBlurs()
{
	int Type;
	for (int i = 0; i < MAX_OBJECTBLURS; i++)
	{
		OBJECT_BLUR* b = &ObjectBlur[i];
		if (b->Live)
		{
			Type = b->Type;
			int nTexture = BITMAP_BLUR + Type;
			switch (Type)
			{
			case 3:
				nTexture = BITMAP_BLUR2;
				break;
			case 4:
				nTexture = BITMAP_BLUR;
				break;
			case 5:
				nTexture = BITMAP_LAVA;
				break;
			default:
				if (Type > BITMAP_BLUR_END)
				{
					nTexture = b->Type;
				}
				break;
			}

			EnableAlphaBlend();

			//if (Type > 2) Type = Type - 3;
			if (b->Number >= 2)
			{
				BindTexture(nTexture);
				for (int j = 0; j < b->Number - 1; j++)
				{
					float Data = 300.f;
					if (b->SubType == 113 || b->SubType == 114)
					{
						if (abs(b->p1[j][0] - b->p1[j + 1][0]) > Data || abs(b->p1[j][1] - b->p1[j + 1][1]) > Data || abs(b->p1[j][2] - b->p1[j + 1][2]) > Data ||
							abs(b->p1[j][0] - b->p2[j + 1][0]) > Data || abs(b->p1[j][1] - b->p2[j + 1][1]) > Data || abs(b->p2[j][2] - b->p2[j + 1][2]) > Data)
							continue;
					}

					glBegin(GL_TRIANGLE_FAN);
					float Light;
					float TexU;
					Light = (b->Number - j) / (float)b->Number;
					glColor3f(b->Light[0] * Light, b->Light[1] * Light, b->Light[2] * Light);
					TexU = (j) / (float)b->Number;
					glTexCoord2f(TexU, 1.f);
					glVertex3fv(b->p1[j]);
					glTexCoord2f(TexU, 0.f);
					glVertex3fv(b->p2[j]);
					Light = (b->Number - (j + 1)) / (float)b->Number;
					glColor3f(b->Light[0] * Light, b->Light[1] * Light, b->Light[2] * Light);
					TexU = (j + 1) / (float)b->Number;
					glTexCoord2f(TexU, 0.f);
					glVertex3fv(b->p2[j + 1]);
					glTexCoord2f(TexU, 1.f);
					glVertex3fv(b->p1[j + 1]);
					glEnd();
				}
			}
		}
	}
}

void RemoveObjectBlurs(OBJECT* Owner, int SubType)
{
	for (int i = 0; i < MAX_OBJECTBLURS; i++)
	{
		OBJECT_BLUR* b = &ObjectBlur[i];
		if (b->Live && b->Owner == Owner)
		{
			ObjectBlur[i].Live = false;
		}
	}
}

void CreateSpark(int Type, CHARACTER* tc, vec3_t Position, vec3_t Angle)
{
	OBJECT* to = &tc->Object;
	vec3_t Light;
	Vector(1.f, 1.f, 1.f, Light);
	CreateParticle(BITMAP_SPARK + 1, Position, to->Angle, Light);
	vec3_t p, p2;
	float Matrix[3][4];
	Vector(0.f, 50.f, 0.f, p);
	AngleMatrix(Angle, Matrix);
	VectorRotate(p, Matrix, p2);
	VectorAdd(p2, Position, p2);
	for (int i = 0; i < 20; i++)
	{
		vec3_t a;
		Vector((float)(rand() % 360), 0.f, (float)(rand() % 360), a);
		VectorAdd(a, Angle, a);
		//CreateJoint(BITMAP_JOINT_SPARK,p2,p2,a);
		CreateParticle(BITMAP_SPARK, Position, to->Angle, Light);
	}
}

void CreateBlood(OBJECT* o)
{
	int BoneHead = gmClientModels->GetModel(o->Type)->BoneHead;
	if (BoneHead != -1)
	{
		if (o->Type == MODEL_MONSTER01 + 15)
		{
			o->Live = false;
			for (int i = 0; i < 10; i++)
				CreateEffect(MODEL_ICE_SMALL, o->Position, o->Angle, o->Light);
		}
		else if (o->Type != MODEL_MONSTER01 + 7 && o->Type != MODEL_MONSTER01 + 14 && o->Type != MODEL_MONSTER01 + 18)
		{
			vec3_t p, Position;
			for (int i = 0; i < 2; i++)
			{
				Vector((float)(rand() % 100 - 50), (float)(rand() % 100 - 50), 0.f, p);
				gmClientModels->GetModel(o->Type)->TransformPosition(o->BoneTransform[BoneHead], p, Position, true);
				CreatePointer(BITMAP_BLOOD, Position, (float)(rand() % 360), o->Light, (float)(rand() % 4 + 8) * 0.1f);
			}
		}
	}
}