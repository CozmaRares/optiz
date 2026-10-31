// NOLINTBEGIN(bugprone-suspicious-missing-comma)
#include <stdbool.h>
#include <stdlib.h>

#define NOB_UNSTRIP_PREFIX
#define NOB_IMPLEMENTATION
#include "vendor/tsoding/nob.h/nob.h"

#define BUILD   "build/"
#define SRC     "src/"
#define INCLUDE "-I" SRC

const char* SRCS[] = {
    SRC "main.cpp",
    BUILD "lexer.cpp"
};
const int SRCS_LEN = NOB_ARRAY_LEN(SRCS);

const char* INCLUDES[] = {
    INCLUDE "include"
};
const int INCLUDES_LEN = NOB_ARRAY_LEN(INCLUDES);

#define run(cmd, ...)                      \
    if (!nob_cmd_run(&cmd, __VA_ARGS__)) { \
        nob_cmd_free(cmd);                 \
        return 1;                          \
    }

#define append(cmd, ...)      nob_cmd_append(&cmd, __VA_ARGS__)
#define append_many(cmd, ...) nob_da_append_many(&cmd, __VA_ARGS__)

void rebuild_flags();
void setup(int argc, char** argv);

struct {
    char* program;
    bool should_run;
} args;
void parse_args(int argc, char** argv);

int main(int argc, char** argv) {
    setup(argc, argv);

    Nob_Cmd cmd = { 0 };
    append(cmd, "flex", "-o", BUILD "lexer.cpp", SRC "lexer.l");
    run(cmd);

    append(cmd, "clang++", "-Wall", "-Wextra");
    append_many(cmd, INCLUDES, INCLUDES_LEN);
    append(cmd, "-o", BUILD "optiz");
    append_many(cmd, SRCS, SRCS_LEN);
    run(cmd);

    if (args.should_run) {
        append(cmd, BUILD "optiz");
        run(cmd);
    }
    nob_cmd_free(cmd);

    return 0;
}

void rebuild_flags() {
    int flags_need_rebuild = nob_needs_rebuild1("compile_flags.txt", __FILE__);
    if (flags_need_rebuild < 0) {
        exit(1);
    }

    if (!flags_need_rebuild) {
        nob_log(NOB_INFO, "compile_flags.txt doesn't need to be rebuilt");
        return;
    }

    nob_log(NOB_INFO, "Rebuilding compile_flags.txt");
    Nob_String_Builder flags = { 0 };
    for (int i = 0; i < INCLUDES_LEN; ++i) {
        nob_sb_append_cstr(&flags, INCLUDES[i]);
        nob_sb_append_cstr(&flags, "\n");
    }
    bool ok = nob_write_entire_file("compile_flags.txt", flags.items, flags.count);
    nob_sb_free(flags);

    if (!ok) {
        exit(1);
    }
    nob_log(NOB_INFO, "Wrote %d include flags to compile_flags.txt", INCLUDES_LEN);
}

void print_help() {
    nob_log(NOB_INFO, "Usage: %s [run]", args.program);
}

void parse_args(int argc, char** argv) {
    args.program = nob_shift(argv, argc);

    args.should_run = false;
    while (argc > 0) {
        const char* arg = nob_shift(argv, argc);
        if (strcmp(arg, "run") == 0) {
            args.should_run = true;
        } else {
            nob_log(NOB_ERROR, "Unknown argument: %s", arg);
            print_help();
            exit(1);
        }
    }
}

void setup(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    parse_args(argc, argv);
    rebuild_flags();

    if (!nob_mkdir_if_not_exists(BUILD)) {
        exit(1);
    }
}

// NOLINTEND(bugprone-suspicious-missing-comma)
