#version 120                  // GLSL 1.20

uniform mat4 u_PVM;           // Proj * View * Model

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec3 a_color;       // per-vertex color (per-vertex input)

varying   vec3 v_color;       // per-vertex color (per-vertex output)

void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);
  v_color = a_color;
}