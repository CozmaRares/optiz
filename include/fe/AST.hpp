#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <vector>

#include "fe/ASTVisitor.hpp"
#include "fe/Lexer.hpp"
#include "fe/SrcLocation.hpp"

namespace optiz::fe {

    struct GenericASTNode {
        SrcLocation startLocation;
        SrcLocation endLocation;

        GenericASTNode(SrcLocation startLocation, SrcLocation endLocation);
        virtual ~GenericASTNode()                      = default;
        virtual void accept(ASTVisitor& visitor) const = 0;
    };

    struct NumberExprAST : public GenericASTNode {
        int value;

        NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation);
        void accept(ASTVisitor& visitor) const override;
    };

    struct UnaryExprAST : public GenericASTNode {
        TokenType operation;
        std::unique_ptr<GenericASTNode> expr;

        UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation);
        void accept(ASTVisitor& visitor) const override;
    };

    struct BinaryExprAST : public GenericASTNode {
        std::unique_ptr<GenericASTNode> lhs;
        std::unique_ptr<GenericASTNode> rhs;
        TokenType operation;

        BinaryExprAST(std::unique_ptr<GenericASTNode> lhs, std::unique_ptr<GenericASTNode> rhs, TokenType operation,
                      SrcLocation startLocation, SrcLocation endLocation);
        void accept(ASTVisitor& visitor) const override;
    };

    struct ProgramAST : public GenericASTNode {
        std::vector<std::unique_ptr<GenericASTNode>> expressions;

        ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation);
        void accept(ASTVisitor& visitor) const override;
    };

}  // namespace optiz::fe

#endif  // AST_HPP
