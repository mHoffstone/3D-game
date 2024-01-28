#ifndef VEC_H
#define VEC_H

#include <string>

struct vec2f{
	float x = 0.0f, y = 0.0f;

	bool operator==(const vec2f& rhs) const;

	vec2f& operator+=(const vec2f& rhs);
	vec2f operator+(const vec2f& rhs) const;

	vec2f& operator-=(const vec2f& rhs);
	vec2f operator-(const vec2f& rhs) const;
	vec2f operator-() const;

	vec2f& operator*=(const float& rhs);
	vec2f operator*(const float& rhs) const;

	vec2f& operator*=(const vec2f& rhs);
	vec2f operator*(const vec2f& rhs) const;

	friend vec2f operator*(float lhs, vec2f rhs);

	vec2f& operator/=(const float& rhs);
	vec2f operator/(const float& rhs) const;

	float length() const;
	float length2() const;
	void normalize();
	vec2f normalized() const;

	float dot(const vec2f& v2) const;
	vec2f cross(const vec2f& v2) const;
};
struct vec3f{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	static const vec3f X;
	static const vec3f Y;
	static const vec3f Z;
	static const vec3f origo;

	bool operator==(const vec3f& rhs) const;

	vec3f& operator+=(const vec3f& rhs);
	vec3f operator+(const vec3f& rhs) const;

	vec3f& operator-=(const vec3f& rhs);
	vec3f operator-(const vec3f& rhs) const;
	vec3f operator-() const;

	vec3f& operator*=(const float& rhs);
	vec3f operator*(const float& rhs) const;

	vec3f& operator*=(const vec3f& rhs);
	vec3f operator*(const vec3f& rhs) const;

	friend vec3f operator*(float lhs, vec3f rhs);

	vec3f& operator/=(const float& rhs);
	vec3f operator/(const float& rhs) const;

	float length() const;
	float length2() const;
	void normalize();
	vec3f normalized() const;

	vec2f angle() const;

	float dot(const vec3f& v2) const;
	vec3f cross(const vec3f& v2) const;

	operator std::string() const;
};

#endif
