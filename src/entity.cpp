#include "entity.h"

#include "player.h"

#include "world.h"
#include "rigidbody.h"

#include <cmath>

Hitbox::Hitbox(Entity* owner) : owner(owner){}
Hitbox::~Hitbox(){}

BasicHitbox::BasicHitbox(Entity* owner) : Hitbox(owner){}
BasicHitbox::~BasicHitbox(){}

Hitbox::Type BasicHitbox::getType(){
	return Type::BASIC;
}

bool BasicHitbox::resolveCollision(float dt, Entity* e){
	if(e->hitbox->getType() == Hitbox::Type::BASIC){
		BasicHitbox he = *std::dynamic_pointer_cast<BasicHitbox>(e->hitbox);
		if(owner->pos.y + height > e->pos.y && owner->pos.y < e->pos.y + he.height){
			vec2f pt = vec2f{owner->pos.x, owner->pos.z};
			vec2f pe = vec2f{e->pos.x, e->pos.z};
			float r = radius+he.radius;
			if((pe-pt).length2() < r*r-0.01f){
				owner->pos = e->pos + (owner->pos - e->pos).normalized() * r;
				//owner->pos = owner->oldPos;
				return true;
			}
		}
	}
	else if(e->hitbox->getType() == Hitbox::Type::V_PLANE){
		VerticalPlaneHitbox he = *std::dynamic_pointer_cast<VerticalPlaneHitbox>(e->hitbox);
		if(owner->pos.y+height < e->pos.y || owner->pos.y > e->pos.y+he.getDelta().y) return false;

		vec3f planeDir = ((he.getDelta()) * vec3f{1.0f, 0.0f, 1.0f}).normalized();

		float d = planeDir.dot(owner->pos);
		if(planeDir.dot(e->pos) < d+radius && d-radius < planeDir.dot(e->pos+he.getDelta())){ // Entity is in front of plane
			if(he.getNormal().dot(owner->pos) > he.getNormal().dot(e->pos) &&
				he.getNormal().dot(owner->pos) < he.getNormal().dot(e->pos+he.getNormal()*radius)){

				owner->pos -= he.getNormal() * (owner->pos-(e->pos + he.getNormal() * radius)).dot(he.getNormal());
				return true;
			}
			if(he.getNormal().dot(owner->pos) < he.getNormal().dot(e->pos) &&
				he.getNormal().dot(owner->pos) > he.getNormal().dot(e->pos-he.getNormal()*radius)){

				owner->pos -= he.getNormal() * (owner->pos-(e->pos - he.getNormal() * radius)).dot(he.getNormal());
				return true;
			}
		}
		return false;
	}
	else if(e->hitbox->getType() == Hitbox::Type::H_PLANE){
		HorizontalPlaneHitbox he = *std::dynamic_pointer_cast<HorizontalPlaneHitbox>(e->hitbox);
		if(
			(owner->pos - e->pos).dot(he.getSide1()) < 0 || he.getSide1().dot(owner->pos) > he.getSide1().dot(e->pos+he.getSide1()) ||
			(owner->pos - e->pos).dot(he.getSide2()) < 0 || he.getSide2().dot(owner->pos) > he.getSide2().dot(e->pos+he.getSide2())
		) return false;

/*		if(
			(owner->oldPos.y+height/2.0f > e->pos.y && owner->pos.y < e->pos.y) ||
			(owner->oldPos.y+height/2.0f < e->pos.y && owner->pos.y > e->pos.y)
		){*/
		if(owner->pos.y < e->pos.y){
			owner->pos.y = e->pos.y;
			owner->onGround = true;
			owner->brake(dt, owner->frictionCoeff);
			return true;
		}

		return false;
	}
	return false;
}


VerticalPlaneHitbox::VerticalPlaneHitbox(Entity* owner) : Hitbox(owner){}
VerticalPlaneHitbox::~VerticalPlaneHitbox(){}

vec3f VerticalPlaneHitbox::getDelta(){
	return delta;
}
void VerticalPlaneHitbox::setDelta(vec3f delta){
	this->delta = delta;
	cachedNormal = delta.cross(vec3f::Y).normalized();
}
vec3f VerticalPlaneHitbox::getNormal(){
	return cachedNormal;
}

VerticalPlaneHitbox::Type VerticalPlaneHitbox::getType(){
	return Type::V_PLANE;
}

bool VerticalPlaneHitbox::resolveCollision(float dt, Entity* e){
	if(e->hitbox->getType() == Hitbox::Type::BASIC){
		return e->hitbox->resolveCollision(dt, owner);
	}
	return false;
}


