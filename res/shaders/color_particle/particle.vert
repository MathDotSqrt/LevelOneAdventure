#version 330
#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_uv;
layout(location=POSITION_SIZE_ATTRIB_LOCATION) in vec4 v_pos_size;
layout(location=COLOR_ATTRIB_LOCATION) in vec4 v_color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec4 f_color;

mat4 translate(mat4 m, vec3 pos){
  m[3][0] += pos.x;
  m[3][1] += pos.y;
  m[3][2] += pos.z;
  return m;
}

mat4 billboard(mat4 m, vec3 scale=vec3(1)){
  // Column 0:
  m[0][0] = scale.x;
  m[0][1] = 0;
  m[0][2] = 0;

  // Column 1:
  m[1][0] = 0;
  m[1][1] = scale.y;
  m[1][2] = 0;

  // Column 2:
  m[2][0] = 0;
  m[2][1] = 0;
  m[2][2] = scale.z;

  return m;
}

void main(){
  vec3 offset = v_pos_size.xyz;
  float size = v_pos_size.w;

  mat4 MV = V * translate(M, offset);
	gl_Position = P * billboard(MV, vec3(size)) * vec4(v_pos, 1);

  f_color = v_color;
}