#version 330


uniform vec3 u_cam_pos;

uniform samplerCube u_texture_cubemap;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;

out vec4 out_color;

void main() {
  // YOUR CODE HERE
  vec4 u_cam = vec4(u_cam_pos, 1);
  vec4 wo = v_position - u_cam;
  vec4 wi = wo - 2 * (dot(wo, v_normal) / dot(v_normal, v_normal)) * v_normal;

  vec3 dir = vec3(wi[0], wi[1], wi[2]);

  out_color = texture(u_texture_cubemap, dir);
}
