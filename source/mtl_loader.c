#include "../include/mtl_loader.h"

#include <stdio.h>
#include <string.h>

int loadMtl(const char* path, MaterialData* materialData){
    printf("Loading MTL file: (%s)\n", path);

     // Open the file
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file: (%s)\n", path);
        return 0;
    }

    char line_buffer[128];
    // Read the file line by line
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        // Ambient
        if (strncmp(line_buffer, "Ka ", 3) == 0) {
            sscanf(line_buffer, "Ka %f %f %f", &materialData->Ka[0], &materialData->Ka[1], &materialData->Ka[2]);
        }
        // Diffuse
        else if (strncmp(line_buffer, "Kd ", 3) == 0) {
            sscanf(line_buffer, "Kd %f %f %f", &materialData->Kd[0], &materialData->Kd[1], &materialData->Kd[2]);
        }
        // Specular
        else if (strncmp(line_buffer, "Ks ", 3) == 0) {
            sscanf(line_buffer, "Ks %f %f %f", &materialData->Ks[0], &materialData->Ks[1], &materialData->Ks[2]);
        }
        // Shininess
        else if (strncmp(line_buffer, "Ns ", 3) == 0) {
            sscanf(line_buffer, "Ns %f", &materialData->Ns);
        }
        // Transparency
        else if (strncmp(line_buffer, "d ", 2) == 0) {
            sscanf(line_buffer, "d %f", &materialData->d);
        }
        // Illumination Model
        else if (strncmp(line_buffer, "illum ", 6) == 0) {
            sscanf(line_buffer, "illum %d", &materialData->illum);
        }
    }

    fclose(file);
    return 1;
}