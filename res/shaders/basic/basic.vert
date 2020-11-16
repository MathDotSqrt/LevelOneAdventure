#version 330
in vec3 v_pos;

out vec3 f_color;

uniform mat4 M;
uniform float u_time;

void main(){
	gl_Position = M * vec4(v_pos, 1);

	f_color = v_pos + vec3(sin(u_time));
}