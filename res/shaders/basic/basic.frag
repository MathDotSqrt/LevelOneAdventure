#version 330
#include "color_space.glsl"
in vec2 f_texcoord;

out vec4 out_color;

uniform vec3 u_color;
uniform sampler2D diffuse;
uniform float u_alpha = .5;

void main(){
	vec4 tex_value = texture(diffuse, f_texcoord);

	vec3 tex_color = to_linear(tex_value.rgb);
	float tex_alpha = tex_value.a;

	out_color = vec4(tex_color * to_linear(u_color), u_alpha * tex_alpha);
}
