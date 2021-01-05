#version 330
#include "color_space.glsl"

layout (location = 0) out vec3 g_view_pos;
layout (location = 1) out vec3 g_view_normal;
layout (location = 2) out vec4 g_albedo;

in vec3 f_view_pos;
in vec3 f_view_normal;
in vec2 f_texcoord;

uniform vec3 u_color;
uniform sampler2D diffuse;

void main(){

  vec3 tex_color = to_linear(texture(diffuse, f_texcoord).rgb);

  tex_color *= to_linear(u_color);

  g_view_pos = f_view_pos;
  g_view_normal = normalize(f_view_normal);
  g_albedo = vec4(tex_color, 1);
}
