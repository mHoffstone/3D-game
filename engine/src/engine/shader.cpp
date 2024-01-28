#include "shader.h"

#include <GL/glew.h>

#include "util.h"

static unsigned int compileShader(unsigned int type, const std::string& source){
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE){
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);

		std::string typeString;
		switch(type){
			case GL_VERTEX_SHADER:
				typeString = "vertex shader";
				break;
			case GL_FRAGMENT_SHADER:
				typeString = "fragment shader";
				break;
			case GL_GEOMETRY_SHADER:
				typeString = "geometry shader";
				break;
			default:
				typeString = "unknown shader";
				break;
		}
		util::log("Failed to compile", typeString);
		util::log(message);
		util::log("Source:");
		util::log(source);

		delete[] message;

		return 0;
	}

	return id;
}

Shader::Shader(){

}


Shader::Shader(unsigned int type, const std::string& code) : type(type) {
	shader = compileShader(type, code);
	createLife();
}

Shader::~Shader(){
	//deleteShader();
	maybeDestroy();
}

unsigned int Shader::getType() const{
	return type;
}
unsigned int Shader::getShader() const{
	return shader;
}
void Shader::deleteShader(){
	if(shader != 0) glDeleteShader(shader);
	shader = 0;
}

void Shader::destroy(){
	glDeleteShader(shader);
}

