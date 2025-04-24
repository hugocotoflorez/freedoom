#ifndef MESH_H_
#define MESH_H_

#define HexColor(hex_color)                     \
        ((hex_color & 0xFF0000) >> 16) / 256.0, \
        ((hex_color & 0xFF00) >> 8) / 256.0,    \
        ((hex_color & 0xFF)) / 256.0

#include <glad.h>

#include <glm/glm.hpp>
#include <vector>

using namespace glm;
using namespace std;

class Mesh
{
    private:
        const char *name;
        GLuint vao = 0;
        GLuint indexes_n = 0;
        bool printable;
        int color;
        mat4 model; // relative model
        mat4 default_model; // default starting relative model
        vector<Mesh *> attached;
        Mesh *parent;
        unsigned int __shader;
        void *scene;
        void (*before_draw)(Mesh *);

    public:
        Mesh(const char *name, int _color = 0xFFFFFF, bool _printable = true)
        : name(name),
          printable(_printable),
          color(_color),
          model(mat4(1.0f)),
          default_model(mat4(1.0f)),
          attached(),
          parent(nullptr),
          __shader(0),
          scene(nullptr),
          before_draw(nullptr)
        {
        }

        void set_vao(GLuint _vao, GLuint _indexes_n);
        void show();
        void hide();
        Mesh &set_shader(GLuint shader);
        const char *get_name();
        vec3 get_position();
        mat4 get_absolute_model();
        vec3 get_absolute_position();
        mat3 get_rotation_matrix();
        vec3 get_rotation();
        mat3 get_default_rotation_matrix();
        mat4 get_model();
        Mesh &set_model(mat4 _model);
        Mesh &set_before_draw_function(void (*_before_draw)(Mesh *));
        unsigned int get_shader();
        Mesh &rotate(float angle, vec3 v);
        Mesh &translate(vec3 v);
        void look_at(vec3 view_pos);
        void draw(mat4 _model = mat4(1.0f));
        void attach(Mesh *child);
        void set_before_draw(Mesh *mesh, void (*func)(Mesh *));
        bool is_attached();
        void delete_vao();
        void set_scene(void *);
        Mesh &init();
};

#endif
