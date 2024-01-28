#version 460 core

out vec4 color;

in vec2 v_position;

uniform int u_text[128];

uniform sampler2D u_sampler;

void main(){
	int index = int(floor(v_position.x*128));
	int char = u_text[index];
	vec2 texel = vec2(float(char%16)/16.0f, float(15-char/16)/16.0f);
	texel += vec2(mod(v_position.x*128.0f, 1.0f)/16.0f, (1.0f-v_position.y)/16.0f);
	color = texture(u_sampler, texel);
}

