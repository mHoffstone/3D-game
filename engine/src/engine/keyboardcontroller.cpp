#include "keyboardcontroller.h"

#include "matrix4x4.h"

KeyboardController::KeyboardController(GameEngine* gameEngine){
	this->gameEngine = gameEngine;
}

KeyboardController::~KeyboardController(){
	//dtor
}

void KeyboardController::tick(vec2f rot){
	delta = {0, 0, 0};
	if(gameEngine->getKey(GLFW_KEY_W)) delta.z += -1.0f;
	if(gameEngine->getKey(GLFW_KEY_S)) delta.z += 1.0f;
	if(gameEngine->getKey(GLFW_KEY_A)) delta.x += -1.0f;
	if(gameEngine->getKey(GLFW_KEY_D)) delta.x += 1.0f;
	if(gameEngine->getKey(GLFW_KEY_SPACE)) delta.y += 1.0f;
	if(gameEngine->getKey(GLFW_KEY_LEFT_SHIFT)) delta.y += -1.0f;
	Matrix4x4 rotMat = Matrix4x4::rotationY(rot.y);
	delta = rotMat * delta;
}
