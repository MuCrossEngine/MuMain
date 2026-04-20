#pragma once
typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec34_t[3][4];
extern float PerspectiveX;
extern float PerspectiveY;
extern int OpenglWindowWidth;
extern int OpenglWindowHeight;
extern unsigned int WindowWidth;
extern unsigned int WindowHeight;
extern vec3_t CollisionPosition;
extern float  FPS;
#if defined(LDS_FOR_DEVELOPMENT_TESTMODE) || defined(LDS_UNFIXED_FIXEDFRAME_FORDEBUG)		// ��FPS.(���� 20FPS ��Ȳ���� ������.)
extern float g_fFrameEstimate;
#endif // defined(LDS_FOR_DEVELOPMENT_TESTMODE) || defined(LDS_UNFIXED_FIXEDFRAME_FORDEBUG)
extern float  WorldTime;
extern bool   CameraTopViewEnable;
extern float  CameraViewNear;
extern float  CameraViewFar;
extern float  CameraFOV;
extern vec3_t CameraPosition;
extern vec3_t CameraAngle;
extern float  CameraMatrix[3][4];
extern float  g_fCameraCustomDistance;
extern bool   FogEnable;
extern bool   TextureEnable;
extern bool   DepthTestEnable;
extern bool   CullFaceEnable;
extern bool   DepthMaskEnable;
extern vec3_t       MousePosition;
extern vec3_t       MouseTarget;
extern float        MouseX;
extern float        MouseY;
extern float        BackMouseX;
extern float        BackMouseY;
extern float        MouseRenderX;
extern float        MouseRenderY;
extern bool         MouseLButton;
extern bool     	MouseLButtonPop;
extern bool 		MouseLButtonPush;
extern bool         MouseRButton;
extern bool 		MouseRButtonPop;
extern bool 		MouseRButtonPush;
extern bool 		MouseLButtonDBClick;
extern bool         MouseMButton;
extern bool 		MouseMButtonPop;
extern bool 		MouseMButtonPush;
extern int          MouseWheel;
extern DWORD		MouseRButtonPress;
extern char         GrabFileName[];
extern bool         GrabEnable;

