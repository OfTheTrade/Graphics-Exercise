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

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

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
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Load shaders, attached to a program

    GLuint shaderProgram = loadShaders("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (shaderProgram == 0) {
        printf("Failed to load shaders\n");
        glfwTerminate();
        return 1;
    }

    // --------- Load textures ---------

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

    // --------- Load OBJ model for planet ---------

    LoadedObject planet;
    if (loadObj("resources/planet/planet.obj", &planet) == 0) {
        printf("Failed to load OBJ model\n");
        glDeleteProgram(shaderProgram);
        glDeleteTextures(1, &cubeTexture);
        glDeleteTextures(1, &planetTexture);
        glfwTerminate();
        return 1;
    }

    // --------- Initialise VAO, VBO for planet ---------

    // Create and bind empty VAO and VBO
    GLuint planetVAO, planetVBO;
    glGenVertexArrays(1, &planetVAO);
    glBindVertexArray(planetVAO);

    glGenBuffers(1, &planetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planetVBO); 

    // Give the vertex data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, planet.numVertices * sizeof(Vertex), planet.vertices, GL_STATIC_DRAW);

    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    // UVs
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(1);

    // Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(2);

    // --------- Initialise VAO, VBO for planet ---------

    // Create and bind empty VAO and VBO
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    
    // Give vertex data to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Position 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normals 
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // UVs 
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --------- Initialize camera ---------

    Camera camera;
    initCamera(&camera);

    // Shortcuts for shader interaction
    glUseProgram(shaderProgram);
    // Matrixes
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "projection");
    // Texures
    GLint planetLoc  = glGetUniformLocation(shaderProgram, "planet");
    GLint cubeLoc  = glGetUniformLocation(shaderProgram, "cube");
    // Lighting
    GLint lightLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLint isPlanetLoc = glGetUniformLocation(shaderProgram, "isPlanet");
    // --------- Main render loop ---------

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
        glm_perspective(glm_rad(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f, projection);
        
        // View matrix
        updateCameraMatrix(&camera);

        // Model matrix (for planet)
        mat4 model;
        glm_mat4_identity(model);
        float planetX = sin(activeTime) * 10.0f;
        float planetZ = cos(activeTime) * 10.0f;
        vec3 planetPos = {planetX, 0.0f, planetZ};
        glm_translate(model, planetPos);
        
        // Upload to shader
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)camera.viewMatrix);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);

        glUniform3fv(lightLoc, 1, planetPos);
        glUniform1i(isPlanetLoc, 1);    

        // --------- Render the planet ---------

        // Used by the shader to make the planet bright
        glUniform1i(isPlanetLoc, 1);

        // Set texture for planet
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planetTexture);
        glUniform1i(planetLoc, 0);

        // Bind VAO for planet       
        glBindVertexArray(planetVAO);

        // Render planet
        glDrawArrays(GL_TRIANGLES, 0, planet.numVertices);

        // --------- Render the cubes ---------
        glBindVertexArray(cubeVAO);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glUniform1i(isPlanetLoc, 0);
        
        // Calculate the model matrix for each cube and draw it
        for(int i = 0; i < 6; i++) {
            mat4 cubeModelMat;
            glm_mat4_identity(cubeModelMat);
            
            // Orbit calculation: angle depends on index i to space them out
            float orbitAngle = activeTime + (i * (6.28f / 6.0f));
            float cx = planetX + sin(orbitAngle) * 4.0f;
            float cz = planetZ + cos(orbitAngle) * 4.0f;
            
            glm_translate(cubeModelMat, (vec3){cx, 0.0f, cz});
            
            // Different cubes at different points in the orbit
            glm_rotate(cubeModelMat, activeTime * (1.0f + i * 0.5f), (vec3){0.5f, 1.0f, 0.0f});
            
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)cubeModelMat);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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