#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// OpenGLESRenderBackend.cpp
// Emulates OpenGL fixed-function pipeline on top of GLES 3.0.
// ─────────────────────────────────────────────────────────────────────────────

#include "OpenGLESRenderBackend.h"
#include "RenderStateCompat.h"
#include "GameAssetPath.h"

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <android/log.h>
#include <dlfcn.h>
#ifdef min
#  undef min
#endif
#ifdef max
#  undef max
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef GL_QUADS
#  define GL_QUADS 0x0007
#endif
#ifndef GL_POLYGON
#  define GL_POLYGON 0x0009
#endif

#include <vector>
#include <string>
#include <cstdio>
#include <cmath>

#define LOG_TAG "MURender"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ─────────────────────────────────────────────────────────────────────────────
// Vertex layout — matches the GLSL attribute locations
// ─────────────────────────────────────────────────────────────────────────────
struct ImmVertex
{
    float pos[3];
    float uv[2];
    float color[4];
    float normal[3];
};

static constexpr int ATTRIB_POS    = 0;
static constexpr int ATTRIB_UV     = 1;
static constexpr int ATTRIB_COLOR  = 2;
static constexpr int ATTRIB_NORMAL = 3;

// ─────────────────────────────────────────────────────────────────────────────
// Shader source (loaded from assets/shaders/ at Init time)
// ─────────────────────────────────────────────────────────────────────────────
static GLuint s_program = 0;

// Uniform locations
struct UniformLoc
{
    GLint mvp, modelview, normalMatrix;
    GLint lightingEnabled;
    GLint lightAmbient, lightDiffuse, lightSpecular, lightPos;
    GLint matAmbient, matDiffuse, matSpecular, matEmission, matShininess;
    GLint globalAmbient;
    GLint texture, useTexture;
    GLint alphaTestEnabled, alphaTestRef;
    GLint fogEnabled, fogMode, fogColor, fogStart, fogEnd, fogDensity;
};
static UniformLoc s_u;

// ─────────────────────────────────────────────────────────────────────────────
// Matrix stacks — plain std::vector avoids std::deque multi-chunk complexity
// and MTE tag mismatch crashes (Android 15 SIGSEGV in deque::push_back).
// ─────────────────────────────────────────────────────────────────────────────
static std::vector<glm::mat4> s_modelviewStack;
static std::vector<glm::mat4> s_projectionStack;
static std::vector<glm::mat4> s_textureStack;
static int s_matrixMode = 0; // 0=MV, 1=Proj, 2=Tex
// Use dlsym to get native GLES3 function pointers.
// eglGetProcAddress returns NULL for core functions on many Android
// implementations; dlsym("libGLESv2.so") bypasses our GLFixedFunctionStubs
// inline overrides and gives us the real driver entry points.
static void (*s_nativeBindTexture)(GLenum, GLuint)           = nullptr;
static void (*s_nativeDrawArrays)(GLenum, GLint, GLsizei)    = nullptr;

static void ResolveNativeGLProcs()
{
    if (s_nativeBindTexture && s_nativeDrawArrays) return;

    // Try libGLESv3.so first, then libGLESv2.so (GLES3 functions live in GLES2 lib on Android)
    static void* s_glesLib = nullptr;
    if (!s_glesLib)
    {
        s_glesLib = dlopen("libGLESv3.so", RTLD_NOW | RTLD_LOCAL);
        if (!s_glesLib)
            s_glesLib = dlopen("libGLESv2.so", RTLD_NOW | RTLD_LOCAL);
    }

    if (s_glesLib)
    {
        if (!s_nativeBindTexture)
            s_nativeBindTexture  = reinterpret_cast<void(*)(GLenum,GLuint)>(dlsym(s_glesLib, "glBindTexture"));
        if (!s_nativeDrawArrays)
            s_nativeDrawArrays   = reinterpret_cast<void(*)(GLenum,GLint,GLsizei)>(dlsym(s_glesLib, "glDrawArrays"));
    }

    // eglGetProcAddress fallback (works for extensions on all Android versions)
    if (!s_nativeBindTexture)
        s_nativeBindTexture = reinterpret_cast<void(*)(GLenum,GLuint)>(eglGetProcAddress("glBindTexture"));
    if (!s_nativeDrawArrays)
        s_nativeDrawArrays = reinterpret_cast<void(*)(GLenum,GLint,GLsizei)>(eglGetProcAddress("glDrawArrays"));

    LOGI("ResolveNativeGLProcs: bindTexture=%p drawArrays=%p",
         (void*)s_nativeBindTexture, (void*)s_nativeDrawArrays);
}

