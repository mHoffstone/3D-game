#ifndef ENTITY_H
#define ENTITY_H

#include "verletpoint.h"
#include "items.h"
#include "inventory.h"
#include "animation.h"

class HorizontalPlane;
class VerticalPlane;
class World;
class Chunk;

#include "engine/renderer.h"
#include "engine/renderablemesh.h"
#include "engine/instancedrenderablemesh.h"

#include <vector>
#include <memory>

class Entity;

class Hitbox{
public:
	enum class Type { BASIC, V_PLANE, H_PLANE, CUBE };

	Hitbox(Entity* owner);
	virtual ~Hitbox();
	virtual Type getType() = 0;
	virtual bool resolveCollision(float dt, Entity* e) = 0;

protected:
	Entity* owner;
};

class BasicHitbox : public Hitbox{
public:
	BasicHitbox(Entity* owner);
	virtual ~BasicHitbox();

	float radius, height;

	virtual Type getType() override;
	virtual bool resolveCollision(float dt, Entity* e) override;
};

class VerticalPlaneHitbox : public Hitbox{
public:
	VerticalPlaneHitbox(Entity* owner);
	virtual ~VerticalPlaneHitbox();

	vec3f getDelta();
	void setDelta(vec3f delta);
	vec3f getNormal();

	virtual Type getType() override;
	virtual bool resolveCollision(float dt, Entity* e) override;

private:
	vec3f delta;
	vec3f cachedNormal;
};

class HorizontalPlaneHitbox : public Hitbox{
public:
	HorizontalPlaneHitbox(Entity* owner);
	virtual ~HorizontalPlaneHitbox();

	void setSize(vec3f side, float width);
	vec3f getSide1();
	vec3f getSide2();

	virtual Type getType() override;
	virtual bool resolveCollision(float dt, Entity* e) override;

private:
	vec3f side1;
	vec3f side2;
	float width;
};

class EntityAI{
public:
	/*EntityAI();
	virtual ~EntityAI();*/

	virtual void tick(float dt) = 0;

	Entity* owner;
};

class StupidAI : public EntityAI{
public:
	StupidAI(){}
	virtual ~StupidAI(){}

	virtual void tick(float dt) override;
};

class ChaserAI : public EntityAI{
public:
	ChaserAI(){}
	virtual ~ChaserAI(){}

	virtual void tick(float dt) override;
};

class EntityRenderer{
public:
	EntityRenderer(Entity* owner);
	virtual void create() = 0;
	virtual void apply() = 0;
	virtual void render(Renderer& r) = 0;

	Entity* owner;
};

class MeshEntityRenderer : public EntityRenderer{
public:
	MeshEntityRenderer(Entity* owner) : EntityRenderer(owner){}
	virtual ~MeshEntityRenderer(){}

	virtual void create() override;
	virtual void apply() override;
	virtual void render(Renderer& r) override;

	RenderableMesh mesh;
};

class AnimationEntityRenderer : public EntityRenderer{
public:
	AnimationEntityRenderer(Entity* owner) : EntityRenderer(owner){}
	virtual ~AnimationEntityRenderer(){}

	virtual void create() override;
	virtual void apply() override;
	virtual void render(Renderer& r) override;

	Animation animation;
};




class Entity : public VerletPoint{
public:
	struct StoreData{
		bool tickable = false;
		int render = 0;
		//InstancedRenderableMesh* irMesh = nullptr;
		Chunk* chunk = nullptr;
		int irIndex = -1;
		int levelOfDetail = -1;
		int detailIndex = -1;
		bool hitable = false;
	};
	Entity();
	Entity(vec3f pos);
	virtual ~Entity();

	StoreData d;
	std::shared_ptr<Hitbox> hitbox = nullptr;
	std::shared_ptr<EntityAI> entityAI = nullptr;
	std::shared_ptr<EntityRenderer> entityRenderer = nullptr;

	bool onGround = false;

	float mass = 1.0f;
	float acceleration = 20.0f;
	vec2f rotation;

	float eyeHeight = 1.55f;
	float frictionCoeff = 8.0f;

	void setPos(float x, float z);

	virtual vec3f getEyePos();
	virtual vec3f getLookDir();

	virtual vec3f getCenterPos();

	virtual void tick(float dt) override;
	virtual void tryMove(float dt, float dx, float dz);
	virtual void tryJump(float dt);
	virtual void tryHurt(std::shared_ptr<Entity> e, float hp);

	//virtual void brake(float dt) override;

	virtual bool resolveCollision(float dt, World& world);
	virtual bool resolveCollision(float dt, VerticalPlane& plane);
	virtual bool resolveCollision(float dt, HorizontalPlane& plane);

	virtual bool resolveCollision(float dt, std::shared_ptr<Entity> e);

	virtual bool interactWith(std::shared_ptr<Entity> e, ItemStack& itemStack);

	virtual void create();
	virtual void apply();
	virtual void render(Renderer& r);

	virtual void setInstanceRender(int detailIndex, int levelOfDetail, const Instance& instance);

	virtual void hurt(float hp);
	float hp = 20.0f;

	float interactCooldown = 0.0f;

	Item item;

	Inventory inventory;
	
};

#endif // ENTITY_H
