// NOLINTBEGIN(bugprone-suspicious-missing-comma)
#include <stdbool.h>
#include <stdlib.h>

#define BUILD "build/"
#define SRC   "src/"

#define NOB_IMPLEMENTATION
#define NOB_REBUILD_URSELF(binary_path, source_path) "clang", "-o", binary_path, source_path
#include "vendor/tsoding/nob.h/nob.h"
#undef NOB_IMPLEMENTATION

const char* const SRCS[] = {
    SRC "main.cpp",
    SRC "ast_printer.cpp",
    SRC "parser.cpp",
    BUILD "lexer.cpp"
};

void build_lexer();
void build_optiz();
void run_optiz(const char* file);

typedef struct {
    char* program;

    struct {
        bool should_run;
        const char* file;
    } run_settings;

} Program_Args;

Program_Args parse_args(int argc, char** argv);

int main(int argc, char** argv) {
    GO_REBUILD_URSELF(argc, argv);

    Program_Args args = parse_args(argc, argv);

    if (!mkdir_if_not_exists(BUILD) || !mkdir_if_not_exists(BUILD "obj")) exit(1);

    build_lexer();
    build_optiz();

    if (args.run_settings.should_run) {
        run_optiz(args.run_settings.file);
    }

    return 0;
}

void print_help(const char* program) {
    nob_log(INFO, "Usage: %s [run [file]]", program);
}

Program_Args parse_args(int argc, char** argv) {
    Program_Args args = { .program = shift(argv, argc) };

    if (argc == 0) return args;

    const char* command = shift(argv, argc);
    if (strcmp(command, "run") != 0) {
        nob_log(ERROR, "Unknown command: %s", command);
        print_help(args.program);
        exit(1);
    }

    args.run_settings.should_run = true;
    if (argc > 0) args.run_settings.file = shift(argv, argc);
    if (argc > 0) {
        nob_log(ERROR, "Unexpected argument: %s", argv[0]);
        print_help(args.program);
        exit(1);
    }

    return args;
}

void build_lexer() {
    const char* inputs[] = {
        SRC "lexer.l"
    };

    int need_rebuild = needs_rebuild(BUILD "lexer.cpp", inputs, ARRAY_LEN(inputs));
    if (need_rebuild < 0) exit(1);
    if (!need_rebuild) return;

    Cmd cmd = { 0 };
    cmd_append(&cmd, "flex", "-o", BUILD "lexer.cpp", SRC "lexer.l");

    if (!cmd_run(&cmd)) exit(1);
    cmd_free(cmd);
}

bool dependencies_changed(const char* object, const char* depfile);
#define nob_changed(output) needs_rebuild1(output, "nob")

Cmd llvm_flags() {
    Cmd cmd = { 0 };
    cmd_append(&cmd, "llvm-config", "--cxxflags", "--ldflags", "--system-libs", "--libs", "core");
    if (!cmd_run(&cmd, .stdout_path = BUILD "llvm-flags.txt")) exit(1);

    String_Builder flags = { 0 };
    if (!read_entire_file(BUILD "llvm-flags.txt", &flags)) exit(1);
    sb_append_null(&flags);

    // llvm-config emits whitespace-separated compiler and linker options.
    for (char* flag = strtok(flags.items, " \t\r\n"); flag; flag = strtok(NULL, " \t\r\n")) {
        // Suppress warnings from LLVM headers while retaining project warnings.
        if (strncmp(flag, "-I", 2) == 0) {
            cmd_append(&cmd, "-isystem");
            if (flag[2]) cmd_append(&cmd, temp_strdup(flag + 2));
        } else {
            cmd_append(&cmd, temp_strdup(flag));
        }
    }
    sb_free(flags);
    return cmd;
}

