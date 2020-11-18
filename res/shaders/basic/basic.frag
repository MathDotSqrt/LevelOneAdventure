#version 330

in vec3 f_pos;
in vec3 f_color;

out vec4 out_color;

uniform float u_time;

void main(){
	out_color = vec4(f_color, 1);
}