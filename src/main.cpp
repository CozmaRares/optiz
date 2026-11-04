#include <iostream>

#include "lexer.hpp"

int main() {
    Lexer lexer;

    for (;;) {
        lexer.NextToken();
        const Token& token = lexer.token;
        switch (token.type) {
            case Token::End:
                std::cout << "End\n";
                return 0;
            case Token::Integer:
                std::cout << "Integer: " << token.value.integer_value << '\n';
                break;
            case Token::Float:
                std::cout << "Float: " << token.value.float_value << '\n';
                break;
            case Token::Invalid:
                std::cout << "Invalid\n";
                break;
            default:
                std::cout << '\'' << static_cast<char>(token.type) << "'\n";
                break;
        }
    }
}
