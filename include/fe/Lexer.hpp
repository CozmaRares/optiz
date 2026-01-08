#ifndef LEXER_HPP
#define LEXER_HPP

#include <ostream>
#include <string>

#include "fe/Diagnostic.hpp"
#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    enum class TokenType {
        // PRIMITIVES
        Number,

        // OPERATORS
        Plus,
        Minus,
        Star,
        Slash,

        // SYNTAX
        LParen,
        RParen,

        // KEYWORDS

        // DELIMITERS
        SemiColon,
        EndOfFile,

        Error,
    };

    std::ostream& operator<<(std::ostream& out, TokenType type);

    struct Token {
        TokenType type;
        std::string lexeme;
        std::pair<SrcLocation, SrcLocation> location;

        Token(TokenType type = TokenType::Error);
        Token(TokenType type, std::string lexeme, SrcLocation location);
        Token(TokenType type, std::string lexeme, SrcLocation locationStart, SrcLocation locationEnd);

        friend std::ostream& operator<<(std::ostream& out, const Token& token);
    };

    class Lexer {
        std::string input;
        uint position;
        SrcLocation location;
        char current;
        DiagnosticEngine& diagnosticEngine;

    public:
        Lexer(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine);
        Token nextToken();

    private:
        void advance();

        void skipWhitespace();
        Token tokenizeNumber();
    };

}  // namespace optiz::fe

#endif  // LEXER_HPP
