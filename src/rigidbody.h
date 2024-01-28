#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "entity.h"
#include "verletpoint.h"

class World;

#include "world.h"
#include "plane.h"

#include "engine/renderer.h"
#include "engine/renderablemesh.h"

//class RigidBody : public Entity{
class RigidBody : public Entity{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void setPos(vec3f pos) = 0;
	virtual void constrainTo(vec3f pos) = 0;
	virtual void accelerate(vec3f acc) = 0;
	virtual void nudge(vec3f diff) = 0;

	virtual vec3f getCenterPos() = 0;
	virtual vec3f getCenterOldPos() = 0;

	virtual void tickMovement(float dt) = 0;
	virtual void tickShape(float dt) = 0;
	virtual void render(Renderer& r) = 0;
};

class RigidCube : public RigidBody{
public:
	RigidCube();
	RigidCube(float size);
	virtual ~RigidCube();

	virtual void setPos(vec3f pos) override;
	virtual void constrainTo(vec3f pos) override;
	virtual void accelerate(vec3f acc) override;
	virtual void nudge(vec3f diff) override;

	virtual vec3f getCenterPos() override;
	virtual vec3f getCenterOldPos() override;
	//virtual WorldObjectType getWorldObjectType() override;

	virtual void tickMovement(float dt) override;
	virtual void tickShape(float dt) override;
	virtual bool resolveCollision(float dt, std::shared_ptr<RigidCube> body);
	virtual bool resolveCollision(float dt, World& world);
	virtual bool resolveCollision(float dt, VerticalPlane& plane);
	virtual bool resolveCollision(float dt, HorizontalPlane& plane);
	virtual void render(Renderer& r) override;

	virtual void recalculateNormals();

	VerletPoint points[8];
	vec3f normals[6];
	RenderableMesh mesh;

private:
	float size;
	vec3f delta[8];
};

#endif // RIGIDBODY_H
