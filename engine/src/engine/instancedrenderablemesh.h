#ifndef INSTANCEDRENDERABLEMESH_H
#define INSTANCEDRENDERABLEMESH_H

#include "renderablemesh.h"

#include "renderer.h"

#include <vector>

struct Instance{
	vec3f pos;
	raw4x4 matrix;
};

class InstancedRenderableMesh : public RenderableMesh{
public:
	InstancedRenderableMesh(){}
	virtual ~InstancedRenderableMesh();

	static void init(Renderer& r);
	void create() override;
	void apply() override;

	std::vector<Instance> instanceData;

	void render(Renderer& r) override;

protected:
	static Program program;
	static int renderID;
	unsigned int instanceBuffer = 0;

private:
	//using RenderableMesh::create;

};

#endif // INSTANCEDRENDERABLEMESH_H
