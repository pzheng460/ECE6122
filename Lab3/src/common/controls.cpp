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
int enableLighting = 1;
bool lKeyPressed = false;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}
int getEnableLighting() {
    return enableLighting;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 5 );

// Initial center of view : on origin point
glm::vec3 center = glm::vec3( 0, 0, 0 );

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
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
            cos(verticalAngle) * sin(horizontalAngle),
            sin(verticalAngle),
            cos(verticalAngle) * cos(horizontalAngle)
    );

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

    // Up vector
	glm::vec3 up = glm::cross( right, direction );

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= direction * deltaTime * speed;
    }
    // Rotate camera to the left maintaining radial distance (Press 'a' key)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngle -= speed * deltaTime;
        float radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );
        position = (center - newVec) * radius;
    }

    // Rotate camera to the right maintaining radial distance (Press 'd' key)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngle += speed * deltaTime;
        float radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );
        position = (center - newVec) * radius;
    }

    // Radially rotate camera up (Press up arrow key)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        verticalAngle -= speed * deltaTime;
        float radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );
        position = (center - newVec) * radius;
    }

    // Radially rotate camera down (Press down arrow key)
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        verticalAngle += speed * deltaTime;
        float radius = glm::length(position);

        glm::vec3 newVec = glm::vec3(
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );
        position = (center - newVec) * radius;
    }

    // Toggle specular and diffuse light components (Press 'L' key)
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!lKeyPressed) { // Check if the L key was not pressed in the previous frame
            enableLighting = 1 - enableLighting;
            lKeyPressed = true; // Set the flag to true to debounce input
        }
    } else {
        lKeyPressed = false; // Reset the flag when the L key is released
    }


	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								center, // and looks here : at the origin
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}