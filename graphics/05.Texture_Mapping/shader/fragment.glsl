#version 120                  // GLSL 1.20

uniform sampler2D u_texture_cats;
uniform sampler2D u_texture_frame;

varying vec2 v_texcoord;

void main() 
{
  vec4 color_cats  = texture2D(u_texture_cats,  v_texcoord);
  vec4 color_frame = texture2D(u_texture_frame, v_texcoord);

  gl_FragColor = color_cats*0.5 + color_frame*0.5;
}