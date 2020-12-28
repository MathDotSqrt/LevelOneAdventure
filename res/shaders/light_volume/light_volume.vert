#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;

uniform mat4 VP;

uniform vec3 u_pos;
uniform float u_radius = 1;

out vec2 f_uv;
out vec3 f_world_pos;

void main(){
	mat4 M = mat4(1.0);
	M[3] = vec4(u_pos, 1);
	M[0][0] = u_radius;
	M[1][1] = u_radius;
	M[2][2] = u_radius;
	//M[3][3] = u_radius;

	vec4 world_pos =  M * vec4(v_pos, 1);
	gl_Position = VP * world_pos;

	f_world_pos = world_pos.xyz;

	vec3 ndc = gl_Position.xyz / gl_Position.w;
	f_uv = ndc.xy * .5 + .5;
	f_uv.y = 1 - f_uv.y;
}
