#include "player.h"

#include "engine/mousecontroller.h"
#include "engine/keyboardcontroller.h"

#include "crafting.h"
#include "world.h"

#include <cmath>

static KeyboardController keyboardController;
static MouseController mouseController;

Player::Player(GameEngine* gameEngine){
	keyboardController = KeyboardController(gameEngine);
	mouseController = MouseController(gameEngine);
	handItem.type = -1;
	//frictionCoeff = 1.0f;
}

Player::~Player(){
	//std::cout << "~Player" << std::endl;
}

void Player::tryInteract(){
	if(interactCooldown > 0.0f) return;
	if(heldCube != nullptr){
		vec3f delta = player->getLookDir();
		vec3f nextPos = player->getEyePos() + delta * 2.0f;
		heldCube->points[0].pos = nextPos;
	}
	else{
		std::shared_ptr<Entity> e = World::getWorld().getEntityFrom(player);
		ItemStack origHandItem = handItem;
		bool didInteract = interactWith(e, handItem);
		if(didInteract) interactCooldown = 0.4f;
		inventory.give(handItem);
		inventory.take(origHandItem);
		if(handItem.count <= 0) handItem.type = -1;
	}
}

void Player::processInput(float dt, GameEngine* gameEngine){
	mouseController.tick();
	keyboardController.tick({0,0});

	rotation = mouseController.getRotation();

	//if(onGround){
		/*vec3f dir = keyboardController.getDelta();
		dir.y = 0.0f;
		Matrix4x4 rotMat = Matrix4x4::rotationY(rotation.y);
		acc += rotMat * dir * acceleration * (gameEngine->getKey(GLFW_KEY_LEFT_SHIFT) ? 6.0f : 1.0f);*/
		tryMove(dt, keyboardController.getDX(), keyboardController.getDZ());
		if(keyboardController.getDY() > 0.0f){
			//acc.y += std::sqrt(2.0f*9.82f*1.0f)/dt; // Force requred to jump one meter.
			tryJump(dt);
		}
	//}

	// Airplane code
	/*{
		Matrix4x4 rotMat = Matrix4x4::rotationXY(rotation);
		vec3f dir = keyboardController.getDelta();
		if(dir.z < 0.0f){
			acc += rotMat * -vec3f::Z * 9.82f;

		}
	}*/


	if(gameEngine->getKey(GLFW_MOUSE_BUTTON_LEFT)){
		//tryPickup();
		tryInteract();
	}
	else{
		heldCube = nullptr;
	}
}


void Player::tick(float dt){
	// Airplane code
	/*Matrix4x4 rotMat = Matrix4x4::rotationXY(rotation);
	float v = ((pos-oldPos).dot(-(rotMat*vec3f::Z)))/dt;
	acc += v*v * (rotMat*vec3f::Y) * std::cos(rotation.x) * 0.1f;*/

	Entity::tick(dt);
}