HorizontalPlaneHitbox::HorizontalPlaneHitbox(Entity* owner) : Hitbox(owner){}
HorizontalPlaneHitbox::~HorizontalPlaneHitbox(){}

void HorizontalPlaneHitbox::setSize(vec3f side, float width){
	side1 = side;
	this->width = width;
	side2 = -side1.cross(vec3f::Y) * width/side1.length();
}

vec3f HorizontalPlaneHitbox::getSide1(){
	return side1;
}

vec3f HorizontalPlaneHitbox::getSide2(){
	return side2;
}

Hitbox::Type HorizontalPlaneHitbox::getType(){
	return Type::H_PLANE;
}

bool HorizontalPlaneHitbox::resolveCollision(float dt, Entity* e){
	if(e->hitbox->getType() == Hitbox::Type::BASIC){
		return e->hitbox->resolveCollision(dt, owner);
	}
	return false;
}


Entity::Entity(){
	auto hb = std::make_shared<BasicHitbox>(this);
	hb->radius = 0.5f;
	hb->height = 1.7f;
	hitbox = hb;
}

Entity::Entity(vec3f pos){
	this->pos = pos;
	this->oldPos = pos;

	auto hb = std::make_shared<BasicHitbox>(this);
	hb->radius = 0.5f;
	hb->height = 1.7f;
	hitbox = hb;
}

Entity::~Entity(){

}

void Entity::setPos(float x, float z) {
	float h = World::getWorld().getHeightAt(x, z);
	pos = {x, h, z};
	oldPos = pos;
}


vec3f Entity::getCenterPos(){
	return pos + vec3f::Y * 0/2.0f; // 0 was height
}

vec3f Entity::getEyePos(){
	return pos + vec3f::Y * eyeHeight;
}
vec3f Entity::getLookDir(){
	return vec3f{std::sin(rotation.y)*std::cos(rotation.x), -std::sin(rotation.x), -std::cos(rotation.y)*std::cos(rotation.x)};
}

void Entity::tick(float dt){
	if(entityAI != nullptr) entityAI->tick(dt);
	onGround = false;

	/*vec3f dp = pos - oldPos;
	oldPos = pos;
	pos = pos + dp + acc * dt*dt;

	acc = {0.0f, 0.0f, 0.0f};*/
	VerletPoint::tick(dt);

	/*float h = World::getWorld().getHeightAt(pos.x, pos.z);
	if(pos.y <= h){
		onGround = true;
		acc -= (pos-oldPos) * (1/dt) * vec3f{1,0,1} * 4.0f;

		//oldPos.y = h - (oldPos.y-pos.y)*0.5f; // To add bounce

		pos.y = h;

		//vel.y *= 0.9f;
		//force += vec3f::Y * 2*(h-pos.y)*(h-pos.y);
		//force.y += mass * 9.82;
	}*/
	interactCooldown -= dt;
}

void Entity::tryMove(float dt, float dx, float dz){
	if(onGround){
		Matrix4x4 rotMat = Matrix4x4::rotationY(rotation.y);
		acc += rotMat * vec3f{dx, 0.0f, dz} * acceleration;
	}
}

void Entity::tryHurt(std::shared_ptr<Entity> e, float hp) {
	if(interactCooldown <= 0.0f){
		e->hurt(hp);
		interactCooldown = 0.4f;
	}
}


void Entity::tryJump(float dt){
	if(onGround) acc.y += std::sqrt(2.0f*9.82f*1.0f)/dt; // Force requred to jump one meter.
}




/*void Entity::brake(float dt){
	acc -= (pos-oldPos) * (1/dt) * vec3f{1,0,1} * 8.0f;
}*/

bool Entity::resolveCollision(float dt, World& world){
	float h = world.getHeightAt(pos.x, pos.z);
	if(pos.y <= h){
		onGround = true;
		brake(dt, frictionCoeff);

		vec3f n = world.getNormalAt(pos.x, pos.z);
		vec3f p = world.getPosAt(pos.x, pos.z);
		float d = p.dot(n);
		float a = d - pos.dot(n);
		pos += a*n;
		return true;
	}
	return false;
}

