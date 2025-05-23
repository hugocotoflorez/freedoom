#define FROG_IMPLEMENTATION
#include "thirdparty/frog/frog.h"

#define FILENAME "freedoom"
#define FLAGS "-Wall", "-Wextra", "-Wno-unused-parameter", "-Wno-unused-but-set-variable", "-ggdb", "-Wno-write-strings"
#define LIBS "-lglad", "-lglfw"
#define CC "g++"

int
main(int argc, char *argv[])
{
        frog_rebuild_itself(argc, argv);

        frog_cmd_filtered_foreach("thirdparty/load_obj", ".*cpp", CC, FLAGS, "-c");
        frog_cmd_filtered_foreach("thirdparty/vshcfp/src", ".*c$", "gcc" , FLAGS, "-c");
        frog_cmd_filtered_foreach("src", ".*cpp", CC, FLAGS, "-c");
        frog_makedir("objs");
        frog_shell_cmd("mv *.o objs")
        frog_shell_cmd("g++ objs/* -lglad -lglfw -ggdb -o " FILENAME);
        frog_cmd_wait("rm", "objs", "-r");

        // UNREACHABLE("Just do not execute");

        frog_cmd_wait("sudo", "systemctl", "stop", "keyd");
        frog_cmd_wait("./" FILENAME);
        frog_cmd_wait("sudo", "systemctl", "start", "keyd");
        frog_cmd_wait("rm", FILENAME);

        return 0;
}
