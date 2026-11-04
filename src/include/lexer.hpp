#pragma once

#include <expected>
#include <string_view>

#include "error.hpp"
#include "token.hpp"

#ifndef yyFlexLexerOnce
#include <FlexLexer.h>
#endif

struct LexError final : Error {
    using Error::Error;

    void Print() const override {
        PrintDiagnostic("Lexer error: {}", message);
    }
};

using LexResult = std::expected<Token, LexError>;

class Lexer : public yyFlexLexer {
    size_t offset = 0;
    SourceLocation location;
    SourceSpan matchedSpan;

    void Track(std::string_view text) {
        matchedSpan = { offset, text.size(), location };
        offset += text.size();
        for (char ch : text) {
            if (ch == '\n') {
                ++location.line;
                location.column = 1;
            } else {
                ++location.column;
            }
        }
    }

public:
    using yyFlexLexer::yyFlexLexer;

    Lexer(const Lexer&)            = delete;
    Lexer& operator=(const Lexer&) = delete;

    LexResult NextToken();
};
