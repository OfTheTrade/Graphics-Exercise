#ifndef SHADER_H
#define SHADER_H

#include <cglm/cglm.h>
#include <glad/glad.h>

// Load and compile vertex and fragment shaders from given file paths
GLuint loadShaders(const char* vertex_file_path, const char* fragment_file_path);

#endif