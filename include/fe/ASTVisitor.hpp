#pragma once

namespace optiz::fe {

    class NumberExprAST;
    class UnaryExprAST;
    class BinaryExprAST;
    class ProgramAST;
    class ErrorAST;

    class ASTVisitor {
    public:
        virtual ~ASTVisitor() = default;

        virtual void Visit(const UnaryExprAST& node)  = 0;
        virtual void Visit(const BinaryExprAST& node) = 0;
        virtual void Visit(const NumberExprAST& node) = 0;
        virtual void Visit(const ProgramAST& node)    = 0;
        virtual void Visit(const ErrorAST& node)      = 0;
    };

}  // namespace optiz::fe
