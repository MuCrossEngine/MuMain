#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// RenderStateCompat.h
// Mirrors the OpenGL fixed-function state machine for GLES3 emulation.
// All glEnable/glDisable/glBlendFunc/glAlphaFunc/glFog*/glLight*/glMaterial*
// calls update this state; the render backend reads it when building uniforms.
// ─────────────────────────────────────────────────────────────────────────────

#include <GLES3/gl3.h>
#include <glm/glm.hpp>

// ── Matrix mode (mirrors glMatrixMode tokens) ────────────────────────────────
enum RsMatrixMode { RS_MODELVIEW = 0, RS_PROJECTION = 1, RS_TEXTURE = 2 };

// ── Fog mode (mirrors GL_LINEAR / GL_EXP / GL_EXP2) ─────────────────────────
enum RsFogMode { RS_FOG_LINEAR = 0, RS_FOG_EXP = 1, RS_FOG_EXP2 = 2 };

struct RenderState
{
    // ── Enable flags ─────────────────────────────────────────────────────────
    bool texture2D       = false;
    bool depthTest       = true;
    bool cullFace        = false;
    bool blend           = false;
    bool alphaTest       = false;
    bool fog             = false;
    bool lighting        = false;
    bool colorMaterial   = false;
    bool normalize       = false;
    bool scissorTest     = false;

    // ── Blend ─────────────────────────────────────────────────────────────────
    GLenum blendSrc = GL_ONE;
    GLenum blendDst = GL_ZERO;

    // ── Depth ─────────────────────────────────────────────────────────────────
    GLenum depthFunc = GL_LESS;
    bool   depthMask = true;

    // ── Cull ──────────────────────────────────────────────────────────────────
    GLenum cullFaceMode = GL_BACK;

    // ── Alpha test ────────────────────────────────────────────────────────────
    GLenum alphaFunc = GL_GREATER;
    float  alphaRef  = 0.0f;

    // ── Fog ───────────────────────────────────────────────────────────────────
    RsFogMode  fogMode    = RS_FOG_LINEAR;
    glm::vec4  fogColor   = {0, 0, 0, 0};
    float      fogStart   = 0.0f;
    float      fogEnd     = 1.0f;
    float      fogDensity = 1.0f;

    // ── Light 0 ───────────────────────────────────────────────────────────────
    glm::vec4  lightAmbient  = {0,0,0,1};
    glm::vec4  lightDiffuse  = {1,1,1,1};
    glm::vec4  lightSpecular = {1,1,1,1};
    glm::vec4  lightPosition = {0,0,1,0}; // w=0 → directional

    // ── Material ──────────────────────────────────────────────────────────────
    glm::vec4  matAmbient   = {0.2f,0.2f,0.2f,1};
    glm::vec4  matDiffuse   = {0.8f,0.8f,0.8f,1};
    glm::vec4  matSpecular  = {0,0,0,1};
    glm::vec4  matEmission  = {0,0,0,0};
    float      matShininess = 0.0f;

    // ── Global ambient ────────────────────────────────────────────────────────
    glm::vec4  globalAmbient = {0.2f,0.2f,0.2f,1};

    // ── Active texture unit & bound texture ───────────────────────────────────
    GLuint boundTexture = 0;

    // ── Color (glColor*) ──────────────────────────────────────────────────────
    glm::vec4 currentColor = {1,1,1,1};

    // ── Normal (glNormal*) ────────────────────────────────────────────────────
    glm::vec3 currentNormal = {0,0,1};

    // ── Texture coord (glTexCoord*) ───────────────────────────────────────────
    glm::vec2 currentTexCoord = {0,0};

    // ── Polygon offset ────────────────────────────────────────────────────────
    bool  polygonOffsetFill   = false;
    float polygonOffsetFactor = 0.0f;
    float polygonOffsetUnits  = 0.0f;

    // ── Line / point ──────────────────────────────────────────────────────────
    float lineWidth  = 1.0f;
    float pointSize  = 1.0f;     // informational only (ES3 ignores glPointSize)

    // ── Scissor ───────────────────────────────────────────────────────────────
    int scissorX = 0, scissorY = 0, scissorW = 0, scissorH = 0;

    // ── Viewport ──────────────────────────────────────────────────────────────
    int vpX = 0, vpY = 0, vpW = 0, vpH = 0;
};

// Global instance — written by GLFixedFunctionStubs, read by OpenGLESRenderBackend
extern RenderState g_rs;

// Apply current RS blend/depth/cull/scissor state to actual GLES3 driver.
// Called by the render backend before each draw call.
void RS_ApplyToDriver();

// Called by glLightfv emulation (converts position to eye space using current MV)
void RS_SetLightPosition(const float* pos4);

#endif // __ANDROID__
