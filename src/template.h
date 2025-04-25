#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "shape.h"

#include "setShaders.h"

#define PIMED 1.5708f
#define PI 3.1416f

class Template
{
    private:
    public:
        Template()
        {
        }
        ~Template()
        {
        }

        static Scene *plain_scene()
        {
                Scene *s = new Scene();
                Mesh *cube = Shape::cube(1);
                Mesh *plane = Shape::plane(10);
                CamView *portal = Shape::portal_plane(3);
                CamView *portal2 = Shape::portal_plane(3);
                CamView *portal3 = Shape::portal_plane(3);
                Camera *c = new Camera(vec3(0.0f, 10.0f, 10.0f));
                Camera *c2 = new Camera(vec3(-2.0f, 0.5f, 0.0f));
                Camera *c3 = new Camera(vec3(0.0f, 2.0f, -1.0f));
                Camera *c4 = new Camera(vec3(-2.0f, 2.0f, 0.0f));

                cube->translate(vec3(0.0f, 0.5f, 0.0f));
                plane->set_color(0xAADD00);

                portal->config();
                portal->translate(vec3(-3.5f, 2.0f, -5.0f));
                portal->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));
                // portal->rotate(PI, vec3(0.0f, 1.0f, 0.0f));

                portal2->config();
                portal2->translate(vec3(0.0f, 2.0f, -5.0f));
                portal2->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));
                // portal2->rotate(PI, vec3(0.0f, 1.0f, 0.0f));

                portal3->config();
                portal3->translate(vec3(3.5f, 2.0f, -5.0f));
                portal3->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));
                // portal3->rotate(PI, vec3(0.0f, 1.0f, 0.0f));

                c2->look_at(cube->get_absolute_position());
                c3->look_at(cube->get_absolute_position());
                c4->look_at(cube->get_absolute_position());
                s->add_mesh(plane);
                s->add_mesh(cube);
                s->add_mesh(portal);
                s->add_mesh(portal2);
                s->add_mesh(portal3);
                s->add_camera(c);

                int cam_id = s->add_camera(c2);
                portal->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c3);
                portal2->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c4);
                // portal3->set_dynamic_camera(cam_id);

                s->use_shader("shaders/simple_vs.glsl", "shaders/simple_fs.glsl");
                portal->set_shader(setShaders("shaders/texture_vs.glsl", "shaders/texture_fs.glsl"));
                portal2->set_shader(setShaders("shaders/texture_vs.glsl", "shaders/texture_fs.glsl"));
                portal3->set_shader(setShaders("shaders/texture_vs.glsl", "shaders/texture_fs.glsl"));
                return s;
        }
};
