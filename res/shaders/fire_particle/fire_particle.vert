#version 330
#include "preamble.glsl"
#include "math.glsl"

//quad
layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_uv;

//instanced
layout(location=POSITION_SIZE_ATTRIB_LOCATION) in vec4 v_pos_size;
layout(location=COLOR_ATTRIB_LOCATION) in vec4 v_color;
layout(location=TEX_INDEX_ATTRIB_LOCATION) in vec2 v_tex_index_angle;
layout(location=LIFE_TIME_LOCATION) in float v_lifetime;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 f_uv;
out vec4 f_color;
flat out float f_tex_index;
flat out float f_lifetime;

void main(){
  vec3 offset = v_pos_size.xyz;
  float size = v_pos_size.w;
  float tex_index = v_tex_index_angle.x;
  float angle = v_tex_index_angle.y;

  mat4 MV = V * translate(M, offset);
	gl_Position = P * billboard(MV, vec3(size)) * rotateZ(angle) * vec4(v_pos, 1);

  f_uv = v_uv;
  f_color = v_color;
  f_tex_index = tex_index;
  f_lifetime = v_lifetime;
}
