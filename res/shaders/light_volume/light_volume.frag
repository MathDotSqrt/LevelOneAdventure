#version 330
#include "fbo_attachment.glsl"

in vec2 f_uv;
in vec3 f_world_pos;


uniform fbo_attachment_size color_attachment_size;
uniform sampler2D normal_attachment;

out vec4 out_color;

void main(){
	vec3 tex_color = texture(normal_attachment,getUV(color_attachment_size, f_uv)).rgb;
	out_color = vec4(tex_color, 1);
}
