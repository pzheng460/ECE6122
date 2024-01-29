cmake_minimum_required(VERSION 2.6)
project(tutorial09_AssImp)

find_package(OpenGL REQUIRED)
find_package(assimp REQUIRED)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
    message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
    message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()

set(LOCAL_H /opt/homebrew/include)
include_directories(${LOCAL_H})

set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.8/include/GLFW)
link_directories(${GLEW_H} ${GLFW_H})

set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.8/lib/libglfw.3.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK})

set(FREE_IMAGE_DIR /opt/homebrew/Cellar/freeimage/3.18.0) # set the lib path

include_directories(${FREE_IMAGE_DIR}/include/) # include the lib

link_directories(${FREE_IMAGE_DIR}/lib/) # link the lib

include_directories(
        .
)
## Compile external dependencies
#add_subdirectory (external)
#
#add_subdirectory(distrib)
#
##add_subdirectory(FreeImage)
#
#include_directories(
#        external/AntTweakBar-1.16/include/
#        external/glfw-3.1.2/include/GLFW/
#        external/glm-0.9.7.1/
#        external/glew-1.13.0/include/
#        external/assimp-3.0.1270/include/
#        external/bullet-2.81-rev2613/src/
#        .
#)
#
#include_directories(${FreeImage_INCLUDE_DIRS})
#
#set(ALL_LIBS
#        ${OPENGL_LIBRARY}
#        glfw
#        GLEW_1130
#)
#
#add_definitions(
#        -DTW_STATIC
#        -DTW_NO_LIB_PRAGMA
#        -DTW_NO_DIRECT3D
#        -DGLEW_STATIC
#        -D_CRT_SECURE_NO_WARNINGS
#)

set(LOCAL_H /opt/homebrew/include)
include_directories(${LOCAL_H})

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_OSX_ARCHITECTURES  "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

set(CMAKE_CXX_STANDARD 17)


add_executable(tutorial09_AssImp
        tutorial09_vbo_indexing/tutorial09_AssImp.cpp
        common/shader.cpp
        common/shader.hpp
        common/controls.cpp
        common/controls.hpp
        common/texture.cpp
        common/texture.hpp
        common/objloader.cpp
        common/objloader.hpp
        common/vboindexer.cpp
        common/vboindexer.hpp

        tutorial09_vbo_indexing/StandardShading.vertexshader
        tutorial09_vbo_indexing/StandardShading.fragmentshader
        tutorial09_vbo_indexing/assimp_model.h
        tutorial09_vbo_indexing/assimp_model.cpp
        tutorial09_vbo_indexing/vertexBufferObject.cpp
        tutorial09_vbo_indexing/vertexBufferObject.h
        tutorial09_vbo_indexing/texture.cpp
        tutorial09_vbo_indexing/texture.h
        tutorial09_vbo_indexing/common_header.h
        glad.c
)

if (APPLE)
    target_link_libraries(tutorial09_AssImp "-framework OpenGL")
    target_link_libraries(tutorial09_AssImp "-framework GLUT")
    target_link_libraries(tutorial09_AssImp
            ${ALL_LIBS}
            assimp::assimp
    )
    set_target_properties(tutorial09_AssImp PROPERTIES COMPILE_DEFINITIONS "USE_ASSIMP")
    target_link_libraries(tutorial09_AssImp freeimage)
#    target_include_directories(tutorial09_AssImp PUBLIC ${FreeImage_SOURCE_DIR}/Source)
#    target_link_libraries(tutorial09_AssImp FreeImage)
endif ()
