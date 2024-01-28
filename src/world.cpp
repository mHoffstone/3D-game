#include "world.h"

#include "entity.h"
#include "player.h"

#include <cmath>
#include <algorithm>
#include <mutex>
#include <thread>

static std::mutex mtx;
static std::thread::id lockingThread;
static World world;

World::World(){
	Vegetation::instance().addTreeType();
}

float World::getHeightAt(float x, float y){
	return getChunk(x, y)->grid.getHeightAt(
		x - std::floor(x/Chunk::SIZE) * Chunk::SIZE,
		y - std::floor(y/Chunk::SIZE) * Chunk::SIZE);

	//return getChunk(x, y)->grid.getHeightAt((int)x % Chunk::SIZE, (int)y % Chunk::SIZE);

}

void World::setHeightAt(int x, int y, float height){
	float xr = x - std::floor(x/Chunk::SIZE) * Chunk::SIZE;
	float yr = y - std::floor(y/Chunk::SIZE) * Chunk::SIZE;

	getChunk(x, y)->grid.setHeightAt(xr, yr, height);

	if(x % Chunk::SIZE == 0){
		getChunk(x-1, y)->grid.setHeightAt(xr+Chunk::SIZE, yr, height);
	}
	if(y % Chunk::SIZE == 0){
		getChunk(x, y-1)->grid.setHeightAt(xr, yr+Chunk::SIZE, height);
	}
	if(x % Chunk::SIZE == 0 && y % Chunk::SIZE == 0){
		getChunk(x-1, y-1)->grid.setHeightAt(xr+Chunk::SIZE, yr+Chunk::SIZE, height);
	}
}

vec3f World::getNormalAt(float x, float y) {
	//return getChunk(x, y)->grid.getNormalAt(std::fmod(x, Chunk::SIZE), std::fmod(y, Chunk::SIZE));
	return getChunk(x, y)->grid.getNormalAt((int)x % Chunk::SIZE, (int)y % Chunk::SIZE);
}

vec3f World::getPosAt(float x, float y) {
	return vec3f{x, getHeightAt(x, y), y};
}



void World::tick(float dt){
	mtx.lock();
	lockingThread = std::this_thread::get_id();
	//for(auto& e : entities){
	for(auto it = entities.begin(); it != entities.end(); it++){
		auto& e = *it;
		e->acc.y -= 9.82f;
		e->tick(dt);
		e->resolveCollision(dt, *this);
		for(auto& a : hPlanes) e->resolveCollision(dt, *a);
		for(auto& a : vPlanes) e->resolveCollision(dt, *a);
		//for(auto& e : entities){
			std::array<Chunk*, 4> closeChunks;
			int dx, dz;
			if(e->pos.x-std::floor(e->pos.x) > 0.5f) dx = 1;
			else dx = -1;
			if(e->pos.z-std::floor(e->pos.z) > 0.5f) dz = 1;
			else dz = -1;
			closeChunks[0] = getChunk(e->pos.x, e->pos.z);
			closeChunks[1] = getChunk(e->pos.x+dx, e->pos.z);
			closeChunks[2] = getChunk(e->pos.x, e->pos.z+dz);
			closeChunks[3] = getChunk(e->pos.x+dx, e->pos.z+dz);
			for(Chunk* c : closeChunks){
				for(auto& f : c->hitableEntities){
					e->resolveCollision(dt, f);
				}
			}
			if(entities.size() >= 2 && it != entities.end()-1) for(auto jt = it+1; jt != entities.end(); jt++){
				e->resolveCollision(dt, *jt);
			}
		//}
	}
	for(auto* c : chunks){
		c->tick(dt);
	}
	//Tree::treeMesh.apply();
	lockingThread = std::thread::id();
	mtx.unlock();
}

/*void World::popGrass(){
	for(auto c : chunks){
		for(int i = 0; i < 120; i++){
			c->entities.pop_back();
			c->grassMesh.instanceData.pop_back();
		}
		c->grassMesh.apply();
	}
}*/

void World::addEntity(std::shared_ptr<Entity> e){
	if(e->d.chunk == nullptr){
		entities.push_back(e);
	}
	else{
		e->d.chunk->addEntity(e);
	}
}


