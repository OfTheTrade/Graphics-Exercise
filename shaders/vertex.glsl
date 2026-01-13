#version 330 core

layout (location = 0) in vec3 aPos;      // Position 
layout (location = 1) in vec2 aTexCoord; // UVs
layout (location = 2) in vec3 aNormal;   // Normals 

// Sent from main
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

// Transform matrixes sent from main
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Model transform
    FragPos = vec3(model * vec4(aPos, 1.0));

    TexCoord = aTexCoord;
    Normal = mat3(transpose(inverse(model))) * aNormal;  

    // View and Model transform
    gl_Position = projection * view * vec4(FragPos, 1.0);
}