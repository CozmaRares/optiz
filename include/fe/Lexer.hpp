#pragma once

#include "fe/Diagnostic.hpp"
#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    enum class TokenType {
        // R-Values
        Number,
        // Char,
        // String,
        Identifier,

        // OPERATORS
        // Equals,        // =
        // EqualsEquals,  // ==

        // Bang,        // !
        // BangEquals,  // !=

        // Less,        // <
        // LessEquals,  // <=
        // ShiftLeft,   // <<

        // Greater,        // >
        // GreaterEquals,  // >=
        // ShiftRight,     // >>

        // Amp,  // &
        // And,  // &&

        // BitOr,  // |
        // Or,     // ||

        // Plus,     // +
        // Minus,    // -
        Star,  // *
        // Slash,    // /
        // Percent,  // %
        // Caret,    // ^
        // Tilde,    // ~

        // SYNTAX
        LParen,  // (
        RParen,  // )
        // LSquare,  // [
        // RSquare,  // ]
        // LCurly,   // {
        // RCurly,   // }

        // DELIMITERS
        Comma,  // ,
        // Dot,        // .
        Colon,  // :
        SemiColon,  // ;

        // KEYWORDS
        // True,
        // False,
        // AtOpti,
        // AtUse,
        // Return,
        AtProfile,
        Import,
        // If,
        // Then,
        // Else,
        // While,
        // Do,
        Fn,
        Struct,

        // SPECIAL
        Error,
        EndOfFile,
    };

    struct Token {
        TokenType m_Type;
        als::String m_Lexeme;
        SrcSpan m_Span;

        Token(TokenType type = TokenType::Error) : m_Type(type) {}

        Token(TokenType type, als::String lexeme, SrcLocation location)
            : m_Type(type), m_Lexeme(lexeme), m_Span{ location, location } {}

        Token(TokenType type, als::String lexeme, SrcLocation startLocation, SrcLocation endLocation)
            : m_Type(type), m_Lexeme(lexeme), m_Span{ startLocation, endLocation } {}

        bool operator==(const TokenType& type) const { return m_Type == type; }
    };

    class Lexer {
        als::String m_Input;
        uint m_Cursor;
        SrcLocation m_Location;
        char m_Current;
        DiagnosticEngine& m_DiagnosticEngine;

    public:
        Lexer(const als::String& input, const als::String& file, DiagnosticEngine& diagnosticEngine)
            : m_Input(input), m_Cursor(0), m_Location{ 1, 1, file }, m_Current(input[m_Cursor]), m_DiagnosticEngine(diagnosticEngine) {}

        Token GetNextToken();

    private:
        void Advance();

        void SkipWhitespace();

        Token TokenizeNumber();
        Token TokenizeChar();
        Token TokenizeString();
        Token TokenizeIdentifierOrKeyword();
    };

    als::OutStream& operator<<(als::OutStream& out, TokenType type);
    als::OutStream& operator<<(als::OutStream& out, const Token& token);

}  // namespace optiz::fe
