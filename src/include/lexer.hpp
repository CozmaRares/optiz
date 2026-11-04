#pragma once

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

struct Token {
    enum Type {
        End   = 0,
        Float = 256,
        Invalid,
    };

    union Value {
        double float_value;
    };

    int type = End;
    Value value{};

    static Token FromType(int type) {
        Token token;
        token.type = type;
        return token;
    }

    static Token FromFloat(double float_value) {
        Token token;
        token.type              = Float;
        token.value.float_value = float_value;
        return token;
    }

private:
    Token() = default;
};

class Lexer : public yyFlexLexer {
public:
    using yyFlexLexer::yyFlexLexer;

    Lexer(const Lexer&)            = delete;
    Lexer& operator=(const Lexer&) = delete;

    Token token = Token::FromType(Token::End);

    void NextToken() { yylex(); }

private:
    int yylex() override;
};
