#include "../include/camera.h"
#include <cglm/cglm.h>
#include <stdio.h>

const float ROTATION_SPEED = 2.0f;
const float MIN_RADIUS = 5.0f;
const float MAX_RADIUS = 50.0f;
vec3 STATIC_POS = {0.0f, 0.0f, -20.0f};

// Initializes the camera with default values
void initCamera(Camera* camera){
    camera->angleX = -1.57f; 
    camera->angleY = 0.2f;   
    camera->radius = 40.0f;
    updateCameraMatrix(camera);
    // Initialize view matrix to identity
    glm_mat4_identity(camera->viewMatrix);
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
    if (camera->angleY < -2.0f) camera->angleY = -2.0f;
    if (camera->angleY > 2.0f) camera->angleY = 2.0f;
}

// Updates the camera's view matrix based on its values
void updateCameraMatrix(Camera* camera){
    // Calculate direction 
    vec3 front;
    front[0] = cosf(camera->angleY) * cosf(camera->angleX);
    front[1] = sinf(camera->angleY);
    front[2] = cosf(camera->angleY) * sinf(camera->angleX);
    glm_normalize(front);

    // Calculate POS depending on radius to use
    vec3 cameraPos = {0.0f, 0.0f, camera->radius};

    // Calculate target
    vec3 target;
    glm_vec3_add(cameraPos, front, target); 

    // Define up
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_lookat(cameraPos, target, up, camera->viewMatrix);
}