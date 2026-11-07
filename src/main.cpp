#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/FileUtilities.h>
#include <llvm/Support/Program.h>

#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <string_view>

#include "codegen.hpp"
#include "compiler_options.hpp"
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

int main(int argc, char** argv) {
    auto args = ParseCompilerOptions(argc, argv);

    std::ifstream file;
    std::istream* source_input = &std::cin;
    if (!args.input.empty()) {
        file.open(args.input, std::ios::binary);
        if (!file) {
            std::println(stderr, "Cannot open file: {}", args.input);
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
    auto& codegen = session.GetOrCreateModule("optiz-main");
    if (auto result = codegen.GenerateMain(**expression); !result) {
        std::println(stderr, "{}", result.error());
        return 1;
    }

    if (args.emit == EmitKind::IR) {
        if (auto result = codegen.WriteIR(args.output); !result) {
            std::println(stderr, "{}", result.error());
            return 1;
        }
        return 0;
    }

    return CompileNative(codegen, args.output);
}
