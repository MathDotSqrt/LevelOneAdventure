#ifndef LIGHT_GLSL
#define LIGHT_GLSL

#include "color_space.glsl"

struct PointLight {
  vec3 pos;
  vec3 color;
  float intensity;
  float radius;
};

struct DirLight {
  vec3 dir;
	vec3 color;
	float intensity;
};

struct AmbientLight {
  vec3 color;
  float intensity;
};

vec3 compute_ambient(AmbientLight light){
  return to_linear(light.color) * light.intensity;
}

vec3 compute_dir(vec3 normal, DirLight light){
  normal = normalize(normal);
  float d = max(dot(normal, -light.dir), 0);

  return to_linear(light.color) * light.intensity * d;
}

vec3 compute_point_light(vec3 pos, vec3 normal, PointLight light){
	normal = normalize(normal);

	float dist = length(light.pos - pos);
	vec3 light_dir = normalize(light.pos - pos);
	float d = max(dot(light_dir, normal), 0);

	float falloff = (dist) / (light.radius);
	float attenuation = light.intensity * clamp(1 - falloff, 0, 1);
	attenuation *= attenuation;

	vec3 light_color = to_linear(light.color) * d * attenuation;
	return light_color;
}

#endif
