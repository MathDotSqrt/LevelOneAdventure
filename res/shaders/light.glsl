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