void build_optiz() {
    Cmd flags = { 0 };
    Cmd objects  = { 0 };
    Cmd cmd      = { 0 };
    Procs procs  = { 0 };
    bool success = true;
    bool link    = nob_changed(BUILD "optiz");

    for (int i = 0; i < ARRAY_LEN(SRCS); ++i) {
        // Preserve the full source path in a flat filename to avoid collisions.
        char* name = temp_strdup(SRCS[i]);
        for (char* p = name; *p; ++p) {
            if (*p == '/') *p = '!';
        }
        const char* object = temp_sprintf(BUILD "obj/%s.o", name);
        const char* deps   = temp_sprintf("%s.d", object);
        cmd_append(&objects, object);

        if (nob_changed(object) || needs_rebuild1(object, SRCS[i]) || dependencies_changed(object, deps)) {
            if (!flags.items) flags = llvm_flags();
            cmd_append(&cmd, "clang++");
            // Library and linker options are only needed for the final link.
            for (size_t j = 0; j < flags.count; ++j) {
                const char* flag = flags.items[j];
                if (strncmp(flag, "-l", 2) == 0 || strncmp(flag, "-L", 2) == 0 || strncmp(flag, "-Wl,", 4) == 0) continue;
                cmd_append(&cmd, flag);
            }
            cmd_append(&cmd, "-std=c++23", "-Wall", "-Wextra", "-fno-exceptions", "-I" SRC "include");
            cmd_append(&cmd, "-MMD", "-MF", deps, "-MT", "dependencies");
            cmd_append(&cmd, "-c", SRCS[i], "-o", object);

            if (!cmd_run(&cmd, .async = &procs)) {
                success = false;
                break;
            }
            link = true;
        }
        if (needs_rebuild1(BUILD "optiz", object)) link = true;
    }

    success = procs_wait(procs) && success;
    da_free(procs);
    if (!success) exit(1);

    if (link) {
        if (!flags.items) flags = llvm_flags();
        cmd_append(&cmd, "clang++");
        cmd_append(&cmd, "-std=c++23", "-Wall", "-Wextra", "-fno-exceptions", "-I" SRC "include");
        da_append_many(&cmd, objects.items, objects.count);
        da_append_many(&cmd, flags.items, flags.count);
        cmd_append(&cmd, "-std=c++23");
        cmd_append(&cmd, "-o", BUILD "optiz");
        if (!cmd_run(&cmd)) exit(1);
    }

    cmd_free(cmd);
    cmd_free(objects);
    cmd_free(flags);
}

void run_optiz(const char* file) {
    Cmd cmd = { 0 };
    cmd_append(&cmd, BUILD "optiz");
    if (file) cmd_append(&cmd, file);

    if (!cmd_run(&cmd)) exit(1);
    cmd_free(cmd);
}

bool dependencies_changed(const char* object, const char* depfile) {
    if (!file_exists(depfile)) return true;

    String_Builder deps = { 0 };
    if (!read_entire_file(depfile, &deps)) exit(1);
    // -MT dependencies gives every depfile a simple, fixed target.
    char* colon = deps.count ? memchr(deps.items, ':', deps.count) : NULL;
    if (!colon) {
        sb_free(deps);
        return true;
    }

    bool changed        = false;
    size_t count        = 0;
    String_Builder path = { 0 };
    for (size_t i = (size_t)(colon - deps.items) + 1; i < deps.count;) {
        char ch = deps.items[i++];
        if (ch == '\\' && i < deps.count) {
            ch = deps.items[i++];
            if (ch == '\n') continue;
            if (ch == '\r' && i < deps.count && deps.items[i] == '\n') {
                ++i;
                continue;
            }
            sb_append_buf(&path, &ch, 1);
        } else if (isspace((unsigned char)ch)) {
            if (!path.count) continue;
            sb_append_null(&path);
            ++count;
            if (needs_rebuild1(object, path.items) != 0) changed = true;
            path.count = 0;
        } else {
            // Make escapes dollar signs by doubling them.
            if (ch == '$' && i < deps.count && deps.items[i] == '$') ++i;
            sb_append_buf(&path, &ch, 1);
        }
    }
    if (path.count) {
        sb_append_null(&path);
        ++count;
        if (needs_rebuild1(object, path.items) != 0) changed = true;
    }
    sb_free(path);
    sb_free(deps);
    return changed || count == 0;
}

// NOLINTEND(bugprone-suspicious-missing-comma)
