#version 330
#include "preamble.glsl"

in vec2 f_uv;

uniform sampler2D color_attachment;
uniform vec2 fbo_size;
uniform vec2 window_size;

out vec4 out_color;

void main(){
  vec2 scale = window_size / fbo_size;
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y) * scale;
  out_color = texture(color_attachment, uv);
}
