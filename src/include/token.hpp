#pragma once

#include <cstdint>

#include "source.hpp"

struct Token {
    enum Type {
        End     = 0,
        Integer = 256,
        Float,
    };

    union Value {
        int64_t integerValue;
        double floatValue;
    };

    int type = End;
    Value value{};
    SourceSpan span;

    static Token FromType(int type, SourceSpan span = {}) {
        Token token;
        token.type = type;
        token.span = span;
        return token;
    }

    static Token FromInteger(int64_t integerValue, SourceSpan span = {}) {
        Token token;
        token.type               = Integer;
        token.value.integerValue = integerValue;
        token.span               = span;
        return token;
    }

    static Token FromFloat(double floatValue, SourceSpan span = {}) {
        Token token;
        token.type             = Float;
        token.value.floatValue = floatValue;
        token.span             = span;
        return token;
    }

private:
    Token() = default;
};
