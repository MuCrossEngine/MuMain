///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Platform/GameAssetPath.h"
#include <new>

static inline int32_t ReadSerializedInt32(const unsigned char* data)
{
	int32_t value = 0;
	memcpy(&value, data, sizeof(value));
	return value;
}
#include "ZzzOpenglUtil.h"
#include "ZzzInfomation.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "zzzlodterrain.h"
#include "ZzzTexture.h"
#include "ZzzAI.h"
#include "SMD.h"
#include "zzzEffect.h"
#include "MapManager.h"
#include "./Utilities/Log/muConsoleDebug.h"
//#include "FillPolygon.h"
#include "GMBattleCastle.h"
#include "UIMng.h"
#include "CameraMove.h"
#include "PhysicsManager.h"
#include "MuCrypto/MuCrypto.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

extern float MouseX;
extern float MouseY;
extern bool MouseLButton;
#ifdef __ANDROID__
extern bool AlphaTestEnable;
extern int AlphaBlendType;
#endif


vec4_t BoneQuaternion[MAX_BONES];
short  BoundingVertices[MAX_BONES];
vec3_t BoundingMin[MAX_BONES];
vec3_t BoundingMax[MAX_BONES];

float  BoneTransform[MAX_BONES][3][4];

vec3_t VertexTransform[MAX_MESH][MAX_VERTICES];
vec3_t NormalTransform[MAX_MESH][MAX_VERTICES];
float  IntensityTransform[MAX_MESH][MAX_VERTICES];
vec3_t LightTransform[MAX_MESH][MAX_VERTICES];

vec3_t RenderArrayVertices[MAX_VERTICES * 3];
vec4_t RenderArrayColors[MAX_VERTICES * 3];
vec2_t RenderArrayTexCoords[MAX_VERTICES * 3];


unsigned char ShadowBuffer[256 * 256];
int           ShadowBufferWidth = 256;
int           ShadowBufferHeight = 256;


vec3_t	g_vright;		// needs to be set to viewer's right in order for chrome to work
int		g_smodels_total = 1;				// cookie
float	g_chrome[MAX_VERTICES][2];	// texture coords for surface normals
int		g_chromeage[MAX_BONES];	// last time chrome vectors were updated
vec3_t	g_chromeup[MAX_BONES];		// chrome vector "up" in bone reference frames
vec3_t	g_chromeright[MAX_BONES];	// chrome vector "right" in bone reference frames

extern int EditFlag;
extern int WaterTextureNumber;

bool HighLight = true;
float BoneScale = 1.f;
bool  StopMotion = false;
float ParentMatrix[3][4];

static vec3_t LightVector = { 0.f, -0.1f, -0.8f };
static vec3_t LightVector2 = { 0.f, -0.5f, -0.8f };

void BMD::Animation(float(*BoneMatrix)[3][4], float AnimationFrame, float PriorFrame, unsigned short PriorAction, vec3_t Angle, vec3_t HeadAngle, bool Parent, bool Translate)
{
	if (NumActions <= 0)
		return;

	if (PriorAction >= NumActions)
		PriorAction = 0;
	if (CurrentAction >= NumActions)
		CurrentAction = 0;

	VectorCopy(Angle, BodyAngle);

	CurrentAnimation = AnimationFrame;
	CurrentAnimationFrame = (int)AnimationFrame;
	float s1 = (CurrentAnimation - CurrentAnimationFrame);
	float s2 = 1.f - s1;

	int PriorAnimationFrame = (int)PriorFrame;
	if (NumActions > 0)
	{
		if (PriorAnimationFrame < 0)
			PriorAnimationFrame = 0;
		if (CurrentAnimationFrame < 0)
			CurrentAnimationFrame = 0;
		if (PriorAnimationFrame >= Actions[PriorAction].NumAnimationKeys)
			PriorAnimationFrame = 0;
		if (CurrentAnimationFrame >= Actions[CurrentAction].NumAnimationKeys)
			CurrentAnimationFrame = 0;
	}

	// bones
	for (int i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		if (b->Dummy)
		{
			continue;
		}
		BoneMatrix_t* bm1 = &b->BoneMatrixes[PriorAction];
		BoneMatrix_t* bm2 = &b->BoneMatrixes[CurrentAction];
		vec4_t q1, q2;

		if (i == BoneHead)
		{
			vec3_t Angle1, Angle2;
			VectorCopy(bm1->Rotation[PriorAnimationFrame], Angle1);
			VectorCopy(bm2->Rotation[CurrentAnimationFrame], Angle2);

			float HeadAngleX = HeadAngle[0] / (180.f / Q_PI);
			float HeadAngleY = HeadAngle[1] / (180.f / Q_PI);
			Angle1[0] -= HeadAngleX;
			Angle2[0] -= HeadAngleX;
			Angle1[2] -= HeadAngleY;
			Angle2[2] -= HeadAngleY;
			AngleQuaternion(Angle1, q1);
			AngleQuaternion(Angle2, q2);
		}
		else
		{
			QuaternionCopy(bm1->Quaternion[PriorAnimationFrame], q1);
			QuaternionCopy(bm2->Quaternion[CurrentAnimationFrame], q2);
		}
		if (!QuaternionCompare(q1, q2))
		{
			QuaternionSlerp(q1, q2, s1, BoneQuaternion[i]);
		}
		else
		{
			QuaternionCopy(q1, BoneQuaternion[i]);
		}

		float Matrix[3][4];
		QuaternionMatrix(BoneQuaternion[i], Matrix);
		float* Position1 = bm1->Position[PriorAnimationFrame];
		float* Position2 = bm2->Position[CurrentAnimationFrame];

		if (i == 0 && (Actions[PriorAction].LockPositions || Actions[CurrentAction].LockPositions))
		{
			Matrix[0][3] = bm2->Position[0][0];
			Matrix[1][3] = bm2->Position[0][1];
			Matrix[2][3] = Position1[2] * s2 + Position2[2] * s1 + BodyHeight;
		}
		else
		{
			Matrix[0][3] = Position1[0] * s2 + Position2[0] * s1;
			Matrix[1][3] = Position1[1] * s2 + Position2[1] * s1;
			Matrix[2][3] = Position1[2] * s2 + Position2[2] * s1;
		}

		if (b->Parent == -1)
		{
			if (!Parent)
			{
				AngleMatrix(BodyAngle, ParentMatrix);

				if (Translate)
				{
					for (int y = 0; y < 3; ++y)
					{
						for (int x = 0; x < 3; ++x)
						{
							ParentMatrix[y][x] *= BodyScale;
						}
					}

					ParentMatrix[0][3] = BodyOrigin[0];
					ParentMatrix[1][3] = BodyOrigin[1];
					ParentMatrix[2][3] = BodyOrigin[2];
				}
			}
			R_ConcatTransforms(ParentMatrix, Matrix, BoneMatrix[i]);
		}
		else
		{
			R_ConcatTransforms(BoneMatrix[b->Parent], Matrix, BoneMatrix[i]);
		}
	}
}

bool BMD::PlayAnimation(float* AnimationFrame, float* PriorAnimationFrame, unsigned short* PriorAction, float Speed, vec3_t Origin, vec3_t Angle)
{
	bool Loop = true;

	if (AnimationFrame == NULL || PriorAnimationFrame == NULL || PriorAction == NULL || (NumActions > 0 && CurrentAction >= NumActions))
	{
		return Loop;
	}

	if (NumActions == 0 || Actions[CurrentAction].NumAnimationKeys <= 1)
	{
		return Loop;
	}

	int Temp = (int)*AnimationFrame;

	*AnimationFrame += timefac(Speed);

	if (Temp != (int)*AnimationFrame)
	{
		*PriorAction = CurrentAction;
		*PriorAnimationFrame = (float)Temp;
	}
	if (*AnimationFrame <= 0.f)
	{
		*AnimationFrame += ((float)Actions[CurrentAction].NumAnimationKeys - 1.f);
	}

	if (Actions[CurrentAction].Loop)
	{
		if (*AnimationFrame >= (float)Actions[CurrentAction].NumAnimationKeys)
		{
			*AnimationFrame = (float)Actions[CurrentAction].NumAnimationKeys - 0.01f;
			Loop = false;
		}
	}
	else
	{
		int Key;
		if (Actions[CurrentAction].LockPositions)
			Key = Actions[CurrentAction].NumAnimationKeys - 1;
		else
			Key = Actions[CurrentAction].NumAnimationKeys;

		float fTemp;

		if (SceneFlag == 4)
		{
			fTemp = *AnimationFrame + 2;
		}
		else if (gMapManager->currentMap == WD_39KANTURU_3RD && CurrentAction == MONSTER01_APEAR)
		{
			fTemp = *AnimationFrame + 1;
		}
		else
		{
			fTemp = *AnimationFrame;
		}

		if (fTemp >= (int)Key)
		{
			int Frame = (int)*AnimationFrame;
			*AnimationFrame = (float)(Frame % (Key)) + (*AnimationFrame - (float)Frame);
			Loop = false;
		}
	}
	CurrentAnimation = *AnimationFrame;
	CurrentAnimationFrame = (int)CurrentAnimation;
	return Loop;
}

void BMD::Transform(float(*BoneMatrix)[3][4], vec3_t BoundingBoxMin, vec3_t BoundingBoxMax, OBB_t* OBB, bool Translate, float _Scale)
{
	vec3_t LightPosition;

	if (LightEnable)
	{
		vec3_t Position;

		float Matrix[3][4];
		if (HighLight)
		{
			Vector(1.3f, 0.f, 2.f, Position);
		}
		else if (gMapManager->InBattleCastle())
		{
			Vector(0.5f, -1.f, 1.f, Position);
			Vector(0.f, 0.f, -45.f, ShadowAngle);
		}
		else
		{
			Vector(0.f, -1.5f, 0.f, Position);
		}

		AngleMatrix(ShadowAngle, Matrix);
		VectorIRotate(Position, Matrix, LightPosition);
	}
	vec3_t BoundingMin;
	vec3_t BoundingMax;
	Vector(0.0, 0.0, 0.0, BoundingMin);
	Vector(0.0, 0.0, 0.0, BoundingMax);

#ifdef _DEBUG
#else
	if (EditFlag == 2)
#endif
	{
		Vector(999999.f, 999999.f, 999999.f, BoundingMin);
		Vector(-999999.f, -999999.f, -999999.f, BoundingMax);
	}
	for (int i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];
		for (int j = 0; j < m->NumVertices; j++)
		{
			Vertex_t* v = &m->Vertices[j];
			float* vp = VertexTransform[i][j];

			if (BoneScale == 1.f)
			{
#ifdef PBG_ADD_NEWCHAR_MONK_ITEM
				if (_Scale)
				{
					vec3_t Position;
					VectorCopy(v->Position, Position);
					VectorScale(Position, _Scale, Position);
					VectorTransform(Position, BoneMatrix[v->Node], vp);
				}
				else
#endif //PBG_ADD_NEWCHAR_MONK_ITEM
					VectorTransform(v->Position, BoneMatrix[v->Node], vp);
				if (Translate)
					VectorScale(vp, BodyScale, vp);
			}
			else
			{
				VectorRotate(v->Position, BoneMatrix[v->Node], vp);
				vp[0] = vp[0] * BoneScale + BoneMatrix[v->Node][0][3];
				vp[1] = vp[1] * BoneScale + BoneMatrix[v->Node][1][3];
				vp[2] = vp[2] * BoneScale + BoneMatrix[v->Node][2][3];
				if (Translate)
					VectorScale(vp, BodyScale, vp);
			}
#ifdef _DEBUG
#else
			if (EditFlag == 2)
#endif
			{
				for (int k = 0; k < 3; k++)
				{
					if (vp[k] < BoundingMin[k]) BoundingMin[k] = vp[k];
					if (vp[k] > BoundingMax[k]) BoundingMax[k] = vp[k];
				}
			}
			if (Translate)
				VectorAdd(vp, BodyOrigin, vp);
		}

		for (int j = 0; j < m->NumNormals; j++)
		{
			Normal_t* sn = &m->Normals[j];
			float* tn = NormalTransform[i][j];
			VectorRotate(sn->Normal, BoneMatrix[sn->Node], tn);
			if (LightEnable)
			{
				float Luminosity;
				Luminosity = DotProduct(tn, LightPosition) * 0.8f + 0.4f;

				if (Luminosity < 0.2f) Luminosity = 0.2f;
				IntensityTransform[i][j] = Luminosity;
			}
		}
	}
	if (EditFlag == 2)
	{
		VectorCopy(BoundingMin, OBB->StartPos);
		OBB->XAxis[0] = (BoundingMax[0] - BoundingMin[0]);
		OBB->YAxis[1] = (BoundingMax[1] - BoundingMin[1]);
		OBB->ZAxis[2] = (BoundingMax[2] - BoundingMin[2]);
	}
	else
	{
		VectorCopy(BoundingBoxMin, OBB->StartPos);
		OBB->XAxis[0] = (BoundingBoxMax[0] - BoundingBoxMin[0]);
		OBB->YAxis[1] = (BoundingBoxMax[1] - BoundingBoxMin[1]);
		OBB->ZAxis[2] = (BoundingBoxMax[2] - BoundingBoxMin[2]);
	}
	fTransformedSize = max(max(BoundingMax[0] - BoundingMin[0], BoundingMax[1] - BoundingMin[1]), BoundingMax[2] - BoundingMin[2]);

	//fTransformedSize *= 0.3f;
	VectorAdd(OBB->StartPos, BodyOrigin, OBB->StartPos);
	OBB->XAxis[1] = 0.f;
	OBB->XAxis[2] = 0.f;
	OBB->YAxis[0] = 0.f;
	OBB->YAxis[2] = 0.f;
	OBB->ZAxis[0] = 0.f;
	OBB->ZAxis[1] = 0.f;
}

void BMD::TransformByObjectBone(vec3_t vResultPosition, OBJECT* pObject, int iBoneNumber, vec3_t vRelativePosition)
{
	if (iBoneNumber >= 0 && iBoneNumber < NumBones && pObject)
	{
		vec3_t vOutTemp;
		float(*TransformMatrix)[4];

		TransformMatrix = pObject->GetBoneTransform(iBoneNumber);

		if (TransformMatrix == NULL)
		{
			TransformMatrix = BoneTransform[iBoneNumber];
		}

		if (vRelativePosition)
		{
			VectorTransform(vRelativePosition, TransformMatrix, vOutTemp);
		}
		else
		{
			vOutTemp[0] = TransformMatrix[0][3];
			vOutTemp[1] = TransformMatrix[1][3];
			vOutTemp[2] = TransformMatrix[2][3];
		}

		VectorScale(vOutTemp, BodyScale, vOutTemp);
		VectorAdd(vOutTemp, pObject->Position, vResultPosition);
	}
	else
	{
		assert(!"Empty Bone");
	}
}

void BMD::TransformByBoneMatrix(vec3_t vResultPosition, float(*BoneMatrix)[4], vec3_t vWorldPosition, vec3_t vRelativePosition)
{
	if (BoneMatrix == NULL)
	{
		assert(!"Empty Matrix");
		return;
	}

	vec3_t vTemp;
	if (vRelativePosition == NULL)
	{
		vTemp[0] = BoneMatrix[0][3];
		vTemp[1] = BoneMatrix[1][3];
		vTemp[2] = BoneMatrix[2][3];
	}
	else
	{
		VectorTransform(vRelativePosition, BoneMatrix, vTemp);
	}
	if (vWorldPosition != NULL)
	{
		VectorScale(vTemp, BodyScale, vTemp);
		VectorAdd(vTemp, vWorldPosition, vResultPosition);
	}
	else
	{
		VectorScale(vTemp, BodyScale, vResultPosition);
	}
}

void BMD::TransformPosition(float(*Matrix)[4], vec3_t Position, vec3_t WorldPosition, bool Translate)
{
	if (Translate)
	{
		vec3_t p;
		VectorTransform(Position, Matrix, p);
		VectorScale(p, BodyScale, p);
		VectorAdd(p, BodyOrigin, WorldPosition);
	}
	else
	{
		VectorTransform(Position, Matrix, WorldPosition);
	}
}

void BMD::RotationPosition(float(*Matrix)[4], vec3_t Position, vec3_t WorldPosition)
{
	vec3_t p;
	VectorRotate(Position, Matrix, p);
	VectorScale(p, BodyScale, WorldPosition);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ParentMatrix[i][j] = Matrix[i][j];
		}
	}
}


void BMD::AnimationTransformWithAttachHighModel_usingGlobalTM(OBJECT* oHighHierarchyModel, BMD* bmdHighHierarchyModel, int iBoneNumberHighHierarchyModel, vec3_t& vOutPosHighHiearachyModelBone, vec3_t* arrOutSetfAllBonePositions, bool bApplyTMtoVertices)
{
	if (NumBones < 1) return;
	if (NumBones > MAX_BONES) return;

	vec34_t* arrBonesTMLocal;

	vec34_t		tmBoneHierarchicalObject;

	vec3_t		Temp, v3Position;
	OBB_t		OBB;

	arrBonesTMLocal = new vec34_t[NumBones];
	Vector(0.0f, 0.0f, 0.0f, Temp);

	memset(arrBonesTMLocal, 0, sizeof(vec34_t) * NumBones);
	memset(tmBoneHierarchicalObject, 0, sizeof(vec34_t));

	memcpy(tmBoneHierarchicalObject, oHighHierarchyModel->BoneTransform[iBoneNumberHighHierarchyModel], sizeof(vec34_t));
	BodyScale = oHighHierarchyModel->Scale;

	tmBoneHierarchicalObject[0][3] = tmBoneHierarchicalObject[0][3] * BodyScale;
	tmBoneHierarchicalObject[1][3] = tmBoneHierarchicalObject[1][3] * BodyScale;
	tmBoneHierarchicalObject[2][3] = tmBoneHierarchicalObject[2][3] * BodyScale;

	if (NULL != vOutPosHighHiearachyModelBone)
	{
		Vector(tmBoneHierarchicalObject[0][3], tmBoneHierarchicalObject[1][3], tmBoneHierarchicalObject[2][3],
			vOutPosHighHiearachyModelBone);
	}

	VectorCopy(oHighHierarchyModel->Position, v3Position);

	Animation(arrBonesTMLocal, 0, 0, 0, Temp, Temp, false, false);

	for (int i_ = 0; i_ < NumBones; ++i_)
	{
		R_ConcatTransforms(tmBoneHierarchicalObject, arrBonesTMLocal[i_], BoneTransform[i_]);
		BoneTransform[i_][0][3] = BoneTransform[i_][0][3] + v3Position[0];
		BoneTransform[i_][1][3] = BoneTransform[i_][1][3] + v3Position[1];
		BoneTransform[i_][2][3] = BoneTransform[i_][2][3] + v3Position[2];

		Vector(BoneTransform[i_][0][3],
			BoneTransform[i_][1][3],
			BoneTransform[i_][2][3],
			arrOutSetfAllBonePositions[i_]);
	}

	if (true == bApplyTMtoVertices)
	{
		Transform(BoneTransform, Temp, Temp, &OBB, false);
	}

	delete[] arrBonesTMLocal;
}

void BMD::AnimationTransformWithAttachHighModel(OBJECT* oHighHierarchyModel, BMD* bmdHighHierarchyModel, int iBoneNumberHighHierarchyModel, vec3_t& vOutPosHighHiearachyModelBone, vec3_t* arrOutSetfAllBonePositions)
{
	if (NumBones < 1)
		return;
	if (NumBones > MAX_BONES)
		return;

	vec34_t* arrBonesTMLocal;
	vec34_t* arrBonesTMLocalResult;
	vec34_t	tmBoneHierarchicalObject;
	vec3_t	Temp, v3Position;

	arrBonesTMLocal = new vec34_t[NumBones];
	Vector(0.0f, 0.0f, 0.0f, Temp);

	arrBonesTMLocalResult = new vec34_t[NumBones];

	memset(arrBonesTMLocalResult, 0, sizeof(vec34_t) * NumBones);
	memset(arrBonesTMLocal, 0, sizeof(vec34_t) * NumBones);

	memset(tmBoneHierarchicalObject, 0, sizeof(vec34_t));

	memcpy(tmBoneHierarchicalObject, oHighHierarchyModel->BoneTransform[iBoneNumberHighHierarchyModel], sizeof(vec34_t));

	BodyScale = oHighHierarchyModel->Scale;

	tmBoneHierarchicalObject[0][3] = tmBoneHierarchicalObject[0][3] * BodyScale;
	tmBoneHierarchicalObject[1][3] = tmBoneHierarchicalObject[1][3] * BodyScale;
	tmBoneHierarchicalObject[2][3] = tmBoneHierarchicalObject[2][3] * BodyScale;

	if (NULL != vOutPosHighHiearachyModelBone)
	{
		Vector(tmBoneHierarchicalObject[0][3], tmBoneHierarchicalObject[1][3], tmBoneHierarchicalObject[2][3],
			vOutPosHighHiearachyModelBone);
	}

	VectorCopy(oHighHierarchyModel->Position, v3Position);

	Animation(arrBonesTMLocal, 0, 0, 0, Temp, Temp, false, false);
	for (int i_ = 0; i_ < NumBones; ++i_)
	{
		R_ConcatTransforms(tmBoneHierarchicalObject, arrBonesTMLocal[i_], arrBonesTMLocalResult[i_]);
		arrBonesTMLocalResult[i_][0][3] = arrBonesTMLocalResult[i_][0][3] + v3Position[0];
		arrBonesTMLocalResult[i_][1][3] = arrBonesTMLocalResult[i_][1][3] + v3Position[1];
		arrBonesTMLocalResult[i_][2][3] = arrBonesTMLocalResult[i_][2][3] + v3Position[2];

		Vector(arrBonesTMLocalResult[i_][0][3], arrBonesTMLocalResult[i_][1][3], arrBonesTMLocalResult[i_][2][3], arrOutSetfAllBonePositions[i_]);
	}

	delete[] arrBonesTMLocalResult;
	delete[] arrBonesTMLocal;
}

