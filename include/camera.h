#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

typedef struct{
    // Angles
    float angleX;
    float angleY;
    // Distance from the center
    float radius;
    // View matrix
    mat4 viewMatrix;  
} Camera;

// Initializes the camera with default values
void initCamera(Camera* camera);

// Processes input to update camera parameters
void processCameraInput(Camera* camera, GLFWwindow* window, float time);

// Updates the camera's view matrix based on its values
void updateCameraMatrix(Camera* camera);

#endif