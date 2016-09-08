#version 410 core

in float oi_state;
in float oi_lighting;
in float oi_lighting2;
in vec3 oi_pos;
in vec3 oi_normal;

out vec4 outputColor;

void main(){
	outputColor = vec4(oi_lighting*vec3(0.2f, 1.0f, 0.2f), 1.0f);
}