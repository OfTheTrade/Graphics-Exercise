#include "../include/obj_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to realloc temporary arrays if needed
void capacityCheck(float** array, int* capacity,int count){
    if (count >= *capacity){
        *capacity *= 2;
        *array = (float*)realloc(*array, (*capacity) * sizeof(float));
    }
}

// Loads an OBJ file from the given path into the provided LoadedObject structure
// 0 on failure, 1 on success
int loadObj(const char* path,LoadedObject* returnObject){
    printf("Loading OBJ file: (%s)\n", path);
    // Open the file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file: (%s)\n", path);
        return 0;
    }

    // Starting maximum size (will realloc if needed)
    int max_vertices = 1024, max_uvs = 1024, max_normals = 1024, max_indices = 1024;
    int vertex_count = 0, uv_count = 0, normal_count = 0, index_count = 0;
    
    // Allocate temporary object data arrays
    float* temp_vertices = (float*)malloc(max_vertices * sizeof(float));
    float* temp_uvs = (float*)malloc(max_uvs * sizeof(float));
    float* temp_normals = (float*)malloc(max_normals * sizeof(float));
    // Used to index with face lines in the OBJ file
    // Three arrays running in parallel. Each of their indeces correspond to a face.
    unsigned int* vertex_indices = (unsigned int*)malloc(max_indices * sizeof(unsigned int));
    unsigned int* uv_indices = (unsigned int*)malloc(max_indices * sizeof(unsigned int));
    unsigned int* normal_indices = (unsigned int*)malloc(max_indices * sizeof(unsigned int));

    char line_buffer[128];
    // Read the file line by line
    char line[256];
while (fgets(line, sizeof(line), file)) {
    // Skip comments or empty lines
    if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

    char prefix[10];
    sscanf(line, "%s", prefix);

    if (strcmp(prefix, "v") == 0) {
        capacityCheck(&temp_vertices, &max_vertices, vertex_count + 3);
        sscanf(line, "v %f %f %f", &temp_vertices[vertex_count], 
                                   &temp_vertices[vertex_count+1], 
                                   &temp_vertices[vertex_count+2]);
        vertex_count += 3;
    } 
    else if (strcmp(prefix, "vt") == 0) {
        capacityCheck(&temp_uvs, &max_uvs, uv_count + 2);
        sscanf(line, "vt %f %f", &temp_uvs[uv_count], 
                                 &temp_uvs[uv_count+1]);
        uv_count += 2;
    } 
    else if (strcmp(prefix, "vn") == 0) {
        capacityCheck(&temp_normals, &max_normals, normal_count + 3);
        sscanf(line, "vn %f %f %f", &temp_normals[normal_count], 
                                    &temp_normals[normal_count+1], 
                                    &temp_normals[normal_count+2]);
        normal_count += 3;
    } 
    else if (strcmp(prefix, "f") == 0) {
        unsigned int v[3], t[3], n[3];
        // Blender often exports f v/t/n v/t/n v/t/n
        int matches = sscanf(line, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
                             &v[0], &t[0], &n[0], 
                             &v[1], &t[1], &n[1], 
                             &v[2], &t[2], &n[2]);

        if (matches == 9) {
            // Your existing index_count check and storage logic here...
            // ...
        } else {
            // Optional: Handle f v//n or f v/t formats if sscanf fails
            printf("Warning: Unsupported face format or non-triangulated mesh!\n");
        }
    }
}

    // Allocate final arrays
    returnObject->numVertices = index_count;
    returnObject->vertices = (float*)malloc(index_count * 3 * sizeof(float));
    returnObject->uvs = (float*)malloc(index_count * 2 * sizeof(float));
    returnObject->normals = (float*)malloc(index_count * 3 * sizeof(float));

    // Fill final arrays
    for (int i = 0; i < index_count; i++){
        // Get indices (from 1 to n in OBJ files so we subtract 1)
        unsigned int vertex_index = vertex_indices[i] - 1;
        unsigned int uv_index = uv_indices[i] - 1;
        unsigned int normal_index = normal_indices[i] - 1;

        // Fill object arrays with data from temporary arrays
        returnObject->vertices[i*3] = temp_vertices[vertex_index*3];
        returnObject->vertices[i*3 + 1] = temp_vertices[vertex_index*3 + 1];
        returnObject->vertices[i*3 + 2] = temp_vertices[vertex_index*3 + 2];

        returnObject->uvs[i*2] = temp_uvs[uv_index*2];
        returnObject->uvs[i*2 + 1] = temp_uvs[uv_index*2 + 1];

        returnObject->normals[i*3] = temp_normals[normal_index*3];
        returnObject->normals[i*3 + 1] = temp_normals[normal_index*3 + 1];
        returnObject->normals[i*3 + 2] = temp_normals[normal_index*3 + 2];
    }
    
    // Free temporary arrays
    free(temp_vertices);
    free(temp_uvs);
    free(temp_normals);
    free(vertex_indices);
    free(uv_indices);
    free(normal_indices);
    fclose(file);

    return 1;
}

// Frees the final arrays in the LoadedObject
void freeObj(LoadedObject* obj){
    free(obj->vertices);
    free(obj->uvs);
    free(obj->normals);
}