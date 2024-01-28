#include "verletpoint.h"

VerletPoint::VerletPoint(){}

VerletPoint::~VerletPoint(){}

void VerletPoint::tick(float dt){
	vec3f dp = pos - oldPos;
	oldPos = pos;
	pos = pos + dp + acc * dt*dt;

	acc = {0.0f, 0.0f, 0.0f};
}

void VerletPoint::brake(float dt, float coeff){
	acc -= (pos-oldPos) * (1/dt) * vec3f{1,0,1} * coeff;
}


