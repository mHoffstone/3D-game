#include "uniform.h"

#include <GL/glew.h>

#include <stdexcept>

Uniform::Uniform(){
	//ctor
}

Uniform::~Uniform(){
	//dtor
}

void Uniform::setLocation(const Program& program, const std::string& name){
	location = glGetUniformLocation(program.getProgram(), name.c_str());
	if(location == -1){
		throw std::runtime_error(name + " could not be found in shader!");
	}
}

int Uniform::getLocation(){
	return location;
}
