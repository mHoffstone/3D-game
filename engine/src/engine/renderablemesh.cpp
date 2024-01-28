#include "renderablemesh.h"

#include <iostream>

#include <GL/glew.h>

RenderableMesh::RenderableMesh(){

}

RenderableMesh::~RenderableMesh(){
	maybeDestroy();
}

RenderableMesh& RenderableMesh::operator=(const Mesh& rhs){
	vertices = rhs.vertices;
	indices = rhs.indices;

	return *this;
}


void RenderableMesh::create(){
	if(m_hasCreated) throw std::runtime_error("RenderableMesh was already created!");
	MeshRenderer::create();
	createLife();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(1 * sizeof(vec3f)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(2 * sizeof(vec3f)));
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(struct vertex), (void*)(2 * sizeof(vec3f) + 1 * sizeof(vec2f)));

	m_hasCreated = true;
}

void RenderableMesh::apply(){
	if(!m_hasCreated) throw std::runtime_error("RenderableMesh was not created!");
	MeshRenderer::apply(*this);
	//std::cout << "vertices: " << vertices.size() << std::endl << "indices: " << indices.size() << std::endl << "triangles: " << indices.size()/3 << std::endl;
	m_hasApplied = true;
}

void RenderableMesh::render(Renderer& r){
	if(vertices.empty()) return;
	if(!m_hasApplied) throw std::runtime_error("RenderableMesh was not applied!");
	MeshRenderer::render(r, *this);
}

void RenderableMesh::bind(){
	MeshRenderer::bind();
}
bool RenderableMesh::hasCreated(){
	return m_hasCreated;
}
bool RenderableMesh::hasApplied(){
	return m_hasApplied;
}


void RenderableMesh::destroy(){
	glBindVertexArray(vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteVertexArrays(1, &vao);
}
