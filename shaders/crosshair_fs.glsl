#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform int texture_count;
uniform sampler2D textures[16];
uniform vec3 color;

void
main()
{
        if (texture_count == 0) {
                FragColor = vec4(color, 1.0);
        } else {
                FragColor = texture(textures[0], TexCoord);
                if (FragColor.a < 0.1) {
                        discard;
                }
        }
}
