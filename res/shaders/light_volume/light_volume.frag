#version 330

in vec3 f_world_pos;
out vec4 out_color;

void main(){
	out_color = vec4(f_world_pos, 1);
}
