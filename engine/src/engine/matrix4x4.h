#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include "util.h"

class Matrix4x4{
	public:
		Matrix4x4();
		Matrix4x4(raw4x4 m);
		virtual ~Matrix4x4();

		raw4x4 elements;
		float& at(int r, int c);
		float at(int r, int c) const;

		void set(const Matrix4x4& m);
		void setColumn(int c, const vec3f& v);

		static Matrix4x4 identity();
		static Matrix4x4 projection(float left, float right, float top, float bottom, float near, float far);
		static Matrix4x4 projection(float fov, float w, float h, float far, float near);
		static Matrix4x4 rotationX(float angle);
		static Matrix4x4 rotationY(float angle);
		static Matrix4x4 rotationZ(float angle);
		//static Matrix4x4 rotationZ(float angle);
		static Matrix4x4 rotationXY(float xAngle, float yAngle);
		static Matrix4x4 rotationYX(float xAngle, float yAngle);
		static Matrix4x4 rotationXY(vec2f angles);
		static Matrix4x4 lookAt(const vec3f& r, const vec3f& u, const vec3f& f);
		static Matrix4x4 translation(vec3f pos);
		static Matrix4x4 scale(vec3f factor);
		static Matrix4x4 scale(float factor);

		Matrix4x4& operator*=(const Matrix4x4& rhs);
		Matrix4x4 operator*(const Matrix4x4& rhs) const;

		vec3f operator*(const vec3f& rhs) const;
};

#endif // MATRIX4X4_H
