#pragma once

#include <expected>

#include "ast.hpp"
#include "lexer.hpp"

struct ParseError {
    const char* message;
};

using ParseResult = std::expected<ast::ExpressionPtr, ParseError>;

class Parser {
    Lexer& lexer;
    Token token = Token::FromType(Token::End);

    ParseResult Advance();

    ParseResult Expression();
    ParseResult Term();
    ParseResult Factor();
    ParseResult Fail(const char* message);

public:
    explicit Parser(Lexer& lexer) : lexer(lexer) {}

    ParseResult Parse();
};
