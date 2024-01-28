#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "renderer.h"
#include "specificmesh.h"

template <class T>
class MeshRenderer{
public:
	MeshRenderer();
	virtual ~MeshRenderer();
	virtual void create();
	virtual void apply(const SpecificMesh<T>& mesh);
	virtual void render(Renderer& r, const SpecificMesh<T>& mesh);
	virtual void bind();

	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int ibo = 0;
};



template <class T>
MeshRenderer<T>::MeshRenderer(){

}
template <class T>
MeshRenderer<T>::~MeshRenderer(){

}

template <class T>
void MeshRenderer<T>::create(){
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
}

template <class T>
void MeshRenderer<T>::apply(const SpecificMesh<T>& mesh){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(T), mesh.vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);
}

template <class T>
void MeshRenderer<T>::render(Renderer& r, const SpecificMesh<T>& mesh){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
}

template <class T>
void MeshRenderer<T>::bind(){
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

#endif
