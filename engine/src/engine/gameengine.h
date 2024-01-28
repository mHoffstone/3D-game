#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "util.h"
#include "program.h"
#include "uniformmatrix4x4.h"
#include "uniformvec3f.h"
#include "renderer.h"

class GameEngine{
	public:
		GameEngine(std::string title, int width = 800, int height = 600, bool debug = false);
		virtual ~GameEngine();

		void run();

		int getKey(int key);
		vec2f getMousePosition();
		void enableCursor();
		void disableCursor();

		Renderer& getRenderer();

		bool isRunning();
		void stop();

	protected:
		void setVSync(bool vsync);
		void setTickFrequency(int f);

		virtual bool init() = 0;
		virtual bool tick(float dt) = 0;
		virtual void render(Renderer& r) = 0;
		virtual void exit(){}

	private:
		friend void framebufferSizeCallback(GLFWwindow* window, int width, int height);
		GLFWwindow* window = nullptr;

		int keys[GLFW_KEY_LAST];

		bool vsync = false;
		int tickFrequency = 60;

		Renderer renderer;

		bool m_isRunning = true;
};

#endif // GAMEENGINE_H
