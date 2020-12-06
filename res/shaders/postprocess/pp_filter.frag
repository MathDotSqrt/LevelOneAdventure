#include "postprocess/pp_base.frag"
#include "math.glsl"

out vec4 out_color;

void main(){
  vec3 tex_color = texture(color_attachment, getUV()).rgb;

  if(max3(tex_color) > .3){
    out_color = vec4(tex_color, 1);
  }
  else{
    out_color = vec4(0);
  }
}
