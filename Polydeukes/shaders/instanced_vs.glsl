#version 330 core

layout (location = 0) in vec2 aPos;         // Quad vertex (-0.5 to +0.5 range)
layout (location = 1) in vec2 aInstanceXY;  // Pixel position (0 to 159, 0 to 143)

uniform vec2 uScreenSize;

out vec2 TexCoord;

void main()
{
    // Map pixel coords to NDC [-1, 1]
    vec2 pixelSize = 2.0 / uScreenSize;
    vec2 offset = aInstanceXY * pixelSize - 1.0 + pixelSize;

    gl_Position = vec4(aPos * pixelSize + offset, 0.0, 1.0);
    TexCoord = aInstanceXY / uScreenSize; // for texture lookup
}
