#pragma once

#include <string>

enum class EmitKind {
    Executable,
    IR
};

struct CompilerOptions {
    EmitKind emit = EmitKind::Executable;
    std::string input;
    std::string output;
};

CompilerOptions ParseCompilerOptions(int argc, char** argv);
