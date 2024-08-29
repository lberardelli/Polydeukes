#version 330 core
in vec3 aColour;
out vec4 FragColor;
void main() {
    FragColor = vec4(aColour.x, aColour.y, aColour.z, 1.0);
}