bool Entity::resolveCollision(float dt, VerticalPlane& plane){
	float height = 1.0f;
	float radius = 0.25f;
	if(pos.y+height < plane.p1.y || pos.y > plane.p2.y) return false;

	vec3f planeDir = ((plane.p2-plane.p1) * vec3f{1.0f, 0.0f, 1.0f}).normalized();

	float d = planeDir.dot(pos);
	if(planeDir.dot(plane.p1) < d+radius && d-radius < planeDir.dot(plane.p2)){ // Entity is in front of plane
		if(plane.normal.dot(pos) > plane.normal.dot(plane.p1) &&
			plane.normal.dot(pos) < plane.normal.dot(plane.p1+plane.normal*radius))
		{
			pos -= plane.normal * (pos-(plane.p1 + plane.normal * radius)).dot(plane.normal);
			return true;
		}
		if(plane.normal.dot(pos) < plane.normal.dot(plane.p1) &&
			plane.normal.dot(pos) > plane.normal.dot(plane.p1-plane.normal*radius))
		{
			pos -= plane.normal * (pos-(plane.p1 - plane.normal * radius)).dot(plane.normal);
			return true;
		}
	}
	return false;
}

bool Entity::resolveCollision(float dt, HorizontalPlane& plane){
	float height = 1.0f;
	if(
		(pos-plane.p).dot(plane.v1) < 0 || plane.v1.dot(pos) > plane.v1.dot(plane.p+plane.v1) ||
		(pos-plane.p).dot(plane.v2) < 0 || plane.v2.dot(pos) > plane.v2.dot(plane.p+plane.v2)
	) return false;

	if(
		(oldPos.y+height/2.0f > plane.p.y && pos.y < plane.p.y) ||
		(oldPos.y+height/2.0f < plane.p.y && pos.y > plane.p.y)
	){
		pos.y = plane.p.y;
		onGround = true;
		brake(dt, frictionCoeff);
		return true;
	}

	return false;
}

bool Entity::resolveCollision(float dt, std::shared_ptr<Entity> e){
	return hitbox->resolveCollision(dt, e.get());
}

bool Entity::interactWith(std::shared_ptr<Entity> e, ItemStack& itemStack){
	if(e == nullptr){
		if(itemStack.type == -1) return false;
		float yLookDir = getLookDir().y;
		vec3f hitPoint = getEyePos() + (getLookDir() * -eyeHeight/yLookDir);
		hitPoint.x = std::round(hitPoint.x);
		hitPoint.z = std::round(hitPoint.z);

		if(itemStack.getPropertyLevel(Items::getPropertyId("Soil Digging")) > 0){
			if((hitPoint-pos).length() < 2.0f){
				hitPoint.y = World::getWorld().getHeightAt(hitPoint.x, hitPoint.z);
				World::getWorld().setHeightAt(hitPoint.x, hitPoint.z, std::floor(hitPoint.y-0.001f));
				inventory.give(ItemStack(Items::getItemType("Soil"), 0, 100));
				return true;
			}
		}
		else if(itemStack.type == Items::getItemType("Soil") && itemStack.count >= 100){
			if((hitPoint-pos).length() < 2.0f){
				hitPoint.y = World::getWorld().getHeightAt(hitPoint.x, hitPoint.z);
				World::getWorld().setHeightAt(hitPoint.x, hitPoint.z, std::ceil(hitPoint.y+0.001f));
				itemStack.count -= 100;
				return true;
			}
		}
		else if(itemStack.type == Items::getItemType("Wood Slab") && itemStack.count > 0){
			if((hitPoint-pos).length() < 2.0f){
				hitPoint.y = World::getWorld().getHeightAt(hitPoint.x, hitPoint.z);
				/*World::getWorld().hPlanes.push_back(std::make_shared<HorizontalPlane>(
					hitPoint,
					vec3f{5.0f, 0.0f, 0.0f},
					-5.0f
				));*/
				std::shared_ptr<Entity> plane = std::make_shared<Entity>(hitPoint);
				auto hb = std::make_shared<HorizontalPlaneHitbox>(plane.get());
				hb->setSize(vec3f::X * 5.0f, -5.0f);
				plane->hitbox = hb;
				plane->d.render = 2;
				plane->d.hitable = true;
				plane->create();
				//std::dynamic_pointer_cast<MeshEntityRenderer>(plane->entityRenderer)->mesh.add(Mesh::createXZPlane(vec3f::origo, {5.0f, 5.0f}).translate(hitPoint));
				auto renderer = std::make_shared<MeshEntityRenderer>(plane.get());
				renderer->mesh.add(Mesh::createXZPlane(vec3f::origo, {5.0f, 5.0f}).setTexture(3));
				plane->entityRenderer = renderer;
				plane->apply();
				World::getWorld().addEntity(plane);
				itemStack.count--;
			}
		}
		else if(itemStack.type == Items::getItemType("Rock")){
			auto f = std::make_shared<Entity>();
			f->item = itemStack;
			auto renderer = std::make_shared<MeshEntityRenderer>(f.get());
			renderer->mesh = Items::getMesh(itemStack.type);
			f->entityRenderer = renderer;
			auto hb = std::dynamic_pointer_cast<BasicHitbox>(f->hitbox);
			hb->radius = 0.1f;
			hb->height = 0.1f;
			f->pos = getEyePos() + getLookDir() * 0.99f;
			f->oldPos = f->pos + getLookDir() * -0.2f;
			World::getWorld().addEntity(f);
			itemStack.count--;
			return true;
		}
	}
	else if(itemStack.type == -1){
		inventory.give(e->item);
		World::getWorld().remove(e);
		return true;
	}
	else if(e->item.type == Items::getItemType("entity_tree")){
		if(itemStack.getPropertyLevel(Items::getPropertyId("Wood Cutting")) > 0){
			World::getWorld().remove(e);
			inventory.give(ItemStack(Items::getItemType("Wood"), 0, 10));
			/*e->pos.y += 0.1f;
			//util::log("Chop");
			e->apply();
			//World::getWorld().getChunk(e->pos.x, e->pos.z)->;*/
			return true;
		}
	}
	return false;
}

