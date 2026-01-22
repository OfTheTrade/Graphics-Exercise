#include "../include/texture.h"
#include <stdio.h>
#include <stdlib.h>
#include "../glad/glad.h"

// For image loading
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

// Loads a texture from the given file path
// Returns the texture ID on success, 0 on failure
GLuint loadTexture(const char* path){
    printf("Loading Texture: (%s)\n", path);

    // Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    int width, height, nrChannels;
    // Flip image on load
    stbi_set_flip_vertically_on_load(1); 

    // Read the image file
    unsigned char *input = stbi_load(path, &width, &height, &nrChannels, 0);

    if (input){
        GLenum format = GL_RGB;
        if (nrChannels == 4){
            format = GL_RGBA;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, input);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free image data
        stbi_image_free(input);
        return textureID;
    } else {
        printf("Failed to load texture: (%s)\n", path);
        stbi_image_free(input);
        return 0;
    }
}