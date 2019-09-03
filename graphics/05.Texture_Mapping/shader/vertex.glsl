#version 120                  // GLSL 1.20

uniform mat4 u_PVM;           // Proj * View * Model

attribute vec3 a_position;    // per-vertex position (per-vertex input)
attribute vec2 a_texcoord;    

varying   vec2 v_texcoord;    

void main()
{
  gl_Position = u_PVM * vec4(a_position, 1.0f);
  v_texcoord = a_texcoord;
}