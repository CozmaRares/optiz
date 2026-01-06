#include "fe/Lexer.hpp"

#include <iostream>

#include "fe/Diagnostic.hpp"

namespace optiz::fe {
    Token::Token(TokenType type) : type(type) {}

    Token::Token(TokenType type, std::string lexeme, SrcLocation location)
        : type(type), lexeme(lexeme), location(location, location) {}

    Token::Token(TokenType type, std::string lexeme, SrcLocation locationStart, SrcLocation locationEnd)
        : type(type), lexeme(lexeme), location(locationStart, locationEnd) {}

    Lexer::Lexer(const std::string& input, const std::string& file, DiagnosticEngine& diagnosticEngine)
        : input(input), position(0), location(SrcLocation{ .line = 1, .column = 1, .file = file }), current(input[position]), diagnosticEngine(diagnosticEngine) {}

    Token Lexer::nextToken() {
        this->skipWhitespace();

        if (this->current == '\0') {
            return Token(TokenType::EndOfFile, "", this->location);
        }

        if (std::isdigit(this->current)) {
            return this->tokenizeNumber();
        }

        TokenType type = TokenType::EndOfFile;

        // 1-wide tokens
        switch (this->current) {
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
            char current      = this->current;
            SrcLocation start = this->location;
            this->advance();
            return Token(type, std::string(1, current), start, this->location);
        }

        this->diagnosticEngine.report(this->location, std::string("Unexpected character: ") + this->current, DiagnosticLevel::Error);
        this->advance();
        return Token(TokenType::Error);
    }

    void Lexer::advance() {
        this->position++;
        this->location.advance(this->current);
        this->current = this->input[this->position];
    }

    void Lexer::skipWhitespace() {
        while (std::isspace(this->current)) {
            this->advance();
        }
    }

    Token Lexer::tokenizeNumber() {
        std::string lexeme        = "";
        SrcLocation locationStart = this->location;

        while (std::isdigit(this->current)) {
            lexeme += this->current;
            this->advance();
        }

        if (this->current != '.') {
            goto end;
        }

        lexeme += this->current;
        advance();

        while (std::isdigit(this->current)) {
            lexeme += this->current;
            this->advance();
        }

    end:
        return Token(TokenType::Number, lexeme, locationStart, this->location);
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
        out << "SrcLoc { line = " << loc.line << ", col = " << loc.column << ", file = " << loc.file << " }";
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Token& token) {
        out << "Token { type = " << token.type << ", value = " << token.lexeme << ", loc = {"
            << token.location.first << ", " << token.location.second << "} }";
        return out;
    }

}  // namespace optiz::fe
