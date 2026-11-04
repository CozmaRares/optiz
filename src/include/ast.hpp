#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include "token.hpp"

namespace ast {
    class Integer;
    class Float;
    class Unary;
    class Binary;

    class Visitor {
    public:
        virtual ~Visitor()                            = default;
        virtual void Visit(const Integer& expression) = 0;
        virtual void Visit(const Float& expression)   = 0;
        virtual void Visit(const Unary& expression)   = 0;
        virtual void Visit(const Binary& expression)  = 0;
    };

    class Expression {
    public:
        SourceSpan span;

        virtual ~Expression()                       = default;
        virtual void Accept(Visitor& visitor) const = 0;

    protected:
        explicit Expression(SourceSpan span) : span(span) {}
    };

    using ExpressionPtr = std::unique_ptr<Expression>;

    class Integer final : public Expression {
    public:
        const int64_t value;

        explicit Integer(const Token& token) : Expression(token.span), value(token.value.integerValue) {
            assert(token.type == Token::Integer);
        }
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };

    class Float final : public Expression {
    public:
        const double value;

        explicit Float(const Token& token) : Expression(token.span), value(token.value.floatValue) {
            assert(token.type == Token::Float);
        }
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };

    enum class UnaryOperator {
        Plus,
        Negate
    };

    class Unary final : public Expression {
        ExpressionPtr operand;

        static UnaryOperator Operator(const Token& token) {
            switch (token.type) {
                case '+': return UnaryOperator::Plus;
                case '-': return UnaryOperator::Negate;
                default: {
                    assert(false && "Invalid unary operator token");
                    __builtin_unreachable();
                }
            }
        }

    public:
        const UnaryOperator op;

        Unary(const Token& token, ExpressionPtr operand) : Expression(token.span), operand(std::move(operand)), op(Operator(token)) {
            assert(this->operand && "Unary expression requires an operand");
            span = SpanFromTo(token.span, this->operand->span);
        }

        const Expression& Operand() const { return *operand; }
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };

    enum class BinaryOperator {
        Add,
        Subtract,
        Multiply,
        Divide,
        Remainder
    };

    class Binary final : public Expression {
        ExpressionPtr left;
        ExpressionPtr right;

        static BinaryOperator Operator(const Token& token) {
            switch (token.type) {
                case '+': return BinaryOperator::Add;
                case '-': return BinaryOperator::Subtract;
                case '*': return BinaryOperator::Multiply;
                case '/': return BinaryOperator::Divide;
                case '%': return BinaryOperator::Remainder;
                default: {
                    assert(false && "Invalid binary operator token");
                    __builtin_unreachable();
                }
            }
        }

    public:
        const BinaryOperator op;

        Binary(const Token& token, ExpressionPtr left, ExpressionPtr right)
            : Expression(token.span), left(std::move(left)), right(std::move(right)), op(Operator(token)) {
            assert(this->left && this->right && "Binary expression requires two operands");
            span = SpanFromTo(this->left->span, this->right->span);
        }

        const Expression& Left() const { return *left; }
        const Expression& Right() const { return *right; }
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };
}  // namespace ast
