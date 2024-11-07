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
    FragColor = vec4(1.0, 1.0, 1.0, .1);
}
