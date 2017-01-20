#version 410

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 instanced_position;
layout(location = 3) in float state;
layout(location = 4) in float iso_diff;

struct Light
{
  vec3 pos;
  float power;
};

uniform Light lights[3];

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform int tolerance;
uniform vec3 scale;

uniform vec3 lower_bounds;
uniform vec3 upper_bounds;
uniform vec3 eye_pos;

out float oi_state;
out vec3 oi_pos;
out vec3 oi_normal;
out float oi_lighting;
out float oi_lighting2;
out vec3 oi_instanced_position;

void main()
{
		oi_state = state;
		oi_pos = vec3(M*vec4(position + instanced_position, 1.0f));
		oi_normal = normalize(vec3(M*vec4(normal, 0.0f)));
		oi_instanced_position = normalize(instanced_position)*1.5f;

		vec3 to_light1 = lights[0].pos - oi_pos;
		vec3 to_light2 = lights[1].pos - oi_pos;
		vec3 to_light3 = lights[2].pos - oi_pos;

		float light_dist1 = length(to_light1);
		float light_dist2 = length(to_light2);
		float light_dist3 = length(to_light3);

		oi_state = iso_diff;

		oi_lighting  =  0.5f*clamp(dot(oi_normal, normalize(to_light1)),0.0f, 1.0f)/(light_dist1*light_dist1);
		oi_lighting +=  0.5f*clamp(dot(oi_normal, normalize(to_light2)),0.0f, 1.0f)/(light_dist2*light_dist2);
		oi_lighting +=  0.5f*clamp(dot(oi_normal, normalize(to_light3)),0.0f, 1.0f)/(light_dist3*light_dist3);

		gl_Position = P * V * vec4(oi_pos,1.0f) + max(sign(iso_diff - tolerance), 0.0)*vec4(100000.0f,100000.0f,100000.0f,100000.0f);
}
