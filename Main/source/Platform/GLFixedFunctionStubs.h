#pragma once
#ifdef __ANDROID__
// ─────────────────────────────────────────────────────────────────────────────
// GLFixedFunctionStubs.h
// Drop-in replacement for <GL/gl.h> + <GL/glu.h> on Android.
// Include this (via StdAfx.h guards) instead of the Windows GL headers.
//
// Every fixed-function call is redirected to GLESFF:: or g_rs fields.
// ES3 passthrough calls (glBindTexture, glDrawArrays, etc.) map directly to
// the underlying GLES3 entry points after uploading current uniforms.
// ─────────────────────────────────────────────────────────────────────────────

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <dlfcn.h>
#include <cstdint>
#include "OpenGLESRenderBackend.h"
#include "RenderStateCompat.h"

namespace GLFixedNative
{
inline void* LoadProc(const char* name)
{
    static void* s_glesHandle = dlopen("libGLESv3.so", RTLD_NOW | RTLD_LOCAL);
    void* proc = nullptr;
    if (s_glesHandle)
    {
        proc = dlsym(s_glesHandle, name);
    }
    if (!proc)
    {
        proc = reinterpret_cast<void*>(eglGetProcAddress(name));
    }
    return proc;
}

template <typename Fn>
inline Fn Proc(const char* name)
{
    return reinterpret_cast<Fn>(LoadProc(name));
}
} // namespace GLFixedNative

// ─────────────────────────────────────────────────────────────────────────────
// Legacy GL tokens not present in GLES3 headers
// ─────────────────────────────────────────────────────────────────────────────
#ifndef GL_QUADS
#  define GL_QUADS          0x0007
#endif
#ifndef GL_POLYGON
#  define GL_POLYGON        0x0009
#endif
#ifndef GL_QUAD_STRIP
#  define GL_QUAD_STRIP     0x0008
#endif
#ifndef GL_LINE
#  define GL_LINE           0x1B01
#endif
#ifndef GL_FILL
#  define GL_FILL           0x1B02
#endif
#ifndef GL_VERTEX_ARRAY
#  define GL_VERTEX_ARRAY        0x8074
#endif
#ifndef GL_COLOR_ARRAY
#  define GL_COLOR_ARRAY         0x8076
#endif
#ifndef GL_NORMAL_ARRAY
#  define GL_NORMAL_ARRAY        0x8075
#endif
#ifndef GL_TEXTURE_COORD_ARRAY
#  define GL_TEXTURE_COORD_ARRAY 0x8078
#endif
#ifndef GL_CURRENT_COLOR
#  define GL_CURRENT_COLOR       0x0B00
#endif

// Alpha test token (ES3 does it in shader)
#ifndef GL_ALPHA_TEST
#  define GL_ALPHA_TEST     0x0BC0
#endif

// Fog tokens
#ifndef GL_FOG
#  define GL_FOG            0x0B60
#endif
#ifndef GL_FOG_MODE
#  define GL_FOG_MODE       0x0B65
#endif
#ifndef GL_FOG_DENSITY
#  define GL_FOG_DENSITY    0x0B62
#endif
#ifndef GL_FOG_START
#  define GL_FOG_START      0x0B63
#endif
#ifndef GL_FOG_END
#  define GL_FOG_END        0x0B64
#endif
#ifndef GL_FOG_COLOR
#  define GL_FOG_COLOR      0x0B66
#endif
#ifndef GL_LINEAR
#  define GL_LINEAR         0x2601
#endif
#ifndef GL_EXP
#  define GL_EXP            0x0800
#endif
#ifndef GL_EXP2
#  define GL_EXP2           0x0801
#endif

// Lighting tokens
#ifndef GL_LIGHTING
#  define GL_LIGHTING           0x0B50
#endif
#ifndef GL_LIGHT0
#  define GL_LIGHT0             0x4000
#endif
#ifndef GL_AMBIENT
#  define GL_AMBIENT            0x1200
#endif
#ifndef GL_DIFFUSE
#  define GL_DIFFUSE            0x1201
#endif
#ifndef GL_SPECULAR
#  define GL_SPECULAR           0x1202
#endif
#ifndef GL_POSITION
#  define GL_POSITION           0x1203
#endif
#ifndef GL_SHININESS
#  define GL_SHININESS          0x1601
#endif
#ifndef GL_EMISSION
#  define GL_EMISSION           0x1600
#endif
#ifndef GL_AMBIENT_AND_DIFFUSE
#  define GL_AMBIENT_AND_DIFFUSE 0x1602
#endif
#ifndef GL_COLOR_MATERIAL
#  define GL_COLOR_MATERIAL     0x0B57
#endif
#ifndef GL_NORMALIZE
#  define GL_NORMALIZE          0x0BA1
#endif
#ifndef GL_LIGHT_MODEL_AMBIENT
#  define GL_LIGHT_MODEL_AMBIENT 0x0B53
#endif

