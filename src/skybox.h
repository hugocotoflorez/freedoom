#ifndef SKYBOX_H_
#define SKYBOX_H_

#include <vector>
#include <string>

#define Point(a, b, c) a, b, c

class Skybox
{
    private:
        unsigned int __shader;
        unsigned int texture;
        unsigned int vao;
        void *scene;

        void set_shader();

    public:
        Skybox()
        {
        }

        void set_scene(void *s)
        {
                scene = s;
        }

        void init();
        void add_cubemap_image(std::vector<std::string> faces);
        void draw();
};


#endif // SKYBOX_H_
