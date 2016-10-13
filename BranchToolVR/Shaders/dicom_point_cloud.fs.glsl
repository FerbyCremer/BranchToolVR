#version 410 core

in float oi_state;
in float oi_lighting;
in float oi_lighting2;
in vec3 oi_pos;
in vec3 oi_normal;
in vec3 oi_instanced_position;

out vec4 outputColor;
uniform vec3 ambient;

void main(){
	outputColor = vec4(oi_lighting*oi_instanced_position + ambient, 1.0f);
}