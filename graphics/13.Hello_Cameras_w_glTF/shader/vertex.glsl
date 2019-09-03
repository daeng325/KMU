#version 120                  // GLSL 1.20

uniform mat4 u_PVM;           // Proj * View * Model

attribute vec3 a_position;    // per-vertex position (per-vertex input)

void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);
}