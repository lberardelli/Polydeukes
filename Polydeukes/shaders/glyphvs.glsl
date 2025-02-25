#version 410 core

layout(location = 0) in vec2 emPos;

out vec2 fragCoord;
out vec2 texCoord;
out vec3 fragColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 minBounds;
uniform vec2 maxBounds;

uniform vec2 resolution;

void main() {
    fragCoord = emPos;
    texCoord = (emPos - minBounds) / (maxBounds - minBounds);
    gl_Position = projection * view * model * vec4(emPos.x, emPos.y, 0.0, 1.0);
    fragColour = vec3(0.f,1.0f,0.f);
}