void BMD::AnimationTransformOnlySelf(vec3_t* arrOutSetfAllBonePositions, const OBJECT* oSelf)
{
	if (NumBones < 1)
		return;

	if (NumBones > MAX_BONES)
		return;

	vec34_t* arrBonesTMLocal;

	vec3_t		Temp;

	arrBonesTMLocal = new vec34_t[NumBones];
	Vector(0.0f, 0.0f, 0.0f, Temp);

	memset(arrBonesTMLocal, 0, sizeof(vec34_t) * NumBones);

	Animation(arrBonesTMLocal, oSelf->AnimationFrame, oSelf->PriorAnimationFrame, oSelf->PriorAction, (const_cast<OBJECT*>(oSelf))->Angle, Temp, false, true);

	for (int i_ = 0; i_ < NumBones; ++i_)
	{
		Vector(arrBonesTMLocal[i_][0][3], arrBonesTMLocal[i_][1][3], arrBonesTMLocal[i_][2][3], arrOutSetfAllBonePositions[i_]);
	}
	delete[] arrBonesTMLocal;
}

void BMD::AnimationTransformOnlySelf(vec3_t* arrOutSetfAllBonePositions,
	const vec3_t& v3Angle,
	const vec3_t& v3Position,
	const float& fScale,
	OBJECT* oRefAnimation,
	const float fFrameArea,
	const float fWeight)
{
	if (NumBones < 1)
		return;

	if (NumBones > MAX_BONES)
		return;

	vec34_t* arrBonesTMLocal;
	vec3_t v3RootAngle, v3RootPosition;
	float fRootScale;
	vec3_t Temp;

	fRootScale = const_cast<float&>(fScale);

	v3RootAngle[0] = v3Angle[0];
	v3RootAngle[1] = v3Angle[1];
	v3RootAngle[2] = v3Angle[2];

	v3RootPosition[0] = v3Position[0];
	v3RootPosition[1] = v3Position[1];
	v3RootPosition[2] = v3Position[2];

	arrBonesTMLocal = new vec34_t[NumBones];
	Vector(0.0f, 0.0f, 0.0f, Temp);

	memset(arrBonesTMLocal, 0, sizeof(vec34_t) * NumBones);

	if (NULL == oRefAnimation)
	{
		Animation(arrBonesTMLocal, 0, 0, 0, v3RootAngle, Temp, false, true);
	}
	else
	{
		float fAnimationFrame = oRefAnimation->AnimationFrame;
		float fPiriorAnimationFrame = oRefAnimation->PriorAnimationFrame;

		unsigned short	iPiriorAction = oRefAnimation->PriorAction;

		if (fWeight >= 0.0f && fFrameArea > 0.0f)
		{
			float fAnimationFrameStart = fAnimationFrame - fFrameArea;
			float fAnimationFrameEnd = fAnimationFrame;
			LInterpolationF(fAnimationFrame, fAnimationFrameStart, fAnimationFrameEnd, fWeight);
		}

		Animation(arrBonesTMLocal, fAnimationFrame, fPiriorAnimationFrame, iPiriorAction, v3RootAngle, Temp, false, true);
	}

	vec3_t	v3RelatePos;
	Vector(1.0f, 1.0f, 1.0f, v3RelatePos);
	for (int i_ = 0; i_ < NumBones; ++i_)
	{
		Vector(arrBonesTMLocal[i_][0][3],
			arrBonesTMLocal[i_][1][3],
			arrBonesTMLocal[i_][2][3],
			arrOutSetfAllBonePositions[i_]);
	}

	delete[] arrBonesTMLocal;
}

void BMD::Chrome(float* pchrome, int bone, vec3_t normal)
{
	Vector(0.f, 0.f, 1.f, g_vright);

	float n;

	//if (g_chromeage[bone] != g_smodels_total)
	{
		// calculate vectors from the viewer to the bone. This roughly adjusts for position
		vec3_t chromeupvec;		// g_chrome t vector in world reference frame
		vec3_t chromerightvec;	// g_chrome s vector in world reference frame
		vec3_t tmp;				// vector pointing at bone in world reference frame
		VectorScale(BodyOrigin, -1, tmp);
		//tmp[0] += BoneMatrix[bone][0][3];
		//tmp[1] += BoneMatrix[bone][1][3];
		//tmp[2] += BoneMatrix[bone][2][3];
		//tmp[0] += LinkBoneMatrix[0][3];
		//tmp[1] += LinkBoneMatrix[1][3];
		//tmp[2] += LinkBoneMatrix[2][3];
		VectorNormalize(tmp);
		CrossProduct(tmp, g_vright, chromeupvec);
		VectorNormalize(chromeupvec);
		CrossProduct(tmp, chromeupvec, chromerightvec);
		VectorNormalize(chromerightvec);

		//VectorIRotate( chromeupvec, BoneMatrix[bone], g_chromeup[bone] );
		//VectorIRotate( chromerightvec, BoneMatrix[bone], g_chromeright[bone] );
		//VectorIRotate( chromeupvec, LinkBoneMatrix, g_chromeup[bone] );
		//VectorIRotate( chromerightvec, LinkBoneMatrix, g_chromeright[bone] );

		g_chromeage[bone] = g_smodels_total;
	}

	// calc s coord
	n = DotProduct(normal, g_chromeright[bone]);
	pchrome[0] = (n + 1.f); // FIX: make this a float

	// calc t coord
	n = DotProduct(normal, g_chromeup[bone]);
	pchrome[1] = (n + 1.f); // FIX: make this a float
}

void BMD::Lighting(float* pLight, Light_t* lp, vec3_t Position, vec3_t Normal)
{
	vec3_t Light;
	VectorSubtract(lp->Position, Position, Light);
	float Length = sqrtf(Light[0] * Light[0] + Light[1] * Light[1] + Light[2] * Light[2]);

	float LightCos = (DotProduct(Normal, Light) / Length) * 0.8f + 0.3f;

	if (Length > lp->Range)
		LightCos -= (Length - lp->Range) * 0.01f;

	if (LightCos < 0.f)
		LightCos = 0.f;

	pLight[0] += LightCos * lp->Color[0];
	pLight[1] += LightCos * lp->Color[1];
	pLight[2] += LightCos * lp->Color[2];
}

///////////////////////////////////////////////////////////////////////////////
// light map
///////////////////////////////////////////////////////////////////////////////

#define AXIS_X  0
#define AXIS_Y  1
#define AXIS_Z  2

float SubPixel = 16.f;

void SmoothBitmap(int Width, int Height, unsigned char* Buffer)
{
	int RowStride = Width * 3;
	for (int i = 1; i < Height - 1; i++)
	{
		for (int j = 1; j < Width - 1; j++)
		{
			int Index = (i * Width + j) * 3;
			for (int k = 0; k < 3; k++)
			{
				Buffer[Index] = (Buffer[Index - RowStride - 3] + Buffer[Index - RowStride] + Buffer[Index - RowStride + 3] +
					Buffer[Index - 3] + Buffer[Index + 3] +
					Buffer[Index + RowStride - 3] + Buffer[Index + RowStride] + Buffer[Index + RowStride + 3]) / 8;
				Index++;
			}
		}
	}
}

bool BMD::CollisionDetectLineToMesh(vec3_t Position, vec3_t Target, bool Collision, int Mesh, int Triangle)
{
	int i, j;
	for (i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];

		for (j = 0; j < m->NumTriangles; j++)
		{
			if (i == Mesh && j == Triangle)
				continue;

			Triangle_t* tp = &m->Triangles[j];
			float* vp1 = VertexTransform[i][tp->VertexIndex[0]];
			float* vp2 = VertexTransform[i][tp->VertexIndex[1]];
			float* vp3 = VertexTransform[i][tp->VertexIndex[2]];
			float* vp4 = VertexTransform[i][tp->VertexIndex[3]];

			vec3_t Normal;
			FaceNormalize(vp1, vp2, vp3, Normal);
			bool success = CollisionDetectLineToFace(Position, Target, tp->Polygon, vp1, vp2, vp3, vp4, Normal, Collision);
			if (success == true) return true;
		}
	}
	return false;
}

void BMD::CreateLightMapSurface(Light_t* lp, Mesh_t* m, int i, int j, int MapWidth, int MapHeight, int MapWidthMax, int MapHeightMax, vec3_t BoundingMin, vec3_t BoundingMax, int Axis)
{
	int k, l;
	Triangle_t* tp = &m->Triangles[j];
	float* np = NormalTransform[i][tp->NormalIndex[0]];
	float* vp = VertexTransform[i][tp->VertexIndex[0]];
	float d = -DotProduct(vp, np);

	Bitmap_t* lmp = &LightMaps[NumLightMaps];
	if (lmp->Buffer == NULL)
	{
		lmp->Width = MapWidthMax;
		lmp->Height = MapHeightMax;
		int BufferBytes = lmp->Width * lmp->Height * 3;
		lmp->Buffer = new unsigned char[BufferBytes];
		memset(lmp->Buffer, 0, BufferBytes);
	}

	for (k = 0; k < MapHeight; k++)
	{
		for (l = 0; l < MapWidth; l++)
		{
			vec3_t p;
			Vector(0.f, 0.f, 0.f, p);
			switch (Axis)
			{
			case AXIS_Z:
				p[0] = BoundingMin[0] + l * SubPixel;
				p[1] = BoundingMin[1] + k * SubPixel;
				if (p[0] >= BoundingMax[0]) p[0] = BoundingMax[0];
				if (p[1] >= BoundingMax[1]) p[1] = BoundingMax[1];
				p[2] = (np[0] * p[0] + np[1] * p[1] + d) / -np[2];
				break;
			case AXIS_Y:
				p[0] = BoundingMin[0] + (float)l * SubPixel;
				p[2] = BoundingMin[2] + (float)k * SubPixel;
				if (p[0] >= BoundingMax[0]) p[0] = BoundingMax[0];
				if (p[2] >= BoundingMax[2]) p[2] = BoundingMax[2];
				p[1] = (np[0] * p[0] + np[2] * p[2] + d) / -np[1];
				break;
			case AXIS_X:
				p[2] = BoundingMin[2] + l * SubPixel;
				p[1] = BoundingMin[1] + k * SubPixel;
				if (p[2] >= BoundingMax[2]) p[2] = BoundingMax[2];
				if (p[1] >= BoundingMax[1]) p[1] = BoundingMax[1];
				p[0] = (np[2] * p[2] + np[1] * p[1] + d) / -np[0];
				break;
			}
			vec3_t Direction;
			VectorSubtract(p, lp->Position, Direction);
			VectorNormalize(Direction);
			VectorSubtract(p, Direction, p);
			bool success = CollisionDetectLineToMesh(lp->Position, p, true, i, j);
			/*if(success == true)
			{
				DisableTexture();
				glBegin(GL_LINES);
				glColor3fv(lp->Color);
				glVertex3fv(p);
				glVertex3fv(lp->Position);
				glEnd();
			}*/
			if (success == false)
			{
				unsigned char* Bitmap = &lmp->Buffer[(k * MapWidthMax + l) * 3];
				vec3_t Light;
				Vector(0.f, 0.f, 0.f, Light);
				Lighting(Light, lp, p, np);
				for (int c = 0; c < 3; c++)
				{
					int Color;
					Color = Bitmap[c];
					Color += (unsigned char)(Light[c] * 255.f);
					if (Color > 255) Color = 255;
					Bitmap[c] = Color;
				}
			}
		}
	}
}

void BMD::CreateLightMaps()
{
}

void BMD::BindLightMaps()
{
	if (LightMapEnable == true)
		return;

	for (int i = 0; i < NumLightMaps; i++)
	{
		Bitmap_t* lmp = &LightMaps[i];
		if (lmp->Buffer != NULL)
		{
			SmoothBitmap(lmp->Width, lmp->Height, lmp->Buffer);
			SmoothBitmap(lmp->Width, lmp->Height, lmp->Buffer);

			glBindTexture(GL_TEXTURE_2D, i + IndexLightMap);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, lmp->Width, lmp->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, lmp->Buffer);
		}
	}
	LightMapEnable = true;
}

void BMD::ReleaseLightMaps()
{
	if (LightMapEnable == false)
		return;

	for (int i = 0; i < NumLightMaps; i++)
	{
		Bitmap_t* lmp = &LightMaps[i];
		if (lmp->Buffer != NULL)
		{
			delete lmp->Buffer;
			lmp->Buffer = NULL;
		}
	}
	LightMapEnable = false;
}

void BMD::BeginRender(float Alpha)
{
	glPushMatrix();
}

void BMD::EndRender()
{
	glPopMatrix();
}

bool BMD::runtime_linkaction(PART_t* f, OBJECT* pObject)
{
	return false;
	if (unknown != 0 && commandLua != NULL)
	{
		if (commandLua->RenderLinkAction(f, pObject))
		{
			return true;
		}
	}
	return false;
}

bool BMD::runtime_render(OBJECT* pObject, float Alpha, int RenderType, BMD* oHighHierarchyModel)
{
	return false;
	if (unknown != 0 && commandLua != NULL)
	{
		if (commandLua->RenderPartObjectBody(this, pObject, Alpha, RenderType))
		{
			return true;
		}
	}

	return false;
}

bool BMD::runtime_render_effect(OBJECT* pObject, float Alpha, int RenderType, int Texture, BMD* oHighHierarchyModel)
{
	return false;
	if (unknown != 0 && commandLua != NULL)
	{
		if (commandLua->RenderEffectObjectBody(this, pObject, Alpha, RenderType, Texture))
		{
			return true;
		}
	}
	return false;
}

void BMD::RenderMeshEffect(int i, int iType, int iSubType, vec3_t Angle, VOID* obj)
{
	if (i >= NumMeshs || i < 0)
		return;

	Mesh_t* m = &Meshs[i];
	if (m->NumTriangles <= 0)
		return;

	vec3_t angle, Light;
	int iEffectCount = 0;

	Vector(0.f, 0.f, 0.f, angle);
	Vector(1.f, 1.f, 1.f, Light);

	for (int j = 0; j < m->NumTriangles; j++)
	{
		Triangle_t* tp = &m->Triangles[j];
		for (int k = 0; k < tp->Polygon; k++)
		{
			int vi = tp->VertexIndex[k];

			switch (iType)
			{
			case MODEL_STONE_COFFIN:
				if (iSubType == 0)
				{
					if ((rand() % 2) == 0)
					{
						CreateEffectSync(MODEL_STONE_COFFIN + 1, VertexTransform[i][vi], angle, Light);
					}
					if ((rand() % 10) == 0)
					{
						CreateEffectSync(MODEL_STONE_COFFIN, VertexTransform[i][vi], angle, Light);
					}
				}
				else if (iSubType == 1)
				{
					CreateEffectSync(MODEL_STONE_COFFIN + 1, VertexTransform[i][vi], angle, Light, 2);
				}
				else if (iSubType == 2)
				{
					CreateEffectSync(MODEL_STONE_COFFIN + 1, VertexTransform[i][vi], angle, Light, 3);
				}
				else if (iSubType == 3)
				{
					CreateEffectSync(MODEL_STONE_COFFIN + rand() % 2, VertexTransform[i][vi], angle, Light, 4);
				}
				break;
			case MODEL_GATE:
				if (iSubType == 1)
				{
					Vector(0.2f, 0.2f, 0.2f, Light);
					if ((rand() % 5) == 0)
					{
						CreateEffectSync(MODEL_GATE + 1, VertexTransform[i][vi], angle, Light, 2);
					}
					if ((rand() % 10) == 0)
					{
						CreateEffectSync(MODEL_GATE, VertexTransform[i][vi], angle, Light, 2);
					}
				}
				else if (iSubType == 0)
				{
					Vector(0.2f, 0.2f, 0.2f, Light);
					if ((rand() % 12) == 0)
					{
						CreateEffectSync(MODEL_GATE + 1, VertexTransform[i][vi], angle, Light);
					}
					if ((rand() % 50) == 0)
					{
						CreateEffectSync(MODEL_GATE, VertexTransform[i][vi], angle, Light);
					}
				}
				break;
			case MODEL_BIG_STONE_PART1:
				if ((rand() % 3) == 0)
				{
					CreateEffectSync(MODEL_BIG_STONE_PART1 + rand() % 2, VertexTransform[i][vi], angle, Light, 1);
				}
				break;

			case MODEL_BIG_STONE_PART2:
				if ((rand() % 3) == 0)
				{
					CreateEffectSync(MODEL_BIG_STONE_PART1 + rand() % 2, VertexTransform[i][vi], angle, Light);
				}
				break;

			case MODEL_WALL_PART1:
				if ((rand() % 3) == 0)
				{
					CreateEffectSync(MODEL_WALL_PART1 + rand() % 2, VertexTransform[i][vi], angle, Light);
				}
				break;

			case MODEL_GATE_PART1:
				Vector(0.2f, 0.2f, 0.2f, Light);
				if ((rand() % 12) == 0)
				{
					CreateEffectSync(MODEL_GATE_PART1 + 1, VertexTransform[i][vi], angle, Light);
				}
				if ((rand() % 40) == 0)
				{
					CreateEffectSync(MODEL_GATE_PART1, VertexTransform[i][vi], angle, Light);
				}
				if ((rand() % 40) == 0)
				{
					CreateEffectSync(MODEL_GATE_PART1 + 2, VertexTransform[i][vi], angle, Light);
				}
				break;
			case MODEL_GOLEM_STONE:
				if ((rand() % 45) == 0 && iEffectCount < 20)
				{
					if (iSubType == 0)
					{
						CreateEffectSync(MODEL_GOLEM_STONE, VertexTransform[i][vi], angle, Light);
					}
					else if (iSubType == 1)
					{
						CreateEffectSync(MODEL_BIG_STONE_PART1, VertexTransform[i][vi], angle, Light, 2);
						CreateEffectSync(MODEL_BIG_STONE_PART2, VertexTransform[i][vi], angle, Light, 2);
					}
					iEffectCount++;
				}
				break;
			case MODEL_SKIN_SHELL:
				if ((rand() % 8) == 0)
				{
					CreateEffectSync(MODEL_SKIN_SHELL, VertexTransform[i][vi], angle, Light, iSubType);
				}
				break;
			case BITMAP_LIGHT:
				Vector(0.08f, 0.08f, 0.08f, Light);
				if (iSubType == 0)
				{
					CreateSprite(BITMAP_LIGHT, VertexTransform[i][vi], BodyScale, Light, NULL);
				}
				else if (iSubType == 1)
				{
					Vector(1.f, 0.8f, 0.2f, Light);
					if ((j % 22) == 0)
					{
						OBJECT* o = (OBJECT*)obj;
						angle[0] = -(float)(rand() % 90);
						angle[1] = 0.f;
						angle[2] = Angle[2] + (float)(rand() % 120 - 60);
						CreateJointSync(BITMAP_JOINT_SPIRIT, VertexTransform[i][vi], o->Position, angle, 13, o, 20.f, 0, 0);
					}
				}
				break;
			case BITMAP_BUBBLE:
				Vector(1.f, 1.f, 1.f, Light);
				if ((rand() % 30) == 0)
				{
					CreateParticleSync(BITMAP_BUBBLE, VertexTransform[i][vi], angle, Light, 2);
				}
				break;
			}
		}
	}
}

