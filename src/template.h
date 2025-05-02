#include "actor.h"
#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "settings.h"
#include "shape.h"

#include "setShaders.h"
#include <cassert>
#include <cmath>
#include <cstdlib>
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

static bool
calc_x2(float x1, float y1, float y2, float &x2)
{
        float value = x1 * x1 + y1 * y1 - y2 * y2;

        if (value < 0) {
                return false;
        }

        x2 = fabsl(sqrt(value));
        return true;
}

extern double *interframe_time();
extern int mouse_mode;
Mesh *crosshair;

static void
__1person_handler(GLFWwindow *window, Scene *scene)
{
        /* Esto no deberia ir aqui */
        if (mouse_mode != GLFW_CURSOR_DISABLED) {
                mouse_mode = GLFW_CURSOR_DISABLED;
                if (glfwRawMouseMotionSupported())
                        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

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
        if (calc_x2(X, Y, Y + J, I)) {
                actor->set_camera_yoffset(Y + J);
                actor->set_camera_xoffset(I);
        } else {
                // printf("No existe solucion para x2\n");
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

#define SPEED 1.0f
#define T (static_cast<float>(*interframe_time()))


        mat4 m = actor->get_absolute_model();
        vec3 dirf = -normalize(vec3(m[2])) * SPEED;
        vec3 right = normalize(vec3(m[0])) * SPEED;
        // vec3 up = normalize(vec3(m[1])) * SPEED;
        vec3 posi = actor->get_position();

        static vec3 movement = vec3(0.0f);
        static vec3 current_speed = vec3(0.0f);
        current_speed *= pow(0.001f, T);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                current_speed += dirf;

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                current_speed -= dirf;

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                current_speed -= right;

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                current_speed += right;

#define JUMP_VEL 8.0f
#define G 20.0f

        static bool sp_lock = false;
        static float vertical_velocity = 0.0f;

        // Iniciar el salto
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !sp_lock && actor->is_bottom_colliding()) {
                actor->set_jumping(true);
                vertical_velocity = JUMP_VEL;
                sp_lock = true;
        }

        // Si está en el aire, aplicar gravedad
        if (actor->is_jumping() && !actor->is_bottom_colliding()) {
                vertical_velocity -= G * T;
        }

        // Aplicar la velocidad vertical
        current_speed.y = vertical_velocity;

        // Aplicar movimiento
        movement = current_speed * T;
        actor->move(movement);

        // Si toca el suelo, resetear el salto
        if (actor->is_bottom_colliding()) {
                vec3 pos = actor->get_absolute_position();
                actor->place(vec3(pos.x, 1.5f, pos.z)); // opcional: ajusta la altura base del personaje
                actor->set_jumping(false);
                vertical_velocity = 0.0f;
                current_speed.y = 0.0f;
                sp_lock = false;
        }
}

static void
__select_obj(SphereCollider *c)
{
        // printf("Selecting %s\n", c->get_parent()->get_name());
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
        // printf("Calling __select_camera\n");
        ((Scene *) m->get_scene())->set_camera((Camera *) m->get_attached_camera());
}

static void
__move(Mesh *m)
{
        static vec3 position = m->get_position();
        static bool init = false;
        if (!init)
                srand(time(NULL));
        int y = rand() % 5 - 2;
        int z = rand() % 5 - 2;
        m->place(vec3(position.x, position.y + y, position.z + z));
        m->enable_sphere_collider();
        m->deselect();
}

static void
change_controls_default(Mesh *mesh)
{
        crosshair->hide();
        Scene *scene = (Scene *) mesh->get_scene();
        scene->get_actor()->deselect();
        scene->set_handler(scene->get_default_handler());
        scene->set_camera((GLuint) 0);
        // printf("Change handler -> default_handler\n");
}

static void
disable_depth(Mesh *)
{
        glDisable(GL_DEPTH_TEST);
}

static void
change_controls_1person(Mesh *mesh)
{
        Scene *scene = (Scene *) mesh->get_scene();


        Actor *actor = scene->get_actor();
        if (actor == nullptr) {
                // printf("1st person actor missing!\n");
                return;
        }
        scene->set_camera(actor->get_following_camera());
        scene->set_handler(__1person_handler);
        crosshair->show();
        // printf("Change handler -> __1person_handler\n");
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
                Mesh *sphere1 = Shape::sphere(0.25);
                CamView *camviewer = Shape::camviewer_plane(3);
                CamView *camviewer2 = Shape::camviewer_plane(3);
                CamView *camviewer3 = Shape::camviewer_plane(3);
                Camera *c = new Camera(vec3(0.0f, 10.0f, 10.0f));
                Camera *c2 = new Camera(vec3(-2.0f, 0.5f, 0.0f));
                Camera *c3 = new Camera(vec3(0.0f, 2.0f, -1.0f));
                Camera *c4 = new Camera(vec3(-2.0f, 2.0f, 0.0f));
                Camera *ac = new Camera(vec3(0.0f, 0.0f, 0.0f));
                Actor *a = Shape::actor(0.5, 0.5, 0.5);
                Mesh *abody = Shape::cube_nocollider(0.5, 1.0, 0.5);
                crosshair = Shape::crosshair(0);

                Mesh *weapon = new Mesh("Weapon", 0x000000, true, false, false);
                // Mesh *piano = new Mesh("piano", 0xabc012, true, false, false);

                float _scale = 5.0f;
                weapon->set_model(scale(weapon->get_model(), vec3(_scale, _scale, _scale)));
                weapon->rotate(PIMED, vec3(0.0f, 1.0f / _scale, 0.0f));
                weapon->translate(vec3(0.1f, 0.1f, 0.1f));

                // cube->translate(vec3(0.0f, 0.7f, 0.0f));
                plane->set_color(0xAADD00);

                sphere1->translate(vec3(8.0f, 2.0f, 0.0f));

                crosshair->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));
                crosshair->set_color(0xABC000);
                crosshair->disable_sphere_collider();
                crosshair->set_before_draw_function(disable_depth);

                camviewer->config();
                camviewer->translate(vec3(-3.5f, 2.0f, -5.0f));
                camviewer->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                camviewer2->config();
                camviewer2->translate(vec3(0.0f, 2.0f, -5.0f));
                camviewer2->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                camviewer3->config();
                camviewer3->translate(vec3(3.5f, 2.0f, -5.0f));
                camviewer3->rotate(PIMED, vec3(1.0f, 0.0f, 0.0f));

                c2->get_mesh()->set_on_select(change_controls_default);
                c3->get_mesh()->set_on_select(change_controls_default);
                c4->get_mesh()->set_on_select(change_controls_default);
                cube->set_on_select(change_controls_default);

                c2->look_at(cube->get_absolute_position());
                c3->look_at(cube->get_absolute_position());
                c4->look_at(cube->get_absolute_position());
                s->add_mesh(plane);
                s->add_mesh(cube);
                s->add_mesh(sphere1);
                s->add_mesh(camviewer);
                s->add_mesh(camviewer2);
                s->add_mesh(camviewer3);

                a->set_follow_camera(ac);
                a->translate(vec3(1.5f, 1.5f, 0.0f));
                abody->translate(vec3(0.0f, -1.0f, 0.0f));
                s->add_actor(a);
                a->attach(abody);
                abody->attach(weapon);
                a->set_on_select(change_controls_1person);

                weapon->import_obj("assets/Untitled.obj");
                // s->add_mesh(piano);
                // cube->import_obj("assets/WoodenChessSet.obj");
                cube->import_obj("assets/grand_piano_(OBJ).obj");

                c->get_mesh()->set_on_select(__select_camera);
                c2->get_mesh()->set_on_select(__select_camera);
                c3->get_mesh()->set_on_select(__select_camera);
                c4->get_mesh()->set_on_select(__select_camera);

                crosshair->add_texture_image("textures/crosshair.png");


                int cam_id;
                cam_id = s->add_camera(c);
                // c->get_mesh()->hide();
                // c->get_mesh()->disable_sphere_collider();

                cam_id = s->add_camera(c2);
                camviewer->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c3);
                camviewer2->set_dynamic_camera(cam_id);
                cam_id = s->add_camera(c4);

                cam_id = s->add_camera(ac);
                camviewer3->set_dynamic_camera(cam_id);

                s->set_on_collision(__unselect_all);
                s->set_on_collision(__select_obj);
                s->set_on_select(disable_collision_sphere);
                s->set_on_deselect(enable_collision_sphere);
                s->use_shader("shaders/texture_vs.glsl", "shaders/texture_fs.glsl");

                s->add_mesh(crosshair);

                sphere1->set_on_select(__move);

                int shader_program;
                shader_program = setShaders("shaders/texture_vs.glsl", "shaders/texture_fs.glsl");
                assert(shader_program > 0);
                weapon->set_shader(shader_program);
                // piano->set_shader(shader_program);
                camviewer->set_shader(shader_program);
                camviewer2->set_shader(shader_program);
                camviewer3->set_shader(shader_program);
                abody->set_shader(shader_program);

                shader_program = setShaders("shaders/crosshair_vs.glsl", "shaders/crosshair_fs.glsl");
                assert(shader_program > 0);
                crosshair->set_shader(shader_program);

                // clang-format off
                s->create_skybox({
                                "./textures/cubemap/posx.jpg",
                                "./textures/cubemap/negx.jpg",
                                "./textures/cubemap/posy.jpg",
                                "./textures/cubemap/negy.jpg",
                                "./textures/cubemap/posz.jpg",
                                "./textures/cubemap/negz.jpg",
                                });
                // clang-format on

                change_controls_default(a);

                return s;
        }
};
