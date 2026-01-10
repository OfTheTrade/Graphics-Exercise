#ifndef VERTEX_H
#define VERTEX_H

// A vertex structure 
typedef struct {
    float x, y, z; // Position
    float nx, ny, nz; // Normal 
    float u, v; // Texture coordinates
} Vertex;

#endif