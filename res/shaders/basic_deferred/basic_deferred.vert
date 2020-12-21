#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=TEXCOORD_ATTRIB_LOCATION) in vec3 v_color;

uniform mat4 VP;

out vec3 f_color;

void main(){
	gl_Position = VP * vec4(v_pos, 1);
	f_color = v_color;
}
