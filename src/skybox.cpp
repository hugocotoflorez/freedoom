#include "skybox.h"
#include "setShaders.h"

#include <cassert>
#include <glad/glad.h>

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "scene.h"
static const char vertex_shader_skybox[] =
"#version 330 core \n"
"layout(location = 0) in vec3 aPos; \n"
"out vec3 TexCoords; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"void main() \n"
"{  \n"
"       mat4 viewRot = mat4(mat3(view)); \n"
"       vec4 pos = projection * viewRot * vec4(aPos, 1.0); \n"
"       gl_Position = pos.xyww; \n"
"       TexCoords = aPos; \n"
"} \n\0";

static const char fragment_shader_skybox[] =
"#version 330 core \n"
"in vec3 TexCoords; \n"
"out vec4 FragColor; \n"
"uniform samplerCube skybox; \n"
"void main() \n"
"{  \n"
"       FragColor = texture(skybox, TexCoords); \n"
"} \n\0";


void
Skybox::draw()
{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glUseProgram(__shader);
        ((Scene *) scene)->get_camera()->set_camera(__shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glUniform1i(glGetUniformLocation(__shader, "skybox"), 0);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void
Skybox::set_shader()
{
        __shader = setShaders_str(vertex_shader_skybox, fragment_shader_skybox);
}

void
Skybox::add_cubemap_image(vector<string> faces)
{
        stbi_set_flip_vertically_on_load(0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        unsigned char *data;
        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++) {
                data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
                assert(data);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void
Skybox::init()
{
        set_shader();
        float vertices[] = {
                Point(-1.0f, 1.0f, -1.0f),
                Point(-1.0f, -1.0f, -1.0f),
                Point(1.0f, -1.0f, -1.0f),
                Point(1.0f, -1.0f, -1.0f),
                Point(1.0f, 1.0f, -1.0f),
                Point(-1.0f, 1.0f, -1.0f),

                Point(-1.0f, -1.0f, 1.0f),
                Point(-1.0f, -1.0f, -1.0f),
                Point(-1.0f, 1.0f, -1.0f),
                Point(-1.0f, 1.0f, -1.0f),
                Point(-1.0f, 1.0f, 1.0f),
                Point(-1.0f, -1.0f, 1.0f),

                Point(1.0f, -1.0f, -1.0f),
                Point(1.0f, -1.0f, 1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(1.0f, 1.0f, -1.0f),
                Point(1.0f, -1.0f, -1.0f),

                Point(-1.0f, -1.0f, 1.0f),
                Point(-1.0f, 1.0f, 1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(1.0f, -1.0f, 1.0f),
                Point(-1.0f, -1.0f, 1.0f),

                Point(-1.0f, 1.0f, -1.0f),
                Point(1.0f, 1.0f, -1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(1.0f, 1.0f, 1.0f),
                Point(-1.0f, 1.0f, 1.0f),
                Point(-1.0f, 1.0f, -1.0f),

                Point(-1.0f, -1.0f, -1.0f),
                Point(-1.0f, -1.0f, 1.0f),
                Point(1.0f, -1.0f, -1.0f),
                Point(1.0f, -1.0f, -1.0f),
                Point(-1.0f, -1.0f, 1.0f),
                Point(1.0f, -1.0f, 1.0f)
        };

        GLuint vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
}
