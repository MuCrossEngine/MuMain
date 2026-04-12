#version 300 es
// ─────────────────────────────────────────────────────────────────────────────
// fixed_vert.glsl
// Emulates the OpenGL fixed-function vertex pipeline:
//   - Model/View/Projection transforms
//   - Single directional light (Blinn-Phong per-vertex)
//   - Texture coordinate passthrough
//   - Fog depth calculation
// ─────────────────────────────────────────────────────────────────────────────

// Per-vertex attributes (from immediate-mode VBO)
in vec3  a_position;
in vec2  a_texcoord;
in vec4  a_color;
in vec3  a_normal;

// Matrices
uniform mat4 u_mvp;           // modelview * projection
uniform mat4 u_modelview;     // modelview only (for lighting)
uniform mat3 u_normalMatrix;  // transpose(inverse(modelview)) 3x3

// Lighting (mirror glLightfv / glMaterialfv)
uniform bool  u_lightingEnabled;
uniform vec4  u_lightAmbient;
uniform vec4  u_lightDiffuse;
uniform vec4  u_lightSpecular;
uniform vec3  u_lightPos;         // in eye space
uniform vec4  u_materialAmbient;
uniform vec4  u_materialDiffuse;
uniform vec4  u_materialSpecular;
uniform float u_materialShininess;
uniform vec4  u_materialEmission;
uniform vec4  u_globalAmbient;

// Outputs to fragment shader
out vec2  v_texcoord;
out vec4  v_color;
out float v_fogDepth;

void main()
{
    vec4 eyePos = u_modelview * vec4(a_position, 1.0);
    gl_Position = u_mvp * vec4(a_position, 1.0);

    v_texcoord = a_texcoord;
    v_fogDepth = abs(eyePos.z);

    if (u_lightingEnabled)
    {
        vec3 N = normalize(u_normalMatrix * a_normal);
        vec3 L = normalize(u_lightPos - eyePos.xyz);
        vec3 V = normalize(-eyePos.xyz);
        vec3 H = normalize(L + V);

        float diff = max(dot(N, L), 0.0);
        float spec = (diff > 0.0) ? pow(max(dot(N, H), 0.0), u_materialShininess) : 0.0;

        vec4 ambient  = u_globalAmbient * u_materialAmbient
                      + u_lightAmbient  * u_materialAmbient;
        vec4 diffuse  = u_lightDiffuse  * u_materialDiffuse  * diff;
        vec4 specular = u_lightSpecular * u_materialSpecular * spec;

        v_color = clamp(u_materialEmission + ambient + diffuse + specular, 0.0, 1.0);
        v_color.a = u_materialDiffuse.a;
    }
    else
    {
        v_color = a_color;
    }
}
