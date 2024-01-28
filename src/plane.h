#ifndef PLANE_H
#define PLANE_H

#include "engine/vec.h"
#include "engine/renderablemesh.h"

class Plane{
public:
	Plane();
	virtual ~Plane();
};

class VerticalPlane{
public:
	VerticalPlane();
	VerticalPlane(vec3f p1, vec3f p2);
	virtual ~VerticalPlane();

	virtual void render(Renderer& r);

	vec3f p1, p2;
	vec3f normal;
	RenderableMesh mesh;
};

class HorizontalPlane{
public:
	HorizontalPlane();
	HorizontalPlane(vec3f p, vec3f v1, float d2);
	virtual ~HorizontalPlane();

	virtual void render(Renderer& r);

	vec3f p;
	vec3f v1, v2;
	float d2 = 0.0f;

	RenderableMesh mesh;
};

#endif // PLANE_H
