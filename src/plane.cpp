#include "plane.h"

Plane::Plane(){}
Plane::~Plane(){}



VerticalPlane::VerticalPlane(){}

VerticalPlane::VerticalPlane(vec3f p1, vec3f p2) : p1(p1), p2(p2){
	normal = (p2-p1).cross(vec3f::Y).normalized();

	mesh.vertices.resize(4);
	mesh.indices = {0,1,2, 0,2,3};

	float h = p2.y-p1.y;
	mesh.vertices[0] = vertex{p1           , normal, vec2f{0.0f, 0.0f}, 0};
	mesh.vertices[1] = vertex{p2-vec3f::Y*h, normal, vec2f{1.0f, 0.0f}, 0};
	mesh.vertices[2] = vertex{p2           , normal, vec2f{1.0f, 1.0f}, 0};
	mesh.vertices[3] = vertex{p1+vec3f::Y*h, normal, vec2f{0.0f, 1.0f}, 0};

	mesh.create();
	mesh.apply();
}

VerticalPlane::~VerticalPlane(){}

void VerticalPlane::render(Renderer& r){
	mesh.render(r);
}


HorizontalPlane::HorizontalPlane(){}
HorizontalPlane::HorizontalPlane(vec3f p, vec3f v1, float d2) : p(p), v1(v1), d2(d2){
	v2 = -v1.cross(vec3f::Y) * d2/v1.length();

	mesh.vertices.resize(4);
	mesh.indices = {0,1,2, 0,2,3};

	mesh.vertices[0] = vertex{p      , vec3f::Y, vec2f{0.0f, 0.0f}, 0};
	mesh.vertices[1] = vertex{p+v1   , vec3f::Y, vec2f{1.0f, 0.0f}, 0};
	mesh.vertices[2] = vertex{p+v1+v2, vec3f::Y, vec2f{1.0f, 1.0f}, 0};
	mesh.vertices[3] = vertex{p+v2   , vec3f::Y, vec2f{0.0f, 1.0f}, 0};

	mesh.create();
	mesh.apply();
}

HorizontalPlane::~HorizontalPlane(){}

void HorizontalPlane::render(Renderer& r){
	mesh.render(r);
}




