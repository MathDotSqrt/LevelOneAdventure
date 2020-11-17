#version 330

#include "preamble.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=COLOR_ATTRIB_LOCATION) in vec3 v_color;

out vec3 f_pos;
out vec3 f_color;

uniform mat4 MP;

void main(){
	gl_Position = MP * vec4(v_pos, 1);

	f_pos = v_pos;
	f_color = v_color; 
}