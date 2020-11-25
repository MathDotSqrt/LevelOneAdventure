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

uniform sampler2D diffuse;

vec3 point_color(PointLight light){
	float dist = length(light.pos - f_world_pos);
	vec3 light_dir = normalize(light.pos - f_world_pos);
	float d = dot(light_dir, normalize(f_world_normal));
	d = max(d * light.intensity, 0);

	float attenuation = 1 / pow(dist, 2);
	vec3 light_color = light.color * d * attenuation;
	return light_color;
}

vec3 calc_light(){
	vec3 diffuse_light_color = vec3(0);

	for(int i = 0; i < u_num_point_lights; i++){
		diffuse_light_color += point_color(u_point_lights[i]);
	}

	diffuse_light_color = min(diffuse_light_color, vec3(1));
	return diffuse_light_color;
}

void main(){

	vec3 diffuse_texture = pow(texture(diffuse, f_uv).rgb, vec3(2.2));
	vec3 f_color = diffuse_texture * calc_light();
	vec3 gamma = pow(f_color, vec3(1/2.2));
	out_color = vec4(gamma, 1);
}
