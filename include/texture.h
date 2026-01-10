#ifndef TEXTURE_H
#define TEXTURE_H

#include "../glad/glad.h"

// Loads a texture from the given file path
// Returns the texture ID on success, 0 on failure
GLuint loadTexture(const char* texture_file_path);
// Once loaded, to free call glDeleteTextures(1, &textureID);

#endif