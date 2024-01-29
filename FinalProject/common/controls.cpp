// Include GLFW
#include <GLFW/glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;
int enableMoving = 0;
bool gKeyPressed = false;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}
int getEnableMoving() {
    return enableMoving;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( 50, 0, 0 );

float radius = glm::length(position);

// Initial center of view : on origin point
glm::vec3 center = glm::vec3( 0, 0, 0 );

// Initial horizontal angle : toward -Z
float horizontalAngle = 0.0f;
// Initial vertical angle : none
float verticalAngle = M_PI / 2.0f;;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
            sin(verticalAngle) * cos(horizontalAngle),
            sin(verticalAngle) * sin(horizontalAngle),
            cos(verticalAngle)
    );

    // Up vector
	glm::vec3 up = glm::vec3(0, 0, 1);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        position -= direction * deltaTime * speed * 10.0f;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        position += direction * deltaTime * speed * 10.0f;
    }
    // Rotate camera to the left maintaining radial distance (Press left key)
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        horizontalAngle -= speed * deltaTime;
        radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                sin(verticalAngle) * cos(horizontalAngle),
                sin(verticalAngle) * sin(horizontalAngle),
                cos(verticalAngle)
        );
        position = (newVec - center) * radius;
    }

    // Rotate camera to the right maintaining radial distance (Press right key)
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        horizontalAngle += speed * deltaTime;
        radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                sin(verticalAngle) * cos(horizontalAngle),
                sin(verticalAngle) * sin(horizontalAngle),
                cos(verticalAngle)
        );
        position = (newVec - center) * radius;
    }

    // Radially rotate camera up (Press "u" arrow key)
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        verticalAngle -= speed * deltaTime;
        if (verticalAngle < 0.0f) {
            verticalAngle = 0.001f;
        }
        radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                sin(verticalAngle) * cos(horizontalAngle),
                sin(verticalAngle) * sin(horizontalAngle),
                cos(verticalAngle)
        );
        position = (newVec - center) * radius;
    }

    // Radially rotate camera down (Press "d" arrow key)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        verticalAngle += speed * deltaTime;
        if (verticalAngle > M_PI) {
            verticalAngle = M_PI - 0.001f;
        }
        radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                sin(verticalAngle) * cos(horizontalAngle),
                sin(verticalAngle) * sin(horizontalAngle),
                cos(verticalAngle)
        );
        position = (newVec - center) * radius;
    }

    // Toggle random behaviors (Press 'g' key)
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!gKeyPressed) { // Check if the L key was not pressed in the previous frame
            enableMoving = 1 - enableMoving;
            gKeyPressed = true; // Set the flag to true to debounce input
        }
    } else {
        gKeyPressed = false; // Reset the flag when the L key is released
    }


	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								center, // and looks here : at the origin
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}