void Entity::create(){
	//if(d.render > 0 && !mesh.hasCreated()) mesh.create();
	if(entityRenderer != nullptr) entityRenderer->create();
}
void Entity::apply(){
	if(d.render > 0){
		if(entityRenderer != nullptr) entityRenderer->apply();
		/*if(!mesh.hasCreated()) mesh.create();
		mesh.apply();*/
	}
	if(d.irIndex != -1){
		InstancedRenderableMesh& m = d.chunk->detailCollections[d.levelOfDetail].getDescriber(d.detailIndex).irMesh;
		m.instanceData[d.irIndex].pos = pos;
		m.apply();
	}
}
void Entity::render(Renderer& r){
	/*if(d.render > 0){
		if(!mesh.hasApplied()) apply();
		mesh.render(r);
	}*/
	if(entityRenderer != nullptr) entityRenderer->render(r);
}

void Entity::setInstanceRender(int detailIndex, int levelOfDetail, const Instance& instance){
	d.levelOfDetail = levelOfDetail;
	d.chunk->detailCollections[levelOfDetail].getDescriber(detailIndex).irMesh.instanceData.push_back(instance);
	d.detailIndex = detailIndex;
	d.irIndex = d.chunk->detailCollections[levelOfDetail].getDescriber(detailIndex).irMesh.instanceData.size()-1;
}

void Entity::hurt(float hp) {
	this->hp -= hp;
	if(this->hp <= 0.0f){
		this->hp = 20.0f;
		setPos(15, 15);
	}
}





void StupidAI::tick(float dt){
	static float t = 0.0f;
	if((int)(t/10) % 2 == 0){
		owner->tryMove(dt, 0.0f, -1.0f);
	}
	t += dt;
}

void ChaserAI::tick(float dt){
	vec3f targetPos = World::getWorld().player->pos;
	vec3f delta = targetPos - owner->pos;
	delta.y = 0;

	if(delta.length() < 1){
		owner->tryHurt(World::getWorld().player, 2.0f);
	}

	delta.normalize();

	static int t = 0;
	if(t % 60*20 < 60){
		//owner->tryJump(dt);
		owner->tryMove(dt, delta.z, delta.x);
	}
	else{
		owner->tryMove(dt, delta.x, delta.z);
	}
	t++;
}

EntityRenderer::EntityRenderer(Entity* owner) : owner(owner){}

void MeshEntityRenderer::create(){
	mesh.create();
}
void MeshEntityRenderer::apply(){
	if(!mesh.hasCreated()) create();
	mesh.apply();
}
void MeshEntityRenderer::render(Renderer& r){
	if(!mesh.hasApplied()) apply();
	r.setTranslation(owner->pos);
	mesh.render(r);
	r.setTranslation(vec3f::origo);
}

void AnimationEntityRenderer::create(){
	animation.create();
}
void AnimationEntityRenderer::apply(){
	animation.apply();
}
void AnimationEntityRenderer::render(Renderer& r){
	animation.pos = owner->pos;
	animation.render(r);
}






