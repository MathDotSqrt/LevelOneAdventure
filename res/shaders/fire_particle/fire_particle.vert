#version 330
#include "preamble.glsl"

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

mat4 rotateZ(float angle){
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(
      c, -s, 0, 0,
      s, c, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    );
}

mat4 rotate(float angle, vec3 axis){
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(
      oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
      oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
      oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
      0.0, 0.0, 0.0, 1.0
    );
}

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
