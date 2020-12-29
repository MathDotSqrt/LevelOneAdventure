#ifndef COLOR_SPACE_GLSL
#define COLOR_SPACE_GLSL

vec3 to_linear(vec3 color){
	const float gamma = 2.2;
	return pow(color, vec3(gamma));
}

vec3 to_gamma(vec3 color){
  float gamma = 2.2;
  return pow(color.rgb, vec3(1.0/gamma));
}

#endif
