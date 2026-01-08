#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <vector>

#include "fe/ASTVisitor.hpp"
#include "fe/Lexer.hpp"
#include "fe/SrcLocation.hpp"

#define SHARED_METHODS                         \
    void accept(ASTVisitor& v) const override; \
    static bool classof(const GenericASTNode* T)

namespace optiz::fe {

    enum class NodeKind {
        ErrorAST,
        NumberExprAST,
        UnaryExprAST,
        BinaryExprAST,
        ProgramAST
    };

    class GenericASTNode {
        NodeKind kind;
        SrcLocation startLocation;
        SrcLocation endLocation;

    public:
        GenericASTNode(NodeKind kind, SrcLocation startLocation, SrcLocation endLocation);
        virtual ~GenericASTNode() = default;

        virtual void accept(ASTVisitor& visitor) const = 0;
        NodeKind getKind() const;
        const SrcLocation& getStartLocation() const;
        const SrcLocation& getEndLocation() const;
    };

    class ErrorAST : public GenericASTNode {
    public:
        ErrorAST();
        SHARED_METHODS;
    };

    class NumberExprAST : public GenericASTNode {
        int value;

    public:
        NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        int getValue() const;
    };

    class UnaryExprAST : public GenericASTNode {
        TokenType operation;
        std::unique_ptr<GenericASTNode> expr;

    public:
        UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        TokenType getOperation() const;
        const GenericASTNode* getExpr() const;
    };

    class BinaryExprAST : public GenericASTNode {
        std::unique_ptr<GenericASTNode> lhs;
        std::unique_ptr<GenericASTNode> rhs;
        TokenType operation;

    public:
        BinaryExprAST(std::unique_ptr<GenericASTNode> lhs, std::unique_ptr<GenericASTNode> rhs, TokenType operation,
                      SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        const GenericASTNode* getLHS() const;
        const GenericASTNode* getRHS() const;
        TokenType getOperation() const;
    };

    class ProgramAST : public GenericASTNode {
        std::vector<std::unique_ptr<GenericASTNode>> expressions;

    public:
        ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        const std::vector<std::unique_ptr<GenericASTNode>>& getExpressions() const;
    };

}  // namespace optiz::fe

#undef SHARED_METHODS

#endif  // AST_HPP
