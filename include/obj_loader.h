#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "../include/vertex.h"
#include "../glad/glad.h"

// Holds loaded object data
typedef struct {
    Vertex* vertices; 
    int numVertices;
} LoadedObject;

// Loads an OBJ file from the given path into the provided LoadedObject structure
// 0 on failure, 1 on success
int loadObj(const char* path, LoadedObject* returnObject);

// Frees the final arrays in the LoadedObject
void freeObj(LoadedObject* obj);

#endif