#include <cassert>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "scene.h"
#include "shape.h"

#include "camera.h"
#include "mesh.h"

#include "setShaders.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
using namespace glm;

extern GLuint width;
extern GLuint height;

void
Camera::__recalculate_view()
{
        view = lookAt(position, looking_at, up);
}

/* Look from current position to POS. To change current position use place() */
void
Camera::look_at(vec3 pos)
{
        looking_at = pos;
        __recalculate_view();
        if (mesh) {
                mesh->look_at(looking_at);
        }
}

Mesh *
Camera::get_mesh()
{
        return mesh;
}

/* Place camera in POS. It is an absolute position. To move the camera in a relative
 * way use transform(). */
void
Camera::place(vec3 pos)
{
        position = pos;
        __recalculate_view();
        if (mesh) {
                mesh->place(pos);
                mesh->look_at(looking_at);
        }
}

void
Camera::transform(vec3 pos)
{
        //printf("Moving camera by %f,%f,%f\n", pos.x, pos.y, pos.z);
        position += pos;
        if (mesh) {
             mesh->place(position);
             mesh->look_at(looking_at);
        }
}

vec3
Camera::get_position()
{
        return position;
}

mat4
Camera::get_view()
{
        return view;
}

void
Camera::set_scene(void *s)
{
        scene = s;
        if (mesh)
                mesh->set_scene(s);
}

void
Camera::axis_rotate(vec3 rotation)
{
        mat4 model = mat4(1.0f);
        model = rotate(model, rotation.x, vec3(0.0f, 1.0f, 0.0f));
        position = vec3(model * vec4(position, 0));
        position.y += rotation.y;
        place(position);

        __recalculate_view();
}

void
Camera::init_mesh()
{
        mesh = Shape::camera();
        mesh->set_shader(setShaders("shaders/camera_vs.glsl", "shaders/camera_fs.glsl"));
        mesh->set_color(0x404040);
        mesh->attach_camera(this);

        mesh->translate(position);
        mesh->look_at(looking_at);
}

mat4
Camera::get_projection()
{
        return perspective(radians(45.0f), (float) width / (float) height, 0.3f, 1000.0f);
}

void
Camera::set_camera(int shader)
{
        GLuint viewLoc = glGetUniformLocation(shader, "view");
        GLuint projectionLoc = glGetUniformLocation(shader, "projection");
        mat4 projection = get_projection();

        __recalculate_view();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
}
