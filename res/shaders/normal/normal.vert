#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;

out vec3 f_world_normal;

uniform mat4 M;
uniform mat4 P;

uniform mat3 inverse_transpose;

void main(){
	vec4 world_pos = M * vec4(v_pos, 1);
	gl_Position = P * world_pos;

	f_world_normal = inverse_transpose * v_normal; 
}