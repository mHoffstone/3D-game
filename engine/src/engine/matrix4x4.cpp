#include "matrix4x4.h"

#include <iostream>
#include <cmath>

Matrix4x4::Matrix4x4(){
	for(int i = 0; i < 16; i++){
		elements.elements[i] = 0.0f;
	}
}
Matrix4x4::Matrix4x4(raw4x4 m) : elements(m){

}


Matrix4x4::~Matrix4x4(){
	//dtor
}

float& Matrix4x4::at(int r, int c){
	return elements.elements[4*c + r];
}
float Matrix4x4::at(int r, int c) const{
	return elements.elements[4*c + r];
}
void Matrix4x4::set(const Matrix4x4& m){
	for(int i = 0; i < 16; i++) elements.elements[i] = m.elements.elements[i];
}

void Matrix4x4::setColumn(int c, const vec3f& v){
	elements.elements[4*c + 0] = v.x;
	elements.elements[4*c + 1] = v.y;
	elements.elements[4*c + 2] = v.z;
}

Matrix4x4 Matrix4x4::identity(){
	Matrix4x4 m;
	m.at(0,0) = 1.0f;
	m.at(1,1) = 1.0f;
	m.at(2,2) = 1.0f;
	m.at(3,3) = 1.0f;
	return m;
}

Matrix4x4 Matrix4x4::projection(float left, float right, float top, float bottom, float near, float far){
	Matrix4x4 m;
	m.at(0, 0) = (2*near) / (right-left);

	m.at(1, 1) = (2*near) / (top-bottom);

	m.at(0, 2) = (right+left) / (right-left);
	m.at(1, 2) = (top+bottom) / (top-bottom);
	m.at(2, 2) = -(far+near) / (far-near);
	m.at(3, 2) = -1.0f,

	m.at(2, 3) = (-2*far*near) / (far-near);
	return m;
}
Matrix4x4 Matrix4x4::projection(float fov, float w, float h, float far, float near){
	Matrix4x4 m;
	m.at(0, 0) = (h/w) / std::tan(fov/2.0f);

	m.at(1, 1) = 1.0f / std::tan(fov/2.0f);

	m.at(2, 2) = -(far) / (far-near);
	m.at(2, 3) = -(far*near) / (far-near);
	m.at(3, 2) = -1.0f;
	return m;
}

Matrix4x4 Matrix4x4::rotationX(float angle){
	Matrix4x4 m;
	m.at(0, 0) = 1.0f;

	m.at(1, 1) = std::cos(angle);
	m.at(2, 1) = std::sin(angle);

	m.at(1, 2) = -std::sin(angle);
	m.at(2, 2) = std::cos(angle);

	m.at(3, 3) = 1.0f;
	return m;
}
Matrix4x4 Matrix4x4::rotationY(float angle){
	Matrix4x4 m;
	m.at(0, 0) = std::cos(angle);
	m.at(2, 0) = -std::sin(angle);

	m.at(1, 1) = 1.0f;

	m.at(0, 2) = std::sin(angle);
	m.at(2, 2) = std::cos(angle);

	m.at(3, 3) = 1.0f;
	return m;
}
Matrix4x4 Matrix4x4::rotationZ(float angle){
	Matrix4x4 m;
	m.at(0, 0) = std::cos(angle);
	m.at(1, 0) = std::sin(angle);

	m.at(0, 1) = -std::sin(angle);
	m.at(1, 1) = std::cos(angle);

	m.at(2, 2) = 1.0f;

	m.at(3, 3) = 1.0f;
	return m;
}

Matrix4x4 Matrix4x4::rotationXY(float xAngle, float yAngle){
	Matrix4x4 x = rotationX(xAngle);
	Matrix4x4 y = rotationY(yAngle);
	return x*y;
}
Matrix4x4 Matrix4x4::rotationYX(float xAngle, float yAngle){
	Matrix4x4 x = rotationX(xAngle);
	Matrix4x4 y = rotationY(yAngle);
	return y*x;
}
Matrix4x4 Matrix4x4::rotationXY(vec2f angles){
	Matrix4x4 x = rotationX(angles.x);
	Matrix4x4 y = rotationY(angles.y);
	return x*y;
}

Matrix4x4 Matrix4x4::translation(vec3f pos){
	Matrix4x4 m = identity();
	m.at(0, 3) = pos.x;
	m.at(1, 3) = pos.y;
	m.at(2, 3) = pos.z;
	m.at(3, 3) = 1.0f;
	return m;
}
Matrix4x4 Matrix4x4::lookAt(const vec3f& r, const vec3f& u, const vec3f& f){
	Matrix4x4 m;
	m.setColumn(0, r);
	m.setColumn(1, u);
	m.setColumn(2, f);
	m.at(3,3) = 1;
	return m;
}

Matrix4x4 Matrix4x4::scale(vec3f factor){
	Matrix4x4 m;
	m.at(0, 0) = factor.x;
	m.at(1, 1) = factor.y;
	m.at(2, 2) = factor.z;
	m.at(3, 3) = 1.0f;
	return m;
}
Matrix4x4 Matrix4x4::scale(float factor){
	Matrix4x4 m;
	m.at(0, 0) = factor;
	m.at(1, 1) = factor;
	m.at(2, 2) = factor;
	m.at(3, 3) = 1.0f;
	return m;
}


Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs){
	Matrix4x4 m = *this;
	for(int c = 0; c < 4; c++){
		for(int r = 0; r < 4; r++){
			float val = 0.0f;
			for(int k = 0; k < 4; k++){
				val += m.at(r, k) * rhs.at(k, c);
			}
			this->at(r, c) = val;
		}
	}
	return *this;
}
Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const{
	Matrix4x4 v = *this;
	v *= rhs;
	return v;
}

vec3f Matrix4x4::operator*(const vec3f& rhs) const{
	vec3f v;
	v.x = rhs.x*at(0, 0) + rhs.y*at(1, 0) + rhs.z*at(2, 0);
	v.y = rhs.x*at(0, 1) + rhs.y*at(1, 1) + rhs.z*at(2, 1);
	v.z = rhs.x*at(0, 2) + rhs.y*at(1, 2) + rhs.z*at(2, 2);
	return v;
}
