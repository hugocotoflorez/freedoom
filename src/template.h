#include "actor.h"
#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "settings.h"
#include "shape.h"

#include "setShaders.h"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <utility>

#define PIMED 1.5708f
#define PI 3.1416f

#include <GLFW/glfw3.h>

static void
disable_collision_sphere(Mesh *m)
{
        m->disable_sphere_collider();
}

static void
enable_collision_sphere(Mesh *m)
{
        m->enable_sphere_collider();
}

#include <cmath>
#include <iostream>

bool
calcular_x2(double x1, double y1, double y2, double &x2)
{
        double value = x1 * x1 + y1 * y1 - y2 * y2;

        if (value < 0) {
                return false;
        }

        x2 = fabsl(sqrt(x1 * x1 + y1 * y1 - y2 * y2));
        return true;
}

extern double *interframe_time();

static void
__1person_handler(GLFWwindow *window, Scene *scene)
{
        /* Esto no deberia ir aqui */
        if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

        static bool initialized = false;
        static float xprev, yprev;
        double _xpos, _ypos;

        Actor *actor = scene->get_actor();

        glfwGetCursorPos(window, &_xpos, &_ypos);
        float xpos = static_cast<float>(_xpos);
        float ypos = static_cast<float>(_ypos);

        if (!initialized) {
                xprev = xpos;
                yprev = ypos;
                initialized = true;
        }

        float xoffset = -xpos + xprev;
        float yoffset = ypos - yprev;

        actor->rotate(xoffset * MOUSE_SENS_X * MOUSE_REDUCTION * MOUSE_FIRST_PERSON_SENS_X, vec3(0, 1, 0));

        float X = actor->get_camera_xoffset();
        float Y = actor->get_camera_yoffset();
        float J = yoffset * MOUSE_SENS_Y * MOUSE_REDUCTION * MOUSE_FIRST_PERSON_SENS_Y;
        float I;
        double x2_pos, x2_neg;
        if (calcular_x2(X, Y, Y + J, x2_pos)) {
                I = x2_pos;
                actor->set_camera_yoffset(Y + J);
                actor->set_camera_xoffset(I);
        } else {
                printf("No existe solucion para x2\n");
        }


        static bool lock_l = false;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !lock_l) {
                /* The *2.0f - 1.0 is to transform from [0,1] to [-1, 1] */
                vec2 mouse = vec2(0, 0);
                scene->get_raycast_collision(mouse);
                lock_l = true;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && lock_l) {
                lock_l = false;
        }

        xprev = xpos;
        yprev = ypos;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);

        if (oneclick(window, GLFW_KEY_T)) {
                scene->next_camera();
        }

        mat4 m = actor->get_absolute_model();
        vec3 dirf = -normalize(vec3(m[2])); // frente (hacia -Z)
        vec3 right = normalize(vec3(m[0])); // derecha (hacia +X)
        vec3 posi = actor->get_position();

        // printf("Pos: (%2.2f,%2.2f,%2.2f) Dirf: (%2.2f,%2.2f,%2.2f) Dirr (%2.2f,%2.2f,%2.2f)\n",
        // posi.x, posi.y, posi.z, dirf.x, dirf.y, dirf.z, right.x, right.y, right.z);

        vec3 movement = vec3(0);

        // Movimiento
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                movement += dirf;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                movement -= dirf;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                movement -= right;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                movement += right;

        // Escalar movimiento
        double speed = 2.0f;
        movement *= (*interframe_time() * speed);

        // printf("Movement: (%2.2f,%2.2f,%2.2f)\n", movement.x, movement.y, movement.z);

        // Aplicar movimiento en espacio mundial
        vec3 newPos = actor->get_position() + movement;
        mat4 newModel = actor->get_model();
        newModel[3] = vec4(newPos, 1.0f);
        actor->set_model(newModel);
}

