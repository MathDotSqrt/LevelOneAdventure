#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_uv;

out vec3 f_world_pos;
out vec3 f_world_normal;
out vec2 f_uv;

uniform mat4 M;
uniform mat4 VP;

uniform mat3 inverse_transpose;

void main(){
	vec4 world_pos = M * vec4(v_pos, 1);
	gl_Position = VP * world_pos;

	f_world_pos = world_pos.xyz;
	f_world_normal = inverse_transpose * v_normal;
	f_uv = v_uv;
}
