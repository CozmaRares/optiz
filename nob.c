// NOLINTBEGIN(bugprone-suspicious-missing-comma)
#define NOB_IMPLEMENTATION
#include "vendor/tsoding/nob.h/nob.h"

#define BUILD "build/"
#define SRC   "src/"

const char* SRCS[] = {
    SRC "main.cpp"
};

int main(int argc, char** argv) {
    GO_REBUILD_URSELF(argc, argv);

    if (!nob_mkdir_if_not_exists(BUILD))
        return 1;

    Cmd cmd = { 0 };
    cmd_append(&cmd, "clang++", "-Wall", "-Wextra", "-o", BUILD "optiz");
    da_append_many(&cmd, SRCS, ARRAY_LEN(SRCS));
    bool ok = cmd_run(&cmd);
    cmd_free(cmd);

    return !ok;
}
// NOLINTEND(bugprone-suspicious-missing-comma)
