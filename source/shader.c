#include "../include/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Help function to read the contents of a file into a string
char* readFile(const char* path){
    // Open file
    FILE* file = fopen(path, "rb");
    if (!file){
        printf("Failed to open file: (%s)\n", path);
        return 0;
    }

    // Get file length
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer for file contents
    char* buffer = (char*)malloc(length + 1);
    if (!buffer){
        printf("Failed to allocate memory for file: (%s)\n", path);
        fclose(file);
        return 0;
    }

    // Read file contents into buffer
    fread(buffer, 1, length, file);
    buffer[length] = '\0';

    fclose(file);
    return buffer;
}


// Helper function to compile a shader and check for errors
// Returns shader ID on success, 0 on failure
GLuint compileShader(const char* source, GLenum shaderType){
    // Create shader object and attempt compilation
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &source, NULL);
    glCompileShader(shaderID);

    // Check compilation status
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success){
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        printf("Shader compilation error: %s\n", infoLog);
        glDeleteShader(shaderID);
        return 0;
    }
    return shaderID;
}


// Load and compile vertex and fragment shaders from given file paths
GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path){
    printf("Loading Shaders: S:(%s) | F:(%s)\n", vertex_file_path, fragment_file_path);

    // Read vertex and fragment shader code from files
    char* vertexShaderCode = readFile(vertex_file_path);
    if (!vertexShaderCode){
        return 0;
    }

    char* fragmentShaderCode = readFile(fragment_file_path);
    if (!fragmentShaderCode) {
        free(vertexShaderCode);
        return 0;
    }

    // Compile the shaders
    GLuint vertexShader = compileShader(vertexShaderCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderCode,GL_FRAGMENT_SHADER);

    // Free the shader code strings
    free(vertexShaderCode);
    free(fragmentShaderCode);

    // Link the shaders with a program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    // Check linking status
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(programID, 1024, NULL, infoLog);
        printf("Linking failed: (%s)\n", infoLog);
        return 0;
    }

    // Clean up shaders as they are no longer needed after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programID;
}