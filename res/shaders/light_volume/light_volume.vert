#version 330
#include "preamble.glsl"
#include "light.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;

uniform mat4 VP;

uniform PointLight u_light;

//out vec2 f_uv;

void main(){
	mat4 M = mat4(1.0);
	M[3] = vec4(u_light.pos, 1);
	M[0][0] = u_light.radius;
	M[1][1] = u_light.radius;
	M[2][2] = u_light.radius;
	//M[3][3] = u_radius;

	gl_Position = VP * M * vec4(v_pos, 1);

	// vec3 ndc = gl_Position.xyz / gl_Position.w;
	// f_uv = ndc.xy * .5 + .5;
	// f_uv.y = 1 - f_uv.y;
}
