#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "ast_printer.hpp"
#include "parser.hpp"

int main() {
    const std::string source(std::istreambuf_iterator<char>{ std::cin }, {});
    std::istringstream input(source);
    Lexer lexer(&input);
    Parser parser(lexer);

    auto expression = parser.Parse();

    if (!expression) {
        expression.error()->Print(source);
        return 1;
    }

    AstPrinter printer(std::cout);
    printer.Print(**expression);

    std::cout << '\n';
}
