#ifndef ANIMATION_H
#define ANIMATION_H

#include "engine/specificmesh.h"

#include "engine/mesh.h"
#include "engine/meshrenderer.h"

#include "engine/util.h"
#include "engine/renderer.h"

struct AnimatedVertex : public vertex{
	float boneId;
	float weight;
};

class Animation{
public:
	Animation();
	virtual ~Animation();

	static void init(Renderer& r);

	void setBaseMesh(const Mesh& m);
	int addPart(const Mesh& m);
	vec3f pos;

	void create();
	void apply();

	void render(Renderer& r);

private:
	bool m_hasCreated = false;
	bool m_hasApplied = false;
	SpecificMesh<AnimatedVertex> mesh;
	int currentBone = 1;
	MeshRenderer<AnimatedVertex> meshRenderer;

	static Program program;
	static int rendererID;
	static Uniform displacements;

};

#endif
