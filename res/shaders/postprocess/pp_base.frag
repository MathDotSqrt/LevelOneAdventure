#version 330
#include "preamble.glsl"

in vec2 f_uv;

struct fbo_attachment{
  sampler2D attachment;
  vec2 fbo_size;
  vec2 window_size;
}

uniform fbo_attachment color_attachment;

uniform sampler2D color_attachment;
uniform vec2 fbo_size;
uniform vec2 window_size;

out vec4 out_color;

vec2 getUV(){
  vec2 scale = window_size / fbo_size;
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y) * scale;
  return uv;
}
