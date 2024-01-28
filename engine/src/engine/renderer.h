#ifndef RENDERER_H
#define RENDERER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "program.h"
#include "uniformmatrix4x4.h"
#include "uniformvec3f.h"
#include "uniform.h"

#include <vector>

struct render_program{
	Program program;
	UniformMatrix4x4 projection;
	UniformMatrix4x4 rotation;
	UniformVec3f translation;
};

class Renderer{
public:
	Renderer();

	~Renderer();

	int addProgram(Program program);
	void useProgram(int id);

	void setTranslation(vec3f pos);

	void setCameraPosition(vec3f pos);
	void setCameraRotation(vec2f rot);

	int getViewportWidth();
	int getViewportHeight();

private:
	std::vector<render_program> renderPrograms;

	int vpWidth, vpHeight;

	Uniform sampler;
	vec3f cameraPosition;
	vec2f cameraRotation;
	Matrix4x4 projectionMatrix;
	int currentProgram;

	Matrix4x4 createProjectionMatrix();

	friend void framebufferSizeCallback(GLFWwindow* window, int width, int height);
	friend class GameEngine;
};

#endif // RENDERER_H
