#include "postprocess/pp_base.frag"
#include "math.glsl"

//uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

//uniform float weight[7] = float[](0.098779, 0.09683, 0.091209, 0.082557, 0.071805, 0.060013, 0.048196);
uniform float weight[10] = float[](0.084509,0.082841,0.078033,0.07063,0.061432,0.051343,0.041234,0.031821,0.023597,0.016815);
uniform sampler2D color_attachment;

out vec4 out_color;

void main(){
  vec2 uv = getUV();

  vec2 tex_offset = 1 / vec2(color_attachment_size.fbo_size);
  vec3 tex_color = texture(color_attachment, uv).rgb * weight[0];

  for(int i = 1; i < 10; i++){
    tex_color += texture(color_attachment, uv + vec2(0, tex_offset.y * i)).rgb * weight[i];
    tex_color += texture(color_attachment, uv - vec2(0, tex_offset.y * i)).rgb * weight[i];
  }

  out_color = vec4(tex_color, 1);
}
