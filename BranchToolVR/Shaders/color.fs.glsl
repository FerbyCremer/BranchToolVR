#version 410 core

in vec3 oi_position;
in vec3 oi_normal;

uniform vec4 color;

out vec4 output_color;

struct Light{
  vec3 pos;
  float pow;
};

uniform Light lights[3];
 
void main(){
	float light_res  = 0.5f*clamp(abs(dot(oi_normal, normalize(lights[0].pos - oi_position)))/length(lights[0].pos - oi_position), 0.0f, 1.0f);
	      light_res += 0.5f*clamp(abs(dot(oi_normal, normalize(lights[1].pos - oi_position)))/length(lights[1].pos - oi_position), 0.0f, 1.0f);
	      light_res += 0.5f*clamp(abs(dot(oi_normal, normalize(lights[2].pos - oi_position)))/length(lights[2].pos - oi_position), 0.0f, 1.0f);
	output_color = vec4(light_res*color.xyz, color.w);
}
    