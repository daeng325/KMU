#version 120                  // GLSL 1.20

uniform sampler2D u_texture;

varying vec2 v_texcoord;

void main()
{
    gl_FragColor = texture2D(u_texture, v_texcoord); 
    //주어진 texture에서 주어진 coordinate의 위치의 컬러를 받아와서 gl_FragColor로 설정.
 