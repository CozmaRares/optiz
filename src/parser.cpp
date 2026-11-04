#include "parser.hpp"

#define TRY_ADVANCE() \
    if (auto result = Advance(); !result) return result;

#define TRY_PARSE(var, expression) \
    auto var = (expression);       \
    if (!var) return var

ParseResult Parser::Parse() {
    TRY_ADVANCE();

    TRY_PARSE(expression, Expression());

    if (token.type != Token::End) return Fail("Expected end of input");

    return expression;
}

// E -> T (('+' | '-') E)?
ParseResult Parser::Expression() {
    TRY_PARSE(left, Term());

    const int type = token.type;
    if (type != '+' && type != '-') return left;
    TRY_ADVANCE();

    TRY_PARSE(right, Expression());

    const auto op = type == '+' ? ast::BinaryOperator::Add : ast::BinaryOperator::Subtract;
    return std::make_unique<ast::Binary>(op, std::move(*left), std::move(*right));
}

// T -> F (('*' | '/') T)*
ParseResult Parser::Term() {
    TRY_PARSE(left, Factor());

    const int type = token.type;
    if (type != '*' && type != '/') return left;
    TRY_ADVANCE();

    TRY_PARSE(right, Term());

    const auto op = type == '*' ? ast::BinaryOperator::Multiply : ast::BinaryOperator::Divide;
    return std::make_unique<ast::Binary>(op, std::move(*left), std::move(*right));
}

// F -> INT | FLOAT | '(' E ')'
ParseResult Parser::Factor() {
    switch (token.type) {
        case Token::Integer: {
            int v = token.value.integer_value;
            TRY_ADVANCE();

            auto expression = std::make_unique<ast::Integer>(v);
            return expression;
        }
        case Token::Float: {
            double v = token.value.float_value;
            TRY_ADVANCE();

            auto expression = std::make_unique<ast::Float>(v);
            return expression;
        }
        case '(': {
            TRY_ADVANCE();

            TRY_PARSE(expression, Expression());

            if (token.type != ')') return Fail("Expected ')'");
            TRY_ADVANCE();

            return expression;
        }
        default: return Fail("Expected an integer, float, or '('");
    }
}

ParseResult Parser::Fail(const char* message) {
    return std::unexpected(ParseError{ message });
}

ParseResult Parser::Advance() {
    auto result = lexer.NextToken();
    if (!result) return Fail(result.error().message);
    token = *result;
    return ast::ExpressionPtr{ nullptr };
}
