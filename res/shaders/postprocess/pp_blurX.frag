#include "postprocess/pp_base.frag"
#include "math.glsl"

uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

out vec4 out_color;

void main(){
  vec2 uv = getUV();

  vec2 tex_offset = 1 / vec2(color_attachment_size.window_size);
  vec3 tex_color = texture(color_attachment, uv).rgb * weight[0];

  for(int i = 1; i < 5; i++){
    tex_color += texture(color_attachment, uv + vec2(tex_offset.x * i, 0)).rgb * weight[i];
    tex_color += texture(color_attachment, uv - vec2(tex_offset.x * i, 0)).rgb * weight[i];
  }

  out_color = vec4(tex_color, 1);
}
