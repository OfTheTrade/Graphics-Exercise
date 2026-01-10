#include "../include/shader.h"
#include "../include/texture.h"
#include "../include/obj_loader.h"
#include "../include/camera.h"
#include "../include/cube.h"
#include "../include/vertex.h"

#include "../glad/glad.h"
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// For input handling
bool isPaused = false;
bool pPressed = false;

// Process all input
// Camera input handled by processCameraInput function
void processInput(GLFWwindow *window, Camera* camera, float deltaTime) {
    // Close window on ESC key press
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    // Toggle pause on P key press
    if((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && !pPressed){
        isPaused = !isPaused;
        pPressed = true;
    }
    // Reset pPressed when P key is released
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE){
        pPressed = false;
    }
    // Camera controls handled here
    processCameraInput(camera, window, deltaTime);
    
}

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Somewhat Accurate Solar System", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    // Load shaders, attached to a program

    GLuint shaderProgram = loadShaders("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (shaderProgram == 0) {
        printf("Failed to load shaders\n");
        glfwTerminate();
        return 1;
    }

    // Load textures

    // Cube texture
    GLuint cubeTexture = loadTexture("resources/texture/container.png");
    if (cubeTexture == 0) {
        printf("Failed to load cubetexture\n");
        glDeleteProgram(shaderProgram);
        glfwTerminate();
        return 1;
    }

    // Planet texture
    GLuint planetTexture = loadTexture("resources/planet/planet_Quom1200.png");
    if (planetTexture == 0) {
        printf("Failed to load planet texture\n");
        glDeleteProgram(shaderProgram);
        glDeleteTextures(1, &cubeTexture);
        glfwTerminate();
        return 1;
    }

    // Load OBJ model for planet

    LoadedObject planet;
    if (loadObj("resources/planet/planet.obj", &planet) == 0) {
        printf("Failed to load OBJ model\n");
        glDeleteProgram(shaderProgram);
        glDeleteTextures(1, &cubeTexture);
        glDeleteTextures(1, &planetTexture);
        glfwTerminate();
        return 1;
    }

    // Initialise VAO, VBO for planet

    // Create and bind empty VAO and VBO
    GLuint planetVAO, planetVBO;
    glGenVertexArrays(1, &planetVAO);
    glBindVertexArray(planetVAO);

    glGenBuffers(1, &planetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planetVBO); 

    // Give the vertex data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, planet.numVertices * sizeof(Vertex), planet.vertices, GL_STATIC_DRAW);

    // Attribute 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    // Attribute 1: Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(1);

    // Attribute 2: Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(2);

    // Create the cubes


    
    // Initialize camera

    Camera camera;
    initCamera(&camera);


    // Main render loop
    glUseProgram(shaderProgram);

    double lastFrame = 0.0f;
    double activeTime = 0.0f;
    while(!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        double crntFrame = glfwGetTime();
        float deltaTime = crntFrame - lastFrame;
        lastFrame = crntFrame;
        if (!isPaused){
            activeTime += deltaTime;
        }

        // Process input
        processInput(window, &camera, deltaTime);

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --------- Transformation Matrices ---------

        // Projection Matrix
        mat4 projection;
        glm_perspective(glm_rad(45.0f), 1200.0f/800.0f, 0.1f, 100.0f, projection);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        
        // View matrix
        updateCameraMatrix(&camera);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)(camera.viewMatrix));
       
        // Model matrix (for planet)
        mat4 model;
        glm_mat4_identity(model);
        // Planet travels in circles
        float planetX = sin(activeTime) * 10.0f;
        float planetZ = cos(activeTime) * 10.0f;
        vec3 planetPos = {planetX, 0.0f, planetZ};

        glm_translate(model, planetPos);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)model);
        
        // Planet position is also a light source
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, planetPos);        

        // --------- Render the planet ---------

        // Set texture for planet
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planetTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "planet"), 0);

        // Bind VAO for planet       
        glBindVertexArray(planetVAO);

        // Render planet uses:
        // Transformation matrixes
        // Planet texture
        // planetVAO loaded in VRAM
        // planet.numVertices
        glDrawArrays(GL_TRIANGLES, 0, planet.numVertices);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Free resources before exiting
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &cubeTexture);
    glDeleteTextures(1, &planetTexture);
    freeObj(&planet);
    glfwTerminate();
}