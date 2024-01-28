#include "instancedrenderablemesh.h"

#include "resource.h"

#include <iostream>

#include <GL/glew.h>

Program InstancedRenderableMesh::program;
int InstancedRenderableMesh::renderID = -1;

void InstancedRenderableMesh::init(Renderer& r){
	program.createProgram();
	std::string vertexSource, fragmentSource;
	LOAD_RESOURCE_STRING(vertexSource, instanced_vertex_glsl);
	LOAD_RESOURCE_STRING(fragmentSource, fragment_glsl);

	// 		Shader vertexShader(GL_VERTEX_SHADER, vertexSource);
	// 		Shader fragmentShader(GL_FRAGMENT_SHADER, fragmentSource);
	Shader vertexShader(GL_VERTEX_SHADER, util::readFile("engine/res/shaders/instanced_vertex.glsl"));
	Shader fragmentShader(GL_FRAGMENT_SHADER, util::readFile("engine/res/shaders/fragment.glsl"));
	program.setShaders({&vertexShader, &fragmentShader});
	renderID = r.addProgram(program);
}

void InstancedRenderableMesh::create(){
	RenderableMesh::create();

	glBindVertexArray(vao);
	glGenBuffers(1, &instanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(0));
	glVertexAttribDivisor(4, 1);

	size_t rowSize = 4 * sizeof(float);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(sizeof(vec3f) + 0));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(sizeof(vec3f) + 1 * rowSize));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(sizeof(vec3f) + 2 * rowSize));
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Instance), (void*)(sizeof(vec3f) + 3 * rowSize));
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
}

InstancedRenderableMesh::~InstancedRenderableMesh(){
	glDeleteBuffers(1, &instanceBuffer);
}

void InstancedRenderableMesh::apply(){
	RenderableMesh::apply();
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(Instance), instanceData.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void InstancedRenderableMesh::render(Renderer& r){
	r.useProgram(renderID);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceData.size());
	//glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, 100);
	glBindVertexArray(0);
}
