#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

// From mtl
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

uniform sampler2D planet;  
uniform sampler2D cube;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform int isPlanet;     

void main()
{
    if(isPlanet == 1) {
        // --- PLANET ---
        vec4 texColor = texture(planet, TexCoord);
        
        // We use the .mtl's Diffuse and Ambient for ambient lighting
        vec3 glow = (material.diffuse + material.ambient) * texColor.rgb;
        
        FragColor = vec4(glow, texColor.a);
    } 
    else {
        // --- CUBE ---
        // I reuse the planets mtl data, looks good enough
        // Some of the values where unused anyway since it's a light source
        vec4 texColor = texture(cube, TexCoord);

        // Ambient
        vec3 ambient = material.ambient * texColor.rgb;

        // Diffuse (The Cube faces the Planet)
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = (diff * material.diffuse) * texColor.rgb;

        // Specular (Shine)
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = (spec * material.specular);

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, texColor.a);
    }
}