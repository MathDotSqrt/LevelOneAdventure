#version 330
#include "preamble.glsl"


in vec3 f_world_pos;
in vec3 f_world_normal;
in vec2 f_uv;


out vec4 out_color;

struct PointLight {
	vec3 pos;
	vec3 color;
	float intensity;
};

//uniform vec3 u_camera_pos = vec3(0);
uniform int u_num_point_lights = 0;
uniform PointLight u_point_lights[MAX_POINT_LIGHTS];

uniform float u_dissolve = .1;
uniform float u_offset = .1;
uniform vec3 u_dissolve_color = vec3(1, 1, 1);

uniform sampler3D noise;
uniform sampler2D diffuse;

vec3 to_linear(vec3 color){
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

vec3 point_color(PointLight light){
	float dist = length(light.pos - f_world_pos);
	vec3 light_dir = normalize(light.pos - f_world_pos);
	float d = dot(light_dir, normalize(f_world_normal));
	d = max(d * light.intensity, 0);

	float attenuation = 1 / pow(dist, 2);
	vec3 light_color = to_linear(light.color) * d * attenuation;
	return light_color;
}

vec3 ambient_color(){
	return to_linear(vec3(1)) * .05;
}

vec3 calc_light(){
	vec3 diffuse_light_color = vec3(0);

	for(int i = 0; i < u_num_point_lights; i++){
		diffuse_light_color += point_color(u_point_lights[i]);
	}

	diffuse_light_color += ambient_color();
	return diffuse_light_color;
}


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
	vec3 f_color = diffuse_texture * calc_light() * 1;

	if(noise - u_dissolve < u_offset){
		float mix_factor = (noise - u_dissolve) / u_offset;
		vec3 mix_color = mix(u_dissolve_color, f_color, pow(mix_factor, 40));
		out_color = vec4(to_linear(u_dissolve_color), 1);
	}
	else{
		out_color = vec4(f_color, 1);
	}

}
