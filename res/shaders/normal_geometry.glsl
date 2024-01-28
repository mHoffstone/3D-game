#version 460 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT{
	vec3 position;
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 u_projection;
uniform vec3 u_translation;
uniform mat4 u_rotation;

void GenerateLine(int index){
	gl_Position = u_projection * u_rotation * vec4(gs_in[index].position - u_translation, 1.0f);
	EmitVertex();

	gl_Position = u_projection * u_rotation * vec4(gs_in[index].position - u_translation + gs_in[index].normal * MAGNITUDE, 1.0f);

	EmitVertex();
	EndPrimitive();
}

void main(){
	GenerateLine(0); // first vertex normal
	GenerateLine(1); // second vertex normal
	GenerateLine(2); // third vertex normal
}
