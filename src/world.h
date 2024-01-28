#ifndef WORLD_H
#define WORLD_H

class RigidCube;
class VerticalPlane;
class HorizontalPlane;
class Player;

#include "chunk.h"
#include "grid.h"
#include "vegetation.h"
#include "rigidbody.h"
#include "plane.h"

#include "engine/renderablemesh.h"
#include "engine/instancedrenderablemesh.h"

#include <vector>

class World{
public:
	World();

	float getHeightAt(float x, float y);
	void setHeightAt(int x, int y, float height);
	vec3f getPosAt(float x, float y);
	vec3f getNormalAt(float x, float y);
	Chunk* getChunk(float x, float y);

	void generate(float x, float y);
	void generateGround(float x, float y);
	void generateDetails(float x, float y, int levelOfDetails);

	void tick(float dt);

	//void popGrass();
	void addEntity(std::shared_ptr<Entity> e);
	std::shared_ptr<Entity> getEntityFrom(std::shared_ptr<Entity> e);
	std::shared_ptr<Entity> getEntityFrom(vec3f pos, vec3f dir, float maxDist);
	void remove(std::shared_ptr<Entity> e);

	void render(Renderer& r);

	static World& getWorld();

	std::vector<std::shared_ptr<RigidCube>> cubes;
	std::vector<std::shared_ptr<VerticalPlane>> vPlanes;
	std::vector<std::shared_ptr<HorizontalPlane>> hPlanes;
	std::shared_ptr<Player> player;

private:
	std::vector<Chunk*> chunks;

	std::vector<std::shared_ptr<Entity>> entities;
};

#endif // WORLD_H
