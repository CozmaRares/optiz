#include <fstream>
#include <iostream>
#include <print>

#include "ast_printer.hpp"
#include "codegen.hpp"
#include "parser.hpp"

int main(int argc, char** argv) {
    if (argc > 2) {
        std::println(stderr, "Usage: {} [file]", argv[0]);
        return 1;
    }

    std::ifstream file;
    std::istream* source_input = &std::cin;
    if (argc == 2) {
        file.open(argv[1], std::ios::binary);
        if (!file) {
            std::println(stderr, "Cannot open file: {}", argv[1]);
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

    AstPrinter printer(std::cout);
    printer.Print(**expression);

    Codegen codegen;

    std::cout << '\n';
}
