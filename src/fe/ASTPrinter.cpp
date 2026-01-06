#include "fe/ASTPrinter.hpp"

#include <iostream>

#include "fe/AST.hpp"

namespace optiz::fe {
    void ASTPrinter::visit(const UnaryExprAST& node) {
        std::cout << node.operation << " ";
        node.expr->accept(*this);
    }

    void ASTPrinter::visit(const BinaryExprAST& node) {
        std::cout << "(";
        node.lhs->accept(*this);
        std::cout << ") " << node.operation << " (";
        node.rhs->accept(*this);
        std::cout << ")";
    }

    void ASTPrinter::visit(const NumberExprAST& node) {
        std::cout << node.value;
    }

    void ASTPrinter::visit(const ProgramAST& node) {
        for (auto& expr : node.expressions) {
            expr->accept(*this);
            std::cout << "\n";
        }
    }
}  // namespace optiz::fe
