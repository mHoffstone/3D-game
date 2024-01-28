#include "objectlife.h"

#include <vector>
#include <iostream>

static std::vector<ObjectLifeEntity*> destroyList;

ObjectLife::ObjectLife(){

}

ObjectLife::~ObjectLife(){
	if(!destroyChecked) throw std::runtime_error("Object never called maybeDestroy()!");
}


void ObjectLife::createLife(){
	//lifeEntity = std::make_shared<ObjectLifeEntity>();
}

void ObjectLife::reset(){

}

void ObjectLife::maybeDestroy(){
	destroyChecked = true;
	/*ObjectLifeEntity* ptr = lifeEntity.get();
	lifeEntity = nullptr;
	for(auto it = destroyList.begin(); it != destroyList.end(); it++){
		if(*it == ptr){
			destroy();
			destroyList.erase(it);
			break;
		}
	}*/
}

bool ObjectLife::isCleaned(){
	return destroyList.empty();
}


ObjectLifeEntity::ObjectLifeEntity(){

}

ObjectLifeEntity::~ObjectLifeEntity(){
	//destroyList.push_back(this);
}
