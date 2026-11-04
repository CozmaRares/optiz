#pragma once

#include <cstdint>
#include <expected>

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

struct Token {
    enum Type {
        End     = 0,
        Integer = 256,
        Float,
    };

    union Value {
        int64_t integer_value;
        double float_value;
    };

    int type = End;
    Value value{};

    static Token FromType(int type) {
        Token token;
        token.type = type;
        return token;
    }

    static Token FromInteger(int64_t integer_value) {
        Token token;
        token.type                = Integer;
        token.value.integer_value = integer_value;
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

struct LexError {
    const char* message;
};

using LexResult = std::expected<Token, LexError>;

class Lexer : public yyFlexLexer {
public:
    using yyFlexLexer::yyFlexLexer;

    Lexer(const Lexer&)            = delete;
    Lexer& operator=(const Lexer&) = delete;

    LexResult NextToken();
};
