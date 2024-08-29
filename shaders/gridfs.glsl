#version 330 core
in vec4 FragNormal;
in vec4 FragPosition;
out vec4 FragColor;
in vec2 aTextures;
uniform vec3 aColour;
uniform vec3 lightPosition;
uniform vec3 eye;
uniform vec3 lightColour;
uniform sampler2D texture1;
void main() {
    vec2 uv = vec2(FragPosition.x, FragPosition.y);
    float total = floor(uv.x * 40.0) + floor(uv.y * 40.0);
    bool isEven = mod(total, 2.0) == 0.0;
    vec4 col1 = vec4(0.1, 0.1, 0.1, 1.0);
    vec4 col2 = vec4(0.5, 0.5, 0.5, 1.0);
    FragColor = (isEven) ? col1 : col2;
}
