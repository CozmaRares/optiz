#include "fe/Lexer.hpp"

#include "fe/Diagnostic.hpp"
#include "utils/aliases.hpp"

using optiz::fe::TokenType;

static als::Map<als::String, TokenType> s_KeywordMap = {
    { "true", TokenType::True },
    { "false", TokenType::False },
    { "@opti", TokenType::AtOpti },
    { "@use", TokenType::AtUse },
    { "return", TokenType::Return },
    { "@likely", TokenType::AtLikely },
    { "@profile", TokenType::AtProfile },
    { "if", TokenType::If },
    { "then", TokenType::Then },
    { "else", TokenType::Else },
    { "while", TokenType::While },
    { "do", TokenType::Do },
    { "fn", TokenType::Fn },
    { "struct", TokenType::Struct },
};

TokenType getOneCharToken(char c);
TokenType getPossiblyTwoCharToken(TokenType previousType, char c);

bool isIdentifierStart(char c);
bool isIdentifierChar(char c);
char getEscapedChar(char c);

namespace optiz::fe {

    Token Lexer::GetNextToken() {
        SkipWhitespace();

        if (m_Current == '\0') {
            return Token(TokenType::EndOfFile, "", m_Location);
        }

        if (als::IsNumeric(m_Current)) {
            return TokenizeNumber();
        }

        if (m_Current == '\'') {
            return TokenizeChar();
        }

        if (m_Current == '"') {
            return TokenizeString();
        }

        if (isIdentifierStart(m_Current)) {
            return TokenizeIdentifierOrKeyword();
        }

        TokenType type = getOneCharToken(m_Current);

        if (type != TokenType::Error) {
            SrcLocation start = m_Location;
            als::String lexeme(1, m_Current);
            Advance();
            TokenType maybeTwoCharType = getPossiblyTwoCharToken(type, m_Current);

            // is a two-char token
            if (type != maybeTwoCharType) {
                type = maybeTwoCharType;
                lexeme += m_Current;
                Advance();
            }

            return Token(type, lexeme, start, m_Location);
        }

        m_DiagnosticEngine.Report(m_Location, als::String("Unexpected character: ") + m_Current, DiagnosticLevel::Error);
        Advance();
        return Token(TokenType::Error);
    }

    void Lexer::Advance() {
        m_Cursor++;
        m_Location.advance(m_Current);

        if (m_Cursor < m_Input.size()) {
            m_Current = m_Input[m_Cursor];
        } else {
            m_Current = '\0';
        }
    }

    void Lexer::SkipWhitespace() {
        while (als::IsSpace(m_Current)) {
            Advance();
        }
    }

    Token Lexer::TokenizeNumber() {
        als::String lexeme        = "";
        SrcLocation locationStart = m_Location;

        while (als::IsNumeric(m_Current)) {
            lexeme += m_Current;
            Advance();
        }

        if (m_Current != '.') {
            goto end;
        }

        lexeme += m_Current;
        Advance();

        while (als::IsNumeric(m_Current)) {
            lexeme += m_Current;
            Advance();
        }

    end:
        return Token(TokenType::Number, lexeme, locationStart, m_Location);
    }

    Token Lexer::TokenizeChar() {
        SrcLocation start = m_Location;
        Advance();

        char c;

        if (m_Current == '\\') {
            Advance();
            c = getEscapedChar(m_Current);
        } else {
            c = m_Current;
        }

        Advance();

        if (m_Current == '\'') {
            Advance();
            return Token(TokenType::Char, als::String(1, c), start, m_Location);
        }

        m_DiagnosticEngine.Report(m_Location, als::String("Expected apostrophe (\')"), DiagnosticLevel::Error);
        return Token(TokenType::Error);
    }

    Token Lexer::TokenizeString() {
        SrcLocation start = m_Location;
        Advance();

        bool escaped = false;
        als::String lexeme;

        while (m_Current != '\0' && (m_Current != '"' || escaped)) {
            if (escaped) {
                lexeme += getEscapedChar(m_Current);
            } else if (m_Current == '\\') {
                escaped = true;
            } else {
                lexeme += m_Current;
            }

            Advance();
        }

        if (m_Current == '"') {
            Advance();
            return Token(TokenType::String, lexeme, start, m_Location);
        }

        m_DiagnosticEngine.Report(m_Location, als::String("Expected quote (\")"), DiagnosticLevel::Error);
        return Token(TokenType::Error);
    }

    Token Lexer::TokenizeIdentifierOrKeyword() {
        als::String lexeme;
        SrcLocation start = m_Location;

        while (isIdentifierChar(m_Current)) {
            lexeme += m_Current;
            Advance();
        }

        TokenType type;

        auto it = s_KeywordMap.find(lexeme);
        if (it != s_KeywordMap.end()) {
            type = it->second;
        } else {
            type = TokenType::Identifier;
        }

        return Token(type, lexeme, start, m_Location);
    }

