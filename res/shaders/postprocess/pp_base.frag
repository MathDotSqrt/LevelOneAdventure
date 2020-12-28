#version 330
#include "preamble.glsl"
#include "./fbo_attachment.glsl"

in vec2 f_uv;

uniform fbo_attachment_size color_attachment_size;

vec2 getUV(){
  return getUV(color_attachment_size, f_uv);
}

vec2 getUV(fbo_attachment_size attachment){
  return getUV(attachment, f_uv);
}
