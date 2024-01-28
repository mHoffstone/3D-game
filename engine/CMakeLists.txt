cmake_minimum_required(VERSION 3.0)

project(engine)

set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE GLVND)

#add_compile_options(-finstrument-functions)
#add_link_options(-ldl -rdynamic)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

add_library(${PROJECT_NAME}
src/engine/gameengine.cpp
src/engine/instancedrenderablemesh.cpp
src/engine/keyboardcontroller.cpp
src/engine/matrix4x4.cpp
src/engine/mesh.cpp
src/engine/meshrenderer.cpp
src/engine/mousecontroller.cpp
src/engine/objectlife.cpp
src/engine/program.cpp
src/engine/renderablemesh.cpp
src/engine/renderabletexture.cpp
src/engine/renderabletexturearray.cpp
src/engine/renderer.cpp
src/engine/shader.cpp
src/engine/texture.cpp
src/engine/uniform.cpp
src/engine/uniformmatrix4x4.cpp
src/engine/uniformvec3f.cpp
src/engine/util.cpp
src/engine/vec.cpp
shaders.o
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW)
target_link_libraries(${PROJECT_NAME} glfw)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
