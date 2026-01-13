#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D planet;  
uniform sampler2D cube;

uniform vec3 lightPos; 
uniform int isPlanet;     

void main()
{
    
    vec4 texColor;
    // If this is the planet, just show it
    if(isPlanet == 1) {
        texColor = texture(planet, TexCoord);
        FragColor = texColor;
        return; 
    }
    texColor = texture(cube, TexCoord);

    // Cube lighting logic
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // Ambient
    vec3 ambient = 0.2 * vec3(1.0);

    // Final result for non-planet objects
    FragColor = vec4(ambient + diffuse, 1.0) * texColor;
}