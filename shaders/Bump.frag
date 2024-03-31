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
  return 0.0;
}

void main() {
  // YOUR CODE HERE
  vec4 l = (vec4(u_light_pos, 1) - v_position);
  float r_inv =  1 / length(l);
  vec3 Ia = vec3(1.0, 1.0, 1.0);
  vec3 IoverR2 = u_light_intensity * r_inv * r_inv;
  float ndotl = dot(v_normal, normalize(l));
  vec4 h = normalize(v_position + l);
  float hdot = dot(v_normal, h);

  vec3 color_out =  (0.3 * Ia) + (.2 * IoverR2 * max(0, ndotl)) + (2.0 * IoverR2 * max(0, pow(hdot, 32)));
  // (Placeholder code. You will want to replace it.)
  out_color = vec4(color_out, 1);
}