// Matrix mode tokens
#ifndef GL_MODELVIEW
#  define GL_MODELVIEW      0x1700
#endif
#ifndef GL_PROJECTION
#  define GL_PROJECTION     0x1701
#endif
#ifndef GL_TEXTURE
#  define GL_TEXTURE        0x1702
#endif
#ifndef GL_MODELVIEW_MATRIX
#  define GL_MODELVIEW_MATRIX    0x0BA6
#endif
#ifndef GL_PROJECTION_MATRIX
#  define GL_PROJECTION_MATRIX   0x0BA7
#endif
#ifndef GL_TEXTURE_MATRIX
#  define GL_TEXTURE_MATRIX      0x0BA8
#endif

// Texture env (no-op on ES3, kept for source compat)
#ifndef GL_TEXTURE_ENV
#  define GL_TEXTURE_ENV        0x2300
#endif
#ifndef GL_TEXTURE_ENV_MODE
#  define GL_TEXTURE_ENV_MODE   0x2200
#endif
#ifndef GL_MODULATE
#  define GL_MODULATE           0x2100
#endif
#ifndef GL_REPLACE
#  define GL_REPLACE            0x1E01
#endif
#ifndef GL_DECAL
#  define GL_DECAL              0x2101
#endif
#ifndef GL_ADD
#  define GL_ADD                0x0104
#endif

// Shade model (ES3 always smooth — kept for source compat)
#ifndef GL_SMOOTH
#  define GL_SMOOTH         0x1D01
#endif
#ifndef GL_FLAT
#  define GL_FLAT           0x1D00
#endif
#ifndef GL_SHADE_MODEL
#  define GL_SHADE_MODEL    0x0B54
#endif
#ifndef GL_LINE_SMOOTH
#  define GL_LINE_SMOOTH    0x0B20
#endif
#ifndef GL_LINE_SMOOTH_HINT
#  define GL_LINE_SMOOTH_HINT 0x0C52
#endif

// Raster ops stubs
#ifndef GL_RASTER_POS
#  define GL_RASTER_POS 0  // placeholder
#endif

// ─────────────────────────────────────────────────────────────────────────────
// glEnable / glDisable overrides
// ─────────────────────────────────────────────────────────────────────────────
inline void glEnable(GLenum cap)
{
    using EnableFn = void (*)(GLenum);
    static EnableFn nativeEnable = GLFixedNative::Proc<EnableFn>("glEnable");

    switch (cap)
    {
    case GL_TEXTURE_2D:      g_rs.texture2D     = true; break;
    case GL_BLEND:           g_rs.blend         = true; if (nativeEnable) nativeEnable(GL_BLEND); break;
    case GL_DEPTH_TEST:      g_rs.depthTest     = true; if (nativeEnable) nativeEnable(GL_DEPTH_TEST); break;
    case GL_CULL_FACE:       g_rs.cullFace      = true; if (nativeEnable) nativeEnable(GL_CULL_FACE); break;
    case GL_ALPHA_TEST:      g_rs.alphaTest     = true; break;
    case GL_FOG:             g_rs.fog           = true; break;
    case GL_LIGHTING:        g_rs.lighting      = true; break;
    case GL_COLOR_MATERIAL:  g_rs.colorMaterial = true; break;
    case GL_NORMALIZE:       g_rs.normalize     = true; break;
    case GL_SCISSOR_TEST:    g_rs.scissorTest   = true; if (nativeEnable) nativeEnable(GL_SCISSOR_TEST); break;
    case GL_POLYGON_OFFSET_FILL: g_rs.polygonOffsetFill = true; if (nativeEnable) nativeEnable(GL_POLYGON_OFFSET_FILL); break;
    default: if (nativeEnable) nativeEnable(cap); break;
    }
}
// Prevent recursive call for passthrough path above — undef the macro if needed
// (This works because the inline definition shadows the macro after this header)