std::shared_ptr<Entity> World::getEntityFrom(std::shared_ptr<Entity> e){
	return getEntityFrom(e->getEyePos(), e->getLookDir(), 2.0f);
}

std::shared_ptr<Entity> World::getEntityFrom(vec3f pos, vec3f dir, float maxDist){
	for(auto& c : cubes){
		vec3f delta = c->getCenterPos() - pos;
		if(dir.dot(delta.normalized()) > 0.9f && delta.length2() < maxDist*maxDist){
			return std::static_pointer_cast<Entity>(c);
		}
	}
	std::array<Chunk*, 4> closeChunks;
	int dx, dz;
	if(pos.x-std::floor(pos.x) > 0.5f) dx = 1;
	else dx = -1;
	if(pos.z-std::floor(pos.z) > 0.5f) dz = 1;
	else dz = -1;
	closeChunks[0] = getChunk(pos.x, pos.z);
	closeChunks[1] = getChunk(pos.x+dx, pos.z);
	closeChunks[2] = getChunk(pos.x, pos.z+dz);
	closeChunks[3] = getChunk(pos.x+dx, pos.z+dz);

	for(auto c : closeChunks){
		for(auto& e : c->entities){
			vec3f delta = e->getCenterPos() - pos;
			if(dir.dot(delta.normalized()) > 0.9f && delta.length2() < maxDist*maxDist){
				// Do NOT uncomment
				/*e->d.chunk = c;
				if(e->item.type == Items::getItemType("Grass")) e->d.irMesh = &c->grassMesh;
				else if(e->item.type == Items::getItemType("Rock")) e->d.irMesh = &c->rockMesh;
				else if(e->item.type == Items::getItemType("Blue Flower")) e->d.irMesh = &c->flower1Mesh;
				else e->d.render = 1;*/
				return e;
			}
		}
	}
	return nullptr;
}

void World::remove(std::shared_ptr<Entity> e){
	if(e->d.chunk == nullptr){
		entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
	}
	else{
		e->d.chunk->remove(e);
	}
}




void World::render(Renderer& r){
	mtx.lock();
	vec3f player3dPos = player->pos;
	vec2f pos = {player3dPos.x, player3dPos.z};
	for(auto c : chunks){
		c->render(r, (c->getPos()+vec2f{Chunk::SIZE/2.0f, Chunk::SIZE/2.0f}-pos).length2() < 160*160);
	}
	for(auto& e : entities){
		e->render(r);
	}
	mtx.unlock();
}

Chunk* World::getChunk(float x, float y){
	for(auto c : chunks){
		if(x >= c->pos.x && x < c->pos.x + Chunk::SIZE &&
		   y >= c->pos.y && y < c->pos.y + Chunk::SIZE){

			return c;
		}
	}
	Chunk* c = new Chunk(x, y);
	c->generateGround();

	if(lockingThread != std::this_thread::get_id()) mtx.lock();
	chunks.push_back(c);
	if(lockingThread != std::this_thread::get_id()) mtx.unlock();
	return c;
}

void World::generateGround(float x, float y){
	getChunk(x, y);
}
void World::generateDetails(float x, float y, int levelOfDetails){
	getChunk(x, y)->generateDetails(levelOfDetails);
}


