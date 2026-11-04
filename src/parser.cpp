#include "parser.hpp"

#include <algorithm>
#include <initializer_list>

#define TRY_ADVANCE() \
    if (auto result = Advance(); !result) return result;

#define TRY_PARSE(var, expression) \
    auto var = (expression);       \
    if (!var) return var

namespace {
    inline bool Matches(int type, std::initializer_list<int> types) {
        return std::ranges::find(types, type) != types.end();
    }

    inline bool NotMatches(int type, std::initializer_list<int> types) {
        return !Matches(type, types);
    }
}  // namespace

ParseResult Parser::Parse() {
    TRY_ADVANCE();

    TRY_PARSE(expression, Expression());

    if (NotMatches(token.type, { Token::End })) return Fail("Expected end of input");

    return expression;
}

// E -> T (('+' | '-') E)?
ParseResult Parser::Expression() {
    TRY_PARSE(left, Term());

    const auto op = token;
    if (NotMatches(op.type, { '+', '-' })) return left;
    TRY_ADVANCE();

    TRY_PARSE(right, Expression());

    return std::make_unique<ast::Binary>(op, std::move(*left), std::move(*right));
}

// T -> F (('*' | '/') T)*
ParseResult Parser::Term() {
    TRY_PARSE(left, Factor());

    const auto op = token;
    if (NotMatches(op.type, { '*', '/' })) return left;
    TRY_ADVANCE();

    TRY_PARSE(right, Term());

    return std::make_unique<ast::Binary>(op, std::move(*left), std::move(*right));
}

// F -> INT | FLOAT | '(' E ')'
ParseResult Parser::Factor() {
    switch (token.type) {
        case Token::Integer: {
            auto expression = std::make_unique<ast::Integer>(token);
            TRY_ADVANCE();

            return expression;
        }
        case Token::Float: {
            auto expression = std::make_unique<ast::Float>(token);
            TRY_ADVANCE();

            return expression;
        }
        case '(': {
            TRY_ADVANCE();

            TRY_PARSE(expression, Expression());

            if (NotMatches(token.type, { ')' })) return Fail("Expected '{}'", ')');
            TRY_ADVANCE();

            return expression;
        }
        default: return Fail("Expected an integer, float, or '('");
    }
}

ParseResult Parser::Advance() {
    auto result = lexer.NextToken();
    if (!result) return std::unexpected(std::make_unique<LexError>(std::move(result.error())));
    token = *result;
    return ast::ExpressionPtr{ nullptr };
}
