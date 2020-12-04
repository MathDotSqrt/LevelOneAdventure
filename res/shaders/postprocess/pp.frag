#version 330
#include "preamble.glsl"

in vec2 f_uv;

uniform sampler2D color_attachment;

out vec4 out_color;

void main(){
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y);
  out_color = texture(color_attachment, uv) + vec4(.1);
}
