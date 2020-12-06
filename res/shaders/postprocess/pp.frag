#include "postprocess/pp_base.frag"

uniform sampler2D blur;

out vec4 out_color;

vec3 tonemapFilmic(vec3 x) {
  vec3 X = max(vec3(0.0), x - 0.004);
  vec3 result = (X * (6.2 * X + 0.5)) / (X * (6.2 * X + 1.7) + 0.06);
  return pow(result, vec3(2.2));
}

vec3 unreal(vec3 x) {
  return x / (x + 0.155) * 1.019;
}

vec3 aces(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

vec3 exposure(vec3 color, float exposure){
  const float gamma = 2.2;
  vec3 mapped = vec3(1.0) - exp(-color * exposure);
  mapped = pow(mapped, vec3(1 / gamma));
  return mapped;
}

void main(){
  vec3 hdr_color = texture(color_attachment, getUV(color_attachment_size)).rgb;

  //vec3 mapped = hdr_color / (hdr_color + vec3(1));

  //out_color = vec4(exposure(hdr_color, 5), 1);
  out_color = vec4(unreal(hdr_color), 1);

}
