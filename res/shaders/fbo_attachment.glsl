#ifndef FBO_ATTACHMENT_GLSL
#define FBO_ATTACHMENT_GLSL

struct fbo_attachment_size{
  vec2 fbo_size;
  vec2 window_size;
};

vec2 getUV(fbo_attachment_size attachment, vec2 f_uv){
  vec2 scale = attachment.window_size / attachment.fbo_size;
  vec2 uv = vec2(f_uv.x, 1 - f_uv.y) * scale;
  return uv;
}

#endif
