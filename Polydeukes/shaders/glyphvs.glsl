#version 410 core

layout(location = 0) in vec2 emPos;

out vec2 fragCoord;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 minBounds;
uniform vec2 maxBounds;
uniform float uTime;

uniform vec2 resolution;

mat4 rotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,  0, s, 0,
        0,  1, 0, 0,
       -s,  0, c, 0,
        0,  0, 0, 1
    );
}

void main() {
    float speed = 0.7;
    float angle = uTime * speed * 2.0 * 3.14159;
    mat4 rotation = mat4(1.f);
    fragCoord = emPos;
    texCoord = (emPos - minBounds) / (maxBounds - minBounds);
    gl_Position = projection * view * model * rotation * vec4(emPos.x, emPos.y, 0.0, 1.0);
}
