mat4 translate(mat4 m, vec3 pos){
  m[3][0] += pos.x;
  m[3][1] += pos.y;
  m[3][2] += pos.z;
  return m;
}

mat4 billboard(mat4 m, vec3 scale=vec3(1)){
  // Column 0:
  m[0][0] = scale.x;
  m[0][1] = 0;
  m[0][2] = 0;

  // Column 1:
  m[1][0] = 0;
  m[1][1] = scale.y;
  m[1][2] = 0;

  // Column 2:
  m[2][0] = 0;
  m[2][1] = 0;
  m[2][2] = scale.z;

  return m;
}

mat4 rotateZ(float angle){
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(
      c, -s, 0, 0,
      s, c, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1
    );
}

mat4 rotate(float angle, vec3 axis){
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(
      oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
      oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0,
      oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0,
      0.0, 0.0, 0.0, 1.0
    );
}

float max3(vec3 v){
  return max(max(v.x, v.y), v.z);
}