inline void glDisable(GLenum cap)
{
    using DisableFn = void (*)(GLenum);
    static DisableFn nativeDisable = GLFixedNative::Proc<DisableFn>("glDisable");

    switch (cap)
    {
    case GL_TEXTURE_2D:      g_rs.texture2D     = false; break;
    case GL_BLEND:           g_rs.blend         = false; if (nativeDisable) nativeDisable(GL_BLEND); break;
    case GL_DEPTH_TEST:      g_rs.depthTest     = false; if (nativeDisable) nativeDisable(GL_DEPTH_TEST); break;
    case GL_CULL_FACE:       g_rs.cullFace      = false; if (nativeDisable) nativeDisable(GL_CULL_FACE); break;
    case GL_ALPHA_TEST:      g_rs.alphaTest     = false; break;
    case GL_FOG:             g_rs.fog           = false; break;
    case GL_LIGHTING:        g_rs.lighting      = false; break;
    case GL_COLOR_MATERIAL:  g_rs.colorMaterial = false; break;
    case GL_NORMALIZE:       g_rs.normalize     = false; break;
    case GL_SCISSOR_TEST:    g_rs.scissorTest   = false; if (nativeDisable) nativeDisable(GL_SCISSOR_TEST); break;
    case GL_POLYGON_OFFSET_FILL: g_rs.polygonOffsetFill = false; if (nativeDisable) nativeDisable(GL_POLYGON_OFFSET_FILL); break;
    default: if (nativeDisable) nativeDisable(cap); break;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Matrix stack
// ─────────────────────────────────────────────────────────────────────────────
namespace GLClientCompat
{
struct ClientArrayDesc
{
    bool enabled = false;
    GLint size = 0;
    GLenum type = GL_FLOAT;
    GLsizei stride = 0;
    const void* pointer = nullptr;
};

inline ClientArrayDesc& VertexArray()   { static ClientArrayDesc d; return d; }
inline ClientArrayDesc& ColorArray()    { static ClientArrayDesc d; return d; }
inline ClientArrayDesc& NormalArray()   { static ClientArrayDesc d; return d; }
inline ClientArrayDesc& TexCoordArray() { static ClientArrayDesc d; return d; }

inline size_t TypeSize(GLenum type)
{
    switch (type)
    {
    case GL_FLOAT:         return sizeof(float);
    case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
    case GL_BYTE:          return sizeof(int8_t);
    case GL_UNSIGNED_SHORT:return sizeof(uint16_t);
    case GL_SHORT:         return sizeof(int16_t);
    case GL_UNSIGNED_INT:  return sizeof(uint32_t);
    case GL_INT:           return sizeof(int32_t);
    default:               return 0;
    }
}

inline size_t EffectiveStride(const ClientArrayDesc& d)
{
    if (d.stride > 0)
    {
        return (size_t)d.stride;
    }
    const size_t ts = TypeSize(d.type);
    return ts > 0 ? (size_t)d.size * ts : 0;
}

inline const uint8_t* ElementPtr(const ClientArrayDesc& d, GLint index)
{
    if (!d.enabled || d.pointer == nullptr || d.size <= 0)
    {
        return nullptr;
    }

    const size_t stride = EffectiveStride(d);
    if (stride == 0)
    {
        return nullptr;
    }

    return reinterpret_cast<const uint8_t*>(d.pointer) + stride * (size_t)index;
}

inline void ReadFloatN(const ClientArrayDesc& d, GLint index, float* out, int outCount, float defaultValue)
{
    for (int i = 0; i < outCount; ++i)
    {
        out[i] = defaultValue;
    }

    const uint8_t* src = ElementPtr(d, index);
    if (!src)
    {
        return;
    }

    const int count = (d.size < outCount) ? d.size : outCount;
    switch (d.type)
    {
    case GL_FLOAT:
    {
        const float* p = reinterpret_cast<const float*>(src);
        for (int i = 0; i < count; ++i) out[i] = p[i];
        break;
    }
    case GL_UNSIGNED_BYTE:
    {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
        for (int i = 0; i < count; ++i) out[i] = (float)p[i] / 255.0f;
        break;
    }
    case GL_BYTE:
    {
        const int8_t* p = reinterpret_cast<const int8_t*>(src);
        for (int i = 0; i < count; ++i) out[i] = (float)p[i] / 127.0f;
        break;
    }
    default:
        break;
    }
}

inline bool HasActiveClientArrays()
{
    const auto& v = VertexArray();
    const auto& c = ColorArray();
    const auto& n = NormalArray();
    const auto& t = TexCoordArray();
    return (v.enabled && v.pointer) || (c.enabled && c.pointer) || (n.enabled && n.pointer) || (t.enabled && t.pointer);
}

inline void EmitClientArrayVertex(GLint index)
{
    float tex[2]   = {0.0f, 0.0f};
    float col[4]   = {1.0f, 1.0f, 1.0f, 1.0f};
    float normal[3]= {0.0f, 0.0f, 1.0f};
    float pos[3]   = {0.0f, 0.0f, 0.0f};

    ReadFloatN(TexCoordArray(), index, tex, 2, 0.0f);
    ReadFloatN(ColorArray(), index, col, 4, 1.0f);
    ReadFloatN(NormalArray(), index, normal, 3, 0.0f);
    if (!NormalArray().enabled || NormalArray().pointer == nullptr)
    {
        normal[2] = 1.0f;
    }
    ReadFloatN(VertexArray(), index, pos, 3, 0.0f);

    GLESFF::ImmTexCoord2f(tex[0], tex[1]);
    GLESFF::ImmColor4f(col[0], col[1], col[2], col[3]);
    GLESFF::ImmNormal3f(normal[0], normal[1], normal[2]);
    GLESFF::ImmVertex3f(pos[0], pos[1], pos[2]);
}
} // namespace GLClientCompat

inline void glEnableClientState(GLenum array)
{
    switch (array)
    {
    case GL_VERTEX_ARRAY:        GLClientCompat::VertexArray().enabled = true; break;
    case GL_COLOR_ARRAY:         GLClientCompat::ColorArray().enabled = true; break;
    case GL_NORMAL_ARRAY:        GLClientCompat::NormalArray().enabled = true; break;
    case GL_TEXTURE_COORD_ARRAY: GLClientCompat::TexCoordArray().enabled = true; break;
    default: break;
    }
}
inline void glDisableClientState(GLenum array)
{
    switch (array)
    {
    case GL_VERTEX_ARRAY:        GLClientCompat::VertexArray().enabled = false; break;
    case GL_COLOR_ARRAY:         GLClientCompat::ColorArray().enabled = false; break;
    case GL_NORMAL_ARRAY:        GLClientCompat::NormalArray().enabled = false; break;
    case GL_TEXTURE_COORD_ARRAY: GLClientCompat::TexCoordArray().enabled = false; break;
    default: break;
    }
}
inline void glPolygonMode(GLenum, GLenum) {}
inline void glVertexPointer(GLint size, GLenum type, GLsizei stride, const void* pointer)
{
    auto& d = GLClientCompat::VertexArray();
    d.size = size; d.type = type; d.stride = stride; d.pointer = pointer;
}
inline void glColorPointer(GLint size, GLenum type, GLsizei stride, const void* pointer)
{
    auto& d = GLClientCompat::ColorArray();
    d.size = size; d.type = type; d.stride = stride; d.pointer = pointer;
}
inline void glNormalPointer(GLenum type, GLsizei stride, const void* pointer)
{
    auto& d = GLClientCompat::NormalArray();
    d.size = 3; d.type = type; d.stride = stride; d.pointer = pointer;
}
inline void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const void* pointer)
{
    auto& d = GLClientCompat::TexCoordArray();
    d.size = size; d.type = type; d.stride = stride; d.pointer = pointer;
}
inline void glClientActiveTexture(GLenum) {}

inline void glMatrixMode(GLenum mode)
{
    switch (mode)
    {
    case GL_MODELVIEW:  GLESFF::MatrixMode(0); break;
    case GL_PROJECTION: GLESFF::MatrixMode(1); break;
    case GL_TEXTURE:    GLESFF::MatrixMode(2); break;
    }
}
inline void glPushMatrix()              { GLESFF::PushMatrix(); }
inline void glPopMatrix()               { GLESFF::PopMatrix(); }
inline void glLoadIdentity()            { GLESFF::LoadIdentity(); }
inline void glLoadMatrixf(const float* m) { GLESFF::LoadMatrixf(m); }
inline void glMultMatrixf(const float* m) { GLESFF::MultMatrixf(m); }
inline void glRotatef(float a,float x,float y,float z) { GLESFF::Rotatef(a,x,y,z); }
inline void glTranslatef(float x,float y,float z)      { GLESFF::Translatef(x,y,z); }
inline void glScalef(float x,float y,float z)          { GLESFF::Scalef(x,y,z); }
inline void glOrtho(double l,double r,double b,double t,double n,double f) { GLESFF::Ortho(l,r,b,t,n,f); }
inline void glFrustum(double l,double r,double b,double t,double n,double f) { GLESFF::Frustum(l,r,b,t,n,f); }

// ─────────────────────────────────────────────────────────────────────────────
// Immediate mode
// ─────────────────────────────────────────────────────────────────────────────
inline void glBegin(GLenum mode)        { GLESFF::ImmBegin(mode); }
inline void glEnd()                     { GLESFF::ImmEnd(); }

inline void glVertex3f(float x,float y,float z)  { GLESFF::ImmVertex3f(x,y,z); }
inline void glVertex3fv(const float* v)           { GLESFF::ImmVertex3fv(v); }
inline void glVertex3i(int x,int y,int z)         { GLESFF::ImmVertex3f((float)x,(float)y,(float)z); }
inline void glVertex2f(float x,float y)           { GLESFF::ImmVertex2f(x,y); }
inline void glVertex2i(int x,int y)               { GLESFF::ImmVertex2f((float)x,(float)y); }
inline void glVertex2fv(const float* v)           { GLESFF::ImmVertex2f(v[0],v[1]); }

inline void glTexCoord2f(float s,float t)   { GLESFF::ImmTexCoord2f(s,t); }
inline void glTexCoord2fv(const float* v)   { GLESFF::ImmTexCoord2f(v[0],v[1]); }
inline void glTexCoord1f(float s)           { GLESFF::ImmTexCoord2f(s,0); }

inline void glColor3f(float r,float g,float b)                     { GLESFF::ImmColor4f(r,g,b,1); }
inline void glColor4f(float r,float g,float b,float a)             { GLESFF::ImmColor4f(r,g,b,a); }
inline void glColor3fv(const float* v)                             { GLESFF::ImmColor4f(v[0],v[1],v[2],1); }
inline void glColor4fv(const float* v)                             { GLESFF::ImmColor4f(v[0],v[1],v[2],v[3]); }
inline void glColor4ub(unsigned char r,unsigned char g,unsigned char b,unsigned char a) { GLESFF::ImmColor4ub(r,g,b,a); }
inline void glColor3ub(unsigned char r,unsigned char g,unsigned char b)                 { GLESFF::ImmColor4ub(r,g,b,255); }
inline void glColor4ubv(const unsigned char* v) { GLESFF::ImmColor4ub(v[0],v[1],v[2],v[3]); }

inline void glNormal3f(float x,float y,float z)  { GLESFF::ImmNormal3f(x,y,z); }
inline void glNormal3fv(const float* v)           { GLESFF::ImmNormal3f(v[0],v[1],v[2]); }

// ─────────────────────────────────────────────────────────────────────────────
// Texture environment (store but ignore on ES3 — shader does modulate)
// ─────────────────────────────────────────────────────────────────────────────
inline void glTexEnvf(GLenum t,GLenum p,float v)  { GLESFF::TexEnvf(t,p,v); }
inline void glTexEnvi(GLenum,GLenum,int)           { /* no-op */ }
inline void glTexEnvfv(GLenum,GLenum,const float*) { /* no-op */ }

// glBindTexture redirect — keeps g_rs in sync
inline void glBindTexture(GLenum target, GLuint tex) { GLESFF::BindTexture(target, tex); }

// ─────────────────────────────────────────────────────────────────────────────
// Blend / depth / cull state
// ─────────────────────────────────────────────────────────────────────────────
inline void glBlendFunc(GLenum s, GLenum d)        { g_rs.blendSrc = s; g_rs.blendDst = d; }
inline void glDepthMask(GLboolean f)               { g_rs.depthMask = (f == GL_TRUE); }
inline void glDepthFunc(GLenum f)                  { g_rs.depthFunc = f; }
inline void glCullFace(GLenum m)                   { g_rs.cullFaceMode = m; }
inline void glPolygonOffset(float factor, float units) { g_rs.polygonOffsetFactor = factor; g_rs.polygonOffsetUnits = units; }
inline void glLineWidth(float w)                   { g_rs.lineWidth = w; }
inline void glPointSize(float)                     { /* ignored on ES3 */ }

// ─────────────────────────────────────────────────────────────────────────────
// Alpha test
// ─────────────────────────────────────────────────────────────────────────────
inline void glAlphaFunc(GLenum func, float ref)
{
    g_rs.alphaFunc = func;
    g_rs.alphaRef  = ref;
    // Shader uses GL_GREATER semantics: discard if color.a <= ref.
    // For GL_LESS we'd need to invert — handle the common cases only:
    // (MU uses GL_GREATER in nearly all paths)
}

// ─────────────────────────────────────────────────────────────────────────────
// Fog
// ─────────────────────────────────────────────────────────────────────────────
inline void glFogf(GLenum pname, float param)
{
    switch (pname)
    {
    case GL_FOG_MODE:    g_rs.fogMode = (RsFogMode)(int)param; break;
    case GL_FOG_DENSITY: g_rs.fogDensity = param; break;
    case GL_FOG_START:   g_rs.fogStart   = param; break;
    case GL_FOG_END:     g_rs.fogEnd     = param; break;
    }
}
inline void glFogi(GLenum pname, int param) { glFogf(pname, (float)param); }
inline void glFogfv(GLenum pname, const float* params)
{
    if (pname == GL_FOG_COLOR)
        g_rs.fogColor = {params[0],params[1],params[2],params[3]};
    else
        glFogf(pname, params[0]);
}

// ─────────────────────────────────────────────────────────────────────────────
// Lighting
// ─────────────────────────────────────────────────────────────────────────────
inline void glLightfv(GLenum light, GLenum pname, const float* params)
{
    if (light != GL_LIGHT0) return; // only light 0 supported
    switch (pname)
    {
    case GL_AMBIENT:  g_rs.lightAmbient  = {params[0],params[1],params[2],params[3]}; break;
    case GL_DIFFUSE:  g_rs.lightDiffuse  = {params[0],params[1],params[2],params[3]}; break;
    case GL_SPECULAR: g_rs.lightSpecular = {params[0],params[1],params[2],params[3]}; break;
    case GL_POSITION: RS_SetLightPosition(params); break;
    }
}
inline void glLightf(GLenum, GLenum, float) { /* attenuation — ignored */ }

inline void glMaterialfv(GLenum face, GLenum pname, const float* params)
{
    (void)face; // GL_FRONT and GL_FRONT_AND_BACK treated the same
    switch (pname)
    {
    case GL_AMBIENT:            g_rs.matAmbient  = {params[0],params[1],params[2],params[3]}; break;
    case GL_DIFFUSE:            g_rs.matDiffuse  = {params[0],params[1],params[2],params[3]}; break;
    case GL_SPECULAR:           g_rs.matSpecular = {params[0],params[1],params[2],params[3]}; break;
    case GL_EMISSION:           g_rs.matEmission = {params[0],params[1],params[2],params[3]}; break;
    case GL_AMBIENT_AND_DIFFUSE:
        g_rs.matAmbient = g_rs.matDiffuse = {params[0],params[1],params[2],params[3]};
        break;
    case GL_SHININESS: g_rs.matShininess = params[0]; break;
    }
}
inline void glMaterialf(GLenum face, GLenum pname, float param)
{
    float v[4] = {param,param,param,param};
    glMaterialfv(face, pname, v);
}

inline void glLightModelfv(GLenum pname, const float* params)
{
    if (pname == GL_LIGHT_MODEL_AMBIENT)
        g_rs.globalAmbient = {params[0],params[1],params[2],params[3]};
}
inline void glLightModelf(GLenum, float)  { /* other model params ignored */ }
inline void glLightModeli(GLenum, int)    { /* two-sided lighting ignored */ }

// ─────────────────────────────────────────────────────────────────────────────
// Shade model (ES3 is always smooth)
// ─────────────────────────────────────────────────────────────────────────────
inline void glShadeModel(GLenum) { /* no-op */ }

// ─────────────────────────────────────────────────────────────────────────────
// Raster / pixel ops (no-op / passthrough)
// ─────────────────────────────────────────────────────────────────────────────
inline void glRasterPos2i(int x, int y) { GLESFF::RasterPos2i(x, y); }
inline void glRasterPos2f(float, float) { }
inline void glPixelStorei(GLenum pname, GLint param)
{
    using Fn = void (*)(GLenum, GLint);
    static Fn fn = GLFixedNative::Proc<Fn>("glPixelStorei");
    if (fn) fn(pname, param);
}
inline void glReadPixels(GLint x,GLint y,GLsizei w,GLsizei h,GLenum fmt,GLenum type,void* d)
{
    using Fn = void (*)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, void*);
    static Fn fn = GLFixedNative::Proc<Fn>("glReadPixels");
    if (fn) fn(x, y, w, h, fmt, type, d);
}

