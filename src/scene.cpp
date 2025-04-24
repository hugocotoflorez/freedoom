#include "scene.h"
#include "camera.h"
#include "mesh.h"
#include "setShaders.h"

#define LOG_PRINT 0
#include "../thirdparty/frog/frog.h"

Camera &
Scene::get_camera(unsigned int index = 0)
{
        // LOG("Calling  get_camera %u\n", index);
        if (index >= cameras.size()) {
                PANIC("Can not locate camera");
        }
        return cameras.at(index);
}

void
Scene::add_camera(Camera c)
{
        cameras.push_back(c);
        LOG("Adding camera %ld\n", cameras.size());
}

void
Scene::add_mesh(Mesh m)
{
        m.set_scene(this);
        meshes.push_back(m);
}

void
Scene::draw()
{
        for (auto m : meshes)
                m.draw();
}

void
Scene::init()
{
        for (auto m : meshes)
                m.init();
}


void
Scene::use_shader(const char *vs, const char *fs)
{
        GLuint shader = setShaders(vs, fs);
        for (auto &m : meshes)
                m.set_shader(shader);
}
