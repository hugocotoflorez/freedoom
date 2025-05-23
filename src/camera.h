#ifndef CAMERA_H_
#define CAMERA_H_

#include <cstddef>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
using namespace glm;

#include "mesh.h"
#include "setShaders.h"

/* Camera: set view points */
class Camera
{
    private:
        vec3 position;
        vec3 up;
        mat4 view;
        vec3 looking_at;
        Mesh *mesh;
        void *scene;

        void __recalculate_view();
        void init_mesh();

    public:
        Camera(vec3 pos, vec3 _up = vec3(0.0f, 1.0f, 0.0f), vec3 _at = vec3(0.0f, 0.0f, 0.0f))
        : position(pos),
          up(_up),
          looking_at(_at),
          mesh(nullptr)
        {
                __recalculate_view();
                init_mesh();
        }

        ~Camera()
        {
        }

        /* Look from current position to POS. To change current
         * position use place() */
        void look_at(vec3 pos);

        /* Place camera in POS. It is an absolute position.
         * To move the camera in a relative way use transform(). */
        void place(vec3 pos);

        /* Modify camera place by POS. It is an relative position.
         * To place the camera in absolute coords, use place(). */
        void transform(vec3 pos);
        vec3 get_position();
        mat4 get_view();
        void set_camera(int shader);
        void set_scene(void *s);
        mat4 get_projection();
        Mesh *get_mesh();

        void set_up(vec3 u)
        {
                up = u;
        }

        /* Rotate around given axis */
        void axis_rotate(vec3);
};

#endif
