#version 330

in vec2 f_texcoord;

out vec4 out_color;

uniform sampler2D diffuse;
uniform float alpha = .5;

vec3 to_linear(vec3 color){
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

void main(){
	vec3 tex_color = to_linear(texture(diffuse, f_texcoord).rgb);
	out_color = vec4(tex_color, alpha);
}
