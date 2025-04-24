#include <glad/glad.h>

#include "template.h"
#include "camera.h"
#include "mesh.h"
#include "scene.h"

#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdio>
#include <glm/glm.hpp>
using namespace glm;

/* Enable VSync. FPS limited to screen refresh rate
 * (0: disable, 1: enable, undef: default) */
#define VSYNC 0

/* Mouse sensibility */
#define MOUSE_SENS_X 2.0f
#define MOUSE_SENS_Y 2.0f

/* Show fps if SHOW_FPS is defined and not 0 */
#define SHOW_FPS 0

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

GLuint WIDTH = 640;
GLuint HEIGHT = 480;

Scene main_scene;

static void
process_mouse(GLFWwindow *window)
{
        static bool initialized = false;
        static double prev_x;
        static double prev_y;
        double xpos;
        double ypos;
        double xoffset;
        double yoffset;

        glfwGetCursorPos(window, &xpos, &ypos);
        if (!initialized) {
                prev_x = xpos;
                prev_y = ypos;
                initialized = false;
        }

        xoffset = xpos - prev_x;
        yoffset = ypos - prev_y;
        prev_x = xpos;
        prev_y = ypos;
}

static void
process_input(GLFWwindow *window)
{
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(window, true);
}

static void
framebuffer_size_callback(GLFWwindow *, int width, int height)
{
        WIDTH = width;
        HEIGHT = height;
        glViewport(0, 0, width, height);
}

static double *
interframe_time()
{
        static double interframe_time = 0;
        return &interframe_time;
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
#if defined(SHOW_FPS) && SHOW_FPS
                printf("[FPS] %u\n", fps);
#endif
                fps = 0;
        }
}


int
mainloop(GLFWwindow *window)
{
        main_scene = Template::plain_scene();
        main_scene.init();
        main_scene.use_shader("shaders/simple_vs.glsl", "shaders/simple_fs.glsl");
        while (!glfwWindowShouldClose(window)) {
                process_input(window);
                process_mouse(window);
                glfwPollEvents();

                fps();
                main_scene.draw();

                glClearColor(color4split(BG_COLOR));
                glClear(GL_COLOR_BUFFER_BIT);
                glClear(GL_DEPTH_BUFFER_BIT);

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
        GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Titulo", monitor, share);

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

        /* Mouse stuff */
        // if (glfwRawMouseMotionSupported())
        //         glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glClearDepth(1.0f);
        glClearColor(color4split(BG_COLOR));
        glEnable(GL_DEPTH_TEST);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_BACK);

        mainloop(window);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
}