void BMD::RenderMesh(int i, int RenderFlag, float Alpha, int BlendMesh, float BlendMeshLight, float BlendMeshTexCoordU, float BlendMeshTexCoordV, int MeshTexture)
{
	if (i >= 0 && i < NumMeshs)
	{
		Mesh_t* m = &Meshs[i];

		if (m->NumTriangles)
		{
			float Wave = (int)WorldTime % 10000 * 0.0001f;
			int Texture = IndexTexture[m->Texture];

			if (RenderFlag != RENDER_TEXTURE && (Texture == BITMAP_SKIN
				|| Texture == BITMAP_HQSKIN
				|| Texture == BITMAP_HQSKIN2
				|| Texture == BITMAP_HQSKIN3
				|| Texture == BITMAP_HAIR
				|| Texture == BITMAP_HQHAIR)
				)
			{
				return;
			}

			//if ((Texture == BITMAP_SKIN
			//	|| Texture == BITMAP_HQSKIN
			//	|| Texture == BITMAP_HQSKIN2
			//	|| Texture == BITMAP_HQSKIN3
			//	|| Texture == BITMAP_HAIR
			//	|| Texture == BITMAP_HQHAIR)
			//	)
			//{
			//	g_ConsoleDebug->Write(5, "[BITMAP: %d][SKIN: %d]", Texture, Skin);
			//}

			if (Texture != BITMAP_HIDE)
			{
				switch (Texture)
				{
				case BITMAP_SKIN:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_SKIN;
					break;
				case BITMAP_HQSKIN:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_HQSKIN;
					break;
				case BITMAP_HQSKIN2:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_HQSKIN2;
					break;
				case BITMAP_HQSKIN3:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_HQSKIN3;
					break;
				case BITMAP_WATER:
					if (HideSkin == true)
						return;
					Texture = (int)WaterTextureNumber + BITMAP_WATER;
					break;
				case BITMAP_HAIR:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_HAIR;
					break;
				case BITMAP_HQHAIR:
					if (HideSkin == true)
						return;
					Texture = Skin + BITMAP_HQHAIR;
					break;
				}

				if (MeshTexture != -1)
					Texture = MeshTexture;

				if (Texture != -1)
				{
					if (LightTexture[m->Texture] != 0 && (HideSkin || RenderFlag != RENDER_TEXTURE))
					{
						return;
					}

					BITMAP_t* pBitmap = Bitmaps.GetTexture(Texture);

					bool EnableWave = false;
					int streamMesh = StreamMesh;

					if (m->m_csTScript && m->m_csTScript->getStreamMesh())
						streamMesh = i;

					if ((i == BlendMesh || i == streamMesh) && (BlendMeshTexCoordU != 0.0 || BlendMeshTexCoordV != 0.0))
						EnableWave = true;

					bool EnableLight = LightEnable;

					if (i == StreamMesh)
					{
						glColor3fv(BodyLight);
						EnableLight = false;
					}
					else if (EnableLight)
					{
						for (int j = 0; j < m->NumNormals; j++)
						{
							VectorScale(BodyLight, IntensityTransform[i][j], LightTransform[i][j]);
						}
					}

					int renderFlags = RenderFlag;

					if (RenderFlag & RENDER_COLOR)
					{
						renderFlags = RENDER_COLOR;
						if (RenderFlag & RENDER_BRIGHT)
						{
							EnableAlphaBlend();
						}
						else if (RenderFlag & RENDER_DARK)
						{
							EnableAlphaBlendMinus();
						}
						else
						{
							DisableAlphaBlend();
						}
						if (RenderFlag & RENDER_NODEPTH)
							DisableDepthTest();
						DisableTexture(false);

						if (Alpha < 0.99)
						{
							EnableAlphaTest(true);
							glColor4f(BodyLight[0], BodyLight[1], BodyLight[2], Alpha);
						}
						else
						{
							Alpha = 1.f;
							glColor3fv(BodyLight);
						}
					}
					else if (RenderFlag & RENDER_CHROME
						|| RenderFlag & RENDER_METAL
						|| RenderFlag & RENDER_CHROME2
						|| RenderFlag & RENDER_CHROME3
						|| RenderFlag & RENDER_CHROME4
						|| RenderFlag & RENDER_CHROME5
						|| RenderFlag & RENDER_OIL
						|| RenderFlag & RENDER_CHROME6
						|| RenderFlag & RENDER_CHROME7
						|| RenderFlag & RENDER_CHROME8)
					{
						if ((m->m_csTScript && m->m_csTScript->getNoneBlendMesh()) || m->NoneBlendMesh)
							return;

						renderFlags = RENDER_CHROME;
						if (RenderFlag & RENDER_CHROME4)
							renderFlags = RENDER_CHROME4;
						if (RenderFlag & RENDER_CHROME8)
							renderFlags = RENDER_CHROME8;
						if (RenderFlag & RENDER_OIL)
							renderFlags = RENDER_OIL;


						float Wave2 = ((int)WorldTime % 5000) * 0.00024f - 0.4f;

						vec3_t L;
						L[0] = (float)(cos(WorldTime * 0.001f));
						L[1] = (float)(sin(WorldTime * 0.002f));
						L[2] = 1.0;

						for (int j = 0; j < m->NumNormals && j <= MAX_VERTICES; ++j)
						{
							float* Normal = NormalTransform[i][j];

							if (RenderFlag & RENDER_CHROME2)
							{
								g_chrome[j][0] = (Normal[2] + Normal[0]) * 0.8f + Wave2 * 2.f;
								g_chrome[j][1] = (Normal[1] + Normal[0]) * 1.0f + Wave2 * 3.f;
							}
							else if (RenderFlag & RENDER_CHROME3)
							{
								g_chrome[j][0] = DotProduct(Normal, LightVector);
								g_chrome[j][1] = 1.f - DotProduct(Normal, LightVector);
							}
							else if (RenderFlag & RENDER_CHROME4)
							{
								g_chrome[j][0] = DotProduct(Normal, L);
								g_chrome[j][1] = 1.f - DotProduct(Normal, L);
								g_chrome[j][1] -= ((Normal[2] * 0.5f + Wave * 3.f));
								g_chrome[j][0] += ((Normal[1] * 0.5f + L[1] * 3.f));
							}
							else if (RenderFlag & RENDER_CHROME5)
							{
								g_chrome[j][0] = DotProduct(Normal, L);
								g_chrome[j][1] = 1.f - DotProduct(Normal, L);
								g_chrome[j][1] -= ((Normal[2] * 2.5f + Wave * 1.f));
								g_chrome[j][0] += ((Normal[1] * 3.f + L[1] * 5.f));
							}
							else if (RenderFlag & RENDER_CHROME6)
							{
								g_chrome[j][0] = (Normal[2] + Normal[0]) * 0.8f + Wave2 * 2.f;
								g_chrome[j][1] = (Normal[2] + Normal[0]) * 0.8f + Wave2 * 2.f;
							}
							else if (RenderFlag & RENDER_CHROME7)
							{
								g_chrome[j][0] = (Normal[2] + Normal[0]) * 0.8f + WorldTime * 0.00006f;
								g_chrome[j][1] = (Normal[2] + Normal[0]) * 0.8f + WorldTime * 0.00006f;
							}
							else if (RenderFlag & RENDER_CHROME8)
							{
								g_chrome[j][0] = Normal[0];
								g_chrome[j][1] = Normal[1];
							}
							else if (RenderFlag & RENDER_OIL)
							{
								g_chrome[j][0] = Normal[0];
								g_chrome[j][1] = Normal[1];
							}
							else if (RenderFlag & RENDER_CHROME)
							{
								g_chrome[j][0] = Normal[2] * 0.5f + Wave;
								g_chrome[j][1] = Normal[1] * 0.5f + Wave * 2.f;
							}
							else
							{
								g_chrome[j][0] = Normal[2] * 0.5f + 0.2f;
								g_chrome[j][1] = Normal[1] * 0.5f + 0.5f;
							}
						}

						if (RenderFlag & RENDER_CHROME3 || RenderFlag & RENDER_CHROME4 || RenderFlag & RENDER_CHROME5 || RenderFlag & RENDER_CHROME7 || RenderFlag & RENDER_CHROME8)
						{
							if (Alpha < 0.99f)
							{
								BodyLight[0] *= (Alpha);
								BodyLight[1] *= (Alpha);
								BodyLight[2] *= (Alpha);
							}
							EnableAlphaBlend();
						}
						else if (RenderFlag & RENDER_BRIGHT)
						{
							if (Alpha < 0.99f)
							{
								BodyLight[0] *= (Alpha);
								BodyLight[1] *= (Alpha);
								BodyLight[2] *= (Alpha);
							}
							EnableAlphaBlend();
						}
						else if (RenderFlag & RENDER_DARK)
						{
							EnableAlphaBlendMinus();
						}
						else if (RenderFlag & RENDER_LIGHTMAP)
						{
							EnableLightMap();
						}
						else if (Alpha < 0.99f)
						{
							EnableAlphaTest();
						}
						else
						{
							DisableAlphaBlend();
						}

						if (RenderFlag & RENDER_NODEPTH)
						{
							DisableDepthTest();
						}

						if (RenderFlag & RENDER_CHROME2 && MeshTexture == -1)
						{
							BindTexture(BITMAP_CHROME2);
						}
						else if (RenderFlag & RENDER_CHROME3 && MeshTexture == -1)
						{
							BindTexture(BITMAP_CHROME2);
						}
						else if (RenderFlag & RENDER_CHROME4 && MeshTexture == -1)
						{
							BindTexture(BITMAP_CHROME2);
						}
						else if (RenderFlag & RENDER_CHROME6 && MeshTexture == -1)
						{
							BindTexture(BITMAP_CHROME6);
						}
						else if (RenderFlag & RENDER_CHROME && MeshTexture == -1)
						{
							BindTexture(BITMAP_CHROME);
						}
						else if (RenderFlag & RENDER_METAL && MeshTexture == -1)
						{
							BindTexture(BITMAP_SHINY);
						}
						else
						{
							BindTexture(Texture);
						}
					}
					else if (BlendMesh > -2 && m->Texture != BlendMesh)
					{
						if (RenderFlag & 2)
						{
							renderFlags = RENDER_TEXTURE;
							BindTexture(Texture);

							if (RenderFlag & RENDER_BRIGHT)
							{
								EnableAlphaBlend();
							}
							else if (RenderFlag & RENDER_DARK)
							{
								EnableAlphaBlendMinus();
							}
							else if (Alpha > 0.99f && pBitmap->Components != 4)
							{
								DisableAlphaBlend();
							}
							else
							{
								if (pBitmap->Components == 4 && pBitmap->HasNonBinaryAlpha)
								{
									EnableAlphaBlend3();
								}
								else
								{
									EnableAlphaTest();
								}
							}

							if (RenderFlag & RENDER_NODEPTH)
								DisableDepthTest();
						}
						else if (RenderFlag & RENDER_BRIGHT)
						{
							if (pBitmap->Components == 4 || m->Texture == BlendMesh)
								return;

							renderFlags = RENDER_BRIGHT;

							EnableAlphaBlend();
							DisableTexture();
							DisableDepthMask();

							if (RenderFlag & RENDER_NODEPTH)
								DisableDepthTest();
						}
						else
						{
							renderFlags = RENDER_TEXTURE;
						}
					}
					else
					{
						renderFlags = RENDER_TEXTURE;
						BindTexture(Texture);

						if (RenderFlag & RENDER_DARK)
							EnableAlphaBlendMinus();
						else
							EnableAlphaBlend();

						if (RenderFlag & RENDER_NODEPTH)
							DisableDepthTest();

						glColor3f(BodyLight[0] * BlendMeshLight, BodyLight[1] * BlendMeshLight, BodyLight[2] * BlendMeshLight);
						EnableLight = false;
					}

					if (RenderFlag & RENDER_DOPPELGANGER && pBitmap->Components != 4)
					{
						EnableCullFace();
						EnableDepthMask();
					}

					const bool hasSpecialBlendFlag =
						(RenderFlag & RENDER_BRIGHT) ||
						(RenderFlag & RENDER_DARK) ||
						(RenderFlag & RENDER_LIGHTMAP) ||
						(RenderFlag & RENDER_CHROME) ||
						(RenderFlag & RENDER_CHROME2) ||
						(RenderFlag & RENDER_CHROME3) ||
						(RenderFlag & RENDER_CHROME4) ||
						(RenderFlag & RENDER_CHROME5) ||
						(RenderFlag & RENDER_CHROME6) ||
						(RenderFlag & RENDER_CHROME7) ||
						(RenderFlag & RENDER_CHROME8) ||
						(RenderFlag & RENDER_OIL);

					// Textures with non-binary alpha need true alpha blending (not cutout)
					// to avoid dark/black fringes and opaque background artifacts.
					if (renderFlags == RENDER_TEXTURE &&
						pBitmap &&
						pBitmap->Components == 4 &&
						pBitmap->HasNonBinaryAlpha &&
						!hasSpecialBlendFlag)
					{
						EnableAlphaBlend3();
					}

#ifdef __ANDROID__
					if (pBitmap && pBitmap->Components == 4 && !AlphaTestEnable && AlphaBlendType == 0)
					{
						// RGBA texture about to be drawn without alpha handling —
						// enable alpha test to prevent black-background artifacts.
						if (pBitmap->HasNonBinaryAlpha)
							EnableAlphaBlend3();
						else
							EnableAlphaTest();

						static int s_rgbaFixupCount = 0;
						if (s_rgbaFixupCount < 40)
						{
							++s_rgbaFixupCount;
							__android_log_print(ANDROID_LOG_WARN, "MURender",
								"RGBA alpha fixup: model=%s mesh=%d tex=%s renderFlags=0x%X alpha=%.3f nonBinary=%d",
								FileName, i, pBitmap->FileName, renderFlags, Alpha,
								pBitmap->HasNonBinaryAlpha ? 1 : 0);
						}
					}
#endif

					auto vertices = RenderArrayVertices;
					auto colors = RenderArrayColors;
					auto textCoords = RenderArrayTexCoords;

					int target_vertex_index = -1;

					bool enableColor = (EnableLight && renderFlags == RENDER_TEXTURE)
						|| renderFlags == RENDER_CHROME
						|| renderFlags == RENDER_CHROME4
						|| renderFlags == RENDER_CHROME8
						|| renderFlags == RENDER_OIL;

					for (int j = 0; j < m->NumTriangles; j++)
					{
						Triangle_t* triangle = &m->Triangles[j];

						for (int k = 0; k < triangle->Polygon; k++)
						{
							int source_vertex_index = triangle->VertexIndex[k];

							target_vertex_index++;

							VectorCopy(VertexTransform[i][source_vertex_index], vertices[target_vertex_index]);

							colors[target_vertex_index][3] = Alpha;
							colors[target_vertex_index][0] = BodyLight[0];
							colors[target_vertex_index][1] = BodyLight[1];
							colors[target_vertex_index][2] = BodyLight[2];

							int normalIndex = triangle->NormalIndex[k];

							TexCoord_t* textcoord = &m->TexCoords[triangle->TexCoordIndex[k]];
							textCoords[target_vertex_index][0] = textcoord->TexCoordU;
							textCoords[target_vertex_index][1] = textcoord->TexCoordV;

							switch (renderFlags)
							{
							case RENDER_TEXTURE:
								if (EnableWave)
								{
									textCoords[target_vertex_index][0] += BlendMeshTexCoordU;
									textCoords[target_vertex_index][1] += BlendMeshTexCoordV;
								}
								if (EnableLight)
								{
									colors[target_vertex_index][3] = Alpha;
									colors[target_vertex_index][0] = LightTransform[i][normalIndex][0];
									colors[target_vertex_index][1] = LightTransform[i][normalIndex][1];
									colors[target_vertex_index][2] = LightTransform[i][normalIndex][2];
								}
								break;
							case RENDER_OIL:
								textCoords[target_vertex_index][0] = (g_chrome[normalIndex][0] * textcoord->TexCoordU) + BlendMeshTexCoordU;
								textCoords[target_vertex_index][1] = (g_chrome[normalIndex][1] * textcoord->TexCoordV) + BlendMeshTexCoordV;
								break;
							case RENDER_CHROME:
								textCoords[target_vertex_index][0] = g_chrome[normalIndex][0];
								textCoords[target_vertex_index][1] = g_chrome[normalIndex][1];
								break;
							case RENDER_CHROME4:
							case RENDER_CHROME8:
								textCoords[target_vertex_index][0] = g_chrome[normalIndex][0] + BlendMeshTexCoordU;
								textCoords[target_vertex_index][1] = g_chrome[normalIndex][1] + BlendMeshTexCoordV;
								break;
							}

							if (RenderFlag & RENDER_SHADOWMAP)
							{
								float sx = 2000.f;
								float sy = 4000.f;

								if (gMapManager->InBattleCastle())
								{
									sx = 2500.f;
									sy = 4000.f;
								}

								vec3_t pos;
								VectorSubtract(vertices[target_vertex_index], BodyOrigin, pos);
								pos[0] += pos[2] * (pos[0] + sx) / (pos[2] - sy);
								pos[2] = 5.f;
								VectorAdd(pos, BodyOrigin, pos);
							}
							else if (RenderFlag & RENDER_WAVE)
							{
								float time_sin = sinf((float)((int)WorldTime + source_vertex_index * 931) * 0.007f) * 28.0f;
								float* normal = NormalTransform[i][normalIndex];

								for (int iCoord = 0; iCoord < 3; ++iCoord)
								{
									vertices[target_vertex_index][iCoord] += normal[iCoord] * time_sin;
								}
							}
						}
					}

					if (target_vertex_index != -1)
					{
#ifdef SHADER_VERSION_TEST
						this->RenderVertexBuffer(i, m, target_vertex_index + 1, vertices, textCoords, colors);
#else
						glEnableClientState(GL_VERTEX_ARRAY);
						if (enableColor) glEnableClientState(GL_COLOR_ARRAY);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);

						glVertexPointer(3, GL_FLOAT, 0, vertices);
						if (enableColor) glColorPointer(4, GL_FLOAT, 0, colors);
						glTexCoordPointer(2, GL_FLOAT, 0, textCoords);
						glDrawArrays(GL_TRIANGLES, 0, m->NumTriangles * 3);

						glDisableClientState(GL_TEXTURE_COORD_ARRAY);
						if (enableColor) glDisableClientState(GL_COLOR_ARRAY);
						glDisableClientState(GL_VERTEX_ARRAY);
#endif // SHADER_VERSION_TEST
					}
				}
			}
		}
	}
}

void BMD::RenderBody(int Flag, float Alpha, int BlendMesh, float BlendMeshLight, float BlendMeshTexCoordU, float BlendMeshTexCoordV, int HiddenMesh, int Texture)
{
	if (NumMeshs)
	{
		int iBlendMesh = BlendMesh;

		this->BeginRender(Alpha);

		if (!LightEnable)
		{
			if (Alpha < 0.99)
				glColor4f(BodyLight[0], BodyLight[1], BodyLight[2], Alpha);
			else
				glColor3fv(BodyLight);
		}

		for (int i = 0; i < NumMeshs; i++)
		{
			Mesh_t* m = &Meshs[i];

			iBlendMesh = BlendMesh;

			if (m->m_csTScript)
			{
				if (!m->m_csTScript->getHiddenMesh() && i != HiddenMesh)
				{
					if (m->m_csTScript->getBright())
						iBlendMesh = i;

					this->RenderMesh(i, Flag, Alpha, iBlendMesh, BlendMeshLight, BlendMeshTexCoordU, BlendMeshTexCoordV, Texture);

					BYTE shadowType = m->m_csTScript->getShadowMesh();

					if (shadowType == SHADOW_RENDER_COLOR)
					{
						DisableAlphaBlend();
						if (Alpha < 0.99)
							glColor4f(0.f, 0.f, 0.f, Alpha);
						else
							glColor3f(0.f, 0.f, 0.f);

						this->RenderMesh(i, RENDER_COLOR | RENDER_SHADOWMAP, Alpha, iBlendMesh, BlendMeshLight, BlendMeshTexCoordU, BlendMeshTexCoordV);
						glColor3f(1.f, 1.f, 1.f);
					}
					else if (shadowType == SHADOW_RENDER_TEXTURE)
					{
						DisableAlphaBlend();
						if (Alpha < 0.99)
							glColor4f(0.f, 0.f, 0.f, Alpha);
						else
							glColor3f(0.f, 0.f, 0.f);

						this->RenderMesh(i, RENDER_TEXTURE | RENDER_SHADOWMAP, Alpha, iBlendMesh, BlendMeshLight, BlendMeshTexCoordU, BlendMeshTexCoordV);
						glColor3f(1.f, 1.f, 1.f);
					}
				}
			}
			else if (i != HiddenMesh)
			{
				this->RenderMesh(i, Flag, Alpha, BlendMesh, BlendMeshLight, BlendMeshTexCoordU, BlendMeshTexCoordV, Texture);
			}
		}
		this->EndRender();
	}
}

