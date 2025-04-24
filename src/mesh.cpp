#include "mesh.h"
#include "camera.h"
#include "scene.h"
#include <cstdio>
#include <cstring>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LOG_PRINT 0
#include "../thirdparty/frog/frog.h"

void
Mesh::show()
{
        printable = true;
}

void
Mesh::hide()
{
        printable = false;
}

void
Mesh::set_vao(GLuint _vao, GLuint _indexes_n)
{
        vao = _vao;
        indexes_n = _indexes_n;
}

Mesh &
Mesh::
set_shader(GLuint shader)
{
        __shader = shader;
        return *this;
}

const char *
Mesh::
get_name()
{
        return name;
}

vec3
Mesh::get_position()
{
        return vec3(model[3]);
}

mat4
Mesh::get_absolute_model()
{
        if (parent) {
                return parent->get_absolute_model() * model;
        }
        return model;
}

vec3
Mesh::get_absolute_position()
{
        return vec3(get_absolute_model()[3]);
}

mat3
Mesh::get_rotation_matrix()
{
        return mat3(model);
}

vec3
Mesh::get_rotation()
{
        return eulerAngles(quat_cast(mat3(model)));
}

mat3
Mesh::get_default_rotation_matrix()
{
        return mat3(default_model);
}

mat4
Mesh::get_model()
{
        return model;
}

Mesh &
Mesh::
set_model(mat4 _model)
{
        model = _model;
        return *this;
}

Mesh &
Mesh::
set_before_draw_function(void (*_before_draw)(Mesh *))
{
        before_draw = _before_draw;
        return *this;
}

unsigned int
Mesh::
get_shader()
{
        return __shader;
}

Mesh &
Mesh::
rotate(float angle, vec3 v)
{
        model = glm::rotate(model, angle, v);
        return *this;
}

Mesh &
Mesh::
translate(vec3 v)
{
        model = glm::translate(model, v);
        return *this;
}

void
Mesh::look_at(vec3 view_pos)
{
        vec3 mesh_pos = get_position();
        mat3 rotation = get_default_rotation_matrix();
        vec3 dir = normalize(vec3(view_pos.x - mesh_pos.x, 0.0, view_pos.z - mesh_pos.z));
        float angle = atan2(dir.x, dir.z);
        mat4 _model = mat4(1.0f);
        _model = glm::translate(_model, mesh_pos);
        _model = glm::rotate(_model, angle, vec3(0.0, 1.0, 0.0));
        _model *= mat4(rotation);
        model = _model;
}

void
Mesh::draw(mat4 _model)
{
        _model = _model * model;

        printf("Model: %s -- vao:%d, indexes:%d (shader:%d)\n", name, vao, indexes_n, __shader);

        for (Mesh *attached_mesh : attached) {
                (*attached_mesh).draw(_model);
        }

        //if (!printable) return;
        if (before_draw) before_draw(this);

        glUseProgram(__shader);

        int camera_id = 0;
        Camera cam = ((Scene *) scene)->get_camera(camera_id);
        cam.set_camera(__shader);

        glUniform3f(glGetUniformLocation(__shader, "color"), HexColor(color));

        glUniformMatrix4fv(glGetUniformLocation(__shader, "model"), 1, GL_FALSE, value_ptr(_model));

        glBindVertexArray(vao);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, indexes_n, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        //glBindTexture(GL_TEXTURE_2D, 0);
}

void
Mesh::attach(Mesh *child)
{
        if (child->parent) return;
        attached.push_back(child);
        child->parent = this;
}

void
Mesh::set_before_draw(Mesh *mesh, void (*func)(Mesh *))
{
        mesh->before_draw = func;
}

bool
Mesh::is_attached()
{
        return parent != nullptr;
}

void
Mesh::set_scene(void *s)
{
        scene = s;
}

void
Mesh::delete_vao()
{
        glDeleteVertexArrays(1, &vao);
}

Mesh &
Mesh::
init()
{
        default_model = model;
        return *this;
}
