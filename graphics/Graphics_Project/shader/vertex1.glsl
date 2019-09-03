#version 120// GLSL 1.20
uniform mat4 u_PVM;
attribute vec3 a_position;
uniform mat4 u_M;
attribute vec2 a_texcoord;
varying vec3 v_normal_wc;
varying vec3 v_position_wc;
attribute vec3 a_normal;
void main(){
	gl_Position=u_PVM*vec4(a_position,1.0f);
	v_position_wc = (u_M * vec4(a_position, 1)).xyz;
	v_normal_wc=normalize((u_M * vec4(a_normal, 0)).xyz);
}