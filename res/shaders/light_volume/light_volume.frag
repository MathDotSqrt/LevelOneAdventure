#version 330
#include "fbo_attachment.glsl"
#include "light.glsl"


uniform fbo_attachment_size color_attachment_size;

uniform sampler2D position_attachment;
uniform sampler2D normal_attachment;
uniform sampler2D color_attachment;

uniform vec2 inv_viewport_size;

uniform PointLight u_light;

out vec4 out_color;

void main(){
	vec2 frag_pos = gl_FragCoord.xy * inv_viewport_size;
	frag_pos.y = 1 - frag_pos.y;

	vec2 uv = getUV(color_attachment_size, frag_pos);

	vec3 world_pos = texture(position_attachment, uv).rgb;
	vec3 world_normal = texture(normal_attachment, uv).rgb;
	vec4 color_specular = texture(color_attachment, uv);

	vec3 color = color_specular.rgb;
	float specular = color_specular.a;

	vec3 final_color = vec3(0);
	final_color += compute_point_light(world_pos, world_normal, u_light) * color;

	out_color = vec4(final_color, 1);
}
