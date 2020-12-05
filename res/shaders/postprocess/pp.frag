#include "postprocess/pp_base.frag"

uniform sampler2D blur;

void main(){
  out_color = texture(color_attachment, getUV());
}
