#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D uPixelTexture; // 160x144 RGB8 texture

void main()
{
    FragColor = texture(uPixelTexture, TexCoord);
}
