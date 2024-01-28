#ifndef VERLETPOINT_H
#define VERLETPOINT_H

#include "engine/vec.h"

class VerletPoint{
public:
	VerletPoint();
	virtual ~VerletPoint();

	virtual void tick(float dt);

	vec3f pos;
	vec3f oldPos;
	vec3f acc;

	virtual void brake(float dt, float coeff);
	bool onGround = false;

};

#endif // VERLETPOINT_H
