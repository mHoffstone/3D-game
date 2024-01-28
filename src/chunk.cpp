#include "chunk.h"

#include "vegetation.h"
#include "world.h"
#include "noisemap.h"

#include <algorithm>
#include <cmath>

Chunk::Chunk(float x, float y){
	pos.x = std::floor(x/Chunk::SIZE) * Chunk::SIZE;
	pos.y = std::floor(y/Chunk::SIZE) * Chunk::SIZE;
}

Chunk::~Chunk(){

}

void Chunk::create(){
	grid.create();
	treeMesh.create();
	treeMesh.add(Vegetation::instance().generateTree(0));
	// Note: Entities created in generateDetails might never get created
	for(auto& a : entities){
		a->create();
	}
	hasCreated = true;
}

void Chunk::apply(){
	if(!hasCreated) create();
	treeMesh.apply();
	grid.apply();
	detailCollections[0].apply();
	detailCollections[1].apply();
	for(auto& a : entities){
		a->apply();
	}
	hasApplied = true;
}

void Chunk::generateGround(){
	grid.generate(pos.x, pos.y, Chunk::SIZE, Chunk::SIZE);
}

void Chunk::applyDetails(const DetailCollection& detailCollection){
	for(int i = 0; i < detailCollection.count(); i++){
		auto& d = detailCollection.getDescriber(i);
		for(int j = 0; j < d.tries; j++){
			float rx = rand() % 1000 / 1000.0f * Chunk::SIZE;
			float ry = rand() % 1000 / 1000.0f * Chunk::SIZE;
			float ax = rx+pos.x;
			float ay = ry+pos.y;
			float h = grid.getHeightAt(rx, ry);
			if(h < d.heightMin || h > d.heightMax) continue;

			if(d.densityIndex >= 0){
				float density = detailCollection.getNoiseValue(rx, ry, d.densityIndex);
				//density *= density;
				//density *= density;
				//density *= density;
				//if(rand() % 1000 > density * 1000) continue;
				if(density < d.threshold) continue;
				if(rand() % 1000 > (density-d.threshold)/(1-d.threshold) * 1000) continue;
			}

			vec3f pos = vec3f{ax, h, ay};

			if(d.instanceRendered){
				//auto e = std::make_shared<Entity>(pos);
				auto e = std::make_shared<Entity>();
				if(d.entityTemplate != nullptr)
					*e = *d.entityTemplate;
				e->pos = pos;
				e->oldPos = pos;
				if(e->item.type == -1)
					e->item = d.items[rand() % d.items.size()];
				addEntity(e);
				//e->instanceIndex = c->grassMesh.instanceData.size();

				vec3f f1 = vec3f::X, f2 = vec3f::Y, f3 = vec3f::Z;
				if(d.followNormal){
					f2 = grid.getNormalAt(rx, ry);
					f1 = f1 - f2*f1.dot(f2);
					f1.normalize();
					f3 = f1.cross(f2);
				}

				e->setInstanceRender(i, d.levelOfDetail, Instance{
					pos,
					(Matrix4x4::lookAt(f1, f2, f3)*Matrix4x4::rotationY(rand()*1000/1000.0f * 2*PI)).elements
				});
			}
			else{
				auto e = std::make_shared<Entity>();
				*e = *d.entityTemplate;
				e->pos = pos;
				e->oldPos = pos;
				auto meshRenderer = std::make_shared<MeshEntityRenderer>(e.get());
				//auto meshRenderer = std::dynamic_pointer_cast<MeshEntityRenderer>(e->entityRenderer);
				meshRenderer->mesh = d.mesh;
				e->entityRenderer = meshRenderer;
				addEntity(e);
			}
		}
	}
}

