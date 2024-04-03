#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;

float h(vec2 uv) {
  // You may want to use this helper function...
  return texture(u_texture_2, uv).r;
}

void main() {
  // YOUR CODE HERE
  
  vec3 n = vec3(v_normal[0], v_normal[1], v_normal[2]);
  vec3 t = vec3(v_tangent[0], v_tangent[1], v_tangent[2]);
  vec3 b = cross(n, t);
  mat3 tbn = mat3(t, b, n);

  float u = v_uv[0];
  float v = v_uv[1];
  float width = u_texture_2_size[0];
  float height = u_texture_2_size[1];

  vec2 u_input = vec2(u + (1.0 / width), v);
  vec2 v_input = vec2(u , v + (1.0 / height));

  float dU =   (h(u_input) - h(v_uv) ) * u_height_scaling * u_normal_scaling;
  float dV = ( h(v_input) - h(v_uv) ) * u_height_scaling * u_normal_scaling;

  vec3 no = vec3(-1.0 * dU, -1.0 * -dV, 1.0);
  vec3 nd = normalize(tbn * no);


  vec3 v_pos = vec3(v_position[0], v_position[1], v_position[2]);
  vec3 l = (u_light_pos - v_pos);
  float r_inv =  1 / length(l);
  vec3 Ia = vec3(1.0, 1.0, 1.0);
  vec3 IoverR2 = u_light_intensity * r_inv * r_inv;
  float ndotl = dot(nd, normalize(l));
  vec3 he = normalize(v_pos + l);
  float hdot = dot(nd, he);

  vec3 color_out =  (0.3 * Ia) + (.2 * IoverR2 * max(0, ndotl)) + (2.0 * IoverR2 * max(0, pow(hdot, 16)));
  // (Placeholder code. You will want to replace it.)
  out_color = vec4(color_out, 1);
}

