#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// OpenGLESRenderBackend.h
// Fixed-function emulation layer for OpenGL ES 3.0.
//
// Public surface used by GLFixedFunctionStubs.h — everything here is called
// from the macro redirects, so keep signatures simple.
// ─────────────────────────────────────────────────────────────────────────────

#include <GLES3/gl3.h>
#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif
#include <glm/glm.hpp>
#include <stack>
#include <vector>

namespace GLESFF  // GLES Fixed-Function
{

// ── Lifecycle ─────────────────────────────────────────────────────────────────
bool  Init(int screenW, int screenH);   // compile shaders, create VBOs
void  Shutdown();
void  SetScreenSize(int w, int h);

// ── Frame ─────────────────────────────────────────────────────────────────────
void  BeginFrame();   // clear + reset per-frame state
void  EndFrame();     // flush remaining draws

// ── Matrix stack ──────────────────────────────────────────────────────────────
void  MatrixMode(int mode);            // 0=MV, 1=Proj, 2=Tex
void  PushMatrix();
void  PopMatrix();
void  LoadIdentity();
void  LoadMatrixf(const float* m);
void  MultMatrixf(const float* m);
void  Rotatef(float angle, float x, float y, float z);
void  Translatef(float x, float y, float z);
void  Scalef(float x, float y, float z);
void  Ortho(double l, double r, double b, double t, double n, double f);
void  Frustum(double l, double r, double b, double t, double n, double f);
void  PerspectiveFov(float fovY, float aspect, float zNear, float zFar);
void  LookAt(float ex,float ey,float ez,float cx,float cy,float cz,float ux,float uy,float uz);

// Matrix accessors for uniform upload
const glm::mat4& GetModelView();
const glm::mat4& GetProjection();
glm::mat4         GetMVP();
glm::mat3         GetNormalMatrix();

// ── Immediate mode ────────────────────────────────────────────────────────────
void  ImmBegin(GLenum mode);
void  ImmReserve(int vertexCount);
void  ImmVertex3f(float x, float y, float z);
void  ImmVertexPacked(float x, float y, float z,
					  float s, float t,
					  float r, float g, float b, float a,
					  float nx, float ny, float nz);
void  ImmVertex3fv(const float* v);
void  ImmVertex2f(float x, float y);
void  ImmTexCoord2f(float s, float t);
void  ImmColor4f(float r, float g, float b, float a);
void  ImmColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void  ImmNormal3f(float x, float y, float z);
void  ImmEnd();    // flush accumulated vertices as a draw call

// ── Texture ───────────────────────────────────────────────────────────────────
void  BindTexture(GLenum target, GLuint tex);
void  TexEnvf(GLenum target, GLenum pname, float param);  // no-op / stored
// Texture format tracking: call after glTexImage2D to register whether the
// texture has a real alpha channel.  BindTexture reads this to set
// g_rs.boundTextureHasAlpha, which the shader uses to decide between
// alpha-cutoff discard (RGBA) and color-key discard (RGB).
void  RegisterTextureFormat(GLuint texID, bool hasAlpha);
// ── Raster position (for 2-D blits) ──────────────────────────────────────────
void  RasterPos2i(int x, int y);

// ── Draw arrays compatibility wrapper ─────────────────────────────────────────
// Used when existing code calls glDrawArrays/glDrawElements directly
// (these pass through to GLES3 unchanged, but need uniforms uploaded first)
void  FlushUniforms();   // upload current matrix/state uniforms before a draw call

// ── State query helpers (for glGetFloatv / glGetIntegerv emulation) ───────────
// GL_CURRENT_COLOR: returns the current per-vertex color set by glColor*
void  GetCurrentColor(float* rgba4);      // out: 4 floats (r,g,b,a)
// GL_MODELVIEW_MATRIX: returns the top of the modelview stack as column-major 4x4
void  GetModelViewMatrix(float* m16);     // out: 16 floats

} // namespace GLESFF

#endif // __ANDROID__
