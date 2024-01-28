#include "renderer.h"
#include "matrix4x4.h"

Renderer::Renderer(){

}

Renderer::~Renderer(){

}
int Renderer::addProgram(Program program){
	render_program p;
	p.program = program;
	p.projection.setLocation(program, "u_projection");
	p.rotation.setLocation(program, "u_rotation");
	p.translation.setLocation(program, "u_translation");
	renderPrograms.push_back(p);
	return renderPrograms.size() - 1;
}

void Renderer::useProgram(int id){
	currentProgram = id;
	render_program& p = renderPrograms.at(id);
	p.program.useProgram();
	p.projection = projectionMatrix;
	p.projection.updateUniform();
	p.rotation = Matrix4x4::rotationXY(cameraRotation);
	p.rotation.updateUniform();
	p.translation = cameraPosition;
	p.translation.updateUniform();
}

void Renderer::setTranslation(vec3f pos){
	renderPrograms.at(currentProgram).translation = cameraPosition - pos;
	renderPrograms.at(currentProgram).translation.updateUniform();
}


void Renderer::setCameraPosition(vec3f pos){
	cameraPosition = pos;
}
void Renderer::setCameraRotation(vec2f rot){
	cameraRotation = rot;
}

int Renderer::getViewportWidth(){
	return vpWidth;
}
int Renderer::getViewportHeight(){
	return vpHeight;
}


Matrix4x4 Renderer::createProjectionMatrix(){
	const float fov = 2.0f*PI/5.0f;
	return Matrix4x4::projection(fov, vpWidth, vpHeight, 500.0f, 0.1f);
}
