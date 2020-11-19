#version 330

in vec3 f_world_pos;
in vec3 f_world_normal;

out vec4 out_color;

//uniform vec3 u_camera_pos = vec3(0);
uniform vec3 u_light_pos = vec3(0);

void main(){
	
	vec3 light_dir = normalize(u_light_pos - f_world_pos);
	float d = dot(light_dir, normalize(f_world_normal));

	d = max(d, 0);
	vec3 light_color = vec3(1, 1, 1) * d;

	out_color = vec4(light_color, 1);
}