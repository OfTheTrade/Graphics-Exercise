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
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // Vertex
        if (strncmp(line_buffer, "v ", 2) == 0) {
            capacityCheck(&temp_vertices, &max_vertices, vertex_count + 3);
            sscanf(line_buffer, "v %f %f %f", &temp_vertices[vertex_count], &temp_vertices[vertex_count+1], &temp_vertices[vertex_count+2]);
            vertex_count += 3;
        }
        // Texture Coordinates
        else if (strncmp(line_buffer, "vt ", 3) == 0) {
            capacityCheck(&temp_uvs, &max_uvs, uv_count + 2);
            sscanf(line_buffer, "vt %f %f", &temp_uvs[uv_count], &temp_uvs[uv_count+1]);
            uv_count += 2;
        }
        // Normals
        else if (strncmp(line_buffer, "vn ", 3) == 0) {
            capacityCheck(&temp_normals, &max_normals, normal_count + 3);
            sscanf(line_buffer, "vn %f %f %f", &temp_normals[normal_count], &temp_normals[normal_count+1], &temp_normals[normal_count+2]);
            normal_count += 3;
        }
        // Face
        // Used to index rest of data
        else if (strncmp(line_buffer, "f ", 2) == 0) {
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = sscanf(line_buffer, "f %u/%u/%u %u/%u/%u %u/%u/%u", 
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            
            if (matches == 9) {
                if (index_count + 3 >= max_indices) {
                    max_indices *= 2;
                    vertex_indices = (unsigned int*)realloc(vertex_indices, max_indices * sizeof(unsigned int));
                    uv_indices     = (unsigned int*)realloc(uv_indices,     max_indices * sizeof(unsigned int));
                    normal_indices = (unsigned int*)realloc(normal_indices, max_indices * sizeof(unsigned int));
                }

                for(int i = 0; i < 3; i++) {
                    vertex_indices[index_count] = vertexIndex[i];
                    uv_indices[index_count]     = uvIndex[i];
                    normal_indices[index_count] = normalIndex[i];
                    index_count++;
                }
            }
        }
    }

    // Allocate a the return structs
    returnObject->numVertices = index_count;
    returnObject->vertices = (Vertex*)malloc(index_count * sizeof(Vertex));

    // Fill the interleaved array
    for (int i = 0; i < index_count; i++) {
        unsigned int v_idx = vertex_indices[i] - 1;
        unsigned int uv_idx = uv_indices[i] - 1;
        unsigned int n_idx = normal_indices[i] - 1;

        // Position
        returnObject->vertices[i].x = temp_vertices[v_idx * 3 + 0];
        returnObject->vertices[i].y = temp_vertices[v_idx * 3 + 1];
        returnObject->vertices[i].z = temp_vertices[v_idx * 3 + 2];

        // Texture Cooirdinates
        returnObject->vertices[i].u = temp_uvs[uv_idx * 2 + 0];
        returnObject->vertices[i].v = temp_uvs[uv_idx * 2 + 1];

        // Normals
        returnObject->vertices[i].nx = temp_normals[n_idx * 3 + 0];
        returnObject->vertices[i].ny = temp_normals[n_idx * 3 + 1];
        returnObject->vertices[i].nz = temp_normals[n_idx * 3 + 2];
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
}