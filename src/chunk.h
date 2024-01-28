#ifndef CHUNK_H
#define CHUNK_H

#include "grid.h"
#include "entity.h"
#include "detailcollection.h"

#include "engine/instancedrenderablemesh.h"

class Chunk{
public:
	Chunk(float x, float y);
	virtual ~Chunk();

	Grid* getGrid();

	void create();
	void apply();

	void generateGround();
	void generateDetails(int levelOfDetail);

	vec2f getPos();

	void addEntity(std::shared_ptr<Entity> e);
	void remove(std::shared_ptr<Entity> e);

	void tick(float dt);

	void render(Renderer& r, bool details);

	DetailCollection detailCollections[2];

	static const int SIZE = 40;

private:
	Grid grid;
	std::vector<std::shared_ptr<Entity>> entities;
	std::vector<std::shared_ptr<Entity>> tickEntities;
	std::vector<std::shared_ptr<Entity>> renderClose;
	std::vector<std::shared_ptr<Entity>> renderFar;
	std::vector<std::shared_ptr<Entity>> hitableEntities;
	InstancedRenderableMesh treeMesh;
	vec2f pos;

	bool hasCreated = false;
	bool hasApplied = false;
	bool detailsGenerated[2] = {false, false};
	void applyDetails(const DetailCollection& detailCollection);

	friend class World;
};

#endif
