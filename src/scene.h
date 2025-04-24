#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "mesh.h"
#include "camera.h"


class Scene
{
    private:
        vector<Camera> cameras;
        vector<Mesh> meshes;

    public:
        Scene()
        {
        }
        ~Scene()
        {
        }

        Camera &get_camera(unsigned int index);
        void add_camera(Camera c);
        void add_mesh(Mesh m);
        void draw();
        void init();
        void use_shader(const char*, const char*);
};

#endif // SCENE_H_
