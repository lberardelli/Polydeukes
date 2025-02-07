#version 410 core

layout(location = 0) in vec2 worldPos;

out vec2 fragCoord;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 minBounds;
uniform vec2 maxBounds;

uniform vec2 resolution;

void main() {
    fragCoord = worldPos;
    texCoord = (worldPos - minBounds) / (maxBounds - minBounds);
    gl_Position = projection * view * model * vec4(worldPos.x, worldPos.y, 0.0, 1.0);
}
