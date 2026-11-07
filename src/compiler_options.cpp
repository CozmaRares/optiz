#include "compiler_options.hpp"

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>

namespace {
    namespace cl = llvm::cl;

    cl::OptionCategory category("Optiz options");
    cl::SubCommand ir("ir", "Emit LLVM IR");
    cl::SubCommandGroup modes{ &cl::SubCommand::getTopLevel(), &ir };

    cl::opt<std::string> input(cl::Positional, cl::desc("[input file]"), cl::Optional, cl::cat(category), cl::sub(modes));
    cl::opt<std::string> output("o", cl::desc("Output file"), cl::value_desc("path"), cl::Required, cl::cat(category), cl::sub(modes));
}  // namespace

CompilerOptions ParseCompilerOptions(int argc, char** argv) {
    cl::HideUnrelatedOptions(category);
    cl::HideUnrelatedOptions(category, ir);

    std::string error;
    llvm::raw_string_ostream diagnostics(error);
    if (!cl::ParseCommandLineOptions(argc, argv, "Optiz compiler\n", &diagnostics)) {
        std::cerr << error << std::endl;
        exit(1);
    }
    if (output.getValue().empty()) {
        std::cerr << "Output file must not be empty";
        exit(1);
    }
    if (input.getNumOccurrences() && input.getValue().empty()) {
        std::cerr << "Input file must not be empty";
        exit(1);
    };

    return CompilerOptions{
        .emit   = ir ? EmitKind::IR : EmitKind::Executable,
        .input  = input.getValue(),
        .output = output.getValue()
    };
}
