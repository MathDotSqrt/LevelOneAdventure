#version 330
#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec2 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_uv;

out vec2 f_uv;

void main(){
  gl_Position = vec4(v_pos.x, v_pos.y, 0, 1);

  f_uv = v_uv;
}
