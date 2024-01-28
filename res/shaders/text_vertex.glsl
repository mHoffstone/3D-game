#version 460 core

layout(location = 0) in vec2 i_position;

out vec2 v_position;

uniform vec2 u_translation;
uniform vec2 u_vpSize;

void main(){
	v_position = i_position;

	vec2 scale = vec2(u_vpSize.y/u_vpSize.x, -1.0f);
	float fontSize = 32.0f/u_vpSize.y;
	gl_Position = vec4((i_position * vec2(128.0f, 1.0f) + u_translation) * fontSize * scale + vec2(-1.0f, 1.0f), 0.0f, 1.0f);
}



