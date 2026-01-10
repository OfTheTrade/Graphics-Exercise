#include "../include/camera.h"
#include <cglm/cglm.h>
#include <stdio.h>

const float ROTATION_SPEED = 2.0f;
const float MIN_RADIUS = 5.0f;
const float MAX_RADIUS = 50.0f;

// Initializes the camera with default values
void initCamera(Camera* camera){
    camera->angleX = 0.0f;
    camera->angleY = 0.0f;
    camera->radius = 20.0f;
    // Initialize view matrix to identity
    glm_mat4_identity(camera->viewMatrix);
    // Update the view matrix based on initial values
    updateCameraMatrix(camera);
}

// Processes input to update camera parameters
void processCameraInput(Camera* camera, GLFWwindow* window, float time){
    // Rotate around Y-axis 
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        camera->angleX += ROTATION_SPEED * time;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        camera->angleX -= ROTATION_SPEED * time;
    }

    // Rotate around X-axis
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera->angleY += ROTATION_SPEED * time ;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera->angleY -= ROTATION_SPEED * time;
    }

    // Zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (camera->radius > MIN_RADIUS) camera->radius -= 10.0f * time;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (camera->radius < MAX_RADIUS) camera->radius += 10.0f * time;
    }

    // Don't allow the camera to flip upside down
    if (camera->angleY < 0.1f) camera->angleY = 0.1f;
    if (camera->angleY > 3.0f) camera->angleY = 3.0f;
}

// Updates the camera's view matrix based on its values
void updateCameraMatrix(Camera* camera){

    // Position (Camera)
    // From spherical coordinates to Cartesian coordinates
    vec3 cameraPos;
    cameraPos[0] = camera->radius * sinf(camera->angleY) * cosf(camera->angleX);
    cameraPos[2] = camera->radius * sinf(camera->angleY) * sinf(camera->angleX);
    cameraPos[1] = camera->radius * cosf(camera->angleY);

    // Target (Planet)
    vec3 target = {0.0f, 0.0f, 0.0f};

    // Up vector
    vec3 up = {0.0f, 1.0f, 0.0f};

    // Create view matrix
    glm_lookat(cameraPos, target, up, camera->viewMatrix);
}