    als::OutStream& operator<<(als::OutStream& out, TokenType type) {
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
                return out << "EOF";
            case TokenType::SemiColon:
                return out << "SEMICOLON";
            case TokenType::Error:
                return out << "ERROR";
            case TokenType::Char:
                return out << "CHAR";
            case TokenType::String:
                return out << "STRING";
            case TokenType::Identifier:
                return out << "IDENTIFIER";
            case TokenType::Equals:
                return out << "EQUALS";
            case TokenType::EqualsEquals:
                return out << "EQUALSEQUALS";
            case TokenType::Bang:
                return out << "BANG";
            case TokenType::BangEquals:
                return out << "BANGEQUALS";
            case TokenType::Less:
                return out << "LESS";
            case TokenType::LessEquals:
                return out << "LESSEQUALS";
            case TokenType::ShiftLeft:
                return out << "SHIFTLEFT";
            case TokenType::Greater:
                return out << "GREATER";
            case TokenType::GreaterEquals:
                return out << "GREATEREQUALS";
            case TokenType::ShiftRight:
                return out << "SHIFTRIGHT";
            case TokenType::Amp:
                return out << "AMP";
            case TokenType::And:
                return out << "AND";
            case TokenType::BitOr:
                return out << "BITOR";
            case TokenType::Or:
                return out << "OR";
            case TokenType::Percent:
                return out << "PERCENT";
            case TokenType::Caret:
                return out << "CARET";
            case TokenType::Tilde:
                return out << "TILDE";
            case TokenType::LSquare:
                return out << "LSQUARE";
            case TokenType::RSquare:
                return out << "RSQUARE";
            case TokenType::LCurly:
                return out << "LCURLY";
            case TokenType::RCurly:
                return out << "RCURLY";
            case TokenType::Comma:
                return out << "COMMA";
            case TokenType::Dot:
                return out << "DOT";
            case TokenType::Colon:
                return out << "COLON";
            case TokenType::True:
                return out << "TRUE";
            case TokenType::False:
                return out << "FALSE";
            case TokenType::AtOpti:
                return out << "ATOPTI";
            case TokenType::AtUse:
                return out << "ATUSE";
            case TokenType::Return:
                return out << "RETURN";
            case TokenType::AtLikely:
                return out << "ATLIKELY";
            case TokenType::AtProfile:
                return out << "ATPROFILE";
            case TokenType::If:
                return out << "IF";
            case TokenType::Then:
                return out << "THEN";
            case TokenType::Else:
                return out << "ELSE";
            case TokenType::While:
                return out << "WHILE";
            case TokenType::Do:
                return out << "DO";
            case TokenType::Fn:
                return out << "FN";
            case TokenType::Struct:
                return out << "STRUCT";
            default:
                break;
        }

        return out << "UNKNOWN(" << static_cast<int>(type) << ')';
    }

    als::OutStream& operator<<(als::OutStream& out, const Token& token) {
        als::Print(out, "Token { type = ", token.m_Type, ", value = ", token.m_Lexeme, ", span = ", token.m_Span, " }");
        return out;
    }

}  // namespace optiz::fe

TokenType getOneCharToken(char c) {
    switch (c) {
        case '+': return TokenType::Plus;
        case '-': return TokenType::Minus;
        case '*': return TokenType::Star;
        case '/': return TokenType::Slash;
        case '%': return TokenType::Percent;
        case '^': return TokenType::Caret;
        case '~': return TokenType::Tilde;

        case '(': return TokenType::LParen;
        case ')': return TokenType::RParen;
        case '[': return TokenType::LSquare;
        case ']': return TokenType::RSquare;
        case '{': return TokenType::LCurly;
        case '}': return TokenType::RCurly;

        case ',': return TokenType::Comma;
        case '.': return TokenType::Dot;
        case ':': return TokenType::Colon;
        case ';': return TokenType::SemiColon;

        case '=': return TokenType::Equals;
        case '!': return TokenType::Bang;
        case '<': return TokenType::Less;
        case '>': return TokenType::Greater;
        case '&': return TokenType::Amp;
        case '|': return TokenType::BitOr;

        default: break;
    }

    return TokenType::Error;
}

TokenType getPossiblyTwoCharToken(optiz::fe::TokenType previousType, char c) {
    switch (previousType) {
        case TokenType::Equals:
            if (c == '=')
                return TokenType::EqualsEquals;
            break;

        case TokenType::Bang:
            if (c == '=')
                return TokenType::BangEquals;
            break;

        case TokenType::Less:
            if (c == '=')
                return TokenType::LessEquals;
            else if (c == '<')
                return TokenType::ShiftLeft;
            break;

        case TokenType::Greater:
            if (c == '=')
                return TokenType::GreaterEquals;
            else if (c == '>')
                return TokenType::ShiftRight;
            break;

        case TokenType::Amp:
            if (c == '&')
                return TokenType::And;
            break;

        case TokenType::Or:
            if (c == '|')
                return TokenType::Or;
            break;

        default:
            break;
    }

    return previousType;
}

char getEscapedChar(char c) {
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '\\': return '\\';
        case '"': return '"';
        case '\'': return '\'';
        default: return c;
    }
}

bool isIdentifierStart(char c) {
    return als::IsAlpha(c) || c == '_';
}

bool isIdentifierChar(char c) {
    return als::IsAlphaNumeric(c) || c == '_';
}
