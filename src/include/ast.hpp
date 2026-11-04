#pragma once

#include <cstdint>
#include <cassert>
#include <memory>
#include <utility>

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
        virtual ~Expression()                       = default;
        virtual void Accept(Visitor& visitor) const = 0;
    };

    using ExpressionPtr = std::unique_ptr<Expression>;

    class Integer final : public Expression {
    public:
        const int64_t value;

        explicit Integer(int64_t value) : value(value) {}
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };

    class Float final : public Expression {
    public:
        const double value;

        explicit Float(double value) : value(value) {}
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };

    enum class UnaryOperator {
        Plus,
        Negate
    };

    class Unary final : public Expression {
        ExpressionPtr operand;

    public:
        const UnaryOperator op;

        Unary(UnaryOperator op, ExpressionPtr operand) : operand(std::move(operand)), op(op) {
            assert(this->operand && "Unary expression requires an operand");
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

    public:
        const BinaryOperator op;

        Binary(BinaryOperator op, ExpressionPtr left, ExpressionPtr right)
            : left(std::move(left)), right(std::move(right)), op(op) {
            assert(this->left && this->right && "Binary expression requires two operands");
        }

        const Expression& Left() const { return *left; }
        const Expression& Right() const { return *right; }
        void Accept(Visitor& visitor) const override { visitor.Visit(*this); }
    };
}  // namespace ast
