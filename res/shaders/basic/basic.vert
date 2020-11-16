#version 330
in vec3 v_pos;

out vec3 f_color;

void main(){
	gl_Position = vec4(v_pos, 1);

	f_color = abs(v_pos);
}