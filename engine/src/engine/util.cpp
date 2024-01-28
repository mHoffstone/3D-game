#include "util.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <exception>
#include <stdexcept>

#include <GL/glew.h>

#define SMART_GET_NAME(str, val, con) if(str == "" && val == con) str = #con;

color& color::operator+=(const color& rhs){
	r += rhs.r;
	g += rhs.g;
	b += rhs.b;
	return *this;
}
color color::operator+(const color& rhs) const{
	color c = *this;
	c += rhs;
	return c;
}

std::string util::readFile(const std::string& path){

	std::ifstream reader;
	reader.open(path, std::ios::in|std::ios::binary|std::ios::ate);

	if(reader.is_open()){
		std::streampos size = reader.tellg();
		char* data = new char[size];
		reader.seekg(0, std::ios::beg);
		reader.read(data, size);
		reader.close();

		std::string str(data, size);
		delete[] data;

		return str;
	}
	else{
		throw std::ios_base::failure(path + ": Could not open file");
	}
	return "";
}
std::string util::readTextFile(const std::string& path){
	std::string content = readFile(path);
	replaceAll(content, "\r\n", "\n");
	replaceAll(content, "\r", "\n");
	while(content.find("\n\n") != std::string::npos) replaceAll(content, "\n\n", "\n");
	return content;
}
void util::replaceAll(std::string& str, const std::string& from, const std::string& to){
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}
bool util::isInt(std::string str){
	size_t i = 0;
	if(str[0] == '-') i = 1;
	for(; i < str.length(); i++){
		char c = str[i];
		if(c < '0' || c > '9') return false;
	}
	return true;
}
std::string util::toHex(int i){
	std::stringstream stream;
	stream << "0x"
		<< std::setfill('0') << std::setw(sizeof(int))
		<< std::hex << i;
	return stream.str();
}

bool util::checkGLErrors(){
	return false;
	bool hasError = false;
	while(GLenum err = glGetError()){
		std::string errText = "";
		SMART_GET_NAME(errText, err, GL_INVALID_ENUM)
		SMART_GET_NAME(errText, err, GL_INVALID_VALUE)
		SMART_GET_NAME(errText, err, GL_INVALID_OPERATION)
		SMART_GET_NAME(errText, err, GL_STACK_OVERFLOW)
		SMART_GET_NAME(errText, err, GL_STACK_UNDERFLOW)
		SMART_GET_NAME(errText, err, GL_OUT_OF_MEMORY)
		SMART_GET_NAME(errText, err, GL_INVALID_FRAMEBUFFER_OPERATION)
		if(errText == "") errText = toHex(err);
		log("OpenGL Error", errText);
		hasError = true;
	}
	return hasError;
}
