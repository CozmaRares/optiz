// NOLINTBEGIN(bugprone-suspicious-missing-comma)
#include <stdbool.h>
#include <stdlib.h>

#include "config.h"

#define NOB_IMPLEMENTATION
#define NOB_REBUILD_URSELF(binary_path, source_path) "clang", "-o", binary_path, source_path
#include "../vendor/tsoding/nob.h/nob.h"

const char* const SRCS[] = {
    SRC "main.cpp",
    SRC "ast_printer.cpp",
    SRC "parser.cpp",
    BUILD "lexer.cpp"
};
const int SRCS_LEN = sizeof(SRCS) / sizeof(SRCS[0]);

void setup(int argc, char** argv);

void build_flags();
void build_lexer();
void build_optiz();
void run_optiz();

typedef struct {
    char* program;
    bool should_run;
} Program_Args;

Program_Args parse_args(int argc, char** argv);

int main(int argc, char** argv) {
    GO_REBUILD_URSELF_PLUS(argc, argv, SRC_BUILD "config.h");

    Program_Args args = parse_args(argc, argv);

    if (!mkdir_if_not_exists(BUILD)) {
        exit(1);
    }

    build_flags();
    build_lexer();
    build_optiz();

    if (args.should_run) {
        run_optiz();
    }

    return 0;
}

void print_help(const char* program) {
    nob_log(INFO, "Usage: %s [run]", program);
}

Program_Args parse_args(int argc, char** argv) {
    Program_Args args;

    args.program = shift(argv, argc);

    args.should_run = false;
    while (argc > 0) {
        const char* arg = shift(argv, argc);
        if (strcmp(arg, "run") == 0) {
            args.should_run = true;
        } else {
            nob_log(ERROR, "Unknown argument: %s", arg);
            print_help(args.program);
            exit(1);
        }
    }

    return args;
}

void build_flags() {
    int need_rebuild = needs_rebuild1("compile_flags.txt", SRC_BUILD "config.h");
    if (need_rebuild < 0) exit(1);
    if (!need_rebuild) {
        nob_log(NOB_INFO, "compile_flags.txt doesn't need to be rebuilt");
        return;
    }

    nob_log(NOB_INFO, "rebuilding compile_flags.txt");

    String_Builder flags = { 0 };
    for (int i = 0; i < CPP_FLAGS_LEN; ++i) {
        sb_append_cstr(&flags, CPP_FLAGS[i]);
        sb_append_cstr(&flags, "\n");
    }
    bool ok = write_entire_file("compile_flags.txt", flags.items, flags.count);
    if (!ok) exit(1);

    nob_log(INFO, "wrote %d flags to compile_flags.txt", CPP_FLAGS_LEN);
    sb_free(flags);
}

void build_lexer() {
    const char* inputs[] = {
        SRC "lexer.l",
        SRC "include/lexer.hpp"
    };

    int need_rebuild = needs_rebuild(BUILD "lexer.cpp", inputs, ARRAY_LEN(inputs));
    if (need_rebuild < 0) exit(1);
    if (!need_rebuild) return;

    Cmd cmd = { 0 };
    cmd_append(&cmd, "flex", "-o", BUILD "lexer.cpp", SRC "lexer.l");

    if (!cmd_run(&cmd)) exit(1);
    cmd_free(cmd);
}

void build_optiz() {
    Cmd cmd = { 0 };
    cmd_append(&cmd, "clang++");
    da_append_many(&cmd, CPP_FLAGS, CPP_FLAGS_LEN);
    cmd_append(&cmd, "-o", BUILD "optiz");
    da_append_many(&cmd, SRCS, SRCS_LEN);

    if (!cmd_run(&cmd)) exit(1);
    cmd_free(cmd);
}

void run_optiz() {
    Cmd cmd = { 0 };
    cmd_append(&cmd, BUILD "optiz");

    if (!cmd_run(&cmd)) exit(1);
    cmd_free(cmd);
}

// NOLINTEND(bugprone-suspicious-missing-comma)
