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
uniform int       u_texEnvMode;      // 0=modulate, 1=add, 2=replace, 3=modulate*2
uniform float     u_texEnvRgbScale;  // combines with MODULATE for GL_RGB_SCALE

// Alpha test (mirrors glAlphaFunc)
uniform bool  u_alphaTestEnabled;
uniform float u_alphaTestRef;    // e.g. 0.25 for GL_GREATER
uniform bool  u_blendEnabled;
uniform bool  u_autoAlphaDiscardEnabled;
uniform float u_autoAlphaDiscardRef;

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
    vec4 texel = vec4(1.0);
    vec4 color = v_color;
    if (u_useTexture)
    {
        texel = texture(u_texture, v_texcoord);
        color = ApplyTexEnv(texel, v_color);
    }

    // Cutout threshold for foliage/items with binary-like alpha masks.
    const float kCutoutAlpha = 0.1;

    // Fallback for legacy cutout assets: if a textured draw forgot to enable
    // alpha test/blending, still discard fully transparent texels.
    if (u_useTexture &&
        u_autoAlphaDiscardEnabled &&
        !u_alphaTestEnabled &&
        !u_blendEnabled &&
        texel.a < max(u_autoAlphaDiscardRef, kCutoutAlpha))
    {
        discard;
    }

    // Alpha test
    if (u_alphaTestEnabled && color.a < max(u_alphaTestRef, kCutoutAlpha))
        discard;

    // Fog
    if (u_fogEnabled)
    {
        float fogFactor = CalcFogFactor();
        color.rgb = mix(u_fogColor.rgb, color.rgb, fogFactor);
    }

    fragColor = color;
}
