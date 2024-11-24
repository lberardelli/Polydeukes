#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textures;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec4 FragNormal;
out vec4 FragPosition;
out vec2 aTextures;
void main() {
    gl_Position = projection * view * model * vec4(position,1.0f);
    FragPosition = model * vec4(position,1.0f);
    FragNormal = normalize(inverse(transpose(model)) * vec4(normal, 0.0f));
    aTextures = textures;
}
