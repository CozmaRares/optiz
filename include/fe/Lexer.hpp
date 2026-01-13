#pragma once

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
        TokenType m_Type;
        std::string m_Lexeme;
        SrcLocation m_StartLocation;
        SrcLocation m_EndLocation;

        Token(TokenType type = TokenType::Error);
        Token(TokenType type, std::string lexeme, SrcLocation location);
        Token(TokenType type, std::string lexeme, SrcLocation startLocation, SrcLocation endLocation);

        friend std::ostream& operator<<(std::ostream& out, const Token& token);
    };

    class Lexer {
        std::string m_Input;
        uint m_Cursor;
        SrcLocation m_Location;
        char m_Current;
        DiagnosticEngine& m_DiagnosticEngine;

    public:
        Lexer(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine);
        Token GetNextToken();

    private:
        void Advance();

        void SkipWhitespace();
        Token TokenizeNumber();
    };

}  // namespace optiz::fe
