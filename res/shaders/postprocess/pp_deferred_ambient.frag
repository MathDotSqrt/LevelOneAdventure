#include "postprocess/pp_base.frag"
#include "math.glsl"

out vec4 out_color;

//Assume all of the attachments are the same size
//uniform sampler2D position_attachment;
uniform sampler2D normal_attachment;
uniform sampler2D color_attachment;


void main(){
  vec3 tex_color = texture(color_attachment, getUV(color_attachment_size)).rgb;
  out_color = vec4(tex_color, 1);
}
