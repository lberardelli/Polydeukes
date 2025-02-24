#version 410 core

in vec2 fragCoord;
in vec2 texCoord;
in vec3 fragColour;

out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 resolution;
uniform float threshold;
uniform sampler2D sdfTexture;
uniform vec3 colour;

void main() {
    float sdfValue = texture(sdfTexture, texCoord).r;
    if (sdfValue < threshold) {
        FragColor = vec4(fragColour, 1.0);
    } else {
        discard;
    }
}