static inline void DriverBindTexture(GLenum target, GLuint tex)
{
    if (s_nativeBindTexture)
        s_nativeBindTexture(target, tex);
}

static inline void DriverDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (s_nativeDrawArrays)
        s_nativeDrawArrays(mode, first, count);
}

static std::vector<glm::mat4>& ActiveStack()
{
    switch (s_matrixMode)
    {
        case 1:  return s_projectionStack;
        case 2:  return s_textureStack;
        default: return s_modelviewStack;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Immediate-mode accumulation buffer
// ─────────────────────────────────────────────────────────────────────────────
static constexpr int IMM_BUFFER_CAPACITY = 65536;

static std::vector<ImmVertex> s_immVerts;
static GLenum                 s_immMode = GL_TRIANGLES;
static bool                   s_inImmediate = false;

// Current per-vertex state (updated by glColor*, glNormal*, glTexCoord*)
static float s_curColor[4]  = {1,1,1,1};
static float s_curNormal[3] = {0,0,1};
static float s_curUV[2]     = {0,0};

// ─────────────────────────────────────────────────────────────────────────────
// VBO for streaming immediate data
// ─────────────────────────────────────────────────────────────────────────────
static GLuint s_vao = 0;
static GLuint s_vbo = 0;
static int    s_screenW = 0, s_screenH = 0;

// ─────────────────────────────────────────────────────────────────────────────
// Shader helpers
// ─────────────────────────────────────────────────────────────────────────────
static std::string ReadShaderFile(const char* assetRel)
{
    FILE* f = GameAssetPath::OpenFile(assetRel, "rb");
    if (!f) { LOGE("Shader not found: %s", assetRel); return ""; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    std::string src(sz, '\0');
    fread(&src[0], 1, sz, f);
    fclose(f);
    return src;
}

static GLuint CompileShader(GLenum type, const std::string& src)
{
    GLuint s = glCreateShader(type);
    const char* p = src.c_str();
    glShaderSource(s, 1, &p, nullptr);
    glCompileShader(s);
    GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char buf[2048]; glGetShaderInfoLog(s, sizeof(buf), nullptr, buf);
        LOGE("Shader compile error: %s", buf);
        glDeleteShader(s); return 0;
    }
    return s;
}

static GLuint LinkProgram(GLuint vert, GLuint frag)
{
    GLuint p = glCreateProgram();
    glAttachShader(p, vert);
    glAttachShader(p, frag);
    glBindAttribLocation(p, ATTRIB_POS,    "a_position");
    glBindAttribLocation(p, ATTRIB_UV,     "a_texcoord");
    glBindAttribLocation(p, ATTRIB_COLOR,  "a_color");
    glBindAttribLocation(p, ATTRIB_NORMAL, "a_normal");
    glLinkProgram(p);
    GLint ok; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char buf[1024]; glGetProgramInfoLog(p, sizeof(buf), nullptr, buf);
        LOGE("Program link error: %s", buf);
        glDeleteProgram(p); return 0;
    }
    return p;
}

static void CacheUniformLocations()
{
#define ULOC(field, name) s_u.field = glGetUniformLocation(s_program, name)
    ULOC(mvp,             "u_mvp");
    ULOC(modelview,       "u_modelview");
    ULOC(normalMatrix,    "u_normalMatrix");
    ULOC(lightingEnabled, "u_lightingEnabled");
    ULOC(lightAmbient,    "u_lightAmbient");
    ULOC(lightDiffuse,    "u_lightDiffuse");
    ULOC(lightSpecular,   "u_lightSpecular");
    ULOC(lightPos,        "u_lightPos");
    ULOC(matAmbient,      "u_materialAmbient");
    ULOC(matDiffuse,      "u_materialDiffuse");
    ULOC(matSpecular,     "u_materialSpecular");
    ULOC(matEmission,     "u_materialEmission");
    ULOC(matShininess,    "u_materialShininess");
    ULOC(globalAmbient,   "u_globalAmbient");
    ULOC(texture,         "u_texture");
    ULOC(useTexture,      "u_useTexture");
    ULOC(alphaTestEnabled,"u_alphaTestEnabled");
    ULOC(alphaTestRef,    "u_alphaTestRef");
    ULOC(fogEnabled,      "u_fogEnabled");
    ULOC(fogMode,         "u_fogMode");
    ULOC(fogColor,        "u_fogColor");
    ULOC(fogStart,        "u_fogStart");
    ULOC(fogEnd,          "u_fogEnd");
    ULOC(fogDensity,      "u_fogDensity");
#undef ULOC
}

// ─────────────────────────────────────────────────────────────────────────────
// RenderState global + RS_ApplyToDriver
// ─────────────────────────────────────────────────────────────────────────────
RenderState g_rs;

void RS_ApplyToDriver()
{
    // Blend
    if (g_rs.blend) { glEnable(GL_BLEND); glBlendFunc(g_rs.blendSrc, g_rs.blendDst); }
    else              glDisable(GL_BLEND);

    // Depth
    if (g_rs.depthTest) { glEnable(GL_DEPTH_TEST); glDepthFunc(g_rs.depthFunc); }
    else                  glDisable(GL_DEPTH_TEST);
    glDepthMask(g_rs.depthMask ? GL_TRUE : GL_FALSE);

    // Cull
    if (g_rs.cullFace) { glEnable(GL_CULL_FACE); glCullFace(g_rs.cullFaceMode); }
    else                 glDisable(GL_CULL_FACE);

    // Polygon offset
    if (g_rs.polygonOffsetFill)
    {
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(g_rs.polygonOffsetFactor, g_rs.polygonOffsetUnits);
    }
    else glDisable(GL_POLYGON_OFFSET_FILL);

    // Scissor
    if (g_rs.scissorTest)
    {
        glEnable(GL_SCISSOR_TEST);
        glScissor(g_rs.scissorX, g_rs.scissorY, g_rs.scissorW, g_rs.scissorH);
    }
    else glDisable(GL_SCISSOR_TEST);

    // Line width (clamped to [1,1] on most GLES3 — best effort)
    glLineWidth(g_rs.lineWidth);
}

void RS_SetLightPosition(const float* pos4)
{
    // Transform light position to eye space using current MV
    const glm::mat4& mv = s_modelviewStack.back();
    glm::vec4 ep = mv * glm::vec4(pos4[0], pos4[1], pos4[2], pos4[3]);
    g_rs.lightPosition = ep;
}

// ─────────────────────────────────────────────────────────────────────────────
// namespace GLESFF implementation
// ─────────────────────────────────────────────────────────────────────────────
namespace GLESFF {

bool Init(int screenW, int screenH)
{
    s_screenW = screenW; s_screenH = screenH;

    // Resolve native GL function pointers via dlsym (bypasses our GLFixedFunctionStubs inline overrides).
    ResolveNativeGLProcs();

    // Clear any pending GL errors before setup
    while (glGetError() != GL_NO_ERROR) {}

    // Init matrix stacks with identity (pre-reserve to avoid realloc during rendering)
    s_modelviewStack.clear();
    s_projectionStack.clear();
    s_textureStack.clear();
    s_modelviewStack.reserve(128);
    s_projectionStack.reserve(16);
    s_textureStack.reserve(16);
    s_modelviewStack.push_back(glm::mat4(1));
    s_projectionStack.push_back(glm::mat4(1));
    s_textureStack.push_back(glm::mat4(1));

    // Load shaders
    std::string vsrc = ReadShaderFile("shaders/fixed_vert.glsl");
    std::string fsrc = ReadShaderFile("shaders/fixed_frag.glsl");
    if (vsrc.empty() || fsrc.empty()) return false;

    GLuint vert = CompileShader(GL_VERTEX_SHADER,   vsrc);
    GLuint frag = CompileShader(GL_FRAGMENT_SHADER, fsrc);
    if (!vert || !frag) return false;

    s_program = LinkProgram(vert, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);
    if (!s_program) return false;

    CacheUniformLocations();

    // Create streaming VAO/VBO
    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vbo);

    if (!s_vao || !s_vbo)
    {
        LOGE("Init: failed to create VAO/VBO (vao=%u vbo=%u)", s_vao, s_vbo);
        return false;
    }
    LOGI("Init: VAO=%u VBO=%u", s_vao, s_vbo);

    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ImmVertex) * IMM_BUFFER_CAPACITY, nullptr, GL_STREAM_DRAW);

    glEnableVertexAttribArray(ATTRIB_POS);
    glEnableVertexAttribArray(ATTRIB_UV);
    glEnableVertexAttribArray(ATTRIB_COLOR);
    glEnableVertexAttribArray(ATTRIB_NORMAL);

    size_t stride = sizeof(ImmVertex);
    glVertexAttribPointer(ATTRIB_POS,    3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ImmVertex, pos));
    glVertexAttribPointer(ATTRIB_UV,     2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ImmVertex, uv));
    glVertexAttribPointer(ATTRIB_COLOR,  4, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ImmVertex, color));
    glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ImmVertex, normal));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    s_immVerts.reserve(4096);

    LOGI("OpenGLESRenderBackend initialized (%dx%d)", screenW, screenH);
    return true;
}

