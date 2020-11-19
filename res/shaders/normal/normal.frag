#version 330

in vec3 f_world_normal;

out vec4 out_color;

void main(){
	out_color = vec4(abs(normalize(f_world_normal)), 1);
}