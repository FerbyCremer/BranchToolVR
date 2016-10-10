#version 410 core

in vec3 oi_position;
in vec3 oi_normal;
in vec2 oi_uv;

out vec4 output_color;

uniform vec4 color;

void main(){
	output_color = color;
}
