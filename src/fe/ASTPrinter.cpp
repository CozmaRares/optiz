#include "fe/ASTPrinter.hpp"

#include <iostream>

#include "fe/AST.hpp"

namespace optiz::fe {

    void ASTPrinter::Visit(const UnaryExprAST& node) {
        std::cout << node.getOperation() << ' ';
        node.GetExpr()->accept(*this);
    }

    void ASTPrinter::Visit(const BinaryExprAST& node) {
        node.GetLHS()->accept(*this);
        std::cout << ' ' << node.GetOperation() << ' ';
        node.GetRHS()->accept(*this);
    }

    void ASTPrinter::Visit(const NumberExprAST& node) {
        std::cout << node.GetValue();
    }

    void ASTPrinter::Visit(const ProgramAST& node) {
        for (auto& expr : node.GetExpressions()) {
            expr->accept(*this);
            std::cout << "\n";
        }
    }

    void ASTPrinter::Visit(const ErrorAST& node) {
        std::cout << "[Error]";
    }

}  // namespace optiz::fe
