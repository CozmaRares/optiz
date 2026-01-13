#pragma once

#include "fe/ASTVisitor.hpp"

namespace optiz::fe {

    class ASTPrinter : public ASTVisitor {
        void Visit(const UnaryExprAST& node) override;
        void Visit(const BinaryExprAST& node) override;
        void Visit(const NumberExprAST& node) override;
        void Visit(const ProgramAST& node) override;
        void Visit(const ErrorAST& node) override;
    };

}  // namespace optiz::fe
