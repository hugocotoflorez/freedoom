#ifndef SCENE_H_
#define SCENE_H_

#include <vector>

#include "camera.h"
#include "mesh.h"


class Scene
{
    private:
        vector<Camera *> cameras;
        vector<Mesh *> meshes;
        int camera_index;

    public:
        Scene()
        : camera_index(0)
        {
        }
        ~Scene()
        {
        }

        Camera *get_camera(int index = -1);
        int get_camera_id();
        int add_camera(Camera *c);
        void add_mesh(Mesh *m);
        void draw();
        void init();
        void use_shader(const char *, const char *);
        void next_camera();
        void render();
        Mesh *get_raycast_collision(vec2 mouse);

        void set_camera(GLuint);
};

#endif // SCENE_H_
