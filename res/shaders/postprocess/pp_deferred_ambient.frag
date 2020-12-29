#include "postprocess/pp_base.frag"
#include "math.glsl"
#include "light.glsl"

out vec4 out_color;

//Assume all of the attachments are the same size
//uniform sampler2D position_attachment;
uniform sampler2D normal_attachment;
uniform sampler2D color_attachment;

uniform DirLight u_dir_light;
uniform AmbientLight u_ambient_light;

void main(){
  vec3 color = texture(color_attachment, getUV(color_attachment_size)).rgb;
  vec3 normal = texture(normal_attachment, getUV(color_attachment_size)).rgb;

  vec3 light_color = vec3(0);

  light_color += compute_ambient(u_ambient_light) * color;
  light_color += compute_dir(normal, u_dir_light) * color;
  out_color = vec4(light_color, 1);
}
