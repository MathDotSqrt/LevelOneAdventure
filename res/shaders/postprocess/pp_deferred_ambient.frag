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

vec3 to_linear(vec3 color){
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

vec3 compute_ambient(vec3 color, AmbientLight light){
  return color * to_linear(light.color) * light.intensity;
}

vec3 compute_dir(vec3 color, vec3 normal, DirLight light){
  normal = normalize(normal);
  float d = max(dot(normal, -light.dir), 0);

  return color * to_linear(light.color) * light.intensity * d;
}

void main(){
  vec3 color = texture(color_attachment, getUV(color_attachment_size)).rgb;
  vec3 normal = texture(normal_attachment, getUV(color_attachment_size)).rgb;

  vec3 light_color = vec3(0);

  light_color += compute_ambient(color, u_ambient_light);
  light_color += compute_dir(color, normal, u_dir_light);
  out_color = vec4(light_color, 1);
}
