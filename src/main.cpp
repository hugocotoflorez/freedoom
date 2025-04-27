#include <glad/glad.h>

#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "template.h"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <ctime>
#include <glm/glm.hpp>
using namespace glm;

#include "settings.h"

#if defined(SHOW_FPS) && SHOW_FPS
#define show_fps(...) printf(__VA_ARGS__)
#else
#define show_fps(...)
#endif

#define HexColor(hex_color)                     \
        ((hex_color & 0xFF0000) >> 16) / 256.0, \
        ((hex_color & 0xFF00) >> 8) / 256.0,    \
        ((hex_color & 0xFF)) / 256.0

#define color4split(c) \
        (vec4(c)).r,   \
        (vec4(c)).g,   \
        (vec4(c)).b,   \
        (vec4(c)).a

#define BG_COLOR HexColor(0x87CEEB), 1.0


GLuint width = 640;
GLuint height = 480;

Scene *main_scene;

double *
interframe_time()
{
        static double interframe_time = 0;
        return &interframe_time;
}

vec2
get_window_size()
{
        return vec2(width, height);
}

static void
framebuffer_size_callback(GLFWwindow *, int _width, int _height)
{
        width = _width;
        height = _height;
        glViewport(0, 0, width, height);
}

static void
fps()
{
        static time_t last_time = 0;
        static unsigned int fps = 0;
        time_t t;
        struct timespec tp;
        static struct timespec last_tp = { 0, 0 };

        if (clock_gettime(CLOCK_REALTIME, &tp) < 0)
                return;

        t = glfwGetTime();

        ++fps;

        *interframe_time() = tp.tv_sec - last_tp.tv_sec + (tp.tv_nsec - last_tp.tv_nsec) * 1e-9;
        last_tp = tp;

        if (t - last_time >= 1) {
                last_time = t;
                show_fps("[FPS] %u\n", fps);
                fps = 0;
        }
}


int
mainloop(GLFWwindow *window)
{
        main_scene = Template::plain_scene();
        main_scene->init();

        while (!glfwWindowShouldClose(window)) {
                main_scene->process_input(window);
                glfwPollEvents();

                fps();

                glClearColor(color4split(BG_COLOR));
                glClear(GL_COLOR_BUFFER_BIT);
                glClear(GL_DEPTH_BUFFER_BIT);

                main_scene->render();
                main_scene->draw();

                glfwSwapBuffers(window);
        }

        return 0;
}

int
main()
{
        if (!glfwInit()) {
                fprintf(stderr, "Can not init glfw\n");
                exit(1);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // GLFWmonitor *monitor = glfwGetPrimaryMonitor(); // fullscreen
        GLFWmonitor *monitor = NULL; // floating (or not)
        GLFWwindow *share = NULL;
        GLFWwindow *window = glfwCreateWindow(width, height, "Titulo", monitor, share);

        if (window == NULL) {
                perror("glfwCreateWindow");
                glfwTerminate(); // terminate initialized glfw
                return 1;
        }

        printf("GLFW version: %s\n", glfwGetVersionString());

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
                fprintf(stderr, "gladLoadGLLoader failed");
                glfwTerminate();
                return 1;
        }


#if defined(VSYNC)
        glfwSwapInterval(VSYNC);
#endif


        glClearColor(color4split(BG_COLOR));
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        mainloop(window);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
}
