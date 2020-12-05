#version 330
#include "preamble.glsl"

in vec2 f_uv;

struct fbo_attachment_size{
  vec2 fbo_size;
  vec2 window_size;
};

uniform sampler2D color_attachment;
uniform fbo_attachment_size color_attachment_size;
// uniform vec2 fbo_size;
// uniform vec2 window_size;

vec2 getUV(){
  vec2 scale = color_attachment_size.window_size / color_attachment_size.fbo_size;
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y) * scale;
  return uv;
}

vec2 getUV(fbo_attachment_size attachment){
  vec2 scale = attachment.window_size / attachment.fbo_size;
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y) * scale;
  return uv;
}
