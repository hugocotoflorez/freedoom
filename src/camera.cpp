#include <cassert>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "camera.h"
#include "mesh.h"
#include "shape.h"

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
        if (mesh)
                mesh->look_at(pos);
}

/* Place camera in POS. It is an absolute position. To move the camera in a relative
 * way use transform(). */
void
Camera::place(vec3 pos)
{
        position = pos;
        __recalculate_view();
        if (mesh) {
                mesh->set_model(mesh->get_rotation_matrix());
                mesh->translate(pos);
        }
}

void
Camera::transform(vec3 pos)
{
        position += pos;
        __recalculate_view();
        if (mesh)
                mesh->translate(pos);
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

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
}

/* Draw a camera-like object that represents the camera position and looking direction */
void
Camera::draw()
{
        if (mesh) {
                mesh->draw();
                return;
        }

        mesh = Shape::camera();
        mesh->set_scene(scene);
        mesh->set_shader(setShaders("shaders/camera_vs.glsl", "shaders/camera_fs.glsl"));
        mesh->set_color(0x404040);

        mesh->translate(position);
        mesh->look_at(looking_at);
        mesh->draw();
}
