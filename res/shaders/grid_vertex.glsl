#version 460 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_texCoords;
layout(location = 3) in float i_texIndex;

layout(std430, binding = 0) buffer heightSSBO{
    float data[];
};

out vec3 v_position;
out vec3 v_normal;
out vec2 v_texCoords;
out float v_texIndex;

uniform mat4 u_projection;
uniform vec3 u_translation;
uniform mat4 u_rotation;

uniform int u_width;

// Position of the grid
uniform vec2 u_position;

void main(){
	v_texIndex = i_texIndex;
	v_texCoords = i_texCoords;

	float x = float(gl_InstanceID % u_width) + i_position.x;
	//float x = float(gl_InstanceID - (gl_InstanceID/u_width) * u_width) + i_position.x;
	float z = float(gl_InstanceID / u_width) + i_position.z;
	int k = (int(z)*(u_width+1)+int(x))*4;
	vec3 pos = vec3(x, data[k], z);
	v_position = pos;
	v_normal = vec3(data[k+1], data[k+2], data[k+3]);
	gl_Position = u_projection * u_rotation * vec4(pos - u_translation + vec3(u_position.x, 0.0f, u_position.y), 1.0);
}

