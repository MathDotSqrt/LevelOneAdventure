#version 330

in vec3 f_world_pos;
in vec3 f_world_normal;
in vec2 f_uv;

out vec4 out_color;

//uniform vec3 u_camera_pos = vec3(0);
uniform vec3 u_light_pos = vec3(0);

uniform sampler2D diffuse;

vec3 dir_light(vec3 light_pos, vec3 color, float intensity){
	float light_len = length(light_pos - f_world_pos);
	float attenuation = 1 / (light_len * light_len);

	vec3 light_dir = normalize(light_pos - f_world_pos);
	float d = dot(light_dir, normalize(f_world_normal));
	float light_intensity = min(max(d * intensity * attenuation, 0), 1);

	return color * light_intensity;
}

vec3 ambient_light(float intensity, vec3 color){
	return color * intensity;
}

void main(){
	vec3 diffuse_color = pow(texture(diffuse, f_uv).rgb, vec3(2.2));

	vec3 dir_light_color = dir_light(u_light_pos, vec3(1, .5, .4), .6);
	vec3 ambient_light_color = ambient_light(.05, vec3(.2, .5, .7));

	vec3 light_color = dir_light_color + ambient_light_color;

	light_color = min(light_color, 1);

	out_color = vec4(diffuse_color * light_color, 1);
	out_color = pow(out_color, vec4(1/2.2));
}
