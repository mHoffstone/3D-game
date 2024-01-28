#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gameengine.h"

class KeyboardController{
	public:
		KeyboardController(){}
		KeyboardController(GameEngine* gameEngine);
		virtual ~KeyboardController();

		void tick(vec2f rot);

		float getDX(){
			return delta.x;
		}
		float getDY(){
			return delta.y;
		}
		float getDZ(){
			return delta.z;
		}
		vec3f getDelta(){
			return delta;
		}

	protected:

	private:
		GameEngine* gameEngine;
		vec3f delta;
};

#endif // KEYBOARDCONTROLLER_H