void BMD::RenderMeshTranslate(int i, int RenderFlag, float Alpha, int BlendMesh, float BlendMeshLight, float BlendMeshTexCoordU, float BlendMeshTexCoordV, int MeshTexture)
{
	if (i >= NumMeshs || i < 0)
		return;

	Mesh_t* m = &Meshs[i];
	if (m->NumTriangles == 0)
		return;
	float Wave = (int)WorldTime % 10000 * 0.0001f;

	int Texture = IndexTexture[m->Texture];
	if (Texture == BITMAP_HIDE)
		return;
	else if (Texture == BITMAP_SKIN)
	{
		if (HideSkin) return;
		Texture = BITMAP_SKIN + Skin;
	}
	else if (Texture == BITMAP_WATER)
	{
		Texture = BITMAP_WATER + (int)WaterTextureNumber;
	}
	if (MeshTexture != -1)
		Texture = MeshTexture;

	BITMAP_t* pBitmap = Bitmaps.GetTexture(Texture);

	bool EnableWave = false;
	int streamMesh = StreamMesh;
	if (m->m_csTScript != NULL)
	{
		if (m->m_csTScript->getStreamMesh())
		{
			streamMesh = i;
		}
	}
	if ((i == BlendMesh || i == streamMesh) && (BlendMeshTexCoordU != 0.f || BlendMeshTexCoordV != 0.f))
		EnableWave = true;

	bool EnableLight = LightEnable;
	if (i == StreamMesh)
	{
		//vec3_t Light;
		//Vector(1.f,1.f,1.f,Light);
		glColor3fv(BodyLight);
		EnableLight = false;
	}
	else if (EnableLight)
	{
		for (int j = 0; j < m->NumNormals; j++)
		{
			VectorScale(BodyLight, IntensityTransform[i][j], LightTransform[i][j]);
		}
	}

	int Render = RenderFlag;
	if ((RenderFlag & RENDER_COLOR) == RENDER_COLOR)
	{
		Render = RENDER_COLOR;
		if ((RenderFlag & RENDER_BRIGHT) == RENDER_BRIGHT)
			EnableAlphaBlend();
		else if ((RenderFlag & RENDER_DARK) == RENDER_DARK)
			EnableAlphaBlendMinus();
		else
			DisableAlphaBlend();
		DisableTexture();
		glColor3fv(BodyLight);
	}
	else if ((RenderFlag & RENDER_CHROME) == RENDER_CHROME
		|| (RenderFlag & RENDER_METAL) == RENDER_METAL
		|| (RenderFlag & RENDER_CHROME2) == RENDER_CHROME2
		|| (RenderFlag & RENDER_CHROME6) == RENDER_CHROME6
		)
	{
		if (m->m_csTScript != NULL)
		{
			if (m->m_csTScript->getNoneBlendMesh()) return;
		}
		if (m->NoneBlendMesh)
			return;
		Render = RENDER_CHROME;

		float Wave2 = (int)WorldTime % 5000 * 0.00024f - 0.4f;

		for (int j = 0; j < m->NumNormals; j++)
		{
			//			Normal_t *np = &m->Normals[j];
			if (j > MAX_VERTICES) break;
			float* Normal = NormalTransform[i][j];

			if ((RenderFlag & RENDER_CHROME2) == RENDER_CHROME2)
			{
				g_chrome[j][0] = (Normal[2] + Normal[0]) * 0.8f + Wave2 * 2.f;
				g_chrome[j][1] = (Normal[1] + Normal[0]) * 1.0f + Wave2 * 3.f;
			}
			else if ((RenderFlag & RENDER_CHROME) == RENDER_CHROME)
			{
				g_chrome[j][0] = Normal[2] * 0.5f + Wave;
				g_chrome[j][1] = Normal[1] * 0.5f + Wave * 2.f;
			}
			else if ((RenderFlag & RENDER_CHROME6) == RENDER_CHROME6)
			{
				g_chrome[j][0] = (Normal[2] + Normal[0]) * 0.8f + Wave2 * 2.f;
				g_chrome[j][1] = (Normal[1] + Normal[0]) * 1.0f + Wave2 * 3.f;
			}
			else
			{
				g_chrome[j][0] = Normal[2] * 0.5f + 0.2f;
				g_chrome[j][1] = Normal[1] * 0.5f + 0.5f;
			}
		}

		if ((RenderFlag & RENDER_BRIGHT) == RENDER_BRIGHT)
			EnableAlphaBlend();
		else if ((RenderFlag & RENDER_DARK) == RENDER_DARK)
			EnableAlphaBlendMinus();
		else if ((RenderFlag & RENDER_LIGHTMAP) == RENDER_LIGHTMAP)
			EnableLightMap();
		else
			DisableAlphaBlend();

		if ((RenderFlag & RENDER_CHROME2) == RENDER_CHROME2 && MeshTexture == -1)
		{
			BindTexture(BITMAP_CHROME2);
		}
		else if ((RenderFlag & RENDER_CHROME) == RENDER_CHROME && MeshTexture == -1)
			BindTexture(BITMAP_CHROME);
		else if ((RenderFlag & RENDER_METAL) == RENDER_METAL && MeshTexture == -1)
			BindTexture(BITMAP_SHINY);
		else
			BindTexture(Texture);
	}
	else if (BlendMesh <= -2 || m->Texture == BlendMesh)
	{
		Render = RENDER_TEXTURE;
		BindTexture(Texture);
		if ((RenderFlag & RENDER_DARK) == RENDER_DARK)
			EnableAlphaBlendMinus();
		else
			EnableAlphaBlend();
		glColor3f(BodyLight[0] * BlendMeshLight, BodyLight[1] * BlendMeshLight, BodyLight[2] * BlendMeshLight);
		//glColor3f(BlendMeshLight,BlendMeshLight,BlendMeshLight);
		EnableLight = false;
	}
	else if ((RenderFlag & RENDER_TEXTURE) == RENDER_TEXTURE)
	{
		Render = RENDER_TEXTURE;
		BindTexture(Texture);
		if ((RenderFlag & RENDER_BRIGHT) == RENDER_BRIGHT)
		{
			EnableAlphaBlend();
		}
		else if ((RenderFlag & RENDER_DARK) == RENDER_DARK)
		{
			EnableAlphaBlendMinus();
		}
		else if (pBitmap->Components == 4)
		{
			if (pBitmap->HasNonBinaryAlpha)
			{
				EnableAlphaBlend3();
			}
			else
			{
				EnableAlphaTest();
			}
		}
		else if (Alpha < 0.99f)
		{
			EnableAlphaTest();
		}
		else
		{
			DisableAlphaBlend();
		}
	}
	else if ((RenderFlag & RENDER_BRIGHT) == RENDER_BRIGHT)
	{
		if (pBitmap->Components == 4 || m->Texture == BlendMesh)
		{
			return;
		}
		Render = RENDER_BRIGHT;
		EnableAlphaBlend();
		DisableTexture();
		DisableDepthMask();
	}
	else
	{
		Render = RENDER_TEXTURE;
	}

	const bool hasSpecialBlendFlag =
		(RenderFlag & RENDER_BRIGHT) ||
		(RenderFlag & RENDER_DARK) ||
		(RenderFlag & RENDER_LIGHTMAP) ||
		(RenderFlag & RENDER_CHROME) ||
		(RenderFlag & RENDER_CHROME2) ||
		(RenderFlag & RENDER_CHROME6);

	if (Render == RENDER_TEXTURE &&
		pBitmap &&
		pBitmap->Components == 4 &&
		pBitmap->HasNonBinaryAlpha &&
		!hasSpecialBlendFlag)
	{
		EnableAlphaBlend3();
	}

#ifdef __ANDROID__
	if (pBitmap && pBitmap->Components == 4 && !AlphaTestEnable && AlphaBlendType == 0)
	{
		static int s_rgbaNoAlphaStateWarnCountTranslate = 0;
		if (s_rgbaNoAlphaStateWarnCountTranslate < 80)
		{
			++s_rgbaNoAlphaStateWarnCountTranslate;
			__android_log_print(ANDROID_LOG_WARN, "MURender",
				"RGBA texture without alpha state (translate): model=%s mesh=%d tex=%s render=0x%X alpha=%.3f",
				FileName, i, pBitmap->FileName, Render, Alpha);
		}
	}
#endif

	glBegin(GL_TRIANGLES);
	for (int j = 0; j < m->NumTriangles; j++)
	{
		vec3_t  pos;
		Triangle_t* tp = &m->Triangles[j];
		for (int k = 0; k < tp->Polygon; k++)
		{
			int vi = tp->VertexIndex[k];
			switch (Render)
			{
			case RENDER_TEXTURE:
			{
				TexCoord_t* texp = &m->TexCoords[tp->TexCoordIndex[k]];
				if (EnableWave)
					glTexCoord2f(texp->TexCoordU + BlendMeshTexCoordU, texp->TexCoordV + BlendMeshTexCoordV);
				else
					glTexCoord2f(texp->TexCoordU, texp->TexCoordV);
				if (EnableLight)
				{
					int ni = tp->NormalIndex[k];
					if (Alpha >= 0.99f)
					{
						glColor3fv(LightTransform[i][ni]);
					}
					else
					{
						float* Light = LightTransform[i][ni];
						glColor4f(Light[0], Light[1], Light[2], Alpha);
					}
				}
				break;
			}
			case RENDER_CHROME:
			{
				if (Alpha >= 0.99f)
					glColor3fv(BodyLight);
				else
					glColor4f(BodyLight[0], BodyLight[1], BodyLight[2], Alpha);
				int ni = tp->NormalIndex[k];
				glTexCoord2f(g_chrome[ni][0], g_chrome[ni][1]);
				break;
			}
			}
			{
				VectorAdd(VertexTransform[i][vi], BodyOrigin, pos);
				glVertex3fv(pos);
			}
		}
	}
	glEnd();
}

void BMD::RenderBodyTranslate(int Flag, float Alpha, int BlendMesh, float BlendMeshLight, float BlendMeshTexCoordU, float BlendMeshTexCoordV, int HiddenMesh, int Texture)
{
	if (NumMeshs == 0) return;

	BeginRender(Alpha);
	if (!LightEnable)
	{
		if (Alpha >= 0.99f)
			glColor3fv(BodyLight);
		else
			glColor4f(BodyLight[0], BodyLight[1], BodyLight[2], Alpha);
	}
	for (int i = 0; i < NumMeshs; i++)
	{
		if (i != HiddenMesh)
		{
			RenderMeshTranslate(i, Flag, Alpha, BlendMesh, BlendMeshLight, BlendMeshTexCoordU, BlendMeshTexCoordV, Texture);
		}
	}
	EndRender();
}

__forceinline void CalcShadowPosition(vec3_t* position, const vec3_t origin, const float sx, const float sy)
{
	vec3_t result;
	VectorCopy(*position, result);

	// Subtract the origin (position of the character) from the current position of the vertex
	// The result is the relative coordinate of the vertex to the origin.
	VectorSubtract(result, origin, result);

	// scale the shadow in the x direction
	result[0] += result[2] * (result[0] + sx) / (result[2] - sy);

	// Add the origin again, to get the absolute coordinate of the vertex again
	VectorAdd(result, origin, result);

	// put it on the ground by adding 5 to the actual ground coordinate.
	result[2] = RequestTerrainHeight(result[0], result[1]) + 5.f;

	// copy to result
	VectorCopy(result, *position);
}

__forceinline void GetClothShadowPosition(vec3_t* target, CPhysicsCloth* pCloth, const int index, const vec3_t origin, const float sx, const float sy)
{
	pCloth->GetPosition(index, target);
	CalcShadowPosition(target, origin, sx, sy);
}

void BMD::AddClothesShadowTriangles(void* pClothes, const int clothesCount, const float sx, const float sy) const
{
	auto vertices = RenderArrayVertices;
	int target_vertex_index = -1;

	for (int i = 0; i < clothesCount; i++)
	{
		auto* const pCloth = &static_cast<CPhysicsCloth*>(pClothes)[i];
		auto const columns = pCloth->GetVerticalCount();
		auto const rows = pCloth->GetHorizontalCount();

		for (int col = 0; col < columns - 1; ++col)
		{
			for (int row = 0; row < rows - 1; ++row)
			{
				// first we take each point for an square from which we derive
				// a A-Triangle and the V-Triangle.
				int a = rows * col + row;
				int b = rows * (col + 1) + row;
				int c = rows * col + row + 1;
				int d = rows * (col + 1) + row + 1;

				vec3_t posA, posB, posC, posD;

				GetClothShadowPosition(&posA, pCloth, a, BodyOrigin, sx, sy);
				GetClothShadowPosition(&posB, pCloth, b, BodyOrigin, sx, sy);
				GetClothShadowPosition(&posC, pCloth, c, BodyOrigin, sx, sy);
				GetClothShadowPosition(&posD, pCloth, d, BodyOrigin, sx, sy);

				// A-Triangle:
				target_vertex_index++;
				VectorCopy(posA, vertices[target_vertex_index]);
				target_vertex_index++;
				VectorCopy(posB, vertices[target_vertex_index]);
				target_vertex_index++;
				VectorCopy(posC, vertices[target_vertex_index]);

				// V-Triangle:
				target_vertex_index++;
				VectorCopy(posD, vertices[target_vertex_index]);
				target_vertex_index++;
				VectorCopy(posB, vertices[target_vertex_index]);
				target_vertex_index++;
				VectorCopy(posC, vertices[target_vertex_index]);
			}
		}
	}

	if (target_vertex_index < 0)
	{
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_TRIANGLES, 0, target_vertex_index + 1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void BMD::AddMeshShadowTriangles(const int blendMesh, const int hiddenMesh, const int startMesh, const int endMesh, const float sx, const float sy) const
{
	auto vertices = RenderArrayVertices;
	int target_vertex_index = -1;

	for (int i = startMesh; i < endMesh; i++)
	{
		if (i == hiddenMesh)
		{
			continue;
		}

		const Mesh_t* mesh = &Meshs[i];
		if (mesh->NumTriangles <= 0 || mesh->Texture == blendMesh)
		{
			continue;
		}

		for (int j = 0; j < mesh->NumTriangles; j++)
		{
			const auto* tp = &mesh->Triangles[j];
			for (int k = 0; k < tp->Polygon; k++)
			{
				const int source_vertex_index = tp->VertexIndex[k];
				target_vertex_index++;

				VectorCopy(VertexTransform[i][source_vertex_index], vertices[target_vertex_index]);
				CalcShadowPosition(&vertices[target_vertex_index], BodyOrigin, sx, sy);
			}
		}
	}

	if (target_vertex_index < 0)
	{
		return;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_TRIANGLES, 0, target_vertex_index + 1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void BMD::RenderBodyShadow(int BlendMesh, int HiddenMesh, int StartMeshNumber, int EndMeshNumber, void* pClothes, int ClothesCount)
{
	if (NumMeshs == 0)
		return;

	DisableTexture();
	DisableDepthMask();
	BeginRender(1.f);

	int startMesh = 0;
	int endMesh = NumMeshs;

	if (StartMeshNumber != -1)
		startMesh = StartMeshNumber;

	if (EndMeshNumber != -1)
		endMesh = EndMeshNumber;

	float sx = 2000.f;
	float sy = 4000.f;

	if (gMapManager->InBattleCastle())
	{
		sx = 2500.f;
		sy = 4000.f;
	}

	if (ClothesCount == 0)
	{
		AddMeshShadowTriangles(BlendMesh, HiddenMesh, startMesh, endMesh, sx, sy);
	}
	else
	{
		AddClothesShadowTriangles(pClothes, ClothesCount, sx, sy);
	}

	EndRender();
	EnableDepthMask();
	glDisable(GL_STENCIL_TEST);
}

void BMD::RenderObjectBoundingBox()
{
	DisableTexture();
	glPushMatrix();
	glTranslatef(BodyOrigin[0], BodyOrigin[1], BodyOrigin[2]);
	glScalef(BodyScale, BodyScale, BodyScale);
	for (int i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		if (b->BoundingBox)
		{
			vec3_t BoundingVertices[8];
			for (int j = 0; j < 8; j++)
			{
				VectorTransform(b->BoundingVertices[j], BoneTransform[i], BoundingVertices[j]);
			}

			glBegin(GL_QUADS);
			//glBegin(GL_LINES);
			glColor3f(0.2f, 0.2f, 0.2f);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[7]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[6]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[4]);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[5]);

			glColor3f(0.2f, 0.2f, 0.2f);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[0]);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[2]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[3]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[1]);

			glColor3f(0.6f, 0.6f, 0.6f);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[7]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[3]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[2]);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[6]);

			glColor3f(0.6f, 0.6f, 0.6f);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[0]);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[1]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[5]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[4]);

			glColor3f(0.4f, 0.4f, 0.4f);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[7]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[5]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[1]);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[3]);

			glColor3f(0.4f, 0.4f, 0.4f);
			glTexCoord2f(0.0F, 1.0F);
			glVertex3fv(BoundingVertices[0]);
			glTexCoord2f(1.0F, 1.0F);
			glVertex3fv(BoundingVertices[4]);
			glTexCoord2f(1.0F, 0.0F);
			glVertex3fv(BoundingVertices[6]);
			glTexCoord2f(0.0F, 0.0F);
			glVertex3fv(BoundingVertices[2]);
			glEnd();
		}
	}
	glPopMatrix();
	DisableAlphaBlend();
}

void BMD::RenderBone(float(*BoneMatrix)[3][4])
{
	DisableTexture();
	glDepthFunc(GL_ALWAYS);
	glColor3f(0.8f, 0.8f, 0.2f);
	for (int i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		if (!b->Dummy)
		{
			BoneMatrix_t* bm = &b->BoneMatrixes[CurrentAction];
			int Parent = b->Parent;
			if (Parent > 0)
			{
				float Scale = 1.f;
				float dx = bm->Position[CurrentAnimationFrame][0];
				float dy = bm->Position[CurrentAnimationFrame][1];
				float dz = bm->Position[CurrentAnimationFrame][2];
				Scale = sqrtf(dx * dx + dy * dy + dz * dz) * 0.05f;
				vec3_t Position[3];
				Vector(0.f, 0.f, -Scale, Position[0]);
				Vector(0.f, 0.f, Scale, Position[1]);
				Vector(0.f, 0.f, 0.f, Position[2]);
				vec3_t BoneVertices[3];
				VectorTransform(Position[0], BoneMatrix[Parent], BoneVertices[0]);
				VectorTransform(Position[1], BoneMatrix[Parent], BoneVertices[1]);
				VectorTransform(Position[2], BoneMatrix[i], BoneVertices[2]);
				for (int j = 0; j < 3; j++)
				{
					VectorMA(BodyOrigin, BodyScale, BoneVertices[j], BoneVertices[j]);
				}
				glBegin(GL_LINES);
				glVertex3fv(BoneVertices[0]);
				glVertex3fv(BoneVertices[1]);
				glEnd();
				glBegin(GL_LINES);
				glVertex3fv(BoneVertices[1]);
				glVertex3fv(BoneVertices[2]);
				glEnd();
				glBegin(GL_LINES);
				glVertex3fv(BoneVertices[2]);
				glVertex3fv(BoneVertices[0]);
				glEnd();
			}
		}
	}
	glDepthFunc(GL_LEQUAL);
}

void BlurShadow()
{
	for (int i = 1; i < ShadowBufferHeight - 1; i++)
	{
		unsigned char* ptr = &ShadowBuffer[i * ShadowBufferWidth];
		for (int j = 1; j < ShadowBufferWidth - 1; j++)
		{
			ptr[j] = (ptr[j - ShadowBufferWidth] + ptr[j + ShadowBufferWidth] +
				ptr[j - 1] + ptr[j + 1]) >> 2;
		}
	}
}

void BMD::Release()
{
	if (Bones)
	{
		for (int i = 0; i < NumBones; i++)
		{
			Bone_t* b = &Bones[i];

			if (!b->Dummy && b->BoneMatrixes)
			{
				for (int j = 0; j < NumActions; j++)
				{
					BoneMatrix_t* bm = &b->BoneMatrixes[j];
					if (bm)
					{
						SAFE_DELETE_ARRAY(bm->Position);
						SAFE_DELETE_ARRAY(bm->Rotation);
						SAFE_DELETE_ARRAY(bm->Quaternion);
					}
				}
				SAFE_DELETE_ARRAY(b->BoneMatrixes);
			}
		}
	}

	if (Actions)
	{
		for (int i = 0; i < NumActions; i++)
		{
			Action_t* a = &Actions[i];
			if (a->LockPositions)
			{
				SAFE_DELETE_ARRAY(a->Positions);
			}
		}
	}

	if (Meshs)
	{
		for (int i = 0; i < NumMeshs; i++)
		{
			Mesh_t* m = &Meshs[i];

			SAFE_DELETE_ARRAY(m->Vertices);
			SAFE_DELETE_ARRAY(m->Normals);
			SAFE_DELETE_ARRAY(m->TexCoords);
			SAFE_DELETE_ARRAY(m->Triangles);

			if (m->m_csTScript)
			{
				delete m->m_csTScript;
				m->m_csTScript = NULL;
			}
			if (IndexTexture &&
				m->Texture >= 0 &&
				m->Texture < NumMeshs &&
				IndexTexture[m->Texture] != BITMAP_SKIN &&
				IndexTexture[m->Texture] != BITMAP_HQSKIN &&
				IndexTexture[m->Texture] != BITMAP_HQSKIN2 &&
				IndexTexture[m->Texture] != BITMAP_HQSKIN3)
			{
				DeleteBitmap(IndexTexture[m->Texture]);

			}
		}
	}

	SAFE_DELETE_ARRAY(Meshs);
	SAFE_DELETE_ARRAY(Bones);
	SAFE_DELETE_ARRAY(Actions);
	SAFE_DELETE_ARRAY(Textures);
	SAFE_DELETE_ARRAY(IndexTexture);
	SAFE_DELETE_ARRAY(LightTexture);

	NumBones = 0;
	NumActions = 0;
	NumMeshs = 0;

	m_bCompletedAlloc = false;
}

void BMD::FindNearTriangle(void)
{
	for (int iMesh = 0; iMesh < NumMeshs; iMesh++)
	{
		Mesh_t* m = &Meshs[iMesh];

		Triangle_t* pTriangle = m->Triangles;
		int iNumTriangles = m->NumTriangles;
		for (int iTri = 0; iTri < iNumTriangles; ++iTri)
		{
			for (int i = 0; i < 3; ++i)
			{
				pTriangle[iTri].EdgeTriangleIndex[i] = -1;
			}
		}
		for (int iTri = 0; iTri < iNumTriangles; ++iTri)
		{
			FindTriangleForEdge(iMesh, iTri, 0);
			FindTriangleForEdge(iMesh, iTri, 1);
			FindTriangleForEdge(iMesh, iTri, 2);
		}
	}
}

void BMD::FindTriangleForEdge(int iMesh, int iTri1, int iIndex11)
{
	if (iMesh >= NumMeshs || iMesh < 0) return;

	Mesh_t* m = &Meshs[iMesh];
	Triangle_t* pTriangle = m->Triangles;

	Triangle_t* pTri1 = &pTriangle[iTri1];
	if (pTri1->EdgeTriangleIndex[iIndex11] != -1)
	{
		return;
	}

	int iNumTriangles = m->NumTriangles;
	for (int iTri2 = 0; iTri2 < iNumTriangles; ++iTri2)
	{
		if (iTri1 == iTri2)
		{
			continue;
		}

		Triangle_t* pTri2 = &pTriangle[iTri2];
		int iIndex12 = (iIndex11 + 1) % 3;
		for (int iIndex21 = 0; iIndex21 < 3; ++iIndex21)
		{
			int iIndex22 = (iIndex21 + 1) % 3;
			if (pTri2->EdgeTriangleIndex[iIndex21] == -1 &&
				pTri1->VertexIndex[iIndex11] == pTri2->VertexIndex[iIndex22] &&
				pTri1->VertexIndex[iIndex12] == pTri2->VertexIndex[iIndex21])
			{
				pTri1->EdgeTriangleIndex[iIndex11] = iTri2;
				pTri2->EdgeTriangleIndex[iIndex21] = iTri1;
				return;
			}
		}
	}
}
//#endif //USE_SHADOWVOLUME

int BMD::FindBoneName(char* name)
{
	int iNum = -1;
	if (Bones)
	{
		for (int i = 0; i < NumBones; i++)
		{
			if (Strnicmp(Bones[i].Name, name))
			{
				return i;
			}
		}
	}
	return iNum;
}

bool BMD::GetBonePosition(OBJECT* pObject, char* name, OUT vec3_t Position)
{
	bool success = false;
	vec3_t LocalPos, Relative;
	int Bone = this->FindBoneName(name);

	Vector(0, 0, 0, Relative);

	if (Bone != -1)
	{
		this->TransformPosition(pObject->BoneTransform[Bone], Relative, LocalPos, false);
		VectorScale(LocalPos, pObject->Scale, LocalPos);
		VectorAdd(LocalPos, pObject->Position, Position);
		success = true;
	}
	return success;
}

