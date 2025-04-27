#ifndef SCENE_H_
#define SCENE_H_

#include <glm/ext/vector_float2.hpp>
#include <vector>

#include "actor.h"
#include "camera.h"
#include "mesh.h"

#include <GLFW/glfw3.h>

#include "settings.h"

static bool locks[1024] = { 0 };
#define oneclick(__window, __key)                                            \
                glfwGetKey(__window, __key) == GLFW_RELEASE && locks[__key]) \
        {                                                                    \
                locks[__key] = false;                                        \
        }                                                                    \
        if (glfwGetKey(__window, __key) == GLFW_PRESS &&!locks[__key] &&(locks[__key]= true)

class Scene;
static void cam_movement_input_handler(GLFWwindow *window, Scene *scene);


class Scene
{
    private:
        vector<Camera *> cameras;
        vector<Mesh *> meshes;
        int camera_index;
        void (*_input_handler)(GLFWwindow *, Scene *);
        Actor *actor; // todo: handle actors in a proper way

    public:
        Scene()
        : camera_index(0),
          _input_handler(cam_movement_input_handler),
          actor(nullptr)
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
        void set_on_collision(void (*_on_collide)(SphereCollider *));


        void (*get_default_handler())(GLFWwindow *, Scene *)
        {
                return cam_movement_input_handler;
        }

        void set_handler(void (*handler)(GLFWwindow *, Scene *))
        {
                _input_handler = handler;
        }

        void process_input(GLFWwindow *window)
        {
                _input_handler(window, this);
        }

        void unselect_all()
        {
                for (auto m : meshes)
                        if (m->selected())
                                m->deselect();
        }

        void add_actor(Actor *a)
        {
                actor = a;
                add_mesh(a);
        }

        Actor *get_actor()
        {
                return actor;
        }

        void set_camera(Camera *c)
        {
                for (int i = 0; i < cameras.size(); i++) {
                        if (c == cameras.at(i)) {
                                set_camera(i);
                                return;
                        }
                }
        }

        void set_camera(GLuint);
};

/* From main as it should be a class that control state
 * to handle all this stuff but there isnt. */
extern double *interframe_time();
extern vec2 get_window_size();

static void
cam_movement_input_handler(GLFWwindow *window, Scene *scene)
{
        /* Esto no deberia ir aqui */
        if (glfwRawMouseMotionSupported())
                glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);

        static bool initialized = false;
        static float xprev, yprev;
        double _xpos, _ypos;

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

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                scene->get_camera()->axis_rotate(vec3(MOUSE_SENS_X * xoffset * MOUSE_REDUCTION,
                                                      MOUSE_SENS_Y * yoffset * MOUSE_REDUCTION * 10.0f,
                                                      0.0f));
        }

        static bool lock_l = false;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !lock_l) {
                /* The *2.0f - 1.0 is to transform from [0,1] to [-1, 1] */
                vec2 mouse = (vec2(xpos, ypos) / get_window_size()) * 2.0f - 1.0f;
                mouse.y = -mouse.y; // origin is top-left and +y mouse is down
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
}

#endif // SCENE_H_
