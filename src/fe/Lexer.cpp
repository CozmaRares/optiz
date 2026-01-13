#include "fe/Lexer.hpp"

#include <iostream>

#include "fe/Diagnostic.hpp"

namespace optiz::fe {

    Token::Token(TokenType type) : m_Type(type) {}

    Token::Token(TokenType type, std::string lexeme, SrcLocation location)
        : m_Type(type), m_Lexeme(lexeme), m_StartLocation(location), m_EndLocation(location) {}

    Token::Token(TokenType type, std::string lexeme, SrcLocation startLocation, SrcLocation endLocation)
        : m_Type(type), m_Lexeme(lexeme), m_StartLocation(startLocation), m_EndLocation(endLocation) {}

    Lexer::Lexer(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine)
        : m_Input(input), m_Cursor(0), m_Location(SrcLocation{ 1, 1, file }), m_Current(input[m_Cursor]), m_DiagnosticEngine(diagnosticEngine) {}

    Token Lexer::GetNextToken() {
        SkipWhitespace();

        if (m_Current == '\0') {
            return Token(TokenType::EndOfFile, "", m_Location);
        }

        if (std::isdigit(m_Current)) {
            return TokenizeNumber();
        }

        TokenType type = TokenType::EndOfFile;

        // 1-wide tokens
        switch (m_Current) {
            case '+':
                type = TokenType::Plus;
                break;
            case '-':
                type = TokenType::Minus;
                break;
            case '*':
                type = TokenType::Star;
                break;
            case '/':
                type = TokenType::Slash;
                break;
            case '(':
                type = TokenType::LParen;
                break;
            case ')':
                type = TokenType::RParen;
                break;
            case ';':
                type = TokenType::SemiColon;
                break;
            default:
                break;
        }

        if (type != TokenType::EndOfFile) {
            char current      = m_Current;
            SrcLocation start = m_Location;
            Advance();
            return Token(type, std::string(1, current), start, m_Location);
        }

        m_DiagnosticEngine.Report(m_Location, std::string("Unexpected character: ") + m_Current, DiagnosticLevel::Error);
        Advance();
        return Token(TokenType::Error);
    }

    void Lexer::Advance() {
        m_Cursor++;
        m_Location.advance(m_Current);
        m_Current = m_Input[m_Cursor];
    }

    void Lexer::SkipWhitespace() {
        while (std::isspace(m_Current)) {
            Advance();
        }
    }

    Token Lexer::TokenizeNumber() {
        std::string lexeme        = "";
        SrcLocation locationStart = m_Location;

        while (std::isdigit(m_Current)) {
            lexeme += m_Current;
            Advance();
        }

        if (m_Current != '.') {
            goto end;
        }

        lexeme += m_Current;
        Advance();

        while (std::isdigit(m_Current)) {
            lexeme += m_Current;
            Advance();
        }

    end:
        return Token(TokenType::Number, lexeme, locationStart, m_Location);
    }

    std::ostream& operator<<(std::ostream& out, TokenType type) {
        switch (type) {
            case TokenType::Number:
                return out << "NUMBER";
            case TokenType::Plus:
                return out << "PLUS";
            case TokenType::Minus:
                return out << "MINUS";
            case TokenType::Star:
                return out << "STAR";
            case TokenType::Slash:
                return out << "SLASH";
            case TokenType::LParen:
                return out << "LPAREN";
            case TokenType::RParen:
                return out << "RPAREN";
            case TokenType::EndOfFile:
                return out << "EOI";
                break;
            case TokenType::SemiColon:
                return out << "SEMICOLON";
            case TokenType::Error:
                return out << "ERROR";
        }

        return out << "UNKNOWN";
    }

    std::ostream& operator<<(std::ostream& out, const SrcLocation& loc) {
        out << "SrcLoc { line = " << loc.m_Line << ", col = " << loc.m_Column << ", file = " << loc.m_File << " }";
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Token& token) {
        out << "Token { type = " << token.m_Type << ", value = " << token.m_Lexeme << ", loc = {"
            << token.m_StartLocation << ", " << token.m_EndLocation << "} }";
        return out;
    }

}  // namespace optiz::fe