bool BMD::Open(char* DirName, char* ModelFileName)
{
	char ModelName[64];
	strcpy(ModelName, DirName);
	strcat(ModelName, ModelFileName);
	FILE* fp = MU_FOPEN(ModelName, "rb");
	if (fp == NULL)
	{
		return false;
	}
	fseek(fp, 0, SEEK_END);
	int DataBytes = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	unsigned char* Data = new unsigned char[DataBytes];
	fread(Data, 1, DataBytes, fp);
	fclose(fp);

	int Size;
	int DataPtr = 3;
	Version = *((char*)(Data + DataPtr)); DataPtr += 1;
	memcpy(Name, Data + DataPtr, 32); DataPtr += 32;
	NumMeshs = *((short*)(Data + DataPtr)); DataPtr += 2;
	NumBones = *((short*)(Data + DataPtr)); DataPtr += 2;
	NumActions = *((short*)(Data + DataPtr)); DataPtr += 2;

	Meshs = new Mesh_t[max(1, NumMeshs)];
	Bones = new Bone_t[max(1, NumBones)];
	Actions = new Action_t[max(1, NumActions)];
	Textures = new Texture_t[max(1, NumMeshs)];
	IndexTexture = new GLuint[max(1, NumMeshs)];
	LightTexture = new GLuint[max(1, NumMeshs)];

	int i;
	for (i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];
		m->NumVertices = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumNormals = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumTexCoords = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumTriangles = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->Texture = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NoneBlendMesh = false;
		//m->NumCommandBytes  = *((int   *)(Data+DataPtr));DataPtr+=4;
		m->Vertices = new Vertex_t[m->NumVertices];
		m->Normals = new Normal_t[m->NumNormals];
		m->TexCoords = new TexCoord_t[m->NumTexCoords];
		m->Triangles = new Triangle_t[m->NumTriangles];
		//m->Commands  = new unsigned char [m->NumCommandBytes];
		Size = m->NumVertices * sizeof(Vertex_t);
		memcpy(m->Vertices, Data + DataPtr, Size); DataPtr += Size;
		Size = m->NumNormals * sizeof(Normal_t);
		memcpy(m->Normals, Data + DataPtr, Size); DataPtr += Size;
		Size = m->NumTexCoords * sizeof(TexCoord_t);
		memcpy(m->TexCoords, Data + DataPtr, Size); DataPtr += Size;
		//Size = m->NumTriangles * sizeof(Triangle_t);
		//memcpy(m->Triangles,Data+DataPtr,Size);DataPtr+=Size;
		Size = sizeof(Triangle_t);
		int Size2 = sizeof(Triangle_t2);
		for (int j = 0; j < m->NumTriangles; j++)
		{
			memcpy(&m->Triangles[j], Data + DataPtr, Size); DataPtr += Size2;
		}
		//memcpy(m->Commands ,Data+DataPtr,m->NumCommandBytes);DataPtr+=m->NumCommandBytes;
		memcpy(Textures[i].FileName, Data + DataPtr, 32); DataPtr += 32;

		TextureScriptParsing TSParsing;

		if (TSParsing.parsingTScript(Textures[i].FileName))
		{
			m->m_csTScript = new TextureScript;
			m->m_csTScript->setScript((TextureScript&)TSParsing);
		}
		else
		{
			m->m_csTScript = NULL;
		}
	}
	//#ifdef USE_SHADOWVOLUME
		/*for(i=0;i<NumMeshs;i++)
		{
			Mesh_t *m = &Meshs[i];
			EdgeTriangleIndex(m->Triangles,m->NumTriangles);
		}*/
	FindNearTriangle();
	//#endif
	for (i = 0; i < NumActions; i++)
	{
		Action_t* a = &Actions[i];
		a->Loop = false;
		a->NumAnimationKeys = *((short*)(Data + DataPtr)); DataPtr += 2;
		a->LockPositions = *((bool*)(Data + DataPtr)); DataPtr += 1;
		if (a->LockPositions)
		{
			a->Positions = new vec3_t[a->NumAnimationKeys];
			Size = a->NumAnimationKeys * sizeof(vec3_t);
			memcpy(a->Positions, Data + DataPtr, Size); DataPtr += Size;
		}
	}
	for (i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		b->Dummy = *((char*)(Data + DataPtr)); DataPtr += 1;
		if (!b->Dummy)
		{
			memcpy(b->Name, Data + DataPtr, 32); DataPtr += 32;
			b->Parent = *((short*)(Data + DataPtr)); DataPtr += 2;
			b->BoneMatrixes = new BoneMatrix_t[NumActions];
			for (int j = 0; j < NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				Size = Actions[j].NumAnimationKeys * sizeof(vec3_t);
				int NumAnimationKeys = Actions[j].NumAnimationKeys;
				bm->Position = new vec3_t[NumAnimationKeys];
				bm->Rotation = new vec3_t[NumAnimationKeys];
				bm->Quaternion = new vec4_t[NumAnimationKeys];
				memcpy(bm->Position, Data + DataPtr, Size); DataPtr += Size;
				memcpy(bm->Rotation, Data + DataPtr, Size); DataPtr += Size;
				for (int k = 0; k < NumAnimationKeys; k++)
				{
					AngleQuaternion(bm->Rotation[k], bm->Quaternion[k]);
				}
			}
		}
	}
	delete[] Data;
	Init(false);
	return true;
}

bool BMD::Save(char* DirName, char* ModelFileName)
{
	char ModelName[64];
	strcpy(ModelName, DirName);
	strcat(ModelName, ModelFileName);
	FILE* fp = fopen(ModelName, "wb");
	if (fp == NULL) return false;
	putc('B', fp);
	putc('M', fp);
	putc('D', fp);
	fwrite(&Version, 1, 1, fp);
	fwrite(Name, 32, 1, fp);
	fwrite(&NumMeshs, 2, 1, fp);
	fwrite(&NumBones, 2, 1, fp);
	fwrite(&NumActions, 2, 1, fp);
	int i;
	for (i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];
		fwrite(&m->NumVertices, 2, 1, fp);
		fwrite(&m->NumNormals, 2, 1, fp);
		fwrite(&m->NumTexCoords, 2, 1, fp);
		fwrite(&m->NumTriangles, 2, 1, fp);
		fwrite(&m->Texture, 2, 1, fp);
		//fwrite(&m->NumCommandBytes ,4,1,fp);
		fwrite(m->Vertices, m->NumVertices * sizeof(Vertex_t), 1, fp);
		fwrite(m->Normals, m->NumNormals * sizeof(Normal_t), 1, fp);
		fwrite(m->TexCoords, m->NumTexCoords * sizeof(TexCoord_t), 1, fp);
		//fwrite(m->Triangles,m->NumTriangles*sizeof(Triangle_t),1,fp);
		for (int j = 0; j < m->NumTriangles; j++)
		{
			fwrite(&m->Triangles[j], sizeof(Triangle_t2), 1, fp);
		}
		//fwrite(m->Commands ,m->NumCommandBytes                ,1,fp);
		fwrite(Textures[i].FileName, 32, 1, fp);
	}
	for (i = 0; i < NumActions; i++)
	{
		Action_t* a = &Actions[i];
		fwrite(&a->NumAnimationKeys, 2, 1, fp);
		fwrite(&a->LockPositions, 1, 1, fp);
		if (a->LockPositions)
		{
			fwrite(a->Positions, a->NumAnimationKeys * sizeof(vec3_t), 1, fp);
		}
	}
	for (i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		fwrite(&b->Dummy, 1, 1, fp);
		if (!b->Dummy)
		{
			fwrite(b->Name, 32, 1, fp);
			fwrite(&b->Parent, 2, 1, fp);
			for (int j = 0; j < NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				fwrite(bm->Position, Actions[j].NumAnimationKeys * sizeof(vec3_t), 1, fp);
				fwrite(bm->Rotation, Actions[j].NumAnimationKeys * sizeof(vec3_t), 1, fp);
			}
		}
	}
	fclose(fp);
	return true;
}

void BMD::decripted_unique_lua(BYTE* DataBytes, int pos, int DataSize)
{
#ifdef __ANDROID__
	(void)DataBytes;
	(void)pos;
	(void)DataSize;
	unknown = 0;
	commandLua = NULL;
#else
	std::vector<unsigned char> binaryLua(DataSize, 0);
	memcpy(binaryLua.data(), DataBytes, DataSize);

	unknown = DataSize;
	commandLua = new CGMLuaBase;

	if (commandLua->runtime_link(binaryLua) == false)
	{
		unknown = 0;
		delete commandLua;
		commandLua = NULL;
	}
#endif
}

bool BMD::Open2(char* DirName, char* ModelFileName, bool bReAlloc)
{
	if (true == m_bCompletedAlloc)
	{
		if (true == bReAlloc)
			Release();
		else
			return true;
	}

	char ModelName[MAX_PATH];
	strcpy(ModelName, DirName);
	strcat(ModelName, ModelFileName);

	FILE* fp = MU_FOPEN(ModelName, "rb");

	if (fp == NULL)
	{
		m_bCompletedAlloc = false;
		return false;
	}
	strcpy(FileName, ModelFileName);

	fseek(fp, 0, SEEK_END);
	const long rawFileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	if (rawFileSize <= 0 || rawFileSize > (256L * 1024L * 1024L))
	{
		fclose(fp);
		m_bCompletedAlloc = false;
		return false;
	}
	const int fileSize = (int)rawFileSize;

	unsigned char* Data = new (std::nothrow) unsigned char[fileSize];
	if (Data == NULL)
	{
		fclose(fp);
		m_bCompletedAlloc = false;
		return false;
	}

	const size_t readCount = fread(Data, 1, (size_t)fileSize, fp);
	fclose(fp);
	if (readCount != (size_t)fileSize)
	{
		delete[] Data;
		Release();
		m_bCompletedAlloc = false;
		return false;
	}
	int dataSize = fileSize;

	auto failOpen = [&]() -> bool
	{
		if (Data)
		{
			delete[] Data;
			Data = NULL;
		}
		Release();
		m_bCompletedAlloc = false;
		return false;
	};

	int Size = 0;
	int DataPtr = 3;
	auto hasBytes = [&](size_t need) -> bool
	{
		if (DataPtr < 0)
		{
			return false;
		}
		return ((size_t)DataPtr + need) <= (size_t)dataSize;
	};

	if (!hasBytes(1))
	{
		return failOpen();
	}

	Version = *((char*)(Data + DataPtr));
	DataPtr += 1;

	if (Version == 12 || Version == 0xE)
	{
		if (Data[0] == 'L' && Data[1] == 'U' && Data[2] == 'A')
		{
			int pos = DataPtr;
			if ((size_t)pos + sizeof(int32_t) > (size_t)dataSize)
			{
				return failOpen();
			}
			const int32_t lSize = ReadSerializedInt32(Data + pos);
			if (lSize < 0)
			{
				return failOpen();
			}
			pos += 4;
			if ((size_t)pos + (size_t)lSize > (size_t)dataSize)
			{
				return failOpen();
			}
			pos += lSize;

			if ((size_t)pos + sizeof(int32_t) > (size_t)dataSize)
			{
				return failOpen();
			}
			const int32_t DataLen = ReadSerializedInt32(Data + pos);
			pos += 4;
			if (DataLen < 0 || ((size_t)pos + (size_t)DataLen > (size_t)dataSize))
			{
				return failOpen();
			}

			if (DataLen > 0)
			{
				decripted_unique_lua((Data + pos), 0, DataLen);
			}
		}
	}

	if (Version == 12)
	{
		if (!hasBytes(sizeof(int32_t)))
		{
			return failOpen();
		}
		const int32_t lSize = ReadSerializedInt32(Data + DataPtr); DataPtr += 4;
		if (lSize <= 0 || !hasBytes((size_t)lSize))
		{
			return failOpen();
		}
		const int32_t lDecSize = MapFileDecrypt(NULL, Data + DataPtr, lSize);
		if (lDecSize <= 0 || lDecSize > (256 * 1024 * 1024))
		{
			return failOpen();
		}
		BYTE* pbyDec = new (std::nothrow) BYTE[lDecSize];
		if (pbyDec == NULL)
		{
			return failOpen();
		}
		MapFileDecrypt(pbyDec, Data + DataPtr, lSize);
		delete[] Data;
		Data = pbyDec;
		dataSize = lDecSize;
		DataPtr = 0;
	}
	else if (Version == 0xE)
	{
		if (!hasBytes(sizeof(int32_t)))
		{
			return failOpen();
		}
		const int32_t lSize = ReadSerializedInt32(Data + DataPtr);
		DataPtr += 4;
		if (lSize <= 0 || !hasBytes((size_t)lSize))
		{
			return failOpen();
		}
		const int32_t lDecSize = MuFileDecrypt(NULL, &Data[DataPtr], lSize);
		if (lDecSize <= 0 || lDecSize > (256 * 1024 * 1024))
		{
			return failOpen();
		}
		BYTE* pbyDec = new (std::nothrow) BYTE[lDecSize];
		if (pbyDec == NULL)
		{
			return failOpen();
		}
		MuFileDecrypt(pbyDec, &Data[DataPtr], lSize);
		delete[] Data;
		Data = pbyDec;
		dataSize = lDecSize;
		DataPtr = 0;
	}

	if (!hasBytes(32 + sizeof(short) * 3))
	{
		return failOpen();
	}
	memcpy(Name, Data + DataPtr, 32); DataPtr += 32;

	NumMeshs = *((short*)(Data + DataPtr)); DataPtr += 2;
	NumBones = *((short*)(Data + DataPtr)); DataPtr += 2;
	NumActions = *((short*)(Data + DataPtr)); DataPtr += 2;
	if (NumMeshs < 0 || NumMeshs > MAX_MESH ||
		NumBones < 0 || NumBones > MAX_BONES ||
		NumActions < 0 || NumActions > 4096)
	{
		return failOpen();
	}

	Meshs = new (std::nothrow) Mesh_t[max(1, NumMeshs)];
	Bones = new (std::nothrow) Bone_t[max(1, NumBones)]();
	Actions = new (std::nothrow) Action_t[max(1, NumActions)]();
	Textures = new (std::nothrow) Texture_t[max(1, NumMeshs)]();
	IndexTexture = new (std::nothrow) GLuint[max(1, NumMeshs)]();
	LightTexture = new (std::nothrow) GLuint[max(1, NumMeshs)]();
	if (Meshs == NULL || Bones == NULL || Actions == NULL ||
		Textures == NULL || IndexTexture == NULL || LightTexture == NULL)
	{
		return failOpen();
	}

	int i;

	for (i = 0; i < NumMeshs; i++)
	{
		if (!hasBytes(sizeof(short) * 5))
		{
			return failOpen();
		}

		Mesh_t* m = &Meshs[i];
		m->NumVertices = *((short*)(Data + DataPtr));
		DataPtr += 2;

		m->NumNormals = *((short*)(Data + DataPtr));
		DataPtr += 2;

		m->NumTexCoords = *((short*)(Data + DataPtr));
		DataPtr += 2;

		m->NumTriangles = *((short*)(Data + DataPtr));
		DataPtr += 2;

		m->Texture = *((short*)(Data + DataPtr));
		DataPtr += 2;
		if (m->NumVertices < 0 || m->NumVertices > MAX_VERTICES ||
			m->NumNormals < 0 || m->NumNormals > MAX_VERTICES ||
			m->NumTexCoords < 0 || m->NumTexCoords > MAX_VERTICES ||
			m->NumTriangles < 0 || m->NumTriangles > 30000)
		{
			return failOpen();
		}

		m->NoneBlendMesh = false;
		m->Vertices = (m->NumVertices > 0) ? new (std::nothrow) Vertex_t[m->NumVertices] : NULL;
		m->Normals = (m->NumNormals > 0) ? new (std::nothrow) Normal_t[m->NumNormals] : NULL;
		m->TexCoords = (m->NumTexCoords > 0) ? new (std::nothrow) TexCoord_t[m->NumTexCoords] : NULL;
		m->Triangles = (m->NumTriangles > 0) ? new (std::nothrow) Triangle_t[m->NumTriangles] : NULL;
		if ((m->NumVertices > 0 && m->Vertices == NULL) ||
			(m->NumNormals > 0 && m->Normals == NULL) ||
			(m->NumTexCoords > 0 && m->TexCoords == NULL) ||
			(m->NumTriangles > 0 && m->Triangles == NULL))
		{
			return failOpen();
		}

		Size = m->NumVertices * (int)sizeof(Vertex_t);
		if (Size > 0)
		{
			if (!hasBytes((size_t)Size))
			{
				return failOpen();
			}
			memcpy(m->Vertices, Data + DataPtr, (size_t)Size);
			DataPtr += Size;
		}

		Size = m->NumNormals * (int)sizeof(Normal_t);
		if (Size > 0)
		{
			if (!hasBytes((size_t)Size))
			{
				return failOpen();
			}
			memcpy(m->Normals, Data + DataPtr, (size_t)Size);
			DataPtr += Size;
		}

		Size = m->NumTexCoords * (int)sizeof(TexCoord_t);
		if (Size > 0)
		{
			if (!hasBytes((size_t)Size))
			{
				return failOpen();
			}
			memcpy(m->TexCoords, Data + DataPtr, (size_t)Size);
			DataPtr += Size;
		}

		Size = sizeof(Triangle_t);
		const size_t triangleBytes = (size_t)m->NumTriangles * sizeof(Triangle_t2);
		if (!hasBytes(triangleBytes))
		{
			return failOpen();
		}

		for (int j = 0; j < m->NumTriangles; j++)
		{
			memcpy(&m->Triangles[j], Data + DataPtr, Size);
			DataPtr += sizeof(Triangle_t2);
		}

		if (!hasBytes(32))
		{
			return failOpen();
		}
		memcpy(Textures[i].FileName, Data + DataPtr, 32);
		DataPtr += 32;

		TextureScriptParsing TSParsing;

		if (TSParsing.parsingTScript(Textures[i].FileName))
		{
			m->m_csTScript = new TextureScript;
			m->m_csTScript->setScript((TextureScript&)TSParsing);
		}
		else
		{
			m->m_csTScript = NULL;
		}

#ifdef SHADER_VERSION_TEST
		this->CreateVertexBuffer(i, *m);
#endif // SHADER_VERSION_TEST
	}

	for (i = 0; i < NumActions; i++)
	{
		if (!hasBytes(sizeof(short) + sizeof(bool)))
		{
			return failOpen();
		}

		Action_t* a = &Actions[i];
		a->Loop = false;
		a->NumAnimationKeys = *((short*)(Data + DataPtr));
		DataPtr += 2;
		if (a->NumAnimationKeys < 0 || a->NumAnimationKeys > 20000)
		{
			return failOpen();
		}

		a->LockPositions = *((bool*)(Data + DataPtr));
		DataPtr += 1;

		if (a->LockPositions && a->NumAnimationKeys > 0)
		{
			a->Positions = new (std::nothrow) vec3_t[a->NumAnimationKeys];
			if (a->Positions == NULL)
			{
				return failOpen();
			}
			Size = a->NumAnimationKeys * (int)sizeof(vec3_t);
			if (!hasBytes((size_t)Size))
			{
				return failOpen();
			}
			memcpy(a->Positions, Data + DataPtr, (size_t)Size);
			DataPtr += Size;
		}
		else
		{
			a->Positions = NULL;
		}
	}

	for (i = 0; i < NumBones; i++)
	{
		if (!hasBytes(sizeof(char)))
		{
			return failOpen();
		}

		Bone_t* b = &Bones[i];
		b->Dummy = *((char*)(Data + DataPtr));
		DataPtr += 1;

		if (!b->Dummy)
		{
			if (!hasBytes(32 + sizeof(short)))
			{
				return failOpen();
			}
			memcpy(b->Name, Data + DataPtr, 32);
			DataPtr += 32;

			b->Parent = *((short*)(Data + DataPtr));
			DataPtr += 2;
			if (b->Parent >= NumBones)
			{
				b->Parent = -1;
			}

			b->BoneMatrixes = (NumActions > 0) ? new (std::nothrow) BoneMatrix_t[NumActions]() : NULL;
			if (NumActions > 0 && b->BoneMatrixes == NULL)
			{
				return failOpen();
			}

			for (int j = 0; j < NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				int NumAnimationKeys = Actions[j].NumAnimationKeys;
				if (NumAnimationKeys < 0 || NumAnimationKeys > 20000)
				{
					return failOpen();
				}

				bm->Position = (NumAnimationKeys > 0) ? new (std::nothrow) vec3_t[NumAnimationKeys] : NULL;
				bm->Rotation = (NumAnimationKeys > 0) ? new (std::nothrow) vec3_t[NumAnimationKeys] : NULL;
				bm->Quaternion = (NumAnimationKeys > 0) ? new (std::nothrow) vec4_t[NumAnimationKeys] : NULL;
				if (NumAnimationKeys > 0 &&
					(bm->Position == NULL || bm->Rotation == NULL || bm->Quaternion == NULL))
				{
					return failOpen();
				}

				Size = NumAnimationKeys * (int)sizeof(vec3_t);
				if (Size > 0)
				{
					if (!hasBytes((size_t)Size))
					{
						return failOpen();
					}
					memcpy(bm->Position, Data + DataPtr, (size_t)Size);
					DataPtr += Size;

					if (!hasBytes((size_t)Size))
					{
						return failOpen();
					}
					memcpy(bm->Rotation, Data + DataPtr, (size_t)Size);
					DataPtr += Size;
				}

				for (int k = 0; k < NumAnimationKeys; k++)
				{
					AngleQuaternion(bm->Rotation[k], bm->Quaternion[k]);
				}
			}
		}
	}

	delete[] Data;
	Data = NULL;
	Init(false);

	m_bCompletedAlloc = true;
	return true;
}