//  etc
//void OpenExploper(char *Name,char *para=NULL);
extern bool CheckID_HistoryDay(char* Name, WORD day);
extern void SaveScreen();
extern void gluPerspective2(float Fov, float Aspect, float ZNear, float ZFar);
extern void glViewport2(int x, int y, int Width, int Height);
extern void CreateScreenVector(int sx, int sy, vec3_t Target, bool bFixView = true, bool bFixScreen = false);
extern void Projection(vec3_t Position, int* sx, int* sy);
extern void Projection2(vec3_t Position, int* sx, int* sy);
extern void GetOpenGLMatrix(float Matrix[3][4]);
extern void TransformPosition(vec3_t Position, vec3_t WorldPosition, int* x, int* y);
extern bool TestDepthBuffer(vec3_t Position);
extern void BeginSprite();
extern void EndSprite();
extern void EnableDepthTest();
extern void DisableDepthTest();
extern void EnableCullFace();
extern void DisableCullFace();
extern void EnableDepthMask();
extern void DisableDepthMask();
extern void DisableTexture(bool AlphaTest = false);
extern void DisableAlphaBlend();
extern void EnableLightMap();
extern void EnableAlphaTest(bool DepthMask = true);
extern void EnableAlphaBlend();
extern void EnableAlphaBlendMinus();
extern void EnableAlphaBlend2();
extern void EnableAlphaBlend3();
extern void EnableAlphaBlend4();
extern void BindTexture(int tex);
extern void BindTextureStream(int tex);
extern void EndTextureStream();
extern void BeginOpengl(int x = 0, int y = 0, int Width = 640, int Height = 480, bool Screen = false);
extern void EndOpengl();
extern void UpdateMousePositionn();
static inline void TEXCOORD(float* c, float u, float v)
{
    c[0] = u;
    c[1] = v;
}
extern void RenderBox(float Matrix[3][4]);
extern void RenderPlane3D(float Width, float Height, float Matrix[3][4]);
extern void RenderSprite(int Texture, vec3_t Position, float Width, float Height, vec3_t Light, float Angle = 0.f, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f);
extern void RenderSpriteUV(int Texture, vec3_t Position, float Width, float Height, float(*UV)[2], vec3_t Light[4], float Alpha = 1.f);
extern void RenderNumber(vec3_t Position, int Num, vec3_t Color, float Alpha = 1.f, float Scale = 15.f);
extern float RenderNumber2D(float x, float y, int Num, float Width, float Height);
extern float RenderNumberHQ(float x, float y, int Num, float Width, float Height);
extern void RenderColor(float x, float y, float Width, float Height, float Alpha = 0.f, int Flag = 0, bool Scale = true);
extern void RenderNoColor(float x, float y, float Width, float Height, float Alpha = 0.f, int Flag = 0);
extern void EndRenderColor();
extern void RenderBitmap(int Texture, float x, float y, float Width, float Height, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f, bool Scale = true, bool StartScale = true, float Alpha = 0.f);
extern void RenderNoBitmap(int Texture, float x, float y, float Width, float Height, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f, bool Scale = true, bool StartScale = true, float Alpha = 0.f);
extern void RenderColorBitmap(int Texture, float x, float y, float Width, float Height, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f, unsigned int color = 0xffffffff);
extern void RenderBitmapRotate(int Texture, float x, float y, float Width, float Height, float Angle, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f, bool Scale = true);
extern void RenderBitRotate(int Texture, float x, float y, float Width, float Height, float Rotate);
extern void RenderPointRotate(int Texture, float ix, float iy, float iWidth, float iHeight, float x, float y, float Width, float Height, float Rotate, float Rotate_Loc, float uWidth, float vHeight, int Num = -1);
extern void RenderBitmapLocalRotate(int Texture, float x, float y, float Width, float Height, float Rotate, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f);
extern void RenderNoBitmapLocalRotate(int Texture, float x, float y, float Width, float Height, float Rotate, float u = 0.f, float v = 0.f, float uWidth = 1.f, float vHeight = 1.f);
extern void RenderBitmapLocalRotate(int Texture, float x, float y, float Width, float Height, float Rotate, float u, float v, float uWidth, float vHeight, bool Scale);
extern void RenderBitmapLocalRotate2(int Texture, float x, float y, float Width, float Height, float Rotate, float u, float v, float uWidth, float vHeight, bool Scale = true);
extern void RenderBitmapLocalProjection(int Texture, float x, float y, float Width, float Height, vec3_t Angle, float u, float v, float uWidth, float vHeight, bool Scale = true);
extern void RenderBitmapAlpha(int Texture, float sx, float sy, float Width, float Height);
extern void RenderBitmapUV(int Texture, float x, float y, float Width, float Height, float u, float v, float uWidth, float vHeight);
extern void BeginBitmap();
extern void EndBitmap();
extern float ConvertNoX(float x);
extern float ConvertNoY(float y);
extern float absf(float a);
extern float minf(float a, float b);
extern float maxf(float a, float b);
extern void InitCollisionDetectLineToFace();
extern bool CollisionDetectLineToFace(vec3_t Position, vec3_t Target, int Polygon, float* v1, float* v2, float* v3, float* v4, vec3_t Normal, bool Collision = true);
extern bool CollisionDetectLineToOBB(vec3_t p1, vec3_t p2, OBB_t obb);
extern void CollisionDetectRotate(float centerX, float centerY, float angle, float& x, float& y);

#ifdef V_SYNCRONIZE
extern void InitVSync();
extern bool IsVSyncAvailable();
extern bool IsVSyncEnabled();
extern void EnableVSync();
extern void DisableVSync();
extern int GetFPSLimit();
#endif // V_SYNCRONIZE

void CalcFPS();

extern double  DeltaT;
extern float   FPS;
extern float   WorldTime;

