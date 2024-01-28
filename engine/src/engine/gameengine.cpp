#include "gameengine.h"

#include "uniformmatrix4x4.h"
#include "instancedrenderablemesh.h"

#include <execinfo.h>

#include <stdexcept>
#include <csignal>

#define SMART_GET_NAME(str, val, con) if(str == "" && val == con) str = #con;

static GameEngine* currentGame = nullptr;

static void signalHandler(int signum){
	void* arr[10];
	int count = backtrace(arr, 10);
	char** strings = backtrace_symbols(arr, count);
	if(signum == SIGSEGV) util::log("Segmentation fault");
	else if(signum == SIGABRT) util::log("Aborted");
	else util::log("Signal", signum);
	for(int i = 0; i < count; i++){
		util::log(strings[i]);
	}
	std::exit(1);

	/*switch(signum){
		case SIGINT:
			currentGame->stop();
		case SIGSEGV:

	}*/
}

static void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
	std::string typeText = "";
	if(type == GL_DEBUG_TYPE_ERROR) return;
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_ERROR)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_PORTABILITY)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_PERFORMANCE)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_MARKER)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_PUSH_GROUP)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_POP_GROUP)
	SMART_GET_NAME(typeText, type, GL_DEBUG_TYPE_OTHER)
	if(typeText == "") typeText = util::toHex(type);
	throw std::runtime_error(std::string("OpenGL Error: (") + typeText + ") " + message);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height){
	glViewport(0,0, width, height);
	currentGame->renderer.vpWidth = width;
	currentGame->renderer.vpHeight = height;
	currentGame->renderer.projectionMatrix = currentGame->renderer.createProjectionMatrix();
}

GameEngine::GameEngine(std::string title, int width, int height, bool debug){
	if(currentGame != nullptr){
		throw std::runtime_error("Not allowed to create multiple instances of GameEngine");
	}
	currentGame = this;

	std::signal(SIGSEGV, signalHandler);
	std::signal(SIGABRT, signalHandler);

	if(!glfwInit()){
		throw std::runtime_error("Could not initiate GLFW!");
	}
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if(debug) glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if(!window){
		throw std::runtime_error("Could not create window!");
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwGetFramebufferSize(window, &renderer.vpWidth, &renderer.vpHeight);

	if(glewInit() != GLEW_OK){
		throw std::runtime_error("Could not initiate GLEW!");
	}

	if(debug){
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(messageCallback, 0);
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Program defaultProgram;
	defaultProgram.createProgram();
	Shader vertexShader(GL_VERTEX_SHADER, util::readFile("engine/res/shaders/vertex.glsl"));
	Shader fragmentShader(GL_FRAGMENT_SHADER, util::readFile("engine/res/shaders/fragment.glsl"));
	defaultProgram.setShaders({&vertexShader, &fragmentShader});
	renderer.addProgram(defaultProgram);

	InstancedRenderableMesh::init(renderer);

	renderer.projectionMatrix = renderer.createProjectionMatrix();

	if(util::checkGLErrors()){
		throw std::runtime_error("OpenGL error in GameEngine constructor!");
	}
}

GameEngine::~GameEngine(){
	if(window != nullptr) glfwDestroyWindow(window);
	glfwTerminate();
}

void GameEngine::run(){
	if(init()){
		exit();
		return;
	}
	if(util::checkGLErrors()){
		throw std::runtime_error("OpenGL error in init!");
	}
	glfwShowWindow(window);

	double startTime = glfwGetTime();
	double previousTime = glfwGetTime();
	double lag = 0.0;

	double TIME_PER_TICK = 1.0/(double)tickFrequency;

	int ticks = 0;
	int draws = 0;

	double lastTick = glfwGetTime();

	bool shouldRun = true;
	while(shouldRun){
		double currentTime = glfwGetTime();
		double elapsed = currentTime - previousTime;
		previousTime = currentTime;
		lag += elapsed;

		while((vsync || lag >= TIME_PER_TICK) && shouldRun){
			if(glfwWindowShouldClose(window)) shouldRun = false;
			double now = glfwGetTime();

			if(now-lastTick < 0.8){
				bool result = tick((now-lastTick));
				if(result == true) shouldRun = false;
			}
			//else shouldRun = false; // Quit if tick takes too long

			lastTick = now;

			glfwPollEvents();
			ticks++;
			lag -= TIME_PER_TICK;
			if(util::checkGLErrors()){
				throw std::runtime_error("OpenGL error in tick!");
			}

			if(lag < 2*TIME_PER_TICK || vsync){
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				render(renderer);
				glfwSwapBuffers(window);
				if(util::checkGLErrors()){
					throw std::runtime_error("OpenGL error in draw!");
				}
				draws++;
			}
		}
	}
	m_isRunning = false;
	glfwHideWindow(window);
	double endTime = glfwGetTime();
	util::logt("TPS", ticks/(endTime-startTime));
	util::logt("FPS", draws/(endTime-startTime));

	exit();
}
bool GameEngine::isRunning(){
	return m_isRunning;
}
void GameEngine::stop(){
	m_isRunning = false;
}


void GameEngine::setVSync(bool vsync){
	this->vsync = vsync;
	if(vsync) glfwSwapInterval(1);
	else glfwSwapInterval(0);
}
void GameEngine::setTickFrequency(int f){
	tickFrequency = f;
}


int GameEngine::getKey(int key){
	int state;
	switch(key){
		case GLFW_MOUSE_BUTTON_LEFT:
		case GLFW_MOUSE_BUTTON_RIGHT:
		case GLFW_MOUSE_BUTTON_MIDDLE:
			state = glfwGetMouseButton(window, key);
			break;
		default:
			state = glfwGetKey(window, key);
			break;
	}
	if(state == GLFW_PRESS) keys[key]++;
	else if(state == GLFW_RELEASE) keys[key] = 0;
	return keys[key];
}
vec2f GameEngine::getMousePosition(){
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	return {(float)mx, (float)my};
}

void GameEngine::enableCursor(){
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void GameEngine::disableCursor(){
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Renderer& GameEngine::getRenderer(){
	return renderer;
}