bool BMD::OpenPack(unsigned char* Data, long DataBytes, bool bReAlloc)
{
	if (true == m_bCompletedAlloc)
	{
		if (true == bReAlloc)
		{
			Release();
		}
		else
		{
			return true;
		}
	}

	if (Data == NULL || DataBytes == 0)
	{
		m_bCompletedAlloc = false;
		return false;
	}

	int Size;
	int DataPtr = 3;
	Version = *((char*)(Data + DataPtr));
	DataPtr += 1;

	if (Version == 12)
	{
		const int32_t lSize = ReadSerializedInt32(Data + DataPtr); DataPtr += 4;
		const int32_t lDecSize = MapFileDecrypt(NULL, Data + DataPtr, lSize);
		BYTE* pbyDec = new BYTE[lDecSize];
		MapFileDecrypt(pbyDec, Data + DataPtr, lSize);
		delete[] Data;
		Data = pbyDec;
		DataPtr = 0;
	}
	else if (Version == 14)
	{
		const int32_t lSize = ReadSerializedInt32(Data + DataPtr);
		DataPtr += 4;
		const int32_t lDecSize = MuFileDecrypt(NULL, &Data[DataPtr], lSize);
		BYTE* pbyDec = new BYTE[lDecSize];
		MuFileDecrypt(pbyDec, &Data[DataPtr], lSize);
		delete[] Data;
		Data = pbyDec;
		DataPtr = 0;
	}
	else if (Version == 16)
	{
		//-- aqui va el script del render
	}
	else if (Version == 18)
	{
		//-- aqui va el script del render
	}

	memcpy(Name, Data + DataPtr, 32); DataPtr += 32;

	NumMeshs = *((short*)(Data + DataPtr)); DataPtr += 2;
	NumBones = *((short*)(Data + DataPtr)); DataPtr += 2;
	assert(NumBones <= MAX_BONES && "Bones 200");
	NumActions = *((short*)(Data + DataPtr)); DataPtr += 2;

	Meshs = new Mesh_t[max(1, NumMeshs)];
	Bones = new Bone_t[max(1, NumBones)];
	Actions = new Action_t[max(1, NumActions)];
	Textures = new Texture_t[max(1, NumMeshs)];
	IndexTexture = new GLuint[max(1, NumMeshs)];
	LightTexture = new GLuint[max(1, NumMeshs)];

	int i;

	for (i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];
		m->NumVertices = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumNormals = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumTexCoords = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NumTriangles = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->Texture = *((short*)(Data + DataPtr)); DataPtr += 2;
		m->NoneBlendMesh = false;
		//m->NumCommandBytes  = *((int   *)(Data+DataPtr));DataPtr+=4;
		m->Vertices = new Vertex_t[m->NumVertices];
		m->Normals = new Normal_t[m->NumNormals];
		m->TexCoords = new TexCoord_t[m->NumTexCoords];
		m->Triangles = new Triangle_t[m->NumTriangles];
		//m->Commands  = new unsigned char [m->NumCommandBytes];
		Size = m->NumVertices * sizeof(Vertex_t);
		memcpy(m->Vertices, Data + DataPtr, Size); DataPtr += Size;
		Size = m->NumNormals * sizeof(Normal_t);
		memcpy(m->Normals, Data + DataPtr, Size); DataPtr += Size;
		Size = m->NumTexCoords * sizeof(TexCoord_t);
		memcpy(m->TexCoords, Data + DataPtr, Size); DataPtr += Size;
		//Size = m->NumTriangles * sizeof(Triangle_t);
		//memcpy(m->Triangles,Data+DataPtr,Size);DataPtr+=Size;
		Size = sizeof(Triangle_t);
		int Size2 = sizeof(Triangle_t2);

		for (int j = 0; j < m->NumTriangles; j++)
		{
			memcpy(&m->Triangles[j], Data + DataPtr, Size); DataPtr += Size2;
		}
		//memcpy(m->Commands ,Data+DataPtr,m->NumCommandBytes);DataPtr+=m->NumCommandBytes;
		memcpy(Textures[i].FileName, Data + DataPtr, 32); DataPtr += 32;

		TextureScriptParsing TSParsing;

		if (TSParsing.parsingTScript(Textures[i].FileName))
		{
			m->m_csTScript = new TextureScript;
			m->m_csTScript->setScript((TextureScript&)TSParsing);
		}
		else
		{
			m->m_csTScript = NULL;
		}
	}

	for (i = 0; i < NumActions; i++)
	{
		Action_t* a = &Actions[i];
		a->Loop = false;
		a->NumAnimationKeys = *((short*)(Data + DataPtr)); DataPtr += 2;
		a->LockPositions = *((bool*)(Data + DataPtr)); DataPtr += 1;
		if (a->LockPositions)
		{
			a->Positions = new vec3_t[a->NumAnimationKeys];
			Size = a->NumAnimationKeys * sizeof(vec3_t);
			memcpy(a->Positions, Data + DataPtr, Size); DataPtr += Size;
		}
		else
		{
			a->Positions = NULL;
		}
	}

	for (i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		b->Dummy = *((char*)(Data + DataPtr)); DataPtr += 1;
		if (!b->Dummy)
		{
			memcpy(b->Name, Data + DataPtr, 32); DataPtr += 32;
			b->Parent = *((short*)(Data + DataPtr)); DataPtr += 2;
			b->BoneMatrixes = new BoneMatrix_t[NumActions];
			for (int j = 0; j < NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				Size = Actions[j].NumAnimationKeys * sizeof(vec3_t);
				int NumAnimationKeys = Actions[j].NumAnimationKeys;
				bm->Position = new vec3_t[NumAnimationKeys];
				bm->Rotation = new vec3_t[NumAnimationKeys];
				bm->Quaternion = new vec4_t[NumAnimationKeys];
				memcpy(bm->Position, Data + DataPtr, Size); DataPtr += Size;
				memcpy(bm->Rotation, Data + DataPtr, Size); DataPtr += Size;
				for (int k = 0; k < NumAnimationKeys; k++)
				{
					AngleQuaternion(bm->Rotation[k], bm->Quaternion[k]);
				}
			}
		}
	}

	Init(false);
	m_bCompletedAlloc = true;
	return true;
}

bool BMD::Save2(char* DirName, char* ModelFileName)
{
	char ModelName[64];
	strcpy(ModelName, DirName);
	strcat(ModelName, ModelFileName);
	FILE* fp = fopen(ModelName, "wb");
	if (fp == NULL)
		return false;
	putc('B', fp);
	putc('M', fp);
	putc('D', fp);
	Version = 12;
	fwrite(&Version, 1, 1, fp);

	BYTE* pbyBuffer = new BYTE[1024 * 1024];
	BYTE* pbyCur = pbyBuffer;
	memcpy(pbyCur, Name, 32); pbyCur += 32;
	memcpy(pbyCur, &NumMeshs, 2); pbyCur += 2;
	memcpy(pbyCur, &NumBones, 2); pbyCur += 2;
	memcpy(pbyCur, &NumActions, 2); pbyCur += 2;

	int i;
	for (i = 0; i < NumMeshs; i++)
	{
		Mesh_t* m = &Meshs[i];
		memcpy(pbyCur, &m->NumVertices, 2); pbyCur += 2;
		memcpy(pbyCur, &m->NumNormals, 2); pbyCur += 2;
		memcpy(pbyCur, &m->NumTexCoords, 2); pbyCur += 2;
		memcpy(pbyCur, &m->NumTriangles, 2); pbyCur += 2;
		memcpy(pbyCur, &m->Texture, 2); pbyCur += 2;
		//fwrite(&m->NumCommandBytes ,4,1,fp);
		memcpy(pbyCur, m->Vertices, m->NumVertices * sizeof(Vertex_t)); pbyCur += m->NumVertices * sizeof(Vertex_t);
		memcpy(pbyCur, m->Normals, m->NumNormals * sizeof(Normal_t)); pbyCur += m->NumNormals * sizeof(Normal_t);
		memcpy(pbyCur, m->TexCoords, m->NumTexCoords * sizeof(TexCoord_t)); pbyCur += m->NumTexCoords * sizeof(TexCoord_t);
		//fwrite(m->Triangles,m->NumTriangles*sizeof(Triangle_t),1,fp);
		for (int j = 0; j < m->NumTriangles; j++)
		{
			memcpy(pbyCur, &m->Triangles[j], sizeof(Triangle_t2)); pbyCur += sizeof(Triangle_t2);
		}
		//fwrite(m->Commands ,m->NumCommandBytes                ,1,fp);
		memcpy(pbyCur, Textures[i].FileName, 32); pbyCur += 32;
	}
	for (i = 0; i < NumActions; i++)
	{
		Action_t* a = &Actions[i];
		memcpy(pbyCur, &a->NumAnimationKeys, 2); pbyCur += 2;
		memcpy(pbyCur, &a->LockPositions, 1); pbyCur += 1;
		if (a->LockPositions)
		{
			memcpy(pbyCur, a->Positions, a->NumAnimationKeys * sizeof(vec3_t)); pbyCur += a->NumAnimationKeys * sizeof(vec3_t);
		}
	}
	for (i = 0; i < NumBones; i++)
	{
		Bone_t* b = &Bones[i];
		memcpy(pbyCur, &b->Dummy, 1); pbyCur += 1;
		if (!b->Dummy)
		{
			memcpy(pbyCur, b->Name, 32); pbyCur += 32;
			memcpy(pbyCur, &b->Parent, 2); pbyCur += 2;
			for (int j = 0; j < NumActions; j++)
			{
				BoneMatrix_t* bm = &b->BoneMatrixes[j];
				memcpy(pbyCur, bm->Position, Actions[j].NumAnimationKeys * sizeof(vec3_t));
				pbyCur += Actions[j].NumAnimationKeys * sizeof(vec3_t);
				memcpy(pbyCur, bm->Rotation, Actions[j].NumAnimationKeys * sizeof(vec3_t));
				pbyCur += Actions[j].NumAnimationKeys * sizeof(vec3_t);
			}
		}
	}
	long lSize = (long)(pbyCur - pbyBuffer);
	long lEncSize = MapFileEncrypt(NULL, pbyBuffer, lSize);
	BYTE* pbyEnc = new BYTE[lEncSize];
	MapFileEncrypt(pbyEnc, pbyBuffer, lSize);
	fwrite(&lEncSize, sizeof(long), 1, fp);
	fwrite(pbyEnc, lEncSize, 1, fp);
	fclose(fp);
	delete[] pbyBuffer;
	delete[] pbyEnc;
	return true;
}

void BMD::Init(bool Dummy)
{
	if (Dummy)
	{
		int i;
		for (i = 0; i < NumBones; i++)
		{
			Bone_t* b = &Bones[i];
			if (b->Name[0] == 'D' && b->Name[1] == 'u')
				b->Dummy = true;
			else
				b->Dummy = false;
		}
	}
	renderCount = 0;
	BoneHead = -1;
	StreamMesh = -1;
	CreateBoundingBox();
}

void BMD::CreateBoundingBox()
{
	const int safeBones = (NumBones < 0) ? 0 : ((NumBones > MAX_BONES) ? MAX_BONES : NumBones);
	const int safeMeshes = (NumMeshs < 0) ? 0 : ((NumMeshs > MAX_MESH) ? MAX_MESH : NumMeshs);

	for (int i = 0; i < safeBones; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			BoundingMin[i][j] = 9999.0;
			BoundingMax[i][j] = -9999.0;
		}
		BoundingVertices[i] = 0;
	}

	for (int i = 0; i < safeMeshes; i++)
	{
		Mesh_t* m = &Meshs[i];
		if (m == NULL || m->Vertices == NULL || m->NumVertices <= 0)
		{
			continue;
		}

		const int safeVertices = (m->NumVertices > MAX_VERTICES) ? MAX_VERTICES : m->NumVertices;
		for (int j = 0; j < safeVertices; j++)
		{
			Vertex_t* v = &m->Vertices[j];
			if (v->Node < 0 || v->Node >= safeBones)
			{
				continue;
			}

			for (int k = 0; k < 3; k++)
			{
				if (v->Position[k] < BoundingMin[v->Node][k])
					BoundingMin[v->Node][k] = v->Position[k];

				if (v->Position[k] > BoundingMax[v->Node][k])
					BoundingMax[v->Node][k] = v->Position[k];
				}
				BoundingVertices[v->Node]++;
		}
	}
	for (int i = 0; i < safeBones; i++)
	{
		Bone_t* b = &Bones[i];
		if (BoundingVertices[i])
			b->BoundingBox = true;
		else
			b->BoundingBox = false;
		Vector(BoundingMax[i][0], BoundingMax[i][1], BoundingMax[i][2], b->BoundingVertices[0]);
		Vector(BoundingMax[i][0], BoundingMax[i][1], BoundingMin[i][2], b->BoundingVertices[1]);
		Vector(BoundingMax[i][0], BoundingMin[i][1], BoundingMax[i][2], b->BoundingVertices[2]);
		Vector(BoundingMax[i][0], BoundingMin[i][1], BoundingMin[i][2], b->BoundingVertices[3]);
		Vector(BoundingMin[i][0], BoundingMax[i][1], BoundingMax[i][2], b->BoundingVertices[4]);
		Vector(BoundingMin[i][0], BoundingMax[i][1], BoundingMin[i][2], b->BoundingVertices[5]);
		Vector(BoundingMin[i][0], BoundingMin[i][1], BoundingMax[i][2], b->BoundingVertices[6]);
		Vector(BoundingMin[i][0], BoundingMin[i][1], BoundingMin[i][2], b->BoundingVertices[7]);
	}
}

#include "CShaderGL.h"

void createPerspectiveMatrix(float* matrix, float fov, float aspect, float _near, float _far)
{
	float tanHalfFOV = tan(fov * 0.5f);
	float range = _near - _far;

	matrix[0] = 1.0f / (aspect * tanHalfFOV);
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;

	matrix[4] = 0.0f;
	matrix[5] = 1.0f / tanHalfFOV;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;

	matrix[8] = 0.0f;
	matrix[9] = 0.0f;
	matrix[10] = (_far + _near) / range;
	matrix[11] = -1.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = (2.0f * _far * _near) / range;
	matrix[15] = 0.0f;
}

void multiplyMatrices(float* result, const float* mat1, const float* mat2)
{
	// Multiplicaci�n de matrices 4x4
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result[i * 4 + j] = 0.0f;
			for (int k = 0; k < 4; k++) {
				result[i * 4 + j] += mat1[i * 4 + k] * mat2[k * 4 + j];
			}
		}
	}
}

void rotateMatrix(float* matrix, float angle, float x, float y, float z)
{
	// Calculamos la matriz de rotaci�n usando OpenGL
	float rad = angle * (Q_PI / 180.f);
	float cosA = cosf(rad);
	float sinA = sinf(rad);

	// Matriz de rotaci�n 3D en torno al eje (x, y, z)
	float rot[16] = {
		cosA + (1 - cosA) * x * x,        (1 - cosA) * x * y - sinA * z, (1 - cosA) * x * z + sinA * y, 0.0f,
		(1 - cosA) * y * x + sinA * z,    cosA + (1 - cosA) * y * y,      (1 - cosA) * y * z - sinA * x, 0.0f,
		(1 - cosA) * z * x - sinA * y,    (1 - cosA) * z * y + sinA * x,  cosA + (1 - cosA) * z * z,      0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Multiplicamos la matriz actual por la matriz de rotaci�n
	multiplyMatrices(matrix, matrix, rot);
}

void translateMatrix(float* matrix, float tx, float ty, float tz)
{
	// Matriz de traslaci�n
	float translation[16] = {
		1.0f, 0.0f, 0.0f, tx,
		0.0f, 1.0f, 0.0f, ty,
		0.0f, 0.0f, 1.0f, tz,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Multiplicamos la matriz actual por la matriz de traslaci�n
	multiplyMatrices(matrix, matrix, translation);
}

void createViewMatrix(float* matrix, float* cameraPosition, float* cameraAngles, bool cameraTopViewEnable)
{
	// Inicializamos la matriz a la identidad
	float identity[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Primero, copiamos la matriz de identidad al arreglo final
	memcpy(matrix, identity, sizeof(float) * 16);

	// Realizamos las rotaciones de la c�mara seg�n los �ngulos
	// Rotaci�n en el eje Y (alrededor de Y)
	rotateMatrix(matrix, cameraAngles[1], 0.f, 1.f, 0.f);

	// Si la vista no es superior, aplicamos la rotaci�n en el eje X
	if (!cameraTopViewEnable) {
		rotateMatrix(matrix, cameraAngles[0], 1.f, 0.f, 0.f);
	}

	// Rotaci�n en el eje Z (alrededor de Z)
	rotateMatrix(matrix, cameraAngles[2], 0.f, 0.f, 1.f);

	// Aplicamos la traslaci�n para posicionar la c�mara (en el espacio mundial)
	translateMatrix(matrix, -cameraPosition[0], -cameraPosition[1], -cameraPosition[2]);
}


void BMD::CreateVertexBuffer(int i, Mesh_t& mesh)
{
#ifdef SHADER_VERSION_TEST
	GLuint shader_id = gShaderGL->GetShaderId();

	if (shader_id != 0)
	{
		auto vertices = RenderArrayVertices;
		auto colors = RenderArrayColors;
		auto textCoords = RenderArrayTexCoords;
		std::vector<unsigned short> indices;

		int target_vertex_index = -1;

		for (int j = 0; j < mesh.NumTriangles; j++)
		{
			Triangle_t* triangle = &mesh.Triangles[j];

			for (int k = 0; k < triangle->Polygon; k++)
			{
				int source_vertex_index = triangle->VertexIndex[k];

				target_vertex_index++;

				VectorCopy(VertexTransform[i][source_vertex_index], vertices[target_vertex_index]);

				TexCoord_t* textcoord = &mesh.TexCoords[triangle->TexCoordIndex[k]];
				textCoords[target_vertex_index][0] = 0.0;
				textCoords[target_vertex_index][1] = 0.0;
				colors[target_vertex_index][0] = 1.f;
				colors[target_vertex_index][1] = 1.f;
				colors[target_vertex_index][2] = 1.f;
				colors[target_vertex_index][3] = 1.f;
				indices.push_back(target_vertex_index);
			}
		}


		glGenVertexArrays(1, &mesh.VAO);
		glGenBuffers(1, &mesh.VBO_Vertices);
		glGenBuffers(1, &mesh.VBO_TexCoords);
		glGenBuffers(1, &mesh.VBO_Colors);
		glGenBuffers(1, &mesh.EBO);

		glBindVertexArray(mesh.VAO);
		// Vertices
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_Vertices);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(vec3_t), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3_t), (void*)0);
		glEnableVertexAttribArray(0);
		// Coordenadas de Textura
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_TexCoords);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(vec2_t), textCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2_t), (void*)0);
		glEnableVertexAttribArray(1);
		// Color de la textura
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO_Colors);
		glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(vec4_t), colors, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(vec4_t), (void*)0);
		glEnableVertexAttribArray(2);
		// Indices de los Triangulos
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0); // Desvinculamos el VAO
	}
#endif // SHADER_VERSION_TEST
}

void BMD::RenderVertexBuffer(int i, Mesh_t* m, int vertex_index, vec3_t* vertices, vec2_t* textCoords, vec4_t* colors)
{
#ifdef SHADER_VERSION_TEST
	GLuint shader_id = gShaderGL->GetShaderId();

	if (shader_id != 0)
	{
		gShaderGL->run_projection();

		glBindBuffer(GL_ARRAY_BUFFER, m->VBO_Vertices);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_index * sizeof(vec3_t), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//--
		glBindBuffer(GL_ARRAY_BUFFER, m->VBO_TexCoords);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_index * sizeof(vec2_t), textCoords);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//--
		if (colors != NULL)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m->VBO_Colors);
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertex_index * sizeof(vec4_t), colors);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		glUseProgram(shader_id);
		glBindVertexArray(m->VAO);
		glDrawElements(GL_TRIANGLES, vertex_index, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
	else
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		if (colors != NULL) glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, vertices);
		if (colors != NULL) glColorPointer(4, GL_FLOAT, 0, colors);
		glTexCoordPointer(2, GL_FLOAT, 0, textCoords);
		glDrawArrays(GL_TRIANGLES, 0, m->NumTriangles * 3);

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		if (colors != NULL) glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
#endif // SHADER_VERSION_TEST
}


BMD::~BMD()
{
	Release();
}

void BMD::InterpolationTrans(float(*Mat1)[4], float(*TransMat2)[4], float _Scale)
{
	TransMat2[0][3] = TransMat2[0][3] - (TransMat2[0][3] - Mat1[0][3]) * (1 - _Scale);
	TransMat2[1][3] = TransMat2[1][3] - (TransMat2[1][3] - Mat1[1][3]) * (1 - _Scale);
	TransMat2[2][3] = TransMat2[2][3] - (TransMat2[2][3] - Mat1[2][3]) * (1 - _Scale);
}

Action_t* BMD::GetAction(int i)
{
	if (i >= 0 && i < NumActions)
	{
		return &Actions[i];
	}
	return nullptr;
}

Mesh_t* BMD::GetMesh(int i)
{
	if (i >= 0 && i < NumMeshs)
	{
		return &Meshs[i];
	}
	return nullptr;
}

bool BMD::IsTextureHide(int index)
{
	if (Strnicmp(Textures[index].FileName, "blur02_mono")
		|| Strnicmp(Textures[index].FileName, "ground_wind2")
		|| Strnicmp(Textures[index].FileName, "line_of_big_R")
		|| Strnicmp(Textures[index].FileName, "fire_15frame")
		|| Strnicmp(Textures[index].FileName, "fire_15fmono")
		|| Strnicmp(Textures[index].FileName, "fire_15fmono_blue")
		|| Strnicmp(Textures[index].FileName, "damage01mono")
		|| Strnicmp(Textures[index].FileName, "gra2")
		|| Strnicmp(Textures[index].FileName, "gra3")
		|| Strnicmp(Textures[index].FileName, "lightmarks")
		|| Strnicmp(Textures[index].FileName, "s195")
		|| Strnicmp(Textures[index].FileName, "fire_sq")
		)
	{
		return true;
	}
	return false;
}

