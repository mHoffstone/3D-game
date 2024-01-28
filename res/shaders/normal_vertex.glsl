#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_texCoords;
layout(location = 3) in float i_texIndex;

out VS_OUT{
	vec3 position;
	vec3 normal;
} vs_out;

uniform mat4 u_projection;
uniform vec3 u_translation;
uniform mat4 u_rotation;

void main(){
	vs_out.position = i_position;
	vs_out.normal = i_normal;

	gl_Position = u_projection * u_rotation * vec4(i_position - u_translation, 1.0);
}


