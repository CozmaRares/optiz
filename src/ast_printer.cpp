#include "ast_printer.hpp"

#include <cassert>
#include <ostream>

const char* Symbol(ast::UnaryOperator op) {
    switch (op) {
        case ast::UnaryOperator::Plus: return "+";
        case ast::UnaryOperator::Negate: return "-";
        default: {
            assert(false && "Unknown unary operator");
            __builtin_unreachable();
        }
    }
}

const char* Symbol(ast::BinaryOperator op) {
    switch (op) {
        case ast::BinaryOperator::Add: return "+";
        case ast::BinaryOperator::Subtract: return "-";
        case ast::BinaryOperator::Multiply: return "*";
        case ast::BinaryOperator::Divide: return "/";
        case ast::BinaryOperator::Remainder: return "%";
        default: {
            assert(false && "Unknown binary operator");
            __builtin_unreachable();
        }
    }
}

void AstPrinter::Print(const ast::Expression& expression) {
    expression.Accept(*this);
}

void AstPrinter::Visit(const ast::Integer& expression) {
    outputStream << expression.value;
}

void AstPrinter::Visit(const ast::Float& expression) {
    outputStream << expression.value;
}

void AstPrinter::Visit(const ast::Unary& expression) {
    outputStream << '(' << Symbol(expression.op) << ' ';
    expression.Operand().Accept(*this);
    outputStream << ')';
}

void AstPrinter::Visit(const ast::Binary& expression) {
    outputStream << '(';
    expression.Left().Accept(*this);
    outputStream << ' ' << Symbol(expression.op) << ' ';
    expression.Right().Accept(*this);
    outputStream << ')';
}
