#include "mousecontroller.h"

MouseController::MouseController(GameEngine* gameEngine){
	this->gameEngine = gameEngine;
	gameEngine->disableCursor();
}

MouseController::~MouseController(){
	//dtor
}


void MouseController::tick(){
	vec2f mouse = gameEngine->getMousePosition();

	static vec2f oldMouse;
	rot.x += (mouse.y-oldMouse.y) * 0.002f;
	if(rot.x < -PI/2.0f) rot.x = -PI/2.0f;
	if(rot.x >  PI/2.0f) rot.x =  PI/2.0f;
	rot.y += (mouse.x-oldMouse.x) * 0.002f;

	oldMouse = mouse;
}
