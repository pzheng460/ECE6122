/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Dec 5, 2023

Description:

It's a C++ application that uses a custom class(es) with OpenGL and a third-party library (i.e. ASSIMP) to load and display animated 3D objects in OBJ files. The program should load and render the objects in the 3D screen. The animated 3D objects are frozen in place until the user presses the “g” key and then the objects start moving around at random speeds and rotating randomly about an axis. We have four animated 3D objects and each object’s movements are calculated in a separate thread. The objects are able to collide and bounce off each other. The objects are confined to the space around the center of the scene and cannot just float off into space.

*/

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <iostream>

#include "assimp_model.h"
#include <ctime>

using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

int main( void )
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 09 - Loading with AssImp", NULL, NULL);
	if( window == NULL )
    {
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
    {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

//    const GLubyte* glVersion = glGetString(GL_VERSION);
//    std::cout << "OpenGL Version: " << glVersion << std::endl;

    CAssimpModel model;

    if (!model.LoadModelFromFile("OBJ.obj"))
    {
        fprintf(stderr, "Failed to load model!\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    CAssimpModel floor;
    if (!floor.LoadModelFromFile("rectangle.obj"))
    {
        fprintf(stderr, "Failed to load rectangle!\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    CAssimpModel basketball;
    if (!basketball.LoadModelFromFile("10487_basketball_v1_3dmax2011_it2.obj"))
    {
        fprintf(stderr, "Failed to load basketball!\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    CAssimpModel::FinalizeVBO();

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

    int isStartMoving = 0;

    glm::mat4 ModelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    std::unordered_map<std::string, glm::mat4> objectModelMatrices = model.InitModelPos(glm::vec3(0.0f, 0.0f, 0.0f));
    std::unordered_map<std::string, glm::mat4> sceneModelMatrix = floor.InitModelPos(glm::vec3(0.0f, 0.0f, 0.0f));
    std::unordered_map<std::string, glm::mat4> basketballModelMatrix = basketball.InitModelPos(glm::vec3(-80.0f, 0.0f, 0.0f));
    model.initVelocity();
	do{

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 )
        { // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

        int numModels = model.GetNumModels();
        std::vector<string> modelNameGroup = {"Gost", "Pumpkin2", "Pumpkin3", "Pumpkin4"};
        std::vector<glm::vec3> lightPositions(numModels);
        std::vector<float> lightPower(numModels);

        for (int i = 0; i < numModels; i++)
        {
            glm::vec3 center = model.GetModelCenter(modelNameGroup[i]);
            glm::mat4 modelMatrix = objectModelMatrices[modelNameGroup[i]];
            modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            glm::vec4 transformedPosition = modelMatrix * glm::vec4(center, 1.0f);
            center = glm::vec3(transformedPosition);
            lightPositions[i] = center;
            // let lightPower[i] be a random number between 0.0 and 100.0
            if (isStartMoving == 1)
                lightPower[i] = (float) (rand() % 10000) / 100.0f;
            else
                lightPower[i] = 100.0f;
        }
        lightPositions.push_back(glm::vec3(20.0f, 0.0f, 20.0f));
        lightPower.push_back(500.0f);

        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // all light sources have the same color
        // Set the light
        for (int i = 0; i < numModels + 1; i++)
        {
            std::string lightPosName = "LightPosition_worldspace[" + std::to_string(i) + "]";
            std::string lightColorName = "LightColor[" + std::to_string(i) + "]";
            std::string lightPowerName = "LightPower[" + std::to_string(i) + "]";

            GLuint LightID = glGetUniformLocation(programID, lightPosName.c_str());
            glm::vec3 lightPos = lightPositions[i];
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            GLuint LightColorID = glGetUniformLocation(programID, lightColorName.c_str());
            glUniform3f(LightColorID, lightColor.x, lightColor.y, lightColor.z);
            GLuint LightPowerID = glGetUniformLocation(programID, lightPowerName.c_str());
            glUniform1f(LightPowerID, lightPower[i]);
        }

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);

        CAssimpModel::BindModelsVAO();
        model.RenderModel(programID);
        floor.RenderModel(programID);
        basketball.RenderModel(programID);
        isStartMoving = getEnableMoving();
        if (isStartMoving == 1)
        {
            objectModelMatrices = model.ComputeModelMatrix();
        }

        // Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );


	// Cleanup VBO and shader
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

    // close the model
    model.closeCAssimpModel();

	return 0;
}

