#version 410 core
uniform vec3 LineColour;
layout ( location = 0 ) out vec4 FragColor;
void main()
{
    FragColor = vec4(LineColour.x,LineColour.y,LineColour.z,1.0);
}
