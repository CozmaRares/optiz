#pragma once

#include <ostream>
#include <string>

#include "fe/Diagnostic.hpp"
#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    enum class TokenType {
        // PRIMITIVES
        Number,
        Char,
        String,
        Identifier,

        // OPERATORS
        Equals,        // =
        EqualsEquals,  // ==

        Bang,        // !
        BangEquals,  // !=

        Less,        // <
        LessEquals,  // <=
        ShiftLeft,   // <<

        Greater,        // >
        GreaterEquals,  // >=
        ShiftRight,     // >>

        Amp,  // &
        And,  // &&

        BitOr,  // |
        Or,     // ||

        Plus,     // +
        Minus,    // -
        Star,     // *
        Slash,    // /
        Percent,  // %
        Caret,    // ^
        Tilde,    // ~

        // SYNTAX
        LParen,   // (
        RParen,   // )
        LSquare,  // [
        RSquare,  // ]
        LCurly,   // {
        RCurly,   // }

        // DELIMITERS
        Comma,      // ,
        Dot,        // .
        Colon,      // :
        SemiColon,  // ;

        // KEYWORDS
        True,
        False,
        AtOpti,
        AtUse,
        Return,
        AtLikely,
        AtProfile,
        If,
        Then,
        Else,
        While,
        Do,
        Fn,
        Struct,

        // SPECIAL
        Error,
        EndOfFile,
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

        bool operator==(const TokenType& type) const;

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
        Token TokenizeChar();
        Token TokenizeString();
        Token TokenizeIdentifierOrKeyword();
    };

}  // namespace optiz::fe
