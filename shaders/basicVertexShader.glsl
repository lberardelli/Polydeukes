#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec4 FragNormal;
out vec4 FragPosition;
void main() {
    gl_Position = projection * view * model * vec4(position,1.0f);
    FragPosition = model * vec4(position,1.0f);
    FragNormal = vec4(1.0f,0.f,0.f,0.f);
}