#ifdef EFFECT_KERNEL_VERTEX
void BMD::runtime_render_level(OBJECT* pObj, int GroupId, float r, float g, float b)
{
	vec3_t body_light;
	VectorCopy(BodyLight, body_light);

	this->SetBodyLight(1.f, 1.f, 1.f);
	RenderMesh(GroupId, RENDER_TEXTURE | RENDER_BRIGHT | RENDER_CHROME2, pObj->Alpha, GroupId, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

	this->SetBodyLight((r / 255.0f), (g / 255.0f), (b / 255.0f));
	RenderMesh(GroupId, RENDER_BRIGHT | RENDER_CHROME, pObj->Alpha, GroupId, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);
	RenderMesh(GroupId, RENDER_BRIGHT | RENDER_METAL, pObj->Alpha, GroupId, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_SHINY);

	VectorCopy(body_light, BodyLight);
}

bool BMD::runtime_make_render(int EffectIndex, OBJECT* pObj)
{
	float fLuminosity;
	float blend_mesh_light;
	float blend_mesh_tex_coord_u;
	float blend_mesh_tex_coord_v;
	vec3_t body_light;

	VectorCopy(BodyLight, body_light);

	switch (EffectIndex)
	{
	case Blessed_Divine_Staff:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 576, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		blend_mesh_tex_coord_u = ((int)(1000 - timeGetTime()) % 1001) / (float)1000;
		BodyLight[0] = 1.f; BodyLight[1] = 0.9f; BodyLight[2] = 0.7f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.f; BodyLight[1] = 0.1f; BodyLight[2] = 1.f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 5, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_Sword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

		this->SetBodyLight(0.0, 0.0, 0.1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		VectorCopy(body_light, BodyLight);

		fLuminosity = Render22(-4.f, 0.f) * 0.4f + 0.6f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_absolute02_swordrender);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMesh, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_absolute02_swordrender);

		this->SetBodyLight(0.7, 0.8, 1.0);
		RenderMesh(3, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

		this->SetBodyLight(0.7, 0.9, 1.0);
		blend_mesh_tex_coord_u = (float)(((1000 - timeGetTime()) % 1001) / (float)1000);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_CrossBow:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(0, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		float blessedcb03 = (((int)((timeGetTime()) * 0.08f)) % 16 / 4) * 0.25f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, blessedcb03, pObj->BlendMeshTexCoordV, -1);
		float blessedcb01 = (((int)((1000 - timeGetTime())) % 1001)) / (float)1000;
		BodyLight[0] = 1.f; BodyLight[1] = 0.9f; BodyLight[2] = 0.5f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, blessedcb01, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_Scepter:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.5f; BodyLight[1] = 0.5f; BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom03);
		blend_mesh_tex_coord_u = (int)(1000 - timeGetTime()) % 1001 / (float)1000;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_Stick:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, 576, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		RenderMesh(5, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f; BodyLight[1] = 0.9f; BodyLight[2] = 0.5f;
		blend_mesh_tex_coord_u = (int)(1000 - timeGetTime()) % 1001 / (float)1000;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.f; BodyLight[1] = 0.1f; BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, 1.0, 3, 1.0, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra);
	}
	break;
	case Blessed_Divine_Claw:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f; BodyLight[1] = 0.9f; BodyLight[2] = 0.5f;
		blend_mesh_tex_coord_u = (int)(1000 - timeGetTime()) % 1001 / (float)1000;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, 1.f, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_Lancer:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		fLuminosity = Render22(-1.2f, 0.f) + 0.3f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		fLuminosity = (int)(rand() % 15 / 37.5) + 0.5f;

		BodyLight[0] = fLuminosity * 0.8f;
		BodyLight[1] = fLuminosity * 0.6f;
		BodyLight[2] = fLuminosity * 0.4f;

		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		fLuminosity = 1.f - Render22(4.f, 0.f);

		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;

		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, 3, 1.f, fLuminosity, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blessed_Divine_Mace:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		blend_mesh_tex_coord_u = 0.65f - Render22(4.f, 0.f);

		BodyLight[0] = 0.7f;
		BodyLight[1] = 0.9f;
		BodyLight[2] = 1.f;

		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, -1);

		fLuminosity = (int)(rand() % 20 / 32.5) + 0.4f;

		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 1.f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Blessed_Divine_ShortSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		fLuminosity = (Render22(-4.f, 0.f) * 0.8f + 0.2f);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

		blend_mesh_tex_coord_u = ((int)timeGetTime() % 2000) * 0.0005f;
		blend_mesh_tex_coord_v = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, fLuminosity, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, BITMAP_black_gold_crom05);

		fLuminosity = Render22(-8.f, 0.f);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

		int Texture = timeGetTime() % 3 + BITMAP_LIGHTNING_MEGA1;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);

		fLuminosity = (int)(rand() % 20 / 32.5) + 0.4f;
		BodyLight[0] = fLuminosity * 1.f;
		BodyLight[1] = fLuminosity * 0.8f;
		BodyLight[2] = fLuminosity * 0.4f;
		RenderMesh(6, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 6, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);

		BodyLight[0] = 1.f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 0.05f;
		blend_mesh_tex_coord_u = ((int)timeGetTime() % 800) * 0.00125f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, -blend_mesh_tex_coord_u, pObj->BlendMeshTexCoordV, BITMAP_ground_wind2);
	}
	break;
	case Heavenly_Wings:
	{
		glColor3fv(BodyLight);
		blend_mesh_light = Render22(-2.f, 0.f) * 0.7f + 0.2f;

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, blend_mesh_light, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_magicianwing4da_render);
		runtime_render_level(pObj, 0, 182, 131, 69);
	}
	break;
	case Avatar_Wings:
	{
		glColor3fv(BodyLight);
		float blend_mesh_light = Render22(-1.75f, 0.f);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int Avatar1 = (((int)((timeGetTime()) * 0.75f)) % 600 / 40);
		blend_mesh_tex_coord_u = (float)(Avatar1 % 4) * 0.25f;
		blend_mesh_tex_coord_v = (float)(Avatar1 / 4) * 0.25f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, blend_mesh_light, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_dknightwing4db_render);
		runtime_render_level(pObj, 0, 212, 200, 170);
	}
	break;
	case Orb_Wings:
	{
		glColor3fv(BodyLight);
		float elfwing4da = Render22(-4.f, 0.f) * 0.5f + 0.5f;
		float elfwing4da_render01 = Render22(-4.f, 120.f);
		float elfwing4da_render02 = Render22(-4.f, 240.f);

		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, elfwing4da, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, elfwing4da_render01, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_elfwing4da_render01);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, elfwing4da_render02, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_elfwing4da_render02);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, elfwing4da, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		runtime_render_level(pObj, 2, 255, 166, 124);
	}
	break;
	case Fall_Wings:
	{
		glColor3fv(BodyLight);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int fall1 = (int)timeGetTime() % 600 / 40;
		blend_mesh_tex_coord_u = (float)(fall1 % 4) * 0.25f;
		blend_mesh_tex_coord_v = (float)(fall1 / 4) * 0.25f;
		BodyLight[0] = 0.50f;
		BodyLight[1] = 0.50f;
		BodyLight[2] = 0.50f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 0, pObj->BlendMeshLight, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, -1);
		runtime_render_level(pObj, 1, 204, 190, 151);
		runtime_render_level(pObj, 2, 204, 190, 151);
		runtime_render_level(pObj, 3, 204, 190, 151);
	}
	break;
	case null01:
		RenderBody(2, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1, -1);
		break;
	case The_Wings:
	{
		glColor3fv(BodyLight);
		float alicewing4db_render = Render22(-4.f, 0.f) * 0.1f + 0.3f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, alicewing4db_render, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_alicewing4db_render);
		runtime_render_level(pObj, 0, 255, 20, 20);
	}
	break;
	case null02:
		RenderBody(2, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1, -1);
		break;
	case Holyangel_Sword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		float v403 = -v401;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v403, BITMAP_line_of_big_R);
	}
	break;
	case Holyangel_MagicSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v641 = Render22(-4.f, 0.f) + 0.3f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v641, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v401, BITMAP_line_of_big_R);
	}
	break;
	case Holyangel_Scepter:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.1f;
		BodyLight[1] = 0.2f;
		BodyLight[2] = 1.0f;
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, 1.0, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v401, BITMAP_line_of_big_R);
		float HolyangelSc01 = (float)(rand() % 15) / 37.5f;
		float HolyangelSc02 = HolyangelSc01 + 0.5f;
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.7f;
		BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, HolyangelSc02, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Holyangel_Staff:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int v655 = (int)timeGetTime() % 600 / 40;
		blend_mesh_tex_coord_u = (float)(v655 % 4) * 0.25f;
		blend_mesh_tex_coord_v = (float)(v655 / 4) * 0.25f;
		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.45f;
		BodyLight[2] = 1.f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, -1);
		float v645 = Render22(-6.f, 0.f) + 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, v645, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Holyangel_Stick:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int v655 = (int)timeGetTime() % 600 / 40;
		blend_mesh_tex_coord_u = (float)(v655 % 4) * 0.25f;
		blend_mesh_tex_coord_v = (float)(v655 / 4) * 0.25f;
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 1.f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, -1);
	}
	break;
	case Holyangel_Claw:
	{
		glColor3fv(BodyLight);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v638 = Render22(-4.f, 0.f) + 0.3f;
		BodyLight[0] = 0.15f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 1.f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v638, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v551 = (int)(1000 - timeGetTime()) % 1001 / (float)1000;
		BodyLight[0] = 0.6f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 1.f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, 1.f, v551, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, 1.f, pObj->BlendMeshTexCoordU, v551, BITMAP_line_of_big_R);
	}
	break;
	case Awakening_Sword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, 1.f, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_soulknightswordB_render);
		RenderMesh(1, RENDER_TEXTURE, 1.f, 1, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_soulknightswordA_render);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.9f;
		BodyLight[2] = 0.4f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 3, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Awakening_MagicSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		float MagicSword = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(0, RENDER_TEXTURE, 1.f, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_soulmagicswordA_render);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, MagicSword, BITMAP_flame_chrome);
	}
	break;
	case Awakening_Bow:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, 1.0, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_hs_soularcherbowA01_render);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v401, BITMAP_flame_chrome);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v657, v659, -1);
	}
	break;
	case Awakening_Staff:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(1, RENDER_TEXTURE, 1.f, 1, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_SoulWizardStaff01_render);
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v657, v659, -1);
	}
	break;
	case Awakening_Stick:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, 1.f, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_soulsummonerstickA02_render);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v401, BITMAP_flame_chrome);
	}
	break;
	case Awakening_Claw:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, 1.f, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_soulfighterclawA_render);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, 1.f, v657, v659, -1);
	}
	break;
	case Blue_Eye_Sword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.55f;
		BodyLight[1] = 0.7f;
		BodyLight[2] = 1.f;
		int Cal = (int)timeGetTime() % 1125 / 75;
		blend_mesh_tex_coord_u = (float)(Cal % 4) * 0.25f;
		blend_mesh_tex_coord_v = (float)(Cal / 4) * 0.25f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 3, pObj->BlendMeshLight, blend_mesh_tex_coord_u, blend_mesh_tex_coord_v, -1);
		fLuminosity = Render22(-6.f, 0.f) * 0.8f + 0.2f;

		if (fLuminosity > 0.8f)
			fLuminosity = 0.8f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_hs_blueeyeknightA02_render);
		float v639 = ((int)timeGetTime() % 2000) * 0.0005f;
		float v640 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, pObj->BlendMeshLight, v640, v639, BITMAP_black_gold_crom04_mono);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_hs_blueeyeknightA03r);
	}
	break;
	case Blue_Eye_MagicSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		fLuminosity = Render22(-6.f, 0.f) * 0.8f + 0.2f;
		if (fLuminosity > 1.f)
			fLuminosity = 1.f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_blueeyemagicsword03r);
		fLuminosity = Render22(-6.f, 0.f) * 0.9f + 0.2f;
		if (fLuminosity > 0.8f)
			fLuminosity = 0.8f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, fLuminosity, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_blueeyemagicsword02r);
		float v670 = pObj->BlendMeshTexCoordV;
		float v671 = ((int)timeGetTime() % 10000) * 0.0003f;
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, 0.7f, v671, v670, BITMAP_black_gold_crom04_mono);
		int v834 = 1200;
		float v833 = (((int)((v834 - timeGetTime())) % 1201)) / (float)v834;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, 0.3f, v833, pObj->BlendMeshTexCoordV, BITMAP_alpha_line2);
	}
	break;
	case Blue_Eye_Scepter:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v631 = pObj->BlendMeshTexCoordV;
		float v632 = pObj->BlendMeshTexCoordU;
		float v633 = Render22(-4.f, 0.f);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v633, v632, v631, BITMAP_blueeyesceptre_render);
		float v634 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		float v635 = pObj->BlendMeshTexCoordU;
		float v636 = Render22(-4.f, 0.f) * 0.7f
			+ 0.15f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v636, v635, v634, BITMAP_black_gold_crom04);
	}
	break;
	case Blue_Eye_Bow:
	{
		glColor3fv(BodyLight);
		int v854 = (int)timeGetTime() % 900 / 60;
		float v855 = (float)(v854 % 4) * 0.25f;
		float v857 = (float)(v854 / 4) * 0.25f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v643 = pObj->BlendMeshTexCoordV;
		float v644 = pObj->BlendMeshTexCoordU;
		float v645 = Render22(-4.f, 0.f);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v645, v644, v643, BITMAP_Blue_eyebow02r);
		float v646 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		float v647 = pObj->BlendMeshTexCoordU;
		float v648 = Render22(-4.f, 0.f) * 0.8500000238418579f + 0.15f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v648, v647, v646, BITMAP_black_gold_crom04);
		BodyLight[0] = 0.1f;
		BodyLight[1] = 0.65f;
		BodyLight[2] = 0.5f;
		float v650 = v857;
		float v651 = v855;
		float v652 = Render22(-4.f, 0.f) * 0.7f
			+ 0.3f;
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, v652, v651, v650, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, RENDER_TEXTURE, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blue_Eye_Staff:
	{
		glColor3fv(BodyLight);
		int v849 = (int)timeGetTime() % 750 / 50;
		float v850 = (float)(v849 % 4) * 0.25f;
		float v852 = (float)(v849 / 4) * 0.25f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v653 = pObj->BlendMeshTexCoordV;
		float v654 = pObj->BlendMeshTexCoordU;
		float v655 = Render22(-4.f, 0.f);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v655, v654, v653, BITMAP_blue_eye_ws02_render);
		float v656 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		float v657 = pObj->BlendMeshTexCoordU;
		float v658 = Render22(-4.f, 0.f) * 0.85f + 0.15f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v658, v657, v656, BITMAP_black_gold_crom04);
		BodyLight[0] = 0.1f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v850, v852, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v850, v852, -1);
		int v851 = 15;
		signed int v659 = timeGetTime();
		v849 = v659 % (36 * v851) / v851;
		float v660 = (float)v849 * 0.03f;
		v852 = minf(1.0, v660);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 0.2f;
		float v661 = 1.f - v852;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v661, -1);
	}
	break;
	case Blue_Eye_Stick:
	{
		glColor3fv(BodyLight);
		int v859 = (int)timeGetTime() % 900 / 60;
		float v860 = (float)(v859 % 4) * 0.25f;
		float v862 = (float)(v859 / 4) * 0.25f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v637 = pObj->BlendMeshTexCoordV;
		float v638 = pObj->BlendMeshTexCoordU;
		float v639 = Render22(-4.f, 0.f);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v639, v638, v637, BITMAP_bsummoners02_render);
		float v640 = ((int)timeGetTime() % 2000) * 0.0005f;
		float v641 = pObj->BlendMeshTexCoordU;
		float v642 = Render22(-4.f, 0.f) * 0.85f + 0.15f;
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, v642, v641, v640, BITMAP_black_gold_crom04);
		BodyLight[0] = 0.3f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 0.6f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, v860, v862, -1);
	}
	break;
	case Blue_Eye_Claw:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v827 = Render22(-8.f, 0.f);
		if (v827 > 1.f)
			v827 = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, 0, v827, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_Blueeyeclaw_render);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v827, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_Blueeyeclaw_render);
		v827 = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (v827 > 1.f)
			v827 = 1.f;
		int v826 = (int)timeGetTime() % 750 / 50;
		float v829 = (float)(v826 % 4) * 0.25f;
		float v831 = (float)(v826 / 4) * 0.25f;
		BodyLight[0] = 0.15f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v827, v829, v831, BITMAP_fire_15frame_mono);
	}
	break;
	case Darkangel_Bow:
	{
		glColor3fv(BodyLight);
		float DarkAngelBow = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, DarkAngelBow, BITMAP_black_gold_crom05);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Darkangel_Lance:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 576, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		float v803 = ((int)(timeGetTime() * 0.1f) % 200) * 0.01f - 1.f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v803, -1);
	}
	break;
	case Darkangel_Mace:
	{
		glColor3fv(BodyLight);
		float v808 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v808, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_mace_mastr02s_render);
		float v398 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v398, BITMAP_black_gold_crom05);
		int v807 = (int)timeGetTime() % 1200 / 80;
		float v809 = (float)(v807 % 4) * 0.25f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 0.34f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, pObj->BlendMeshLight, v809, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Darkangel_Staff:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, 1.f, -1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v525 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE, 1.0, 0, v525, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_Darkangel_StaffA_render);
		float v486 = BodyLight[0] * 0.3f;
		BodyLight[0] = v486;
		float v487 = BodyLight[1] * 0.3f;
		BodyLight[1] = v487;
		float v488 = BodyLight[2] * 0.3f;
		BodyLight[2] = v488;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 576, 1.f, 1, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
	}
	break;
	case Darkangel_MagicSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v401, BITMAP_black_gold_crom05);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.7f;
		BodyLight[2] = 1.f;
		float v551 = (float)(1000 - (int)timeGetTime() % 1001) / (float)1000;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v551, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v551, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Darkangel_ShortSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1011 = ((int)timeGetTime() % 2000) * 0.0005f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v1011, -1);
	}
	break;
	case Darkangel_Sword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, 1.f, -1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 576, 1.f, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		BodyLight[0] = 1.0f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 1.0f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, 1.0, 2, 1.0, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, 1.0, 2, 1.0, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Holyangel_Mace:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.5;
		BodyLight[1] = 0.5;
		BodyLight[2] = 0.5;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v507 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v507, -1);
		BodyLight[0] = 0.0;
		BodyLight[1] = 0.5;
		BodyLight[2] = 1.0;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v598 = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, v598, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Holyangel_Lance:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 1.f;
		float v777 = Render22(-4.f, 0.f) + 0.3f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, v777, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Holyangel_ShortSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		float v403 = -v401;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v403, BITMAP_line_of_big_R);
		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.6f;
		BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Awakening_Lance:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v503 = (float)(rand() % 20) / 52.5f;
		float v601 = v503 + 0.4f;
		int v1148 = 47;
		float v44 = v601 * 0.4f;
		float v45 = v601 * 0.6f;
		float v46 = v601 * 0.8f;
		BodyLight[0] = v46;
		BodyLight[1] = v45;
		BodyLight[2] = v44;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.8f;
		BodyLight[0] = v46;
		BodyLight[1] = v45;
		BodyLight[2] = v44;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v600 = Render22(-5.f, 0.f) * 0.8f + 0.2f;
		if (v600 > 1.f)
			float v600 = 1.f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v600, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_soullance01_render);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v600, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_soullance01_render);
	}
	break;
	case Awakening_Mace:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v595 = Render22(-4.0, 0.0);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v595, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v508 = (float)(rand() % 20) / 52.5f;
		float v597 = v508 + 0.4f;
		float v596 = ((int)timeGetTime() % 4000) * 0.00025f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v597, v596, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Awakening_ShortSword:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v657, v659, -1);
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 0.3f;
		float v1010 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v1010, BITMAP_flame_chrome);
	}
	break;
	case Blue_Eye_ShortSword:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1008 = (Render22(-4.f, 0.f) * 0.7f
			+ 0.3f)
			* pObj->Alpha;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, v1008, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1009 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		float v728 = -v1009;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, v728, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		BodyLight[0] = 0.55f;
		BodyLight[1] = 0.7f;
		BodyLight[2] = 1.f;
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, v657, v659, -1);
	}
	break;
	case Blue_Eye_Lance:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v976 = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (v976 > 1.f)
			v976 = 1.f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v976, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_hs_blueeyelanceA02r);
		float v770 = pObj->BlendMeshTexCoordV,
			v771 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v976, v771, v770, BITMAP_hs_blueeyelanceA02r);
		int v979 = 50;
		int v772 = (int)timeGetTime();
		int v975 = v772 % (15 * v979) / v979;
		float v978 = (float)(v975 % 4) * 0.25f;
		float v980 = (float)(v975 / 4) * 0.25f;
		BodyLight[0] = 0.15f;
		BodyLight[1] = 0.4f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v976, v978, v980, -1);
		v976 = Render22(-4.0, 0.0);
		if (v976 > 1.0)
			v976 = 1.0;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, v976, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Blue_Eye_Mace:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v974 = Render22(-5.f, 0.f);
		if (v974 > 1.f)
			v974 = 1.f;
		float v775 = pObj->BlendMeshTexCoordV,
			v776 = ((int)timeGetTime() % 2000)
			* 0.0005f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v974, v776, v775, BITMAP_MMsSs2_r);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, v974, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_mace5master02_render);
	}
	break;
	case Wings_of_Disillusionment:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1137 = Render22(-2.f, 0.f) * 0.4f + 0.6f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v1137, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		v1137 = Render22(-4.0, 0.0);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v1137, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_runewing3db_render);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Wings_of_Fate:
	{

		glColor3fv(BodyLight);

		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1121 = Render22(-4.f, 0.f) * 0.5f + 0.5f;
		float v1122 = Render22(-4.f, 120.f);
		float v1119 = Render22(-4.f, 240.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v1121, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v1122, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_runewing4dc_star01);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v1119, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_runewing4dc_star02);
		float v1120 = Render22(-4.f, 0.f) * 0.6f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v1120, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_runewing4db_render);
		runtime_render_level(pObj, 1, 188, 79, 88);
	}
	break;
	case Wings_of_Silence:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 0.3f;
		float v731 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v731, BITMAP_black_gold_crom05);
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		float v1002 = (Render22(-4.f, 0.f) * 0.8f + 0.2f) * pObj->Alpha;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v1002, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_wings_van_render);
		v1002 = (Render22(-4.f, 0.f) * 0.5f + 0.5f) * pObj->Alpha;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v1002, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, v1002, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int v1004 = 60;
		int v732 = (int)timeGetTime();
		int v1001 = v732 % (13 * v1004) / v1004;
		float v1003 = (float)(v1001 % 4) * 0.25f;
		float v1005 = (float)(v1001 / 4) * 0.25f;
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 0.23f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, v1003, v1005, -1);
	}
	break;
	case Wings_of_Condemnation:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v997 = Render22(-4.f, 0.f) * pObj->Alpha;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v997, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_benzia_wing4_render);
		v997 = (Render22(-2.f, 0.f) * 0.7f + 0.3f) * pObj->Alpha;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v997, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_benzia_wing4a_render);
		v997 = Render22(-2.f, 180.f) * pObj->Alpha;
		BodyLight[0] = 0.8f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.8f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v997, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_benzia_wing4b_render);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v997, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_benzia_wing4b_render);
		int v999 = 80;
		int v735 = (int)timeGetTime();
		int v996 = v735 % (15 * v999) / v999;
		float v998 = (float)(v996 % 4) * 0.25f;
		float v1000 = (float)(v996 / 4) * 0.25f;
		BodyLight[0] = 0.4f;
		BodyLight[1] = 0.4f;
		BodyLight[2] = 0.4f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v998, v1000, -1);
		runtime_render_level(pObj, 1, 135, 147, 144);
	}
	break;
	case Wings_of_Power_old:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1503 = Render22(-6.f, 0.f) + 0.3f;
		if (v1503 > 1.f)
			v1503 = 1.f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 66, v1503, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_season_wing_period_render);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.65f;
		RenderMesh(0, 68, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);
		BodyLight[0] = 0.4f;
		BodyLight[1] = 0.45f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.4f;
		BodyLight[1] = 0.76f;
		BodyLight[2] = 0.95f;
		int v1505 = 40;
		signed int v49 = timeGetTime();
		int v1502 = v49 % (15 * v1505) / v1505;
		float v1504 = (float)(v1502 % 4) * 0.25f;
		float v1506 = (float)(v1502 / 4) * 0.25f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v1504, v1506, -1);
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.35f;
		BodyLight[2] = 1.f;
		int wofpeffect1 = (int)(timeGetTime() * 1.25f) % 600 / 40;
		float wofpeffect2 = (float)(wofpeffect1 % 4) * 0.25f;
		float wofpeffect3 = (float)(wofpeffect1 / 4) * 0.25f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, wofpeffect2, wofpeffect3, BITMAP_firek_sq_mono);
	}
	break;
	case Control_Cloak:
	{
		if (BodyLight[0] != 1.f || BodyLight[1] != 1.f || BodyLight[2] != 1.f)
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;

			glColor3fv(BodyLight);

			RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			RenderMesh(3, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			runtime_render_level(pObj, 4, 61, 112, 255);
		}
		else
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderBody(2, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1, -1);
		}

		glColor3fv(BodyLight);

		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		runtime_render_level(pObj, 4, 61, 112, 255);
	}
	break;
	case Referre_Cloak:
	{
		if (BodyLight[0] != 1.f || BodyLight[1] != 1.f || BodyLight[2] != 1.f)
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderMesh(6, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		}
		else
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		}

		glColor3fv(BodyLight);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		runtime_render_level(pObj, 1, 181, 137, 139);
	}
	break;
	case Wings_of_Power:
	{
		if (BodyLight[0] != 1.f || BodyLight[1] != 1.f || BodyLight[2] != 1.f)
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderMesh(6, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		}
		else
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderMesh(5, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		}

		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1503 = Render22(-6.f, 0.f) + 0.3f;
		if (v1503 > 1.0)
			v1503 = 1.0;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 66, v1503, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_season_wing_period_render);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.65f;
		RenderMesh(0, 68, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);
		BodyLight[0] = 0.4f;
		BodyLight[1] = 0.45f;
		BodyLight[2] = 1.f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.4f;
		BodyLight[1] = 0.76f;
		BodyLight[2] = 0.95f;
		int v1505 = 40;
		signed int v49 = timeGetTime();
		int v1502 = v49 % (15 * v1505) / v1505;
		float v1504 = (float)(v1502 % 4) * 0.25f;
		float v1506 = (float)(v1502 / 4) * 0.25f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v1504, v1506, -1);
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.35f;
		BodyLight[2] = 1.f;
		int wofpeffect1 = (int)(timeGetTime() * 1.25f) % 600 / 40;
		float wofpeffect2 = (float)(wofpeffect1 % 4) * 0.25f;
		float wofpeffect3 = (float)(wofpeffect1 / 4) * 0.25f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, wofpeffect2, wofpeffect3, -1);
	}
	break;
	case Bloodangel_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.75f;
		BodyLight[2] = 0.4f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 0.1f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Darkangel_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 576, 1.0, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom05);
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 1.f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.75f;
		BodyLight[2] = 1.f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Holyangel_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.85f;
		BodyLight[2] = 0.45f;
		float v401 = ((int)timeGetTime() % 2000) * 0.0005f;
		float v776 = -v401;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v776, BITMAP_line_of_big_R);
		float HolyangelSc01 = (float)(rand() % 15) / 37.5f;
		float HolyangelSc02 = HolyangelSc01 + 0.5f;
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.55f;
		BodyLight[2] = 1.5f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, HolyangelSc02, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Awakening_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.25f;
		BodyLight[2] = 0.1f;
		RenderMesh(3, 6, pObj->Alpha, 0, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom04_mono);
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		float v1010 = ((int)timeGetTime() % 2000) * 0.0005f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v1010, BITMAP_flame_chrome);
		float v777 = Render22(-4.f, 0.f) * 0.8f + 0.2f;
		BodyLight[0] = 1.f;
		BodyLight[1] = 0.08f;
		BodyLight[2] = 0.05f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, v777, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Blueeye_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.6f;
		BodyLight[1] = 0.85f;
		BodyLight[2] = 1.f;
		float v778 = (float)(timeGetTime() % 2000) * 0.00050f;
		float v779 = (float)(timeGetTime() % 2000) * 0.0005f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v779, v778, BITMAP_CHROME);//Chrome01.jpg
		BodyLight[0] = 0.3f;
		BodyLight[1] = 0.75f;
		BodyLight[2] = 1.7f;
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	//Correccion 19/10/2020
	case Silverheart_Shield:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.55f / 2;
		BodyLight[1] = 0.25f / 2;
		BodyLight[2] = 1.f / 2;
		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra2);
	}
	break;
	case Mayas_Wings_MuOrigin:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int v658 = 40;
		int v655 = (int)timeGetTime() % 600 / 40;
		float v657 = (float)(v655 % 4) * 0.25f;
		float v659 = (float)(v655 / 4) * 0.25f;
		BodyLight[0] = 0.75f;
		BodyLight[1] = 0.75f;
		BodyLight[2] = 0.75f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, v657, v659, -1);
		BodyLight[0] = 0.35f;
		BodyLight[1] = 0.35f;
		BodyLight[2] = 1.00f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v657, v659, -1);
		BodyLight[0] = 0.35f;
		BodyLight[1] = 0.35f;
		BodyLight[2] = 1.00f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, v657, v659, -1);
		float v551 = (1000 - timeGetTime() % 1001) / 1000.0f;
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, v551, pObj->BlendMeshTexCoordV, BITMAP_0120);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.15f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 0.25f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, v551, pObj->BlendMeshTexCoordV, BITMAP_0143);
		BodyLight[0] = 0.87f;
		BodyLight[1] = 0.41f;
		BodyLight[2] = 1.00f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Trigis_Claw:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float tigris = ((int)(timeGetTime() * 0.05f) % 8 / 2) * -0.25f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, 1.f, tigris, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Wing_of_Conqueror:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v1209 = Render22(-2.f, 0.3f);
		if (v1209 < 0.4f)
			v1209 = 0.4f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v1209, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_ChaWing_renderbright);
	}
	break;
	case Absolute_Claw:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, -1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -1, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, 1.f, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v51 = timeGetTime() * 0.0005f;
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.5f;
		RenderMesh(1, 18, pObj->Alpha, 1, 1.0, pObj->BlendMeshTexCoordU, v51, BITMAP_CHROME);//Chrome01.jpg
		float v52 = (float)(rand() % 15) / 21.5f;
		float v1179 = v52 + 0.4f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, v1179, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Absolute_Mace:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0f;
		BodyLight[1] = 0.5f;
		BodyLight[2] = 0.15f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		//30/05/2020
	}
	break;
	case CollectorWings_Purple_MU2:
	{
		glColor3fv(BodyLight);
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int fall1 = (int)timeGetTime() % 600 / 40;
		float fall2 = (float)(fall1 % 4) * 0.25f;
		float fall3 = (float)(fall1 / 4) * 0.25f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 1, pObj->BlendMeshLight, fall2, fall3, -1);
		BodyLight[0] = 0.20f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 0.25f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 125) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_KCWWingP_Render);
	}
	break;
	case CollectorWings_Red_MU2:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		int fall1 = (int)timeGetTime() % 600 / 40;
		float fall2 = (float)(fall1 % 4) * 0.25f;
		float fall3 = (float)(fall1 / 4) * 0.25f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, 1.f, 1, pObj->BlendMeshLight, fall2, fall3, -1);
		BodyLight[0] = 0.20f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 0.25f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_CHROME | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 125) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.4f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_KCWWingR_Render);
	}
	break;
	case Kit_Of_War_2_0_MuAway:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v551 = ((int)(4000 - timeGetTime() * 0.75f) % 4001) * 0.0002500000118743628f;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, pObj->BlendMeshLight, -v551, v551, -1);
	}
	case Russell_Light_Lance:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v698 = ((int)timeGetTime() % 15) / 37.5f;
		float v800 = v698 + 0.5f;
		BodyLight[0] = v800 * 1.f;
		BodyLight[1] = v800 * 0.2f;
		BodyLight[2] = v800 * 0.6f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Constant_Shield:
	{

		glColor3fv(BodyLight);

		float v796 = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v796, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0f;
		BodyLight[1] = 0.0f;
		BodyLight[2] = 0.2f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v796, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra);
	}
	break;
	case Bloodangel_Sword:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_BoodAnelsword_render);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.2f;
		BodyLight[2] = 1.0f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.2f;
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	case Bloodangel_MagicSword:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_BAGsword_render);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 0.2f;
		BodyLight[1] = 0.2f;
		BodyLight[2] = 1.0f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0f;
		BodyLight[1] = 0.8f;
		BodyLight[2] = 0.2f;
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	case Bloodangel_Claw:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_bloodclaw2_render);
	}
	break;
	case Bloodangel_Scepter:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_booldangel_sapterr);
	}
	break;
	case Bloodangel_Bow:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_blood_bow_render);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v407 = ((int)(timeGetTime() * 0.05f) % 16 / 4) * 0.25f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v407, -1);
	}
	break;
	case Bloodangel_Staff:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_bloodangelstaff01_render);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_bloodangelstaff02);
	}
	break;
	case Bloodangel_Stick:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v810 = Render22(-4.f, 0.f);
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 0, v810, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_bloodangelstick01_render);
		float v407 = ((int)(timeGetTime() * 0.05f) % 16 / 4) * 0.25f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v407, -1);
	}
	break;
	//19/10/2020
	case Silver_Heart_Sword:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.1f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02_mono);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02_mono);
		BodyLight[0] = 0.35f;
		BodyLight[1] = 0.3f;
		BodyLight[2] = 0.5f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_MagicSword:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.1f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02_mono);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02_mono);
		BodyLight[0] = 0.25f;
		BodyLight[1] = 0.05f;
		BodyLight[2] = 0.525f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Claws:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.3f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_ShortSword:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, 68, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		BodyLight[0] = 0.15f;
		BodyLight[1] = 0.f;
		BodyLight[2] = 0.5f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Scepter:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Mace:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.f)
			RenderWizardBE = 1.f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Lance://3.30
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.0f, 0.0f) * 0.7f + 0.300000011920929f;
		if (RenderWizardBE > 1.0f)
			RenderWizardBE = 1.0f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, 68, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		BodyLight[0] = 0.5f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.3f;
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Silver_Heart_Bow:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.0f, 0.0f) * 0.7f + 0.300000011920929f;
		if (RenderWizardBE > 1.0f)
			RenderWizardBE = 1.0f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, 0.5, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, 68, pObj->Alpha, 5, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, 68, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		//if (SafeZone(Hero) == 0)
		//{
		BodyLight[0] = 0.6f;
		BodyLight[1] = 0.4f;
		BodyLight[2] = 0.95f;
		RenderMesh(6, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 6, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		//}
		BodyLight[0] = 0.12f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.37f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Staff:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.0f) * 0.7f + 0.300000011920929f;
		if (RenderWizardBE > 1.0f)
			RenderWizardBE = 1.0f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(5, 68, pObj->Alpha, 5, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		float v1438 = (int)(timeGetTime() % 2000) * 0.0005000000237487257f;
		float v934 = -v1438;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, 0.0f, v934, -1);
		BodyLight[0] = 0.12f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.37f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	case Silver_Heart_Stick:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.0f, 0.0f) * 0.7f + 0.3f;
		if (RenderWizardBE > 1.0f)
			RenderWizardBE = 1.0f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, 68, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		RenderMesh(4, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(4, 68, pObj->Alpha, 4, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_black_gold_crom02);
		int v1431 = timeGetTime() % (15 * 40) / 40;
		float v1432 = (float)(v1431 % 4) * 0.25f;
		float v1435 = (float)(v1431 / 4) * 0.25f;
		BodyLight[0] = 0.75f;
		BodyLight[1] = 0.15f;
		BodyLight[2] = 1.0f;
		RenderMesh(5, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 5, pObj->BlendMeshLight, v1432, v1435, -1);
		BodyLight[0] = 0.12f;
		BodyLight[1] = 0.07f;
		BodyLight[2] = 0.37f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
	}
	break;
	//20/10/2020
	case Cyclone_Sword:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0;
		BodyLight[1] = 1.0;
		BodyLight[2] = 1.0;
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	case Blast_Break:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.0;
		BodyLight[1] = 1.0;
		BodyLight[2] = 1.0;
		float v443 = timeGetTime() * 0.002f;
		float v1817 = (sinf(v443) + 1.0f) * 0.3f + 0.3f;
		float v444 = pObj->BlendMeshLight * v1817;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -2, v444, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, -2, v444, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v220 = (float)((int)timeGetTime() % 10000) * 0.0003f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v220, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	case Horn_of_Steal:
	{
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v224 = timeGetTime() * 0.002f;
		float v258 = (sinf(v224) + 1.f) * 0.2f + 0.4f;
		float v225 = pObj->BlendMeshLight * v258;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -2, v225, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Magmus_Peer:
	{
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v224 = timeGetTime() * 0.002f;
		float v258 = (sinf(v224) + 1.f) * 0.3f + 0.3f;
		float v225 = pObj->BlendMeshLight * v258;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -2, v225, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v220 = (float)((signed int)timeGetTime() % 10000) * 0.0003f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, v220, BITMAP_flame_chrome);
	}
	break;
	case Angelic_Bow:
	{
		float v215 = timeGetTime() * 0.003f;
		float v262 = sinf(v215) * 0.25f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = v262 + 0.7f;
		BodyLight[1] = v262 + 0.7f;
		BodyLight[2] = v262 + 0.7f;
		RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, -2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_AngelicBow_overlay);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Miracle_Staff:
	{
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v224 = timeGetTime() * 0.002f;
		float v258 = (sinf(v224) + 1.f) * 0.3f + 0.5f;
		float v225 = pObj->BlendMeshLight * v258;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -2, v225, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, -2, v225, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Lazy_Wind_Shield:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.8f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Light_Lord_Shield:
	case Dark_Devil_Shield:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 20) / 32.5f;
		float Parpadeo2 = Parpadeo1 + 0.1f;
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
	}
	break;
	case Magic_Knight_Shield:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float v224 = timeGetTime() * 0.002f;
		float v258 = (sinf(v224) + 1.f) * 0.3f + 0.5f;
		float v225 = pObj->BlendMeshLight * v258;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, -2, v225, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		BodyLight[0] = 1.f;
		BodyLight[1] = 1.f;
		BodyLight[2] = 1.f;
		RenderMesh(2, 68, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
		RenderMesh(2, 68, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	case Wings_of_Hit:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_wing3C_render);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_wing3B_render);
	}
	break;
	case Kit_StylizedMoon_Kapocha:
	{

		glColor3fv(BodyLight);

		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_E02);
		RenderMesh(2, 68, pObj->Alpha, 2, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_E02);
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_E02);
		int BlessedSword01 = 1000;
		float BlessedSword02 = (((int)((BlessedSword01 - timeGetTime())) % 1001)) / (float)BlessedSword01;
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, -BlessedSword02, -1);
		float v979 = Render22(-4.f, 120.f);
		float v977 = Render22(-4.f, 240.f);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v979, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_SwordB02_Render01);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v977, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_SwordB02_Render02);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v979, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_SwordB02_Render01);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v977, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_K_StylizedMoon_SwordB02_Render02);
	}
	break;
	//Agregado 20/12/2020
	case Bloody_wings:
	{

		glColor3fv(BodyLight);

		float RenderWizardBE = Render22(-4.f, 0.f) * 0.7f + 0.3f;
		RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(3, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_wing4D_render);
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, RenderWizardBE, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_sd_wing4C_Red_render);
		runtime_render_level(pObj, 0, 124, 55, 32);
		BodyLight[0] = (float)(100 / 255.0f);
		BodyLight[1] = (float)(100 / 255.0f);
		BodyLight[2] = (float)(100 / 255.0f);
		RenderMesh(1, 68, pObj->Alpha, 1, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_CHROME);//Chrome01.jpg
	}
	break;
	//Agregado 21/12/2020
	case Cloak_of_Isolation:
	{
		if (BodyLight[0] != 1.f || BodyLight[1] != 1.f || BodyLight[2] != 1.f)
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;

			glColor3fv(BodyLight);

			RenderMesh(0, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			RenderMesh(2, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			RenderMesh(1, RENDER_TEXTURE, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			float v2165 = Render22(-5.f, 0.f);
			RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v2165, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_growlancerwing4db_render);
			v2165 = Render22(-3.f, 0.f) * 0.5f;
			RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v2165, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
			float Parpadeo1 = (float)(rand() % 20) / 23.5f;
			float Parpadeo2 = Parpadeo1 + 0.5f;
			RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
			runtime_render_level(pObj, 0, 195, 127, 125);
		}
		else
		{
			BodyLight[0] = 1.f;
			BodyLight[1] = 1.f;
			BodyLight[2] = 1.f;
			glColor3fv(BodyLight);
			RenderBody(2, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1, -1);
		}
		float v2165 = Render22(-5.f, 0.f);
		RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 1, v2165, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_growlancerwing4db_render);
		v2165 = Render22(-3.f, 0.f) * 0.5f;
		RenderMesh(2, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 2, v2165, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		float Parpadeo1 = (float)(rand() % 20) / 23.5f;
		float Parpadeo2 = Parpadeo1 + 0.5f;
		RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, pObj->Alpha, 3, Parpadeo2, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, BITMAP_gra3);
		runtime_render_level(pObj, 0, 195, 127, 125);
	}
	break;
	default:
		this->RenderBody(2, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, pObj->HiddenMesh, -1);
		break;
	}
	return false;
}

