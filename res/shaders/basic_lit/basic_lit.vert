#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;

out vec3 f_world_pos;
out vec3 f_world_normal;

uniform mat4 M;
uniform mat4 P;

void main(){
	vec4 world_pos = M * vec4(v_pos, 1);
	gl_Position = P * world_pos;

	f_world_pos = world_pos.xyz;
	f_world_normal = mat3(M) * v_normal; 
}