void Shutdown()
{
    if (s_vbo)     { glDeleteBuffers(1, &s_vbo);       s_vbo = 0; }
    if (s_vao)     { glDeleteVertexArrays(1, &s_vao);  s_vao = 0; }
    if (s_program) { glDeleteProgram(s_program);       s_program = 0; }
}

void SetScreenSize(int w, int h)
{
    s_screenW = w; s_screenH = h;
}

void BeginFrame()
{
    glViewport(g_rs.vpX, g_rs.vpY,
               g_rs.vpW  ? g_rs.vpW  : s_screenW,
               g_rs.vpH  ? g_rs.vpH  : s_screenH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndFrame()
{
    // Flush any dangling immediate-mode verts
    if (!s_immVerts.empty()) ImmEnd();
}

// ── Matrix stack ─────────────────────────────────────────────────────────────

void MatrixMode(int mode)    { s_matrixMode = mode; }
void PushMatrix()            { auto& st = ActiveStack(); st.push_back(st.back()); }
void PopMatrix()             { auto& st = ActiveStack(); if (st.size() > 1) st.pop_back(); }
void LoadIdentity()          { ActiveStack().back() = glm::mat4(1); }

void LoadMatrixf(const float* m)
{
    ActiveStack().back() = glm::make_mat4(m);
}

void MultMatrixf(const float* m)
{
    ActiveStack().back() *= glm::make_mat4(m);
}

void Rotatef(float angle, float x, float y, float z)
{
    ActiveStack().back() = glm::rotate(ActiveStack().back(),
                                       glm::radians(angle), glm::vec3(x,y,z));
}

void Translatef(float x, float y, float z)
{
    ActiveStack().back() = glm::translate(ActiveStack().back(), glm::vec3(x,y,z));
}

void Scalef(float x, float y, float z)
{
    ActiveStack().back() = glm::scale(ActiveStack().back(), glm::vec3(x,y,z));
}

void Ortho(double l, double r, double b, double t, double n, double f)
{
    ActiveStack().back() *= glm::ortho((float)l,(float)r,(float)b,(float)t,(float)n,(float)f);
}

void Frustum(double l, double r, double b, double t, double n, double f)
{
    ActiveStack().back() *= glm::frustum((float)l,(float)r,(float)b,(float)t,(float)n,(float)f);
}

void PerspectiveFov(float fovY, float aspect, float zNear, float zFar)
{
    s_projectionStack.back() = glm::perspective(glm::radians(fovY), aspect, zNear, zFar);
}

void LookAt(float ex,float ey,float ez,float cx,float cy,float cz,float ux,float uy,float uz)
{
    s_modelviewStack.back() *= glm::lookAt(glm::vec3(ex,ey,ez),
                                           glm::vec3(cx,cy,cz),
                                           glm::vec3(ux,uy,uz));
}

const glm::mat4& GetModelView()  { return s_modelviewStack.back(); }
const glm::mat4& GetProjection() { return s_projectionStack.back(); }
glm::mat4 GetMVP()               { return s_projectionStack.back() * s_modelviewStack.back(); }

glm::mat3 GetNormalMatrix()
{
    return glm::mat3(glm::transpose(glm::inverse(s_modelviewStack.back())));
}

// ── Uniforms upload ───────────────────────────────────────────────────────────
void FlushUniforms()
{
    glUseProgram(s_program);

    glm::mat4 mvp = GetMVP();
    glm::mat4 mv  = GetModelView();
    glm::mat3 nm  = GetNormalMatrix();

    glUniformMatrix4fv(s_u.mvp,        1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(s_u.modelview,  1, GL_FALSE, glm::value_ptr(mv));
    glUniformMatrix3fv(s_u.normalMatrix,1,GL_FALSE, glm::value_ptr(nm));

    // Lighting
    glUniform1i(s_u.lightingEnabled, g_rs.lighting ? 1 : 0);
    if (g_rs.lighting)
    {
        glUniform4fv(s_u.lightAmbient,  1, glm::value_ptr(g_rs.lightAmbient));
        glUniform4fv(s_u.lightDiffuse,  1, glm::value_ptr(g_rs.lightDiffuse));
        glUniform4fv(s_u.lightSpecular, 1, glm::value_ptr(g_rs.lightSpecular));
        glm::vec3 lp3(g_rs.lightPosition);
        glUniform3fv(s_u.lightPos,      1, glm::value_ptr(lp3));
        glUniform4fv(s_u.matAmbient,    1, glm::value_ptr(g_rs.matAmbient));
        glUniform4fv(s_u.matDiffuse,    1, glm::value_ptr(g_rs.matDiffuse));
        glUniform4fv(s_u.matSpecular,   1, glm::value_ptr(g_rs.matSpecular));
        glUniform4fv(s_u.matEmission,   1, glm::value_ptr(g_rs.matEmission));
        glUniform1f (s_u.matShininess,  g_rs.matShininess);
        glUniform4fv(s_u.globalAmbient, 1, glm::value_ptr(g_rs.globalAmbient));
    }

    // Texture
    bool useTexture = g_rs.texture2D && (g_rs.boundTexture != 0);
    glUniform1i(s_u.useTexture, useTexture ? 1 : 0);
    if (useTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        DriverBindTexture(GL_TEXTURE_2D, g_rs.boundTexture);
        glUniform1i(s_u.texture, 0);
    }

    // Alpha test
    glUniform1i(s_u.alphaTestEnabled, g_rs.alphaTest ? 1 : 0);
    glUniform1f(s_u.alphaTestRef, g_rs.alphaRef);

    // Fog
    glUniform1i(s_u.fogEnabled, g_rs.fog ? 1 : 0);
    if (g_rs.fog)
    {
        glUniform1i(s_u.fogMode,    (int)g_rs.fogMode);
        glUniform4fv(s_u.fogColor,  1, glm::value_ptr(g_rs.fogColor));
        glUniform1f(s_u.fogStart,   g_rs.fogStart);
        glUniform1f(s_u.fogEnd,     g_rs.fogEnd);
        glUniform1f(s_u.fogDensity, g_rs.fogDensity);
    }

    RS_ApplyToDriver();
}

// ── Immediate mode ────────────────────────────────────────────────────────────
void ImmBegin(GLenum mode)
{
    s_immMode = mode;
    s_inImmediate = true;
    s_immVerts.clear();
}

void ImmVertex3f(float x, float y, float z)
{
    ImmVertex v;
    v.pos[0]=x; v.pos[1]=y; v.pos[2]=z;
    v.uv[0]=s_curUV[0];    v.uv[1]=s_curUV[1];
    v.color[0]=s_curColor[0]; v.color[1]=s_curColor[1];
    v.color[2]=s_curColor[2]; v.color[3]=s_curColor[3];
    v.normal[0]=s_curNormal[0]; v.normal[1]=s_curNormal[1]; v.normal[2]=s_curNormal[2];
    s_immVerts.push_back(v);
}

void ImmVertex3fv(const float* v) { ImmVertex3f(v[0],v[1],v[2]); }
void ImmVertex2f(float x, float y) { ImmVertex3f(x, y, 0.0f); }

void ImmTexCoord2f(float s, float t)
{
    s_curUV[0] = s; s_curUV[1] = t;
}

void ImmColor4f(float r, float g, float b, float a)
{
    s_curColor[0]=r; s_curColor[1]=g; s_curColor[2]=b; s_curColor[3]=a;
}

void ImmColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    ImmColor4f(r/255.f, g/255.f, b/255.f, a/255.f);
}

void ImmNormal3f(float x, float y, float z)
{
    s_curNormal[0]=x; s_curNormal[1]=y; s_curNormal[2]=z;
}

void ImmEnd()
{
    s_inImmediate = false;
    if (s_immVerts.empty()) return;

    // GL_QUADS → two triangles per quad (most common fixed-function use in MU)
    std::vector<ImmVertex> tris;
    if (s_immMode == GL_QUADS)
    {
        size_t n = s_immVerts.size();
        tris.reserve(n / 4 * 6);
        for (size_t i = 0; i + 3 < n; i += 4)
        {
            tris.push_back(s_immVerts[i+0]);
            tris.push_back(s_immVerts[i+1]);
            tris.push_back(s_immVerts[i+2]);
            tris.push_back(s_immVerts[i+0]);
            tris.push_back(s_immVerts[i+2]);
            tris.push_back(s_immVerts[i+3]);
        }
        s_immMode = GL_TRIANGLES;
    }
    else if (s_immMode == GL_POLYGON)
    {
        // Fan triangulation (convex polygons only — sufficient for MU UI)
        size_t n = s_immVerts.size();
        tris.reserve((n - 2) * 3);
        for (size_t i = 1; i + 1 < n; ++i)
        {
            tris.push_back(s_immVerts[0]);
            tris.push_back(s_immVerts[i]);
            tris.push_back(s_immVerts[i+1]);
        }
        s_immMode = GL_TRIANGLES;
    }

    const std::vector<ImmVertex>& src = tris.empty() ? s_immVerts : tris;
    size_t count = src.size();
    if (count == 0) return;

    FlushUniforms();

    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

    // Orphan + re-upload
    size_t bytes = count * sizeof(ImmVertex);
    glBufferData(GL_ARRAY_BUFFER, bytes, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bytes, src.data());

    DriverDrawArrays(s_immMode, 0, (GLsizei)count);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        LOGE("ImmEnd: glGetError=0x%04x mode=0x%04x count=%zu prog=%u",
             (unsigned)err, (unsigned)s_immMode, count, s_program);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    s_immVerts.clear();
}

void BindTexture(GLenum /*target*/, GLuint tex)
{
    g_rs.boundTexture = tex;
    if (tex) DriverBindTexture(GL_TEXTURE_2D, tex);
}

void TexEnvf(GLenum /*target*/, GLenum /*pname*/, float /*param*/) { /* stored in RS if needed */ }

void RasterPos2i(int /*x*/, int /*y*/) { /* no-op — 2-D positioning handled by caller's matrices */ }

void GetCurrentColor(float* rgba4)
{
    rgba4[0] = s_curColor[0];
    rgba4[1] = s_curColor[1];
    rgba4[2] = s_curColor[2];
    rgba4[3] = s_curColor[3];
}

void GetModelViewMatrix(float* m16)
{
    const glm::mat4& mv = s_modelviewStack.back();
    // glm stores column-major which matches OpenGL convention
    for (int i = 0; i < 16; ++i)
        m16[i] = glm::value_ptr(mv)[i];
}

} // namespace GLESFF

#endif // __ANDROID__
