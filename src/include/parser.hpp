#pragma once

#include <expected>
#include <memory>

#include "ast.hpp"
#include "lexer.hpp"

struct ParseError final : Error {
    using Error::Error;

    void Print() const override {
        PrintDiagnostic("Parse error: {}", message);
    }
};

using ParseResult = std::expected<ast::ExpressionPtr, std::unique_ptr<Error>>;

class Parser {
    Lexer& lexer;
    Token token = Token::FromType(Token::End);

    ParseResult Advance();

    ParseResult Expression();
    ParseResult Term();
    ParseResult Factor();
    template <typename... Args>
    ParseResult Fail(std::format_string<Args...> format, Args&&... args) {
        return std::unexpected(std::make_unique<ParseError>(token.span, format, std::forward<Args>(args)...));
    }

public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}

    ParseResult Parse();
};
