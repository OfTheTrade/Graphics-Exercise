#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <glad/glad.h>

// Holds loaded object data
typedef struct{
    float* vertices;
    float* uvs;
    float* normals;
    unsigned int numVertices;
} LoadedObject;

// Loads an OBJ file from the given path into the provided LoadedObject structure
// 1 on success, 0 on failure
int loadObj(const char* path, LoadedObject* returnObject);

// Frees loaded object data
void freeObj(LoadedObject* obj);

#endif