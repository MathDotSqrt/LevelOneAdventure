#include "postprocess/pp_base.frag"
#include "math.glsl"

uniform sampler2D color_attachment;

out vec4 out_color;

float luminance0(vec3 color){
  return max3(color);
}

float luminance1(vec3 color){
  return dot(vec3(0.2126, 0.7152,0.0722), color);
}

void main(){
  vec3 tex_color = texture(color_attachment, getUV()).rgb;
  //.3
  if(luminance1(tex_color) > 0.4){
    //out_color = vec4(.0, 0, 10, 1);
    out_color = vec4(tex_color, 1);
  }
  else{
    out_color = vec4(0);
  }
}