bool BMD::runtime_make_effect(int EffectIndex, OBJECT* pObj, float Alpha, int RenderType, int Texture)
{
	switch (EffectIndex)
	{
	case Blessed_Divine_Staff:
	case Blessed_Divine_Mace:
	case Awakening_Sword:
	case Darkangel_Lance:
	case Awakening_Staff:
	case Bloodangel_Shield:
	case Darkangel_Shield:
	case Awakening_Shield:
	case Blueeye_Shield:
	case Blue_Eye_Sword:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(2, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Blessed_Divine_Sword:
	case Blessed_Divine_Scepter:
		//case Blessed_Divine_Claw:
	case Blessed_Divine_Lancer:
	case Holyangel_Sword:
	case Awakening_Bow:
	case Holyangel_ShortSword:
	case Silver_Heart_Sword:
	case Silver_Heart_MagicSword:
	case Silver_Heart_Lance:
		//case Awakening_Claw:
	case Awakening_MagicSword:
	case Holyangel_Lance:
		//case Trigis_Claw:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Holyangel_Scepter:
		//case Absolute_Claw:
	case Absolute_Mace:
	case Bloodangel_Sword:
	case Bloodangel_MagicSword:
	case Silver_Heart_Scepter:
	case Silver_Heart_Mace:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(2, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Silver_Heart_ShortSword:
	case Silver_Heart_Bow:
	case Silver_Heart_Stick:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(3, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
		//case Silver_Heart_Claws:
	case Silver_Heart_Staff:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(4, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Blessed_Divine_CrossBow:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(2, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(4, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Blessed_Divine_Stick:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(5, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Blessed_Divine_ShortSword:
	case Holyangel_Staff:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(2, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(3, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
		//case Holyangel_Claw:
	case Darkangel_Bow:
	case Darkangel_MagicSword:
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Wings_of_Condemnation:
		this->RenderMesh(3, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Holyangel_Shield:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(2, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(3, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Silverheart_Shield:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(1, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		this->RenderMesh(3, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	case Blessed_Divine_Claw:
	case Awakening_Claw:
	case Trigis_Claw:
		this->RenderMesh(0, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		this->RenderMesh(1, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		break;
	case Absolute_Claw:
		this->RenderMesh(0, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		this->RenderMesh(2, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		break;
	case Silver_Heart_Claws:
		this->RenderMesh(0, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		this->RenderMesh(4, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		break;
	case Holyangel_Claw:
		this->RenderMesh(1, RenderType, pObj->Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, -1);
		break;
	default:
		this->RenderMesh(0, RenderType, Alpha, pObj->BlendMesh, pObj->BlendMeshLight, pObj->BlendMeshTexCoordU, pObj->BlendMeshTexCoordV, Texture);
		break;
	}
	return false;
}

#endif // EFFECT_KERNEL_VERTEX
