#ifndef ASTVISITOR_HPP
#define ASTVISITOR_HPP

namespace optiz::fe {
    class NumberExprAST;
    class UnaryExprAST;
    class BinaryExprAST;
    class ProgramAST;

    class ASTVisitor {
    public:
        virtual ~ASTVisitor() = default;

        virtual void visit(const UnaryExprAST& node)  = 0;
        virtual void visit(const BinaryExprAST& node) = 0;
        virtual void visit(const NumberExprAST& node) = 0;
        virtual void visit(const ProgramAST& node)    = 0;
    };
}  // namespace optiz::fe

#endif  // ASTVISITOR_HPP
