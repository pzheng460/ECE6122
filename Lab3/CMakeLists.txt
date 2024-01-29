cmake_minimum_required(VERSION 2.6)
project(Lab3)

find_package(assimp REQUIRED)

set(CMAKE_OSX_ARCHITECTURES  "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

include_directories(
        src
)

set(LOCAL_H /opt/homebrew/include)
include_directories(${LOCAL_H})

set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.8/include/GLFW)
link_directories(${GLEW_H} ${GLFW_H})

set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.8/lib/libglfw.3.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK})

set(CMAKE_CXX_STANDARD 17)

add_executable(tutorial09_several_objects
        src/glad.c
        src/tutorial09_vbo_indexing/tutorial09_several_objects.cpp
        src/common/shader.cpp
        src/common/shader.hpp
        src/common/controls.cpp
        src/common/controls.hpp
        src/common/texture.cpp
        src/common/texture.hpp
        src/common/objloader.cpp
        src/common/objloader.hpp
        src/common/vboindexer.cpp
        src/common/vboindexer.hpp

        src/tutorial09_vbo_indexing/StandardShading.vertexshader
        src/tutorial09_vbo_indexing/StandardShading.fragmentshader
)

if (APPLE)
    target_link_libraries(tutorial09_several_objects "-framework OpenGL")
    target_link_libraries(tutorial09_several_objects "-framework GLUT")
endif ()
