#include "rigidbody.h"

#include "tracer.h"

#include <cmath>

RigidBody::RigidBody(){}
RigidBody::~RigidBody(){}


// RigidCube

RigidCube::RigidCube(){}
RigidCube::RigidCube(float size) : size(size) {
	delta[0] = vec3f{0,0,0};
	delta[1] = vec3f::X*size;
	delta[2] = vec3f::X*size + vec3f::Z*size;
	delta[3] = vec3f::Z*size;
	delta[4] = vec3f::Y*size;
	delta[5] = vec3f::Y*size + vec3f::X*size;
	delta[6] = vec3f::Y*size + vec3f::X*size + vec3f::Z*size;
	delta[7] = vec3f::Y*size + vec3f::Z*size;
	setPos(vec3f{0.0f, 0.0f, 0.0f});

	mesh.create();
	Mesh appearance = Mesh::createCuboid(vec3f{0,0,0}, vec3f{0,0,0});
	appearance.setTexture(3);
	mesh.add(appearance);

	recalculateNormals();
}
RigidCube::~RigidCube(){}

void RigidCube::setPos(vec3f pos){
	for(int i = 0; i < 8; i++){
		points[i].pos = points[i].oldPos = pos + delta[i];
	}
}
void RigidCube::constrainTo(vec3f pos){
	for(int i = 0; i < 8; i++){
		points[i].pos = pos + delta[i];
	}
}
void RigidCube::accelerate(vec3f acc){
	for(int i = 0; i < 8; i++){
		points[i].acc += acc;
	}
}
void RigidCube::nudge(vec3f diff){
	for(int i = 0; i < 8; i++){
		points[i].pos += diff;
	}
}

vec3f RigidCube::getCenterPos(){
	vec3f center;
	for(int i = 0; i < 8; i++) center += points[i].pos;
	center /= 8.0f;
	return center;
}
vec3f RigidCube::getCenterOldPos(){
	vec3f center;
	for(int i = 0; i < 8; i++) center += points[i].oldPos;
	center /= 8.0f;
	return center;
}

/*WorldObjectType RigidCube::getWorldObjectType(){
	return WorldObjectType::RigidCube;
}*/


void RigidCube::tickMovement(float dt){
	for(int i = 0; i < 8; i++){
		points[i].tick(dt);
	}
}
void RigidCube::tickShape(float dt){
	for(int i = 0; i < 7; i++){
		for(int j = i+1; j < 8; j++){
			float wantedDist = (delta[j]-delta[i]).length();
			vec3f diff = points[i].pos - points[j].pos;
			float currentDist = diff.length();

			points[i].pos += diff * (0.5f*(wantedDist-currentDist)/currentDist);
			points[j].pos -= diff * (0.5f*(wantedDist-currentDist)/currentDist);

			/*points[i].acc += diff * (200.0f*(wantedDist-currentDist)/currentDist);
			points[j].acc -= diff * (200.0f*(wantedDist-currentDist)/currentDist);*/
		}
	}
	recalculateNormals();
}

// Collision detection
		/*auto testCollision = [](RigidCube& c1, RigidCube& c2){
			int shortestIndex = -1;
			float shortestDist = 99999999.0f;
			for(int i = 0; i < 3; i++){
				vec3f axis = c1.normals[i];
				float edge1 = c1.points[0].pos.dot(axis);
				float edge2 = c1.points[6].pos.dot(axis);
				if(edge1 > edge2) std::swap(edge1, edge2);

				int inside = 0;
				for(int j = 0; j < 8; j++){
					float v = c2.points[j].pos.dot(axis);
					if(edge1 < v && v < edge2){
						inside++;
						float diff;
						if(edge2-v < v-edge1) diff = edge2-v;
						else diff = edge1-v;
						if(std::abs(diff) < std::abs(shortestDist)){
							shortestDist = diff;
							shortestIndex = i;
						}
					}
				}
				if(inside == 0){
					return vec3f{0.0f, 0.0f, 0.0f};
				}
			}
			return c1.normals[shortestIndex] * shortestDist;
		};
		auto testCollision1 = [](RigidCube& c1, RigidCube& c2){
			for(int i = 0; i < 3; i++){
				vec3f axis = c1.normals[i];
				float edge1 = c1.points[0].pos.dot(axis);
				float edge2 = c1.points[6].pos.dot(axis);
				if(edge1 > edge2) std::swap(edge1, edge2);

				int inside = 0;
				for(int j = 0; j < 8; j++){
					float v = c2.points[j].pos.dot(axis);
					if(edge1 < v && v < edge2) inside++;
				}
				if(inside == 0){
					return false;
				}
			}
			return true;
		};*/
