#ifndef MOUSECONTROLLER_H
#define MOUSECONTROLLER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "gameengine.h"

class MouseController{
	public:
		MouseController(){}
		MouseController(GameEngine* gameEngine);
		virtual ~MouseController();

		void tick();

		float getX(){
			return rot.x;
		}
		float getY(){
			return rot.y;
		}
		vec2f getRotation(){
			return rot;
		}

	protected:

	private:
		GameEngine* gameEngine;
		vec2f rot;
};

#endif // MOUSECONTROLLER_H
