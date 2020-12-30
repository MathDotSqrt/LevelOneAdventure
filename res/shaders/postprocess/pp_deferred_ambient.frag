#include "postprocess/pp_base.frag"
#include "math.glsl"
#include "light.glsl"

out vec4 out_color;

//Assume all of the attachments are the same size
//uniform sampler2D position_attachment;
uniform sampler2D normal_attachment;
uniform sampler2D color_attachment;
uniform sampler2D ssao_attachment;

uniform DirLight u_view_dir_light;
uniform AmbientLight u_ambient_light;

float sample_ssao(){
  vec2 texel_size = 1 / color_attachment_size.window_size;

  float result = 0;
  for(int x = -2; x < 2; x++){
    for(int y = -2; y < 2; y++){
      vec2 offset = vec2(float(x), float(y)) * texel_size;
      result += texture(ssao_attachment, getUV(color_attachment_size, f_uv + offset)).r;
    }
  }
  return result / (4 * 4);
}

void main(){
  vec3 color = texture(color_attachment, getUV(color_attachment_size)).rgb;
  vec3 view_normal = texture(normal_attachment, getUV(color_attachment_size)).rgb;
	//float ao = texture(ssao_attachment, getUV(color_attachment_size)).r;
  float ao = sample_ssao();

  vec3 light_color = vec3(0);
  light_color += compute_ambient(u_ambient_light) * color * 2;
  light_color += compute_dir(view_normal, u_view_dir_light) * color * .5;

  out_color = vec4(light_color * ao, 1);
  //out_color = vec4(vec3(ao), 1);
}