bool RigidCube::resolveCollision(float dt, std::shared_ptr<RigidCube> body){
	vec3f cubeDisplacement;
	float cubeDisplacementDist = 99999999.0f;
	bool cubesColliding = true;

	vec3f edgeDisplacement;
	bool edgeColliding = false;
	vec3f* displacedEdge = nullptr;
	RigidCube* displacedCube = nullptr;

	auto collide = [&cubeDisplacement, &cubeDisplacementDist, &edgeDisplacement, &cubesColliding, &edgeColliding, &displacedCube, &displacedEdge](RigidCube* c1, RigidCube* c2, float sign){
		bool collidingEdges[3] = {false, false, false};
		vec3f axes[3];
		float nearEdges[3], farEdges[3];
		for(int i = 0; i < 3; i++){
			vec3f axis = c1->normals[i];
			float edge1 = c1->points[0].pos.dot(axis);
			float edge2 = c1->points[6].pos.dot(axis);
			if(edge1 > edge2) std::swap(edge1, edge2);
			axes[i] = axis;
			nearEdges[i] = edge1;
			farEdges[i] = edge2;
		}

		for(int j = 0; j < 8; j++){
			int edgeInside = 0;
			vec3f localDisplacementAxis;
			float localDisplacementDist = 99999999.0f;
			for(int i = 0; i < 3; i++){
				float v = c2->points[j].pos.dot(axes[i]);
				if(nearEdges[i] <= v && v <= farEdges[i]){
					collidingEdges[i] = true;
					edgeInside++;
					float diff;
					if(farEdges[i]-v < v-nearEdges[i]) diff = farEdges[i]-v;
					else diff = nearEdges[i]-v;
					if(std::abs(diff) < std::abs(cubeDisplacementDist)){
						cubeDisplacementDist = sign * diff;
						cubeDisplacement = axes[i] * sign * diff;
					}
					if(std::abs(diff) < std::abs(localDisplacementDist)){
						localDisplacementDist = diff;
						localDisplacementAxis = axes[i];
					}
				}
			}
			if(edgeInside == 3){
				edgeColliding = true;
				edgeDisplacement = localDisplacementAxis * localDisplacementDist;
				displacedEdge = &c2->points[j].pos;
				displacedCube = c1;

				*displacedEdge += edgeDisplacement * 0.5f;
				displacedCube->nudge(edgeDisplacement * -0.5f);
			}
		}
		if(collidingEdges[0] == false || collidingEdges[1] == false || collidingEdges[2] == false) cubesColliding = false;
	};
	collide(this, body.get(), 1.0f);
	collide(body.get(), this, -1.0f);
	//if(edgeColliding && (!cubesColliding || edgeDisplacement.length2() > cubeDisplacement.length2())){
	/*if(edgeColliding){
		*displacedEdge += edgeDisplacement * 0.5f;
		displacedCube->nudge(edgeDisplacement * -0.5f);
		return true;
	}
	else */if(cubesColliding){
		nudge(cubeDisplacement * -0.5f);
		body->nudge(cubeDisplacement * 0.5f);
		return true;
	}
	return edgeColliding;
	//return false;
	/*vec3f axes[6];
	float nearEdges[6], farEdges[6];
	for(int i = 0; i < 3; i++){
		vec3f axis = normals[i];
		float edge1 = points[0].pos.dot(axis);
		float edge2 = points[6].pos.dot(axis);
		if(edge1 > edge2) std::swap(edge1, edge2);
		axes[i] = axis;
		nearEdges[i] = edge1;
		farEdges[i] = edge2;
	}
	for(int i = 0; i < 3; i++){
		vec3f axis = body->normals[i];
		float edge1 = body->points[0].pos.dot(axis);
		float edge2 = body->points[6].pos.dot(axis);
		if(edge1 > edge2) std::swap(edge1, edge2);
		axes[i+3] = axis;
		nearEdges[i+3] = edge1;
		farEdges[i+3] = edge2;
	}

	vec3f displacementAxis;
	float displacementDist = 99999999.0f;
	bool collidingEdges[3] = {false, false, false};
	bool cubesColliding = true;
	vec3f edgeDisplacementAxis;
	float edgeDisplacementDist = 99999999.0f;
	bool edgeColliding = false;
	vec3f* displacedEdge = nullptr;
	RigidCube* displacedCube = nullptr;

	for(int j = 0; j < 8; j++){
		int edgeInside = 0;
		vec3f localDisplacementAxis;
		float localDisplacementDist = 99999999.0f;
		for(int i = 0; i < 3; i++){
			float v = body->points[j].pos.dot(axes[i]);
			if(nearEdges[i] <= v && v <= farEdges[i]){
				collidingEdges[i] = true;
				edgeInside++;
				float diff;
				if(farEdges[i]-v < v-nearEdges[i]) diff = farEdges[i]-v;
				else diff = nearEdges[i]-v;
				if(std::abs(diff) < std::abs(displacementDist)){
					displacementDist = -diff;
					displacementAxis = axes[i];
				}
				if(std::abs(diff) < std::abs(localDisplacementDist)){
					localDisplacementDist = diff;
					localDisplacementAxis = axes[i];
				}
			}
		}
		if(edgeInside == 3){
			edgeColliding = true;
			edgeDisplacementAxis = localDisplacementAxis;
			edgeDisplacementDist = localDisplacementDist;
			displacedEdge = &body->points[j].pos;
			displacedCube = this;
		}
	}
	if(collidingEdges[0] == false || collidingEdges[1] == false || collidingEdges[2] == false) cubesColliding = false;
	collidingEdges[0] = false;
	collidingEdges[1] = false;
	collidingEdges[2] = false;
	for(int j = 0; j < 8; j++){
		int edgeInside = 0;
		vec3f localDisplacementAxis;
		float localDisplacementDist = 99999999.0f;
		for(int i = 3; i < 6; i++){
			float v = points[j].pos.dot(axes[i]);
			if(nearEdges[i] <= v && v <= farEdges[i]){
				collidingEdges[i-3] = true;
				edgeInside++;
				float diff;
				if(farEdges[i]-v < v-nearEdges[i]) diff = farEdges[i]-v;
				else diff = nearEdges[i]-v;
				if(std::abs(diff) < std::abs(displacementDist)){
					displacementDist = diff;
					displacementAxis = axes[i];
				}
				if(std::abs(diff) < std::abs(localDisplacementDist)){
					localDisplacementDist = diff;
					localDisplacementAxis = axes[i];
				}
			}
		}
		if(edgeInside == 3){
			edgeColliding = true;
			edgeDisplacementAxis = localDisplacementAxis;
			edgeDisplacementDist = localDisplacementDist;
			displacedEdge = &points[j].pos;
			displacedCube = body.get();
		}
	}
	if(collidingEdges[0] == false || collidingEdges[1] == false || collidingEdges[2] == false) cubesColliding = false;

	//if(edgeColliding && (!cubesColliding || std::abs(edgeDisplacementDist) > std::abs(displacementDist))){
	float factor = edgeColliding && cubesColliding ? 0.25f : 0.5f;
	if(edgeColliding){
		*displacedEdge += edgeDisplacementAxis * edgeDisplacementDist * factor;
		displacedCube->nudge(edgeDisplacementAxis * edgeDisplacementDist * -factor);
		return true;
	}
	if(cubesColliding){
		nudge(displacementAxis * displacementDist * factor);
		body->nudge(displacementAxis * displacementDist * -factor);
		return true;
	}

	return false;*/
}

