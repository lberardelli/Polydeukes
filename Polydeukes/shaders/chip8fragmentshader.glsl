#version 330 core
in vec4 FragNormal;
in vec4 FragPosition;
out vec4 FragColor;
in vec2 aTextures;
uniform vec3 aColour;
uniform vec3 lightPosition;
uniform vec3 eye;
uniform vec3 lightColour;
uniform sampler2D texture1;
void main() {
    vec3 fragmentPosition = vec3(FragPosition.x, FragPosition.y, FragPosition.z);
    vec3 lightVector = normalize(lightPosition - fragmentPosition);
    vec3 normal = vec3(FragNormal.x, FragNormal.y, FragNormal.z);
    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColour;
    //lambertian model
    float angle = max(dot(normalize(lightVector), normalize(normal)), 0.0f);
    vec3 diffusion = angle * lightColour;
    FragColor = vec4((ambient + diffusion) * aColour, 1.0f);
}
