#version 410 core

layout(location = 0) in vec2 screenPos;

out vec2 fragCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 resolution;

void main() {
    vec2 adjustedScreenPos = vec2(screenPos.x, screenPos.y);
    fragCoord = adjustedScreenPos;
    vec2 ndcPos = (adjustedScreenPos / resolution) * 2.0 - 1.0;
    gl_Position = vec4(ndcPos, 0.0, 1.0);
}
