#ifndef MTL_LOADER_H
#define MTL_LOADER_H

// Holds material data for the planet
typedef struct {
    float Ka[3]; // Ambient color
    float Kd[3]; // Diffuse color
    float Ks[3]; // Specular color
    float Ns;    // Specular exponent (shininess)
    float d;     // Dissolve (transparency)
    int illum;   // Illumination model
} MaterialData;

// Loads data from the given mtl file to the material data struct
int loadMtl(const char* path, MaterialData* materialData);

#endif