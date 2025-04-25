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

class Mesh;

class SphereCollider
{
    private:
        float radius;
        Mesh *parent;

    public:
        SphereCollider(float radius, Mesh *parent)
        : radius(0.0f),
          parent(nullptr)
        {
        }


        float get_radius()
        {
                return radius;
        }

        vec3 get_position();
};

class Mesh
{
    public:
        const bool need_render;

    private:
        const char *name;
        GLuint vao;
        GLuint indexes_n;
        bool printable;
        int color;
        mat4 model; // relative model
        mat4 default_model; // default starting relative model
        vector<Mesh *> attached;
        vector<GLuint> textures;
        Mesh *parent;
        unsigned int __shader;
        void (*before_draw)(Mesh *);
        SphereCollider *sphere_collider;

    protected:
        void *scene;
        int dynamic_camera;

    public:
        Mesh(const char *name, int _color = 0xFFFFFF, bool _printable = true, bool render = false)
        : need_render(render),
          name(name),
          vao(0),
          indexes_n(0),
          printable(_printable),
          color(_color),
          model(mat4(1.0f)),
          default_model(mat4(1.0f)),
          attached(),
          parent(nullptr),
          __shader(0),
          before_draw(nullptr),
          scene(nullptr),
          dynamic_camera(-1),
          sphere_collider(nullptr)
        {
                new SphereCollider(1.0f, this);
        }

        void set_vao(GLuint _vao, GLuint _indexes_n);
        void show();
        void hide();
        Mesh &set_shader(GLuint shader);
        const char *get_name();
        vec3 get_position();
        mat4 get_absolute_model();
        vec3 get_absolute_position();
        mat4 get_default_absolute_model();
        vec3 get_default_absolute_position();
        mat3 get_rotation_matrix();
        vec3 get_rotation();
        mat3 get_default_rotation_matrix();
        mat4 get_model();
        Mesh &set_model(mat4 _model);
        Mesh &set_before_draw_function(void (*_before_draw)(Mesh *));
        unsigned int get_shader();
        Mesh &rotate(float angle, vec3 v);
        Mesh &translate(vec3 v);
        void look_at2d(vec3 view_pos);
        void look_at(vec3 view_pos);
        void draw(mat4 _model = mat4(1.0f));
        void attach(Mesh *child);
        void set_before_draw(Mesh *mesh, void (*func)(Mesh *));
        bool is_attached();
        void delete_vao();
        void set_scene(void *);
        void set_color(int);
        void init();
        void set_dynamic_camera(int);
        void *get_scene();
        void add_texture(GLuint);
        SphereCollider *get_sphere_collider();
};

#endif
