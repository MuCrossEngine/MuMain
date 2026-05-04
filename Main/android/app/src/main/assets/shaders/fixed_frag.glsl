#version 300 es
precision mediump float;
// ─────────────────────────────────────────────────────────────────────────────
// fixed_frag.glsl
// Emulates fixed-function fragment operations:
//   - Texture modulation with vertex color
//   - Alpha test (glAlphaFunc)
//   - Linear/exponential fog (glFog*)
// ─────────────────────────────────────────────────────────────────────────────

in vec2  v_texcoord;
in vec4  v_color;
in float v_fogDepth;

out vec4 fragColor;

// Texturing
uniform sampler2D u_texture;
uniform bool      u_useTexture;
uniform bool      u_texture2DEnabled;
uniform int       u_texEnvMode;      // 0=modulate, 1=add, 2=replace, 3=modulate*2
uniform float     u_texEnvRgbScale;  // combines with MODULATE for GL_RGB_SCALE

// Alpha test (mirrors glAlphaFunc)
uniform bool  u_alphaTestEnabled;
uniform float u_alphaTestRef;    // e.g. 0.25 for GL_GREATER
uniform bool  u_blendEnabled;

// Universal alpha-cutoff threshold — GLES 3.0 has no glAlphaFunc, so
// any textured pixel below this threshold is discarded unconditionally.
// This removes black rectangles around trees, wings, grass, etc.
uniform float u_alphaCutoff;     // default 0.1, set from C++ side

// True when blend mode is additive/subtractive (GL_ONE/GL_ONE, etc.).
// Additive blends make black pixels invisible via the blend equation,
// so the alpha-cutoff discard must NOT fire — it would kill valid glow.
uniform bool  u_additiveBlend;

// True when the bound texture was uploaded as GL_RGBA (has a real alpha
// channel).  False for GL_RGB textures (OZJ/JPEG) where alpha is always
// 1.0 and the game uses black (0,0,0) as the transparency key instead.
uniform bool  u_textureHasAlpha;

// Fog
uniform bool  u_fogEnabled;
uniform int   u_fogMode;         // 0=linear, 1=exp, 2=exp2
uniform vec4  u_fogColor;
uniform float u_fogStart;
uniform float u_fogEnd;
uniform float u_fogDensity;

vec4 ApplyTexEnv(vec4 texel, vec4 primary)
{
    if (u_texEnvMode == 1)
    {
        // GL_ADD
        vec3 rgb = min(texel.rgb + primary.rgb, vec3(1.0));
        float a = texel.a * primary.a;
        return vec4(rgb, a);
    }

    if (u_texEnvMode == 2)
    {
        // GL_REPLACE
        return texel;
    }

    // GL_MODULATE and GL_COMBINE-like MODULATE*2
    vec4 c = texel * primary;
    float rgbScale = max(u_texEnvRgbScale, 1.0);
    if (u_texEnvMode == 3)
    {
        rgbScale = max(rgbScale, 2.0);
    }
    c.rgb = min(c.rgb * rgbScale, vec3(1.0));
    return c;
}

float CalcFogFactor()
{
    if (u_fogMode == 0) // GL_LINEAR
    {
        return clamp((u_fogEnd - v_fogDepth) / (u_fogEnd - u_fogStart), 0.0, 1.0);
    }
    else if (u_fogMode == 1) // GL_EXP
    {
        float f = u_fogDensity * v_fogDepth;
        return exp(-f);
    }
    else // GL_EXP2
    {
        float f = u_fogDensity * v_fogDepth;
        return exp(-f * f);
    }
}

void main()
{
    // Fixed-function state requested texturing but the texture object is invalid (id=0).
    // Discard avoids rendering solid fallback quads for unloaded assets.
    if (u_texture2DEnabled && !u_useTexture)
    {
        discard;
    }

    vec4 texel = vec4(1.0);
    vec4 color = v_color;
    if (u_useTexture)
    {
        texel = texture(u_texture, v_texcoord);
        color = ApplyTexEnv(texel, v_color);
    }

    // ── GLES 3.0 transparency discard ──────────────────────────────────
    // Two mechanisms replace desktop GL_ALPHA_TEST:
    //
    // 1) Alpha-cutoff: for RGBA textures (u_textureHasAlpha == true),
    //    discard fragments below the threshold.  This handles trees,
    //    wings, grass, fences — anything with a real alpha channel.
    //
    // 2) Color-key (black = transparent): for RGB textures (OZJ/JPEG)
    //    where u_textureHasAlpha == false and alpha is always 1.0.
    //    The original game treated pure black as transparent.
    //    A pixel with R+G+B < 0.16 (~41/255 combined) is discarded.
    //
    // Neither discard fires for additive/subtractive blends, because
    // those blend modes make black invisible via the blend equation
    // (0 + dst = dst).  Discarding would create holes in glow effects.
    // Run transparency discard logic only on blended draws.
    // Opaque world geometry (blend OFF) should skip this path for performance.
    if (u_useTexture && u_blendEnabled && !u_additiveBlend)
    {
        if (u_textureHasAlpha)
        {
            // Real alpha channel — standard cutoff
            if (texel.a < u_alphaCutoff)
            {
                discard;
            }
        }
        else
        {
            // No alpha channel (GL_RGB): use color-key.
            // Black background pixels are discarded as transparent.
            if ((texel.r + texel.g + texel.b) < 0.16)
            {
                discard;
            }
        }
    }

    // GL_GREATER: pass if alpha > ref → discard if alpha <= ref.
    // Use <= to match desktop GL_GREATER semantics exactly.
    // Only applies when the game explicitly set glAlphaFunc.
    if (u_alphaTestEnabled && !u_additiveBlend && color.a <= u_alphaTestRef)
        discard;

    // Fog
    if (u_fogEnabled)
    {
        float fogFactor = CalcFogFactor();
        color.rgb = mix(u_fogColor.rgb, color.rgb, fogFactor);
    }

    fragColor = color;
}
