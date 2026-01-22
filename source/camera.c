#include "../include/camera.h"
#include <cglm/cglm.h>
#include <stdio.h>

const float ROTATION_SPEED = 2.0f;
float STATIC_POS_Z = 30.0f;
float STATIC_POS_Y = 5.0f;
float STATIC_POS_X = 0.0f;

// Initializes the camera with default values
void initCamera(Camera* camera){
    camera->angleX = -1.57f; 
    camera->angleY = -0.3f;   
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

    // Position never changes
    vec3 cameraPos = {STATIC_POS_X, STATIC_POS_Y, STATIC_POS_Z};

    // Calculate target
    vec3 target;
    glm_vec3_add(cameraPos, front, target); 

    // Define up
    vec3 up = {0.0f, 1.0f, 0.0f};
    glm_lookat(cameraPos, target, up, camera->viewMatrix);
}