bool RigidCube::resolveCollision(float dt, World& world){
	bool collided = false;
	auto resolveCollision = [](VerletPoint& p, float dt, World& world) -> bool{
		float h = world.getHeightAt(p.pos.x, p.pos.z);
		if(p.pos.y <= h){
			p.acc -= (p.pos-p.oldPos) * (1/dt) * vec3f{1,0,1} * 8.0f;
			p.pos.y = h;
			return true;
		}
		return false;
	};
	for(int i = 0; i < 8; i++) collided |= resolveCollision(points[i], dt, world);
	return collided;
}
bool RigidCube::resolveCollision(float dt, VerticalPlane& plane){
	bool collided = false;
	vec3f centerOldPos = getCenterOldPos();
	vec3f centerPos = getCenterPos();
	for(int i = 0; i < 8; i++){
		if(points[i].pos.y < plane.p1.y || points[i].pos.y > plane.p2.y) continue;

		vec3f planeDir = ((plane.p2-plane.p1) * vec3f{1.0f, 0.0f, 1.0f}).normalized();

		float d = planeDir.dot(points[i].pos);
		if(planeDir.dot(plane.p1) <= d && d <= planeDir.dot(plane.p2)){ // Entity is in front of plane
			if((plane.normal.dot(centerOldPos) > plane.normal.dot(plane.p1) &&
					plane.normal.dot(points[i].pos) < plane.normal.dot(plane.p1)) ||
					(plane.normal.dot(centerOldPos) < plane.normal.dot(plane.p1) &&
					plane.normal.dot(points[i].pos) > plane.normal.dot(plane.p1))
			){
				points[i].pos -= plane.normal * (points[i].pos-plane.p1).dot(plane.normal);
				collided = true;
			}
		}
	}
	return collided;
}
bool RigidCube::resolveCollision(float dt, HorizontalPlane& plane){
	bool collided = false;
	vec3f centerOldPos = getCenterOldPos();
	for(int i = 0; i < 8; i++){
		if(
			(points[i].pos-plane.p).dot(plane.v1) < 0 || plane.v1.dot(points[i].pos) > plane.v1.dot(plane.p+plane.v1) ||
			(points[i].pos-plane.p).dot(plane.v2) < 0 || plane.v2.dot(points[i].pos) > plane.v2.dot(plane.p+plane.v2)
		) continue;

		if(
			(centerOldPos.y > plane.p.y && points[i].pos.y < plane.p.y) ||
			(centerOldPos.y < plane.p.y && points[i].pos.y > plane.p.y)
		){
			points[i].pos.y = plane.p.y;
			points[i].onGround = true;
			points[i].brake(dt, frictionCoeff);
			collided = true;
		}
	}

	return collided;
}


