#include "vec.h"

#include <cmath>
#include "util.h"

constexpr vec3f vec3f::X = vec3f{1.0f, 0.0f, 0.0f};
constexpr vec3f vec3f::Y = vec3f{0.0f, 1.0f, 0.0f};
constexpr vec3f vec3f::Z = vec3f{0.0f, 0.0f, 1.0f};
constexpr vec3f vec3f::origo = vec3f{0.0f, 0.0f, 0.0f};

bool vec2f::operator==(const vec2f& rhs) const{
	return x == rhs.x && y == rhs.y;
}

vec2f& vec2f::operator+=(const vec2f& rhs){
	x += rhs.x;
	y += rhs.y;
	return *this;
}
vec2f vec2f::operator+(const vec2f& rhs) const{
	vec2f v = *this;
	v += rhs;
	return v;
}

vec2f& vec2f::operator-=(const vec2f& rhs){
	x -= rhs.x;
	y -= rhs.y;
	return *this;
}
vec2f vec2f::operator-(const vec2f& rhs) const{
	vec2f v = *this;
	v -= rhs;
	return v;
}
vec2f vec2f::operator-() const{
	return vec2f{-x, -y};
}

vec2f& vec2f::operator*=(const float& rhs){
	x *= rhs;
	y *= rhs;
	return *this;
}
vec2f vec2f::operator*(const float& rhs) const{
	vec2f v = *this;
	v *= rhs;
	return v;
}

vec2f& vec2f::operator*=(const vec2f& rhs){
	x *= rhs.x;
	y *= rhs.y;
	return *this;
}
vec2f vec2f::operator*(const vec2f& rhs) const{
	vec2f v = *this;
	v *= rhs;
	return v;
}

vec2f operator*(float lhs, vec2f rhs){
	return vec2f{lhs*rhs.x, lhs*rhs.y};
}

vec2f& vec2f::operator/=(const float& rhs){
	x /= rhs;
	y /= rhs;
	return *this;
}
vec2f vec2f::operator/(const float& rhs) const{
	vec2f v = *this;
	v /= rhs;
	return v;
}

float vec2f::length() const{
	return std::sqrt(x*x + y*y);
}
float vec2f::length2() const{
	return x*x + y*y;
}
void vec2f::normalize(){
	*this *= 1.0f/length();
}
vec2f vec2f::normalized() const{
	vec2f v = *this;
	v.normalize();
	return v;
}


bool vec3f::operator==(const vec3f& rhs) const{
	return x == rhs.x && y == rhs.y && z == rhs.z;
}
vec3f& vec3f::operator+=(const vec3f& rhs){
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}
vec3f vec3f::operator+(const vec3f& rhs) const{
	vec3f v = *this;
	v += rhs;
	return v;
}

vec3f& vec3f::operator-=(const vec3f& rhs){
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
	return *this;
}
vec3f vec3f::operator-(const vec3f& rhs) const{
	vec3f v = *this;
	v -= rhs;
	return v;
}
vec3f vec3f::operator-() const{
	return vec3f{-x, -y, -z};
}

vec3f& vec3f::operator*=(const float& rhs){
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}
vec3f vec3f::operator*(const float& rhs) const{
	vec3f v = *this;
	v *= rhs;
	return v;
}

vec3f& vec3f::operator*=(const vec3f& rhs){
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
	return *this;
}
vec3f vec3f::operator*(const vec3f& rhs) const{
	vec3f v = *this;
	v *= rhs;
	return v;
}

vec3f operator*(float lhs, vec3f rhs){
	return vec3f{lhs*rhs.x, lhs*rhs.y, lhs*rhs.z};
}

vec3f& vec3f::operator/=(const float& rhs){
	x /= rhs;
	y /= rhs;
	z /= rhs;
	return *this;
}
vec3f vec3f::operator/(const float& rhs) const{
	vec3f v = *this;
	v /= rhs;
	return v;
}

float vec3f::length() const{
	return std::sqrt(x*x + y*y + z*z);
}
float vec3f::length2() const{
	return x*x + y*y + z*z;
}
void vec3f::normalize(){
	*this *= 1.0f/length();
}
vec3f vec3f::normalized() const{
	vec3f v = *this;
	v.normalize();
	return v;
}
vec2f vec3f::angle() const{
	vec2f rot;
	rot.y = std::atan2(z, x) + PI/2.0f;
	vec3f rel = {x, 0.0f, z};
	rot.x = std::atan2(y, rel.length());
	return rot;
}

float vec3f::dot(const vec3f& v2) const{
	return x * v2.x + y * v2.y + z * v2.z;
}

vec3f vec3f::cross(const vec3f& v2) const{
	return vec3f{
		y*v2.z - z*v2.y,
		z*v2.x - x*v2.z,
		x*v2.y - y*v2.x
	};
}

vec3f::operator std::string() const{
	return std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z);
}


/*vec3f noNegative(const vec3f& v){
	return vec3f{v.x > 0.0f ? v.x : 0.0f, v.y > 0.0f ? v.y : 0.0f, v.z > 0.0f ? v.z : 0.0f};
}

vec3f reflect(const vec3f& dir, const vec3f& normal){
	return dir - normal * 2.0f * dir.dot(normal);
}*/
