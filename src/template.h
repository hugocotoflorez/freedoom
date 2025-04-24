#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "shape.h"

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

        static Scene plain_scene()
        {
                static Scene s = Scene();
                static Camera c = Camera(vec3(0.0f, 1.0f, 0.0f));
                static Mesh plane = Shape::plane(100);
                // static Mesh cube = Shape::cube(1);
                s.add_camera(c);
                s.add_mesh(plane);
                // s.add_mesh(cube);
                return s;
        }
};
