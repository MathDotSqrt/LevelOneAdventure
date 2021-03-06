#version 330
#include "color_space.glsl"
in vec2 f_uv;
in vec4 f_color;
flat in float f_tex_index;
flat in float f_lifetime;

out vec4 out_color;

uniform sampler2D diffuse;

vec2 sample_grid(int index){
  int x = index % 2;
  int y = index / 2;

  vec2 grid = f_uv  * vec2(.5);
  return grid + vec2(.5) * vec2(x, y);
}

vec4 lerp2(vec4 c1, vec4 c2, float t){
  return mix(c1, c2, t);
}

vec4 lerp3(vec4 c0, vec4 c1, vec4 c2, float t){
  if(t < .5){
      return mix(c0, c1, 2 * t);
  }
  return mix(c1, c2, 2 * t - 1);
}

void main(){
  vec4 c0 = vec4(to_linear(vec3(255.0/255.0, 183/255.0, 49/255.0)), .0);
  vec4 c1 = vec4(to_linear(vec3(219/255.0, 70/255.0, 47/255.0)), .7);
  vec4 c2 = vec4(to_linear(vec3(0, 0, .2)), 0);

  // vec4 c0 = vec4(vec3(1, .7, .1), .1);
  // vec4 c1 = vec4(vec3(1, .3, .1), .9);
  // vec4 c2 = vec4(vec3(0, 0, 0), .0);

  vec2 uv = sample_grid(int(f_tex_index));
  float alpha = texture(diffuse, uv).a;
  float t = 1 - f_lifetime / .6;

  vec4 mix_color = lerp3(c0, c1, c2, t);
  out_color = vec4((mix_color.rgb) * 2, alpha * mix_color.a);
}
