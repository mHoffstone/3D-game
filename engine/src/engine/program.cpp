#include "program.h"

#include <GL/glew.h>

#include <iostream>

Program::Program(){
	//std::cout << ' ' << this << std::endl;
}

Program::~Program(){
	//std::cout << '~' << this << std::endl;
	maybeDestroy();
}

void Program::createProgram(){
	program = glCreateProgram();
	createLife();
}
unsigned int Program::getProgram() const{
	return program;
}

bool Program::setShaders(std::initializer_list<Shader*> shaders){
	for(auto s : shaders){
		if(s->getShader() == 0) throw std::runtime_error("Shader was not valid!");
		glAttachShader(program, s->getShader());
	}

	glLinkProgram(program);
	glValidateProgram(program);

	int result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if(result == GL_FALSE){
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		char* message = new char[length];
		glGetProgramInfoLog(program, length, &length, message);

		std::cerr << "Failed to link program!" << std::endl;
		if(length > 0) std::cout << message << std::endl;

		delete[] message;

		for(auto s : shaders){
			glDetachShader(program, s->getShader());
			s->deleteShader();
		}
		deleteProgram();

		if(program == 0){
			throw std::runtime_error("Failed to link program!");
		}
	}

	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
	if(result == GL_FALSE){
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

		char* message = new char[length];
		glGetProgramInfoLog(program, length, &length, message);

		std::cerr << "Failed to validate program!" << std::endl;
		if(length > 0) std::cout << message << std::endl;

		delete[] message;

		for(auto s : shaders){
			glDetachShader(program, s->getShader());
			s->deleteShader();
		}
		deleteProgram();

		if(program == 0){
			throw std::runtime_error("Failed to validate program!");
		}

		return false;
	}

	for(auto s : shaders){
		glDetachShader(program, s->getShader());
		s->deleteShader();
	}

	return true;
}

void Program::useProgram() const{
	if(program == 0) throw std::runtime_error("Program was not a valid program!");
	glUseProgram(program);
}

void Program::deleteProgram(){
	if(program != 0) reset();
	program = 0;
}
void Program::destroy(){
	glDeleteProgram(program);
}
