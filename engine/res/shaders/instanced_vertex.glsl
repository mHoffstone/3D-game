#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_texCoords;
layout(location = 3) in float i_texIndex;
layout(location = 4) in vec3 i_modelPosition;
layout(location = 5) in mat4x4 i_modelMatrix;

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texCoords;
out float v_texIndex;

uniform mat4 u_projection;
uniform vec3 u_translation;
uniform mat4 u_rotation;

void main(){
	v_position = i_position;
	v_normal = normalize((i_modelMatrix * vec4(i_normal, 0.0)).xyz);
	v_texIndex = i_texIndex;
	v_texCoords = i_texCoords;

	gl_Position = u_projection * u_rotation * (i_modelMatrix*vec4(i_position, 1.0f) - vec4(u_translation - i_modelPosition, 0.0f));
// 	gl_Position = u_projection * u_rotation * i_modelMatrix * vec4(i_position - u_translation, 1.0);
// 	gl_Position = u_projection * u_rotation * vec4(i_position - u_translation + vec3(0.0f, gl_InstanceID, 0.0f), 1.0);
}