void World::generate(float x, float y){
	getChunk(x, y);
	return;

#if 0
	/*Chunk* c = new Chunk;
	c->pos.x = floor(x/Chunk::SIZE) * Chunk::SIZE;
	c->pos.y = floor(y/Chunk::SIZE) * Chunk::SIZE;
	c->grid = Grid(c->pos.x, c->pos.y, Chunk::SIZE, Chunk::SIZE);*/
	Chunk* c = getChunk(x, y);

	/*for(int i = 0; i < 3; i++){
		float rx = rand() % Chunk::SIZE;
		float ry = rand() % Chunk::SIZE;
		float ax = rx+c->pos.x;
		float ay = ry+c->pos.y;
		Mesh treeCopy = vegetation.generateTree(0).getMesh();
		treeCopy.translate(vec3f{ax, c->grid.getHeightAt(rx,ry), ay});
		c->staticMesh.add(treeCopy);
	}*/
	for(int i = 0; i < 1; i++){
		float rx = rand() % Chunk::SIZE;
		float ry = rand() % Chunk::SIZE;
		float ax = rx+c->pos.x;
		float ay = ry+c->pos.y;
		float h = c->grid.getHeightAt(rx, ry);
		if(h < 0.0f) continue;
		static int treeId = 0;
		std::shared_ptr<Entity> e = std::make_shared<Entity>(vec3f{ax, h, ay});
		e->item.type = Items::getItemType("entity_tree");
		e->d.hitable = true;
		e->setInstanceRender(&c->treeMesh, Instance{e->pos, Matrix4x4::rotationYX(0.0f, 0.0f).elements});
		c->addEntity(e);
		//c->treeMesh.instanceData.push_back(Instance{e->pos, Matrix4x4::rotationYX(0.0f, 0.0f).elements});
		treeId++;
	}

	c->grassMesh.add(Items::getMesh(Items::getItemType("Grass")));
	//for(int i = 0; i < 10000; i++){
	for(int i = 0; i < 1000; i++){
		float rx = rand() % 1000 / 1000.0f * Chunk::SIZE;
		float ry = rand() % 1000 / 1000.0f * Chunk::SIZE;
		float ax = rx+c->pos.x;
		float ay = ry+c->pos.y;
		float h = c->grid.getHeightAt(rx, ry);
		if(h < 0.0f) continue;
		vec3f grassPos = vec3f{ax, h, ay};
		auto e = std::make_shared<Entity>(grassPos);
		e->item = Item{Items::getItemType("Grass"), 0};
		//e->instanceIndex = c->grassMesh.instanceData.size();

		vec3f f2 = c->grid.getNormalAt(rx, ry);
		vec3f f1 = vec3f::X;
		f1 = f1 - f2*f1.dot(f2);
		f1.normalize();
		vec3f f3 = f1.cross(f2);

		e->setInstanceRender(&c->grassMesh, Instance{
			grassPos,
			//Matrix4x4::identity().elements
			(Matrix4x4::lookAt(f1, f2, f3)*Matrix4x4::rotationY(rand()*1000/1000.0f * 2*PI)).elements
		});
		c->addEntity(e);

		/*vec3f n = Matrix4x4::rotationZ(PI/2.0f) * c->grid.getNormalAt(rx, ry);
		vec2f angle = n.angle();
		e->setInstanceRender(&c->grassMesh, Instance{
			grassPos,
			//(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.5f)).elements
			(Matrix4x4::rotationYX(-angle.x, angle.y)*Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.5f)).elements
		});
		c->addEntity(e);*/
	}

	c->reedsMesh.add(Items::getMesh(Items::getItemType("Reeds")));
	for(int i = 0; i < 1000; i++){
		float rx = rand() % 1000 / 1000.0f * Chunk::SIZE;
		float ry = rand() % 1000 / 1000.0f * Chunk::SIZE;
		//float rx = 0.0f, ry = 0.0f;
		float ax = rx+c->pos.x;
		float ay = ry+c->pos.y;
		float h = c->grid.getHeightAt(rx, ry);
		if(h < -1.0f || h > 1.0f) continue;
		float density = c->grid.density[(int(std::round(ry))*(Chunk::SIZE+1)+int(std::round(rx)))];
		if(rand() % 1000 > density*density * 1000) continue;
		vec3f pos = vec3f{ax, h, ay};
		auto e = std::make_shared<Entity>(pos);
		e->item = Item{Items::getItemType("Reeds"), 0};
		//e->instanceIndex = c->grassMesh.instanceData.size();

		e->setInstanceRender(&c->reedsMesh, Instance{
			pos,
			//(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.5f)).elements
			(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/2000.0f+0.75f)).elements
		});
		c->addEntity(e);
	}

	c->rockMesh.add(Items::getMesh(Items::getItemType("Rock")));
	if(rand() % 3 == 0){
		vec2f rcenter = {Chunk::SIZE/2.0f, Chunk::SIZE/2.0f};
		for(int i = 0; i < 50; i++){
			float angle = rand()%1000/1000.0f * 2*PI;
			float rad = (rand()%1000/1000.0f) * (rand()%1000/1000.0f) * Chunk::SIZE/2.0f;
			float rx = rcenter.x + std::cos(angle)*rad;
			float ry = rcenter.y + std::sin(angle)*rad;
			float ax = rx+c->pos.x;
			float ay = ry+c->pos.y;
			float h = c->grid.getHeightAt(rx, ry);
			//if(h < 0.0f) continue;
			vec3f pos = vec3f{ax, h, ay};
			auto e = std::make_shared<Entity>(pos);
			e->item = Item{Items::getItemType("Rock"), rand()%2};
			e->setInstanceRender(&c->rockMesh, Instance{
				pos,
				(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.25f)).elements
			});
			//e->instanceIndex = c->rockMesh.instanceData.size();
			c->addEntity(e);
			/*c->rockMesh.instanceData.push_back(
				Instance{
					pos,
					(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.25f)).elements
				}
			);*/
		}
	}

	c->flower1Mesh.add(Items::getMesh(Items::getItemType("Blue Flower")));
	if(rand() % 3 == 0){
		vec2f rcenter = {Chunk::SIZE/2.0f, Chunk::SIZE/2.0f};
		for(int i = 0; i < 50; i++){
			float angle = rand()%1000/1000.0f * 2*PI;
			float rad = (rand()%1000/1000.0f) * (rand()%1000/1000.0f) * Chunk::SIZE/2.0f;
			float rx = rcenter.x + std::cos(angle)*rad;
			float ry = rcenter.y + std::sin(angle)*rad;
			float ax = rx+c->pos.x;
			float ay = ry+c->pos.y;
			float h = c->grid.getHeightAt(rx, ry);
			if(h < 0.0f) continue;
			vec3f pos = vec3f{ax, h+0.5f, ay};
			auto e = std::make_shared<Entity>(pos);
			e->item = Item{Items::getItemType("Blue Flower"), 0};
			e->setInstanceRender(&c->flower1Mesh, Instance{
				pos,
				(Matrix4x4::rotationY(rand()%1000/1000.0f * 2*PI)*Matrix4x4::scale(rand()%1000/4000.0f+0.5f)).elements
			});
			c->addEntity(e);
			/*auto obj = std::make_shared<Herb>(1, pos);
			obj->instanceIndex = c->flower1Mesh.instanceData.size();
			c->worldObjects.push_back(obj);*/
		}
	}

	{
		float rx = rand() % 1000 / 1000.0f * Chunk::SIZE;
		float ry = rand() % 1000 / 1000.0f * Chunk::SIZE;
		float ax = rx+c->pos.x;
		float ay = ry+c->pos.y;
		auto e = std::make_shared<Entity>(vec3f{ax, c->grid.getHeightAt(rx,ry), ay});
		e->item = Item{Items::getItemType("Stick"), 0};
		e->mesh = Items::getMesh(e->item.type).translate(e->pos);
		e->mesh.create();
		e->mesh.apply();
		e->d.render = 1;
		c->addEntity(e);
	}
	if(rand() % 5 == 0){
		int count = rand() % 5 + 4;
		for(int i = 0; i < count; i++){
			float rx = rand() % 1000 / 1000.0f * Chunk::SIZE;
			float ry = rand() % 1000 / 1000.0f * Chunk::SIZE;
			float ax = rx+c->pos.x;
			float ay = ry+c->pos.y;
			auto e = std::make_shared<Entity>(vec3f{ax, c->grid.getHeightAt(rx,ry), ay});
			e->item = Item{Items::getItemType("Fire Blossom"), 0};
			e->mesh = Items::getMesh(e->item.type).translate(e->pos);
			e->mesh.create();
			e->mesh.apply();
			e->d.render = 1;
			c->addEntity(e);
		}
	}


	//util::log("New chunk!");
	//return c;
#endif
}

World& World::getWorld(){
	return world;
}

