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

// Alpha test (mirrors glAlphaFunc)
uniform bool  u_alphaTestEnabled;
uniform float u_alphaTestRef;    // e.g. 0.25 for GL_GREATER

// Fog
uniform bool  u_fogEnabled;
uniform int   u_fogMode;         // 0=linear, 1=exp, 2=exp2
uniform vec4  u_fogColor;
uniform float u_fogStart;
uniform float u_fogEnd;
uniform float u_fogDensity;

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
    vec4 color;
    if (u_useTexture)
        color = texture(u_texture, v_texcoord) * v_color;
    else
        color = v_color;

    // Alpha test
    if (u_alphaTestEnabled && color.a <= u_alphaTestRef)
        discard;

    // Fog
    if (u_fogEnabled)
    {
        float fogFactor = CalcFogFactor();
        color.rgb = mix(u_fogColor.rgb, color.rgb, fogFactor);
    }

    fragColor = color;
}
