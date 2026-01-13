#ifndef VERTEX_H
#define VERTEX_H

// A vertex structure 
typedef struct {
    float x, y, z; // Position
    float u, v; // Texture coordinates
    float nx, ny, nz; // Normal 

} Vertex;

#endif