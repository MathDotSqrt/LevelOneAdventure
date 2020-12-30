#include "postprocess/pp_base.frag"
#include "math.glsl"

out float out_color;

uniform sampler2D position_attachment;
uniform sampler2D normal_attachment;
uniform sampler2D ssao_noise;

uniform vec3 samples[SSAO_KERNEL_SIZE];
uniform mat4 P;

uniform vec2 view_port;

const float scale = 4;
const float bias = .04;
void main(){
  vec2 uv = getUV(color_attachment_size);
  vec3 view_pos = texture(position_attachment, uv).xyz;
  vec3 view_normal = texture(normal_attachment, uv).xyz;
  vec3 ssao_noise = texture(ssao_noise, f_uv * view_port / scale).xyz;

  vec3 tangent = normalize(ssao_noise - view_normal * dot(ssao_noise, view_normal));
  vec3 bitangent = cross(view_normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, view_normal);

  //https://learnopengl.com/Advanced-Lighting/SSAO
  float occlusion = 0;
  for(int i = 0; i < SSAO_KERNEL_SIZE; i++){
    vec3 sample_pos = TBN * samples[i];  //tangent to view space
    sample_pos = view_pos + sample_pos * SSAO_RADIUS;

    vec4 offset = vec4(sample_pos, 1);
    offset = P * offset;                //to clip space
    offset.xyz /= offset.w;             //dark magic perspective divide
    offset.xyz = offset.xyz * .5 + .5;  // transform range to [0-1]
    offset.y = 1 - offset.y;            //TODO: fix uv coordinates for quad
    float sample_depth = texture(position_attachment, getUV(color_attachment_size, offset.xy)).z;

    float t = SSAO_RADIUS / abs(view_pos.z - sample_depth);
    float range_check = smoothstep(0.0, 1.0, t);
    occlusion += (sample_depth >= sample_pos.z + bias ? 1 : 0) * range_check;
  }

  occlusion = 1.0 - (occlusion / SSAO_KERNEL_SIZE);
  out_color = pow(occlusion, SSAO_POWER);
}
