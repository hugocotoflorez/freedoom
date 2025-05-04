#ifndef SETTINGS_H_
#define SETTINGS_H_

/* This code is quite weird as I migrate the original settings.h
 * that was a file of defines to vshcfp and this is the easiest
 * way to do that I think */


/* Mouse sensibility */
static float MOUSE_SENS_X = 2.0f;
static float MOUSE_SENS_Y = 2.0f;
static float MOUSE_FIRST_PERSON_SENS_X = 2.0f;
static float MOUSE_FIRST_PERSON_SENS_Y = 4.0f;
static float MOUSE_REDUCTION = 0.001f;

static bool SHOW_FPS = true;

/* Enable VSync. FPS limited to screen refresh rate */
static bool VSYNC = true;

static bool show_collision_point = false;
static bool draw_collision_sphere = false;

extern "C" {
/* Literally fuck cpp */
#include "../thirdparty/vshcfp/include/vshcfp.h"
}

#include <cstdio>

#include "cstdlib"
#include "cstring"

__attribute__((constructor)) static void
__init__()
{
        HcfOpts opts;
        char *s;

        opts = hcf_load("settings.hcf");

        if ((s = hcf_get(opts, "mouse", "sens_x"))) MOUSE_SENS_X = atof(s);
        if ((s = hcf_get(opts, "mouse", "sens_y"))) MOUSE_SENS_Y = atof(s);
        if ((s = hcf_get(opts, "mouse", "sens_x"))) MOUSE_FIRST_PERSON_SENS_X = atof(s);
        if ((s = hcf_get(opts, "mouse", "sens_y"))) MOUSE_FIRST_PERSON_SENS_Y = atof(s);
        if ((s = hcf_get(opts, "mouse", "reduction"))) MOUSE_REDUCTION = atof(s);
        if ((s = hcf_get(opts, "options", "show_fps"))) SHOW_FPS = 0 == strcmp(s, "on");
        if ((s = hcf_get(opts, "options", "vsync"))) VSYNC = 0 == strcmp(s, "on");

        if ((s = hcf_get(opts, "dev", "show_collision_point"))) show_collision_point = 0 == strcmp(s, "on");
        if ((s = hcf_get(opts, "dev", "draw_collision_sphere"))) draw_collision_sphere = 0 == strcmp(s, "on");

        hcf_destroy(&opts);
}


#endif // SETTINGS_H_
