cmake_minimum_required(VERSION 3.0)

project(test2)

#set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 14)
set(OpenGL_GL_PREFERENCE GLVND)

#add_compile_options(-finstrument-functions)
#add_link_options(-ldl -rdynamic)

add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

file(GLOB src_files CONFIGURE_DEPENDS "*.cpp")

add_executable(${CMAKE_PROJECT_NAME} ${src_files})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW)
target_link_libraries(${PROJECT_NAME} glfw)

add_subdirectory(engine)

target_include_directories(${PROJECT_NAME} PRIVATE engine/src/)
target_link_directories(${PROJECT_NAME} PRIVATE engine/build)
target_link_libraries(${PROJECT_NAME} engine)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
