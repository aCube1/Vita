#version 410 core
#extension GL_ARB_shading_language_420pack: enable

layout(binding = 0) uniform sampler2D u_tex0;

in vec4 f_color;
in vec2 f_uv;

out vec4 out_color;

void main() {
    out_color = texture(u_tex0, f_uv) * f_color;
}
