#include "fe/AST.hpp"

#define ACCEPT_IMPL(T) \
    void T::accept(ASTVisitor& visitor) const { visitor.visit(*this); }

#define CLASSOF_IMPL(T) \
    bool T::classof(const GenericASTNode* n) { return n->getKind() == NodeKind::T; }

namespace optiz::fe {

    GenericASTNode::GenericASTNode(NodeKind kind, SrcLocation startLocation, SrcLocation endLocation)
        : kind(kind), startLocation(startLocation), endLocation(endLocation) {}

    ErrorAST::ErrorAST() : GenericASTNode(NodeKind::ErrorAST, SrcLocation(), SrcLocation()) {}

    NumberExprAST::NumberExprAST(int value, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::NumberExprAST, startLocation, endLocation), value(value) {}

    UnaryExprAST::UnaryExprAST(TokenType operation, std::unique_ptr<GenericASTNode> expr, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::UnaryExprAST, startLocation, endLocation), operation(operation), expr(std::move(expr)) {}

    BinaryExprAST::BinaryExprAST(std::unique_ptr<GenericASTNode> left, std::unique_ptr<GenericASTNode> right, TokenType operation,
                                 SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::BinaryExprAST, startLocation, endLocation), lhs(std::move(left)), rhs(std::move(right)), operation(operation) {}

    ProgramAST::ProgramAST(std::vector<std::unique_ptr<GenericASTNode>> expressions, SrcLocation startLocation, SrcLocation endLocation)
        : GenericASTNode(NodeKind::ProgramAST, startLocation, endLocation), expressions(std::move(expressions)) {}

    NodeKind GenericASTNode::getKind() const {
        return this->kind;
    }

    const SrcLocation& GenericASTNode::getStartLocation() const {
        return this->startLocation;
    }

    const SrcLocation& GenericASTNode::getEndLocation() const {
        return this->endLocation;
    }

    int NumberExprAST::getValue() const {
        return this->value;
    }

    TokenType UnaryExprAST::getOperation() const {
        return this->operation;
    }

    const GenericASTNode* UnaryExprAST::getExpr() const {
        return this->expr.get();
    }

    const GenericASTNode* BinaryExprAST::getLHS() const {
        return this->lhs.get();
    }

    const GenericASTNode* BinaryExprAST::getRHS() const {
        return this->rhs.get();
    }

    TokenType BinaryExprAST::getOperation() const {
        return this->operation;
    }

    const std::vector<std::unique_ptr<GenericASTNode>>& ProgramAST::getExpressions() const {
        return this->expressions;
    }

    ACCEPT_IMPL(ErrorAST)
    CLASSOF_IMPL(ErrorAST)

    ACCEPT_IMPL(NumberExprAST)
    CLASSOF_IMPL(NumberExprAST)

    ACCEPT_IMPL(UnaryExprAST)
    CLASSOF_IMPL(UnaryExprAST)

    ACCEPT_IMPL(BinaryExprAST)
    CLASSOF_IMPL(BinaryExprAST)

    ACCEPT_IMPL(ProgramAST)
    CLASSOF_IMPL(ProgramAST)

}  // namespace optiz::fe

#undef ACCEPT_IMPL
#undef CLASSOF_IMPL
