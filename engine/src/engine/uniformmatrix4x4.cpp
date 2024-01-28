#include "uniformmatrix4x4.h"

#include <GL/glew.h>

UniformMatrix4x4::UniformMatrix4x4(){
	//ctor
}

UniformMatrix4x4::~UniformMatrix4x4(){
	//dtor
}

UniformMatrix4x4& UniformMatrix4x4::operator=(const Matrix4x4& m){
	set(m);
	return *this;
}

void UniformMatrix4x4::updateUniform(){
	glUniformMatrix4fv(location, 1, false, &elements.elements[0]);
}