void Chunk::generateDetails(int levelOfDetail){
	if(detailsGenerated[levelOfDetail]) return;
	detailsGenerated[levelOfDetail] = true;

	if(levelOfDetail == 1){
		DetailCollection& detailCollection = detailCollections[1];
		detailCollection.addNoiseMap(NoiseMap::generateMap(pos.x, pos.y, SIZE, SIZE, 20, 0));
		detailCollection.addNoiseMap(NoiseMap::power(NoiseMap::generateMap(pos.x, pos.y, SIZE, SIZE, 20, 1), SIZE, SIZE, 6));
		{
			DetailDescriber d;
			d.tries = 4000;
			d.followNormal = true;
			d.irMesh.add(Items::getMesh(Items::getItemType("Grass")));
			d.items = {Item{Items::getItemType("Grass"), 0}};
			detailCollection.addDetailType(d);
		}
		{
			DetailDescriber d;
			d.tries = 4;
			d.followNormal = true;
			d.irMesh.add(Items::getMesh(Items::getItemType("Fire Blossom")));
			d.items = {Item{Items::getItemType("Fire Blossom"), 0}};
			detailCollection.addDetailType(d);
		}
		{
			DetailDescriber d;
			d.tries = 1000;
			d.irMesh.add(Items::getMesh(Items::getItemType("Blue Flower")));
			d.items = {Item{Items::getItemType("Blue Flower"), 0}};
			d.densityIndex = 0;
			d.threshold = 0.5f;
			detailCollection.addDetailType(d);
		}
		{
			DetailDescriber d;
			d.tries = 50;
			d.irMesh.add(Items::getMesh(Items::getItemType("Rock")));
			d.items = {Item{Items::getItemType("Rock"), 0}, Item{Items::getItemType("Rock"), 1}};
			d.heightMin = -99999.0f;
			d.densityIndex = 1;
			d.threshold = 0.001f;
			detailCollection.addDetailType(d);
		}
		{
			DetailDescriber d;
			d.tries = 1000;
			d.irMesh.add(Items::getMesh(Items::getItemType("Reeds")));
			d.items = {Item{Items::getItemType("Reeds"), 0}};
			d.heightMin = -1.0f;
			d.heightMax =  1.0f;
			d.threshold = 0.1f;
			d.densityIndex = 0;
			detailCollection.addDetailType(d);
		}
		{
			DetailDescriber d;
			d.tries = 10;
			d.heightMin = -99999.0f;
			d.instanceRendered = false;
			d.mesh = Items::getMesh(Items::getItemType("Stick"));
			auto e = std::make_shared<Entity>();
			e->item = Item{Items::getItemType("Stick"), 0};
			e->d.render = 1;
			d.entityTemplate = e;
			detailCollection.addDetailType(d);
		}
	}
	else if(levelOfDetail == 0){
		DetailCollection& detailCollection = detailCollections[0];
		detailCollection.addNoiseMap(NoiseMap::generateMap(pos.x, pos.y, SIZE, SIZE, 200, 1));
		{
			DetailDescriber d;
			d.levelOfDetail = 0;
			d.tries = 20;
			d.followNormal = false;
			d.irMesh.add(Vegetation::instance().generateTree(0));
			d.items = {Item{Items::getItemType("entity_tree"), 0}};
			d.threshold = 0.5f;
			d.densityIndex = 0;
			auto e = std::make_shared<Entity>();
			e->item = Item{Items::getItemType("entity_tree"), 0};
			e->d.hitable = true;
			d.entityTemplate = e;
			detailCollection.addDetailType(d);
		}
	}
	applyDetails(detailCollections[levelOfDetail]);

}

vec2f Chunk::getPos(){
	return pos;
}

void Chunk::addEntity(std::shared_ptr<Entity> e){
	entities.push_back(e);
	e->d.chunk = this;
	if(e->d.tickable) tickEntities.push_back(e);
	if(e->d.render == 1) renderClose.push_back(e);
	if(e->d.render == 2) renderFar.push_back(e);
	if(e->d.hitable) hitableEntities.push_back(e);
}

void Chunk::remove(std::shared_ptr<Entity> e){
	entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
	if(e->d.tickable) tickEntities.erase(std::remove(tickEntities.begin(), tickEntities.end(), e), tickEntities.end());
	if(e->d.render == 1) renderClose.erase(std::remove(renderClose.begin(), renderClose.end(), e), renderClose.end());
	if(e->d.render == 2) renderFar.erase(std::remove(renderFar.begin(), renderFar.end(), e), renderFar.end());
	if(e->d.hitable) hitableEntities.erase(std::remove(hitableEntities.begin(), hitableEntities.end(), e), hitableEntities.end());
	if(e->d.irIndex != -1){

		// Swap e and last entity
		int index = e->d.irIndex;

		InstancedRenderableMesh& m = e->d.chunk->detailCollections[e->d.levelOfDetail].getDescriber(e->d.detailIndex).irMesh;
		m.instanceData[index] = m.instanceData.back();

		for(int i = 0; i < (int)entities.size(); i++){
			if(entities[i]->d.irIndex == (int)m.instanceData.size()-1) entities[i]->d.irIndex = index;
		}

		// This code does what the above loop does, but it ruins some kind of data.
		/*
		auto it = std::find_if(entities.begin(), entities.end(),
			[e, m](auto& a) -> bool{return a->d.irIndex == (int)m.instanceData.size()-1;});

		if(it != entities.end()) (*it)->d.irIndex = index;*/

		// Remove last entity (that now is e)
		m.instanceData.erase(m.instanceData.end()-1);

		/*e->d.irMesh->instanceData.erase(
			std::remove_if(e->d.irMesh->instanceData.begin(), e->d.irMesh->instanceData.end(),
				[e](const Instance& i){
					return i.pos == e->pos;
				}),
			e->d.irMesh->instanceData.end());*/
		hasApplied = false;
	}
}



void Chunk::tick(float dt){
	for(auto& a : tickEntities){
		a->tick(dt);
	}

	if((int)hitableEntities.size() > 1){
		for(auto it = hitableEntities.begin(); it != hitableEntities.end()-1; it++){
			for(auto jt = it+1; jt != hitableEntities.end(); jt++){
				(*it)->resolveCollision(dt, *jt);
			}
		}
	}
}

void Chunk::render(Renderer& r, bool details){
	if(!hasApplied) apply();
	grid.render(r);
	treeMesh.render(r);
	r.useProgram(0);
	for(auto& e : renderFar){
		e->render(r);
	}
	if(details){
		r.useProgram(0);
		for(auto& e : renderClose){
			e->render(r);
		}
		detailCollections[1].render(r);
	}
	detailCollections[0].render(r);
}
