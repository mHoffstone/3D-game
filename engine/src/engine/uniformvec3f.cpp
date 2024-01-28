#include "uniformvec3f.h"

#include <GL/glew.h>

UniformVec3f::UniformVec3f(){
	//ctor
}

UniformVec3f::~UniformVec3f(){
	//dtor
}

UniformVec3f& UniformVec3f::operator=(const vec3f& v){
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

void UniformVec3f::updateUniform(){
	glUniform3f(location, x, y, z);
}
