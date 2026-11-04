#include <iostream>

#include "ast_printer.hpp"
#include "parser.hpp"

int main() {
    Lexer lexer;
    Parser parser(lexer);

    auto expression = parser.Parse();

    if (!expression) {
        std::cerr << "Parse error: " << expression.error().message << '\n';
        return 1;
    }

    AstPrinter printer(std::cout);
    printer.Print(**expression);

    std::cout << '\n';
}