void RigidCube::render(Renderer& r){
	// B
	mesh.vertices[0].normal = normals[0];
	mesh.vertices[1].normal = normals[0];
	mesh.vertices[2].normal = normals[0];
	mesh.vertices[3].normal = normals[0];
	mesh.vertices[0].pos = points[0].pos;
	mesh.vertices[1].pos = points[1].pos;
	mesh.vertices[2].pos = points[2].pos;
	mesh.vertices[3].pos = points[3].pos;

	// F
	mesh.vertices[4].normal = normals[1];
	mesh.vertices[5].normal = normals[1];
	mesh.vertices[6].normal = normals[1];
	mesh.vertices[7].normal = normals[1];
	mesh.vertices[4].pos = points[0].pos;
	mesh.vertices[5].pos = points[4].pos;
	mesh.vertices[6].pos = points[5].pos;
	mesh.vertices[7].pos = points[1].pos;

	// L
	mesh.vertices[8].normal  = normals[2];
	mesh.vertices[9].normal  = normals[2];
	mesh.vertices[10].normal = normals[2];
	mesh.vertices[11].normal = normals[2];
	mesh.vertices[8].pos = points[0].pos;
	mesh.vertices[9].pos = points[3].pos;
	mesh.vertices[10].pos = points[7].pos;
	mesh.vertices[11].pos = points[4].pos;

	// T
	mesh.vertices[12].normal = normals[3];
	mesh.vertices[13].normal = normals[3];
	mesh.vertices[14].normal = normals[3];
	mesh.vertices[15].normal = normals[3];
	mesh.vertices[12].pos = points[6].pos;
	mesh.vertices[13].pos = points[7].pos;
	mesh.vertices[14].pos = points[4].pos;
	mesh.vertices[15].pos = points[5].pos;

	// R
	mesh.vertices[16].normal = normals[4];
	mesh.vertices[17].normal = normals[4];
	mesh.vertices[18].normal = normals[4];
	mesh.vertices[19].normal = normals[4];
	mesh.vertices[16].pos = points[6].pos;
	mesh.vertices[17].pos = points[2].pos;
	mesh.vertices[18].pos = points[1].pos;
	mesh.vertices[19].pos = points[5].pos;

	// N
	mesh.vertices[20].normal = normals[5];
	mesh.vertices[21].normal = normals[5];
	mesh.vertices[22].normal = normals[5];
	mesh.vertices[23].normal = normals[5];
	mesh.vertices[20].pos = points[6].pos;
	mesh.vertices[21].pos = points[7].pos;
	mesh.vertices[22].pos = points[3].pos;
	mesh.vertices[23].pos = points[2].pos;

	mesh.apply();
	mesh.render(r);
}

void RigidCube::recalculateNormals(){
	normals[0] = (points[1].pos- points[0].pos).cross(points[3].pos- points[0].pos).normalized();
	normals[1] = (points[4].pos- points[0].pos).cross(points[1].pos- points[0].pos).normalized();
	normals[2] = (points[3].pos- points[0].pos).cross(points[4].pos- points[0].pos).normalized();
	normals[3] = (points[5].pos- points[6].pos).cross(points[7].pos- points[6].pos).normalized();
	normals[4] = (points[2].pos- points[6].pos).cross(points[5].pos- points[6].pos).normalized();
	normals[5] = (points[7].pos- points[6].pos).cross(points[2].pos- points[6].pos).normalized();
}

