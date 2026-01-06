#ifndef ASTPRINTER_HPP
#define ASTPRINTER_HPP

#include "fe/ASTVisitor.hpp"

namespace optiz::fe {
    class ASTPrinter : public ASTVisitor {
        void visit(const UnaryExprAST& node) override;
        void visit(const BinaryExprAST& node) override;
        void visit(const NumberExprAST& node) override;
        void visit(const ProgramAST& node) override;
    };
}  // namespace optiz::fe

#endif  // ASTPRINTER_HPP
