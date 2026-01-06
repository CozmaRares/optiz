#include "fe/AST.hpp"

namespace optiz::fe {

    GenericASTNode::GenericASTNode(SrcLocation startLocation, SrcLocation endLocation)
        : startLocation(startLocation), endLocation(endLocation) {}

    NumberExprAST::NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(startLocation, endLocation), value(value) {}

    void NumberExprAST::accept(ASTVisitor& visitor) const {
        visitor.visit(*this);
    }

    UnaryExprAST::UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(startLocation, endLocation), operation(operation), expr(std::move(expr)) {}
    void UnaryExprAST::accept(ASTVisitor& visitor) const {
        visitor.visit(*this);
    }

    BinaryExprAST::BinaryExprAST(std::unique_ptr<GenericASTNode> left, std::unique_ptr<GenericASTNode> right, TokenType operation,
                                 SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(startLocation, endLocation), lhs(std::move(left)), rhs(std::move(right)), operation(operation) {}

    void BinaryExprAST::accept(ASTVisitor& visitor) const {
        visitor.visit(*this);
    }

    ProgramAST::ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(startLocation, endLocation), expressions(std::move(expressions)) {}

    void ProgramAST::accept(ASTVisitor& visitor) const {
        visitor.visit(*this);
    }

}  // namespace optiz::fe
