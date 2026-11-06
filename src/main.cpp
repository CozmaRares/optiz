#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/FileUtilities.h>
#include <llvm/Support/Program.h>

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>

#include "codegen.hpp"
#include "parser.hpp"

int CompileNative(const CodegenModule& codegen, std::string_view output) {
    auto clang = llvm::sys::findProgramByName("clang");
    if (!clang) {
        std::println(stderr, "Cannot find clang: {}", clang.getError().message());
        return 1;
    }
    llvm::SmallString<128> irPath;
    if (auto error = llvm::sys::fs::createTemporaryFile("optiz", "ll", irPath)) {
        std::println(stderr, "Cannot create temporary IR file: {}", error.message());
        return 1;
    }
    llvm::FileRemover cleanup(irPath);
    if (auto result = codegen.WriteIR(irPath.str().str()); !result) {
        std::println(stderr, "{}", result.error());
        return 1;
    }
    std::string error;
    int status = llvm::sys::ExecuteAndWait(
        *clang,
        { *clang, "-Wno-override-module", irPath, "-o", llvm::StringRef(output) },
        std::nullopt, {}, 0, 0, &error);
    if (status != 0) {
        std::println(stderr, "Native compilation failed (status {}): {}", status, error);
        return 1;
    }
    return 0;
}

#define shift(argc, argv) (assert(argc > 0), (argc--, *(argv)++))

struct ProgramArgs {
    std::string_view program;
    std::string_view mode;
    std::string_view file;
    std::string_view output;

    void Fail(std::string_view message) const {
        std::println(stderr, "{}", message);
        std::println(stderr,
                     "Usage: {} [file] -o <binary>\n"
                     "       {:<{}} ir [file] -o <ir-file>",
                     program, ' ', program.length());
        std::exit(1);
    }

    ProgramArgs(int argc, char** argv) {
        program = shift(argc, argv);
        if (argc > 0 && std::string_view(argv[0]) == "ir") {
            mode = shift(argc, argv);
        }

        std::string_view arg;

        while (argc > 0) {
            arg = shift(argc, argv);
            if (arg == "-o") {
                if (!output.empty()) Fail("Output file specified more than once");
                if (argc == 0) Fail("Expected an output file after -o");
                output = shift(argc, argv);
                if (output.empty() || output.starts_with('-')) Fail("Expected an output file after -o");
            } else if (arg.starts_with('-')) {
                Fail("Unknown option: " + std::string(arg));
            } else {
                if (!file.empty()) Fail("Unexpected argument: " + std::string(arg));
                if (arg.empty()) Fail("Input file must not be empty");
                file = arg;
            }
        }
        if (output.empty()) Fail("An output file is required; use -o <path>");
    }
};

int main(int argc, char** argv) {
    ProgramArgs args(argc, argv);

    std::ifstream file;
    std::istream* source_input = &std::cin;
    if (args.file.length()) {
        file.open(args.file.data(), std::ios::binary);
        if (!file) {
            std::println(stderr, "Cannot open file: {}", args.file);
            return 1;
        }
        source_input = &file;
    }

    Lexer lexer(source_input);
    Parser parser(lexer);
    auto expression = parser.Parse();
    if (!expression) {
        expression.error()->Print();
        return 1;
    }

    CodegenSession session;
    auto& codegen = session.GetOrCreateModule("optiz");
    if (auto result = codegen.GenerateMain(**expression); !result) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    if (args.mode == "ir") {
        if (auto result = codegen.WriteIR(std::string(args.output)); !result) {
            std::println(stderr, "{}", result.error());
            return 1;
        }
        return 0;
    }

    return CompileNative(codegen, args.output);
}
