#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec2 v_texcoord;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat3 inverse_transpose;

out vec3 f_view_pos;
out vec3 f_normal;
out vec2 f_texcoord;

void main(){
	vec4 view_pos = V * M * vec4(v_pos, 1);
	gl_Position = P * view_pos;

	f_view_pos = view_pos.xyz;
	f_normal = mat3(V) * inverse_transpose * v_normal;
	f_texcoord = v_texcoord; 
}
