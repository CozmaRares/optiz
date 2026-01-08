#include "fe/ASTPrinter.hpp"

#include <iostream>

#include "fe/AST.hpp"

namespace optiz::fe {

    void ASTPrinter::visit(const UnaryExprAST& node) {
        std::cout << node.getOperation() << " ";
        node.getExpr()->accept(*this);
    }

    void ASTPrinter::visit(const BinaryExprAST& node) {
        std::cout << "(";
        node.getLHS()->accept(*this);
        std::cout << ") " << node.getOperation() << " (";
        node.getRHS()->accept(*this);
        std::cout << ")";
    }

    void ASTPrinter::visit(const NumberExprAST& node) {
        std::cout << node.getValue();
    }

    void ASTPrinter::visit(const ProgramAST& node) {
        for (auto& expr : node.getExpressions()) {
            expr->accept(*this);
            std::cout << "\n";
        }
    }

    void ASTPrinter::visit(const ErrorAST& node) {
        std::cout << "[Error]";
    }

}  // namespace optiz::fe
