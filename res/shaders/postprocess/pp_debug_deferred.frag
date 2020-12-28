#include "postprocess/pp_base.frag"
#include "math.glsl"

struct DirLight {
	vec3 color;
  vec3 dir;
	float intensity;
};

struct AmbientLight {
  vec3 color;
  float intensity;
}

out vec4 out_color;

uniform sampler2D color_attachment0;
uniform sampler2D color_attachment1;
uniform sampler2D color_attachment2;

uniform DirLight u_dir_light;
uniform AmbientLight u_ambient_light;


void main(){
  vec3 tex_color = texture(color_attachment2, getUV(color_attachment_size)).rgb;
  out_color = vec4(tex_color, 1);
}
