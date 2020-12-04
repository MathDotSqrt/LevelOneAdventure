#version 330

in vec2 f_uv;
in vec4 f_color;
flat in float f_tex_index;

out vec4 out_color;

uniform sampler2D diffuse;

vec2 sample_grid(int index){
  int x = index % 2;
  int y = index / 2;

  vec2 grid = f_uv  * vec2(.5);

  if(index==0){
    return grid;
  }
  else if(index == 1){
    return grid + vec2(.5, 0);
  }
  else if(index == 2){
    return grid + vec2(0, .5);
  }
  return grid + vec2(.5, .5);
}

void main(){
  vec2 uv = sample_grid(int(f_tex_index));
  float alpha = texture(diffuse, uv).a;

  out_color = vec4(.1, .1, .1, alpha);
}
