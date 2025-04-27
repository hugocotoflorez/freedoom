#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform sampler2D crosshairTexture;

void main()
{
        FragColor = texture(color, 1.0);
}
