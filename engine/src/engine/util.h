#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <string>
#include "vec.h"

const float PI = 3.14159f;

struct color{
	float r, g, b;
	color& operator+=(const color& rhs);
	color operator+(const color& rhs) const;
};

struct vertex{
	vec3f pos;
	vec3f normal;
	vec2f texCoords;
	float texIndex;
};

struct raw4x4{
	float elements[16];
};

namespace util{

template <typename T>
void log_r(T val){
	std::cout << val << std::endl;
}

template <typename T, typename... Args>
void log_r(T first, Args... rest){
	std::cout << first << ' ';
	log_r(rest...);
}

template <typename... Args>
void log(Args... args){
	log_r(args...);
}


template <typename T, typename... Args>
void logt(T title, Args... values){
	//std::cout << title << ": " << value << std::endl;
	std::cout << title << ": ";
	log_r(values...);
}

std::string readFile(const std::string& path);
std::string readTextFile(const std::string& path);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
bool isInt(std::string str);
std::string toHex(int i);

bool checkGLErrors();

/*float lerp(float v0, float v1, float t);
float cerp(float v0, float v1, float t);
float berp(float v00, float v01, float v10, float v11, float tx, float ty, float (*interp)(float, float, float));*/

template <typename T>
T lerp(T v0, T v1, float t){
	return t*(v1-v0)+v0;
}

template <typename T>
T cerp(T v0, T v1, float t){
	float b = -2.0f * t*t*t + 3.0f * t*t;
	return (v1-v0)*b+v0;
}

template <typename T>
T berp(T v00, T v01, T v10, T v11, float tx, float ty, T (*interp)(T, T, float)){
	T vx0 = interp(v00, v10, tx), vx1 = interp(v01, v11, tx);
	return interp(vx0, vx1, ty);
}

}

#endif // UTIL_H
