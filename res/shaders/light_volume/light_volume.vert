#version 330
#include "preamble.glsl"
#include "light.glsl"

layout(location=POSITION_ATTRIB_LOCATION) in vec3 v_pos;
layout(location=NORMAL_ATTRIB_LOCATION) in vec3 v_normal;

uniform mat4 P;

uniform PointLight u_view_light;

//out vec2 f_uv;

void main(){
	mat4 MV = mat4(1.0);
	MV[3] = vec4(u_view_light.pos, 1);
	MV[0][0] = u_view_light.radius;
	MV[1][1] = u_view_light.radius;
	MV[2][2] = u_view_light.radius;
	//M[3][3] = u_radius;

	gl_Position = P * MV * vec4(v_pos, 1);

	// vec3 ndc = gl_Position.xyz / gl_Position.w;
	// f_uv = ndc.xy * .5 + .5;
	// f_uv.y = 1 - f_uv.y;
}