static void
__select_obj(SphereCollider *c)
{
        printf("Selecting %s\n", c->get_parent()->get_name());
        c->get_parent()->select();
}

static void
__unselect_all(SphereCollider *c)
{
        ((Scene *) c->get_parent()->get_scene())->unselect_all();
}

static void
__select_camera(Mesh *m)
{
        printf("Calling __select_camera\n");
        ((Scene *) m->get_scene())->set_camera((Camera *) m->get_attached_camera());
}

static void
change_controls_default(Mesh *mesh)
{
        Scene *scene = (Scene *) mesh->get_scene();
        scene->set_handler(scene->get_default_handler());
        scene->set_camera((GLuint) 0);
        printf("Change handler -> default_handler\n");
}

static void
change_controls_1person(Mesh *mesh)
{
        Scene *scene = (Scene *) mesh->get_scene();

        Actor *actor = scene->get_actor();
        if (actor == nullptr) {
                printf("1st person actor missing!\n");
                return;
        }
        scene->set_camera(actor->get_following_camera());
        scene->set_handler(__1person_handler);
        printf("Change handler -> __1person_handler\n");
}

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
                CamView *camviewer = Shape::camviewer_plane(3);
                CamView *camviewer2 = Shape::camviewer_plane(3);
                CamView *camviewer3 = Shape::camviewer_plane(3);
                Camera *c = new Camera(vec3(0.0f, 10.0f, 10.0f));
                Camera *c2 = new Camera(vec3(-2.0f, 0.5f, 0.0f));
                Camera *c3 = new Camera(vec3(0.0f, 2.0f, -1.0f));
                Camera *c4 = new Camera(vec3(-2.0f, 2.0f, 0.0f));
                Camera *ac = new Camera(vec3(0.0f, 0.0f, 0.0f));
                Actor *a = Shape::actor(1.0f);

                cube->translate(vec3(0.0f, 0.7f, 0.0f));
                plane->set_color(0xAADD00);

                camviewer->config();
                camviewer->translate(vec3(-3.5f, 2.0f, -5.0f));
                camviewer->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                camviewer2->config();
                camviewer2->translate(vec3(0.0f, 2.0f, -5.0f));
                camviewer2->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                camviewer3->config();
                camviewer3->translate(vec3(3.5f, 2.0f, -5.0f));
                camviewer3->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                c2->look_at(cube->get_absolute_position());
                c3->look_at(cube->get_absolute_position());
                c4->look_at(cube->get_absolute_position());
                s->add_mesh(plane);
                s->add_mesh(cube);
                s->add_mesh(camviewer);
                s->add_mesh(camviewer2);
                s->add_mesh(camviewer3);

                a->set_follow_camera(ac);
                a->translate(vec3(1.5f, 1.0f, 0.0f));
                s->add_actor(a);
                a->set_on_select(change_controls_1person);
                a->set_on_deselect(change_controls_default);
                a->set_on_select(disable_collision_sphere);
                a->set_on_deselect(enable_collision_sphere);


                c->get_mesh()->set_on_select(__select_camera);
                c2->get_mesh()->set_on_select(__select_camera);
                c3->get_mesh()->set_on_select(__select_camera);
                c4->get_mesh()->set_on_select(__select_camera);

                int cam_id;
                cam_id = s->add_camera(c);
                cam_id = s->add_camera(c2);
                camviewer->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c3);
                camviewer2->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c4);

                cam_id = s->add_camera(ac);
                camviewer3->set_dynamic_camera(cam_id);

                s->set_on_collision(__unselect_all);
                s->set_on_collision(__select_obj);
                s->use_shader("shaders/simple_vs.glsl", "shaders/simple_fs.glsl");

                int shader_program = setShaders("shaders/texture_vs.glsl", "shaders/texture_fs.glsl");
                camviewer->set_shader(shader_program);
                camviewer2->set_shader(shader_program);
                camviewer3->set_shader(shader_program);

                return s;
        }
};
