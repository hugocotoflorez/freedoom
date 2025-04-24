#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "camera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
using namespace glm;


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
}

/* Place camera in POS. It is an absolute position. To move the camera in a relative
 * way use transform(). */
void
Camera::place(vec3 pos)
{
        position = pos;
        __recalculate_view();
}

void
Camera::transform(vec3 pos)
{
        position += pos;
        __recalculate_view();
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
Camera::set_camera(int shader)
{
        GLuint viewLoc = glGetUniformLocation(shader, "view");
        GLuint projectionLoc = glGetUniformLocation(shader, "projection");
        mat4 projection = perspective(radians(45.0f), (float) 4 / (float) 3, 0.1f, 1000.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
}
