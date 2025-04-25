#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
out vec4 FragColor;
uniform vec3 color;
uniform sampler2D textures[16];
uniform int texture_count;

void
main()
{
        if (texture_count > 0) {
                vec4 texColor = vec4(0, 0, 0, 0);
                for (int i = 0; i < texture_count; i++) {
                        switch (i) {
                        case 0: texColor += texture(textures[0], TexCoord)/texture_count; break;
                        case 1: texColor += texture(textures[1], TexCoord)/texture_count; break;
                        }
                }
                if (texColor.a < 0.1) {
                        discard;
                }
                FragColor = texColor;
        } else {
                FragColor = vec4(color, 1.0);
        }
}