// ─────────────────────────────────────────────────────────────────────────────
// Viewport / scissor
// ─────────────────────────────────────────────────────────────────────────────
inline void glViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
    g_rs.vpX=x; g_rs.vpY=y; g_rs.vpW=w; g_rs.vpH=h;
    using Fn = void (*)(GLint, GLint, GLsizei, GLsizei);
    static Fn fn = GLFixedNative::Proc<Fn>("glViewport");
    if (fn) fn(x, y, w, h);
}
inline void glScissor(GLint x, GLint y, GLsizei w, GLsizei h)
{
    g_rs.scissorX=x; g_rs.scissorY=y; g_rs.scissorW=w; g_rs.scissorH=h;
    using Fn = void (*)(GLint, GLint, GLsizei, GLsizei);
    static Fn fn = GLFixedNative::Proc<Fn>("glScissor");
    if (fn) fn(x, y, w, h);
}

// ─────────────────────────────────────────────────────────────────────────────
// Color mask / clear
// ─────────────────────────────────────────────────────────────────────────────
inline void glClearColor(float r,float g,float b,float a)
{
    using Fn = void (*)(GLfloat, GLfloat, GLfloat, GLfloat);
    static Fn fn = GLFixedNative::Proc<Fn>("glClearColor");
    if (fn) fn(r, g, b, a);
}
inline void glClear(GLbitfield mask)
{
    using Fn = void (*)(GLbitfield);
    static Fn fn = GLFixedNative::Proc<Fn>("glClear");
    if (fn) fn(mask);
}
inline void glColorMask(GLboolean r,GLboolean g,GLboolean b,GLboolean a)
{
    using Fn = void (*)(GLboolean, GLboolean, GLboolean, GLboolean);
    static Fn fn = GLFixedNative::Proc<Fn>("glColorMask");
    if (fn) fn(r, g, b, a);
}
inline void glClearDepthf(float d)
{
    using Fn = void (*)(GLfloat);
    static Fn fn = GLFixedNative::Proc<Fn>("glClearDepthf");
    if (fn) fn(d);
}

