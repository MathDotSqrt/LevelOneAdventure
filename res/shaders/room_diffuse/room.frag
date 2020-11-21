#version 330

in vec3 f_world_pos;
in vec3 f_world_normal;
in vec2 f_uv;

out vec4 out_color;

//uniform vec3 u_camera_pos = vec3(0);
uniform vec3 u_light_pos = vec3(0);

uniform sampler2D diffuse;

void main(){
	vec4 diffuse_color = texture(diffuse, f_uv);

	float light_len = length(u_light_pos - f_world_pos);
	vec3 light_dir = normalize(u_light_pos - f_world_pos);
	float d = dot(light_dir, normalize(f_world_normal));
	float light_intensity = min(max(d, 0), 1);

	vec3 light_color = diffuse_color.rgb * vec3(1, .6, .3) * light_intensity;

	out_color = vec4(light_color, 1);
}
