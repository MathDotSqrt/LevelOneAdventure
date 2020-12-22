#version 330

layout (location = 0) out vec3 g_world_pos;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec3 f_world_pos;
in vec3 f_normal;
in vec2 f_texcoord;

uniform sampler2D diffuse;

vec3 to_linear(vec3 color){
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

void main(){

  vec3 tex_color = to_linear(texture(diffuse, f_texcoord).rgb);

  g_world_pos = f_world_pos;
  g_normal = normalize(f_normal);
  g_albedo = vec4(tex_color, 1);
}
