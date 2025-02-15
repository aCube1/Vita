#version 410

#extension GL_ARB_shading_language_420pack: enable

layout(std140, binding = 0) uniform VT_VertexParams {
    mat4 u_mvp;
};

in vec2 a_pos;
in vec2 a_uv;
in vec4 a_color;

out vec4 f_color;
out vec2 f_uv;

void main() {
    f_color = a_color;
    f_uv = a_uv;

    gl_Position = u_mvp * vec4(a_pos, 0.0, 1.0);
}