// ─────────────────────────────────────────────────────────────────────────────
// Flush / finish
// ─────────────────────────────────────────────────────────────────────────────
inline void glFlush()
{
    using Fn = void (*)();
    static Fn fn = GLFixedNative::Proc<Fn>("glFlush");
    if (fn) fn();
}
inline void glFinish()
{
    using Fn = void (*)();
    static Fn fn = GLFixedNative::Proc<Fn>("glFinish");
    if (fn) fn();
}

// ─────────────────────────────────────────────────────────────────────────────
// Direct draw calls — upload uniforms first
// ─────────────────────────────────────────────────────────────────────────────
inline void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (mode == GL_QUADS && GLClientCompat::HasActiveClientArrays())
    {
        GLESFF::ImmBegin(GL_TRIANGLES);
        const GLsizei quadCount = count / 4;
        for (GLsizei q = 0; q < quadCount; ++q)
        {
            const GLint base = first + q * 4;

            GLClientCompat::EmitClientArrayVertex(base + 0);
            GLClientCompat::EmitClientArrayVertex(base + 1);
            GLClientCompat::EmitClientArrayVertex(base + 2);

            GLClientCompat::EmitClientArrayVertex(base + 0);
            GLClientCompat::EmitClientArrayVertex(base + 2);
            GLClientCompat::EmitClientArrayVertex(base + 3);
        }
        GLESFF::ImmEnd();
        return;
    }

    if (GLClientCompat::HasActiveClientArrays())
    {
        GLESFF::ImmBegin(mode);
        for (GLint i = 0; i < count; ++i)
        {
            GLClientCompat::EmitClientArrayVertex(first + i);
        }
        GLESFF::ImmEnd();
        return;
    }

    GLESFF::FlushUniforms();
    using Fn = void (*)(GLenum, GLint, GLsizei);
    static Fn fn = GLFixedNative::Proc<Fn>("glDrawArrays");
    if (!fn)
    {
        return;
    }

    if (mode == GL_QUADS)
    {
        const GLsizei quadCount = count / 4;
        for (GLsizei q = 0; q < quadCount; ++q)
        {
            fn(GL_TRIANGLE_FAN, first + q * 4, 4);
        }
        return;
    }

    fn(mode, first, count);
}
inline void glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
    auto ReadElementIndex = [indices, type](GLsizei element)->GLint
    {
        if (indices == nullptr)
        {
            return (GLint)element;
        }

        switch (type)
        {
        case GL_UNSIGNED_BYTE:
            return (GLint)reinterpret_cast<const uint8_t*>(indices)[element];
        case GL_UNSIGNED_SHORT:
            return (GLint)reinterpret_cast<const uint16_t*>(indices)[element];
        case GL_UNSIGNED_INT:
            return (GLint)reinterpret_cast<const uint32_t*>(indices)[element];
        default:
            return -1;
        }
    };

    if (GLClientCompat::HasActiveClientArrays())
    {
        if (mode == GL_QUADS)
        {
            GLESFF::ImmBegin(GL_TRIANGLES);
            const GLsizei quadCount = count / 4;
            for (GLsizei q = 0; q < quadCount; ++q)
            {
                const GLint i0 = ReadElementIndex(q * 4 + 0);
                const GLint i1 = ReadElementIndex(q * 4 + 1);
                const GLint i2 = ReadElementIndex(q * 4 + 2);
                const GLint i3 = ReadElementIndex(q * 4 + 3);
                if (i0 < 0 || i1 < 0 || i2 < 0 || i3 < 0)
                {
                    continue;
                }

                GLClientCompat::EmitClientArrayVertex(i0);
                GLClientCompat::EmitClientArrayVertex(i1);
                GLClientCompat::EmitClientArrayVertex(i2);

                GLClientCompat::EmitClientArrayVertex(i0);
                GLClientCompat::EmitClientArrayVertex(i2);
                GLClientCompat::EmitClientArrayVertex(i3);
            }
            GLESFF::ImmEnd();
            return;
        }

        GLESFF::ImmBegin(mode);
        for (GLsizei i = 0; i < count; ++i)
        {
            const GLint idx = ReadElementIndex(i);
            if (idx < 0)
            {
                continue;
            }
            GLClientCompat::EmitClientArrayVertex(idx);
        }
        GLESFF::ImmEnd();
        return;
    }

    GLESFF::FlushUniforms();
    using Fn = void (*)(GLenum, GLsizei, GLenum, const void*);
    static Fn fn = GLFixedNative::Proc<Fn>("glDrawElements");
    if (!fn)
    {
        return;
    }

    if (mode == GL_QUADS)
    {
        const size_t indexSize = GLClientCompat::TypeSize(type);
        if (indexSize == 0)
        {
            return;
        }

        const GLsizei quadCount = count / 4;
        const uintptr_t baseOffset = reinterpret_cast<uintptr_t>(indices);
        for (GLsizei q = 0; q < quadCount; ++q)
        {
            const uintptr_t quadOffset = baseOffset + (uintptr_t)(q * 4) * indexSize;
            fn(GL_TRIANGLE_FAN, 4, type, reinterpret_cast<const void*>(quadOffset));
        }
        return;
    }

    fn(mode, count, type, indices);
}

// ─────────────────────────────────────────────────────────────────────────────
// GLU replacement
// ─────────────────────────────────────────────────────────────────────────────
inline void gluPerspective(float fovy, float aspect, float zNear, float zFar)
{
    GLESFF::PerspectiveFov(fovy, aspect, zNear, zFar);
}
inline void gluLookAt(float ex,float ey,float ez,
                      float cx,float cy,float cz,
                      float ux,float uy,float uz)
{
    GLESFF::LookAt(ex,ey,ez, cx,cy,cz, ux,uy,uz);
}
inline void gluOrtho2D(float l,float r,float b,float t)
{
    GLESFF::Ortho(l,r,b,t,-1,1);
}

// ─────────────────────────────────────────────────────────────────────────────
// Passthrough ES3 calls (no fixed-function equivalent, just re-export)
// ─────────────────────────────────────────────────────────────────────────────
// glGenTextures, glDeleteTextures, glTexImage2D, glTexParameteri, etc.
// are not overridden — they call the native GLES3 entry points directly.
// glUseProgram, glUniform*, glVertexAttrib*, glEnableVertexAttribArray, etc.
// are also native — used by UI code that already writes ES2/3 shaders.

#endif // __ANDROID__
