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

int loadObj(const char* path,LoadedObject* returnObject){
    printf("Loading OBJ file: (%s)\n", path);
    // Open the file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file: (%s)\n", path);
        return 1;
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
    while(fscanf(file, "%s", line_buffer) != EOF){

        if (strcmp(line_buffer, "v") == 0){
            // Vertex line
            capacityCheck(&temp_vertices, &max_vertices, vertex_count + 3);
            fscanf(file, "%f %f %f\n", &temp_vertices[vertex_count], &temp_vertices[vertex_count+1], &temp_vertices[vertex_count+2]);
            vertex_count += 3;
        }else if (strcmp(line_buffer, "vt") == 0) {
            // UV line 
            capacityCheck(&temp_uvs, &max_uvs, uv_count + 2);
            fscanf(file, "%f %f\n", &temp_uvs[uv_count], &temp_uvs[uv_count+1]);
            uv_count += 2;
        }else if (strcmp(line_buffer, "vn") == 0) {
            // Normal line
            capacityCheck(&temp_normals, &max_normals, normal_count + 3);
            fscanf(file, "%f %f %f\n", &temp_normals[normal_count], &temp_normals[normal_count+1], &temp_normals[normal_count+2]);
            normal_count += 3;
        }else if (strcmp(line_buffer, "f") == 0 ){
            // Face line
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int face_buffer = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", 
                                 &vertexIndex[0], &uvIndex[0], &normalIndex[0],
                                 &vertexIndex[1], &uvIndex[1], &normalIndex[1],
                                 &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            
            // Store the indices (resizing if necessary)
           if (index_count + 3 >= max_indices) {
                max_indices *= 2;
                vertex_indices = (unsigned int*)realloc(vertex_indices, max_indices * sizeof(unsigned int));
                uv_indices     = (unsigned int*)realloc(uv_indices,     max_indices * sizeof(unsigned int));
                normal_indices = (unsigned int*)realloc(normal_indices, max_indices * sizeof(unsigned int));
            }

            // First face 
            vertex_indices[index_count] = vertexIndex[0];
            uv_indices[index_count]     = uvIndex[0];
            normal_indices[index_count] = normalIndex[0];
            index_count++;

            // Second face
            vertex_indices[index_count] = vertexIndex[1];
            uv_indices[index_count]     = uvIndex[1];
            normal_indices[index_count] = normalIndex[1];
            index_count++;

            // Third face
            vertex_indices[index_count] = vertexIndex[2];
            uv_indices[index_count]     = uvIndex[2];
            normal_indices[index_count] = normalIndex[2];
            index_count++;
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

    return 0;
}