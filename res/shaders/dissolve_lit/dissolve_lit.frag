#version 330
#include "preamble.glsl"
#include "color_space.glsl"

layout (location = 0) out vec3 g_view_pos;
layout (location = 1) out vec3 g_view_normal;
layout (location = 2) out vec4 g_albedo;

in vec3 f_world_pos;
in vec3 f_view_pos;
in vec3 f_view_normal;
in vec2 f_uv;

uniform vec3 u_color = vec3(1, 1, 1);
uniform float u_dissolve = .1;
uniform float u_offset = .1;
uniform vec3 u_dissolve_color = vec3(1, 1, 1);

uniform sampler3D noise;
uniform sampler2D diffuse;

float sampleNoise(vec3 pos){
	float value = texture(noise, pos * .01).r * 2/4;
	value += texture(noise, pos * .05 + vec3(13,223,-11)).r * 1/4;
	value += texture(noise, pos.zyx * .3 + vec3(10,223,-1)).r * 1/4;

	return value;
}

void main(){
	float noise = sampleNoise(f_world_pos);

	if(u_dissolve >= noise){
		discard;
	}

	vec3 diffuse_texture = to_linear(texture(diffuse, f_uv).rgb);
	vec3 f_color = diffuse_texture * 1;

	vec3 out_color;
	if(noise - u_dissolve < u_offset){
		float mix_factor = (noise - u_dissolve) / u_offset;
		vec3 mix_color = mix(u_dissolve_color, f_color, pow(mix_factor, 40));
		out_color = to_linear(u_dissolve_color);
	}
	else{
		out_color = f_color;
	}

	out_color *= to_linear(u_color);

	g_view_pos = f_view_pos;
  g_view_normal = normalize(f_view_normal);
  g_albedo = vec4(out_color, 1);

}
