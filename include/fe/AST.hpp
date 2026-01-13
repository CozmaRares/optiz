#pragma once

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
        NodeKind m_Kind;
        SrcLocation m_StartLocation;
        SrcLocation m_EndLocation;

    public:
        GenericASTNode(NodeKind kind, SrcLocation startLocation, SrcLocation endLocation);
        virtual ~GenericASTNode() = default;

        virtual void accept(ASTVisitor& visitor) const = 0;
        NodeKind GetKind() const;
        const SrcLocation& GetStartLocation() const;
        const SrcLocation& GetEndLocation() const;
    };

    class ErrorAST : public GenericASTNode {
    public:
        ErrorAST();
        SHARED_METHODS;
    };

    class NumberExprAST : public GenericASTNode {
        int m_Value;

    public:
        NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        int GetValue() const;
    };

    class UnaryExprAST : public GenericASTNode {
        TokenType m_Operation;
        std::unique_ptr<GenericASTNode> m_Expression;

    public:
        UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        TokenType getOperation() const;
        const GenericASTNode* GetExpr() const;
    };

    class BinaryExprAST : public GenericASTNode {
        std::unique_ptr<GenericASTNode> m_LHS;
        std::unique_ptr<GenericASTNode> m_RHS;
        TokenType m_Operation;

    public:
        BinaryExprAST(std::unique_ptr<GenericASTNode> lhs, std::unique_ptr<GenericASTNode> rhs, TokenType operation,
                      SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        const GenericASTNode* GetLHS() const;
        const GenericASTNode* GetRHS() const;
        TokenType GetOperation() const;
    };

    class ProgramAST : public GenericASTNode {
        std::vector<std::unique_ptr<GenericASTNode>> m_Expressions;

    public:
        ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation);
        SHARED_METHODS;

        const std::vector<std::unique_ptr<GenericASTNode>>& GetExpressions() const;
    };

}  // namespace optiz::fe

#undef SHARED_METHODS
