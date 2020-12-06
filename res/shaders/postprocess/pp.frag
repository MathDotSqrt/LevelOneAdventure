#include "postprocess/pp_base.frag"

uniform sampler2D blur;

out vec4 out_color;


void main(){
  const float gamma = 2.2;
  vec3 hdr_color = texture(color_attachment, getUV(color_attachment_size)).rgb;

  //vec3 mapped = hdr_color / (hdr_color + vec3(1));
  const float exposure = 5;
  vec3 mapped = vec3(1.0) - exp(-hdr_color * exposure);
  mapped = pow(mapped, vec3(1 / gamma));
  out_color = vec4(mapped, 1);
}
